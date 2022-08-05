#ifndef XINT_PRIME_HPP
#define XINT_PRIME_HPP

#include <algorithm>
#include <random>

#include "uint.hpp"
#include "random.hpp"
#include "stdlib.hpp"


namespace xint {


    template<unsigned_integral U,
             typename E>
    bool
    miller_rabin(const U& n,
                 unsigned trials,
                 E& engine)
    {
        // special cases: small primes
        if (bit_width(n) <= 8) {
            const unsigned char val = n.limb(0);
            static const unsigned char primes[] = {
                2u, 3u, 5u, 7u, 11u, 13u, 17u, 19u, 23u, 29u, 31u, 37u,
                41u, 43u, 47u, 53u, 59u, 61u, 67u, 71u, 73u, 79u, 83u, 89u,
                97u, 101u, 103u, 107u, 109u, 113u, 127u, 131u, 137u, 139u,
                149u, 151u, 157u, 163u, 167u, 173u, 179u, 181u, 191u, 193u, 197u, 199u,
                211u, 223u, 227u, 229u, 233u, 239u, 241u, 251u
            };
            return std::ranges::binary_search(primes, val);
        }

        // check for even numbers
        if ((n.limb(0) & 1) == 0)
            return false;

        /*
         * From here it's the standard Miller-Rabin test.
         * n is guaranteed to be odd and > 4.
         */
        const U minus_one = n - 1;
        const unsigned s = countr_zero(minus_one);
        const U k = minus_one >> s;

        auto witness = [&n, &minus_one, &k, s](const U& a) -> bool
        {
            auto x = powm(a, k, n);
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
        };

        uniform_int_distribution<U> dist{2, n - 2};
        for (unsigned i = 0; i < trials; ++i)
            if (!witness(dist(engine)))
                return false;
        return true;
    }



    template<unsigned_integral U>
    bool
    miller_rabin(const U& n,
                 unsigned trials)
    {
        std::random_device dev;
        std::mt19937_64 engine{dev()};
        return miller_rabin(n, trials, engine);
    }


}



#endif
