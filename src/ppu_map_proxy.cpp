#include "ppu_map_proxy.h"

#include "constants.h"
#include "memory_map.h"
#include "ppu.h"

PpuMapProxy::PpuMapProxy(Ppu& ppu)
    : ppu(ppu)
    , map_pixels(0x800)
{
}

void PpuMapProxy::reset(bool sb)
{
    std::fill(std::begin(map_pixels), std::end(map_pixels), 0x12);
    ppu.reset(sb);
}

void PpuMapProxy::write8(uint16_t address, uint8_t value)
{
    if (address >= kMap0 && address <= kMap1) {
        uint16_t map_address = address - kMap0;
        int x = map_address % kMapWidth;
        int y = map_address / kMapWidth;
        int px_index {y * kMapWidth + x};
        map_pixels[px_index] = value;
    }

    ppu.write8(address, value);
}

const uint8_t* PpuMapProxy::get_pixels() const
{
    return ppu.get_pixels();
}

const uint8_t* PpuMapProxy::get_map_pixels() const
{
    return map_pixels.data();
}
