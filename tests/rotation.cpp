#include "doctest.h"

#include "cpu.h"
#include "cpu_base.h"
#include "mmu.h"
#include "types.h"

TEST_CASE("RL sets flags and result correctly")
{
	uint8_t flags = 0;
	uint8_t result = 0;

	SUBCASE("RL makes flags Z00C")
	{
		SUBCASE("Carry sets LSB")
		{
			SUBCASE("to 1")
			{
				flags = 0x10;
				RL(0, &result, &flags);
				CHECK(result == 1);
			}
			SUBCASE("to 0")
			{
				flags = 0;
				RL(0, &result, &flags);
				CHECK(result == 0);
			}
		}
		SUBCASE("MSB was 0, does not set carry")
		{
			flags = 0x10;
			RL(0, &result, &flags);
			CHECK(flags == 0);
		}
		SUBCASE("MSB was 1, sets carry and not zero")
		{
			flags = 0;
			RL(0b1100'0000, &result, &flags);
			CHECK(flags == 0x10);
		}
		SUBCASE("result was 0, sets zero")
		{
			flags = 0;
			RL(0, &result, &flags);
			CHECK(flags == 0x80);
		}
		SUBCASE("result was not 0, clears zero")
		{
			flags = 0x90;
			RL(0, &result, &flags);
			CHECK(flags == 0);
		}
		SUBCASE("carry and zero are set correctly")
		{
			flags = 0b1111'0000;
			RL(0b1100'0000, &result, &flags);
			CHECK(flags == 0x10);
		}
	}
	SUBCASE("RL sets result correctly")
	{
		SUBCASE("result is 0")
		{
			SUBCASE("because carry was 0")
			{
				SUBCASE("input is 0x80")
				{
					flags = 0;
					RL(0x80, &result, &flags);
					CHECK(result == 0);
				}
				SUBCASE("input is 0")
				{
					flags = 0;
					RL(0, &result, &flags);
					CHECK(result == 0);
				}
			}
		}
		SUBCASE("result is not 0")
		{
			SUBCASE("carry was not 0")
			{
				flags = 0x10;
				RL(0, &result, &flags);
				CHECK(result == 1);
			}
			SUBCASE("other input bits were > 0 && < 0x80")
			{
				flags = 0;
				RL(2, &result, &flags);
				CHECK(result == 4);
			}
		}
	}
}

TEST_CASE("RLC sets flags and result correctly")
{
	uint8_t flags = 0;
	uint8_t result = 0;

	SUBCASE("RLC makes flags Z00C")
	{
		SUBCASE("Carry is set")
		{
			SUBCASE("to 1")
			{
				flags = 0;
				RLC(0b1000'0001, &result, &flags);
				CHECK(flags == 0x10);
			}
			SUBCASE("to 0")
			{
				flags = 0x10;
				RLC(1, &result, &flags);
				CHECK(flags == 0);
			}
		}
		SUBCASE("Zero is set")
		{
			SUBCASE("to 1")
			{
				flags = 0;
				RLC(0, &result, &flags);
				CHECK(flags == 0x80);
			}
			SUBCASE("to 0")
			{
				flags = 0x80;
				RLC(1, &result, &flags);
				CHECK(flags == 0);
			}
		}
	}
	SUBCASE("RLC sets result correctly")
	{
		SUBCASE("result is 0 because input is 0")
		{
			RLC(0, &result, &flags);
			CHECK(result == 0);
		}
		SUBCASE("result is not 0")
		{
			SUBCASE("MSB was not 0")
			{
				RLC(0x80, &result, &flags);
				CHECK(result == 1);
			}
			SUBCASE("other input bits were > 0 && < 0x80")
			{
				RLC(0b0010'0000, &result, &flags);
				CHECK(result != 0);
			}
		}
	}
}

TEST_CASE("RR sets flags and result correctly")
{
	uint8_t flags = 0;
	uint8_t result = 0;

	SUBCASE("RR makes flags Z00C")
	{
		SUBCASE("Carry sets MSB")
		{
			SUBCASE("to 1")
			{
				flags = 0x10;
				RR(0, &result, &flags);
				CHECK(result == 0x80);
			}
			SUBCASE("to 0")
			{
				flags = 0;
				RR(0x80, &result, &flags);
				CHECK(result == 0x40);
			}
		}
		SUBCASE("LSB was 0, does not set carry or zero")
		{
			flags = 0x10;
			RR(0, &result, &flags);
			CHECK(flags == 0);
		}
		SUBCASE("LSB was 1, sets carry and not zero")
		{
			flags = 0;
			RR(0b1100'0001, &result, &flags);
			CHECK(flags == 0x10);
		}
		SUBCASE("result was 0, sets zero")
		{
			flags = 0;
			RR(0, &result, &flags);
			CHECK(flags == 0x80);
		}
		SUBCASE("result was not 0, clears zero")
		{
			flags = 0x90;
			RR(0, &result, &flags);
			CHECK(flags == 0);
		}
		SUBCASE("carry and zero are set correctly, other flags cleared")
		{
			flags = 0b1111'0000;
			RR(0b0000'0011, &result, &flags);
			CHECK(flags == 0x10);
		}
	}
	SUBCASE("RR sets result correctly")
	{
		SUBCASE("result is 0")
		{
			SUBCASE("because carry was 0")
			{
				SUBCASE("input is 1")
				{
					flags = 0;
					RR(1, &result, &flags);
					CHECK(result == 0);
				}
				SUBCASE("input is 0")
				{
					flags = 0;
					RR(0, &result, &flags);
					CHECK(result == 0);
				}
			}
		}
		SUBCASE("result is not 0")
		{
			SUBCASE("carry was not 0")
			{
				flags = 0x10;
				RR(0, &result, &flags);
				CHECK(result == 0x80);
			}
			SUBCASE("other input bits were > 0 && < 0x80")
			{
				flags = 0;
				RR(2, &result, &flags);
				CHECK(result == 1);
			}
		}
	}
}

TEST_CASE("RRC sets flags and result correctly")
{
	uint8_t flags = 0;
	uint8_t result = 0;

	SUBCASE("RRC makes flags Z00C")
	{
		SUBCASE("Carry is set")
		{
			SUBCASE("to 1")
			{
				flags = 0;
				RRC(0b0000'0001, &result, &flags);
				CHECK(flags == 0x10);
			}
			SUBCASE("to 0")
			{
				flags = 0x10;
				RRC(4, &result, &flags);
				CHECK(flags == 0);
			}
		}
		SUBCASE("Zero is set")
		{
			SUBCASE("to 1")
			{
				flags = 0;
				RRC(0, &result, &flags);
				CHECK(flags == 0x80);
			}
			SUBCASE("to 0")
			{
				flags = 0x80;
				RRC(2, &result, &flags);
				CHECK(flags == 0);
			}
		}
	}
	SUBCASE("RRC sets result correctly")
	{
		SUBCASE("result is 0")
		{
			SUBCASE("because input is 0 and carry is 0")
			{
				flags = 0;
				RRC(0, &result, &flags);
				CHECK(result == 0);
			}
		}
		SUBCASE("result is not 0")
		{
			SUBCASE("LSB was 1")
			{
				flags = 0;
				RRC(0b0000'0001, &result, &flags);
				CHECK(result == 0x80);
			}
			SUBCASE("any input bits except LSB were 1")
			{
				flags = 0;
				RRC(0b0010'0000, &result, &flags);
				CHECK(result == 0x10);
			}
		}
	}
}

TEST_CASE("Rotate instructions")
{
	uint8_t flags = 0;
	uint8_t result = 0;
	int cycles = 0; // unused but necessary argument
	instruction_t i;
	CPU cpu;
	MMU mmu;
	mmu.init();
	cpu.mmu = &mmu;

	SUBCASE("0X")
	{
		i.code = 0x7;
		SetHi(cpu.af, 0);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0);
		SetHi(cpu.af, 0x80);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0x1);
		CHECK(Lo(cpu.af) == FLAGS_CARRY);
		SetLo(cpu.af, FLAGS_CARRY);
		SetHi(cpu.af, 0x80 >> 1);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0x80);
		CHECK(Lo(cpu.af) == 0);

		i.code = 0xF;
		SetHi(cpu.af, 0);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0);
		SetHi(cpu.af, 0x0);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0x0);
		CHECK(Lo(cpu.af) == 0x0);
		SetLo(cpu.af, FLAGS_CARRY);
		SetHi(cpu.af, 0x2);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0x1);
		CHECK(Lo(cpu.af) == 0);
	}

	SUBCASE("1X")
	{
		i.code = 0x17;
		SetLo(cpu.af, 0);
		SetHi(cpu.af, 0x80);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0);
		CHECK(Lo(cpu.af) == FLAGS_CARRY);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 1);
		CHECK(Lo(cpu.af) == 0);

		i.code = 0x1F;
		SetLo(cpu.af, 0);
		SetHi(cpu.af, 1);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0);
		CHECK(Lo(cpu.af) == FLAGS_CARRY);
		cpu.executeRegular(i, cycles);
		CHECK(Hi(cpu.af) == 0x80);
		CHECK(Lo(cpu.af) == 0);
	}
}
