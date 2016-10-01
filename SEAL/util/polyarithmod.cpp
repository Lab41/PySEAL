#include "util/uintcore.h"
#include "util/uintarithmod.h"
#include "util/polycore.h"
#include "util/polyarith.h"
#include "util/polyarithmod.h"
#include "util/polyfftmultmod.h"

using namespace std;

namespace seal
{
    namespace util
    {
        void dyadic_product_coeffmod(const uint64_t *operand1, const uint64_t *operand2, int coeff_count, const Modulus &modulus, uint64_t *result, MemoryPool &pool)
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
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (coeff_count <= 0)
            {
                throw invalid_argument("coeff_count");
            }
            if (modulus.uint64_count() <= 0 || modulus.get() == nullptr)
            {
                throw invalid_argument("modulus");
            }
#endif
            int coeff_uint64_count = modulus.uint64_count();

            // Use the same allocation for every instance of multiply_uint_uint_mod.
            Pointer big_alloc(allocate_uint(4 * coeff_uint64_count, pool));
            for (int i = 0; i < coeff_count; ++i)
            {
                multiply_uint_uint_mod(operand1, operand2, modulus, result, pool, big_alloc.get()); 
                operand1 += coeff_uint64_count;
                operand2 += coeff_uint64_count;
                result += coeff_uint64_count; 
            }
        }

        void modulo_poly_inplace(uint64_t *value, int value_coeff_count, const PolyModulus &poly_modulus, const Modulus &modulus, MemoryPool &pool)
        {
#ifdef _DEBUG
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
#endif
            // Determine most significant coefficients of value and poly_modulus.
            int coeff_uint64_count = modulus.uint64_count();
            int value_coeffs = get_significant_coeff_count_poly(value, value_coeff_count, coeff_uint64_count);
            int poly_modulus_coeff_count = poly_modulus.coeff_count();

            // If value has lesser degree than poly_modulus, then done.
            if (value_coeffs < poly_modulus_coeff_count)
            {
                return;
            }

            // Handle 1x^n + 1 polynomials more efficiently.
            const uint64_t *coeff_modulus = modulus.get();
            if (poly_modulus.is_one_zero_one())
            {
                // Perform coefficient-wise division algorithm.
                while (value_coeffs >= poly_modulus_coeff_count)
                {
                    // Determine leading value coefficient.
                    uint64_t *leading_value_coeff = get_poly_coeff(value, value_coeffs - 1, coeff_uint64_count);

                    // If leading value coefficient is not zero, then need to make zero by subtraction.
                    if (!is_zero_uint(leading_value_coeff, coeff_uint64_count))
                    {
                        // Determine shift necesarry to bring significant coefficients in alignment.
                        int poly_modulus_shift = value_coeffs - poly_modulus_coeff_count;

                        // Subtract top coefficient from bottom-shifted coefficient.
                        uint64_t *value_coeff = get_poly_coeff(value, poly_modulus_shift, coeff_uint64_count);
                        sub_uint_uint_mod(value_coeff, leading_value_coeff, coeff_modulus, coeff_uint64_count, value_coeff);

                        // Zero-out leading coefficient.
                        set_zero_uint(coeff_uint64_count, leading_value_coeff);
                    }

                    // Top value coefficient must now be zero, so adjust coefficient count.
                    value_coeffs--;
                }
                return;
            }

            int intermediate_uint64_count = coeff_uint64_count * 2;
            Pointer big_alloc(allocate_uint(coeff_uint64_count + 2 * intermediate_uint64_count + 7 * coeff_uint64_count, pool));

            // Create scalar to store value that makes poly_modulus monic.
            uint64_t *monic_poly_modulus_scalar = big_alloc.get();

            // Create temporary scalars used during calculation of quotient.
            // Both are purposely twice as wide to store intermediate product prior to modulo operation.
            uint64_t *temp_quotient = monic_poly_modulus_scalar + coeff_uint64_count;
            uint64_t *subtrahend = temp_quotient + intermediate_uint64_count;

            // We still have 7 x coeff_uint64_count memory left in the big allocation
            uint64_t *alloc_ptr = subtrahend + intermediate_uint64_count;

            // Determine scalar necessary to make poly_modulus monic.
            const uint64_t *polymodptr = poly_modulus.get();
            const uint64_t *leading_poly_modulus_coeff = get_poly_coeff(polymodptr, poly_modulus_coeff_count - 1, coeff_uint64_count);
            if (!try_invert_uint_mod(leading_poly_modulus_coeff, coeff_modulus, coeff_uint64_count, monic_poly_modulus_scalar, pool, alloc_ptr))
            {
                throw invalid_argument("coeff_modulus is not coprime with leading poly_modulus coefficient");
            }

            // Perform coefficient-wise division algorithm.
            while (value_coeffs >= poly_modulus_coeff_count)
            {
                // Determine leading value coefficient.
                const uint64_t *leading_value_coeff = get_poly_coeff(value, value_coeffs - 1, coeff_uint64_count);

                // If leading value coefficient is not zero, then need to make zero by subtraction.
                if (!is_zero_uint(leading_value_coeff, coeff_uint64_count))
                {
                    // Determine shift necessary to bring significant coefficients in alignment.
                    int poly_modulus_shift = value_coeffs - poly_modulus_coeff_count;

                    // Determine quotient's coefficient, which is scalar that makes poly_modulus's leading coefficient one
                    // multiplied by leading coefficient of poly_modulus (which when subtracted will zero out the topmost
                    // poly_modulus coefficient).
                    multiply_uint_uint_mod_inplace(monic_poly_modulus_scalar, leading_value_coeff, modulus, temp_quotient, pool, alloc_ptr);

                    // Subtract value and quotient*poly_modulus (shifted by poly_modulus_shift).
                    for (int poly_modulus_coeff_index = 0; poly_modulus_coeff_index < poly_modulus_coeff_count; ++poly_modulus_coeff_index)
                    {
                        // Multiply poly_modulus's coefficient by quotient.
                        const uint64_t *poly_modulus_coeff = get_poly_coeff(polymodptr, poly_modulus_coeff_index, coeff_uint64_count);
                        multiply_uint_uint_mod_inplace(temp_quotient, poly_modulus_coeff, modulus, subtrahend, pool, alloc_ptr);

                        // Subtract value with resulting product, appropriately shifted by poly_modulus shift.
                        uint64_t *value_coeff = get_poly_coeff(value, poly_modulus_coeff_index + poly_modulus_shift, coeff_uint64_count);
                        sub_uint_uint_mod(value_coeff, subtrahend, coeff_modulus, coeff_uint64_count, value_coeff);
                    }
                }

                // Top value coefficient must now be zero, so adjust coefficient count.
                value_coeffs--;
            }
        }

        void modulo_poly(const uint64_t *value, int value_coeff_count, const PolyModulus &poly_modulus, const Modulus &modulus, uint64_t *result, MemoryPool &pool)
        {
#ifdef _DEBUG
            if (value == nullptr)
            {
                throw invalid_argument("value");
            }
            if (value_coeff_count <= 0)
            {
                throw invalid_argument("value_coeff_count");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
#endif
            int coeff_uint64_count = modulus.uint64_count();
            Pointer value_copy(allocate_poly(value_coeff_count, coeff_uint64_count, pool));
            set_poly_poly(value, value_coeff_count, coeff_uint64_count, value_copy.get());
            modulo_poly_inplace(value_copy.get(), value_coeff_count, poly_modulus, modulus, pool);
            set_poly_poly(value_copy.get(), poly_modulus.coeff_count(), coeff_uint64_count, result);
        }

        void nonfft_multiply_poly_poly_polymod_coeffmod(const uint64_t *operand1, const uint64_t *operand2, const PolyModulus &poly_modulus, const Modulus &modulus, uint64_t *result, MemoryPool &pool)
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
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (get_significant_coeff_count_poly(operand1, poly_modulus.coeff_count(), poly_modulus.coeff_uint64_count()) >= poly_modulus.coeff_count())
            {
                throw out_of_range("operand1");
            }
            if (get_significant_coeff_count_poly(operand2, poly_modulus.coeff_count(), poly_modulus.coeff_uint64_count()) >= poly_modulus.coeff_count())
            {
                throw out_of_range("operand2");
            }
#endif
            // Calculate normal product.
            int coeff_count = poly_modulus.coeff_count();
            int coeff_uint64_count = poly_modulus.coeff_uint64_count();
            int intermediate_coeff_count = coeff_count * 2 - 1;
            Pointer intermediate(allocate_poly(intermediate_coeff_count, coeff_uint64_count, pool));
            multiply_poly_poly_coeffmod(operand1, operand2, coeff_count, modulus, intermediate.get(), pool);

            // Perform modulo operation.
            modulo_poly_inplace(intermediate.get(), intermediate_coeff_count, poly_modulus, modulus, pool);

            // Copy to result.
            set_poly_poly(intermediate.get(), coeff_count, coeff_uint64_count, result);
        }

        void nonfft_multiply_poly_poly_polymod_coeffmod_inplace(const uint64_t *operand1, const uint64_t *operand2, const PolyModulus &poly_modulus, const Modulus &modulus, uint64_t *result, MemoryPool &pool)
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
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (get_significant_coeff_count_poly(operand1, poly_modulus.coeff_count(), poly_modulus.coeff_uint64_count()) >= poly_modulus.coeff_count())
            {
                throw out_of_range("operand1");
            }
            if (get_significant_coeff_count_poly(operand2, poly_modulus.coeff_count(), poly_modulus.coeff_uint64_count()) >= poly_modulus.coeff_count())
            {
                throw out_of_range("operand2");
            }
#endif
            // Calculate normal product.
            int coeff_count = poly_modulus.coeff_count();
            int result_coeff_count = coeff_count * 2 - 1;
            multiply_poly_poly_coeffmod(operand1, operand2, coeff_count, modulus, result, pool);

            // Perform modulo operation.
            modulo_poly_inplace(result, result_coeff_count, poly_modulus, modulus, pool);
        }

        bool try_invert_poly_coeffmod(const uint64_t *operand, const uint64_t *poly_modulus, int coeff_count, const Modulus &modulus, uint64_t *result, MemoryPool &pool)
        {
#ifdef _DEBUG
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
            Pointer monic_denominator_scalar(allocate_uint(coeff_uint64_count, pool));

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

            Pointer big_alloc(allocate_uint(7 * coeff_uint64_count, pool));

            // Perform extended Euclidean algorithm.
            const uint64_t *modulusptr = modulus.get();
            while (true)
            {
                // NOTE: degree(numerator) >= degree(denominator).

                // Determine scalar necessary to make denominator monic.
                const uint64_t *leading_denominator_coeff = get_poly_coeff(denominator, denominator_coeffs - 1, coeff_uint64_count);
                if (!try_invert_uint_mod(leading_denominator_coeff, modulusptr, coeff_uint64_count, monic_denominator_scalar.get(), pool, big_alloc.get()))
                {
                    throw invalid_argument("coeff_modulus is not coprime with leading denominator coefficient");
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
                        multiply_uint_uint_mod_inplace(monic_denominator_scalar.get(), leading_numerator_coeff, modulus, temp_quotient.get(), pool, big_alloc.get());
                        set_uint_uint(temp_quotient.get(), coeff_uint64_count, quotient_coeff);

                        // Subtract numerator and quotient*denominator (shifted by denominator_shift).
                        for (int denominator_coeff_index = 0; denominator_coeff_index < denominator_coeffs; ++denominator_coeff_index)
                        {
                            // Multiply denominator's coefficient by quotient.
                            const uint64_t *denominator_coeff = get_poly_coeff(denominator, denominator_coeff_index, coeff_uint64_count);
                            multiply_uint_uint_mod_inplace(temp_quotient.get(), denominator_coeff, modulus, subtrahend.get(), pool, big_alloc.get());

                            // Subtract numerator with resulting product, appropriately shifted by denominator shift.
                            uint64_t *numerator_coeff = get_poly_coeff(numerator, denominator_coeff_index + denominator_shift, coeff_uint64_count);
                            sub_uint_uint_mod(numerator_coeff, subtrahend.get(), modulusptr, coeff_uint64_count, numerator_coeff);
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
                multiply_truncate_poly_poly_coeffmod(quotient.get(), invert_curr, coeff_count, modulus, invert_next, pool);
                sub_poly_poly_coeffmod(invert_prior, invert_next, coeff_count, modulusptr, coeff_uint64_count, invert_next);

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
            if (!try_invert_uint_mod(leading_denominator_coeff, modulusptr, coeff_uint64_count, monic_denominator_scalar.get(), pool))
            {
                throw invalid_argument("coeff_modulus is not coprime with leading denominator coefficient");
            }

            // Multiply inverse by scalar and done.
            multiply_poly_scalar_coeffmod(invert_curr, coeff_count, monic_denominator_scalar.get(), modulus, result, pool);
            return true;
        }
    }
}
