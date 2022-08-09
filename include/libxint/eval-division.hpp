#ifndef XINT_EVAL_DIVISION_HPP
#define XINT_EVAL_DIVISION_HPP

#include <cassert>
#include <iterator>
#include <ranges>
#include <utility> // pair

#include "types.hpp"
#include "traits.hpp"
#include "utils.hpp"

#include "eval-addition.hpp"
#include "eval-assignment.hpp"
#include "eval-bits.hpp"
#include "eval-comparison.hpp"
#include "eval-inc-dec.hpp"
#include "eval-subtraction.hpp"


namespace xint {


    enum class div_status {
        success,
        div_by_zero,
        overflow
    };


    /* Notes:
     *     - `q`, `r` are the output.
     *     - `a` and `r` are modified as part of the calculation.
     *     - `q` must be at least as large as `a` to avoid overflow.
     *     - `r` must have one mre limb than `b`.
     */
    div_status
    eval_div(limb_range auto&& q,
             limb_range auto&& r,
             limb_range auto&& a,
             limb_range auto&& b)
        noexcept
    {
        using std::size;
        using std::views::drop;
        using std::views::take;

        const unsigned b_width = eval_bit_width(b);
        if (!b_width) // same as checking if b is zero
            return div_status::div_by_zero;

        assert(std::data(q) != std::data(a));
        assert(std::data(q) != std::data(b));
        assert(std::data(r) != std::data(b));
        assert(size(r) > size(b));

        std::ranges::fill(q, 0);

        if (eval_compare_three_way(a, b) < 0) {
            if (eval_assign(r, a))
                return div_status::overflow;
            else
                return div_status::success;
        }


        // optimization: b is a power of two: just right-shift a
        if (eval_bit_has_single_bit(b)) {
            const unsigned shift = b_width - 1;

            // turn b into a mask by subtracting 1
            eval_sub_inplace_limb(b, 1);
            eval_bit_and(r, a, b);
            eval_bit_shift_right<false>(a, a, shift);
            if (eval_assign(q, a))
                return div_status::overflow;
            return div_status::success;
        }

        // first, line-up top bit of b with top bit of a
        auto bit_offset = [](unsigned s) -> unsigned { return s % limb_bits; };
        auto limb_offset = [](unsigned s) -> unsigned { return s / limb_bits; };

        unsigned a_width = eval_bit_width(a);
        unsigned shift;

        div_status status = div_status::success;

        do {
            shift = a_width - b_width;
            // note: we set `r = b << shift` so we don't modify b
            // r is guaranteed to have one more limb than b, so it never overflows
            eval_bit_shift_left<false>(r, b, bit_offset(shift));

            if (eval_sub_inplace(a | drop(limb_offset(shift)), r)) {
                // we subtracted too much, try to make b smaller
                if (!shift) {
                    // if can't make b smaller, just rollback the subtraction,
                    // and break the loop
                    eval_add_inplace(a | drop(limb_offset(shift)), r);
                    break;
                }
                --shift;
                eval_bit_shift_left<false>(r, b, bit_offset(shift));
                eval_add_inplace(a | drop(limb_offset(shift)), r);
            }
            // we can avoid looking at limbs known to be zero
            a_width = eval_bit_width(a | take(limb_offset(a_width - 1) + 1));

            /*
             * This is a more efficient version of
             *     q += uint<Bits>(1) << shift;
             */
            const limb_type x = static_cast<limb_type>(1) << bit_offset(shift);
            if (eval_add_inplace_limb(q | drop(limb_offset(shift)), x))
                status = div_status::overflow;

        } while (shift && a_width >= b_width);

        if (eval_assign(r, a))
            status = div_status::overflow;

        return status;
    }


    div_status
    eval_div_limb(limb_range auto&& q,
                  limb_type& r,
                  limb_range auto&& a,
                  limb_type b)
        noexcept
    {
        using std::views::drop;
        using std::size;
        using std::empty;
        using std::begin;
        using std::end;
        using std::next;


        if (!b)
            return div_status::div_by_zero;

        assert(std::data(q) != std::data(a));

        std::ranges::fill(q, 0);

        if (eval_compare_three_way_limb(a, b) < 0) {
            r = empty(a) ? 0 : *begin(a);
            return div_status::success;
        }

        bool a_neg = false;

        unsigned shift = size(a);
        div_status status = div_status::success;

        while (shift-- > 0) {
            if (eval_compare_three_way_limb(a, b, shift) < 0)
                continue;

            auto it = next(begin(a), shift);
            wide_limb_type top_a = *it;
            std::advance(it, 1);
            if (it != end(a))
                top_a |= static_cast<wide_limb_type>(*it) << limb_bits;

            wide_limb_type factor = top_a / b;
            assert((factor >> limb_bits) == 0);

            bool d_neg = a_neg;
            wide_limb_type d = factor * b;
            const limb_type dd[2] = { static_cast<limb_type>(d),
                                      static_cast<limb_type>(d >> limb_bits) };
            if (eval_sub_inplace(a | drop(shift), dd)) {
                a_neg = !a_neg;
                // a = -a;
                eval_bit_flip(a);
                eval_increment(a);
            }

            bool overflow;
            if (d_neg)
                overflow = eval_sub_inplace_limb(q | drop(shift), factor);
            else
                overflow = eval_add_inplace_limb(q | drop(shift), factor);
            if (overflow)
                status = div_status::overflow;
        }

        if (a_neg) {
            // a = -a
            eval_bit_flip(a);
            eval_increment(a);
            eval_add_inplace_limb(a, b);
            eval_decrement(q);
        }

        r = *begin(a);

        return status;
    }

}


#endif
