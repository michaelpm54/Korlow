#ifndef TYPES_H
#define TYPES_H

#include <cstdint>

struct Registers
{
	uint16_t pc;
	uint16_t sp;

	union
	{
		uint16_t af;
		struct {
			uint8_t f;
			uint8_t a;
		};
	};

	union
	{
		uint16_t bc;
		struct {
			uint8_t c;
			uint8_t b;
		};
	};

	union
	{
		uint16_t de;
		struct {
			uint8_t e;
			uint8_t d;
		};
	};

	union
	{
		uint16_t hl;
		struct {
			uint8_t l;
			uint8_t h;
		};
	};
};

class MMU;

struct Core
{
	MMU &mmu;
	uint8_t d8;
	uint16_t d16;
	Registers r;
	bool extraCycles { false };
	bool ime { false };
	bool halt { false };
	bool paused { false };
};

struct instruction_t
{
	uint8_t code;
	uint8_t op8;
	uint16_t op16;
	bool didAction;
};
class CPU;
using Instruction = void (*)(Core &c);
struct Instruction_t
{
	Instruction func;
	int cycles;
};

#endif // TYPES_H
