#include "render/map_window.h"

#include "ppu.h"

MapWindow::MapWindow(Ppu *ppu)
    : m_ppu(ppu)
{
    texture_init(&m_texture, 32, 64, 4);
}

void MapWindow::update()
{
    std::array<u32, 32 * 64> pixels {0};

    int idx = 0;
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 32; j++) {
            u8 val = m_ppu->memory[0x1800 + idx] * 3;
            pixels[idx] = 0xFF263267 | (val << 16) | (val << 8) | val;
            idx++;
        }
    }

    texture_set_pixels(&m_texture, reinterpret_cast<const u8 *>(pixels.data()));
}
