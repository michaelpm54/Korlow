#include "cpu/cpu.h"
#include "cpu/cpu_base.h"
#include "doctest.h"
#include "emu_types.h"
#include "mmu.h"

TEST_CASE("ADD8 sets flags Z0HC")
{
    u8 flags = 0;
    u8 result = 0;

    SUBCASE("Zero is set correctly")
    {
        SUBCASE("Both inputs are zero")
        {
            ADD8(0, 0, &result, &flags);
            CHECK(result == 0);
            CHECK(flags == 0x80);    // z000
        }
        SUBCASE("8 bit int overflow to zero, carry and hc are set")
        {
            ADD8(255, 1, &result, &flags);
            CHECK(result == 0);
            CHECK(flags == 0xB0);    // z0hc
        }
    }
    SUBCASE("Half-carry is set correctly")
    {
        flags = 0;
        ADD8(0b0000'1111, 1, &result, &flags);
        CHECK(flags == 0x20);
    }
    SUBCASE("Addition works")
    {
        ADD8(1, 1, &result, &flags);
        CHECK(result == 2);
    }
}

TEST_CASE("ADD16 sets flags Z0HC")
{
    u8 flags = 0;
    u16 result = 0;

    SUBCASE("Zero is set correctly")
    {
        SUBCASE("Both inputs are zero")
        {
            ADD16(0, 0, &result, &flags);
            CHECK(result == 0);
            CHECK(flags == 0x80);    // z000
        }
        SUBCASE("16 bit int overflow to zero, carry and hc are not set")
        {
            ADD16(0x8000, 0x8000, &result, &flags);
            CHECK(result == 0);
            CHECK(flags == (FLAGS_ZERO | FLAGS_CARRY));    // z00c
        }
    }
    SUBCASE("Half-carry is set correctly")
    {
        flags = 0;
        ADD16(0x0FFF, 0x1, &result, &flags);
        CHECK(result == 0x1000);
        CHECK(flags == FLAGS_HALFCARRY);
    }
}

TEST_CASE("'ADD' instructions")
{
    int cycles = 0;    // unused but necessary argument
    instruction_t i;
    CPU cpu;
    MMU mmu;
    GPU gpu;
    mmu.init(&gpu);
    cpu.mmu = &mmu;

    SUBCASE("0x09 - ADD HL, BC")
    {
        i.code = 0x09;

        cpu.af = FLAGS_ZERO;

        SUBCASE("Z remains unchanged")
        {
            cpu.hl = 0x100;
            cpu.bc = 0x100;
            cpu.executeRegular(i, cycles);
            CHECK(cpu.hl == 0x200);
            CHECK(Lo(cpu.af) == FLAGS_ZERO);
        }

        SUBCASE("Doesn't produce unnecessary HC")
        {
            cpu.hl = 0x01;
            cpu.bc = 0x0F;
            cpu.executeRegular(i, cycles);
            CHECK(cpu.hl == 0x10);
            CHECK(Lo(cpu.af) == FLAGS_ZERO);    // FLAGS_ZERO remains unchanged
        }

        SUBCASE("Doesn't produce unnecessary C")
        {
            cpu.hl = 0x80;
            cpu.bc = 0x80;
            cpu.executeRegular(i, cycles);
            CHECK(cpu.hl == 0x100);
            CHECK(Lo(cpu.af) == FLAGS_ZERO);    // FLAGS_ZERO remains unchanged
        }

        cpu.bc = 0;
    }

    SUBCASE("0x19 - ADD HL, DE")
    {
        i.code = 0x19;

        cpu.af = FLAGS_ZERO;

        SUBCASE("Z remains unchanged")
        {
            cpu.hl = 0x100;
            cpu.de = 0x100;
            cpu.executeRegular(i, cycles);
            CHECK(cpu.hl == 0x200);
            CHECK(cpu.af == FLAGS_ZERO);
        }

        SUBCASE("Doesn't produce unneccessary HC")
        {
            cpu.hl = 1;
            cpu.de = 0xF;
            cpu.executeRegular(i, cycles);
            CHECK(cpu.hl == 16);
            CHECK(cpu.af == FLAGS_ZERO);
        }

        SUBCASE("Doesn't produce unneccessary C")
        {
            cpu.hl = 0x80;
            cpu.de = 0x80;
            cpu.executeRegular(i, cycles);
            CHECK(cpu.hl == 0x100);
            CHECK(cpu.af == FLAGS_ZERO);
        }

        SUBCASE("Produces HC")
        {
            cpu.hl = 0x0FFF;
            cpu.de = 0x0001;
            cpu.executeRegular(i, cycles);
            CHECK(cpu.hl == 0x1000);
            CHECK(cpu.af == (FLAGS_ZERO | FLAGS_HALFCARRY));
        }

        SUBCASE("Produces HC+C")
        {
            cpu.hl = 0xFFFF;
            cpu.de = 0x0001;
            cpu.executeRegular(i, cycles);
            CHECK(cpu.hl == 0x0);
            CHECK(cpu.af == (FLAGS_ZERO | FLAGS_HALFCARRY | FLAGS_CARRY));
        }

        cpu.de = 0;
    }

    SUBCASE("0x29 - ADD HL, HL")
    {
        i.code = 0x29;

        SetLo(cpu.af, FLAGS_ZERO);

        cpu.hl = 0x100;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.hl == 0x200);
        CHECK(Lo(cpu.af) == FLAGS_ZERO);

        cpu.hl = 0x8;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.hl == 0x10);
        CHECK(Lo(cpu.af) == FLAGS_ZERO);

        cpu.hl = 0x80;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.hl == 0x100);
        CHECK(Lo(cpu.af) == FLAGS_ZERO);

        cpu.hl = 0;
    }

    SUBCASE("0x39 - ADD HL, SP")
    {
        i.code = 0x39;

        SetLo(cpu.af, FLAGS_ZERO);
        cpu.hl = 0x100;
        cpu.sp = 0x100;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.hl == 0x200);
        CHECK(Lo(cpu.af) == FLAGS_ZERO);

        cpu.hl = 1;
        cpu.sp = 0xF;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.hl == 16);
        CHECK(Lo(cpu.af) == FLAGS_ZERO);

        cpu.hl = 0x80;
        cpu.sp = 0x80;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.hl == 0x100);
        CHECK(Lo(cpu.af) == FLAGS_ZERO);

        cpu.sp = 0;
    }

    SUBCASE("0xE8 - ADD SP, r8")
    {
        i.code = 0xE8;

        SetLo(cpu.af, (FLAGS_ZERO | FLAGS_SUBTRACT));
        cpu.sp = 2;
        i.op8 = 1;
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.af) == 0);
        CHECK(cpu.sp == 3);

        // check halfcarry
        cpu.sp = 0x8;
        i.op8 = 0x8;
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.af) == FLAGS_HALFCARRY);

        // check carry
        cpu.sp = 0xC0;
        i.op8 = 0x40;
        // 0xC0 + 0x40 = 0x100, also doesn't trigger halfcarry
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.af) == FLAGS_CARRY);

        // check negative
        cpu.sp = 0x50;
        i.op8 = 0xFF;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.sp == 0x4F);
    }

    SUBCASE("0x80 - ADD A, B")
    {
        i.code = 0x80;

        // f: 0100 -> 1000
        // a: 0
        SetLo(cpu.af, FLAGS_SUBTRACT);

        SetHi(cpu.af, 0);
        SetHi(cpu.bc, 0);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == FLAGS_ZERO);
        CHECK(Hi(cpu.af) == 0);

        // f: 0000 -> 0001
        // a: 0
        SetLo(cpu.af, 0);

        SetHi(cpu.af, 0x80);
        SetHi(cpu.bc, 0x80);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
        CHECK(Hi(cpu.af) == 0);

        // f: 0000 -> 0010
        // a: 0x10
        SetLo(cpu.af, 0);

        SetHi(cpu.af, 0x8);
        SetHi(cpu.bc, 0x8);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == FLAGS_HALFCARRY);
        CHECK(Hi(cpu.af) == 0x10);
    }

    SUBCASE("0x81 - ADD A, C")
    {
        i.code = 0x81;

        // f: 0100 -> 1000
        // a: 0
        SetLo(cpu.af, FLAGS_SUBTRACT);

        SetHi(cpu.af, 0);
        SetLo(cpu.bc, 0);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == FLAGS_ZERO);
        CHECK(Hi(cpu.af) == 0);

        // f: 0000 -> 0001
        // a: 0
        SetLo(cpu.af, 0);

        SetHi(cpu.af, 0x80);
        SetLo(cpu.bc, 0x80);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
        CHECK(Hi(cpu.af) == 0);

        // f: 0000 -> 0010
        // a: 0x10
        SetLo(cpu.af, 0);

        SetHi(cpu.af, 0x8);
        SetLo(cpu.bc, 0x8);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == FLAGS_HALFCARRY);
        CHECK(Hi(cpu.af) == 0x10);
    }

    SUBCASE("0x82 - ADD A, D")
    {
        i.code = 0x82;

        // f: 0100 -> 1000
        // a: 0
        SetLo(cpu.af, FLAGS_SUBTRACT);

        SetHi(cpu.af, 0);
        SetHi(cpu.de, 0);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == FLAGS_ZERO);
        CHECK(Hi(cpu.af) == 0);

        // f: 0000 -> 0001
        // a: 0
        SetLo(cpu.af, 0);

        SetHi(cpu.af, 0x80);
        SetHi(cpu.de, 0x80);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
        CHECK(Hi(cpu.af) == 0);

        // f: 0000 -> 0010
        // a: 0x10
        SetLo(cpu.af, 0);

        SetHi(cpu.af, 0x8);
        SetHi(cpu.de, 0x8);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == FLAGS_HALFCARRY);
        CHECK(Hi(cpu.af) == 0x10);
    }

    SUBCASE("0x83 - ADD A, E")
    {
        i.code = 0x83;

        // f: 0100 -> 1000
        // a: 0
        SetLo(cpu.af, FLAGS_SUBTRACT);

        SetHi(cpu.af, 0);
        SetLo(cpu.de, 0);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == FLAGS_ZERO);
        CHECK(Hi(cpu.af) == 0);

        // f: 0000 -> 0001
        // a: 0
        SetLo(cpu.af, 0);

        SetHi(cpu.af, 0x80);
        SetLo(cpu.de, 0x80);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
        CHECK(Hi(cpu.af) == 0);

        // f: 0000 -> 0010
        // a: 0x10
        SetLo(cpu.af, 0);

        SetHi(cpu.af, 0x8);
        SetLo(cpu.de, 0x8);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == FLAGS_HALFCARRY);
        CHECK(Hi(cpu.af) == 0x10);
    }

    SUBCASE("0x84 - ADD A, H")
    {
        i.code = 0x84;

        // f: 0100 -> 1000
        // a: 0
        SetLo(cpu.af, FLAGS_SUBTRACT);

        SetHi(cpu.af, 0);
        SetHi(cpu.hl, 0);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == FLAGS_ZERO);
        CHECK(Hi(cpu.af) == 0);

        // f: 0000 -> 0001
        // a: 0
        SetLo(cpu.af, 0);

        SetHi(cpu.af, 0x80);
        SetHi(cpu.hl, 0x80);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
        CHECK(Hi(cpu.af) == 0);

        // f: 0000 -> 0010
        // a: 0x10
        SetLo(cpu.af, 0);

        SetHi(cpu.af, 0x8);
        SetHi(cpu.hl, 0x8);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == FLAGS_HALFCARRY);
        CHECK(Hi(cpu.af) == 0x10);
    }

    SUBCASE("0x85 - ADD A, L")
    {
        i.code = 0x85;

        // f: 0100 -> 1000
        // a: 0
        SetLo(cpu.af, FLAGS_SUBTRACT);

        SetHi(cpu.af, 0);
        SetLo(cpu.hl, 0);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == FLAGS_ZERO);
        CHECK(Hi(cpu.af) == 0);

        // f: 0000 -> 0001
        // a: 0
        SetLo(cpu.af, 0);

        SetHi(cpu.af, 0x80);
        SetLo(cpu.hl, 0x80);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
        CHECK(Hi(cpu.af) == 0);

        // f: 0000 -> 0010
        // a: 0x10
        SetLo(cpu.af, 0);

        SetHi(cpu.af, 0x8);
        SetLo(cpu.hl, 0x8);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == FLAGS_HALFCARRY);
        CHECK(Hi(cpu.af) == 0x10);
    }

    SUBCASE("0x86 - ADD A, (HL)")
    {
        i.code = 0x86;

        cpu.hl = 0xC000;

        // f: 0100 -> 1000
        // a: 0
        SetLo(cpu.af, FLAGS_SUBTRACT);

        SetHi(cpu.af, 0);
        cpu.mmu->mem[cpu.hl] = 0;

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == FLAGS_ZERO);
        CHECK(cpu.mmu->mem[cpu.hl] == 0);

        // f: 0000 -> 0001
        // a: 0
        SetLo(cpu.af, 0);

        SetHi(cpu.af, 0x80);
        cpu.mmu->mem[cpu.hl] = 0x80;

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
        CHECK(Hi(cpu.af) == 0);

        // f: 0000 -> 0010
        // a: 0x10
        SetLo(cpu.af, 0);

        SetHi(cpu.af, 0x8);
        cpu.mmu->mem[cpu.hl] = 0x8;

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == FLAGS_HALFCARRY);
        CHECK(Hi(cpu.af) == 0x10);
    }

    SUBCASE("0x87 - ADD A, A")
    {
        i.code = 0x87;

        // f: 0100 -> 1000
        // a: 0
        SetLo(cpu.af, FLAGS_SUBTRACT);

        SetHi(cpu.af, 0);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == FLAGS_ZERO);
        CHECK(Hi(cpu.af) == 0);

        // f: 0000 -> 0001
        // a: 0
        SetLo(cpu.af, 0);

        SetHi(cpu.af, 0x80);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
        CHECK(Hi(cpu.af) == 0);

        // f: 0000 -> 0010
        // a: 0x10
        SetLo(cpu.af, 0);

        SetHi(cpu.af, 0x8);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == FLAGS_HALFCARRY);
        CHECK(Hi(cpu.af) == 0x10);
    }

    SUBCASE("0x88 - ADC A, B")
    {
        i.code = 0x88;

        // f: 0100 -> 1000
        // a: 0
        SetLo(cpu.af, FLAGS_SUBTRACT);

        SetHi(cpu.af, 0);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == FLAGS_ZERO);
        CHECK(Hi(cpu.af) == 0);

        // f: 0000 -> 0001
        // a: 0
        SetLo(cpu.af, 0);

        SetHi(cpu.af, 0x80);
        SetHi(cpu.bc, 0x80);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
        CHECK(Hi(cpu.af) == 0);

        // f: 0000 -> 0010
        // a: 0x10
        SetLo(cpu.af, 0);

        SetHi(cpu.af, 0x8);
        SetHi(cpu.bc, 0x8);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == FLAGS_HALFCARRY);
        CHECK(Hi(cpu.af) == 0x10);

        // f: 0001 -> 0000
        // a: 0 -> 1
        SetLo(cpu.af, FLAGS_CARRY);

        SetHi(cpu.af, 0);
        SetHi(cpu.bc, 0);

        cpu.executeRegular(i, cycles);

        CHECK(Lo(cpu.af) == 0);
        CHECK(Hi(cpu.af) == 1);
    }
}
