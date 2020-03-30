#ifndef CPU_INSTRUCTIONS_H
#define CPU_INSTRUCTIONS_H

#include <cstdint>
#include "types.h"

extern const instructionFunc_t kInstructions[0x100];
extern const instructionFunc_t kCbInstructions[0x100];

// Generic

void UNIMPL(CPU *cpu, instruction_t &i);
void INVALID(CPU *cpu, instruction_t &i);

// 0x00

void NOP(CPU *cpu, instruction_t &i);
void LD_BC_IMM16(CPU *cpu, instruction_t &i);
void LD_ABC_A(CPU *cpu, instruction_t &i);
void INC_BC(CPU *cpu, instruction_t &i);
void INC_B(CPU *cpu, instruction_t &i);
void DEC_B(CPU *cpu, instruction_t &i);
void LD_B_IMM8(CPU *cpu, instruction_t &i);
void RLCA(CPU *cpu, instruction_t &i);
void LD_AIMM16_SP(CPU *cpu, instruction_t &i);
void ADD_HL_BC(CPU *cpu, instruction_t &i);
void LD_A_ABC(CPU *cpu, instruction_t &i);
void DEC_BC(CPU *cpu, instruction_t &i);
void INC_C(CPU *cpu, instruction_t &i);
void DEC_C(CPU *cpu, instruction_t &i);
void LD_C_IMM8(CPU *cpu, instruction_t &i);
void RRCA(CPU *cpu, instruction_t &i);

// 0x10

void STOP(CPU *cpu, instruction_t &i);
void LD_DE_IMM16(CPU *cpu, instruction_t &i);
void LD_ADE_A(CPU *cpu, instruction_t &i);
void INC_DE(CPU *cpu, instruction_t &i);
void INC_D(CPU *cpu, instruction_t &i);
void DEC_D(CPU *cpu, instruction_t &i);
void LD_D_IMM8(CPU *cpu, instruction_t &i);
void RLA(CPU *cpu, instruction_t &i);
void JR_IMM8(CPU *cpu, instruction_t &i);
void ADD_HL_DE(CPU *cpu, instruction_t &i);
void LD_A_ADE(CPU *cpu, instruction_t &i);
void DEC_DE(CPU *cpu, instruction_t &i);
void INC_E(CPU *cpu, instruction_t &i);
void DEC_E(CPU *cpu, instruction_t &i);
void LD_E_IMM8(CPU *cpu, instruction_t &i);
void RRA(CPU *cpu, instruction_t &i);

// 0x20

void JR_NZ_IMM8(CPU *cpu, instruction_t &i);
void LD_HL_IMM16(CPU *cpu, instruction_t &i);
void LDI_HL_A(CPU *cpu, instruction_t &i);
void INC_HL(CPU *cpu, instruction_t &i);
void INC_H(CPU *cpu, instruction_t &i);
void DEC_H(CPU *cpu, instruction_t &i);
void LD_H_IMM8(CPU *cpu, instruction_t &i);
void DAA(CPU *cpu, instruction_t &i);
void JR_Z_IMM8(CPU *cpu, instruction_t &i);
void ADD_HL_HL(CPU *cpu, instruction_t &i);
void LDI_A_HL(CPU *cpu, instruction_t &i);
void DEC_HL(CPU *cpu, instruction_t &i);
void INC_L(CPU *cpu, instruction_t &i);
void DEC_L(CPU *cpu, instruction_t &i);
void LD_L_IMM8(CPU *cpu, instruction_t &i);
void CPL(CPU *cpu, instruction_t &i);

// 0x30

void JR_NC_IMM8(CPU *cpu, instruction_t &i);
void LD_SP_IMM16(CPU *cpu, instruction_t &i);
void LDD_HL_A(CPU *cpu, instruction_t &i);
void INC_SP(CPU *cpu, instruction_t &i);
void INC_AHL(CPU *cpu, instruction_t &i);
void DEC_AHL(CPU *cpu, instruction_t &i); // 0x35
void LD_AHL_IMM8(CPU *cpu, instruction_t &i);
void SCF(CPU *cpu, instruction_t &i);
void JR_C_IMM8(CPU *cpu, instruction_t &i); // 0x38
void ADD_HL_SP(CPU *cpu, instruction_t &i);
void LDD_A_HL(CPU *cpu, instruction_t &i);
void DEC_SP(CPU *cpu, instruction_t &i);
void INC_A(CPU *cpu, instruction_t &i);
void DEC_A(CPU *cpu, instruction_t &i);
void LD_A_IMM8(CPU *cpu, instruction_t &i);
void CCF(CPU *cpu, instruction_t &i);

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
void LD_E_AHL(CPU *cpu, instruction_t &i);
void LD_E_A(CPU *cpu, instruction_t &i);

// 0x60

void LD_H_B(CPU *cpu, instruction_t &i);
void LD_H_C(CPU *cpu, instruction_t &i);
void LD_H_D(CPU *cpu, instruction_t &i);
void LD_H_E(CPU *cpu, instruction_t &i);
void LD_H_H(CPU *cpu, instruction_t &i);
void LD_H_L(CPU *cpu, instruction_t &i);
void LD_H_AHL(CPU *cpu, instruction_t &i);
void LD_H_A(CPU *cpu, instruction_t &i);
void LD_L_B(CPU *cpu, instruction_t &i);
void LD_L_C(CPU *cpu, instruction_t &i);
void LD_L_D(CPU *cpu, instruction_t &i);
void LD_L_E(CPU *cpu, instruction_t &i);
void LD_L_H(CPU *cpu, instruction_t &i);
void LD_L_L(CPU *cpu, instruction_t &i);
void LD_L_AHL(CPU *cpu, instruction_t &i);
void LD_L_A(CPU *cpu, instruction_t &i);

// 0x70

void LD_AHL_B(CPU *cpu, instruction_t &i);
void LD_AHL_C(CPU *cpu, instruction_t &i);
void LD_AHL_D(CPU *cpu, instruction_t &i);
void LD_AHL_E(CPU *cpu, instruction_t &i);
void LD_AHL_H(CPU *cpu, instruction_t &i);
void LD_AHL_L(CPU *cpu, instruction_t &i);
void HALT(CPU *cpu, instruction_t &i);
void LD_AHL_A(CPU *cpu, instruction_t &i);
void LD_A_B(CPU *cpu, instruction_t &i);
void LD_A_C(CPU *cpu, instruction_t &i);
void LD_A_D(CPU *cpu, instruction_t &i);
void LD_A_E(CPU *cpu, instruction_t &i);
void LD_A_H(CPU *cpu, instruction_t &i);
void LD_A_L(CPU *cpu, instruction_t &i);
void LD_A_AHL(CPU *cpu, instruction_t &i);
void LD_A_A(CPU *cpu, instruction_t &i);

// 0x80

void ADD_A_B(CPU *cpu, instruction_t &i);
void ADD_A_C(CPU *cpu, instruction_t &i);
void ADD_A_D(CPU *cpu, instruction_t &i);
void ADD_A_E(CPU *cpu, instruction_t &i);
void ADD_A_H(CPU *cpu, instruction_t &i);
void ADD_A_L(CPU *cpu, instruction_t &i);
void ADD_A_AHL(CPU *cpu, instruction_t &i);
void ADD_A_A(CPU *cpu, instruction_t &i);
void ADC_A_B(CPU *cpu, instruction_t &i);
void ADC_A_C(CPU *cpu, instruction_t &i);
void ADC_A_D(CPU *cpu, instruction_t &i);
void ADC_A_E(CPU *cpu, instruction_t &i);
void ADC_A_H(CPU *cpu, instruction_t &i);
void ADC_A_L(CPU *cpu, instruction_t &i);
void ADC_A_AHL(CPU *cpu, instruction_t &i);
void ADC_A_A(CPU *cpu, instruction_t &i);

// 0x90

void SUB_A_B(CPU *cpu, instruction_t &i);
void SUB_A_C(CPU *cpu, instruction_t &i);
void SUB_A_D(CPU *cpu, instruction_t &i);
void SUB_A_E(CPU *cpu, instruction_t &i);
void SUB_A_H(CPU *cpu, instruction_t &i);
void SUB_A_L(CPU *cpu, instruction_t &i);
void SUB_A_AHL(CPU *cpu, instruction_t &i);
void SUB_A_A(CPU *cpu, instruction_t &i);
void SBC_A_B(CPU *cpu, instruction_t &i);
void SBC_A_C(CPU *cpu, instruction_t &i);
void SBC_A_D(CPU *cpu, instruction_t &i);
void SBC_A_E(CPU *cpu, instruction_t &i);
void SBC_A_H(CPU *cpu, instruction_t &i);
void SBC_A_L(CPU *cpu, instruction_t &i);
void SBC_A_AHL(CPU *cpu, instruction_t &i);
void SBC_A_A(CPU *cpu, instruction_t &i);

// 0xA0

void AND_A_B(CPU *cpu, instruction_t &i);
void AND_A_C(CPU *cpu, instruction_t &i);
void AND_A_D(CPU *cpu, instruction_t &i);
void AND_A_E(CPU *cpu, instruction_t &i);
void AND_A_H(CPU *cpu, instruction_t &i);
void AND_A_L(CPU *cpu, instruction_t &i);
void AND_A_AHL(CPU *cpu, instruction_t &i);
void AND_A_A(CPU *cpu, instruction_t &i);
void XOR_A_B(CPU *cpu, instruction_t &i);
void XOR_A_C(CPU *cpu, instruction_t &i);
void XOR_A_D(CPU *cpu, instruction_t &i);
void XOR_A_E(CPU *cpu, instruction_t &i);
void XOR_A_H(CPU *cpu, instruction_t &i);
void XOR_A_L(CPU *cpu, instruction_t &i);
void XOR_A_AHL(CPU *cpu, instruction_t &i);
void XOR_A_A(CPU *cpu, instruction_t &i);

// 0xB0

void OR_A_B(CPU *cpu, instruction_t &i);
void OR_A_C(CPU *cpu, instruction_t &i);
void OR_A_D(CPU *cpu, instruction_t &i);
void OR_A_E(CPU *cpu, instruction_t &i);
void OR_A_H(CPU *cpu, instruction_t &i);
void OR_A_L(CPU *cpu, instruction_t &i);
void OR_A_AHL(CPU *cpu, instruction_t &i);
void OR_A_A(CPU *cpu, instruction_t &i);
void CP_A_B(CPU *cpu, instruction_t &i);
void CP_A_C(CPU *cpu, instruction_t &i);
void CP_A_D(CPU *cpu, instruction_t &i);
void CP_A_E(CPU *cpu, instruction_t &i);
void CP_A_H(CPU *cpu, instruction_t &i);
void CP_A_L(CPU *cpu, instruction_t &i);
void CP_A_AHL(CPU *cpu, instruction_t &i);
void CP_A_A(CPU *cpu, instruction_t &i);

// 0xC0

void RET_NZ(CPU *cpu, instruction_t &i);
void POP_BC(CPU *cpu, instruction_t &i);
void JP_NZ_IMM16(CPU *cpu, instruction_t &i);
void JP_IMM16(CPU *cpu, instruction_t &i);
void CALL_NZ_IMM16(CPU *cpu, instruction_t &i);
void PUSH_BC(CPU *cpu, instruction_t &i);
void ADD_A_IMM8(CPU *cpu, instruction_t &i);
void RST_00(CPU *cpu, instruction_t &i);
void RET_Z(CPU *cpu, instruction_t &i);
void RET(CPU *cpu, instruction_t &i);
void JP_Z_IMM16(CPU *cpu, instruction_t &i);
void CB(CPU *cpu, instruction_t &i);
void CALL_Z_IMM16(CPU *cpu, instruction_t &i);
void CALL_IMM16(CPU *cpu, instruction_t &i);
void ADC_A_IMM8(CPU *cpu, instruction_t &i);
void RST_08(CPU *cpu, instruction_t &i);

// 0xD0

void RET_NC(CPU *cpu, instruction_t &i);
void POP_DE(CPU *cpu, instruction_t &i);
void JP_NC_IMM16(CPU *cpu, instruction_t &i);
// void INVALID(CPU *cpu, instruction_t &i);
void CALL_NC_IMM16(CPU *cpu, instruction_t &i);
void PUSH_DE(CPU *cpu, instruction_t &i);
void SUB_A_IMM8(CPU *cpu, instruction_t &i);
void RST_10(CPU *cpu, instruction_t &i);
void RET_C(CPU *cpu, instruction_t &i);
void RETI(CPU *cpu, instruction_t &i);
void JP_C_IMM16(CPU *cpu, instruction_t &i);
// void INVALID(CPU *cpu, instruction_t &i);
void CALL_C_IMM16(CPU *cpu, instruction_t &i);
// void INVALID(CPU *cpu, instruction_t &i);
void SBC_A_IMM8(CPU *cpu, instruction_t &i);
void RST_18(CPU *cpu, instruction_t &i);

// 0xE0

void LDH_IMM8_A(CPU *cpu, instruction_t &i);
void POP_HL(CPU *cpu, instruction_t &i);
void LDH_C_A(CPU *cpu, instruction_t &i);
// void INVALID(CPU *cpu, instruction_t &i);
// void INVALID(CPU *cpu, instruction_t &i);
void PUSH_HL(CPU *cpu, instruction_t &i);
void AND_A_IMM8(CPU *cpu, instruction_t &i);
void RST_20(CPU *cpu, instruction_t &i);
void ADD_SP_IMM8(CPU *cpu, instruction_t &i);
void JP_HL(CPU *cpu, instruction_t &i);
void LD_AIMM16_A(CPU *cpu, instruction_t &i);
// void INVALID(CPU *cpu, instruction_t &i);
// void INVALID(CPU *cpu, instruction_t &i);
// void INVALID(CPU *cpu, instruction_t &i);
void XOR_A_IMM8(CPU *cpu, instruction_t &i);
void RST_28(CPU *cpu, instruction_t &i);

// 0xF0

void LDH_A_IMM8(CPU *cpu, instruction_t &i);
void POP_AF(CPU *cpu, instruction_t &i);
void LDH_A_C(CPU *cpu, instruction_t &i);
void DI(CPU *cpu, instruction_t &i);
// void INVALID(CPU *cpu, instruction_t &i);
void PUSH_AF(CPU *cpu, instruction_t &i);
void OR_A_IMM8(CPU *cpu, instruction_t &i);
void RST_30(CPU *cpu, instruction_t &i);
void LD_HL_SPIMM8(CPU *cpu, instruction_t &i);
void LD_SP_HL(CPU *cpu, instruction_t &i);
void LD_A_AIMM16(CPU *cpu, instruction_t &i);
void EI(CPU *cpu, instruction_t &i);
// void INVALID(CPU *cpu, instruction_t &i);
// void INVALID(CPU *cpu, instruction_t &i);
void CP_A_IMM8(CPU *cpu, instruction_t &i);
void RST_38(CPU *cpu, instruction_t &i);

// 0xCB // Extended

// 0x00

void RLC_B(CPU *cpu, instruction_t &i);
void RLC_C(CPU *cpu, instruction_t &i);
void RLC_D(CPU *cpu, instruction_t &i);
void RLC_E(CPU *cpu, instruction_t &i);
void RLC_H(CPU *cpu, instruction_t &i);
void RLC_L(CPU *cpu, instruction_t &i);
void RLC_AHL(CPU *cpu, instruction_t &i);
void RLC_A(CPU *cpu, instruction_t &i);
void RRC_B(CPU *cpu, instruction_t &i);
void RRC_C(CPU *cpu, instruction_t &i);
void RRC_D(CPU *cpu, instruction_t &i);
void RRC_E(CPU *cpu, instruction_t &i);
void RRC_H(CPU *cpu, instruction_t &i);
void RRC_L(CPU *cpu, instruction_t &i);
void RRC_AHL(CPU *cpu, instruction_t &i);
void RRC_A(CPU *cpu, instruction_t &i);

// 0x10

void RL_B(CPU *cpu, instruction_t &i);
void RL_C(CPU *cpu, instruction_t &i);
void RL_D(CPU *cpu, instruction_t &i);
void RL_E(CPU *cpu, instruction_t &i);
void RL_H(CPU *cpu, instruction_t &i);
void RL_L(CPU *cpu, instruction_t &i);
void RL_AHL(CPU *cpu, instruction_t &i);
void RL_A(CPU *cpu, instruction_t &i);
void RR_B(CPU *cpu, instruction_t &i);
void RR_C(CPU *cpu, instruction_t &i);
void RR_D(CPU *cpu, instruction_t &i);
void RR_E(CPU *cpu, instruction_t &i);
void RR_H(CPU *cpu, instruction_t &i);
void RR_L(CPU *cpu, instruction_t &i);
void RR_AHL(CPU *cpu, instruction_t &i);
void RR_A(CPU *cpu, instruction_t &i);

// 0x20

void SLA_B(CPU *cpu, instruction_t &i);
void SLA_C(CPU *cpu, instruction_t &i);
void SLA_D(CPU *cpu, instruction_t &i);
void SLA_E(CPU *cpu, instruction_t &i);
void SLA_H(CPU *cpu, instruction_t &i);
void SLA_L(CPU *cpu, instruction_t &i);
void SLA_AHL(CPU *cpu, instruction_t &i);
void SLA_A(CPU *cpu, instruction_t &i);
void SRA_B(CPU *cpu, instruction_t &i);
void SRA_C(CPU *cpu, instruction_t &i);
void SRA_D(CPU *cpu, instruction_t &i);
void SRA_E(CPU *cpu, instruction_t &i);
void SRA_H(CPU *cpu, instruction_t &i);
void SRA_L(CPU *cpu, instruction_t &i);
void SRA_AHL(CPU *cpu, instruction_t &i);
void SRA_A(CPU *cpu, instruction_t &i);

// 0x30

void SWAP_B(CPU *cpu, instruction_t &i);
void SWAP_C(CPU *cpu, instruction_t &i);
void SWAP_D(CPU *cpu, instruction_t &i);
void SWAP_E(CPU *cpu, instruction_t &i);
void SWAP_H(CPU *cpu, instruction_t &i);
void SWAP_L(CPU *cpu, instruction_t &i);
void SWAP_AHL(CPU *cpu, instruction_t &i);
void SWAP_A(CPU *cpu, instruction_t &i);
void SRL_B(CPU *cpu, instruction_t &i);
void SRL_C(CPU *cpu, instruction_t &i);
void SRL_D(CPU *cpu, instruction_t &i);
void SRL_E(CPU *cpu, instruction_t &i);
void SRL_H(CPU *cpu, instruction_t &i);
void SRL_L(CPU *cpu, instruction_t &i);
void SRL_AHL(CPU *cpu, instruction_t &i);
void SRL_A(CPU *cpu, instruction_t &i);

// 0x70

void BIT_7H(CPU *cpu, instruction_t &i);

#endif // CPU_INSTRUCTIONS_H
