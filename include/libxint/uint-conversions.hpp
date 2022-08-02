#ifndef XINT_UINT_CASTING_HPP
#define XINT_UINT_CASTING_HPP

#include <stdexcept>
#include <string>

#include "uint.hpp"


namespace xint {


    template<unsigned Bits, bool Safe>
    template<bool OtherSafe>
    constexpr
    uint<Bits, OtherSafe>&
    uint<Bits, Safe>::safe()
        noexcept
    {
        return reinterpret_cast<uint<Bits, OtherSafe>&>(*this);
    }


    template<unsigned Bits, bool Safe>
    template<bool OtherSafe>
    constexpr
    const uint<Bits, OtherSafe>&
    uint<Bits, Safe>::safe()
        const noexcept
    {
        return reinterpret_cast<const uint<Bits, OtherSafe>&>(*this);
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
    template<bool OtherSafe>
    constexpr
    const uint<Bits, OtherSafe>&
    uint<Bits, Safe>::compat(const uint<Bits, OtherSafe>&)
        const noexcept
    {
        return reinterpret_cast<const uint<Bits, OtherSafe>&>(*this);
    }


    template<unsigned Bits, bool Safe>
    template<bool OtherSafe>
    constexpr
    uint<Bits, OtherSafe>&
    uint<Bits, Safe>::compat(const uint<Bits, OtherSafe>&)
        noexcept
    {
        return reinterpret_cast<uint<Bits, OtherSafe>&>(*this);
    }



    template<unsigned Bits, bool Safe>
    template<unsigned DestBits>
    utils::uint_t<DestBits>
    uint<Bits, Safe>::to_uint()
        const noexcept(!Safe || DestBits >= Bits)
    {
        using std::size;
        using result_t = utils::uint_t<DestBits>;

        if constexpr (Safe)
            if (bit_width(*this) > DestBits)
                throw std::out_of_range{"value "
                        + to_dec()
                        + " is too large for uint"
                        + std::to_string(DestBits)
                        + "_t"};

        if constexpr (limb_bits >= DestBits)
            return static_cast<result_t>(limb(0));
        else {
            result_t result = 0;
            for (unsigned i = 0; i < size(limbs()) && i * limb_bits < DestBits; ++i)
                result |= result_t{limb(i)} << (i * limb_bits);
            return result;
        }
    }


    template<unsigned Bits, bool Safe>
    template<std::unsigned_integral U>
    uint<Bits, Safe>::operator U()
        const noexcept(noexcept(to_uint<std::numeric_limits<U>::digits>()))
    {
        return to_uint<std::numeric_limits<U>::digits>();
    }


    template<unsigned Bits, bool Safe>
    template<unsigned NewBits>
    uint<NewBits, Safe>
    uint<Bits, Safe>::cast()
        const
        noexcept(noexcept(uint<NewBits, Safe>(*this)))
    {
        return uint<NewBits, Safe>(*this);
    }


    template<unsigned Bits, bool Safe>
    uint<Bits, Safe>::operator bool()
        const noexcept
    {
        return utils::is_nonzero(limbs());
    }

}


#endif
