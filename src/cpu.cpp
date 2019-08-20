#include <cstdio>
#include "cpu.h"
#include "cpu_instructions.h"
#include "mmu.h"

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
	// while (cycles < kMaxCyclesPerFrame && !mBreak)
	{
		cycles += executeInstruction();
	}
}

int CPU::executeInstruction()
{
	static int counter = 0;

	int cycles = 0;
	instruction_t i = fetch();

	if (i.code == 0xCB)
	{
		pc += 1;

		if (mInBios && pc > 0xFF)
		{
			mmu->switchToRom();
			mInBios = false;
		}

		i = fetch();

		printf("%04X: ", pc-1);
		printf("[%04X] ", sp);
		printf("[%02X] ", i.code);
		printf("AF:%04X ", af);
		printf("BC:%04X ", bc);
		printf("DE:%04X ", de);
		printf("HL:%04X ", hl);
		printf("| ");
		if (ExtFormatStringOperandSize[i.code] == 0)
		{
			printf(ExtFormatStrings[i.code]);
		}
		else if (ExtFormatStringOperandSize[i.code] == 8)
		{
			printf(ExtFormatStrings[i.code], i.op8);
		}
		else
		{
			printf(ExtFormatStrings[i.code], i.op16);
		}
		puts(" | CB");

		pc += ExtInstructionSizes[i.code];
		(*ExtInstructions[i.code])(this, i);
		cycles += ExtInstructionCycles[i.code];
	}
	else
	{
		printf("%04X: ", pc);
		printf("[%04X] ", sp);
		printf("[%02X] ", i.code);
		printf("AF:%04X ", af);
		printf("BC:%04X ", bc);
		printf("DE:%04X ", de);
		printf("HL:%04X ", hl);

		printf("| ");
		if (FormatStringOperandSize[i.code] == 0)
		{
			printf(FormatStrings[i.code]);
		}
		else if (FormatStringOperandSize[i.code] == 8)
		{
			printf(FormatStrings[i.code], i.op8);
		}
		else
		{
			printf(FormatStrings[i.code], i.op16);
		}
		puts("");

		pc += InstructionSizes[i.code];
		(*InstructionList[i.code])(this, i);
		cycles += i.didAction ? InstructionCycles[i.code] : InstructionCyclesAlt[i.code];

		if (mInBios && pc > 0xFF)
		{
			mmu->switchToRom();
			mInBios = false;
		}
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
