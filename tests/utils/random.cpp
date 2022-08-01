#include <limits>
#include <random>

#include "random.hpp"

using std::numeric_limits;
using std::uint32_t;
using std::uint64_t;
using std::uniform_int_distribution;


namespace utils {

    std::random_device dev;
    std::mt19937 engine{dev()};


    unsigned
    rand(unsigned max)
    {
        uniform_int_distribution<unsigned> dist{0, max};
        return dist(engine);
    }


    unsigned
    rand(unsigned min, unsigned max)
    {
        uniform_int_distribution<unsigned> dist{min, max};
        return dist(engine);
    }


    uint32_t
    rand32()
    {
        uniform_int_distribution<uint32_t> dist{0, numeric_limits<uint32_t>::max()};
        return dist(engine);
    }


    uint64_t
    rand64()
    {
        uniform_int_distribution<uint64_t> dist{0, numeric_limits<uint64_t>::max()};
        return dist(engine);
    }


}
