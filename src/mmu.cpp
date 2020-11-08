#include "mmu.h"

#include "memory_map.h"

bool is_ppu_address(u16 address)
{
    return (address >= kOam && address < kIo) || (address >= kTileRamUnsigned && address < kCartRam) || (address >= kLcdc && address < kZeroPage);
}

bool is_cpu_address(u16 address)
{
    return (address == kIo) || (address == kIe);
}

Mmu::Mmu(Component &cpu, Component &ppu, u8 *memory)
    : cpu(cpu)
    , ppu(ppu)
    , memory(memory)
{
}

u8 Mmu::read8(u16 address)
{
    return memory[address];
}

u16 Mmu::read16(u16 address)
{
    return u16(read8(address + 1) << 8) + read8(address);
}

void Mmu::write8(u16 addr, u8 value)
{
    if (addr == kIf) {
        value |= 0b1110'0000;
        cpu.write8(kIf, value);
        ppu.write8(kIf, value);
    }
    else if (addr == kIo)    // P1/JOYP
    {
        value |= 0xCF;
    }
    else if (addr == 0xFF02)    // Serial transfer control
    {
        value |= 0b0111'1100;
    }
    else if (is_ppu_address(addr)) {
        if (addr == kDmaStartAddr) {
            for (int i = 0; i < 0xA0; i++)
                write8(kOam + i, read8((u16(value) << 8) + i));
        }
        else
            ppu.write8(addr, value);
    }
    else if (is_cpu_address(addr)) {
        cpu.write8(addr, value);
    }

    // Let the other components do their thing, then write regardless.
    memory[addr] = value;
}

void Mmu::write16(u16 address, u16 value)
{
    write8(address, static_cast<u8>(value));
    write8(address + 1, (value & 0xFF00) >> 8);
}
