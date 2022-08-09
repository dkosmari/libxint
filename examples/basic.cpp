#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>

#include <libxint/uint.hpp>
#include <libxint/literals.hpp>


using namespace std;

using namespace xint::literals;


using x32 = xint::uint<32>;
using x32s = xint::uint<32, true>;

using x256 = xint::uint<256>;
using x256s = xint::uint<256, true>;


std::ostream&
operator <<(std::ostream& out,
            const vector<unsigned char>& vec)
{
    out << "[ " << hex << setfill('0');
    for (unsigned char x : vec)
        out << setw(2) << unsigned(x) << " ";
    return out << dec << "]";
}


template<xint::unsigned_integral U,
         typename... Args>
string
info(const U& u,
     Args... args)
{
    ostringstream out;
    (out << ... << args)
        << u << dec << " (uint<" << u.num_bits << "," << boolalpha << u.is_safe << ">)";
    return out.str();
}



#define SHOW cout << "line " << __LINE__ << ": "


int main()
{
    cout << boolalpha << setfill('0');

    // default constructor leaves it uninitialized
    x256 a;

    // can assign from native integers
    a = 10u;
    SHOW << "a = " << info(a) << endl;

    // can
    a = -1; // negative values always are converted to unsigned before being extended
    SHOW << "a = " << info(a, hex) << endl;

    x256 b = 7; // can construct from integer
    SHOW << "b = " << info(b) << endl;

    // can construct from strings and a base
    x256 c{"110011", 2};
    SHOW << "c = " << info(c) << endl;

    // native integers can be converted using a deduction guide
    auto d = xint::uint(5);
    SHOW << "d = " << info(d) << endl;


    // converting to native integers
    a = 10;
    SHOW << "a = " << a.to_uint<64>() << endl;
    SHOW << "a = " << static_cast<uint64_t>(a) << endl;


    // can be converted to bool in conditionals
    if (a)
        SHOW << "a is non-zero: " << info(a) << endl;
    a = 0;
    if (!a)
        SHOW << "a is zero: " << info(a) << endl;


    // with no overflow checking, has same semantics as native unsigned integers
    a = a - 1;
    SHOW << "a = 0x" << info(a, hex) << endl;


    // conversion to floating point
    {
        a = x256{1} << 10;
        SHOW << "converting " << info(a) << " to floating point" << endl;
        SHOW << "   float: " << float(a) << endl;
        SHOW << "  double: " << double(a) << endl;

        a = x256{1} << 100;
        SHOW << "converting " << info(a) << " to floating point" << endl;
        SHOW << "   float: " << float(a) << endl;
        SHOW << "  double: " << double(a) << endl;

        a = x256{1} << 200;
        SHOW << "converting " << info(a) << " to floating point" << endl;
        SHOW << "   float: " << float(a) << endl;
        SHOW << "  double: " << double(a) << endl;

    }


    // safe arithmetic
    try {
        // conversion into safe version must be explicit
        x256s aa = x256s{a};
        // if any argument is safe, the operation is safe
        aa + a; // this will overflow
    }
    catch (overflow_error& e) {
        SHOW << "safe version throws: " << e.what() << endl;
    }

    try {
        x256s aa = 1;
        // even though the destination is unsafe, it uses a safe argument
        // so overflow will be checked.
        a += aa;
    }
    catch (overflow_error& e) {
        // There's no guarantee the output variable has a correct value
        // when an exception occurs.
        // That is, there are no strong exception guarantees.
        SHOW << "(after exception) a = " << info(a) << endl;
    }

    {
        // safety can be changed with safety_cast<>()
        const x256s safe_a = 1;
        x256 unsafe_a = safety_cast<false>(safe_a);
        SHOW << "unsafe_a = " << info(unsafe_a) << endl;
        // alternative use of safety_cast<>():
        x256 unsafe_aa = safety_cast<const x256>(safe_a);
        SHOW << "unsafe_aa = " << info(unsafe_aa) << endl;

        const x256 unsafe_b = 2;
        x256s safe_b = unsafe_b; // no need to cast when adding safety
        SHOW << "safe_b = " << info(safe_b) << endl;
    }


    // serialization/formatting:
    a = 0xdeadbeef;
    SHOW << "a in binary = " << a.to_bin() << endl;
    SHOW << "a in decimal = " << a.to_dec() << endl;
    SHOW << "a in lower case hex = " << a.to_hex() << endl;
    SHOW << "a in upper case hex = " << a.to_hex(true) << endl;
    SHOW << "a in octal = " << a.to_oct() << endl;
    SHOW << "a in base 36 = " << a.to_string(36) << endl;

    // conversion to binary
    vector<unsigned char> buffer(a.num_bits / 8);

    a.to_little_endian(buffer);
    SHOW << "little endian: " << buffer << endl;

    a.to_big_endian(buffer);
    SHOW << "big endian:    " << buffer << endl;


    x256 e{"0x123456789abcdef0"};
    x32 f = e; // truncation happens
    SHOW << "f = " << info(f, hex) << endl;
    try {
        // can't truncate non-zero bits when it's safe
        [[maybe_unused]] x32s ff = e;
        SHOW << "this line will not be reached: " << endl;
    }
    catch (overflow_error& e) {
        SHOW << "truncation was prevented: " << e.what() << endl;
    }


    // assignments
    a = 0;
    a += b;
    SHOW << "a = " << info(a) << endl;
    a += 3;
    SHOW << "a = " << info(a) << endl;

    a -= b;
    SHOW << "a = " << info(a) << endl;
    a -= 1;
    SHOW << "a = " << info(a) << endl;

    a *= b;
    SHOW << "a = " << info(a) << endl;
    a *= 3;
    SHOW << "a = " << info(a) << endl;

    a /= xint::uint(2);
    SHOW << "a = " << info(a) << endl;
    a /= 7;
    SHOW << "a = " << info(a) << endl;

    a %= 2;
    SHOW << "a = " << info(a) << endl;

    a =  0b0101;
    b = x256{"0b1100"};
    a &= b;
    SHOW << "a = 0b" << setw(4) << a.to_bin() << endl;

    a = 0b0101;
    b = x256{"1100", 2};
    a |= b;
    SHOW << "a = 0b" << setw(4) << a.to_bin() << endl;

    a = 0b0101;
    b = 0b1100_uint;
    a ^= b;
    SHOW << "a = 0b" << setw(4) << a.to_bin() << endl;


}
