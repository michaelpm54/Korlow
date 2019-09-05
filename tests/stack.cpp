#include "doctest.h"
#include "cpu.h"
#include "mmu.h"
#include "cpu_base.h"
#include "types.h"

TEST_CASE("Stack instructions")
{
	int cycles = 0; // unused but necessary argument
	instruction_t i;

	CPU cpu;
	MMU mmu;
	mmu.init();
	cpu.mmu = &mmu;

	cpu.sp = 0xC800;

	SUBCASE("BC")
	{
		// PUSH BC
		cpu.bc = 0x1234;
		i.code = 0xC5;
		cpu.executeRegular(i, cycles);
		CHECK(cpu.sp == (0xC800 - 2));
		CHECK(cpu.mmu->read16(cpu.sp) == 0x1234);

		// POP BC
		cpu.bc = 0;
		i.code = 0xC1;
		cpu.executeRegular(i, cycles);
		CHECK(cpu.sp == 0xC800);
		CHECK(cpu.bc == 0x1234);
	}

	SUBCASE("DE")
	{
		// PUSH DE
		cpu.de = 0x1234;
		i.code = 0xD5;
		cpu.executeRegular(i, cycles);
		CHECK(cpu.sp == (0xC800 - 2));
		CHECK(cpu.mmu->read16(cpu.sp) == 0x1234);

		// POP DE
		cpu.de = 0;
		i.code = 0xD1;
		cpu.executeRegular(i, cycles);
		CHECK(cpu.sp == 0xC800);
		CHECK(cpu.de == 0x1234);
	}

	SUBCASE("HL")
	{
		// PUSH HL
		cpu.hl = 0x1234;
		i.code = 0xE5;
		cpu.executeRegular(i, cycles);
		CHECK(cpu.sp == (0xC800 - 2));
		CHECK(cpu.mmu->read16(cpu.sp) == 0x1234);

		// POP HL
		cpu.hl = 0;
		i.code = 0xE1;
		cpu.executeRegular(i, cycles);
		CHECK(cpu.sp == 0xC800);
		CHECK(cpu.hl == 0x1234);
	}

	SUBCASE("AF")
	{
		SetHi(cpu.af, 0x75);
		SetLo(cpu.af, (FLAGS_ZERO | FLAGS_HALFCARRY));
		uint16_t old_af = cpu.af;

		// PUSH HL
		i.code = 0xF5;
		cpu.executeRegular(i, cycles);
		CHECK(cpu.sp == (0xC800 - 2));
		CHECK(cpu.mmu->read16(cpu.sp) == old_af);

		// POP HL
		cpu.af = 0;
		i.code = 0xF1;
		cpu.executeRegular(i, cycles);
		CHECK(cpu.sp == 0xC800);
		CHECK(cpu.af == old_af);
	}
}
