#include "cpu_base.h"
#include "cpu_instructions.h"
#include "cpu.h"
#include "mmu.h"

// Generic

void UNIMPL(CPU *cpu, instruction_t &i)
{
	cpu->doBreak();
}

void INVALID(CPU *cpu, instruction_t &i)
{
	cpu->doBreak();
}

void RST(CPU *cpu, uint16_t addr)
{
	cpu->sp -= 2;
	cpu->mmu->write16(cpu->sp, cpu->pc);
	cpu->pc = addr;
	cpu->ime = false;
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
	/* Shift register right into carry. MSB set to 0. */
	// Z00C
	uint8_t carry = Hi(r) & 0x1;
	uint8_t result = Hi(r) >> 1;
	uint8_t flags = 0;
	if (result == 0)
	{
		flags |= FLAGS_ZERO;
	}
	if (carry != 0)
	{
		flags |= FLAGS_CARRY;
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

void XOR_A_R(CPU *cpu, uint8_t r)
{
	// Z000
	SetHi(cpu->af, Hi(cpu->af) ^ r);
	SetLo(cpu->af, !Hi(cpu->af) ? FLAGS_ZERO : 0);
}

void AND_A_R(CPU *cpu, uint8_t r)
{
	// Z010
	SetHi(cpu->af, Hi(cpu->af) & r);
	SetLo(cpu->af, !Hi(cpu->af) ? (FLAGS_ZERO | FLAGS_HALFCARRY) : FLAGS_HALFCARRY);
}

void OR_A_R(CPU *cpu, uint8_t r)
{
	// Z000
	SetHi(cpu->af, Hi(cpu->af) | r);
	SetLo(cpu->af, !Hi(cpu->af) ? FLAGS_ZERO : 0);
}

uint8_t SLA_REG8(CPU *cpu, uint8_t r)
{
	// Z00C
	uint8_t c = r & 0b1000'0000;
	r <<= 1;
	uint8_t f = 0;
	if (!r)
		f |= FLAGS_ZERO;
	if (c)
		f |= FLAGS_CARRY;
	SetLo(cpu->af, f);
	return r;
}

uint8_t SRA_REG8(CPU *cpu, uint8_t r)
{
	// Z00C
	uint8_t msb = r & 0b1000'0000;
	uint8_t c = r & 0b0000'0001;
	r >>= 1;
	r |= msb;
	uint8_t flags = 0;
	if (!r)
		flags |= FLAGS_ZERO;
	if (c)
		flags |= FLAGS_CARRY;
	SetLo(cpu->af, flags);
	return r;
}

void ADC_A_R(CPU *cpu, uint8_t r)
{
	uint8_t carry = (cpu->af & FLAGS_CARRY) >> 4;
	uint8_t flags = 0;

	uint16_t a, r_, c;
	a = Hi(cpu->af);
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

	SetLo(cpu->af, flags);
	SetHi(cpu->af, result & 0xFF);
}

void SBC_A_R(CPU *cpu, uint8_t d)
{
	uint8_t c = (cpu->af & FLAGS_CARRY) >> 4;
	uint8_t a = Hi(cpu->af);
	uint8_t f = FLAGS_SUBTRACT;

	if ((int(Hi(cpu->af)) - int(d) - int(c)) < 0)
		f |= FLAGS_CARRY;

	if ((int(Hi(cpu->af) & 0xF) - int(d & 0xF) - int(c)) < 0)
		f |= FLAGS_HALFCARRY;

	SetHi(cpu->af, Hi(cpu->af) - d);
	SetHi(cpu->af, Hi(cpu->af) - c);

	if (!Hi(cpu->af))
		f |= FLAGS_ZERO;

	SetLo(cpu->af, f);
}

// Merges flags according to mask
void SetFlags(uint16_t &af, uint8_t flags, uint8_t mask)
{
	uint8_t f = Lo(af);
	SetLo(af, f ^ ((f ^ flags) & mask));
}

// 0x00

void NOP(CPU *cpu, instruction_t &i)
{
}

void LD_BC_IMM16(CPU *cpu, instruction_t &i)
{
	cpu->bc = i.op16;
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
	INC8_HI(cpu, cpu->bc);
}

void DEC_B(CPU *cpu, instruction_t &i)
{
	DEC8_HI(cpu, cpu->bc);
}

void LD_B_IMM8(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, i.op8);
}

// x = cpu.a.bit[7]
// carry = x
// cpu.a << 1
// cpu.a |= x
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
	uint8_t flags = 0;
	uint16_t result = 0;
	ADD16(cpu->hl, cpu->bc, &result, &flags);
	cpu->hl = result;
	flags &= 0b0111'0000;
	SetLo(cpu->af, (cpu->af & FLAGS_ZERO) | flags);
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
	INC8_LO(cpu, cpu->bc);
}

void DEC_C(CPU *cpu, instruction_t &i)
{
	DEC8_LO(cpu, cpu->bc);
}

void LD_C_IMM8(CPU *cpu, instruction_t &i)
{
	cpu->bc = (cpu->bc & 0xFF00) + i.op8;
}

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
	cpu->de = i.op16;
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
	INC8_HI(cpu, cpu->de);
}

void DEC_D(CPU *cpu, instruction_t &i)
{
	DEC8_HI(cpu, cpu->de);
}

void LD_D_IMM8(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, i.op8);
}

void RLA(CPU *cpu, instruction_t &i)
{
	uint8_t flags = Lo(cpu->af);
	uint8_t result = 0;
	RL(Hi(cpu->af), &result, &flags);
	SetHi(cpu->af, result);
	SetLo(cpu->af, flags & 0b0111'0000);
}

void JR_IMM8(CPU *cpu, instruction_t &i)
{
	cpu->pc += int8_t(i.op8);
}

void ADD_HL_DE(CPU *cpu, instruction_t &i)
{
	// -0HC
	uint8_t flags = 0;
	uint16_t result = 0;
	ADD16(cpu->hl, cpu->de, &result, &flags);
	cpu->hl = result;
	SetLo(cpu->af, (flags & 0x70) | (Lo(cpu->af) & 0x80));
}

void LD_A_ADE(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, cpu->mmu->read8(cpu->de));
}

void DEC_DE(CPU *cpu, instruction_t &i)
{
	cpu->de--;
}

void INC_E(CPU *cpu, instruction_t &i)
{
	INC8_LO(cpu, cpu->de);
}

void DEC_E(CPU *cpu, instruction_t &i)
{
	DEC8_LO(cpu, cpu->de);
}

void LD_E_IMM8(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, i.op8);
}

void RRA(CPU *cpu, instruction_t &i)
{
	uint8_t flags = Lo(cpu->af);
	uint8_t result = 0;
	RR(Hi(cpu->af), &result, &flags);
	SetLo(cpu->af, flags & FLAGS_CARRY);
	SetHi(cpu->af, result);
}

// 0x20

void JR_NZ_IMM8(CPU *cpu, instruction_t &i)
{
	if (!(cpu->af & FLAGS_ZERO))
	{
		cpu->pc += int8_t(i.op8);
	}
	else
	{
		i.didAction = false;
	}
}

void LD_HL_IMM16(CPU *cpu, instruction_t &i)
{
	cpu->hl = i.op16;
}

void LDI_HL_A(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(cpu->hl++, Hi(cpu->af));
}

void INC_H(CPU *cpu, instruction_t &i)
{
	INC8_HI(cpu, cpu->hl);
}

void DEC_H(CPU *cpu, instruction_t &i)
{
	DEC8_HI(cpu, cpu->hl);
}

void LD_H_IMM8(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, i.op8);
}

void DAA(CPU *cpu, instruction_t &i)
{
	uint8_t a = Hi(cpu->af);
	uint8_t f = Lo(cpu->af);

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

	SetHi(cpu->af, a);
	SetLo(cpu->af, f);
}

void JR_Z_IMM8(CPU *cpu, instruction_t &i)
{
	if (cpu->af & FLAGS_ZERO)
	{
		cpu->pc += int8_t(i.op8);
	}
	else
	{
		i.didAction = false;
	}
}

void INC_HL(CPU *cpu, instruction_t &i)
{
	cpu->hl++;
}

void ADD_HL_HL(CPU *cpu, instruction_t &i)
{
	// -0HC
	uint8_t flags = 0;
	uint16_t result = 0;
	ADD16(cpu->hl, cpu->hl, &result, &flags);
	cpu->hl = result;
	SetLo(cpu->af, (flags & 0x70) | (Lo(cpu->af) & 0x80));
}

void LDI_A_HL(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, cpu->mmu->read8(cpu->hl));
	cpu->hl++;
}

void DEC_HL(CPU *cpu, instruction_t &i)
{
	cpu->hl--;
}

void INC_L(CPU *cpu, instruction_t &i)
{
	INC8_LO(cpu, cpu->hl);
}

void DEC_L(CPU *cpu, instruction_t &i)
{
	DEC8_LO(cpu, cpu->hl);
}

void LD_L_IMM8(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, i.op8);
}

void CPL(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, ~Hi(cpu->af));
	uint8_t old_flags = Lo(cpu->af);
	uint8_t new_flags = (old_flags & (FLAGS_ZERO | FLAGS_CARRY)) | (FLAGS_SUBTRACT | FLAGS_HALFCARRY);
	SetLo(cpu->af, new_flags);
}

// 0x30

void JR_NC_IMM8(CPU *cpu, instruction_t &i)
{
	if (!(cpu->af & FLAGS_CARRY))
	{
		cpu->pc += int8_t(i.op8);
	}
	else
	{
		i.didAction = false;
	}
}

void LD_SP_IMM16(CPU *cpu, instruction_t &i)
{
	cpu->sp = i.op16;
}

void LDD_HL_A(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(cpu->hl, Hi(cpu->af));
	cpu->hl--;
}

void INC_SP(CPU *cpu, instruction_t &i)
{
	cpu->sp++;
}

void INC_AHL(CPU *cpu, instruction_t &i)
{
	uint8_t flags = Lo(cpu->af);
	uint8_t result = 0;
	INC8(cpu->mmu->read8(cpu->hl), &result, &flags);
	cpu->mmu->write8(cpu->hl, result);
	SetLo(cpu->af, flags);
}

void DEC_AHL(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	SUB8(cpu->mmu->read8(cpu->hl), 1, &result, &flags);
	cpu->mmu->write8(cpu->hl, result);
	SetLo(cpu->af, (flags & 0b1110'0000) | (cpu->af & 0x10));
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
	if (cpu->af & FLAGS_CARRY)
	{
		cpu->pc += int8_t(i.op8);
	}
	else
	{
		i.didAction = false;
	}
}

void ADD_HL_SP(CPU *cpu, instruction_t &i)
{
	// -0HC
	uint8_t flags = 0;
	uint16_t result = 0;
	ADD16(cpu->hl, cpu->sp, &result, &flags);
	cpu->hl = result;
	flags &= 0b0011'0000;
	SetLo(cpu->af, (Lo(cpu->af) & 0b1000'0000) | flags);
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
	INC8_HI(cpu, cpu->af);
}

void DEC_A(CPU *cpu, instruction_t &i)
{
	DEC8_HI(cpu, cpu->af);
}

void LD_A_IMM8(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, i.op8);
}

void CCF(CPU *cpu, instruction_t &i)
{
	uint8_t old_flags = Lo(cpu->af);
	uint8_t new_flags = (old_flags & FLAGS_ZERO) | ((~(old_flags & FLAGS_CARRY)) & FLAGS_CARRY);
	SetLo(cpu->af, new_flags);
}

// 0x40

void LD_B_B(CPU *cpu, instruction_t &i)
{
	// SetHi(cpu->bc, Hi(cpu->bc));
}

void LD_B_C(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, Lo(cpu->bc));
}

void LD_B_D(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, Hi(cpu->de));
}

void LD_B_E(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, Lo(cpu->de));
}

void LD_B_H(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, Hi(cpu->hl));
}

void LD_B_L(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, Lo(cpu->hl));
}

void LD_B_AHL(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, cpu->mmu->read8(cpu->hl));
}

void LD_B_A(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, Hi(cpu->af));
}

void LD_C_B(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, Hi(cpu->bc));
}

void LD_C_C(CPU *cpu, instruction_t &i)
{
	// SetLo(cpu->bc, Lo(cpu->bc));
}

void LD_C_D(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, Hi(cpu->de));
}

void LD_C_E(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, Lo(cpu->de));
}

void LD_C_H(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, Hi(cpu->hl));
}

void LD_C_L(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, Lo(cpu->hl));
}

void LD_C_AHL(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, cpu->mmu->read8(cpu->hl));
}

void LD_C_A(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, Hi(cpu->af));
}

// 0x50

void LD_D_B(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Hi(cpu->bc));
}

void LD_D_C(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Lo(cpu->bc));
}

void LD_D_D(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Hi(cpu->de));
}

void LD_D_E(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Lo(cpu->de));
}

void LD_D_H(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Hi(cpu->hl));
}

void LD_D_L(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Lo(cpu->hl));
}

void LD_D_AHL(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, cpu->mmu->read8(cpu->hl));
}

void LD_D_A(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Hi(cpu->af));
}

void LD_E_B(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, Hi(cpu->bc));
}

void LD_E_C(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, Lo(cpu->bc));
}

void LD_E_D(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, Hi(cpu->de));
}

void LD_E_E(CPU *cpu, instruction_t &i)
{
	// SetLo(cpu->de, Lo(cpu->de));
}

void LD_E_H(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, Hi(cpu->hl));
}

void LD_E_L(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, Lo(cpu->hl));
}

void LD_E_AHL(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, cpu->mmu->read8(cpu->hl));
}

void LD_E_A(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, Hi(cpu->af));
}

// 0x60

void LD_H_B(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, Hi(cpu->bc));
}

void LD_H_C(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, Lo(cpu->bc));
}

void LD_H_D(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, Hi(cpu->de));
}

void LD_H_E(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, Lo(cpu->de));
}

void LD_H_H(CPU *cpu, instruction_t &i)
{
	// SetHi(cpu->hl, Hi(cpu->hl));
}

void LD_H_L(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, Lo(cpu->hl));
}

void LD_H_AHL(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, cpu->mmu->read8(cpu->hl));
}

void LD_H_A(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, Hi(cpu->af));
}

void LD_L_B(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, Hi(cpu->bc));
}

void LD_L_C(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, Lo(cpu->bc));
}

void LD_L_D(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, Hi(cpu->de));
}

void LD_L_E(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, Lo(cpu->de));
}

void LD_L_H(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, Hi(cpu->hl));
}

void LD_L_L(CPU *cpu, instruction_t &i)
{
	// SetLo(cpu->hl, Hi(cpu->hl));
}

void LD_L_AHL(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, cpu->mmu->read8(cpu->hl));
}

void LD_L_A(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, Hi(cpu->af));
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
	cpu->halt();
}

void LD_AHL_A(CPU *cpu, instruction_t &i)
{
	cpu->mmu->write8(cpu->hl, Hi(cpu->af));
}

void LD_A_B(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, Hi(cpu->bc));
}

void LD_A_C(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, Lo(cpu->bc));
}

void LD_A_D(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, Hi(cpu->de));
}

void LD_A_E(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, Lo(cpu->de));
}

void LD_A_H(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, Hi(cpu->hl));
}

void LD_A_L(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, Lo(cpu->hl));
}

void LD_A_AHL(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, cpu->mmu->read8(cpu->hl));
}

void LD_A_A(CPU *cpu, instruction_t &i)
{
	// SetHi(cpu->af, Hi(cpu->af));
}

// 0x80

void ADD_A_B(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	ADD8(Hi(cpu->af), Hi(cpu->bc), &result, &flags);
	SetHi(cpu->af, result);
	SetLo(cpu->af, flags);
}

void ADD_A_C(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	ADD8(Hi(cpu->af), Lo(cpu->bc), &result, &flags);
	SetHi(cpu->af, result);
	SetLo(cpu->af, flags);
}

void ADD_A_D(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	ADD8(Hi(cpu->af), Hi(cpu->de), &result, &flags);
	SetHi(cpu->af, result);
	SetLo(cpu->af, flags);
}

void ADD_A_E(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	ADD8(Hi(cpu->af), Lo(cpu->de), &result, &flags);
	SetHi(cpu->af, result);
	SetLo(cpu->af, flags);
}

void ADD_A_H(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	ADD8(Hi(cpu->af), Hi(cpu->hl), &result, &flags);
	SetHi(cpu->af, result);
	SetLo(cpu->af, flags);
}

void ADD_A_L(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	ADD8(Hi(cpu->af), Lo(cpu->hl), &result, &flags);
	SetHi(cpu->af, result);
	SetLo(cpu->af, flags);
}

void ADD_A_AHL(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	ADD8(Hi(cpu->af), cpu->mmu->read8(cpu->hl), &result, &flags);
	SetHi(cpu->af, result);
	SetLo(cpu->af, flags);
}

void ADD_A_A(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	ADD8(Hi(cpu->af), Hi(cpu->af), &result, &flags);
	SetHi(cpu->af, result);
	SetLo(cpu->af, flags);
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
	ADC_A_R(cpu, Lo(cpu->hl));
}

void ADC_A_AHL(CPU *cpu, instruction_t &i)
{
	ADC_A_R(cpu, cpu->mmu->read8(cpu->hl));
}

void ADC_A_A(CPU *cpu, instruction_t &i)
{
	ADC_A_R(cpu, Hi(cpu->af));
}

// 0x90

void SUB_A_B(CPU *cpu, instruction_t &i)
{
	SUB(cpu, Hi(cpu->bc));
}

void SUB_A_C(CPU *cpu, instruction_t &i)
{
	SUB(cpu, Lo(cpu->bc));
}

void SUB_A_D(CPU *cpu, instruction_t &i)
{
	SUB(cpu, Hi(cpu->de));
}

void SUB_A_E(CPU *cpu, instruction_t &i)
{
	SUB(cpu, Lo(cpu->de));
}

void SUB_A_H(CPU *cpu, instruction_t &i)
{
	SUB(cpu, Hi(cpu->hl));
}

void SUB_A_L(CPU *cpu, instruction_t &i)
{
	SUB(cpu, Lo(cpu->hl));
}

void SUB_A_AHL(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	SUB8(Hi(cpu->af), cpu->mmu->read8(cpu->hl), &result, &flags);
	SetLo(cpu->af, flags);
	SetHi(cpu->af, result);
}

void SUB_A_A(CPU *cpu, instruction_t &i)
{
	SUB(cpu, Hi(cpu->af));
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
	CP(cpu, Hi(cpu->bc));
}

void CP_A_C(CPU *cpu, instruction_t &i)
{
	CP(cpu, Lo(cpu->bc));
}

void CP_A_D(CPU *cpu, instruction_t &i)
{
	CP(cpu, Hi(cpu->de));
}

void CP_A_E(CPU *cpu, instruction_t &i)
{
	CP(cpu, Lo(cpu->de));
}

void CP_A_H(CPU *cpu, instruction_t &i)
{
	CP(cpu, Hi(cpu->hl));
}

void CP_A_L(CPU *cpu, instruction_t &i)
{
	CP(cpu, Lo(cpu->hl));
}

void CP_A_AHL(CPU *cpu, instruction_t &i)
{
	CP(cpu, cpu->mmu->read8(cpu->hl));
}

void CP_A_A(CPU *cpu, instruction_t &i)
{
	CP(cpu, Hi(cpu->af));
}

// 0xC0

void RET_NZ(CPU *cpu, instruction_t &i)
{
	if (!(cpu->af & FLAGS_ZERO))
	{
		cpu->pc = cpu->mmu->read16(cpu->sp);
		cpu->sp += 2;
	}
	else
	{
		i.didAction = false;
	}
}

void POP_BC(CPU *cpu, instruction_t &i)
{
	cpu->bc = cpu->mmu->read16(cpu->sp);
	cpu->sp += 2;
}

void JP_NZ_IMM16(CPU *cpu, instruction_t &i)
{
	if (!(cpu->af & FLAGS_ZERO))
	{
		cpu->pc = i.op16;
	}
	else
	{
		i.didAction = false;
	}
}

void JP_IMM16(CPU *cpu, instruction_t &i)
{
	cpu->pc = i.op16;
}

void CALL_NZ_IMM16(CPU *cpu, instruction_t &i)
{
	if (!(cpu->af & FLAGS_ZERO))
	{
		cpu->sp -= 2;
		cpu->mmu->write16(cpu->sp, cpu->pc);
		cpu->pc = i.op16;
	}
	else
	{
		i.didAction = false;
	}
}

void PUSH_BC(CPU *cpu, instruction_t &i)
{
	cpu->sp -= 2;
	cpu->mmu->write16(cpu->sp, cpu->bc);
}

void ADD_A_IMM8(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	ADD8(Hi(cpu->af), i.op8, &result, &flags);
	SetHi(cpu->af, result);
	SetLo(cpu->af, flags);
}

void RST_00(CPU *cpu, instruction_t &i)
{
	RST(cpu, 0x00);
}

void RET_Z(CPU *cpu, instruction_t &i)
{
	if (cpu->af & FLAGS_ZERO)
	{
		cpu->pc = cpu->mmu->read16(cpu->sp);
		cpu->sp += 2;
	}
	else
	{
		i.didAction = false;
	}
}

void RET(CPU *cpu, instruction_t &i)
{
	cpu->pc = cpu->mmu->read16(cpu->sp);
	cpu->sp += 2;
}

void JP_Z_IMM16(CPU *cpu, instruction_t &i)
{
	if (cpu->af & FLAGS_ZERO)
	{
		cpu->pc = i.op16;
	}
	else
	{
		i.didAction = false;
	}
}

void CB(CPU *cpu, instruction_t &i)
{
}

void CALL_Z_IMM16(CPU *cpu, instruction_t &i)
{
	if (cpu->af & FLAGS_ZERO)
	{
		cpu->sp -= 2;
		cpu->mmu->write16(cpu->sp, cpu->pc);
		cpu->pc = i.op16;
	}
	else
	{
		i.didAction = false;
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
	if (!(cpu->af & FLAGS_CARRY))
	{
		cpu->pc = cpu->mmu->read16(cpu->sp);
		cpu->sp += 2;
	}
	else
	{
		i.didAction = false;
	}
}

void POP_DE(CPU *cpu, instruction_t &i)
{
	cpu->de = cpu->mmu->read16(cpu->sp);
	cpu->sp += 2;
}

void JP_NC_IMM16(CPU *cpu, instruction_t &i)
{
	if (!(cpu->af & FLAGS_CARRY))
	{
		cpu->pc = i.op16;
	}
	else
	{
		i.didAction = false;
	}
}

void CALL_NC_IMM16(CPU *cpu, instruction_t &i)
{
	if (!(cpu->af & FLAGS_CARRY))
	{
		cpu->sp -= 2;
		cpu->mmu->write16(cpu->sp, cpu->pc);
		cpu->pc = i.op16;
	}
	else
	{
		i.didAction = false;
	}
}

void PUSH_DE(CPU *cpu, instruction_t &i)
{
	cpu->sp -= 2;
	cpu->mmu->write16(cpu->sp, cpu->de);
}

void RET_C(CPU *cpu, instruction_t &i)
{
	if (cpu->af & FLAGS_CARRY)
	{
		cpu->pc = cpu->mmu->read16(cpu->sp);
		cpu->sp += 2;
	}
	else
	{
		i.didAction = false;
	}
}

void RETI(CPU* cpu, instruction_t& i)
{
	cpu->ime = true;
	cpu->pc = cpu->mmu->read16(cpu->sp);
	cpu->sp += 2;
}

void JP_C_IMM16(CPU* cpu, instruction_t& i)
{
	if (cpu->af & FLAGS_CARRY)
	{
		cpu->pc = i.op16;
	}
	else
	{
		i.didAction = false;
	}
}

void SUB_A_IMM8(CPU* cpu, instruction_t& i)
{
	SUB(cpu, i.op8);
}

void RST_10(CPU* cpu, instruction_t& i)
{
	RST(cpu, 0x10);
}

void CALL_C_IMM16(CPU* cpu, instruction_t& i)
{
	if (cpu->af & FLAGS_CARRY)
	{
		cpu->sp -= 2;
		cpu->mmu->write16(cpu->sp, cpu->pc);
		cpu->pc = i.op16;
	}
	else
	{
		i.didAction = false;
	}
}

void SBC_A_IMM8(CPU* cpu, instruction_t& i)
{
	SBC_A_R(cpu, i.op8);
}

void RST_18(CPU* cpu, instruction_t& i)
{
	RST(cpu, 0x18);
}

// 0xE0

void LDH_IMM8_A(CPU* cpu, instruction_t& i)
{
	cpu->mmu->write8(0xFF00 + i.op8, Hi(cpu->af));
}

void POP_HL(CPU* cpu, instruction_t& i)
{
	cpu->hl = cpu->mmu->read16(cpu->sp);
	cpu->sp += 2;
}

void LDH_C_A(CPU* cpu, instruction_t& i)
{
	cpu->mmu->write8(0xFF00 + Lo(cpu->bc), Hi(cpu->af));
}

void PUSH_HL(CPU* cpu, instruction_t& i)
{
	cpu->sp -= 2;
	cpu->mmu->write16(cpu->sp, cpu->hl);
}

void AND_A_IMM8(CPU* cpu, instruction_t& i)
{
	AND_A_R(cpu, i.op8);
}

void RST_20(CPU* cpu, instruction_t& i)
{
	RST(cpu, 0x20);
}

void ADD_SP_IMM8(CPU* cpu, instruction_t& i)
{
	// 00HC
	uint8_t flags = 0;
	
	if (((cpu->sp & 0xF) + (i.op8 & 0xF)) & 0x10)
		flags |= FLAGS_HALFCARRY;

	if (((cpu->sp & 0xFF) + i.op8) > 0xFF)
		flags |= FLAGS_CARRY;

	cpu->sp = cpu->sp + int8_t(i.op8);
	SetLo(cpu->af, flags);
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
	cpu->af = cpu->mmu->read16(cpu->sp) & 0xFFF0;
	cpu->sp += 2;
}

void LDH_A_C(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, cpu->mmu->read8(0xFF00 + Lo(cpu->bc)));
}

void DI(CPU *cpu, instruction_t &i)
{
	cpu->ime = false;
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
	uint8_t flags = 0;

	int8_t s8 = i.op8;
	uint16_t sp = cpu->sp + s8;
	if (s8 > 0)
	{
		if (((cpu->sp & 0xFF) + s8) > 0xFF)
		{
			flags |= FLAGS_CARRY;
		}
		if (((cpu->sp & 0xF) + (s8 & 0xF)) > 0xF)
		{
			flags |= FLAGS_HALFCARRY;
		}
	}
	else
	{
		if ((sp & 0xFF) < (cpu->sp & 0xFF))
		{
			flags |= FLAGS_CARRY;
		}
		if ((sp & 0xF) < (cpu->sp & 0xF))
		{
			flags |= FLAGS_HALFCARRY;
		}
	}

	SetLo(cpu->af, flags);

	cpu->hl = sp;
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
	cpu->enableInterrupts();
}

void CP_A_IMM8(CPU *cpu, instruction_t &i)
{
	CP(cpu, i.op8);
}

void RST_38(CPU *cpu, instruction_t &i)
{
	RST(cpu, 0x38);
}

// 0xCB // Extended

// 0xCB 0x00

void RLC_B(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RLC(Hi(cpu->bc), &result, &flags);
	SetHi(cpu->bc, result);
	SetLo(cpu->af, flags);
}

void RLC_C(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RLC(Lo(cpu->bc), &result, &flags);
	SetLo(cpu->bc, result);
	SetLo(cpu->af, flags);
}

void RLC_D(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RLC(Hi(cpu->de), &result, &flags);
	SetHi(cpu->de, result);
	SetLo(cpu->af, flags);
}

void RLC_E(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RLC(Lo(cpu->de), &result, &flags);
	SetLo(cpu->de, result);
	SetLo(cpu->af, flags);
}

void RLC_H(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RLC(Hi(cpu->hl), &result, &flags);
	SetHi(cpu->hl, result);
	SetLo(cpu->af, flags);
}

void RLC_L(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RLC(Lo(cpu->hl), &result, &flags);
	SetLo(cpu->hl, result);
	SetLo(cpu->af, flags);
}

void RLC_AHL(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RLC(cpu->mmu->read8(cpu->hl), &result, &flags);
	cpu->mmu->write8(cpu->hl, result);
	SetLo(cpu->af, flags);
}

void RLC_A(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RLC(Hi(cpu->af), &result, &flags);
	SetHi(cpu->af, result);
	SetLo(cpu->af, flags);
}

void RRC_B(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RRC(Hi(cpu->bc), &result, &flags);
	SetHi(cpu->bc, result);
	SetLo(cpu->af, flags);
}

void RRC_C(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RRC(Lo(cpu->bc), &result, &flags);
	SetLo(cpu->bc, result);
	SetLo(cpu->af, flags);
}

void RRC_D(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RRC(Hi(cpu->de), &result, &flags);
	SetHi(cpu->de, result);
	SetLo(cpu->af, flags);
}

void RRC_E(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RRC(Lo(cpu->de), &result, &flags);
	SetLo(cpu->de, result);
	SetLo(cpu->af, flags);
}

void RRC_H(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RRC(Hi(cpu->hl), &result, &flags);
	SetHi(cpu->hl, result);
	SetLo(cpu->af, flags);
}

void RRC_L(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RRC(Lo(cpu->hl), &result, &flags);
	SetLo(cpu->hl, result);
	SetLo(cpu->af, flags);
}

void RRC_AHL(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RRC(cpu->mmu->read8(cpu->hl), &result, &flags);
	cpu->mmu->write8(cpu->hl, result);
	SetLo(cpu->af, flags);
}

void RRC_A(CPU *cpu, instruction_t &i)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	RRC(Hi(cpu->af), &result, &flags);
	SetHi(cpu->af, result);
	SetLo(cpu->af, flags);
}

// 0xCB 0x10

void RL_B(CPU *cpu, instruction_t &i)
{
	uint8_t flags = cpu->af & 0x10;
	uint8_t result = 0;
	RL(Hi(cpu->bc), &result, &flags);
	SetHi(cpu->bc, result);
	SetLo(cpu->af, flags);
}

void RL_C(CPU *cpu, instruction_t &i)
{
	uint8_t flags = cpu->af & 0x10;
	uint8_t result = 0;
	RL(Lo(cpu->bc), &result, &flags);
	SetLo(cpu->bc, result);
	SetLo(cpu->af, flags);
}

void RL_D(CPU *cpu, instruction_t &i)
{
	uint8_t flags = cpu->af & 0x10;
	uint8_t result = 0;
	RL(Hi(cpu->de), &result, &flags);
	SetHi(cpu->de, result);
	SetLo(cpu->af, flags);
}

void RL_E(CPU *cpu, instruction_t &i)
{
	uint8_t flags = cpu->af & 0x10;
	uint8_t result = 0;
	RL(Lo(cpu->de), &result, &flags);
	SetLo(cpu->de, result);
	SetLo(cpu->af, flags);
}

void RL_H(CPU *cpu, instruction_t &i)
{
	uint8_t flags = cpu->af & 0x10;
	uint8_t result = 0;
	RL(Hi(cpu->hl), &result, &flags);
	SetHi(cpu->hl, result);
	SetLo(cpu->af, flags);
}

void RL_L(CPU *cpu, instruction_t &i)
{
	uint8_t flags = cpu->af & 0x10;
	uint8_t result = 0;
	RL(Lo(cpu->hl), &result, &flags);
	SetLo(cpu->hl, result);
	SetLo(cpu->af, flags);
}

void RL_AHL(CPU *cpu, instruction_t &i)
{
	uint8_t flags = cpu->af & 0x10;
	uint8_t result = 0;
	RL(cpu->mmu->read8(cpu->hl), &result, &flags);
	cpu->mmu->write8(cpu->hl, result);
	SetLo(cpu->af, flags);
}

void RL_A(CPU *cpu, instruction_t &i)
{
	uint8_t flags = cpu->af & 0x10;
	uint8_t result = 0;
	RL(Hi(cpu->af), &result, &flags);
	SetHi(cpu->af, result);
	SetLo(cpu->af, flags);
}

void RR_B(CPU *cpu, instruction_t &i)
{
	uint8_t flags = cpu->af & 0x10;
	uint8_t result = 0;
	RR(Hi(cpu->bc), &result, &flags);
	SetHi(cpu->bc, result);
	SetLo(cpu->af, flags);
}

void RR_C(CPU *cpu, instruction_t &i)
{
	uint8_t flags = cpu->af & 0x10;
	uint8_t result = 0;
	RR(Lo(cpu->bc), &result, &flags);
	SetLo(cpu->bc, result);
	SetLo(cpu->af, flags);
}

void RR_D(CPU *cpu, instruction_t &i)
{
	uint8_t flags = cpu->af & 0x10;
	uint8_t result = 0;
	RR(Hi(cpu->de), &result, &flags);
	SetHi(cpu->de, result);
	SetLo(cpu->af, flags);
}

void RR_E(CPU *cpu, instruction_t &i)
{
	uint8_t flags = cpu->af & 0x10;
	uint8_t result = 0;
	RR(Lo(cpu->de), &result, &flags);
	SetLo(cpu->de, result);
	SetLo(cpu->af, flags);
}

void RR_H(CPU *cpu, instruction_t &i)
{
	uint8_t flags = cpu->af & 0x10;
	uint8_t result = 0;
	RR(Hi(cpu->hl), &result, &flags);
	SetHi(cpu->hl, result);
	SetLo(cpu->af, flags);
}

void RR_L(CPU *cpu, instruction_t &i)
{
	uint8_t flags = cpu->af & 0x10;
	uint8_t result = 0;
	RR(Lo(cpu->hl), &result, &flags);
	SetLo(cpu->hl, result);
	SetLo(cpu->af, flags);
}

void RR_AHL(CPU *cpu, instruction_t &i)
{
	uint8_t flags = cpu->af & 0x10;
	uint8_t result = 0;
	RR(cpu->mmu->read8(cpu->hl), &result, &flags);
	cpu->mmu->write8(cpu->hl, result);
	SetLo(cpu->af, flags);
}

void RR_A(CPU *cpu, instruction_t &i)
{
	uint8_t flags = cpu->af & 0x10;
	uint8_t result = 0;
	RR(Hi(cpu->af), &result, &flags);
	SetHi(cpu->af, result);
	SetLo(cpu->af, flags);
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
	uint8_t r = cpu->mmu->read8(cpu->hl);
	uint8_t c = r & 0b0000'0001;
	uint8_t f = 0;
	r >>= 1;
	cpu->mmu->write8(cpu->hl, r);
	if (!r)
		f |= FLAGS_ZERO;
	f |= (c << 4);
	SetLo(cpu->af, f);
}

void SRL_A(CPU *cpu, instruction_t &i)
{
	SRL_RH(cpu, cpu->af);
}

// 0xCB 0x40

void BIT_0_B(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->bc) & 0b0000'0001);
}

void BIT_0_C(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->bc) & 0b0000'0001);
}

void BIT_0_D(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->de) & 0b0000'0001);
}

void BIT_0_E(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->de) & 0b0000'0001);
}

void BIT_0_H(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->hl) & 0b0000'0001);
}

void BIT_0_L(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->hl) & 0b0000'0001);
}

void BIT_0_AHL(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, cpu->mmu->read8(cpu->hl) & 0b0000'0001);
}

void BIT_0_A(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->af) & 0b0000'0001);
}

void BIT_1_B(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->bc) & 0b0000'0010);
}

void BIT_1_C(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->bc) & 0b0000'0010);
}

void BIT_1_D(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->de) & 0b0000'0010);
}

void BIT_1_E(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->de) & 0b0000'0010);
}

void BIT_1_H(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->hl) & 0b0000'0010);
}

void BIT_1_L(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->hl) & 0b0000'0010);
}

void BIT_1_AHL(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, cpu->mmu->read8(cpu->hl) & 0b0000'0010);
}

void BIT_1_A(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->af) & 0b0000'0010);
}

void BIT_2_B(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->bc) & 0b0000'0100);
}

void BIT_2_C(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->bc) & 0b0000'0100);
}

void BIT_2_D(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->de) & 0b0000'0100);
}

void BIT_2_E(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->de) & 0b0000'0100);
}

void BIT_2_H(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->hl) & 0b0000'0100);
}

void BIT_2_L(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->hl) & 0b0000'0100);
}

void BIT_2_AHL(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, cpu->mmu->read8(cpu->hl) & 0b0000'0100);
}

void BIT_2_A(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->af) & 0b0000'0100);
}

void BIT_3_B(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->bc) & 0b0000'1000);
}

void BIT_3_C(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->bc) & 0b0000'1000);
}

void BIT_3_D(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->de) & 0b0000'1000);
}

void BIT_3_E(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->de) & 0b0000'1000);
}

void BIT_3_H(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->hl) & 0b0000'1000);
}

void BIT_3_L(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->hl) & 0b0000'1000);
}

void BIT_3_AHL(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, cpu->mmu->read8(cpu->hl) & 0b0000'1000);
}

void BIT_3_A(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->af) & 0b0000'1000);
}

void BIT_4_B(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->bc) & 0b0001'0000);
}

void BIT_4_C(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->bc) & 0b0001'0000);
}

void BIT_4_D(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->de) & 0b0001'0000);
}

void BIT_4_E(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->de) & 0b0001'0000);
}

void BIT_4_H(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->hl) & 0b0001'0000);
}

void BIT_4_L(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->hl) & 0b0001'0000);
}

void BIT_4_AHL(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, cpu->mmu->read8(cpu->hl) & 0b0001'0000);
}

void BIT_4_A(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->af) & 0b0001'0000);
}

void BIT_5_B(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->bc) & 0b0010'0000);
}

void BIT_5_C(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->bc) & 0b0010'0000);
}

void BIT_5_D(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->de) & 0b0010'0000);
}

void BIT_5_E(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->de) & 0b0010'0000);
}

void BIT_5_H(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->hl) & 0b0010'0000);
}

void BIT_5_L(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->hl) & 0b0010'0000);
}

void BIT_5_AHL(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, cpu->mmu->read8(cpu->hl) & 0b0010'0000);
}

void BIT_5_A(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->af) & 0b0010'0000);
}

void BIT_6_B(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->bc) & 0b0100'0000);
}

void BIT_6_C(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->bc) & 0b0100'0000);
}

void BIT_6_D(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->de) & 0b0100'0000);
}

void BIT_6_E(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->de) & 0b0100'0000);
}

void BIT_6_H(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->hl) & 0b0100'0000);
}

void BIT_6_L(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->hl) & 0b0100'0000);
}

void BIT_6_AHL(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, cpu->mmu->read8(cpu->hl) & 0b0100'0000);
}

void BIT_6_A(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->af) & 0b0100'0000);
}

void BIT_7_B(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->bc) & 0b1000'0000);
}

void BIT_7_C(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->bc) & 0b1000'0000);
}

void BIT_7_D(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->de) & 0b1000'0000);
}

void BIT_7_E(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->de) & 0b1000'0000);
}

void BIT_7_H(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->hl) & 0b1000'0000);
}

void BIT_7_L(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Lo(cpu->hl) & 0b1000'0000);
}

void BIT_7_AHL(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, cpu->mmu->read8(cpu->hl) & 0b1000'0000);
}

void BIT_7_A(CPU *cpu, instruction_t &i)
{
	TestBit(cpu, Hi(cpu->af) & 0b1000'0000);
}

void RES_0_B(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, Hi(cpu->bc) & ~(0b0000'0001));
}

void RES_0_C(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, Lo(cpu->bc) & ~(0b0000'0001));
}

void RES_0_D(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Hi(cpu->de) & ~(0b0000'0001));
}

void RES_0_E(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, Lo(cpu->de) & ~(0b0000'0001));
}

void RES_0_H(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, Hi(cpu->hl) & ~(0b0000'0001));
}

void RES_0_L(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, Lo(cpu->hl) & ~(0b0000'0001));
}

void RES_0_AHL(CPU *cpu, instruction_t &i)
{
	cpu->mmu->and8(cpu->hl, ~(0b0000'0001));
}

void RES_0_A(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, Hi(cpu->af) & ~(0b0000'0001));
}

void RES_1_B(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, Hi(cpu->bc) & ~(0b0000'0010));
}

void RES_1_C(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, Lo(cpu->bc) & ~(0b0000'0010));
}

void RES_1_D(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Hi(cpu->de) & ~(0b0000'0010));
}

void RES_1_E(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, Lo(cpu->de) & ~(0b0000'0010));
}

void RES_1_H(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, Hi(cpu->hl) & ~(0b0000'0010));
}

void RES_1_L(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, Lo(cpu->hl) & ~(0b0000'0010));
}

void RES_1_AHL(CPU *cpu, instruction_t &i)
{
	cpu->mmu->and8(cpu->hl, ~(0b0000'0010));
}

void RES_1_A(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, Hi(cpu->af) & ~(0b0000'0010));
}

void RES_2_B(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, Hi(cpu->bc) & ~(0b0000'0100));
}

void RES_2_C(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, Lo(cpu->bc) & ~(0b0000'0100));
}

void RES_2_D(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Hi(cpu->de) & ~(0b0000'0100));
}

void RES_2_E(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, Lo(cpu->de) & ~(0b0000'0100));
}

void RES_2_H(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, Hi(cpu->hl) & ~(0b0000'0100));
}

void RES_2_L(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, Lo(cpu->hl) & ~(0b0000'0100));
}

void RES_2_AHL(CPU *cpu, instruction_t &i)
{
	cpu->mmu->and8(cpu->hl, ~(0b0000'0100));
}

void RES_2_A(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, Hi(cpu->af) & ~(0b0000'0100));
}

void RES_3_B(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, Hi(cpu->bc) & ~(0b0000'1000));
}

void RES_3_C(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, Lo(cpu->bc) & ~(0b0000'1000));
}

void RES_3_D(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Hi(cpu->de) & ~(0b0000'1000));
}

void RES_3_E(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, Lo(cpu->de) & ~(0b0000'1000));
}

void RES_3_H(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, Hi(cpu->hl) & ~(0b0000'1000));
}

void RES_3_L(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, Lo(cpu->hl) & ~(0b0000'1000));
}

void RES_3_AHL(CPU *cpu, instruction_t &i)
{
	cpu->mmu->and8(cpu->hl, ~(0b0000'1000));
}

void RES_3_A(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, Hi(cpu->af) & ~(0b0000'1000));
}

void RES_4_B(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, Hi(cpu->bc) & ~(0b0001'0000));
}

void RES_4_C(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, Lo(cpu->bc) & ~(0b0001'0000));
}

void RES_4_D(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Hi(cpu->de) & ~(0b0001'0000));
}

void RES_4_E(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, Lo(cpu->de) & ~(0b0001'0000));
}

void RES_4_H(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, Hi(cpu->hl) & ~(0b0001'0000));
}

void RES_4_L(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, Lo(cpu->hl) & ~(0b0001'0000));
}

void RES_4_AHL(CPU *cpu, instruction_t &i)
{
	cpu->mmu->and8(cpu->hl, ~(0b0001'0000));
}

void RES_4_A(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, Hi(cpu->af) & ~(0b0001'0000));
}

void RES_5_B(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, Hi(cpu->bc) & ~(0b0010'0000));
}

void RES_5_C(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, Lo(cpu->bc) & ~(0b0010'0000));
}

void RES_5_D(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Hi(cpu->de) & ~(0b0010'0000));
}

void RES_5_E(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, Lo(cpu->de) & ~(0b0010'0000));
}

void RES_5_H(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, Hi(cpu->hl) & ~(0b0010'0000));
}

void RES_5_L(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, Lo(cpu->hl) & ~(0b0010'0000));
}

void RES_5_AHL(CPU *cpu, instruction_t &i)
{
	cpu->mmu->and8(cpu->hl, ~(0b0010'0000));
}

void RES_5_A(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, Hi(cpu->af) & ~(0b0010'0000));
}

void RES_6_B(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, Hi(cpu->bc) & ~(0b0100'0000));
}

void RES_6_C(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, Lo(cpu->bc) & ~(0b0100'0000));
}

void RES_6_D(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Hi(cpu->de) & ~(0b0100'0000));
}

void RES_6_E(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, Lo(cpu->de) & ~(0b0100'0000));
}

void RES_6_H(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, Hi(cpu->hl) & ~(0b0100'0000));
}

void RES_6_L(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, Lo(cpu->hl) & ~(0b0100'0000));
}

void RES_6_AHL(CPU *cpu, instruction_t &i)
{
	cpu->mmu->and8(cpu->hl, ~(0b0100'0000));
}

void RES_6_A(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, Hi(cpu->af) & ~(0b0100'0000));
}

void RES_7_B(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, Hi(cpu->bc) & ~(0b1000'0000));
}

void RES_7_C(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, Lo(cpu->bc) & ~(0b1000'0000));
}

void RES_7_D(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Hi(cpu->de) & ~(0b1000'0000));
}

void RES_7_E(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, Lo(cpu->de) & ~(0b1000'0000));
}

void RES_7_H(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, Hi(cpu->hl) & ~(0b1000'0000));
}

void RES_7_L(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, Lo(cpu->hl) & ~(0b1000'0000));
}

void RES_7_AHL(CPU *cpu, instruction_t &i)
{
	cpu->mmu->and8(cpu->hl, static_cast<uint8_t>(~0x80));
}

void RES_7_A(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, Hi(cpu->af) & ~(0b1000'0000));
}

void SET_0_B(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, Hi(cpu->bc) | (0b0000'0001));
}

void SET_0_C(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, Lo(cpu->bc) | (0b0000'0001));
}

void SET_0_D(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Hi(cpu->de) | (0b0000'0001));
}

void SET_0_E(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, Lo(cpu->de) | (0b0000'0001));
}

void SET_0_H(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, Hi(cpu->hl) | (0b0000'0001));
}

void SET_0_L(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, Lo(cpu->hl) | (0b0000'0001));
}

void SET_0_AHL(CPU *cpu, instruction_t &i)
{
	cpu->mmu->or8(cpu->hl, 0b0000'0001);
}

void SET_0_A(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, Hi(cpu->af) | (0b0000'0001));
}

void SET_1_B(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, Hi(cpu->bc) | (0b0000'0010));
}

void SET_1_C(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, Lo(cpu->bc) | (0b0000'0010));
}

void SET_1_D(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Hi(cpu->de) | (0b0000'0010));
}

void SET_1_E(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, Lo(cpu->de) | (0b0000'0010));
}

void SET_1_H(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, Hi(cpu->hl) | (0b0000'0010));
}

void SET_1_L(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, Lo(cpu->hl) | (0b0000'0010));
}

void SET_1_AHL(CPU *cpu, instruction_t &i)
{
	cpu->mmu->or8(cpu->hl, 0b0000'0010);
}

void SET_1_A(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, Hi(cpu->af) | (0b0000'0010));
}

void SET_2_B(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, Hi(cpu->bc) | (0b0000'0100));
}

void SET_2_C(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, Lo(cpu->bc) | (0b0000'0100));
}

void SET_2_D(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Hi(cpu->de) | (0b0000'0100));
}

void SET_2_E(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, Lo(cpu->de) | (0b0000'0100));
}

void SET_2_H(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, Hi(cpu->hl) | (0b0000'0100));
}

void SET_2_L(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, Lo(cpu->hl) | (0b0000'0100));
}

void SET_2_AHL(CPU *cpu, instruction_t &i)
{
	cpu->mmu->or8(cpu->hl, 0b0000'0100);
}

void SET_2_A(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, Hi(cpu->af) | (0b0000'0100));
}

void SET_3_B(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, Hi(cpu->bc) | (0b0000'1000));
}

void SET_3_C(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, Lo(cpu->bc) | (0b0000'1000));
}

void SET_3_D(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Hi(cpu->de) | (0b0000'1000));
}

void SET_3_E(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, Lo(cpu->de) | (0b0000'1000));
}

void SET_3_H(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, Hi(cpu->hl) | (0b0000'1000));
}

void SET_3_L(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, Lo(cpu->hl) | (0b0000'1000));
}

void SET_3_AHL(CPU *cpu, instruction_t &i)
{
	cpu->mmu->or8(cpu->hl, 0b0000'1000);
}

void SET_3_A(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, Hi(cpu->af) | (0b0000'1000));
}

void SET_4_B(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, Hi(cpu->bc) | (0b0001'0000));
}

void SET_4_C(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, Lo(cpu->bc) | (0b0001'0000));
}

void SET_4_D(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Hi(cpu->de) | (0b0001'0000));
}

void SET_4_E(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, Lo(cpu->de) | (0b0001'0000));
}

void SET_4_H(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, Hi(cpu->hl) | (0b0001'0000));
}

void SET_4_L(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, Lo(cpu->hl) | (0b0001'0000));
}

void SET_4_AHL(CPU *cpu, instruction_t &i)
{
	cpu->mmu->or8(cpu->hl, 0b0001'0000);
}

void SET_4_A(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, Hi(cpu->af) | (0b0001'0000));
}

void SET_5_B(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, Hi(cpu->bc) | (0b0010'0000));
}

void SET_5_C(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, Lo(cpu->bc) | (0b0010'0000));
}

void SET_5_D(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Hi(cpu->de) | (0b0010'0000));
}

void SET_5_E(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, Lo(cpu->de) | (0b0010'0000));
}

void SET_5_H(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, Hi(cpu->hl) | (0b0010'0000));
}

void SET_5_L(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, Lo(cpu->hl) | (0b0010'0000));
}

void SET_5_AHL(CPU *cpu, instruction_t &i)
{
	cpu->mmu->or8(cpu->hl, 0b0010'0000);
}

void SET_5_A(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, Hi(cpu->af) | (0b0010'0000));
}

void SET_6_B(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, Hi(cpu->bc) | (0b0100'0000));
}

void SET_6_C(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, Lo(cpu->bc) | (0b0100'0000));
}

void SET_6_D(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Hi(cpu->de) | (0b0100'0000));
}

void SET_6_E(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, Lo(cpu->de) | (0b0100'0000));
}

void SET_6_H(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, Hi(cpu->hl) | (0b0100'0000));
}

void SET_6_L(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, Lo(cpu->hl) | (0b0100'0000));
}

void SET_6_AHL(CPU *cpu, instruction_t &i)
{
	cpu->mmu->or8(cpu->hl, 0b0100'0000);
}

void SET_6_A(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, Hi(cpu->af) | (0b0100'0000));
}

void SET_7_B(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->bc, Hi(cpu->bc) | (0b1000'0000));
}

void SET_7_C(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->bc, Lo(cpu->bc) | (0b1000'0000));
}

void SET_7_D(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->de, Hi(cpu->de) | (0b1000'0000));
}

void SET_7_E(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->de, Lo(cpu->de) | (0b1000'0000));
}

void SET_7_H(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->hl, Hi(cpu->hl) | (0b1000'0000));
}

void SET_7_L(CPU *cpu, instruction_t &i)
{
	SetLo(cpu->hl, Lo(cpu->hl) | (0b1000'0000));
}

void SET_7_AHL(CPU *cpu, instruction_t &i)
{
	cpu->mmu->or8(cpu->hl, 0b1000'0000);
}

void SET_7_A(CPU *cpu, instruction_t &i)
{
	SetHi(cpu->af, Hi(cpu->af) | (0b1000'0000));
}

const instructionFunc_t kInstructions[0x100] =
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
}; 

const instructionFunc_t kCbInstructions[0x100] =
{
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
