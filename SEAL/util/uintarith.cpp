#include "util/uintcore.h"
#include "util/uintarith.h"
#include "util/common.h"

using namespace std;

namespace seal
{
    namespace util
    {
        bool increment_uint(const uint64_t *operand, int uint64_count, uint64_t *result)
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
            bool carry = true;
            for (int i = 0; i < uint64_count; ++i)
            {
                uint64_t value = *operand++;
                uint64_t sum = value;
                if (carry)
                {
                    sum++;
                    carry = sum == 0;
                }
                *result++ = sum;
            }
            return carry;
        }

        bool decrement_uint(const uint64_t *operand, int uint64_count, uint64_t *result)
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
            bool borrow = true;
            for (int i = 0; i < uint64_count; ++i)
            {
                uint64_t value = *operand++;
                uint64_t diff = value;
                if (borrow)
                {
                    diff--;
                    borrow = value == 0;
                }
                *result++ = diff;
            }
            return borrow;
        }

        void negate_uint(const uint64_t *operand, int uint64_count, uint64_t *result)
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
            // Negation is equivalent to inverting bits and adding 1.
            bool carry = true;
            for (int i = 0; i < uint64_count; ++i)
            {
                uint64_t value = ~*operand++;
                uint64_t sum = value;
                if (carry)
                {
                    sum++;
                    carry = sum == 0;
                }
                *result++ = sum;
            }
        }

        void left_shift_uint(const uint64_t *operand, int shift_amount, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand == nullptr && uint64_count > 0)
            {
                throw invalid_argument("operand");
            }
            if (shift_amount < 0 || shift_amount > uint64_count * bits_per_uint64)
            {
                throw invalid_argument("shift_amount");
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
            if (operand == result && shift_amount == 0)
            {
                // Fast path to handle inplace no shifting.
                return;
            }
            const uint64_t *operand_start = operand;
            int uint64_shift_smount = shift_amount / bits_per_uint64;
            int bit_shift_amount = shift_amount % bits_per_uint64;
            int neg_bit_shift_amount = bits_per_uint64 - bit_shift_amount;
            operand += uint64_count - uint64_shift_smount;
            result += uint64_count;
            uint64_t curr = operand > operand_start ? *--operand : 0;
            for (int i = 0; i < uint64_count; ++i)
            {
                uint64_t prior = operand > operand_start ? *--operand : 0;
                *--result = bit_shift_amount > 0 ? ((curr << bit_shift_amount) | (prior >> neg_bit_shift_amount)) : curr;
                curr = prior;
            }
        }

        void right_shift_uint(const uint64_t *operand, int shift_amount, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand == nullptr && uint64_count > 0)
            {
                throw invalid_argument("operand");
            }
            if (shift_amount < 0 || shift_amount > uint64_count * bits_per_uint64)
            {
                throw invalid_argument("shift_amount");
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
            if (operand == result && shift_amount == 0)
            {
                // Fast path to handle inplace no shifting.
                return;
            }
            const uint64_t *operand_end = operand + uint64_count;
            int uint64_shift_smount = shift_amount / bits_per_uint64;
            int bit_shift_amount = shift_amount % bits_per_uint64;
            int neg_bit_shift_amount = bits_per_uint64 - bit_shift_amount;
            operand += uint64_shift_smount;
            uint64_t curr = operand < operand_end ? *operand++ : 0;
            for (int i = 0; i < uint64_count; ++i)
            {
                uint64_t next = operand < operand_end ? *operand++ : 0;
                *result++ = bit_shift_amount > 0 ? ((next << neg_bit_shift_amount) | (curr >> bit_shift_amount)) : curr;
                curr = next;
            }
        }

        void right_shift_sign_extend_uint(const uint64_t *operand, int shift_amount, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand == nullptr && uint64_count > 0)
            {
                throw invalid_argument("operand");
            }
            if (shift_amount < 0 || shift_amount > uint64_count * bits_per_uint64)
            {
                throw invalid_argument("shift_amount");
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
            if (operand == result && shift_amount == 0)
            {
                // Fast path to handle inplace no shifting.
                return;
            }
            const uint64_t *operand_end = operand + uint64_count;
            uint64_t high_bits_shift_in = is_high_bit_set_uint(operand, uint64_count) ? ~static_cast<uint64_t>(0) : 0;
            int uint64_shift_smount = shift_amount / bits_per_uint64;
            int bit_shift_amount = shift_amount % bits_per_uint64;
            int neg_bit_shift_amount = bits_per_uint64 - bit_shift_amount;
            operand += uint64_shift_smount;
            uint64_t curr = operand < operand_end ? *operand++ : high_bits_shift_in;
            for (int i = 0; i < uint64_count; ++i)
            {
                uint64_t next = operand < operand_end ? *operand++ : high_bits_shift_in;
                *result++ = bit_shift_amount > 0 ? ((next << neg_bit_shift_amount) | (curr >> bit_shift_amount)) : curr;
                curr = next;
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

        bool add_uint_uint(const uint64_t *operand1, int operand1_uint64_count, const uint64_t *operand2, int operand2_uint64_count, bool carry, int result_uint64_count, uint64_t *result)
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
            if (result_uint64_count < 0)
            {
                throw invalid_argument("result_uint64_count");
            }
            if (result == nullptr && result_uint64_count > 0)
            {
                throw invalid_argument("result");
            }
#endif
            for (int i = 0; i < result_uint64_count; ++i)
            {
                uint64_t value1 = i < operand1_uint64_count ? *operand1++ : 0;
                uint64_t value2 = i < operand2_uint64_count ? *operand2++ : 0;
                uint64_t sum = value1;
                if (carry)
                {
                    sum++;
                    carry = sum == 0;
                }
                sum += value2;
                if (sum < value2)
                {
                    carry = true;
                }
                *result++ = sum;
            }
            return carry;
        }

        bool sub_uint_uint(const uint64_t *operand1, int operand1_uint64_count, const uint64_t *operand2, int operand2_uint64_count, bool borrow, int result_uint64_count, uint64_t *result)
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
            if (result_uint64_count < 0)
            {
                throw invalid_argument("result_uint64_count");
            }
            if (result == nullptr && result_uint64_count > 0)
            {
                throw invalid_argument("result");
            }
#endif
            for (int i = 0; i < result_uint64_count; ++i)
            {
                uint64_t value1 = i < operand1_uint64_count ? *operand1++ : 0;
                uint64_t value2 = i < operand2_uint64_count ? *operand2++ : 0;
                bool next_borrow = value1 < value2;
                uint64_t diff = value1 - value2;
                if (borrow)
                {
                    if (diff == 0)
                    {
                        next_borrow = true;
                    }
                    diff--;
                }
                *result++ = diff;
                borrow = next_borrow;
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
            if (result_uint64_count < 0)
            {
                throw invalid_argument("result_uint64_count");
            }
            if (result == nullptr && result_uint64_count > 0)
            {
                throw invalid_argument("result");
            }
            if (result != nullptr && (operand1 == result || operand2 == result))
            {
                throw invalid_argument("result cannot point to the same value as operand1 or operand2");
            }
#endif
            // Determine maximum non-zero position and convert into terms of 32-bits.
            int operand1_bit_count = get_significant_bit_count_uint(operand1, operand1_uint64_count);
            operand1_uint64_count = divide_round_up(operand1_bit_count, bits_per_uint64);
            int operand1_uint32_count = divide_round_up(operand1_bit_count, bits_per_uint32);
            int operand2_bit_count = get_significant_bit_count_uint(operand2, operand2_uint64_count);
            operand2_uint64_count = divide_round_up(operand2_bit_count, bits_per_uint64);
            int operand2_uint32_count = divide_round_up(operand2_bit_count, bits_per_uint32);;
            int result_uint32_count = result_uint64_count * 2;

            // Handle fast cases.
            if (operand1_bit_count == 0 || operand2_bit_count == 0)
            {
                // If either operand is 0, then result is 0.
                set_zero_uint(result_uint64_count, result);
                return;
            }
            if (operand1_bit_count == 1)
            {
                // If operand1 is 1 then just copy.
                set_uint_uint(operand2, operand2_uint64_count, result_uint64_count, result);
                return;
            }
            if (operand2_bit_count == 1)
            {
                // If operand2 is 1 then just copy.
                set_uint_uint(operand1, operand1_uint64_count, result_uint64_count, result);
                return;
            }
            if (operand1_uint64_count == 1 && operand2_uint64_count == 1 && result_uint64_count == 1)
            {
                // If everythign is just a single 64-bit value, then use normal multiplication.
                *result = *operand1 * *operand2;
                return;
            }

            // Clear out result.
            set_zero_uint(result_uint64_count, result);

            // Multiply operand1 and operand2.
            for (int operand1_index = 0; operand1_index < operand1_uint32_count; operand1_index++)
            {
                // Verify space remains in result.
                if (operand1_index >= result_uint32_count)
                {
                    break;
                }

                uint64_t carry = 0;
                uint64_t operand1_coeff = *get_uint64_uint32(operand1, operand1_index);
                for (int operand2_index = 0; operand2_index < operand2_uint32_count; operand2_index++)
                {
                    // Calculate position for current running accumulation, and verify space remains.
                    int result_index = operand1_index + operand2_index;
                    if (result_index >= result_uint32_count)
                    {
                        break;
                    }

                    // Calculate product of two coefficients.
                    uint64_t operand2_coeff = *get_uint64_uint32(operand2, operand2_index);
                    uint64_t product = operand1_coeff * operand2_coeff;

                    // Add-in running carry.
                    product += carry;

                    // Add-in running accumulated result.
                    uint64_t accumulation = *get_uint64_uint32(result, result_index);
                    product += accumulation;

                    // NOTE: product is guaranteed to full fit in 64-bits (without overflowing) because:
                    // MAX_UINT32(operand1_coeff) * MAX_UINT32(operand2_coeff) + MAX_UINT32(carry) + MAX_UINT32(accumulation) <= MAX_UINT64.

                    // Save lower 32-bits as new accumulated result.
                    *get_uint64_uint32(result, result_index) = static_cast<uint32_t>(product);

                    // Save upper 32-bits as carry for next iteration.
                    carry = product >> 32;
                }
                if (carry != 0 && operand1_index + operand2_uint32_count < result_uint32_count)
                {
                    // result[operand1_index + operand2_uint32_count] should not have been written to before, so will be zero
                    // and can just assign.
                    *get_uint64_uint32(result, operand1_index + operand2_uint32_count) = static_cast<uint32_t>(carry);
                }
            }
        }

        void divide_uint_uint_inplace(uint64_t *numerator, const uint64_t *denominator, int uint64_count, uint64_t *quotient, MemoryPool &pool)
        {
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

            // Clear quotient.
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
                *numerator %= *denominator;
                return;
            }

            // Create temporary space to store mutable copy of denominator.
            Pointer shifted_denominator(allocate_uint(uint64_count, pool));

            // Create temporary space to store difference calculation.
            Pointer difference(allocate_uint(uint64_count, pool));

            // Shift denominator to bring MSB in alignment with MSB of numerator.
            int denominator_shift = numerator_bits - denominator_bits;
            left_shift_uint(denominator, denominator_shift, uint64_count, shifted_denominator.get());
            denominator_bits += denominator_shift;

            // Perform bit-wise division algorithm.
            int remaining_shifts = denominator_shift;
            while (numerator_bits == denominator_bits)
            {
                // NOTE: MSBs of numerator and denominator are aligned.

                // Subtract numerator and denominator.
                bool difference_is_negative = sub_uint_uint(numerator, shifted_denominator.get(), uint64_count, difference.get());

                // Even though MSB of numerator and denominator are aligned, still possible numerator < shifted_denominator.
                if (difference_is_negative)
                {
                    // numerator < shifted_denominator and MSBs are aligned, so current quotient bit is zero and next one is definitely one.
                    if (remaining_shifts == 0)
                    {
                        // No shifts remain and numerator < denominator so done.
                        break;
                    }

                    // Effectively shift numerator left by 1 by instead adding numerator to difference (to prevent overflow in numerator).
                    add_uint_uint(difference.get(), numerator, uint64_count, difference.get());

                    // Adjust quotient and remaining shifts as a result of shifting numerator.
                    left_shift_uint(quotient, 1, uint64_count, quotient);
                    remaining_shifts--;
                }
                // Difference is the new numerator with denominator subtracted.

                // Update quotient to reflect subtraction.
                quotient[0] |= 1;

                // Determine amount to shift numerator to bring MSB in alignment with denominator.
                numerator_bits = get_significant_bit_count_uint(difference.get(), uint64_count);
                int numerator_shift = denominator_bits - numerator_bits;
                if (numerator_shift > remaining_shifts)
                {
                    // Clip the maximum shift to determine only the integer (as opposed to fractional) bits.
                    numerator_shift = remaining_shifts;
                }

                // Shift and update numerator.
                if (numerator_bits > 0)
                {
                    left_shift_uint(difference.get(), numerator_shift, uint64_count, numerator);
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
    }
}
