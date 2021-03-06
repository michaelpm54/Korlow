#include "cpu/cpu.h"
#include "cpu/cpu_base.h"
#include "doctest.h"
#include "emu_types.h"
#include "mmu.h"

TEST_CASE("decrement")
{
    u8 flags = 0;
    u8 result = 0;

    SUBCASE("Overflow decrement")
    {
        DEC8(0, &result, &flags);
        CHECK(result == 0xFF);
        CHECK(flags == (FLAGS_SUBTRACT | FLAGS_HALFCARRY));
    }
    SUBCASE("Non-overflow decrement")
    {
        flags = 0;
        DEC8(1, &result, &flags);
        CHECK(flags == (FLAGS_ZERO | FLAGS_SUBTRACT));
    }
    SUBCASE("Preserve carry flag")
    {
        flags = FLAGS_CARRY;
        DEC8(1, &result, &flags);
        CHECK(result == 0);
        CHECK(flags == (FLAGS_ZERO | FLAGS_SUBTRACT | FLAGS_CARRY));
    }

    int cycles = 0;    // unused but necessary argument
    instruction_t i;

    CPU cpu;
    MMU mmu;
    GPU gpu;
    mmu.init(&gpu);
    cpu.mmu = &mmu;

    SUBCASE("0X")
    {
        SetLo(cpu.af, 0);
        i.code = 0x5;
        SetHi(cpu.bc, 0x1);
        cpu.executeRegular(i, cycles);
        CHECK(Hi(cpu.bc) == 0x0);
        CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_SUBTRACT));
        SetHi(cpu.bc, 0);
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.af) == (FLAGS_SUBTRACT | FLAGS_HALFCARRY));
        SetLo(cpu.af, 0);

        i.code = 0xB;
        cpu.bc = 0x0001;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.bc == 0x0000);
        cpu.bc = 0x0100;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.bc == 0x00FF);

        i.code = 0xD;
        SetLo(cpu.bc, 0x1);
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.bc) == 0x0);
        CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_SUBTRACT));
        SetLo(cpu.bc, 0);
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.af) == (FLAGS_SUBTRACT | FLAGS_HALFCARRY));
        SetLo(cpu.af, 0);
    }

    SUBCASE("1X")
    {
        i.code = 0x15;
        SetHi(cpu.de, 0x1);
        cpu.executeRegular(i, cycles);
        CHECK(Hi(cpu.de) == 0x0);
        CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_SUBTRACT));
        SetHi(cpu.de, 0);
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.af) == (FLAGS_SUBTRACT | FLAGS_HALFCARRY));
        SetLo(cpu.af, 0);

        i.code = 0x1B;
        cpu.de = 0x0001;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.de == 0x0000);
        cpu.de = 0x0100;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.de == 0x00FF);

        i.code = 0x1D;
        SetLo(cpu.de, 0x1);
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.de) == 0x0);
        CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_SUBTRACT));
        SetLo(cpu.de, 0);
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.af) == (FLAGS_SUBTRACT | FLAGS_HALFCARRY));
        SetLo(cpu.af, 0);
    }

    SUBCASE("2X")
    {
        i.code = 0x25;
        SetHi(cpu.hl, 0x1);
        cpu.executeRegular(i, cycles);
        CHECK(Hi(cpu.hl) == 0x0);
        CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_SUBTRACT));
        SetHi(cpu.hl, 0);
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.af) == (FLAGS_SUBTRACT | FLAGS_HALFCARRY));
        SetLo(cpu.af, 0);

        i.code = 0x2B;
        cpu.hl = 0x0001;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.hl == 0x0000);
        cpu.hl = 0x0100;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.hl == 0x00FF);

        i.code = 0x2D;
        SetLo(cpu.hl, 0x1);
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.hl) == 0x0);
        CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_SUBTRACT));
        SetLo(cpu.hl, 0);
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.af) == (FLAGS_SUBTRACT | FLAGS_HALFCARRY));
        SetLo(cpu.af, 0);
    }

    SUBCASE("3X")
    {
        i.code = 0x35;
        cpu.hl = 0xC000;
        mmu.mem[cpu.hl] = 0x1;
        cpu.executeRegular(i, cycles);
        CHECK(mmu.mem[cpu.hl] == 0x0);
        CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_SUBTRACT));
        mmu.mem[cpu.hl] = 0;
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.af) == (FLAGS_SUBTRACT | FLAGS_HALFCARRY));
        SetLo(cpu.af, 0);

        i.code = 0x3B;
        cpu.sp = 0xC002;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.sp == 0xC001);
        cpu.sp = 0x0100;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.sp == 0x00FF);

        i.code = 0x3D;
        SetHi(cpu.af, 0x1);
        cpu.executeRegular(i, cycles);
        CHECK(Hi(cpu.af) == 0x0);
        CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_SUBTRACT));
        SetHi(cpu.af, 0);
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.af) == (FLAGS_SUBTRACT | FLAGS_HALFCARRY));
        SetLo(cpu.af, 0);
    }
}
