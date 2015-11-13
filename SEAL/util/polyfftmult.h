#ifndef SEAL_UTIL_POLYFFTMULT_H
#define SEAL_UTIL_POLYFFTMULT_H

#include <cstdint>
#include "util/mempool.h"

namespace seal
{
    namespace util
    {
        void fftmultiply_poly_poly_polymod(const std::uint64_t *operand1, const std::uint64_t *operand2, int coeff_count_power, int coeff_uint64_count, int product_coeff_uint64_count, std::uint64_t *result, MemoryPool &pool);
    }
}

#endif // SEAL_UTIL_POLYFFTMULT_H
