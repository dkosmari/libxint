#ifndef XINT_UINT_OPERATORS_HPP
#define XINT_UINT_OPERATORS_HPP

#include <algorithm>
#include <compare>
#include <concepts>
#include <limits>
#include <ostream>
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
#include "uint.hpp"


// TODO: allow mixed arithmetic with native integers


namespace xint {


    /* ---------- */
    /* Assignment */
    /* ---------- */



    // a = b

    template<unsigned Bits, bool Safe>
    template<unsigned Bits2>
    requires (Bits != Bits2)
    uint<Bits, Safe>&
    uint<Bits, Safe>::operator =(const uint<Bits2, Safe>& other)
        noexcept(!Safe)
    {
        bool overflow = eval_assign(limbs(), other.limbs());
        if constexpr (Safe)
            if (overflow)
                throw std::overflow_error{"overflow in ="};
        return *this;
    }



    // a += b

    template<unsigned Bits1, bool Safe1,
             unsigned Bits2, bool Safe2>
    uint<Bits1, Safe1>&
    operator +=(uint<Bits1, Safe1>& a,
                const uint<Bits2, Safe2>& b)
        noexcept(!Safe1)
    {
        bool overflow = eval_add_inplace(a.limbs(), b.limbs());
        if constexpr (Safe1)
            if (overflow)
                throw std::overflow_error{"overflow in +="};
        return a;
    }


    template<unsigned Bits, bool Safe,
             std::integral I>
    uint<Bits, Safe>&
    operator +=(uint<Bits, Safe>& a,
                I b)
        noexcept(!Safe)
    {
        return a += uint(b);
    }



    // a -= b

    template<unsigned Bits1, bool Safe1,
             unsigned Bits2, bool Safe2>
    uint<Bits1, Safe1>&
    operator -=(uint<Bits1, Safe1>& a,
                const uint<Bits2, Safe2>& b)
        noexcept(!Safe1)
    {
        bool overflow = eval_sub_inplace(a.limbs(), b.limbs());
        if constexpr (Safe1)
            if (overflow)
                throw std::overflow_error{"overflow in -="};
        return a;
    }


    template<unsigned Bits, bool Safe,
             std::integral I>
    uint<Bits, Safe>&
    operator -=(uint<Bits, Safe>& a,
                I b)
        noexcept(!Safe)
    {
        return a -= uint(b);
    }



    // a *= b

    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>&
    operator *=(uint<Bits, Safe>& a,
                const uint<Bits, Safe>& b)
        noexcept(!Safe)
    {
        uint<Bits, Safe> c;
        bool overflow = eval_mul_simple(c.limbs(), a.limbs(), b.limbs());
        if constexpr (Safe)
            if (overflow)
                throw std::overflow_error{"overflow in *="};
        a = std::move(c);
        return a;
    }


    template<unsigned Bits, bool Safe,
             std::integral I>
    uint<Bits, Safe>&
    operator *=(uint<Bits, Safe>& a,
                I b)
        noexcept(!Safe)
    {
        return a *= uint(b);
    }



    // a /= b

    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>&
    operator /=(uint<Bits, Safe>& a,
                const uint<Bits, Safe>& b)
    {
        a = eval_div(a, b).first;
        return a;
    }


    // a %= b

    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>&
    operator %=(uint<Bits, Safe>& a,
                const uint<Bits, Safe>& b)
    {
        a = eval_div(a, b).second;
        return a;
    }


    // a &= b

    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>&
    operator &=(uint<Bits, Safe>& a,
                const uint<Bits, Safe>& b)
        noexcept
    {
        eval_bit_and(a.limbs(), a.limbs(), b.limbs());
        return a;
    }




    // a |= b
    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>&
    operator |=(uint<Bits, Safe>& a,
                const uint<Bits, Safe>& b)
        noexcept
    {
        eval_bit_or(a.limbs(), a.limbs(), b.limbs());
        return a;
    }


    // a ^= b
    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>&
    operator ^=(uint<Bits, Safe>& a,
                const uint<Bits, Safe>& b)
        noexcept
    {
        eval_bit_xor(a.limbs(), a.limbs(), b.limbs());
        return a;
    }


    // a <<= b
    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>&
    operator <<=(uint<Bits, Safe>& a,
                 unsigned b)
        noexcept (!Safe)
    {
        bool overflow = eval_bit_shift_left<Safe>(a.limbs(), a.limbs(), b);
        if constexpr (Safe)
            if (overflow)
                throw std::overflow_error{"overflow in <<="};
        return a;
    }


    // a >>= b
    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>&
    operator >>=(uint<Bits, Safe>& a,
                 unsigned b)
        noexcept(!Safe)
    {
        bool overflow = eval_bit_shift_right<Safe>(a.limbs(), a.limbs(), b);
        if constexpr (Safe)
            if (overflow)
                throw std::overflow_error{"overflow in >>="};
        return a;
    }



    /* ------------------- */
    /* Increment/Decrement */
    /* ------------------- */


    // ++ a
    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>&
    operator ++(uint<Bits, Safe>& a)
        noexcept(!Safe)
    {
        bool overflow = eval_increment(a.limbs());
        if constexpr (Safe)
            if (overflow)
                throw std::overflow_error{"overflow in ++"};
        return a;
    }


    // -- a
    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>&
    operator --(uint<Bits, Safe>& a)
        noexcept(!Safe)
    {
        bool overflow = eval_decrement(a.limbs());
        if constexpr (Safe)
            if (overflow)
                throw std::overflow_error{"overflow in --"};
        return a;
    }


    // a ++
    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>
    operator ++(uint<Bits, Safe>& a, int)
        noexcept(!Safe)
    {
        uint<Bits, Safe> b;
        bool overflow = eval_increment(a.limbs(), b.limbs());
        if constexpr (Safe)
            if (overflow)
                throw std::overflow_error{"overflow in ++"};
        return b;
    }


    // a --
    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>
    operator --(uint<Bits, Safe>& a, int)
        noexcept(!Safe)
    {
        uint<Bits, Safe> b;
        bool overflow = eval_decrement(a.limbs(), b.limbs());
        if constexpr (Safe)
            if (overflow)
                throw std::overflow_error{"overflow in --"};
        return b;
    }



    /* ---------- */
    /* Arithmetic */
    /* ---------- */


    // + a
    template<unsigned Bits, bool Safe>
    const uint<Bits, Safe>&
    operator +(const uint<Bits, Safe>& a)
        noexcept
    {
        return a;
    }


    // - a
    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>
    operator -(uint<Bits, Safe> a)
        noexcept
    {
        eval_bit_flip_inplace(a.limbs());
        eval_increment(a.limbs());
        return a;
    }


    // a + b
    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>
    operator +(const uint<Bits, Safe>& a,
               const uint<Bits, Safe>& b)
        noexcept(!Safe)
    {
        uint<Bits, Safe> result;
        bool overflow = eval_add(result.limbs(),
                                 a.limbs(),
                                 b.limbs());
        if constexpr (Safe)
            if (overflow)
                throw std::overflow_error{"overflow in +"};
        return result;
    }


    // a - b
    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>
    operator -(const uint<Bits, Safe>& a,
               const uint<Bits, Safe>& b)
        noexcept(!Safe)
    {
        uint<Bits, Safe> result;
        bool underflow = eval_sub(result.limbs(),
                                  a.limbs(),
                                  b.limbs());
        if constexpr (Safe)
            if (underflow)
                throw std::overflow_error{"overflow in -"};
        return result;
    }


    // a * b
    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>
    operator *(const uint<Bits, Safe>& a,
               const uint<Bits, Safe>& b)
        noexcept(!Safe)
    {
        uint<Bits, Safe> result;
        bool overflow = eval_mul_simple(result.limbs(),
                                        a.limbs(),
                                        b.limbs());
        if constexpr (Safe)
            if (overflow)
                throw std::overflow_error{"overflow in *"};
        return result;
    }


    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>
    operator *(const uint<Bits, Safe>& a,
               limb_type b)
        noexcept(!Safe)
    {
        uint<Bits, Safe> result;
        bool overflow = eval_mul_limb(result.limbs(),
                                      a.limbs(),
                                      b);
        if constexpr (Safe)
            if (overflow)
                throw std::overflow_error{"overflow in *"};
        return result;
    }


    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>
    operator *(limb_type a,
               const uint<Bits, Safe>& b)
        noexcept(!Safe)
    {
        uint<Bits, Safe> result;
        bool overflow = eval_mul_limb(result.limbs(),
                                      b.limbs(),
                                      a);
        if constexpr (Safe)
            if (overflow)
                throw std::overflow_error{"overflow in *"};
        return result;
    }


    // a / b
    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>
    operator /(const uint<Bits, Safe>& a,
               const uint<Bits, Safe>& b)
    {
        return eval_div(a, b).first;
    }


    // a % b
    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>
    operator %(const uint<Bits, Safe>& a,
               const uint<Bits, Safe>& b)
    {
        return eval_div(a, b).second;
    }


    // a % b
    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>
    operator %(const uint<Bits, Safe>& a,
               limb_type b)
    {
        return eval_mod(a, b);
    }


    // ~ a
    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>
    operator ~(uint<Bits, Safe> a)
        noexcept
    {
        a.flip();
        return a;
    }


    // a & b
    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>
    operator &(const uint<Bits, Safe>& a,
               const uint<Bits, Safe>& b)
        noexcept
    {
        uint<Bits, Safe> c;
        eval_bit_and(c.limbs(), a.limbs(), b.limbs());
    }


    // a | b
    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>
    operator |(const uint<Bits, Safe>& a,
               const uint<Bits, Safe>& b)
        noexcept
    {
        uint<Bits, Safe> c;
        eval_bit_or(c.limbs(), a.limbs(), b.limbs());
    }


    // a ^ b
    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>
    operator ^(const uint<Bits, Safe>& a,
               const uint<Bits, Safe>& b)
        noexcept
    {
        uint<Bits, Safe> c;
        eval_bit_xor(c.limbs(), a.limbs(), b.limbs());
    }


    // a << b
    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>
    operator <<(const uint<Bits, Safe>& a,
                unsigned b)
        noexcept(!Safe)
    {
        uint<Bits, Safe> c;
        bool overflow = eval_bit_shift_left<Safe>(c.limbs(), a.limbs(), b);
        if constexpr (Safe)
            if (overflow)
                throw std::overflow_error{"overflow in <<"};
        return c;
    }


    // a >> b
    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>
    operator >>(const uint<Bits, Safe>& a,
                unsigned b)
        noexcept(!Safe)
    {
        uint<Bits, Safe> c;
        bool overflow = eval_bit_shift_right<Safe>(c.limbs(), a.limbs(), b);
        if constexpr (Safe)
            if (overflow)
                throw std::overflow_error{"overflow in >>"};
        return c;
    }



    /* ---------- */
    /* Comparison */
    /* ---------- */


    template<unsigned Bits, bool Safe1, bool Safe2>
    bool
    operator ==(const uint<Bits, Safe1>& a,
                const uint<Bits, Safe2>& b)
        noexcept
    {
        return a.limbs() == b.limbs();
    }


    template<unsigned Bits1, bool Safe1,
             unsigned Bits2, bool Safe2>
    requires (Bits1 != Bits2)
    bool
    operator ==(const uint<Bits1, Safe1>& a,
                const uint<Bits2, Safe2>& b)
        noexcept
    {
        return eval_compare_equal(a.limbs(), b.limbs());
    }


    template<unsigned Bits, bool Safe,
             std::integral I>
    bool
    operator ==(const uint<Bits, Safe>& a,
                I b)
        noexcept
    {
        return a == uint(b);
    }


    template<unsigned Bits, bool Safe,
             std::integral I>
    bool
    operator ==(I a,
                const uint<Bits, Safe>& b)
        noexcept
    {
        return uint(a) == b;
    }


    template<unsigned Bits1, bool Safe1,
             unsigned Bits2, bool Safe2>
    std::strong_ordering
    operator <=>(const uint<Bits1, Safe1>& a,
                 const uint<Bits2, Safe2>& b)
        noexcept
    {
        return eval_compare_three_way(a.limbs(), b.limbs());
    }


    template<unsigned Bits, bool Safe,
             std::integral I>
    std::strong_ordering
    operator <=>(const uint<Bits, Safe>& a,
                 I b)
        noexcept
    {
        return a <=> uint(b);
    }


    template<unsigned Bits, bool Safe,
             std::integral I>
    std::strong_ordering
    operator <=>(I a,
                 const uint<Bits, Safe>& b)
        noexcept
    {
        return uint(a) <=> b;
    }



    // TODO: allow comparison between different sized integers




    /* --- */
    /* I/O */
    /* --- */



    template<unsigned Bits, bool Safe>
    std::istream&
    operator >>(std::istream& in,
                uint<Bits, Safe>& n)
    {
        std::ostream::sentry s{in};
        if (s)
            eval_input(in, n);
        return in;
    }


    template<unsigned Bits, bool Safe>
    std::ostream&
    operator <<(std::ostream& out,
                const uint<Bits, Safe>& n)
    {
        std::ostream::sentry s{out};
        if (s)
            eval_output(out, n);
        return out;
    }
}


#endif
