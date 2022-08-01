#ifndef XINT_UINT_SERIALIZATION_HPP
#define XINT_UINT_SERIALIZATION_HPP

#include <algorithm>
#include <cctype>
#include <ranges>
#include <stdexcept>
#include <string>

#include "eval-bits.hpp"
#include "eval-division.hpp"
#include "uint.hpp"


namespace xint {


    template<unsigned Bits, bool Safe>
    std::string
    uint<Bits, Safe>::to_bin()
        const
    {
        std::string result;
        result.reserve(Bits);
        for (unsigned i = Bits; i > 0; --i) {
            bool v = eval_bit_get(limbs(), i - 1);
            // don't start with a zero
            if (result.empty() && !v)
                continue;
            result += v ? '1' : '0';
        }
        if (result.empty())
            return "0";
        return result;
    }


    template<unsigned Bits, bool Safe>
    std::string
    uint<Bits, Safe>::to_dec()
        const
    {
        if (utils::is_zero(limbs()))
            return "0";

        static const char digits[10 + 1] = "0123456789";
        std::string result;
        uint n = *this;
        limb_type d;
        while (n) {
            std::tie(n, d) = eval_div_limb(n, 10u);
            result += digits[d];
        }
        std::ranges::reverse(result);
        return result;
    }


    template<unsigned Bits, bool Safe>
    std::string
    uint<Bits, Safe>::to_hex(bool upper)
        const
    {
        using std::size;
        static const unsigned char digits[16 + 1] = "0123456789abcdef";
        std::string result;
        result.reserve(Bits / 4);
        for (unsigned i = size(limbs()) - 1; i + 1 > 0; --i) {
            for (unsigned j = limb_bits / 4 - 1; j + 1 > 0; --j) {
                auto nibble = limb(i) >> j * 4 & 0xf;
                // don't start with a zero
                if (result.empty() && nibble == 0)
                    continue;
                auto d = digits[nibble];
                result += upper ? std::toupper(d) : d;
            }
        }
        if (result.empty())
            return "0";
        return result;
    }


    template<unsigned Bits, bool Safe>
    std::string
    uint<Bits, Safe>::to_oct()
        const
    {
        static const char digits[8 + 1] = "01234567";
        std::string result;

        unsigned v = 0;
        for (unsigned i = Bits-1; i + 1 > 0; --i) {
            v = v << 1 | eval_bit_get(limbs(), i);
            if (i % 3 == 0) {
                // don't start with a zero
                if (!result.empty() || v != 0)
                    result += digits[v];
                v = 0;
            }
        }
        if (result.empty())
            return "0";
        return result;
    }


    template<unsigned Bits, bool Safe>
    std::string
    uint<Bits, Safe>::to_string(unsigned base,
                                bool upper)
        const
    {
        if (base < 2)
            throw std::invalid_argument{"base must be >= 2"};
        if (base > 36)
            throw std::invalid_argument{"base must be <= 36"};

        if (utils::is_zero(limbs()))
            return "0";

        static const unsigned char digits[36 + 1] =
            "0123456789"
            "abcdefghij"
            "klmnopqrst"
            "uvwxyz";

        std::string result;
        uint n = *this;
        limb_type d;
        while (n) {
            std::tie(n, d) = eval_div_limb(n, base);
            auto c = digits[d];
            result += upper ? char(std::toupper(c)) : c;
        }
        std::ranges::reverse(result);
        return result;
    }


    template<unsigned Bits, bool Safe>
    template<std::ranges::output_range<unsigned char> R>
    std::ranges::iterator_t<R>
    uint<Bits, Safe>::to_little_endian(R&& buffer)
        const
    {
        auto out = begin(buffer);
        for (limb_type x : limbs())
            for (unsigned i = 0; i < sizeof(limb_type); ++i) {
                if (out == end(buffer))
                    throw std::out_of_range{"buffer is not large enough"};
                *out++ = x >> (i * 8);
            }
        return out;
    }


    template<unsigned Bits, bool Safe>
    template<std::ranges::output_range<unsigned char> R>
    std::ranges::iterator_t<R>
    uint<Bits, Safe>::to_big_endian(R&& buffer)
        const
    {
        auto out = begin(buffer);
        for (limb_type x : limbs() | std::views::reverse)
            for (unsigned i = 0; i < sizeof(limb_type); ++i) {
                if (out == end(buffer))
                    throw std::out_of_range{"buffer is not large enough"};
                *out++ = x >> ((sizeof(limb_type) - i - 1) * 8);
            }
        return out;
    }


}


#endif
