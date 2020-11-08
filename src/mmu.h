#ifndef MMU_H
#define MMU_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "component.h"
#include "types.h"


struct Mmu : Component {
    Mmu(Component& cpu, Component& ppu, uint8_t* memory);

    void reset(bool) override {};
    uint8_t read8(uint16_t address) override;
    uint16_t read16(uint16_t address) override;
    void write8(uint16_t address, uint8_t value) override;
    void write16(uint16_t address, uint16_t value) override;

    uint8_t* memory;

private:
    Component& cpu;
    Component& ppu;
};

#endif    // MMU_H
