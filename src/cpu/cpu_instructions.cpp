#include "cpu_instructions.h"

#include "cpu.h"
#include "cpu_base.h"
#include "mmu.h"

// Generic

void INVALID(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.set_enabled(false);
}

void RST(Cpu &cpu, Component &mmu, u16 addr)
{
    cpu.sp -= 2;
    mmu.write16(cpu.sp, cpu.pc);
    cpu.pc = addr;
}

void SWAP(u8 &n, u8 &f)
{
    u8 hi = (n & 0xF0) >> 4;
    u8 lo = (n & 0x0F);
    n = (lo << 4) | hi;
    f = (!n) ? 0x80 : 0x0;
}

void SRL(u8 &r, u8 &f)
{
    /* Shift register right into carry. MSB set to 0. */
    // Z00C
    u8 carry = r & 0x1;
    r >>= 1;
    u8 flags = 0;
    if (r == 0) {
        flags |= FLAGS_ZERO;
    }
    if (carry) {
        flags |= FLAGS_CARRY;
    }
    f = flags;
}

void XOR(u8 &a, u8 r, u8 &f)
{
    // Z000
    a ^= r;
    f = a ? 0 : FLAGS_ZERO;
}

void AND(u8 &a, u8 r, u8 &f)
{
    // Z010
    a &= r;
    f = a ? FLAGS_HALFCARRY : (FLAGS_ZERO | FLAGS_HALFCARRY);
}

void OR(u8 &a, u8 r, u8 &f)
{
    // Z000
    a |= r;
    f = a ? 0 : FLAGS_ZERO;
}

void SLA(u8 &r, u8 &f)
{
    // Z00C
    u8 carry = r & 0b1000'0000;
    r <<= 1;
    u8 flags = 0;
    if (!r)
        flags |= FLAGS_ZERO;
    if (carry)
        flags |= FLAGS_CARRY;
    f = flags;
}

void SRA(u8 &r, u8 &f)
{
    // Z00C
    u8 msb = r & 0b1000'0000;
    u8 carry = r & 0b0000'0001;
    r >>= 1;
    r |= msb;
    u8 flags = 0;
    if (!r)
        flags |= FLAGS_ZERO;
    if (carry)
        flags |= FLAGS_CARRY;
    f = flags;
}

void ADC(u8 &a, u8 r, u8 &f)
{
    u8 carry = (f & FLAGS_CARRY) >> 4;
    u8 flags = 0;

    u16 a_, r_, c;
    a_ = a;
    r_ = r;
    c = carry;

    u16 result = a + r_ + c;

    if (result > 0xFF) {
        flags |= FLAGS_CARRY;
    }
    if (((a & 0xF) + (r_ & 0xF) + c) > 0xF) {
        flags |= FLAGS_HALFCARRY;
    }
    if ((result & 0xFF) == 0) {
        flags |= FLAGS_ZERO;
    }

    f = flags;
    a = result & 0xFF;
}

void SBC(u8 &a, u8 r, u8 &f)
{
    u8 carry = (f & FLAGS_CARRY) >> 4;
    u8 flags = FLAGS_SUBTRACT;

    if ((int(a) - int(r) - int(carry)) < 0)
        flags |= FLAGS_CARRY;

    if ((int(a & 0xF) - int(r & 0xF) - int(carry)) < 0)
        flags |= FLAGS_HALFCARRY;

    a -= r;
    a -= carry;

    if (!a)
        flags |= FLAGS_ZERO;

    f = flags;
}

// Merges flags according to mask
void SetFlags(u8 &f, u8 flags, u8 mask)
{
    f = f ^ ((f ^ flags) & mask);
}

// 0x00

void NOP(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
}

void LD_BC_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.bc = d16;
}

void LD_ABC_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    mmu.write8(cpu.bc, cpu.a);
}

void INC_BC(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.bc++;
}

void INC_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    INC8(cpu.b, cpu.f);
}

void DEC_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    DEC8(cpu.b, cpu.f);
}

void LD_B_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b = d8;
}

// x = cpu.a.bit[7]
// carry = x
// cpu.a << 1
// cpu.a |= x
void RLCA(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 bit7 = !!(cpu.af & 0b1000'0000'0000'0000);
    cpu.f = bit7 << 4;
    cpu.a = (cpu.a << 1) | bit7;
}

void LD_AIMM16_SP(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    mmu.write16(d16, cpu.sp);
}

void ADD_HL_BC(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    // -0HC
    u8 flags = 0;
    u16 result = 0;
    ADD16(cpu.hl, cpu.bc, &result, &flags);
    cpu.hl = result;
    flags &= 0b0111'0000;
    cpu.f = (cpu.af & FLAGS_ZERO) | flags;
}

void LD_A_ABC(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = mmu.read8(cpu.bc);
}

void DEC_BC(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.bc--;
}

void INC_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    INC8(cpu.c, cpu.f);
}

void DEC_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    DEC8(cpu.c, cpu.f);
}

void LD_C_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.bc = (cpu.bc & 0xFF00) + d8;
}

void RRCA(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 bit0 = !!(cpu.af & 0b0000'0001'0000'0000);
    cpu.f = bit0 << 4;
    cpu.a = ((cpu.a >> 1) & 0b0111'1111) | (bit0 << 7);
}

// 0x10

void STOP(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
}

void LD_DE_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.de = d16;
}

void LD_ADE_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    mmu.write8(cpu.de, cpu.a);
}

void INC_DE(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.de++;
}

void INC_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    INC8(cpu.d, cpu.f);
}

void DEC_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    DEC8(cpu.d, cpu.f);
}

void LD_D_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d = d8;
}

void RLA(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = cpu.f;
    u8 result = 0;
    RL(cpu.a, &result, &flags);
    cpu.a = result;
    cpu.f = flags & 0b0111'0000;
}

void JR_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.pc += int8_t(d8);
}

void ADD_HL_DE(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    // -0HC
    u8 flags = 0;
    u16 result = 0;
    ADD16(cpu.hl, cpu.de, &result, &flags);
    cpu.hl = result;
    cpu.f = (flags & 0x70) | (cpu.f & 0x80);
}

void LD_A_ADE(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = mmu.read8(cpu.de);
}

void DEC_DE(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.de--;
}

void INC_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    INC8(cpu.e, cpu.f);
}

void DEC_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    DEC8(cpu.e, cpu.f);
}

void LD_E_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e = d8;
}

void RRA(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = cpu.f;
    u8 result = 0;
    RR(cpu.a, &result, &flags);
    cpu.f = flags & FLAGS_CARRY;
    cpu.a = result;
}

// 0x20

void JR_NZ_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    if (!(cpu.af & FLAGS_ZERO)) {
        cpu.pc += int8_t(d8);
        extraCycles = true;
    }
}

void LD_HL_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.hl = d16;
}

void LDI_HL_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    mmu.write8(cpu.hl++, cpu.a);
}

void INC_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    INC8(cpu.h, cpu.f);
}

void DEC_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    DEC8(cpu.h, cpu.f);
}

void LD_H_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h = d8;
}

void DAA(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 a = cpu.a;
    u8 f = cpu.f;

    if (!(f & FLAGS_SUBTRACT)) {    // after an addition, adjust if (half-)carry occurred or if result is out of bounds
        if ((f & FLAGS_CARRY) || a > 0x99) {
            a += 0x60;
            f |= FLAGS_CARRY;
        }
        if ((f & FLAGS_HALFCARRY) || (a & 0xF) > 0x9)
            a += 0x6;
    }
    else {    // after a subtraction, only adjust if (half-)carry occurred
        if (f & FLAGS_CARRY)
            a -= 0x60;
        if (f & FLAGS_HALFCARRY)
            a -= 0x6;
    }

    f &= 0b1101'0000;
    f = a ? f & 0b0111'0000 : f | 0b1000'0000;

    cpu.a = a;
    cpu.f = f;
}

void JR_Z_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    if (cpu.af & FLAGS_ZERO) {
        cpu.pc += int8_t(d8);
        extraCycles = true;
    }
}

void INC_HL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.hl++;
}

void ADD_HL_HL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    // -0HC
    u8 flags = 0;
    u16 result = 0;
    ADD16(cpu.hl, cpu.hl, &result, &flags);
    cpu.hl = result;
    cpu.f = (flags & 0x70) | (cpu.f & 0x80);
}

void LDI_A_HL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = mmu.read8(cpu.hl);
    cpu.hl++;
}

void DEC_HL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.hl--;
}

void INC_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    INC8(cpu.l, cpu.f);
}

void DEC_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    DEC8(cpu.l, cpu.f);
}

void LD_L_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l = d8;
}

void CPL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = ~cpu.a;
    u8 old_flags = cpu.f;
    u8 new_flags = (old_flags & (FLAGS_ZERO | FLAGS_CARRY)) | (FLAGS_SUBTRACT | FLAGS_HALFCARRY);
    cpu.f = new_flags;
}

// 0x30

void JR_NC_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    if (!(cpu.af & FLAGS_CARRY)) {
        cpu.pc += int8_t(d8);
        extraCycles = true;
    }
}

void LD_SP_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.sp = d16;
}

void LDD_HL_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    mmu.write8(cpu.hl, cpu.a);
    cpu.hl--;
}

void INC_SP(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.sp++;
}

void INC_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 result = mmu.read8(cpu.hl);
    INC8(result, cpu.f);
    mmu.write8(cpu.hl, result);
}

void DEC_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 f = 0;
    u8 result = mmu.read8(cpu.hl);
    DEC8(result, f);
    mmu.write8(cpu.hl, result);
    cpu.f = (f & 0b1110'0000) | (cpu.af & FLAGS_CARRY);
}

void LD_AHL_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    mmu.write8(cpu.hl, d8);
}

void SCF(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.f = (cpu.f & 0b1000'0000) | 0x10;
}

void JR_C_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    if (cpu.af & FLAGS_CARRY) {
        cpu.pc += int8_t(d8);
        extraCycles = true;
    }
}

void ADD_HL_SP(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    // -0HC
    u8 flags = 0;
    u16 result = 0;
    ADD16(cpu.hl, cpu.sp, &result, &flags);
    cpu.hl = result;
    flags &= 0b0011'0000;
    cpu.f = (cpu.f & 0b1000'0000) | flags;
}

void LDD_A_HL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = mmu.read8(cpu.hl--);
}

void DEC_SP(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.sp--;
}

void INC_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    INC8(cpu.a, cpu.f);
}

void DEC_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    DEC8(cpu.a, cpu.f);
}

void LD_A_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = d8;
}

void CCF(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 old_flags = cpu.f;
    u8 new_flags = (old_flags & FLAGS_ZERO) | ((~(old_flags & FLAGS_CARRY)) & FLAGS_CARRY);
    cpu.f = new_flags;
}

// 0x40

void LD_B_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    // cpu.b = cpu.b;
}

void LD_B_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b = cpu.c;
}

void LD_B_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b = cpu.d;
}

void LD_B_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b = cpu.e;
}

void LD_B_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b = cpu.h;
}

void LD_B_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b = cpu.l;
}

void LD_B_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b = mmu.read8(cpu.hl);
}

void LD_B_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b = cpu.a;
}

void LD_C_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c = cpu.b;
}

void LD_C_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    //
}

void LD_C_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c = cpu.d;
}

void LD_C_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c = cpu.e;
}

void LD_C_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c = cpu.h;
}

void LD_C_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c = cpu.l;
}

void LD_C_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c = mmu.read8(cpu.hl);
}

void LD_C_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c = cpu.a;
}

// 0x50

void LD_D_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d = cpu.b;
}

void LD_D_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d = cpu.c;
}

void LD_D_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    //
}

void LD_D_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d = cpu.e;
}

void LD_D_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d = cpu.h;
}

void LD_D_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d = cpu.l;
}

void LD_D_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d = mmu.read8(cpu.hl);
}

void LD_D_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d = cpu.a;
}

void LD_E_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e = cpu.b;
}

void LD_E_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e = cpu.c;
}

void LD_E_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e = cpu.d;
}

void LD_E_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    //
}

void LD_E_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e = cpu.h;
}

void LD_E_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e = cpu.l;
}

void LD_E_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e = mmu.read8(cpu.hl);
}

void LD_E_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e = cpu.a;
}

// 0x60

void LD_H_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h = cpu.b;
}

void LD_H_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h = cpu.c;
}

void LD_H_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h = cpu.d;
}

void LD_H_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h = cpu.e;
}

void LD_H_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    //
}

void LD_H_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h = cpu.l;
}

void LD_H_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h = mmu.read8(cpu.hl);
}

void LD_H_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h = cpu.a;
}

void LD_L_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l = cpu.b;
}

void LD_L_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l = cpu.c;
}

void LD_L_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l = cpu.d;
}

void LD_L_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l = cpu.e;
}

void LD_L_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l = cpu.h;
}

void LD_L_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    //
}

void LD_L_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l = mmu.read8(cpu.hl);
}

void LD_L_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l = cpu.a;
}

// 0x70

void LD_AHL_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    mmu.write8(cpu.hl, cpu.b);
}

void LD_AHL_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    mmu.write8(cpu.hl, cpu.c);
}

void LD_AHL_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    mmu.write8(cpu.hl, cpu.d);
}

void LD_AHL_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    mmu.write8(cpu.hl, cpu.e);
}

void LD_AHL_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    mmu.write8(cpu.hl, cpu.h);
}

void LD_AHL_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    mmu.write8(cpu.hl, cpu.l);
}

void HALT(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.halt();
}

void LD_AHL_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    mmu.write8(cpu.hl, cpu.a);
}

void LD_A_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = cpu.b;
}

void LD_A_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = cpu.c;
}

void LD_A_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = cpu.d;
}

void LD_A_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = cpu.e;
}

void LD_A_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = cpu.h;
}

void LD_A_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = cpu.l;
}

void LD_A_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = mmu.read8(cpu.hl);
}

void LD_A_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    // cpu.a = cpu.a;
}

// 0x80

void ADD_A_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = ADD8(cpu.a, cpu.b, cpu.f);
}

void ADD_A_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = ADD8(cpu.a, cpu.c, cpu.f);
}

void ADD_A_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = ADD8(cpu.a, cpu.d, cpu.f);
}

void ADD_A_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = ADD8(cpu.a, cpu.e, cpu.f);
}

void ADD_A_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = ADD8(cpu.a, cpu.h, cpu.f);
}

void ADD_A_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = ADD8(cpu.a, cpu.l, cpu.f);
}

void ADD_A_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = ADD8(cpu.a, mmu.read8(cpu.hl), cpu.f);
}

void ADD_A_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = ADD8(cpu.a, cpu.a, cpu.f);
}

void ADC_A_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    ADC(cpu.a, cpu.b, cpu.f);
}

void ADC_A_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    ADC(cpu.a, cpu.c, cpu.f);
}

void ADC_A_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    ADC(cpu.a, cpu.d, cpu.f);
}

void ADC_A_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    ADC(cpu.a, cpu.e, cpu.f);
}

void ADC_A_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    ADC(cpu.a, cpu.h, cpu.f);
}

void ADC_A_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    ADC(cpu.a, cpu.l, cpu.f);
}

void ADC_A_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    ADC(cpu.a, mmu.read8(cpu.hl), cpu.f);
}

void ADC_A_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    ADC(cpu.a, cpu.a, cpu.f);
}

// 0x90

void SUB_A_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = SUB8(cpu.a, cpu.b, cpu.f);
}

void SUB_A_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = SUB8(cpu.a, cpu.c, cpu.f);
}

void SUB_A_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = SUB8(cpu.a, cpu.d, cpu.f);
}

void SUB_A_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = SUB8(cpu.a, cpu.e, cpu.f);
}

void SUB_A_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = SUB8(cpu.a, cpu.h, cpu.f);
}

void SUB_A_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = SUB8(cpu.a, cpu.l, cpu.f);
}

void SUB_A_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = SUB8(cpu.a, mmu.read8(cpu.hl), cpu.f);
}

void SUB_A_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = SUB8(cpu.a, cpu.a, cpu.f);
}

void SBC_A_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SBC(cpu.a, cpu.b, cpu.f);
}

void SBC_A_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SBC(cpu.a, cpu.c, cpu.f);
}

void SBC_A_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SBC(cpu.a, cpu.d, cpu.f);
}

void SBC_A_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SBC(cpu.a, cpu.e, cpu.f);
}

void SBC_A_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SBC(cpu.a, cpu.h, cpu.f);
}

void SBC_A_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SBC(cpu.a, cpu.l, cpu.f);
}

void SBC_A_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SBC(cpu.a, mmu.read8(cpu.hl), cpu.f);
}

void SBC_A_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SBC(cpu.a, cpu.a, cpu.f);
}

// 0xA0

void AND_A_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    AND(cpu.a, cpu.b, cpu.f);
}

void AND_A_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    AND(cpu.a, cpu.c, cpu.f);
}

void AND_A_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    AND(cpu.a, cpu.d, cpu.f);
}

void AND_A_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    AND(cpu.a, cpu.e, cpu.f);
}

void AND_A_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    AND(cpu.a, cpu.h, cpu.f);
}

void AND_A_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    AND(cpu.a, cpu.l, cpu.f);
}

void AND_A_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    AND(cpu.a, mmu.read8(cpu.hl), cpu.f);
}

void AND_A_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    AND(cpu.a, cpu.a, cpu.f);
}

void XOR_A_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    XOR(cpu.a, cpu.b, cpu.f);
}

void XOR_A_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    XOR(cpu.a, cpu.c, cpu.f);
}

void XOR_A_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    XOR(cpu.a, cpu.d, cpu.f);
}

void XOR_A_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    XOR(cpu.a, cpu.e, cpu.f);
}

void XOR_A_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    XOR(cpu.a, cpu.h, cpu.f);
}

void XOR_A_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    XOR(cpu.a, cpu.l, cpu.f);
}

void XOR_A_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    XOR(cpu.a, mmu.read8(cpu.hl), cpu.f);
}

void XOR_A_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    XOR(cpu.a, cpu.a, cpu.f);
}

// 0xB0

void OR_A_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    OR(cpu.a, cpu.b, cpu.f);
}

void OR_A_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    OR(cpu.a, cpu.c, cpu.f);
}

void OR_A_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    OR(cpu.a, cpu.d, cpu.f);
}

void OR_A_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    OR(cpu.a, cpu.e, cpu.f);
}

void OR_A_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    OR(cpu.a, cpu.h, cpu.f);
}

void OR_A_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    OR(cpu.a, cpu.l, cpu.f);
}

void OR_A_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    OR(cpu.a, mmu.read8(cpu.hl), cpu.f);
}

void OR_A_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    OR(cpu.a, cpu.a, cpu.f);
}

void CP_A_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    CP(cpu.a, cpu.b, cpu.f);
}

void CP_A_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    CP(cpu.a, cpu.c, cpu.f);
}

void CP_A_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    CP(cpu.a, cpu.d, cpu.f);
}

void CP_A_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    CP(cpu.a, cpu.e, cpu.f);
}

void CP_A_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    CP(cpu.a, cpu.h, cpu.f);
}

void CP_A_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    CP(cpu.a, cpu.l, cpu.f);
}

void CP_A_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    CP(cpu.a, mmu.read8(cpu.hl), cpu.f);
}

void CP_A_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    CP(cpu.a, cpu.a, cpu.f);
}

// 0xC0

void RET_NZ(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    if (!(cpu.af & FLAGS_ZERO)) {
        cpu.pc = mmu.read16(cpu.sp);
        cpu.sp += 2;
        extraCycles = true;
    }
}

void POP_BC(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.bc = mmu.read16(cpu.sp);
    cpu.sp += 2;
}

void JP_NZ_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    if (!(cpu.af & FLAGS_ZERO)) {
        cpu.pc = d16;
        extraCycles = true;
    }
}

void JP_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.pc = d16;
}

void CALL_NZ_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    if (!(cpu.af & FLAGS_ZERO)) {
        cpu.sp -= 2;
        mmu.write16(cpu.sp, cpu.pc);
        cpu.pc = d16;
        extraCycles = true;
    }
}

void PUSH_BC(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.sp -= 2;
    mmu.write16(cpu.sp, cpu.bc);
}

void ADD_A_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = ADD8(cpu.a, d8, cpu.f);
}

void RST_00(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    RST(cpu, mmu, 0x00);
}

void RET_Z(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    if (cpu.af & FLAGS_ZERO) {
        cpu.pc = mmu.read16(cpu.sp);
        cpu.sp += 2;
        extraCycles = true;
    }
}

void RET(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.pc = mmu.read16(cpu.sp);
    cpu.sp += 2;
}

void JP_Z_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    if (cpu.af & FLAGS_ZERO) {
        cpu.pc = d16;
        extraCycles = true;
    }
}

void CB(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
}

void CALL_Z_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    if (cpu.af & FLAGS_ZERO) {
        cpu.sp -= 2;
        mmu.write16(cpu.sp, cpu.pc);
        cpu.pc = d16;
        extraCycles = true;
    }
}

void CALL_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.sp -= 2;
    mmu.write16(cpu.sp, cpu.pc);
    cpu.pc = d16;
}

void ADC_A_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    ADC(cpu.a, d8, cpu.f);
}

void RST_08(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    RST(cpu, mmu, 0x08);
}

// 0xD0

void RET_NC(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    if (!(cpu.af & FLAGS_CARRY)) {
        cpu.pc = mmu.read16(cpu.sp);
        cpu.sp += 2;
        extraCycles = true;
    }
}

void POP_DE(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.de = mmu.read16(cpu.sp);
    cpu.sp += 2;
}

void JP_NC_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    if (!(cpu.af & FLAGS_CARRY)) {
        cpu.pc = d16;
        extraCycles = true;
    }
}

void CALL_NC_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    if (!(cpu.af & FLAGS_CARRY)) {
        cpu.sp -= 2;
        mmu.write16(cpu.sp, cpu.pc);
        cpu.pc = d16;
        extraCycles = true;
    }
}

void PUSH_DE(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.sp -= 2;
    mmu.write16(cpu.sp, cpu.de);
}

void RET_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    if (cpu.af & FLAGS_CARRY) {
        cpu.pc = mmu.read16(cpu.sp);
        cpu.sp += 2;
        extraCycles = true;
    }
}

void RETI(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.enable_interrupts();
    cpu.pc = mmu.read16(cpu.sp);
    cpu.sp += 2;
}

void JP_C_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    if (cpu.af & FLAGS_CARRY) {
        cpu.pc = d16;
        extraCycles = true;
    }
}

void SUB_A_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = SUB8(cpu.a, d8, cpu.f);
}

void RST_10(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    RST(cpu, mmu, 0x10);
}

void CALL_C_IMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    if (cpu.af & FLAGS_CARRY) {
        cpu.sp -= 2;
        mmu.write16(cpu.sp, cpu.pc);
        cpu.pc = d16;
        extraCycles = true;
    }
}

void SBC_A_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SBC(cpu.a, d8, cpu.f);
}

void RST_18(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    RST(cpu, mmu, 0x18);
}

// 0xE0

void LDH_IMM8_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    mmu.write8(0xFF00 + d8, cpu.a);
}

void POP_HL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.hl = mmu.read16(cpu.sp);
    cpu.sp += 2;
}

void LDH_C_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    mmu.write8(0xFF00 + cpu.c, cpu.a);
}

void PUSH_HL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.sp -= 2;
    mmu.write16(cpu.sp, cpu.hl);
}

void AND_A_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    AND(cpu.a, d8, cpu.f);
}

void RST_20(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    RST(cpu, mmu, 0x20);
}

void ADD_SP_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    // 00HC
    u8 flags = 0;

    if (((cpu.sp & 0xF) + (d8 & 0xF)) & 0x10)
        flags |= FLAGS_HALFCARRY;

    if (((cpu.sp & 0xFF) + d8) > 0xFF)
        flags |= FLAGS_CARRY;

    cpu.sp = cpu.sp + int8_t(d8);
    cpu.f = flags;
}

void JP_HL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.pc = cpu.hl;
}

void LD_AIMM16_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    mmu.write8(d16, cpu.a);
}

void XOR_A_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    XOR(cpu.a, d8, cpu.f);
}

void RST_28(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    RST(cpu, mmu, 0x28);
}

// 0xF0

void LDH_A_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = mmu.read8(0xFF00 + d8);
}

void POP_AF(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.af = mmu.read16(cpu.sp) & 0xFFF0;
    cpu.sp += 2;
}

void LDH_A_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = mmu.read8(0xFF00 + cpu.c);
}

void DI(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.disable_interrupts();
}

void PUSH_AF(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.sp -= 2;
    mmu.write16(cpu.sp, cpu.af);
}

void OR_A_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    OR(cpu.a, d8, cpu.f);
}

void RST_30(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    RST(cpu, mmu, 0x30);
}

void LD_HL_SPIMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = 0;

    int8_t s8 = d8;
    u16 sp = cpu.sp + s8;
    if (s8 > 0) {
        if (((cpu.sp & 0xFF) + s8) > 0xFF) {
            flags |= FLAGS_CARRY;
        }
        if (((cpu.sp & 0xF) + (s8 & 0xF)) > 0xF) {
            flags |= FLAGS_HALFCARRY;
        }
    }
    else {
        if ((sp & 0xFF) < (cpu.sp & 0xFF)) {
            flags |= FLAGS_CARRY;
        }
        if ((sp & 0xF) < (cpu.sp & 0xF)) {
            flags |= FLAGS_HALFCARRY;
        }
    }

    cpu.f = flags;

    cpu.hl = sp;
}

void LD_SP_HL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.sp = cpu.hl;
}

void LD_A_AIMM16(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = mmu.read8(d16);
}

void EI(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.enable_interrupts();
}

void CP_A_IMM8(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    CP(cpu.a, d8, cpu.f);
}

void RST_38(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    RST(cpu, mmu, 0x38);
}

// 0xCB // Extended

// 0xCB 0x00

void RLC_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = 0;
    u8 result = 0;
    RLC(cpu.b, &result, &flags);
    cpu.b = result;
    cpu.f = flags;
}

void RLC_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = 0;
    u8 result = 0;
    RLC(cpu.c, &result, &flags);
    cpu.c = result;
    cpu.f = flags;
}

void RLC_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = 0;
    u8 result = 0;
    RLC(cpu.d, &result, &flags);
    cpu.d = result;
    cpu.f = flags;
}

void RLC_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = 0;
    u8 result = 0;
    RLC(cpu.e, &result, &flags);
    cpu.e = result;
    cpu.f = flags;
}

void RLC_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = 0;
    u8 result = 0;
    RLC(cpu.h, &result, &flags);
    cpu.h = result;
    cpu.f = flags;
}

void RLC_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = 0;
    u8 result = 0;
    RLC(cpu.l, &result, &flags);
    cpu.l = result;
    cpu.f = flags;
}

void RLC_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = 0;
    u8 result = 0;
    RLC(mmu.read8(cpu.hl), &result, &flags);
    mmu.write8(cpu.hl, result);
    cpu.f = flags;
}

void RLC_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = 0;
    u8 result = 0;
    RLC(cpu.a, &result, &flags, true);
    cpu.a = result;
    cpu.f = flags;
}

void RRC_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = 0;
    u8 result = 0;
    RRC(cpu.b, &result, &flags);
    cpu.b = result;
    cpu.f = flags;
}

void RRC_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = 0;
    u8 result = 0;
    RRC(cpu.c, &result, &flags);
    cpu.c = result;
    cpu.f = flags;
}

void RRC_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = 0;
    u8 result = 0;
    RRC(cpu.d, &result, &flags);
    cpu.d = result;
    cpu.f = flags;
}

void RRC_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = 0;
    u8 result = 0;
    RRC(cpu.e, &result, &flags);
    cpu.e = result;
    cpu.f = flags;
}

void RRC_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = 0;
    u8 result = 0;
    RRC(cpu.h, &result, &flags);
    cpu.h = result;
    cpu.f = flags;
}

void RRC_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = 0;
    u8 result = 0;
    RRC(cpu.l, &result, &flags);
    cpu.l = result;
    cpu.f = flags;
}

void RRC_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = 0;
    u8 result = 0;
    RRC(mmu.read8(cpu.hl), &result, &flags);
    mmu.write8(cpu.hl, result);
    cpu.f = flags;
}

void RRC_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = 0;
    u8 result = 0;
    RRC(cpu.a, &result, &flags);
    cpu.a = result;
    cpu.f = flags;
}

// 0xCB 0x10

void RL_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = cpu.af & 0x10;
    u8 result = 0;
    RL(cpu.b, &result, &flags);
    cpu.b = result;
    cpu.f = flags;
}

void RL_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = cpu.af & 0x10;
    u8 result = 0;
    RL(cpu.c, &result, &flags);
    cpu.c = result;
    cpu.f = flags;
}

void RL_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = cpu.af & 0x10;
    u8 result = 0;
    RL(cpu.d, &result, &flags);
    cpu.d = result;
    cpu.f = flags;
}

void RL_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = cpu.af & 0x10;
    u8 result = 0;
    RL(cpu.e, &result, &flags);
    cpu.e = result;
    cpu.f = flags;
}

void RL_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = cpu.af & 0x10;
    u8 result = 0;
    RL(cpu.h, &result, &flags);
    cpu.h = result;
    cpu.f = flags;
}

void RL_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = cpu.af & 0x10;
    u8 result = 0;
    RL(cpu.l, &result, &flags);
    cpu.l = result;
    cpu.f = flags;
}

void RL_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = cpu.f & 0x10;
    u8 result = 0;
    RL(mmu.read8(cpu.hl), &result, &flags);
    mmu.write8(cpu.hl, result);
    cpu.f = flags;
}

void RL_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = cpu.af & 0x10;
    u8 result = 0;
    RL(cpu.a, &result, &flags);
    cpu.a = result;
    cpu.f = flags;
}

void RR_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = cpu.af & 0x10;
    u8 result = 0;
    RR(cpu.b, &result, &flags);
    cpu.b = result;
    cpu.f = flags;
}

void RR_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = cpu.af & 0x10;
    u8 result = 0;
    RR(cpu.c, &result, &flags);
    cpu.c = result;
    cpu.f = flags;
}

void RR_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = cpu.af & 0x10;
    u8 result = 0;
    RR(cpu.d, &result, &flags);
    cpu.d = result;
    cpu.f = flags;
}

void RR_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = cpu.af & 0x10;
    u8 result = 0;
    RR(cpu.e, &result, &flags);
    cpu.e = result;
    cpu.f = flags;
}

void RR_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = cpu.af & 0x10;
    u8 result = 0;
    RR(cpu.h, &result, &flags);
    cpu.h = result;
    cpu.f = flags;
}

void RR_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = cpu.af & 0x10;
    u8 result = 0;
    RR(cpu.l, &result, &flags);
    cpu.l = result;
    cpu.f = flags;
}

void RR_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = cpu.af & 0x10;
    u8 result = 0;
    RR(mmu.read8(cpu.hl), &result, &flags);
    mmu.write8(cpu.hl, result);
    cpu.f = flags;
}

void RR_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 flags = cpu.af & 0x10;
    u8 result = 0;
    RR(cpu.a, &result, &flags);
    cpu.a = result;
    cpu.f = flags;
}

// 0xCB 0x20

void SLA_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SLA(cpu.b, cpu.f);
}

void SLA_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SLA(cpu.c, cpu.f);
}

void SLA_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SLA(cpu.d, cpu.f);
}

void SLA_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SLA(cpu.e, cpu.f);
}

void SLA_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SLA(cpu.h, cpu.f);
}

void SLA_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SLA(cpu.l, cpu.f);
}

void SLA_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 r {mmu.read8(cpu.hl)};
    SLA(r, cpu.f);
    mmu.write8(cpu.hl, r);
}

void SLA_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SLA(cpu.a, cpu.f);
}

void SRA_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SRA(cpu.b, cpu.f);
}

void SRA_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SRA(cpu.c, cpu.f);
}

void SRA_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SRA(cpu.d, cpu.f);
}

void SRA_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SRA(cpu.e, cpu.f);
}

void SRA_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SRA(cpu.h, cpu.f);
}

void SRA_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SRA(cpu.l, cpu.f);
}

void SRA_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 r {mmu.read8(cpu.hl)};
    SRA(r, cpu.f);
    mmu.write8(cpu.hl, r);
}

void SRA_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SRA(cpu.a, cpu.f);
}

// 0xCB 0x30

void SWAP_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SWAP(cpu.b, cpu.f);
}

void SWAP_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SWAP(cpu.c, cpu.f);
}

void SWAP_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SWAP(cpu.d, cpu.f);
}

void SWAP_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SWAP(cpu.e, cpu.f);
}

void SWAP_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SWAP(cpu.h, cpu.f);
}

void SWAP_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SWAP(cpu.l, cpu.f);
}

void SWAP_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 r = mmu.read8(cpu.hl);
    SWAP(r, cpu.f);
    mmu.write8(cpu.hl, r);
}

void SWAP_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SWAP(cpu.a, cpu.f);
}

void SRL_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SRL(cpu.b, cpu.f);
}

void SRL_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SRL(cpu.c, cpu.f);
}

void SRL_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SRL(cpu.d, cpu.f);
}

void SRL_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SRL(cpu.e, cpu.f);
}

void SRL_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SRL(cpu.h, cpu.f);
}

void SRL_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SRL(cpu.l, cpu.f);
}

void SRL_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    u8 r = mmu.read8(cpu.hl);
    u8 carryBit = r & 0b0000'0001;
    u8 f = 0;
    r >>= 1;
    mmu.write8(cpu.hl, r);
    if (!r)
        f |= FLAGS_ZERO;
    f |= (carryBit << 4);
    cpu.f = f;
}

void SRL_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    SRL(cpu.a, cpu.f);
}

// 0xCB 0x40

void BIT_0_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.b & 0b0000'0001, cpu.f);
}

void BIT_0_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.c & 0b0000'0001, cpu.f);
}

void BIT_0_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.d & 0b0000'0001, cpu.f);
}

void BIT_0_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.e & 0b0000'0001, cpu.f);
}

void BIT_0_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.h & 0b0000'0001, cpu.f);
}

void BIT_0_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.l & 0b0000'0001, cpu.f);
}

void BIT_0_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(mmu.read8(cpu.hl) & 0b0000'0001, cpu.f);
}

void BIT_0_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.a & 0b0000'0001, cpu.f);
}

void BIT_1_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.b & 0b0000'0010, cpu.f);
}

void BIT_1_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.c & 0b0000'0010, cpu.f);
}

void BIT_1_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.d & 0b0000'0010, cpu.f);
}

void BIT_1_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.e & 0b0000'0010, cpu.f);
}

void BIT_1_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.h & 0b0000'0010, cpu.f);
}

void BIT_1_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.l & 0b0000'0010, cpu.f);
}

void BIT_1_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(mmu.read8(cpu.hl) & 0b0000'0010, cpu.f);
}

void BIT_1_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.a & 0b0000'0010, cpu.f);
}

void BIT_2_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.b & 0b0000'0100, cpu.f);
}

void BIT_2_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.c & 0b0000'0100, cpu.f);
}

void BIT_2_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.d & 0b0000'0100, cpu.f);
}

void BIT_2_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.e & 0b0000'0100, cpu.f);
}

void BIT_2_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.h & 0b0000'0100, cpu.f);
}

void BIT_2_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.l & 0b0000'0100, cpu.f);
}

void BIT_2_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(mmu.read8(cpu.hl) & 0b0000'0100, cpu.f);
}

void BIT_2_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.a & 0b0000'0100, cpu.f);
}

void BIT_3_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.b & 0b0000'1000, cpu.f);
}

void BIT_3_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.c & 0b0000'1000, cpu.f);
}

void BIT_3_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.d & 0b0000'1000, cpu.f);
}

void BIT_3_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.e & 0b0000'1000, cpu.f);
}

void BIT_3_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.h & 0b0000'1000, cpu.f);
}

void BIT_3_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.l & 0b0000'1000, cpu.f);
}

void BIT_3_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(mmu.read8(cpu.hl) & 0b0000'1000, cpu.f);
}

void BIT_3_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.a & 0b0000'1000, cpu.f);
}

void BIT_4_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.b & 0b0001'0000, cpu.f);
}

void BIT_4_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.c & 0b0001'0000, cpu.f);
}

void BIT_4_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.d & 0b0001'0000, cpu.f);
}

void BIT_4_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.e & 0b0001'0000, cpu.f);
}

void BIT_4_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.h & 0b0001'0000, cpu.f);
}

void BIT_4_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.l & 0b0001'0000, cpu.f);
}

void BIT_4_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(mmu.read8(cpu.hl) & 0b0001'0000, cpu.f);
}

void BIT_4_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.a & 0b0001'0000, cpu.f);
}

void BIT_5_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.b & 0b0010'0000, cpu.f);
}

void BIT_5_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.c & 0b0010'0000, cpu.f);
}

void BIT_5_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.d & 0b0010'0000, cpu.f);
}

void BIT_5_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.e & 0b0010'0000, cpu.f);
}

void BIT_5_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.h & 0b0010'0000, cpu.f);
}

void BIT_5_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.l & 0b0010'0000, cpu.f);
}

void BIT_5_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(mmu.read8(cpu.hl) & 0b0010'0000, cpu.f);
}

void BIT_5_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.a & 0b0010'0000, cpu.f);
}

void BIT_6_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.b & 0b0100'0000, cpu.f);
}

void BIT_6_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.c & 0b0100'0000, cpu.f);
}

void BIT_6_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.d & 0b0100'0000, cpu.f);
}

void BIT_6_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.e & 0b0100'0000, cpu.f);
}

void BIT_6_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.h & 0b0100'0000, cpu.f);
}

void BIT_6_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.l & 0b0100'0000, cpu.f);
}

void BIT_6_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(mmu.read8(cpu.hl) & 0b0100'0000, cpu.f);
}

void BIT_6_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.a & 0b0100'0000, cpu.f);
}

void BIT_7_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.b & 0b1000'0000, cpu.f);
}

void BIT_7_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.c & 0b1000'0000, cpu.f);
}

void BIT_7_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.d & 0b1000'0000, cpu.f);
}

void BIT_7_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.e & 0b1000'0000, cpu.f);
}

void BIT_7_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.h & 0b1000'0000, cpu.f);
}

void BIT_7_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.l & 0b1000'0000, cpu.f);
}

void BIT_7_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(mmu.read8(cpu.hl) & 0b1000'0000, cpu.f);
}

void BIT_7_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    TestBit(cpu.a & 0b1000'0000, cpu.f);
}

void RES_0_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b = cpu.b & ~(0b0000'0001);
}

void RES_0_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c = cpu.c & ~(0b0000'0001);
}

void RES_0_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d = cpu.d & ~(0b0000'0001);
}

void RES_0_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e = cpu.e & ~(0b0000'0001);
}

void RES_0_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h = cpu.h & ~(0b0000'0001);
}

void RES_0_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l = cpu.l & ~(0b0000'0001);
}

void RES_0_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    auto val {mmu.read8(cpu.hl)};
    mmu.write8(cpu.hl, val & ~(0b0000'0001));
}

void RES_0_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = cpu.a & ~(0b0000'0001);
}

void RES_1_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b = cpu.b & ~(0b0000'0010);
}

void RES_1_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c = cpu.c & ~(0b0000'0010);
}

void RES_1_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d = cpu.d & ~(0b0000'0010);
}

void RES_1_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e = cpu.e & ~(0b0000'0010);
}

void RES_1_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h = cpu.h & ~(0b0000'0010);
}

void RES_1_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l = cpu.l & ~(0b0000'0010);
}

void RES_1_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    auto val {mmu.read8(cpu.hl)};
    mmu.write8(cpu.hl, val & ~(0b0000'0010));
}

void RES_1_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = cpu.a & ~(0b0000'0010);
}

void RES_2_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b = cpu.b & ~(0b0000'0100);
}

void RES_2_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c = cpu.c & ~(0b0000'0100);
}

void RES_2_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d = cpu.d & ~(0b0000'0100);
}

void RES_2_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e = cpu.e & ~(0b0000'0100);
}

void RES_2_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h = cpu.h & ~(0b0000'0100);
}

void RES_2_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l = cpu.l & ~(0b0000'0100);
}

void RES_2_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    auto val {mmu.read8(cpu.hl)};
    mmu.write8(cpu.hl, val & ~(0b0000'0100));
}

void RES_2_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = cpu.a & ~(0b0000'0100);
}

void RES_3_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b = cpu.b & ~(0b0000'1000);
}

void RES_3_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c = cpu.c & ~(0b0000'1000);
}

void RES_3_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d = cpu.d & ~(0b0000'1000);
}

void RES_3_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e = cpu.e & ~(0b0000'1000);
}

void RES_3_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h = cpu.h & ~(0b0000'1000);
}

void RES_3_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l = cpu.l & ~(0b0000'1000);
}

void RES_3_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    auto val {mmu.read8(cpu.hl)};
    mmu.write8(cpu.hl, val & ~(0b0000'1000));
}

void RES_3_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = cpu.a & ~(0b0000'1000);
}

void RES_4_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b = cpu.b & ~(0b0001'0000);
}

void RES_4_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c = cpu.c & ~(0b0001'0000);
}

void RES_4_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d = cpu.d & ~(0b0001'0000);
}

void RES_4_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e = cpu.e & ~(0b0001'0000);
}

void RES_4_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h = cpu.h & ~(0b0001'0000);
}

void RES_4_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l = cpu.l & ~(0b0001'0000);
}

void RES_4_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    auto val {mmu.read8(cpu.hl)};
    mmu.write8(cpu.hl, val & ~(0b0001'0000));
}

void RES_4_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = cpu.a & ~(0b0001'0000);
}

void RES_5_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b = cpu.b & ~(0b0010'0000);
}

void RES_5_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c = cpu.c & ~(0b0010'0000);
}

void RES_5_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d = cpu.d & ~(0b0010'0000);
}

void RES_5_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e = cpu.e & ~(0b0010'0000);
}

void RES_5_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h = cpu.h & ~(0b0010'0000);
}

void RES_5_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l = cpu.l & ~(0b0010'0000);
}

void RES_5_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    auto val {mmu.read8(cpu.hl)};
    mmu.write8(cpu.hl, val & ~(0b0010'0000));
}

void RES_5_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = cpu.a & ~(0b0010'0000);
}

void RES_6_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b = cpu.b & ~(0b0100'0000);
}

void RES_6_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c = cpu.c & ~(0b0100'0000);
}

void RES_6_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d = cpu.d & ~(0b0100'0000);
}

void RES_6_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e = cpu.e & ~(0b0100'0000);
}

void RES_6_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h = cpu.h & ~(0b0100'0000);
}

void RES_6_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l = cpu.l & ~(0b0100'0000);
}

void RES_6_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    auto val {mmu.read8(cpu.hl)};
    mmu.write8(cpu.hl, val & ~(0b0100'0000));
}

void RES_6_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = cpu.a & ~(0b0100'0000);
}

void RES_7_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b = cpu.b & ~(0b1000'0000);
}

void RES_7_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c = cpu.c & ~(0b1000'0000);
}

void RES_7_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d = cpu.d & ~(0b1000'0000);
}

void RES_7_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e = cpu.e & ~(0b1000'0000);
}

void RES_7_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h = cpu.h & ~(0b1000'0000);
}

void RES_7_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l = cpu.l & ~(0b1000'0000);
}

void RES_7_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    auto val {mmu.read8(cpu.hl)};
    mmu.write8(cpu.hl, val & static_cast<u8>(~0x80));
}

void RES_7_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a = cpu.a & ~(0b1000'0000);
}

void SET_0_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b |= 0b0000'0001;
}

void SET_0_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c |= 0b0000'0001;
}

void SET_0_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d |= 0b0000'0001;
}

void SET_0_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e |= 0b0000'0001;
}

void SET_0_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h |= 0b0000'0001;
}

void SET_0_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l |= 0b0000'0001;
}

void SET_0_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    auto val {mmu.read8(cpu.hl)};
    mmu.write8(cpu.hl, val | 0b0000'0001);
}

void SET_0_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a |= 0b0000'0001;
}

void SET_1_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b |= 0b0000'0010;
}

void SET_1_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c |= 0b0000'0010;
}

void SET_1_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d |= 0b0000'0010;
}

void SET_1_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e |= 0b0000'0010;
}

void SET_1_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h |= 0b0000'0010;
}

void SET_1_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l |= 0b0000'0010;
}

void SET_1_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    auto val {mmu.read8(cpu.hl)};
    mmu.write8(cpu.hl, val | 0b0000'0010);
}

void SET_1_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a |= 0b0000'0010;
}

void SET_2_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b |= 0b0000'0100;
}

void SET_2_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c |= 0b0000'0100;
}

void SET_2_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d |= 0b0000'0100;
}

void SET_2_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e |= 0b0000'0100;
}

void SET_2_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h |= 0b0000'0100;
}

void SET_2_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l |= 0b0000'0100;
}

void SET_2_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    auto val {mmu.read8(cpu.hl)};
    mmu.write8(cpu.hl, val | 0b0000'0100);
}

void SET_2_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a |= 0b0000'0100;
}

void SET_3_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b |= 0b0000'1000;
}

void SET_3_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c |= 0b0000'1000;
}

void SET_3_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d |= 0b0000'1000;
}

void SET_3_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e |= 0b0000'1000;
}

void SET_3_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h |= 0b0000'1000;
}

void SET_3_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l |= 0b0000'1000;
}

void SET_3_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    auto val {mmu.read8(cpu.hl)};
    mmu.write8(cpu.hl, val | 0b0000'1000);
}

void SET_3_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a |= 0b0000'1000;
}

void SET_4_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b |= 0b0001'0000;
}

void SET_4_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c |= 0b0001'0000;
}

void SET_4_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d |= 0b0001'0000;
}

void SET_4_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e |= 0b0001'0000;
}

void SET_4_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h |= 0b0001'0000;
}

void SET_4_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l |= 0b0001'0000;
}

void SET_4_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    auto val {mmu.read8(cpu.hl)};
    mmu.write8(cpu.hl, val | 0b0001'0000);
}

void SET_4_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a |= 0b0001'0000;
}

void SET_5_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b |= 0b0010'0000;
}

void SET_5_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c |= 0b0010'0000;
}

void SET_5_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d |= 0b0010'0000;
}

void SET_5_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e |= 0b0010'0000;
}

void SET_5_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h |= 0b0010'0000;
}

void SET_5_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l |= 0b0010'0000;
}

void SET_5_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    auto val {mmu.read8(cpu.hl)};
    mmu.write8(cpu.hl, val | 0b0010'0000);
}

void SET_5_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a |= 0b0010'0000;
}

void SET_6_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b |= 0b0100'0000;
}

void SET_6_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c |= 0b0100'0000;
}

void SET_6_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d |= 0b0100'0000;
}

void SET_6_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e |= 0b0100'0000;
}

void SET_6_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h |= 0b0100'0000;
}

void SET_6_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l |= 0b0100'0000;
}

void SET_6_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    auto val {mmu.read8(cpu.hl)};
    mmu.write8(cpu.hl, val | 0b0100'0000);
}

void SET_6_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a |= 0b0100'0000;
}

void SET_7_B(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.b |= 0b1000'0000;
}

void SET_7_C(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.c |= 0b1000'0000;
}

void SET_7_D(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.d |= 0b1000'0000;
}

void SET_7_E(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.e |= 0b1000'0000;
}

void SET_7_H(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.h |= 0b1000'0000;
}

void SET_7_L(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.l |= 0b1000'0000;
}

void SET_7_AHL(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    auto val {mmu.read8(cpu.hl)};
    mmu.write8(cpu.hl, val | 0b1000'0000);
}

void SET_7_A(Cpu &cpu, Component &mmu, u8 d8, u16 d16, bool &extraCycles)
{
    cpu.a |= 0b1000'0000;
}

// clang-format off

const Instruction kInstructions[] =
{     
/*      0           1            2            3         4              5         6            7         8             9          A            B        C             D           E           F   */
/* 0 */ NOP,        LD_BC_IMM16, LD_ABC_A,    INC_BC,   INC_B,         DEC_B,    LD_B_IMM8,   RLCA,     LD_AIMM16_SP, ADD_HL_BC, LD_A_ABC,    DEC_BC,  INC_C,        DEC_C ,     LD_C_IMM8,  RRCA,
/* 1 */ STOP,       LD_DE_IMM16, LD_ADE_A,    INC_DE,   INC_D,         DEC_D,    LD_D_IMM8,   RLA,      JR_IMM8,      ADD_HL_DE, LD_A_ADE,    DEC_DE,  INC_E,        DEC_E,      LD_E_IMM8,  RRA,
/* 2 */ JR_NZ_IMM8, LD_HL_IMM16, LDI_HL_A,    INC_HL,   INC_H,         DEC_H,    LD_H_IMM8,   DAA,      JR_Z_IMM8,    ADD_HL_HL, LDI_A_HL,    DEC_HL,  INC_L,        DEC_L,      LD_L_IMM8,  CPL,
/* 3 */ JR_NC_IMM8, LD_SP_IMM16, LDD_HL_A,    INC_SP,   INC_AHL,       DEC_AHL,  LD_AHL_IMM8, SCF,      JR_C_IMM8,    ADD_HL_SP, LDD_A_HL,    DEC_SP,  INC_A,        DEC_A,      LD_A_IMM8,  CCF,
/* 4 */ LD_B_B,     LD_B_C,      LD_B_D,      LD_B_E,   LD_B_H,        LD_B_L,   LD_B_AHL,    LD_B_A,   LD_C_B,       LD_C_C,    LD_C_D,      LD_C_E,  LD_C_H,       LD_C_L,     LD_C_AHL,   LD_C_A,
/* 5 */ LD_D_B,     LD_D_C,      LD_D_D,      LD_D_E,   LD_D_H,        LD_D_L,   LD_D_AHL,    LD_D_A,   LD_E_B,       LD_E_C,    LD_E_D,      LD_E_E,  LD_E_H,       LD_E_L,     LD_E_AHL,   LD_E_A,
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
/* CB */
/*      0        1        2        3        4        5        6          7        8        9        A        B        C        D        E          F   */
/* 0 */ RLC_B,   RLC_C,   RLC_D,   RLC_E,   RLC_H,   RLC_L,   RLC_AHL,   RLC_A,   RRC_B,   RRC_C,   RRC_D,   RRC_E,   RRC_H,   RRC_L,   RRC_AHL,   RRC_A,
/* 1 */ RL_B,    RL_C,    RL_D,    RL_E,    RL_H,    RL_L,    RL_AHL,    RL_A,    RR_B,    RR_C,    RR_D,    RR_E,    RR_H,    RR_L,    RR_AHL,    RR_A,
/* 2 */ SLA_B,   SLA_C,   SLA_D,   SLA_E,   SLA_H,   SLA_L,   SLA_AHL,   SLA_A,   SRA_B,   SRA_C,   SRA_D,   SRA_E,   SRA_H,   SRA_L,   SRA_AHL,   SRA_A,
/* 3 */ SWAP_B,  SWAP_C,  SWAP_D,  SWAP_E,  SWAP_H,  SWAP_L,  SWAP_AHL,  SWAP_A,  SRL_B,   SRL_C,   SRL_D,   SRL_E,   SRL_H,   SRL_L,   SRL_AHL,   SRL_A,
/* 4 */ BIT_0_B, BIT_0_C, BIT_0_D, BIT_0_E, BIT_0_H, BIT_0_L, BIT_0_AHL, BIT_0_A, BIT_1_B, BIT_1_C, BIT_1_D, BIT_1_E, BIT_1_H, BIT_1_L, BIT_1_AHL, BIT_1_A,
/* 5 */ BIT_2_B, BIT_2_C, BIT_2_D, BIT_2_E, BIT_2_H, BIT_2_L, BIT_2_AHL, BIT_2_A, BIT_3_B, BIT_3_C, BIT_3_D, BIT_3_E, BIT_3_H, BIT_3_L, BIT_3_AHL, BIT_3_A,
/* 6 */ BIT_4_B, BIT_4_C, BIT_4_D, BIT_4_E, BIT_4_H, BIT_4_L, BIT_4_AHL, BIT_4_A, BIT_5_B, BIT_5_C, BIT_5_D, BIT_5_E, BIT_5_H, BIT_5_L, BIT_5_AHL, BIT_5_A,
/* 7 */ BIT_6_B, BIT_6_C, BIT_6_D, BIT_6_E, BIT_6_H, BIT_6_L, BIT_6_AHL, BIT_6_A, BIT_7_B, BIT_7_C, BIT_7_D, BIT_7_E, BIT_7_H, BIT_7_L, BIT_7_AHL, BIT_7_A,
/* 8 */ RES_0_B, RES_0_C, RES_0_D, RES_0_E, RES_0_H, RES_0_L, RES_0_AHL, RES_0_A, RES_1_B, RES_1_C, RES_1_D, RES_1_E, RES_1_H, RES_1_L, RES_1_AHL, RES_1_A,
/* 9 */ RES_2_B, RES_2_C, RES_2_D, RES_2_E, RES_2_H, RES_2_L, RES_2_AHL, RES_2_A, RES_3_B, RES_3_C, RES_3_D, RES_3_E, RES_3_H, RES_3_L, RES_3_AHL, RES_3_A,
/* A */ RES_4_B, RES_4_C, RES_4_D, RES_4_E, RES_4_H, RES_4_L, RES_4_AHL, RES_4_A, RES_5_B, RES_5_C, RES_5_D, RES_5_E, RES_5_H, RES_5_L, RES_5_AHL, RES_5_A,
/* B */ RES_6_B, RES_6_C, RES_6_D, RES_6_E, RES_6_H, RES_6_L, RES_6_AHL, RES_6_A, RES_7_B, RES_7_C, RES_7_D, RES_7_E, RES_7_H, RES_7_L, RES_7_AHL, RES_7_A,
/* C */ SET_0_B, SET_0_C, SET_0_D, SET_0_E, SET_0_H, SET_0_L, SET_0_AHL, SET_0_A, SET_1_B, SET_1_C, SET_1_D, SET_1_E, SET_1_H, SET_1_L, SET_1_AHL, SET_1_A,
/* D */ SET_2_B, SET_2_C, SET_2_D, SET_2_E, SET_2_H, SET_2_L, SET_2_AHL, SET_2_A, SET_3_B, SET_3_C, SET_3_D, SET_3_E, SET_3_H, SET_3_L, SET_3_AHL, SET_3_A,
/* E */ SET_4_B, SET_4_C, SET_4_D, SET_4_E, SET_4_H, SET_4_L, SET_4_AHL, SET_4_A, SET_5_B, SET_5_C, SET_5_D, SET_5_E, SET_5_H, SET_5_L, SET_5_AHL, SET_5_A,
/* F */ SET_6_B, SET_6_C, SET_6_D, SET_6_E, SET_6_H, SET_6_L, SET_6_AHL, SET_6_A, SET_7_B, SET_7_C, SET_7_D, SET_7_E, SET_7_H, SET_7_L, SET_7_AHL, SET_7_A,
};

// clang-format on
