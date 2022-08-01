#ifndef XINT_EVAL_ASSIGNMENT_HPP
#define XINT_EVAL_ASSIGNMENT_HPP

#include <algorithm>
#include <array>
#include <ranges>

#include "utils.hpp"


namespace xint {


    template<std::ranges::contiguous_range A,
             std::ranges::contiguous_range B>
    requires utils::same_element_type<A, B>
    bool
    eval_assign(A&& a,
                const B& b)
        noexcept
    {
        using std::views::drop;
        using std::views::take;
        using std::size;

        if (size(a) >= size(b)) {
            std::ranges::copy(b, a);
            std::ranges::fill(a | drop(size(b)),
                              0u);
            return false;
        } else {
            std::ranges::copy(b | take(size(a)),
                              a);
            return utils::is_nonzero(b | drop(size(a)));
        }
    }


}

#endif
