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

#include "types.hpp"
#include "utils.hpp"
#include "eval-assignment.hpp"
#include "eval-comparison.hpp"


namespace xint {


    constexpr
    bool
    eval_bit_has_single_bit(const limb_range auto& a)
        noexcept
    {
        bool found = false;
        for (auto x : a) {
            if (std::has_single_bit(x)) {
                if (found)
                    return false;
                else
                    found = true;
            } else if (x) // more than a single 1 on this limb
                return false;
        }
        return found;
    }


    [[nodiscard]]
    constexpr
    bool
    eval_bit_get(const limb_range auto& a,
                 unsigned idx)
        noexcept
    {
        const auto limb_idx = idx / limb_bits;
        const auto bit_idx = idx % limb_bits;
        assert(limb_idx < std::size(a));
        assert(bit_idx < limb_bits);
        return a[limb_idx] & (limb_type(1) << bit_idx);
    }


    constexpr
    void
    eval_bit_set(limb_range auto&& a,
                 unsigned idx,
                 bool v)
        noexcept
    {
        const unsigned limb_idx = idx / limb_bits;
        const unsigned bit_idx = idx % limb_bits;
        assert(limb_idx < std::size(a));
        assert(bit_idx < limb_bits);
        const limb_type mask = limb_type(1) << bit_idx;
        if (v)
            a[limb_idx] |= mask;
        else
            a[limb_idx] &= ~mask;
    }


    void
    eval_bit_op(limb_range auto&& out,
                const limb_range auto& a,
                const limb_range auto& b,
                auto op)
        noexcept
    {
        using std::size;
        for (std::size_t i = 0; i < size(out); ++i) {
            limb_type ai = i < size(a) ? a[i] : 0;
            limb_type bi = i < size(b) ? b[i] : 0;
            out[i] = op(ai, bi);
        }
    }


    void
    eval_bit_and(limb_range auto&& out,
                 const limb_range auto& a,
                 const limb_range auto& b)
        noexcept
    {
        eval_bit_op(out, a, b, std::bit_and{});
    }


    void
    eval_bit_or(limb_range auto&& out,
                const limb_range auto& a,
                const limb_range auto& b)
        noexcept
    {
        eval_bit_op(out, a, b, std::bit_or{});
    }


    void
    eval_bit_xor(limb_range auto&& out,
                 const limb_range auto& a,
                 const limb_range auto& b)
        noexcept
    {
        eval_bit_op(out, a, b, std::bit_xor{});
    }


    // TODO: check if overflow logic is correct
    template<bool Check>
    bool
    eval_bit_shift_left(limb_range auto&& out,
                        const limb_range auto& a,
                        unsigned b)
        noexcept
    {
        using std::size;
        const auto out_bits = limb_bits * size(out);

        if (b >= out_bits) {
            bool overflow = false;
            // note: must check before writing to out
            if constexpr (Check)
                overflow = utils::is_nonzero(a);
            std::ranges::fill(out, 0);
            return overflow;
        }

        const auto limb_offset = b / limb_bits;
        assert(limb_offset < size(out));
        const auto bit_offset  = b % limb_bits;

        wide_limb_type aj = size(out) - limb_offset < size(a) + 1
                          ? a[size(out) - limb_offset - 1] : 0;

        bool overflow = aj >> (limb_bits - bit_offset);
        // note: must check before writing to out
        if constexpr (Check)
            if (!overflow)
                for (std::size_t i = size(out) - limb_offset; i < size(a); ++i)
                    if (a[i]) {
                        overflow = true;
                        break;
                    }

        aj <<= limb_bits;
        for (std::size_t i = size(out) - 1; i + 1 > 0; --i) {
            if (i >= limb_offset + 1 && i < size(a) + limb_offset + 1) {
                wide_limb_type ajj = a[i - limb_offset - 1];
                aj |= ajj;
            }
            out[i] = static_cast<limb_type>(aj >> (limb_bits - bit_offset));
            aj <<= limb_bits;
        }

        if constexpr (!Check)
            return false;
        return overflow;
    }


    template<bool Check>
    bool
    eval_bit_shift_right(limb_range auto&& out,
                         const limb_range auto& a,
                         unsigned b)
        noexcept
    {
        using std::size;
        const auto a_bits = limb_bits * size(a);

        if (b >= a_bits) {
            bool overflow = false;
            // note: must check before writing to out
            if constexpr (Check)
                overflow = utils::is_nonzero(a);
            std::ranges::fill(out, 0);
            return overflow;
        }

        const auto limb_offset = b / limb_bits;
        const auto bit_offset  = b % limb_bits;

        wide_limb_type aj = limb_offset < size(a) ? a[limb_offset] : 0;

        bool overflow = aj & ((wide_limb_type{1} << bit_offset) - 1);
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
                wide_limb_type ajj = a[i + limb_offset + 1];
                aj |= ajj << limb_bits;
            }
            out[i] = static_cast<limb_type>(aj >> bit_offset);
            aj >>= limb_bits;
        }

        if constexpr (!Check)
            return false;
        return overflow;

    }


    constexpr
    void
    eval_bit_flip(limb_range auto&& out,
                  const limb_range auto& a)
        noexcept
    {
        using std::size;
        for (std::size_t i = 0; i < size(out); ++i) {
            limb_type v = i < size(a) ? a[i] : 0;
            out[i] = ~v;
        }
    }


    constexpr
    void
    eval_bit_flip(limb_range auto&& a)
        noexcept
    {
        for (auto& v : a)
            v = ~v;
    }


    unsigned
    eval_bit_countl_zero(const limb_range auto& a)
        noexcept
    {
        unsigned zeros = 0;
        for (auto v : a | std::views::reverse) {
            if (!v)
                zeros += limb_bits;
            else {
                zeros += std::countl_zero(v);
                break;
            }
        }
        return zeros;
    }


    unsigned
    eval_bit_countl_one(const limb_range auto& a)
        noexcept
    {
        unsigned ones = 0;
        for (auto v : a | std::views::reverse) {
            if (!~v)
                ones += limb_bits;
            else {
                ones += std::countl_one(v);
                break;
            }
        }
        return ones;
    }


    unsigned
    eval_bit_countr_zero(const limb_range auto& a)
        noexcept
    {
        unsigned zeros = 0;
        for (auto v : a) {
            if (!v)
                zeros += limb_bits;
            else {
                zeros += std::countr_zero(v);
                break;
            }
        }
        return zeros;
    }


    unsigned
    eval_bit_countr_one(const limb_range auto& a)
        noexcept
    {
        unsigned ones = 0;
        for (auto v : a) {
            if (!~v)
                ones += limb_bits;
            else {
                ones += std::countr_one(v);
                break;
            }
        }
        return ones;
    }


    constexpr
    void
    eval_bit_ceil(limb_range auto&& out,
                  const limb_range auto& a)
        noexcept
    {
        using std::begin;
        using std::size;

        std::ranges::fill(out, 0);
        if (eval_compare_three_way_limb(a, 1) <= 0) {
            *begin(out) = 1;
            return;
        }
        if (eval_bit_has_single_bit(a)) {
            eval_assign(out, a);
            return;
        }
        const auto top_bit = eval_bit_width(a);
        assert(top_bit + 1 < size(a));
        eval_bit_set(out, top_bit + 1, true);
    }


    constexpr
    void
    eval_bit_floor(limb_range auto&& out,
                   const limb_range auto& a)
        noexcept
    {
        std::ranges::fill(out, 0);
        if (utils::is_zero(a))
            return;
        eval_bit_set(out, eval_bit_width(a), true);
    }


    unsigned
    eval_bit_width(const limb_range auto& a)
        noexcept
    {
        using std::size;
        return limb_bits * size(a) - eval_bit_countl_zero(a);
    }


    constexpr
    unsigned
    eval_bit_popcount(const limb_range auto& a)
        noexcept
    {
        using std::begin;
        using std::end;
        return std::transform_reduce(begin(a), end(a),
                                     0u,
                                     std::plus<>{},
                                     std::popcount<limb_type>);
    }


}


#endif
