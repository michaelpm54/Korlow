#ifndef MMU_H
#define MMU_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "component.h"

struct Mmu : Component {
    Mmu(Component& cpu, Component& ppu, u8* memory);

    void reset(bool) override {};
    u8 read8(u16 address) override;
    u16 read16(u16 address) override;
    void write8(u16 address, u8 value) override;
    void write16(u16 address, u16 value) override;

    u8* memory;

private:
    Component& cpu;
    Component& ppu;
};

#endif    // MMU_H
