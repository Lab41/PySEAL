#include "util/uintcore.h"
#include "util/uintarith.h"
#include "util/uintarithmod.h"
#include "util/common.h"
#include "uintextras.h"
#include <random>

using namespace std;

namespace seal
{
    namespace util
    {
        void modulo_uint_inplace(uint64_t *value, int value_uint64_count, const Modulus &modulus, MemoryPool &pool, uint64_t *alloc_ptr)
        {
            // alloc_ptr should point to 2 x value_uint64_count memory

#ifdef _DEBUG
            if (value == nullptr && value_uint64_count > 0)
            {
                throw invalid_argument("value");
            }
            if (value_uint64_count < 0)
            {
                throw invalid_argument("value_uint64_count");
            }
#endif
            // Handle fast cases.
            if (value_uint64_count == 0)
            {
                return;
            }

            // Determine significant bits in value and modulus.
            int value_bits = get_significant_bit_count_uint(value, value_uint64_count);
            int modulus_bits = modulus.significant_bit_count();

            // If value has fewer bits than modulus, then done.
            if (value_bits < modulus_bits)
            {
                return;
            }

            // Only perform computation on non-zero uint64s.
            int uint64_count = divide_round_up(value_bits, bits_per_uint64);

            int modulus_uint64_count = modulus.uint64_count();
            if (uint64_count < modulus_uint64_count)
            {
                modulus_uint64_count = uint64_count;
            }

            // If value is smaller, then done.
            const uint64_t *modulusptr = modulus.get();
            if (value_bits == modulus_bits && is_less_than_uint_uint(value, modulusptr, uint64_count))
            {
                return;
            }

            // Handle fast case.
            if (uint64_count == 1)
            {
                *value %= *modulusptr;
                return;
            }

            // Handle fast case modulo is power of 2 minus one.
            int modulo_power_min_one = modulus.power_of_two_minus_one();
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

                while (value_bits >= modulus_bits + 1)
                {
                    right_shift_uint(value, modulo_power_min_one, uint64_count, shifted_ptr);
                    filter_highbits_uint(value, uint64_count, modulo_power_min_one);
                    add_uint_uint(value, shifted_ptr, uint64_count, value);
                    value_bits = get_significant_bit_count_uint(value, uint64_count);
                }
                if (is_greater_than_or_equal_uint_uint(value, uint64_count, modulus.get(), modulus_uint64_count))
                {
                    // No need to do subtraction due to the shape of the modulus.
                    //sub_uint_uint(value, uint64_count, modulus.get(), modulus_uint64_count, false, uint64_count, value);
                    set_zero_uint(uint64_count, value);
                }
                return;
            }

            // Handle fast case -modulo (clipped to modulus_bits) is small.
            const uint64_t *invmodulus = modulus.get_inverse();
            if (invmodulus != nullptr)
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

                // If invmodulus is at most 64 bits, we can use multiply_uint_uint64, which is faster
                function<void()> multiply_shifted_invmodulus;
                if (modulus.inverse_significant_bit_count() <= bits_per_uint64)
                {
                    multiply_shifted_invmodulus = [&]() {
                        multiply_uint_uint64(shifted_ptr, uint64_count, *invmodulus, uint64_count, product_ptr);
                    };
                }
                else
                {
                    multiply_shifted_invmodulus = [&]() {
                        multiply_uint_uint(shifted_ptr, uint64_count, invmodulus, modulus_uint64_count, uint64_count, product_ptr);
                    };
                }

                while (value_bits >= modulus_bits + 1)
                {
                    right_shift_uint(value, modulus_bits, uint64_count, shifted_ptr);
                    filter_highbits_uint(value, uint64_count, modulus_bits);

                    multiply_shifted_invmodulus();

                    add_uint_uint(value, product_ptr, uint64_count, value);
                    value_bits = get_significant_bit_count_uint(value, uint64_count);
                }

                // Use subtraction for few remaining iterations.
                if (is_greater_than_or_equal_uint_uint(value, uint64_count, modulus.get(), modulus_uint64_count))
                {
                    sub_uint_uint(value, uint64_count, modulus.get(), modulus_uint64_count, false, uint64_count, value);
                }
                return;
            }

            // Create temporary space to store mutable copy of modulus.

            // Set shifted_modulus_ptr to point to either an existing allocation given as parameter,
            // or else to a new allocation from the memory pool.
            Pointer shifted_modulus;
            uint64_t *shifted_modulus_ptr = alloc_ptr;
            if (alloc_ptr == nullptr)
            {
                shifted_modulus = allocate_uint(uint64_count, pool);
                shifted_modulus_ptr = shifted_modulus.get();
            }
            else
            {
                alloc_ptr += uint64_count;
            }
            set_uint_uint(modulusptr, modulus_uint64_count, uint64_count, shifted_modulus_ptr);

            // Create temporary space to store difference calculation.
            // Set difference_ptr to point to either an existing allocation given as parameter,
            // or else to a new allocation from the memory pool.
            Pointer difference;
            uint64_t *difference_ptr = alloc_ptr;
            if (alloc_ptr == nullptr)
            {
                difference = allocate_uint(uint64_count, pool);
                difference_ptr = difference.get();
            }

            // Shift modulus to bring MSB in alignment with MSB of value.
            int modulus_shift = value_bits - modulus_bits;
            left_shift_uint(shifted_modulus_ptr, modulus_shift, uint64_count, shifted_modulus_ptr);
            modulus_bits += modulus_shift;

            // Perform bit-wise division algorithm.
            int remaining_shifts = modulus_shift;
            while (value_bits == modulus_bits)
            {
                // NOTE: MSBs of value and shifted modulus are aligned.

                // Even though MSB of value and modulus are aligned, still possible value < shifted_modulus.
                if (sub_uint_uint(value, shifted_modulus_ptr, uint64_count, difference_ptr))
                {
                    // value < shifted_modulus, so current quotient bit is zero and next one is definitely one.
                    if (remaining_shifts == 0)
                    {
                        // No shifts remain and value < modulus so done.
                        break;
                    }

                    // Effectively shift value left by 1 by instead adding value to difference (to prevent overflow in value).
                    add_uint_uint(difference_ptr, value, uint64_count, difference_ptr);

                    // Adjust remaining shifts as a result of shifting value.
                    remaining_shifts--;
                }
                // Difference is the new value with modulus subtracted.

                // Determine amount to shift value to bring MSB in alignment with modulus.
                value_bits = get_significant_bit_count_uint(difference_ptr, uint64_count);
                int value_shift = modulus_bits - value_bits;
                if (value_shift > remaining_shifts)
                {
                    // Clip the maximum shift to determine only the integer (as opposed to fractional) bits.
                    value_shift = remaining_shifts;
                }

                // Shift and update value.
                if (value_bits > 0)
                {
                    left_shift_uint(difference_ptr, value_shift, uint64_count, value);
                    value_bits += value_shift;
                }
                else
                {
                    // Value is zero so no need to shift, just set to zero.
                    set_zero_uint(uint64_count, value);
                }

                // Adjust remaining shifts as a result of shifting value.
                remaining_shifts -= value_shift;
            }

            // Correct value (which is also the remainder) for shifting of modulus.
            right_shift_uint(value, modulus_shift, uint64_count, value);
        }

        void modulo_uint(const std::uint64_t *value, int value_uint64_count, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool, uint64_t *alloc_ptr)
        {
            // alloc_ptr should point to 3 x value_uint64_count memory

#ifdef _DEBUG
            if (value == nullptr && value_uint64_count > 0)
            {
                throw invalid_argument("value");
            }
            if (value_uint64_count < 0)
            {
                throw invalid_argument("value_uint64_count");
            }
            if (result == nullptr && value_uint64_count > 0)
            {
                throw invalid_argument("result");
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

            // Set value_copy to point to either an existing allocation given as parameter,
            // or else to a new allocation from the memory pool.
            Pointer value_copy_anchor;
            uint64_t *value_copy = alloc_ptr;
            if (alloc_ptr == nullptr)
            {
                value_copy_anchor = allocate_uint(value_uint64_count, pool);
                value_copy = value_copy_anchor.get();
            }
            else
            {
                alloc_ptr += value_uint64_count;
            }
            set_uint_uint(value, value_uint64_count, value_copy);

            modulo_uint_inplace(value_copy, value_uint64_count, modulus, pool, alloc_ptr);
            set_uint_uint(value_copy, value_uint64_count, modulus.uint64_count(), result);
        }

        void increment_uint_mod(const uint64_t *operand, const uint64_t *modulus, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand == nullptr)
            {
                throw invalid_argument("operand");
            }
            if (modulus == nullptr)
            {
                throw invalid_argument("modulus");
            }
            if (uint64_count <= 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (is_greater_than_or_equal_uint_uint(operand, modulus, uint64_count))
            {
                throw out_of_range("operand");
            }
            if (modulus == result)
            {
                throw invalid_argument("result cannot point to the same value as modulus");
            }
#endif
            unsigned char carry = increment_uint(operand, uint64_count, result);
            if (carry || is_greater_than_or_equal_uint_uint(result, modulus, uint64_count))
            {
                sub_uint_uint(result, modulus, uint64_count, result);
            }
        }

        void decrement_uint_mod(const uint64_t *operand, const uint64_t *modulus, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand == nullptr)
            {
                throw invalid_argument("operand");
            }
            if (modulus == nullptr)
            {
                throw invalid_argument("modulus");
            }
            if (uint64_count <= 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (is_greater_than_or_equal_uint_uint(operand, modulus, uint64_count))
            {
                throw out_of_range("operand");
            }
            if (modulus == result)
            {
                throw invalid_argument("result cannot point to the same value as modulus");
            }
#endif
            if (decrement_uint(operand, uint64_count, result))
            {
                add_uint_uint(result, modulus, uint64_count, result);
            }
        }

        void negate_uint_mod(const uint64_t *operand, const uint64_t *modulus, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand == nullptr)
            {
                throw invalid_argument("operand");
            }
            if (modulus == nullptr)
            {
                throw invalid_argument("modulus");
            }
            if (uint64_count <= 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (is_greater_than_or_equal_uint_uint(operand, modulus, uint64_count))
            {
                throw out_of_range("operand");
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

        void div2_uint_mod(const uint64_t *operand, const uint64_t *modulus, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand == nullptr)
            {
                throw invalid_argument("operand");
            }
            if (modulus == nullptr)
            {
                throw invalid_argument("modulus");
            }
            if (uint64_count <= 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (!is_bit_set_uint(modulus, uint64_count, 0))
            {
                throw invalid_argument("modulus");
            }
            //if (is_greater_than_or_equal_uint_uint(operand, modulus, uint64_count))
            //{
            //    throw out_of_range("operand");
            //}
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

        void add_uint_uint_mod(const uint64_t *operand1, const uint64_t *operand2, const uint64_t *modulus, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand1 == nullptr)
            {
                throw invalid_argument("operand1");
            }
            if (operand2 == nullptr)
            {
                throw invalid_argument("operand2");
            }
            if (modulus == nullptr)
            {
                throw invalid_argument("modulus");
            }
            if (uint64_count <= 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (is_greater_than_or_equal_uint_uint(operand1, modulus, uint64_count))
            {
                throw out_of_range("operand1");
            }
            if (is_greater_than_or_equal_uint_uint(operand2, modulus, uint64_count))
            {
                throw out_of_range("operand2");
            }
            if (modulus == result)
            {
                throw invalid_argument("result cannot point to the same value as modulus");
            }
#endif
            unsigned char carry = add_uint_uint(operand1, operand2, uint64_count, result);
            if (carry || is_greater_than_or_equal_uint_uint(result, modulus, uint64_count))
            {
                sub_uint_uint(result, modulus, uint64_count, result);
            }
        }

        void sub_uint_uint_mod(const uint64_t *operand1, const uint64_t *operand2, const uint64_t *modulus, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand1 == nullptr)
            {
                throw invalid_argument("operand1");
            }
            if (operand2 == nullptr)
            {
                throw invalid_argument("operand2");
            }
            if (modulus == nullptr)
            {
                throw invalid_argument("modulus");
            }
            if (uint64_count <= 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (is_greater_than_or_equal_uint_uint(operand1, modulus, uint64_count))
            {
                throw out_of_range("operand1");
            }
            if (is_greater_than_or_equal_uint_uint(operand2, modulus, uint64_count))
            {
                throw out_of_range("operand2");
            }
            if (modulus == result)
            {
                throw invalid_argument("result cannot point to the same value as modulus");
            }
#endif
            if (sub_uint_uint(operand1, operand2, uint64_count, result))
            {
                add_uint_uint(result, modulus, uint64_count, result);
            }
        }

        void multiply_uint_uint_mod(const uint64_t *operand1, const uint64_t *operand2, const Modulus &modulus, uint64_t *result, MemoryPool &pool, uint64_t *alloc_ptr)
        {
            // alloc_ptr should point to 4 x modulus.uint64_count() memory

#ifdef _DEBUG
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
            if (is_greater_than_or_equal_uint_uint(operand1, modulus.get(), modulus.uint64_count()))
            {
                throw invalid_argument("operand1");
            }
            if (is_greater_than_or_equal_uint_uint(operand2, modulus.get(), modulus.uint64_count()))
            {
                throw invalid_argument("operand2");
            }
            if (operand1 == result || operand2 == result || modulus.get() == result)
            {
                throw invalid_argument("result cannot point to the same value as operand1, operand2, or modulus");
            }
#endif
            // Calculate normal product.
            int uint64_count = modulus.uint64_count();
            int intermediate_uint64_count = uint64_count * 2;
            Pointer intermediate(allocate_uint(intermediate_uint64_count, pool));
            multiply_uint_uint(operand1, operand2, uint64_count, intermediate.get());

            // Perform modulo operation.
            modulo_uint_inplace(intermediate.get(), intermediate_uint64_count, modulus, pool, alloc_ptr);

            // Copy to result.
            set_uint_uint(intermediate.get(), uint64_count, result);
        }

        void multiply_uint_uint_mod_inplace(const uint64_t *operand1, const uint64_t *operand2, const Modulus &modulus, uint64_t *result, MemoryPool &pool, uint64_t *alloc_ptr)
        {
            // alloc_ptr should point to 4 x modulus.uint64_count() memory

#ifdef _DEBUG
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
            if (is_greater_than_or_equal_uint_uint(operand1, modulus.get(), modulus.uint64_count()))
            {
                throw out_of_range("operand1");
            }
            if (is_greater_than_or_equal_uint_uint(operand2, modulus.get(), modulus.uint64_count()))
            {
                throw out_of_range("operand2");
            }
            if (operand1 == result || operand2 == result || modulus.get() == result)
            {
                throw invalid_argument("result cannot point to the same value as operand1, operand2, or modulus");
            }
#endif
            // Calculate normal product.
            int uint64_count = modulus.uint64_count();
            int result_uint64_count = uint64_count * 2;
            multiply_uint_uint(operand1, operand2, uint64_count, result);

            // Perform modulo operation.
            modulo_uint_inplace(result, result_uint64_count, modulus, pool, alloc_ptr);
        }

        bool try_invert_uint_mod(const uint64_t *operand, const uint64_t *modulus, int uint64_count, uint64_t *result, MemoryPool &pool, uint64_t *alloc_ptr)
        {
            // alloc_ptr should point to 7 x uint64_count memory

#ifdef _DEBUG
            if (operand == nullptr)
            {
                throw invalid_argument("operand");
            }
            if (modulus == nullptr)
            {
                throw invalid_argument("modulus");
            }
            if (uint64_count <= 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (is_greater_than_or_equal_uint_uint(operand, modulus, uint64_count))
            {
                throw out_of_range("operand");
            }
#endif
            // Cannot invert 0.
            int bit_count = get_significant_bit_count_uint(operand, uint64_count);
            if (bit_count == 0)
            {
                return false;
            }

            // If it is 1, then its invert is itself.
            if (bit_count == 1)
            {
                set_uint(1, uint64_count, result);
                return true;
            }

            // Construct a mutable copy of operand and modulus, with numerator being modulus
            // and operand being denominator. Notice that numerator > denominator.

            // Set numerator to point to either an existing allocation given as parameter,
            // or else to a new allocation from the memory pool.
            Pointer numerator_anchor;
            uint64_t *numerator = alloc_ptr;
            if (alloc_ptr == nullptr)
            {
                numerator_anchor = allocate_uint(uint64_count, pool);
                numerator = numerator_anchor.get();
            }
            else
            {
                alloc_ptr += uint64_count;
            }
            set_uint_uint(modulus, uint64_count, numerator);
            
            // Set denominator to point to either an existing allocation given as parameter,
            // or else to a new allocation from the memory pool.
            Pointer denominator_anchor;
            uint64_t *denominator = alloc_ptr;
            if (alloc_ptr == nullptr)
            {
                denominator_anchor = allocate_uint(uint64_count, pool);
                denominator = denominator_anchor.get();
            }
            else
            {
                alloc_ptr += uint64_count;
            }
            set_uint_uint(operand, uint64_count, denominator);

            // Create space to store difference.
            // Set difference to point to either an existing allocation given as parameter,
            // or else to a new allocation from the memory pool.
            Pointer difference_alloc;
            uint64_t *difference = alloc_ptr;
            if (alloc_ptr == nullptr)
            {
                difference_alloc = allocate_uint(uint64_count, pool);
                difference = difference_alloc.get();
            }
            else
            {
                alloc_ptr += uint64_count;
            }

            // Determine highest bit index of each.
            int numerator_bits = get_significant_bit_count_uint(numerator, uint64_count);
            int denominator_bits = get_significant_bit_count_uint(denominator, uint64_count);

            // Create space to store quotient.
            // Set quotient to point to either an existing allocation given as parameter,
            // or else to a new allocation from the memory pool.
            Pointer quotient_alloc;
            uint64_t *quotient = alloc_ptr;
            if (alloc_ptr == nullptr)
            {
                quotient_alloc = allocate_uint(uint64_count, pool);
                quotient = quotient_alloc.get();
            }
            else
            {
                alloc_ptr += uint64_count;
            }
            // Create three sign/magnitude values to store coefficients.
            // Initialize invert_prior to +0 and invert_curr to +1.
            // Set invert_prior to point to either an existing allocation given as parameter,
            // or else to a new allocation from the memory pool.
            Pointer invert_prior_anchor;
            uint64_t *invert_prior = alloc_ptr;
            if (alloc_ptr == nullptr)
            {
                invert_prior_anchor = allocate_uint(uint64_count, pool);
                invert_prior = invert_prior_anchor.get();
            }
            else
            {
                alloc_ptr += uint64_count;
            }
            set_zero_uint(uint64_count, invert_prior);
            bool invert_prior_positive = true;

            // Set invert_curr to point to either an existing allocation given as parameter,
            // or else to a new allocation from the memory pool.
            Pointer invert_curr_anchor;
            uint64_t *invert_curr = alloc_ptr;
            if (alloc_ptr == nullptr)
            {
                invert_curr_anchor = allocate_uint(uint64_count, pool);
                invert_curr = invert_curr_anchor.get();
            }
            else
            {
                alloc_ptr += uint64_count;
            }
            set_uint(1, uint64_count, invert_curr);
            bool invert_curr_positive = true;

            // Set invert_next to point to either an existing allocation given as parameter,
            // or else to a new allocation from the memory pool.
            Pointer invert_next_anchor;
            uint64_t *invert_next = alloc_ptr;
            if (alloc_ptr == nullptr)
            {
                invert_next_anchor = allocate_uint(uint64_count, pool);
                invert_next = invert_next_anchor.get();
            }
            bool invert_next_positive = true;

            // Perform extended Euclidean algorithm.
            while (true)
            {
                // NOTE: Numerator is > denominator.

                // Only perform computation up to last non-zero uint64s.
                int division_uint64_count = divide_round_up(numerator_bits, bits_per_uint64);

                // Shift denominator to bring MSB in alignment with MSB of numerator.
                int denominator_shift = numerator_bits - denominator_bits;
                left_shift_uint(denominator, denominator_shift, division_uint64_count, denominator);
                denominator_bits += denominator_shift;

                // Clear quotient.
                set_zero_uint(uint64_count, quotient);

                // Perform bit-wise division algorithm.
                int remaining_shifts = denominator_shift;
                while (numerator_bits == denominator_bits)
                {
                    // NOTE: MSBs of numerator and denominator are aligned.

                    // Even though MSB of numerator and denominator are aligned, still possible numerator < denominator.
                    if (sub_uint_uint(numerator, denominator, division_uint64_count, difference))
                    {
                        // numerator < denominator and MSBs are aligned, so current quotient bit is zero and next one is definitely one.
                        if (remaining_shifts == 0)
                        {
                            // No shifts remain and numerator < denominator so done.
                            break;
                        }

                        // Effectively shift numerator left by 1 by instead adding numerator to difference (to prevent overflow in numerator).
                        add_uint_uint(difference, numerator, division_uint64_count, difference);

                        // Adjust quotient and remaining shifts as a result of shifting numerator.
                        left_shift_uint(quotient, 1, division_uint64_count, quotient);
                        remaining_shifts--;
                    }
                    // Difference is the new numerator with denominator subtracted.

                    // Update quotient to reflect subtraction.
                    *quotient |= 1;

                    // Determine amount to shift numerator to bring MSB in alignment with denominator.
                    numerator_bits = get_significant_bit_count_uint(difference, division_uint64_count);
                    int numerator_shift = denominator_bits - numerator_bits;
                    if (numerator_shift > remaining_shifts)
                    {
                        // Clip the maximum shift to determine only the integer (as opposed to fractional) bits.
                        numerator_shift = remaining_shifts;
                    }

                    // Shift and update numerator.
                    if (numerator_bits > 0)
                    {
                        left_shift_uint(difference, numerator_shift, division_uint64_count, numerator);
                        numerator_bits += numerator_shift;
                    }
                    else
                    {
                        // Difference is zero so no need to shift, just set to zero.
                        set_zero_uint(division_uint64_count, numerator);
                    }

                    // Adjust quotient and remaining shifts as a result of shifting numerator.
                    left_shift_uint(quotient, numerator_shift, division_uint64_count, quotient);
                    remaining_shifts -= numerator_shift;
                }

                // Correct for shifting of denominator.
                right_shift_uint(denominator, denominator_shift, division_uint64_count, denominator);
                denominator_bits -= denominator_shift;

                // We are done if remainder (which is stored in numerator) is zero.
                if (numerator_bits == 0)
                {
                    break;
                }

                // Correct for shifting of denominator.
                right_shift_uint(numerator, denominator_shift, division_uint64_count, numerator);
                numerator_bits -= denominator_shift;

                // Integrate quotient with invert coefficients.
                // Calculate: invert_prior + -quotient * invert_curr
                multiply_truncate_uint_uint(quotient, invert_curr, uint64_count, invert_next);
                invert_next_positive = !invert_curr_positive;
                if (invert_prior_positive == invert_next_positive)
                {
                    // If both sides of add have same sign, then simple add and do not need to worry about overflow
                    // due to known limits on the coefficients proved in the euclidean algorithm.
                    add_uint_uint(invert_prior, invert_next, uint64_count, invert_next);
                }
                else
                {
                    // If both sides of add have opposite sign, then subtract and check for overflow.
                    uint64_t borrow = sub_uint_uint(invert_prior, invert_next, uint64_count, invert_next);
                    if (borrow == 0)
                    {
                        // No borrow means |invert_prior| >= |invert_next|, so sign is same as invert_prior.
                        invert_next_positive = invert_prior_positive;
                    }
                    else
                    {
                        // Borrow means |invert prior| < |invert_next|, so sign is opposite of invert_prior.
                        invert_next_positive = !invert_prior_positive;
                        negate_uint(invert_next, uint64_count, invert_next);
                    }
                }

                // Swap prior and curr, and then curr and next.
                swap(invert_prior, invert_curr);
                swap(invert_prior_positive, invert_curr_positive);
                swap(invert_curr, invert_next);
                swap(invert_curr_positive, invert_next_positive);

                // Swap numerator and denominator using pointer swings.
                swap(numerator, denominator);
                swap(numerator_bits, denominator_bits);
            }

            if (!is_equal_uint(denominator, uint64_count, 1))
            {
                // GCD is not one, so unable to find inverse.
                return false;
            }

            // Correct coefficient if negative by modulo.
            if (!invert_curr_positive && !is_zero_uint(invert_curr, uint64_count))
            {
                sub_uint_uint(modulus, invert_curr, uint64_count, invert_curr);
                invert_curr_positive = true;
            }

            // Set result.
            set_uint_uint(invert_curr, uint64_count, result);
            return true;
        }

        bool is_primitive_root(const uint64_t *root, uint64_t degree, const Modulus &prime_modulus, MemoryPool &pool)
        {
            int uint64_count = prime_modulus.uint64_count();
#ifdef _DEBUG
            if (root == nullptr)
            {
                throw invalid_argument("root");
            }
            if (prime_modulus.significant_bit_count() < 2)
            {
                throw invalid_argument("modulus");
            }
            if (is_greater_than_or_equal_uint_uint(root, prime_modulus.get(), uint64_count))
            {
                throw out_of_range("operand");
            }
            if (get_power_of_two(degree) < 1)
            {
                throw invalid_argument("degree must be a power of two and at least two");
            }
#endif
            if (is_zero_uint(root, uint64_count))
            {
                return false;
            }

            // We check if root is a degree-th root of unity in integers modulo modulus, where degree is a power of two.
            // It suffices to check that root^(degree/2) is -1 modulo modulus.
            Pointer power(allocate_uint(uint64_count, pool));
            degree >>= 1;
            exponentiate_uint_mod(root, &degree, 1, prime_modulus, power.get(), pool);
            increment_uint_mod(power.get(), prime_modulus.get(), uint64_count, power.get());

            return is_zero_uint(power.get(), uint64_count);
        }

        bool try_primitive_root(uint64_t degree, const Modulus &prime_modulus, MemoryPool &pool, uint64_t *destination)
        {
            int uint64_count = prime_modulus.uint64_count();
#ifdef _DEBUG
            if (destination == nullptr)
            {
                throw invalid_argument("destination");
            }
            if (prime_modulus.significant_bit_count() < 2)
            {
                throw invalid_argument("modulus");
            }
            if (get_power_of_two(degree) < 1)
            {
                throw invalid_argument("degree must be a power of two and at least two");
            }
#endif
            // We need to divide modulus-1 by degree to get the size of the quotient group
            Pointer size_entire_group(allocate_uint(uint64_count, pool));
            decrement_uint(prime_modulus.get(), uint64_count, size_entire_group.get());

            Pointer divisor(allocate_uint(uint64_count, pool));
            set_uint(degree, uint64_count, divisor.get());

            // Compute size of quotient group
            Pointer size_quotient_group(allocate_uint(uint64_count, pool));
            divide_uint_uint_inplace(size_entire_group.get(), divisor.get(), uint64_count, size_quotient_group.get(), pool);

            // size_entire_group must be zero now, or otherwise the primitive root does not exist in integers modulo modulus
            if (!is_zero_uint(size_entire_group.get(), uint64_count))
            {
                return false;
            }

            // For randomness
            random_device rd;

            int attempt_counter = 0;
            int attempt_counter_max = 100;
            do
            {
                attempt_counter++;

                // Set destination to be a random number modulo modulus
                uint32_t *random_uint32_ptr = reinterpret_cast<uint32_t*>(destination);
                for (int i = 0; i < 2 * uint64_count; ++i)
                {
                    *random_uint32_ptr++ = rd();
                }
                modulo_uint_inplace(destination, uint64_count, prime_modulus, pool);

                // Raise the random number to power the size of the quotient to get rid of irrelevant part
                exponentiate_uint_mod(destination, size_quotient_group.get(), uint64_count, prime_modulus, destination, pool);
            }
            while(!is_primitive_root(destination, degree, prime_modulus, pool) && attempt_counter < attempt_counter_max);

            return is_primitive_root(destination, degree, prime_modulus, pool);
        }

        bool try_minimal_primitive_root(uint64_t degree, const Modulus &prime_modulus, MemoryPool &pool, std::uint64_t *destination)
        {
            if (!try_primitive_root(degree, prime_modulus, pool, destination))
            {
                return false;
            }

            int uint64_count = prime_modulus.uint64_count();

            Pointer generator_sq(allocate_uint(uint64_count, pool));
            multiply_uint_uint_mod(destination, destination, prime_modulus, generator_sq.get(), pool);

            Pointer current_generator(allocate_uint(uint64_count, pool));
            set_uint_uint(destination, uint64_count, current_generator.get());

            // destination is going to always contain the smallest generator found
            set_uint_uint(current_generator.get(), uint64_count, destination);

            for (size_t i = 0; i < degree; ++i)
            {
                // If our current generator is strictly smaller than destination, update
                if (compare_uint_uint(current_generator.get(), destination, uint64_count) == -1)
                {
                    set_uint_uint(current_generator.get(), uint64_count, destination);
                }

                // Then move on to the next generator
                ConstPointer current_generator_copy(duplicate_uint_if_needed(current_generator.get(), uint64_count, uint64_count, true, pool));
                multiply_uint_uint_mod(current_generator_copy.get(), generator_sq.get(), prime_modulus, current_generator.get(), pool);
            }

            return true;
        }
    }
}
