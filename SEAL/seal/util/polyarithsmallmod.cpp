#include "seal/util/uintcore.h"
#include "seal/util/uintarithsmallmod.h"
#include "seal/util/uintarith.h"
#include "seal/util/polycore.h"
#include "seal/util/polyarith.h"
#include "seal/util/polyarithsmallmod.h"
#include "seal/util/polyfftmultsmallmod.h"
#include "seal/util/defines.h"

using namespace std;

namespace seal
{
    namespace util
    {
        void multiply_poly_poly_coeffmod(const uint64_t *operand1, int operand1_coeff_count, const uint64_t *operand2, int operand2_coeff_count,
            const SmallModulus &modulus, int result_coeff_count, uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr && operand1_coeff_count > 0)
            {
                throw invalid_argument("operand1");
            }
            if (operand1_coeff_count < 0)
            {
                throw invalid_argument("operand1_coeff_count");
            }
            if (operand2 == nullptr && operand2_coeff_count > 0)
            {
                throw invalid_argument("operand2");
            }
            if (operand2_coeff_count < 0)
            {
                throw invalid_argument("operand2_coeff_count");
            }
            if (result_coeff_count < 0)
            {
                throw invalid_argument("result_coeff_count");
            }
            if (result == nullptr && result_coeff_count > 0)
            {
                throw invalid_argument("result");
            }
            if (result != nullptr && (operand1 == result || operand2 == result))
            {
                throw invalid_argument("result cannot point to the same value as operand1, operand2, or modulus");
            }
            if (modulus.is_zero())
            {
                throw invalid_argument("modulus");
            }
#endif
            // Clear product.
            set_zero_uint(result_coeff_count, result);

            operand1_coeff_count = get_significant_coeff_count_poly(operand1, operand1_coeff_count, 1);
            operand2_coeff_count = get_significant_coeff_count_poly(operand2, operand2_coeff_count, 1);
            for (int operand1_index = 0; operand1_index < operand1_coeff_count; operand1_index++)
            {
                if (operand1[operand1_index] == 0)
                {
                    // If coefficient is 0, then move on to next coefficient.
                    continue;
                }
                // Do expensive add
                for (int operand2_index = 0; operand2_index < operand2_coeff_count; operand2_index++)
                {
                    int product_coeff_index = operand1_index + operand2_index;
                    if (product_coeff_index >= result_coeff_count)
                    {
                        break;
                    }

                    if (operand2[operand2_index] == 0)
                    {
                        // If coefficient is 0, then move on to next coefficient.
                        continue;
                    }

                    // Lazy reduction
                    uint64_t temp[2];
                    multiply_uint64(operand1[operand1_index], operand2[operand2_index], temp);
                    temp[1] += add_uint64(temp[0], result[product_coeff_index], 0, temp);
                    result[product_coeff_index] = barrett_reduce_128(temp, modulus);
                }
            }
        }

        void multiply_poly_poly_coeffmod(const uint64_t *operand1, const uint64_t *operand2, int coeff_count, const SmallModulus &modulus, uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr && coeff_count > 0)
            {
                throw invalid_argument("operand1");
            }
            if (operand2 == nullptr && coeff_count > 0)
            {
                throw invalid_argument("operand2");
            }
            if (coeff_count < 0)
            {
                throw invalid_argument("coeff_count");
            }
            if (result == nullptr && coeff_count > 0)
            {
                throw invalid_argument("result");
            }
            if (result != nullptr && (operand1 == result || operand2 == result))
            {
                throw invalid_argument("result cannot point to the same value as operand1, operand2, or modulus");
            }
            if (modulus.is_zero())
            {
                throw invalid_argument("modulus");
            }
#endif
            int result_coeff_count = coeff_count + coeff_count - 1;

            // Clear product.
            set_zero_uint(result_coeff_count, result);

            for (int operand1_index = 0; operand1_index < coeff_count; operand1_index++)
            {
                //const uint64_t *operand1_coeff = get_poly_coeff(operand1, operand1_index, modulus.uint64_count());
                if (operand1[operand1_index] == 0)
                {
                    // If coefficient is 0, then move on to next coefficient.
                    continue;
                }
                // Lastly, do more expensive add if other cases don't handle it.
                for (int operand2_index = 0; operand2_index < coeff_count; operand2_index++)
                {
                    //const uint64_t *operand2_coeff = get_poly_coeff(operand2, operand2_index, modulus.uint64_count());
                    uint64_t operand2_coeff = operand2[operand2_index];
                    if (operand2_coeff == 0)
                    {
                        // If coefficient is 0, then move on to next coefficient.
                        continue;
                    }

                    // Lazy reduction
                    uint64_t temp[2];
                    multiply_uint64(operand1[operand1_index], operand2_coeff, temp);
                    temp[1] += add_uint64(temp[0], result[operand1_index + operand2_index], 0, temp);

                    result[operand1_index + operand2_index] = barrett_reduce_128(temp, modulus);
                }
            }
        }

        void divide_poly_poly_coeffmod_inplace(uint64_t *numerator, const uint64_t *denominator, int coeff_count, const SmallModulus &modulus, uint64_t *quotient, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
            if (numerator == nullptr)
            {
                throw invalid_argument("numerator");
            }
            if (denominator == nullptr)
            {
                throw invalid_argument("denominator");
            }
            if (is_zero_poly(denominator, coeff_count, modulus.uint64_count()))
            {
                throw invalid_argument("denominator");
            }
            if (coeff_count < 0)
            {
                throw invalid_argument("coeff_count");
            }
            if (quotient == nullptr)
            {
                throw invalid_argument("quotient");
            }
            if (numerator == quotient || denominator == quotient)
            {
                throw invalid_argument("quotient cannot point to same value as numerator or denominator");
            }
            if (numerator == denominator)
            {
                throw invalid_argument("numerator cannot point to same value as denominator");
            }
            if (modulus.is_zero())
            {
                throw invalid_argument("modulus");
            }
#endif
            // Clear quotient.
            int coeff_uint64_count = modulus.uint64_count();
            set_zero_poly(coeff_count, coeff_uint64_count, quotient);

            // Determine most significant coefficients of numerator and denominator.
            int numerator_coeffs = get_significant_coeff_count_poly(numerator, coeff_count, coeff_uint64_count);
            int denominator_coeffs = get_significant_coeff_count_poly(denominator, coeff_count, coeff_uint64_count);

            // If numerator has lesser degree than denominator, then done.
            if (numerator_coeffs < denominator_coeffs)
            {
                return;
            }

            int intermediate_uint64_count = coeff_uint64_count * 2;
            Pointer big_alloc(allocate_uint(coeff_uint64_count + intermediate_uint64_count + intermediate_uint64_count + 7 * coeff_uint64_count, pool));

            // Create scalar to store value that makes denominator monic.
            uint64_t *monic_denominator_scalar = big_alloc.get();

            // Create temporary scalars used during calculation of quotient.
            // Both are purposely twice as wide to store intermediate product prior to modulo operation.
            uint64_t *temp_quotient = monic_denominator_scalar + coeff_uint64_count;
            uint64_t *subtrahend = temp_quotient + intermediate_uint64_count;

            // Determine scalar necessary to make denominator monic.
            const uint64_t *leading_denominator_coeff = get_poly_coeff(denominator, denominator_coeffs - 1, coeff_uint64_count);
            if (!try_invert_uint_mod(*leading_denominator_coeff, modulus, *monic_denominator_scalar))
            {
                throw invalid_argument("modulus is not coprime with leading denominator coefficient");
            }

            // Perform coefficient-wise division algorithm.
            while (numerator_coeffs >= denominator_coeffs)
            {
                // Determine leading numerator coefficient.
                const uint64_t *leading_numerator_coeff = get_poly_coeff(numerator, numerator_coeffs - 1, coeff_uint64_count);

                // If leading numerator coefficient is not zero, then need to make zero by subtraction.
                if (!is_zero_uint(leading_numerator_coeff, coeff_uint64_count))
                {
                    // Determine shift necesarry to bring significant coefficients in alignment.
                    int denominator_shift = numerator_coeffs - denominator_coeffs;

                    // Determine quotient's coefficient, which is scalar that makes denominator's leading coefficient one
                    // multiplied by leading coefficient of denominator (which when subtracted will zero out the topmost
                    // denominator coefficient).
                    uint64_t *quotient_coeff = get_poly_coeff(quotient, denominator_shift, coeff_uint64_count);
                    *temp_quotient = multiply_uint_uint_mod(*monic_denominator_scalar, *leading_numerator_coeff, modulus);
                    set_uint_uint(temp_quotient, coeff_uint64_count, quotient_coeff);

                    // Subtract numerator and quotient*denominator (shifted by denominator_shift).
                    for (int denominator_coeff_index = 0; denominator_coeff_index < denominator_coeffs; denominator_coeff_index++)
                    {
                        // Multiply denominator's coefficient by quotient.
                        const uint64_t *denominator_coeff = get_poly_coeff(denominator, denominator_coeff_index, coeff_uint64_count);
                        *subtrahend = multiply_uint_uint_mod(*temp_quotient, *denominator_coeff, modulus);

                        // Subtract numerator with resulting product, appropriately shifted by denominator shift.
                        uint64_t *numerator_coeff = get_poly_coeff(numerator, denominator_coeff_index + denominator_shift, coeff_uint64_count);
                        *numerator_coeff = sub_uint_uint_mod(*numerator_coeff, *subtrahend, modulus);
                    }
                }

                // Top numerator coefficient must now be zero, so adjust coefficient count.
                numerator_coeffs--;
            }
        }

        void modulo_poly_inplace(uint64_t *value, int value_coeff_count, const PolyModulus &poly_modulus, const SmallModulus &modulus)
        {
#ifdef SEAL_DEBUG
            if (value == nullptr)
            {
                throw invalid_argument("value");
            }
            if (value_coeff_count <= 0)
            {
                throw invalid_argument("value_coeff_count");
            }
            if (value == poly_modulus.get())
            {
                throw invalid_argument("value cannot point to same value as poly_modulus");
            }
            if (modulus.is_zero())
            {
                throw invalid_argument("modulus");
            }
#endif
            // Determine most significant coefficients of value and poly_modulus.
            int value_coeffs = value_coeff_count;
            while (value_coeffs > 0 && value[value_coeffs - 1] == 0)
            {
                value_coeffs--;
            }
            int poly_modulus_coeff_count = poly_modulus.coeff_count();

            // If value has lesser degree than poly_modulus, then done.
            if (value_coeffs < poly_modulus_coeff_count)
            {
                return;
            }

            // Handle 1x^n + 1 polynomials more efficiently.
            if (poly_modulus.is_one_zero_one())
            {
                // Perform coefficient-wise division algorithm.
                while (value_coeffs >= poly_modulus_coeff_count)
                {
                    // Determine leading value coefficient.
                    uint64_t *leading_value_coeff = value + value_coeffs - 1;

                    // If leading value coefficient is not zero, then need to make zero by subtraction.
                    if (*leading_value_coeff != 0)
                    {
                        // Determine shift necesarry to bring significant coefficients in alignment.
                        int poly_modulus_shift = value_coeffs - poly_modulus_coeff_count;

                        // Subtract top coefficient from bottom-shifted coefficient.
                        value[poly_modulus_shift] = sub_uint_uint_mod(value[poly_modulus_shift], *leading_value_coeff, modulus);

                        // Zero-out leading coefficient.
                        *leading_value_coeff = 0;
                    }

                    // Top value coefficient must now be zero, so adjust coefficient count.
                    value_coeffs--;
                }
                return;
            }

            // Is poly modulus monic?
            if (poly_modulus.get()[poly_modulus.coeff_count() - 1] == 1)
            {
                const uint64_t *polymodptr = poly_modulus.get();

                // Perform coefficient-wise division algorithm.
                while (value_coeffs >= poly_modulus_coeff_count)
                {
                    // Determine leading value coefficient.
                    uint64_t leading_value_coeff = value[value_coeffs - 1];

                    // If leading value coefficient is not zero, then need to make zero by subtraction.
                    if (leading_value_coeff != 0)
                    {
                        // Determine shift necessary to bring significant coefficients in alignment.
                        int poly_modulus_shift = value_coeffs - poly_modulus_coeff_count;

                        uint64_t temp_quotient = modulus.value() - leading_value_coeff;

                        // Subtract value and quotient*poly_modulus (shifted by poly_modulus_shift).
                        for (int poly_modulus_coeff_index = 0; poly_modulus_coeff_index < poly_modulus_coeff_count; poly_modulus_coeff_index++)
                        {
                            // Multiply poly_modulus's coefficient by quotient.
                            if (polymodptr[poly_modulus_coeff_index] != 0)
                            {
                                uint64_t *value_coeff = value + (poly_modulus_coeff_index + poly_modulus_shift);

                                // Lazy reduction
                                uint64_t temp[2];
                                multiply_uint64(temp_quotient, polymodptr[poly_modulus_coeff_index], temp);
                                temp[1] += add_uint64(temp[0], *value_coeff, 0, temp);
                                *value_coeff = barrett_reduce_128(temp, modulus);
                            }
                        }
                    }

                    // Top value coefficient must now be zero, so adjust coefficient count.
                    value_coeffs--;
                }

                return;
            }

            // Generic case
            uint64_t monic_poly_modulus_scalar = 0, temp_quotient = 0, subtrahend = 0;

            // Determine scalar necessary to make poly_modulus monic.
            const uint64_t *polymodptr = poly_modulus.get();
            uint64_t leading_poly_modulus_coeff = polymodptr[poly_modulus_coeff_count - 1];
            if (!try_invert_uint_mod(leading_poly_modulus_coeff, modulus, monic_poly_modulus_scalar))
            {
                throw invalid_argument("modulus is not coprime with leading poly_modulus coefficient");
            }

            // Perform coefficient-wise division algorithm.
            while (value_coeffs >= poly_modulus_coeff_count)
            {
                // Determine leading value coefficient.
                uint64_t leading_value_coeff = value[value_coeffs - 1];

                // If leading value coefficient is not zero, then need to make zero by subtraction.
                if (leading_value_coeff != 0)
                {
                    // Determine shift necessary to bring significant coefficients in alignment.
                    int poly_modulus_shift = value_coeffs - poly_modulus_coeff_count;

                    // Determine quotient's coefficient, which is scalar that makes poly_modulus's leading coefficient one
                    // multiplied by leading coefficient of poly_modulus (which when subtracted will zero out the topmost
                    // poly_modulus coefficient).
                    temp_quotient = multiply_uint_uint_mod(monic_poly_modulus_scalar, leading_value_coeff, modulus);

                    // Subtract value and quotient*poly_modulus (shifted by poly_modulus_shift).
                    for (int poly_modulus_coeff_index = 0; poly_modulus_coeff_index < poly_modulus_coeff_count; poly_modulus_coeff_index++)
                    {
                        // Multiply poly_modulus's coefficient by quotient.
                        uint64_t poly_modulus_coeff = polymodptr[poly_modulus_coeff_index];
                        if (poly_modulus_coeff != 0)
                        {
                            subtrahend = multiply_uint_uint_mod(temp_quotient, poly_modulus_coeff, modulus);

                            // Subtract value with resulting product, appropriately shifted by poly_modulus shift.
                            uint64_t *value_coeff = value + (poly_modulus_coeff_index + poly_modulus_shift);
                            *value_coeff = sub_uint_uint_mod(*value_coeff, subtrahend, modulus);
                        }
                    }
                }

                // Top value coefficient must now be zero, so adjust coefficient count.
                value_coeffs--;
            }
        }

        uint64_t poly_infty_norm_coeffmod(const std::uint64_t *poly, int poly_coeff_count, const SmallModulus &modulus)
        {
            // Construct negative threshold (first negative modulus value) to compute absolute values of coeffs.
            uint64_t modulus_neg_threshold = (modulus.value() + 1) >> 1;

            // Mod out the poly coefficients and choose a symmetric representative from [-modulus,modulus). Keep track of the max.
            uint64_t result = 0;
            for (int coeff_index = 0; coeff_index < poly_coeff_count; coeff_index++)
            {
                uint64_t poly_coeff = poly[coeff_index] % modulus.value();
                if (poly_coeff >= modulus_neg_threshold)
                {
                    poly_coeff = modulus.value() - poly_coeff;
                }
                if (poly_coeff > result)
                {
                    result = poly_coeff;
                }
            }
            return result;
        }

        bool try_invert_poly_coeffmod(const uint64_t *operand, const uint64_t *poly_modulus, int coeff_count, const SmallModulus &modulus, uint64_t *result, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
            if (operand == nullptr)
            {
                throw invalid_argument("operand");
            }
            if (poly_modulus == nullptr)
            {
                throw invalid_argument("poly_modulus");
            }
            if (coeff_count <= 0)
            {
                throw invalid_argument("coeff_count");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (get_significant_coeff_count_poly(operand, coeff_count, modulus.uint64_count()) >= get_significant_coeff_count_poly(poly_modulus, coeff_count, modulus.uint64_count()))
            {
                throw out_of_range("operand");
            }
            if (modulus.is_zero())
            {
                throw invalid_argument("modulus");
            }
#endif
            // Cannot invert 0 poly.
            int coeff_uint64_count = modulus.uint64_count();
            if (is_zero_poly(operand, coeff_count, coeff_uint64_count))
            {
                return false;
            }

            // Construct a mutable copy of operand and modulus, with numerator being modulus
            // and operand being denominator. Notice that degree(numerator) >= degree(denominator).
            Pointer numerator_anchor(allocate_poly(coeff_count, coeff_uint64_count, pool));
            uint64_t *numerator = numerator_anchor.get();
            set_poly_poly(poly_modulus, coeff_count, coeff_uint64_count, numerator);
            Pointer denominator_anchor(allocate_poly(coeff_count, coeff_uint64_count, pool));
            uint64_t *denominator = denominator_anchor.get();
            set_poly_poly(operand, coeff_count, coeff_uint64_count, denominator);

            // Determine most significant coefficients of each.
            int numerator_coeffs = get_significant_coeff_count_poly(numerator, coeff_count, coeff_uint64_count);
            int denominator_coeffs = get_significant_coeff_count_poly(denominator, coeff_count, coeff_uint64_count);

            // Create poly to store quotient.
            Pointer quotient(allocate_poly(coeff_count, coeff_uint64_count, pool));

            // Create scalar to store value that makes denominator monic.
            uint64_t monic_denominator_scalar;

            // Create temporary scalars used during calculation of quotient.
            // Both are purposely twice as wide to store intermediate product prior to modulo operation.
            int intermediate_uint64_count = coeff_uint64_count * 2;
            Pointer temp_quotient(allocate_uint(intermediate_uint64_count, pool));
            Pointer subtrahend(allocate_uint(intermediate_uint64_count, pool));

            // Create three polynomials to store inverse.
            // Initialize invert_prior to 0 and invert_curr to 1.
            Pointer invert_prior_anchor(allocate_poly(coeff_count, coeff_uint64_count, pool));
            uint64_t *invert_prior = invert_prior_anchor.get();
            set_zero_poly(coeff_count, coeff_uint64_count, invert_prior);
            Pointer invert_curr_anchor(allocate_poly(coeff_count, coeff_uint64_count, pool));
            uint64_t *invert_curr = invert_curr_anchor.get();
            set_zero_poly(coeff_count, coeff_uint64_count, invert_curr);
            uint64_t *invert_curr_first_coeff = get_poly_coeff(invert_curr, 0, coeff_uint64_count);
            set_uint(1, coeff_uint64_count, invert_curr_first_coeff);
            Pointer invert_next_anchor(allocate_poly(coeff_count, coeff_uint64_count, pool));
            uint64_t *invert_next = invert_next_anchor.get();

            // Perform extended Euclidean algorithm.
            while (true)
            {
                // NOTE: degree(numerator) >= degree(denominator).

                // Determine scalar necessary to make denominator monic.
                const uint64_t *leading_denominator_coeff = get_poly_coeff(denominator, denominator_coeffs - 1, coeff_uint64_count);
                if (!try_invert_uint_mod(*leading_denominator_coeff, modulus, monic_denominator_scalar))
                {
                    throw invalid_argument("modulus is not coprime with leading denominator coefficient");
                }

                // Clear quotient.
                set_zero_poly(coeff_count, coeff_uint64_count, quotient.get());

                // Perform coefficient-wise division algorithm.
                while (numerator_coeffs >= denominator_coeffs)
                {
                    // Determine leading numerator coefficient.
                    const uint64_t *leading_numerator_coeff = get_poly_coeff(numerator, numerator_coeffs - 1, coeff_uint64_count);

                    // If leading numerator coefficient is not zero, then need to make zero by subtraction.
                    if (!is_zero_uint(leading_numerator_coeff, coeff_uint64_count))
                    {
                        // Determine shift necessary to bring significant coefficients in alignment.
                        int denominator_shift = numerator_coeffs - denominator_coeffs;

                        // Determine quotient's coefficient, which is scalar that makes denominator's leading coefficient one
                        // multiplied by leading coefficient of denominator (which when subtracted will zero out the topmost
                        // denominator coefficient).
                        uint64_t *quotient_coeff = get_poly_coeff(quotient.get(), denominator_shift, coeff_uint64_count);
                        *temp_quotient.get() = multiply_uint_uint_mod(monic_denominator_scalar, *leading_numerator_coeff, modulus);
                        set_uint_uint(temp_quotient.get(), coeff_uint64_count, quotient_coeff);

                        // Subtract numerator and quotient*denominator (shifted by denominator_shift).
                        for (int denominator_coeff_index = 0; denominator_coeff_index < denominator_coeffs; denominator_coeff_index++)
                        {
                            // Multiply denominator's coefficient by quotient.
                            const uint64_t *denominator_coeff = get_poly_coeff(denominator, denominator_coeff_index, coeff_uint64_count);
                            *subtrahend.get() = multiply_uint_uint_mod(*temp_quotient.get(), *denominator_coeff, modulus);

                            // Subtract numerator with resulting product, appropriately shifted by denominator shift.
                            uint64_t *numerator_coeff = get_poly_coeff(numerator, denominator_coeff_index + denominator_shift, coeff_uint64_count);
                            *numerator_coeff = sub_uint_uint_mod(*numerator_coeff, *subtrahend.get(), modulus);
                        }
                    }

                    // Top numerator coefficient must now be zero, so adjust coefficient count.
                    numerator_coeffs--;
                }

                // Double check that numerator coefficients is correct because possible other coefficients are zero.
                numerator_coeffs = get_significant_coeff_count_poly(numerator, coeff_count, coeff_uint64_count);

                // We are done if numerator is zero.
                if (numerator_coeffs == 0)
                {
                    break;
                }

                // Integrate quotient with invert coefficients.
                // Calculate: invert_next = invert_prior + -quotient * invert_curr
                multiply_truncate_poly_poly_coeffmod(quotient.get(), invert_curr, coeff_count, modulus, invert_next);
                sub_poly_poly_coeffmod(invert_prior, invert_next, coeff_count, modulus, invert_next);

                // Swap prior and curr, and then curr and next.
                swap(invert_prior, invert_curr);
                swap(invert_curr, invert_next);

                // Swap numerator and denominator.
                swap(numerator, denominator);
                swap(numerator_coeffs, denominator_coeffs);
            }

            // Polynomial is invertible only if denominator is just a scalar.
            if (denominator_coeffs != 1)
            {
                return false;
            }

            // Determine scalar necessary to make denominator monic.
            const uint64_t *leading_denominator_coeff = get_poly_coeff(denominator, 0, coeff_uint64_count);
            if (!try_invert_uint_mod(*leading_denominator_coeff, modulus, monic_denominator_scalar))
            {
                throw invalid_argument("modulus is not coprime with leading denominator coefficient");
            }

            // Multiply inverse by scalar and done.
            multiply_poly_scalar_coeffmod(invert_curr, coeff_count, monic_denominator_scalar, modulus, result);
            return true;
        }

        void exponentiate_poly_polymod_coeffmod(const uint64_t *poly, const uint64_t *exponent, int exponent_uint64_count, const PolyModulus &poly_modulus, const SmallModulus &modulus, uint64_t *result, MemoryPool &pool)
        {
            int poly_modulus_coeff_count = poly_modulus.coeff_count();
#ifdef SEAL_DEBUG
            int poly_modulus_coeff_uint64_count = poly_modulus.coeff_uint64_count();
            if (poly == nullptr)
            {
                throw invalid_argument("poly");
            }
            if (exponent == nullptr)
            {
                throw invalid_argument("exponent");
            }
            if (exponent_uint64_count <= 0)
            {
                throw invalid_argument("exponent_uint64_count");
            }
            if (modulus.is_zero())
            {
                throw invalid_argument("modulus");
            }
            if (is_zero_poly(poly_modulus.get(), poly_modulus_coeff_count, poly_modulus_coeff_uint64_count))
            {
                throw invalid_argument("poly_modulus");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
#endif
            // Fast cases
            if (is_zero_uint(exponent, exponent_uint64_count))
            {
                set_uint(1, poly_modulus_coeff_count, result);
                return;
            }

            modulo_poly(poly, poly_modulus_coeff_count, poly_modulus, modulus, result, pool);

            if (is_equal_uint(exponent, exponent_uint64_count, 1))
            {
                return;
            }

            // Need to make a copy of exponent
            Pointer exponent_copy(allocate_uint(exponent_uint64_count, pool));
            set_uint_uint(exponent, exponent_uint64_count, exponent_copy.get());

            // Perform binary exponentiation.
            Pointer power(allocate_uint(poly_modulus_coeff_count, pool));
            Pointer product(allocate_uint(poly_modulus_coeff_count, pool));
            Pointer intermediate(allocate_uint(poly_modulus_coeff_count, pool));

            uint64_t *powerptr = power.get();
            uint64_t *productptr = product.get();
            uint64_t *intermediateptr = intermediate.get();

            set_uint_uint(result, poly_modulus_coeff_count, powerptr);
            set_uint(1, poly_modulus_coeff_count, intermediateptr);

            // Initially: power = operand and intermediate = 1, product is not initialized.
            if (poly_modulus.is_fft_modulus() && poly_modulus.coeff_count_power_of_two() > 1)
            {
                while (true)
                {
                    if (*exponent_copy.get() & 1)
                    {
                        nussbaumer_multiply_poly_poly_coeffmod(powerptr, intermediateptr, poly_modulus.coeff_count_power_of_two(), modulus, productptr, pool);
                        swap(productptr, intermediateptr);
                    }
                    right_shift_uint(exponent_copy.get(), 1, exponent_uint64_count, exponent_copy.get());
                    if (is_zero_uint(exponent_copy.get(), exponent_uint64_count))
                    {
                        break;
                    }
                    nussbaumer_multiply_poly_poly_coeffmod(powerptr, powerptr, poly_modulus.coeff_count_power_of_two(), modulus, productptr, pool);
                    swap(productptr, powerptr);
                }
            }
            else
            {
                while (true)
                {
                    if (*exponent_copy.get() & 1)
                    {
                        nonfft_multiply_poly_poly_polymod_coeffmod(powerptr, intermediateptr, poly_modulus, modulus, productptr, pool);
                        swap(productptr, intermediateptr);
                    }
                    right_shift_uint(exponent_copy.get(), 1, exponent_uint64_count, exponent_copy.get());
                    if (is_zero_uint(exponent_copy.get(), exponent_uint64_count))
                    {
                        break;
                    }
                    nonfft_multiply_poly_poly_polymod_coeffmod(powerptr, powerptr, poly_modulus, modulus, productptr, pool);
                    swap(productptr, powerptr);
                }
            }
            set_uint_uint(intermediateptr, poly_modulus_coeff_count, result);
        }
    }
}
