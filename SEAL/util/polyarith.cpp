#include "util/uintcore.h"
#include "util/uintarith.h"
#include "util/uintarithmod.h"
#include "util/polycore.h"
#include "util/polyarith.h"

using namespace std;

namespace seal
{
    namespace util
    {

        void rightshift_poly_coeffs(uint64_t *poly, int coeff_count, int coeff_uint64_count, int shift_amount, MemoryPool &pool)
        {
            Pointer temp(allocate_uint(coeff_uint64_count, pool));
            for (int j = 0; j < coeff_count; j++)
            {
                right_shift_uint(poly, shift_amount, coeff_uint64_count, temp.get());
                set_uint_uint(temp.get(), coeff_uint64_count, poly);
                poly += coeff_uint64_count;
            }
        }

        void modulo_poly_coeffs_uint64(uint64_t *poly, int coeff_count, uint64_t mod, MemoryPool &pool)
        {
            for (int i = 0; i < coeff_count; ++i)
            {
                *poly %= mod;
                poly += 1;
            }
        }


        void modulo_poly_coeffs(uint64_t *poly, int coeff_count, const Modulus &modulus, MemoryPool &pool)
        {
#ifdef _DEBUG
            if (poly == nullptr && coeff_count > 0)
            {
                throw invalid_argument("poly");
            }
            if (coeff_count < 0)
            {
                throw invalid_argument("coeff_count");
            }
#endif

            int coeff_uint64_count = modulus.uint64_count();
            Pointer big_alloc(allocate_uint(2 * coeff_uint64_count, pool));
            for (int i = 0; i < coeff_count; ++i)
            {
                modulo_uint_inplace(poly, coeff_uint64_count, modulus, pool, big_alloc.get());
                poly += coeff_uint64_count;
            }
        }

        void negate_poly(const uint64_t *poly, int coeff_count, int coeff_uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (poly == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw invalid_argument("poly");
            }
            if (coeff_count < 0)
            {
                throw invalid_argument("coeff_count");
            }
            if (coeff_uint64_count < 0)
            {
                throw invalid_argument("coeff_uint64_count");
            }
            if (result == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw invalid_argument("result");
            }
#endif
            for (int i = 0; i < coeff_count; ++i)
            {
                negate_uint(poly, coeff_uint64_count, result);
                poly += coeff_uint64_count;
                result += coeff_uint64_count;
            }
        }

        void negate_poly_coeffmod(const uint64_t *poly, int coeff_count, const uint64_t *coeff_modulus, int coeff_uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (poly == nullptr && coeff_count > 0)
            {
                throw invalid_argument("poly");
            }
            if (coeff_count < 0)
            {
                throw invalid_argument("coeff_count");
            }
            if (coeff_modulus == nullptr)
            {
                throw invalid_argument("coeff_modulus");
            }
            if (coeff_uint64_count <= 0)
            {
                throw invalid_argument("coeff_uint64_count");
            }
            if (result == nullptr && coeff_count > 0)
            {
                throw invalid_argument("result");
            }
#endif
            for (int i = 0; i < coeff_count; ++i)
            {
                negate_uint_mod(poly, coeff_modulus, coeff_uint64_count, result);
                poly += coeff_uint64_count;
                result += coeff_uint64_count;
            }
        }

        void add_poly_poly(const uint64_t *operand1, const uint64_t *operand2, int coeff_count, int coeff_uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand1 == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw invalid_argument("operand1");
            }
            if (operand2 == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw invalid_argument("operand2");
            }
            if (coeff_count < 0)
            {
                throw invalid_argument("coeff_count");
            }
            if (coeff_uint64_count < 0)
            {
                throw invalid_argument("coeff_uint64_count");
            }
            if (result == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw invalid_argument("result");
            }
#endif
            for (int i = 0; i < coeff_count; ++i)
            {
                add_uint_uint(operand1, operand2, coeff_uint64_count, result);
                operand1 += coeff_uint64_count;
                operand2 += coeff_uint64_count;
                result += coeff_uint64_count;
            }
        }

        void sub_poly_poly(const uint64_t *operand1, const uint64_t *operand2, int coeff_count, int coeff_uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand1 == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw invalid_argument("operand1");
            }
            if (operand2 == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw invalid_argument("operand2");
            }
            if (coeff_count < 0)
            {
                throw invalid_argument("coeff_count");
            }
            if (coeff_uint64_count < 0)
            {
                throw invalid_argument("coeff_uint64_count");
            }
            if (result == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw invalid_argument("result");
            }
#endif
            for (int i = 0; i < coeff_count; ++i)
            {
                sub_uint_uint(operand1, operand2, coeff_uint64_count, result);
                operand1 += coeff_uint64_count;
                operand2 += coeff_uint64_count;
                result += coeff_uint64_count;
            }
        }

        void add_poly_poly_coeffmod(const uint64_t *operand1, const uint64_t *operand2, int coeff_count, const uint64_t *coeff_modulus, int coeff_uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
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
            if (coeff_modulus == nullptr)
            {
                throw invalid_argument("coeff_modulus");
            }
            if (coeff_uint64_count <= 0)
            {
                throw invalid_argument("coeff_uint64_count");
            }
            if (result == nullptr && coeff_count > 0)
            {
                throw invalid_argument("result");
            }
#endif
            for (int i = 0; i < coeff_count; ++i)
            {
                add_uint_uint_mod(operand1, operand2, coeff_modulus, coeff_uint64_count, result);
                operand1 += coeff_uint64_count;
                operand2 += coeff_uint64_count;
                result += coeff_uint64_count;
            }
        }

        void sub_poly_poly_coeffmod(const uint64_t *operand1, const uint64_t *operand2, int coeff_count, const uint64_t *coeff_modulus, int coeff_uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
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
            if (coeff_modulus == nullptr)
            {
                throw invalid_argument("coeff_modulus");
            }
            if (coeff_uint64_count <= 0)
            {
                throw invalid_argument("coeff_uint64_count");
            }
            if (result == nullptr && coeff_count > 0)
            {
                throw invalid_argument("result");
            }
#endif
            for (int i = 0; i < coeff_count; ++i)
            {
                sub_uint_uint_mod(operand1, operand2, coeff_modulus, coeff_uint64_count, result);
                operand1 += coeff_uint64_count;
                operand2 += coeff_uint64_count;
                result += coeff_uint64_count;
            }
        }

        void multiply_poly_scalar_coeffmod(const uint64_t *poly, int coeff_count, const uint64_t *scalar, const Modulus &modulus, uint64_t *result, MemoryPool &pool)
        {
#ifdef _DEBUG
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
            Pointer big_alloc(allocate_uint(4 * coeff_uint64_count, pool));
            for (int i = 0; i < coeff_count; ++i)
            {
                multiply_uint_uint_mod_inplace(poly, scalar, modulus, intermediate.get(), pool, big_alloc.get());
                set_uint_uint(intermediate.get(), coeff_uint64_count, result);
                poly += coeff_uint64_count;
                result += coeff_uint64_count;
            }
        }

        void multiply_poly_poly(const uint64_t *operand1, int operand1_coeff_count, int operand1_coeff_uint64_count, const uint64_t *operand2, int operand2_coeff_count, int operand2_coeff_uint64_count,
            int result_coeff_count, int result_coeff_uint64_count, uint64_t *result, MemoryPool &pool)
        {
#ifdef _DEBUG
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
            if (result_coeff_uint64_count < 0)
            {
                throw invalid_argument("result_coeff_uint64_count");
            }
            if (result == nullptr && result_coeff_count > 0 && result_coeff_uint64_count > 0)
            {
                throw invalid_argument("result");
            }
            if (result != nullptr && (operand1 == result || operand2 == result))
            {
                throw invalid_argument("result cannot point to the same value as operand1 or operand2");
            }
#endif
            Pointer intermediate(allocate_uint(result_coeff_uint64_count, pool));

            // Clear product.
            set_zero_poly(result_coeff_count, result_coeff_uint64_count, result);

            operand1_coeff_count = get_significant_coeff_count_poly(operand1, operand1_coeff_count, operand1_coeff_uint64_count);
            operand2_coeff_count = get_significant_coeff_count_poly(operand2, operand2_coeff_count, operand2_coeff_uint64_count);
            for (int operand1_index = 0; operand1_index < operand1_coeff_count; ++operand1_index)
            {
                const uint64_t *operand1_coeff = get_poly_coeff(operand1, operand1_index, operand1_coeff_uint64_count);
                for (int operand2_index = 0; operand2_index < operand2_coeff_count; ++operand2_index)
                {
                    int product_coeff_index = operand1_index + operand2_index;
                    if (product_coeff_index >= result_coeff_count)
                    {
                        break;
                    }

                    const uint64_t *operand2_coeff = get_poly_coeff(operand2, operand2_index, operand2_coeff_uint64_count);
                    multiply_uint_uint(operand1_coeff, operand1_coeff_uint64_count, operand2_coeff, operand2_coeff_uint64_count, result_coeff_uint64_count, intermediate.get());
                    uint64_t *result_coeff = get_poly_coeff(result, product_coeff_index, result_coeff_uint64_count);
                    add_uint_uint(result_coeff, intermediate.get(), result_coeff_uint64_count, result_coeff);
                }
            }
        }

        void multiply_poly_poly_coeffmod(const uint64_t *operand1, int operand1_coeff_count, int operand1_coeff_uint64_count, const uint64_t *operand2, int operand2_coeff_count, int operand2_coeff_uint64_count,
            const Modulus &modulus, int result_coeff_count, uint64_t *result, MemoryPool &pool)
        {
#ifdef _DEBUG
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
            Pointer big_alloc(allocate_uint(intermediate_uint64_count + 2 * intermediate_uint64_count, pool));
            uint64_t *intermediate = big_alloc.get();
            uint64_t *alloc_ptr = intermediate + intermediate_uint64_count;

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

                    multiply_uint_uint(operand1_coeff, operand1_coeff_uint64_count, operand2_coeff, operand2_coeff_uint64_count, intermediate_uint64_count, intermediate);
                    modulo_uint_inplace(intermediate, intermediate_uint64_count, modulus, pool, alloc_ptr);
                    uint64_t *result_coeff = get_poly_coeff(result, product_coeff_index, result_coeff_uint64_count);
                    add_uint_uint_mod(result_coeff, intermediate, modulusptr, result_coeff_uint64_count, result_coeff);
                }
            }
        }

        void divide_poly_poly_coeffmod_inplace(uint64_t *numerator, const uint64_t *denominator, int coeff_count, const Modulus &modulus, uint64_t *quotient, MemoryPool &pool)
        {
#ifdef _DEBUG
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
            Pointer big_alloc(allocate_uint(coeff_uint64_count + intermediate_uint64_count + intermediate_uint64_count + 7 * coeff_uint64_count, pool));

            // Create scalar to store value that makes denominator monic.
            uint64_t *monic_denominator_scalar = big_alloc.get();

            // Create temporary scalars used during calculation of quotient.
            // Both are purposely twice as wide to store intermediate product prior to modulo operation.
            uint64_t *temp_quotient = monic_denominator_scalar + coeff_uint64_count;
            uint64_t *subtrahend = temp_quotient + intermediate_uint64_count;

            // We still have 7 x coeff_uint64_count of memory left in the big allocation
            uint64_t *alloc_ptr = subtrahend + intermediate_uint64_count;

            // Determine scalar necessary to make denominator monic.
            const uint64_t *modulusptr = modulus.get();
            const uint64_t *leading_denominator_coeff = get_poly_coeff(denominator, denominator_coeffs - 1, coeff_uint64_count);
            if (!try_invert_uint_mod(leading_denominator_coeff, modulusptr, coeff_uint64_count, monic_denominator_scalar, pool, alloc_ptr))
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
                    multiply_uint_uint_mod_inplace(monic_denominator_scalar, leading_numerator_coeff, modulus, temp_quotient, pool, alloc_ptr);
                    set_uint_uint(temp_quotient, coeff_uint64_count, quotient_coeff);

                    // Subtract numerator and quotient*denominator (shifted by denominator_shift).
                    for (int denominator_coeff_index = 0; denominator_coeff_index < denominator_coeffs; ++denominator_coeff_index)
                    {
                        // Multiply denominator's coefficient by quotient.
                        const uint64_t *denominator_coeff = get_poly_coeff(denominator, denominator_coeff_index, coeff_uint64_count);
                        multiply_uint_uint_mod_inplace(temp_quotient, denominator_coeff, modulus, subtrahend, pool, alloc_ptr);

                        // Subtract numerator with resulting product, appropriately shifted by denominator shift.
                        uint64_t *numerator_coeff = get_poly_coeff(numerator, denominator_coeff_index + denominator_shift, coeff_uint64_count);
                        sub_uint_uint_mod(numerator_coeff, subtrahend, modulusptr, coeff_uint64_count, numerator_coeff);
                    }
                }

                // Top numerator coefficient must now be zero, so adjust coefficient count.
                numerator_coeffs--;
            }
        }

        void add_bigpolyarray_coeffmod(const std::uint64_t *array1, const std::uint64_t *array2, int count, int coeff_count, const Modulus &modulus, std::uint64_t *result)
        {
            // Check validity of inputs
#ifdef _DEBUG
            if (array1 == nullptr)
            {
                throw invalid_argument("array1");
            }
            if (array2 == nullptr)
            {
                throw invalid_argument("array2");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (count < 1)
            {
                throw invalid_argument("count");
            }
            if (coeff_count < 1)
            {
                throw invalid_argument("coeff_count");
            }
#endif
            // Calculate pointer increment
            int coeff_bit_count = modulus.significant_bit_count();
            int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
            int poly_ptr_increment = coeff_count * coeff_uint64_count;

            // initialise pointers for addition
            const uint64_t *current_array1 = array1;
            const uint64_t *current_array2 = array2;
            uint64_t *current_result = result;
     
            for (int i = 0; i < count; ++i)
            {
                add_poly_poly_coeffmod(current_array1, current_array2, coeff_count, modulus.get(), coeff_uint64_count, current_result);
                current_array1 += poly_ptr_increment;
                current_array2 += poly_ptr_increment;
                current_result += poly_ptr_increment;
            }
        }

    }
}
