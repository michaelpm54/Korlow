#ifndef TYPES_H
#define TYPES_H

struct instruction_t
{
	uint8_t code;
	uint8_t op8;
	uint16_t op16;
	bool didAction;
};

class CPU;
using instructionFunc_t = void (*)(CPU *cpu, instruction_t &instruction);

#endif // TYPES_H
