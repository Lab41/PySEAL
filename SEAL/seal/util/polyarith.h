#pragma once

#include <cstdint>
#include "seal/util/uintcore.h"
#include "seal/util/uintarith.h"
#include "seal/util/uintarithmod.h"
#include "seal/util/polycore.h"
#include "seal/util/mempool.h"

namespace seal
{
    namespace util
    {
        inline void right_shift_poly_coeffs(const std::uint64_t *poly, int coeff_count, int coeff_uint64_count, int shift_amount, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (poly == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw std::invalid_argument("poly");
            }
            if (coeff_count < 0)
            {
                throw std::invalid_argument("coeff_count");
            }
            if (coeff_uint64_count < 0)
            {
                throw std::invalid_argument("coeff_uint64_count");
            }
#endif
            for (int j = 0; j < coeff_count; j++)
            {
                right_shift_uint(poly, shift_amount, coeff_uint64_count, result);
                poly += coeff_uint64_count;
                result += coeff_uint64_count;
            }
        }

        inline void negate_poly(const std::uint64_t *poly, int coeff_count, int coeff_uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (poly == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw std::invalid_argument("poly");
            }
            if (coeff_count < 0)
            {
                throw std::invalid_argument("coeff_count");
            }
            if (coeff_uint64_count < 0)
            {
                throw std::invalid_argument("coeff_uint64_count");
            }
            if (result == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            for (int i = 0; i < coeff_count; i++)
            {
                negate_uint(poly, coeff_uint64_count, result);
                poly += coeff_uint64_count;
                result += coeff_uint64_count;
            }
        }

        inline void add_poly_poly(const std::uint64_t *operand1, const std::uint64_t *operand2, int coeff_count, int coeff_uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw std::invalid_argument("operand1");
            }
            if (operand2 == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw std::invalid_argument("operand2");
            }
            if (coeff_count < 0)
            {
                throw std::invalid_argument("coeff_count");
            }
            if (coeff_uint64_count < 0)
            {
                throw std::invalid_argument("coeff_uint64_count");
            }
            if (result == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            for (int i = 0; i < coeff_count; i++)
            {
                add_uint_uint(operand1, operand2, coeff_uint64_count, result);
                operand1 += coeff_uint64_count;
                operand2 += coeff_uint64_count;
                result += coeff_uint64_count;
            }
        }

        inline void sub_poly_poly(const std::uint64_t *operand1, const std::uint64_t *operand2, int coeff_count, int coeff_uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw std::invalid_argument("operand1");
            }
            if (operand2 == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw std::invalid_argument("operand2");
            }
            if (coeff_count < 0)
            {
                throw std::invalid_argument("coeff_count");
            }
            if (coeff_uint64_count < 0)
            {
                throw std::invalid_argument("coeff_uint64_count");
            }
            if (result == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            for (int i = 0; i < coeff_count; i++)
            {
                sub_uint_uint(operand1, operand2, coeff_uint64_count, result);
                operand1 += coeff_uint64_count;
                operand2 += coeff_uint64_count;
                result += coeff_uint64_count;
            }
        }

        void multiply_poly_poly(const std::uint64_t *operand1, int operand1_coeff_count, int operand1_coeff_uint64_count, const std::uint64_t *operand2, int operand2_coeff_count, int operand2_coeff_uint64_count,
            int result_coeff_count, int result_coeff_uint64_count, std::uint64_t *result, MemoryPool &pool);

        inline void poly_infty_norm(const std::uint64_t *poly, int coeff_count, int coeff_uint64_count, std::uint64_t *result)
        {
            set_zero_uint(coeff_uint64_count, result);
            for (int coeff_index = 0; coeff_index < coeff_count; coeff_index++)
            {
                if (is_greater_than_uint_uint(poly, result, coeff_uint64_count))
                {
                    set_uint_uint(poly, coeff_uint64_count, result);
                }

                poly += coeff_uint64_count;
            }
        }

        void poly_eval_poly(const std::uint64_t *poly_to_eval, int poly_to_eval_coeff_count, int poly_to_eval_coeff_uint64_count, const std::uint64_t *value,
            int value_coeff_count, int value_coeff_uint64_count, int result_coeff_count, int result_coeff_uint64_count, std::uint64_t *result, MemoryPool &pool);

        void exponentiate_poly(const std::uint64_t *poly, int poly_coeff_count, int poly_coeff_uint64_count, const std::uint64_t *exponent,
            int exponent_uint64_count, int result_coeff_count, int result_coeff_uint64_count, std::uint64_t *result, MemoryPool &pool);
    }
}