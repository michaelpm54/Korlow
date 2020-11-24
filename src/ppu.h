#ifndef GPU_H
#define GPU_H

#include <array>
#include <cstdint>
#include <vector>

#include "component.h"

struct sprite_t {
    u8 y;
    u8 x;
    u8 patternNum;
    u8 flags;
};

enum Mode {
    MODE_HBLANK = 0,
    MODE_VBLANK = 1,
    MODE_OAM = 2,
    MODE_OAM_VRAM = 3,
};

struct PpuRegisters {
    u8& if_;
    u8& lcdc;
    u8& stat;
    u8& scx;
    u8& scy;
    u8& ly;
    u8& lyc;
    u8& wy;
    u8& wx;
};

struct Ppu : Component {
    Ppu(PpuRegisters);

    void refresh();
    const u8* get_pixels() const;
    void reset(bool) override;
    void write8(u16 address, u8 value) override;
    void tick(bool& redraw);

    void set_pixel(int x, int y, u8 colour);
    void draw_sprite(const sprite_t& sprite)
    {
    }
    void draw_scanline(int line);

    std::array<sprite_t, 40> sprites;
    bool sprites_dirty {false};

    PpuRegisters registers;

    int mode {MODE_OAM};
    int mode_counter {0};

    u8 bg_palette[4];
    u8 sprite_palette[2][4];

    std::vector<u8> memory;
    std::vector<u8> oam;

    u8* unsignedTiles {nullptr};
    u8* signedTiles {nullptr};
    u8* map0 {nullptr};
    u8* map1 {nullptr};

    std::vector<u8> pixels;

    int cycles {0};
    int prev_line {-1};
};

#endif    // GPU_H
