#ifndef XINT_EVAL_INC_DEC_HPP
#define XINT_EVAL_INC_DEC_HPP

#include <cassert>
#include <concepts>
#include <ranges>

#include "utils.hpp"


namespace xint {


    template<std::ranges::contiguous_range A,
             std::ranges::contiguous_range Old>
    requires utils::same_element_type<A, Old>
    bool
    eval_increment(A&& a,
                   Old&& old)
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


    template<std::ranges::contiguous_range A>
    bool
    eval_increment(A&& a)
        noexcept
    {
        for (auto& v : a)
            if (++v)
                return false;
        return true;
    }


    template<std::ranges::contiguous_range A,
             std::ranges::contiguous_range Old>
    requires utils::same_element_type<A, Old>
    bool
    eval_decrement(A&& a,
                   Old& old)
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


    template<std::ranges::contiguous_range A>
    bool
    eval_decrement(A&& a)
        noexcept
    {
        for (auto& v : a)
            if (v--)
                return false;
        return true;
    }

}

#endif
