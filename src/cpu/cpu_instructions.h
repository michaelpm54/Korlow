#ifndef CPU_INSTRUCTIONS_H
#define CPU_INSTRUCTIONS_H

#include <cstdint>
#include "types.h"

extern const Instruction kInstructions[];

// Generic

void UNIMPL(Core &c);
void INVALID(Core &c);

// 0x00

void NOP(Core &c);
void LD_BC_IMM16(Core &c);
void LD_ABC_A(Core &c);
void INC_BC(Core &c);
void INC_B(Core &c);
void DEC_B(Core &c);
void LD_B_IMM8(Core &c);
void RLCA(Core &c);
void LD_AIMM16_SP(Core &c);
void ADD_HL_BC(Core &c);
void LD_A_ABC(Core &c);
void DEC_BC(Core &c);
void INC_C(Core &c);
void DEC_C(Core &c);
void LD_C_IMM8(Core &c);
void RRCA(Core &c);

// 0x10

void STOP(Core &c);
void LD_DE_IMM16(Core &c);
void LD_ADE_A(Core &c);
void INC_DE(Core &c);
void INC_D(Core &c);
void DEC_D(Core &c);
void LD_D_IMM8(Core &c);
void RLA(Core &c);
void JR_IMM8(Core &c);
void ADD_HL_DE(Core &c);
void LD_A_ADE(Core &c);
void DEC_DE(Core &c);
void INC_E(Core &c);
void DEC_E(Core &c);
void LD_E_IMM8(Core &c);
void RRA(Core &c);

// 0x20

void JR_NZ_IMM8(Core &c);
void LD_HL_IMM16(Core &c);
void LDI_HL_A(Core &c);
void INC_HL(Core &c);
void INC_H(Core &c);
void DEC_H(Core &c);
void LD_H_IMM8(Core &c);
void DAA(Core &c);
void JR_Z_IMM8(Core &c);
void ADD_HL_HL(Core &c);
void LDI_A_HL(Core &c);
void DEC_HL(Core &c);
void INC_L(Core &c);
void DEC_L(Core &c);
void LD_L_IMM8(Core &c);
void CPL(Core &c);

// 0x30

void JR_NC_IMM8(Core &c);
void LD_SP_IMM16(Core &c);
void LDD_HL_A(Core &c);
void INC_SP(Core &c);
void INC_AHL(Core &c);
void DEC_AHL(Core &c); // 0x35
void LD_AHL_IMM8(Core &c);
void SCF(Core &c);
void JR_C_IMM8(Core &c); // 0x38
void ADD_HL_SP(Core &c);
void LDD_A_HL(Core &c);
void DEC_SP(Core &c);
void INC_A(Core &c);
void DEC_A(Core &c);
void LD_A_IMM8(Core &c);
void CCF(Core &c);

// 0x40

void LD_B_B(Core &c);
void LD_B_C(Core &c);
void LD_B_D(Core &c);
void LD_B_E(Core &c);
void LD_B_H(Core &c);
void LD_B_L(Core &c);
void LD_B_AHL(Core &c);
void LD_B_A(Core &c);
void LD_C_B(Core &c);
void LD_C_C(Core &c);
void LD_C_D(Core &c);
void LD_C_E(Core &c);
void LD_C_H(Core &c);
void LD_C_L(Core &c);
void LD_C_AHL(Core &c);
void LD_C_A(Core &c);

// 0x50

void LD_D_B(Core &c);
void LD_D_C(Core &c);
void LD_D_D(Core &c);
void LD_D_E(Core &c);
void LD_D_H(Core &c);
void LD_D_L(Core &c);
void LD_D_AHL(Core &c);
void LD_D_A(Core &c);
void LD_E_B(Core &c);
void LD_E_C(Core &c);
void LD_E_D(Core &c);
void LD_E_E(Core &c);
void LD_E_H(Core &c);
void LD_E_L(Core &c);
void LD_E_AHL(Core &c);
void LD_E_A(Core &c);

// 0x60

void LD_H_B(Core &c);
void LD_H_C(Core &c);
void LD_H_D(Core &c);
void LD_H_E(Core &c);
void LD_H_H(Core &c);
void LD_H_L(Core &c);
void LD_H_AHL(Core &c);
void LD_H_A(Core &c);
void LD_L_B(Core &c);
void LD_L_C(Core &c);
void LD_L_D(Core &c);
void LD_L_E(Core &c);
void LD_L_H(Core &c);
void LD_L_L(Core &c);
void LD_L_AHL(Core &c);
void LD_L_A(Core &c);

// 0x70

void LD_AHL_B(Core &c);
void LD_AHL_C(Core &c);
void LD_AHL_D(Core &c);
void LD_AHL_E(Core &c);
void LD_AHL_H(Core &c);
void LD_AHL_L(Core &c);
void HALT(Core &c);
void LD_AHL_A(Core &c);
void LD_A_B(Core &c);
void LD_A_C(Core &c);
void LD_A_D(Core &c);
void LD_A_E(Core &c);
void LD_A_H(Core &c);
void LD_A_L(Core &c);
void LD_A_AHL(Core &c);
void LD_A_A(Core &c);

// 0x80

void ADD_A_B(Core &c);
void ADD_A_C(Core &c);
void ADD_A_D(Core &c);
void ADD_A_E(Core &c);
void ADD_A_H(Core &c);
void ADD_A_L(Core &c);
void ADD_A_AHL(Core &c);
void ADD_A_A(Core &c);
void ADC_A_B(Core &c);
void ADC_A_C(Core &c);
void ADC_A_D(Core &c);
void ADC_A_E(Core &c);
void ADC_A_H(Core &c);
void ADC_A_L(Core &c);
void ADC_A_AHL(Core &c);
void ADC_A_A(Core &c);

// 0x90

void SUB_A_B(Core &c);
void SUB_A_C(Core &c);
void SUB_A_D(Core &c);
void SUB_A_E(Core &c);
void SUB_A_H(Core &c);
void SUB_A_L(Core &c);
void SUB_A_AHL(Core &c);
void SUB_A_A(Core &c);
void SBC_A_B(Core &c);
void SBC_A_C(Core &c);
void SBC_A_D(Core &c);
void SBC_A_E(Core &c);
void SBC_A_H(Core &c);
void SBC_A_L(Core &c);
void SBC_A_AHL(Core &c);
void SBC_A_A(Core &c);

// 0xA0

void AND_A_B(Core &c);
void AND_A_C(Core &c);
void AND_A_D(Core &c);
void AND_A_E(Core &c);
void AND_A_H(Core &c);
void AND_A_L(Core &c);
void AND_A_AHL(Core &c);
void AND_A_A(Core &c);
void XOR_A_B(Core &c);
void XOR_A_C(Core &c);
void XOR_A_D(Core &c);
void XOR_A_E(Core &c);
void XOR_A_H(Core &c);
void XOR_A_L(Core &c);
void XOR_A_AHL(Core &c);
void XOR_A_A(Core &c);

// 0xB0

void OR_A_B(Core &c);
void OR_A_C(Core &c);
void OR_A_D(Core &c);
void OR_A_E(Core &c);
void OR_A_H(Core &c);
void OR_A_L(Core &c);
void OR_A_AHL(Core &c);
void OR_A_A(Core &c);
void CP_A_B(Core &c);
void CP_A_C(Core &c);
void CP_A_D(Core &c);
void CP_A_E(Core &c);
void CP_A_H(Core &c);
void CP_A_L(Core &c);
void CP_A_AHL(Core &c);
void CP_A_A(Core &c);

// 0xC0

void RET_NZ(Core &c);
void POP_BC(Core &c);
void JP_NZ_IMM16(Core &c);
void JP_IMM16(Core &c);
void CALL_NZ_IMM16(Core &c);
void PUSH_BC(Core &c);
void ADD_A_IMM8(Core &c);
void RST_00(Core &c);
void RET_Z(Core &c);
void RET(Core &c);
void JP_Z_IMM16(Core &c);
void CB(Core &c);
void CALL_Z_IMM16(Core &c);
void CALL_IMM16(Core &c);
void ADC_A_IMM8(Core &c);
void RST_08(Core &c);

// 0xD0

void RET_NC(Core &c);
void POP_DE(Core &c);
void JP_NC_IMM16(Core &c);
// void INVALID(Core &c);
void CALL_NC_IMM16(Core &c);
void PUSH_DE(Core &c);
void SUB_A_IMM8(Core &c);
void RST_10(Core &c);
void RET_C(Core &c);
void RETI(Core &c);
void JP_C_IMM16(Core &c);
// void INVALID(Core &c);
void CALL_C_IMM16(Core &c);
// void INVALID(Core &c);
void SBC_A_IMM8(Core &c);
void RST_18(Core &c);

// 0xE0

void LDH_IMM8_A(Core &c);
void POP_HL(Core &c);
void LDH_C_A(Core &c);
// void INVALID(Core &c);
// void INVALID(Core &c);
void PUSH_HL(Core &c);
void AND_A_IMM8(Core &c);
void RST_20(Core &c);
void ADD_SP_IMM8(Core &c);
void JP_HL(Core &c);
void LD_AIMM16_A(Core &c);
// void INVALID(Core &c);
// void INVALID(Core &c);
// void INVALID(Core &c);
void XOR_A_IMM8(Core &c);
void RST_28(Core &c);

// 0xF0

void LDH_A_IMM8(Core &c);
void POP_AF(Core &c);
void LDH_A_C(Core &c);
void DI(Core &c);
// void INVALID(Core &c);
void PUSH_AF(Core &c);
void OR_A_IMM8(Core &c);
void RST_30(Core &c);
void LD_HL_SPIMM8(Core &c);
void LD_SP_HL(Core &c);
void LD_A_AIMM16(Core &c);
void EI(Core &c);
// void INVALID(Core &c);
// void INVALID(Core &c);
void CP_A_IMM8(Core &c);
void RST_38(Core &c);

// 0xCB // Extended

// 0x00

void RLC_B(Core &c);
void RLC_C(Core &c);
void RLC_D(Core &c);
void RLC_E(Core &c);
void RLC_H(Core &c);
void RLC_L(Core &c);
void RLC_AHL(Core &c);
void RLC_A(Core &c);
void RRC_B(Core &c);
void RRC_C(Core &c);
void RRC_D(Core &c);
void RRC_E(Core &c);
void RRC_H(Core &c);
void RRC_L(Core &c);
void RRC_AHL(Core &c);
void RRC_A(Core &c);

// 0x10

void RL_B(Core &c);
void RL_C(Core &c);
void RL_D(Core &c);
void RL_E(Core &c);
void RL_H(Core &c);
void RL_L(Core &c);
void RL_AHL(Core &c);
void RL_A(Core &c);
void RR_B(Core &c);
void RR_C(Core &c);
void RR_D(Core &c);
void RR_E(Core &c);
void RR_H(Core &c);
void RR_L(Core &c);
void RR_AHL(Core &c);
void RR_A(Core &c);

// 0x20

void SLA_B(Core &c);
void SLA_C(Core &c);
void SLA_D(Core &c);
void SLA_E(Core &c);
void SLA_H(Core &c);
void SLA_L(Core &c);
void SLA_AHL(Core &c);
void SLA_A(Core &c);
void SRA_B(Core &c);
void SRA_C(Core &c);
void SRA_D(Core &c);
void SRA_E(Core &c);
void SRA_H(Core &c);
void SRA_L(Core &c);
void SRA_AHL(Core &c);
void SRA_A(Core &c);

// 0x30

void SWAP_B(Core &c);
void SWAP_C(Core &c);
void SWAP_D(Core &c);
void SWAP_E(Core &c);
void SWAP_H(Core &c);
void SWAP_L(Core &c);
void SWAP_AHL(Core &c);
void SWAP_A(Core &c);
void SRL_B(Core &c);
void SRL_C(Core &c);
void SRL_D(Core &c);
void SRL_E(Core &c);
void SRL_H(Core &c);
void SRL_L(Core &c);
void SRL_AHL(Core &c);
void SRL_A(Core &c);

// 0x70

void BIT_7H(Core &c);

#endif // CPU_INSTRUCTIONS_H
