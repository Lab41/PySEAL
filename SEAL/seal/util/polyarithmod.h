#pragma once

#include <cstdint>
#include "seal/util/modulus.h"
#include "seal/util/mempool.h"
#include "seal/util/polymodulus.h"
#include "seal/util/polycore.h"
#include "seal/util/uintarithmod.h"

namespace seal
{
    namespace util
    {
        inline void modulo_poly_coeffs(std::uint64_t *poly, int coeff_count, const Modulus &modulus, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
            if (poly == nullptr && coeff_count > 0)
            {
                throw std::invalid_argument("poly");
            }
            if (coeff_count < 0)
            {
                throw std::invalid_argument("coeff_count");
            }
#endif
            int coeff_uint64_count = modulus.uint64_count();
            for (int i = 0; i < coeff_count; i++)
            {
                modulo_uint_inplace(poly, coeff_uint64_count, modulus, pool);
                poly += coeff_uint64_count;
            }
        }

        inline void negate_poly_coeffmod(const std::uint64_t *poly, int coeff_count, const std::uint64_t *coeff_modulus, int coeff_uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (poly == nullptr && coeff_count > 0)
            {
                throw std::invalid_argument("poly");
            }
            if (coeff_count < 0)
            {
                throw std::invalid_argument("coeff_count");
            }
            if (coeff_modulus == nullptr)
            {
                throw std::invalid_argument("coeff_modulus");
            }
            if (coeff_uint64_count <= 0)
            {
                throw std::invalid_argument("coeff_uint64_count");
            }
            if (result == nullptr && coeff_count > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            for (int i = 0; i < coeff_count; i++)
            {
                negate_uint_mod(poly, coeff_modulus, coeff_uint64_count, result);
                poly += coeff_uint64_count;
                result += coeff_uint64_count;
            }
        }

        inline void add_poly_poly_coeffmod(const std::uint64_t *operand1, const std::uint64_t *operand2, int coeff_count, const std::uint64_t *coeff_modulus, int coeff_uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr && coeff_count > 0)
            {
                throw std::invalid_argument("operand1");
            }
            if (operand2 == nullptr && coeff_count > 0)
            {
                throw std::invalid_argument("operand2");
            }
            if (coeff_count < 0)
            {
                throw std::invalid_argument("coeff_count");
            }
            if (coeff_modulus == nullptr)
            {
                throw std::invalid_argument("coeff_modulus");
            }
            if (coeff_uint64_count <= 0)
            {
                throw std::invalid_argument("coeff_uint64_count");
            }
            if (result == nullptr && coeff_count > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            for (int i = 0; i < coeff_count; i++)
            {
                add_uint_uint_mod(operand1, operand2, coeff_modulus, coeff_uint64_count, result);
                operand1 += coeff_uint64_count;
                operand2 += coeff_uint64_count;
                result += coeff_uint64_count;
            }
        }

        inline void sub_poly_poly_coeffmod(const std::uint64_t *operand1, const std::uint64_t *operand2, int coeff_count, const std::uint64_t *coeff_modulus, int coeff_uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr && coeff_count > 0)
            {
                throw std::invalid_argument("operand1");
            }
            if (operand2 == nullptr && coeff_count > 0)
            {
                throw std::invalid_argument("operand2");
            }
            if (coeff_count < 0)
            {
                throw std::invalid_argument("coeff_count");
            }
            if (coeff_modulus == nullptr)
            {
                throw std::invalid_argument("coeff_modulus");
            }
            if (coeff_uint64_count <= 0)
            {
                throw std::invalid_argument("coeff_uint64_count");
            }
            if (result == nullptr && coeff_count > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            for (int i = 0; i < coeff_count; i++)
            {
                sub_uint_uint_mod(operand1, operand2, coeff_modulus, coeff_uint64_count, result);
                operand1 += coeff_uint64_count;
                operand2 += coeff_uint64_count;
                result += coeff_uint64_count;
            }
        }

        void multiply_poly_scalar_coeffmod(const std::uint64_t *poly, int coeff_count, const std::uint64_t *scalar, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool);

        void multiply_poly_poly_coeffmod(const std::uint64_t *operand1, int operand1_coeff_count, int operand1_coeff_uint64_count, const std::uint64_t *operand2, int operand2_coeff_count, int operand2_coeff_uint64_count,
            const Modulus &modulus, int result_coeff_count, std::uint64_t *result, MemoryPool &pool);

        inline void multiply_poly_poly_coeffmod(const std::uint64_t *operand1, const std::uint64_t *operand2, int coeff_count, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool)
        {
            int result_coeff_count = coeff_count + coeff_count - 1;
            int coeff_uint64_count = modulus.uint64_count();
            multiply_poly_poly_coeffmod(operand1, coeff_count, coeff_uint64_count, operand2, coeff_count, coeff_uint64_count, modulus, result_coeff_count, result, pool);
        }

        inline void multiply_truncate_poly_poly_coeffmod(const std::uint64_t *operand1, const std::uint64_t *operand2, int coeff_count, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool)
        {
            int coeff_uint64_count = modulus.uint64_count();
            multiply_poly_poly_coeffmod(operand1, coeff_count, coeff_uint64_count, operand2, coeff_count, coeff_uint64_count, modulus, coeff_count, result, pool);
        }

        void divide_poly_poly_coeffmod_inplace(std::uint64_t *numerator, const std::uint64_t *denominator, int coeff_count, const Modulus &modulus, std::uint64_t *quotient, MemoryPool &pool);

        inline void divide_poly_poly_coeffmod(const std::uint64_t *numerator, const std::uint64_t *denominator, int coeff_count, const Modulus &modulus, std::uint64_t *quotient, std::uint64_t *remainder, MemoryPool &pool)
        {
            int coeff_uint64_count = modulus.uint64_count();
            set_poly_poly(numerator, coeff_count, coeff_uint64_count, remainder);
            divide_poly_poly_coeffmod_inplace(remainder, denominator, coeff_count, modulus, quotient, pool);
        }

        inline void add_bigpolyarray_coeffmod(const std::uint64_t *array1, const std::uint64_t *array2, int count, int coeff_count, const Modulus &modulus, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (array1 == nullptr)
            {
                throw std::invalid_argument("array1");
            }
            if (array2 == nullptr)
            {
                throw std::invalid_argument("array2");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
            if (count < 1)
            {
                throw std::invalid_argument("count");
            }
            if (coeff_count < 1)
            {
                throw std::invalid_argument("coeff_count");
            }
#endif
            // Calculate pointer increment
            int coeff_bit_count = modulus.significant_bit_count();
            int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
            int poly_ptr_increment = coeff_count * coeff_uint64_count;

            // initialise pointers for addition
            const std::uint64_t *current_array1 = array1;
            const std::uint64_t *current_array2 = array2;
            std::uint64_t *current_result = result;

            for (int i = 0; i < count; i++)
            {
                add_poly_poly_coeffmod(current_array1, current_array2, coeff_count, modulus.get(), coeff_uint64_count, current_result);
                current_array1 += poly_ptr_increment;
                current_array2 += poly_ptr_increment;
                current_result += poly_ptr_increment;
            }
        }

        inline void dyadic_product_coeffmod(const std::uint64_t *operand1, const std::uint64_t *operand2, int coeff_count, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr)
            {
                throw std::invalid_argument("operand1");
            }
            if (operand2 == nullptr)
            {
                throw std::invalid_argument("operand2");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
            if (coeff_count <= 0)
            {
                throw std::invalid_argument("coeff_count");
            }
            if (modulus.uint64_count() <= 0 || modulus.get() == nullptr)
            {
                throw std::invalid_argument("modulus");
            }
#endif
            int coeff_uint64_count = modulus.uint64_count();

            // Use the same allocation for every instance of multiply_uint_uint_mod.
            for (int i = 0; i < coeff_count; i++)
            {
                multiply_uint_uint_mod(operand1, operand2, modulus, result, pool);
                operand1 += coeff_uint64_count;
                operand2 += coeff_uint64_count;
                result += coeff_uint64_count;
            }
        }

        void modulo_poly_inplace(std::uint64_t *value, int value_coeff_count, const PolyModulus &poly_modulus, const Modulus &modulus, MemoryPool &pool);

        inline void modulo_poly(const std::uint64_t *value, int value_coeff_count, const PolyModulus &poly_modulus, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
            if (value == nullptr)
            {
                throw std::invalid_argument("value");
            }
            if (value_coeff_count <= 0)
            {
                throw std::invalid_argument("value_coeff_count");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
#endif
            int coeff_uint64_count = modulus.uint64_count();
            Pointer value_copy(allocate_poly(value_coeff_count, coeff_uint64_count, pool));
            set_poly_poly(value, value_coeff_count, coeff_uint64_count, value_copy.get());
            modulo_poly_inplace(value_copy.get(), value_coeff_count, poly_modulus, modulus, pool);
            set_poly_poly(value_copy.get(), poly_modulus.coeff_count(), coeff_uint64_count, result);
        }

        void nonfft_multiply_poly_poly_polymod_coeffmod(const std::uint64_t *operand1, const std::uint64_t *operand2, const PolyModulus &poly_modulus, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool);

        void nonfft_multiply_poly_poly_polymod_coeffmod_inplace(const std::uint64_t *operand1, const std::uint64_t *operand2, const PolyModulus &poly_modulus, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool);

        bool try_invert_poly_coeffmod(const std::uint64_t *operand, const std::uint64_t *poly_modulus, int coeff_count, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool);
    
        void poly_infty_norm_coeffmod(const std::uint64_t *poly, int poly_coeff_count, int poly_coeff_uint64_count, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool);
    
        void poly_eval_poly_polymod_coeffmod(const std::uint64_t *poly_to_eval, const std::uint64_t *value, const PolyModulus &poly_modulus, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool);

        void poly_eval_uint_mod(const std::uint64_t *poly_to_eval, int poly_to_eval_coeff_count, const std::uint64_t *value, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool);

        void exponentiate_poly_polymod_coeffmod(const std::uint64_t *poly, const std::uint64_t *exponent, int exponent_uint64_count,
            const PolyModulus &poly_modulus, const Modulus &coeff_modulus, std::uint64_t *result, MemoryPool &pool);
    }
}