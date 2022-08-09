#ifndef XINT_EVAL_ASSIGNMENT_HPP
#define XINT_EVAL_ASSIGNMENT_HPP

#include <algorithm>
#include <array>
#include <ranges>

#include "types.hpp"
#include "utils.hpp"


namespace xint {


    bool
    eval_assign(limb_range auto&& a,
                const limb_range auto& b)
        noexcept
    {
        using namespace std::ranges;
        using views::drop;
        using views::take;
        // using std::size;
        // using std::begin;

        if (size(a) >= size(b)) {
            copy(b, begin(a));
            fill(a | drop(size(b)), 0u);
            return false;
        } else {
            copy_n(begin(b), size(a), begin(a));
            return utils::is_nonzero(b | drop(size(a)));
        }
    }


}

#endif
