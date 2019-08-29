#ifndef CPU_BASE_H
#define CPU_BASE_H

#include <cstdint>

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
void ADD8(uint8_t a, uint8_t b, uint8_t *result, uint8_t *flags);
void ADD16(uint16_t a, uint16_t b, uint16_t *result, uint8_t *flags);

// Z1HC
void SUB8(uint8_t a, uint8_t b, uint8_t *result, uint8_t *flags);

// Z0H-
void INC8(uint8_t val, uint8_t *result, uint8_t *flags);

// Generalisations
class CPU;
uint8_t Hi(uint16_t n);
uint8_t Lo(uint16_t n);
void SetHi(uint16_t &r, uint8_t n);
void SetLo(uint16_t &r, uint8_t n);
void INC8_HI(CPU *cpu, uint16_t &reg);
void INC8_LO(CPU *cpu, uint16_t &reg);
void DEC8_HI(CPU *cpu, uint16_t &reg);
void DEC8_LO(CPU *cpu, uint16_t &reg);
void CP(CPU *cpu, uint8_t r);
void SUB(CPU *cpu, uint8_t r);

#endif // CPU_BASE_H
