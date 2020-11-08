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
    std::fill(std::begin(map_pixels), std::end(map_pixels), 0);
    ppu.reset(sb);
}

void PpuMapProxy::write8(u16 address, u8 value)
{
    if (address >= kMap0 && address <= kMap1) {
        u16 map_address = address - kMap0;
        int x = map_address % kMapWidth;
        int y = map_address / kMapWidth;
        int px_index {y * kMapWidth + x};
        map_pixels[px_index] = 0xFFAA7755 | u32((!!value) * 0xFF) << 4;
    }

    ppu.write8(address, value);
}

const u8* PpuMapProxy::get_pixels() const
{
    return ppu.get_pixels();
}

const u8* PpuMapProxy::get_map_pixels() const
{
    return reinterpret_cast<const u8*>(map_pixels.data());
}
