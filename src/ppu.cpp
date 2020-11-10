#include "ppu.h"

#include <algorithm>

#include "constants.h"
#include "memory_map.h"

constexpr u8 kShades[4] = {0x00, 0x3F, 0x7E, 0xFF};

/*
u8 paletteIndex(u8 byte0, u8 byte1, u8 pxIndex)
{
		return ((!!(byte1 & (0x80 >> pxIndex))) << 1) | (!!(byte0 & (0x80 >>
pxIndex)));
}
*/

/*
void decodeTile(u8 *palette, u8 *tile, u8 *pixels)
{
		int b = 0;
		for (int byte = 0; byte < 16; byte++)
		{
				for (int i = 0; i < 8; i++)
				{
						u8 mask = 0x80 >> i;
						u8 upper = !!(tile[byte] & mask);
						u8 lower = !!(tile[byte+1] & mask);
						u8 combined = (lower << 1) | upper;
						pixels[b++] = palette[combined];
				}
				byte++;
		}
}
*/

u8 decodePixel(u16 row, int pixelIdx)
{
    u8 mask = 0x80 >> pixelIdx;
    u8 lower = !!((row >> 8) & mask);
    u8 upper = !!((row & 0xFF) & mask);
    u8 paletteIdx = (upper << 1) | lower;
    return paletteIdx;
}

/*
std::array<std::array<u8, 2>, 8> getTile(u8 *tiles, int patternNum,
bool isSigned)
{
		if (isSigned)
		{
				patternNum = int8_t(patternNum);
		}

		std::array<std::array<u8, 2>, 8> lines;
		memcpy(lines.data(), tiles + (patternNum * 16), 16);
		return lines;
}
*/

Ppu::Ppu(PpuRegisters registers)
    : registers(registers)
    , memory(0x2000)
    , oam(0x100)
    , pixels(kLcdWidth * kLcdHeight)
{
}

const u8* Ppu::get_pixels() const
{
    return pixels.data();
}

void Ppu::draw_scanline(int line)
{
    if (!(registers.lcdc & 0x80) || line >= kLcdHeight)
        return;

    bool isSigned = true;

    u8* tiles = signedTiles;
    if (registers.lcdc & 0x10) {
        tiles = unsignedTiles;
        isSigned = false;
    }

    u8* bgMap = registers.lcdc & 0x8 ? map1 : map0;

    int y_abs = line + registers.scy;
    int y_map = y_abs / 8;
    int y_px_in_tile = y_abs % 8;

    // background
    for (int x = 0; x < kLcdWidth; x++) {
        int x_abs = x + registers.scx;
        int x_map = x_abs / 8;
        int x_px_in_tile = x_abs % 8;

        // Wrap around if it tries to draw past the end of a map
        int idx_offset_in_map = ((y_map * kMapWidth) + x_map) % 0x400;

        u8 map_val = bgMap[idx_offset_in_map];
        int idx = isSigned ? int8_t(map_val) : map_val;

        int tile_offset = idx * 16;
        int row_offset = tile_offset + (y_px_in_tile * 2);

        u16 row = tiles[row_offset] << 8;
        row |= tiles[row_offset + 1];
        u8 colour = bg_palette[decodePixel(row, x_px_in_tile)];

        set_pixel(x, line, colour);
    }

    if (registers.lcdc & 0x20) {
        u8* windowMap = registers.lcdc & 0x40 ? map1 : map0;

        // window
        for (int x = 0; x < kLcdWidth; x++) {
            int x_abs = x + registers.scx;
            int x_map = x_abs / 8;
            int x_px_in_tile = x_abs % 8;

            // Wrap around if it tries to draw past the end of a map
            int idx_offset = ((y_map * kMapWidth) + x_map) % 0x400;
            u8 map_val = windowMap[idx_offset];
            int idx = isSigned ? int8_t(map_val) : map_val;

            int tile_offset = idx * 16;
            int row_offset = tile_offset + (y_px_in_tile * 2);

            u16 row = tiles[row_offset];
            row |= tiles[row_offset + 1];

            u8 colour = bg_palette[decodePixel(row, x_px_in_tile)];

            set_pixel(x, line, colour);
        }
    }

    if (sprites_dirty) {
        memcpy(sprites.data(), oam.data(), sizeof(sprite_t) * 40);
        std::sort(
            sprites.begin(),
            sprites.end(),
            [](const sprite_t& lh, const sprite_t& rh) {
                return lh.x < rh.x;
            });
        for (const auto& sprite : sprites) {
            draw_sprite(sprite);
        }
        sprites_dirty = false;
    }
}

void Ppu::set_pixel(int x, int y, u8 colour)
{
    pixels[(y * kLcdWidth + x) % (kLcdWidth * kLcdHeight)] = colour;
}

void Ppu::reset(bool)
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

void Ppu::write8(u16 address, u8 value)
{
    if (address >= kTileRamUnsigned && address < kCartRam) {
        memory[address - kTileRamUnsigned] = value;
        return;
    }
    else if (address >= kOam && address < kIo) {
        oam[address - kOam] = value;
        return;
    }

    switch (address) {
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

static constexpr int kCyclesPerLine = 456;
static constexpr int kLinesPerVblank = 10;
static constexpr int kMaxLines = kLcdHeight + kLinesPerVblank;
static constexpr int kCyclesPerVblank = kCyclesPerLine * kMaxLines;

void Ppu::tick(bool& redraw)
{
    int frame_progress = cycles % kCyclesPerVblank;
    int line = frame_progress / kCyclesPerLine;

    registers.ly = line;

    if ((registers.lcdc & 0x80) && line != prev_line && line == registers.lyc && (registers.stat & 0x40)) {
        registers.if_ |= 0x2;
        registers.stat |= 0x4;
    }

    if ((registers.lcdc & 0x80) && line != prev_line && (registers.lcdc & 0x80)) {
        draw_scanline(line);
    }

    if (line == 144 && prev_line == 143) {
        redraw = true;
        registers.if_ |= 0x1;
    }

    prev_line = line;
    cycles++;
}

void Ppu::refresh()
{
    for (int i = 1; i <= kLcdHeight; i++)
        draw_scanline(i);
}
