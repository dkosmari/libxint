/*
 * Main header.
 * License: GPLv3+
 */

#ifndef XINT_UINT_HPP
#define XINT_UINT_HPP


#include <array>
#include <concepts>
#include <cstddef> // std::size_t
#include <limits>
#include <ranges>
#include <string>

#include "types.hpp"
#include "utils.hpp"
#include "storage.hpp"


namespace xint {


    template<unsigned Bits,
             bool Safe = false>
    struct uint {

        static_assert(Bits > 0);
        static_assert(Bits % limb_bits == 0, "total bit width must be a multiple of limb bits");

        static inline constexpr std::size_t num_bits = Bits;
        static inline constexpr std::size_t num_limbs = (Bits-1) / limb_bits + 1;

        using storage_type = auto_storage<std::array<limb_type, num_limbs>>;

        // true when limbs are stored locally, and not in the heap
        static inline constexpr bool is_local = storage_type::is_local;

        // true when operations cannot throw exceptions at all
        static inline constexpr bool is_noexcept = Safe || !is_local;

        // true when all operations are checked against overflow
        static inline constexpr bool is_safe = Safe;


        using array_type = storage_type::data_type;

        storage_type data;

        constexpr const array_type& limbs() const noexcept { return *data; }
        constexpr       array_type& limbs()       noexcept { return *data; }

        constexpr const limb_type& limb(std::size_t idx) const noexcept { return limbs()[idx]; }
        constexpr       limb_type& limb(std::size_t idx)       noexcept { return limbs()[idx]; }


        // constructors

        constexpr uint() noexcept(is_local) = default;
        constexpr uint(const uint&) noexcept(is_local) = default;

        template<unsigned Bits2, bool Safe2>
        explicit(!Safe && Safe2) // must be explicit when lifting up safety
        uint(const uint<Bits2, Safe2>&)
            noexcept(is_local && (Bits >= Bits2 || !(Safe || Safe2)));


        template<std::integral I>
        uint(I sval)
            noexcept(is_local
                     && (!Safe
                         ||
                         std::numeric_limits<std::make_unsigned_t<I>>::digits <= Bits));

        // note: base must be a valid argument for std::stoul()
        explicit uint(const std::string& arg, unsigned base = 0);


        // assignment

        template<unsigned Bits2, bool Safe2>
        uint& operator =(const uint<Bits2, Safe2>& other)
            noexcept((Bits == Bits2) || (!Safe && !Safe2));


        // conversions

        template<unsigned DestBits>
        utils::uint_t<DestBits> to_uint() const noexcept(!Safe || DestBits >= Bits);

        template<std::unsigned_integral U>
        explicit
        operator U() const
            noexcept(noexcept(to_uint<std::numeric_limits<U>::digits>()));


        explicit
        operator bool() const noexcept;


        // serialization

        std::string to_bin() const;
        std::string to_dec() const;
        std::string to_hex(bool upper = false) const;
        std::string to_oct() const;
        std::string to_string(unsigned base, bool upper = false) const;

        template<std::ranges::output_range<unsigned char> R>
        std::ranges::iterator_t<R>
        to_little_endian(R&& buffer) const;

        template<std::ranges::output_range<unsigned char> R>
        std::ranges::iterator_t<R>
        to_big_endian(R&& buffer) const;

    };


    /* Deduction guide for converting integers.
     * An integer will always turn into the smallest uint<> possible, unsafe.
     */
    template<std::integral I>
    uint(I x) ->
        uint<std::max<unsigned>(std::numeric_limits<std::make_unsigned_t<I>>::digits,
                                limb_bits),
             false>;


}

// member functions are implemented here
#include "uint-constructors.hpp"
#include "uint-conversions.hpp"
#include "uint-serialization.hpp"

// the rest is implemented here
#include "limits.hpp"
#include "literals.hpp"
#include "operators.hpp"
#include "random.hpp"
#include "stdlib.hpp"
#include "traits.hpp"


#endif


/*
 *  libxint: extended fixed-width unsigned int implementation.
 *  Copyright (C) 2022  Daniel K. O.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
