#pragma once

#include <cstdint>
#include "util/mempool.h"
#include "util/modulus.h"

namespace seal
{
    namespace util
    {
        void modulo_uint_inplace(std::uint64_t *value, int value_uint64_count, const Modulus &modulus, MemoryPool &pool, std::uint64_t *alloc_ptr = nullptr);

        void modulo_uint(const std::uint64_t *value, int value_uint64_count, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool, std::uint64_t *alloc_ptr = nullptr);

        void increment_uint_mod(const std::uint64_t *operand, const std::uint64_t *modulus, int uint64_count, std::uint64_t *result);

        void decrement_uint_mod(const std::uint64_t *operand, const std::uint64_t *modulus, int uint64_count, std::uint64_t *result);

        void negate_uint_mod(const std::uint64_t *operand, const std::uint64_t *modulus, int uint64_count, std::uint64_t *result);

        void div2_uint_mod(const std::uint64_t *operand, const std::uint64_t *modulus, int uint64_count, std::uint64_t *result);

        void add_uint_uint_mod(const std::uint64_t *operand1, const std::uint64_t *operand2, const std::uint64_t *modulus, int uint64_count, std::uint64_t *result);

        void sub_uint_uint_mod(const std::uint64_t *operand1, const std::uint64_t *operand2, const std::uint64_t *modulus, int uint64_count, std::uint64_t *result);
        
        void multiply_uint_uint_mod(const std::uint64_t *operand1, const std::uint64_t *operand2, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool, std::uint64_t *alloc_ptr = nullptr);

        void multiply_uint_uint_mod_inplace(const std::uint64_t *operand1, const std::uint64_t *operand2, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool, std::uint64_t *alloc_ptr = nullptr);

        bool try_invert_uint_mod(const std::uint64_t *operand, const std::uint64_t *modulus, int uint64_count, std::uint64_t *result, MemoryPool &pool, std::uint64_t *alloc_ptr = nullptr);
    
        // Find if root is a primitive degree-th root of unity modulo prime_modulus, where degree must be a power of two.
        bool is_primitive_root(const std::uint64_t *root, std::uint64_t degree, const Modulus &prime_modulus, MemoryPool &pool);

        // Try to find a primitive degree-th root of unity modulo prime_modulus, where degree must be a power of two.
        bool try_primitive_root(std::uint64_t degree, const Modulus &prime_modulus, MemoryPool &pool, std::uint64_t *destination);

        // Try to find the smallest (as integer) primitive degree-th root of unity modulo prime_modulus, where degree must be a power of two.
        bool try_minimal_primitive_root(std::uint64_t degree, const Modulus &prime_modulus, MemoryPool &pool, std::uint64_t *destination);
    }
}