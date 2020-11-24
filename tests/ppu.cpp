#include "ppu.h"

#include <doctest/doctest.h>

#include "cpu/cpu.h"
#include "cpu/cpu_base.h"
#include "emu_types.h"
#include "memory_map.h"
#include "mmu.h"

TEST_CASE("PPU produces correct pixels")
{
    u8* mem = new u8[0x10000]();

    Cpu cpu(CpuRegisters {
        .io = mem[kIo],
        .if_ = mem[kIf],
        .ie = mem[kIe],
    });
    Component null_ppu;
    Ppu ppu(PpuRegisters {
        .if_ = mem[kIf],
        .lcdc = mem[kLcdc],
        .stat = mem[kStat],
        .scx = mem[kScx],
        .scy = mem[kScy],
        .ly = mem[kLy],
        .lyc = mem[kLyc],
        .wy = mem[kWy],
        .wx = mem[kWx],
    });
    Mmu mmu(cpu, ppu, mem);

    /* Enable LCD */
    mem[kLcdc] = 0x80;
    /* Unsigned map */
    mem[kLcdc] |= 0x10;

    /* LD (HL), 0xE4 */
    /* Selects shades 3-0 in order. */
    /* 0xE4 = 11 10 01 00 */
    /*      =  3  2  1  0 */
    cpu.hl = kBgPalette;
    cpu.do_instruction(0x36, 0xE4, 0, mmu);
    CHECK(mem[kBgPalette] == 0xE4);

    /* Write a completely black tile to tile 66. */
    cpu.hl = kTileRamUnsigned + 66 * 16;
    for (int i = 0; i < 16; i++) {
        cpu.do_instruction(0x36, 0xFF, 0, mmu);
        cpu.hl++;
    }

    /* Set the first map entry to tile 66. */
    cpu.hl = kMap0 + 0;
    cpu.do_instruction(0x36, 66, 0, mmu);

    /* Write an alternatingly coloured tile to tile 10. */
    cpu.hl = kTileRamUnsigned + 10 * 16;
    for (int i = 0; i < 16; i++) {
        cpu.do_instruction(0x36, 0xAA, 0, mmu);
        cpu.hl++;
    }

    /* Set the 3rd map entry to tile 10. */
    cpu.hl = kMap0 + 2;
    cpu.do_instruction(0x36, 10, 0, mmu);

    CHECK(mem[kMap0 + 0] == 66);
    CHECK(mem[kMap0 + 2] == 10);

    bool redraw {false};
    for (int i = 0; i < 1000; i++) {
        ppu.tick(redraw);
    }

    CHECK(ppu.pixels[0] == 0xFF);              // tile0 x0 y0
    CHECK(ppu.pixels[7 * 160 + 7] == 0xFF);    // tile0 x7 y7
    CHECK(ppu.pixels[7 * 160 + 8] == 0);       // tile0 x8 y7
    CHECK(ppu.pixels[8 * 160 + 0] == 0);       // tile0 y7 y8

    CHECK(ppu.pixels[16] == 0xFF);    // tile2 x0 y0
    CHECK(ppu.pixels[17] == 0);       // tile2 x1 y0
    CHECK(ppu.pixels[18] == 0xFF);    // tile2 x2 y0
    CHECK(ppu.pixels[19] == 0);       // tile2 x3 y0

    delete[] mem;
}
