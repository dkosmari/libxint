#include <iostream>
#include <iomanip>
#include <random>

#define XINT_LIMB_SIZE 8
#include <libxint/uint.hpp>


using std::cout;
using std::endl;
using std::setw;
using std::hex;
using std::boolalpha;



template<xint::unsigned_integral U>
U
pow_mod(const U& x,
        const U& y,
        const U& m)
{
    U r = 1;
    auto xx = x;
    const unsigned y_width = bit_width(y);
    for (unsigned b = 0; b < y_width; ++b) {
        if (xint::eval_bit_get(y.limbs(), b))
            r = r * xx % m;
        xx = xx * xx % m;
    }
    return r;
}


// assumes n > 4 and n is odd
template<xint::unsigned_integral U>
bool
miller_rabin(const U& n,
             const U& a)
{
    auto minus_one = n - 1;
    unsigned s = countr_zero(minus_one);
    auto x = pow_mod(a,
                     minus_one >> s,
                     n);
    if (x == 1 || x == minus_one)
        return true;

    /*
     * This loop will turn x from
     *     a ^ ((n-1) >> s)
     * to
     *     a ^ ((n-1) >> 1)
     * The only operation needed is squaring x, s-1 times.
     *
     * If we never encounter -1 in the s-1 squares, that means either:
     *     - we never reach 1: that breaks Fermat's little theorem.
     *     - we reach 1 by a root that's not -1: that breaks the square root theorem.
     * Both possibilities imply that p is not prime.
     */
    for (unsigned i = 0; i < s - 1;  ++i) {
        x = x * x % n;
        if (x == minus_one)
            return true;
    }
    return false;
}


template<xint::unsigned_integral U>
bool
is_prime(const U& n,
         unsigned tries = 256)
{
    if (bit_width(n) <= xint::limb_bits) {
        // fast check on the lowest limb against small values
        switch (n.limb(0)) {
            case 0:
            case 1:
            case 4:
            case 6:
            case 8:
            case 9:
            case 10:
            case 12:
            case 14:
            case 15:
                return false;

            case 2:
            case 3:
            case 5:
            case 7:
            case 11:
            case 13:
                return true;
        }
    }
    // check if even
    if ((n.limb(0) & 1) == 0)
        return false;

    std::random_device dev;
    std::mt19937_64 eng{dev()};
    xint::uniform_int_distribution<U> dist{2, n - 2};
    for (unsigned i = 0; i < tries; ++i)
        if (!miller_rabin(n, dist(eng)))
            return false;
    return true;
}



int main()
{
    using x64 = xint::uint<64, true>;

    auto test = [](auto x)
    {
        cout << "is " << x << " prime? ";
        auto r = is_prime(x64{x});
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
