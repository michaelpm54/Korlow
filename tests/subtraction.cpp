#include "doctest.h"
#include "cpu/cpu.h"
#include "cpu/cpu_base.h"
#include "gpu.h"
#include "mmu.h"
#include "types.h"

TEST_CASE("Subtraction yields correct results and flags")
{
	uint8_t flags = 0;
	uint8_t result = 0;

	SUBCASE("Zero is set")
	{
		flags = 0;
		SUB8(1, 1, &result, &flags);
		CHECK(flags == 0b1100'0000);
	}
	SUBCASE("Zero is unset")
	{
		flags = 0x80;
		SUB8(2, 1, &result, &flags);
		CHECK(flags == 0b0100'0000);
	}
	SUBCASE("Carry and half-carry are set on underflow")
	{
		flags = 0;
		SUB8(1, 2, &result, &flags);
		CHECK(flags == 0b0111'0000);
	}
}

TEST_CASE("SBC")
{
	int cycles = 0; // unused but necessary argument
	instruction_t i;

	CPU cpu;
	MMU mmu;
	GPU gpu;
	mmu.init(&gpu);
	cpu.mmu = &mmu;

	SUBCASE("SBC A, d8")
	{
		i.code = 0xDE;

		i.op8 = 0;
		cpu.af = 0x0000;
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0);
		CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_SUBTRACT));

		i.op8 = 1;
		cpu.af = 0x0000;
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0xFF);
		CHECK(Lo(cpu.af) == (FLAGS_SUBTRACT | FLAGS_HALFCARRY | FLAGS_CARRY));

		i.op8 = 1;
		cpu.af = 0x1000;
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0x0F);
		CHECK(Lo(cpu.af) == (FLAGS_SUBTRACT | FLAGS_HALFCARRY));

		i.op8 = 0;
		cpu.af = 0x0010;
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0xFF);
		CHECK(Lo(cpu.af) == (FLAGS_SUBTRACT | FLAGS_HALFCARRY | FLAGS_CARRY));

		i.op8 = 1;
		cpu.af = 0x0010;
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0xFE);
		CHECK(Lo(cpu.af) == (FLAGS_SUBTRACT | FLAGS_HALFCARRY | FLAGS_CARRY));

		i.op8 = 1;
		cpu.af = 0x0110;
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0xFF);
		CHECK(Lo(cpu.af) == (FLAGS_SUBTRACT | FLAGS_HALFCARRY | FLAGS_CARRY));
	}
}
