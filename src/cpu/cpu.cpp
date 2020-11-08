#include <cstdio>

#include "cpu/cpu.h"
#include "cpu/cpu_base.h"
#include "cpu/cpu_instructions.h"
#include "cpu/inst_data.h"

#include "memory_map.h"
#include "mmu.h"
#include "ppu.h"


Cpu::Cpu(CpuRegisters registers) : registers(registers) {}

void Cpu::reset(bool) {
	pc = 0;
	sp = 0xFFFF;
	af = 0;
	bc = 0;
	de = 0;
	hl = 0;
	ime = true;
	/*
	pc = 0x0100;
	sp = 0xFFFE;
	af = 0x01B0;
	bc = 0x0013;
	de = 0x00D8;
	hl = 0x014D;
	ime = false;
	*/
}

bool Cpu::is_enabled() const { return enabled; }

void Cpu::print_instruction(uint16_t op, uint8_t d8, uint16_t d16) {
	/* Print info */
	printf("%04X: (%04X)  IME:%c  AF:%04X  BC:%04X  DE:%04X  HL:%04X  |%02X|  ",
		pc, sp, ime ? '1' : '.', af, bc, de, hl, op % 0x100);

	const int fsize{ kInstFmtSizes[op] };

	if (fsize == 0) {
		printf(kInstFmts[op]);
	}
	else if (fsize == 8) {
		printf(kInstFmts[op], d8);
	}
	else if (fsize == 16) {
		printf(kInstFmts[op], d16);
	}

	puts("");
}

int Cpu::interrupt_handler(Component& mmu) {
	int int_cycles = 0;

	while (auto mask = registers.ie & registers.if_)
		int_cycles += interrupts(mask, mmu);

	return int_cycles;
}

void Cpu::halt_bug() {
	switch (halt_bug_state) {
	case HaltBug::Triggered:
		halt_bug_state = HaltBug::RepeatNext;
		break;
	case HaltBug::RepeatNext:
		pc--;
		halt_bug_state = HaltBug::None;
		break;
	default:
		break;
	}
}

void Cpu::ei_bug() {
	switch (ei_bug_state) {
	case EIBug::Triggered:
		ei_bug_state = EIBug::Enable;
		break;
	case EIBug::Enable:
		ime = true;
		ei_bug_state = EIBug::None;
		break;
	default:
		break;
	}
}

int Cpu::tick(Component& mmu) {
	int cycles = 0;

	uint16_t op{ mmu.read8(pc) };
	uint16_t d16{ mmu.read16(pc + 1) };
	uint8_t d8{ static_cast<uint8_t>(d16) };

	if (ime) {
		int int_cycles = interrupt_handler(mmu);
		if (int_cycles)
			return int_cycles + cycles;
	}

	if (op == 0xCB)
		op = d8 + 0x100;

	if (debug)
		print_instruction(op, d8, d16);

	pc += kInstSizes[op];

	if (op > 0xFF) // CB
		pc++;

	/* Execute */
	bool extraCycles{ false };
	kInstructions[op](*this, mmu, d8, d16, extraCycles);

	cycles += extraCycles ? kInstCyclesAlt[op] : kInstCycles[op];

	halt_bug();
	ei_bug();

	return cycles;
}

void Cpu::enable_interrupts() { ei_bug_state = EIBug::Triggered; }

void Cpu::halt() {
	if (ime)
		halted = true;
	else
		halt_bug_state = HaltBug::Triggered;
}

int Cpu::interrupts(uint8_t mask, Component& mmu) {
	ime = false;
	sp--;
	mmu.write8(sp, (pc & 0xFF00) >> 8);

	uint8_t if_ = registers.if_ & registers.ie;

	if (!if_)
		pc = 0;

	sp--;
	mmu.write8(sp, pc & 0xFF);

	int cycles = 0;

	if (if_ & 0b0000'0001) {
		pc = 0x40;
		if_ &= ~0b0000'0001;
	}
	if (if_ & 0b0000'0010) {
		pc = 0x48;
		if_ &= ~0b0000'0010;
	}
	if (if_ & 0b0000'0100) {
		pc = 0x50;
		if_ &= ~0b0000'0100;
	}
	if (if_ & 0b0000'1000) {
		pc = 0x58;
		if_ &= ~0b0000'1000;
	}
	if (if_ & 0b0001'0000) {
		pc = 0x60;
		if_ &= ~0b0001'0000;
	}

	if (registers.if_ != if_) {
		cycles += 4;
		halted = false;
		mmu.write8(kIf, if_);
	}

	return cycles;
}

void Cpu::set_enabled(bool value) { enabled = value; }

void Cpu::disable_interrupts() { ime = false; }
