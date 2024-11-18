#include "../../../../external/catch2/catch.hpp"
#include "../SecretFunction/IsPrime.h"

TEST_CASE("IsPrime: under bottom bound")
{
	REQUIRE(!IsPrime(1));
}

TEST_CASE("IsPrime: First even number")
{
	REQUIRE(IsPrime(2));
}

TEST_CASE("IsPrime: number = 25 (%5 = 0)")
{
	REQUIRE(!IsPrime(25));
}

TEST_CASE("IsPrime: number = 29")
{
	REQUIRE(IsPrime(29));
}

TEST_CASE("IsPrime: number = 4 (Even)")
{
	REQUIRE(!IsPrime(4));
}

TEST_CASE("IsPrime: number = 9 (Divided by 3)")
{
	REQUIRE(!IsPrime(9));
}

TEST_CASE("IsPrime: number = 97 - 1 (before prime)")
{
	REQUIRE(!IsPrime(96));
}

TEST_CASE("IsPrime: number = 97")
{
	REQUIRE(IsPrime(97));
}

TEST_CASE("IsPrime: number = 97 + 1 (after prime)")
{
	REQUIRE(!IsPrime(98));
}
