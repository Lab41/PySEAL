#ifndef SEAL_UTIL_POLYFFTMULTMOD_H
#define SEAL_UTIL_POLYFFTMULTMOD_H

#include <cstdint>
#include "util/modulus.h"

namespace seal
{
    namespace util
    {
        void fftmultiply_poly_poly_polymod_coeffmod(const std::uint64_t *operand1, const std::uint64_t *operand2, int coeff_count_power, const Modulus &modulus, std::uint64_t *result, MemoryPool &pool);
    }
}

#endif // SEAL_UTIL_POLYFFTMULTMOD_H
