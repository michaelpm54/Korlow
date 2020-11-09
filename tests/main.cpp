#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "cpu/cpu.h"
#include "cpu/cpu_base.h"
#include "emu_types.h"
#include "memory_map.h"
#include "mmu.h"

TEST_CASE("Base operations")
{
    u8* mem = new u8[0x10000]();

    Cpu cpu(CpuRegisters {
        .io = mem[kIo],
        .if_ = mem[kIf],
        .ie = mem[kIe],
    });
    Component null_ppu;
    Mmu mmu(cpu, null_ppu, mem);

    /* Z00C */
    SUBCASE("Rotate left")
    {
        /* Carry bit goes into LSB */
        u8 flags = FLAGS_CARRY;
        u8 input = 0;
        u8 output = 0;
        RL(input, &output, &flags);
        CHECK(output == 1);

        /* Left shift works on normal number */
        /* 2 << 1 = 4 */
        input = 2;
        RL(input, &output, &flags);
        CHECK(output == 4);

        /* Zero flag gets set */
        /* Left shift works on zero */
        /* 0 << 0 = 0 */
        flags = 0;
        input = 0;
        RL(input, &output, &flags);
        CHECK(output == 0);
        CHECK(flags == FLAGS_ZERO);

        /* MSB goes into carry bit */
        /* Left shift works on overflow */
        flags = 0;
        input = 0x80;
        RL(input, &output, &flags);
        CHECK(output == 0x00);    // 0x100 & 0xFF
        CHECK(flags == (FLAGS_ZERO | FLAGS_CARRY));
    }

    SUBCASE("Rotate left through carry")
    {
        /* Shift works */
        u8 flags = 0;
        u8 input = 1;
        u8 output = 0;
        RLC(input, &output, &flags);
        CHECK(output == 2);

        /* Carry bit gets set */
        /* MSB goes into LSB */
        input = 0xFF;
        RLC(input, &output, &flags);
        CHECK(flags == FLAGS_CARRY);
        CHECK(output == 0xFF);    // 0xFE + Carry(1) = 0xFF

        /* Zero gets set */
        input = 0;
        flags = 0;
        RLC(input, &output, &flags);
        CHECK(output == 0);
        CHECK(flags == FLAGS_ZERO);
    }
}
