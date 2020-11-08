#ifndef CPU_BASE_H
#define CPU_BASE_H

#include <cstdint>

#include "emu_types.h"

enum {
    FLAGS_ZERO = 0x80,
    FLAGS_SUBTRACT = 0x40,
    FLAGS_HALFCARRY = 0x20,
    FLAGS_CARRY = 0x10,
};

class CPU;
void RST(CPU *cpu, u16 addr);

// Z00C
// Sets LSB to carry
void RL(u8 val, u8 *result, u8 *flags);

// Z00C
// Doesn't set LSB
void RLC(u8 val, u8 *result, u8 *flags);

// Z00C
// Sets LSB to carry
void RR(u8 val, u8 *result, u8 *flags);

// Z00C
// Doesn't set LSB
void RRC(u8 val, u8 *result, u8 *flags);

// Z0HC
u8 ADD8(u8 a, u8 b, u8 &flags);
void ADD16(u16 a, u16 b, u16 *result, u8 *flags);

// Z1HC
u8 SUB8(u8 a, u8 b, u8 &flags);

// Z0H-
void INC8(u8 &r, u8 &f);

// Z1H-
void DEC8(u8 &r, u8 &f);

// Generalisations
class CPU;
void CP(u8 a, u8 r, u8 &f);
void SUB(CPU *cpu, u8 r);
void TestBit(u8 bit, u8 &f);

#endif    // CPU_BASE_H
