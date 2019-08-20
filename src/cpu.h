#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include "types.h"

class MMU;

class CPU
{
public:
	void setMmu(MMU *mmu);
	void frame();
	void setPc(uint16_t addr);
	instruction_t fetch();
	int executeInstruction();
	void doBreak();
	bool didBreak() const;

public:
	MMU *mmu { nullptr };
	uint16_t pc { 0 };
	uint16_t sp { 0 };
	uint16_t af { 0 };
	uint16_t bc { 0 };
	uint16_t de { 0 };
	uint16_t hl { 0 };

private:
	bool mBreak { false };
	bool mInBios { true };
};

#endif // CPU_H
