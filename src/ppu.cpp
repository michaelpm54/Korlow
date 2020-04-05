#include "ppu.h"
#include "memory_map.h"

constexpr uint8_t kShades[4] =
{
	0x00,
	0x3F,
	0x7E,
	0xFF
};

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

int getMapIndex(int x, int y)
{
	return ((y / 8) * 32) + (x / 8);
}

int getMapEntry(uint8_t *map, int x, int y)
{
	return map[getMapIndex(x, y)];
}

/*
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
*/

PpuMapProxy::PpuMapProxy(Ppu &ppu)
	: ppu(ppu)
	, map_pixels(0x800)
{}

void PpuMapProxy::reset()
{
}

void PpuMapProxy::write8(uint16_t address, uint8_t value)
{
	if (address >= kMap0 && address <= kMap1)
	{
		uint16_t map_address = address - kMap0;
		int x = map_address % 32;
		int y = map_address / 32;
		map_pixels[y * 32 + x] = value;
	}

	ppu.write8(address, value);
}

const uint8_t *PpuMapProxy::get_pixels() const
{
	return map_pixels.data();
}

Ppu::Ppu(PpuRegisters registers)
	: registers(registers)
	, memory(0x2000)
	, oam(0x100)
	, pixels(160 * 144)
{}

const uint8_t* Ppu::get_pixels() const
{
	return pixels.data();
}

void Ppu::draw_scanline(int line)
{
	if (!(registers.lcdc & 0x80))
		return;

	bool isSigned = true;

	uint8_t *tiles = signedTiles;
	if (registers.lcdc & 0x10)
	{
		tiles = unsignedTiles;
		isSigned = false;
	}

	uint8_t *bgMap = registers.lcdc & 0x8 ? map1 : map0;

	int y_abs = line + registers.scy;
	int y_map = y_abs / 8;
	int y_px_in_tile = y_abs % 8;

	// background
	for (int x = 0; x < 160; x++)
	{
		int x_abs = x + registers.scx;
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
		uint8_t colour = bg_palette[decodePixel(row, x_px_in_tile)];

		set_pixel(x, line - 1, colour);
	}

	if (registers.lcdc & 0x20)
	{
		uint8_t *windowMap = registers.lcdc & 0x40 ? map1 : map0;

		// window
		for (int x = 0; x < 160; x++)
		{
			int x_abs = x + registers.scx;
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

			uint8_t colour = bg_palette[decodePixel(row, x_px_in_tile)];

			set_pixel(x, line - 1, colour);
		}
	}

	if (sprites_dirty)
	{
		memcpy(sprites.data(), oam.data(), sizeof(sprite_t) * 40);
		std::sort(sprites.begin(), sprites.end(), [](const sprite_t &lh, const sprite_t &rh) {return lh.x < rh.x;});
		for (const auto &sprite : sprites)
		{
			draw_sprite(sprite);
		}
		sprites_dirty = false;
	}
}

void Ppu::set_pixel(int x, int y, uint8_t colour)
{
	pixels[(y * 160 + x) % (160 * 144)] = colour;
}

void Ppu::reset()
{
	std::fill(std::begin(memory), std::end(memory), 0x00);
	std::fill(std::begin(oam), std::end(oam), 0x00);
	std::fill(std::begin(pixels), std::end(pixels), 0x00);
	std::memset(bg_palette, 0, 4);
	std::memset(sprite_palette, 0, 8);
	sprites_dirty = false;
	mode = MODE_OAM;
	mode_counter = 0;
	unsignedTiles = &memory[0];
	signedTiles = &memory[0x1000];
	map0 = &memory[0x1800];
	map1 = &memory[0x1C00];
}

void Ppu::write8(uint16_t address, uint8_t value)
{
	if (address >= kTileRamUnsigned && address < kCartRam)
	{
		memory[address - kTileRamUnsigned] = value;
		return;
	}
	else if (address >= kOam && address < kIo)
	{
		oam[address - kOam] = value;
		return;
	}

	switch (address)
	{
	case kIf:
		registers.if_ = value;
		break;
	case kLcdc:
		registers.lcdc = value;
		break;
	case kStat:
		registers.stat = value;
		break;
	case kScy:
		registers.scy = value;
		break;
	case kScx:
		registers.scx = value;
		break;
	case kLy:
		registers.ly = 0;
		break;
	case kLyc:
		registers.lyc = value;
		break;
	case kWy:
		registers.wy = value;
		break;
	case kWx:
		registers.wx = value;
		break;
	case kBgPalette:
		bg_palette[0] = kShades[value & 0x3];
		bg_palette[1] = kShades[(value & 0xC) >> 2];
		bg_palette[2] = kShades[(value & 0x30) >> 4];
		bg_palette[3] = kShades[(value & 0xC0) >> 6];
		break;
	case kObj0Palette:
		sprite_palette[0][1] = kShades[(value & 0xC) >> 2];
		sprite_palette[0][2] = kShades[(value & 0x30) >> 4];
		sprite_palette[0][3] = kShades[(value & 0xC0) >> 6];
		break;
	case kObj1Palette:
		sprite_palette[1][1] = kShades[(value & 0xC) >> 2];
		sprite_palette[1][2] = kShades[(value & 0x30) >> 4];
		sprite_palette[1][3] = kShades[(value & 0xC0) >> 6];
		break;
	default:
		break;
	}
}

void Ppu::tick(int cycles)
{
	mode_counter += cycles;

	if (mode == MODE_OAM)
	{
		if (mode_counter >= 83)
		{
			mode = MODE_OAM_VRAM;
			mode_counter = 0;
		}
	}
	else if (mode == MODE_OAM_VRAM)
	{
		if (mode_counter >= 175)
		{
			mode = MODE_HBLANK;
			mode_counter = 0;
		}
	}
	else if (mode == MODE_HBLANK)
	{
		if (mode_counter >= 207)
		{
			if (registers.ly > 0x99)
			{
				mode = MODE_VBLANK;
				draw_scanline(registers.ly);
				registers.ly = 0;
				mode_counter = 0;
				registers.if_ |= 0x1;
			}
			else
			{
				mode = MODE_OAM;
				registers.ly++;
				if (registers.ly == registers.lyc)
				{
					registers.stat |= 0x4;
					registers.if_ |= 0x2;
				}
				draw_scanline(registers.ly);
				mode_counter = 0;
			}
		}
	}
	else if (mode == MODE_VBLANK)
	{
		if (mode_counter >= 4560)
		{
			mode = MODE_OAM;
			mode_counter = 0;
		}
	}

	registers.stat = (registers.stat & 0b1111'1100) | mode;
}
