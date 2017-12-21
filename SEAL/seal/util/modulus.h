#pragma once

#include <cstdint>
#include "seal/util/mempool.h"

namespace seal
{
    namespace util
    {
        class Modulus
        {
        public:
            Modulus() = default;

            Modulus(const std::uint64_t *modulus, int uint64_count);

            Modulus(const std::uint64_t *modulus, int uint64_count, MemoryPool &pool);

            Modulus &operator =(const Modulus &assign);

            Modulus(const Modulus &copy);

            Modulus &operator =(Modulus &&assign);

            Modulus(Modulus &&source) noexcept;

            inline bool is_power_of_two_minus_one() const
            {
                return power_of_two_minus_one_ >= 0;
            }

            inline bool has_inverse() const
            {
                return inverse_modulus_.is_set();
            }

            inline const std::uint64_t *get() const
            {
                return modulus_;
            }

            inline const std::uint64_t *get_inverse() const
            {
                return inverse_modulus_.get();
            }

            inline int uint64_count() const
            {
                return uint64_count_;
            }

            inline int significant_bit_count() const
            {
                return significant_bit_count_;
            }

            inline int power_of_two_minus_one() const
            {
                return power_of_two_minus_one_;
            }

            inline int inverse_significant_bit_count() const
            {
                return inverse_significant_bit_count_;
            }

        private:
            const std::uint64_t *modulus_ = nullptr;

            int uint64_count_ = 0;

            int significant_bit_count_ = 0;

            int power_of_two_minus_one_ = -1;

            int inverse_significant_bit_count_ = 0;

            Pointer inverse_modulus_;
        };
    }
}