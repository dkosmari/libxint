#include <cstdint>

#include <libxint/uint.hpp>

#include "catch2/catch_amalgamated.hpp"
#include "utils/random.hpp"


const unsigned max_tries = 1000000;


TEST_CASE("random64", "[random][64]")
{
    using std::uint64_t;
    using x64 = xint::uint<64>;
    for (unsigned i = 0; i < max_tries; ++i) {
        uint64_t a = utils::rand64();
        uint64_t b = utils::rand64();
        uint64_t c = a - b;

        x64 xa = a;
        x64 xb = b;
        x64 xc = xa - xb;

        CHECK(xc == c);
    }
}
