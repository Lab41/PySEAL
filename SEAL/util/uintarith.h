#ifndef SEAL_UTIL_UINTARITH_H
#define SEAL_UTIL_UINTARITH_H

#include <cstdint>
#include "util/uintcore.h"
#include "util/mempool.h"

namespace seal
{
    namespace util
    {
        bool increment_uint(const std::uint64_t *operand, int uint64_count, std::uint64_t *result);

        bool decrement_uint(const std::uint64_t *operand, int uint64_count, std::uint64_t *result);

        void negate_uint(const std::uint64_t *operand, int uint64_count, std::uint64_t *result);

        void left_shift_uint(const std::uint64_t *operand, int shift_amount, int uint64_count, std::uint64_t *result);

        void right_shift_uint(const std::uint64_t *operand, int shift_amount, int uint64_count, std::uint64_t *result);

        void right_shift_sign_extend_uint(const uint64_t *operand, int shift_amount, int uint64_count, uint64_t *result);

        void half_round_up_uint(const uint64_t *operand, int uint64_count, uint64_t *result);

        void not_uint(const std::uint64_t *operand, int uint64_count, std::uint64_t *result);

        void and_uint_uint(const std::uint64_t *operand1, const std::uint64_t *operand2, int uint64_count, std::uint64_t *result);

        void or_uint_uint(const std::uint64_t *operand1, const std::uint64_t *operand2, int uint64_count, std::uint64_t *result);

        void xor_uint_uint(const std::uint64_t *operand1, const std::uint64_t *operand2, int uint64_count, std::uint64_t *result);

        bool add_uint_uint(const std::uint64_t *operand1, int operand1_uint64_count, const std::uint64_t *operand2, int operand2_uint64_count, bool carry, int result_uint64_count, std::uint64_t *result);

        inline bool add_uint_uint(const std::uint64_t *operand1, const std::uint64_t *operand2, int uint64_count, std::uint64_t *result)
        {
            return add_uint_uint(operand1, uint64_count, operand2, uint64_count, false, uint64_count, result);
        }

        bool sub_uint_uint(const std::uint64_t *operand1, int operand1_uint64_count, const std::uint64_t *operand2, int operand2_uint64_count, bool borrow, int result_uint64_count, std::uint64_t *result);

        inline bool sub_uint_uint(const std::uint64_t *operand1, const std::uint64_t *operand2, int uint64_count, std::uint64_t *result)
        {
            return sub_uint_uint(operand1, uint64_count, operand2, uint64_count, false, uint64_count, result);
        }

        void multiply_uint_uint(const std::uint64_t *operand1, int operand1_uint64_count, const std::uint64_t *operand2, int operand2_uint64_count, int result_uint64_count, std::uint64_t *result);

        inline void multiply_uint_uint(const std::uint64_t *operand1, const std::uint64_t *operand2, int uint64_count, std::uint64_t *result)
        {
            int intermediate_uint64_count = uint64_count * 2;
            multiply_uint_uint(operand1, uint64_count, operand2, uint64_count, intermediate_uint64_count, result);
        }

        inline void multiply_truncate_uint_uint(const std::uint64_t *operand1, const std::uint64_t *operand2, int uint64_count, std::uint64_t *result)
        {
            multiply_uint_uint(operand1, uint64_count, operand2, uint64_count, uint64_count, result);
        }

        void divide_uint_uint_inplace(std::uint64_t *numerator, const std::uint64_t *denominator, int uint64_count, std::uint64_t *quotient, MemoryPool &pool);

        inline void divide_uint_uint(const std::uint64_t *numerator, const std::uint64_t *denominator, int uint64_count, std::uint64_t *quotient, std::uint64_t *remainder, MemoryPool &pool)
        {
            set_uint_uint(numerator, uint64_count, remainder);
            divide_uint_uint_inplace(remainder, denominator, uint64_count, quotient, pool);
        }
    }
}

#endif // SEAL_UTIL_UINTARITH_H
