#pragma once

#include <cstdint>
#include "util/mempool.h"

namespace seal
{
    namespace util
    {
        class Modulus
        {
        public:
            Modulus();

            Modulus(const std::uint64_t *modulus, int uint64_count);

            Modulus(const std::uint64_t *modulus, int uint64_count, MemoryPool &pool);

            Modulus &operator =(const Modulus &assign);

            Modulus(const Modulus &copy);

            Modulus &operator =(Modulus &&assign);

            Modulus(Modulus &&source) noexcept;

            bool is_power_of_two_minus_one() const
            {
                return power_of_two_minus_one_ >= 0;
            }

            bool has_inverse() const
            {
                return inverse_modulus_.is_set();
            }

            const std::uint64_t *get() const
            {
                return modulus_;
            }

            const std::uint64_t *get_inverse() const
            {
                return inverse_modulus_.get();
            }

            int uint64_count() const
            {
                return uint64_count_;
            }

            int significant_bit_count() const
            {
                return significant_bit_count_;
            }

            int power_of_two_minus_one() const
            {
                return power_of_two_minus_one_;
            }

            int inverse_significant_bit_count() const
            {
                return inverse_significant_bit_count_;
            }

        private:
            const std::uint64_t *modulus_;

            int uint64_count_;

            int significant_bit_count_;

            int power_of_two_minus_one_;

            int inverse_significant_bit_count_;

            Pointer inverse_modulus_;
        };
    }
}