#include "doctest.h"

#include "cpu.h"
#include "cpu_base.h"
#include "mmu.h"
#include "types.h"

TEST_CASE("increment")
{
	uint8_t flags = 0;
	uint8_t result = 0;

	SUBCASE("Overflow increment")
	{
		INC8(255, &result, &flags);
		CHECK(result == 0);
		CHECK(flags == 0xA0);
	}
	SUBCASE("Non-overflow increment")
	{
		INC8(3, &result, &flags);
		CHECK(flags == 0);
	}
	SUBCASE("Preserve carry flag")
	{
		flags = 0x10;
		INC8(1, &result, &flags);
		CHECK(result == 2);
		CHECK(flags == 0x10);
	}

	int cycles = 0; // unused but necessary argument
	instruction_t i;

	CPU cpu;
	MMU mmu;
	mmu.init();
	cpu.mmu = &mmu;

	SUBCASE("0X")
	{
		i.code = 0x03;
		cpu.bc = 0x1;
		cpu.executeRegular(i, cycles);
		CHECK(cpu.bc == 0x2);
		cpu.bc = 0x00FF;
		cpu.executeRegular(i, cycles);
		CHECK(cpu.bc == 0x0100);

		i.code = 0x04;
		SetHi(cpu.bc, 0x2);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.bc) == 0x3);

		i.code = 0x0C;
		SetLo(cpu.bc, 0x3);
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.bc) == 0x4);
	}

	SUBCASE("1X")
	{
		i.code = 0x13;
		cpu.de = 0x2;
		cpu.executeRegular(i, cycles);
		CHECK(cpu.de == 0x3);
		cpu.de = 0x00FF;
		cpu.executeRegular(i, cycles);
		CHECK(cpu.de == 0x0100);

		i.code = 0x14;
		SetHi(cpu.de, 0x3);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.de) == 0x4);

		i.code = 0x1C;
		SetLo(cpu.de, 0x4);
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.de) == 0x5);
	}

	SUBCASE("2X")
	{
		i.code = 0x23;
		cpu.hl = 0x5;
		cpu.executeRegular(i, cycles);
		CHECK(cpu.hl == 0x6);
		cpu.hl = 0x00FF;
		cpu.executeRegular(i, cycles);
		CHECK(cpu.hl == 0x0100);

		i.code = 0x24;
		SetHi(cpu.hl, 0x6);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.hl) == 0x7);

		i.code = 0x2C;
		SetLo(cpu.hl, 0x8);
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.hl) == 0x9);
	}

	SUBCASE("3X")
	{
		i.code = 0x33;
		cpu.sp = 0xA;
		cpu.executeRegular(i, cycles);
		CHECK(cpu.sp == 0xB);
		cpu.sp = 0x00FF;
		cpu.executeRegular(i, cycles);
		CHECK(cpu.sp == 0x0100);

		i.code = 0x34;
		cpu.hl = 0xC000;
		mmu.mem[cpu.hl] = 0xC;
		cpu.executeRegular(i, cycles);
		CHECK(mmu.mem[0xC000] == 0xD);

		i.code = 0x3C;
		SetHi(cpu.af, 0xE);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0xF);
	}
}
