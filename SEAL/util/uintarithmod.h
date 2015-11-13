#ifndef SEAL_UTIL_UINTARITHMOD_H
#define SEAL_UTIL_UINTARITHMOD_H

#include <cstdint>
#include "util/mempool.h"
#include "util/modulus.h"

namespace seal
{
    namespace util
    {
        void modulo_uint_inplace(std::uint64_t *value, int value_uint64_count, const Modulus &modulus, MemoryPool &pool);

        void modulo_uint(const std::uint64_t *value, int value_uint64_count, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool);

        void increment_uint_mod(const std::uint64_t *operand, const std::uint64_t *modulus, int uint64_count, std::uint64_t *result);

        void decrement_uint_mod(const std::uint64_t *operand, const std::uint64_t *modulus, int uint64_count, std::uint64_t *result);

        void negate_uint_mod(const std::uint64_t *operand, const std::uint64_t *modulus, int uint64_count, std::uint64_t *result);

        void div2_uint_mod(const std::uint64_t *operand, const std::uint64_t *modulus, int uint64_count, std::uint64_t *result);

        void add_uint_uint_mod(const std::uint64_t *operand1, const std::uint64_t *operand2, const std::uint64_t *modulus, int uint64_count, std::uint64_t *result);

        void sub_uint_uint_mod(const std::uint64_t *operand1, const std::uint64_t *operand2, const std::uint64_t *modulus, int uint64_count, std::uint64_t *result);

        void multiply_uint_uint_mod(const std::uint64_t *operand1, const std::uint64_t *operand2, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool);

        void multiply_uint_uint_mod_inplace(const std::uint64_t *operand1, const std::uint64_t *operand2, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool);

        bool try_invert_uint_mod(const std::uint64_t *operand, const std::uint64_t *modulus, int uint64_count, std::uint64_t *result, MemoryPool &pool);
    }
}

#endif // SEAL_UTIL_UINTARITHMOD_H
