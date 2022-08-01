#ifndef XINT_EVAL_IO_HPP
#define XINT_EVAL_IO_HPP

#include <iomanip>
#include <ios>
#include <istream>
#include <locale>
#include <ostream>

#include "uint.hpp"


namespace xint {


    template<unsigned Bits, bool Safe>
    void
    eval_input_hex(std::istream& in,
                   uint<Bits, Safe>& n)
    {
        using std::toupper;
        using std::isxdigit;
        const auto eof = std::istream::traits_type::eof();
        char c;
        if (!(in >> c))
            return;
        n = 0;
        const auto loc = in.getloc();
        if (c == '0') { // might be a prefix
            auto next = in.peek();
            if (next == eof)
                return;
            if (toupper(next, loc) == 'X') { // we got '0x'
                next = in.get(); // extract the 'x'
                auto next2 = in.peek();
                if (next2 == eof || !isxdigit(next2, loc)) {
                    // nothing to read after '0x'
                    in.unget(); // put the 'x' back
                    return; // extracted only the '0', n was initialized to zero.
                } // otherwise keep going; '0x' was extracted
            } else
                // not 0x
                in.unget(); // put the '0' back
        } else // c was not '0'
            in.unget(); // put c back

        // at this point, if there was a prefix, it's been extracted
        // we will consume all hex digits until they stop
        auto getxdigit = [&in, &loc, eof](char& c) -> bool
        {
            auto p = in.peek();
            if (p == eof)
                return false;
            if (!isxdigit(p, loc))
                return false;
            in.get(c);
            return true;
        };

        bool overflow = false;
        bool consumed = false;
        while (getxdigit(c)) {
            consumed = true;
            limb_type value = std::stoul(std::string(1, c), nullptr, 16);
            if (eval_bit_shit_left<true>(n.limbs(), n.limbs(), 4))
                overflow = true;
            n |= value;
        }
        if (!consumed || overflow)
            in.setstate(std::ios_base::failbit);
    }


    template<unsigned Bits, bool Safe>
    void
    eval_input(std::istream& in,
               uint<Bits, Safe>& n)
    {
        auto base = in.flags() & std::ios_base::basefield;
        switch (base) {
            case std::ios_base::hex:
                eval_input_hex(in, n);
                break;
            case std::ios_base::oct:
                {
                    // TODO
                    std::string arg;
                    in >> arg;
                    n = uint<Bits, Safe>(arg, 8);
                }
                break;
            case std::ios_base::dec:
                {
                    // TODO
                    std::string arg;
                    in >> arg;
                    n = uint<Bits, Safe>(arg, 10);
                }
                break;
            default:
                {
                    // TODO
                    std::string arg;
                    in >> arg;
                    n = uint<Bits, Safe>(arg);
                }
        }
    }


    template<unsigned Bits, bool Safe>
    void
    eval_output(std::ostream& out,
                const uint<Bits, Safe>& n)
    {
        auto base = out.flags() & std::ios_base::basefield;
        switch (base) {
            case std::ios_base::hex:
                out << n.to_hex(out.flags() & std::ios_base::uppercase);
                break;
            case std::ios_base::oct:
                out << n.to_oct();
                break;
            case std::ios_base::dec:
            default:
                out << n.to_dec();
        }
    }


}


#endif
