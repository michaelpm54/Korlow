#include "render/tiles_window.h"

#include <array>

#include "emu_types.h"

TilesWindow::TilesWindow(u8* mem, u8* palette)
{
    m_mem = mem;
    m_pal = palette;
    texture_init(&m_texture, 24 * 8, 16 * 8, 4);
}

void TilesWindow::update()
{
    u8* map = &m_mem[0x9800];
    u8* tiles = &m_mem[0x8000];

    const int tiles_w = 24;
    const int tiles_h = 16;
    const int tex_w = tiles_w * 8;
    const int tex_h = tiles_h * 8;
    const int n_pixels = tex_w * tex_h;

    std::array<u32, n_pixels> pixels {0};

    for (int y = 0; y < tex_h; y++) {
        for (int x = 0; x < tex_w; x++) {
            int tile_x = x / 8;
            int tile_y = y / 8;
            int tile_idx = tile_y * tiles_w + tile_x;
            int row_addr = (tile_idx * 16) + ((y % 8) * 2);
            u8* row = &tiles[row_addr];
            u8 mask = 0x80 >> (x % 8);
            u8 pal_idx = !!(row[0] & mask) | !!(row[1] & mask) << 1;
            u8 pc = m_pal[pal_idx];
            u32 c = 0xFF000000 | (pc << 16) | (pc << 8) | pc;
            pixels[y * tex_w + x] = c;
        }
    }

    texture_set_pixels(&m_texture, reinterpret_cast<const u8*>(pixels.data()));
}
