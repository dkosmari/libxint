#include <cstdint>

#include <libxint/uint.hpp>

#include "catch2/catch_amalgamated.hpp"
#include "utils/random.hpp"


const unsigned max_tries = 100000;


TEST_CASE("random64", "[random][64]")
{
    using std::uint64_t;
    using x64 = xint::uint<64>;
    for (unsigned i = 0; i < max_tries; ++i) {
        uint64_t a = utils::rand64();
        unsigned b = utils::rand(63);
        uint64_t c = a << b;
        uint64_t d = a >> b;

        x64 xa = a;
        x64 xc = xa << b;
        x64 xd = xa >> b;

        CHECK(xc == c);
        CHECK(xd == d);
    }
}



TEST_CASE("special", "[32]")
{
    using x32 = xint::uint<32>;

    std::uint32_t z = 0b11110000'11000011'01011010'10010110;
    std::uint32_t w = 0;
    x32 a = z;
    x32 b;

#define TEST CHECK(b == w)

    b = a << 0;
    w = z << 0;
    TEST;
    b = a >> 0;
    w = z >> 0;
    TEST;

    b = a << 1;
    w = z << 1;
    TEST;
    b = a >> 1;
    w = z >> 1;
    TEST;

    b = a << 8;
    w = z << 8;
    TEST;
    b = a >> 8;
    w = z >> 8;
    TEST;

    b = a << 9;
    w = z << 9;
    TEST;
    b = a >> 9;
    w = z >> 9;
    TEST;

    b = a << 16;
    w = z << 16;
    TEST;
    b = a >> 16;
    w = z >> 16;
    TEST;

    b = a << 17;
    w = z << 17;
    TEST;
    b = a >> 17;
    w = z >> 17;
    TEST;

    b = a << 31;
    w = z << 31;
    TEST;
    b = a >> 31;
    w = z >> 31;
    TEST;
#undef TEST

    b = a << 32;
    CHECK(!b);
    b = a >> 32;
    CHECK(!b);
}


TEST_CASE("left-overflow", "[24]")
{
    using x24 = xint::uint<24, true>;

    {
        x24 a = 0b1111'1111'1111'1111'1111'1111;
        CHECK_NOTHROW(a << 0);
        for (int i = 1; i <= 24; ++i)
            CHECK_THROWS_AS(a << i, std::overflow_error);
    }

    {
        x24 a = 0b0111'1111'1111'1111'1111'1111;
        for (int i = 0; i < 2; ++i)
            CHECK_NOTHROW(a << i);
        for (int i = 2; i <= 24; ++i)
            CHECK_THROWS_AS(a << i, std::overflow_error);
    }

    {
        x24 a = 0b0011'1111'1111'1111'1111'1111;
        for (int i = 0; i < 3; ++i)
            CHECK_NOTHROW(a << i);
        for (int i = 3; i <= 24; ++i)
            CHECK_THROWS_AS(a << i, std::overflow_error);
    }

    {
        x24 a = 0b0000'0001'1111'1111'1111'1111;
        for (int i = 0; i < 8; ++i)
            CHECK_NOTHROW(a << i);
        for (int i = 8; i <= 24; ++i)
            CHECK_THROWS_AS(a << i, std::overflow_error);
    }

    {
        x24 a = 0b0000'0000'1111'1111'1111'1111;
        for (int i = 0; i < 9; ++i)
            CHECK_NOTHROW(a << i);
        for (int i = 9; i <= 24; ++i)
            CHECK_THROWS_AS(a << i, std::overflow_error);
    }

    {
        x24 a = 0b0000'0000'0111'1111'1111'1111;
        for (int i = 0; i < 10; ++i)
            CHECK_NOTHROW(a << i);
        for (int i = 10; i <= 24; ++i)
            CHECK_THROWS_AS(a << i, std::overflow_error);
    }

    {
        x24 a = 0b0000'0000'0000'0001'1111'1111;
        for (int i = 0; i < 16; ++i)
            CHECK_NOTHROW(a << i);
        for (int i = 16; i <= 24; ++i)
            CHECK_THROWS_AS(a << i, std::overflow_error);
    }

    {
        x24 a = 0b0000'0000'0000'0000'1111'1111;
        for (int i = 0; i < 17; ++i)
            CHECK_NOTHROW(a << i);
        for (int i = 17; i <= 24; ++i)
            CHECK_THROWS_AS(a << i, std::overflow_error);
    }

    {
        x24 a = 0b0000'0000'0000'0000'0111'1111;
        for (int i = 0; i < 18; ++i)
            CHECK_NOTHROW(a << i);
        for (int i = 18; i <= 24; ++i)
            CHECK_THROWS_AS(a << i, std::overflow_error);
    }

    {
        x24 a = 0b0000'0000'0000'0000'0011'1111;
        for (int i = 0; i < 19; ++i)
            CHECK_NOTHROW(a << i);
        for (int i = 19; i <= 24; ++i)
            CHECK_THROWS_AS(a << i, std::overflow_error);
    }

    {
        x24 a = 0b0000'0000'0000'0000'0000'0001;
        for (int i = 0; i < 24; ++i)
            CHECK_NOTHROW(a << i);
        CHECK_THROWS_AS(a << 24, std::overflow_error);
    }

    {
        x24 a = 0;
        for (int i = 0; i <= 24; ++i)
            CHECK_NOTHROW(a << i);
    }

}


TEST_CASE("right-overflow", "[24]")
{
    using x24 = xint::uint<24, true>;

    {
        x24 a = 0b1111'1111'1111'1111'1111'1111;
        CHECK_NOTHROW(a >> 0);
        for (int i = 1; i <= 24; ++i)
            CHECK_THROWS_AS(a >> i, std::overflow_error);
    }

    {
        x24 a = 0b1111'1111'1111'1111'1111'1110;
        for (int i = 0; i < 2; ++i)
            CHECK_NOTHROW(a >> i);
        for (int i = 2; i <= 24; ++i)
            CHECK_THROWS_AS(a >> i, std::overflow_error);
    }

    {
        x24 a = 0b1111'1111'1111'1111'1111'1100;
        for (int i = 0; i < 3; ++i)
            CHECK_NOTHROW(a >> i);
        for (int i = 3; i <= 24; ++i)
            CHECK_THROWS_AS(a >> i, std::overflow_error);
    }

    {
        x24 a = 0b1111'1111'1111'1111'1000'0000;
        for (int i = 0; i < 8; ++i)
            CHECK_NOTHROW(a >> i);
        for (int i = 8; i <= 24; ++i)
            CHECK_THROWS_AS(a >> i, std::overflow_error);
    }

    {
        x24 a = 0b1111'1111'1111'1111'0000'0000;
        for (int i = 0; i < 9; ++i)
            CHECK_NOTHROW(a >> i);
        for (int i = 9; i <= 24; ++i)
            CHECK_THROWS_AS(a >> i, std::overflow_error);
    }

    {
        x24 a = 0b1111'1111'1111'1110'0000'0000;
        for (int i = 0; i < 10; ++i)
            CHECK_NOTHROW(a >> i);
        for (int i = 10; i <= 24; ++i)
            CHECK_THROWS_AS(a >> i, std::overflow_error);
    }

    {
        x24 a = 0b1111'1111'1000'0000'0000'0000;
        for (int i = 0; i < 16; ++i)
            CHECK_NOTHROW(a >> i);
        for (int i = 16; i <= 24; ++i)
            CHECK_THROWS_AS(a >> i, std::overflow_error);
    }

    {
        x24 a = 0b1111'1111'0000'0000'0000'0000;
        for (int i = 0; i < 17; ++i)
            CHECK_NOTHROW(a >> i);
        for (int i = 17; i <= 24; ++i)
            CHECK_THROWS_AS(a >> i, std::overflow_error);
    }

    {
        x24 a = 0b1111'1110'0000'0000'0000'0000;
        for (int i = 0; i < 18; ++i)
            CHECK_NOTHROW(a >> i);
        for (int i = 18; i <= 24; ++i)
            CHECK_THROWS_AS(a >> i, std::overflow_error);
    }

    {
        x24 a = 0b1111'1100'0000'0000'0000'0000;
        for (int i = 0; i < 19; ++i)
            CHECK_NOTHROW(a >> i);
        for (int i = 19; i <= 24; ++i)
            CHECK_THROWS_AS(a >> i, std::overflow_error);
    }

    {
        x24 a = 0b1000'0000'0000'0000'0000'0000;
        for (int i = 0; i < 24; ++i)
            CHECK_NOTHROW(a >> i);
        CHECK_THROWS_AS(a >> 24, std::overflow_error);
    }

    {
        x24 a = 0;
        for (int i = 0; i <= 24; ++i)
            CHECK_NOTHROW(a >> i);
    }

}
