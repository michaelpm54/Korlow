#include <cinttypes>
#include <cstdio>
#include "cpu_instructions.h"
#include "cpu.h"
#include "mmu.h"

instructionFunc_t InstructionList[0x100] =
{     
/*      0           1            2            3         4              5         6            7         8             9          A            B        C             D           E           F   */
/* 0 */ NOP,        LD_BC_IMM16, LD_ABC_A,    INC_BC,   INC_B,         DEC_B,    LD_B_IMM8,   RLCA,     LD_AIMM16_SP, ADD_HL_BC, LD_A_ABC,    DEC_BC,  INC_C,        DEC_C ,     LD_C_IMM8,  RRCA,
/* 1 */ STOP,       LD_DE_IMM16, LD_ADE_A,    INC_DE,   INC_D,         DEC_D,    LD_D_IMM8,   RL_A,     JR_IMM8,      ADD_HL_DE, LD_A_ADE,    DEC_DE,  INC_E,        DEC_E,      LD_E_IMM8,  RRA,
/* 2 */ JR_NZ_IMM8, LD_HL_IMM16, LDI_HL_A,    INC_HL,   INC_H,         DEC_H,    LD_H_IMM8,   DAA,      JR_Z_IMM8,    ADD_HL_HL, LDI_A_HL,    DEC_HL,  INC_L,        DEC_L,      LD_L_IMM8,  CPL,
/* 3 */ JR_NC_IMM8, LD_SP_IMM16, LDD_HL_A,    INC_SP,   INC_AHL,       DEC_AHL,  LD_AHL_IMM8, SCF,      JR_C_IMM8,    ADD_HL_SP, LDD_A_HL,    DEC_SP,  INC_A,        DEC_A,      LD_A_IMM8,  CCF,
/* 4 */ LD_B_B,     LD_B_C,      LD_B_D,      LD_B_E,   LD_B_H,        LD_B_L,   LD_B_AHL,    LD_B_A,   LD_C_B,       LD_C_C,    LD_C_D,      LD_C_E,  LD_C_H,       LD_C_L,     LD_C_AHL,   LD_C_A,
/* 5 */ LD_D_B,     LD_D_C,      LD_D_D,      LD_D_E,   LD_D_H,        LD_D_L,   LD_D_AHL,    LD_D_A,   LD_E_B,       LD_E_C,    LD_E_D,      LD_E_E,  LD_E_H,       LD_H_L,     LD_E_AHL,   LD_E_A,
/* 6 */ LD_H_B,     LD_H_C,      LD_H_D,      LD_H_E,   LD_H_H,        LD_H_L,   LD_H_AHL,    LD_H_A,   LD_L_B,       LD_L_C,    LD_L_D,      LD_L_E,  LD_L_H,       LD_L_L,     LD_L_AHL,   LD_L_A,
/* 7 */ LD_AHL_B,   LD_AHL_C,    LD_AHL_D,    LD_AHL_E, LD_AHL_H,      LD_AHL_L, HALT,        LD_AHL_A, LD_A_B,       LD_A_C,    LD_A_D,      LD_A_E,  LD_A_H,       LD_A_L,     LD_A_AHL,   LD_A_A,
/* 8 */ ADD_A_B,    ADD_A_C,     ADD_A_D,     ADD_A_E,  ADD_A_H,       ADD_A_L,  ADD_A_AHL,   ADD_A_A,  ADC_A_B,      ADC_A_C,   ADC_A_D,     ADC_A_E, ADC_A_H,      ADC_A_L,    ADC_A_AHL,  ADC_A_A,
/* 9 */ SUB_A_B,    SUB_A_C,     SUB_A_D,     SUB_A_E,  SUB_A_H,       SUB_A_L,  SUB_A_AHL,   SUB_A_A,  SBC_A_B,      SBC_A_C,   SBC_A_D,     SBC_A_E, SBC_A_H,      SBC_A_L,    SBC_A_AHL,  SBC_A_A,
/* A */ AND_A_B,    AND_A_C,     AND_A_D,     AND_A_E,  AND_A_H,       AND_A_L,  AND_A_AHL,   AND_A_A,  XOR_A_B,      XOR_A_C,   XOR_A_D,     XOR_A_E, XOR_A_H,      XOR_A_L,    XOR_A_AHL,  XOR_A_A,
/* B */ OR_A_B,     OR_A_C,      OR_A_D,      OR_A_E,   OR_A_H,        OR_A_L,   OR_A_AHL,    OR_A_A,   CP_A_B,       CP_A_C,    CP_A_D,      CP_A_E,  CP_A_H,       CP_A_L,     CP_A_AHL,   CP_A_A,
/* C */ RET_NZ,     POP_BC,      JP_NZ_IMM16, JP_IMM16, CALL_NZ_IMM16, PUSH_BC,  ADD_A_IMM8,  RST_00,   RET_Z,        RET,       JP_Z_IMM16,  CB,      CALL_Z_IMM16, CALL_IMM16, ADC_A_IMM8, RST_08,
/* D */ RET_NC,     POP_DE,      JP_NC_IMM16, INVALID,  CALL_NC_IMM16, PUSH_DE,  SUB_A_IMM8,  RST_10,   RET_C,        RETI,      JP_C_IMM16,  INVALID, CALL_C_IMM16, INVALID,    SBC_A_IMM8, RST_18,
/* E */ LDH_IMM8_A, POP_HL,      LDH_C_A,     INVALID,  INVALID,       PUSH_HL,  AND_A_IMM8,  RST_20,   ADD_SP_IMM8,  JP_HL,     LD_AIMM16_A, INVALID, INVALID,      INVALID,    XOR_A_IMM8, RST_28,
/* F */ LDH_A_IMM8, POP_AF,      LDH_A_C,     DI,       INVALID,       PUSH_AF,  OR_A_IMM8,   RST_30,   LD_HL_SPIMM8, LD_SP_HL,  LD_A_AIMM16, EI,      INVALID,      INVALID,    CP_A_IMM8,  RST_38,
}; 

const char *FormatStrings[0x100] =
{    
/*      0                   1              2              3             4                5             6                7             8                      9             A               B           C               D            E              F */
/* 0 */ "NOP",              "LD BC, %04X", "LD (BC), A",  "INC BC",     "INC B",         "DEC B",      "LD B, %02X",    "RLCA",       "LD (%04X), SP",       "ADD HL, BC", "LD A, (BC)",   "DEC BC",   "INC C",        "DEC C",     "LD C, %02X",  "RRCA",
/* 1 */ "STOP",             "LD DE, %04X", "LD (DE), A",  "INC DE",     "INC D",         "DEC D",      "LD D, %02X",    "RL A",       "JR %02" PRIX8,        "ADD HL, DE", "LD A, (DE)",   "DEC DE",   "INC E",        "DEC E",     "LD E, %02X",  "RRA",
/* 2 */ "JR NZ, %02" PRIX8, "LD HL, %04X", "LD (HL+), A", "INC HL",     "INC H",         "DEC H",      "LD H, %02X",    "DAA",        "JR Z, %02" PRIX8,     "ADD HL, HL", "LD A, (HL+)",  "DEC HL",   "INC L",        "DEC L",     "LD L, %02X",  "CPL",
/* 3 */ "JR NC, %02" PRIX8, "LD SP, %04X", "LD (HL-), A", "INC SP",     "INC (HL)",      "DEC (HL)",   "LD (HL), %02X", "SCF",        "JR C, %02" PRIX8,     "",           "",             "DEC SP",   "INC A",        "DEC A",     "LD A, %02X",  "CCF",
/* 4 */ "LD B, B",          "LD B, C",     "LD B, D",     "LD B, E",    "LD B, H",       "LD B, L",    "LD B, (HL)",    "LD B, A",    "LD C, B",             "LD C, C",    "LD C, D",      "LD C, E",  "LD C, H",      "LD C, L",   "LD C, (HL)",  "LD C, A",
/* 5 */ "LD D, B",          "LD D, C",     "LD D, D",     "LD D, E",    "LD D, H",       "LD D, L",    "LD D, (HL)",    "LD D, A",    "LD E, B",             "LD E, C",    "LD E, D",      "LD E, E",  "LD E, H",      "LD E, L",   "LD E, (HL)",  "LD E, A",
/* 6 */ "LD H, B",          "LD H, C",     "LD H, D",     "LD H, E",    "LD H, H",       "LD H, L",    "LD H, (HL)",    "LD H, A",    "LD L, B",             "LD L, C",    "LD L, D",      "LD L, E",  "LD L, H",      "LD L, L",   "LD L, (HL)",  "LD L, A",
/* 7 */ "LD (HL), B",       "LD (HL), C",  "LD (HL), D",  "LD (HL), E", "LD (HL), H",    "LD (HL), L", "HALT",          "LD (HL), A", "LD A, B",             "LD A, C",    "LD A, D",      "LD A, E",  "LD A, H",      "LD A, L",   "LD A, (HL)",  "LD A, A",
/* 8 */ "ADD A, B",         "ADD A, C",    "ADD A, D",    "ADD A, E",   "ADD A, H",      "ADD A, L",   "ADD A, (HL)",   "ADD A, A",   "ADC A, B",            "ADC A, C",   "ADC A, D",     "ADC A, E", "ADC A, H",     "ADC A, L",  "ADC A, (HL)", "ADC A, A",
/* 9 */ "SUB A, B",         "SUB A, C",    "SUB A, D",    "SUB A, E",   "SUB A, H",      "SUB A, L",   "SUB A, (HL)",   "SUB A, A",   "SBC A, B",            "SBC A, C",   "SBC A, D",     "SBC A, E", "SBC A, H",     "SBC A, L",  "SBC A, (HL)", "SBC A, A",
/* A */ "AND A, B",         "AND A, C",    "AND A, D",    "AND A, E",   "AND A, H",      "AND A, L",   "AND A, (HL)",   "AND A, A",   "XOR A, B",            "XOR A, C",   "XOR A, D",     "XOR A, E", "XOR A, H",     "XOR A, L",  "XOR A, (HL)", "XOR A, A",
/* B */ "OR A, B",          "OR A, C",     "OR A, D",     "OR A, E",    "OR A, H",       "OR A, L",    "OR A, (HL)",    "OR A, A",    "CP A, B",             "CP A, C",    "CP A, D",      "CP A, E",  "CP A, H",      "CP A, L",   "CP A, (HL)",  "CP A, A",
/* C */ "RET NZ",           "POP BC",      "JP NZ, %04X", "JP %04X",    "CALL NZ, %04X", "PUSH BC",    "ADD A, %02X",   "RST 00",     "RET Z",               "RET",        "JP Z, %04X",   "CB",       "CALL Z, %04X", "CALL %04X", "ADC A, %02X", "RST 10",
/* D */ "RET_NC",           "POP DE",      "JP NC, %04X", "INVALID",    "CALL NC, %04X", "PUSH DE",    "SUB A, %02X",   "RST 10",     "RET C",               "RETI",       "JP C, %04X",   "INVALID",  "",             "INVALID",   "SBC A, %02X", "RST 18",
/* E */ "LD (FF%02X), A",   "POP HL",      "LD (C), A",   "INVALID",    "INVALID",       "PUSH HL",    "AND A, %02X",   "RST 20",     "ADD SP, %02" PRIX8,   "JP (HL)",   "LD (%04X), A",  "INVALID",  "INVALID",      "INVALID",   "XOR A, %02X", "RST 28",
/* F */ "LD A, (FF%02X)",   "POP AF",      "LD A, (C)",   "DI",         "INVALID",       "PUSH AF",    "OR A, %02X",    "RST 30",     "LD HL, SP+%02" PRIX8, "LD SP, HL",  "LD A, (%04X)", "EI",       "INVALID",      "INVALID",   "CP A, %02X",  "RST 38",
};      

int FormatStringOperandSize[0x100] =
{
/*      0  1   2   3   4   5  6  7  8   9  A   B  C   D   E  F */
/* 0 */ 0, 16, 0,  0,  0,  0, 8, 0, 16, 0, 0,  0, 0,  0,  8, 0,
/* 1 */ 0, 16, 0,  0,  0,  0, 8, 0, 8,  0, 0,  0, 0,  0,  8, 0,
/* 2 */ 8, 16, 0,  0,  0,  0, 8, 0, 8,  0, 0,  0, 0,  0,  8, 0,
/* 3 */ 8, 16, 0,  0,  0,  0, 8, 0, 8,  0, 0,  0, 0,  0,  8, 0,
/* 4 */ 0, 0,  0,  0,  0,  0, 0, 0, 0,  0, 0,  0, 0,  0,  0, 0,
/* 5 */ 0, 0,  0,  0,  0,  0, 0, 0, 0,  0, 0,  0, 0,  0,  0, 0,
/* 6 */ 0, 0,  0,  0,  0,  0, 0, 0, 0,  0, 0,  0, 0,  0,  0, 0,
/* 7 */ 0, 0,  0,  0,  0,  0, 0, 0, 0,  0, 0,  0, 0,  0,  0, 0,
/* 8 */ 0, 0,  0,  0,  0,  0, 0, 0, 0,  0, 0,  0, 0,  0,  0, 0,
/* 9 */ 0, 0,  0,  0,  0,  0, 0, 0, 0,  0, 0,  0, 0,  0,  0, 0,
/* A */ 0, 0,  0,  0,  0,  0, 0, 0, 0,  0, 0,  0, 0,  0,  0, 0,
/* B */ 0, 0,  0,  0,  0,  0, 0, 0, 0,  0, 0,  0, 0,  0,  0, 0,
/* C */ 0, 0,  16, 16, 16, 8, 8, 0, 0,  0, 16, 0, 16, 16, 8, 0,
/* D */ 0, 0,  16, 0,  16, 0, 8, 0, 0,  0, 16, 0, 0,  0,  8, 0,
/* E */ 8, 0,  0,  0,  0,  0, 8, 0, 8,  0, 16, 0, 0,  0,  8, 0,
/* F */ 8, 0,  0,  0,  0,  0, 8, 0, 8,  0, 16, 0, 0,  0,  8, 0,
};

int InstructionSizes[0x100] =
{
/*      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
/* 0 */ 1, 3, 1, 1, 1, 1, 2, 1, 3, 1, 1, 1, 1, 1, 2, 1,
/* 1 */ 2, 3, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1,
/* 2 */ 2, 3, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1,
/* 3 */ 2, 3, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1,
/* 4 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 5 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 6 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 7 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 8 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 9 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* A */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* B */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* C */ 1, 1, 3, 3, 3, 1, 2, 1, 1, 1, 3, 1, 3, 3, 2, 1,
/* D */ 1, 1, 3, 0, 3, 1, 2, 1, 1, 1, 3, 0, 3, 0, 2, 1,
/* E */ 2, 1, 1, 0, 0, 1, 2, 1, 2, 1, 3, 0, 0, 0, 2, 1,
/* F */ 2, 1, 1, 1, 0, 1, 2, 1, 2, 1, 3, 1, 0, 0, 2, 1,
};

int InstructionCycles[0x100] =
{
/*      0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
/* 0 */ 4,  12, 8,  8,  4,  4,  8,  4,  20, 8,  8,  8,  4,  4,  8,  4,
/* 1 */ 4,  12, 8,  8,  4,  4,  8,  4,  12, 8,  8,  8,  4,  4,  8,  4,
/* 2 */ 12, 12, 8,  8,  4,  4,  8,  4,  12, 8,  8,  8,  4,  4,  8,  4,
/* 3 */ 12, 12, 8,  8,  12, 12, 12, 4,  12, 8,  8,  8,  4,  4,  8,  4,
/* 4 */ 4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
/* 5 */ 4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
/* 6 */ 4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
/* 7 */ 8,  8,  8,  8,  8,  8,  4,  8,  4,  4,  4,  4,  4,  4,  8,  4,
/* 8 */ 4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
/* 9 */ 4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
/* A */ 4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
/* B */ 4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
/* C */ 20, 12, 16, 16, 24, 16, 8,  16, 20, 16, 16, 4,  24, 24, 8,  16,
/* D */ 20, 12, 16, 0,  24, 16, 8,  16, 20, 16, 16, 0,  24, 0,  8,  16,
/* E */ 12, 12, 8,  0,  0,  16, 8,  16, 16, 4,  16, 0,  0,  0,  8,  16,
/* F */ 12, 12, 8,  4,  0,  16, 8,  16, 12, 8,  16, 4,  0,  0,  8,  16,
};

int InstructionCyclesAlt[0x100] =
{
/*      0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
/* 0 */ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* 1 */ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* 2 */ 8,  0,  0,  0,  0,  0,  0,  0,  8,  0,  0,  0,  0,  0,  0,  0,
/* 3 */ 8,  0,  0,  0,  0,  0,  0,  0,  8,  0,  0,  0,  0,  0,  0,  0,
/* 4 */ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* 5 */ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* 6 */ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* 7 */ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* 8 */ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* 9 */ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* A */ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* B */ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* C */ 8,  0,  12, 0,  12, 0,  0,  0,  8,  0, 12,  0,  12, 24, 0,  0,
/* D */ 8,  0,  12, 0,  12, 0,  0,  0,  8,  0, 12,  0,  12, 0,  0,  0,
/* E */ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/* F */ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

instructionFunc_t ExtInstructions[0x100] =
{
/*      0       1       2       3       4       5       6         7       8       9       A       B       C       D       E        F   */
/* 0 */ RLC_B,  RLC_C,  RLC_D,  RLC_E,  RLC_H,  RLC_L,  RLC_AHL,  RLC_A,  RRC_B,  RRC_C,  RRC_D,  RRC_E,  RRC_H,  RRC_L,  RRC_AHL, RRC_A,
/* 1 */ RL_B,   RL_C,   RL_D,   RL_E,   RL_H,   RL_L,   RL_AHL,   RL_A,   RR_B,   RR_C,   RR_D,   RR_E,   RR_H,   RR_L,   RR_AHL,  RR_A,
/* 2 */ SLA_B,  SLA_C,  SLA_D,  SLA_E,  SLA_H,  SLA_L,  SLA_AHL,  SLA_A,  SRA_B,  SRA_C,  SRA_D,  SRA_E,  SRA_H,  SRA_L,  SRA_AHL, SRA_A,
/* 3 */ SWAP_B, SWAP_C, SWAP_D, SWAP_E, SWAP_H, SWAP_L, SWAP_AHL, SWAP_A, SRL_B,  SRL_C,  SRL_D,  SRL_E,  SRL_H,  SRL_L,  SRL_AHL, SRL_A,
/* 4 */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,   UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,  UNIMPL,
/* 5 */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,   UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,  UNIMPL,
/* 6 */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,   UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,  UNIMPL,
/* 7 */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,   UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, BIT_7H, UNIMPL, UNIMPL,  UNIMPL,
/* 8 */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,   UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,  UNIMPL,
/* 9 */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,   UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,  UNIMPL,
/* A */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,   UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,  UNIMPL,
/* B */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,   UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,  UNIMPL,
/* C */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,   UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,  UNIMPL,
/* D */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,   UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,  UNIMPL,
/* E */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,   UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,  UNIMPL,
/* F */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,   UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,  UNIMPL,
};

int ExtInstructionSizes[0x100] =
{
/*      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
/* 0 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 1 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 2 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 3 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 4 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 5 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 6 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 7 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 8 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 9 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* A */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* B */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* C */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* D */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* E */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* F */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

int ExtInstructionCycles[0x100] =
{
/*      0  1  2  3  4  5  6   7  8  9  A  B  C  D  E   F */
/* 0 */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* 1 */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* 2 */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* 3 */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* 4 */ 8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8,
/* 5 */ 8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8,
/* 6 */ 8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8,
/* 7 */ 8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8,
/* 8 */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* 9 */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* A */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* B */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* C */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* D */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* E */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
/* F */ 8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
};

const char *ExtFormatStrings[0x100] =
{
/*      0         1         2         3         4         5         6            7         8        9        A        B        C        D        E           F       */
/* 0 */ "RLC B",  "RLC C",  "RLC D",  "RLC E",  "RLC H",  "RLC L",  "RLC (HL)",  "RLC A",  "RRC B", "RRC C", "RRC D", "RRC E", "RRC H", "RRC L", "RRC (HL)", "RRC A",
/* 1 */ "RL B",   "RL C",   "RL D",   "RL E",   "RL H",   "RL L",   "RL (HL)",   "RL A",   "RR B",  "RR C",  "RR D",  "RR E",  "RR H",  "RR L",  "RR (HL)",  "RR A",
/* 2 */ "SLA B",  "SLA C",  "SLA D",  "SLA E",  "SLA H",  "SLA L",  "SLA (HL)",  "SLA A",  "SRA B", "SRA C", "SRA D", "SRA E", "SRA H", "SRA L", "SRA (HL)", "SRA A",
/* 3 */ "SWAP B", "SWAP C", "SWAP D", "SWAP E", "SWAP H", "SWAP L", "SWAP (HL)", "SWAP A", "SRL B", "SRL C", "SRL D", "SRL E", "SRL H", "SRL L", "SRL (HL)", "SRL A",
/* 4 */ "",       "",       "",       "",       "",       "",       "",          "",       "",      "",     "",     "",     "",       "", "", "",
/* 5 */ "",       "",       "",       "",       "",       "",       "",          "",       "",      "",     "",     "",     "",       "", "", "",
/* 6 */ "",       "",       "",       "",       "",       "",       "",          "",       "",      "",     "",     "",     "",       "", "", "",
/* 7 */ "",       "",       "",       "",       "",       "",       "",          "",       "",      "",     "",     "",     "BIT 7H", "", "", "",
/* 8 */ "",       "",       "",       "",       "",       "",       "",          "",       "",      "",     "",     "",     "",       "", "", "",
/* 9 */ "",       "",       "",       "",       "",       "",       "",          "",       "",      "",     "",     "",     "",       "", "", "",
/* A */ "",       "",       "",       "",       "",       "",       "",          "",       "",      "",     "",     "",     "",       "", "", "",
/* B */ "",       "",       "",       "",       "",       "",       "",          "",       "",      "",     "",     "",     "",       "", "", "",
/* C */ "",       "",       "",       "",       "",       "",       "",          "",       "",      "",     "",     "",     "",       "", "", "",
/* D */ "",       "",       "",       "",       "",       "",       "",          "",       "",      "",     "",     "",     "",       "", "", "",
/* E */ "",       "",       "",       "",       "",       "",       "",          "",       "",      "",     "",     "",     "",       "", "", "",
/* F */ "",       "",       "",       "",       "",       "",       "",          "",       "",      "",     "",     "",     "",       "", "", "",
};

int ExtFormatStringOperandCount[0x100] =
{
/*      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
/* 0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 1 */ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 2 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 3 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 4 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 5 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 6 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 7 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 8 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 9 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* A */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* B */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* C */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* D */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* E */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* F */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

int ExtFormatStringOperandSize[0x100] =
{
/*      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
/* 0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 1 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 2 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 3 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 4 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 5 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 6 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 7 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 8 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 9 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* A */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* B */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* C */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* D */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* E */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* F */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

uint8_t additionFlags8(uint8_t a, uint8_t b)
{
	uint8_t flags = 0;

	// half carry
	if (((a & 0xF) + (b & 0xF)) & 0x10)
	{
		flags |= 0x20;
	}

	// carry
	if ((uint16_t(a) + uint16_t(b)) & 0x100)
	{
		flags |= 0x10;
	}

 	// zero
	if ((a + b) == 0)
	{
		flags |= 0x80;
	}

	return flags;
}

uint8_t subtractionFlags8(uint8_t a, uint8_t b)
{
	uint8_t flags = 0x40;

	// half carry
	if (((a & 0xF) - (b & 0xF)) & 0x10)
	{
		flags |= 0x20;
	}

	// carry
	if ((uint16_t(a) - uint16_t(b)) & 0x100)
	{
		flags |= 0x10;
	}

 	// zero
	if ((a - b) == 0)
	{
		flags |= 0x80;
	}

	return flags;
}

// Generic

void UNIMPL(CPU *cpu, instruction_t &i)
{
	cpu->doBreak();
}

void INVALID(CPU *cpu, instruction_t &i)
{
	cpu->doBreak();
}

static constexpr uint8_t Hi(uint16_t n)
{
	return (n & 0xFF00) >> 8;
}

static constexpr Lo(uint16_t n)
{
	return (n & 0xFF);
}

static constexpr void SetHi(uint16_t &r, uint8_t n)
{
	r = (uint16_t(n) << 8) + Lo(r);
}

static constexpr void SetLo(uint16_t &r, uint8_t n)
{
	r = (r & 0xFF00) + n;
}

uint8_t RLC_R(CPU *cpu, uint8_t r)
{
	// Z00C
	uint8_t bit7 = !!(r & 0b1000'0000);
	uint8_t result = (r << 1) | bit7;
	SetLo(cpu->af, (bit7 << 4) | (!result ? 0x80 : 0x0));
	return result;
}

void RLC_RH(CPU *cpu, uint16_t &r)
{
	SetHi(r, RLC_R(cpu, Hi(r)));
}

void RLC_RL(CPU *cpu, uint16_t &r)
{
	SetLo(r, RLC_R(cpu, Lo(r)));
}

uint8_t RRC_R(CPU *cpu, uint8_t r)
{
	// Z00C
	uint8_t bit0 = !!(r & 0b0000'0000);
	uint8_t result = (r >> 1) | bit0;
	SetLo(cpu->af, (bit0 << 4) | (!result ? 0x80 : 0x0));
	return result;
}

void RRC_RH(CPU *cpu, uint16_t &r)
{
	SetHi(r, RRC_R(cpu, Hi(r)));
}

void RRC_RL(CPU *cpu, uint16_t &r)
{
	SetLo(r, RRC_R(cpu, Lo(r)));
}

void RST(CPU *cpu, uint16_t addr)
{
	cpu->sp -= 2;
	cpu->mmu->write16(cpu->sp, cpu->pc);
	cpu->pc = addr;
}

uint16_t ADD16(uint16_t a, uint16_t b, uint8_t &flags)
{
	uint8_t  sum4  = (a & 0xF) + (b & 0xF);
	uint16_t sum16 = a + b;

	uint8_t f = 0;
	if (sum16 > 0xFF)
	{
		f |= 0x10;
	}
	if (sum4 > 0xF)
	{
		f |= 0x20;
	}
	if (sum16 == 0)
	{
		f |= 0x80;
	}
	flags = f;

	return sum16;
}

uint8_t ADD8(uint8_t a, uint8_t b, uint8_t &flags)
{
	uint8_t  sum4  = (a & 0xF) + (b & 0xF);
	uint8_t  sum8  = a + b;
	uint16_t sum16 = uint16_t(a) + uint16_t(b);

	uint8_t f = 0;
	if (sum16 > 0xFF)
	{
		f |= 0x10;
	}
	if (sum4 > 0xF)
	{
		f |= 0x20;
	}
	if (sum8 == 0)
	{
		f |= 0x80;
	}
	flags = f;

	return sum8;
}

uint8_t SUB8(uint8_t a, uint8_t b, uint8_t &flags)
{
	// Z1HC
	uint8_t dif8 = a - b;

	uint8_t f = 0x40;
	if (b > a)
	{
		f |= 0x10;
	}
	if ((b & 0xF) > (a & 0xF))
	{
		f |= 0x20;
	}
	if (dif8 == 0x00)
	{
		f |= 0x80;
	}
	flags = f;

	return dif8;
}

void SWAP_RH(CPU *cpu, uint16_t &r)
{
	uint8_t n = Hi(r);
	uint8_t hi = (n & 0xF0) >> 4;
	uint8_t lo = (n & 0x0F);
	uint8_t result = (lo << 4) | hi;
	SetHi(r, result);
	SetLo(cpu->af, (!result) ? 0x80 : 0x0);
}

void SWAP_RL(CPU *cpu, uint16_t &r)
{
	uint8_t n = Lo(r);
	uint8_t hi = (n & 0xF0) >> 4;
	uint8_t lo = (n & 0x0F);
	uint8_t result = (lo << 4) | hi;
	SetLo(r, result);
	SetLo(cpu->af, (!result) ? 0x80 : 0x0);
}

void SRL_RH(CPU *cpu, uint16_t &r)
{
	// Z00C
	uint8_t carry = r & 0x1;
	uint8_t result = Hi(r) >> 1;
	uint8_t flags = 0;
	if (result == 0)
	{
		flags |= 0x80;
	}
	if (carry)
	{
		flags |= 0x10;
	}
	SetHi(r, result);
	SetLo(cpu->af, flags);
}

void SRL_RL(CPU *cpu, uint16_t &r)
{
	// Z00C
	uint8_t carry = r & 0x1;
	uint8_t result = Lo(r) >> 1;
	uint8_t flags = 0;
	if (result == 0)
	{
		flags |= 0x80;
	}
	if (carry)
	{
		flags |= 0x10;
	}
	SetLo(r, result);
	SetLo(cpu->af, flags);
}

void CP_A_R(CPU *cpu, uint8_t r)
{
	// Z1HC
	uint8_t flags;
	uint8_t result = SUB8(Hi(cpu->af), r, flags);
	SetLo(cpu->af, flags);
}

void XOR_A_R(CPU *cpu, uint8_t r)
{
	SetHi(cpu->af, Hi(cpu->af ^ r));
	SetLo(cpu->af, Hi(cpu->af) == 0 ? 0x80 : 0);
}

void AND_A_R(CPU *cpu, uint8_t r)
{
	// Z010
	SetHi(cpu->af, Hi(cpu->af) & r);
	SetLo(cpu->af, Hi(cpu->af) ? 0xA0 : 0x20);
}

void OR_A_R(CPU *cpu, uint8_t b)
{
	// Z000
	uint8_t result = Hi(cpu->af) | b;
	if (result == 0)
	{
		SetLo(cpu->af, 0x80);
	}
	else
	{
		SetLo(cpu->af, 0x00);
	}
	SetHi(cpu->af, result);
}

void LD_REG16_IMM16(uint16_t &reg, uint16_t val)
{
	reg = val;
}

void LDD_REG16_REG8(CPU *cpu, uint16_t &reg, uint8_t val)
{
	cpu->mmu->write8(reg, val);
	reg--;
}

void LDI_REG16_REG8(CPU *cpu, uint16_t &reg, uint8_t val)
{
	cpu->mmu->write8(reg, val);
	reg++;
}

void LD_REG8_HI_AREG16(CPU *cpu, uint16_t &a, uint16_t &b)
{
	uint16_t val = cpu->mmu->read8(b);
	a = (val << 8) | (a & 0x00FF);
}

void LD_REG8_LO_REG8_HI(uint16_t &a, uint16_t &b)
{
	SetLo(a, Hi(b));
}

void LD_REG8_LO_REG8_LO(uint16_t &a, uint16_t &b)
{
	SetLo(a, Lo(b));
}

void LD_REG8_HI_REG8_HI(uint16_t &a, uint16_t &b)
{
	SetHi(a, Hi(b));
}

void LD_REG8_HI_REG8_LO(uint16_t &a, uint16_t &b)
{
	SetHi(a, Lo(b));
}

void LD_REG8_HI_IMM8(uint16_t &r, uint8_t val)
{
	SetHi(r, val);
}

void LD_REG8_LO_IMM8(uint16_t &r, uint8_t val)
{
	SetLo(r, val);
}

uint8_t RR_REG8_NONPREFIX(CPU *cpu, uint8_t r)
{
	// 000C
	uint8_t carry = (cpu->af & 0x10) >> 4;
	uint8_t newCarry = r & 0x1;
	r >>= 1;
	r |= (carry << 7);
	uint8_t f = 0;
	if (r & 0x80)
	{
		f |= (newCarry << 4);
	}
	SetLo(cpu->af, f);
	return r;
}

uint8_t RR_REG8(CPU *cpu, uint8_t r)
{
	// Z00C
	uint8_t carry = (cpu->af & 0x10) >> 4;
	uint8_t newCarry = r & 0x1;
	r >>= 1;
	r |= (carry << 7);
	uint8_t f = 0;
	if (!r)
	{
		f |= 0x80;
	}
	if (r & 0x80)
	{
		f |= (newCarry << 4);
	}
	SetLo(cpu->af, f);
	return r;
}

uint8_t RL_REG8(CPU *cpu, uint8_t r)
{
	// Z00C
	uint8_t carry = (cpu->af & 0x10) >> 4;
	uint8_t newCarry = (r & 0x80) >> 7;
	r <<= 1;
	r += carry;
	uint8_t f = 0;
	if (!r)
	{
		f |= 0x80;
	}
	if (r & 0x80)
	{
		f |= (newCarry << 4);
	}
	SetLo(cpu->af, f);
	return r;
}

uint8_t SLA_REG8(CPU *cpu, uint8_t r)
{
	// Z00C
	uint8_t carry = (cpu->af & 0x10) >> 4;
	uint8_t newCarry = (r & 0x80) >> 7;
	r <<= 1;
	uint8_t f = 0;
	if (!r)
	{
		f |= 0x80;
	}
	if (r & 0x80)
	{
		f |= (newCarry << 4);
	}
	SetLo(cpu->af, f);
	return r;
}

uint8_t SRA_REG8(CPU *cpu, uint8_t r)
{
	// Z00C
	uint8_t carry = (cpu->af & 0x10) >> 4;
	uint8_t newCarry = r & 0x1;
	r >>= 1;
	uint8_t f = 0;
	if (!r)
	{
		f |= 0x80;
	}
	if (r & 0x80)
	{
		f |= (newCarry << 4);
	}
	SetLo(cpu->af, f);
	return r;
}

void RR_REG8_HI_NONPREFIX(CPU *cpu, uint16_t &r)
{
	LD_REG8_HI_IMM8(r, RR_REG8_NONPREFIX(cpu, Hi(r)));
}

void RR_REG8_HI(CPU *cpu, uint16_t &r)
{
	LD_REG8_HI_IMM8(r, RR_REG8(cpu, Hi(r)));
}

void RR_REG8_LO(CPU *cpu, uint16_t &r)
{
	LD_REG8_LO_IMM8(r, RR_REG8(cpu, Lo(r)));
}

void RL_REG8_HI(CPU *cpu, uint16_t &r)
{
	LD_REG8_HI_IMM8(r, RL_REG8(cpu, Hi(r)));
}

void RL_REG8_LO(CPU *cpu, uint16_t &r)
{
	LD_REG8_LO_IMM8(r, RL_REG8(cpu, Lo(r)));
}

void ADD_A_R(CPU *cpu, uint8_t r)
{
	// Z0HC
	uint8_t flags;
	uint8_t sum = ADD8(Hi(cpu->af), r, flags);
	SetHi(cpu->af, sum);
	SetLo(cpu->af, flags);
}

void ADC_A_R(CPU *cpu, uint8_t r)
{
	// Z0HC
	uint8_t flagsA;
	uint8_t result = ADD8(Hi(cpu->af), r, flagsA);
	uint8_t flagsB;
	result = ADD8(result, (cpu->af & 0x10) >> 4, flagsB);
	SetLo(cpu->af, flagsA | flagsB);
	SetHi(cpu->af, result);
}

void SUB_A_R(CPU *cpu, uint8_t r)
{
	// Z1HC
	uint8_t flags;
	uint8_t result = SUB8(Hi(cpu->af), r, flags);
	SetHi(cpu->af, result);
	SetLo(cpu->af, flags);
}

void SBC_A_R(CPU *cpu, uint8_t r)
{
	// Z1HC
	uint8_t flagsA;
	uint8_t result = SUB8(Hi(cpu->af), r, flagsA);
	uint8_t flagsB;
	result = SUB8(result, (cpu->af & 0x10) >> 4, flagsB);
	SetLo(cpu->af, flagsA | flagsB);
	SetHi(cpu->af, result);
}

// Merges flags according to mask
void SetFlags(uint16_t &af, uint8_t flags, uint8_t mask)
{
	uint8_t f = Lo(af);
	SetLo(af, f ^ ((f ^ flags) & mask));
}

void INC_REG8_HI(CPU *cpu, uint16_t &r)
{
	// Z0H-
	uint8_t n = Hi(r);
	uint8_t flags;
	n = ADD8(n, 1, flags);
	SetFlags(cpu->af, flags, 0b1110'0000);
	SetHi(r, n);
}

void INC_REG8_LO(CPU *cpu, uint16_t &r)
{
	// Z0H-
	uint8_t n = Lo(r);
	uint8_t flags;
	n = ADD8(n, 1, flags);
	SetFlags(cpu->af, flags, 0b1110'0000);
	SetLo(r, n);
}

void DEC_REG8_HI(CPU *cpu, uint16_t &r)
{
	// Z1H-
	uint8_t n = Hi(r);
	uint8_t flags;
	n = SUB8(n, 1, flags);
	SetFlags(cpu->af, flags, 0b1110'0000);
	SetHi(r, n);
}

void DEC_REG8_LO(CPU *cpu, uint16_t &r)
{
	// Z1H-
	uint8_t n = Lo(r);
	uint8_t flags;
	n = SUB8(n, 1, flags);
	SetFlags(cpu->af, flags, 0b1110'0000);
	SetLo(r, n);
}

// 0x00

void NOP(CPU *cpu, instruction_t &i)
{
}

void LD_BC_IMM16(CPU *cpu, instruction_t &i)
{
	LD_REG16_IMM16(cpu->bc, i.op16);
}

void LD_ABC_A(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(cpu->bc, Hi(cpu->af));
}

void INC_BC(CPU *cpu, instruction_t &i)
{
	cpu->bc++;
}

void INC_B(CPU *cpu, instruction_t &i)
{
	INC_REG8_HI(cpu, cpu->bc);
}

void DEC_B(CPU *cpu, instruction_t &i)
{
	DEC_REG8_HI(cpu, cpu->bc);
}

void LD_B_IMM8(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_IMM8(cpu->bc, i.op8);
}

/*
rlca - Rotates a to the left with bit 7 being moved to bit 0 and also stored into the carry.
0 0 0 C
*/
void RLCA(CPU *cpu, instruction_t &i)
{
	uint8_t bit7 = !!(cpu->af & 0b1000'0000'0000'0000);
	SetLo(cpu->af, bit7 << 4);
	SetHi(cpu->af, (Hi(cpu->af) << 1) | bit7);
}

void LD_AIMM16_SP(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write16(i.op16, cpu->sp);
}

void ADD_HL_BC(CPU *cpu, instruction_t &i)
{
	// -0HC
	uint8_t flags;
	cpu->hl = ADD16(cpu->hl, cpu->bc, flags);
	SetFlags(cpu->af, flags, 0b0111'0000);	
}

void LD_A_ABC(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, cpu->mmu->read8(cpu->bc));
}

void DEC_BC(CPU *cpu, instruction_t &i)
{
	cpu->bc--;
}

void INC_C(CPU *cpu, instruction_t &i)
{
	INC_REG8_LO(cpu, cpu->bc);
}

void DEC_C(CPU *cpu, instruction_t &i)
{
	DEC_REG8_LO(cpu, cpu->bc);
}

void LD_C_IMM8(CPU *cpu, instruction_t &i)
{
	cpu->bc = (cpu->bc & 0xFF00) + i.op8;
}

/*
rrca - Rotates arg1 to the right with bit 0 moved to bit 7 and also stored into the carry.
0 0 0 C
*/
void RRCA(CPU *cpu, instruction_t &i)
{
	uint8_t bit0 = !!(cpu->af & 0b0000'0001'0000'0000);
	SetLo(cpu->af, bit0 << 4);
	SetHi(cpu->af, ((Hi(cpu->af) >> 1) & 0b0111'1111) | (bit0 << 7));
}

// 0x10

void STOP(CPU *cpu, instruction_t &i)
{
}

void LD_DE_IMM16(CPU *cpu, instruction_t &i)
{
	LD_REG16_IMM16(cpu->de, i.op16);
}

void LD_ADE_A(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(cpu->de, Hi(cpu->af));
}

void INC_DE(CPU *cpu, instruction_t &i)
{
	cpu->de++;
}

void INC_D(CPU *cpu, instruction_t &i)
{
	INC_REG8_HI(cpu, cpu->de);
}

void DEC_D(CPU *cpu, instruction_t &i)
{
	DEC_REG8_HI(cpu, cpu->de);
}

void LD_D_IMM8(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_IMM8(cpu->de, i.op8);
}

void RL_A(CPU *cpu, instruction_t &i)
{
	RL_REG8_HI(cpu, cpu->af);
}

void JR_IMM8(CPU *cpu, instruction_t &i)
{
	cpu->pc += int8_t(i.op8);
}

void ADD_HL_DE(CPU *cpu, instruction_t &i)
{
	// -0HC
	uint8_t flags;
	cpu->hl = ADD16(cpu->hl, cpu->de, flags);
	SetFlags(cpu->af, flags, 0b0111'0000);	
}

void LD_A_ADE(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_AREG16(cpu, cpu->af, cpu->de);
}

void DEC_DE(CPU *cpu, instruction_t &i)
{
	cpu->de--;
}

void INC_E(CPU *cpu, instruction_t &i)
{
	INC_REG8_LO(cpu, cpu->de);
}

void DEC_E(CPU *cpu, instruction_t &i)
{
	DEC_REG8_LO(cpu, cpu->de);
}

void LD_E_IMM8(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_IMM8(cpu->de, i.op8);
}

void RRA(CPU *cpu, instruction_t &i)
{
	RR_REG8_HI_NONPREFIX(cpu, cpu->af);
}

// 0x20

void JR_NZ_IMM8(CPU *cpu, instruction_t &i)
{
	if (!(cpu->af & 0x80))
	{
		cpu->pc += int8_t(i.op8);
		i.didAction = true;
	}
}

void LD_HL_IMM16(CPU *cpu, instruction_t &i)
{
	LD_REG16_IMM16(cpu->hl, i.op16);
}

void LDI_HL_A(CPU *cpu, instruction_t &i)
{
	LDI_REG16_REG8(cpu, cpu->hl, (cpu->af & 0xFF00) >> 8);
}

void INC_H(CPU *cpu, instruction_t &i)
{
	INC_REG8_HI(cpu, cpu->hl);
}

void DEC_H(CPU *cpu, instruction_t &i)
{
	DEC_REG8_HI(cpu, cpu->hl);
}

void LD_H_IMM8(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_IMM8(cpu->hl, i.op8);
}

void DAA(CPU *cpu, instruction_t &i)
{
	int tmp = Hi(cpu->af);

	uint8_t f = Lo(cpu->af);

	if ( ! ( f & 0x40 ) ) {
		if ( ( f & 0x20 ) || ( tmp & 0x0F ) > 9 )
			tmp += 6;
		if ( ( f & 0x10 ) || tmp > 0x9F )
			tmp += 0x60;
	} else {
		if ( f & 0x20 ) {
			tmp -= 6;
			if ( ! ( f & 0x10 ) )
				tmp &= 0xFF;
		}
		if ( f & 0x10 )
			tmp -= 0x60;
	}
	f &= 0b0101'0000;
	if ( tmp & 0x100 )
		f |= 0x10;
	if ( ! (tmp & 0xFF) )
		f |= 0x80;
	SetHi(cpu->af, tmp & 0xFF);
	SetLo(cpu->af, f);
}

void JR_Z_IMM8(CPU *cpu, instruction_t &i)
{
	if (cpu->af & 0x80)
	{
		cpu->pc += int8_t(i.op8);
		i.didAction = true;
	}
}

void INC_HL(CPU *cpu, instruction_t &i)
{
	cpu->hl++;
}

void ADD_HL_HL(CPU *cpu, instruction_t &i)
{
	// -0HC
	uint8_t flags;
	cpu->hl = ADD16(cpu->hl, cpu->hl, flags);
	SetFlags(cpu->af, flags, 0b0111'0000);
}

void LDI_A_HL(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, cpu->mmu->read8(cpu->hl++));
}

void DEC_HL(CPU *cpu, instruction_t &i)
{
	cpu->hl--;
}

void INC_L(CPU *cpu, instruction_t &i)
{
	INC_REG8_LO(cpu, cpu->hl);
}

void DEC_L(CPU *cpu, instruction_t &i)
{
	DEC_REG8_LO(cpu, cpu->hl);
}

void LD_L_IMM8(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_IMM8(cpu->hl, i.op8);
}

void CPL(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, ~Hi(cpu->af));
	SetFlags(cpu->af, 0b0110'0000, 0b0110'0000);
}

// 0x30

void JR_NC_IMM8(CPU *cpu, instruction_t &i)
{
	if (!(cpu->af & 0x10))
	{
		cpu->pc += int8_t(i.op8);
		i.didAction = true;
	}
}

void LD_SP_IMM16(CPU *cpu, instruction_t &i)
{
	LD_REG16_IMM16(cpu->sp, i.op16);
}

void LDD_HL_A(CPU *cpu, instruction_t &i)
{
	LDD_REG16_REG8(cpu, cpu->hl, (cpu->af & 0xFF00) >> 8);
}

void INC_SP(CPU *cpu, instruction_t &i)
{
	cpu->sp++;
}

void INC_AHL(CPU *cpu, instruction_t &i)
{
	// Z0H-
	uint8_t flags;
	uint8_t result = ADD8(cpu->mmu->read8(cpu->hl), 1, flags);
	cpu->mmu->write8(cpu->hl, result);
	SetFlags(cpu->af, flags, 0b1110'0000);
}

void DEC_AHL(CPU *cpu, instruction_t &i)
{
	// Z1H-
	uint8_t flags;
	uint8_t result = SUB8(cpu->mmu->read8(cpu->hl), 1, flags);
	cpu->mmu->write8(cpu->hl, result);
	SetFlags(cpu->af, flags, 0b1110'0000);
}

void LD_AHL_IMM8(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(cpu->hl, i.op8);
}

void SCF(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->af, (Lo(cpu->af) & 0b1000'0000) | 0x10);
}

void JR_C_IMM8(CPU *cpu, instruction_t &i)
{
	if (cpu->af & 0x10)
	{
		cpu->pc += int8_t(i.op8);
		i.didAction = true;
	}
}

void ADD_HL_SP(CPU *cpu, instruction_t &i)
{
	// -0HC
	uint8_t flags;
	cpu->hl = ADD16(cpu->hl, cpu->sp, flags);
	SetFlags(cpu->af, flags, 0b0111'0000);	
}

void LDD_A_HL(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, cpu->mmu->read8(cpu->hl--));
}

void DEC_SP(CPU *cpu, instruction_t &i)
{
	cpu->sp--;
}

void INC_A(CPU *cpu, instruction_t &i)
{
	INC_REG8_HI(cpu, cpu->af);
}

void DEC_A(CPU *cpu, instruction_t &i)
{
	DEC_REG8_HI(cpu, cpu->af);
}

void LD_A_IMM8(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, i.op8);
}

void CCF(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->af, 	Lo(cpu->af) & 0b1000'0000);	
}

// 0x40

void LD_B_B(CPU *cpu, instruction_t &i)
{
	// SetHi(cpu->bc, Hi(cpu->bc));
}

void LD_B_C(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_LO(cpu->bc, cpu->bc);
}

void LD_B_D(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_HI(cpu->bc, cpu->de);
}

void LD_B_E(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_LO(cpu->bc, cpu->de);
}

void LD_B_H(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_HI(cpu->bc, cpu->hl);
}

void LD_B_L(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_LO(cpu->bc, cpu->hl);
}

void LD_B_AHL(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_AREG16(cpu, cpu->bc, cpu->hl);
}

void LD_B_A(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_HI(cpu->bc, cpu->af);
}

void LD_C_B(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_REG8_HI(cpu->bc, cpu->bc);
}

void LD_C_C(CPU *cpu, instruction_t &i)
{
	// LD_REG8_LO_REG8_LO(cpu->bc, cpu->bc);
}

void LD_C_D(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_REG8_HI(cpu->bc, cpu->de);
}

void LD_C_E(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_REG8_LO(cpu->bc, cpu->de);
}

void LD_C_H(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_REG8_HI(cpu->bc, cpu->hl);
}

void LD_C_L(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_REG8_LO(cpu->bc, cpu->hl);
}

void LD_C_AHL(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, cpu->mmu->read8(cpu->hl));
}

void LD_C_A(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_REG8_HI(cpu->bc, cpu->af);
}

// 0x50

void LD_D_B(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_HI(cpu->de, cpu->bc);
}

void LD_D_C(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_LO(cpu->de, cpu->bc);
}

void LD_D_D(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_HI(cpu->de, cpu->de);
}

void LD_D_E(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_LO(cpu->de, cpu->de);
}

void LD_D_H(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_HI(cpu->de, cpu->hl);
}

void LD_D_L(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_LO(cpu->de, cpu->hl);
}

void LD_D_AHL(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, cpu->mmu->read8(cpu->hl));
}

void LD_D_A(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_HI(cpu->de, cpu->af);
}

void LD_E_B(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_REG8_HI(cpu->de, cpu->bc);
}

void LD_E_C(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_REG8_LO(cpu->de, cpu->bc);
}

void LD_E_D(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_REG8_HI(cpu->de, cpu->de);
}

void LD_E_E(CPU *cpu, instruction_t &i)
{
	// LD_REG8_LO_REG8_LO(cpu->de, cpu->de);
}

void LD_E_H(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_REG8_HI(cpu->de, cpu->hl);
}

void LD_E_L(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_REG8_LO(cpu->de, cpu->hl);
}

void LD_E_AHL(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, cpu->mmu->read8(cpu->hl));
}

void LD_E_A(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_REG8_HI(cpu->de, cpu->af);
}

// 0x60

void LD_H_B(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_HI(cpu->hl, cpu->bc);
}

void LD_H_C(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_LO(cpu->hl, cpu->bc);
}

void LD_H_D(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_HI(cpu->hl, cpu->de);
}

void LD_H_E(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_LO(cpu->hl, cpu->de);
}

void LD_H_H(CPU *cpu, instruction_t &i)
{
	// LD_REG8_HI_REG8_HI(cpu->hl, cpu->hl);
}

void LD_H_L(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_LO(cpu->hl, cpu->hl);
}

void LD_H_AHL(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, cpu->mmu->read8(cpu->hl));
}

void LD_H_A(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_HI(cpu->hl, cpu->af);
}

void LD_L_B(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_REG8_HI(cpu->hl, cpu->bc);
}

void LD_L_C(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_REG8_HI(cpu->hl, cpu->bc);
}

void LD_L_D(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_REG8_HI(cpu->hl, cpu->de);
}

void LD_L_E(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_REG8_HI(cpu->hl, cpu->de);
}

void LD_L_H(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_REG8_HI(cpu->hl, cpu->hl);
}

void LD_L_L(CPU *cpu, instruction_t &i)
{
	// LD_REG8_LO_REG8_HI(cpu->hl, cpu->hl);
}

void LD_L_AHL(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, cpu->mmu->read8(cpu->hl));
}

void LD_L_A(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_REG8_HI(cpu->hl, cpu->af);
}

// 0x70

void LD_AHL_B(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(cpu->hl, Hi(cpu->bc));
}

void LD_AHL_C(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(cpu->hl, Lo(cpu->bc));
}

void LD_AHL_D(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(cpu->hl, Hi(cpu->de));
}

void LD_AHL_E(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(cpu->hl, Lo(cpu->de));
}

void LD_AHL_H(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(cpu->hl, Hi(cpu->hl));
}

void LD_AHL_L(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(cpu->hl, Lo(cpu->hl));
}

void HALT(CPU *cpu, instruction_t &i)
{
	cpu->doBreak();
}

void LD_AHL_A(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(cpu->hl, Hi(cpu->af));
}

void LD_A_B(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_HI(cpu->af, cpu->bc);
}

void LD_A_C(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_LO(cpu->af, cpu->bc);
}

void LD_A_D(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_HI(cpu->af, cpu->de);
}

void LD_A_E(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_LO(cpu->af, cpu->de);
}

void LD_A_H(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_HI(cpu->af, cpu->hl);
}

void LD_A_L(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_REG8_LO(cpu->af, cpu->hl);
}

void LD_A_AHL(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, cpu->mmu->read8(cpu->hl));
}

void LD_A_A(CPU *cpu, instruction_t &i)
{
	// LD_REG8_HI_REG8_HI(cpu->af, cpu->af);
}

// 0x80

void ADD_A_B(CPU *cpu, instruction_t &i)
{
	ADD_A_R(cpu, Hi(cpu->bc));
}

void ADD_A_C(CPU *cpu, instruction_t &i)
{
	ADD_A_R(cpu, Lo(cpu->bc));
}

void ADD_A_D(CPU *cpu, instruction_t &i)
{
	ADD_A_R(cpu, Hi(cpu->de));
}

void ADD_A_E(CPU *cpu, instruction_t &i)
{
	ADD_A_R(cpu, Lo(cpu->de));
}

void ADD_A_H(CPU *cpu, instruction_t &i)
{
	ADD_A_R(cpu, Hi(cpu->hl));
}

void ADD_A_L(CPU *cpu, instruction_t &i)
{
	ADD_A_R(cpu, Lo(cpu->hl));
}

void ADD_A_AHL(CPU *cpu, instruction_t &i)
{
	ADD_A_R(cpu, cpu->mmu->read8(cpu->hl));
}

void ADD_A_A(CPU *cpu, instruction_t &i)
{
	ADD_A_R(cpu, Hi(cpu->af));
}

void ADC_A_B(CPU *cpu, instruction_t &i)
{
	ADC_A_R(cpu, Hi(cpu->bc));
}

void ADC_A_C(CPU *cpu, instruction_t &i)
{
	ADC_A_R(cpu, Lo(cpu->bc));
}

void ADC_A_D(CPU *cpu, instruction_t &i)
{
	ADC_A_R(cpu, Hi(cpu->de));
}

void ADC_A_E(CPU *cpu, instruction_t &i)
{
	ADC_A_R(cpu, Lo(cpu->de));
}

void ADC_A_H(CPU *cpu, instruction_t &i)
{
	ADC_A_R(cpu, Hi(cpu->hl));
}

void ADC_A_L(CPU *cpu, instruction_t &i)
{
	ADD_A_R(cpu, Lo(cpu->hl));
}

void ADC_A_AHL(CPU *cpu, instruction_t &i)
{
	ADC_A_R(cpu, cpu->mmu->read8(cpu->hl));
}

void ADC_A_A(CPU *cpu, instruction_t &i)
{
	ADD_A_R(cpu, Lo(cpu->af));
}

// 0x90

void SUB_A_B(CPU *cpu, instruction_t &i)
{
	SUB_A_R(cpu, Hi(cpu->bc));
}

void SUB_A_C(CPU *cpu, instruction_t &i)
{
	SUB_A_R(cpu, Lo(cpu->bc));
}

void SUB_A_D(CPU *cpu, instruction_t &i)
{
	SUB_A_R(cpu, Hi(cpu->de));
}

void SUB_A_E(CPU *cpu, instruction_t &i)
{
	SUB_A_R(cpu, Lo(cpu->de));
}

void SUB_A_H(CPU *cpu, instruction_t &i)
{
	SUB_A_R(cpu, Hi(cpu->hl));
}

void SUB_A_L(CPU *cpu, instruction_t &i)
{
	SUB_A_R(cpu, Lo(cpu->hl));
}

void SUB_A_AHL(CPU *cpu, instruction_t &i)
{
	SUB_A_R(cpu, cpu->mmu->read8(cpu->hl));
}

void SUB_A_A(CPU *cpu, instruction_t &i)
{
	SUB_A_R(cpu, Hi(cpu->af));
}

void SBC_A_B(CPU *cpu, instruction_t &i)
{
	SBC_A_R(cpu, Hi(cpu->bc));
}

void SBC_A_C(CPU *cpu, instruction_t &i)
{
	SBC_A_R(cpu, Lo(cpu->bc));
}

void SBC_A_D(CPU *cpu, instruction_t &i)
{
	SBC_A_R(cpu, Hi(cpu->de));
}

void SBC_A_E(CPU *cpu, instruction_t &i)
{
	SBC_A_R(cpu, Lo(cpu->de));
}

void SBC_A_H(CPU *cpu, instruction_t &i)
{
	SBC_A_R(cpu, Hi(cpu->hl));
}

void SBC_A_L(CPU *cpu, instruction_t &i)
{
	SBC_A_R(cpu, Lo(cpu->hl));
}

void SBC_A_AHL(CPU *cpu, instruction_t &i)
{
	SBC_A_R(cpu, cpu->mmu->read8(cpu->hl));
}

void SBC_A_A(CPU *cpu, instruction_t &i)
{
	SBC_A_R(cpu, Hi(cpu->af));
}

// 0xA0

void AND_A_B(CPU *cpu, instruction_t &i)
{
	AND_A_R(cpu, Hi(cpu->bc));
}

void AND_A_C(CPU *cpu, instruction_t &i)
{
	AND_A_R(cpu, Lo(cpu->bc));
}

void AND_A_D(CPU *cpu, instruction_t &i)
{
	AND_A_R(cpu, Hi(cpu->de));
}

void AND_A_E(CPU *cpu, instruction_t &i)
{
	AND_A_R(cpu, Lo(cpu->de));
}

void AND_A_H(CPU *cpu, instruction_t &i)
{
	AND_A_R(cpu, Hi(cpu->hl));
}

void AND_A_L(CPU *cpu, instruction_t &i)
{
	AND_A_R(cpu, Lo(cpu->hl));
}

void AND_A_AHL(CPU *cpu, instruction_t &i)
{
	AND_A_R(cpu, cpu->mmu->read8(cpu->hl));
}

void AND_A_A(CPU *cpu, instruction_t &i)
{
	AND_A_R(cpu, Hi(cpu->af));
}

void XOR_A_B(CPU *cpu, instruction_t &i)
{
	XOR_A_R(cpu, Hi(cpu->bc));
}

void XOR_A_C(CPU *cpu, instruction_t &i)
{
	XOR_A_R(cpu, Lo(cpu->bc));
}

void XOR_A_D(CPU *cpu, instruction_t &i)
{
	XOR_A_R(cpu, Hi(cpu->de));
}

void XOR_A_E(CPU *cpu, instruction_t &i)
{
	XOR_A_R(cpu, Lo(cpu->de));
}

void XOR_A_H(CPU *cpu, instruction_t &i)
{
	XOR_A_R(cpu, Hi(cpu->hl));
}

void XOR_A_L(CPU *cpu, instruction_t &i)
{
	XOR_A_R(cpu, Lo(cpu->hl));
}

void XOR_A_AHL(CPU *cpu, instruction_t &i)
{
	XOR_A_R(cpu, cpu->mmu->read8(cpu->hl));
}

void XOR_A_A(CPU *cpu, instruction_t &i)
{
	XOR_A_R(cpu, Hi(cpu->af));
}

// 0xB0

void OR_A_B(CPU *cpu, instruction_t &i)
{
	OR_A_R(cpu, Hi(cpu->bc));
}

void OR_A_C(CPU *cpu, instruction_t &i)
{
	OR_A_R(cpu, Lo(cpu->bc));
}

void OR_A_D(CPU *cpu, instruction_t &i)
{
	OR_A_R(cpu, Hi(cpu->de));
}

void OR_A_E(CPU *cpu, instruction_t &i)
{
	OR_A_R(cpu, Lo(cpu->de));
}

void OR_A_H(CPU *cpu, instruction_t &i)
{
	OR_A_R(cpu, Hi(cpu->hl));
}

void OR_A_L(CPU *cpu, instruction_t &i)
{
	OR_A_R(cpu, Lo(cpu->hl));
}

void OR_A_AHL(CPU *cpu, instruction_t &i)
{
	OR_A_R(cpu, cpu->mmu->read8(cpu->hl));
}

void OR_A_A(CPU *cpu, instruction_t &i)
{
	OR_A_R(cpu, Hi(cpu->af));
}

void CP_A_B(CPU *cpu, instruction_t &i)
{
	CP_A_R(cpu, Hi(cpu->bc));
}

void CP_A_C(CPU *cpu, instruction_t &i)
{
	CP_A_R(cpu, Lo(cpu->bc));
}

void CP_A_D(CPU *cpu, instruction_t &i)
{
	CP_A_R(cpu, Hi(cpu->de));
}

void CP_A_E(CPU *cpu, instruction_t &i)
{
	CP_A_R(cpu, Lo(cpu->de));
}

void CP_A_H(CPU *cpu, instruction_t &i)
{
	CP_A_R(cpu, Hi(cpu->hl));
}

void CP_A_L(CPU *cpu, instruction_t &i)
{
	CP_A_R(cpu, Lo(cpu->hl));
}

void CP_A_AHL(CPU *cpu, instruction_t &i)
{
	CP_A_R(cpu, cpu->mmu->read8(cpu->hl));
}

void CP_A_A(CPU *cpu, instruction_t &i)
{
	CP_A_R(cpu, Hi(cpu->af));
}

// 0xC0

void RET_NZ(CPU *cpu, instruction_t &i)
{
	if (!(cpu->af & 0x80))
	{
		cpu->pc = cpu->mmu->read16(cpu->sp);
		cpu->sp += 2;
		i.didAction = true;
	}
}

void POP_BC(CPU *cpu, instruction_t &i)
{
	cpu->bc = cpu->mmu->read16(cpu->sp);
	cpu->sp += 2;
}

void JP_NZ_IMM16(CPU *cpu, instruction_t &i)
{
	if (!(cpu->af & 0x80))
	{
		cpu->pc = i.op16;
		i.didAction = true;
	}
}

void JP_IMM16(CPU *cpu, instruction_t &i)
{
	cpu->pc = i.op16;
}

void CALL_NZ_IMM16(CPU *cpu, instruction_t &i)
{
	if (!(cpu->af & 0x80))
	{
		cpu->sp -= 2;
		cpu->mmu->write16(cpu->sp, cpu->pc);
		cpu->pc = i.op16;
		i.didAction = true;
	}
}

void PUSH_BC(CPU *cpu, instruction_t &i)
{
	cpu->sp -= 2;
	cpu->mmu->write16(cpu->sp, cpu->bc);
}

void ADD_A_IMM8(CPU *cpu, instruction_t &i)
{
	// Z0HC
	uint8_t flags;
	uint8_t sum = ADD8(Hi(cpu->af), i.op8, flags);
	SetHi(cpu->af, sum);
	SetLo(cpu->af, flags);
}

void RST_00(CPU *cpu, instruction_t &i)
{
	RST(cpu, 0x00);
}

void RET_Z(CPU *cpu, instruction_t &i)
{
	if (cpu->af & 0x80)
	{
		cpu->pc = cpu->mmu->read16(cpu->sp);
		cpu->sp += 2;
		i.didAction = true;
	}
}

void RET(CPU *cpu, instruction_t &i)
{
	cpu->pc = cpu->mmu->read16(cpu->sp);
	cpu->sp += 2;
}

void JP_Z_IMM16(CPU *cpu, instruction_t &i)
{
	if (cpu->af & 0x80)
	{
		cpu->pc = i.op16;
		i.didAction = true;
	}
}

void CB(CPU *cpu, instruction_t &i)
{
}

void CALL_Z_IMM16(CPU *cpu, instruction_t &i)
{
	if (cpu->af & 0x80)
	{
		cpu->sp -= 2;
		cpu->mmu->write16(cpu->sp, cpu->pc);
		cpu->pc = i.op16;
		i.didAction = true;
	}
}

void CALL_IMM16(CPU *cpu, instruction_t &i)
{
	cpu->sp -= 2;
	cpu->mmu->write16(cpu->sp, cpu->pc);
	cpu->pc = i.op16;
}

void ADC_A_IMM8(CPU *cpu, instruction_t &i)
{
	ADC_A_R(cpu, i.op8);
}

void RST_08(CPU *cpu, instruction_t &i)
{
	RST(cpu, 0x08);
}

// 0xD0

void RET_NC(CPU *cpu, instruction_t &i)
{
	if (!(cpu->af & 0x10))
	{
		cpu->pc = cpu->mmu->read16(cpu->sp);
		cpu->sp += 2;
		i.didAction = true;
	}
}

void POP_DE(CPU *cpu, instruction_t &i)
{
	cpu->de = cpu->mmu->read16(cpu->sp);
	cpu->sp += 2;
}

void JP_NC_IMM16(CPU *cpu, instruction_t &i)
{
	if (!(cpu->af & 0x10))
	{
		cpu->pc = i.op16;
		i.didAction = true;
	}
}

void CALL_NC_IMM16(CPU *cpu, instruction_t &i)
{
	if (!(cpu->af & 0x10))
	{
		cpu->sp -= 2;
		cpu->mmu->write16(cpu->sp, cpu->pc);
		cpu->pc = i.op16;
		i.didAction = true;
	}
}

void PUSH_DE(CPU *cpu, instruction_t &i)
{
	cpu->sp -= 2;
	cpu->mmu->write16(cpu->sp, cpu->de);
}

void RET_C(CPU *cpu, instruction_t &i)
{
	if (cpu->af & 0x10)
	{
		cpu->pc = cpu->mmu->read16(cpu->sp);
		cpu->sp += 2;
		i.didAction = true;
	}
}

void RETI(CPU *cpu, instruction_t &i)
{
	// cpu->ime = false;
	cpu->pc = cpu->mmu->read16(cpu->sp);
	cpu->sp += 2;
}

void JP_C_IMM16(CPU *cpu, instruction_t &i)
{
	if (cpu->af & 0x10)
	{
		cpu->pc = i.op16;
		i.didAction = true;
	}
}

void SUB_A_IMM8(CPU *cpu, instruction_t &i)
{
	// Z1HC
	uint8_t flags;
	uint8_t result = SUB8(Hi(cpu->af), i.op8, flags);
	SetHi(cpu->af, result);
	SetLo(cpu->af, flags);
}

void RST_10(CPU *cpu, instruction_t &i)
{
	RST(cpu, 0x10);
}

void CALL_C_IMM16(CPU *cpu, instruction_t &i)
{
	if (cpu->af & 0x10)
	{
		cpu->sp -= 2;
		cpu->mmu->write16(cpu->sp, cpu->pc);
		cpu->pc = i.op16;
		i.didAction = true;
	}
}

void SBC_A_IMM8(CPU *cpu, instruction_t &i)
{
	SBC_A_R(cpu, i.op8);
}

void RST_18(CPU *cpu, instruction_t &i)
{
	RST(cpu, 0x18);
}

// 0xE0

void LDH_IMM8_A(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(0xFF00 + i.op8, Hi(cpu->af));
}

void POP_HL(CPU *cpu, instruction_t &i)
{
	cpu->hl = cpu->mmu->read16(cpu->sp);
	cpu->sp += 2;
}

void LDH_C_A(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(0xFF00 + Lo(cpu->bc), Hi(cpu->af));
}

void PUSH_HL(CPU *cpu, instruction_t &i)
{
	cpu->sp -= 2;
	cpu->mmu->write16(cpu->sp, cpu->hl);	
}

void AND_A_IMM8(CPU *cpu, instruction_t &i)
{
	AND_A_R(cpu, i.op8);
}

void RST_20(CPU *cpu, instruction_t &i)
{
	RST(cpu, 0x20);
}

void ADD_SP_IMM8(CPU *cpu, instruction_t &i)
{
	// 00HC
	uint8_t flags = 0;
	int8_t b = i.op8;
	uint8_t  sum4  = (cpu->sp & 0xF) + (b & 0xF);
	uint16_t sum16 = cpu->sp + b;

	if (sum16 > 0xFF)
	{
		flags |= 0x10;
	}
	if (sum4 > 0xF)
	{
		flags |= 0x20;
	}
	if (sum16 == 0)
	{
		flags |= 0x80;
	}

	cpu->sp = sum16;
	SetLo(cpu->af, flags & 0b0011'0000);
}

void JP_HL(CPU *cpu, instruction_t &i)
{
	cpu->pc = cpu->hl;
}

void LD_AIMM16_A(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(i.op16, Hi(cpu->af));
}

void XOR_A_IMM8(CPU *cpu, instruction_t &i)
{
	XOR_A_R(cpu, i.op8);
}

void RST_28(CPU *cpu, instruction_t &i)
{
	RST(cpu, 0x28);
}

// 0xF0

void LDH_A_IMM8(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, cpu->mmu->read8(0xFF00 + i.op8));
}

void POP_AF(CPU *cpu, instruction_t &i)
{
	cpu->af = cpu->mmu->read16(cpu->sp) & 0b1111'1111'1111'0000;
	cpu->sp += 2;
}

void LDH_A_C(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, cpu->mmu->read8(0xFF00 + Lo(cpu->bc)));
}

void DI(CPU *cpu, instruction_t &i)
{
	// cpu->ime = false;
}

void PUSH_AF(CPU *cpu, instruction_t &i)
{
	cpu->sp -= 2;
	cpu->mmu->write16(cpu->sp, cpu->af);	
}

void OR_A_IMM8(CPU *cpu, instruction_t &i)
{
	OR_A_R(cpu, i.op8);
}

void RST_30(CPU *cpu, instruction_t &i)
{
	RST(cpu, 0x30);
}

void LD_HL_SPIMM8(CPU *cpu, instruction_t &i)
{
	// 00HC
	uint8_t flagsA = 0;
	int8_t b = i.op8;
	uint8_t  sum4  = (cpu->sp & 0xF) + (b & 0xF);
	uint16_t sum16 = cpu->sp + b;

	if (sum16 > 0xFF)
	{
		flagsA |= 0x10;
	}
	if (sum4 > 0xF)
	{
		flagsA |= 0x20;
	}
	if (sum16 == 0)
	{
		flagsA |= 0x80;
	}

	uint8_t flagsB;
	cpu->hl = ADD16(cpu->hl, sum16, flagsB);
	SetLo(cpu->af, (flagsA | flagsB) & 0b0011'0000);
}

void LD_SP_HL(CPU *cpu, instruction_t &i)
{
	cpu->sp = cpu->hl;
}

void LD_A_AIMM16(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, cpu->mmu->read8(i.op16));
}

void EI(CPU *cpu, instruction_t &i)
{
	// cpu->ime = true;
}

void CP_A_IMM8(CPU *cpu, instruction_t &i)
{
	// Z1HC
	uint8_t flags;
	uint8_t result = SUB8(Hi(cpu->af), i.op8, flags);
	SetLo(cpu->af, flags);
}

void RST_38(CPU *cpu, instruction_t &i)
{
	RST(cpu, 0x38);
}

// 0xCB // Extended

// 0xCB 0x00

/*
rlc arg1 - Rotates arg1 to the left with bit 7 being moved to bit 0 and also stored into the carry. 
Z 0 0 C
*/
void RLC_B(CPU *cpu, instruction_t &i)
{
	RLC_RH(cpu, cpu->bc);
}

void RLC_C(CPU *cpu, instruction_t &i)
{
	RLC_RL(cpu, cpu->bc);	
}

void RLC_D(CPU *cpu, instruction_t &i)
{
	RLC_RH(cpu, cpu->de);
}

void RLC_E(CPU *cpu, instruction_t &i)
{
	RLC_RL(cpu, cpu->de);
}

void RLC_H(CPU *cpu, instruction_t &i)
{
	RLC_RH(cpu, cpu->hl);
}

void RLC_L(CPU *cpu, instruction_t &i)
{
	RLC_RL(cpu, cpu->hl);
}

void RLC_AHL(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(cpu->hl, RLC_R(cpu, cpu->mmu->read8(cpu->hl)));
}

void RLC_A(CPU *cpu, instruction_t &i)
{
	RLC_RH(cpu, cpu->af);
}

/*
rrc arg1 - Rotates arg1 to the right with bit 0 moved to bit 7 and also stored into the carry.
Z 0 0 C
*/

void RRC_B(CPU *cpu, instruction_t &i)
{
	RRC_RH(cpu, cpu->bc);
}

void RRC_C(CPU *cpu, instruction_t &i)
{
	RRC_RL(cpu, cpu->bc);	
}

void RRC_D(CPU *cpu, instruction_t &i)
{
	RRC_RH(cpu, cpu->de);
}

void RRC_E(CPU *cpu, instruction_t &i)
{
	RRC_RL(cpu, cpu->de);
}

void RRC_H(CPU *cpu, instruction_t &i)
{
	RRC_RH(cpu, cpu->hl);
}

void RRC_L(CPU *cpu, instruction_t &i)
{
	RRC_RL(cpu, cpu->hl);
}

void RRC_AHL(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(cpu->hl, RRC_R(cpu, cpu->mmu->read8(cpu->hl)));
}

void RRC_A(CPU *cpu, instruction_t &i)
{
	RRC_RH(cpu, cpu->af);
}

// 0xCB 0x10

/*
rlc arg1 - Rotates arg1 to the left with bit 7 being moved to bit 0 and also stored into the carry. 
Z 0 0 C
*/
void RL_B(CPU *cpu, instruction_t &i)
{
	RLC_RH(cpu, cpu->bc);
}

void RL_C(CPU *cpu, instruction_t &i)
{
	RLC_RL(cpu, cpu->bc);	
}

void RL_D(CPU *cpu, instruction_t &i)
{
	RLC_RH(cpu, cpu->de);
}

void RL_E(CPU *cpu, instruction_t &i)
{
	RLC_RL(cpu, cpu->de);
}

void RL_H(CPU *cpu, instruction_t &i)
{
	RLC_RH(cpu, cpu->hl);
}

void RL_L(CPU *cpu, instruction_t &i)
{
	RLC_RL(cpu, cpu->hl);
}

void RL_AHL(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(cpu->hl, RLC_R(cpu, cpu->mmu->read8(cpu->hl)));
}

/*
rrc arg1 - Rotates arg1 to the right with bit 0 moved to bit 7 and also stored into the carry.
Z 0 0 C
*/

void RR_B(CPU *cpu, instruction_t &i)
{
	RRC_RH(cpu, cpu->bc);
}

void RR_C(CPU *cpu, instruction_t &i)
{
	RRC_RL(cpu, cpu->bc);	
}

void RR_D(CPU *cpu, instruction_t &i)
{
	RRC_RH(cpu, cpu->de);
}

void RR_E(CPU *cpu, instruction_t &i)
{
	RRC_RL(cpu, cpu->de);
}

void RR_H(CPU *cpu, instruction_t &i)
{
	RRC_RH(cpu, cpu->hl);
}

void RR_L(CPU *cpu, instruction_t &i)
{
	RRC_RL(cpu, cpu->hl);
}

void RR_AHL(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(cpu->hl, RRC_R(cpu, cpu->mmu->read8(cpu->hl)));
}

void RR_A(CPU *cpu, instruction_t &i)
{
	RRC_RH(cpu, cpu->af);
}

// 0xCB 0x20

void SLA_B(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, SLA_REG8(cpu, Hi(cpu->bc)));
}

void SLA_C(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, SLA_REG8(cpu, Lo(cpu->bc)));
}

void SLA_D(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, SLA_REG8(cpu, Hi(cpu->de)));
}

void SLA_E(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, SLA_REG8(cpu, Lo(cpu->de)));
}

void SLA_H(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, SLA_REG8(cpu, Hi(cpu->hl)));
}

void SLA_L(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, SLA_REG8(cpu, Lo(cpu->hl)));
}

void SLA_AHL(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(cpu->hl, SLA_REG8(cpu, cpu->mmu->read8(cpu->hl)));
}

void SLA_A(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, SLA_REG8(cpu, Hi(cpu->af)));
}

void SRA_B(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, SRA_REG8(cpu, Hi(cpu->bc)));
}

void SRA_C(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, SRA_REG8(cpu, Lo(cpu->bc)));
}

void SRA_D(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, SRA_REG8(cpu, Hi(cpu->de)));
}

void SRA_E(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, SRA_REG8(cpu, Lo(cpu->de)));
}

void SRA_H(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, SRA_REG8(cpu, Hi(cpu->hl)));
}

void SRA_L(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, SRA_REG8(cpu, Lo(cpu->hl)));
}

void SRA_AHL(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(cpu->hl, SRA_REG8(cpu, cpu->mmu->read8(cpu->hl)));
}

void SRA_A(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, SRA_REG8(cpu, Hi(cpu->af)));
}

// 0xCB 0x30

void SWAP_B(CPU *cpu, instruction_t &i)
{
	SWAP_RH(cpu, cpu->bc);
}

void SWAP_C(CPU *cpu, instruction_t &i)
{
	SWAP_RL(cpu, cpu->bc);
}

void SWAP_D(CPU *cpu, instruction_t &i)
{
	SWAP_RH(cpu, cpu->de);
}

void SWAP_E(CPU *cpu, instruction_t &i)
{
	SWAP_RL(cpu, cpu->de);
}

void SWAP_H(CPU *cpu, instruction_t &i)
{
	SWAP_RH(cpu, cpu->hl);
}

void SWAP_L(CPU *cpu, instruction_t &i)
{
	SWAP_RL(cpu, cpu->hl);
}

void SWAP_AHL(CPU *cpu, instruction_t &i)
{
	uint16_t b = cpu->mmu->read8(cpu->hl);
	SWAP_RL(cpu, b);
	cpu->mmu->write8(cpu->hl, b & 0xFF);
}

void SWAP_A(CPU *cpu, instruction_t &i)
{
	SWAP_RH(cpu, cpu->af);
}

void SRL_B(CPU *cpu, instruction_t &i)
{
	SRL_RH(cpu, cpu->bc);
}

void SRL_C(CPU *cpu, instruction_t &i)
{
	SRL_RL(cpu, cpu->bc);
}

void SRL_D(CPU *cpu, instruction_t &i)
{
	SRL_RH(cpu, cpu->de);
}

void SRL_E(CPU *cpu, instruction_t &i)
{
	SRL_RL(cpu, cpu->de);
}

void SRL_H(CPU *cpu, instruction_t &i)
{
	SRL_RH(cpu, cpu->hl);
}

void SRL_L(CPU *cpu, instruction_t &i)
{
	SRL_RL(cpu, cpu->hl);
}

void SRL_AHL(CPU *cpu, instruction_t &i)
{
	uint16_t b = cpu->mmu->read8(cpu->hl);
	SRL_RH(cpu, b);
	cpu->mmu->write8(cpu->hl, b & 0xFF);
}

void SRL_A(CPU *cpu, instruction_t &i)
{
	SRL_RH(cpu, cpu->af);
}

// 0xCB 0x70

void BIT_7H(CPU *cpu, instruction_t &i)
{
	// Z01-
	uint8_t bit = (Hi(cpu->hl) & 0x80) >> 7;

	uint8_t flags = 0x20;
	if (bit == 0)
	{
		flags |= 0x80;
	}
	SetFlags(cpu->af, flags, 0b1110'0000);
}
