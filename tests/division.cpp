#include <cstdint>

#include <libxint/uint.hpp>

#include "catch2/catch_amalgamated.hpp"
#include "utils/random.hpp"


const unsigned max_tries = 100000;


TEST_CASE("random64", "[random][64]")
{
    using std::uint64_t;
    using x64 = xint::uint<64, true>;
    for (unsigned i = 0; i < max_tries; ++i) {
        uint64_t a = utils::rand64();
        uint64_t b = utils::rand64();
        if (!b)
            continue;

        uint64_t c = a / b;
        uint64_t d = a % b;

        x64 xa = a;
        x64 xb = b;
        auto [xc, xd] = div(xa, xb);

        CHECK(xc == c);
        CHECK(xd == d);
    }
}


TEST_CASE("special", "[64]")
{
    using std::uint64_t;
    using x64 = xint::uint<64, true>;

    {
        uint64_t a = 18021314295701889875ull;
        uint64_t b =   152711102361896281ull;
        uint64_t c = a / b;
        uint64_t d = a % b;

        x64 xa = a;
        x64 xb = b;
        auto [xc, xd] = div(xa, xb);

        CHECK(xc == c);
        CHECK(xd == d);
    }

    {
        uint64_t a = 14933659896240572917ull;
        uint64_t b =  8135395356393059844ull;
        uint64_t c = a / b;
        uint64_t d = a % b;

        x64 xa = a;
        x64 xb = b;
        auto [xc, xd] = div(xa, xb);

        CHECK(xc == c);
        CHECK(xd == d);
    }

    {
        x64 xa = 65535;
        x64 xb = 65535;
        auto [xc, xd] = div(xa, xb);
        CHECK(xc == 1);
        CHECK(xd == 0);
    }

    {
        x64 xa = 65536;
        x64 xb = 65536;
        auto [xc, xd] = div(xa, xb);
        CHECK(xc == 1);
        CHECK(xd == 0);
    }

    {
        x64 xa = 65537;
        x64 xb = 65537;
        auto [xc, xd] = div(xa, xb);
        CHECK(xc == 1);
        CHECK(xd == 0);
    }
}
