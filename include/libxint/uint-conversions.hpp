#ifndef XINT_UINT_CASTING_HPP
#define XINT_UINT_CASTING_HPP

#include <stdexcept>
#include <string>
#include <utility> // forward()

#include "traits.hpp"
#include "uint.hpp"


namespace xint {


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
    uint<Bits, Safe>::operator bool()
        const noexcept
    {
        return utils::is_nonzero(limbs());
    }


    template<unsigned_integral U1,
             unsigned_integral U2>
    requires(U1::num_bits == U2::num_bits)
    constexpr
    U1&
    safety_cast(U2&& a)
        noexcept
    {
        return reinterpret_cast<U1&>(a);
    }


    template<bool Safe,
             unsigned_integral U>
    constexpr
    const uint<U::num_bits, Safe>&
    safety_cast(const U& a)
        noexcept
    {
        return reinterpret_cast<const uint<U::num_bits, Safe>&>(a);
    }


    template<bool Safe,
             unsigned_integral U>
    constexpr
    uint<U::num_bits, Safe>&
    safety_cast(U& a)
        noexcept
    {
        return reinterpret_cast<uint<U::num_bits, Safe>&>(a);
    }


}


#endif
