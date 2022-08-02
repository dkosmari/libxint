#ifndef XINT_STDLIB_HPP
#define XINT_STDLIB_HPP

#include <algorithm> // min()
#include <bit>
#include <cmath>
#include <limits>
#include <string>
#include <utility> // swap(), pair

#include "eval-bits.hpp"
#include "uint.hpp"


namespace xint {


    template<unsigned Bits, bool Safe1, bool Safe2>
    void swap(uint<Bits, Safe1>& a,
              uint<Bits, Safe2>& b)
        noexcept
    {
        swap(a.data, b.data);
    }


    template<unsigned Bits, bool Safe>
    std::string
    to_string(const uint<Bits, Safe>& a)
    {
        return a.to_dec();
    }


    template<unsigned Bits, bool Safe>
    constexpr
    bool
    has_single_bit(const uint<Bits, Safe>& x)
        noexcept
    {
        bool found = false;
        for (auto d : x.limbs()) {
            if (std::has_single_bit(x)) {
                if (found)
                    return false;
                else
                    found = true;
            } else if (x) // more than a single 1 on this limb
                return false;
        }
        return found;
    }


    template<unsigned Bits, bool Safe>
    constexpr
    uint<Bits, Safe>
    bit_ceil(const uint<Bits, Safe>& x)
        noexcept(!Safe)
    {
        if (x <= 1u)
            return 1u;
        if (has_single_bit(x))
            return x;
        uint<Bits, Safe> y = 0;
        const unsigned top_bit = Bits - countl_zero(x);
        assert(top_bit + 1 < size(y.limbs()));
        y.bit(top_bit + 1, true);
        return y;
    }


    template<unsigned Bits, bool Safe>
    constexpr
    uint<Bits, Safe>
    bit_floor(const uint<Bits, Safe>& x)
        noexcept(!Safe)
    {
        if (!x)
            return 0u;
        uint<Bits, Safe> y = 0;
        y.bit(bit_width(x), true);
        return y;
    }


    template<unsigned Bits, bool Safe>
    constexpr
    uint<Bits, Safe>
    bit_width(const uint<Bits, Safe>& x)
        noexcept
    {
        return Bits - countl_zero(x);
    }


    template<unsigned Bits, bool Safe>
    [[nodiscard]]
    constexpr
    uint<Bits, Safe>
    rotl(const uint<Bits, Safe>& x,
         int r)
        noexcept(!Safe)
    {
        if (r == 0)
            return x;
        if (r < 0)
            return rotr(x, -r);
        return (x << r) | (x >> (Bits - r));
    }


    template<unsigned Bits, bool Safe>
    [[nodiscard]]
    constexpr
    uint<Bits, Safe>
    rotr(const uint<Bits, Safe>& x,
         int r)
        noexcept(!Safe)
    {
        if (r == 0)
            return x;
        if (r < 0)
            return rotl(x, -r);
        return (x >> r) | (x << (Bits - r));
    }


    template<unsigned Bits, bool Safe>
    unsigned
    countl_zero(const uint<Bits, Safe>& x)
        noexcept
    {
        return eval_bit_countl_zero(x.limbs());
    }


    template<unsigned Bits, bool Safe>
    unsigned
    countl_one(const uint<Bits, Safe>& x)
        noexcept
    {
        return eval_bit_countl_one(x.limbs());
    }


    template<unsigned Bits, bool Safe>
    unsigned
    countr_zero(const uint<Bits, Safe>& x)
        noexcept
    {
        return eval_bit_countr_zero(x.limbs());
    }


    template<unsigned Bits, bool Safe>
    unsigned
    countr_one(const uint<Bits, Safe>& x)
        noexcept
    {
        return eval_bit_countr_one(x.limbs());
    }


    template<unsigned Bits, bool Safe>
    unsigned
    popcount(const uint<Bits, Safe>& x)
        noexcept
    {
        return eval_bit_popcount(x.limbs());
    }


    // this is Stein's binary GCD algorithm
    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>
    gcd(uint<Bits, Safe> a,
        uint<Bits, Safe> b)
        noexcept(uint<Bits, Safe>::is_local)
    {
        if (!a)
            return b;
        if (!b)
            return a;

        // find trailing zeros for both
        const auto az = countr_zero(a);
        auto bz = countr_zero(b);
        const auto cz = std::min(az, bz);

        a >>= az;

        while (bz != Bits) { // equivalent to asking if b != 0

            // invariant: a is always odd

            b >>= bz; // now b is also odd

            if (a > b)
                swap(a, b);
            b -= a; // b becomes even

            bz = countr_zero(b);
        }

        a <<= cz;
        return a;
    }

    // TODO: extended gcd


    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>
    lcm(const uint<Bits, Safe>& a,
        const uint<Bits, Safe>& b)
        noexcept(!Safe && uint<Bits, Safe>::is_local)
    {
        if (!a || !b)
            return 0;
        return a / gcd(a, b) * b;
    }


    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>
    midpoint(const uint<Bits, Safe>& a,
             const uint<Bits, Safe>& b)
        noexcept(uint<Bits, Safe>::is_local)
    {
        // note: must avoid overflow
        uint<Bits, Safe> c;
        bool overflow = eval_add(c.limbs(), a.limbs(), b.limbs());
        eval_bit_shift_right(c.limbs(), c.limbs(), 1);
        eval_bit_set(c.limbs(), Bits - 1, overflow);
        return c;
    }


    template<unsigned Bits, bool Safe>
    std::pair<uint<Bits, Safe>,
              uint<Bits, Safe>>
    div(const uint<Bits, Safe>& a,
        const uint<Bits, Safe>& b)
    {
        return eval_div(a, b);
    }


}


namespace std {

    template<unsigned Bits, bool Safe>
    struct numeric_limits<xint::uint<Bits, Safe>> {
        static constexpr bool is_specialized = true;
        static constexpr bool is_signed = false;
        static constexpr bool is_integer = true;
        static constexpr bool is_exact = true;
        static constexpr bool has_infinity = false;
        static constexpr bool has_quiet_NaN = false;
        static constexpr bool has_signaling_NaN = false;
        static constexpr float_denorm_style has_denorm = denorm_absent;
        static constexpr bool has_denorm_loss = false;
        static constexpr float_round_style round_style = round_toward_zero;
        static constexpr bool is_iec559 = false;
        static constexpr bool is_bounded = true;
        static constexpr bool is_modulo = true;
        static constexpr int digits = xint::uint<Bits, Safe>::num_limbs; // 2
        static constexpr int digits10 = Bits * log10(2);
        static constexpr int max_digits10 = 0;
        static constexpr auto radix = xint::wide_limb_type{1} << xint::limb_bits;
        static constexpr int min_exponent = 0;
        static constexpr int min_exponent10 = 0;
        static constexpr int max_exponent = 0;
        static constexpr int max_exponent10 = 0;
        static constexpr bool traps = true;
        static constexpr bool tinyness_before = false;

        static constexpr xint::uint<Bits, Safe> denorm_min() noexcept { return 0u; }
        static constexpr xint::uint<Bits, Safe> epsilon() noexcept { return 0u; }
        static constexpr xint::uint<Bits, Safe> infinity() noexcept { return 0u; }
        static constexpr xint::uint<Bits, Safe> lowest() noexcept { return min(); }
        static constexpr xint::uint<Bits, Safe> max() noexcept { return ~min(); }
        static constexpr xint::uint<Bits, Safe> min() noexcept { return 0u; }
        static constexpr xint::uint<Bits, Safe> quiet_NaN() noexcept { return 0u; }
        static constexpr xint::uint<Bits, Safe> round_error() noexcept { return 0u; }
        static constexpr xint::uint<Bits, Safe> signaling_NaN() noexcept { return 0u; }
    };


    template<unsigned Bits1, unsigned Bits2,
             bool Safe>
    struct common_type<xint::uint<Bits1, Safe>,
                       xint::uint<Bits2, Safe>> {
        using type = xint::uint<max(Bits1, Bits2), Safe>;
    };


}



#endif
