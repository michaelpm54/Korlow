#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include "types.h"

#include "component.h"

constexpr int kCpuFreq = 4194304;
constexpr int kMaxCyclesPerFrame = kCpuFreq / 60;

enum class HaltBug
{
	None,
	Triggered,
	RepeatNext,
};

enum class EIBug
{
	None,
	Triggered,
	Enable,
};

struct CpuRegisters
{
	uint8_t &io;
	uint8_t &if_;
	uint8_t &ie;
};

struct Cpu : Component
{
	Cpu(CpuRegisters);

	void reset() override;
	void write8(uint16_t address, uint8_t value) {}
	int tick(Component &mmu); // Returns # of cycles taken
	int interrupts(uint8_t mask, Component &mmu);
	void enable_interrupts();
	void disable_interrupts();
	void halt();
	void set_enabled(bool value);
	bool is_enabled() const;
	void print_instruction(uint16_t op, uint8_t d8, uint16_t d16);
	int interrupt_handler(Component& mmu);
	void halt_bug();
	void ei_bug();

	HaltBug halt_bug_state { HaltBug::None };
	EIBug ei_bug_state { EIBug::None };

	bool debug { true };

	CpuRegisters registers;

	uint16_t pc;
	uint16_t sp;

	union { uint16_t af; struct { uint8_t f; uint8_t a; }; };
	union { uint16_t bc; struct { uint8_t c; uint8_t b; }; };
	union { uint16_t de; struct { uint8_t e; uint8_t d; }; };
	union { uint16_t hl; struct { uint8_t l; uint8_t h; }; };

private:
	bool halted { false };
	bool enabled { true };

	bool ime;

};

#endif // CPU_H
