#ifndef XINT_EVAL_MULTIPLICATION_HPP
#define XINT_EVAL_MULTIPLICATION_HPP

#include <algorithm>
#include <cassert>
#include <limits>
#include <ranges>

#include "eval-addition.hpp"
#include "utils.hpp"


namespace xint {


    // a *= b (single limb)
    template<std::ranges::contiguous_range A>
    bool
    eval_mul_inplace_limb(A&& a,
                          std::ranges::range_value_t<A> b)
        noexcept
    {
        using limb_t = std::ranges::range_value_t<A>;
        using wide_limb_t = utils::wider_uint_t<limb_t>;
        constexpr auto limb_width = std::numeric_limits<limb_t>::digits;

        if (b == 0) {
            std::ranges::fill(a, 0);
            return false;
        }
        if (utils::is_zero(a))
            return false;

        wide_limb_t sum = 0;
        for (auto& ai : a) {
            sum += ai * wide_limb_type{b};
            ai = static_cast<limb_t>(sum);
            sum >>= limb_width;
        }
        return sum;
    }


    // out = a * b (single limb)
    template<std::ranges::contiguous_range Out,
             std::ranges::contiguous_range A>
    requires utils::same_element_type<Out, A>
    bool
    eval_mul_limb(Out& out,
                  const A& a,
                  std::ranges::range_value_t<Out> b) noexcept
    {
        using std::size;

        using limb_t = std::ranges::range_value_t<Out>;
        using wide_limb_t = utils::wider_uint_t<limb_t>;
        constexpr auto limb_width = std::numeric_limits<limb_t>::digits;

        assert(&a[0] != &out[0]);

        if (b == 0) {
            std::ranges::fill(out, 0);
            return false;
        }
        if (utils::is_zero(a)) {
            std::ranges::fill(out, 0);
            return false;
        }

        wide_limb_t sum = 0;
        for (std::size_t i = 0; i < out.size(); ++i) {
            if (i < a.size())
                sum += a[i] * wide_limb_t{b};
            out[i] = static_cast<limb_t>(sum);
            sum >>= limb_width;
        }
        if (sum)
            return true;

        // if a is longer than out, any non-zero unused limb will imply an overflow
        if (size(a) > size(out)
                && utils::is_nonzero(a | std::views::drop(size(out))))
            return true;

        return false;
    }


    // out = a * b
    template<std::ranges::contiguous_range Out,
             std::ranges::contiguous_range A,
             std::ranges::contiguous_range B>
    requires utils::same_element_type<Out, A, B>
    bool
    eval_mul_simple(Out& out,
                    const A& a,
                    const B& b) noexcept
    {
        using std::views::drop;
        using std::size;

        using limb_t = std::ranges::range_value_t<Out>;
        using wide_limb_t = utils::wider_uint_t<limb_t>;
        constexpr auto limb_width = std::numeric_limits<limb_t>::digits;

        assert(&a[0] != &out[0]);
        assert(&b[0] != &out[0]);

        std::ranges::fill(out, 0);

        if (utils::is_zero(a) || utils::is_zero(b))
            return false;

        bool overflow = false;
        for (std::size_t i = 0; i < size(out); ++i) {
            for (std::size_t j = 0; j <= i; ++j) {
                wide_limb_t x = j < size(a) ? a[j] : 0;
                wide_limb_t y = i - j < size(b) ? b[i - j] : 0;
                wide_limb_t prod = x * y;
                limb_t prod_lo = static_cast<limb_t>(prod);
                limb_t prod_hi = static_cast<limb_t>(prod >> limb_width);
                if (eval_add_inplace_limb(out | drop(i), prod_lo))
                    overflow = true;
                if (eval_add_inplace_limb(out | drop(i + 1), prod_hi))
                    overflow = true;
            }
        }
        if (overflow)
            return true;

        // if either `a` or `b` are longer than `out`, check if the unused limbs are non-zero
        if (size(a) > size(out)
            && utils::is_nonzero(a | drop(size(out))))
            return true;
        if (size(b) > size(out)
            && utils::is_nonzero(b | drop(size(out))))
            return true;

        return false;
    }


}


#endif
