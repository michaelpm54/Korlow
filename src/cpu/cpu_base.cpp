#include "cpu.h"
#include "cpu_base.h"

void RL(uint8_t val, uint8_t *result, uint8_t *flags)
{
	// Z00C

	// LSH 1 and add carry bit
	*result = (val << 1) + ((*flags & 0x10) >> 4);

	// MSB becomes new carry bit
	*flags = (val & 0x80) >> 3;

	if (!*result)
	{
		*flags |= 0x80;
	}
}

void RLC(uint8_t val, uint8_t *result, uint8_t *flags)
{
	uint8_t msb = (val & 0x80) >> 7;
	*result = (val << 1) | msb;
	*flags = msb << 4;
	if (!*result)
	{
		*flags |= 0x80;
	}
}

void RR(uint8_t val, uint8_t *result, uint8_t *flags)
{
	uint8_t carry = *flags & 0x10;
	uint8_t lsb = val & 0x1;
	*result = (val >> 1) | (carry << 3);
	*flags = lsb << 4;
	if (!*result)
	{
		*flags |= 0x80;
	}
}

void RRC(uint8_t val, uint8_t *result, uint8_t *flags)
{
	uint8_t lsb = val & 0x1;
	*result = (val >> 1) | (lsb << 7);
	*flags = lsb << 4;
	if (!*result)
	{
		*flags |= 0x80;
	}
}

void ADD8(uint8_t a, uint8_t b, uint8_t *result, uint8_t *flags)
{
	// Z0HC

	*flags = 0;
	*result = a + b;

	if ((a ^ b ^ *result) & FLAGS_CARRY)
	{
		*flags |= FLAGS_HALFCARRY;
	}
	if (uint16_t(a) + uint16_t(b) > 0xFF)
	{
		*flags |= FLAGS_CARRY;
	}
	if (*result == 0)
	{
		*flags |= FLAGS_ZERO;
	}
}

void ADD16(uint16_t a, uint16_t b, uint16_t *result, uint8_t *flags)
{
	*flags = 0;
	*result = a + b;
	if (((a & 0xFFF) + (b & 0xFFF)) & 0x1000)
	{
		*flags |= FLAGS_HALFCARRY;
	}
	if ((uint32_t(a) + uint32_t(b)) & 0x10000)
	{
		*flags |= FLAGS_CARRY;
	}
	if (*result == 0)
	{
		*flags |= FLAGS_ZERO;
	}
}

void SUB8(uint8_t a, uint8_t b, uint8_t *result, uint8_t *flags)
{
	// Z1HC
	*flags = FLAGS_SUBTRACT;
	*result = a - b;
	if (!*result)
	{
		*flags |= FLAGS_ZERO;
	}
	if (b > a)
	{
		*flags |= FLAGS_CARRY;
	}
	if ((int(a) & 0xF) - (int(b) & 0xF) < 0)
	{
		*flags |= FLAGS_HALFCARRY;
	}
}

void INC8(uint8_t val, uint8_t *result, uint8_t *flags)
{
	uint8_t carry = *flags & 0x10;
	ADD8(val, 1, result, flags);
	*flags = (*flags & 0b1110'0000) | carry;
}

void DEC8(uint8_t val, uint8_t *result, uint8_t *flags)
{
	uint8_t carry = *flags & 0x10;
	SUB8(val, 1, result, flags);
	*flags = (*flags & 0b1110'0000) | carry;
}

uint8_t Hi(uint16_t n)
{
	return (n & 0xFF00) >> 8;
}

uint8_t Lo(uint16_t n)
{
	return (n & 0xFF);
}

void SetHi(uint16_t &r, uint8_t n)
{
	r = (uint16_t(n) << 8) + Lo(r);
}

void SetLo(uint16_t &r, uint8_t n)
{
	r = (r & 0xFF00) + n;
}

void INC8_HI(CPU *cpu, uint16_t &r)
{
	// Z0H-
	uint8_t flags = Lo(cpu->af);
	uint8_t result = 0;
	INC8(Hi(r), &result, &flags);
	SetHi(r, result);
	SetLo(cpu->af, flags);
}

void INC8_LO(CPU *cpu, uint16_t &r)
{
	// Z0H-
	uint8_t flags = Lo(cpu->af);
	uint8_t result = 0;
	INC8(Lo(r), &result, &flags);
	SetLo(r, result);
	SetLo(cpu->af, flags);
}

void DEC8_HI(CPU *cpu, uint16_t &r)
{
	// Z1H-
	uint8_t flags = Lo(cpu->af);
	uint8_t result = 0;
	DEC8(Hi(r), &result, &flags);
	SetHi(r, result);
	SetLo(cpu->af, flags);
}

void DEC8_LO(CPU *cpu, uint16_t &r)
{
	// Z1H-
	uint8_t flags = Lo(cpu->af);
	uint8_t result = 0;
	DEC8(Lo(r), &result, &flags);
	SetLo(r, result);
	SetLo(cpu->af, flags);
}

void CP(CPU *cpu, uint8_t r)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	SUB8(Hi(cpu->af), r, &result, &flags);
	SetLo(cpu->af, flags);
}

void SUB(CPU *cpu, uint8_t r)
{
	uint8_t flags = 0;
	uint8_t result = 0;
	SUB8(Hi(cpu->af), r, &result, &flags);
	SetLo(cpu->af, flags);
	SetHi(cpu->af, result);
}

void TestBit(CPU *cpu, uint8_t bit)
{
	uint8_t flags = FLAGS_HALFCARRY;
	if (!bit)
	{
		flags |= FLAGS_ZERO;
	}
	SetLo(cpu->af, (Lo(cpu->af) & FLAGS_CARRY) | flags);
}
