#ifndef XINT_STDLIB_HPP
#define XINT_STDLIB_HPP

#include <algorithm> // min()
#include <bit>
#include <limits>
#include <string>
#include <type_traits>
#include <utility> // swap(), pair

#include "eval-bits.hpp"
#include "limits.hpp"
#include "traits.hpp"
#include "uint.hpp"
#include "uint-conversions.hpp"


namespace xint {


    template<unsigned_integral U1,
             unsigned_integral U2>
    void swap(U1& a, U2& b)
        noexcept
    {
        swap(a.data, b.data);
    }


    template<unsigned_integral U>
    std::string
    to_string(const U& a)
    {
        return a.to_dec();
    }


    template<unsigned_integral U>
    constexpr
    bool
    has_single_bit(const U& x)
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


    template<unsigned_integral U>
    constexpr
    U
    bit_ceil(const U& x)
        noexcept(noexcept(U{}))
    {
        if (x <= 1)
            return limb_type{1};
        if (has_single_bit(x))
            return x;
        U y = limb_type{0};
        const unsigned top_bit = bit_width(x);
        assert(top_bit + 1 < size(y.limbs()));
        eval_bit_set(y.limbs(), top_bit + 1, true);
        return y;
    }


    template<unsigned_integral U>
    constexpr
    U
    bit_floor(const U& x)
        noexcept(noexcept(U{}))
    {
        if (!x)
            return limb_type{0};
        U y = 0;
        eval_bit_set(y.limbs(), bit_width(x), true);
        return y;
    }


    template<unsigned_integral U>
    constexpr
    unsigned
    bit_width(const U& x)
        noexcept
    {
        return U::num_bits - countl_zero(x);
    }


    template<unsigned_integral U>
    [[nodiscard]]
    constexpr
    U
    rotl(const U& x,
         int r)
        noexcept(noexcept(U{}))
    {
        if (r == 0)
            return x;
        if (r < 0)
            return rotr(x, -r);
        return safety_cast<U>((safety_cast<false>(x) << r)
                              |
                              (safety_cast<false>(x) >> (U::num_bits - r)));
    }


    template<unsigned_integral U>
    [[nodiscard]]
    constexpr
    U
    rotr(const U& x,
         int r)
        noexcept(noexcept(U{}))
    {
        if (r == 0)
            return x;
        if (r < 0)
            return rotl(x, -r);
        return safety_cast<U>((safety_cast<false>(x) >> r)
                              |
                              safety_cast<false>(x) << (U::num_bits - r));
    }


    template<unsigned_integral U>
    unsigned
    countl_zero(const U& x)
        noexcept
    {
        return eval_bit_countl_zero(x.limbs());
    }


    template<unsigned_integral U>
    unsigned
    countl_one(const U& x)
        noexcept
    {
        return eval_bit_countl_one(x.limbs());
    }


    template<unsigned_integral U>
    unsigned
    countr_zero(const U& x)
        noexcept
    {
        return eval_bit_countr_zero(x.limbs());
    }


    template<unsigned_integral U>
    unsigned
    countr_one(const U& x)
        noexcept
    {
        return eval_bit_countr_one(x.limbs());
    }


    template<unsigned_integral U>
    unsigned
    popcount(const U& x)
        noexcept
    {
        return eval_bit_popcount(x.limbs());
    }


    // this is Stein's binary GCD algorithm
    template<unsigned_integral U>
    U
    gcd(U a,
        U b)
        noexcept(noexcept(U{}))
    {
        if (!a)
            return b;
        if (!b)
            return a;

        // find trailing zeros for both
        const unsigned az = countr_zero(a);
        unsigned bz = countr_zero(b);
        const unsigned cz = std::min(az, bz);

        a >>= az;

        while (bz != U::num_bits) { // equivalent to asking if b != 0

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


    template<unsigned_integral U>
    U
    lcm(const U& a,
        const U& b)
    //noexcept(!is_safe_v<U> && is_local_v<U>)
        noexcept(noexcept(a / gcd(a, b) * b))
    {
        if (!a || !b)
            return 0;
        return a / gcd(a, b) * b;
    }


    template<unsigned_integral U>
    U
    midpoint(const U& a,
             const U& b)
        noexcept(noexcept(U{}))
    {
        // note: must avoid overflow
        U c;
        bool overflow = eval_add(c.limbs(), a.limbs(), b.limbs());
        eval_bit_shift_right(c.limbs(), c.limbs(), 1);
        eval_bit_set(c.limbs(), U::num_bits - 1, overflow);
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



#endif
