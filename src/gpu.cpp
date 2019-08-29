#include <cmath>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <GL/glew.h>
#include "gpu.h"
#include "mmu.h"
#include "memory_map.h"
#include "util.h"

constexpr uint8_t kShades[4] =
{
	0x00,
	0x3F,
	0x7E,
	0xFF
};

const int kWidth = 160;
const int kHeight = 144;
const int kComponents = 1;
const int kNumPixels = kWidth * kHeight * kComponents;

GPU::GPU()
	:
	mPixels(new uint8_t[kNumPixels])
{
	memcpy(mBgPalette, kShades, 4);
}

GPU::~GPU()
{
	destroyGLObjects();
	delete mPixels;
}

void GPU::reset()
{
	std::fill_n(mPixels, kNumPixels, 0x00);
	createGLObjects();
}

void GPU::destroyGLObjects()
{
	if (mProgram)
	{
		glDeleteProgram(mProgram);
	}
	if (mFrameTexture)
	{
		glDeleteTextures(1, &mFrameTexture);
	}
	if (mVao)
	{
		glDeleteVertexArrays(1, &mVao);
	}
	if (mVbo)
	{
		glDeleteBuffers(1, &mVbo);
	}
}

void GPU::initOpenGL()
{
	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void GPU::createGLObjects()
{
	destroyGLObjects();

	mProgram = glCreateProgram();
	glGenTextures(1, &mFrameTexture);
	glGenVertexArrays(1, &mVao);
	glGenBuffers(1, &mVbo);

	glBindTexture(GL_TEXTURE_2D, mFrameTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, kWidth, kHeight, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, mPixels);

	loadShaders(mProgram, "../assets/shaders/ssquad.vs", "../assets/shaders/ssquad.fs");

	GLfloat vertices[] =
	{
		/* xyzw */ -1.0f, -1.0f, 0.0f, 1.0f, /* uv */ 0.0f, 1.0f,
		/* xyzw */  1.0f, -1.0f, 0.0f, 1.0f, /* uv */ 1.0f, 1.0f,
		/* xyzw */  1.0f,  1.0f, 0.0f, 1.0f, /* uv */ 1.0f, 0.0f,
		/* xyzw */ -1.0f, -1.0f, 0.0f, 1.0f, /* uv */ 0.0f, 1.0f,
		/* xyzw */  1.0f,  1.0f, 0.0f, 1.0f, /* uv */ 1.0f, 0.0f,
		/* xyzw */ -1.0f,  1.0f, 0.0f, 1.0f, /* uv */ 0.0f, 0.0f,
	};

	glBindVertexArray(mVao);
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 6, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid *)(sizeof(GLfloat) * 0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid *)(sizeof(GLfloat) * 4));
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

uint8_t paletteIndex(uint8_t byte0, uint8_t byte1, uint8_t pxIndex)
{
	return ((!!(byte1 & (0x80 >> pxIndex))) << 1) | (!!(byte0 & (0x80 >> pxIndex)));
}

void decodeTile(uint8_t *palette, uint8_t *tile, uint8_t *pixels)
{
	int b = 0;
	for (int byte = 0; byte < 16; byte++)
	{
		for (int i = 0; i < 8; i++)
		{
			uint8_t mask = 0x80 >> i;
			uint8_t upper = !!(tile[byte] & mask);
			uint8_t lower = !!(tile[byte+1] & mask);
			uint8_t combined = (lower << 1) | upper;
			pixels[b++] = palette[combined];
		}
		byte++;
	}
}

int getPatternIndex(uint8_t *map, int xtile, int ytile, bool signedIdx)
{
	// 32 rows of 32 bytes
	int patternIdx = map[ytile * 32 + xtile];
	if (signedIdx)
		patternIdx = int8_t(patternIdx);
	return patternIdx;
}

void getTile(uint8_t *tileData, int patternIdx, uint8_t *tile)
{
	memcpy(tile, &tileData[patternIdx * 16], 16);
}

void GPU::drawTile8x8(uint8_t *pixels, int x, int y)
{
	int pxIdx = 0;
	for (int px_y = 0; px_y < 8; px_y++)
	{
		for (int px_x = 0; px_x < 8; px_x++)
		{
			int absY = ((y+px_y) * kWidth);
			int absX = (x + px_x);
			int pxIdx = absY + absX;
			mPixels[pxIdx] = pixels[px_y * 8 + px_x];
		}
	}
}

void GPU::updatePixels()
{
	uint8_t lcdc = mmu->mem[kLcdc];

	// if (!(lcdc & 0x80))
		// return;

	uint16_t tileData = kTileRamSigned;
	if (lcdc & 0x10)
	{
		tileData = kTileRamUnsigned;
	}

	uint16_t bgMap = kBgMap0;
	if (lcdc & 0x8)
	{
		bgMap = kBgMap1;
	}

	int startTileX = mmu->mem[kScx] / 32;
	int startTileY = mmu->mem[kScy] / 32;
	int endTileX = startTileX + (kWidth / 8);
	int endTileY = startTileY + (kHeight / 8);

	for (int tiley = startTileY; tiley < endTileY; tiley++)
	{
		for (int tilex = startTileX; tilex < endTileX; tilex++)
		{
			int patternIdx = getPatternIndex(&mmu->mem[bgMap], tilex, tiley, tileData == kTileRamSigned);

			uint8_t tile[16];
			getTile(&mmu->mem[tileData], patternIdx, tile);

			uint8_t pixels[64];
			decodeTile(mBgPalette, tile, pixels);

			drawTile8x8(pixels, (tilex - startTileX)*8, (tiley - startTileY)*8);
		}
	}
	
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, kWidth, kHeight, GL_RED_INTEGER, GL_UNSIGNED_BYTE, mPixels);
}

void GPU::frame()
{
	glBindTexture(GL_TEXTURE_2D, mFrameTexture);
	updatePixels();
	glUseProgram(mProgram);
	glBindVertexArray(mVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glUseProgram(0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GPU::tick(int cycles)
{
	mClock += cycles;
	uint8_t stat = mmu->mem[kStat];
	if (mClock >= 77 && mClock <= 83)
	{
		mode = 2;
		stat = (stat & 0b1111'1100) | 0x2;
	}
	else if (mClock >= 169 && mClock <= 175)
	{
		mode = 3;
		stat = (stat & 0b1111'1100) | 0x3;
	}
	else if (mClock >= 201 && mClock <= 207)
	{
		mode = 0;
		stat = (stat & 0b1111'1100);
	}
	else if (mClock >= 456 && mClock < 4560)
	{
		mode = 1;
		stat = (stat & 0b1111'1100) | 0x1;
	}
	else if (mClock >= 4560)
	{
		mode = 2;
		mClock = 0;
	}
	if (stat != mmu->mem[kStat])
	{
		mmu->mem[kStat] = stat;
	}
}

void GPU::setBgPalette(uint8_t val)
{
	mBgPalette[0] = kShades[val & 0x3];
	mBgPalette[1] = kShades[(val & 0xC) >> 2];
	mBgPalette[2] = kShades[(val & 0x30) >> 4];
	mBgPalette[3] = kShades[(val & 0xC0) >> 6];
}
