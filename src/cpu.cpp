#include <cstdio>
#include "cpu.h"
#include "cpu_instructions.h"
#include "mmu.h"

#define DEBUG

static constexpr int kCpuFreq = 4194304;
static constexpr int kMaxCyclesPerFrame = kCpuFreq / 60;

void CPU::setMmu(MMU *mmu)
{
	this->mmu = mmu;
}

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
		printf("AF:%04X ", mRegisters.af);
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
	instructionFunc_t *instructions = InstructionList;
	int *operandSizes = FormatStringOperandSize;
	const char **formatStrings = FormatStrings;
	if (cb)
	{
		instructions = ExtInstructions;
		operandSizes = ExtFormatStringOperandSize;
		formatStrings = ExtFormatStrings;
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

	pc += InstructionSizes[i.code];
	(*InstructionList[i.code])(this, i);
	cycles += i.didAction ? InstructionCycles[i.code] : InstructionCyclesAlt[i.code];

#ifdef DEBUG
	printInstruction(i, false);
#else
	if (mBreak)
	{
		printRegisters(i.code, false, true);
		printInstruction(i, false);
	}
#endif

	if (mInBios && pc > 0xFF)
	{
		mmu->switchToRom();
		mInBios = false;
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
		pc-1, // -1 to get the original CB pc
		sp,
		af,
		bc,
		de,
		hl
	};
#endif

	pc += ExtInstructionSizes[i.code];
	(*ExtInstructions[i.code])(this, i);
	cycles += ExtInstructionCycles[i.code];

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
	static int counter = 0;

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

	counter++;

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
