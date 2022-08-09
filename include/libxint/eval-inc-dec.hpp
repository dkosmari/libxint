#ifndef XINT_EVAL_INC_DEC_HPP
#define XINT_EVAL_INC_DEC_HPP

#include <cassert>
#include <concepts>
#include <ranges>

#include "types.hpp"
#include "utils.hpp"


namespace xint {


    bool
    eval_increment(limb_range auto&& a,
                   limb_range auto&& old)
        noexcept
    {
        using std::size;

        assert(size(a) == size(old));
        bool carry = true;
        for (std::size_t i = 0; i < size(a); ++i) {
            old[i] = a[i];
            a[i] += carry;
            carry = carry && a[i] == 0;
        }
        return carry;
    }


    bool
    eval_increment(limb_range auto&& a)
        noexcept
    {
        for (auto& v : a)
            if (++v)
                return false;
        return true;
    }


    bool
    eval_decrement(limb_range auto&& a,
                   limb_range auto& old)
        noexcept
    {
        using std::size;

        assert(size(a) == size(old));
        bool borrow = true;
        for (std::size_t i = 0; i < size(a); ++i) {
            old[i] = a[i];
            a[i] -= borrow;
            borrow = borrow && old[i] == 0;
        }
        return borrow;
    }


    bool
    eval_decrement(limb_range auto&& a)
        noexcept
    {
        for (auto& v : a)
            if (v--)
                return false;
        return true;
    }

}

#endif
