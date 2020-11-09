#include "cpu/cpu_base.h"

#include "cpu/cpu.h"

void RL(u8 val, u8 *result, u8 *flags)
{
    // Z00C

    u8 old_carry = (*flags & FLAGS_CARRY) >> 4;
    u8 new_carry = val & 0x80;
    u8 r = (val << 1) | old_carry;

    *flags = 0;

    if (r == 0) {
        *flags |= FLAGS_ZERO;
    }

    *flags |= new_carry >> 3;

    *result = r;
}

void RLC(u8 val, u8 *result, u8 *flags, bool unset_zero)
{
    /* Push the MSB to LSB. */
    u8 msb = (val & 0x80) >> 7;

    /* Left shift and add the MSB (now LSB). */
    *result = (val << 1) | msb;

    /* Push the former MSB to the carry flag. */
    *flags = msb << 4;

    /* RLC A always unsets the zero flag. */
    if (*result != 0 || unset_zero) {
        *flags &= ~(FLAGS_ZERO);
    }
    else {
        *flags |= FLAGS_ZERO;
    }
}

void RR(u8 val, u8 *result, u8 *flags)
{
    u8 carry = *flags & 0x10;
    u8 lsb = val & 0x1;
    *result = (val >> 1) | (carry << 3);
    *flags = lsb << 4;
    if (!*result) {
        *flags |= 0x80;
    }
}

void RRC(u8 val, u8 *result, u8 *flags)
{
    u8 lsb = val & 0x1;
    *result = (val >> 1) | (lsb << 7);
    *flags = lsb << 4;
    if (!*result) {
        *flags |= 0x80;
    }
}

u8 ADD8(u8 a, u8 b, u8 &f)
{
    // Z0HC
    f = 0;

    u8 result = a + b;

    if ((a ^ b ^ result) & FLAGS_CARRY) {
        f |= FLAGS_HALFCARRY;
    }
    if (u16(a) + u16(b) > 0xFF) {
        f |= FLAGS_CARRY;
    }
    if (result == 0) {
        f |= FLAGS_ZERO;
    }

    return result;
}

void ADD16(u16 a, u16 b, u16 *result, u8 *flags)
{
    *flags = 0;
    *result = a + b;
    if (((a & 0xFFF) + (b & 0xFFF)) & 0x1000) {
        *flags |= FLAGS_HALFCARRY;
    }
    if ((uint32_t(a) + uint32_t(b)) & 0x10000) {
        *flags |= FLAGS_CARRY;
    }
    if (*result == 0) {
        *flags |= FLAGS_ZERO;
    }
}

u8 SUB8(u8 a, u8 b, u8 &f)
{
    // Z1HC
    f = FLAGS_SUBTRACT;
    u8 result = a - b;
    if (!result) {
        f |= FLAGS_ZERO;
    }
    if (b > a) {
        f |= FLAGS_CARRY;
    }
    if ((int(a) & 0xF) - (int(b) & 0xF) < 0) {
        f |= FLAGS_HALFCARRY;
    }
    return result;
}

void INC8(u8 &r, u8 &f)
{
    u8 carry = f & FLAGS_CARRY;
    r = ADD8(r, 1, f);
    f = (f & 0b1110'0000) | carry;
}

void DEC8(u8 &r, u8 &f)
{
    u8 carry = f & FLAGS_CARRY;
    r = SUB8(r, 1, f);
    f = (f & 0b1110'0000) | carry;
}

void CP(u8 a, u8 r, u8 &f)
{
    (void)SUB8(a, r, f);
}

void SUB(CPU *cpu, u8 r)
{
    //u8 flags = 0;
    //u8 result = 0;
    //SUB8(Hi(cpu->af), r, &result, &flags);
    //SetLo(cpu->af, flags);
    //SetHi(cpu->af, result);
}

void TestBit(u8 bit, u8 &f)
{
    u8 flags = FLAGS_HALFCARRY;
    if (!bit) {
        flags |= FLAGS_ZERO;
    }
    f = (f & FLAGS_CARRY) | flags;
}
