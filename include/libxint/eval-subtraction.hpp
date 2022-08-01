#ifndef XINT_EVAL_SUBTRACTION_HPP
#define XINT_EVAL_SUBTRACTION_HPP

#include <algorithm>
#include <array>
#include <limits>
#include <ranges>
#include <type_traits>

#include "utils.hpp"


namespace xint {

    // out = a - b
    // returns true if there's underflow
    template<std::ranges::contiguous_range Out,
             std::ranges::contiguous_range A,
             std::ranges::contiguous_range B>
    requires utils::same_element_type<Out, A, B>
    bool
    eval_sub(Out&& out,
             const A& a,
             const B& b,
             unsigned shift = 0)
        noexcept
    {
        using utils::is_nonzero;
        using std::views::drop;
        using std::size;

        using limb_t = std::ranges::range_value_t<Out>;
        using wide_limb_t = utils::wider_uint_t<limb_t>;
        using signed_wide_limb_t = std::make_signed_t<wide_limb_t>;
        constexpr auto limb_width = std::numeric_limits<limb_t>::digits;

        signed_wide_limb_t diff = 0;
        const auto common_size = std::max({size(out),
                                           size(a),
                                           size(b) + shift});
        for (std::size_t i = 0; i < common_size; ++i) {
            if (i < size(a))
                diff += a[i];
            if (i >= shift && i < size(b) + shift)
                diff -= b[i - shift];
            if (i < size(out))
                out[i] = static_cast<limb_t>(diff);
            diff >>= limb_width;
        }
        return diff;
    }


    // a -= b
    // returns true if there's underflow
    template<std::ranges::contiguous_range A,
             std::ranges::contiguous_range B>
    requires utils::same_element_type<A, B>
    bool
    eval_sub_inplace(A&& a,
                     const B& b)
        noexcept
    {
        using utils::is_nonzero;
        using std::empty;
        using std::size;

        using limb_t = std::ranges::range_value_t<A>;
        using wide_limb_t = utils::wider_uint_t<limb_t>;
        using signed_wide_limb_t = std::make_signed_t<wide_limb_t>;
        constexpr auto limb_width = std::numeric_limits<limb_t>::digits;

        if (empty(a))
            return is_nonzero(b);

        signed_wide_limb_t diff = 0;
        for (std::size_t i = 0; i < size(a); ++i) {
            diff += a[i];
            if (i < size(b))
                diff -= b[i];
            a[i] = static_cast<limb_t>(diff);
            diff >>= limb_width;
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
    template<std::ranges::contiguous_range A>
    bool
    eval_sub_inplace_limb(A&& a,
                          std::ranges::range_value_t<A> b)
        noexcept
    {
        using std::empty;

        using limb_t = std::ranges::range_value_t<A>;
        using wide_limb_t = utils::wider_uint_t<limb_t>;
        using signed_wide_limb_t = std::make_signed_t<wide_limb_t>;
        constexpr auto limb_width = std::numeric_limits<limb_t>::digits;

        if (empty(a))
            return b;

        signed_wide_limb_t diff = -signed_wide_limb_t{b};
        for (limb_t& ai : a) {
            diff += ai;
            ai = static_cast<limb_t>(diff);
            diff >>= limb_width;
            if (!diff)
                break; // early termination when there's no borrow
        }

        if (diff)
            return true;

        return false;
    }
}


#endif
