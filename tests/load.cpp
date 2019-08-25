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
		i.code = 0x41;
		SetLo(cpu.bc, 0x1);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.bc) == 0x1);

		i.code = 0x42;
		SetHi(cpu.de, 0x2);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.bc) == 0x2);
	}
}
