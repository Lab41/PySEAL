#include "util/polyextras.h"
#include "util/uintarith.h"
#include "util/polyarith.h"
#include "util/uintarithmod.h"
#include "util/polyarithmod.h"

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
            Pointer temp2(allocate_poly(result_coeff_count, result_coeff_uint64_count, pool));
            uint64_t *productptr = temp1.get();
            uint64_t *intermediateptr = temp2.get();
            set_zero_poly(result_coeff_count, result_coeff_uint64_count, intermediateptr);
            for (int coeff_index = poly_to_eval_coeff_count - 1; coeff_index >= 0; --coeff_index)
            {
                multiply_poly_poly(intermediateptr, result_coeff_count, result_coeff_uint64_count, value, value_coeff_count, value_coeff_uint64_count, result_coeff_count, result_coeff_uint64_count, productptr, pool);
                const uint64_t *curr_coeff = get_poly_coeff(poly_to_eval, coeff_index, poly_to_eval_coeff_uint64_count);
                add_uint_uint(productptr, result_coeff_uint64_count, curr_coeff, poly_to_eval_coeff_uint64_count, false, result_coeff_uint64_count, productptr);
                swap(productptr, intermediateptr);
            }
            set_poly_poly(intermediateptr, result_coeff_count, result_coeff_uint64_count, result);
        }

        void exponentiate_poly(const std::uint64_t *poly, int poly_coeff_count, int poly_coeff_uint64_count, int exponent,
            int result_coeff_count, int result_coeff_uint64_count, std::uint64_t *result, MemoryPool &pool)
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
            if (exponent < 0)
            {
                throw out_of_range("exponent");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (result_coeff_count <= 0)
            {
                throw out_of_range("result_coeff_count");
            }
            if (result_coeff_uint64_count <= 0)
            {
                throw out_of_range("result_coeff_uint64_count");
            }
#endif
            // Fast cases
            if (exponent == 0)
            {
                set_zero_poly(result_coeff_count, result_coeff_uint64_count, result);
                *result = 1;
                return;
            }
            if (exponent == 1)
            {
                set_poly_poly(poly, poly_coeff_count, poly_coeff_uint64_count, result_coeff_count, result_coeff_uint64_count, result);
                return;
            }

            // Perform binary exponentiation.
            Pointer power(allocate_poly(result_coeff_count, result_coeff_uint64_count, pool));
            Pointer temp1(allocate_poly(result_coeff_count, result_coeff_uint64_count, pool));
            Pointer temp2(allocate_poly(result_coeff_count, result_coeff_uint64_count, pool));
            uint64_t *powerptr = power.get();
            uint64_t *productptr = temp1.get();
            uint64_t *intermediateptr = temp2.get();
            set_poly_poly(poly, poly_coeff_count, poly_coeff_uint64_count, result_coeff_count, result_coeff_uint64_count, powerptr);
            set_zero_poly(result_coeff_count, result_coeff_uint64_count, intermediateptr);
            *intermediateptr = 1;
            // Initially: power = operand and intermediate = 1, product is not initialized.
            while (true)
            {
                if ((exponent % 2) == 1)
                {
                    multiply_poly_poly(powerptr, result_coeff_count, result_coeff_uint64_count, intermediateptr, result_coeff_count, result_coeff_uint64_count,
                        result_coeff_count, result_coeff_uint64_count, productptr, pool);
                    swap(productptr, intermediateptr);
                }
                exponent >>= 1;
                if (exponent == 0)
                {
                    break;
                }
                multiply_poly_poly(powerptr, result_coeff_count, result_coeff_uint64_count, powerptr, result_coeff_count, result_coeff_uint64_count,
                    result_coeff_count, result_coeff_uint64_count, productptr, pool);
                swap(productptr, powerptr);
            }
            set_poly_poly(intermediateptr, result_coeff_count, result_coeff_uint64_count, result);
        }

        void exponentiate_poly(const std::uint64_t *poly, int exponent, const PolyModulus &poly_modulus, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool)
        {
#ifdef _DEBUG
            if (poly == nullptr)
            {
                throw invalid_argument("poly");
            }
            if (exponent < 0)
            {
                throw out_of_range("exponent");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
#endif
            // Fast cases
            int coeff_count = poly_modulus.coeff_count();
            int coeff_uint64_count = poly_modulus.coeff_uint64_count();
            if (exponent == 0)
            {
                set_zero_poly(coeff_count, coeff_uint64_count, result);
                *result = 1;
                return;
            }
            if (exponent == 1)
            {
                set_poly_poly(poly, coeff_count, coeff_uint64_count, result);
                return;
            }

            // Perform binary exponentiation.
            Pointer power(allocate_poly(coeff_count, coeff_uint64_count, pool));
            Pointer temp1(allocate_poly(coeff_count, coeff_uint64_count, pool));
            Pointer temp2(allocate_poly(coeff_count, coeff_uint64_count, pool));
            uint64_t *powerptr = power.get();
            uint64_t *productptr = temp1.get();
            uint64_t *intermediateptr = temp2.get();
            set_poly_poly(poly, coeff_count, coeff_uint64_count, powerptr);
            set_zero_poly(coeff_count, coeff_uint64_count, intermediateptr);
            *intermediateptr = 1;
            // Initially: power = operand and intermediate = 1, product is not initialized.
            while (true)
            {
                if ((exponent % 2) == 1)
                {
                    multiply_poly_poly_polymod_coeffmod(powerptr, intermediateptr, poly_modulus, modulus, productptr, pool);
                    swap(productptr, intermediateptr);
                }
                exponent >>= 1;
                if (exponent == 0)
                {
                    break;
                }
                multiply_poly_poly_polymod_coeffmod(powerptr, powerptr, poly_modulus, modulus, productptr, pool);
                swap(productptr, powerptr);
            }
            set_poly_poly(intermediateptr, coeff_count, coeff_uint64_count, result);
        }
    }
}