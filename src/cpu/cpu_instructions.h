#ifndef CPU_INSTRUCTIONS_H
#define CPU_INSTRUCTIONS_H

#include "emu_types.h"

extern const Instruction kInstructions[];

// Generic

void UNIMPL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void INVALID(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);

// 0x00

void NOP(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_BC_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_ABC_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void INC_BC(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void INC_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void DEC_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_B_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RLCA(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_AIMM16_SP(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void ADD_HL_BC(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_A_ABC(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void DEC_BC(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void INC_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void DEC_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_C_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RRCA(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);

// 0x10

void STOP(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_DE_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_ADE_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void INC_DE(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void INC_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void DEC_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_D_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RLA(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void JR_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void ADD_HL_DE(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_A_ADE(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void DEC_DE(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void INC_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void DEC_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_E_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RRA(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);

// 0x20

void JR_NZ_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_HL_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LDI_HL_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void INC_HL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void INC_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void DEC_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_H_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void DAA(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void JR_Z_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void ADD_HL_HL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LDI_A_HL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void DEC_HL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void INC_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void DEC_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_L_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void CPL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);

// 0x30

void JR_NC_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_SP_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LDD_HL_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void INC_SP(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void INC_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void DEC_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);    // 0x35
void LD_AHL_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SCF(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void JR_C_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);    // 0x38
void ADD_HL_SP(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LDD_A_HL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void DEC_SP(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void INC_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void DEC_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_A_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void CCF(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);

// 0x40

void LD_B_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_B_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_B_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_B_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_B_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_B_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_B_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_B_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_C_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_C_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_C_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_C_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_C_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_C_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_C_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_C_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);

// 0x50

void LD_D_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_D_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_D_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_D_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_D_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_D_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_D_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_D_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_E_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_E_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_E_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_E_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_E_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_E_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_E_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_E_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);

// 0x60

void LD_H_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_H_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_H_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_H_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_H_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_H_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_H_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_H_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_L_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_L_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_L_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_L_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_L_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_L_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_L_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_L_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);

// 0x70

void LD_AHL_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_AHL_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_AHL_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_AHL_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_AHL_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_AHL_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void HALT(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_AHL_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_A_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_A_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_A_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_A_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_A_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_A_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_A_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_A_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);

// 0x80

void ADD_A_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void ADD_A_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void ADD_A_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void ADD_A_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void ADD_A_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void ADD_A_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void ADD_A_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void ADD_A_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void ADC_A_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void ADC_A_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void ADC_A_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void ADC_A_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void ADC_A_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void ADC_A_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void ADC_A_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void ADC_A_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);

// 0x90

void SUB_A_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SUB_A_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SUB_A_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SUB_A_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SUB_A_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SUB_A_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SUB_A_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SUB_A_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SBC_A_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SBC_A_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SBC_A_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SBC_A_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SBC_A_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SBC_A_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SBC_A_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SBC_A_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);

// 0xA0

void AND_A_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void AND_A_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void AND_A_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void AND_A_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void AND_A_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void AND_A_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void AND_A_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void AND_A_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void XOR_A_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void XOR_A_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void XOR_A_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void XOR_A_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void XOR_A_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void XOR_A_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void XOR_A_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void XOR_A_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);

// 0xB0

void OR_A_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void OR_A_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void OR_A_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void OR_A_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void OR_A_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void OR_A_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void OR_A_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void OR_A_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void CP_A_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void CP_A_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void CP_A_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void CP_A_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void CP_A_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void CP_A_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void CP_A_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void CP_A_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);

// 0xC0

void RET_NZ(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void POP_BC(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void JP_NZ_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void JP_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void CALL_NZ_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void PUSH_BC(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void ADD_A_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RST_00(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RET_Z(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RET(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void JP_Z_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void CB(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void CALL_Z_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void CALL_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void ADC_A_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RST_08(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);

// 0xD0

void RET_NC(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void POP_DE(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void JP_NC_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
// void INVALID(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void CALL_NC_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void PUSH_DE(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SUB_A_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RST_10(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RET_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RETI(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void JP_C_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
// void INVALID(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void CALL_C_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
// void INVALID(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SBC_A_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RST_18(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);

// 0xE0

void LDH_IMM8_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void POP_HL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LDH_C_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
// void INVALID(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
// void INVALID(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void PUSH_HL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void AND_A_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RST_20(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void ADD_SP_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void JP_HL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_AIMM16_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
// void INVALID(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
// void INVALID(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
// void INVALID(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void XOR_A_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RST_28(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);

// 0xF0

void LDH_A_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void POP_AF(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LDH_A_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void DI(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
// void INVALID(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void PUSH_AF(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void OR_A_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RST_30(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_HL_SPIMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_SP_HL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void LD_A_AIMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void EI(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
// void INVALID(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
// void INVALID(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void CP_A_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RST_38(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);

// 0xCB // Extended

// 0x00

void RLC_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RLC_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RLC_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RLC_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RLC_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RLC_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RLC_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RLC_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RRC_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RRC_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RRC_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RRC_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RRC_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RRC_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RRC_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RRC_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);

// 0x10

void RL_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RL_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RL_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RL_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RL_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RL_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RL_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RL_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RR_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RR_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RR_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RR_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RR_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RR_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RR_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void RR_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);

// 0x20

void SLA_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SLA_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SLA_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SLA_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SLA_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SLA_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SLA_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SLA_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SRA_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SRA_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SRA_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SRA_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SRA_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SRA_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SRA_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SRA_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);

// 0x30

void SWAP_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SWAP_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SWAP_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SWAP_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SWAP_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SWAP_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SWAP_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SWAP_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SRL_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SRL_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SRL_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SRL_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SRL_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SRL_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SRL_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);
void SRL_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);

// 0x70

void BIT_7H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles);

#endif    // CPU_INSTRUCTIONS_H
