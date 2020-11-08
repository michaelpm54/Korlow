#include <doctest/doctest.h>

#include "cpu/cpu.h"
#include "cpu/cpu_base.h"
#include "emu_types.h"
#include "memory_map.h"
#include "mmu.h"

TEST_CASE("Load instructions")
{
    u8* mem = new u8[0x10000]();

    Cpu cpu(CpuRegisters {
        .io = mem[kIo],
        .if_ = mem[kIf],
        .ie = mem[kIe],
    });
    Component null_ppu;
    Mmu mmu(cpu, null_ppu, mem);

    SUBCASE("0X")
    {
        /* LD BC, d16 */
        cpu.do_instruction(0x01, 0x0, 0xDEAD, mmu);
        CHECK(cpu.bc == 0xDEAD);

        /* LD (BC), A */
        cpu.a = 0x12;
        cpu.do_instruction(0x02, 0x0, 0x0, mmu);
        CHECK(mem[cpu.bc] == 0x12);

        /* LD B, d8 */
        cpu.do_instruction(0x06, 0x9B, 0x0, mmu);
        CHECK(cpu.b == 0x9B);

        /* LD (a16), SP */
        cpu.sp = 0xD400;
        cpu.do_instruction(0x08, 0x0, 0xC080, mmu);
        CHECK(0xD400 == mmu.read16(0xC080));

        /* LD A, (BC) */
        mem[cpu.bc] = 0x65;
        cpu.do_instruction(0x0A, 0x0, 0x0, mmu);
        CHECK(cpu.a == 0x65);

        /* LD C, d8 */
        cpu.do_instruction(0x0E, 0x99, 0x0, mmu);
        CHECK(cpu.c == 0x99);
    }

    SUBCASE("1X")
    {
        /* LD DE, d16 */
        cpu.do_instruction(0x11, 0x0, 0xBEEF, mmu);
        CHECK(cpu.de == 0xBEEF);

        /* LD (DE), A */
        cpu.a = 0xF8;
        cpu.do_instruction(0x12, 0x0, 0x0, mmu);
        CHECK(mem[cpu.de] == 0xF8);

        /* LD D, d8 */
        cpu.do_instruction(0x16, 0x2B, 0x0, mmu);
        CHECK(cpu.d == 0x2B);

        /* LD A, (DE) */
        mem[cpu.de] = 0xD5;
        cpu.do_instruction(0x1A, 0x0, 0x0, mmu);
        CHECK(cpu.a == 0xD5);

        /* LD E, d8 */
        cpu.do_instruction(0x1E, 0x59, 0x0, mmu);
        CHECK(cpu.e == 0x59);
    }

    SUBCASE("2X")
    {
        /* LD HL, d16 */
        cpu.do_instruction(0x21, 0x0, 0xFA11, mmu);
        CHECK(cpu.hl == 0xFA11);

        /* LD (HL+), A */
        cpu.hl = 0xC020;
        cpu.a = 0x18;
        cpu.do_instruction(0x22, 0x0, 0x0, mmu);
        CHECK(mem[0xC020] == 0x18);
        CHECK(cpu.hl == 0xC021);

        /* LD H, d8 */
        cpu.do_instruction(0x26, 0x6, 0x0, mmu);
        CHECK(cpu.h == 0x6);

        /* LD A, (HL+) */
        cpu.hl = 0xC025;
        mem[cpu.hl] = 0x81;
        cpu.do_instruction(0x2A, 0x0, 0x0, mmu);
        CHECK(cpu.a == 0x81);
        CHECK(cpu.hl == 0xC026);

        /* LD L, d8 */
        cpu.do_instruction(0x2E, 0x07, 0x0, mmu);
        CHECK(cpu.l == 0x07);
    }

    SUBCASE("3X")
    {
        /* LD SP, d16 */
        cpu.do_instruction(0x31, 0x0, 0xC600, mmu);
        CHECK(cpu.sp == 0xC600);

        /* LD (HL-), A */
        cpu.hl = 0xC220;
        cpu.a = 0x44;
        cpu.do_instruction(0x32, 0x0, 0x0, mmu);
        CHECK(mem[0xC220] == 0x44);
        CHECK(cpu.hl == 0xC21F);

        /* LD (HL), d8 */
        cpu.hl = 0xC300;
        cpu.do_instruction(0x36, 0xB7, 0x0, mmu);
        CHECK(mem[0xC300] == 0xB7);

        /* LD A, (HL-) */
        cpu.hl = 0xC310;
        mem[cpu.hl] = 0x92;
        cpu.do_instruction(0x3A, 0x0, 0x0, mmu);
        CHECK(cpu.a == 0x92);
        CHECK(cpu.hl == 0xC30F);

        /* LD A, d8 */
        cpu.do_instruction(0x3E, 0x05, 0x0, mmu);
        CHECK(cpu.a == 0x05);
    }

    SUBCASE("4X")
    {
        /* LD B, B */
        cpu.b = 0x5;
        cpu.do_instruction(0x40, 0, 0, mmu);
        CHECK(cpu.b == 0x5);

        /* LD B, C */
        cpu.bc = 0x0702;
        cpu.do_instruction(0x41, 0, 0, mmu);
        CHECK(cpu.bc == 0x0202);

        /* LD B, D */
        cpu.d = 0x4;
        cpu.do_instruction(0x42, 0, 0, mmu);
        CHECK(cpu.b == 0x4);

        /* LD B, E */
        cpu.e = 0x3;
        cpu.do_instruction(0x43, 0, 0, mmu);
        CHECK(cpu.b == 0x3);

        /* LD B, H */
        cpu.h = 0x4;
        cpu.do_instruction(0x44, 0, 0, mmu);
        CHECK(cpu.b == 0x4);

        /* LD B, L */
        cpu.l = 0x5;
        cpu.do_instruction(0x45, 0, 0, mmu);
        CHECK(cpu.b == 0x5);

        /* LD B, (HL) */
        cpu.hl = 0xC314;
        mem[cpu.hl] = 0x96;
        cpu.do_instruction(0x46, 0, 0, mmu);
        CHECK(cpu.b == 0x96);

        /* LD B, A */
        cpu.a = 0x6;
        cpu.do_instruction(0x47, 0, 0, mmu);
        CHECK(cpu.b == 0x6);

        /* LD C, B */
        cpu.b = 0x7;
        cpu.do_instruction(0x48, 0, 0, mmu);
        CHECK(cpu.c == 0x7);

        /* LD C, C */
        cpu.c = 0xA;
        cpu.do_instruction(0x49, 0, 0, mmu);
        CHECK(cpu.c == 0xA);

        /* LD C, D */
        cpu.d = 0x8;
        cpu.do_instruction(0x4A, 0, 0, mmu);
        CHECK(cpu.c == 0x8);

        /* LD C, E */
        cpu.e = 0x90;
        cpu.do_instruction(0x4B, 0, 0, mmu);
        CHECK(cpu.c == 0x90);

        /* LD C, H */
        cpu.h = 0x39;
        cpu.do_instruction(0x4C, 0, 0, mmu);
        CHECK(cpu.c == 0x39);

        /* LD C, L */
        cpu.l = 0x72;
        cpu.do_instruction(0x4D, 0, 0, mmu);
        CHECK(cpu.c == 0x72);

        /* LD C, (HL) */
        cpu.hl = 0xC318;
        mem[cpu.hl] = 0xC;
        cpu.do_instruction(0x4E, 0, 0, mmu);
        CHECK(cpu.c == 0xC);

        /* LD C, A */
        cpu.a = 0xD;
        cpu.do_instruction(0x4F, 0, 0, mmu);
        CHECK(cpu.c == 0xD);
    }

#if 0
    SUBCASE("5X")
    {
        i.code = 0x50;
        cpu.bc = 0xE;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.d == 0xE);

        i.code = 0x51;
        SetLo(cpu.bc, 0xF);
        cpu.executeRegular(i, cycles);
        CHECK(cpu.d == 0xF);

        // 0x52 = LD D, D, skip

        i.code = 0x53;
        SetLo(cpu.de, 0x10);
        cpu.executeRegular(i, cycles);
        CHECK(cpu.d == 0x10);

        i.code = 0x54;
        cpu.hl = 0x11;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.d == 0x11);

        i.code = 0x55;
        SetLo(cpu.hl, 0x12);
        cpu.executeRegular(i, cycles);
        CHECK(cpu.d == 0x12);

        i.code = 0x56;
        cpu.hl = 0xC32B;
        mem[cpu.hl] = 0x13;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.d == 0x13);

        i.code = 0x57;
        cpu.af = 0x14;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.d == 0x14);

        i.code = 0x58;
        cpu.bc = 0x15;
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.de) == 0x15);

        i.code = 0x59;
        SetLo(cpu.bc, 0x16);
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.de) == 0x16);

        i.code = 0x5A;
        cpu.de = 0x17;
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.de) == 0x17);

        // 0x5B = LD E, E, skip

        i.code = 0x5C;
        cpu.hl = 0x18;
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.de) == 0x18);

        i.code = 0x5D;
        SetLo(cpu.hl, 0x19);
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.de) == 0x19);

        i.code = 0x5E;
        cpu.hl = 0xC318;
        mem[cpu.hl] = 0x1A;
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.de) == 0x1A);

        i.code = 0x5F;
        cpu.af = 0x1B;
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.de) == 0x1B);
    }

    SUBCASE("6X")
    {
        i.code = 0x60;
        cpu.bc = 0x1C;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.h == 0x1C);

        i.code = 0x61;
        SetLo(cpu.bc, 0x1D);
        cpu.executeRegular(i, cycles);
        CHECK(cpu.h == 0x1D);

        i.code = 0x62;
        cpu.de = 0x1E;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.h == 0x1E);

        i.code = 0x63;
        SetLo(cpu.de, 0x1F);
        cpu.executeRegular(i, cycles);
        CHECK(cpu.h == 0x1F);

        i.code = 0x64;
        cpu.hl = 0x20;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.h == 0x20);

        i.code = 0x65;
        SetLo(cpu.hl, 0x21);
        cpu.executeRegular(i, cycles);
        CHECK(cpu.h == 0x21);

        i.code = 0x66;
        cpu.hl = 0xC32B;
        mem[cpu.hl] = 0x22;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.h == 0x22);

        i.code = 0x67;
        cpu.af = 0x23;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.h == 0x23);

        i.code = 0x68;
        cpu.bc = 0x24;
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.hl) == 0x24);

        i.code = 0x69;
        SetLo(cpu.bc, 0x25);
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.hl) == 0x25);

        i.code = 0x6A;
        cpu.de = 0x26;
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.hl) == 0x26);

        i.code = 0x6B;
        SetLo(cpu.de, 0x27);
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.hl) == 0x27);

        i.code = 0x6C;
        cpu.hl = 0x28;
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.hl) == 0x28);

        // 0x6D = LD L, L, skip

        i.code = 0x6E;
        cpu.hl = 0xC318;
        mem[cpu.hl] = 0x29;
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.hl) == 0x29);

        i.code = 0x6F;
        cpu.af = 0x2A;
        cpu.executeRegular(i, cycles);
        CHECK(Lo(cpu.hl) == 0x2A);
    }

    SUBCASE("0x7X")
    {
        i.code = 0x70;
        cpu.hl = 0xC32E;
        cpu.bc = 0x2B;
        cpu.executeRegular(i, cycles);
        CHECK(mmu.read8(cpu.hl) == 0x2B);

        i.code = 0x71;
        cpu.hl = 0xC332;
        SetLo(cpu.bc, 0x2D);
        cpu.executeRegular(i, cycles);
        CHECK(mmu.read8(cpu.hl) == 0x2D);

        i.code = 0x72;
        cpu.hl = 0xC33A;
        cpu.de = 0x2E;
        cpu.executeRegular(i, cycles);
        CHECK(mmu.read8(cpu.hl) == 0x2E);

        i.code = 0x73;
        cpu.hl = 0xC33C;
        SetLo(cpu.de, 0x2F);
        cpu.executeRegular(i, cycles);
        CHECK(mmu.read8(cpu.hl) == 0x2F);

        i.code = 0x74;
        cpu.hl = 0xC33F;
        cpu.hl = 0x30;
        cpu.executeRegular(i, cycles);
        CHECK(mmu.read8(cpu.hl) == 0x30);

        i.code = 0x75;
        cpu.hl = 0xC342;
        SetLo(cpu.hl, 0x31);
        cpu.executeRegular(i, cycles);
        CHECK(mmu.read8(cpu.hl) == 0x31);

        i.code = 0x77;
        cpu.hl = 0xC344;
        cpu.af = 0x32;
        cpu.executeRegular(i, cycles);
        CHECK(mmu.read8(cpu.hl) == 0x32);

        i.code = 0x78;
        cpu.bc = 0x33;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.a == 0x33);

        i.code = 0x79;
        SetLo(cpu.bc, 0x34);
        cpu.executeRegular(i, cycles);
        CHECK(cpu.a == 0x34);

        i.code = 0x7A;
        cpu.de = 0x35;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.a == 0x35);

        i.code = 0x7B;
        SetLo(cpu.de, 0x36);
        cpu.executeRegular(i, cycles);
        CHECK(cpu.a == 0x36);

        i.code = 0x7C;
        cpu.hl = 0x37;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.a == 0x37);

        i.code = 0x7D;
        SetLo(cpu.hl, 0x38);
        cpu.executeRegular(i, cycles);
        CHECK(cpu.a == 0x38);

        i.code = 0x7E;
        cpu.hl = 0xC346;
        mem[cpu.hl] = 0x39;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.a == 0x39);

        // 0x7F = LD A, A, skip
    }

    SUBCASE("EX")
    {
        // LD (C), A -> LD (FF00+C), A

        i.code = 0xE2;
        cpu.af = 0x3A00;
        cpu.bc = 0x0099;
        cpu.executeRegular(i, cycles);
        CHECK(mem[0xFF00 + Lo(cpu.bc)] == 0x3A);

        i = {0xEA, 0x0, 0xA93F, false};
        cpu.af = 0x3B;
        cpu.executeRegular(i, cycles);
        CHECK(mem[i.op16] == 0x3B);
    }

    SUBCASE("FX")
    {
        i.code = 0xF2;
        SetLo(cpu.bc, 0x3C);
        mem[0xFF00 + 0x3C] = 0x3D;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.a == 0x3D);

        i = {0xF8, 0x0, 0x0, false};
        SetLo(cpu.af, 0);
        cpu.sp = 0xC000;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.sp == 0xC000);
        CHECK(cpu.hl == 0xC000);
        CHECK(Lo(cpu.af) == 0);
        i.op8 = 0xFF;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.hl == 0xBFFF);
        i.op8 = 1;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.hl == 0xC001);
        cpu.sp = 0xFF;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.hl == 0x0100);
        CHECK(Lo(cpu.af) == (FLAGS_CARRY | FLAGS_HALFCARRY));

        i.code = 0xF9;
        cpu.hl = 0xCD02;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.sp == 0xCD02);

        i = {0xFA, 0x0, 0xC001, false};
        mem[i.op16] = 0x3F;
        cpu.executeRegular(i, cycles);
        CHECK(cpu.a == 0x3F);
    }
#endif
    delete[] mem;
}
