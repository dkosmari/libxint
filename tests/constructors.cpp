#include <algorithm>
#include <vector>

#include <libxint/uint.hpp>

#include "catch2/catch_amalgamated.hpp"


using std::uint32_t;
using std::uint64_t;
using std::string;

using namespace std::literals;


using x1024 = xint::uint<1024>;


TEST_CASE("zero")
{
    x1024 z = 0;
    std::vector<xint::limb_type> z_ref(x1024::num_limbs);

    CHECK(!z);
    CHECK(std::ranges::equal(z.limbs(), z_ref));
}


TEST_CASE("u32")
{
    x1024 xa = 0;
    CHECK(!xa);

    x1024 xb = uint32_t{1};
    CHECK(!!xb);

    uint32_t c = ~uint32_t{0};
    x1024 xc = c;
    CHECK(xc == c);
}


TEST_CASE("u64")
{
    x1024 xa = 0;
    CHECK(!xa);

    x1024 xb = uint32_t{1};
    CHECK(!!xb);
    --xb;
    CHECK(!xb);

    uint64_t c = ~uint64_t{0};
    x1024 xc = c;
    CHECK(xc == c);
    CHECK(xc.to_hex() == "ffffffffffffffff");
}



TEST_CASE("string")
{
    x1024 a{"0b000000"};
    CHECK(a.to_bin() == "0"s);

    x1024 b{"000000"};
    CHECK(b.to_dec() == "0"s);

    x1024 c{"0x000000"};
    CHECK(c.to_hex() == "0"s);

    x1024 d{"0o000000"};
    CHECK(d.to_oct() == "0"s);

    {
        string ref = "10010110111010010011111010011110101101111010101";
        x1024 a{"0b" + ref};
        x1024 b{ref, 2};
        CHECK(a.to_bin() == ref);
        CHECK(b.to_bin() == ref);
    }

    {
        string ref = "123456789012345678901234567890";
        x1024 a{ref};
        x1024 b{ref, 10};
        CHECK(a.to_dec() == ref);
        CHECK(b.to_dec() == ref);
    }

    {
        string ref = "123456789abcdef0123456789abcdef0123456789abcdef";
        x1024 a{"0x" + ref};
        x1024 b{ref, 16};
        CHECK(a.to_hex() == ref);
        CHECK(b.to_hex() == ref);
    }

    {
        string ref = "354135402456540441045665716544101657012670";
        x1024 a{"0" + ref};
        x1024 b{"0o" + ref};
        x1024 c{ref, 8};
        CHECK(a.to_oct() == ref);
        CHECK(b.to_oct() == ref);
        CHECK(c.to_oct() == ref);
    }


    {
        string a_ref = "zz";
        x1024 a{a_ref, 36};
        CHECK(a.to_uint<32>() == 35*36 + 35);
        CHECK(a.to_string(36) == a_ref);

        string b_ref = "yy";
        x1024 b{b_ref, 35};
        CHECK(b.to_uint<32>() == 34*35 + 34);
        CHECK(b.to_string(35) == b_ref);
    }


    {
        using x64 = xint::uint<64>;
        // must throw with empty argument
        CHECK_THROWS_AS(x64(""), std::invalid_argument);
        // must throw with conflicting bases
        CHECK_THROWS_AS(x64("0x1", 8), std::invalid_argument);
        // must throw with overflow
        CHECK_THROWS_AS(x64("0x1ffffffffffffffff"), std::overflow_error);
        // must throw with invalid char
        CHECK_THROWS_AS(x64("123:"), std::invalid_argument);
    }

}
