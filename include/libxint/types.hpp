#ifndef XINT_TYPES_HPP
#define XINT_TYPES_HPP

#include <cstdint>
#include <limits>
#include <ranges>


namespace xint {


#ifndef XINT_LIMB_SIZE
#define XINT_LIMB_SIZE 32
#endif


#if XINT_LIMB_SIZE == 8

    using limb_type             = std::uint8_t;
    using wide_limb_type        = std::uint16_t;
    using signed_wide_limb_type = std::int16_t;

#elif XINT_LIMB_SIZE == 16

    using limb_type             = std::uint16_t;
    using wide_limb_type        = std::uint32_t;
    using signed_wide_limb_type = std::int32_t;

#elif XINT_LIMB_SIZE == 32

    using limb_type             = std::uint32_t;
    using wide_limb_type        = std::uint64_t;
    using signed_wide_limb_type = std::int64_t;
#endif

    static_assert(sizeof(wide_limb_type) > sizeof(limb_type));

    inline constexpr const unsigned limb_bits = std::numeric_limits<limb_type>::digits;


    template<typename T>
    concept limb_range = std::ranges::contiguous_range<T>;


}

#endif
