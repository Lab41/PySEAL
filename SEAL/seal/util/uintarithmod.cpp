#include "seal/util/uintcore.h"
#include "seal/util/uintarith.h"
#include "seal/util/uintarithmod.h"
#include "seal/util/common.h"
#include <random>

using namespace std;

namespace seal
{
    namespace util
    {
        void modulo_uint_inplace(uint64_t *value, int value_uint64_count, const Modulus &modulus, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
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

            Pointer shifted(allocate_uint(uint64_count, pool));

            // Handle fast case modulo is power of 2 minus one.
            int modulo_power_min_one = modulus.power_of_two_minus_one();
            if (modulo_power_min_one >= 2)
            {
                // Set shifted_ptr to point to either an existing allocation given as parameter,
                // or else to a new allocation from the memory pool.

                while (value_bits >= modulus_bits + 1)
                {
                    right_shift_uint(value, modulo_power_min_one, uint64_count, shifted.get());
                    filter_highbits_uint(value, uint64_count, modulo_power_min_one);
                    add_uint_uint(value, shifted.get(), uint64_count, value);
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
                Pointer product(allocate_uint(uint64_count, pool));

                // If invmodulus is at most 64 bits, we can use multiply_uint_uint64, which is faster
                function<void()> multiply_shifted_invmodulus;
                if (modulus.inverse_significant_bit_count() <= bits_per_uint64)
                {
                    multiply_shifted_invmodulus = [&]() {
                        multiply_uint_uint64(shifted.get(), uint64_count, *invmodulus, uint64_count, product.get());
                    };
                }
                else
                {
                    multiply_shifted_invmodulus = [&]() {
                        multiply_uint_uint(shifted.get(), uint64_count, invmodulus, modulus_uint64_count, uint64_count, product.get());
                    };
                }

                while (value_bits >= modulus_bits + 1)
                {
                    right_shift_uint(value, modulus_bits, uint64_count, shifted.get());
                    filter_highbits_uint(value, uint64_count, modulus_bits);

                    multiply_shifted_invmodulus();

                    add_uint_uint(value, product.get(), uint64_count, value);
                    value_bits = get_significant_bit_count_uint(value, uint64_count);
                }

                // Use subtraction for few remaining iterations.
                if (is_greater_than_or_equal_uint_uint(value, uint64_count, modulus.get(), modulus_uint64_count))
                {
                    sub_uint_uint(value, uint64_count, modulus.get(), modulus_uint64_count, false, uint64_count, value);
                }
                return;
            }

            // Store mutable copy of modulus.
            set_uint_uint(modulusptr, modulus_uint64_count, uint64_count, shifted.get());

            // Create temporary space to store difference calculation.
            Pointer difference(allocate_uint(uint64_count, pool));

            // Shift modulus to bring MSB in alignment with MSB of value.
            int modulus_shift = value_bits - modulus_bits;
            left_shift_uint(shifted.get(), modulus_shift, uint64_count, shifted.get());
            modulus_bits += modulus_shift;

            // Perform bit-wise division algorithm.
            int remaining_shifts = modulus_shift;
            while (value_bits == modulus_bits)
            {
                // NOTE: MSBs of value and shifted modulus are aligned.

                // Even though MSB of value and modulus are aligned, still possible value < shifted_modulus.
                if (sub_uint_uint(value, shifted.get(), uint64_count, difference.get()))
                {
                    // value < shifted_modulus, so current quotient bit is zero and next one is definitely one.
                    if (remaining_shifts == 0)
                    {
                        // No shifts remain and value < modulus so done.
                        break;
                    }

                    // Effectively shift value left by 1 by instead adding value to difference (to prevent overflow in value).
                    add_uint_uint(difference.get(), value, uint64_count, difference.get());

                    // Adjust remaining shifts as a result of shifting value.
                    remaining_shifts--;
                }
                // Difference is the new value with modulus subtracted.

                // Determine amount to shift value to bring MSB in alignment with modulus.
                value_bits = get_significant_bit_count_uint(difference.get(), uint64_count);
                int value_shift = modulus_bits - value_bits;
                if (value_shift > remaining_shifts)
                {
                    // Clip the maximum shift to determine only the integer (as opposed to fractional) bits.
                    value_shift = remaining_shifts;
                }

                // Shift and update value.
                if (value_bits > 0)
                {
                    left_shift_uint(difference.get(), value_shift, uint64_count, value);
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

        bool try_invert_uint_mod(const uint64_t *operand, const uint64_t *modulus, int uint64_count, uint64_t *result, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
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

            Pointer alloc_anchor(allocate_uint(7 * uint64_count, pool));

            // Construct a mutable copy of operand and modulus, with numerator being modulus
            // and operand being denominator. Notice that numerator > denominator.
            uint64_t *numerator = alloc_anchor.get();
            set_uint_uint(modulus, uint64_count, numerator);
            
            uint64_t *denominator = numerator + uint64_count;
            set_uint_uint(operand, uint64_count, denominator);

            // Create space to store difference.
            uint64_t *difference = denominator + uint64_count;

            // Determine highest bit index of each.
            int numerator_bits = get_significant_bit_count_uint(numerator, uint64_count);
            int denominator_bits = get_significant_bit_count_uint(denominator, uint64_count);

            // Create space to store quotient.
            uint64_t *quotient = difference + uint64_count;

            // Create three sign/magnitude values to store coefficients.
            // Initialize invert_prior to +0 and invert_curr to +1.
            uint64_t *invert_prior = quotient + uint64_count;
            set_zero_uint(uint64_count, invert_prior);
            bool invert_prior_positive = true;

            uint64_t *invert_curr = invert_prior + uint64_count;
            set_uint(1, uint64_count, invert_curr);
            bool invert_curr_positive = true;

            uint64_t *invert_next = invert_curr + uint64_count;
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
#ifdef SEAL_DEBUG
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

        bool try_primitive_root(uint64_t degree, const Modulus &prime_modulus, uint64_t *destination, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
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
            int uint64_count = prime_modulus.uint64_count();

            // We need to divide modulus-1 by degree to get the size of the quotient group
            Pointer alloc_anchor(allocate_uint(3 * uint64_count, pool));
            uint64_t *size_entire_group = alloc_anchor.get();
            decrement_uint(prime_modulus.get(), uint64_count, size_entire_group);

            uint64_t *divisor = size_entire_group + uint64_count;
            set_uint(degree, uint64_count, divisor);

            // Compute size of quotient group
            uint64_t *size_quotient_group = divisor + uint64_count;
            divide_uint_uint_inplace(size_entire_group, divisor, uint64_count, size_quotient_group, pool);

            // size_entire_group must be zero now, or otherwise the primitive root does not exist in integers modulo modulus
            if (!is_zero_uint(size_entire_group, uint64_count))
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
                for (int i = 0; i < 2 * uint64_count; i++)
                {
                    *random_uint32_ptr++ = rd();
                }
                modulo_uint_inplace(destination, uint64_count, prime_modulus, pool);

                // Raise the random number to power the size of the quotient to get rid of irrelevant part
                exponentiate_uint_mod(destination, size_quotient_group, uint64_count, prime_modulus, destination, pool);
            }
            while(!is_primitive_root(destination, degree, prime_modulus, pool) && attempt_counter < attempt_counter_max);

            return is_primitive_root(destination, degree, prime_modulus, pool);
        }

        bool try_minimal_primitive_root(uint64_t degree, const Modulus &prime_modulus, uint64_t *destination, MemoryPool &pool)
        {
            if (!try_primitive_root(degree, prime_modulus, destination, pool))
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

            for (size_t i = 0; i < degree; i++)
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

        void exponentiate_uint_mod(const uint64_t *operand, const uint64_t *exponent, int exponent_uint64_count, const Modulus &modulus, uint64_t *result, MemoryPool &pool)
        {
            int modulus_uint64_count = modulus.uint64_count();

#ifdef SEAL_DEBUG
            if (operand == nullptr)
            {
                throw invalid_argument("operand");
            }
            if (exponent == nullptr)
            {
                throw invalid_argument("exponent");
            }
            if (exponent_uint64_count <= 0)
            {
                throw invalid_argument("exponent_uint64_count");
            }
            if (is_zero_uint(modulus.get(), modulus_uint64_count))
            {
                throw invalid_argument("modulus");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
#endif

            // Fast cases
            if (is_zero_uint(exponent, exponent_uint64_count))
            {
                set_uint(1, modulus_uint64_count, result);
                return;
            }

            modulo_uint(operand, modulus_uint64_count, modulus, result, pool);
            if (is_equal_uint(exponent, exponent_uint64_count, 1))
            {
                return;
            }

            Pointer alloc_anchor(allocate_uint(exponent_uint64_count + 3 * modulus_uint64_count, pool));

            // Need to make a copy of exponent
            uint64_t *exponent_copy = alloc_anchor.get();
            set_uint_uint(exponent, exponent_uint64_count, exponent_copy);

            // Perform binary exponentiation.
            uint64_t *powerptr = exponent_copy + exponent_uint64_count;
            uint64_t *productptr = powerptr + modulus_uint64_count;
            uint64_t *intermediateptr = productptr + modulus_uint64_count;

            set_uint_uint(result, modulus_uint64_count, modulus_uint64_count, powerptr);
            set_uint(1, modulus_uint64_count, intermediateptr);

            // Initially: power = operand and intermediate = 1, product is not initialized.
            while (true)
            {
                if ((*exponent_copy % 2) == 1)
                {
                    multiply_uint_uint_mod(powerptr, intermediateptr, modulus, productptr, pool);
                    swap(productptr, intermediateptr);
                }
                right_shift_uint(exponent_copy, 1, exponent_uint64_count, exponent_copy);
                if (is_zero_uint(exponent_copy, exponent_uint64_count))
                {
                    break;
                }
                multiply_uint_uint_mod(powerptr, powerptr, modulus, productptr, pool);
                swap(productptr, powerptr);
            }
            set_uint_uint(intermediateptr, modulus_uint64_count, result);
        }
    }
}
