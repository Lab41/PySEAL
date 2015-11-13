#ifndef SEAL_UTIL_POLYMODULUS_H
#define SEAL_UTIL_POLYMODULUS_H

#include <cstdint>

namespace seal
{
    namespace util
    {
        class PolyModulus
        {
        public:
            PolyModulus();

            PolyModulus(const std::uint64_t *poly, int coeff_count, int coeff_uint64_count);

            const std::uint64_t *get() const
            {
                return poly_;
            }

            int coeff_count() const
            {
                return coeff_count_;
            }

            int coeff_uint64_count() const
            {
                return coeff_uint64_count_;
            }

            bool is_coeff_count_power_of_two() const
            {
                return coeff_count_power_of_two_ >= 0;
            }

            int coeff_count_power_of_two() const
            {
                return coeff_count_power_of_two_;
            }

            bool is_one_zero_one() const
            {
                return is_one_zero_one_;
            }

        private:
            const std::uint64_t *poly_;

            int coeff_count_;

            int coeff_uint64_count_;

            int coeff_count_power_of_two_;

            bool is_one_zero_one_;
        };
    }
}

#endif // SEAL_UTIL_POLYMODULUS_H
