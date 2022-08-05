#include <cstdint>
#include <iostream>
#include <iomanip>
#include <random>

#define XINT_LIMB_SIZE 8
#include <libxint/uint.hpp>
#include <libxint/prime.hpp>


using std::cout;
using std::endl;
using std::setw;
using std::hex;
using std::boolalpha;



int main()
{
    using x64 = xint::uint<64, true>;

    auto test = [](std::uint64_t x)
    {
        cout << "is " << x << " prime? ";
        auto r = xint::miller_rabin(x64{x}, 25);
        cout << boolalpha << r << endl;
    };

    test(0);
    test(1);
    test(2);
    test(3);
    test(4);
    test(5);
    test(77);
    test(79);

    int carmichael[] = {
        561, 1105, 1729, 2465, 2821, 6601, 8911, 10585, 15841, 29341,
        41041, 46657, 52633, 62745, 63973, 75361, 101101, 115921, 126217, 162401,
        172081, 188461, 252601, 278545, 294409, 314821, 334153, 340561, 399001,
        410041, 449065, 488881, 512461 };
    for (auto x : carmichael)
        test(x);

}
