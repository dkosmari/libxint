#include <iostream>

#include "libxint/uint.hpp"


using namespace std;

int main()
{
    using U = xint::uint<256>;

    U a = 42;
    cout << "a: " << a << endl;

    a += 4u;
    cout << "a: " << a << endl;

    U b = a << 100;
    cout << "b: " << b << endl;
    ++b;
    cout << "b: " << b << endl;

}
