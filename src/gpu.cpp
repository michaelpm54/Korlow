#include <cmath>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <GL/glew.h>
#include "gpu.h"
#include "mmu.h"
#include "memory_map.h"
#include "util.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

constexpr uint8_t kShades[4] =
{
	0x00,
	0x3F,
	0x7E,
	0xFF
};

const int kNumComponents = 1;

GPU::GPU()
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
	if (mPixels)
	{
		delete[] mPixels;
	}
	mNumPixels = 160 * 144 * kNumComponents;
	mPixels = new uint8_t[mNumPixels];
	std::fill_n(mPixels, mNumPixels, 0x00);
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
	if (mMapTex)
	{
		glDeleteTextures(1, &mMapTex);
	}
	if (mMapData)
	{
		free(mMapData);
		mMapData = nullptr;
	}
}

void GPU::initOpenGL()
{
	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	mProjMatrix = glm::ortho(0.0f, 160.0f + 64.0f, 0.0f, 144.0f);
	mMainModel = glm::scale(mMainModel, glm::vec3(160.0f, 144.0f, 1.0f));
	mMapModel = glm::scale(mMapModel, glm::vec3(32.0f, 64.0f, 1.0f));
	// mMapModel = glm::translate(mMapModel, glm::vec3(160.0f, 0.0f, 0.0f));
}

void GPU::setSize(int w, int h)
{
	mWidth = w;
	mHeight = h;
	createGLObjects();
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, 160, 144, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, mPixels);
	glBindTexture(GL_TEXTURE_2D, 0);

	loadShaders(mProgram, "../assets/shaders/ssquad.vs", "../assets/shaders/ssquad.fs");

	GLfloat vertices[] =
	{
		/* xyzw */ 0.0f, 0.0f, 0.0f, 1.0f, /* uv */ 0.0f, 1.0f,
		/* xyzw */ 1.0f, 0.0f, 0.0f, 1.0f, /* uv */ 1.0f, 1.0f,
		/* xyzw */ 1.0f, 1.0f, 0.0f, 1.0f, /* uv */ 1.0f, 0.0f,
		/* xyzw */ 0.0f, 0.0f, 0.0f, 1.0f, /* uv */ 0.0f, 1.0f,
		/* xyzw */ 1.0f, 1.0f, 0.0f, 1.0f, /* uv */ 1.0f, 0.0f,
		/* xyzw */ 0.0f, 1.0f, 0.0f, 1.0f, /* uv */ 0.0f, 0.0f,
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

	/*
		Map: a 32x32 array of tile indices, which are 1 byte long.
		a tile index refers to a location in the tile data starting at either
		0x8000 or 0x9000 depending on whether LCDC.4 is set.
		0 = 0x8000 and the tile index is unsigned
		1 = 0x9000 and the tile index is signed
		Each tile data set is 0x1000 bytes long.

		For the map view I will create a texture showing an RGB colour from 0-255 based on the index,
		for each index.

		There are two maps of size 0x400. One is at 0x9800 and the other is at 0x9C00.
		Which one is used is set based on LCDC.3.
	*/
	glGenTextures(1, &mMapTex);

	mMapData = static_cast<uint8_t *>(malloc(0x800));
	memset(mMapData, 0x46, 0x800);

	glBindTexture(GL_TEXTURE_2D, mMapTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, 32, 64, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, mMapData);
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

uint8_t decodePixel(std::array<uint8_t, 2> data, int pixelIdx)
{
	uint8_t mask = 0x80 >> pixelIdx;
	uint8_t lower = !!(data[0] & mask);
	uint8_t upper = !!(data[1] & mask);
	uint8_t paletteIdx = (upper << 1) | lower;
	return paletteIdx;
}

uint8_t decodePixel(uint16_t row, int pixelIdx)
{
	uint8_t mask = 0x80 >> pixelIdx;
	uint8_t lower = !!((row >> 8) & mask);
	uint8_t upper = !!((row & 0xFF) & mask);
	uint8_t paletteIdx = (upper << 1) | lower;
	return paletteIdx;
}

void GPU::setPixel(int x, int y, uint8_t colour)
{
	mPixels[(y * 160 + x) % (mNumPixels)] = colour;
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

int getMapIndex(int x, int y)
{
	return ((y / 8) * 32) + (x / 8);
}

int getMapEntry(uint8_t *map, int x, int y)
{
	return map[getMapIndex(x, y)];
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

	int scx = mmu->mem[kScx];

	int y_abs = line + mmu->mem[kScy];
	int y_map = y_abs / 8;
	int y_px_in_tile = y_abs % 8;

	// bg
	for (int x = 0; x < 160; x++)
	{
		int x_abs = x + scx;
		int x_map = x_abs / 8;
		int x_px_in_tile = x_abs % 8;

		// Wrap around if it tries to draw past the end of a map
		int idx_offset_in_map = ((y_map * 32) + x_map) % 0x400;

		int idx_offset = bgMap + idx_offset_in_map;
		int idx = isSigned ? int8_t(mmu->mem[idx_offset]) : uint8_t(mmu->mem[idx_offset]);

		int tile_offset_in_data = idx * 16;
		int tile_offset = tileData + tile_offset_in_data;

		int row_offset = tile_offset + (y_px_in_tile * 2);

		uint16_t row = mmu->read16(row_offset);
		uint8_t colour = mBgPalette[decodePixel(row, x_px_in_tile)];

		setPixel(x, line - 1, colour);
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
	glUseProgram(mProgram);
	glUniformMatrix4fv(glGetUniformLocation(mProgram, "projection"), 1, GL_FALSE, glm::value_ptr(mProjMatrix));

	// Draw main frame

	// Update texture
	glBindTexture(GL_TEXTURE_2D, mFrameTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 160, 144, GL_RED_INTEGER, GL_UNSIGNED_BYTE, mPixels);

	// Update main model
	glm::mat4 mainModel(1.0f);
	mainModel = glm::scale(mainModel, glm::vec3(160.0f, 144.0f, 1.0f));
	glUniformMatrix4fv(glGetUniformLocation(mProgram, "model"), 1, GL_FALSE, glm::value_ptr(mainModel));

	// Draw
	glBindVertexArray(mVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Draw map view

	// Update map model
	glm::mat4 mapModel(1.0f);
	mapModel = glm::translate(mapModel, glm::vec3(160.0f, 0.0f, 0.0f));
	mapModel = glm::scale(mapModel, glm::vec3(64.0f, 144.0f, 1.0f));
	glUniformMatrix4fv(glGetUniformLocation(mProgram, "model"), 1, GL_FALSE, glm::value_ptr(mapModel));

	// Draw
	glBindTexture(GL_TEXTURE_2D, mMapTex);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glUseProgram(0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GPU::updateMap()
{
	mMapModel = glm::translate(mMapModel, glm::vec3(1.0f, 0.0f, 0.0f));
	if (!(mmu->mem[kLcdc] & (1U << 3)))
	{
		for (int i = 0; i < 0x400; i++)
		{
			mMapData[i] = mmu->mem[kBgMap0 + i];
		}

		int left = getMapIndex(mmu->mem[kScx], mmu->mem[kScy]);
		int right = left + 20;
		int bottomLeft = (left + (18 * 32)) % 0x400;
		int bottomRight = (right + (18 * 32)) % 0x400;

		for (int i = 0; i <= 18; i++)
		{
			int offsetLeft = (left + (i * 32)) % 0x400;
			int offsetRight = (right + (i * 32)) % 0x400;
			mMapData[offsetLeft] = 0xFF;
			mMapData[offsetRight] = 0xFF;
		}

		memset(&mMapData[left], 0xFF, 20);
		memset(&mMapData[bottomLeft], 0xFF, 20);
	}
	else
	{
		for (int i = 0; i < 0x400; i++)
		{
			mMapData[0x400 + i] = mmu->mem[kBgMap1 + i];
		}

		int left = getMapIndex(mmu->mem[kScx], mmu->mem[kScy]);
		int right = left + 20;
		int bottomLeft = left + (18 * 32);

		memset(&mMapData[left], 0xFF, 20);
		memset(&mMapData[bottomLeft], 0xFF, 20);
		for (int i = 0; i <= 18; i++)
		{
			mMapData[left + (i * 32)] = 0xFF;
			mMapData[right + (i * 32)] = 0xFF;
		}
	}
	glBindTexture(GL_TEXTURE_2D, mMapTex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 32, 64, GL_RED_INTEGER, GL_UNSIGNED_BYTE, mMapData);
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
