#ifndef XINT_EVAL_ADDITION_HPP
#define XINT_EVAL_ADDITION_HPP

#include <algorithm>
#include <limits>
#include <ranges>

#include "utils.hpp"


namespace xint {


    /** out = a + b
     * TESTED
     * @return true if there's overflow
     */
    template<std::ranges::contiguous_range Out,
             std::ranges::contiguous_range A,
             std::ranges::contiguous_range B>
    requires utils::same_element_type<Out, A, B>
    bool
    eval_add(Out&& out,
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
        constexpr auto limb_width = std::numeric_limits<limb_t>::digits;

        wide_limb_t sum = 0;
        for (std::size_t i = 0; i < size(out); ++i) {
            if (i < size(a))
                sum += a[i];
            if (i >= shift && i < size(b) + shift)
                sum += b[i - shift];
            out[i] = static_cast<limb_t>(sum);
            sum >>= limb_width;
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
    template<std::ranges::contiguous_range A,
             std::ranges::contiguous_range B>
    requires utils::same_element_type<A, B>
    bool
    eval_add_inplace(A&& a,
                     const B& b) noexcept
    {
        using std::size;
        using std::empty;
        using utils::is_nonzero;

        using limb_t = std::ranges::range_value_t<A>;
        using wide_limb_t = utils::wider_uint_t<limb_t>;
        constexpr auto limb_width = std::numeric_limits<limb_t>::digits;

        if (empty(a))
            return is_nonzero(b);

        wide_limb_t sum = 0;
        for (std::size_t i = 0; i < size(a); ++i) {
            sum += a[i];
            if (i < size(b))
                sum += b[i];
            a[i] = static_cast<limb_t>(sum);
            sum >>= limb_width;
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
    template<std::ranges::contiguous_range A>
    bool
    eval_add_inplace_limb(A&& a,
                          std::ranges::range_value_t<A> b) noexcept
    {
        using std::empty;

        using limb_t = std::ranges::range_value_t<A>;
        using wide_limb_t = utils::wider_uint_t<limb_t>;
        constexpr auto limb_width = std::numeric_limits<limb_t>::digits;

        if (empty(a))
            return b;

        wide_limb_t sum = b;
        for (limb_t& ai : a) {
            sum += ai;
            ai = static_cast<limb_t>(sum);
            sum >>= limb_width;
            if (!sum)
                break; // early termination when there's no carry
        }
        if (sum)
            return true;
        return false;
    }


}


#endif
