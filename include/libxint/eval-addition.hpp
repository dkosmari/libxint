#ifndef XINT_EVAL_ADDITION_HPP
#define XINT_EVAL_ADDITION_HPP

#include <algorithm>
#include <limits>
#include <ranges>

#include "types.hpp"
#include "utils.hpp"


namespace xint {


    /** out = a + b
     * TESTED
     * @return true if there's overflow
     */
    bool
    eval_add(limb_range auto&& out,
             const limb_range auto& a,
             const limb_range auto& b,
             unsigned shift = 0)
        noexcept
    {
        using utils::is_nonzero;
        using std::views::drop;
        using std::size;

        wide_limb_type sum = 0;
        for (std::size_t i = 0; i < size(out); ++i) {
            if (i < size(a))
                sum += a[i];
            if (i >= shift && i < size(b) + shift)
                sum += b[i - shift];
            out[i] = static_cast<limb_type>(sum);
            sum >>= limb_bits;
        }
        if (sum)
            return true;

        // a is longer than out? check if excess limbs are nonzero
        if (size(a) > size(out)
                && is_nonzero(a | drop(size(out))))
            return true;

        // if b was shifted up entirely
        if (shift >= size(out))
            return is_nonzero(b);

        // if b+shift partially extends beyond out
        if (size(b) + shift > size(out)
                && is_nonzero(b | drop(size(out) - shift)))
            return true;

        return false;
    }


    // a += b
    // returns true if there's overflow
    bool
    eval_add_inplace(limb_range auto&& a,
                     const limb_range auto& b) noexcept
    {
        using std::size;
        using std::empty;
        using utils::is_nonzero;

        if (empty(a))
            return is_nonzero(b);

        wide_limb_type sum = 0;
        for (std::size_t i = 0; i < size(a); ++i) {
            sum += a[i];
            if (i < size(b))
                sum += b[i];
            a[i] = static_cast<limb_type>(sum);
            sum >>= limb_bits;
        }
        if (sum)
            return true;

        // if b is longer than a
        if (size(b) > size(a))
            return is_nonzero(b | std::views::drop(size(a)));

        return false;
    }


    // a += b (single limb)
    // returns true if there's overflow
    bool
    eval_add_inplace_limb(limb_range auto&& a,
                          limb_type b) noexcept
    {
        using std::empty;

        if (empty(a))
            return b;

        wide_limb_type sum = b;
        for (auto& ai : a) {
            sum += ai;
            ai = static_cast<limb_type>(sum);
            sum >>= limb_bits;
            if (!sum)
                break; // early termination when there's no carry
        }
        if (sum)
            return true;
        return false;
    }


}


#endif
