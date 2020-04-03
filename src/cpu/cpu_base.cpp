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

uint8_t ADD8(uint8_t a, uint8_t b, uint8_t &f)
{
	// Z0HC
	f = 0;

	uint8_t result = a + b;

	if ((a ^ b ^ result) & FLAGS_CARRY)
	{
		f |= FLAGS_HALFCARRY;
	}
	if (uint16_t(a) + uint16_t(b) > 0xFF)
	{
		f |= FLAGS_CARRY;
	}
	if (result == 0)
	{
		f |= FLAGS_ZERO;
	}

	return result;
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

uint8_t SUB8(uint8_t a, uint8_t b, uint8_t &f)
{
	// Z1HC
	f = FLAGS_SUBTRACT;
	uint8_t result = a - b;
	if (!result)
	{
		f |= FLAGS_ZERO;
	}
	if (b > a)
	{
		f |= FLAGS_CARRY;
	}
	if ((int(a) & 0xF) - (int(b) & 0xF) < 0)
	{
		f |= FLAGS_HALFCARRY;
	}
	return result;
}

void INC8(uint8_t &r, uint8_t &f)
{
	uint8_t carry = f & FLAGS_CARRY;
	r = ADD8(r, 1, f);
	f = (f & 0b1110'0000) | carry;
}

void DEC8(uint8_t &r, uint8_t &f)
{
	uint8_t carry = f & FLAGS_CARRY;
	r = SUB8(r, 1, f);
	f = (f & 0b1110'0000) | carry;
}

void CP(uint8_t a, uint8_t r, uint8_t &f)
{
	(void)SUB8(a, r, f);
}

void SUB(CPU *cpu, uint8_t r)
{
	//uint8_t flags = 0;
	//uint8_t result = 0;
	//SUB8(Hi(cpu->af), r, &result, &flags);
	//SetLo(cpu->af, flags);
	//SetHi(cpu->af, result);
}

void TestBit(uint8_t bit, uint8_t &f)
{
	uint8_t flags = FLAGS_HALFCARRY;
	if (!bit)
	{
		flags |= FLAGS_ZERO;
	}
	f = (f & FLAGS_CARRY) | flags;
}
