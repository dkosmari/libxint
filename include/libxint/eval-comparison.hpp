#ifndef XINT_EVAL_COMPARISON_HPP
#define XINT_EVAL_COMPARISON_HPP

#include <algorithm>
#include <compare>
#include <ranges>

#include "types.hpp"
#include "utils.hpp"


namespace xint {


    bool
    eval_compare_equal(const limb_range auto& a,
                       const limb_range auto& b)
        noexcept
    {
        using std::size;

        const auto min_size = std::min(size(a), size(b));
        for (std::size_t i = 0; i < min_size; ++i)
            if (a[i] != b[i])
                return false;

        auto filter = std::views::drop(min_size);
        return utils::is_zero(a | filter) && utils::is_zero(b | filter);
    }


    std::strong_ordering
    eval_compare_three_way(const limb_range auto& a,
                           const limb_range auto& b,
                           unsigned shift = 0)
        noexcept
    {
        using std::empty;
        using std::size;

        if (empty(a) && empty(b))
            return std::strong_ordering::equal;
        for (std::size_t i = std::max(size(a), size(b) + shift) - 1; i + 1 > 0; --i) {
            auto ai = i < size(a)
                      ? a[i] : 0;
            auto bi = i - shift < size(b) && i >= shift
                      ? b[i - shift] : 0;
            auto r = ai <=> bi;
            if (r != 0)
                return r;
        }
        return std::strong_ordering::equal;
    }


    std::strong_ordering
    eval_compare_three_way_limb(const limb_range auto& a,
                                limb_type b,
                                unsigned shift = 0)
        noexcept
    {
        using utils::is_nonzero;
        using std::size;
        using std::strong_ordering;

        if (size(a) < shift + 1) {
            // a is shorter than shifted b
            if (b)
                return strong_ordering::less;
            if (utils::is_nonzero(a))
                return strong_ordering::greater;
            return strong_ordering::equal;
        }

        // if anything above b is nonzero, a is greater
        if (is_nonzero(a | std::views::drop(shift + 1)))
            return strong_ordering::greater;

        // check the matching limbs
        auto r = a[shift] <=> b;
        if (r != 0)
            return r;

        // check the lower limbs of a
        if (is_nonzero(a | std::views::take(shift)))
            return strong_ordering::greater;

        return strong_ordering::equal;
    }



}

#endif
