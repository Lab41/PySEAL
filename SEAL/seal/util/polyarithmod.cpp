#include "seal/util/uintcore.h"
#include "seal/util/uintarith.h"
#include "seal/util/uintarithmod.h"
#include "seal/util/polycore.h"
#include "seal/util/polyarith.h"
#include "seal/util/polyarithmod.h"
#include "seal/util/polyfftmultmod.h"

using namespace std;

namespace seal
{
    namespace util
    {
        void multiply_poly_scalar_coeffmod(const uint64_t *poly, int coeff_count, const uint64_t *scalar, const Modulus &modulus, uint64_t *result, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
            if (poly == nullptr && coeff_count > 0)
            {
                throw invalid_argument("poly");
            }
            if (coeff_count < 0)
            {
                throw invalid_argument("coeff_count");
            }
            if (scalar == nullptr)
            {
                throw invalid_argument("scalar");
            }
            if (result == nullptr && coeff_count > 0)
            {
                throw invalid_argument("result");
            }
#endif
            int coeff_uint64_count = modulus.uint64_count();
            int intermediate_uint64_count = coeff_uint64_count * 2;
            Pointer intermediate(allocate_uint(intermediate_uint64_count, pool));
            for (int i = 0; i < coeff_count; i++)
            {
                multiply_uint_uint_mod_inplace(poly, scalar, modulus, intermediate.get(), pool);
                set_uint_uint(intermediate.get(), coeff_uint64_count, result);
                poly += coeff_uint64_count;
                result += coeff_uint64_count;
            }
        }

        void multiply_poly_poly_coeffmod(const uint64_t *operand1, int operand1_coeff_count, int operand1_coeff_uint64_count, const uint64_t *operand2, int operand2_coeff_count, int operand2_coeff_uint64_count,
            const Modulus &modulus, int result_coeff_count, uint64_t *result, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr && operand1_coeff_count > 0 && operand1_coeff_uint64_count > 0)
            {
                throw invalid_argument("operand1");
            }
            if (operand1_coeff_count < 0)
            {
                throw invalid_argument("operand1_coeff_count");
            }
            if (operand1_coeff_uint64_count < 0)
            {
                throw invalid_argument("operand1_coeff_uint64_count");
            }
            if (operand2 == nullptr && operand2_coeff_count > 0 && operand2_coeff_uint64_count > 0)
            {
                throw invalid_argument("operand2");
            }
            if (operand2_coeff_count < 0)
            {
                throw invalid_argument("operand2_coeff_count");
            }
            if (operand2_coeff_uint64_count < 0)
            {
                throw invalid_argument("operand2_coeff_uint64_count");
            }
            if (result_coeff_count < 0)
            {
                throw invalid_argument("result_coeff_count");
            }
            if (result == nullptr && result_coeff_count > 0)
            {
                throw invalid_argument("result");
            }
            if (result != nullptr && (operand1 == result || operand2 == result || modulus.get() == result))
            {
                throw invalid_argument("result cannot point to the same value as operand1, operand2, or modulus");
            }
#endif
            int intermediate_uint64_count = operand1_coeff_uint64_count + operand2_coeff_uint64_count;
            Pointer intermediate(allocate_uint(intermediate_uint64_count, pool));

            // Clear product.
            int result_coeff_uint64_count = modulus.uint64_count();
            set_zero_poly(result_coeff_count, result_coeff_uint64_count, result);

            const uint64_t *modulusptr = modulus.get();
            operand1_coeff_count = get_significant_coeff_count_poly(operand1, operand1_coeff_count, operand1_coeff_uint64_count);
            operand2_coeff_count = get_significant_coeff_count_poly(operand2, operand2_coeff_count, operand2_coeff_uint64_count);
            for (int operand1_index = 0; operand1_index < operand1_coeff_count; ++operand1_index)
            {
                const uint64_t *operand1_coeff = get_poly_coeff(operand1, operand1_index, operand1_coeff_uint64_count);
                if (is_zero_uint(operand1_coeff, operand1_coeff_uint64_count))
                {
                    // If coefficient is 0, then move on to next coefficient.
                    continue;
                }
                // Lastly, do more expensive add if other cases don't handle it.
                for (int operand2_index = 0; operand2_index < operand2_coeff_count; ++operand2_index)
                {
                    int product_coeff_index = operand1_index + operand2_index;
                    if (product_coeff_index >= result_coeff_count)
                    {
                        break;
                    }

                    const uint64_t *operand2_coeff = get_poly_coeff(operand2, operand2_index, operand2_coeff_uint64_count);
                    if (is_zero_uint(operand2_coeff, operand2_coeff_uint64_count))
                    {
                        // If coefficient is 0, then move on to next coefficient.
                        continue;
                    }

                    multiply_uint_uint(operand1_coeff, operand1_coeff_uint64_count, operand2_coeff, operand2_coeff_uint64_count, intermediate_uint64_count, intermediate.get());
                    modulo_uint_inplace(intermediate.get(), intermediate_uint64_count, modulus, pool);
                    uint64_t *result_coeff = get_poly_coeff(result, product_coeff_index, result_coeff_uint64_count);
                    add_uint_uint_mod(result_coeff, intermediate.get(), modulusptr, result_coeff_uint64_count, result_coeff);
                }
            }
        }

        void divide_poly_poly_coeffmod_inplace(uint64_t *numerator, const uint64_t *denominator, int coeff_count, const Modulus &modulus, uint64_t *quotient, MemoryPool &pool)
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
            Pointer alloc_anchor(allocate_uint(coeff_uint64_count + intermediate_uint64_count + intermediate_uint64_count, pool));

            // Create scalar to store value that makes denominator monic.
            uint64_t *monic_denominator_scalar = alloc_anchor.get();

            // Create temporary scalars used during calculation of quotient.
            // Both are purposely twice as wide to store intermediate product prior to modulo operation.
            uint64_t *temp_quotient = monic_denominator_scalar + coeff_uint64_count;
            uint64_t *subtrahend = temp_quotient + intermediate_uint64_count;

            // Determine scalar necessary to make denominator monic.
            const uint64_t *modulusptr = modulus.get();
            const uint64_t *leading_denominator_coeff = get_poly_coeff(denominator, denominator_coeffs - 1, coeff_uint64_count);
            if (!try_invert_uint_mod(leading_denominator_coeff, modulusptr, coeff_uint64_count, monic_denominator_scalar, pool))
            {
                throw invalid_argument("coeff_modulus is not coprime with leading denominator coefficient");
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
                    multiply_uint_uint_mod_inplace(monic_denominator_scalar, leading_numerator_coeff, modulus, temp_quotient, pool);
                    set_uint_uint(temp_quotient, coeff_uint64_count, quotient_coeff);

                    // Subtract numerator and quotient*denominator (shifted by denominator_shift).
                    for (int denominator_coeff_index = 0; denominator_coeff_index < denominator_coeffs; ++denominator_coeff_index)
                    {
                        // Multiply denominator's coefficient by quotient.
                        const uint64_t *denominator_coeff = get_poly_coeff(denominator, denominator_coeff_index, coeff_uint64_count);
                        multiply_uint_uint_mod_inplace(temp_quotient, denominator_coeff, modulus, subtrahend, pool);

                        // Subtract numerator with resulting product, appropriately shifted by denominator shift.
                        uint64_t *numerator_coeff = get_poly_coeff(numerator, denominator_coeff_index + denominator_shift, coeff_uint64_count);
                        sub_uint_uint_mod(numerator_coeff, subtrahend, modulusptr, coeff_uint64_count, numerator_coeff);
                    }
                }

                // Top numerator coefficient must now be zero, so adjust coefficient count.
                numerator_coeffs--;
            }
        }

        void modulo_poly_inplace(uint64_t *value, int value_coeff_count, const PolyModulus &poly_modulus, const Modulus &modulus, MemoryPool &pool)
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
            if (poly_modulus.coeff_uint64_count() > modulus.uint64_count())
            {
                throw invalid_argument("poly_modulus coefficients cannot be wider than modulus");
            }
            for (int i = 0; i < poly_modulus.coeff_count(); i++)
            {
                if (is_greater_than_or_equal_uint_uint(poly_modulus.get() + (i * poly_modulus.coeff_uint64_count()), poly_modulus.coeff_uint64_count(),
                    modulus.get(), modulus.uint64_count()))
                {
                    throw invalid_argument("poly_modulus coefficients are not reduced modulo modulus");
                }
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

            // Widen poly_modulus to have same size coefficients as modulus
            ConstPointer polymodptr(duplicate_poly_if_needed(poly_modulus.get(), poly_modulus.coeff_count(), poly_modulus.coeff_uint64_count(),
                poly_modulus.coeff_count(), coeff_uint64_count, false, pool));

            int intermediate_uint64_count = coeff_uint64_count * 2;
            Pointer alloc_anchor(allocate_uint(coeff_uint64_count + 2 * intermediate_uint64_count, pool));

            // Create scalar to store value that makes poly_modulus monic.
            uint64_t *monic_poly_modulus_scalar = alloc_anchor.get();

            // Create temporary scalars used during calculation of quotient.
            // Both are purposely twice as wide to store intermediate product prior to modulo operation.
            uint64_t *temp_quotient = monic_poly_modulus_scalar + coeff_uint64_count;
            uint64_t *subtrahend = temp_quotient + intermediate_uint64_count;

            // Determine scalar necessary to make poly_modulus monic.
            const uint64_t *leading_poly_modulus_coeff = get_poly_coeff(polymodptr.get(), poly_modulus_coeff_count - 1, coeff_uint64_count);
            if (!try_invert_uint_mod(leading_poly_modulus_coeff, coeff_modulus, coeff_uint64_count, monic_poly_modulus_scalar, pool))
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
                    multiply_uint_uint_mod_inplace(monic_poly_modulus_scalar, leading_value_coeff, modulus, temp_quotient, pool);

                    // Subtract value and quotient*poly_modulus (shifted by poly_modulus_shift).
                    for (int poly_modulus_coeff_index = 0; poly_modulus_coeff_index < poly_modulus_coeff_count; poly_modulus_coeff_index++)
                    {
                        // Multiply poly_modulus's coefficient by quotient.
                        const uint64_t *poly_modulus_coeff = get_poly_coeff(polymodptr.get(), poly_modulus_coeff_index, coeff_uint64_count);
                        multiply_uint_uint_mod_inplace(temp_quotient, poly_modulus_coeff, modulus, subtrahend, pool);

                        // Subtract value with resulting product, appropriately shifted by poly_modulus shift.
                        uint64_t *value_coeff = get_poly_coeff(value, poly_modulus_coeff_index + poly_modulus_shift, coeff_uint64_count);
                        sub_uint_uint_mod(value_coeff, subtrahend, coeff_modulus, coeff_uint64_count, value_coeff);
                    }
                }

                // Top value coefficient must now be zero, so adjust coefficient count.
                value_coeffs--;
            }
        }

        void nonfft_multiply_poly_poly_polymod_coeffmod(const uint64_t *operand1, const uint64_t *operand2, const PolyModulus &poly_modulus, const Modulus &modulus, uint64_t *result, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
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
            if (poly_modulus.coeff_uint64_count() > modulus.uint64_count())
            {
                throw invalid_argument("poly_modulus coefficients cannot be wider than modulus");
            }
            for (int i = 0; i < poly_modulus.coeff_count(); i++)
            {
                if (is_greater_than_or_equal_uint_uint(poly_modulus.get() + (i * poly_modulus.coeff_uint64_count()), poly_modulus.coeff_uint64_count(),
                    modulus.get(), modulus.uint64_count()))
                {
                    throw invalid_argument("poly_modulus coefficients are not reduced modulo modulus");
                }
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
#ifdef SEAL_DEBUG
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
            if (poly_modulus.coeff_uint64_count() > modulus.uint64_count())
            {
                throw invalid_argument("poly_modulus coefficients cannot be wider than modulus");
            }
            for (int i = 0; i < poly_modulus.coeff_count(); i++)
            {
                if (is_greater_than_or_equal_uint_uint(poly_modulus.get() + (i * poly_modulus.coeff_uint64_count()), poly_modulus.coeff_uint64_count(),
                    modulus.get(), modulus.uint64_count()))
                {
                    throw invalid_argument("poly_modulus coefficients are not reduced modulo modulus");
                }
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
            for (int i = 0; i < coeff_count; i++)
            {
                if (is_greater_than_or_equal_uint_uint(poly_modulus + (i * modulus.uint64_count()), modulus.uint64_count(), modulus.get(), modulus.uint64_count()))
                {
                    throw invalid_argument("poly_modulus coefficients are not reduced modulo modulus");
                }
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

            // Perform extended Euclidean algorithm.
            const uint64_t *modulusptr = modulus.get();
            while (true)
            {
                // NOTE: degree(numerator) >= degree(denominator).

                // Determine scalar necessary to make denominator monic.
                const uint64_t *leading_denominator_coeff = get_poly_coeff(denominator, denominator_coeffs - 1, coeff_uint64_count);
                if (!try_invert_uint_mod(leading_denominator_coeff, modulusptr, coeff_uint64_count, monic_denominator_scalar.get(), pool))
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
                        multiply_uint_uint_mod_inplace(monic_denominator_scalar.get(), leading_numerator_coeff, modulus, temp_quotient.get(), pool);
                        set_uint_uint(temp_quotient.get(), coeff_uint64_count, quotient_coeff);

                        // Subtract numerator and quotient*denominator (shifted by denominator_shift).
                        for (int denominator_coeff_index = 0; denominator_coeff_index < denominator_coeffs; ++denominator_coeff_index)
                        {
                            // Multiply denominator's coefficient by quotient.
                            const uint64_t *denominator_coeff = get_poly_coeff(denominator, denominator_coeff_index, coeff_uint64_count);
                            multiply_uint_uint_mod_inplace(temp_quotient.get(), denominator_coeff, modulus, subtrahend.get(), pool);

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

        void poly_infty_norm_coeffmod(const uint64_t *poly, int poly_coeff_count, int poly_coeff_uint64_count, const Modulus &modulus, uint64_t *result, MemoryPool &pool)
        {
            // Construct negative threshold (first negative modulus value) to compute absolute values of coeffs.
            int modulus_uint64_count = modulus.uint64_count();
            Pointer modulus_neg_threshold(allocate_uint(modulus_uint64_count, pool));
            uint64_t *modulusthresholdptr = modulus_neg_threshold.get();

            // Set to value of (modulus + 1) / 2. To prevent overflowing with the +1, just add 1 to the result if modulus was odd.
            const uint64_t *modulusptr = modulus.get();
            half_round_up_uint(modulusptr, modulus_uint64_count, modulusthresholdptr);

            // Mod out the poly coefficients and choose a symmetric representative from [-modulus,modulus). Keep track of the max.
            set_zero_uint(modulus_uint64_count, result);
            Pointer coeff_moded(allocate_uint(modulus_uint64_count, pool));
            uint64_t *coeffmodedptr = coeff_moded.get();
            for (int coeff_index = 0; coeff_index < poly_coeff_count; ++coeff_index)
            {
                modulo_uint(poly, poly_coeff_uint64_count, modulus, coeffmodedptr, pool);
                if (is_greater_than_or_equal_uint_uint(coeffmodedptr, modulusthresholdptr, modulus_uint64_count))
                {
                    sub_uint_uint(modulusptr, coeffmodedptr, modulus_uint64_count, coeffmodedptr);
                }
                if (is_greater_than_uint_uint(coeffmodedptr, result, modulus_uint64_count))
                {
                    set_uint_uint(coeffmodedptr, modulus_uint64_count, result);
                }
                poly += poly_coeff_uint64_count;
            }
        }

        void poly_eval_poly_polymod_coeffmod(const uint64_t *poly_to_eval, const uint64_t *value, const PolyModulus &poly_modulus, const Modulus &modulus, uint64_t *result, MemoryPool &pool)
        {
            int modulus_uint64_count = modulus.uint64_count();
            int poly_modulus_coeff_count = poly_modulus.coeff_count();

#ifdef SEAL_DEBUG
            int poly_modulus_coeff_uint64_count = poly_modulus.coeff_uint64_count();
            if (poly_to_eval == nullptr)
            {
                throw invalid_argument("poly_to_eval");
            }
            if (value == nullptr)
            {
                throw invalid_argument("value");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (is_zero_uint(modulus.get(), modulus_uint64_count))
            {
                throw invalid_argument("modulus");
            }
            if (is_zero_poly(poly_modulus.get(), poly_modulus_coeff_count, poly_modulus_coeff_uint64_count))
            {
                throw invalid_argument("poly_modulus");
            }
            if (poly_modulus.coeff_uint64_count() > modulus.uint64_count())
            {
                throw invalid_argument("poly_modulus coefficients cannot be wider than modulus");
            }
            for (int i = 0; i < poly_modulus.coeff_count(); i++)
            {
                if (is_greater_than_or_equal_uint_uint(poly_modulus.get() + (i * poly_modulus.coeff_uint64_count()), poly_modulus.coeff_uint64_count(),
                    modulus.get(), modulus.uint64_count()))
                {
                    throw invalid_argument("poly_modulus coefficients are not reduced modulo modulus");
                }
            }
#endif
            // Evaluate poly at value using Horner's method
            Pointer temp1(allocate_poly(poly_modulus_coeff_count, modulus_uint64_count, pool));
            Pointer temp2(allocate_zero_poly(poly_modulus_coeff_count, modulus_uint64_count, pool));
            uint64_t *productptr = temp1.get();
            uint64_t *intermediateptr = temp2.get();

            if (poly_modulus.is_fft_modulus())
            {
                for (int coeff_index = poly_modulus_coeff_count - 1; coeff_index >= 0; --coeff_index)
                {
                    nussbaumer_multiply_poly_poly_coeffmod(intermediateptr, value, poly_modulus.coeff_count_power_of_two(), modulus, productptr, pool);
                    const uint64_t *curr_coeff = get_poly_coeff(poly_to_eval, coeff_index, modulus_uint64_count);
                    add_uint_uint_mod(productptr, curr_coeff, modulus.get(), modulus_uint64_count, productptr);
                    swap(productptr, intermediateptr);
                }
            }
            else
            {
                for (int coeff_index = poly_modulus_coeff_count - 1; coeff_index >= 0; --coeff_index)
                {
                    nonfft_multiply_poly_poly_polymod_coeffmod(intermediateptr, value, poly_modulus, modulus, productptr, pool);
                    const uint64_t *curr_coeff = get_poly_coeff(poly_to_eval, coeff_index, modulus_uint64_count);
                    add_uint_uint_mod(productptr, curr_coeff, modulus.get(), modulus_uint64_count, productptr);
                    swap(productptr, intermediateptr);
                }
            }
            set_poly_poly(intermediateptr, poly_modulus_coeff_count, modulus_uint64_count, result);
        }

        void poly_eval_uint_mod(const uint64_t *poly_to_eval, int poly_to_eval_coeff_count, const uint64_t *value, const Modulus &modulus, uint64_t *result, MemoryPool &pool)
        {
            int modulus_uint64_count = modulus.uint64_count();

#ifdef SEAL_DEBUG
            if (poly_to_eval == nullptr)
            {
                throw invalid_argument("poly_to_eval");
            }
            if (value == nullptr)
            {
                throw invalid_argument("value");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (is_zero_uint(modulus.get(), modulus_uint64_count))
            {
                throw invalid_argument("modulus");
            }
#endif
            // Evaluate poly at value using Horner's method
            Pointer temp1(allocate_uint(modulus_uint64_count, pool));
            Pointer temp2(allocate_zero_uint(modulus_uint64_count, pool));
            uint64_t *productptr = temp1.get();
            uint64_t *intermediateptr = temp2.get();

            for (int coeff_index = poly_to_eval_coeff_count - 1; coeff_index >= 0; --coeff_index)
            {
                multiply_uint_uint_mod(intermediateptr, value, modulus, productptr, pool);
                const uint64_t *curr_coeff = get_poly_coeff(poly_to_eval, coeff_index, modulus_uint64_count);
                add_uint_uint_mod(productptr, curr_coeff, modulus.get(), modulus_uint64_count, productptr);
                swap(productptr, intermediateptr);
            }
            set_uint_uint(intermediateptr, modulus_uint64_count, result);
        }

        void exponentiate_poly_polymod_coeffmod(const std::uint64_t *poly, const uint64_t *exponent, int exponent_uint64_count,
            const PolyModulus &poly_modulus, const Modulus &coeff_modulus, std::uint64_t *result, MemoryPool &pool)
        {
            int coeff_modulus_uint64_count = coeff_modulus.uint64_count();
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
            if (is_zero_uint(coeff_modulus.get(), coeff_modulus_uint64_count))
            {
                throw invalid_argument("coeff_modulus");
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
                set_zero_poly(poly_modulus_coeff_count, coeff_modulus_uint64_count, result);
                *result = 1;
                return;
            }

            modulo_poly(poly, poly_modulus_coeff_count, poly_modulus, coeff_modulus, result, pool);

            if (is_equal_uint(exponent, exponent_uint64_count, 1))
            {
                return;
            }

            // Need to make a copy of exponent
            Pointer exponent_copy(allocate_uint(exponent_uint64_count, pool));
            set_uint_uint(exponent, exponent_uint64_count, exponent_copy.get());

            // Perform binary exponentiation.
            Pointer big_alloc(allocate_poly(poly_modulus_coeff_count + poly_modulus_coeff_count + poly_modulus_coeff_count, coeff_modulus_uint64_count, pool));

            uint64_t *powerptr = big_alloc.get();
            uint64_t *productptr = get_poly_coeff(powerptr, poly_modulus_coeff_count, coeff_modulus_uint64_count);
            uint64_t *intermediateptr = get_poly_coeff(productptr, poly_modulus_coeff_count, coeff_modulus_uint64_count);

            set_poly_poly(result, poly_modulus_coeff_count, coeff_modulus_uint64_count, powerptr);
            set_zero_poly(poly_modulus_coeff_count, coeff_modulus_uint64_count, intermediateptr);
            *intermediateptr = 1;

            // Initially: power = operand and intermediate = 1, product is not initialized.
            if (poly_modulus.is_fft_modulus())
            {
                while (true)
                {
                    if ((*exponent_copy.get() % 2) == 1)
                    {
                        nussbaumer_multiply_poly_poly_coeffmod(powerptr, intermediateptr, poly_modulus.coeff_count_power_of_two(), coeff_modulus, productptr, pool);
                        swap(productptr, intermediateptr);
                    }
                    right_shift_uint(exponent_copy.get(), 1, exponent_uint64_count, exponent_copy.get());
                    if (is_zero_uint(exponent_copy.get(), exponent_uint64_count))
                    {
                        break;
                    }
                    nussbaumer_multiply_poly_poly_coeffmod(powerptr, powerptr, poly_modulus.coeff_count_power_of_two(), coeff_modulus, productptr, pool);
                    swap(productptr, powerptr);
                }
            }
            else
            {
                while (true)
                {
                    if ((*exponent_copy.get() % 2) == 1)
                    {
                        nonfft_multiply_poly_poly_polymod_coeffmod(powerptr, intermediateptr, poly_modulus, coeff_modulus, productptr, pool);
                        swap(productptr, intermediateptr);
                    }
                    right_shift_uint(exponent_copy.get(), 1, exponent_uint64_count, exponent_copy.get());
                    if (is_zero_uint(exponent_copy.get(), exponent_uint64_count))
                    {
                        break;
                    }
                    nonfft_multiply_poly_poly_polymod_coeffmod(powerptr, powerptr, poly_modulus, coeff_modulus, productptr, pool);
                    swap(productptr, powerptr);
                }
            }
            set_poly_poly(intermediateptr, poly_modulus_coeff_count, coeff_modulus_uint64_count, result);
        }
    }
}
