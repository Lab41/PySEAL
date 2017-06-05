#include "util/polyextras.h"
#include "util/uintarith.h"
#include "util/polyarith.h"
#include "util/uintarithmod.h"
#include "util/polyarithmod.h"
#include "util/polyfftmultmod.h"

using namespace std;

namespace seal
{
    namespace util
    {
        void poly_infty_norm(const uint64_t *poly, int coeff_count, int coeff_uint64_count, uint64_t *result)
        {
            set_zero_uint(coeff_uint64_count, result);
            for (int coeff_index = 0; coeff_index < coeff_count; ++coeff_index)
            {
                if (is_greater_than_uint_uint(poly, result, coeff_uint64_count))
                {
                    set_uint_uint(poly, coeff_uint64_count, result);
                }

                poly += coeff_uint64_count;
            }
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
            Pointer big_alloc(allocate_uint(3 * poly_coeff_uint64_count, pool));
            for (int coeff_index = 0; coeff_index < poly_coeff_count; ++coeff_index)
            {
                modulo_uint(poly, poly_coeff_uint64_count, modulus, coeffmodedptr, pool, big_alloc.get());
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

        void poly_eval_poly(const uint64_t *poly_to_eval, int poly_to_eval_coeff_count, int poly_to_eval_coeff_uint64_count, const uint64_t *value,
            int value_coeff_count, int value_coeff_uint64_count, int result_coeff_count, int result_coeff_uint64_count, uint64_t *result, MemoryPool &pool)
        {
#ifdef _DEBUG
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
            if (poly_to_eval_coeff_count <= 0)
            {
                throw invalid_argument("poly_to_eval_coeff_count");
            }
            if (poly_to_eval_coeff_uint64_count <= 0)
            {
                throw invalid_argument("poly_to_eval_coeff_uint64_count");
            }
            if (value_coeff_count <= 0)
            {
                throw invalid_argument("value_coeff_count");
            }
            if (value_coeff_uint64_count <= 0)
            {
                throw invalid_argument("value_coeff_uint64_count");
            }
            if (result_coeff_count <= 0)
            {
                throw invalid_argument("result_coeff_count");
            }
            if (result_coeff_uint64_count <= 0)
            {
                throw invalid_argument("result_coeff_uint64_count");
            }
#endif

            // Evaluate poly at value using Horner's method
            Pointer temp1(allocate_poly(result_coeff_count, result_coeff_uint64_count, pool));
            Pointer temp2(allocate_zero_poly(result_coeff_count, result_coeff_uint64_count, pool));
            uint64_t *productptr = temp1.get();
            uint64_t *intermediateptr = temp2.get();
            
            const uint64_t *curr_coeff;
            for (int coeff_index = poly_to_eval_coeff_count - 1; coeff_index >= 0; --coeff_index)
            {
                multiply_poly_poly(intermediateptr, result_coeff_count, result_coeff_uint64_count, value, value_coeff_count, value_coeff_uint64_count, result_coeff_count, result_coeff_uint64_count, productptr, pool);
                curr_coeff = get_poly_coeff(poly_to_eval, coeff_index, poly_to_eval_coeff_uint64_count);
                add_uint_uint(productptr, result_coeff_uint64_count, curr_coeff, poly_to_eval_coeff_uint64_count, false, result_coeff_uint64_count, productptr);
                swap(productptr, intermediateptr);
            }
            set_poly_poly(intermediateptr, result_coeff_count, result_coeff_uint64_count, result);
        }

        void poly_eval_poly_polymod_coeffmod(const uint64_t *poly_to_eval, const uint64_t *value, const PolyModulus &poly_modulus, const Modulus &coeff_modulus, uint64_t *result, MemoryPool &pool)
        {
            int coeff_modulus_uint64_count = coeff_modulus.uint64_count();
            int poly_modulus_coeff_count = poly_modulus.coeff_count();

#ifdef _DEBUG
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
            if (is_zero_uint(coeff_modulus.get(), coeff_modulus_uint64_count))
            {
                throw invalid_argument("coeff_modulus");
            }
            if (is_zero_poly(poly_modulus.get(), poly_modulus_coeff_count, poly_modulus_coeff_uint64_count))
            {
                throw invalid_argument("poly_modulus");
            }
#endif
            // Evaluate poly at value using Horner's method
            Pointer temp1(allocate_poly(poly_modulus_coeff_count, coeff_modulus_uint64_count, pool));
            Pointer temp2(allocate_zero_poly(poly_modulus_coeff_count, coeff_modulus_uint64_count, pool));
            uint64_t *productptr = temp1.get();
            uint64_t *intermediateptr = temp2.get();

            const uint64_t *curr_coeff;
            if (poly_modulus.is_fft_modulus())
            {
                for (int coeff_index = poly_modulus_coeff_count - 1; coeff_index >= 0; --coeff_index)
                {
                    nussbaumer_multiply_poly_poly_coeffmod(intermediateptr, value, poly_modulus.coeff_count_power_of_two(), coeff_modulus, productptr, pool);
                    curr_coeff = get_poly_coeff(poly_to_eval, coeff_index, coeff_modulus_uint64_count);
                    add_uint_uint_mod(productptr, curr_coeff, coeff_modulus.get(), coeff_modulus_uint64_count, productptr);
                    swap(productptr, intermediateptr);
                }
            }
            else
            {
                for (int coeff_index = poly_modulus_coeff_count - 1; coeff_index >= 0; --coeff_index)
                {
                    nonfft_multiply_poly_poly_polymod_coeffmod(intermediateptr, value, poly_modulus, coeff_modulus, productptr, pool);
                    curr_coeff = get_poly_coeff(poly_to_eval, coeff_index, coeff_modulus_uint64_count);
                    add_uint_uint_mod(productptr, curr_coeff, coeff_modulus.get(), coeff_modulus_uint64_count, productptr);
                    swap(productptr, intermediateptr);
                }
            }
            set_poly_poly(intermediateptr, poly_modulus_coeff_count, coeff_modulus_uint64_count, result);
        }

        void poly_eval_uint_mod(const uint64_t *poly_to_eval, int poly_to_eval_coeff_count, const uint64_t *value, const Modulus &modulus, uint64_t *result, MemoryPool &pool)
        {
            int modulus_uint64_count = modulus.uint64_count();

#ifdef _DEBUG
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

            const uint64_t *curr_coeff;
            for (int coeff_index = poly_to_eval_coeff_count - 1; coeff_index >= 0; --coeff_index)
            {
                multiply_uint_uint_mod(intermediateptr, value, modulus, productptr, pool);
                curr_coeff = get_poly_coeff(poly_to_eval, coeff_index, modulus_uint64_count);
                add_uint_uint_mod(productptr, curr_coeff, modulus.get(), modulus_uint64_count, productptr);
                swap(productptr, intermediateptr);
            }
            set_uint_uint(intermediateptr, modulus_uint64_count, result);
        }

        void exponentiate_poly(const std::uint64_t *poly, int poly_coeff_count, int poly_coeff_uint64_count, const uint64_t *exponent,
            int exponent_uint64_count, int result_coeff_count, int result_coeff_uint64_count, std::uint64_t *result, MemoryPool &pool)
        {
#ifdef _DEBUG
            if (poly == nullptr)
            {
                throw invalid_argument("poly");
            }
            if (poly_coeff_count <= 0)
            {
                throw invalid_argument("poly_coeff_count");
            }
            if (poly_coeff_count <= 0)
            {
                throw invalid_argument("poly_coeff_uint64_count");
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
            if (result_coeff_count <= 0)
            {
                throw invalid_argument("result_coeff_count");
            }
            if (result_coeff_uint64_count <= 0)
            {
                throw invalid_argument("result_coeff_uint64_count");
            }
#endif
            // Fast cases
            if (is_zero_uint(exponent, exponent_uint64_count))
            {
                set_zero_poly(result_coeff_count, result_coeff_uint64_count, result);
                *result = 1;
                return;
            }
            if (is_equal_uint(exponent, exponent_uint64_count, 1))
            {
                set_poly_poly(poly, poly_coeff_count, poly_coeff_uint64_count, result_coeff_count, result_coeff_uint64_count, result);
                return;
            }

            // Need to make a copy of exponent
            Pointer exponent_copy(allocate_uint(exponent_uint64_count, pool));
            set_uint_uint(exponent, exponent_uint64_count, exponent_copy.get());

            // Perform binary exponentiation.
            Pointer big_alloc(allocate_poly(result_coeff_count + result_coeff_count + result_coeff_count, result_coeff_uint64_count, pool));

            uint64_t *powerptr = big_alloc.get();
            uint64_t *productptr = get_poly_coeff(powerptr, result_coeff_count, result_coeff_uint64_count);
            uint64_t *intermediateptr = get_poly_coeff(productptr, result_coeff_count, result_coeff_uint64_count);

            set_poly_poly(poly, poly_coeff_count, poly_coeff_uint64_count, result_coeff_count, result_coeff_uint64_count, powerptr);
            set_zero_poly(result_coeff_count, result_coeff_uint64_count, intermediateptr);
            *intermediateptr = 1;

            // Initially: power = operand and intermediate = 1, product is not initialized.
            while (true)
            {
                if ((*exponent_copy.get() % 2) == 1)
                {
                    multiply_poly_poly(powerptr, result_coeff_count, result_coeff_uint64_count, intermediateptr, result_coeff_count, result_coeff_uint64_count,
                        result_coeff_count, result_coeff_uint64_count, productptr, pool);
                    swap(productptr, intermediateptr);
                }
                right_shift_uint(exponent_copy.get(), 1, exponent_uint64_count, exponent_copy.get());
                if (is_zero_uint(exponent_copy.get(), exponent_uint64_count))
                {
                    break;
                }
                multiply_poly_poly(powerptr, result_coeff_count, result_coeff_uint64_count, powerptr, result_coeff_count, result_coeff_uint64_count,
                    result_coeff_count, result_coeff_uint64_count, productptr, pool);
                swap(productptr, powerptr);
            }
            set_poly_poly(intermediateptr, result_coeff_count, result_coeff_uint64_count, result);
        }

        void exponentiate_poly_polymod_coeffmod(const std::uint64_t *poly, const uint64_t *exponent, int exponent_uint64_count,
            const PolyModulus &poly_modulus, const Modulus &coeff_modulus, std::uint64_t *result, MemoryPool &pool)
        {
            int coeff_modulus_uint64_count = coeff_modulus.uint64_count();
            int poly_modulus_coeff_count = poly_modulus.coeff_count();

#ifdef _DEBUG
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