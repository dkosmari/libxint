#ifndef XINT_TRAITS_HPP
#define XINT_TRAITS_HPP

#include <concepts>
#include <limits>
#include <type_traits>

#include "uint.hpp"


namespace xint {


    template<typename T> struct make_uint;

    template<std::integral I>
    struct make_uint<I> {
        using type = decltype(uint(I{}));
    };

    template<typename T>
    using make_uint_t = make_uint<T>::type;



    template<typename T>
    struct is_unsigned_integral :
        std::false_type
    {};

    template<unsigned Bits, bool Safe>
    struct is_unsigned_integral<uint<Bits, Safe>> :
        std::true_type
    {};

    template<typename T>
    inline constexpr bool is_unsigned_integral_v =
        is_unsigned_integral<std::remove_cvref_t<T>>::value;



    template<typename T> struct is_safe;

    template<unsigned Bits, bool Safe>
    struct is_safe<uint<Bits, Safe>> :
        std::bool_constant<Safe>
    {};

    template<typename T>
    inline constexpr bool is_safe_v = is_safe<T>::value;


    template<typename... T>
    struct any_are_safe :
        std::bool_constant<(... || is_safe_v<T>)>
    {};

    template<typename... T>
    inline constexpr bool any_are_safe_v = any_are_safe<T...>::value;



    template<typename T> struct is_local;

    template<unsigned Bits, bool Safe>
    struct is_local<uint<Bits, Safe>> :
        std::bool_constant<uint<Bits, Safe>::is_local>
    {};

    template<typename T>
    inline constexpr bool is_local_v = is_local<T>::value;



    // concepts

    template<typename T>
    concept unsigned_integral = is_unsigned_integral_v<T>;

    template<typename T>
    concept safe_unsigned_integral = is_unsigned_integral_v<T> && is_safe_v<T>;





    // TODO: in_range()

    // TODO: hash?


}



namespace std {

    template<xint::unsigned_integral U1,
             xint::unsigned_integral U2>
    struct common_type<U1, U2> {
        using type = xint::uint<max(U1::num_bits, U2::num_bits),
                                xint::any_are_safe_v<U1, U2>>;
    };


    template<xint::unsigned_integral U,
             integral I>
    struct common_type<U, I> {
        using type = common_type_t<U, xint::make_uint_t<I>>;
    };


    template<integral I,
             xint::unsigned_integral U>
    struct common_type<I, U> {
        using type = common_type_t<xint::make_uint_t<I>, U>;
    };


}


#endif
