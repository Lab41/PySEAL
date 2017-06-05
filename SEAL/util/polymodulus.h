#pragma once

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

            PolyModulus &operator =(const PolyModulus &assign) = default;

            PolyModulus(const PolyModulus &copy) = default;

            PolyModulus &operator =(PolyModulus &&assign) noexcept;

            PolyModulus(PolyModulus &&source) noexcept;

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

            bool is_fft_modulus() const
            {
                return is_one_zero_one_ && (coeff_count_power_of_two_ >= 0);
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