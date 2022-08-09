#ifndef XINT_EVAL_SUBTRACTION_HPP
#define XINT_EVAL_SUBTRACTION_HPP

#include <algorithm>
#include <array>
#include <limits>
#include <ranges>
#include <type_traits>

#include "types.hpp"
#include "utils.hpp"


namespace xint {

    // out = a - b
    // returns true if there's underflow
    bool
    eval_sub(limb_range auto&& out,
             const limb_range auto& a,
             const limb_range auto& b,
             unsigned shift = 0)
        noexcept
    {
        using utils::is_nonzero;
        using std::views::drop;
        using std::size;

        signed_wide_limb_type diff = 0;
        const auto common_size = std::max({size(out),
                                           size(a),
                                           size(b) + shift});
        for (std::size_t i = 0; i < common_size; ++i) {
            if (i < size(a))
                diff += a[i];
            if (i >= shift && i < size(b) + shift)
                diff -= b[i - shift];
            if (i < size(out))
                out[i] = static_cast<limb_type>(diff);
            diff >>= limb_bits;
        }
        return diff;
    }


    // a -= b
    // returns true if there's underflow
    bool
    eval_sub_inplace(limb_range auto&& a,
                     const limb_range auto& b)
        noexcept
    {
        using utils::is_nonzero;
        using std::empty;
        using std::size;

        if (empty(a))
            return is_nonzero(b);

        signed_wide_limb_type diff = 0;
        for (std::size_t i = 0; i < size(a); ++i) {
            diff += a[i];
            if (i < size(b))
                diff -= b[i];
            a[i] = static_cast<limb_type>(diff);
            diff >>= limb_bits;
        }
        if (diff)
            return true;

        // if b is longer than a
        if (size(b) > size(a))
            return is_nonzero(b | std::views::drop(size(a)));

        return false;
    }


    // a -= b (single limb)
    // returns true if there's underflow
    bool
    eval_sub_inplace_limb(limb_range auto&& a,
                          limb_type b)
        noexcept
    {
        using std::empty;

        if (empty(a))
            return b;

        signed_wide_limb_type diff = -signed_wide_limb_type{b};
        for (limb_type& ai : a) {
            diff += ai;
            ai = static_cast<limb_type>(diff);
            diff >>= limb_bits;
            if (!diff)
                break; // early termination when there's no borrow
        }

        if (diff)
            return true;

        return false;
    }

}


#endif
