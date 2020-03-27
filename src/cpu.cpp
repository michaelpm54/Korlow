#include <cstdio>
#include "cpu.h"
#include "cpu_base.h"
#include "cpu_instructions.h"
#include "inst_data.h"
#include "gpu.h"
#include "memory_map.h"
#include "mmu.h"

//#define DEBUG

static constexpr int kCpuFreq = 4194304;
static constexpr int kMaxCyclesPerFrame = kCpuFreq / 60;

void CPU::halt()
{
	mHalt = true;
}

void CPU::frame()
{
	if (mBreak)
	{
		return;
	}

	int cycles = 0;
	while (cycles < kMaxCyclesPerFrame && !mBreak && !mHalt)
	{
		if (!mHalt)
		{
			cycles += executeInstruction();

			if (mDelayedImeEnable)
			{
				if (mDelayedImeEnable == 2)
				{
					mDelayedImeEnable = 0;
					ime = true;
				}
				else
				{
					mDelayedImeEnable = 2;
				}
			}
		}

		gpu->tick(cycles);
	}
}

void CPU::printRegisters(uint8_t opcode, bool newline, bool saved)
{
	char n = ' ';
	if (newline)
	{
		n = '\n';
	}
	if (saved)
	{
		printf("%04X: ", mRegisters.pc);
		printf("[%04X] ", mRegisters.sp);
		printf("[%02X] ", opcode);
		printf("AF:%04X %d%d%d%d ", mRegisters.af, mRegisters.af & 0x80, mRegisters.af & 0x40, mRegisters.af & 0x20, mRegisters.af & 0x10);
		printf("BC:%04X ", mRegisters.bc);
		printf("DE:%04X ", mRegisters.de);
		printf("HL:%04X%c", mRegisters.hl, n);
	}
	else
	{
		printf("%04X: ", mRegisters.pc);
		printf("[%04X] ", sp);
		printf("[%02X] ", opcode);
		printf("A:%02X ", Hi(af));
		printf("F:%c%c%c%c ", af & 0x80 ? 'Z':'-', af & 0x40 ? 'N':'-', af & 0x20 ? 'H':'-', af & 0x10 ? 'C':'-');
		printf("BC:%04X ", bc);
		printf("DE:%04X ", de);
		printf("HL:%04X%c", hl, n);
	}
}

void CPU::printInstruction(const instruction_t &i, bool cb)
{
	const instructionFunc_t *instructions = kInstructions;
	const int *operandSizes = kInstFmtSizes;
	auto formatStrings = kInstFmts;
	if (cb)
	{
		instructions = kCbInstructions;
		operandSizes = kCbInstFmtSizes;
		formatStrings = kCbInstFmts;
	}
	printf(" | ");
	if (operandSizes[i.code] == 0)
	{
		printf(formatStrings[i.code]);
	}
	else if (operandSizes[i.code] == 8)
	{
		printf(formatStrings[i.code], i.op8);
	}
	else
	{
		printf(formatStrings[i.code], i.op16);
	}
	if (cb)
	{
		puts(" | CB");
	}
	else
	{
		puts("");
	}
}

void CPU::executeRegular(instruction_t &i, int &cycles)
{
#ifdef DEBUG
	mRegisters.pc = pc;
	printRegisters(i.code, false, false);
#else
	mRegisters =
	{
		pc,
		sp,
		af,
		bc,
		de,
		hl
	};
#endif

	pc += kInstSizes[i.code];
	(*kInstructions[i.code])(this, i);
	cycles += i.didAction ? kInstCycles[i.code] : kInstCyclesAlt[i.code];

#ifdef DEBUG
	printInstruction(i, false);
#else
	if (mBreak)
	{
		printRegisters(i.code, false, true);
		printInstruction(i, false);
	}
#endif
}

void CPU::executeCB(instruction_t &i, int &cycles)
{
#ifdef DEBUG
	mRegisters.pc = pc;
	printRegisters(i.code, false, false);
#else
	mRegisters =
	{
		static_cast<uint16_t>(pc-static_cast<uint16_t>(1)), // -1 to get the original CB pc
		sp,
		af,
		bc,
		de,
		hl
	};
#endif

	pc += kCbInstSizes[i.code];
	(*kCbInstructions[i.code])(this, i);
	cycles += kCbInstCycles[i.code];

#ifdef DEBUG
	printInstruction(i, true);
#else
	if (mBreak)
	{
		printRegisters(i.code, false, true);
		printInstruction(i, true);
	}
#endif
}

int CPU::executeInstruction()
{
	int cycles = 0;

	uint8_t mask = 0;
	if (ime)
	{
		ime = false;
		while (mask = mmu->mem[kIe] & mmu->mem[kIf])
		{
			puts("IME && (IE & IF)");
			cycles += interrupts(mask);
		}
	}

	if (cycles)
		return cycles;

	instruction_t i = fetch();


	if (i.code == 0xCB)
	{
		pc++;
		i = fetch();
		executeCB(i, cycles);
	}
	else
	{
		executeRegular(i, cycles);
	}

	mInstructionCounter++;

	return cycles;
}

instruction_t CPU::fetch()
{
	instruction_t instruction =
	{
		.code = mmu->read8(pc),
		.op8  = mmu->read8(pc+1),
		.op16 = mmu->read16(pc+1),
		.didAction = true,
	};

	return instruction;
}

void CPU::doBreak()
{
	mBreak = true;
}

bool CPU::didBreak() const
{
	return mBreak;
}

int CPU::numInstructionsExecuted() const
{
	return mInstructionCounter;
}

int CPU::interrupts(uint8_t mask)
{
	sp--;
	mmu->write8(sp, (pc & 0xFF00) >> 8);

	uint8_t If = mmu->mem[kIe] & mmu->mem[kIf];

	if (!If)
		pc = 0;

	sp--;
	mmu->write8(sp, pc & 0xFF);

	int cycles = 0;

	if (If & 0b0000'0001)
	{
		puts("INT VBLANK");
		pc = 0x40;
		If &= ~0b0000'0001;
	}
	if (If & 0b0000'0010)
	{
		puts("INT STAT");
		pc = 0x48;
		If &= ~0b0000'0010;
	}
	if (If & 0b0000'0100)
	{
		puts("INT TIMER");
		pc = 0x50;
		If &= ~0b0000'0100;
	}
	if (If & 0b0000'1000)
	{
		puts("INT SERIAL");
		pc = 0x58;
		If &= ~0b0000'1000;
	}
	if (If & 0b0001'0000)
	{
		puts("INT JOYPAD");
		pc = 0x60;
		If &= ~0b0001'0000;
	}

	if (mmu->mem[kIf] != If)
	{
		cycles += 4;
		mHalt = false;
		mmu->mem[kIf] = If;
	}

	return cycles;
}

void CPU::enableInterrupts()
{
	mDelayedImeEnable = 1;
}

void CPU::initWithoutBios()
{
	pc = 0x0100;
	af = 0x01B0;
	bc = 0x0013;
	de = 0x00D8;
	hl = 0x014D;
	sp = 0xFFFE;
	mmu->write8(0xFF00, 0xCF);
	mmu->write8(0xFF10, 0x80);
	mmu->write8(0xFF11, 0xBF);
	mmu->write8(0xFF12, 0xF3);
	mmu->write8(0xFF14, 0xBF);
	mmu->write8(0xFF16, 0x3F);
	mmu->write8(0xFF19, 0xBF);
	mmu->write8(0xFF1A, 0x7F);
	mmu->write8(0xFF1B, 0xFF);
	mmu->write8(0xFF1C, 0x9F);
	mmu->write8(0xFF1E, 0xBF);
	mmu->write8(0xFF20, 0xFF);
	mmu->write8(0xFF23, 0xBF);
	mmu->write8(0xFF24, 0x77);
	mmu->write8(0xFF25, 0xF3);
	mmu->write8(0xFF26, 0xF1);
	mmu->write8(0xFF40, 0x91);
	mmu->write8(0xFF47, 0xFC);
	mmu->write8(0xFF48, 0xFF);
	mmu->write8(0xFF49, 0xFF);
}
