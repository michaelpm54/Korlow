#ifndef GPU_H
#define GPU_H

#include <cstdint>
#include <array>
#include <vector>

#include "component.h"
#include "types.h"

struct sprite_t
{
	uint8_t y;
	uint8_t x;
	uint8_t patternNum;
	uint8_t flags;
};

enum Mode
{
	MODE_HBLANK = 0,
	MODE_VBLANK = 1,
	MODE_OAM = 2,
	MODE_OAM_VRAM = 3,
};

struct PpuRegisters
{
	uint8_t &if_;
	uint8_t &lcdc;
	uint8_t &stat;
	uint8_t &scx;
	uint8_t &scy;
	uint8_t &ly;
	uint8_t &lyc;
	uint8_t &wy;
	uint8_t &wx;
};

struct Ppu : Component
{
	Ppu(PpuRegisters);

	const uint8_t *get_pixels() const;
	void reset() override;
	void write8(uint16_t address, uint8_t value) override;
	void tick(int cycles);

	void set_pixel(int x, int y, uint8_t colour);
	void draw_sprite(const sprite_t &sprite) {}
	void draw_scanline(int line);

	std::array<sprite_t, 40> sprites;
	bool sprites_dirty { false };

	PpuRegisters registers;
	
	int mode { MODE_OAM };
	int mode_counter { 0 };

	uint8_t bg_palette[4];
	uint8_t sprite_palette[2][4];

	std::vector<uint8_t> memory;
	std::vector<uint8_t> oam;

	uint8_t *unsignedTiles { nullptr };
	uint8_t *signedTiles { nullptr };
	uint8_t *map0 { nullptr };
	uint8_t *map1 { nullptr };

	std::vector<uint8_t> pixels;
};

struct PpuMapProxy : Component
{
	PpuMapProxy(Ppu &ppu);

	void reset() override;
	void write8(uint16_t address, uint8_t value) override;
	const uint8_t *get_pixels() const;

	Ppu &ppu;
	std::vector<uint8_t> map_pixels;
};

#endif // GPU_H
