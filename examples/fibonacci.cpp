#include <iostream>
#include <stdexcept>

#include <libxint/uint.hpp>


using std::cout;
using std::dec;
using std::endl;
using std::hex;


void print_fibo()
{
    unsigned i = 0;
    xint::uint<8192, true> a = 1, b = 0, c = b;
    try {
        for (;; ++i) {
            //cout << i << ": " << hex << b << dec << "\n";
            c += a;
            a = b;
            b = c;
        }
    }
    catch (std::overflow_error&) {
        cout << "stopped due to overflow" << endl;
    }
    cout << i << ": " << hex << b << dec <<endl;
}


int main()
{
    print_fibo();
}
