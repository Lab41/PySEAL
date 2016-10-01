#pragma once

#include <cstdint>
#include "util/mempool.h"
#include "util/modulus.h"

namespace seal
{
    namespace util
    {
        void exponentiate_uint(const std::uint64_t *operand, int operand_uint64_count, const std::uint64_t *exponent,
            int exponent_uint64_count, int result_uint64_count, std::uint64_t *result, MemoryPool &pool);

        void exponentiate_uint_mod(const std::uint64_t *operand, const std::uint64_t *exponent, int exponent_uint64_count, 
            const util::Modulus &modulus, std::uint64_t *result, MemoryPool &pool);
    }
}