#ifndef XINT_UINT_OPERATORS_HPP
#define XINT_UINT_OPERATORS_HPP

#include <algorithm>
#include <compare>
#include <concepts>
#include <limits>
#include <ostream>
#include <ranges>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "eval-addition.hpp"
#include "eval-assignment.hpp"
#include "eval-bits.hpp"
#include "eval-comparison.hpp"
#include "eval-division.hpp"
#include "eval-inc-dec.hpp"
#include "eval-io.hpp"
#include "eval-multiplication.hpp"
#include "eval-subtraction.hpp"
#include "stdlib.hpp"
#include "traits.hpp"
#include "uint.hpp"


// TODO: allow mixed arithmetic with native integers


namespace xint {


    /* ---------- */
    /* Assignment */
    /* ---------- */



    // a = b

    template<unsigned Bits, bool Safe>
    template<unsigned Bits2, bool Safe2>
    uint<Bits, Safe>&
    uint<Bits, Safe>::operator =(const uint<Bits2, Safe2>& other)
        noexcept((Bits == Bits2) || (!Safe && !Safe2))
    {
        if constexpr (Bits != Bits2) {
            bool overflow = eval_assign(limbs(), other.limbs());
            if constexpr (Safe || Safe2)
                if (overflow)
                    throw std::overflow_error{"overflow in ="};
        } else
            limbs() = other.limbs();
        return *this;
    }



    // a += b

    template<unsigned_integral U1,
             unsigned_integral U2>
    U1&
    operator +=(U1& a,
                const U2& b)
        noexcept(!are_safe_v<U1, U2>)
    {
        bool overflow = eval_add_inplace(a.limbs(), b.limbs());
        if constexpr (are_safe_v<U1, U2>)
            if (overflow)
                throw std::overflow_error{"overflow in +="};
        return a;
    }


    template<unsigned_integral U,
             std::integral I>
    U&
    operator +=(U& a,
                I b)
        noexcept(noexcept(a += uint(b)))
    {
        return a += uint(b);
    }



    // a -= b

    template<unsigned_integral U1,
             unsigned_integral U2>
    U1&
    operator -=(U1& a,
                const U2& b)
        noexcept(!are_safe_v<U1, U2>)
    {
        bool overflow = eval_sub_inplace(a.limbs(), b.limbs());
        if constexpr (are_safe_v<U1, U2>)
            if (overflow)
                throw std::overflow_error{"overflow in -="};
        return a;
    }


    template<unsigned_integral U,
             std::integral I>
    U&
    operator -=(U& a,
                I b)
        noexcept(noexcept(a += uint(b)))
    {
        return a -= uint(b);
    }



    // a *= b

    template<unsigned_integral U1,
             unsigned_integral U2>
    U1&
    operator *=(U1& a,
                const U2& b)
        noexcept(noexcept(U1{}) && !are_safe_v<U1, U2>)
    {
        U1 c;
        bool overflow = eval_mul_simple(c.limbs(), a.limbs(), b.limbs());
        if constexpr (are_safe_v<U1, U2>)
            if (overflow)
                throw std::overflow_error{"overflow in *="};
        return a = std::move(c);
    }


    template<unsigned_integral U,
             std::integral I>
    U&
    operator *=(U& a,
                I b)
        noexcept(noexcept(a *= uint(b)))
    {
        return a *= uint(b);
    }



    // a /= b

    template<unsigned_integral U>
    U&
    operator /=(U& a,
                const U& b)
        noexcept(noexcept(a = eval_div(a, b).first))
    {
        return a = eval_div(a, b).first;
    }


    // a %= b

    template<unsigned_integral U>
    U&
    operator %=(U& a,
                const U& b)
        noexcept(noexcept(a = eval_div(a, b).second))
    {
        return a = eval_div(a, b).second;
    }


    // a &= b

    template<unsigned_integral U>
    U&
    operator &=(U& a,
                const U& b)
        noexcept
    {
        eval_bit_and(a.limbs(), a.limbs(), b.limbs());
        return a;
    }




    // a |= b
    template<unsigned_integral U>
    U&
    operator |=(U& a,
                const U& b)
        noexcept
    {
        eval_bit_or(a.limbs(), a.limbs(), b.limbs());
        return a;
    }


    // a ^= b
    template<unsigned_integral U>
    U&
    operator ^=(U& a,
                const U& b)
        noexcept
    {
        eval_bit_xor(a.limbs(), a.limbs(), b.limbs());
        return a;
    }


    // a <<= b
    template<unsigned_integral U>
    U&
    operator <<=(U& a,
                 unsigned b)
        noexcept (!is_safe_v<U>)
    {
        bool overflow = eval_bit_shift_left<is_safe_v<U>>(a.limbs(), a.limbs(), b);
        if constexpr (is_safe_v<U>)
            if (overflow)
                throw std::overflow_error{"overflow in <<="};
        return a;
    }


    // a >>= b
    template<unsigned_integral U>
    U&
    operator >>=(U& a,
                 unsigned b)
        noexcept(!is_safe_v<U>)
    {
        bool overflow = eval_bit_shift_right<is_safe_v<U>>(a.limbs(), a.limbs(), b);
        if constexpr (is_safe_v<U>)
            if (overflow)
                throw std::overflow_error{"overflow in >>="};
        return a;
    }



    /* ------------------- */
    /* Increment/Decrement */
    /* ------------------- */


    // ++ a
    template<unsigned_integral U>
    U&
    operator ++(U& a)
        noexcept(!is_safe_v<U>)
    {
        bool overflow = eval_increment(a.limbs());
        if constexpr (is_safe_v<U>)
            if (overflow)
                throw std::overflow_error{"overflow in ++"};
        return a;
    }


    // -- a
    template<unsigned_integral U>
    U&
    operator --(U& a)
        noexcept(!is_safe_v<U>)
    {
        bool overflow = eval_decrement(a.limbs());
        if constexpr (is_safe_v<U>)
            if (overflow)
                throw std::overflow_error{"overflow in --"};
        return a;
    }


    // a ++
    template<unsigned_integral U>
    U
    operator ++(U& a, int)
        noexcept(noexcept(U{}) && !is_safe_v<U>)
    {
        U b;
        bool overflow = eval_increment(a.limbs(), b.limbs());
        if constexpr (is_safe_v<U>)
            if (overflow)
                throw std::overflow_error{"overflow in ++"};
        return b;
    }


    // a --
    template<unsigned_integral U>
    U
    operator --(U& a, int)
        noexcept(noexcept(U{}) && !is_safe_v<U>)
    {
        U b;
        bool overflow = eval_decrement(a.limbs(), b.limbs());
        if constexpr (is_safe_v<U>)
            if (overflow)
                throw std::overflow_error{"overflow in --"};
        return b;
    }



    /* ---------- */
    /* Arithmetic */
    /* ---------- */


    // + a
    template<unsigned_integral U>
    const U&
    operator +(const U& a)
        noexcept
    {
        return a;
    }


    // - a
    template<unsigned_integral U>
    U
    operator -(const U& a)
        noexcept(noexcept(U{}))
    {
        U b;
        eval_bit_flip(b.limbs(), a.limbs());
        eval_increment(b.limbs());
        return b;
    }


    // a + b
    template<unsigned_integral U1,
             unsigned_integral U2>
    std::common_type_t<U1, U2>
    operator +(const U1& a,
               const U2& b)
        noexcept(noexcept(std::common_type_t<U1, U2>{}) && !are_safe_v<U1, U2>)
    {
        std::common_type_t<U1, U2> result;
        bool overflow = eval_add(result.limbs(),
                                 a.limbs(),
                                 b.limbs());
        if constexpr (are_safe_v<U1, U2>)
            if (overflow)
                throw std::overflow_error{"overflow in +"};
        return result;
    }

    template<unsigned_integral U,
             std::integral I>
    std::common_type_t<U, I>
    operator +(const U& a,
               I b)
        noexcept(noexcept(a + uint(b)))
    {
        return a + uint(b);
    }

    template<std::integral I,
             unsigned_integral U>
    std::common_type_t<I, U>
    operator +(I a,
               const U& b)
        noexcept(noexcept(uint(a) + b))
    {
        return uint(a) + b;
    }


    // a - b
    template<unsigned_integral U1,
             unsigned_integral U2>
    std::common_type_t<U1, U2>
    operator -(const U1& a,
               const U2& b)
        noexcept(noexcept(std::common_type_t<U1, U2>{}) && !are_safe_v<U1, U2>)
    {
        std::common_type_t<U1, U2> result;
        bool underflow = eval_sub(result.limbs(),
                                  a.limbs(),
                                  b.limbs());
        if constexpr (are_safe_v<U1, U2>)
            if (underflow)
                throw std::overflow_error{"overflow in -"};
        return result;
    }

    template<unsigned_integral U,
             std::integral I>
    std::common_type_t<U, I>
    operator -(const U& a,
               I b)
        noexcept(noexcept(a - uint(b)))
    {
        return a - uint(b);
    }

    template<std::integral I,
             unsigned_integral U>
    std::common_type_t<I, U>
    operator -(I a,
               const U& b)
        noexcept(noexcept(uint(a) - b))
    {
        return uint(a) - b;
    }


    // a * b
    template<unsigned_integral U1,
             unsigned_integral U2>
    std::common_type_t<U1, U2>
    operator *(const U1& a,
               const U2& b)
        noexcept(noexcept(std::common_type_t<U1, U2>{}) && !are_safe_v<U1, U2>)
    {
        std::common_type_t<U1, U2> result;
        bool overflow = eval_mul_simple(result.limbs(),
                                        a.limbs(),
                                        b.limbs());
        if constexpr (are_safe_v<U1, U2>)
            if (overflow)
                throw std::overflow_error{"overflow in *"};
        return result;
    }


    template<unsigned_integral U,
             std::integral I>
    std::common_type_t<U, I>
    operator *(const U& a,
               I b)
        noexcept(noexcept(std::common_type_t<U, I>{}) && !is_safe_v<U>)
    {
        if constexpr (sizeof(I) <= sizeof(limb_type)) {
            std::common_type_t<U, I> result;
            bool overflow = eval_mul_limb(result.limbs(),
                                          a.limbs(),
                                          b);
            if constexpr (is_safe_v<U>)
                if (overflow)
                    throw std::overflow_error{"overflow in *"};
            return result;
        } else
            return a * uint(b);
    }


    template<std::integral I,
             unsigned_integral U>
    std::common_type_t<I, U>
    operator *(I a,
               const U& b)
        noexcept(noexcept(b * a))
    {
        return b * a;
    }


    // a / b
    template<unsigned_integral U>
    U
    operator /(const U& a,
               const U& b)
        noexcept(noexcept(eval_div(a, b)))
    {
        return eval_div(a, b).first;
    }


    // a % b
    template<unsigned_integral U>
    U
    operator %(const U& a,
               const U& b)
        noexcept(noexcept(eval_div(a, b)))
    {
        return eval_div(a, b).second;
    }


    // a % b
    template<unsigned_integral U,
             std::integral I>
    requires(sizeof(I) <= sizeof(limb_type))
    limb_type
    operator %(const U& a,
               I b)
        noexcept(noexcept(eval_div_limb(a, b)))
    {
        return eval_div_limb(a, b).second;
    }


    // ~ a
    template<unsigned_integral U>
    U
    operator ~(const U& a)
        noexcept(noexcept(U{}))
    {
        U b;
        eval_bit_flip(b.limbs(), a.limbs());
        return b;
    }


    // a & b
    template<unsigned_integral U1,
             unsigned_integral U2>
    std::common_type_t<U1, U2>
    operator &(const U1& a,
               const U2& b)
        noexcept(noexcept(std::common_type_t<U1, U2>{}))
    {
        std::common_type_t<U1, U2> c;
        eval_bit_and(c.limbs(), a.limbs(), b.limbs());
        return c;
    }


    // a | b
    template<unsigned_integral U1,
             unsigned_integral U2>
    std::common_type_t<U1, U2>
    operator |(const U1& a,
               const U2& b)
        noexcept(noexcept(std::common_type_t<U1, U2>{}))
    {
        std::common_type_t<U1, U2> c;
        eval_bit_or(c.limbs(), a.limbs(), b.limbs());
        return c;
    }


    // a ^ b
    template<unsigned_integral U1,
             unsigned_integral U2>
    std::common_type_t<U1, U2>
    operator ^(const U1& a,
               const U2& b)
        noexcept(noexcept(std::common_type_t<U1, U2>{}))
    {
        std::common_type_t<U1, U2> c;
        eval_bit_xor(c.limbs(), a.limbs(), b.limbs());
        return c;
    }


    // a << b
    template<unsigned_integral U>
    U
    operator <<(const U& a,
                unsigned b)
        noexcept(noexcept(U{}) && !is_safe_v<U>)
    {
        U c;
        bool overflow = eval_bit_shift_left<is_safe_v<U>>(c.limbs(),
                                                          a.limbs(),
                                                          b);
        if constexpr (is_safe_v<U>)
            if (overflow)
                throw std::overflow_error{"overflow in <<"};
        return c;
    }


    // a >> b
    template<unsigned_integral U>
    U
    operator >>(const U& a,
                unsigned b)
        noexcept(noexcept(U{}) && !is_safe_v<U>)
    {
        U c;
        bool overflow = eval_bit_shift_right<is_safe_v<U>>(c.limbs(),
                                                           a.limbs(),
                                                           b);
        if constexpr (is_safe_v<U>)
            if (overflow)
                throw std::overflow_error{"overflow in >>"};
        return c;
    }



    /* ---------- */
    /* Comparison */
    /* ---------- */


    template<unsigned_integral U1,
             unsigned_integral U2>
    bool
    operator ==(const U1& a,
                const U2& b)
        noexcept
    {
        if constexpr (U1::num_bits == U2::num_bits)
            return a.limbs() == b.limbs();
        else
            return eval_compare_equal(a.limbs(), b.limbs());
    }

    // TODO: check if this overload is necessary
    template<unsigned_integral U,
             std::integral I>
    bool
    operator ==(const U& a,
                I b)
        noexcept(noexcept(uint(b)))
    {
        return a == uint(b);
    }


    template<unsigned_integral U,
             std::integral I>
    bool
    operator ==(I a,
                const U& b)
        noexcept(noexcept(uint(a)))
    {
        return uint(a) == b;
    }


    template<unsigned_integral U1,
             unsigned_integral U2>
    std::strong_ordering
    operator <=>(const U1& a,
                 const U2& b)
        noexcept
    {
        using std::views::reverse;
        if constexpr (U1::num_bits == U2::num_bits)
            return std::lexicographical_compare_three_way(rbegin(a.limbs()),
                                                          rend(a.limbs()),
                                                          rbegin(b.limbs()),
                                                          rend(b.limbs()));
        else
            return eval_compare_three_way(a.limbs(), b.limbs());
    }


    template<unsigned_integral U,
             std::integral I>
    std::strong_ordering
    operator <=>(const U& a,
                 I b)
        noexcept(noexcept(uint(b)))
    {
        return a <=> uint(b);
    }


    template<std::integral I,
             unsigned_integral U>
    std::strong_ordering
    operator <=>(I a,
                 const U& b)
        noexcept(noexcept(uint(a)))
    {
        return uint(a) <=> b;
    }




    /* --- */
    /* I/O */
    /* --- */



    template<unsigned_integral U>
    std::istream&
    operator >>(std::istream& in,
                U& n)
    {
        std::ostream::sentry s{in};
        if (s)
            eval_input(in, n);
        return in;
    }


    template<unsigned_integral U>
    std::ostream&
    operator <<(std::ostream& out,
                const U& n)
    {
        std::ostream::sentry s{out};
        if (s)
            eval_output(out, n);
        return out;
    }
}


#endif
