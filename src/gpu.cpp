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

int getMapIndex(int x, int y)
{
	return ((y / 8) * 32) + (x / 8);
}

int getMapEntry(uint8_t *map, int x, int y)
{
	return map[getMapIndex(x, y)];
}

uint8_t decodePixel(std::array<uint8_t, 2> data, int pixelIdx)
{
	uint8_t mask = 0x80 >> pixelIdx;
	uint8_t lower = !!(data[0] & mask);
	uint8_t upper = !!(data[1] & mask);
	uint8_t paletteIdx = (upper << 1) | lower;
	return paletteIdx;
}

void GPU::setPixel(int x, int y, uint8_t colour)
{
	mPixels[y * kWidth + x] = colour;
}

std::array<std::array<uint8_t, 2>, 8> getTile(uint8_t *tiles, int patternNum, bool isSigned)
{
	if (isSigned)
	{
		patternNum = int8_t(patternNum);
	}

	std::array<std::array<uint8_t, 2>, 8> lines;
	memcpy(lines.data(), tiles + (patternNum * 16), 16);
	return lines;
}

void GPU::drawSprite(const sprite_t &sprite)
{
	
}

void GPU::drawScanline(int line)
{
	uint8_t lcdc = mmu->mem[kLcdc];

	if (!(lcdc & 0x80))
		return;

	bool isSigned = true;

	uint16_t tileData = kTileRamSigned;
	if (lcdc & 0x10)
	{
		tileData = kTileRamUnsigned;
		isSigned = false;
	}

	uint16_t bgMap = kBgMap0;
	if (lcdc & 0x8)
	{
		bgMap = kBgMap1;
	}

	uint8_t *map = &mmu->mem[bgMap];
	uint8_t *tiles = &mmu->mem[tileData];

	line += mmu->mem[kScy];

	int pxY = (mmu->mem[kLy] - 1) % 0x90;

	// bg
	for (int x = 0; x < 160; x += 8)
	{
		int mapX = x + mmu->mem[kScx];
		int mapY = line;
		auto tileData = getTile(tiles, getMapEntry(map, mapX, mapY), isSigned);
		auto lineData = tileData[line % 8];

		for (int i = 0; i < 8; i++)
		{
			uint8_t palIdx = decodePixel(lineData, i);
			uint8_t colour = mBgPalette[palIdx];
			int pxX = x + i;
			setPixel(pxX, pxY, colour);
		}
	}

	if (mSpritesChanged)
	{
		memcpy(mSprites.data(), &mmu->mem[kOam], sizeof(sprite_t) * 40);
		std::sort(mSprites.begin(), mSprites.end(), [](const sprite_t &lh, const sprite_t &rh) {return lh.x < rh.x;});
		for (const auto &sprite : mSprites)
		{
			drawSprite(sprite);
		}
		mSpritesChanged = false;
	}
}

void GPU::frame()
{
	glBindTexture(GL_TEXTURE_2D, mFrameTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, kWidth, kHeight, GL_RED_INTEGER, GL_UNSIGNED_BYTE, mPixels);
	glUseProgram(mProgram);
	glBindVertexArray(mVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glUseProgram(0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GPU::tick(int cycles)
{
	mModeClock += cycles;
	uint8_t stat = mmu->mem[kStat];

	if (mode == MODE_OAM)
	{
		if (mModeClock >= 83)
		{
			mode = MODE_OAM_VRAM;
			mModeClock = 0;
		}
	}
	else if (mode == MODE_OAM_VRAM)
	{
		if (mModeClock >= 175)
		{
			mode = MODE_HBLANK;
			mModeClock = 0;
		}
	}
	else if (mode == MODE_HBLANK)
	{
		if (mModeClock >= 207)
		{
			if (mmu->mem[kLy] >= 0x91)
			{
				mode = MODE_VBLANK;
				drawScanline(mmu->mem[kLy]);
				mmu->mem[kLy] = 0;
				mModeClock = 0;
				mmu->or8(kIf, 0x1);
			}
			else
			{
				mode = MODE_OAM;
				mmu->mem[kLy]++;
				if (mmu->mem[kLy] == mmu->mem[kLyc])
				{
					mmu->mem[kStat] |= 0x4;
					mmu->mem[kIf] |= 0x2;
				}
				drawScanline(mmu->mem[kLy]);
				mModeClock = 0;
			}
		}
	}
	else if (mode == MODE_VBLANK)
	{
		if (mModeClock >= 4560)
		{
			mode = MODE_OAM;
			mModeClock = 0;
		}
	}

	mmu->mem[kStat] = (mmu->mem[kStat] & 0b1111'1100) | mode;
}

void GPU::setBgPalette(uint8_t val)
{
	mBgPalette[0] = kShades[val & 0x3];
	mBgPalette[1] = kShades[(val & 0xC) >> 2];
	mBgPalette[2] = kShades[(val & 0x30) >> 4];
	mBgPalette[3] = kShades[(val & 0xC0) >> 6];
}
