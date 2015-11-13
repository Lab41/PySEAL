#ifndef SEAL_UTIL_MODULUS_H
#define SEAL_UTIL_MODULUS_H

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

            Modulus(Modulus &&move);

            Modulus &operator =(Modulus &&assign);

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

        private:
            Modulus(const Modulus &copy) = delete;

            Modulus &operator =(const Modulus &assign) = delete;

            const std::uint64_t *modulus_;

            Pointer inverse_modulus_;

            int uint64_count_;

            int significant_bit_count_;

            int power_of_two_minus_one_;
        };
    }
}

#endif // SEAL_UTIL_MODULUS_H
