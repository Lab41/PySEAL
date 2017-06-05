#pragma once

#include <cstdint>
#include "util/polycore.h"
#include "util/mempool.h"
#include "util/uintarithmod.h"

namespace seal
{
    namespace util
    {
        void rightshift_poly_coeffs(std::uint64_t *poly, int coeff_count, int coeff_uint64_count, int shift_amount, MemoryPool &pool); 

        void modulo_poly_coeffs_uint64(uint64_t *poly, int coeff_count, uint64_t mod, MemoryPool &pool); 

        void modulo_poly_coeffs(std::uint64_t *poly, int coeff_count, const Modulus &modulus, MemoryPool &pool);

        void negate_poly(const std::uint64_t *poly, int coeff_count, int coeff_uint64_count, std::uint64_t *result);

        void negate_poly_coeffmod(const std::uint64_t *poly, int coeff_count, const std::uint64_t *coeff_modulus, int coeff_uint64_count, std::uint64_t *result);

        void add_poly_poly(const std::uint64_t *operand1, const std::uint64_t *operand2, int coeff_count, int coeff_uint64_count, std::uint64_t *result);

        void sub_poly_poly(const std::uint64_t *operand1, const std::uint64_t *operand2, int coeff_count, int coeff_uint64_count, std::uint64_t *result);

        void add_poly_poly_coeffmod(const std::uint64_t *operand1, const std::uint64_t *operand2, int coeff_count, const std::uint64_t *coeff_modulus, int coeff_uint64_count, std::uint64_t *result);

        void sub_poly_poly_coeffmod(const std::uint64_t *operand1, const std::uint64_t *operand2, int coeff_count, const std::uint64_t *coeff_modulus, int coeff_uint64_count, std::uint64_t *result);

        void multiply_poly_scalar_coeffmod(const std::uint64_t *poly, int coeff_count, const std::uint64_t *scalar, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool);


        void multiply_poly_poly(const std::uint64_t *operand1, int operand1_coeff_count, int operand1_coeff_uint64_count, const std::uint64_t *operand2, int operand2_coeff_count, int operand2_coeff_uint64_count,
            int result_coeff_count, int result_coeff_uint64_count, std::uint64_t *result, MemoryPool &pool);

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

        void add_bigpolyarray_coeffmod(const std::uint64_t *array1, const std::uint64_t *array2, int count, int coeff_count, const Modulus &modulus, std::uint64_t *result);
    }
}