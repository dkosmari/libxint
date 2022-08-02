#include <cstdint>
#include <vector>

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
        uint64_t c = a + b;

        x64 xa = a;
        x64 xb = b;
        x64 xc = xa + xb;

        CHECK(xc == c);
    }
}


TEST_CASE("eval_add")
{
    using std::vector;
    using std::uint8_t;
    using vec = vector<uint8_t>;


    // same lengths

    {
        // no overflow
        vec a = { 1, 1, 1, 1 };
        vec b = { 4, 4, 4, 4 };
        vec c = { 5, 5, 5, 5 };
        vec out(4);
        CHECK(!xint::eval_add(out, a, b));
        CHECK(c == out);
    }

    {
        // with overflow
        vec a = { 1, 0, 0, 0 };
        vec b = { 0xff, 0xff, 0xff, 0xff };
        vec c = { 0, 0, 0, 0 };
        vec out(4);
        CHECK(xint::eval_add(out, a, b) == true);
        CHECK(c == out);
    }

    {
        // no overflow
        vec a = { 1, 0, 0, 0 };
        vec b = { 0xff, 0xff, 0xff, 0x7f };
        vec c = { 0, 0, 0, 0x80 };
        vec out(4);
        CHECK(!xint::eval_add(out, a, b));
        CHECK(c == out);
    }


    // out is longer: must fill with zeros, never overflow

    {
        // no overflow
        vec a = { 1, 1, 1, 1 };
        vec b = { 4, 4, 4, 4 };
        vec c = { 5, 5, 5, 5, 0 };
        vec out(5);
        CHECK(!xint::eval_add(out, a, b));
        CHECK(c == out);
    }

    {
        // still no overflow because out has enough space
        vec a = { 1, 0, 0, 0 };
        vec b = { 0xff, 0xff, 0xff, 0xff };
        vec c = { 0, 0, 0, 0, 1};
        vec out(5);
        CHECK(!xint::eval_add(out, a, b));
        CHECK(c == out);
    }


    // a is longer than b and out

    {
        // non-zero truncated, overflow
        vec a = { 1, 1, 1, 1, 1 };
        vec b = { 4, 4, 4, 4 };
        vec c = { 5, 5, 5, 5 };
        vec out(4);
        CHECK(xint::eval_add(out, a, b));
        CHECK(c == out);
    }

    {
        // zero truncated, no overflow
        vec a = { 1, 1, 1, 1, 0 };
        vec b = { 4, 4, 4, 4 };
        vec c = { 5, 5, 5, 5 };
        vec out(4);
        CHECK(!xint::eval_add(out, a, b));
        CHECK(c == out);
    }


    // b is longer than a and out

    {
        // non-zero truncated, overflow
        vec a = { 1, 1, 1, 1 };
        vec b = { 4, 4, 4, 4, 4 };
        vec c = { 5, 5, 5, 5 };
        vec out(4);
        CHECK(xint::eval_add(out, a, b));
        CHECK(c == out);
    }

    {
        // zero truncated, no overflow
        vec a = { 1, 1, 1, 1 };
        vec b = { 4, 4, 4, 4, 0 };
        vec c = { 5, 5, 5, 5 };
        vec out(4);
        CHECK(!xint::eval_add(out, a, b));
        CHECK(c == out);
    }


    // b is shifted up

    {
        // shifted once, no overflow
        vec a = { 1, 1, 1, 1 };
        vec b = { 4, 4, 4 };
        vec c = { 1, 5, 5, 5 };
        vec out(4);
        CHECK(!xint::eval_add(out, a, b, 1));
        CHECK(c == out);
    }

    {
        // shifted up completely, overflow
        vec a = { 1, 1, 1, 1 };
        vec b = { 4 };
        vec c = { 1, 1, 1, 1 };
        vec out(4);
        CHECK(xint::eval_add(out, a, b, 7));
        CHECK(c == out);
    }

    {
        // shifted up completely, but zeros, no overflow
        vec a = { 1, 1, 1, 1 };
        vec b = { 0, 0, 0, 0, 0, 0 };
        vec c = { 1, 1, 1, 1 };
        vec out(4);
        CHECK(!xint::eval_add(out, a, b, 7));
        CHECK(c == out);
    }

    {
        // shifted up partially out, no overflow
        vec a = { 1, 1, 1, 1 };
        vec b = { 4, 4, 0 };
        vec c = { 1, 1, 5, 5 };
        vec out(4);
        CHECK(!xint::eval_add(out, a, b, 2));
        CHECK(c == out);
    }

    {
        // shifted up partially out, overflow
        vec a = { 1, 1, 1, 1 };
        vec b = { 4, 4, 4 };
        vec c = { 1, 1, 5, 5 };
        vec out(4);
        CHECK(xint::eval_add(out, a, b, 2));
        CHECK(c == out);
    }

}
