#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include "types.h"

constexpr int kCpuFreq = 4194304;
constexpr int kMaxCyclesPerFrame = kCpuFreq / 60;

class GPU;
struct MMU;

struct registers_t
{
	uint16_t pc;
	uint16_t sp;
	uint16_t af;
	uint16_t bc;
	uint16_t de;
	uint16_t hl;
};

class CPU
{
public:
	instruction_t fetch();
	void executeRegular(instruction_t &i, int &cycles);
	void executeCB(instruction_t &i, int &cycles);
	int executeInstruction();
	void doBreak();
	bool didBreak() const;
	void printRegisters(uint8_t opcode, bool newline, bool saved);
	void printInstruction(const instruction_t &i, bool cb);

	int numInstructionsExecuted() const;
	void enableInterrupts();

	void halt();

	void reset(bool haveBios);

public:
	GPU *gpu { nullptr };
	MMU *mmu { nullptr };

	uint16_t pc { 0 };
	uint16_t sp { 0 };
	uint16_t af { 0 };
	uint16_t bc { 0 };
	uint16_t de { 0 };
	uint16_t hl { 0 };

	bool ime { false };

private:
	int interrupts(uint8_t mask);

private:
	registers_t mRegisters;
	bool mBreak { false };
	int mInstructionCounter { 0 };
	int mDelayedImeEnable { 0 };
	bool mHalt { false };
	int mRepeatNextInstruction { 0 };
	bool mInBios { false };
};

#endif // CPU_H
