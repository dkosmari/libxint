#include <cstdint>
#include <numeric>

#include <libxint/uint.hpp>

#include "catch2/catch_amalgamated.hpp"
#include "utils/random.hpp"


const unsigned max_tries = 10000;


TEST_CASE("gcd special", "[gcd][64]")
{
    using std::uint64_t;
    using std::gcd;
    using x64 = xint::uint<64>;

#define TEST(x, y)                              \
    do {                                        \
        uint64_t a = x;                         \
        uint64_t b = y;                         \
        uint64_t c = std::gcd(a, b);            \
        x64 xa = a;                             \
        x64 xb = b;                             \
        CHECK(gcd(xa, xb) == c);                \
    } while (false)

    TEST(0, 0);
    TEST(0, 1);
    TEST(1, 0);
    TEST(1, 1);
    TEST(2, 0);
    TEST(0, 2);

    TEST(15, 3);
    TEST(17, 17);
    TEST(256, 256);
    TEST(512, 512);
    TEST(511, 511);
#undef TEST
}


TEST_CASE("gcd random 64", "[gcd][random][64]")
{
    using std::uint64_t;
    using x64 = xint::uint<64>;

    for (unsigned i = 0; i < max_tries; ++i) {
        uint64_t a = utils::rand64();
        uint64_t b = utils::rand64();
        uint64_t c = std::gcd(a, b);
        x64 xa = a;
        x64 xb = b;
        CHECK(gcd(xa, xb) == c);
    }
}


TEST_CASE("lcm special", "[lcm][64]")
{
    using std::uint64_t;
    using x64 = xint::uint<64>;

#define TEST(x, y)                              \
    do {                                        \
        uint64_t a = x;                         \
        uint64_t b = y;                         \
        uint64_t c = std::lcm(a, b);            \
        x64 xa = a;                             \
        x64 xb = b;                             \
        CHECK(lcm(xa, xb) == c);                \
    } while (false)

    TEST(0, 0);
    TEST(0, 1);
    TEST(1, 0);
    TEST(1, 1);
    TEST(2, 0);
    TEST(0, 2);

    TEST(15, 3);
    TEST(17, 17);
    TEST(256, 256);
    TEST(512, 512);
    TEST(511, 511);
#undef TEST
}


TEST_CASE("lcm random 64", "[lcm][random][64]")
{
    using std::uint64_t;
    using x64 = xint::uint<64>;

    for (unsigned i = 0; i < max_tries; ++i) {
        uint64_t a = utils::rand64();
        uint64_t b = utils::rand64();
        uint64_t c = std::lcm(a, b);
        x64 xa = a;
        x64 xb = b;
        CHECK(lcm(xa, xb) == c);
    }
}
