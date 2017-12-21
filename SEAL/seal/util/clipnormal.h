#pragma once

#include <random>
#include <cmath>

namespace seal
{
    namespace util
    {
        class ClippedNormalDistribution
        {
        public:
            typedef double result_type;

            typedef ClippedNormalDistribution param_type;

            ClippedNormalDistribution(result_type mean, result_type standard_deviation, result_type max_deviation);

            template <typename RNG>
            inline result_type operator()(RNG &engine, const param_type &parm)
            {
                param(parm);
                return operator()(engine);
            }

            template <typename RNG>
            inline result_type operator()(RNG &engine)
            {
                result_type mean = normal_.mean();
                while (true)
                {
                    result_type value = normal_(engine);
                    result_type deviation = std::abs(value - mean);
                    if (deviation <= max_deviation_)
                    {
                        return value;
                    }
                }
            }

            inline result_type mean() const
            {
                return normal_.mean();
            }

            inline result_type standard_deviation() const
            {
                return normal_.stddev();
            }

            inline result_type max_deviation() const
            {
                return max_deviation_;
            }

            inline result_type min() const
            {
                return normal_.mean() - max_deviation_;
            }

            inline result_type max() const
            {
                return normal_.mean() + max_deviation_;
            }

            inline param_type param() const
            {
                return *this;
            }

            inline void param(const param_type &parm)
            {
                *this = parm;
            }

            inline void reset()
            {
                normal_.reset();
            }

        private:
            std::normal_distribution<result_type> normal_;

            result_type max_deviation_;
        };
    }
}
