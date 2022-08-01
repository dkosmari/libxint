#ifndef XINT_UINT_CONSTRUCTORS_HPP
#define XINT_UINT_CONSTRUCTORS_HPP

#include <algorithm>
#include <cassert>
#include <limits>
#include <ranges>
#include <stdexcept>
#include <string>

#include "eval-assignment.hpp"
#include "eval-multiplication.hpp"
#include "uint.hpp"


namespace xint {


    template<unsigned Bits, bool Safe>
    template<unsigned Bits2>
    requires (Bits != Bits2)
    uint<Bits, Safe>::uint(const uint<Bits2, Safe>& other)
        noexcept(!Safe && is_local)
    {
        bool overload = eval_assign(limbs(), other.limbs());
        if constexpr (Safe)
            if (overload)
                throw std::overflow_error{"assignment overflow"};
    }



    template<unsigned Bits, bool Safe>
    template<std::integral I>
    uint<Bits, Safe>::uint(I sval)
        noexcept(!Safe && is_local)
    {
        using std::begin;
        using U = std::make_unsigned_t<I>;

        constexpr auto U_bits = std::numeric_limits<U>::digits;
        U uval = sval;
        auto out = begin(limbs());
        while (uval) {
            if (out == end(limbs())) {
                if constexpr (Safe)
                    throw std::out_of_range{"initializer is too large"};
                else
                    break;
            }
            *out++ = uval;
            if (limb_bits < U_bits)
                uval >>= limb_bits;
            else // guaranteed to have consumed the whole input
                break;
        }
        std::fill(out, end(limbs()), 0);
    }


    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>::uint(const std::string& arg,
                           unsigned base)
    {
        assert(base <= std::numeric_limits<limb_type>::max());

        if (arg.empty())
            throw std::invalid_argument{"argument string is empty"};

        std::ranges::fill(limbs(), 0);

        if (arg == "0")
            return;

        auto [dbase, skip] = utils::detect_base(arg);

        if (base && dbase && base != dbase)
            throw std::invalid_argument{"explicit base (" + std::to_string(base)
                    + ") conflicts with prefix base (" + std::to_string(dbase) + ")"};

        if (!base)
            base = dbase;
        if (!base)
            base = 10;

        uint multiplier = 1u;
        bool too_large = false;
        for (char c : arg | std::views::drop(skip) | std::views::reverse) {
            uint x;

            // allow separators
            // if (c == '\'')
            //     continue;

            if (too_large && c != '0')
                throw std::overflow_error{"too_large && c != '0'"};

            limb_type d = static_cast<limb_type>(std::stoul(std::string{c}, nullptr, base));
            if (eval_mul_limb(x.limbs(),
                              multiplier.limbs(),
                              d))
                throw std::overflow_error{"overflow in eval_mul_limb()"};

            if (eval_add_inplace(limbs(), x.limbs()))
                throw std::overflow_error{"overflow in eval_add_inplace()"};

            if (eval_mul_inplace_limb(multiplier.limbs(), base))
                too_large = true;
        }
    }


}


#endif
