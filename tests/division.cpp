#include <libxint/uint.hpp>

#include "catch2/catch_amalgamated.hpp"
#include "utils/random.hpp"


const unsigned max_tries = 1000000;


TEST_CASE("special")
{
    using std::uint64_t;
    using x64 = xint::uint<64>;

    {
        uint64_t a = 18021314295701889875ull;
        uint64_t b =   152711102361896281ull;
        uint64_t c = a / b;
        uint64_t d = a % b;

        x64 xa = a;
        x64 xb = b;
        auto [xc, xd] = eval_div(xa, xb);

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
        auto [xc, xd] = eval_div(xa, xb);

        CHECK(xc == c);
        CHECK(xd == d);
    }
}


TEST_CASE("random 64")
{
    using std::uint64_t;
    using x64 = xint::uint<64>;
    for (unsigned i = 0; i < max_tries; ++i) {
        uint64_t a = utils::rand64();
        uint64_t b = utils::rand64();
        if (!b)
            continue;

        uint64_t c = a / b;
        uint64_t d = a % b;

        x64 xa = a;
        x64 xb = b;
        auto [xc, xd] = eval_div(xa, xb);

        CHECK(xc == c);
        CHECK(xd == d);
    }
}
