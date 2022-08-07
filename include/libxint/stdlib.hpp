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
    has_single_bit(const U& a)
        noexcept
    {
        return eval_bit_has_single_bit(a);
    }


    template<unsigned_integral U>
    constexpr
    bool
    bit_get(const U& a,
            unsigned i)
        noexcept
    {
        return eval_bit_get(a.limbs(), i);
    }


    template<unsigned_integral U>
    constexpr
    bool
    bit_set(const U& a,
            unsigned i,
            bool val)
        noexcept
    {
        return eval_bit_set(a.limbs(), i, val);
    }


    template<unsigned_integral U>
    constexpr
    U
    bit_ceil(const U& a)
        noexcept(noexcept(U{}))
    {
        U b;
        eval_bit_ceil(b.limbs());
        return b;
    }


    template<unsigned_integral U>
    constexpr
    U
    bit_floor(const U& a)
        noexcept(noexcept(U{}))
    {
        U b;
        eval_bit_floor(b.limbs());
        return b;
    }


    template<unsigned_integral U>
    constexpr
    unsigned
    bit_width(const U& a)
        noexcept
    {
        //return U::num_bits - countl_zero(a);
        return eval_bit_width(a.limbs());
    }


    template<unsigned_integral U>
    [[nodiscard]]
    constexpr
    U
    rotl(const U& a,
         int r)
        noexcept(noexcept(U{}))
    {
        if (r == 0)
            return a;
        if (r < 0)
            return rotr(a, -r);
        return safety_cast<U>((safety_cast<false>(a) << r)
                              |
                              (safety_cast<false>(a) >> (U::num_bits - r)));
    }


    template<unsigned_integral U>
    [[nodiscard]]
    constexpr
    U
    rotr(const U& a,
         int r)
        noexcept(noexcept(U{}))
    {
        if (r == 0)
            return a;
        if (r < 0)
            return rotl(a, -r);
        return safety_cast<U>((safety_cast<false>(a) >> r)
                              |
                              safety_cast<false>(a) << (U::num_bits - r));
    }


    template<unsigned_integral U>
    unsigned
    countl_zero(const U& a)
        noexcept
    {
        return eval_bit_countl_zero(a.limbs());
    }


    template<unsigned_integral U>
    unsigned
    countl_one(const U& a)
        noexcept
    {
        return eval_bit_countl_one(a.limbs());
    }


    template<unsigned_integral U>
    unsigned
    countr_zero(const U& a)
        noexcept
    {
        return eval_bit_countr_zero(a.limbs());
    }


    template<unsigned_integral U>
    unsigned
    countr_one(const U& a)
        noexcept
    {
        return eval_bit_countr_one(a.limbs());
    }


    template<unsigned_integral U>
    unsigned
    popcount(const U& a)
        noexcept
    {
        return eval_bit_popcount(a.limbs());
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


   template<xint::unsigned_integral U>
   U
   powm(const U& x,
        const U& y,
        const U& m)
   {
       U r = 1;
       auto xx = x;
       const unsigned y_width = bit_width(y);
       for (unsigned b = 0; b < y_width; ++b) {
           if (xint::eval_bit_get(y.limbs(), b))
               r = r * xx % m;
           xx = xx * xx % m;
       }
       return r;
   }


}



#endif
