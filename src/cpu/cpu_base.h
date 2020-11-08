#ifndef CPU_BASE_H
#define CPU_BASE_H

#include <cstdint>

enum {
    FLAGS_ZERO = 0x80,
    FLAGS_SUBTRACT = 0x40,
    FLAGS_HALFCARRY = 0x20,
    FLAGS_CARRY = 0x10,
};

class CPU;
void RST(CPU *cpu, uint16_t addr);

// Z00C
// Sets LSB to carry
void RL(uint8_t val, uint8_t *result, uint8_t *flags);

// Z00C
// Doesn't set LSB
void RLC(uint8_t val, uint8_t *result, uint8_t *flags);

// Z00C
// Sets LSB to carry
void RR(uint8_t val, uint8_t *result, uint8_t *flags);

// Z00C
// Doesn't set LSB
void RRC(uint8_t val, uint8_t *result, uint8_t *flags);

// Z0HC
uint8_t ADD8(uint8_t a, uint8_t b, uint8_t &flags);
void ADD16(uint16_t a, uint16_t b, uint16_t *result, uint8_t *flags);

// Z1HC
uint8_t SUB8(uint8_t a, uint8_t b, uint8_t &flags);

// Z0H-
void INC8(uint8_t &r, uint8_t &f);

// Z1H-
void DEC8(uint8_t &r, uint8_t &f);

// Generalisations
class CPU;
void CP(uint8_t a, uint8_t r, uint8_t &f);
void SUB(CPU *cpu, uint8_t r);
void TestBit(uint8_t bit, uint8_t &f);

#endif    // CPU_BASE_H
