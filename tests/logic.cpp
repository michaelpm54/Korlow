#include "doctest.h"
#include "cpu.h"
#include "mmu.h"
#include "cpu_base.h"
#include "types.h"

TEST_CASE("Logical instructions")
{
	int cycles = 0; // unused but necessary argument
	instruction_t i;

	CPU cpu;
	MMU mmu;
	mmu.init();
	cpu.mmu = &mmu;

	cpu.sp = 0xC800;

	SUBCASE("CPL")
	{
		// complement A and set flags -11-
		SetHi(cpu.af, 0x40);

		// set to confirm that it is unaffected
		SetLo(cpu.af, FLAGS_ZERO);

		i.code = 0x2F;
		cpu.executeRegular(i, cycles);

		CHECK(Hi(cpu.af) == uint8_t(~0x40));
		CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_SUBTRACT | FLAGS_HALFCARRY));
	}

	SUBCASE("SCF")
	{
		// set carry flag -001
		SetLo(cpu.af, (FLAGS_ZERO | FLAGS_SUBTRACT));
		i.code = 0x37;

		cpu.executeRegular(i, cycles);

		CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
	}

	SUBCASE("CCF")
	{
		// complement carry flag -00C
		SetLo(cpu.af, FLAGS_CARRY | FLAGS_SUBTRACT);
		i.code = 0x3F;
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.af) == 0);
		SetLo(cpu.af, 0);
		cpu.executeRegular(i, cycles);
		CHECK(Lo(cpu.af) == FLAGS_CARRY);
	}
}
