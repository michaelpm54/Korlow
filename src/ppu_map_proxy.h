#ifndef PPU_MAP_PROXY_H
#define PPU_MAP_PROXY_H

#include <vector>

#include "component.h"

/*
	Writes intended for the PPU first pass through here
	when an instance of this class is given to Gameboy() constructor
	instead of an instance of PPU.

	This is to build a visual representation of the map data.
*/

struct Ppu;

struct PpuMapProxy : Component
{
	PpuMapProxy(Ppu &ppu);

	void reset() override;
	void write8(uint16_t address, uint8_t value) override;
	const uint8_t *get_pixels() const;
	const uint8_t *get_map_pixels() const;

	Ppu &ppu;
	std::vector<uint8_t> map_pixels;
};

#endif // PPU_MAP_PROXY_H
