#include "doctest.h"
#include "cpu.h"
#include "mmu.h"
#include "cpu_base.h"
#include "types.h"

TEST_CASE("Load instructions")
{
	int cycles = 0; // unused but necessary argument
	instruction_t i;

	CPU cpu;
	MMU mmu;
	mmu.init();
	cpu.mmu = &mmu;

	SUBCASE("0X")
	{
		i = { 0x01, 0x0, 0xDEAD, false };
		cpu.executeRegular(i, cycles);
		CHECK(cpu.bc == 0xDEAD);

		SetHi(cpu.af, 0x12);
		i = { 0x02, 0x0, 0x0, false };
		cpu.executeRegular(i, cycles);
		CHECK(mmu.mem[cpu.bc] == 0x12);

		i = { 0x06, 0x9B, 0x0, false };
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.bc) == 0x9B);

		cpu.sp = 0xD400;
		i = { 0x08, 0x0, 0xC080, false };
		cpu.executeRegular(i, cycles);
		CHECK(0xD400 == mmu.read16(0xC080));

		mmu.mem[cpu.bc] = 0x65;
		i = { 0x0A, 0x0, 0x0, false };
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0x65);

		i = { 0x0E, 0x99, 0x0, false };
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.bc) == 0x99);
	}

	SUBCASE("1X")
	{
		i = { 0x11, 0x0, 0xBEEF, false };
		cpu.executeRegular(i, cycles);
		CHECK(cpu.de == 0xBEEF);

		SetHi(cpu.af, 0xF8);
		i = { 0x12, 0x0, 0x0, false };
		cpu.executeRegular(i, cycles);
		CHECK(mmu.mem[cpu.de] == 0xF8);

		i = { 0x16, 0x2B, 0x0, false };
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.de) == 0x2B);

		mmu.mem[cpu.de] = 0xD5;
		i = { 0x1A, 0x0, 0x0, false };
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0xD5);

		i = { 0x1E, 0x59, 0x0, false };
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.de) == 0x59);
	}

	SUBCASE("2X")
	{
		i = { 0x21, 0x0, 0xFA11, false };
		cpu.executeRegular(i, cycles);
		CHECK(cpu.hl == 0xFA11);

		cpu.hl = 0xC020;
		SetHi(cpu.af, 0x18);
		i = { 0x22, 0x0, 0x0, false };
		cpu.executeRegular(i, cycles);
		CHECK(mmu.mem[0xC020] == 0x18);
		CHECK(cpu.hl == 0xC021);

		i = { 0x26, 0x06, 0x0, false };
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.hl) == 0x06);

		cpu.hl = 0xC025;
		mmu.mem[cpu.hl] = 0x81;
		i = { 0x2A, 0x0, 0x0, false };
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0x81);
		CHECK(cpu.hl == 0xC026);

		i = { 0x2E, 0x07, 0x0, false };
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.hl) == 0x07);
	}

	SUBCASE("3X")
	{
		i = { 0x31, 0x0, 0xC600, false };
		cpu.executeRegular(i, cycles);
		CHECK(cpu.sp == 0xC600);

		cpu.hl = 0xC220;
		SetHi(cpu.af, 0x44);
		i = { 0x32, 0x0, 0x0, false };
		cpu.executeRegular(i, cycles);
		CHECK(mmu.mem[0xC220] == 0x44);
		CHECK(cpu.hl == 0xC21F);

		i = { 0x36, 0xB7, 0x0, false };
		cpu.hl = 0xC300;
		cpu.executeRegular(i, cycles);
		CHECK(mmu.mem[0xC300] == 0xB7);

		cpu.hl = 0xC310;
		mmu.mem[cpu.hl] = 0x92;
		i = { 0x3A, 0x0, 0x0, false };
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0x92);
		CHECK(cpu.hl == 0xC30F);

		i = { 0x3E, 0x05, 0x0, false };
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0x05);
	}

	SUBCASE("4X")
	{
		// 0x40 = LD B, B, skip

		i.code = 0x41;
		SetLo(cpu.bc, 0x1);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.bc) == 0x1);

		i.code = 0x42;
		SetHi(cpu.de, 0x2);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.bc) == 0x2);

		i.code = 0x43;
		SetLo(cpu.de, 0x3);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.bc) == 0x3);

		i.code = 0x44;
		SetHi(cpu.hl, 0x4);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.bc) == 0x4);

		i.code = 0x45;
		SetLo(cpu.hl, 0x5);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.bc) == 0x5);

		i.code = 0x46;
		cpu.hl = 0xC314;
		mmu.mem[cpu.hl] = 0x96;
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.bc) == 0x96);

		i.code = 0x47;
		SetHi(cpu.af, 0x6);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.bc) == 0x6);

		i.code = 0x48;
		SetHi(cpu.bc, 0x7);
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.bc) == 0x7);

		// 0x49 = LD C, C, skip

		i.code = 0x4A;
		SetHi(cpu.de, 0x8);
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.bc) == 0x8);

		i.code = 0x4B;
		SetLo(cpu.de, 0x9);
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.bc) == 0x9);

		i.code = 0x4C;
		SetHi(cpu.hl, 0xA);
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.bc) == 0xA);

		i.code = 0x4D;
		SetLo(cpu.hl, 0xB);
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.bc) == 0xB);

		i.code = 0x4E;
		cpu.hl = 0xC318;
		mmu.mem[cpu.hl] = 0xC;
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.bc) == 0xC);

		i.code = 0x4F;
		SetHi(cpu.af, 0xD);
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.bc) == 0xD);
	}

	SUBCASE("5X")
	{
		i.code = 0x50;
		SetHi(cpu.bc, 0xE);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.de) == 0xE);

		i.code = 0x51;
		SetLo(cpu.bc, 0xF);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.de) == 0xF);

		// 0x52 = LD D, D, skip

		i.code = 0x53;
		SetLo(cpu.de, 0x10);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.de) == 0x10);

		i.code = 0x54;
		SetHi(cpu.hl, 0x11);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.de) == 0x11);

		i.code = 0x55;
		SetLo(cpu.hl, 0x12);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.de) == 0x12);

		i.code = 0x56;
		cpu.hl = 0xC32B;
		mmu.mem[cpu.hl] = 0x13;
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.de) == 0x13);

		i.code = 0x57;
		SetHi(cpu.af, 0x14);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.de) == 0x14);

		i.code = 0x58;
		SetHi(cpu.bc, 0x15);
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.de) == 0x15);

		i.code = 0x59;
		SetLo(cpu.bc, 0x16);
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.de) == 0x16);

		i.code = 0x5A;
		SetHi(cpu.de, 0x17);
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.de) == 0x17);

		// 0x5B = LD E, E, skip

		i.code = 0x5C;
		SetHi(cpu.hl, 0x18);
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.de) == 0x18);

		i.code = 0x5D;
		SetLo(cpu.hl, 0x19);
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.de) == 0x19);

		i.code = 0x5E;
		cpu.hl = 0xC318;
		mmu.mem[cpu.hl] = 0x1A;
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.de) == 0x1A);

		i.code = 0x5F;
		SetHi(cpu.af, 0x1B);
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.de) == 0x1B);
	}

	SUBCASE("6X")
	{
		i.code = 0x60;
		SetHi(cpu.bc, 0x1C);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.hl) == 0x1C);

		i.code = 0x61;
		SetLo(cpu.bc, 0x1D);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.hl) == 0x1D);

		i.code = 0x62;
		SetHi(cpu.de, 0x1E);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.hl) == 0x1E);

		i.code = 0x63;
		SetLo(cpu.de, 0x1F);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.hl) == 0x1F);

		i.code = 0x64;
		SetHi(cpu.hl, 0x20);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.hl) == 0x20);

		i.code = 0x65;
		SetLo(cpu.hl, 0x21);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.hl) == 0x21);

		i.code = 0x66;
		cpu.hl = 0xC32B;
		mmu.mem[cpu.hl] = 0x22;
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.hl) == 0x22);

		i.code = 0x67;
		SetHi(cpu.af, 0x23);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.hl) == 0x23);

		i.code = 0x68;
		SetHi(cpu.bc, 0x24);
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.hl) == 0x24);

		i.code = 0x69;
		SetLo(cpu.bc, 0x25);
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.hl) == 0x25);

		i.code = 0x6A;
		SetHi(cpu.de, 0x26);
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.hl) == 0x26);

		i.code = 0x6B;
		SetLo(cpu.de, 0x27);
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.hl) == 0x27);

		i.code = 0x6C;
		SetHi(cpu.hl, 0x28);
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.hl) == 0x28);

		// 0x6D = LD L, L, skip

		i.code = 0x6E;
		cpu.hl = 0xC318;
		mmu.mem[cpu.hl] = 0x29;
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.hl) == 0x29);

		i.code = 0x6F;
		SetHi(cpu.af, 0x2A);
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.hl) == 0x2A);
	}

	SUBCASE("0x7X")
	{
		i.code = 0x70;
		cpu.hl = 0xC32E;
		SetHi(cpu.bc, 0x2B);
		cpu.executeRegular(i, cycles);
		CHECK(mmu.read8(cpu.hl) == 0x2B);

		i.code = 0x71;
		cpu.hl = 0xC332;
		SetLo(cpu.bc, 0x2D);
		cpu.executeRegular(i, cycles);
		CHECK(mmu.read8(cpu.hl) == 0x2D);

		i.code = 0x72;
		cpu.hl = 0xC33A;
		SetHi(cpu.de, 0x2E);
		cpu.executeRegular(i, cycles);
		CHECK(mmu.read8(cpu.hl) == 0x2E);

		i.code = 0x73;
		cpu.hl = 0xC33C;
		SetLo(cpu.de, 0x2F);
		cpu.executeRegular(i, cycles);
		CHECK(mmu.read8(cpu.hl) == 0x2F);

		i.code = 0x74;
		cpu.hl = 0xC33F;
		SetHi(cpu.hl, 0x30);
		cpu.executeRegular(i, cycles);
		CHECK(mmu.read8(cpu.hl) == 0x30);

		i.code = 0x75;
		cpu.hl = 0xC342;
		SetLo(cpu.hl, 0x31);
		cpu.executeRegular(i, cycles);
		CHECK(mmu.read8(cpu.hl) == 0x31);

		i.code = 0x77;
		cpu.hl = 0xC344;
		SetHi(cpu.af, 0x32);
		cpu.executeRegular(i, cycles);
		CHECK(mmu.read8(cpu.hl) == 0x32);

		i.code = 0x78;
		SetHi(cpu.bc, 0x33);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0x33);

		i.code = 0x79;
		SetLo(cpu.bc, 0x34);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0x34);

		i.code = 0x7A;
		SetHi(cpu.de, 0x35);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0x35);

		i.code = 0x7B;
		SetLo(cpu.de, 0x36);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0x36);

		i.code = 0x7C;
		SetHi(cpu.hl, 0x37);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0x37);

		i.code = 0x7D;
		SetLo(cpu.hl, 0x38);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0x38);

		i.code = 0x7E;
		cpu.hl = 0xC346;
		mmu.mem[cpu.hl] = 0x39;
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0x39);

		// 0x7F = LD A, A, skip
	}

	SUBCASE("EX")
	{
		i.code = 0xE2;
		SetHi(cpu.af, 0x3A)	;
		cpu.executeRegular(i, cycles);
		CHECK(mmu.mem[0xFF00 + Lo(cpu.bc)] == 0x3A);

		i = { 0xEA, 0x0, 0xA93F, false };
		SetHi(cpu.af, 0x3B);
		cpu.executeRegular(i, cycles);
		CHECK(mmu.mem[i.op16] == 0x3B);
	}

	SUBCASE("FX")
	{
		i.code = 0xF2;
		SetLo(cpu.bc, 0x3C);
		mmu.mem[0xFF00 + 0x3C] = 0x3D;
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0x3D);

		i = { 0xF8, 0x0, 0x0, false };
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

		i = { 0xFA, 0x0, 0xC001, false };
		mmu.mem[i.op16] = 0x3F;
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0x3F);
	}
}
