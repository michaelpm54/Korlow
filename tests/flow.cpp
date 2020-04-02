#include "doctest.h"

#include <functional>

#include "cpu/cpu.h"
#include "cpu/cpu_base.h"
#include "cpu/inst_data.h"
#include "gpu.h"
#include "mmu.h"
#include "types.h"

TEST_CASE("Flow")
{
	int cycles = 0; // unused but necessary argument
	instruction_t i;

	CPU cpu;
	MMU mmu;
	GPU gpu;
	mmu.init(&gpu);
	cpu.mmu = &mmu;

	SUBCASE("1X")
	{
		SUBCASE("JR r8")
		{
			i.code = 0x18;

			cpu.pc = 0;
			i.op8 = 0;
			cpu.executeRegular(i, cycles);
			CHECK(cpu.pc == 0 + kInstSizes[i.code]);

			cpu.pc = 0;
			i.op8 = 1;
			cpu.executeRegular(i, cycles);
			CHECK(cpu.pc == 1 + kInstSizes[i.code]);

			cpu.pc = 1;
			i.op8 = 0xFF;
			cpu.executeRegular(i, cycles);
			CHECK(cpu.pc == 0 + kInstSizes[i.code]);
		}
	}

	SUBCASE("2X")
	{
		SUBCASE("JR NZ, r8")
		{
			i.code = 0x20;

			SUBCASE("Not zero, do action")
			{
				SetLo(cpu.af, 0);

				SUBCASE("r8 = 0")
				{
					cpu.pc = 0;
					i.op8 = 0;
					cpu.executeRegular(i, cycles);
					CHECK(cpu.pc == 0 + kInstSizes[i.code]);
				}

				SUBCASE("r8 = 1")
				{
					cpu.pc = 0;
					i.op8 = 1;
					cpu.executeRegular(i, cycles);
					CHECK(cpu.pc == 1 + kInstSizes[i.code]);
				}

				SUBCASE("r8 = -1")
				{
					cpu.pc = 1;
					i.op8 = 0xFF;
					cpu.executeRegular(i, cycles);
					CHECK(cpu.pc == 0 + kInstSizes[i.code]);
				}
			}

			SUBCASE("Zero, don't do action")
			{
				SetLo(cpu.af, FLAGS_ZERO);

				int pc = cpu.pc;
				cpu.executeRegular(i, cycles);
				CHECK(cpu.pc == pc + kInstSizes[i.code]);
			}
		}

		SUBCASE("JR Z, r8")
		{
			i.code = 0x28;

			SUBCASE("Zero, do action")
			{
				SetLo(cpu.af, FLAGS_ZERO);

				SUBCASE("r8 = 0")
				{
					cpu.pc = 0;
					i.op8 = 0;
					cpu.executeRegular(i, cycles);
					CHECK(cpu.pc == 0 + kInstSizes[i.code]);
				}

				SUBCASE("r8 = 1")
				{
					cpu.pc = 0;
					i.op8 = 1;
					cpu.executeRegular(i, cycles);
					CHECK(cpu.pc == 1 + kInstSizes[i.code]);
				}

				SUBCASE("r8 = -1")
				{
					cpu.pc = 1;
					i.op8 = 0xFF;
					cpu.executeRegular(i, cycles);
					CHECK(cpu.pc == 0 + kInstSizes[i.code]);
				}
			}

			SUBCASE("Not zero, don't do action")
			{
				SetLo(cpu.af, 0);

				int pc = cpu.pc;
				cpu.executeRegular(i, cycles);
				CHECK(cpu.pc == pc + kInstSizes[i.code]);
			}
		}
	}

	SUBCASE("2X")
	{
		SUBCASE("JR NC, r8")
		{
			i.code = 0x30;

			SUBCASE("Not carry, do action")
			{
				SetLo(cpu.af, 0);

				SUBCASE("r8 = 0")
				{
					cpu.pc = 0;
					i.op8 = 0;
					cpu.executeRegular(i, cycles);
					CHECK(cpu.pc == 0 + kInstSizes[i.code]);
				}

				SUBCASE("r8 = 1")
				{
					cpu.pc = 0;
					i.op8 = 1;
					cpu.executeRegular(i, cycles);
					CHECK(cpu.pc == 1 + kInstSizes[i.code]);
				}

				SUBCASE("r8 = -1")
				{
					cpu.pc = 1;
					i.op8 = 0xFF;
					cpu.executeRegular(i, cycles);
					CHECK(cpu.pc == 0 + kInstSizes[i.code]);
				}
			}

			SUBCASE("Carry, don't do action")
			{
				SetLo(cpu.af, FLAGS_CARRY);

				int pc = cpu.pc;
				cpu.executeRegular(i, cycles);
				CHECK(cpu.pc == pc + kInstSizes[i.code]);
			}
		}

		SUBCASE("JR C, r8")
		{
			i.code = 0x38;

			SUBCASE("Carry, do action")
			{
				SetLo(cpu.af, FLAGS_CARRY);

				SUBCASE("r8 = 0")
				{
					cpu.pc = 0;
					i.op8 = 0;
					cpu.executeRegular(i, cycles);
					CHECK(cpu.pc == 0 + kInstSizes[i.code]);
				}

				SUBCASE("r8 = 1")
				{
					cpu.pc = 0;
					i.op8 = 1;
					cpu.executeRegular(i, cycles);
					CHECK(cpu.pc == 1 + kInstSizes[i.code]);
				}

				SUBCASE("r8 = -1")
				{
					cpu.pc = 1;
					i.op8 = 0xFF;
					cpu.executeRegular(i, cycles);
					CHECK(cpu.pc == 0 + kInstSizes[i.code]);
				}
			}

			SUBCASE("Not carry, don't do action")
			{
				SetLo(cpu.af, 0);

				int pc = cpu.pc;
				cpu.executeRegular(i, cycles);
				CHECK(cpu.pc == pc + kInstSizes[i.code]);
			}
		}
	}

	SUBCASE("CX")
	{
		SUBCASE("RET NZ")
		{
			i.code = 0xC0;

			// Simulate Call(0x3000)
			cpu.sp = 0xC600;
			cpu.sp -= 2;
			int old_address = 0x5000;
			int new_address = 0x3000;
			cpu.mmu->write16(cpu.sp, old_address);
			cpu.pc = new_address;

			SUBCASE("NZ, do action")
			{
				SetLo(cpu.af, 0);

				cpu.executeRegular(i, cycles);
				CHECK(cpu.pc == old_address);
			}

			SUBCASE("Z, don't do action")
			{
				SetLo(cpu.af, FLAGS_ZERO);

				cpu.executeRegular(i, cycles);
				CHECK(cpu.pc == new_address + kInstSizes[i.code]);
			}
		}

		SUBCASE("JP NZ, a16")
		{
			i.code = 0xC2;

			SUBCASE("Not zero, do action")
			{
				SetLo(cpu.af, 0);

				cpu.pc = 0;
				i.op16 = 0x500;
				cpu.executeRegular(i, cycles);
				CHECK(cpu.pc == 0x500);
			}

			SUBCASE("Zero, don't do action")
			{
				SetLo(cpu.af, FLAGS_ZERO);

				int pc = cpu.pc;
				cpu.executeRegular(i, cycles);
				CHECK(cpu.pc == pc + kInstSizes[i.code]);
			}
		}

		SUBCASE("JP a16")
		{
			i.code = 0xC3;

			cpu.pc = 0;
			i.op16 = 0x500;
			cpu.executeRegular(i, cycles);
			CHECK(cpu.pc == 0x500);
		}

		SUBCASE("CALL NZ, a16")
		{
			i.code = 0xC4;

			SUBCASE("Not zero, do action")
			{
				SetLo(cpu.af, 0);

				int pc_before_call = 0x1214;
				int pc_after_ret = pc_before_call + kInstSizes[0xC4];
				int new_pc = 0x500;

				int sp_before_call = 0xC406;
				int sp_after_call = sp_before_call - 2;

				cpu.pc = pc_before_call;
				i.op16 = new_pc;
				cpu.sp = sp_before_call;
				cpu.executeRegular(i, cycles);
				CHECK(cpu.pc == new_pc);
				CHECK(cpu.sp == sp_after_call);
				CHECK(cpu.mmu->read16(cpu.sp) == pc_after_ret);
			}

			SUBCASE("Zero, don't do action")
			{
				SetLo(cpu.af, FLAGS_ZERO);

				int pc = cpu.pc;
				cpu.executeRegular(i, cycles);
				CHECK(cpu.pc == pc + kInstSizes[i.code]);
			}
		}

		SUBCASE("RST 00")
		{
			i.code = 0xC7;
			cpu.ime = true;
			cpu.pc = 0x1234;
			cpu.executeRegular(i, cycles);
			CHECK(cpu.pc == 0x00);
			CHECK(cpu.ime == false);
		}

		SUBCASE("RET Z")
		{
			const int pc_before_call = cpu.pc = 0x1234;
			const int sp_before_call = cpu.sp = 0xC400;

			const int pc_after_call = 0x3000;
			const int sp_after_call = sp_before_call - 2;

			const int pc_after_ret = pc_before_call + 3;

			// Call 0x3000
			i = { 0xCD, 0x0, pc_after_call, true };
			cpu.executeRegular(i, cycles);

			i.code = 0xC8;

			SUBCASE("Zero, do action")
			{
				SetLo(cpu.af, FLAGS_ZERO);

				cpu.executeRegular(i, cycles);

				CHECK(cpu.pc == pc_after_ret);
				CHECK(cpu.sp == sp_before_call);
			}

			SUBCASE("Not zero, don't do action")
			{
				SetLo(cpu.af, 0);

				cpu.executeRegular(i, cycles);

				CHECK(cpu.pc == pc_after_call + kInstSizes[i.code]);
				CHECK(cpu.sp == sp_after_call);
			}
		}

		SUBCASE("RET")
		{
			const int pc_before_call = cpu.pc = 0x1234;
			const int sp_before_call = cpu.sp = 0xC400;

			const int pc_after_call = 0x3000;
			const int sp_after_call = sp_before_call - 2;

			const int pc_after_ret = pc_before_call + 3;

			// Call 0x3000
			i = { 0xCD, 0x0, pc_after_call, true };
			cpu.executeRegular(i, cycles);

			i.code = 0xC9;

			cpu.executeRegular(i, cycles);

			CHECK(cpu.pc == pc_after_ret);
			CHECK(cpu.sp == sp_before_call);
		}

		SUBCASE("JP Z, a16")
		{
			i.code = 0xCA;

			SUBCASE("Zero, do action")
			{
				SetLo(cpu.af, FLAGS_ZERO);

				cpu.pc = 0;
				i.op16 = 0x500;
				cpu.executeRegular(i, cycles);
				CHECK(cpu.pc == 0x500);
			}

			SUBCASE("Not zero, don't do action")
			{
				SetLo(cpu.af, 0);

				int pc = cpu.pc;
				cpu.executeRegular(i, cycles);
				CHECK(cpu.pc == pc + kInstSizes[i.code]);
			}
		}

		SUBCASE("CALL Z, a16")
		{
			i.code = 0xCC;

			SUBCASE("Zero, do action")
			{
				SetLo(cpu.af, FLAGS_ZERO);

				int pc_before_call = 0x1224;
				int pc_after_ret = pc_before_call + kInstSizes[0xCC];
				int new_pc = 0x500;

				int sp_before_call = 0xC406;
				int sp_after_call = sp_before_call - 2;

				cpu.pc = pc_before_call;
				i.op16 = new_pc;
				cpu.sp = sp_before_call;
				cpu.executeRegular(i, cycles);
				CHECK(cpu.pc == new_pc);
				CHECK(cpu.sp == sp_after_call);
				CHECK(cpu.mmu->read16(cpu.sp) == pc_after_ret);
			}

			SUBCASE("Not zero, don't do action")
			{
				SetLo(cpu.af, 0);

				int pc = cpu.pc;
				cpu.executeRegular(i, cycles);
				CHECK(cpu.pc == pc + kInstSizes[i.code]);
			}
		}

		SUBCASE("CALL a16")
		{
			i.code = 0xCD;

			int pc_before_call = 0x1234;
			int pc_after_ret = pc_before_call + kInstSizes[0xCD];
			int new_pc = 0x500;

			int sp_before_call = 0xC406;
			int sp_after_call = sp_before_call - 2;

			cpu.pc = pc_before_call;
			i.op16 = new_pc;
			cpu.sp = sp_before_call;
			cpu.executeRegular(i, cycles);
			CHECK(cpu.pc == new_pc);
			CHECK(cpu.sp == sp_after_call);
			CHECK(cpu.mmu->read16(cpu.sp) == pc_after_ret);
		}

		SUBCASE("RST 08")
		{
			i.code = 0xCF;
			cpu.ime = true;
			cpu.pc = 0x1234;
			cpu.executeRegular(i, cycles);
			CHECK(cpu.pc == 0x08);
			CHECK(cpu.ime == false);
		}

		SUBCASE("RET NC")
		{
			const int pc_before_call = cpu.pc = 0x1234;
			const int sp_before_call = cpu.sp = 0xC400;

			const int pc_after_call = 0x3000;
			const int sp_after_call = sp_before_call - 2;

			const int pc_after_ret = pc_before_call + 3;

			// Call 0x3000
			i = { 0xCD, 0x0, pc_after_call, true };
			cpu.executeRegular(i, cycles);

			i.code = 0xD0;

			SUBCASE("Not carry, do action")
			{
				SetLo(cpu.af, 0);

				cpu.executeRegular(i, cycles);

				CHECK(cpu.pc == pc_after_ret);
				CHECK(cpu.sp == sp_before_call);
			}

			SUBCASE("Carry, don't do action")
			{
				SetLo(cpu.af, FLAGS_CARRY);

				cpu.executeRegular(i, cycles);

				CHECK(cpu.pc == pc_after_call + kInstSizes[i.code]);
				CHECK(cpu.sp == sp_after_call);
			}
		}

		SUBCASE("JP NC, a16")
		{
			i.code = 0xD2;

			SUBCASE("Not carry, do action")
			{
				SetLo(cpu.af, 0);

				cpu.pc = 0;
				i.op16 = 0x500;
				cpu.executeRegular(i, cycles);
				CHECK(cpu.pc == 0x500);
			}

			SUBCASE("Carry, don't do action")
			{
				SetLo(cpu.af, FLAGS_CARRY);

				int pc = cpu.pc;
				cpu.executeRegular(i, cycles);
				CHECK(cpu.pc == pc + kInstSizes[i.code]);
			}
		}

		SUBCASE("CALL NC, a16")
		{
			i.code = 0xD4;

			SUBCASE("Not carry, do action")
			{
				SetLo(cpu.af, 0);

				int pc_before_call = 0x1244;
				int pc_after_ret = pc_before_call + kInstSizes[0xC4];
				int new_pc = 0x500;

				int sp_before_call = 0xC406;
				int sp_after_call = sp_before_call - 2;

				cpu.pc = pc_before_call;
				i.op16 = new_pc;
				cpu.sp = sp_before_call;
				cpu.executeRegular(i, cycles);
				CHECK(cpu.pc == new_pc);
				CHECK(cpu.sp == sp_after_call);
				CHECK(cpu.mmu->read16(cpu.sp) == pc_after_ret);
			}

			SUBCASE("Carry, don't do action")
			{
				SetLo(cpu.af, FLAGS_CARRY);

				int pc = cpu.pc;
				cpu.executeRegular(i, cycles);
				CHECK(cpu.pc == pc + kInstSizes[i.code]);
			}
		}

		SUBCASE("RST 10")
		{
			i.code = 0xD7;
			cpu.ime = true;
			cpu.pc = 0x1234;
			cpu.executeRegular(i, cycles);
			CHECK(cpu.pc == 0x10);
			CHECK(cpu.ime == false);
		}

		SUBCASE("RET C")
		{
			const int pc_before_call = cpu.pc = 0x1254;
			const int sp_before_call = cpu.sp = 0xC400;

			const int pc_after_call = 0x3000;
			const int sp_after_call = sp_before_call - 2;

			const int pc_after_ret = pc_before_call + 3;

			// Call 0x3000
			i = { 0xCD, 0x0, pc_after_call, true };
			cpu.executeRegular(i, cycles);

			i.code = 0xD8;

			SUBCASE("Carry, do action")
			{
				SetLo(cpu.af, FLAGS_CARRY);

				cpu.executeRegular(i, cycles);

				CHECK(cpu.pc == pc_after_ret);
				CHECK(cpu.sp == sp_before_call);
			}

			SUBCASE("Not carry, don't do action")
			{
				SetLo(cpu.af, 0);

				cpu.executeRegular(i, cycles);

				CHECK(cpu.pc == pc_after_call + kInstSizes[i.code]);
				CHECK(cpu.sp == sp_after_call);
			}
		}

		SUBCASE("RETI")
		{
			const int pc_before_call = cpu.pc = 0x1264;
			const int sp_before_call = cpu.sp = 0xC400;

			const int pc_after_call = 0x3000;
			const int sp_after_call = sp_before_call - 2;

			const int pc_after_ret = pc_before_call + 3;

			// Call 0x3000
			i = { 0xCD, 0x0, pc_after_call, true };
			cpu.executeRegular(i, cycles);
			cpu.ime = false;

			i.code = 0xD9;

			cpu.executeRegular(i, cycles);

			CHECK(cpu.pc == pc_after_ret);
			CHECK(cpu.sp == sp_before_call);
			CHECK(cpu.ime == true);
		}

		SUBCASE("JP C, a16")
		{
			i.code = 0xDA;

			SUBCASE("Carry, do action")
			{
				SetLo(cpu.af, FLAGS_CARRY);

				cpu.pc = 0;
				i.op16 = 0x500;
				cpu.executeRegular(i, cycles);
				CHECK(cpu.pc == 0x500);
			}

			SUBCASE("Not carry, don't do action")
			{
				SetLo(cpu.af, 0);

				int pc = cpu.pc;
				cpu.executeRegular(i, cycles);
				CHECK(cpu.pc == pc + kInstSizes[i.code]);
			}
		}

		SUBCASE("CALL C, a16")
		{
			i.code = 0xDC;

			SUBCASE("Carry, do action")
			{
				SetLo(cpu.af, FLAGS_CARRY);

				int pc_before_call = 0x1244;
				int pc_after_ret = pc_before_call + kInstSizes[0xC4];
				int new_pc = 0x500;

				int sp_before_call = 0xC406;
				int sp_after_call = sp_before_call - 2;

				cpu.pc = pc_before_call;
				i.op16 = new_pc;
				cpu.sp = sp_before_call;
				cpu.executeRegular(i, cycles);
				CHECK(cpu.pc == new_pc);
				CHECK(cpu.sp == sp_after_call);
				CHECK(cpu.mmu->read16(cpu.sp) == pc_after_ret);
			}

			SUBCASE("Not carry, don't do action")
			{
				SetLo(cpu.af, 0);

				int pc = cpu.pc;
				cpu.executeRegular(i, cycles);
				CHECK(cpu.pc == pc + kInstSizes[i.code]);
			}
		}

		SUBCASE("RST 18")
		{
			i.code = 0xDF;
			cpu.ime = true;
			cpu.pc = 0x1234;
			cpu.executeRegular(i, cycles);
			CHECK(cpu.pc == 0x18);
			CHECK(cpu.ime == false);
		}

		SUBCASE("RST 20")
		{
			i.code = 0xE7;
			cpu.ime = true;
			cpu.pc = 0x1234;
			cpu.executeRegular(i, cycles);
			CHECK(cpu.pc == 0x20);
			CHECK(cpu.ime == false);
		}

		SUBCASE("JP a16")
		{
			i.code = 0xE9;

			cpu.pc = 0;
			i.op16 = 0x500;
			cpu.hl = 0x1274;
			cpu.executeRegular(i, cycles);
			CHECK(cpu.pc == cpu.hl);
		}

		SUBCASE("RST 28")
		{
			i.code = 0xEF;
			cpu.ime = true;
			cpu.pc = 0x1234;
			cpu.executeRegular(i, cycles);
			CHECK(cpu.pc == 0x28);
			CHECK(cpu.ime == false);
		}

		SUBCASE("RST 30")
		{
			i.code = 0xF7;
			cpu.ime = true;
			cpu.pc = 0x1234;
			cpu.executeRegular(i, cycles);
			CHECK(cpu.pc == 0x30);
			CHECK(cpu.ime == false);
		}

		SUBCASE("RST 38")
		{
			i.code = 0xFF;
			cpu.ime = true;
			cpu.pc = 0x1234;
			cpu.executeRegular(i, cycles);
			CHECK(cpu.pc == 0x38);
			CHECK(cpu.ime == false);
		}
	}
}
