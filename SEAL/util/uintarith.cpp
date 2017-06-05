#include "util/uintcore.h"
#include "util/uintarith.h"
#include "util/common.h"
#include "util/modulus.h"
#include <algorithm>
#include <functional>
#include <array>

using namespace std;

namespace seal
{
    namespace util
    {
        unsigned char increment_uint(const uint64_t *operand, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand == nullptr)
            {
                throw invalid_argument("operand");
            }
            if (uint64_count <= 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
#endif
            *result = *operand++ + 1;
            unsigned char carry = (*result++ == 0);
            for (int i = 1; i < uint64_count; i++)
            {
                *result = *operand++ + carry;
                carry = (*result++ < static_cast<uint64_t>(carry));
            }
            return carry;
        }

        unsigned char decrement_uint(const uint64_t *operand, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand == nullptr && uint64_count > 0)
            {
                throw invalid_argument("operand");
            }
            if (uint64_count < 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (result == nullptr && uint64_count > 0)
            {
                throw invalid_argument("result");
            }
#endif
            unsigned char carry = (*operand == 0);
            *result++ = *operand++ - 1;
            for (int i = 1; i < uint64_count; i++) 
            {
                *result = *operand++ - carry;
                carry &= (*result++ == 0xFFFFFFFFFFFFFFFF);
            }
            return carry;
        }

        void negate_uint(const uint64_t *operand, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand == nullptr)
            {
                throw invalid_argument("operand");
            }
            if (uint64_count <= 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
#endif
            // Negation is equivalent to inverting bits and adding 1.
            unsigned char carry = (*operand == 0);
            *result++ = ~*operand++ + 1;
            for (int i = 1; i < uint64_count; i++)
            {
                *result++ = ~*operand + carry;
                carry &= (*operand++ == 0);
            }
        }

        void left_shift_uint(const uint64_t *operand, int shift_amount, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand == nullptr)
            {
                throw invalid_argument("operand");
            }
            if (shift_amount < 0 || shift_amount > uint64_count * bits_per_uint64)
            {
                throw invalid_argument("shift_amount");
            }
            if (uint64_count <= 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
#endif
            if (operand == result && shift_amount == 0)
            {
                // Fast path to handle inplace no shifting.
                return;
            }

            int uint64_shift_amount = shift_amount / bits_per_uint64;
            int bit_shift_amount = shift_amount - uint64_shift_amount * bits_per_uint64;
            int neg_bit_shift_amount = (bits_per_uint64 - bit_shift_amount) & (static_cast<uint64_t>(bit_shift_amount == 0) - 1);

            result += (uint64_count - 1);
            operand += (uint64_count - 1);

            for (int i = 0; i < uint64_count - uint64_shift_amount - 1; ++i)
            {
                *result = *(operand - uint64_shift_amount);
                *result <<= bit_shift_amount;
                *result-- |= (*(--operand - uint64_shift_amount) >> neg_bit_shift_amount) & static_cast<uint64_t>(-(neg_bit_shift_amount != 0));
            }
            if (uint64_shift_amount < uint64_count)
            {
                *result = *(operand - uint64_shift_amount);
                *result-- <<= bit_shift_amount;
            }
            for (int i = uint64_count - uint64_shift_amount; i < uint64_count; ++i)
            {
                *result-- = 0;
            }
        }

        void right_shift_uint(const uint64_t *operand, int shift_amount, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand == nullptr)
            {
                throw invalid_argument("operand");
            }
            if (shift_amount < 0 || shift_amount > uint64_count * bits_per_uint64)
            {
                throw invalid_argument("shift_amount");
            }
            if (uint64_count <= 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
#endif
            if (operand == result && shift_amount == 0)
            {
                // Fast path to handle inplace no shifting.
                return;
            }

            int uint64_shift_amount = shift_amount / bits_per_uint64;
            int bit_shift_amount = shift_amount - uint64_shift_amount * bits_per_uint64;
            int neg_bit_shift_amount = (bits_per_uint64 - bit_shift_amount) & (static_cast<uint64_t>(bit_shift_amount == 0) - 1);

            for (int i = 0; i < uint64_count - uint64_shift_amount - 1; i++)
            {
                *result = *(operand + uint64_shift_amount);

                *result >>= bit_shift_amount;
                *result++ |= (*(++operand + uint64_shift_amount) << neg_bit_shift_amount) & static_cast<uint64_t>(-(neg_bit_shift_amount != 0));
            }
            if (uint64_shift_amount < uint64_count)
            {
                *result = *(operand + uint64_shift_amount);
                *result++ >>= bit_shift_amount;
            }
            for (int i = uint64_count - uint64_shift_amount; i < uint64_count; i++)
            {
                *result++ = 0;
            }
        }

        void right_shift_sign_extend_uint(const uint64_t *operand, int shift_amount, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand == nullptr)
            {
                throw invalid_argument("operand");
            }
            if (shift_amount < 0 || shift_amount > uint64_count * bits_per_uint64)
            {
                throw invalid_argument("shift_amount");
            }
            if (uint64_count <= 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
#endif
            if (operand == result && shift_amount == 0)
            {
                // Fast path to handle inplace no shifting.
                return;
            }

            int uint64_shift_amount = shift_amount / bits_per_uint64;
            int bit_shift_amount = shift_amount - uint64_shift_amount * bits_per_uint64;
            int neg_bit_shift_amount = (bits_per_uint64 - bit_shift_amount) & (static_cast<uint64_t>(bit_shift_amount == 0) - 1);
            uint64_t high_bits_shift_in = ~((operand[uint64_count - 1] >> (bits_per_uint64 - 1)) - 1);
                
            for (int i = 0; i < uint64_count - uint64_shift_amount - 1; i++)
            {
                *result = *(operand++ + uint64_shift_amount) >> bit_shift_amount;
                *result++ |= (*(operand + uint64_shift_amount) << neg_bit_shift_amount) & static_cast<uint64_t>(-(neg_bit_shift_amount != 0));
            }
            if (uint64_shift_amount < uint64_count)
            {
                *result = *(operand + uint64_shift_amount) >> bit_shift_amount;
                *result++ |= (high_bits_shift_in << neg_bit_shift_amount) & static_cast<uint64_t>(-(neg_bit_shift_amount != 0));
            }
            for (int i = uint64_count - uint64_shift_amount; i < uint64_count; i++)
            {
                *result++ = high_bits_shift_in;
            }
        }

        void half_round_up_uint(const uint64_t *operand, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand == nullptr && uint64_count > 0)
            {
                throw invalid_argument("operand");
            }
            if (uint64_count < 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (result == nullptr && uint64_count > 0)
            {
                throw invalid_argument("result");
            }
#endif
            if (uint64_count == 0)
            {
                return;
            }
            // Set result to (operand + 1) / 2. To prevent overflowing operand, right shift
            // and then increment result if low-bit of operand was set.
            bool low_bit_set = operand[0] & 1;
            right_shift_uint(operand, 1, uint64_count, result);
            if (low_bit_set)
            {
                increment_uint(result, uint64_count, result);
            }
        }

        void not_uint(const uint64_t *operand, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand == nullptr && uint64_count > 0)
            {
                throw invalid_argument("operand");
            }
            if (uint64_count < 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (result == nullptr && uint64_count > 0)
            {
                throw invalid_argument("result");
            }
#endif
            for (int i = 0; i < uint64_count; ++i)
            {
                *result++ = ~*operand++;
            }
        }

        void and_uint_uint(const uint64_t *operand1, const uint64_t *operand2, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand1 == nullptr && uint64_count > 0)
            {
                throw invalid_argument("operand1");
            }
            if (operand2 == nullptr && uint64_count > 0)
            {
                throw invalid_argument("operand2");
            }
            if (uint64_count < 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (result == nullptr && uint64_count > 0)
            {
                throw invalid_argument("result");
            }
#endif
            for (int i = 0; i < uint64_count; ++i)
            {
                *result++ = *operand1++ & *operand2++;
            }
        }

        void or_uint_uint(const uint64_t *operand1, const uint64_t *operand2, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand1 == nullptr && uint64_count > 0)
            {
                throw invalid_argument("operand1");
            }
            if (operand2 == nullptr && uint64_count > 0)
            {
                throw invalid_argument("operand2");
            }
            if (uint64_count < 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (result == nullptr && uint64_count > 0)
            {
                throw invalid_argument("result");
            }
#endif
            for (int i = 0; i < uint64_count; ++i)
            {
                *result++ = *operand1++ | *operand2++;
            }
        }

        void xor_uint_uint(const uint64_t *operand1, const uint64_t *operand2, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand1 == nullptr && uint64_count > 0)
            {
                throw invalid_argument("operand1");
            }
            if (operand2 == nullptr && uint64_count > 0)
            {
                throw invalid_argument("operand2");
            }
            if (uint64_count < 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (result == nullptr && uint64_count > 0)
            {
                throw invalid_argument("result");
            }
#endif
            for (int i = 0; i < uint64_count; ++i)
            {
                *result++ = *operand1++ ^ *operand2++;
            }
        }

        unsigned char add_uint_uint(const uint64_t *operand1, int operand1_uint64_count, const uint64_t *operand2, int operand2_uint64_count, unsigned char carry, int result_uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand1_uint64_count < 0 && operand1 != nullptr)
            {
                throw invalid_argument("operand1_uint64_count");
            }
            if (operand2_uint64_count < 0 && operand2 != nullptr)
            {
                throw invalid_argument("operand2_uint64_count");
            }
            if (result_uint64_count <= 0)
            {
                throw invalid_argument("result_uint64_count");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
#endif
            for (int i = 0; i < result_uint64_count; i++)
            {
                carry = add_uint64_uint64((i < operand1_uint64_count) ? *operand1++ : 0,
                    (i < operand2_uint64_count) ? *operand2++ : 0, carry, result++);
            }
            return carry;
        }

        unsigned char add_uint_uint(const uint64_t *operand1, const uint64_t *operand2, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (uint64_count <= 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (operand1 == nullptr)
            {
                throw invalid_argument("operand1");
            }
            if (operand2 == nullptr)
            {
                throw invalid_argument("operand2");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
#endif
            // Unroll first iteration of loop. We assume uint64_count > 0.
            unsigned char carry = add_uint64_uint64(*operand1++, *operand2++, 0, result++);

            // Do the rest
            for (int i = 1; i < uint64_count; i++)
            {
                carry = add_uint64_uint64(*operand1++, *operand2++, carry, result++);
            }
            return carry;
        }

        unsigned char sub_uint_uint(const uint64_t *operand1, int operand1_uint64_count, const uint64_t *operand2, int operand2_uint64_count, unsigned char borrow, int result_uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand1_uint64_count < 0 && operand1 != nullptr)
            {
                throw invalid_argument("operand1_uint64_count");
            }
            if (operand2_uint64_count < 0 && operand2 != nullptr)
            {
                throw invalid_argument("operand2_uint64_count");
            }
            if (result_uint64_count <= 0)
            {
                throw invalid_argument("result_uint64_count");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
#endif
            for (int i = 0; i < result_uint64_count; i++)
            {
                borrow = sub_uint64_uint64((i < operand1_uint64_count) ? *operand1++ : 0, 
                    (i < operand2_uint64_count) ? *operand2++ : 0, borrow, result++);
            }
            return borrow;
        }

        unsigned char sub_uint_uint(const uint64_t *operand1, const uint64_t *operand2, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (uint64_count <= 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (operand1 == nullptr)
            {
                throw invalid_argument("operand1");
            }
            if (operand2 == nullptr)
            {
                throw invalid_argument("operand2");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
#endif
            // Unroll first iteration of loop. We assume uint64_count > 0.
            unsigned char borrow = sub_uint64_uint64(*operand1++, *operand2++, 0, result++);

            // Do the rest
            for (int i = 1; i < uint64_count; i++)
            {
                borrow = sub_uint64_uint64(*operand1++, *operand2++, borrow, result++);
            }
            return borrow;
        }
        
        void multiply_uint_uint(const uint64_t *operand1, int operand1_uint64_count, const uint64_t *operand2, int operand2_uint64_count, int result_uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand1 == nullptr && operand1_uint64_count > 0)
            {
                throw invalid_argument("operand1");
            }
            if (operand1_uint64_count < 0)
            {
                throw invalid_argument("operand1_uint64_count");
            }
            if (operand2 == nullptr && operand2_uint64_count > 0)
            {
                throw invalid_argument("operand2");
            }
            if (operand2_uint64_count < 0)
            {
                throw invalid_argument("operand2_uint64_count");
            }
            if (result_uint64_count <= 0)
            {
                throw invalid_argument("result_uint64_count");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (result != nullptr && (operand1 == result || operand2 == result))
            {
                throw invalid_argument("result cannot point to the same value as operand1 or operand2");
            }
#endif
            // Handle fast cases.
            if (operand1_uint64_count == 0 || operand2_uint64_count == 0)
            {
                // If either operand is 0, then result is 0.
                set_zero_uint(result_uint64_count, result);
                return;
            }
            if (result_uint64_count == 1)
            {
                *result = *operand1 * *operand2;
                return;
            }
            
            // In some cases these improve performance.
            operand1_uint64_count = get_significant_uint64_count_uint(operand1, operand1_uint64_count);
            operand2_uint64_count = get_significant_uint64_count_uint(operand2, operand2_uint64_count);

            // More fast cases
            if (operand1_uint64_count == 1)
            {
                multiply_uint_uint64(operand2, operand2_uint64_count, *operand1, result_uint64_count, result);
                return;
            }
            if (operand2_uint64_count == 1)
            {
                multiply_uint_uint64(operand1, operand1_uint64_count, *operand2, result_uint64_count, result);
                return;
            }

            // Clear out result.
            set_zero_uint(result_uint64_count, result);

            // Multiply operand1 and operand2.            
            uint64_t temp_result;
            uint64_t carry, new_carry;
            const uint64_t *inner_operand2;
            uint64_t *inner_result;

            int operand1_index_max = min(operand1_uint64_count, result_uint64_count);
            for (int operand1_index = 0; operand1_index < operand1_index_max; operand1_index++)
            {
                inner_operand2 = operand2;
                inner_result = result++;
                carry = 0;
                int operand2_index = 0;
                int operand2_index_max = min(operand2_uint64_count, result_uint64_count - operand1_index);
                for (; operand2_index < operand2_index_max; operand2_index++)
                {
                    // Perform 64-bit multiplication of operand1 and operand2
                    temp_result = multiply_uint64_uint64(*operand1, *inner_operand2++, &new_carry);
                    carry = new_carry + add_uint64_uint64(temp_result, carry, 0, &temp_result);
                    carry += add_uint64_uint64(*inner_result, temp_result, 0, inner_result);

                    inner_result++;
                }

                // Write carry if there is room in result
                if (operand1_index + operand2_index_max < result_uint64_count)
                {
                    *inner_result = carry;
                }

                operand1++;
            }
        }

        void multiply_uint_uint64(const uint64_t *operand1, int operand1_uint64_count, uint64_t operand2, int result_uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand1 == nullptr && operand1_uint64_count > 0)
            {
                throw invalid_argument("operand1");
            }
            if (operand1_uint64_count < 0)
            {
                throw invalid_argument("operand1_uint64_count");
            }
            if (result_uint64_count <= 0)
            {
                throw invalid_argument("result_uint64_count");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (result != nullptr && operand1 == result)
            {
                throw invalid_argument("result cannot point to the same value as operand1");
            }
#endif
            // Handle fast cases.
            if (operand1_uint64_count == 0 || operand2 == 0)
            {
                // If either operand is 0, then result is 0.
                set_zero_uint(result_uint64_count, result);
                return;
            }
            if (result_uint64_count == 1)
            {
                *result = *operand1 * operand2;
                return;
            }

            // More fast cases
            if (result_uint64_count == 2 && operand1_uint64_count > 1)
            {
                *result = multiply_uint64_uint64(*operand1, operand2, result + 1);
                *(result + 1) += *(operand1 + 1) * operand2;
                return;
            }

            // Clear out result.
            set_zero_uint(result_uint64_count, result);

            // Multiply operand1 and operand2.            
            uint64_t new_carry, carry = 0;
            uint64_t temp_result;

            int operand1_index_max = min(operand1_uint64_count, result_uint64_count);
            for (int operand1_index = 0; operand1_index < operand1_index_max; operand1_index++)
            {
                temp_result = multiply_uint64_uint64(*operand1++, operand2, &new_carry);
                carry = new_carry + add_uint64_uint64(temp_result, carry, 0, result++);
            }

            // Write carry if there is room in result
            if (operand1_index_max < result_uint64_count)
            {
                *result = carry;
            }
        }

        void divide_uint_uint_inplace(uint64_t *numerator, const uint64_t *denominator, int uint64_count, uint64_t *quotient, MemoryPool &pool, uint64_t *alloc_ptr)
        {
            // alloc_ptr should point to 2 x uint64_count memory

#ifdef _DEBUG
            if (numerator == nullptr && uint64_count > 0)
            {
                throw invalid_argument("numerator");
            }
            if (denominator == nullptr && uint64_count > 0)
            {
                throw invalid_argument("denominator");
            }
            if (uint64_count < 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (quotient == nullptr && uint64_count > 0)
            {
                throw invalid_argument("quotient");
            }
            if (is_zero_uint(denominator, uint64_count) && uint64_count > 0)
            {
                throw invalid_argument("denominator");
            }
            if (quotient != nullptr && (numerator == quotient || denominator == quotient))
            {
                throw invalid_argument("quotient cannot point to same value as numerator or denominator");
            }
#endif
            if (uint64_count == 0)
            {
                return;
            }

            // Clear quotient. Set it to zero. 
            set_zero_uint(uint64_count, quotient);

            // Determine significant bits in numerator and denominator.
            int numerator_bits = get_significant_bit_count_uint(numerator, uint64_count);
            int denominator_bits = get_significant_bit_count_uint(denominator, uint64_count);

            // If numerator has fewer bits than denominator, then done.
            if (numerator_bits < denominator_bits)
            {
                return;
            }

            // Only perform computation up to last non-zero uint64s.
            uint64_count = divide_round_up(numerator_bits, bits_per_uint64);

            // Handle fast case.
            if (uint64_count == 1)
            {
                *quotient = *numerator / *denominator;
                *numerator -= *quotient * *denominator;
                return;
            }

            // Create temporary space to store mutable copy of denominator.
            // Set shifted_denominator to point to either an existing allocation given as parameter,
            // or else to a new allocation from the memory pool.
            Pointer shifted_denominator_anchor;
            uint64_t *shifted_denominator = alloc_ptr;
            if (alloc_ptr == nullptr)
            {
                shifted_denominator_anchor = allocate_uint(uint64_count, pool);
                shifted_denominator = shifted_denominator_anchor.get();
            }
            else
            {
                alloc_ptr += uint64_count;
            }

            // Create temporary space to store difference calculation.
            // Set difference to point to either an existing allocation given as parameter,
            // or else to a new allocation from the memory pool.
            Pointer difference_anchor;
            uint64_t *difference = alloc_ptr;
            if (alloc_ptr == nullptr)
            {
                difference_anchor = allocate_uint(uint64_count, pool);
                difference = difference_anchor.get();
            }

            // Shift denominator to bring MSB in alignment with MSB of numerator.
            int denominator_shift = numerator_bits - denominator_bits;
            left_shift_uint(denominator, denominator_shift, uint64_count, shifted_denominator);
            denominator_bits += denominator_shift;

            // Perform bit-wise division algorithm.
            int remaining_shifts = denominator_shift;
            while (numerator_bits == denominator_bits)
            {
                // NOTE: MSBs of numerator and denominator are aligned.

                // Even though MSB of numerator and denominator are aligned, still possible numerator < shifted_denominator.
                if (sub_uint_uint(numerator, shifted_denominator, uint64_count, difference))
                {
                    // numerator < shifted_denominator and MSBs are aligned, so current quotient bit is zero and next one is definitely one.
                    if (remaining_shifts == 0)
                    {
                        // No shifts remain and numerator < denominator so done.
                        break;
                    }

                    // Effectively shift numerator left by 1 by instead adding numerator to difference (to prevent overflow in numerator).
                    add_uint_uint(difference, numerator, uint64_count, difference);

                    // Adjust quotient and remaining shifts as a result of shifting numerator.
                    left_shift_uint(quotient, 1, uint64_count, quotient);
                    remaining_shifts--;
                }
                // Difference is the new numerator with denominator subtracted.

                // Update quotient to reflect subtraction.
                quotient[0] |= 1;

                // Determine amount to shift numerator to bring MSB in alignment with denominator.
                numerator_bits = get_significant_bit_count_uint(difference, uint64_count);
                int numerator_shift = denominator_bits - numerator_bits;
                if (numerator_shift > remaining_shifts)
                {
                    // Clip the maximum shift to determine only the integer (as opposed to fractional) bits.
                    numerator_shift = remaining_shifts;
                }

                // Shift and update numerator.
                if (numerator_bits > 0)
                {
                    left_shift_uint(difference, numerator_shift, uint64_count, numerator);
                    numerator_bits += numerator_shift;
                }
                else
                {
                    // Difference is zero so no need to shift, just set to zero.
                    set_zero_uint(uint64_count, numerator);
                }

                // Adjust quotient and remaining shifts as a result of shifting numerator.
                left_shift_uint(quotient, numerator_shift, uint64_count, quotient);
                remaining_shifts -= numerator_shift;
            }

            // Correct numerator (which is also the remainder) for shifting of denominator, unless it is just zero.
            if (numerator_bits > 0)
            {
                right_shift_uint(numerator, denominator_shift, uint64_count, numerator);
            }
        }

        void divide_uint_uint_inplace(uint64_t *numerator, const Modulus &denominator, int uint64_count, uint64_t *quotient, MemoryPool &pool, uint64_t *alloc_ptr)
        {
            // alloc_ptr should point to 2 x uint64_count memory

#ifdef _DEBUG
            if (numerator == nullptr && uint64_count > 0)
            {
                throw invalid_argument("numerator");
            }
            if (denominator.significant_bit_count() <= 0)
            {
                throw invalid_argument("denominator");
            }
            if (uint64_count < 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (quotient == nullptr && uint64_count > 0)
            {
                throw invalid_argument("quotient");
            }
            if (quotient != nullptr && (numerator == quotient || denominator.get() == quotient))
            {
                throw invalid_argument("quotient cannot point to same value as numerator or denominator");
            }
#endif
            if (uint64_count == 0)
            {
                return;
            }

            // Clear quotient. Set it to zero. 
            set_zero_uint(uint64_count, quotient);

            // Determine significant bits in numerator and denominator.
            int numerator_bits = get_significant_bit_count_uint(numerator, uint64_count);
            int denominator_bits = denominator.significant_bit_count();
            int denominator_uint64_count = denominator.uint64_count();

            // If numerator has fewer bits than denominator, then done.
            if (numerator_bits < denominator_bits)
            {
                return;
            }

            // Only perform computation up to last non-zero uint64s.
            uint64_count = divide_round_up(numerator_bits, bits_per_uint64);

            // Handle fast case. Note that here numerator cannot have fewer bits than denominator so
            // don't need to worry about higher words of denominator.
            if (uint64_count == 1)
            {
                *quotient = *numerator / *denominator.get();
                *numerator -= *quotient * *denominator.get();
                return;
            }

            // Handle fast case denominator is power of 2 minus one.
            int modulo_power_min_one = denominator.power_of_two_minus_one();
            if (modulo_power_min_one >= 2)
            {
                // Set shifted_ptr to point to either an existing allocation given as parameter,
                // or else to a new allocation from the memory pool.
                Pointer shifted;
                uint64_t *shifted_ptr = alloc_ptr;
                if (alloc_ptr == nullptr)
                {
                    shifted = allocate_uint(uint64_count, pool);
                    shifted_ptr = shifted.get();
                }

                while (numerator_bits >= modulo_power_min_one + 1)
                {
                    right_shift_uint(numerator, modulo_power_min_one, uint64_count, shifted_ptr);
                    filter_highbits_uint(numerator, uint64_count, modulo_power_min_one);
                    add_uint_uint(numerator, shifted_ptr, uint64_count, numerator);
                    add_uint_uint(shifted_ptr, quotient, uint64_count, quotient);
                    numerator_bits = get_significant_bit_count_uint(numerator, uint64_count);
                }

                if (is_greater_than_or_equal_uint_uint(numerator, uint64_count, denominator.get(), denominator_uint64_count))
                {
                    // No need to do subtraction due to the shape of the modulus.
                    //sub_uint_uint(value, uint64_count, modulus.get(), modulus_uint64_count, false, uint64_count, value);
                    set_zero_uint(uint64_count, numerator);
                    increment_uint(quotient, uint64_count, quotient);
                }
                return;
            }
            
            // Handle fast case -denominator (clipped to denominator_bits) is small.
            const uint64_t *invdenominator = denominator.get_inverse();
            if (invdenominator != nullptr)
            {
                // Iterate to shorten value.

                // Set shifted_ptr to point to either an existing allocation given as parameter,
                // or else to a new allocation from the memory pool.
                Pointer shifted;
                uint64_t *shifted_ptr = alloc_ptr;
                if (alloc_ptr == nullptr)
                {
                    shifted = allocate_uint(uint64_count, pool);
                    shifted_ptr = shifted.get();
                }
                else
                {
                    alloc_ptr += uint64_count;
                }

                // Set shifted_ptr to point to either an existing allocation given as parameter,
                // or else to a new allocation from the memory pool.
                Pointer product;
                uint64_t *product_ptr = alloc_ptr;
                if (alloc_ptr == nullptr)
                {
                    product = allocate_uint(uint64_count, pool);
                    product_ptr = product.get();
                }

                // If invdenominator is at most 64 bits, we can use multiply_uint_uint64, which is faster
                function<void()> multiply_shifted_invdenom;
                if (denominator.inverse_significant_bit_count() <= bits_per_uint64)
                {
                    multiply_shifted_invdenom = [&]() {
                        multiply_uint_uint64(shifted_ptr, uint64_count, *invdenominator, uint64_count, product_ptr);
                    };
                }
                else
                {
                    multiply_shifted_invdenom = [&]() {
                        multiply_uint_uint(shifted_ptr, uint64_count, invdenominator, denominator_uint64_count, uint64_count, product_ptr); 
                    };
                }

                while (numerator_bits >= denominator_bits + 1)
                {
                    right_shift_uint(numerator, denominator_bits, uint64_count, shifted_ptr);
                    filter_highbits_uint(numerator, uint64_count, denominator_bits);

                    multiply_shifted_invdenom();

                    add_uint_uint(numerator, product_ptr, uint64_count, numerator);
                    add_uint_uint(shifted_ptr, quotient, uint64_count, quotient);
                    numerator_bits = get_significant_bit_count_uint(numerator, uint64_count);
                }

                // Use subtraction for few remaining iterations.
                if (is_greater_than_or_equal_uint_uint(numerator, uint64_count, denominator.get(), denominator_uint64_count))
                {
                    sub_uint_uint(numerator, uint64_count, denominator.get(), denominator_uint64_count, false, uint64_count, numerator);
                    increment_uint(quotient, uint64_count, quotient);
                }
                return;
            }

            ConstPointer wide_denominator = duplicate_uint_if_needed(denominator.get(), denominator_uint64_count, uint64_count, false, pool);
            divide_uint_uint_inplace(numerator, wide_denominator.get(), uint64_count, quotient, pool, alloc_ptr);
        }
    }
}