#ifndef CPU_INSTRUCTIONS_H
#define CPU_INSTRUCTIONS_H

#include <cstdint>
#include "types.h"

extern instructionFunc_t InstructionList[0x100];
extern int InstructionSizes[0x100];
extern int InstructionCycles[0x100];
extern int InstructionCyclesAlt[0x100];

extern instructionFunc_t ExtInstructions[0x100];
extern int ExtInstructionSizes[0x100];
extern int ExtInstructionCycles[0x100];

extern const char *FormatStrings[0x100];
extern int FormatStringOperandSize[0x100];

extern const char *ExtFormatStrings[0x100];
extern int ExtFormatStringOperandSize[0x100];

// Generic

void UNIMPL(CPU *cpu, instruction_t &i);

// 0x00

void NOP(CPU *cpu, instruction_t &i);
void LD_BC_IMM16(CPU *cpu, instruction_t &i);
void INC_BC(CPU *cpu, instruction_t &i);
void INC_B(CPU *cpu, instruction_t &i);
void DEC_B(CPU *cpu, instruction_t &i);
void LD_B_IMM8(CPU *cpu, instruction_t &i);
void INC_C(CPU *cpu, instruction_t &i);
void DEC_C(CPU *cpu, instruction_t &i);
void LD_C_IMM8(CPU *cpu, instruction_t &i);

// 0x10

void LD_DE_IMM16(CPU *cpu, instruction_t &i);
void LD_ADE_A(CPU *cpu, instruction_t &i);
void INC_DE(CPU *cpu, instruction_t &i);
void INC_D(CPU *cpu, instruction_t &i);
void DEC_D(CPU *cpu, instruction_t &i);
void LD_D_IMM8(CPU *cpu, instruction_t &i);
void RL_A(CPU *cpu, instruction_t &i);
void JR_IMM8(CPU *cpu, instruction_t &i);
void LD_A_ADE(CPU *cpu, instruction_t &i);
void INC_E(CPU *cpu, instruction_t &i);
void DEC_E(CPU *cpu, instruction_t &i);
void LD_E_IMM8(CPU *cpu, instruction_t &i);
void RRA(CPU *cpu, instruction_t &i);

// 0x20

void JR_NZ_IMM8(CPU *cpu, instruction_t &i);
void LD_HL_IMM16(CPU *cpu, instruction_t &i);
void LDI_HL_A(CPU *cpu, instruction_t &i);
void INC_H(CPU *cpu, instruction_t &i);
void DEC_H(CPU *cpu, instruction_t &i);
void LD_H_IMM8(CPU *cpu, instruction_t &i);
void JR_Z_IMM8(CPU *cpu, instruction_t &i);
void INC_HL(CPU *cpu, instruction_t &i);
void LDI_A_HL(CPU *cpu, instruction_t &i);
void INC_L(CPU *cpu, instruction_t &i);
void DEC_L(CPU *cpu, instruction_t &i);
void LD_L_IMM8(CPU *cpu, instruction_t &i);

// 0x30

void JR_NC_IMM8(CPU *cpu, instruction_t &i);
void LD_SP_IMM16(CPU *cpu, instruction_t &i);
void LDD_HL_A(CPU *cpu, instruction_t &i);
void DEC_A(CPU *cpu, instruction_t &i);
void LD_A_IMM8(CPU *cpu, instruction_t &i);

// 0x40

void LD_B_B(CPU *cpu, instruction_t &i);
void LD_B_C(CPU *cpu, instruction_t &i);
void LD_B_D(CPU *cpu, instruction_t &i);
void LD_B_E(CPU *cpu, instruction_t &i);
void LD_B_H(CPU *cpu, instruction_t &i);
void LD_B_L(CPU *cpu, instruction_t &i);
void LD_B_AHL(CPU *cpu, instruction_t &i);
void LD_B_A(CPU *cpu, instruction_t &i);
void LD_C_B(CPU *cpu, instruction_t &i);
void LD_C_C(CPU *cpu, instruction_t &i);
void LD_C_D(CPU *cpu, instruction_t &i);
void LD_C_E(CPU *cpu, instruction_t &i);
void LD_C_H(CPU *cpu, instruction_t &i);
void LD_C_L(CPU *cpu, instruction_t &i);
void LD_C_AHL(CPU *cpu, instruction_t &i);
void LD_C_A(CPU *cpu, instruction_t &i);

// 0x50

void LD_D_B(CPU *cpu, instruction_t &i);
void LD_D_C(CPU *cpu, instruction_t &i);
void LD_D_D(CPU *cpu, instruction_t &i);
void LD_D_E(CPU *cpu, instruction_t &i);
void LD_D_H(CPU *cpu, instruction_t &i);
void LD_D_L(CPU *cpu, instruction_t &i);
void LD_D_AHL(CPU *cpu, instruction_t &i);
void LD_D_A(CPU *cpu, instruction_t &i);
void LD_E_B(CPU *cpu, instruction_t &i);
void LD_E_C(CPU *cpu, instruction_t &i);
void LD_E_D(CPU *cpu, instruction_t &i);
void LD_E_E(CPU *cpu, instruction_t &i);
void LD_E_H(CPU *cpu, instruction_t &i);
void LD_E_L(CPU *cpu, instruction_t &i);
void LD_E_A(CPU *cpu, instruction_t &i);

// 0x60

void LD_H_B(CPU *cpu, instruction_t &i);
void LD_H_C(CPU *cpu, instruction_t &i);
void LD_H_D(CPU *cpu, instruction_t &i);
void LD_H_E(CPU *cpu, instruction_t &i);
void LD_H_H(CPU *cpu, instruction_t &i);
void LD_H_L(CPU *cpu, instruction_t &i);
void LD_H_A(CPU *cpu, instruction_t &i);
void LD_L_B(CPU *cpu, instruction_t &i);
void LD_L_C(CPU *cpu, instruction_t &i);
void LD_L_D(CPU *cpu, instruction_t &i);
void LD_L_E(CPU *cpu, instruction_t &i);
void LD_L_H(CPU *cpu, instruction_t &i);
void LD_L_L(CPU *cpu, instruction_t &i);
void LD_L_A(CPU *cpu, instruction_t &i);

// 0x70

void LD_AHL_B(CPU *cpu, instruction_t &i);
void LD_AHL_C(CPU *cpu, instruction_t &i);
void LD_AHL_D(CPU *cpu, instruction_t &i);
void LD_AHL_A(CPU *cpu, instruction_t &i);
void LD_A_B(CPU *cpu, instruction_t &i);
void LD_A_C(CPU *cpu, instruction_t &i);
void LD_A_D(CPU *cpu, instruction_t &i);
void LD_A_E(CPU *cpu, instruction_t &i);
void LD_A_H(CPU *cpu, instruction_t &i);
void LD_A_L(CPU *cpu, instruction_t &i);
void LD_A_A(CPU *cpu, instruction_t &i);

// 0x80

void ADD_A_AHL(CPU *cpu, instruction_t &i);

// 0x90

void SUB_A_B(CPU *cpu, instruction_t &i);

// 0xA0

void XOR_A_C(CPU *cpu, instruction_t &i);
void XOR_A_AHL(CPU *cpu, instruction_t &i);
void XOR_A_A(CPU *cpu, instruction_t &i);

// 0xB0

void OR_A_B(CPU *cpu, instruction_t &i);
void OR_A_C(CPU *cpu, instruction_t &i);
void OR_A_D(CPU *cpu, instruction_t &i);
void OR_A_E(CPU *cpu, instruction_t &i);
void OR_A_H(CPU *cpu, instruction_t &i);
void OR_A_L(CPU *cpu, instruction_t &i);
void OR_A_A(CPU *cpu, instruction_t &i);
void CP_A_AHL(CPU *cpu, instruction_t &i);

// 0xC0

void POP_BC(CPU *cpu, instruction_t &i);
void PUSH_BC(CPU *cpu, instruction_t &i);
void ADD_A_IMM8(CPU *cpu, instruction_t &i);
void JP_IMM16(CPU *cpu, instruction_t &i);
void CALL_NZ_IMM16(CPU *cpu, instruction_t &i);
void RET(CPU *cpu, instruction_t &i);
void CB(CPU *cpu, instruction_t &i);
void CALL_IMM16(CPU *cpu, instruction_t &i);

// 0xD0

void POP_DE(CPU *cpu, instruction_t &i);
void PUSH_DE(CPU *cpu, instruction_t &i);
void SUB_A_IMM8(CPU *cpu, instruction_t &i);

// 0xE0

void LDH_IMM8_A(CPU *cpu, instruction_t &i);
void POP_HL(CPU *cpu, instruction_t &i);
void LDH_C_A(CPU *cpu, instruction_t &i);
void PUSH_HL(CPU *cpu, instruction_t &i);
void AND_A_IMM8(CPU *cpu, instruction_t &i);
void LD_AIMM16_A(CPU *cpu, instruction_t &i);
void XOR_A_IMM8(CPU *cpu, instruction_t &i);

// 0xF0

void LDH_A_IMM8(CPU *cpu, instruction_t &i);
void POP_AF(CPU *cpu, instruction_t &i);
void DI(CPU *cpu, instruction_t &i);
void PUSH_AF(CPU *cpu, instruction_t &i);
void LD_A_AIMM16(CPU *cpu, instruction_t &i);
void EI(CPU *cpu, instruction_t &i);
void CP_A_IMM8(CPU *cpu, instruction_t &i);

// 0xCB // Extended

// 0x10

void RL_C(CPU *cpu, instruction_t &i);
void RR_C(CPU *cpu, instruction_t &i);
void RR_D(CPU *cpu, instruction_t &i);
void RR_A(CPU *cpu, instruction_t &i);

// 0x30

void SRL_B(CPU *cpu, instruction_t &i); // 0x38

// 0x70

void BIT_7H(CPU *cpu, instruction_t &i);

#endif // CPU_INSTRUCTIONS_H
