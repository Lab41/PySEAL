#include "seal/util/uintcore.h"
#include "seal/util/uintarith.h"
#include "seal/util/common.h"
#include "seal/util/modulus.h"
#include <algorithm>
#include <functional>
#include <array>

using namespace std;

namespace seal
{
    namespace util
    {
        void multiply_uint_uint(const uint64_t *operand1, int operand1_uint64_count, const uint64_t *operand2, int operand2_uint64_count, int result_uint64_count, uint64_t *result)
        {
#ifdef SEAL_DEBUG
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
            int operand1_index_max = min(operand1_uint64_count, result_uint64_count);
            for (int operand1_index = 0; operand1_index < operand1_index_max; operand1_index++)
            {
                const uint64_t *inner_operand2 = operand2;
                uint64_t *inner_result = result++;
                uint64_t carry = 0;
                int operand2_index = 0;
                int operand2_index_max = min(operand2_uint64_count, result_uint64_count - operand1_index);
                for (; operand2_index < operand2_index_max; operand2_index++)
                {
                    // Perform 64-bit multiplication of operand1 and operand2
                    uint64_t temp_result[2];
                    multiply_uint64(*operand1, *inner_operand2++, temp_result);
                    carry = temp_result[1] + add_uint64(temp_result[0], carry, 0, temp_result);
                    carry += add_uint64(*inner_result, temp_result[0], 0, inner_result);

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
#ifdef SEAL_DEBUG
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
                multiply_uint64(*operand1, operand2, result);
                *(result + 1) += *(operand1 + 1) * operand2;
                return;
            }

            // Clear out result.
            set_zero_uint(result_uint64_count, result);

            // Multiply operand1 and operand2.            
            uint64_t carry = 0;
            int operand1_index_max = min(operand1_uint64_count, result_uint64_count);
            for (int operand1_index = 0; operand1_index < operand1_index_max; operand1_index++)
            {
                uint64_t temp_result[2];
                multiply_uint64(*operand1++, operand2, temp_result);
                carry = temp_result[1] + add_uint64(temp_result[0], carry, 0, result++);
            }

            // Write carry if there is room in result
            if (operand1_index_max < result_uint64_count)
            {
                *result = carry;
            }
        }

        void divide_uint_uint_inplace(uint64_t *numerator, const uint64_t *denominator, int uint64_count, uint64_t *quotient, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
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

            Pointer alloc_anchor(allocate_uint(2 * uint64_count, pool));

            // Create temporary space to store mutable copy of denominator.
            uint64_t *shifted_denominator = alloc_anchor.get();

            // Create temporary space to store difference calculation.
            uint64_t *difference = shifted_denominator + uint64_count;

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

        void divide_uint_uint_inplace(uint64_t *numerator, const Modulus &denominator, int uint64_count, uint64_t *quotient, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
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

            Pointer alloc_anchor(allocate_uint(2 * uint64_count, pool));
            uint64_t *shifted = alloc_anchor.get();

            // Handle fast case denominator is power of 2 minus one.
            int modulo_power_min_one = denominator.power_of_two_minus_one();
            if (modulo_power_min_one >= 2)
            {
                while (numerator_bits >= modulo_power_min_one + 1)
                {
                    right_shift_uint(numerator, modulo_power_min_one, uint64_count, shifted);
                    filter_highbits_uint(numerator, uint64_count, modulo_power_min_one);
                    add_uint_uint(numerator, shifted, uint64_count, numerator);
                    add_uint_uint(shifted, quotient, uint64_count, quotient);
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
                uint64_t *product = shifted + uint64_count;

                // If invdenominator is at most 64 bits, we can use multiply_uint_uint64, which is faster
                function<void()> multiply_shifted_invdenom;
                if (denominator.inverse_significant_bit_count() <= bits_per_uint64)
                {
                    multiply_shifted_invdenom = [&]() {
                        multiply_uint_uint64(shifted, uint64_count, *invdenominator, uint64_count, product);
                    };
                }
                else
                {
                    multiply_shifted_invdenom = [&]() {
                        multiply_uint_uint(shifted, uint64_count, invdenominator, denominator_uint64_count, uint64_count, product); 
                    };
                }

                while (numerator_bits >= denominator_bits + 1)
                {
                    right_shift_uint(numerator, denominator_bits, uint64_count, shifted);
                    filter_highbits_uint(numerator, uint64_count, denominator_bits);

                    multiply_shifted_invdenom();

                    add_uint_uint(numerator, product, uint64_count, numerator);
                    add_uint_uint(shifted, quotient, uint64_count, quotient);
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

            //ConstPointer wide_denominator = duplicate_uint_if_needed(denominator.get(), denominator_uint64_count, uint64_count, false, pool);
            //divide_uint_uint_inplace(numerator, wide_denominator.get(), uint64_count, quotient, pool);
        }

        void divide_uint128_uint64_inplace(uint64_t *numerator, uint64_t denominator, uint64_t *quotient)
        {
#ifdef SEAL_DEBUG
            if (numerator == nullptr)
            {
                throw invalid_argument("numerator");
            }
            if (denominator == 0)
            {
                throw invalid_argument("denominator");
            }
            if (quotient == nullptr)
            {
                throw invalid_argument("quotient");
            }
            if (numerator == quotient)
            {
                throw invalid_argument("quotient cannot point to same value as numerator");
            }
#endif
            // We expect 129-bit input
            int uint64_count = 2;

            // Clear quotient. Set it to zero. 
            quotient[0] = 0;
            quotient[1] = 0;

            // Determine significant bits in numerator and denominator.
            int numerator_bits = get_significant_bit_count_uint(numerator, uint64_count);
            int denominator_bits = get_significant_bit_count(denominator);

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
                *quotient = *numerator / denominator;
                *numerator -= *quotient * denominator;
                return;
            }

            // Create temporary space to store mutable copy of denominator.
            vector<uint64_t> shifted_denominator(uint64_count, 0);
            shifted_denominator[0] = denominator;

            // Create temporary space to store difference calculation.
            vector<uint64_t> difference(uint64_count);

            // Shift denominator to bring MSB in alignment with MSB of numerator.
            int denominator_shift = numerator_bits - denominator_bits;

            left_shift_uint(shifted_denominator.data(), denominator_shift, uint64_count, shifted_denominator.data());
            denominator_bits += denominator_shift;

            // Perform bit-wise division algorithm.
            int remaining_shifts = denominator_shift;
            while (numerator_bits == denominator_bits)
            {
                // NOTE: MSBs of numerator and denominator are aligned.

                // Even though MSB of numerator and denominator are aligned, still possible numerator < shifted_denominator.
                if (sub_uint_uint(numerator, shifted_denominator.data(), uint64_count, difference.data()))
                {
                    // numerator < shifted_denominator and MSBs are aligned, so current quotient bit is zero and next one is definitely one.
                    if (remaining_shifts == 0)
                    {
                        // No shifts remain and numerator < denominator so done.
                        break;
                    }

                    // Effectively shift numerator left by 1 by instead adding numerator to difference (to prevent overflow in numerator).
                    add_uint_uint(difference.data(), numerator, uint64_count, difference.data());

                    // Adjust quotient and remaining shifts as a result of shifting numerator.
                    left_shift_uint(quotient, 1, uint64_count, quotient);
                    remaining_shifts--;
                }
                // Difference is the new numerator with denominator subtracted.

                // Update quotient to reflect subtraction.
                quotient[0] |= 1;

                // Determine amount to shift numerator to bring MSB in alignment with denominator.
                numerator_bits = get_significant_bit_count_uint(difference.data(), uint64_count);
                int numerator_shift = denominator_bits - numerator_bits;
                if (numerator_shift > remaining_shifts)
                {
                    // Clip the maximum shift to determine only the integer (as opposed to fractional) bits.
                    numerator_shift = remaining_shifts;
                }

                // Shift and update numerator.
                if (numerator_bits > 0)
                {
                    left_shift_uint(difference.data(), numerator_shift, uint64_count, numerator);
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

        void divide_uint192_uint64_inplace(uint64_t *numerator, uint64_t denominator, uint64_t *quotient)
        {
#ifdef SEAL_DEBUG
            if (numerator == nullptr)
            {
                throw invalid_argument("numerator");
            }
            if (denominator == 0)
            {
                throw invalid_argument("denominator");
            }
            if (quotient == nullptr)
            {
                throw invalid_argument("quotient");
            }
            if (numerator == quotient)
            {
                throw invalid_argument("quotient cannot point to same value as numerator");
            }
#endif
            // We expect 192-bit input
            int uint64_count = 3;

            // Clear quotient. Set it to zero. 
            quotient[0] = 0;
            quotient[1] = 0;
            quotient[2] = 0;

            // Determine significant bits in numerator and denominator.
            int numerator_bits = get_significant_bit_count_uint(numerator, uint64_count);
            int denominator_bits = get_significant_bit_count(denominator);

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
                *quotient = *numerator / denominator;
                *numerator -= *quotient * denominator;
                return;
            }

            // Create temporary space to store mutable copy of denominator.
            vector<uint64_t> shifted_denominator(uint64_count, 0);
            shifted_denominator[0] = denominator;

            // Create temporary space to store difference calculation.
            vector<uint64_t> difference(uint64_count);

            // Shift denominator to bring MSB in alignment with MSB of numerator.
            int denominator_shift = numerator_bits - denominator_bits;

            left_shift_uint(shifted_denominator.data(), denominator_shift, uint64_count, shifted_denominator.data());
            denominator_bits += denominator_shift;

            // Perform bit-wise division algorithm.
            int remaining_shifts = denominator_shift;
            while (numerator_bits == denominator_bits)
            {
                // NOTE: MSBs of numerator and denominator are aligned.

                // Even though MSB of numerator and denominator are aligned, still possible numerator < shifted_denominator.
                if (sub_uint_uint(numerator, shifted_denominator.data(), uint64_count, difference.data()))
                {
                    // numerator < shifted_denominator and MSBs are aligned, so current quotient bit is zero and next one is definitely one.
                    if (remaining_shifts == 0)
                    {
                        // No shifts remain and numerator < denominator so done.
                        break;
                    }

                    // Effectively shift numerator left by 1 by instead adding numerator to difference (to prevent overflow in numerator).
                    add_uint_uint(difference.data(), numerator, uint64_count, difference.data());

                    // Adjust quotient and remaining shifts as a result of shifting numerator.
                    left_shift_uint(quotient, 1, uint64_count, quotient);
                    remaining_shifts--;
                }
                // Difference is the new numerator with denominator subtracted.

                // Update quotient to reflect subtraction.
                quotient[0] |= 1;

                // Determine amount to shift numerator to bring MSB in alignment with denominator.
                numerator_bits = get_significant_bit_count_uint(difference.data(), uint64_count);
                int numerator_shift = denominator_bits - numerator_bits;
                if (numerator_shift > remaining_shifts)
                {
                    // Clip the maximum shift to determine only the integer (as opposed to fractional) bits.
                    numerator_shift = remaining_shifts;
                }

                // Shift and update numerator.
                if (numerator_bits > 0)
                {
                    left_shift_uint(difference.data(), numerator_shift, uint64_count, numerator);
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

        void exponentiate_uint(const uint64_t *operand, int operand_uint64_count, const uint64_t *exponent, int exponent_uint64_count, int result_uint64_count, uint64_t *result, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
            if (operand == nullptr)
            {
                throw invalid_argument("operand");
            }
            if (operand_uint64_count <= 0)
            {
                throw invalid_argument("operand_uint64_count");
            }
            if (exponent == nullptr)
            {
                throw invalid_argument("exponent");
            }
            if (exponent_uint64_count <= 0)
            {
                throw invalid_argument("exponent_uint64_count");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (result_uint64_count <= 0)
            {
                throw invalid_argument("result_uint64_count");
            }
#endif
            // Fast cases
            if (is_zero_uint(exponent, exponent_uint64_count))
            {
                set_uint(1, result_uint64_count, result);
                return;
            }
            if (is_equal_uint(exponent, exponent_uint64_count, 1))
            {
                set_uint_uint(operand, operand_uint64_count, result_uint64_count, result);
                return;
            }

            // Need to make a copy of exponent
            Pointer exponent_copy(allocate_uint(exponent_uint64_count, pool));
            set_uint_uint(exponent, exponent_uint64_count, exponent_copy.get());

            // Perform binary exponentiation.
            Pointer big_alloc(allocate_uint(result_uint64_count + result_uint64_count + result_uint64_count, pool));

            uint64_t *powerptr = big_alloc.get();
            uint64_t *productptr = powerptr + result_uint64_count;
            uint64_t *intermediateptr = productptr + result_uint64_count;

            set_uint_uint(operand, operand_uint64_count, result_uint64_count, powerptr);
            set_uint(1, result_uint64_count, intermediateptr);

            // Initially: power = operand and intermediate = 1, product is not initialized.
            while (true)
            {
                if ((*exponent_copy.get() % 2) == 1)
                {
                    multiply_truncate_uint_uint(powerptr, intermediateptr, result_uint64_count, productptr);
                    swap(productptr, intermediateptr);
                }
                right_shift_uint(exponent_copy.get(), 1, exponent_uint64_count, exponent_copy.get());
                if (is_zero_uint(exponent_copy.get(), exponent_uint64_count))
                {
                    break;
                }
                multiply_truncate_uint_uint(powerptr, powerptr, result_uint64_count, productptr);
                swap(productptr, powerptr);
            }
            set_uint_uint(intermediateptr, result_uint64_count, result);
        }

        uint64_t exponentiate_uint64(uint64_t operand, uint64_t exponent)
        {
            // Fast cases
            if (exponent == 0)
            {
                return 1;
            }
            if (exponent == 1)
            {
                return operand;
            }

            // Perform binary exponentiation.
            uint64_t power = operand;
            uint64_t product = 0;
            uint64_t intermediate = 1;

            // Initially: power = operand and intermediate = 1, product irrelevant.
            while (true)
            {
                if (exponent & 1)
                {
                    product = power * intermediate;
                    swap(product, intermediate);
                }
                exponent >>= 1;
                if (exponent == 0)
                {
                    break;
                }
                product = power * power;
                swap(product, power);
            }

            return intermediate;
        }
    }
}