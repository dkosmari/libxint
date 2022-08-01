#ifndef TESTS_UTILS_RANDOM_HPP
#define TESTS_UTILS_RANDOM_HPP

#include <cstdint>


namespace utils {

    unsigned rand(unsigned max);
    unsigned rand(unsigned min, unsigned max);

    std::uint32_t rand32();
    std::uint64_t rand64();

}

#endif
