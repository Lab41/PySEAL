#ifndef SEAL_UTIL_UINTEXTRAS_H
#define SEAL_UTIL_UINTEXTRAS_H

#include <cstdint>
#include "util/mempool.h"

namespace seal
{
    namespace util
    {
        void exponentiate_uint(const std::uint64_t *operand, int operand_uint64_count, int exponent, int result_uint64_count, std::uint64_t *result, MemoryPool &pool);
    }
}

#endif // SEAL_UTIL_UINTEXTRAS_H
