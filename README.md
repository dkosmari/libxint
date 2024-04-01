libxint - a C++ library for extended size unsigned integers
===========================================================

This is a headers-only C++20 library to emulate large fixed-size integers.
It's licensed under the GPLv3+ license.


Installation
------------

When building from a tarball,  you can skip step 0.

  0. `./bootstrap`

  1. `./configure`

  2. `make`

  3. `sudo make install`


To uninstall:

    sudo make uninstall


This is a standard Automake package; run `./configure --help` or check `INSTALL` for more
details.


Usage
-----

First, include the header `libxint/uint.hpp` in your program.
The type `xint::uint<B>` is now available; the template argument `B` is the number
of desired bits. It can be used as any ordinary unsigned integer:

```cpp
#include <iostream>
#include <libxint/uint.hpp>

using namespace std;
using namespace xint::literals;

int main()
{
    xint::uint<1024> x = 42;
    cout << x << endl;
    x <<= 1000;
    cout << x << endl;
    ++x;
    cout << x << endl;
    cout << popcount(x) << endl;
    auto y = 0xdeadbeef0000babecafe0000dec0de_uint;
    cout << hex << y << dec << " has " << y.num_bits << " bits." << endl;
}
```


Implementation details
----------------------

This extended integer is implemented through multiple native integers, called limbs.
The size of each limb can be controlled by defining the `XINT_LIMB_SIZE` macro. Valid
sizes are 8, 16 and 32 (default is 32.) For instance:

    g++ -std=c++20 -DXINT_LIMB_SIZE=16 my-program.cpp

The limbs can be accessed directly through the `limbs()` and `limb(i)` member functions.

NOTE: it's not possible to specify a bit size that is not a multiple of the limb size. No
attempt is made to mask out the "excess" bits (for performance reasons) so the results of
all operations will be inconsistent. A `static_assert()` exists to prevent invalid limb
sizes.

