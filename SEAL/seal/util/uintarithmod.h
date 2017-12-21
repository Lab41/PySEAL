#pragma once

#include <cstdint>
#include "seal/util/uintcore.h"
#include "seal/util/uintarith.h"
#include "seal/util/mempool.h"
#include "seal/util/modulus.h"

namespace seal
{
    namespace util
    {
        void modulo_uint_inplace(std::uint64_t *value, int value_uint64_count, const Modulus &modulus, MemoryPool &pool);

        inline void modulo_uint(const std::uint64_t *value, int value_uint64_count, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
            if (value == nullptr && value_uint64_count > 0)
            {
                throw std::invalid_argument("value");
            }
            if (value_uint64_count < 0)
            {
                throw std::invalid_argument("value_uint64_count");
            }
            if (result == nullptr && value_uint64_count > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            if (value_uint64_count == 0)
            {
                return;
            }
            if (value_uint64_count == 1)
            {
                if (modulus.significant_bit_count() > bits_per_uint64)
                {
                    set_uint_uint(value, value_uint64_count, modulus.uint64_count(), result);
                }
                else
                {
                    *result = *value % *modulus.get();
                }
                return;
            }

            Pointer value_copy(allocate_uint(value_uint64_count, pool));
            set_uint_uint(value, value_uint64_count, value_copy.get());

            modulo_uint_inplace(value_copy.get(), value_uint64_count, modulus, pool);
            set_uint_uint(value_copy.get(), value_uint64_count, modulus.uint64_count(), result);
        }

        inline void increment_uint_mod(const std::uint64_t *operand, const std::uint64_t *modulus, int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand == nullptr)
            {
                throw std::invalid_argument("operand");
            }
            if (modulus == nullptr)
            {
                throw std::invalid_argument("modulus");
            }
            if (uint64_count <= 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
            if (is_greater_than_or_equal_uint_uint(operand, modulus, uint64_count))
            {
                throw std::invalid_argument("operand");
            }
            if (modulus == result)
            {
                throw std::invalid_argument("result cannot point to the same value as modulus");
            }
#endif
            unsigned char carry = increment_uint(operand, uint64_count, result);
            if (carry || is_greater_than_or_equal_uint_uint(result, modulus, uint64_count))
            {
                sub_uint_uint(result, modulus, uint64_count, result);
            }
        }

        inline void decrement_uint_mod(const std::uint64_t *operand, const std::uint64_t *modulus, int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand == nullptr)
            {
                throw std::invalid_argument("operand");
            }
            if (modulus == nullptr)
            {
                throw std::invalid_argument("modulus");
            }
            if (uint64_count <= 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
            if (is_greater_than_or_equal_uint_uint(operand, modulus, uint64_count))
            {
                throw std::invalid_argument("operand");
            }
            if (modulus == result)
            {
                throw std::invalid_argument("result cannot point to the same value as modulus");
            }
#endif
            if (decrement_uint(operand, uint64_count, result))
            {
                add_uint_uint(result, modulus, uint64_count, result);
            }
        }

        inline void negate_uint_mod(const std::uint64_t *operand, const std::uint64_t *modulus, int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand == nullptr)
            {
                throw std::invalid_argument("operand");
            }
            if (modulus == nullptr)
            {
                throw std::invalid_argument("modulus");
            }
            if (uint64_count <= 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
            if (is_greater_than_or_equal_uint_uint(operand, modulus, uint64_count))
            {
                throw std::invalid_argument("operand");
            }
#endif
            if (is_zero_uint(operand, uint64_count))
            {
                // Negation of zero is zero.
                set_zero_uint(uint64_count, result);
            }
            else
            {
                // Otherwise, we know operand > 0 and < modulus so subtract modulus - operand.
                sub_uint_uint(modulus, operand, uint64_count, result);
            }
        }

        inline void div2_uint_mod(const std::uint64_t *operand, const std::uint64_t *modulus, int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand == nullptr)
            {
                throw std::invalid_argument("operand");
            }
            if (modulus == nullptr)
            {
                throw std::invalid_argument("modulus");
            }
            if (uint64_count <= 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
            if (!is_bit_set_uint(modulus, uint64_count, 0))
            {
                throw std::invalid_argument("modulus");
            }
            if (is_greater_than_or_equal_uint_uint(operand, modulus, uint64_count))
            {
                throw std::invalid_argument("operand");
            }
#endif
            if (*operand & 1)
            {
                unsigned char carry = add_uint_uint(operand, modulus, uint64_count, result);
                right_shift_uint(result, 1, uint64_count, result);
                if (carry)
                {
                    set_bit_uint(result, uint64_count, uint64_count * bits_per_uint64 - 1);
                }
            }
            else
            {
                right_shift_uint(operand, 1, uint64_count, result);
            }
        }

        inline void add_uint_uint_mod(const std::uint64_t *operand1, const std::uint64_t *operand2, const std::uint64_t *modulus, int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr)
            {
                throw std::invalid_argument("operand1");
            }
            if (operand2 == nullptr)
            {
                throw std::invalid_argument("operand2");
            }
            if (modulus == nullptr)
            {
                throw std::invalid_argument("modulus");
            }
            if (uint64_count <= 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
            if (is_greater_than_or_equal_uint_uint(operand1, modulus, uint64_count))
            {
                throw std::invalid_argument("operand1");
            }
            if (is_greater_than_or_equal_uint_uint(operand2, modulus, uint64_count))
            {
                throw std::invalid_argument("operand2");
            }
            if (modulus == result)
            {
                throw std::invalid_argument("result cannot point to the same value as modulus");
            }
#endif
            unsigned char carry = add_uint_uint(operand1, operand2, uint64_count, result);
            if (carry || is_greater_than_or_equal_uint_uint(result, modulus, uint64_count))
            {
                sub_uint_uint(result, modulus, uint64_count, result);
            }
        }

        inline void sub_uint_uint_mod(const std::uint64_t *operand1, const std::uint64_t *operand2, const std::uint64_t *modulus, int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr)
            {
                throw std::invalid_argument("operand1");
            }
            if (operand2 == nullptr)
            {
                throw std::invalid_argument("operand2");
            }
            if (modulus == nullptr)
            {
                throw std::invalid_argument("modulus");
            }
            if (uint64_count <= 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
            if (is_greater_than_or_equal_uint_uint(operand1, modulus, uint64_count))
            {
                throw std::invalid_argument("operand1");
            }
            if (is_greater_than_or_equal_uint_uint(operand2, modulus, uint64_count))
            {
                throw std::invalid_argument("operand2");
            }
            if (modulus == result)
            {
                throw std::invalid_argument("result cannot point to the same value as modulus");
            }
#endif
            if (sub_uint_uint(operand1, operand2, uint64_count, result))
            {
                add_uint_uint(result, modulus, uint64_count, result);
            }
        }
        
        inline void multiply_uint_uint_mod(const std::uint64_t *operand1, const std::uint64_t *operand2, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr)
            {
                throw std::invalid_argument("operand1");
            }
            if (operand2 == nullptr)
            {
                throw std::invalid_argument("operand2");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
            if (is_greater_than_or_equal_uint_uint(operand1, modulus.get(), modulus.uint64_count()))
            {
                throw std::invalid_argument("operand1");
            }
            if (is_greater_than_or_equal_uint_uint(operand2, modulus.get(), modulus.uint64_count()))
            {
                throw std::invalid_argument("operand2");
            }
            if (operand1 == result || operand2 == result || modulus.get() == result)
            {
                throw std::invalid_argument("result cannot point to the same value as operand1, operand2, or modulus");
            }
#endif
            // Calculate normal product.
            int uint64_count = modulus.uint64_count();
            int intermediate_uint64_count = uint64_count * 2;
            Pointer intermediate(allocate_uint(intermediate_uint64_count, pool));
            multiply_uint_uint(operand1, operand2, uint64_count, intermediate.get());

            // Perform modulo operation.
            modulo_uint_inplace(intermediate.get(), intermediate_uint64_count, modulus, pool);

            // Copy to result.
            set_uint_uint(intermediate.get(), uint64_count, result);
        }

        inline void multiply_uint_uint_mod_inplace(const std::uint64_t *operand1, const std::uint64_t *operand2, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr)
            {
                throw std::invalid_argument("operand1");
            }
            if (operand2 == nullptr)
            {
                throw std::invalid_argument("operand2");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
            if (is_greater_than_or_equal_uint_uint(operand1, modulus.get(), modulus.uint64_count()))
            {
                throw std::invalid_argument("operand1");
            }
            if (is_greater_than_or_equal_uint_uint(operand2, modulus.get(), modulus.uint64_count()))
            {
                throw std::invalid_argument("operand2");
            }
            if (operand1 == result || operand2 == result || modulus.get() == result)
            {
                throw std::invalid_argument("result cannot point to the same value as operand1, operand2, or modulus");
            }
#endif
            // Calculate normal product.
            int uint64_count = modulus.uint64_count();
            int result_uint64_count = uint64_count * 2;
            multiply_uint_uint(operand1, operand2, uint64_count, result);

            // Perform modulo operation.
            modulo_uint_inplace(result, result_uint64_count, modulus, pool);
        }

        bool try_invert_uint_mod(const std::uint64_t *operand, const std::uint64_t *modulus, int uint64_count, std::uint64_t *result, MemoryPool &pool);
    
        // Find if root is a primitive degree-th root of unity modulo prime_modulus, where degree must be a power of two.
        bool is_primitive_root(const std::uint64_t *root, std::uint64_t degree, const Modulus &prime_modulus, MemoryPool &pool);

        // Try to find a primitive degree-th root of unity modulo prime_modulus, where degree must be a power of two.
        bool try_primitive_root(std::uint64_t degree, const Modulus &prime_modulus, std::uint64_t *destination, MemoryPool &pool);

        // Try to find the smallest (as integer) primitive degree-th root of unity modulo prime_modulus, where degree must be a power of two.
        bool try_minimal_primitive_root(std::uint64_t degree, const Modulus &prime_modulus, std::uint64_t *destination, MemoryPool &pool);
    
        void exponentiate_uint_mod(const std::uint64_t *operand, const std::uint64_t *exponent, int exponent_uint64_count,
            const util::Modulus &modulus, std::uint64_t *result, MemoryPool &pool);
    }
}