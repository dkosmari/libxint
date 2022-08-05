#include <cstdint>
#include <limits>
#include <random>

#undef XINT_LIMB_SIZE
#define XINT_LIMB_SIZE 32
#include <libxint/uint.hpp>
#include <libxint/prime.hpp>

#include "catch2/catch_amalgamated.hpp"


TEST_CASE("prime-counting")
{
    using std::uint64_t;
    using xint::uint;
    using xint::miller_rabin;

    std::random_device dev;
    std::mt19937 engine{dev()};

    {
        unsigned count = 0;
        const uint64_t max = (1 << 16) - 1;
        for (uint64_t i = 0; i <= max; ++i)
            if (miller_rabin<uint<64, false>>(i, 25, engine))
                ++count;
        CHECK(count == 6542);
    }

    {
        unsigned count = 0;
        const uint64_t max = (1 << 16) - 1;
        for (uint64_t i = 0; i <= max; ++i)
            if (miller_rabin<uint<64, true>>(i, 25, engine))
                ++count;
        CHECK(count == 6542);
    }
}


TEST_CASE("overflow")
{
    using x64 = xint::uint<64, true>;

    x64 p = 4294967291u;
    CHECK(miller_rabin(p, 25));

    x64 q = 8589934583ull;
    CHECK_THROWS_AS(miller_rabin(q, 25), std::overflow_error);

    xint::uint<128, true> r = q;
    CHECK(miller_rabin(r, 25));
}
