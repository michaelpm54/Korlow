#include <cstdio>
#include "cpu.h"
#include "cpu_base.h"
#include "cpu_instructions.h"
#include "inst_data.h"
#include "gpu.h"
#include "memory_map.h"
#include "mmu.h"

// #define DEBUG

static constexpr int kCpuFreq = 4194304;
static constexpr int kMaxCyclesPerFrame = kCpuFreq / 60;

void CPU::frame()
{
	if (mBreak)
	{
		return;
	}

	int cycles = 0;
	while (cycles < kMaxCyclesPerFrame && !mBreak)
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
				mDelayedImeEnable++;
			}
		}
		gpu->tick(cycles);
		interrupts();
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
		printf("AF:%04X ", af);
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

	if (mmu->inBios && pc > 0xFF)
	{
		mmu->inBios = false;
	}
}

void CPU::executeCB(instruction_t &i, int &cycles)
{
	pc += 1;

	i = fetch();

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
	instruction_t i = fetch();

	if (i.code == 0xCB)
	{
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

void CPU::interrupts()
{
	if (!ime)
	{
		return;
	}

	uint8_t If = mmu->mem[kIf] & mmu->mem[kIe];

	if (If & 1)
	{
		RST(this, 0x40);
		If &= ~(1UL << 1);
	}
	if (If & 2)
	{
		RST(this, 0x48);
		If &= ~(1UL << 2);
	}
	if (If & 4)
	{
		RST(this, 0x50);
		If &= ~(1UL << 4);
	}
	if (If & 8)
	{
		RST(this, 0x58);
		If &= ~(1UL << 8);
	}
	if (If & 16)
	{
		RST(this, 0x60);
		If &= ~(1UL << 16);
	}

	mmu->mem[kIf] = If;
}

void CPU::delayImeEnable()
{
	mDelayedImeEnable = 1;
}
