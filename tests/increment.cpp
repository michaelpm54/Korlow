#include "doctest.h"

#include "cpu_base.h"

TEST_CASE("8 bit increment")
{
	uint8_t flags = 0;
	uint8_t result = 0;

	SUBCASE("Overflow increment")
	{
		INC8(255, &result, &flags);
		CHECK(result == 0);
		CHECK(flags == 0xA0);
	}
	SUBCASE("Non-overflow increment")
	{
		INC8(3, &result, &flags);
		CHECK(flags == 0);
	}
	SUBCASE("Preserve carry flag")
	{
		flags = 0x10;
		INC8(1, &result, &flags);
		CHECK(result == 2);
		CHECK(flags == 0x10);
	}
}
