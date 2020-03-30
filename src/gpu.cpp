#include <cmath>
#include <cstring>
#include <algorithm>
#include <iostream>

#include "gpu.h"
#include "memory_map.h"

constexpr uint8_t kShades[4] =
{
	0x00,
	0x3F,
	0x7E,
	0xFF
};

GPU::GPU(GpuRegisters registers, GpuMem mem, OpenGLWidget *openGLWidget)
	: reg(registers), mem(mem)
{
	std::memset(mBgPalette, 0, 4);
	std::memset(mSpritePalettes, 0, 8);
}

GPU::~GPU()
{
	if (mPixels)
		delete mPixels;
	if (mMapData)
		free(mMapData);
}

void GPU::reset()
{
	if (mPixels)
	{
		delete[] mPixels;
	}

	mPixels = new uint8_t[160 * 144];
	std::fill_n(mPixels, 160 * 144, 0x00);

	if (mMapData)
	{
		free(mMapData);
	}

	mMapData = static_cast<uint8_t *>(malloc(0x800));
	memset(mMapData, 0x46, 0x800);
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
	mPixels[(y * 160 + x) % (160 * 144)] = colour;
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
	if (!(reg.lcdc & 0x80))
		return;

	bool isSigned = true;

	uint8_t *tiles = mem.tilesSigned;
	if (reg.lcdc & 0x10)
	{
		tiles = mem.tilesUnsigned;
		isSigned = false;
	}

	uint8_t *bgMap = reg.lcdc & 0x8 ? mem.map1 : mem.map0;

	int y_abs = line + reg.scy;
	int y_map = y_abs / 8;
	int y_px_in_tile = y_abs % 8;

	// background
	for (int x = 0; x < 160; x++)
	{
		int x_abs = x + reg.scx;
		int x_map = x_abs / 8;
		int x_px_in_tile = x_abs % 8;

		// Wrap around if it tries to draw past the end of a map
		int idx_offset_in_map = ((y_map * 32) + x_map) % 0x400;

		uint8_t map_val = bgMap[idx_offset_in_map];
		int idx = isSigned ? int8_t(map_val) : map_val;

		int tile_offset = idx * 16;
		int row_offset = tile_offset + (y_px_in_tile * 2);

		uint16_t row = tiles[row_offset] << 8;
		row |= tiles[row_offset + 1];
		uint8_t colour = mBgPalette[decodePixel(row, x_px_in_tile)];

		setPixel(x, line - 1, colour);
	}

	if (reg.lcdc & 0x20)
	{
		uint8_t *windowMap = reg.lcdc & 0x40 ? mem.map1 : mem.map0;

		// window
		for (int x = 0; x < 160; x++)
		{
			int x_abs = x + reg.scx;
			int x_map = x_abs / 8;
			int x_px_in_tile = x_abs % 8;

			// Wrap around if it tries to draw past the end of a map
			int idx_offset = ((y_map * 32) + x_map) % 0x400;
			uint8_t map_val = windowMap[idx_offset];
			int idx = isSigned ? int8_t(map_val) : map_val;

			int tile_offset = idx * 16;
			int row_offset = tile_offset + (y_px_in_tile * 2);

			uint16_t row = tiles[row_offset];
			row |= tiles[row_offset + 1];

			uint8_t colour = mBgPalette[decodePixel(row, x_px_in_tile)];

			setPixel(x, line - 1, colour);
		}
	}

	if (mSpritesChanged)
	{
		memcpy(mSprites.data(), mem.oam, sizeof(sprite_t) * 40);
		std::sort(mSprites.begin(), mSprites.end(), [](const sprite_t &lh, const sprite_t &rh) {return lh.x < rh.x;});
		for (const auto &sprite : mSprites)
		{
			drawSprite(sprite);
		}
		mSpritesChanged = false;
	}
}

const uint8_t* GPU::getMap()
{
	return mMapData;
}

const uint8_t* GPU::getPixels()
{
	return mPixels;
}

void GPU::updateMap()
{
	if (!(reg.lcdc & (1U << 3)))
	{
		for (int i = 0; i < 0x400; i++)
		{
			mMapData[i] = mem.map0[i];
		}

		int left = getMapIndex(reg.scx, reg.scy);
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
			mMapData[0x400 + i] = mem.map1[i];
		}

		int left = getMapIndex(reg.scx, reg.scy);
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
}

void GPU::tick(int cycles)
{
	mModeClock += cycles;
	uint8_t stat = reg.stat;

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
			if (reg.ly > 0x99)
			{
				mode = MODE_VBLANK;
				drawScanline(reg.ly);
				reg.ly = 0;
				mModeClock = 0;
				reg.if_ |= 0x1;
			}
			else
			{
				mode = MODE_OAM;
				reg.ly++;
				if (reg.ly == reg.lyc)
				{
					reg.stat |= 0x4;
					reg.if_ |= 0x2;
				}
				drawScanline(reg.ly);
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

	reg.stat = (reg.stat & 0b1111'1100) | mode;
}

void GPU::setBgPalette(uint8_t val)
{
	mBgPalette[0] = kShades[val & 0x3];
	mBgPalette[1] = kShades[(val & 0xC) >> 2];
	mBgPalette[2] = kShades[(val & 0x30) >> 4];
	mBgPalette[3] = kShades[(val & 0xC0) >> 6];
}

void GPU::setSpritePalette(int paletteIdx, uint8_t val)
{
	mSpritePalettes[paletteIdx][1] = kShades[(val & 0xC) >> 2];
	mSpritePalettes[paletteIdx][2] = kShades[(val & 0x30) >> 4];
	mSpritePalettes[paletteIdx][3] = kShades[(val & 0xC0) >> 6];
}
