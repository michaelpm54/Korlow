#include "cpu_base.h"
#include "cpu_instructions.h"
#include "cpu.h"
#include "mmu.h"

// Generic

void INVALID(Core &c)
{
	c.paused = true;
}

void RST(Core &c, uint16_t addr)
{
	c.r.sp -= 2;
	c.mmu.write16(c.r.sp, c.r.pc);
	c.r.pc = addr;
}

void SWAP(uint8_t &n, uint8_t &f)
{
	uint8_t hi = (n & 0xF0) >> 4;
	uint8_t lo = (n & 0x0F);
	n = (lo << 4) | hi;
	f = (!n) ? 0x80 : 0x0;
}

void SRL(uint8_t &r, uint8_t &f)
{
	/* Shift register right into carry. MSB set to 0. */
	// Z00C
	uint8_t carry = r & 0x1;
	r >>= 1;
	uint8_t flags = 0;
	if (r == 0)
	{
		flags |= FLAGS_ZERO;
	}
	if (carry)
	{
		flags |= FLAGS_CARRY;
	}
	f = flags;
}

void XOR(uint8_t &a, uint8_t r, uint8_t &f)
{
	// Z000
	a ^= r;
	f = a ? 0 : FLAGS_ZERO;
}

void AND(uint8_t &a, uint8_t r, uint8_t &f)
{
	// Z010
	a &= r;
	f = a ? FLAGS_HALFCARRY : (FLAGS_ZERO | FLAGS_HALFCARRY);
}

void OR(uint8_t &a, uint8_t r, uint8_t &f)
{
	// Z000
	a |= r;
	f = a ? 0 : FLAGS_ZERO;
}

void SLA(uint8_t &r, uint8_t &f)
{
	// Z00C
	uint8_t carry = r & 0b1000'0000;
	r <<= 1;
	uint8_t flags = 0;
	if (!r)
		flags |= FLAGS_ZERO;
	if (carry)
		flags |= FLAGS_CARRY;
	f = flags;
}

void SRA(uint8_t &r, uint8_t &f)
{
	// Z00C
	uint8_t msb = r & 0b1000'0000;
	uint8_t carry = r & 0b0000'0001;
	r >>= 1;
	r |= msb;
	uint8_t flags = 0;
	if (!r)
		flags |= FLAGS_ZERO;
	if (carry)
		flags |= FLAGS_CARRY;
	f = flags;
}

void ADC(uint8_t &a, uint8_t r, uint8_t &f)
{
	uint8_t carry = (f & FLAGS_CARRY) >> 4;
	uint8_t flags = 0;

	uint16_t a_, r_, c;
	a_ = a;
	r_ = r;
	c = carry;

	uint16_t result = a + r_ + c;

	if (result > 0xFF)
	{
		flags |= FLAGS_CARRY;
	}
	if (((a & 0xF) + (r_ & 0xF) + c) > 0xF)
	{
		flags |= FLAGS_HALFCARRY;
	}
	if ((result & 0xFF) == 0)
	{
		flags |= FLAGS_ZERO;
	}

	f = flags;
	a = result & 0xFF;
}

void SBC(uint8_t &a, uint8_t r, uint8_t &f)
{
	uint8_t carry = (f & FLAGS_CARRY) >> 4;
	uint8_t flags = FLAGS_SUBTRACT;

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
void SetFlags(uint8_t &f, uint8_t flags, uint8_t mask)
{
	f = f ^ ((f ^ flags) & mask);
}

// 0x00

void NOP(Core &c)
{
}

void LD_BC_IMM16(Core &c)
{
	c.r.bc = c.d16;
}

void LD_ABC_A(Core &c)
{
	c.mmu.write8(c.r.bc, c.r.a);
}

void INC_BC(Core &c)
{
	c.r.bc++;
}

void INC_B(Core &c)
{
	INC8(c.r.b, c.r.f);
}

void DEC_B(Core &c)
{
	DEC8(c.r.b, c.r.f);
}

void LD_B_IMM8(Core &c)
{
	c.r.b = c.d8;
}

// x = cpu.a.bit[7]
// carry = x
// cpu.a << 1
// cpu.a |= x
void RLCA(Core &c)
{
	uint8_t bit7 = !!(c.r.af & 0b1000'0000'0000'0000);
	c.r.f = bit7 << 4;
	c.r.a = (c.r.a << 1) | bit7;
}

void LD_AIMM16_SP(Core &c)
{
	c.mmu.write16(c.d16, c.r.sp);
}

void ADD_HL_BC(Core &c)
{
	// -0HC
	uint8_t flags = 0;
	uint16_t result = 0;
	ADD16(c.r.hl, c.r.bc, &result, &flags);
	c.r.hl = result;
	flags &= 0b0111'0000;
	c.r.f = (c.r.af & FLAGS_ZERO) | flags;
}

void LD_A_ABC(Core &c)
{
	c.r.a = c.mmu.read8(c.r.bc);
}

void DEC_BC(Core &c)
{
	c.r.bc--;
}

void INC_C(Core &c)
{
	INC8(c.r.c, c.r.f);
}

void DEC_C(Core &c)
{
	DEC8(c.r.c, c.r.f);
}

void LD_C_IMM8(Core &c)
{
	c.r.bc = (c.r.bc & 0xFF00) + c.d8;
}

void RRCA(Core &c)
{
	uint8_t bit0 = !!(c.r.af & 0b0000'0001'0000'0000);
	c.r.f = bit0 << 4;
	c.r.a = ((c.r.a >> 1) & 0b0111'1111) | (bit0 << 7);
}

// 0x10

void STOP(Core &c)
{
}

void LD_DE_IMM16(Core &c)
{
	c.r.de = c.d16;
}

void LD_ADE_A(Core &c)
{
	c.mmu.write8(c.r.de, c.r.a);
}

void INC_DE(Core &c)
{
	c.r.de++;
}

void INC_D(Core &c)
{
	INC8(c.r.d, c.r.f);
}

void DEC_D(Core &c)
{
	DEC8(c.r.d, c.r.f);
}

void LD_D_IMM8(Core &c)
{
	c.r.d = c.d8;
}

void RLA(Core &c)
{
	uint8_t flags = c.r.f;
	uint8_t result = 0;
	RL(c.r.a, &result, &flags);
	c.r.a = result;
	c.r.f = flags & 0b0111'0000;
}

void JR_IMM8(Core &c)
{
	c.r.pc += int8_t(c.d8);
}

void ADD_HL_DE(Core &c)
{
	// -0HC
	uint8_t flags = 0;
	uint16_t result = 0;
	ADD16(c.r.hl, c.r.de, &result, &flags);
	c.r.hl = result;
	c.r.f = (flags & 0x70) | (c.r.f & 0x80);
}

void LD_A_ADE(Core &c)
{
	c.r.a = c.mmu.read8(c.r.de);
}

void DEC_DE(Core &c)
{
	c.r.de--;
}

void INC_E(Core &c)
{
	INC8(c.r.e, c.r.f);
}

void DEC_E(Core &c)
{
	DEC8(c.r.e, c.r.f);
}

void LD_E_IMM8(Core &c)
{
	c.r.e = c.d8;
}

void RRA(Core &c)
{
	uint8_t flags = c.r.f;
	uint8_t result = 0;
	RR(c.r.a, &result, &flags);
	c.r.f = flags & FLAGS_CARRY;
	c.r.a = result;
}

// 0x20

void JR_NZ_IMM8(Core &c)
{
	if (!(c.r.af & FLAGS_ZERO))
	{
		c.r.pc += int8_t(c.d8);
	}
	else
	{
		c.extraCycles = false;
	}
}

void LD_HL_IMM16(Core &c)
{
	c.r.hl = c.d16;
}

void LDI_HL_A(Core &c)
{
	c.mmu.write8(c.r.hl++, c.r.a);
}

void INC_H(Core &c)
{
	INC8(c.r.h, c.r.f);
}

void DEC_H(Core &c)
{
	DEC8(c.r.h, c.r.f);
}

void LD_H_IMM8(Core &c)
{
	c.r.h = c.d8;
}

void DAA(Core &c)
{
	uint8_t a = c.r.a;
	uint8_t f = c.r.f;

	if (!(f & FLAGS_SUBTRACT)) {  // after an addition, adjust if (half-)carry occurred or if result is out of bounds
		if ((f & FLAGS_CARRY) || a > 0x99) {
			a += 0x60;
			f |= FLAGS_CARRY;
		}
		if ((f & FLAGS_HALFCARRY) || (a & 0xF) > 0x9)
			a += 0x6;
	}
	else {  // after a subtraction, only adjust if (half-)carry occurred
		if (f & FLAGS_CARRY)
			a -= 0x60;
		if (f & FLAGS_HALFCARRY)
			a -= 0x6;
	}

	f &= 0b1101'0000;
	f = a ? f & 0b0111'0000 : f | 0b1000'0000;

	c.r.a = a;
	c.r.f = f;
}

void JR_Z_IMM8(Core &c)
{
	if (c.r.af & FLAGS_ZERO)
	{
		c.r.pc += int8_t(c.d8);
	}
	else
	{
		c.extraCycles = false;
	}
}

void INC_HL(Core &c)
{
	c.r.hl++;
}

void ADD_HL_HL(Core &c)
{
	// -0HC
	uint8_t flags = 0;
	uint16_t result = 0;
	ADD16(c.r.hl, c.r.hl, &result, &flags);
	c.r.hl = result;
	c.r.f = (flags & 0x70) | (c.r.f & 0x80);
}

void LDI_A_HL(Core &c)
{
	c.r.a = c.mmu.read8(c.r.hl);
	c.r.hl++;
}

void DEC_HL(Core &c)
{
	c.r.hl--;
}

void INC_L(Core &c)
{
	INC8(c.r.l, c.r.f);
}

void DEC_L(Core &c)
{
	DEC8(c.r.l, c.r.f);
}

void LD_L_IMM8(Core &c)
{
	c.r.l = c.d8;
}

void CPL(Core &c)
{
	c.r.a = ~c.r.a;
	uint8_t old_flags = c.r.f;
	uint8_t new_flags = (old_flags & (FLAGS_ZERO | FLAGS_CARRY)) | (FLAGS_SUBTRACT | FLAGS_HALFCARRY);
	c.r.f = new_flags;
}

// 0x30

void JR_NC_IMM8(Core &c)
{
	if (!(c.r.af & FLAGS_CARRY))
	{
		c.r.pc += int8_t(c.d8);
	}
	else
	{
		c.extraCycles = false;
	}
}

void LD_SP_IMM16(Core &c)
{
	c.r.sp = c.d16;
}

void LDD_HL_A(Core &c)
{
	c.mmu.write8(c.r.hl, c.r.a);
	c.r.hl--;
}

void INC_SP(Core &c)
{
	c.r.sp++;
}

void INC_AHL(Core &c)
{
	uint8_t result = c.mmu.read8(c.r.hl);
	INC8(result, c.r.f);
	c.mmu.write8(c.r.hl, result);
}

void DEC_AHL(Core &c)
{
	uint8_t f = 0;
	uint8_t result = c.mmu.read8(c.r.hl);
	DEC8(result, f);
	c.mmu.write8(c.r.hl, result);
	c.r.f = (f & 0b1110'0000) | (c.r.af & FLAGS_CARRY);
}

void LD_AHL_IMM8(Core &c)
{
	c.mmu.write8(c.r.hl, c.d8);
}

void SCF(Core &c)
{
	c.r.f = (c.r.f & 0b1000'0000) | 0x10;
}

void JR_C_IMM8(Core &c)
{
	if (c.r.af & FLAGS_CARRY)
	{
		c.r.pc += int8_t(c.d8);
	}
	else
	{
		c.extraCycles = false;
	}
}

void ADD_HL_SP(Core &c)
{
	// -0HC
	uint8_t flags = 0;
	uint16_t result = 0;
	ADD16(c.r.hl, c.r.sp, &result, &flags);
	c.r.hl = result;
	flags &= 0b0011'0000;
	c.r.f = (c.r.f & 0b1000'0000) | flags;
}

void LDD_A_HL(Core &c)
{
	c.r.a = c.mmu.read8(c.r.hl--);
}

void DEC_SP(Core &c)
{
	c.r.sp--;
}

void INC_A(Core &c)
{
	INC8(c.r.a, c.r.f);
}

void DEC_A(Core &c)
{
	DEC8(c.r.a, c.r.f);
}

void LD_A_IMM8(Core &c)
{
	c.r.a = c.d8;
}

void CCF(Core &c)
{
	uint8_t old_flags = c.r.f;
	uint8_t new_flags = (old_flags & FLAGS_ZERO) | ((~(old_flags & FLAGS_CARRY)) & FLAGS_CARRY);
	c.r.f = new_flags;
}

// 0x40

void LD_B_B(Core &c)
{
	// c.r.b = c.r.b;
}

void LD_B_C(Core &c)
{
	c.r.b = c.r.c;
}

void LD_B_D(Core &c)
{
	c.r.b = c.r.d;
}

void LD_B_E(Core &c)
{
	c.r.b = c.r.e;
}

void LD_B_H(Core &c)
{
	c.r.b = c.r.h;
}

void LD_B_L(Core &c)
{
	c.r.b = c.r.l;
}

void LD_B_AHL(Core &c)
{
	c.r.b = c.mmu.read8(c.r.hl);
}

void LD_B_A(Core &c)
{
	c.r.b = c.r.a;
}

void LD_C_B(Core &c)
{
	c.r.c = c.r.b;
}

void LD_C_C(Core &c)
{
	//
}

void LD_C_D(Core &c)
{
	c.r.c = c.r.d;
}

void LD_C_E(Core &c)
{
	c.r.c = c.r.e;
}

void LD_C_H(Core &c)
{
	c.r.c = c.r.h;
}

void LD_C_L(Core &c)
{
	c.r.c = c.r.l;
}

void LD_C_AHL(Core &c)
{
	c.r.c = c.mmu.read8(c.r.hl);
}

void LD_C_A(Core &c)
{
	c.r.c = c.r.a;
}

// 0x50

void LD_D_B(Core &c)
{
	c.r.d = c.r.b;
}

void LD_D_C(Core &c)
{
	c.r.d = c.r.c;
}

void LD_D_D(Core &c)
{
	//
}

void LD_D_E(Core &c)
{
	c.r.d = c.r.e;
}

void LD_D_H(Core &c)
{
	c.r.d = c.r.h;
}

void LD_D_L(Core &c)
{
	c.r.d = c.r.l;
}

void LD_D_AHL(Core &c)
{
	c.r.d = c.mmu.read8(c.r.hl);
}

void LD_D_A(Core &c)
{
	c.r.d = c.r.a;
}

void LD_E_B(Core &c)
{
	c.r.e = c.r.b;
}

void LD_E_C(Core &c)
{
	c.r.e = c.r.c;
}

void LD_E_D(Core &c)
{
	c.r.e = c.r.d;
}

void LD_E_E(Core &c)
{
	//
}

void LD_E_H(Core &c)
{
	c.r.e = c.r.h;
}

void LD_E_L(Core &c)
{
	c.r.e = c.r.l;
}

void LD_E_AHL(Core &c)
{
	c.r.e = c.mmu.read8(c.r.hl);
}

void LD_E_A(Core &c)
{
	c.r.e = c.r.a;
}

// 0x60

void LD_H_B(Core &c)
{
	c.r.h = c.r.b;
}

void LD_H_C(Core &c)
{
	c.r.h = c.r.c;
}

void LD_H_D(Core &c)
{
	c.r.h = c.r.d;
}

void LD_H_E(Core &c)
{
	c.r.h = c.r.e;
}

void LD_H_H(Core &c)
{
	//
}

void LD_H_L(Core &c)
{
	c.r.h = c.r.l;
}

void LD_H_AHL(Core &c)
{
	c.r.h = c.mmu.read8(c.r.hl);
}

void LD_H_A(Core &c)
{
	c.r.h = c.r.a;
}

void LD_L_B(Core &c)
{
	c.r.l = c.r.b;
}

void LD_L_C(Core &c)
{
	c.r.l = c.r.c;
}

void LD_L_D(Core &c)
{
	c.r.l = c.r.d;
}

void LD_L_E(Core &c)
{
	c.r.l = c.r.e;
}

void LD_L_H(Core &c)
{
	c.r.l = c.r.h;
}

void LD_L_L(Core &c)
{
	//
}

void LD_L_AHL(Core &c)
{
	c.r.l = c.mmu.read8(c.r.hl);
}

void LD_L_A(Core &c)
{
	c.r.l = c.r.a;
}

// 0x70

void LD_AHL_B(Core &c)
{
	c.mmu.write8(c.r.hl, c.r.b);
}

void LD_AHL_C(Core &c)
{
	c.mmu.write8(c.r.hl, c.r.c);
}

void LD_AHL_D(Core &c)
{
	c.mmu.write8(c.r.hl, c.r.d);
}

void LD_AHL_E(Core &c)
{
	c.mmu.write8(c.r.hl, c.r.e);
}

void LD_AHL_H(Core &c)
{
	c.mmu.write8(c.r.hl, c.r.h);
}

void LD_AHL_L(Core &c)
{
	c.mmu.write8(c.r.hl, c.r.l);
}

void HALT(Core &c)
{
	c.halt = true;
}

void LD_AHL_A(Core &c)
{
	c.mmu.write8(c.r.hl, c.r.a);
}

void LD_A_B(Core &c)
{
	c.r.a = c.r.b;
}

void LD_A_C(Core &c)
{
	c.r.a = c.r.c;
}

void LD_A_D(Core &c)
{
	c.r.a = c.r.d;
}

void LD_A_E(Core &c)
{
	c.r.a = c.r.e;
}

void LD_A_H(Core &c)
{
	c.r.a = c.r.h;
}

void LD_A_L(Core &c)
{
	c.r.a = c.r.l;
}

void LD_A_AHL(Core &c)
{
	c.r.a = c.mmu.read8(c.r.hl);
}

void LD_A_A(Core &c)
{
	// c.r.a = c.r.a;
}

// 0x80

void ADD_A_B(Core &c)
{
	c.r.a = ADD8(c.r.a, c.r.b, c.r.f);
}

void ADD_A_C(Core &c)
{
	c.r.a = ADD8(c.r.a, c.r.c, c.r.f);
}

void ADD_A_D(Core &c)
{
	c.r.a = ADD8(c.r.a, c.r.d, c.r.f);
}

void ADD_A_E(Core &c)
{
	c.r.a = ADD8(c.r.a, c.r.e, c.r.f);
}

void ADD_A_H(Core &c)
{
	c.r.a = ADD8(c.r.a, c.r.h, c.r.f);
}

void ADD_A_L(Core &c)
{
	c.r.a = ADD8(c.r.a, c.r.l, c.r.f);
}

void ADD_A_AHL(Core &c)
{
	c.r.a = ADD8(c.r.a, c.mmu.read8(c.r.hl), c.r.f);
}

void ADD_A_A(Core &c)
{
	c.r.a = ADD8(c.r.a, c.r.a, c.r.f);
}

void ADC_A_B(Core &c)
{
	ADC(c.r.a, c.r.b, c.r.f);
}

void ADC_A_C(Core &c)
{
	ADC(c.r.a, c.r.c, c.r.f);
}

void ADC_A_D(Core &c)
{
	ADC(c.r.a, c.r.d, c.r.f);
}

void ADC_A_E(Core &c)
{
	ADC(c.r.a, c.r.e, c.r.f);
}

void ADC_A_H(Core &c)
{
	ADC(c.r.a, c.r.h, c.r.f);
}

void ADC_A_L(Core &c)
{
	ADC(c.r.a, c.r.l, c.r.f);
}

void ADC_A_AHL(Core &c)
{
	ADC(c.r.a, c.mmu.read8(c.r.hl), c.r.f);
}

void ADC_A_A(Core &c)
{
	ADC(c.r.a, c.r.a, c.r.f);
}

// 0x90

void SUB_A_B(Core &c)
{
	c.r.a = SUB8(c.r.a, c.r.b, c.r.f);
}

void SUB_A_C(Core &c)
{
	c.r.a = SUB8(c.r.a, c.r.c, c.r.f);
}

void SUB_A_D(Core &c)
{
	c.r.a = SUB8(c.r.a, c.r.d, c.r.f);
}

void SUB_A_E(Core &c)
{
	c.r.a = SUB8(c.r.a, c.r.e, c.r.f);
}

void SUB_A_H(Core &c)
{
	c.r.a = SUB8(c.r.a, c.r.h, c.r.f);
}

void SUB_A_L(Core &c)
{
	c.r.a = SUB8(c.r.a, c.r.l, c.r.f);
}

void SUB_A_AHL(Core &c)
{
	c.r.a = SUB8(c.r.a, c.mmu.read8(c.r.hl), c.r.f);
}

void SUB_A_A(Core &c)
{
	c.r.a = SUB8(c.r.a, c.r.a, c.r.f);
}

void SBC_A_B(Core &c)
{
	SBC(c.r.a, c.r.b, c.r.f);
}

void SBC_A_C(Core &c)
{
	SBC(c.r.a, c.r.c, c.r.f);
}

void SBC_A_D(Core &c)
{
	SBC(c.r.a, c.r.d, c.r.f);
}

void SBC_A_E(Core &c)
{
	SBC(c.r.a, c.r.e, c.r.f);
}

void SBC_A_H(Core &c)
{
	SBC(c.r.a, c.r.h, c.r.f);
}

void SBC_A_L(Core &c)
{
	SBC(c.r.a, c.r.l, c.r.f);
}

void SBC_A_AHL(Core &c)
{
	SBC(c.r.a, c.mmu.read8(c.r.hl), c.r.f);
}

void SBC_A_A(Core &c)
{
	SBC(c.r.a, c.r.a, c.r.f);
}

// 0xA0

void AND_A_B(Core &c)
{
	AND(c.r.a, c.r.b, c.r.f);
}

void AND_A_C(Core &c)
{
	AND(c.r.a, c.r.c, c.r.f);
}

void AND_A_D(Core &c)
{
	AND(c.r.a, c.r.d, c.r.f);
}

void AND_A_E(Core &c)
{
	AND(c.r.a, c.r.e, c.r.f);
}

void AND_A_H(Core &c)
{
	AND(c.r.a, c.r.h, c.r.f);
}

void AND_A_L(Core &c)
{
	AND(c.r.a, c.r.l, c.r.f);
}

void AND_A_AHL(Core &c)
{
	AND(c.r.a, c.mmu.read8(c.r.hl), c.r.f);
}

void AND_A_A(Core &c)
{
	AND(c.r.a, c.r.a, c.r.f);
}

void XOR_A_B(Core &c)
{
	XOR(c.r.a, c.r.b, c.r.f);
}

void XOR_A_C(Core &c)
{
	XOR(c.r.a, c.r.c, c.r.f);
}

void XOR_A_D(Core &c)
{
	XOR(c.r.a, c.r.d, c.r.f);
}

void XOR_A_E(Core &c)
{
	XOR(c.r.a, c.r.e, c.r.f);
}

void XOR_A_H(Core &c)
{
	XOR(c.r.a, c.r.h, c.r.f);
}

void XOR_A_L(Core &c)
{
	XOR(c.r.a, c.r.l, c.r.f);
}

void XOR_A_AHL(Core &c)
{
	XOR(c.r.a, c.mmu.read8(c.r.hl), c.r.f);
}

void XOR_A_A(Core &c)
{
	XOR(c.r.a, c.r.a, c.r.f);
}

// 0xB0

void OR_A_B(Core &c)
{
	OR(c.r.a, c.r.b, c.r.f);
}

void OR_A_C(Core &c)
{
	OR(c.r.a, c.r.c, c.r.f);
}

void OR_A_D(Core &c)
{
	OR(c.r.a, c.r.d, c.r.f);
}

void OR_A_E(Core &c)
{
	OR(c.r.a, c.r.e, c.r.f);
}

void OR_A_H(Core &c)
{
	OR(c.r.a, c.r.h, c.r.f);
}

void OR_A_L(Core &c)
{
	OR(c.r.a, c.r.l, c.r.f);
}

void OR_A_AHL(Core &c)
{
	OR(c.r.a, c.mmu.read8(c.r.hl), c.r.f);
}

void OR_A_A(Core &c)
{
	OR(c.r.a, c.r.a, c.r.f);
}

void CP_A_B(Core &c)
{
	CP(c.r.a, c.r.b, c.r.f);
}

void CP_A_C(Core &c)
{
	CP(c.r.a, c.r.c, c.r.f);
}

void CP_A_D(Core &c)
{
	CP(c.r.a, c.r.d, c.r.f);
}

void CP_A_E(Core &c)
{
	CP(c.r.a, c.r.e, c.r.f);
}

void CP_A_H(Core &c)
{
	CP(c.r.a, c.r.h, c.r.f);
}

void CP_A_L(Core &c)
{
	CP(c.r.a, c.r.l, c.r.f);
}

void CP_A_AHL(Core &c)
{
	CP(c.r.a, c.mmu.read8(c.r.hl), c.r.f);
}

void CP_A_A(Core &c)
{
	CP(c.r.a, c.r.a, c.r.f);
}

// 0xC0

void RET_NZ(Core &c)
{
	if (!(c.r.af & FLAGS_ZERO))
	{
		c.r.pc = c.mmu.read16(c.r.sp);
		c.r.sp += 2;
	}
	else
	{
		c.extraCycles = false;
	}
}

void POP_BC(Core &c)
{
	c.r.bc = c.mmu.read16(c.r.sp);
	c.r.sp += 2;
}

void JP_NZ_IMM16(Core &c)
{
	if (!(c.r.af & FLAGS_ZERO))
	{
		c.r.pc = c.d16;
	}
	else
	{
		c.extraCycles = false;
	}
}

void JP_IMM16(Core &c)
{
	c.r.pc = c.d16;
}

void CALL_NZ_IMM16(Core &c)
{
	if (!(c.r.af & FLAGS_ZERO))
	{
		c.r.sp -= 2;
		c.mmu.write16(c.r.sp, c.r.pc);
		c.r.pc = c.d16;
	}
	else
	{
		c.extraCycles = false;
	}
}

void PUSH_BC(Core &c)
{
	c.r.sp -= 2;
	c.mmu.write16(c.r.sp, c.r.bc);
}

void ADD_A_IMM8(Core &c)
{
	c.r.a = ADD8(c.r.a, c.d8, c.r.f);
}

void RST_00(Core &c)
{
	RST(c, 0x00);
}

void RET_Z(Core &c)
{
	if (c.r.af & FLAGS_ZERO)
	{
		c.r.pc = c.mmu.read16(c.r.sp);
		c.r.sp += 2;
	}
	else
	{
		c.extraCycles = false;
	}
}

void RET(Core &c)
{
	c.r.pc = c.mmu.read16(c.r.sp);
	c.r.sp += 2;
}

void JP_Z_IMM16(Core &c)
{
	if (c.r.af & FLAGS_ZERO)
	{
		c.r.pc = c.d16;
	}
	else
	{
		c.extraCycles = false;
	}
}

void CB(Core &c)
{
}

void CALL_Z_IMM16(Core &c)
{
	if (c.r.af & FLAGS_ZERO)
	{
		c.r.sp -= 2;
		c.mmu.write16(c.r.sp, c.r.pc);
		c.r.pc = c.d16;
	}
	else
	{
		c.extraCycles = false;
	}
}

void CALL_IMM16(Core &c)
{
	c.r.sp -= 2;
	c.mmu.write16(c.r.sp, c.r.pc);
	c.r.pc = c.d16;
}

void ADC_A_IMM8(Core &c)
{
	ADC(c.r.a, c.d8, c.r.f);
}

void RST_08(Core &c)
{
	RST(c, 0x08);
}

// 0xD0

void RET_NC(Core &c)
{
	if (!(c.r.af & FLAGS_CARRY))
	{
		c.r.pc = c.mmu.read16(c.r.sp);
		c.r.sp += 2;
	}
	else
	{
		c.extraCycles = false;
	}
}

void POP_DE(Core &c)
{
	c.r.de = c.mmu.read16(c.r.sp);
	c.r.sp += 2;
}

void JP_NC_IMM16(Core &c)
{
	if (!(c.r.af & FLAGS_CARRY))
	{
		c.r.pc = c.d16;
	}
	else
	{
		c.extraCycles = false;
	}
}

void CALL_NC_IMM16(Core &c)
{
	if (!(c.r.af & FLAGS_CARRY))
	{
		c.r.sp -= 2;
		c.mmu.write16(c.r.sp, c.r.pc);
		c.r.pc = c.d16;
	}
	else
	{
		c.extraCycles = false;
	}
}

void PUSH_DE(Core &c)
{
	c.r.sp -= 2;
	c.mmu.write16(c.r.sp, c.r.de);
}

void RET_C(Core &c)
{
	if (c.r.af & FLAGS_CARRY)
	{
		c.r.pc = c.mmu.read16(c.r.sp);
		c.r.sp += 2;
	}
	else
	{
		c.extraCycles = false;
	}
}

void RETI(Core &c)
{
	c.ime = true;
	c.r.pc = c.mmu.read16(c.r.sp);
	c.r.sp += 2;
}

void JP_C_IMM16(Core &c)
{
	if (c.r.af & FLAGS_CARRY)
	{
		c.r.pc = c.d16;
	}
	else
	{
		c.extraCycles = false;
	}
}

void SUB_A_IMM8(Core &c)
{
	c.r.a = SUB8(c.r.a, c.d8, c.r.f);
}

void RST_10(Core &c)
{
	RST(c, 0x10);
}

void CALL_C_IMM16(Core &c)
{
	if (c.r.af & FLAGS_CARRY)
	{
		c.r.sp -= 2;
		c.mmu.write16(c.r.sp, c.r.pc);
		c.r.pc = c.d16;
	}
	else
	{
		c.extraCycles = false;
	}
}

void SBC_A_IMM8(Core &c)
{
	SBC(c.r.a, c.d8, c.r.f);
}

void RST_18(Core &c)
{
	RST(c, 0x18);
}

// 0xE0

void LDH_IMM8_A(Core &c)
{
	c.mmu.write8(0xFF00 + c.d8, c.r.a);
}

void POP_HL(Core &c)
{
	c.r.hl = c.mmu.read16(c.r.sp);
	c.r.sp += 2;
}

void LDH_C_A(Core &c)
{
	c.mmu.write8(0xFF00 + c.r.c, c.r.a);
}

void PUSH_HL(Core &c)
{
	c.r.sp -= 2;
	c.mmu.write16(c.r.sp, c.r.hl);
}

void AND_A_IMM8(Core &c)
{
	AND(c.r.a, c.d8, c.r.f);
}

void RST_20(Core &c)
{
	RST(c, 0x20);
}

void ADD_SP_IMM8(Core &c)
{
	// 00HC
	uint8_t flags = 0;
	
	if (((c.r.sp & 0xF) + (c.d8 & 0xF)) & 0x10)
		flags |= FLAGS_HALFCARRY;

	if (((c.r.sp & 0xFF) + c.d8) > 0xFF)
		flags |= FLAGS_CARRY;

	c.r.sp = c.r.sp + int8_t(c.d8);
	c.r.f = flags;
}

void JP_HL(Core &c)
{
	c.r.pc = c.r.hl;
}

void LD_AIMM16_A(Core &c)
{
	c.mmu.write8(c.d16, c.r.a);
}

void XOR_A_IMM8(Core &c)
{
	XOR(c.r.a, c.d8, c.r.f);
}

void RST_28(Core &c)
{
	RST(c, 0x28);
}

// 0xF0

void LDH_A_IMM8(Core &c)
{
	c.r.a = c.mmu.read8(0xFF00 + c.d8);
}

void POP_AF(Core &c)
{
	c.r.af = c.mmu.read16(c.r.sp) & 0xFFF0;
	c.r.sp += 2;
}

void LDH_A_C(Core &c)
{
	c.r.a = c.mmu.read8(0xFF00 + c.r.c);
}

void DI(Core &c)
{
	c.ime = false;
}

void PUSH_AF(Core &c)
{
	c.r.sp -= 2;
	c.mmu.write16(c.r.sp, c.r.af);	
}

void OR_A_IMM8(Core &c)
{
	OR(c.r.a, c.d8, c.r.f);
}

void RST_30(Core &c)
{
	RST(c, 0x30);
}

void LD_HL_SPIMM8(Core &c)
{
	uint8_t flags = 0;

	int8_t s8 = c.d8;
	uint16_t sp = c.r.sp + s8;
	if (s8 > 0)
	{
		if (((c.r.sp & 0xFF) + s8) > 0xFF)
		{
			flags |= FLAGS_CARRY;
		}
		if (((c.r.sp & 0xF) + (s8 & 0xF)) > 0xF)
		{
			flags |= FLAGS_HALFCARRY;
		}
	}
	else
	{
		if ((sp & 0xFF) < (c.r.sp & 0xFF))
		{
			flags |= FLAGS_CARRY;
		}
		if ((sp & 0xF) < (c.r.sp & 0xF))
		{
			flags |= FLAGS_HALFCARRY;
		}
	}

	c.r.f = flags;

	c.r.hl = sp;
}

void LD_SP_HL(Core &c)
{
	c.r.sp = c.r.hl;
}

void LD_A_AIMM16(Core &c)
{
	c.r.a = c.mmu.read8(c.d16);
}

void EI(Core &c)
{
	c.ime = true;
}

void CP_A_IMM8(Core &c)
{
	CP(c.r.a, c.d8, c.r.f);
}

void RST_38(Core &c)
{
	RST(c, 0x38);
}

// 0xCB // Extended

// 0xCB 0x00

void RLC_B(Core &c)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RLC(c.r.b, &result, &flags);
	c.r.b = result;
	c.r.f = flags;
}

void RLC_C(Core &c)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RLC(c.r.c, &result, &flags);
	c.r.c = result;
	c.r.f = flags;
}

void RLC_D(Core &c)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RLC(c.r.d, &result, &flags);
	c.r.d = result;
	c.r.f = flags;
}

void RLC_E(Core &c)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RLC(c.r.e, &result, &flags);
	c.r.e = result;
	c.r.f = flags;
}

void RLC_H(Core &c)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RLC(c.r.h, &result, &flags);
	c.r.h = result;
	c.r.f = flags;
}

void RLC_L(Core &c)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RLC(c.r.l, &result, &flags);
	c.r.l = result;
	c.r.f = flags;
}

void RLC_AHL(Core &c)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RLC(c.mmu.read8(c.r.hl), &result, &flags);
	c.mmu.write8(c.r.hl, result);
	c.r.f = flags;
}

void RLC_A(Core &c)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RLC(c.r.a, &result, &flags);
	c.r.a = result;
	c.r.f = flags;
}

void RRC_B(Core &c)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RRC(c.r.b, &result, &flags);
	c.r.b = result;
	c.r.f = flags;
}

void RRC_C(Core &c)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RRC(c.r.c, &result, &flags);
	c.r.c = result;
	c.r.f = flags;
}

void RRC_D(Core &c)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RRC(c.r.d, &result, &flags);
	c.r.d = result;
	c.r.f = flags;
}

void RRC_E(Core &c)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RRC(c.r.e, &result, &flags);
	c.r.e = result;
	c.r.f = flags;
}

void RRC_H(Core &c)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RRC(c.r.h, &result, &flags);
	c.r.h = result;
	c.r.f = flags;
}

void RRC_L(Core &c)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RRC(c.r.l, &result, &flags);
	c.r.l = result;
	c.r.f = flags;
}

void RRC_AHL(Core &c)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RRC(c.mmu.read8(c.r.hl), &result, &flags);
	c.mmu.write8(c.r.hl, result);
	c.r.f = flags;
}

void RRC_A(Core &c)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RRC(c.r.a, &result, &flags);
	c.r.a = result;
	c.r.f = flags;
}

// 0xCB 0x10

void RL_B(Core &c)
{
	uint8_t flags = c.r.af & 0x10;
	uint8_t result = 0;
	RL(c.r.b, &result, &flags);
	c.r.b = result;
	c.r.f = flags;
}

void RL_C(Core &c)
{
	uint8_t flags = c.r.af & 0x10;
	uint8_t result = 0;
	RL(c.r.c, &result, &flags);
	c.r.c = result;
	c.r.f = flags;
}

void RL_D(Core &c)
{
	uint8_t flags = c.r.af & 0x10;
	uint8_t result = 0;
	RL(c.r.d, &result, &flags);
	c.r.d = result;
	c.r.f = flags;
}

void RL_E(Core &c)
{
	uint8_t flags = c.r.af & 0x10;
	uint8_t result = 0;
	RL(c.r.e, &result, &flags);
	c.r.e = result;
	c.r.f = flags;
}

void RL_H(Core &c)
{
	uint8_t flags = c.r.af & 0x10;
	uint8_t result = 0;
	RL(c.r.h, &result, &flags);
	c.r.h = result;
	c.r.f = flags;
}

void RL_L(Core &c)
{
	uint8_t flags = c.r.af & 0x10;
	uint8_t result = 0;
	RL(c.r.l, &result, &flags);
	c.r.l = result;
	c.r.f = flags;
}

void RL_AHL(Core &c)
{
	uint8_t flags = c.r.af & 0x10;
	uint8_t result = 0;
	RL(c.mmu.read8(c.r.hl), &result, &flags);
	c.mmu.write8(c.r.hl, result);
	c.r.f = flags;
}

void RL_A(Core &c)
{
	uint8_t flags = c.r.af & 0x10;
	uint8_t result = 0;
	RL(c.r.a, &result, &flags);
	c.r.a = result;
	c.r.f = flags;
}

void RR_B(Core &c)
{
	uint8_t flags = c.r.af & 0x10;
	uint8_t result = 0;
	RR(c.r.b, &result, &flags);
	c.r.b = result;
	c.r.f = flags;
}

void RR_C(Core &c)
{
	uint8_t flags = c.r.af & 0x10;
	uint8_t result = 0;
	RR(c.r.c, &result, &flags);
	c.r.c = result;
	c.r.f = flags;
}

void RR_D(Core &c)
{
	uint8_t flags = c.r.af & 0x10;
	uint8_t result = 0;
	RR(c.r.d, &result, &flags);
	c.r.d = result;
	c.r.f = flags;
}

void RR_E(Core &c)
{
	uint8_t flags = c.r.af & 0x10;
	uint8_t result = 0;
	RR(c.r.e, &result, &flags);
	c.r.e = result;
	c.r.f = flags;
}

void RR_H(Core &c)
{
	uint8_t flags = c.r.af & 0x10;
	uint8_t result = 0;
	RR(c.r.h, &result, &flags);
	c.r.h = result;
	c.r.f = flags;
}

void RR_L(Core &c)
{
	uint8_t flags = c.r.af & 0x10;
	uint8_t result = 0;
	RR(c.r.l, &result, &flags);
	c.r.l = result;
	c.r.f = flags;
}

void RR_AHL(Core &c)
{
	uint8_t flags = c.r.af & 0x10;
	uint8_t result = 0;
	RR(c.mmu.read8(c.r.hl), &result, &flags);
	c.mmu.write8(c.r.hl, result);
	c.r.f = flags;
}

void RR_A(Core &c)
{
	uint8_t flags = c.r.af & 0x10;
	uint8_t result = 0;
	RR(c.r.a, &result, &flags);
	c.r.a = result;
	c.r.f = flags;
}

// 0xCB 0x20

void SLA_B(Core &c)
{
	SLA(c.r.b, c.r.f);
}

void SLA_C(Core &c)
{
	SLA(c.r.c, c.r.f);
}

void SLA_D(Core &c)
{
	SLA(c.r.d, c.r.f);
}

void SLA_E(Core &c)
{
	SLA(c.r.e, c.r.f);
}

void SLA_H(Core &c)
{
	SLA(c.r.h, c.r.f);
}

void SLA_L(Core &c)
{
	SLA(c.r.l, c.r.f);
}

void SLA_AHL(Core &c)
{
	uint8_t r { c.mmu.read8(c.r.hl) };
	SLA(r, c.r.f);
	c.mmu.write8(c.r.hl, r);
}

void SLA_A(Core &c)
{
	SLA(c.r.a, c.r.f);
}

void SRA_B(Core &c)
{
	SRA(c.r.b, c.r.f);
}

void SRA_C(Core &c)
{
	SRA(c.r.c, c.r.f);
}

void SRA_D(Core &c)
{
	SRA(c.r.d, c.r.f);
}

void SRA_E(Core &c)
{
	SRA(c.r.e, c.r.f);
}

void SRA_H(Core &c)
{
	SRA(c.r.h, c.r.f);
}

void SRA_L(Core &c)
{
	SRA(c.r.l, c.r.f);
}

void SRA_AHL(Core &c)
{
	uint8_t r { c.mmu.read8(c.r.hl) };
	SRA(r, c.r.f);
	c.mmu.write8(c.r.hl, r);
}

void SRA_A(Core &c)
{
	SRA(c.r.a, c.r.f);
}

// 0xCB 0x30

void SWAP_B(Core &c)
{
	SWAP(c.r.b, c.r.f);
}

void SWAP_C(Core &c)
{
	SWAP(c.r.c, c.r.f);
}

void SWAP_D(Core &c)
{
	SWAP(c.r.d, c.r.f);
}

void SWAP_E(Core &c)
{
	SWAP(c.r.e, c.r.f);
}

void SWAP_H(Core &c)
{
	SWAP(c.r.h, c.r.f);
}

void SWAP_L(Core &c)
{
	SWAP(c.r.l, c.r.f);
}

void SWAP_AHL(Core &c)
{
	uint8_t r = c.mmu.read8(c.r.hl);
	SWAP(r, c.r.f);
	c.mmu.write8(c.r.hl, r);
}

void SWAP_A(Core &c)
{
	SWAP(c.r.a, c.r.f);
}

void SRL_B(Core &c)
{
	SRL(c.r.b, c.r.f);
}

void SRL_C(Core &c)
{
	SRL(c.r.c, c.r.f);
}

void SRL_D(Core &c)
{
	SRL(c.r.d, c.r.f);
}

void SRL_E(Core &c)
{
	SRL(c.r.e, c.r.f);
}

void SRL_H(Core &c)
{
	SRL(c.r.h, c.r.f);
}

void SRL_L(Core &c)
{
	SRL(c.r.l, c.r.f);
}

void SRL_AHL(Core &c)
{
	uint8_t r = c.mmu.read8(c.r.hl);
	uint8_t carryBit = r & 0b0000'0001;
	uint8_t f = 0;
	r >>= 1;
	c.mmu.write8(c.r.hl, r);
	if (!r)
		f |= FLAGS_ZERO;
	f |= (carryBit << 4);
	c.r.f = f;
}

void SRL_A(Core &c)
{
	SRL(c.r.a, c.r.f);
}

// 0xCB 0x40

void BIT_0_B(Core &c)
{
	TestBit(c.r.b & 0b0000'0001, c.r.f);
}

void BIT_0_C(Core &c)
{
	TestBit(c.r.c & 0b0000'0001, c.r.f);
}

void BIT_0_D(Core &c)
{
	TestBit(c.r.d & 0b0000'0001, c.r.f);
}

void BIT_0_E(Core &c)
{
	TestBit(c.r.e & 0b0000'0001, c.r.f);
}

void BIT_0_H(Core &c)
{
	TestBit(c.r.h & 0b0000'0001, c.r.f);
}

void BIT_0_L(Core &c)
{
	TestBit(c.r.l & 0b0000'0001, c.r.f);
}

void BIT_0_AHL(Core &c)
{
	TestBit(c.mmu.read8(c.r.hl) & 0b0000'0001, c.r.f);
}

void BIT_0_A(Core &c)
{
	TestBit(c.r.a & 0b0000'0001, c.r.f);
}

void BIT_1_B(Core &c)
{
	TestBit(c.r.b & 0b0000'0010, c.r.f);
}

void BIT_1_C(Core &c)
{
	TestBit(c.r.c & 0b0000'0010, c.r.f);
}

void BIT_1_D(Core &c)
{
	TestBit(c.r.d & 0b0000'0010, c.r.f);
}

void BIT_1_E(Core &c)
{
	TestBit(c.r.e & 0b0000'0010, c.r.f);
}

void BIT_1_H(Core &c)
{
	TestBit(c.r.h & 0b0000'0010, c.r.f);
}

void BIT_1_L(Core &c)
{
	TestBit(c.r.l & 0b0000'0010, c.r.f);
}

void BIT_1_AHL(Core &c)
{
	TestBit(c.mmu.read8(c.r.hl) & 0b0000'0010, c.r.f);
}

void BIT_1_A(Core &c)
{
	TestBit(c.r.a & 0b0000'0010, c.r.f);
}

void BIT_2_B(Core &c)
{
	TestBit(c.r.b & 0b0000'0100, c.r.f);
}

void BIT_2_C(Core &c)
{
	TestBit(c.r.c & 0b0000'0100, c.r.f);
}

void BIT_2_D(Core &c)
{
	TestBit(c.r.d & 0b0000'0100, c.r.f);
}

void BIT_2_E(Core &c)
{
	TestBit(c.r.e & 0b0000'0100, c.r.f);
}

void BIT_2_H(Core &c)
{
	TestBit(c.r.h & 0b0000'0100, c.r.f);
}

void BIT_2_L(Core &c)
{
	TestBit(c.r.l & 0b0000'0100, c.r.f);
}

void BIT_2_AHL(Core &c)
{
	TestBit(c.mmu.read8(c.r.hl) & 0b0000'0100, c.r.f);
}

void BIT_2_A(Core &c)
{
	TestBit(c.r.a & 0b0000'0100, c.r.f);
}

void BIT_3_B(Core &c)
{
	TestBit(c.r.b & 0b0000'1000, c.r.f);
}

void BIT_3_C(Core &c)
{
	TestBit(c.r.c & 0b0000'1000, c.r.f);
}

void BIT_3_D(Core &c)
{
	TestBit(c.r.d & 0b0000'1000, c.r.f);
}

void BIT_3_E(Core &c)
{
	TestBit(c.r.e & 0b0000'1000, c.r.f);
}

void BIT_3_H(Core &c)
{
	TestBit(c.r.h & 0b0000'1000, c.r.f);
}

void BIT_3_L(Core &c)
{
	TestBit(c.r.l & 0b0000'1000, c.r.f);
}

void BIT_3_AHL(Core &c)
{
	TestBit(c.mmu.read8(c.r.hl) & 0b0000'1000, c.r.f);
}

void BIT_3_A(Core &c)
{
	TestBit(c.r.a & 0b0000'1000, c.r.f);
}

void BIT_4_B(Core &c)
{
	TestBit(c.r.b & 0b0001'0000, c.r.f);
}

void BIT_4_C(Core &c)
{
	TestBit(c.r.c & 0b0001'0000, c.r.f);
}

void BIT_4_D(Core &c)
{
	TestBit(c.r.d & 0b0001'0000, c.r.f);
}

void BIT_4_E(Core &c)
{
	TestBit(c.r.e & 0b0001'0000, c.r.f);
}

void BIT_4_H(Core &c)
{
	TestBit(c.r.h & 0b0001'0000, c.r.f);
}

void BIT_4_L(Core &c)
{
	TestBit(c.r.l & 0b0001'0000, c.r.f);
}

void BIT_4_AHL(Core &c)
{
	TestBit(c.mmu.read8(c.r.hl) & 0b0001'0000, c.r.f);
}

void BIT_4_A(Core &c)
{
	TestBit(c.r.a & 0b0001'0000, c.r.f);
}

void BIT_5_B(Core &c)
{
	TestBit(c.r.b & 0b0010'0000, c.r.f);
}

void BIT_5_C(Core &c)
{
	TestBit(c.r.c & 0b0010'0000, c.r.f);
}

void BIT_5_D(Core &c)
{
	TestBit(c.r.d & 0b0010'0000, c.r.f);
}

void BIT_5_E(Core &c)
{
	TestBit(c.r.e & 0b0010'0000, c.r.f);
}

void BIT_5_H(Core &c)
{
	TestBit(c.r.h & 0b0010'0000, c.r.f);
}

void BIT_5_L(Core &c)
{
	TestBit(c.r.l & 0b0010'0000, c.r.f);
}

void BIT_5_AHL(Core &c)
{
	TestBit(c.mmu.read8(c.r.hl) & 0b0010'0000, c.r.f);
}

void BIT_5_A(Core &c)
{
	TestBit(c.r.a & 0b0010'0000, c.r.f);
}

void BIT_6_B(Core &c)
{
	TestBit(c.r.b & 0b0100'0000, c.r.f);
}

void BIT_6_C(Core &c)
{
	TestBit(c.r.c & 0b0100'0000, c.r.f);
}

void BIT_6_D(Core &c)
{
	TestBit(c.r.d & 0b0100'0000, c.r.f);
}

void BIT_6_E(Core &c)
{
	TestBit(c.r.e & 0b0100'0000, c.r.f);
}

void BIT_6_H(Core &c)
{
	TestBit(c.r.h & 0b0100'0000, c.r.f);
}

void BIT_6_L(Core &c)
{
	TestBit(c.r.l & 0b0100'0000, c.r.f);
}

void BIT_6_AHL(Core &c)
{
	TestBit(c.mmu.read8(c.r.hl) & 0b0100'0000, c.r.f);
}

void BIT_6_A(Core &c)
{
	TestBit(c.r.a & 0b0100'0000, c.r.f);
}

void BIT_7_B(Core &c)
{
	TestBit(c.r.b & 0b1000'0000, c.r.f);
}

void BIT_7_C(Core &c)
{
	TestBit(c.r.c & 0b1000'0000, c.r.f);
}

void BIT_7_D(Core &c)
{
	TestBit(c.r.d & 0b1000'0000, c.r.f);
}

void BIT_7_E(Core &c)
{
	TestBit(c.r.e & 0b1000'0000, c.r.f);
}

void BIT_7_H(Core &c)
{
	TestBit(c.r.h & 0b1000'0000, c.r.f);
}

void BIT_7_L(Core &c)
{
	TestBit(c.r.l & 0b1000'0000, c.r.f);
}

void BIT_7_AHL(Core &c)
{
	TestBit(c.mmu.read8(c.r.hl) & 0b1000'0000, c.r.f);
}

void BIT_7_A(Core &c)
{
	TestBit(c.r.a & 0b1000'0000, c.r.f);
}

void RES_0_B(Core &c)
{
	c.r.b = c.r.b & ~(0b0000'0001);
}

void RES_0_C(Core &c)
{
	c.r.c = c.r.c & ~(0b0000'0001);
}

void RES_0_D(Core &c)
{
	c.r.d = c.r.d & ~(0b0000'0001);
}

void RES_0_E(Core &c)
{
	c.r.e = c.r.e & ~(0b0000'0001);
}

void RES_0_H(Core &c)
{
	c.r.h = c.r.h & ~(0b0000'0001);
}

void RES_0_L(Core &c)
{
	c.r.l = c.r.l & ~(0b0000'0001);
}

void RES_0_AHL(Core &c)
{
	c.mmu.and8(c.r.hl, ~(0b0000'0001));
}

void RES_0_A(Core &c)
{
	c.r.a = c.r.a & ~(0b0000'0001);
}

void RES_1_B(Core &c)
{
	c.r.b = c.r.b & ~(0b0000'0010);
}

void RES_1_C(Core &c)
{
	c.r.c = c.r.c & ~(0b0000'0010);
}

void RES_1_D(Core &c)
{
	c.r.d = c.r.d & ~(0b0000'0010);
}

void RES_1_E(Core &c)
{
	c.r.e = c.r.e & ~(0b0000'0010);
}

void RES_1_H(Core &c)
{
	c.r.h = c.r.h & ~(0b0000'0010);
}

void RES_1_L(Core &c)
{
	c.r.l = c.r.l & ~(0b0000'0010);
}

void RES_1_AHL(Core &c)
{
	c.mmu.and8(c.r.hl, ~(0b0000'0010));
}

void RES_1_A(Core &c)
{
	c.r.a = c.r.a & ~(0b0000'0010);
}

void RES_2_B(Core &c)
{
	c.r.b = c.r.b & ~(0b0000'0100);
}

void RES_2_C(Core &c)
{
	c.r.c = c.r.c & ~(0b0000'0100);
}

void RES_2_D(Core &c)
{
	c.r.d = c.r.d & ~(0b0000'0100);
}

void RES_2_E(Core &c)
{
	c.r.e = c.r.e & ~(0b0000'0100);
}

void RES_2_H(Core &c)
{
	c.r.h = c.r.h & ~(0b0000'0100);
}

void RES_2_L(Core &c)
{
	c.r.l = c.r.l & ~(0b0000'0100);
}

void RES_2_AHL(Core &c)
{
	c.mmu.and8(c.r.hl, ~(0b0000'0100));
}

void RES_2_A(Core &c)
{
	c.r.a = c.r.a & ~(0b0000'0100);
}

void RES_3_B(Core &c)
{
	c.r.b = c.r.b & ~(0b0000'1000);
}

void RES_3_C(Core &c)
{
	c.r.c = c.r.c & ~(0b0000'1000);
}

void RES_3_D(Core &c)
{
	c.r.d = c.r.d & ~(0b0000'1000);
}

void RES_3_E(Core &c)
{
	c.r.e = c.r.e & ~(0b0000'1000);
}

void RES_3_H(Core &c)
{
	c.r.h = c.r.h & ~(0b0000'1000);
}

void RES_3_L(Core &c)
{
	c.r.l = c.r.l & ~(0b0000'1000);
}

void RES_3_AHL(Core &c)
{
	c.mmu.and8(c.r.hl, ~(0b0000'1000));
}

void RES_3_A(Core &c)
{
	c.r.a = c.r.a & ~(0b0000'1000);
}

void RES_4_B(Core &c)
{
	c.r.b = c.r.b & ~(0b0001'0000);
}

void RES_4_C(Core &c)
{
	c.r.c = c.r.c & ~(0b0001'0000);
}

void RES_4_D(Core &c)
{
	c.r.d = c.r.d & ~(0b0001'0000);
}

void RES_4_E(Core &c)
{
	c.r.e = c.r.e & ~(0b0001'0000);
}

void RES_4_H(Core &c)
{
	c.r.h = c.r.h & ~(0b0001'0000);
}

void RES_4_L(Core &c)
{
	c.r.l = c.r.l & ~(0b0001'0000);
}

void RES_4_AHL(Core &c)
{
	c.mmu.and8(c.r.hl, ~(0b0001'0000));
}

void RES_4_A(Core &c)
{
	c.r.a = c.r.a & ~(0b0001'0000);
}

void RES_5_B(Core &c)
{
	c.r.b = c.r.b & ~(0b0010'0000);
}

void RES_5_C(Core &c)
{
	c.r.c = c.r.c & ~(0b0010'0000);
}

void RES_5_D(Core &c)
{
	c.r.d = c.r.d & ~(0b0010'0000);
}

void RES_5_E(Core &c)
{
	c.r.e = c.r.e & ~(0b0010'0000);
}

void RES_5_H(Core &c)
{
	c.r.h = c.r.h & ~(0b0010'0000);
}

void RES_5_L(Core &c)
{
	c.r.l = c.r.l & ~(0b0010'0000);
}

void RES_5_AHL(Core &c)
{
	c.mmu.and8(c.r.hl, ~(0b0010'0000));
}

void RES_5_A(Core &c)
{
	c.r.a = c.r.a & ~(0b0010'0000);
}

void RES_6_B(Core &c)
{
	c.r.b = c.r.b & ~(0b0100'0000);
}

void RES_6_C(Core &c)
{
	c.r.c = c.r.c & ~(0b0100'0000);
}

void RES_6_D(Core &c)
{
	c.r.d = c.r.d & ~(0b0100'0000);
}

void RES_6_E(Core &c)
{
	c.r.e = c.r.e & ~(0b0100'0000);
}

void RES_6_H(Core &c)
{
	c.r.h = c.r.h & ~(0b0100'0000);
}

void RES_6_L(Core &c)
{
	c.r.l = c.r.l & ~(0b0100'0000);
}

void RES_6_AHL(Core &c)
{
	c.mmu.and8(c.r.hl, ~(0b0100'0000));
}

void RES_6_A(Core &c)
{
	c.r.a = c.r.a & ~(0b0100'0000);
}

void RES_7_B(Core &c)
{
	c.r.b = c.r.b & ~(0b1000'0000);
}

void RES_7_C(Core &c)
{
	c.r.c = c.r.c & ~(0b1000'0000);
}

void RES_7_D(Core &c)
{
	c.r.d = c.r.d & ~(0b1000'0000);
}

void RES_7_E(Core &c)
{
	c.r.e = c.r.e & ~(0b1000'0000);
}

void RES_7_H(Core &c)
{
	c.r.h = c.r.h & ~(0b1000'0000);
}

void RES_7_L(Core &c)
{
	c.r.l = c.r.l & ~(0b1000'0000);
}

void RES_7_AHL(Core &c)
{
	c.mmu.and8(c.r.hl, static_cast<uint8_t>(~0x80));
}

void RES_7_A(Core &c)
{
	c.r.a = c.r.a & ~(0b1000'0000);
}

void SET_0_B(Core &c)
{
	c.r.b |= 0b0000'0001;
}

void SET_0_C(Core &c)
{
	c.r.c |= 0b0000'0001;
}

void SET_0_D(Core &c)
{
	c.r.d |= 0b0000'0001;
}

void SET_0_E(Core &c)
{
	c.r.e |= 0b0000'0001;
}

void SET_0_H(Core &c)
{
	c.r.h |= 0b0000'0001;
}

void SET_0_L(Core &c)
{
	c.r.l |= 0b0000'0001;
}

void SET_0_AHL(Core &c)
{
	c.mmu.or8(c.r.hl, 0b0000'0001);
}

void SET_0_A(Core &c)
{
	c.r.a |= 0b0000'0001;
}

void SET_1_B(Core &c)
{
	c.r.b |= 0b0000'0010;
}

void SET_1_C(Core &c)
{
	c.r.c |= 0b0000'0010;
}

void SET_1_D(Core &c)
{
	c.r.d |= 0b0000'0010;
}

void SET_1_E(Core &c)
{
	c.r.e |= 0b0000'0010;
}

void SET_1_H(Core &c)
{
	c.r.h |= 0b0000'0010;
}

void SET_1_L(Core &c)
{
	c.r.l |= 0b0000'0010;
}

void SET_1_AHL(Core &c)
{
	c.mmu.or8(c.r.hl, 0b0000'0010);
}

void SET_1_A(Core &c)
{
	c.r.a |= 0b0000'0010;
}

void SET_2_B(Core &c)
{
	c.r.b |= 0b0000'0100;
}

void SET_2_C(Core &c)
{
	c.r.c |= 0b0000'0100;
}

void SET_2_D(Core &c)
{
	c.r.d |= 0b0000'0100;
}

void SET_2_E(Core &c)
{
	c.r.e |= 0b0000'0100;
}

void SET_2_H(Core &c)
{
	c.r.h |= 0b0000'0100;
}

void SET_2_L(Core &c)
{
	c.r.l |= 0b0000'0100;
}

void SET_2_AHL(Core &c)
{
	c.mmu.or8(c.r.hl, 0b0000'0100);
}

void SET_2_A(Core &c)
{
	c.r.a |= 0b0000'0100;
}

void SET_3_B(Core &c)
{
	c.r.b |= 0b0000'1000;
}

void SET_3_C(Core &c)
{
	c.r.c |= 0b0000'1000;
}

void SET_3_D(Core &c)
{
	c.r.d |= 0b0000'1000;
}

void SET_3_E(Core &c)
{
	c.r.e |= 0b0000'1000;
}

void SET_3_H(Core &c)
{
	c.r.h |= 0b0000'1000;
}

void SET_3_L(Core &c)
{
	c.r.l |= 0b0000'1000;
}

void SET_3_AHL(Core &c)
{
	c.mmu.or8(c.r.hl, 0b0000'1000);
}

void SET_3_A(Core &c)
{
	c.r.a |= 0b0000'1000;
}

void SET_4_B(Core &c)
{
	c.r.b |= 0b0001'0000;
}

void SET_4_C(Core &c)
{
	c.r.c |= 0b0001'0000;
}

void SET_4_D(Core &c)
{
	c.r.d |= 0b0001'0000;
}

void SET_4_E(Core &c)
{
	c.r.e |= 0b0001'0000;
}

void SET_4_H(Core &c)
{
	c.r.h |= 0b0001'0000;
}

void SET_4_L(Core &c)
{
	c.r.l |= 0b0001'0000;
}

void SET_4_AHL(Core &c)
{
	c.mmu.or8(c.r.hl, 0b0001'0000);
}

void SET_4_A(Core &c)
{
	c.r.a |= 0b0001'0000;
}

void SET_5_B(Core &c)
{
	c.r.b |= 0b0010'0000;
}

void SET_5_C(Core &c)
{
	c.r.c |= 0b0010'0000;
}

void SET_5_D(Core &c)
{
	c.r.d |= 0b0010'0000;
}

void SET_5_E(Core &c)
{
	c.r.e |= 0b0010'0000;
}

void SET_5_H(Core &c)
{
	c.r.h |= 0b0010'0000;
}

void SET_5_L(Core &c)
{
	c.r.l |= 0b0010'0000;
}

void SET_5_AHL(Core &c)
{
	c.mmu.or8(c.r.hl, 0b0010'0000);
}

void SET_5_A(Core &c)
{
	c.r.a |= 0b0010'0000;
}

void SET_6_B(Core &c)
{
	c.r.b |= 0b0100'0000;
}

void SET_6_C(Core &c)
{
	c.r.c |= 0b0100'0000;
}

void SET_6_D(Core &c)
{
	c.r.d |= 0b0100'0000;
}

void SET_6_E(Core &c)
{
	c.r.e |= 0b0100'0000;
}

void SET_6_H(Core &c)
{
	c.r.h |= 0b0100'0000;
}

void SET_6_L(Core &c)
{
	c.r.l |= 0b0100'0000;
}

void SET_6_AHL(Core &c)
{
	c.mmu.or8(c.r.hl, 0b0100'0000);
}

void SET_6_A(Core &c)
{
	c.r.a |= 0b0100'0000;
}

void SET_7_B(Core &c)
{
	c.r.b |= 0b1000'0000;
}

void SET_7_C(Core &c)
{
	c.r.c |= 0b1000'0000;
}

void SET_7_D(Core &c)
{
	c.r.d |= 0b1000'0000;
}

void SET_7_E(Core &c)
{
	c.r.e |= 0b1000'0000;
}

void SET_7_H(Core &c)
{
	c.r.h |= 0b1000'0000;
}

void SET_7_L(Core &c)
{
	c.r.l |= 0b1000'0000;
}

void SET_7_AHL(Core &c)
{
	c.mmu.or8(c.r.hl, 0b1000'0000);
}

void SET_7_A(Core &c)
{
	c.r.a |= 0b1000'0000;
}

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
