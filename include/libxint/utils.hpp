#ifndef XINT_UTILS_HPP
#define XINT_UTILS_HPP

#include <algorithm>
#include <cstdint>
#include <optional>
#include <ranges>
#include <string>
#include <utility> // pair


namespace xint::utils {


    inline
    std::pair<unsigned, unsigned>
    detect_base(const std::string& arg)
    {
        if (arg.starts_with("0x"))
            return {16, 2};
        else if (arg.starts_with("0b"))
            return {2, 2};
        else if (arg.starts_with("0o"))
            return {8, 2};
        else if (arg.starts_with("0"))
            return {8, 1};
        return {0, 0};
    }


    template<limb_range R>
    bool
    is_zero(const R& r) noexcept
    {
        return std::ranges::all_of(r, [](auto x) -> bool { return !x; });
    }


    template<limb_range R>
    bool
    is_nonzero(const R& r) noexcept
    {
        return std::ranges::any_of(r, [](auto x) -> bool { return x; });
    }


    inline
    std::optional<unsigned>
    char_to_val(char c, unsigned base)
    {
        try {
            return std::stoul(std::string{c}, nullptr, base);
        }
        catch (...) {
            return {};
        }
    }



    template<unsigned> struct uint_t_helper;

    template<> struct uint_t_helper<8>  { using type = std::uint8_t;  };
    template<> struct uint_t_helper<16> { using type = std::uint16_t; };
    template<> struct uint_t_helper<32> { using type = std::uint32_t; };
    template<> struct uint_t_helper<64> { using type = std::uint64_t; };

    template<unsigned Bits> using uint_t = uint_t_helper<Bits>::type;

    template<std::unsigned_integral U>
    using wider_uint_t = uint_t<2 * std::numeric_limits<U>::digits>;


    // template<typename T, typename ... U>
    // concept same_element_type = (std::same_as<std::ranges::range_value_t<T>,
    //                              std::ranges::range_value_t<U>> &&... );




} // namespace xint::utils

#endif
