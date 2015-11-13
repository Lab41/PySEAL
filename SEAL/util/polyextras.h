#ifndef SEAL_UTIL_POLYEXTRAS_H
#define SEAL_UTIL_POLYEXTRAS_H

#include <cstdint>
#include "util/modulus.h"
#include "util/polymodulus.h"
#include "util/mempool.h"

namespace seal
{
    namespace util
    {
        void poly_infty_norm(const uint64_t *poly, int coeff_count, int coeff_uint64_count, uint64_t *result);

        void poly_infty_norm_coeffmod(const uint64_t *poly, int poly_coeff_count, int poly_coeff_uint64_count, const Modulus &modulus, uint64_t *result, MemoryPool &pool);

        void poly_eval_poly(const uint64_t *poly_to_eval, int poly_to_eval_coeff_count, int poly_to_eval_coeff_uint64_count, const uint64_t *value,
            int value_coeff_count, int value_coeff_uint64_count, int result_coeff_count, int result_coeff_uint64_count, uint64_t *result, MemoryPool &pool);

        void exponentiate_poly(const std::uint64_t *poly, int poly_coeff_count, int poly_coeff_uint64_count, int exponent,
            int result_coeff_count, int result_coeff_uint64_count, std::uint64_t *result, MemoryPool &pool);

        void exponentiate_poly(const std::uint64_t *poly, int exponent, const PolyModulus &poly_modulus, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool);
    }
}



#endif // SEAL_UTIL_POLYEXTRAS_H
