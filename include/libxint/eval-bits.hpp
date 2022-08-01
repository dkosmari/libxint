#ifndef XINT_EVAL_BITS_HPP
#define XINT_EVAL_BITS_HPP

#include <algorithm>
#include <bit>
#include <cassert>
#include <concepts>
#include <functional>
#include <limits>
#include <numeric>
#include <ranges>
#include <type_traits>
#include <utility>

#include "utils.hpp"


namespace xint {


    template<std::ranges::contiguous_range A>
    [[nodiscard]]
    constexpr
    bool
    eval_bit_get(const A& a,
                 unsigned idx)
        noexcept
    {
        using std::size;
        using limb_t = std::ranges::range_value_t<A>;
        constexpr const auto limb_width = std::numeric_limits<limb_t>::digits;
        const auto limb_idx = idx / limb_width;
        const auto bit_idx = idx % limb_width;
        assert(limb_idx < size(a));
        assert(bit_idx < limb_width);
        return a[limb_idx] & (limb_t(1) << bit_idx);
    }


    template<std::ranges::contiguous_range A>
    constexpr
    void
    eval_bit_set(A&& a,
                 unsigned idx,
                 bool v)
        noexcept
    {
        using std::size;
        using limb_t = std::ranges::range_value_t<A>;
        constexpr const auto limb_width = std::numeric_limits<limb_t>::digits;
        const auto total_bits = size(a) * limb_width;
        const unsigned limb_idx = idx / limb_width;
        const unsigned bit_idx = idx % limb_width;
        assert(idx < total_bits);
        assert(limb_idx < size(a));
        assert(bit_idx < limb_width);
        const limb_t mask = limb_t(1) << bit_idx;
        if (v)
            a[limb_idx] |= mask;
        else
            a[limb_idx] &= ~mask;
    }


    template<std::ranges::contiguous_range Out,
             std::ranges::contiguous_range A,
             std::ranges::contiguous_range B,
             typename Op>
    requires utils::same_element_type<Out, A, B>
    void
    eval_bit_op(Out&& out,
                const A& a,
                const B& b,
                const Op& op)
        noexcept
    {
        using std::size;
        using limb_t = std::ranges::range_value_t<Out>;
        for (std::size_t i = 0; i < size(out); ++i) {
            limb_t ai = i < size(a) ? a[i] : 0;
            limb_t bi = i < size(b) ? b[i] : 0;
            out[i] = op(ai, bi);
        }
    }


    template<std::ranges::contiguous_range Out,
             std::ranges::contiguous_range A,
             std::ranges::contiguous_range B>
    void
    eval_bit_and(Out&& out,
                 const A& a,
                 const B& b)
        noexcept
    {
        eval_bit_op(std::forward<Out>(out),
                    std::forward<A>(a),
                    std::forward<B>(b),
                    std::bit_and{});
    }


    template<std::ranges::contiguous_range Out,
             std::ranges::contiguous_range A,
             std::ranges::contiguous_range B>
    void
    eval_bit_or(Out&& out,
                const A& a,
                const B& b)
        noexcept
    {
        eval_bit_op(std::forward<Out>(out),
                    std::forward<A>(a),
                    std::forward<B>(b),
                    std::bit_or{});

    }


    template<std::ranges::contiguous_range Out,
             std::ranges::contiguous_range A,
             std::ranges::contiguous_range B>
    void
    eval_bit_xor(Out&& out,
                 const A& a,
                 const B& b)
        noexcept
    {
        eval_bit_op(std::forward<Out>(out),
                    std::forward<A>(a),
                    std::forward<B>(b),
                    std::bit_xor{});

    }


    // TODO: check if overflow logic is correct
    template<bool Check = false,
             std::ranges::contiguous_range Out,
             std::ranges::contiguous_range A>
    requires utils::same_element_type<Out, A>
    bool
    eval_bit_shift_left(Out&& out,
                        const A& a,
                        unsigned b)
        noexcept
    {
        using std::size;
        using limb_t = std::ranges::range_value_t<Out>;
        using wide_limb_t = utils::wider_uint_t<limb_t>;
        constexpr auto limb_width = std::numeric_limits<limb_t>::digits;
        const auto out_bits = limb_width * size(out);

        if (b >= out_bits) {
            bool overflow = false;
            // note: must check before writing to out
            if constexpr (Check)
                overflow = utils::is_nonzero(a);
            std::ranges::fill(out, 0);
            return overflow;
        }

        const auto limb_offset = b / limb_width;
        assert(limb_offset < size(out));
        const auto bit_offset  = b % limb_width;

        wide_limb_t aj = size(out) - limb_offset < size(a) + 1
                       ? a[size(out) - limb_offset - 1] : 0;

        bool overflow = aj >> (limb_width - bit_offset);
        // note: must check before writing to out
        if constexpr (Check)
            if (!overflow)
                for (std::size_t i = size(out) - limb_offset; i < size(a); ++i)
                    if (a[i]) {
                        overflow = true;
                        break;
                    }

        aj <<= limb_width;
        for (std::size_t i = size(out) - 1; i + 1 > 0; --i) {
            if (i >= limb_offset + 1 && i < size(a) + limb_offset + 1) {
                wide_limb_t ajj = a[i - limb_offset - 1];
                aj |= ajj;
            }
            out[i] = static_cast<limb_t>(aj >> (limb_width - bit_offset));
            aj <<= limb_width;
        }

        if constexpr (!Check)
            return false;
        return overflow;
    }


    template<bool Check = false,
             std::ranges::contiguous_range Out,
             std::ranges::contiguous_range A>
    requires utils::same_element_type<Out, A>
    bool
    eval_bit_shift_right(Out&& out,
                         const A& a,
                         unsigned b)
        noexcept
    {
        using std::size;
        using limb_t = std::ranges::range_value_t<Out>;
        using wide_limb_t = utils::wider_uint_t<limb_t>;
        constexpr auto limb_width = std::numeric_limits<limb_t>::digits;
        const auto a_bits = limb_width * size(a);

        if (b >= a_bits) {
            bool overflow = false;
            // note: must check before writing to out
            if constexpr (Check)
                overflow = utils::is_nonzero(a);
            std::ranges::fill(out, 0);
            return overflow;
        }

        const auto limb_offset = b / limb_width;
        const auto bit_offset  = b % limb_width;

        wide_limb_t aj = limb_offset < size(a) ? a[limb_offset] : 0;

        bool overflow = aj & ((wide_limb_t{1} << bit_offset) - 1);
        // note: must check before writing to out
        if constexpr (Check) {
            if (!overflow)
                for (std::size_t i = 0; i < limb_offset && i < size(a); ++i)
                    if (a[i]) {
                        overflow = true;
                        break;
                    }
            if (!overflow)
                for (std::size_t i = limb_offset + size(out); i < size(a); ++i)
                    if (a[i]) {
                        overflow = true;
                        break;
                    }
        }

        for (std::size_t i = 0; i < size(out); ++i) {
            if (i + limb_offset + 1 < size(a)) {
                wide_limb_t ajj = a[i + limb_offset + 1];
                aj |= ajj << limb_width;
            }
            out[i] = static_cast<limb_t>(aj >> bit_offset);
            aj >>= limb_width;
        }

        if constexpr (!Check)
            return false;
        return overflow;

    }


    template<std::ranges::contiguous_range Out,
             std::ranges::contiguous_range A>
    requires utils::same_element_type<Out, A>
    void
    eval_bit_flip(Out&& out,
                  const A& a)
        noexcept
    {
        using std::size;
        using limb_t = std::ranges::range_value_t<Out>;
        for (std::size_t i = 0; i < size(out); ++i) {
            limb_t v = i < size(a) ? a[i] : 0;
            out[i] = ~v;
        }
    }


    template<std::ranges::contiguous_range A>
    void
    eval_bit_flip_inplace(A&& a)
        noexcept
    {
        for (auto& v : a)
            v = ~v;
    }


    template<std::ranges::contiguous_range A>
    unsigned
    eval_bit_countl_zero(const A& a)
        noexcept
    {
        unsigned zeros = 0;
        for (auto v : a | std::views::reverse) {
            zeros += std::countl_zero(v);
            if (v)
                break;
        }
        return zeros;
    }


    template<std::ranges::contiguous_range A>
    unsigned
    eval_bit_countl_one(const A& a)
        noexcept
    {
        unsigned ones = 0;
        for (auto v : a | std::views::reverse) {
            ones += std::countl_one(v);
            if (~v)
                break;
        }
        return ones;
    }


    template<std::ranges::contiguous_range A>
    unsigned
    eval_bit_countr_zero(const A& a)
        noexcept
    {
        unsigned zeros = 0;
        for (auto v : a) {
            zeros += std::countr_zero(v);
            if (v)
                break;
        }
        return zeros;
    }


    template<std::ranges::contiguous_range A>
    unsigned
    eval_bit_countr_one(const A& a)
        noexcept
    {
        unsigned ones = 0;
        for (auto v : a) {
            ones += std::countr_one(v);
            if (~v)
                break;
        }
        return ones;
    }


    template<std::ranges::contiguous_range A>
    constexpr
    unsigned
    eval_bit_popcount(const A& a)
        noexcept
    {
        using std::begin;
        using std::end;
        using limb_t = std::ranges::range_value_t<A>;
        return std::transform_reduce(begin(a), end(a),
                                     0u,
                                     std::plus<>{},
                                     std::popcount<limb_t>);
    }


}


#endif
