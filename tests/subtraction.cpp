#include "doctest.h"

#include "cpu_base.h"

TEST_CASE("Subtraction yields correct results and flags")
{
	uint8_t flags = 0;
	uint8_t result = 0;

	SUBCASE("Zero is set")
	{
		flags = 0;
		SUB8(1, 1, &result, &flags);
		CHECK(flags == 0b1100'0000);
	}
	SUBCASE("Zero is unset")
	{
		flags = 0x80;
		SUB8(2, 1, &result, &flags);
		CHECK(flags == 0b0100'0000);
	}
	SUBCASE("Carry and half-carry are set on underflow")
	{
		flags = 0;
		SUB8(1, 2, &result, &flags);
		CHECK(flags == 0b0111'0000);
	}
}
