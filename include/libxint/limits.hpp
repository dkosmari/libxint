#ifndef XINT_LIMITS_HPP
#define XINT_LIMITS_HPP

#include <cmath> // log10()
#include <limits>

#include "traits.hpp"
#include "uint.hpp"


namespace std {

    template<unsigned Bits, bool Safe>
    struct numeric_limits<xint::uint<Bits, Safe>> {
        static constexpr bool is_specialized = true;
        static constexpr bool is_signed = false;
        static constexpr bool is_integer = true;
        static constexpr bool is_exact = true;
        static constexpr bool has_infinity = false;
        static constexpr bool has_quiet_NaN = false;
        static constexpr bool has_signaling_NaN = false;
        static constexpr float_denorm_style has_denorm = denorm_absent;
        static constexpr bool has_denorm_loss = false;
        static constexpr float_round_style round_style = round_toward_zero;
        static constexpr bool is_iec559 = false;
        static constexpr bool is_bounded = true;
        static constexpr bool is_modulo = true;
        static constexpr int digits = xint::uint<Bits, Safe>::num_limbs; // 2
        static constexpr int digits10 = Bits * log10(2);
        static constexpr int max_digits10 = 0;
        static constexpr auto radix = xint::wide_limb_type{1} << xint::limb_bits;
        static constexpr int min_exponent = 0;
        static constexpr int min_exponent10 = 0;
        static constexpr int max_exponent = 0;
        static constexpr int max_exponent10 = 0;
        static constexpr bool traps = true;
        static constexpr bool tinyness_before = false;

        static constexpr xint::uint<Bits, Safe> denorm_min() noexcept { return 0u; }
        static constexpr xint::uint<Bits, Safe> epsilon() noexcept { return 0u; }
        static constexpr xint::uint<Bits, Safe> infinity() noexcept { return 0u; }
        static constexpr xint::uint<Bits, Safe> lowest() noexcept { return min(); }
        static constexpr xint::uint<Bits, Safe> max() noexcept { return ~min(); }
        static constexpr xint::uint<Bits, Safe> min() noexcept { return 0u; }
        static constexpr xint::uint<Bits, Safe> quiet_NaN() noexcept { return 0u; }
        static constexpr xint::uint<Bits, Safe> round_error() noexcept { return 0u; }
        static constexpr xint::uint<Bits, Safe> signaling_NaN() noexcept { return 0u; }
    };


}


#endif
