#include "doctest.h"

#include "cpu_base.h"

TEST_CASE("ADD8 sets flags Z0HC")
{
	uint8_t flags = 0;
	uint8_t result = 0;

	SUBCASE("Zero is set correctly")
	{
		SUBCASE("Both inputs are zero")
		{
			ADD8(0, 0, &result, &flags);
			CHECK(result == 0);
			CHECK(flags == 0x80); // z000
		}
		SUBCASE("8 bit int overflow to zero, carry and hc are set")
		{
			ADD8(255, 1, &result, &flags);
			CHECK(result == 0);
			CHECK(flags == 0xB0); // z0hc
		}
	}
	SUBCASE("Half-carry is set correctly")
	{
		flags = 0;
		ADD8(0b0000'1111, 1, &result, &flags);
		CHECK(flags == 0x20);
	}
	SUBCASE("Addition works")
	{
		ADD8(1, 1, &result, &flags);
		CHECK(result == 2);
	}
}

TEST_CASE("ADD16 sets flags Z0HC")
{
	uint8_t flags = 0;
	uint16_t result = 0;

	SUBCASE("Zero is set correctly")
	{
		SUBCASE("Both inputs are zero")
		{
			ADD16(0, 0, &result, &flags);
			CHECK(result == 0);
			CHECK(flags == 0x80); // z000
		}
		SUBCASE("16 bit int overflow to zero, carry and hc are not set")
		{
			ADD16(0x8000, 0x8000, &result, &flags);
			CHECK(result == 0);
			CHECK(flags == 0x80); // z000
		}
	}
	SUBCASE("Half-carry is set correctly")
	{
		flags = 0;
		ADD16(0b0000'1111, 1, &result, &flags);
		CHECK(result == 0x10);
		CHECK(flags == 0x20);
	}
}
