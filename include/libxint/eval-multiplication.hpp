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
    template<limb_range A>
    bool
    eval_mul_inplace_limb(A&& a,
                          limb_type b)
        noexcept
    {
        if (b == 0) {
            std::ranges::fill(a, 0);
            return false;
        }
        if (utils::is_zero(a))
            return false;

        wide_limb_type sum = 0;
        for (auto& ai : a) {
            sum += ai * wide_limb_type{b};
            ai = static_cast<limb_type>(sum);
            sum >>= limb_bits;
        }
        return sum;
    }


    // out = a * b (single limb)
    template<limb_range Out,
             limb_range A>
    bool
    eval_mul_limb(Out& out,
                  const A& a,
                  limb_type b) noexcept
    {
        using std::size;

        assert(&a[0] != &out[0]);

        if (b == 0) {
            std::ranges::fill(out, 0);
            return false;
        }
        if (utils::is_zero(a)) {
            std::ranges::fill(out, 0);
            return false;
        }

        wide_limb_type sum = 0;
        for (std::size_t i = 0; i < size(out); ++i) {
            if (i < size(a))
                sum += a[i] * wide_limb_type{b};
            out[i] = static_cast<limb_type>(sum);
            sum >>= limb_bits;
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
    template<limb_range Out,
             limb_range A,
             limb_range B>
    bool
    eval_mul_simple(Out& out,
                    const A& a,
                    const B& b) noexcept
    {
        using std::views::drop;
        using std::size;

        assert(&a[0] != &out[0]);
        assert(&b[0] != &out[0]);

        std::ranges::fill(out, 0);

        if (utils::is_zero(a) || utils::is_zero(b))
            return false;

        bool overflow = false;
        for (std::size_t i = 0; i < size(out); ++i) {
            for (std::size_t j = 0; j <= i; ++j) {
                wide_limb_type x = j < size(a) ? a[j] : 0;
                wide_limb_type y = i - j < size(b) ? b[i - j] : 0;
                wide_limb_type prod = x * y;
                limb_type prod_lo = static_cast<limb_type>(prod);
                limb_type prod_hi = static_cast<limb_type>(prod >> limb_bits);
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
