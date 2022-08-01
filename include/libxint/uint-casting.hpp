#ifndef XINT_UINT_CASTING_HPP
#define XINT_UINT_CASTING_HPP

#include <stdexcept>
#include <string>

#include "uint.hpp"


namespace xint {


    template<unsigned Bits, bool Safe>
    constexpr
    uint<Bits, true>&
    uint<Bits, Safe>::safe()
        noexcept
    {
        return reinterpret_cast<uint<Bits, true>&>(*this);
    }


    template<unsigned Bits, bool Safe>
    constexpr
    const uint<Bits, true>&
    uint<Bits, Safe>::safe()
        const noexcept
    {
        return reinterpret_cast<const uint<Bits, true>&>(*this);
    }


    template<unsigned Bits, bool Safe>
    constexpr
    uint<Bits, false>&
    uint<Bits, Safe>::unsafe()
        noexcept
    {
        return reinterpret_cast<uint<Bits, false>&>(*this);
    }


    template<unsigned Bits, bool Safe>
    constexpr
    const uint<Bits, false>&
    uint<Bits, Safe>::unsafe()
        const noexcept
    {
        return reinterpret_cast<const uint<Bits, false>&>(*this);
    }


    template<unsigned Bits, bool Safe>
    template<std::unsigned_integral U>
    uint<Bits, Safe>::operator U()
        const noexcept(!Safe)
    {
        return to_uint<std::numeric_limits<U>::digits>();
    }


    template<unsigned Bits, bool Safe>
    template<unsigned DestBits>
    utils::uint_t<DestBits>
    uint<Bits, Safe>::to_uint()
        const noexcept(!Safe || DestBits >= Bits)
    {
        using result_t = utils::uint_t<DestBits>;

        if constexpr (Safe)
            if (bit_width(*this) > DestBits)
                throw std::out_of_range{"value "
                        + to_dec()
                        + " is too large for uint"
                        + std::to_string(DestBits)
                        + "_t"};

        if constexpr (limb_bits >= DestBits)
            return static_cast<result_t>(limbs.front());
        else {
            result_t result = 0;
            for (unsigned i = 0; i < limbs.size() && i * limb_bits < DestBits; ++i)
                result |= result_t{limbs[i]} << (i * limb_bits);
            return result;
        }
    }


    template<unsigned Bits, bool Safe>
    template<unsigned NewBits>
    uint<NewBits, Safe>
    uint<Bits, Safe>::cast() const
        noexcept(!Safe || NewBits >= Bits)
    {
        uint<NewBits, Safe> dst;
        constexpr const std::size_t dst_size = dst.limbs.size();

        if constexpr (NewBits >= Bits) {
            // widening
            auto [last_in, last_out] = std::ranges::copy(limbs,
                                                         dst.limbs);
            std::fill(last_out, dst.limbs.end(), 0);
        } else {
            // narrowing
            if constexpr (Safe)
                if (utils::is_nonzero(limbs | std::views::drop(dst_size)))
                    throw std::overflow_error{"value too large for destination"};
            std::ranges::copy(limbs | std::views::take(dst_size),
                              dst.limbs);
        }
        return dst;
    }


    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>::operator bool()
        const noexcept
    {
        return utils::is_nonzero(limbs);
    }

}


#endif
