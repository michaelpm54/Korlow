#include <cinttypes>
#include <cstdio>
#include "cpu_instructions.h"
#include "cpu.h"
#include "mmu.h"

instructionFunc_t InstructionList[0x100] =
{
/*      0           1            2         3       4       5        6          7         8          9       A            B       C       D           E          F   */
/* 0 */ NOP,        UNIMPL,      UNIMPL,   UNIMPL, INC_B,  DEC_B,   LD_B_IMM8, UNIMPL,   UNIMPL,    UNIMPL, UNIMPL,      UNIMPL, INC_C,  DEC_C ,     LD_C_IMM8, UNIMPL,
/* 1 */ UNIMPL,     LD_DE_IMM16, UNIMPL,   INC_DE, UNIMPL, DEC_D,   LD_D_IMM8, RL_A,     JR_IMM8,   UNIMPL, LD_A_ADE,    UNIMPL, UNIMPL, DEC_E,      LD_E_IMM8, UNIMPL,
/* 2 */ JR_NZ_IMM8, LD_HL_IMM16, LDI_HL_A, INC_HL, INC_H,  UNIMPL,  UNIMPL,    UNIMPL,   JR_Z_IMM8, UNIMPL, UNIMPL,      UNIMPL, UNIMPL, UNIMPL,     LD_L_IMM8, UNIMPL,
/* 3 */ UNIMPL,     LD_SP_IMM16, LDD_HL_A, UNIMPL, UNIMPL, UNIMPL,  UNIMPL,    UNIMPL,   UNIMPL,    UNIMPL, UNIMPL,      UNIMPL, UNIMPL, DEC_A,      LD_A_IMM8, UNIMPL,
/* 4 */ LD_B_B,     LD_B_C,      LD_B_D,   LD_B_E, LD_B_H, LD_B_L,  UNIMPL,    LD_B_A,   LD_C_B,    LD_C_C, LD_C_D,      LD_C_E, LD_C_H, LD_C_L,     UNIMPL,    LD_C_A,
/* 5 */ LD_D_B,     LD_D_C,      LD_D_D,   LD_D_E, LD_D_H, LD_D_L,  UNIMPL,    LD_D_A,   LD_E_B,    LD_E_C, LD_E_D,      LD_E_E, LD_E_H, LD_H_L,     UNIMPL,    LD_E_A,
/* 6 */ LD_H_B,     LD_H_C,      LD_H_D,   LD_H_E, LD_H_H, LD_H_L,  UNIMPL,    LD_H_A,   LD_L_B,    LD_L_C, LD_L_D,      LD_L_E, LD_L_H, LD_L_L,     UNIMPL,    LD_L_A,
/* 7 */ UNIMPL,     UNIMPL,      UNIMPL,   UNIMPL, UNIMPL, UNIMPL,  UNIMPL,    LD_AHL_A, LD_A_B,    LD_A_C, LD_A_D,      LD_A_E, LD_A_H, LD_A_L,     UNIMPL,    LD_A_A,
/* 8 */ UNIMPL,     UNIMPL,      UNIMPL,   UNIMPL, UNIMPL, UNIMPL,  ADD_A_AHL, UNIMPL,   UNIMPL,    UNIMPL, UNIMPL,      UNIMPL, UNIMPL, UNIMPL,     UNIMPL,    UNIMPL,
/* 9 */ SUB_A_B,    UNIMPL,      UNIMPL,   UNIMPL, UNIMPL, UNIMPL,  UNIMPL,    UNIMPL,   UNIMPL,    UNIMPL, UNIMPL,      UNIMPL, UNIMPL, UNIMPL,     UNIMPL,    UNIMPL,
/* A */ UNIMPL,     UNIMPL,      UNIMPL,   UNIMPL, UNIMPL, UNIMPL,  UNIMPL,    UNIMPL,   UNIMPL,    UNIMPL, UNIMPL,      UNIMPL, UNIMPL, UNIMPL,     UNIMPL,    XOR_A_A,
/* B */ UNIMPL,     UNIMPL,      UNIMPL,   UNIMPL, UNIMPL, UNIMPL,  UNIMPL,    UNIMPL,   UNIMPL,    UNIMPL, UNIMPL,      UNIMPL, UNIMPL, UNIMPL,     CP_A_AHL,  UNIMPL,
/* C */ UNIMPL,     POP_BC,      UNIMPL,   UNIMPL, UNIMPL, PUSH_BC, UNIMPL,    UNIMPL,   UNIMPL,    RET,    UNIMPL,      UNIMPL, CB,     CALL_IMM16, UNIMPL,    UNIMPL,
/* D */ UNIMPL,     UNIMPL,      UNIMPL,   UNIMPL, UNIMPL, UNIMPL,  UNIMPL,    UNIMPL,   UNIMPL,    UNIMPL, UNIMPL,      UNIMPL, UNIMPL, UNIMPL,     UNIMPL,    UNIMPL,
/* E */ LDH_IMM8_A, UNIMPL,      LDH_C_A,  UNIMPL, UNIMPL, UNIMPL,  UNIMPL,    UNIMPL,   UNIMPL,    UNIMPL, LD_AIMM16_A, UNIMPL, UNIMPL, UNIMPL,     UNIMPL,    UNIMPL,
/* F */ LDH_A_IMM8, UNIMPL,      UNIMPL,   UNIMPL, UNIMPL, UNIMPL,  UNIMPL,    UNIMPL,   UNIMPL,    UNIMPL, UNIMPL,      EI,     UNIMPL, UNIMPL,     CP_A_IMM8, UNIMPL,
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
/*      0       1       2       3       4       5       6       7       8       9       A       B       C       D       E       F   */
/* 0 */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,
/* 1 */ UNIMPL, RL_C,   UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,
/* 2 */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,
/* 3 */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,
/* 4 */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,
/* 5 */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,
/* 6 */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,
/* 7 */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, BIT_7H, UNIMPL, UNIMPL, UNIMPL,
/* 8 */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,
/* 9 */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,
/* A */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,
/* B */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,
/* C */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,
/* D */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,
/* E */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,
/* F */ UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL, UNIMPL,
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

const char *FormatStrings[0x100] =
{
/*      0                   1              2              3          4          5          6              7             8                  9               A               B          C          D            E             F */
/* 0 */ "NOP",              "",            "",            "",        "INC B",   "DEC B",   "LD B, %02X",  "",           "",                "",             "",             "",        "INC C",   "DEC C",     "LD C, %02X", "",
/* 1 */ "",                 "LD DE, %04X", "",            "INC DE",  "",        "DEC D",   "LD D, %02X",  "RL A",       "JR %02X" PRIX8,   "",             "LD A, (DE)",   "",        "",        "DEC E",     "LD E, %02X", "",
/* 2 */ "JR NZ, %02" PRIX8, "LD HL, %04X", "LD (HL+), A", "INC HL",  "INC H",   "",        "",            "",           "JR Z, %02" PRIX8, "",             "",             "",        "",        "",          "LD L, %02X", "",
/* 3 */ "",                 "LD SP, %04X", "LD (HL-), A", "",        "",        "",        "",            "",           "",                "",             "",             "",        "",        "DEC A",     "LD A, %02X", "",
/* 4 */ "LD B, B",          "LD B, C",     "LD B, D",     "LD B, E", "LD B, H", "LD B, L", "",            "LD B, A",    "LD C, B",         "LD C, C",      "LD C, D",      "LD C, E", "LD C, H", "LD C, L",   "",           "LD C, A",
/* 5 */ "LD D, B",          "LD D, C",     "LD D, D",     "LD D, E", "LD D, H", "LD D, L", "",            "LD D, A",    "LD E, B",         "LD E, C",      "LD E, D",      "LD E, E", "LD E, H", "LD E, L",   "",           "LD E, A",
/* 6 */ "LD H, B",          "LD H, C",     "LD H, D",     "LD H, E", "LD H, H", "LD H, L", "",            "LD H, A",    "LD L, B",         "LD L, C",      "LD L, D",      "LD L, E", "LD L, H", "LD L, L",   "",           "LD L, A",
/* 7 */ "",                 "",            "",            "",        "",        "",        "",            "LD (HL), A", "LD A, B",         "LD A, C",      "LD A, D",      "LD A, E", "LD A, H", "LD A, L",   "",           "LD A, A",
/* 8 */ "",                 "",            "",            "",        "",        "",        "ADD A, (HL)", "",           "",                "",             "",             "",        "",        "",          "",           "",
/* 9 */ "SUB A, B",         "",            "",            "",        "",        "",        "",            "",           "",                "",             "",             "",        "",        "",          "",           "",
/* A */ "",                 "",            "",            "",        "",        "",        "",            "",           "",                "",             "",             "",        "",        "",          "",           "XOR A, A",
/* B */ "",                 "",            "",            "",        "",        "",        "",            "",           "",                "",             "",             "",        "",        "",          "CP A, (HL)", "",
/* C */ "",                 "POP BC",      "",            "",        "",        "PUSH BC", "",            "",           "",                "RET",          "",             "",        "",        "CALL %04X", "",           "",
/* D */ "",                 "",            "",            "",        "",        "",        "",            "",           "",                "",             "",             "",        "",        "",          "",           "",
/* E */ "LD (FF%02X), A",   "",            "LD (C), A",   "",        "",        "",        "",            "",           "",                "",             "LD (%04X), A", "",        "",        "",          "",           "",
/* F */ "LD A, (FF%02X)",   "",            "",            "",        "",        "",        "",            "",           "",                "",             "",             "EI",      "",        "",          "CP A, %02X", "",
};     

int FormatStringOperandSize[0x100] =
{
/*      0  1   2  3  4  5  6  7  8  9  A  B  C  D  E  F */
/* 0 */ 0, 0,  0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 8, 0,
/* 1 */ 0, 16, 0, 0, 0, 0, 8, 0, 8, 0, 0, 0, 0, 0, 8, 0,
/* 2 */ 8, 16,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0,
/* 3 */ 0, 16,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0,
/* 4 */ 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 5 */ 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 6 */ 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 7 */ 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 8 */ 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 9 */ 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* A */ 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* B */ 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* C */ 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0,
/* D */ 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* E */ 8, 0,  0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0,
/* F */ 8, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0,
};

const char *ExtFormatStrings[0x100] =
{
/*      0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
/* 0 */ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
/* 1 */ "", "RL C", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
/* 2 */ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
/* 3 */ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
/* 4 */ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
/* 5 */ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
/* 6 */ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
/* 7 */ "", "", "", "", "", "", "", "", "", "", "", "", "BIT 7H", "", "", "",
/* 8 */ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
/* 9 */ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
/* A */ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
/* B */ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
/* C */ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
/* D */ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
/* E */ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
/* F */ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
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

uint8_t RL_REG8(CPU *cpu, uint8_t r)
{
	uint8_t carry = (cpu->af & 0x10) >> 4;
	uint8_t newCarry = (r & 0x80) >> 7;
	r <<= 1;
	r += carry;
	// Z00C
	uint8_t f = 0;
	if (!r)
	{
		f |= 0x80;
	}
	if (r & 0x80)
	{
		f |= (newCarry << 4);
	}
	cpu->af = (cpu->af & 0xFF00) | f;
	return r;
}

void RL_REG8_HI(CPU *cpu, uint16_t &r)
{
	LD_REG8_HI_IMM8(r, RL_REG8(cpu, (r & 0xFF00) >> 8));
}

void RL_REG8_LO(CPU *cpu, uint16_t &r)
{
	LD_REG8_LO_IMM8(r, RL_REG8(cpu, r & 0xFF));
}

uint8_t ADD8(uint8_t a, uint8_t b, uint8_t &flags)
{
	uint8_t  sum4  = (a & 0xF) + (b & 0xF);
	uint8_t  sum8  = a + b;
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
	if (sum8 == 0)
	{
		f |= 0x80;
	}
	flags = f;

	return sum8;
}

uint8_t SUB8(uint8_t a, uint8_t b, uint8_t &flags)
{
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

// 0x10

void LD_DE_IMM16(CPU *cpu, instruction_t &i)
{
	LD_REG16_IMM16(cpu->de, i.op16);
}

void INC_DE(CPU *cpu, instruction_t &i)
{
	cpu->de++;
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

void LD_A_ADE(CPU *cpu, instruction_t &i)
{
	LD_REG8_HI_AREG16(cpu, cpu->af, cpu->de);
}

void DEC_E(CPU *cpu, instruction_t &i)
{
	DEC_REG8_LO(cpu, cpu->de);
}

void LD_E_IMM8(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_IMM8(cpu->de, i.op8);
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

void LD_L_IMM8(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_IMM8(cpu->hl, i.op8);
}

// 0x30

void LD_SP_IMM16(CPU *cpu, instruction_t &i)
{
	LD_REG16_IMM16(cpu->sp, i.op16);
}

void LDD_HL_A(CPU *cpu, instruction_t &i)
{
	LDD_REG16_REG8(cpu, cpu->hl, (cpu->af & 0xFF00) >> 8);
}

void DEC_A(CPU *cpu, instruction_t &i)
{
	DEC_REG8_HI(cpu, cpu->af);
}

void LD_A_IMM8(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, i.op8);
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

void LD_L_A(CPU *cpu, instruction_t &i)
{
	LD_REG8_LO_REG8_HI(cpu->hl, cpu->af);
}

// 0x70

void LD_AHL_A(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(cpu->af, Hi(cpu->af));
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

void LD_A_A(CPU *cpu, instruction_t &i)
{
	// LD_REG8_HI_REG8_HI(cpu->af, cpu->af);
}

// 0x80

void ADD_A_AHL(CPU *cpu, instruction_t &i)
{
	// Z0HC
	uint8_t val = cpu->mmu->read8(cpu->hl);
	uint8_t flags;
	uint8_t sum = ADD8(Hi(cpu->af), val, flags);
	SetHi(cpu->af, sum);
	SetLo(cpu->af, flags);
}

// 0x90

void SUB_A_B(CPU *cpu, instruction_t &i)
{
	uint8_t flags;
	uint8_t result = SUB8(Hi(cpu->af), Hi(cpu->bc), flags);
	SetHi(cpu->af, result);
	SetLo(cpu->af, flags);
}

// 0xA0

void XOR_A_A(CPU *cpu, instruction_t &i)
{
	uint8_t val = cpu->af >> 8;
	val ^= val;
	cpu->af = (val << 8) | ((val == 0) * 0x80);
}

// 0xB0

void CP_A_AHL(CPU *cpu, instruction_t &i)
{
	// Z1HC
	uint8_t flags;
	uint8_t result = SUB8(Hi(cpu->af), cpu->mmu->read8(cpu->hl), flags);
	SetLo(cpu->af, flags);
}

// 0xC0

void POP_BC(CPU *cpu, instruction_t &i)
{
	cpu->bc = cpu->mmu->read16(cpu->sp);
	cpu->sp += 2;
}

void PUSH_BC(CPU *cpu, instruction_t &i)
{
	cpu->sp -= 2;
	cpu->mmu->write16(cpu->sp, cpu->bc);
}

void RET(CPU *cpu, instruction_t &i)
{
	cpu->pc = cpu->mmu->read16(cpu->sp);
	cpu->sp += 2;
}

void CB(CPU *cpu, instruction_t &i)
{
}

void CALL_IMM16(CPU *cpu, instruction_t &i)
{
	cpu->sp -= 2;
	cpu->mmu->write16(cpu->sp, cpu->pc);
	cpu->pc = i.op16;
}

// 0xE0

void LDH_IMM8_A(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(0xFF00 + i.op8, Hi(cpu->af));
}

void LDH_C_A(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(0xFF00 + Lo(cpu->bc), Hi(cpu->af));
}

void LD_AIMM16_A(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(i.op16, Hi(cpu->af));
}

// 0xF0

void LDH_A_IMM8(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, cpu->mmu->read8(0xFF00 + i.op8));
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

// 0xCB // Extended

// 0xCB 0x10

void RL_C(CPU *cpu, instruction_t &i)
{
	RL_REG8_LO(cpu, cpu->bc);
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
