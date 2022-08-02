#include <cstdint>
#include <iomanip>
#include <ostream>
#include <string>

#include <libxint/uint.hpp>

#include "catch2/catch_amalgamated.hpp"


using std::string;
using std::uint64_t;

using x64 = xint::uint<64>;
using x128 = xint::uint<128>;

using uvector = std::vector<unsigned char>;

using std::ranges::equal;
using std::ranges::fill;


TEST_CASE("test 1")
{
    x64 a = uint64_t{0x0123456789abcdefull};

    string a_bin = "100100011010001010110011110001001101010111100110111101111";
    CHECK(a.to_bin() == a_bin);

    string a_dec = "81985529216486895";
    CHECK(a.to_dec() == a_dec);

    string a_hex = "123456789abcdef";
    CHECK(a.to_hex() == a_hex);

    string a_oct = "4432126361152746757";
    CHECK(a.to_oct() == a_oct);
}


TEST_CASE("zero")
{
    x64 a = 0;
    CHECK(a.to_bin() == "0");
    CHECK(a.to_dec() == "0");
    CHECK(a.to_hex() == "0");
    CHECK(a.to_oct() == "0");
}


TEST_CASE("endian")
{
    {
        x128 a{"0x00112233445566778899aabbccddeeff"};
        uvector ref_little = {
            0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88,
            0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00
        };
        uvector ref_big = {
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
        };

        uvector data(16);

        fill(data, 0);
        a.to_little_endian(data);
        CHECK(ref_little == data);

        fill(data, 0);
        a.to_big_endian(data);
        CHECK(ref_big == data);

        data.resize(15);
        CHECK_THROWS_AS(a.to_little_endian(data), std::out_of_range);


        data.resize(20);

        fill(data, 0);
        a.to_little_endian(data);
        CHECK(equal(ref_little, data | std::views::take(16)));

        fill(data, 0);
        a.to_big_endian(data);
        CHECK(equal(ref_big, data | std::views::take(16)));

    }

    {
        // x128 a{1u};
        // std::array<unsigned char, 16> data;
        // a.to_little_endian(data);

    }


}
