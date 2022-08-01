#ifndef XINT_EVAL_DIVISION_HPP
#define XINT_EVAL_DIVISION_HPP

#include <cassert>
#include <cstdlib>
#include <ranges>
#include <stdexcept>
#include <utility> // pair

#include "eval-comparison.hpp"
#include "eval-multiplication.hpp"
#include "eval-subtraction.hpp"
#include "uint-operators.hpp"
#include "uint.hpp"
#include "utils.hpp"
#include "stdlib.hpp"


namespace xint {


    // returns { a / b, a % b }
    template<unsigned Bits, bool Safe>
    std::pair<uint<Bits, Safe>, uint<Bits, Safe>>
    eval_div(uint<Bits, Safe> a,
             const uint<Bits, Safe>& b)
    {
        if (!b)
            throw std::domain_error{"division by zero"};
        if (!a)
            return {0, 0};

        uint<Bits, Safe> q = 0;

        const unsigned b_zeros = countl_zero(b);

        while (a >= b) {
            unsigned shift = b_zeros - countl_zero(a);
            uint<Bits, Safe> sb = b << shift;

            if (eval_sub_inplace(a.limbs(), sb.limbs())) {
                --shift;
                sb = b << shift;
                eval_add_inplace(a.limbs(), sb.limbs());
            }
            /*
             * This is a more efficient version of
             *     q += uint<Bits>(1) << shift;
             */
            const auto limb_shift = shift / limb_bits;
            const auto bit_shift = shift % limb_bits;
            const limb_type x = 1u << bit_shift;
            bool o = eval_add_inplace_limb(q.limbs() | std::views::drop(limb_shift),
                                           x);
            assert(!o);
        }

        return {q, a};
    }


    // returns { a / b, a % b }
    template<unsigned Bits, bool Safe>
    std::pair<uint<Bits, Safe>, limb_type>
    eval_div_limb(uint<Bits, Safe> a,
                  limb_type b)
    {
        using std::views::drop;
        using std::size;

        if (!b)
            throw std::domain_error{"division by zero"};
        if (!a)
            return {0, 0};

        uint<Bits, Safe> q = 0;
        bool a_neg = false;

        unsigned shift = size(a.limbs());

        while (shift-- > 0) {
            auto cmp = eval_compare_three_way_limb(a.limbs(), b, shift);
            if (cmp < 0)
                continue;

            wide_limb_type top_a = a.limb(shift);
            if (shift + 1 < size(a.limbs()))
                top_a |= wide_limb_type{a.limb(shift + 1)} << limb_bits;

            wide_limb_type factor = top_a / b;
            assert((factor >> limb_bits) == 0);

            uint<2*limb_bits, Safe> d = static_cast<wide_limb_type>(factor * b);
            bool d_neg = a_neg;
            if (eval_sub_inplace(a.limbs() | drop(shift),
                                 d.limbs())) {
                a_neg = !a_neg;
                a = -a;
            }

            if (d_neg)
                eval_sub_inplace_limb(q.limbs() | drop(shift), factor);
            else
                eval_add_inplace_limb(q.limbs() | drop(shift), factor);
        }

        if (a_neg) {
            a = -a;
            eval_add_inplace_limb(a.limbs(), b);
            --q;
        }

        return {q, a.limb(0)};
    }



}


#endif
