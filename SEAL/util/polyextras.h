#pragma once

#include <cstdint>
#include "util/modulus.h"
#include "util/polymodulus.h"
#include "util/mempool.h"

namespace seal
{
    namespace util
    {
        void poly_infty_norm(const std::uint64_t *poly, int coeff_count, int coeff_uint64_count, std::uint64_t *result);

        void poly_infty_norm_coeffmod(const std::uint64_t *poly, int poly_coeff_count, int poly_coeff_uint64_count, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool);

        void poly_eval_poly(const std::uint64_t *poly_to_eval, int poly_to_eval_coeff_count, int poly_to_eval_coeff_uint64_count, const std::uint64_t *value,
            int value_coeff_count, int value_coeff_uint64_count, int result_coeff_count, int result_coeff_uint64_count, std::uint64_t *result, MemoryPool &pool);

        void poly_eval_poly_polymod_coeffmod(const std::uint64_t *poly_to_eval, const std::uint64_t *value, const PolyModulus &poly_modulus, const Modulus &coeff_modulus, std::uint64_t *result, MemoryPool &pool);

        void poly_eval_uint_mod(const std::uint64_t *poly_to_eval, int poly_to_eval_coeff_count, const std::uint64_t *value, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool);

        void exponentiate_poly(const std::uint64_t *poly, int poly_coeff_count, int poly_coeff_uint64_count, const std::uint64_t *exponent,
            int exponent_uint64_count, int result_coeff_count, int result_coeff_uint64_count, std::uint64_t *result, MemoryPool &pool);

        void exponentiate_poly_polymod_coeffmod(const std::uint64_t *poly, const std::uint64_t *exponent, int exponent_uint64_count,
            const PolyModulus &poly_modulus, const Modulus &coeff_modulus, std::uint64_t *result, MemoryPool &pool);
    }
}