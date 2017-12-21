#pragma once

#include <stdexcept>
#include <cstdint>
#include <functional>
#include "seal/util/uintcore.h"
#include "seal/util/mempool.h"
#include "seal/util/modulus.h"
#include "seal/util/defines.h"

namespace seal
{
    namespace util
    {
        inline unsigned char add_uint64_generic(std::uint64_t operand1, std::uint64_t operand2, unsigned char carry, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (result == nullptr)
            {
                throw std::invalid_argument("result cannot be null");
            }
#endif
            operand1 += operand2;
            *result = operand1 + carry;
            return (operand1 < operand2) || (~operand1 < carry);
        }

#pragma managed(push,off)
        inline unsigned char add_uint64(std::uint64_t operand1, std::uint64_t operand2, unsigned char carry, std::uint64_t *result)
        {
            return SEAL_ADD_CARRY_UINT64(operand1, operand2, carry, result);
        }
#pragma managed(pop)

        inline unsigned char add_uint_uint(const std::uint64_t *operand1, int operand1_uint64_count, const std::uint64_t *operand2, int operand2_uint64_count, unsigned char carry, int result_uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand1_uint64_count <= 0)
            {
                throw std::invalid_argument("operand1_uint64_count");
            }
            if (operand2_uint64_count <= 0)
            {
                throw std::invalid_argument("operand2_uint64_count");
            }
            if (result_uint64_count <= 0)
            {
                throw std::invalid_argument("result_uint64_count");
            }
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
#endif
            for (int i = 0; i < result_uint64_count; i++)
            {
                carry = add_uint64((i < operand1_uint64_count) ? *operand1++ : 0,
                    (i < operand2_uint64_count) ? *operand2++ : 0, carry, result++);
            }
            return carry;
        }

        inline unsigned char add_uint_uint(const std::uint64_t *operand1, const std::uint64_t *operand2, int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (uint64_count <= 0)
            {
                throw std::invalid_argument("uint64_count");
            }
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
#endif
            // Unroll first iteration of loop. We assume uint64_count > 0.
            unsigned char carry = add_uint64(*operand1++, *operand2++, 0, result++);

            // Do the rest
            for (int i = 1; i < uint64_count; i++)
            {
                carry = add_uint64(*operand1++, *operand2++, carry, result++);
            }
            return carry;
        }

        inline unsigned char add_uint_uint64(const std::uint64_t *operand1, std::uint64_t operand2, int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (uint64_count <= 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (operand1 == nullptr)
            {
                throw std::invalid_argument("operand1");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
#endif
            // Unroll first iteration of loop. We assume uint64_count > 0.
            unsigned char carry = add_uint64(*operand1++, operand2, 0, result++);

            // Do the rest
            for (int i = 1; i < uint64_count; i++)
            {
                carry = add_uint64(*operand1++, 0, carry, result++);
            }
            return carry;
        }

        inline unsigned char sub_uint64_generic(std::uint64_t operand1, std::uint64_t operand2, unsigned char borrow, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (result == nullptr)
            {
                throw std::invalid_argument("result cannot be null");
            }
#endif
            std::uint64_t diff = operand1 - operand2;
            *result = diff - (borrow != 0);
            return (diff > operand1) || (diff < borrow);
        }

#pragma managed(push,off)
        inline unsigned char sub_uint64(std::uint64_t operand1, std::uint64_t operand2, unsigned char borrow, std::uint64_t *result)
        {
            return SEAL_SUB_BORROW_UINT64(operand1, operand2, borrow, result);
        }
#pragma managed(pop)

        inline unsigned char sub_uint_uint(const std::uint64_t *operand1, int operand1_uint64_count, const std::uint64_t *operand2, int operand2_uint64_count, unsigned char borrow, int result_uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand1_uint64_count < 0 && operand1 != nullptr)
            {
                throw std::invalid_argument("operand1_uint64_count");
            }
            if (operand2_uint64_count < 0 && operand2 != nullptr)
            {
                throw std::invalid_argument("operand2_uint64_count");
            }
            if (result_uint64_count <= 0)
            {
                throw std::invalid_argument("result_uint64_count");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
#endif
            for (int i = 0; i < result_uint64_count; i++)
            {
                borrow = sub_uint64((i < operand1_uint64_count) ? *operand1++ : 0,
                    (i < operand2_uint64_count) ? *operand2++ : 0, borrow, result++);
            }
            return borrow;
        }

        inline unsigned char sub_uint_uint(const std::uint64_t *operand1, const std::uint64_t *operand2, int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (uint64_count <= 0)
            {
                throw std::invalid_argument("uint64_count");
            }
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
#endif
            // Unroll first iteration of loop. We assume uint64_count > 0.
            unsigned char borrow = sub_uint64(*operand1++, *operand2++, 0, result++);

            // Do the rest
            for (int i = 1; i < uint64_count; i++)
            {
                borrow = sub_uint64(*operand1++, *operand2++, borrow, result++);
            }
            return borrow;
        }

        inline unsigned char sub_uint_uint64(const std::uint64_t *operand1, std::uint64_t operand2, int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (uint64_count <= 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (operand1 == nullptr)
            {
                throw std::invalid_argument("operand1");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
#endif
            // Unroll first iteration of loop. We assume uint64_count > 0.
            unsigned char borrow = sub_uint64(*operand1++, operand2, 0, result++);

            // Do the rest
            for (int i = 1; i < uint64_count; i++)
            {
                borrow = sub_uint64(*operand1++, 0, borrow, result++);
            }
            return borrow;
        }

        inline unsigned char increment_uint(const std::uint64_t *operand, int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand == nullptr)
            {
                throw std::invalid_argument("operand");
            }
            if (uint64_count <= 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
#endif
            return add_uint_uint64(operand, 1, uint64_count, result);
        }

        inline unsigned char decrement_uint(const std::uint64_t *operand, int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("operand");
            }
            if (uint64_count < 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (result == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            return sub_uint_uint64(operand, 1, uint64_count, result);
        }

        inline void negate_uint(const std::uint64_t *operand, int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand == nullptr)
            {
                throw std::invalid_argument("operand");
            }
            if (uint64_count <= 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
#endif
            // Negation is equivalent to inverting bits and adding 1.
            unsigned char carry = add_uint64(~*operand++, 1, 0, result++);
            for (int i = 1; i < uint64_count; i++)
            {
                carry = add_uint64(~*operand++, 0, carry, result++);
            }
        }

        inline void left_shift_uint(const std::uint64_t *operand, int shift_amount, int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand == nullptr)
            {
                throw std::invalid_argument("operand");
            }
            if (shift_amount < 0 || shift_amount > uint64_count * bits_per_uint64)
            {
                throw std::invalid_argument("shift_amount");
            }
            if (uint64_count <= 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
#endif
            int uint64_shift_amount = shift_amount / bits_per_uint64;
            int bit_shift_amount = shift_amount - uint64_shift_amount * bits_per_uint64;
            int neg_bit_shift_amount = (bits_per_uint64 - bit_shift_amount) & (static_cast<std::uint64_t>(bit_shift_amount == 0) - 1);

            for (int i = uint64_count - 1; i >= uint64_shift_amount; i--)
            {
                result[i] = operand[i - uint64_shift_amount];
            }
            for (int i = uint64_shift_amount - 1; i >= 0; i--)
            {
                result[i] = 0;
            }
            if (neg_bit_shift_amount)
            {
                for (int i = uint64_count - 1; i >= uint64_shift_amount + 1; i--)
                {
                    result[i] = (result[i] << bit_shift_amount) | (result[i - 1] >> neg_bit_shift_amount);
                }
                result[uint64_shift_amount] <<= bit_shift_amount;
            }
        }

        inline void right_shift_uint(const std::uint64_t *operand, int shift_amount, int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand == nullptr)
            {
                throw std::invalid_argument("operand");
            }
            if (shift_amount < 0 || shift_amount > uint64_count * bits_per_uint64)
            {
                throw std::invalid_argument("shift_amount");
            }
            if (uint64_count <= 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
#endif
            int uint64_shift_amount = shift_amount / bits_per_uint64;
            int bit_shift_amount = shift_amount - uint64_shift_amount * bits_per_uint64;
            int neg_bit_shift_amount = (bits_per_uint64 - bit_shift_amount) & (static_cast<int32_t>(bit_shift_amount == 0) - 1);

            for (int i = 0; i < uint64_count - uint64_shift_amount; i++)
            {
                result[i] = operand[i + uint64_shift_amount];
            }
            for (int i = uint64_count - uint64_shift_amount; i < uint64_count; i++)
            {
                result[i] = 0;
            }
            if (neg_bit_shift_amount)
            {
                for (int i = 0; i < (uint64_count - uint64_shift_amount - 1); i++)
                {
                    result[i] = (result[i] >> bit_shift_amount) | (result[i + 1] << neg_bit_shift_amount);
                }
                result[uint64_count - uint64_shift_amount - 1] >>= bit_shift_amount;
            }
        }

        inline void half_round_up_uint(const std::uint64_t *operand, int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("operand");
            }
            if (uint64_count < 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (result == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            if (uint64_count == 0)
            {
                return;
            }
            // Set result to (operand + 1) / 2. To prevent overflowing operand, right shift
            // and then increment result if low-bit of operand was set.
            bool low_bit_set = operand[0] & 1;

            for (int i = 0; i < uint64_count - 1; i++)
            {
                result[i] = (operand[i] >> 1) | (operand[i + 1] << (bits_per_uint64 - 1));
            }
            result[uint64_count - 1] = operand[uint64_count - 1] >> 1;

            if (low_bit_set)
            {
                increment_uint(result, uint64_count, result);
            }
        }

        inline void not_uint(const std::uint64_t *operand, int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("operand");
            }
            if (uint64_count < 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (result == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            for (int i = 0; i < uint64_count; ++i)
            {
                *result++ = ~*operand++;
            }
        }

        inline void and_uint_uint(const std::uint64_t *operand1, const std::uint64_t *operand2, int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("operand1");
            }
            if (operand2 == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("operand2");
            }
            if (uint64_count < 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (result == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            for (int i = 0; i < uint64_count; ++i)
            {
                *result++ = *operand1++ & *operand2++;
            }
        }

        inline void or_uint_uint(const std::uint64_t *operand1, const std::uint64_t *operand2, int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("operand1");
            }
            if (operand2 == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("operand2");
            }
            if (uint64_count < 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (result == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            for (int i = 0; i < uint64_count; ++i)
            {
                *result++ = *operand1++ | *operand2++;
            }
        }

        inline void xor_uint_uint(const std::uint64_t *operand1, const std::uint64_t *operand2, int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("operand1");
            }
            if (operand2 == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("operand2");
            }
            if (uint64_count < 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (result == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            for (int i = 0; i < uint64_count; ++i)
            {
                *result++ = *operand1++ ^ *operand2++;
            }
        }

        inline void multiply_uint64_generic(std::uint64_t operand1, std::uint64_t operand2, std::uint64_t *result128)
        {
#ifdef SEAL_DEBUG
            if (result128 == nullptr)
            {
                throw std::invalid_argument("result128 cannot be null");
            }
#endif
            std::uint64_t operand1_coeff_right = operand1 & 0x00000000FFFFFFFF;
            std::uint64_t operand2_coeff_right = operand2 & 0x00000000FFFFFFFF;
            operand1 >>= 32;
            operand2 >>= 32;

            std::uint64_t middle1 = operand1 * operand2_coeff_right;
            std::uint64_t middle;
            std::uint64_t left = operand1 * operand2
                + (static_cast<uint64_t>(add_uint64(middle1, operand2 * operand1_coeff_right, 0, &middle)) << 32);
            std::uint64_t right = operand1_coeff_right * operand2_coeff_right;
            std::uint64_t temp_sum = (right >> 32) + (middle & 0x00000000FFFFFFFF);

            result128[1] = left + (middle >> 32) + (temp_sum >> 32);
            result128[0] = (temp_sum << 32) | (right & 0x00000000FFFFFFFF);
        }

        inline void multiply_uint64(std::uint64_t operand1, std::uint64_t operand2, std::uint64_t *result128)
        {
            SEAL_MULTIPLY_UINT64(operand1, operand2, result128);
        }

        inline void multiply_uint64_hw64_generic(std::uint64_t operand1, std::uint64_t operand2, std::uint64_t *hw64)
        {
#ifdef SEAL_DEBUG
            if (hw64 == nullptr)
            {
                throw std::invalid_argument("hw64 cannot be null");
            }
#endif
            std::uint64_t operand1_coeff_right = operand1 & 0x00000000FFFFFFFF;
            std::uint64_t operand2_coeff_right = operand2 & 0x00000000FFFFFFFF;
            operand1 >>= 32;
            operand2 >>= 32;

            std::uint64_t middle1 = operand1 * operand2_coeff_right;
            std::uint64_t middle;
            std::uint64_t left = operand1 * operand2
                + (static_cast<uint64_t>(add_uint64(middle1, operand2 * operand1_coeff_right, 0, &middle)) << 32);
            std::uint64_t right = operand1_coeff_right * operand2_coeff_right;
            std::uint64_t temp_sum = (right >> 32) + (middle & 0x00000000FFFFFFFF);

            *hw64 = left + (middle >> 32) + (temp_sum >> 32);
        }

        inline void multiply_uint64_hw64(std::uint64_t operand1, std::uint64_t operand2, std::uint64_t *hw64)
        {
            SEAL_MULTIPLY_UINT64_HW64(operand1, operand2, hw64);
        }

        void multiply_uint_uint(const std::uint64_t *operand1, int operand1_uint64_count, const std::uint64_t *operand2, int operand2_uint64_count, int result_uint64_count, std::uint64_t *result);

        inline void multiply_uint_uint(const std::uint64_t *operand1, const std::uint64_t *operand2, int uint64_count, std::uint64_t *result)
        {
            multiply_uint_uint(operand1, uint64_count, operand2, uint64_count, uint64_count * 2, result);
        }

        void multiply_uint_uint64(const std::uint64_t *operand1, int operand1_uint64_count, std::uint64_t operand2, int result_uint64_count, std::uint64_t *result);

        inline void multiply_truncate_uint_uint(const std::uint64_t *operand1, const std::uint64_t *operand2, int uint64_count, std::uint64_t *result)
        {
            multiply_uint_uint(operand1, uint64_count, operand2, uint64_count, uint64_count, result);
        }

        void divide_uint_uint_inplace(std::uint64_t *numerator, const std::uint64_t *denominator, int uint64_count, std::uint64_t *quotient, MemoryPool &pool);

        void divide_uint_uint_inplace(std::uint64_t *numerator, const Modulus &denominator, int uint64_count, std::uint64_t *quotient, MemoryPool &pool);

        inline void divide_uint_uint(const std::uint64_t *numerator, const std::uint64_t *denominator, int uint64_count, std::uint64_t *quotient, std::uint64_t *remainder, MemoryPool &pool)
        {
            set_uint_uint(numerator, uint64_count, remainder);
            divide_uint_uint_inplace(remainder, denominator, uint64_count, quotient, pool);
        }

        void divide_uint128_uint64_inplace(std::uint64_t *numerator, std::uint64_t denominator, std::uint64_t *quotient);

        void divide_uint192_uint64_inplace(std::uint64_t *numerator, std::uint64_t denominator, std::uint64_t *quotient);

        void exponentiate_uint(const std::uint64_t *operand, int operand_uint64_count, const std::uint64_t *exponent,
            int exponent_uint64_count, int result_uint64_count, std::uint64_t *result, MemoryPool &pool);

        std::uint64_t exponentiate_uint64(std::uint64_t operand, std::uint64_t exponent);
    }
}
