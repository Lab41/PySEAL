#pragma once

#include <cstdint>
#include "util/mempool.h"

namespace seal
{
    namespace util
    {
        void nussbaumer_multiply_poly_poly(const std::uint64_t *operand1, const std::uint64_t *operand2, int coeff_count_power, int coeff_uint64_count, int sum_uint64_count, int product_uint64_count, std::uint64_t *result, MemoryPool &pool);
        
        void nussbaumer_cross_multiply_poly_poly(const std::uint64_t *operand1, const std::uint64_t *operand2, int coeff_count_power, int coeff_uint64_count, int sum_uint64_count, int product_uint64_count, std::uint64_t *result_op1_op1, std::uint64_t *result_op2_op2, std::uint64_t *result_op1_op2, MemoryPool &pool);
    }
}