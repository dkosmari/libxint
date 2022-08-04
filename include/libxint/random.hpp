#ifndef XINT_RANDOM_HPP
#define XINT_RANDOM_HPP

#include <limits>
#include <random>

#include "limits.hpp"

namespace xint {

    template<unsigned_integral U>
    struct uniform_int_distribution {

        using result_type = U;

        struct param_type {
            using distribution_type = uniform_int_distribution;

            result_type a_ = 0;
            result_type range_ = std::numeric_limits<result_type>::max();

            param_type() = default;

            explicit
            param_type(const result_type& a,
                       const result_type& b = std::numeric_limits<result_type>::max()) :
                a_{a},
                range_{b - a}
            {}


            result_type a() const { return a_; }
            result_type b() const { return a_ + range_; }


            bool operator ==(const param_type&) const noexcept = default;

        };

        uniform_int_distribution() = default;

        explicit
        uniform_int_distribution(const result_type& a,
                                 const result_type& b = std::numeric_limits<result_type>::max()) :
            param_{a, b}
        {}

        explicit
        uniform_int_distribution(const param_type& p) :
            param_{p}
        {}


        void reset() {}


        result_type a() const { return param_.a; }
        result_type b() const { return param_.a + param_.range; }

        param_type param() const { return param_; }
        void param(const param_type& p) { param_ = p; }

        result_type min() const { return a(); }
        result_type max() const { return b(); }


        template<typename Gen>
        result_type
        operator()(Gen& g)
        {
            return operator()(g, param_);
        }


        template<typename Gen>
        result_type
        operator()(Gen& gen,
                   const param_type& p)
        {
            using limb_dist_t = std::uniform_int_distribution<limb_type>;

            if (p.range_ == 0)
                return p.a_;

            if (p.range_ == std::numeric_limits<result_type>::max()) {
                // range is max, so it's unconstrained
                limb_dist_t dist;
                result_type r;
                for (auto& x : r.limbs())
                    x = dist(gen);
                return r;
            }

            /*
             * Algorithm:
             *    - for all limbs below the most significant, rng is unconstrained
             *    - MSL is constrained by the MSL of range
             *    - if generated number is larger than range, reject and try again
             */
            result_type r = 0;
            auto top_bit = bit_width(p.range_);
            auto top_limb = (top_bit - 1) / limb_bits;
            limb_dist_t top_dist{0, p.range_.limb(top_limb)};
            limb_dist_t tail_dist;
            do {
                for (std::size_t i = 0; i < top_limb; ++i)
                    r.limb(i) = tail_dist(gen);
                r.limb(top_limb) = top_dist(gen);
            } while (r > p.range_);
            return r += p.a_;
        }


    private:

        param_type param_;

    };


}




#endif
