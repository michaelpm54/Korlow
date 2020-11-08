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

struct PpuMapProxy : Component {
    PpuMapProxy(Ppu& ppu);

    void reset(bool) override;
    void write8(u16 address, u8 value) override;
    const u8* get_pixels() const;
    const u8* get_map_pixels() const;

    Ppu& ppu;
    std::vector<u32> map_pixels;
};

#endif    // PPU_MAP_PROXY_H
