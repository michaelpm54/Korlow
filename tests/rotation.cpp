#include "doctest.h"

#include "cpu/cpu.h"
#include "cpu/cpu_base.h"
#include "gpu.h"
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
	int cycles = 0; // unused but necessary argument
	instruction_t i;
	CPU cpu;
	MMU mmu;
	GPU gpu;
	mmu.init(&gpu);
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

	SUBCASE("CB")
	{
		SUBCASE("RLC")
		{
			SUBCASE("RLC B")
			{
				i.code = 0x0;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetHi(cpu.bc, 0);

				SUBCASE("0 << 1")
				{
					SetHi(cpu.bc, 0);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.bc) == 0);
					CHECK(Lo(cpu.af) == FLAGS_ZERO);
				}

				SUBCASE("1 << 1")
				{
					SetHi(cpu.bc, 1);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.bc) == 2);
					CHECK(Lo(cpu.af) == 0);
				}

				SUBCASE("0x80 << 1")
				{
					SetHi(cpu.bc, 0x80);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.bc) == 1);
					CHECK(Lo(cpu.af) == FLAGS_CARRY);
				}
			}

			SUBCASE("RLC C")
			{
				i.code = 0x1;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetLo(cpu.bc, 0);

				SUBCASE("0 << 1")
				{
					SetLo(cpu.bc, 0);
					cpu.executeCB(i, cycles);
					CHECK(Lo(cpu.bc) == 0);
					CHECK(Lo(cpu.af) == FLAGS_ZERO);
				}

				SUBCASE("1 << 1")
				{
					SetLo(cpu.bc, 1);
					cpu.executeCB(i, cycles);
					CHECK(Lo(cpu.bc) == 2);
					CHECK(Lo(cpu.af) == 0);
				}

				SUBCASE("0x80 << 1")
				{
					SetLo(cpu.bc, 0x80);
					cpu.executeCB(i, cycles);
					CHECK(Lo(cpu.bc) == 1);
					CHECK(Lo(cpu.af) == FLAGS_CARRY);
				}
			}

			SUBCASE("RLC D")
			{
				i.code = 0x2;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetHi(cpu.de, 0);

				SUBCASE("0 << 1")
				{
					SetHi(cpu.de, 0);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.de) == 0);
					CHECK(Lo(cpu.af) == FLAGS_ZERO);
				}

				SUBCASE("1 << 1")
				{
					SetHi(cpu.de, 1);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.de) == 2);
					CHECK(Lo(cpu.af) == 0);
				}

				SUBCASE("0x80 << 1")
				{
					SetHi(cpu.de, 0x80);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.de) == 1);
					CHECK(Lo(cpu.af) == FLAGS_CARRY);
				}
			}

			SUBCASE("RLC E")
			{
				i.code = 0x3;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetLo(cpu.de, 0);

				SUBCASE("0 << 1")
				{
					SetLo(cpu.de, 0);
					cpu.executeCB(i, cycles);
					CHECK(Lo(cpu.de) == 0);
					CHECK(Lo(cpu.af) == FLAGS_ZERO);
				}

				SUBCASE("1 << 1")
				{
					SetLo(cpu.de, 1);
					cpu.executeCB(i, cycles);
					CHECK(Lo(cpu.de) == 2);
					CHECK(Lo(cpu.af) == 0);
				}

				SUBCASE("0x80 << 1")
				{
					SetLo(cpu.de, 0x80);
					cpu.executeCB(i, cycles);
					CHECK(Lo(cpu.de) == 1);
					CHECK(Lo(cpu.af) == FLAGS_CARRY);
				}
			}

			SUBCASE("RLC H")
			{
				i.code = 0x4;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetHi(cpu.hl, 0);

				SUBCASE("0 << 1")
				{
					SetHi(cpu.hl, 0);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.hl) == 0);
					CHECK(Lo(cpu.af) == FLAGS_ZERO);
				}

				SUBCASE("1 << 1")
				{
					SetHi(cpu.hl, 1);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.hl) == 2);
					CHECK(Lo(cpu.af) == 0);
				}

				SUBCASE("0x80 << 1")
				{
					SetHi(cpu.hl, 0x80);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.hl) == 1);
					CHECK(Lo(cpu.af) == FLAGS_CARRY);
				}
			}

			SUBCASE("RLC L")
			{
				i.code = 0x5;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetLo(cpu.hl, 0);

				SUBCASE("0 << 1")
				{
					SetLo(cpu.hl, 0);
					cpu.executeCB(i, cycles);
					CHECK(Lo(cpu.hl) == 0);
					CHECK(Lo(cpu.af) == FLAGS_ZERO);
				}

				SUBCASE("1 << 1")
				{
					SetLo(cpu.hl, 1);
					cpu.executeCB(i, cycles);
					CHECK(Lo(cpu.hl) == 2);
					CHECK(Lo(cpu.af) == 0);
				}

				SUBCASE("0x80 << 1")
				{
					SetLo(cpu.hl, 0x80);
					cpu.executeCB(i, cycles);
					CHECK(Lo(cpu.hl) == 1);
					CHECK(Lo(cpu.af) == FLAGS_CARRY);
				}
			}

			SUBCASE("RLC (HL)")
			{
				i.code = 0x6;
				// clear flags
				SetLo(cpu.af, 0);
				// clear value
				cpu.hl = 0xC000;
				mmu.mem[cpu.hl] = 0;

				SUBCASE("0 << 1")
				{
					mmu.mem[cpu.hl] = 0;
					cpu.executeCB(i, cycles);
					CHECK(mmu.mem[cpu.hl] == 0);
					CHECK(Lo(cpu.af) == FLAGS_ZERO);
				}

				SUBCASE("1 << 1")
				{
					mmu.mem[cpu.hl] = 1;
					cpu.executeCB(i, cycles);
					CHECK(mmu.mem[cpu.hl] == 2);
					CHECK(Lo(cpu.af) == 0);
				}

				SUBCASE("0x80 << 1")
				{
					mmu.mem[cpu.hl] = 0x80;
					cpu.executeCB(i, cycles);
					CHECK(mmu.mem[cpu.hl] == 1);
					CHECK(Lo(cpu.af) == FLAGS_CARRY);
				}
			}

			SUBCASE("RLC A")
			{
				i.code = 0x7;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetHi(cpu.af, 0);

				SUBCASE("0 << 1")
				{
					SetHi(cpu.af, 0);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.af) == 0);
					CHECK(Lo(cpu.af) == FLAGS_ZERO);
				}

				SUBCASE("1 << 1")
				{
					SetHi(cpu.af, 1);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.af) == 2);
					CHECK(Lo(cpu.af) == 0);
				}

				SUBCASE("0x80 << 1")
				{
					SetHi(cpu.af, 0x80);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.af) == 1);
					CHECK(Lo(cpu.af) == FLAGS_CARRY);
				}
			}
		}

		SUBCASE("RRC")
		{
			SUBCASE("RRC B")
			{
				i.code = 0x8;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetHi(cpu.bc, 0);

				SUBCASE("0 >> 1")
				{
					SetHi(cpu.bc, 0);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.bc) == 0);
					CHECK(Lo(cpu.af) == FLAGS_ZERO);
				}

				SUBCASE("2 >> 1")
				{
					SetHi(cpu.bc, 2);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.bc) == 1);
					CHECK(Lo(cpu.af) == 0);
				}

				SUBCASE("1 >> 1")
				{
					SetHi(cpu.bc, 1);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.bc) == 0x80);
					CHECK(Lo(cpu.af) == FLAGS_CARRY);
				}
			}

			SUBCASE("RRC C")
			{
				i.code = 0x9;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetLo(cpu.bc, 0);

				SUBCASE("0 >> 1")
				{
					SetLo(cpu.bc, 0);
					cpu.executeCB(i, cycles);
					CHECK(Lo(cpu.bc) == 0);
					CHECK(Lo(cpu.af) == FLAGS_ZERO);
				}

				SUBCASE("2 >> 1")
				{
					SetLo(cpu.bc, 2);
					cpu.executeCB(i, cycles);
					CHECK(Lo(cpu.bc) == 1);
					CHECK(Lo(cpu.af) == 0);
				}

				SUBCASE("1 >> 1")
				{
					SetLo(cpu.bc, 1);
					cpu.executeCB(i, cycles);
					CHECK(Lo(cpu.bc) == 0x80);
					CHECK(Lo(cpu.af) == FLAGS_CARRY);
				}
			}

			SUBCASE("RRC D")
			{
				i.code = 0xA;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetHi(cpu.de, 0);

				SUBCASE("0 >> 1")
				{
					SetHi(cpu.de, 0);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.de) == 0);
					CHECK(Lo(cpu.af) == FLAGS_ZERO);
				}

				SUBCASE("2 >> 1")
				{
					SetHi(cpu.de, 2);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.de) == 1);
					CHECK(Lo(cpu.af) == 0);
				}

				SUBCASE("1 >> 1")
				{
					SetHi(cpu.de, 1);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.de) == 0x80);
					CHECK(Lo(cpu.af) == FLAGS_CARRY);
				}
			}

			SUBCASE("RRC E")
			{
				i.code = 0xB;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetLo(cpu.de, 0);

				SUBCASE("0 >> 1")
				{
					SetLo(cpu.de, 0);
					cpu.executeCB(i, cycles);
					CHECK(Lo(cpu.de) == 0);
					CHECK(Lo(cpu.af) == FLAGS_ZERO);
				}

				SUBCASE("2 >> 1")
				{
					SetLo(cpu.de, 2);
					cpu.executeCB(i, cycles);
					CHECK(Lo(cpu.de) == 1);
					CHECK(Lo(cpu.af) == 0);
				}

				SUBCASE("1 >> 1")
				{
					SetLo(cpu.de, 1);
					cpu.executeCB(i, cycles);
					CHECK(Lo(cpu.de) == 0x80);
					CHECK(Lo(cpu.af) == FLAGS_CARRY);
				}
			}

			SUBCASE("RRC H")
			{
				i.code = 0xC;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetHi(cpu.hl, 0);

				SUBCASE("0 >> 1")
				{
					SetHi(cpu.hl, 0);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.hl) == 0);
					CHECK(Lo(cpu.af) == FLAGS_ZERO);
				}

				SUBCASE("2 >> 1")
				{
					SetHi(cpu.hl, 2);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.hl) == 1);
					CHECK(Lo(cpu.af) == 0);
				}

				SUBCASE("1 >> 1")
				{
					SetHi(cpu.hl, 1);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.hl) == 0x80);
					CHECK(Lo(cpu.af) == FLAGS_CARRY);
				}
			}

			SUBCASE("RRC L")
			{
				i.code = 0xD;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetLo(cpu.hl, 0);

				SUBCASE("0 >> 1")
				{
					SetLo(cpu.hl, 0);
					cpu.executeCB(i, cycles);
					CHECK(Lo(cpu.hl) == 0);
					CHECK(Lo(cpu.af) == FLAGS_ZERO);
				}

				SUBCASE("2 >> 1")
				{
					SetLo(cpu.hl, 2);
					cpu.executeCB(i, cycles);
					CHECK(Lo(cpu.hl) == 1);
					CHECK(Lo(cpu.af) == 0);
				}

				SUBCASE("1 >> 1")
				{
					SetLo(cpu.hl, 1);
					cpu.executeCB(i, cycles);
					CHECK(Lo(cpu.hl) == 0x80);
					CHECK(Lo(cpu.af) == FLAGS_CARRY);
				}
			}

			SUBCASE("RRC (HL)")
			{
				i.code = 0xE;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				cpu.hl = 0xC000;
				mmu.mem[cpu.hl] = 0;

				SUBCASE("0 >> 1")
				{
					mmu.mem[cpu.hl] = 0;
					cpu.executeCB(i, cycles);
					CHECK(mmu.mem[cpu.hl] == 0);
					CHECK(Lo(cpu.af) == FLAGS_ZERO);
				}

				SUBCASE("2 >> 1")
				{
					mmu.mem[cpu.hl] = 2;
					cpu.executeCB(i, cycles);
					CHECK(mmu.mem[cpu.hl] == 1);
					CHECK(Lo(cpu.af) == 0);
				}

				SUBCASE("1 >> 1")
				{
					mmu.mem[cpu.hl] = 1;
					cpu.executeCB(i, cycles);
					CHECK(mmu.mem[cpu.hl] == 0x80);
					CHECK(Lo(cpu.af) == FLAGS_CARRY);
				}
			}

			SUBCASE("RRC A")
			{
				i.code = 0xF;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetHi(cpu.af, 0);

				SUBCASE("0 >> 1")
				{
					SetHi(cpu.af, 0);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.af) == 0);
					CHECK(Lo(cpu.af) == FLAGS_ZERO);
				}

				SUBCASE("2 >> 1")
				{
					SetHi(cpu.af, 2);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.af) == 1);
					CHECK(Lo(cpu.af) == 0);
				}

				SUBCASE("1 >> 1")
				{
					SetHi(cpu.af, 1);
					cpu.executeCB(i, cycles);
					CHECK(Hi(cpu.af) == 0x80);
					CHECK(Lo(cpu.af) == FLAGS_CARRY);
				}
			}
		}

		SUBCASE("RL")
		{
			SUBCASE("RL B")
			{
				i.code = 0x10;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetHi(cpu.bc, 0);

				SUBCASE("0 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.bc, 0);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.bc) == 1);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.bc, 0);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.bc) == 0);
						CHECK(Lo(cpu.af) == FLAGS_ZERO);
					}
				}

				SUBCASE("1 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.bc, 1);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.bc) == 3);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.bc, 1);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.bc) == 2);
						CHECK(Lo(cpu.af) == 0);
					}
				}

				SUBCASE("0x80 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.bc, 0x80);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.bc) == 1);
						CHECK(Lo(cpu.af) == FLAGS_CARRY);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.bc, 0x80);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.bc) == 0);
						CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
					}
				}
			}

			SUBCASE("RL C")
			{
				i.code = 0x11;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetLo(cpu.bc, 0);

				SUBCASE("0 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetLo(cpu.bc, 0);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.bc) == 1);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetLo(cpu.bc, 0);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.bc) == 0);
						CHECK(Lo(cpu.af) == FLAGS_ZERO);
					}
				}

				SUBCASE("1 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetLo(cpu.bc, 1);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.bc) == 3);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetLo(cpu.bc, 1);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.bc) == 2);
						CHECK(Lo(cpu.af) == 0);
					}
				}

				SUBCASE("0x80 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetLo(cpu.bc, 0x80);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.bc) == 1);
						CHECK(Lo(cpu.af) == FLAGS_CARRY);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetLo(cpu.bc, 0x80);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.bc) == 0);
						CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
					}
				}
			}

			SUBCASE("RL D")
			{
				i.code = 0x12;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetHi(cpu.de, 0);

				SUBCASE("0 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.de, 0);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.de) == 1);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.de, 0);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.de) == 0);
						CHECK(Lo(cpu.af) == FLAGS_ZERO);
					}
				}

				SUBCASE("1 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.de, 1);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.de) == 3);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.de, 1);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.de) == 2);
						CHECK(Lo(cpu.af) == 0);
					}
				}

				SUBCASE("0x80 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.de, 0x80);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.de) == 1);
						CHECK(Lo(cpu.af) == FLAGS_CARRY);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.de, 0x80);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.de) == 0);
						CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
					}
				}
			}

			SUBCASE("RL E")
			{
				i.code = 0x13;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetLo(cpu.de, 0);

				SUBCASE("0 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetLo(cpu.de, 0);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.de) == 1);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetLo(cpu.de, 0);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.de) == 0);
						CHECK(Lo(cpu.af) == FLAGS_ZERO);
					}
				}

				SUBCASE("1 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetLo(cpu.de, 1);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.de) == 3);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetLo(cpu.de, 1);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.de) == 2);
						CHECK(Lo(cpu.af) == 0);
					}
				}

				SUBCASE("0x80 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetLo(cpu.de, 0x80);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.de) == 1);
						CHECK(Lo(cpu.af) == FLAGS_CARRY);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetLo(cpu.de, 0x80);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.de) == 0);
						CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
					}
				}
			}

			SUBCASE("RL H")
			{
				i.code = 0x14;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetHi(cpu.hl, 0);

				SUBCASE("0 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.hl, 0);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.hl) == 1);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.hl, 0);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.hl) == 0);
						CHECK(Lo(cpu.af) == FLAGS_ZERO);
					}
				}

				SUBCASE("1 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.hl, 1);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.hl) == 3);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.hl, 1);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.hl) == 2);
						CHECK(Lo(cpu.af) == 0);
					}
				}

				SUBCASE("0x80 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.hl, 0x80);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.hl) == 1);
						CHECK(Lo(cpu.af) == FLAGS_CARRY);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.hl, 0x80);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.hl) == 0);
						CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
					}
				}
			}

			SUBCASE("RL L")
			{
				i.code = 0x15;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetLo(cpu.hl, 0);

				SUBCASE("0 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetLo(cpu.hl, 0);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.hl) == 1);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetLo(cpu.hl, 0);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.hl) == 0);
						CHECK(Lo(cpu.af) == FLAGS_ZERO);
					}
				}

				SUBCASE("1 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetLo(cpu.hl, 1);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.hl) == 3);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetLo(cpu.hl, 1);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.hl) == 2);
						CHECK(Lo(cpu.af) == 0);
					}
				}

				SUBCASE("0x80 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetLo(cpu.hl, 0x80);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.hl) == 1);
						CHECK(Lo(cpu.af) == FLAGS_CARRY);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetLo(cpu.hl, 0x80);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.hl) == 0);
						CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
					}
				}
			}

			SUBCASE("RL (HL)")
			{
				i.code = 0x16;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				cpu.hl = 0xC000;
				mmu.mem[cpu.hl] = 0;

				SUBCASE("0 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						mmu.mem[cpu.hl] = 0;
						cpu.executeCB(i, cycles);
						CHECK(mmu.mem[cpu.hl] == 1);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						mmu.mem[cpu.hl] = 0;
						cpu.executeCB(i, cycles);
						CHECK(mmu.mem[cpu.hl] == 0);
						CHECK(Lo(cpu.af) == FLAGS_ZERO);
					}
				}

				SUBCASE("1 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						mmu.mem[cpu.hl] = 1;
						cpu.executeCB(i, cycles);
						CHECK(mmu.mem[cpu.hl] == 3);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						mmu.mem[cpu.hl] = 1;
						cpu.executeCB(i, cycles);
						CHECK(mmu.mem[cpu.hl] == 2);
						CHECK(Lo(cpu.af) == 0);
					}
				}

				SUBCASE("0x80 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						mmu.mem[cpu.hl] = 0x80;
						cpu.executeCB(i, cycles);
						CHECK(mmu.mem[cpu.hl] == 1);
						CHECK(Lo(cpu.af) == FLAGS_CARRY);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						mmu.mem[cpu.hl] = 0x80;
						cpu.executeCB(i, cycles);
						CHECK(mmu.mem[cpu.hl] == 0);
						CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
					}
				}
			}

			SUBCASE("RL A")
			{
				i.code = 0x17;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetHi(cpu.af, 0);

				SUBCASE("0 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.af, 0);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.af) == 1);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.af, 0);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.af) == 0);
						CHECK(Lo(cpu.af) == FLAGS_ZERO);
					}
				}

				SUBCASE("1 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.af, 1);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.af) == 3);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.af, 1);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.af) == 2);
						CHECK(Lo(cpu.af) == 0);
					}
				}

				SUBCASE("0x80 << 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.af, 0x80);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.af) == 1);
						CHECK(Lo(cpu.af) == FLAGS_CARRY);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.af, 0x80);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.af) == 0);
						CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
					}
				}
			}
		}

		SUBCASE("RR")
		{
			SUBCASE("RR B")
			{
				i.code = 0x18;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetHi(cpu.bc, 0);

				SUBCASE("0 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.bc, 0);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.bc) == 0x80);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.bc, 0);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.bc) == 0);
						CHECK(Lo(cpu.af) == FLAGS_ZERO);
					}
				}

				SUBCASE("2 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.bc, 2);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.bc) == 0x81);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.bc, 2);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.bc) == 1);
						CHECK(Lo(cpu.af) == 0);
					}
				}

				SUBCASE("1 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.bc, 1);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.bc) == 0x80);
						CHECK(Lo(cpu.af) == FLAGS_CARRY);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.bc, 1);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.bc) == 0);
						CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
					}
				}
			}

			SUBCASE("RR C")
			{
				i.code = 0x19;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetLo(cpu.bc, 0);

				SUBCASE("0 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetLo(cpu.bc, 0);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.bc) == 0x80);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetLo(cpu.bc, 0);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.bc) == 0);
						CHECK(Lo(cpu.af) == FLAGS_ZERO);
					}
				}

				SUBCASE("2 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetLo(cpu.bc, 2);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.bc) == 0x81);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetLo(cpu.bc, 2);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.bc) == 1);
						CHECK(Lo(cpu.af) == 0);
					}
				}

				SUBCASE("1 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetLo(cpu.bc, 1);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.bc) == 0x80);
						CHECK(Lo(cpu.af) == FLAGS_CARRY);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetLo(cpu.bc, 1);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.bc) == 0);
						CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
					}
				}
			}

			SUBCASE("RR D")
			{
				i.code = 0x1A;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetHi(cpu.de, 0);

				SUBCASE("0 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.de, 0);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.de) == 0x80);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.de, 0);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.de) == 0);
						CHECK(Lo(cpu.af) == FLAGS_ZERO);
					}
				}

				SUBCASE("2 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.de, 2);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.de) == 0x81);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.de, 2);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.de) == 1);
						CHECK(Lo(cpu.af) == 0);
					}
				}

				SUBCASE("1 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.de, 1);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.de) == 0x80);
						CHECK(Lo(cpu.af) == FLAGS_CARRY);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.de, 1);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.de) == 0);
						CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
					}
				}
			}

			SUBCASE("RR E")
			{
				i.code = 0x1B;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetLo(cpu.de, 0);

				SUBCASE("0 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetLo(cpu.de, 0);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.de) == 0x80);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetLo(cpu.de, 0);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.de) == 0);
						CHECK(Lo(cpu.af) == FLAGS_ZERO);
					}
				}

				SUBCASE("2 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetLo(cpu.de, 2);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.de) == 0x81);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetLo(cpu.de, 2);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.de) == 1);
						CHECK(Lo(cpu.af) == 0);
					}
				}

				SUBCASE("1 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetLo(cpu.de, 1);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.de) == 0x80);
						CHECK(Lo(cpu.af) == FLAGS_CARRY);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetLo(cpu.de, 1);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.de) == 0);
						CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
					}
				}
			}

			SUBCASE("RR H")
			{
				i.code = 0x1C;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetHi(cpu.hl, 0);

				SUBCASE("0 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.hl, 0);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.hl) == 0x80);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.hl, 0);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.hl) == 0);
						CHECK(Lo(cpu.af) == FLAGS_ZERO);
					}
				}

				SUBCASE("2 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.hl, 2);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.hl) == 0x81);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.hl, 2);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.hl) == 1);
						CHECK(Lo(cpu.af) == 0);
					}
				}

				SUBCASE("1 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.hl, 1);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.hl) == 0x80);
						CHECK(Lo(cpu.af) == FLAGS_CARRY);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.hl, 1);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.hl) == 0);
						CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
					}
				}
			}

			SUBCASE("RR L")
			{
				i.code = 0x1D;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetLo(cpu.hl, 0);

				SUBCASE("0 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetLo(cpu.hl, 0);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.hl) == 0x80);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetLo(cpu.hl, 0);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.hl) == 0);
						CHECK(Lo(cpu.af) == FLAGS_ZERO);
					}
				}

				SUBCASE("2 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetLo(cpu.hl, 2);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.hl) == 0x81);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetLo(cpu.hl, 2);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.hl) == 1);
						CHECK(Lo(cpu.af) == 0);
					}
				}

				SUBCASE("1 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetLo(cpu.hl, 1);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.hl) == 0x80);
						CHECK(Lo(cpu.af) == FLAGS_CARRY);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetLo(cpu.hl, 1);
						cpu.executeCB(i, cycles);
						CHECK(Lo(cpu.hl) == 0);
						CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
					}
				}
			}

			SUBCASE("RR (HL)")
			{
				i.code = 0x1E;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				cpu.hl = 0xC000;
				mmu.mem[cpu.hl] = 0;

				SUBCASE("0 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						mmu.mem[cpu.hl] = 0;
						cpu.executeCB(i, cycles);
						CHECK(mmu.mem[cpu.hl] == 0x80);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						mmu.mem[cpu.hl] = 0;
						cpu.executeCB(i, cycles);
						CHECK(mmu.mem[cpu.hl] == 0);
						CHECK(Lo(cpu.af) == FLAGS_ZERO);
					}
				}

				SUBCASE("2 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						mmu.mem[cpu.hl] = 2;
						cpu.executeCB(i, cycles);
						CHECK(mmu.mem[cpu.hl] == 0x81);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						mmu.mem[cpu.hl] = 2;
						cpu.executeCB(i, cycles);
						CHECK(mmu.mem[cpu.hl] == 1);
						CHECK(Lo(cpu.af) == 0);
					}
				}

				SUBCASE("1 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						mmu.mem[cpu.hl] = 1;
						cpu.executeCB(i, cycles);
						CHECK(mmu.mem[cpu.hl] == 0x80);
						CHECK(Lo(cpu.af) == FLAGS_CARRY);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						mmu.mem[cpu.hl] = 1;
						cpu.executeCB(i, cycles);
						CHECK(mmu.mem[cpu.hl] == 0);
						CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
					}
				}
			}

			SUBCASE("RR A")
			{
				i.code = 0x1F;
				// clear flags
				SetLo(cpu.af, 0);
				// clear register
				SetHi(cpu.af, 0);

				SUBCASE("0 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.af, 0);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.af) == 0x80);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.af, 0);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.af) == 0);
						CHECK(Lo(cpu.af) == FLAGS_ZERO);
					}
				}

				SUBCASE("2 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.af, 2);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.af) == 0x81);
						CHECK(Lo(cpu.af) == 0);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.af, 2);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.af) == 1);
						CHECK(Lo(cpu.af) == 0);
					}
				}

				SUBCASE("1 >> 1")
				{
					SUBCASE("With carry")
					{
						SetLo(cpu.af, FLAGS_CARRY);
						SetHi(cpu.af, 1);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.af) == 0x80);
						CHECK(Lo(cpu.af) == FLAGS_CARRY);
					}

					SUBCASE("Without carry")
					{
						SetLo(cpu.af, 0);
						SetHi(cpu.af, 1);
						cpu.executeCB(i, cycles);
						CHECK(Hi(cpu.af) == 0);
						CHECK(Lo(cpu.af) == (FLAGS_ZERO | FLAGS_CARRY));
					}
				}
			}
		}
	}
}
