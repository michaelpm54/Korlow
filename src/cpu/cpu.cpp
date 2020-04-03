#include <cstdio>

#include "cpu/cpu.h"
#include "cpu/cpu_base.h"
#include "cpu/cpu_instructions.h"
#include "cpu/inst_data.h"
#include "cpu/decode.h"

#include "gpu.h"
#include "memory_map.h"
#include "mmu.h"

//#define DEBUG

CPU::CPU(MMU *mmu)
	: mmu(mmu), c{.mmu=*mmu}
{}

void CPU::halt()
{
	if (c.ime)
	{
		c.halt = true;
	}
	else
	{
		mRepeatNextInstruction = 1;
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
		printf("AF:%04X %d%d%d%d IME=%c", mRegisters.af, mRegisters.af & 0x80, mRegisters.af & 0x40, mRegisters.af & 0x20, mRegisters.af & 0x10, ime ? '1' : '0');
		printf("BC:%04X ", mRegisters.bc);
		printf("DE:%04X ", mRegisters.de);
		printf("HL:%04X%c", mRegisters.hl, n);
	}
	else
	{
		printf("%04X: ", mRegisters.pc);
		printf("[%04X] ", sp);
		printf("[%02X] ", opcode);
		printf("F:%c%c%c%c  IE=%02X IF=%02X IME=%c  ", af & 0x80 ? 'Z':'-', af & 0x40 ? 'N':'-', af & 0x20 ? 'H':'-', af & 0x10 ? 'C':'-', mmu->mem[kIe], mmu->mem[kIf], ime ? '1' : '0');
		printf("BC:%04X ", bc);
		printf("DE:%04X ", de);
		printf("HL:%04X%c", hl, n);
	}
}

void CPU::printInstruction(const instruction_t &i, bool cb)
{
	const Instruction *instructions = kInstructions;
	const int *operandSizes = kInstFmtSizes;
	auto formatStrings = kInstFmts;
	if (cb)
	{
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

void CPU::tick(int &cycles)
{
	uint16_t op { mmu->read8(c.r.pc) };

	uint8_t mask = 0;
	if (c.ime)
	{
		int c = 0;
		while (mask = mmu->mem[kIe] & mmu->mem[kIf])
		{
			c += interrupts(mask);
		}
		if (c)
		{
			cycles += c;
			return;
		}
	}

	c.d8 = mmu->read8(c.r.pc+1);
	c.d16 = mmu->read16(c.r.pc+1);

	if (op == 0xCB)
		op = c.d8 + 0x100;

	/* Print info */
	printf("%04X: (%04X)  IME(%c)  AF(%04X)  BC(%04X)  DE(%04X)  HL(%04X)  |%02X|  ", c.r.pc, c.r.sp, c.ime ? '1' : '.', c.r.af, c.r.bc, c.r.de, c.r.hl, op % 0xFF);

	const int fsize { kInstFmtSizes[op] };

	if (fsize == 0)
	{
		printf(kInstFmts[op]);
	}
	else if (fsize == 8)
	{
		printf(kInstFmts[op], c.d8);
	}
	else if (fsize == 16)
	{
		printf(kInstFmts[op], c.d16);
	}

	puts("");

	/* Execute */
	c.r.pc += kInstSizes[op];

	if (op > 0xFF)
		c.r.pc++;

	kInstructions[op](c);

	if (c.extraCycles)
	{
		c.extraCycles = false;
		cycles += kInstCyclesAlt[op];
	}
	else
		cycles += kInstCycles[op];

	if (mRepeatNextInstruction == 1)
	{
		mRepeatNextInstruction++;
	}
	else if (mRepeatNextInstruction == 2)
	{
		mRepeatNextInstruction = 0;
		pc--;
	}

	mInstructionCounter++;

	if (mDelayedImeEnable)
	{
		if (mDelayedImeEnable == 1)
		{
			mDelayedImeEnable++;
		}
		else if (mDelayedImeEnable == 2)
		{
			mDelayedImeEnable = 0;
			c.ime = true;
		}
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
	if (c.ime)
	{
		while (mask = mmu->mem[kIe] & mmu->mem[kIf])
		{
			cycles += interrupts(mask);
		}
	}

	if (cycles)
		return cycles;

	

	

	return cycles;
}

instruction_t CPU::fetch()
{
	if (mInBios && pc == 0x100)
	{
		puts("Leaving bios");
		mInBios = false;
		mmu->setInBios(false);
	}

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
	c.ime = false;
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
		c.halt = false;
		mmu->mem[kIf] = If;
	}

	return cycles;
}

void CPU::enableInterrupts()
{
	mDelayedImeEnable = 1;
}

void CPU::reset(bool haveBios)
{
	mInBios = haveBios;

	mmu->reset();

	if (haveBios)
	{
		pc = 0x0;
		af = 0x0;
		bc = 0x0;
		de = 0x0;
		hl = 0x0;
		sp = 0x0;
	}
	else
	{
		c.r.pc = 0x100;
		c.r.af = 0x1B0;
		c.r.bc = 0x13;
		c.r.de = 0xD8;
		c.r.hl = 0x14D;
		c.r.sp = 0xFFFE;
		pc = 0x0100;
		af = 0x01B0;
		bc = 0x0013;
		de = 0x00D8;
		hl = 0x014D;
		sp = 0xFFFE;
		mmu->write8(0xFF00, 0xCF);
		mmu->mem[0xFF0F] = 0xE1;
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
}

bool CPU::paused() const
{
	return c.paused;
}
