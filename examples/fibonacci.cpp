#include <iostream>
#include <stdexcept>
#include <utility>

#include <libxint/uint.hpp>


using std::cout;
using std::endl;


void print_fibo()
{
    unsigned i = 0;
    xint::uint<4096, true> a = 1, b = 0;
    try {
        for (;; ++i) {
            cout << i << ": " << b << "\n";
            auto c = a + b;
            a = std::move(b);
            b = std::move(c);
        }
    }
    catch (std::overflow_error&) {
        cout << "stopped due to overflow" << endl;
    }
}


int main()
{
    print_fibo();
}
