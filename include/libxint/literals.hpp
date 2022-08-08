#ifndef XINT_LITERALS_HPP
#define XINT_LITERALS_HPP

#include <cmath> // floor, log2
#include <string>
#include <utility> // integer_sequence

#include "uint.hpp"


namespace xint {

    namespace detail {

        template<unsigned Base, unsigned D>
        struct num_bits;

        template<unsigned Base, unsigned D>
        constexpr inline unsigned num_bits_v = num_bits<Base, D>::value;


        template<unsigned D>
        struct num_bits<2, D> {
            static constexpr inline unsigned value = D;
        };


        template<unsigned D>
        struct num_bits<8, D> {
            static constexpr inline unsigned value = 3 * D;
        };


        template<unsigned D>
        struct num_bits<10, D> {
            static constexpr inline unsigned value = 1u + unsigned(std::floor(std::log2(10) * D));
        };


        template<unsigned D>
        struct num_bits<16, D> {
            static constexpr inline unsigned value = 4 * D;
        };



        // used to round the bits up to a multiple of limb_bits

        template<unsigned N>
        struct round_to_limb {
            static constexpr inline unsigned value =
                ((N - 1) / limb_bits + 1) * limb_bits;
        };

        template<unsigned N>
        constexpr inline unsigned round_to_limb_v = round_to_limb<N>::value;



        template<char... Cs>
        struct parser {
            static constexpr inline unsigned base = 10;
            using data = std::integer_sequence<char, Cs...>;
        };

        template<char... Cs>
        struct parser<'0', 'b', Cs...> {
            static constexpr inline unsigned base = 2;
            using data = std::integer_sequence<char, Cs...>;
        };

        template<char... Cs>
        struct parser<'0', 'o', Cs...> {
            static constexpr inline unsigned base = 8;
            using data = std::integer_sequence<char, Cs...>;
        };

        template<char... Cs>
        struct parser<'0', 'x', Cs...> {
            static constexpr inline unsigned base = 16;
            using data = std::integer_sequence<char, Cs...>;
        };

        template<char... Cs>
        struct parser<'0', Cs...> {
            static constexpr inline unsigned base = 8;
            using data = std::integer_sequence<char, Cs...>;
        };


        template<char... Cs>
        std::string
        extract(std::integer_sequence<char, Cs...>)
        {
            return std::string{Cs...};
        }

    } // detail


    namespace literals {

        template<char... Cs>
        auto
        operator ""_uint()
        {
            using Parser = detail::parser<Cs...>;
            using Data = Parser::data;
            constexpr unsigned Base = Parser::base;
            constexpr unsigned bits = detail::num_bits_v<Base, Data::size()>;
            using U = uint<detail::round_to_limb_v<bits>, false>;
            return U{detail::extract(Data{}), Base};
        }

    }

}


#endif
