#ifndef XINT_UINT_OPERATORS_HPP
#define XINT_UINT_OPERATORS_HPP

#include <algorithm>
#include <compare>
#include <cstdlib> // aborT()
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


    // Division; not an operator, but more handy than the / and % operators

    template<unsigned_integral U1,
             unsigned_integral U2>
    std::pair<U1, U2>
    div(U1 a,
        U2 b)
        noexcept(noexcept(std::pair<U1, U2>{}) && !any_are_safe_v<U1, U2>)
    {
        std::pair<U1, U2> result;
        auto s = eval_div(result.first.limbs(),
                          result.second.limbs(),
                          a.limbs(),
                          b.limbs());
        if constexpr (any_are_safe_v<U1, U2>) {
            if (s == div_status::div_by_zero)
                throw std::domain_error{"division by zero"};
            if (s == div_status::overflow)
                throw std::overflow_error{"overflow in division"};
        } else if (s == div_status::div_by_zero)
            abort();
        return result;
    }


    template<unsigned_integral U,
             std::integral I>
    std::pair<U, I>
    div(U a,
        I b)
        noexcept(noexcept(std::pair<U, I>{}) && noexcept(uint(b)) && !is_safe_v<U>)
    {
        using Iu = std::make_unsigned_t<I>;
        if constexpr (sizeof(Iu) <= sizeof(limb_type)) {
            limb_type r;
            std::pair<U, I> result;
            auto s = eval_div_limb(result.first, r, a, b);
            result.second = r;

            if constexpr (is_safe_v<U>) {
                if (s == div_status::div_by_zero)
                    throw std::domain_error{"division by zero"};
                if (s == div_status::overflow)
                    throw std::overflow_error{"overflow in division"};
            } else if (s == div_status::div_by_zero)
                abort();
            return result;
        } else {
            auto bb = uint(b);
            auto res = div(a, bb);
            return { res.first, static_cast<Iu>(res.second) };
        }
    }

    template<std::integral I,
             unsigned_integral U>
    std::pair<I, U>
    div(I a,
        U b)
        noexcept(noexcept(std::pair<I, U>{}) && noexcept(uint(a)) && !is_safe_v<U>)
    {
        using Iu = std::make_unsigned_t<I>;
        auto aa = uint(a);
        auto res = div(aa, b);
        return { static_cast<Iu>(res.first), res.second };
    }



    /* ---------- */
    /* Assignment */
    /* ---------- */



    // a = b

    // covered
    template<unsigned Bits, bool Safe>
    template<unsigned Bits2, bool Safe2>
    uint<Bits, Safe>&
    uint<Bits, Safe>::operator =(const uint<Bits2, Safe2>& other)
        noexcept((Bits == Bits2) || (!Safe && !Safe2))
    {
        if constexpr (Bits != Bits2) {
            bool overflow = eval_assign(limbs(), other.limbs());
            if constexpr (Bits < Bits2 && (Safe || Safe2))
                if (overflow)
                    throw std::overflow_error{"overflow in ="};
        } else
            limbs() = other.limbs();
        return *this;
    }



    // a += b

    // covered
    template<unsigned_integral U1,
             unsigned_integral U2>
    U1&
    operator +=(U1& a,
                const U2& b)
        noexcept(!any_are_safe_v<U1, U2>)
    {
        bool overflow = eval_add_inplace(a.limbs(), b.limbs());
        if constexpr (any_are_safe_v<U1, U2>)
            if (overflow)
                throw std::overflow_error{"overflow in +="};
        return a;
    }

    // covered
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

    // covered
    template<unsigned_integral U1,
             unsigned_integral U2>
    U1&
    operator -=(U1& a,
                const U2& b)
        noexcept(!any_are_safe_v<U1, U2>)
    {
        bool overflow = eval_sub_inplace(a.limbs(), b.limbs());
        if constexpr (any_are_safe_v<U1, U2>)
            if (overflow)
                throw std::overflow_error{"overflow in -="};
        return a;
    }

    // covered
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

    // covered
    template<unsigned_integral U1,
             unsigned_integral U2>
    U1&
    operator *=(U1& a,
                const U2& b)
        noexcept(noexcept(U1{}) && !any_are_safe_v<U1, U2>)
    {
        U1 c;
        bool overflow = eval_mul_simple(c.limbs(), a.limbs(), b.limbs());
        if constexpr (any_are_safe_v<U1, U2>)
            if (overflow)
                throw std::overflow_error{"overflow in *="};
        return a = std::move(c);
    }

    // covered
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

    template<unsigned_integral U1,
             unsigned_integral U2>
    U1&
    operator /=(U1& a,
                const U2& b)
        noexcept(noexcept(a = div(a, b).first))
    {
        return a = div(a, b).first;
    }

    template<unsigned_integral U,
             std::integral I>
    U&
    operator /=(U& a,
                I b)
        noexcept(noexcept(a = div(a, b).first))
    {
        return a = div(a, b).first;
    }


    // a %= b

    template<unsigned_integral U1,
             unsigned_integral U2>
    U1&
    operator %=(U1& a,
                const U2& b)
        noexcept(noexcept(a = div(a, b).second))
    {
        return a = div(a, b).second;
    }

    template<unsigned_integral U,
             std::integral I>
    U&
    operator %=(U& a,
                I b)
        noexcept(noexcept(a = div(a, b).second))
    {
        return a = div(a, b).second;
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
        noexcept(noexcept(std::common_type_t<U1, U2>{}) && !any_are_safe_v<U1, U2>)
    {
        std::common_type_t<U1, U2> result;
        bool overflow = eval_add(result.limbs(),
                                 a.limbs(),
                                 b.limbs());
        if constexpr (any_are_safe_v<U1, U2>)
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
        noexcept(noexcept(std::common_type_t<U1, U2>{}) && !any_are_safe_v<U1, U2>)
    {
        std::common_type_t<U1, U2> result;
        bool underflow = eval_sub(result.limbs(),
                                  a.limbs(),
                                  b.limbs());
        if constexpr (any_are_safe_v<U1, U2>)
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
        noexcept(noexcept(std::common_type_t<U1, U2>{}) && !any_are_safe_v<U1, U2>)
    {
        std::common_type_t<U1, U2> result;
        bool overflow = eval_mul_simple(result.limbs(),
                                        a.limbs(),
                                        b.limbs());
        if constexpr (any_are_safe_v<U1, U2>)
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

    template<unsigned_integral U1,
             unsigned_integral U2>
    U1
    operator /(const U1& a,
               const U2& b)
        noexcept(noexcept(div(a, b).first))
    {
        return div(a, b).first;
    }

    template<unsigned_integral U,
             std::integral I>
    U
    operator /(const U& a,
               I b)
        noexcept(noexcept(div(a, b).first))
    {
        return div(a, b).first;
    }


    // a % b
    template<unsigned_integral U1,
             unsigned_integral U2>
    U1
    operator %(const U1& a,
               const U2& b)
        noexcept(noexcept(div(a, b).second))
    {
        return div(a, b).second;
    }


    // a % b
    template<unsigned_integral U,
             std::integral I>
    I
    operator %(const U& a,
               I b)
        noexcept(noexcept(div(a, b).second))
    {
        return div(a, b).second;
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
