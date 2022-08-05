#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

#include <libxint/uint.hpp>


using namespace std;

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


template<unsigned N>
void
foo(const xint::uint<N>& a)
{
    cout << "foo(" << a << ")" << endl;
}



int main()
{
    cout << boolalpha;

    // default constructor leaves it uninitialized
    x256 a;

    // can assign from native integers
    a = 10u;
    cout << "1: a = " << a << endl;

    // can
    a = -1; // negative values always are converted to unsigned before being extended
    cout << "2: a = " << hex << a << dec << endl;

    x256 b = 7; // can construct from integer
    cout << "3: b = " << b << endl;

    // can construct from strings and a base
    x256 c{"110011", 2};
    cout << "4: c = " << c << endl;

    // native integers can be converted using a deduction guide
    auto d = xint::uint(5);
    cout << "5: d = " << d << " has parameters <"
         << d.num_bits << ", " << d.is_safe << ">" << endl;


    // converting to native integers
    a = 10;
    cout << "6: a = " << a.to_uint<64>() << endl;
    cout << "7: a = " << static_cast<uint64_t>(a) << endl;


    // can be converted to bool in conditionals
    if (a)
        cout << "8: a is non-zero: " << a << endl;
    a = 0;
    if (!a)
        cout << "9: a is zero: " << a << endl;


    // with no overflow checking, has same semantics as native unsigned integers
    a = a - 1;
    cout << "10: a = 0x" << hex << a << dec << endl;


    // safe arithmetic
    try {
        // conversion into safe version must be explicit
        x256s aa = x256s{a};
        // if any argument is safe, the operation is safe
        aa + a; // this will overflow
    }
    catch (overflow_error& e) {
        cout << "11: safe version throws: " << e.what() << endl;
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
        cout << "12: (after exception) a = " << a << endl;
    }


    // serialization/formatting:
    a = 0xdeadbeef;
    cout << "13: a in binary = " << a.to_bin() << endl;
    cout << "14: a in decimal = " << a.to_dec() << endl;
    cout << "15: a in lower case hex = " << a.to_hex() << endl;
    cout << "16: a in upper case hex = " << a.to_hex(true) << endl;
    cout << "17: a in octal = " << a.to_oct() << endl;
    cout << "18: a in base 36 = " << a.to_string(36) << endl;

    // conversion to binary
    vector<unsigned char> buffer(a.num_bits / 8);

    a.to_little_endian(buffer);
    cout << "19: little endian: " << buffer << endl;

    a.to_big_endian(buffer);
    cout << "20: big endian:    " << buffer << endl;


    x256 e{"0x123456789abcdef0"};
    x32 f = e; // truncation happens
    cout << "21: f = " << hex << f << dec << endl;
    try {
        // can't truncate non-zero bits when it's safe
        [[maybe_unused]] x32s ff = e;
        cout << "this line will not be reached: " << endl;
    }
    catch (overflow_error& e) {
        cout << "22: truncation was prevented: " << e.what() << endl;
    }


    // assignments
    a = 0;
    a += b;
    cout << "23: a = " << a << endl;
    a += 3;
    cout << "24: a = " << a << endl;

    a -= b;
    cout << "25: a = " << a << endl;
    a -= 1;
    cout << "26: a = " << a << endl;

    a *= b;
    cout << "27: a = " << a << endl;
    a *= 3;
    cout << "28: a = " << a << endl;
    /*
    a /= xint::uint(2);
    cout << "29: a = " << a << endl;
    a /= 7;
    cout << "30: a = " << a << endl;
    */

}
