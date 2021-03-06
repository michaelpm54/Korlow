#include "ppu.h"

#include <algorithm>

#include "constants.h"
#include "memory_map.h"

constexpr u8 kShades[4] = {0x00, 0x3F, 0x7E, 0xFF};

u8 decodePixel(u16 row, int pixelIdx)
{
    u8 mask = 0x80 >> pixelIdx;
    u8 lower = !!((row >> 8) & mask);
    u8 upper = !!((row & 0xFF) & mask);
    u8 paletteIdx = (upper << 1) | lower;
    return paletteIdx;
}

Ppu::Ppu(PpuRegisters registers)
    : registers(registers)
    , memory(0x2000)
    , oam(0x100)
    , pixels(kLcdWidth * kLcdHeight)
{
    reset(true);
}

const u8* Ppu::get_pixels() const
{
    return pixels.data();
}

std::array<u8, 8> get_row_colors(u8* row, u8* palette)
{
    std::array<u8, 8> arr {0};
    for (int i = 0; i < 8; i++) {
        const u8 mask = 0x80 >> i;
        arr[i] = palette[!!(row[0] & mask) | !!(row[1] & mask) << 1];
    }
    return arr;
}

u16 get_tile_index(int lcd_x, int lcd_y)
{
    return lcd_x / 8 + (lcd_y / 8) * kMapWidth;
}

void Ppu::draw_scanline(int line)
{
    if (!(registers.lcdc & 0x80) || line >= kLcdHeight) {
        return;
    }

    bool is_signed = true;

    u8* tiles = signedTiles;

    if (registers.lcdc & 0x10) {
        tiles = unsignedTiles;
        is_signed = false;
    }

    u8* bg_map = registers.lcdc & 0x8 ? map1 : map0;

    int y_abs = line + registers.scy;
    int y_map = y_abs / 8;
    int y_px_in_tile = y_abs % 8;

    // background
    for (int x = 0; x < kLcdWidth; x++) {
        /* Offset x pixel by xscroll */
        const int x_abs = x + registers.scx;

        const int x_px_in_tile = x_abs % 8;

        /* Tile at this point in the [x+xscroll, y+yscroll] in the 2D map */
        const u8 map_val = bg_map[get_tile_index(x_abs, y_abs)];
        const int tile_idx = is_signed ? int8_t(map_val) : map_val;

        /* Each tile takes 16 bytes; 2 bytes per row */
        const int row_offset = (tile_idx * 16) + (y_px_in_tile * 2);

        /* Each row is 8 pixels long. Each bit-pair in the byte-pair provides 1 bit depth.
			Combined they provide 2 bit depth. 0-3. The 2nd byte provides the MSB bit. */
        const u8* row = &tiles[row_offset];
        const u8 mask = 0x80 >> x_px_in_tile;

        u8 hi_bit = !!(row[1] & mask);
        u8 lo_bit = !!(row[0] & mask);
        u8 pal_id = (hi_bit << 1) | lo_bit;

        set_pixel(x, line, bg_palette[pal_id]);
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
            int idx = is_signed ? int8_t(map_val) : map_val;

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
        /* Palettes are two bits per colour. */

        /* Examples:
		 * write 0x00 [0]
		 * colours become 0, 0, 0, 0 because shade 0 is 0
		 * write 0x01 [01] = [1]
		 * colours become 0x3F, 0, 0, 0 because shade 1 is 0x3F
		 * write 0x09 [10, 01] = [2, 1]
		 * colours become 0x3F, 0x7F, 0, 0 because shade 1 is 0x3F and shade 2 is 0x7F
		 * write 0xF9 [11, 11, 10, 01] = [3, 3, 2, 1]
		 * colours become 0xFF, 0xFF, 0x7F, 0x3F
		 * 
		 * It makes some sense to have high numbers be dark shades, so [11, 10, 01, 00] or 0xE4 is
		 * common(?)
		*/
        case kBgPalette:
            bg_palette[0] = kShades[value & 0b0000'0011];
            bg_palette[1] = kShades[(value & 0b0000'1100) >> 2];
            bg_palette[2] = kShades[(value & 0b0011'0000) >> 4];
            bg_palette[3] = kShades[(value & 0b1100'0000) >> 6];
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
    cycles += 4;
}

void Ppu::refresh()
{
    for (int i = 1; i <= kLcdHeight; i++)
        draw_scanline(i);
}
