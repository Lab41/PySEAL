#pragma once

#include <cstdint>
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include "seal/util/common.h"
#include "seal/util/mempool.h"
#include "seal/util/defines.h"

namespace seal
{
    namespace util
    {
        std::string uint_to_hex_string(const std::uint64_t *value, int uint64_count);

        std::string uint_to_dec_string(const std::uint64_t *value, int uint64_count, MemoryPool &pool);

        inline void hex_string_to_uint(const char *hex_string, int char_count, int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (hex_string == nullptr && char_count > 0)
            {
                throw std::invalid_argument("hex_string");
            }
            if (char_count < 0)
            {
                throw std::invalid_argument("char_count");
        }
            if (uint64_count < 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (uint64_count > 0 && result == nullptr)
            {
                throw std::invalid_argument("result");
            }
            if (get_hex_string_bit_count(hex_string, char_count) > uint64_count * bits_per_uint64)
            {
                throw std::invalid_argument("hex_string");
            }
#endif
            const char *hex_string_ptr = hex_string + char_count;
            for (int uint64_index = 0; uint64_index < uint64_count; ++uint64_index)
            {
                std::uint64_t value = 0;
                for (int bit_index = 0; bit_index < bits_per_uint64; bit_index += bits_per_nibble)
                {
                    if (hex_string_ptr == hex_string)
                    {
                        break;
                    }
                    char hex = *--hex_string_ptr;
                    int nibble = hex_to_nibble(hex);
                    if (nibble == -1)
                    {
                        throw std::invalid_argument("hex_value");
                    }
                    value |= static_cast<std::uint64_t>(nibble) << bit_index;
                }
                result[uint64_index] = value;
            }
    }

        inline Pointer allocate_uint(int uint64_count, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
            if (uint64_count < 0)
            {
                throw std::invalid_argument("uint64_count");
            }
#endif
            return pool.get_for_uint64_count(uint64_count);
        }

        inline void set_zero_uint(int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (uint64_count < 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (result == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            if (!uint64_count)
            {
                return;
            }
            std::memset(result, 0, uint64_count * bytes_per_uint64);
        }

        inline Pointer allocate_zero_uint(int uint64_count, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
            if (uint64_count < 0)
            {
                throw std::invalid_argument("uint64_count");
            }
#endif
            Pointer allocated = allocate_uint(uint64_count, pool);
            set_zero_uint(uint64_count, allocated.get());
            return allocated;
        }

        inline void set_uint(std::uint64_t value, int uint64_count, std::uint64_t* result)
        {
#ifdef SEAL_DEBUG
            if (uint64_count <= 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
#endif
            *result++ = value;
            for (int i = 1; i < uint64_count; i++)
            {
                *result++ = 0;
            }
        }

        inline void set_uint_uint(const std::uint64_t *value, int uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (value == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("value");
            }
            if (uint64_count < 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (result == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            if (value == result || uint64_count == 0)
            {
                // Fast path to handle self assignment.
                return;
            }
            std::memcpy(result, value, uint64_count * bytes_per_uint64);
        }

        inline bool is_zero_uint(const std::uint64_t *value, int uint64_count)
        {
#ifdef SEAL_DEBUG
            if (value == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("value");
            }
            if (uint64_count < 0)
            {
                throw std::invalid_argument("uint64_count");
            }
#endif
            bool result = true;
            for (int i = 0; i < uint64_count; i++)
            {
                result &= (*value++ == 0);
            }
            return result;
        }

        inline bool is_equal_uint(const std::uint64_t *value, int uint64_count, std::uint64_t scalar)
        {
#ifdef SEAL_DEBUG
            if (value == nullptr)
            {
                throw std::invalid_argument("value");
            }
            if (uint64_count <= 0)
            {
                throw std::invalid_argument("uint64_count");
            }
#endif
            if (*value++ != scalar)
            {
                return false;
            }
            bool zeros = true;
            for (int i = 1; i < uint64_count; ++i)
            {
                zeros &= (*value++ == 0);
            }
            return zeros;
        }

        inline bool is_high_bit_set_uint(const std::uint64_t *value, int uint64_count)
        {
#ifdef SEAL_DEBUG
            if (value == nullptr)
            {
                throw std::invalid_argument("value");
            }
            if (uint64_count <= 0)
            {
                throw std::invalid_argument("uint64_count");
            }
#endif
            return (value[uint64_count - 1] >> (bits_per_uint64 - 1)) != 0;
        }

        inline bool is_bit_set_uint(const std::uint64_t *value, int uint64_count, int bit_index)
        {
#ifdef SEAL_DEBUG
            if (value == nullptr)
            {
                throw std::invalid_argument("value");
            }
            if (uint64_count <= 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (bit_index < 0 || bit_index >= uint64_count * bits_per_uint64)
            {
                throw std::invalid_argument("bit_index");
            }
#endif
            int uint64_index = bit_index / bits_per_uint64;
            int sub_bit_index = bit_index - uint64_index * bits_per_uint64;
            return ((value[uint64_index] >> sub_bit_index) & 1) != 0;
        }

        inline void set_bit_uint(std::uint64_t *value, int uint64_count, int bit_index)
        {
#ifdef SEAL_DEBUG
            if (value == nullptr)
            {
                throw std::invalid_argument("value");
            }
            if (uint64_count <= 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (bit_index < 0 || bit_index >= uint64_count * bits_per_uint64)
            {
                throw std::invalid_argument("bit_index");
            }
#endif
            int uint64_index = bit_index / bits_per_uint64;
            int sub_bit_index = bit_index % bits_per_uint64;
            value[uint64_index] |= (1ULL << sub_bit_index);
        }

        inline int get_significant_bit_count_uint(const std::uint64_t *value, int uint64_count)
        {
#ifdef SEAL_DEBUG
            if (value == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("value");
            }
            if (uint64_count <= 0)
            {
                throw std::invalid_argument("uint64_count");
            }
#endif
            if (uint64_count == 0)
            {
                return 0;
            }

            value += uint64_count - 1;
            for (; *value == 0 && uint64_count > 1; uint64_count--)
            {
                value--;
            }

            return (uint64_count - 1) * bits_per_uint64 + get_significant_bit_count(*value);
        }

        inline int get_significant_uint64_count_uint(const std::uint64_t *value, int uint64_count)
        {
#ifdef SEAL_DEBUG
            if (value == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("value");
            }
            if (uint64_count <= 0)
            {
                throw std::invalid_argument("uint64_count");
            }
#endif
            value += uint64_count - 1;
            for (; *value == 0 && uint64_count > 0; uint64_count--)
            {
                value--;
            }

            return uint64_count;
        }

        inline void set_uint_uint(const std::uint64_t *value, int value_uint64_count, int result_uint64_count, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (value == nullptr && value_uint64_count > 0)
            {
                throw std::invalid_argument("value");
            }
            if (value_uint64_count < 0)
            {
                throw std::invalid_argument("value_uint64_count");
            }
            if (result_uint64_count < 0)
            {
                throw std::invalid_argument("result_uint64_count");
            }
            if (result == nullptr && result_uint64_count > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            if (value == result || value_uint64_count == 0)
            {
                // Fast path to handle self assignment.
                result += value_uint64_count;
                for (int i = value_uint64_count; i < result_uint64_count; i++)
                {
                    *result++ = 0;
                }
                return;
            }
            for (int i = 0; i < result_uint64_count; i++)
            {
                *result++ = (i < value_uint64_count) ? *value++ : 0;
            }
        }

        inline int get_power_of_two(std::uint64_t value)
        {
            if (value == 0 || (value & (value - 1)) != 0)
            {
                return -1;
            }

            unsigned long result;
            SEAL_MSB_INDEX_UINT64(&result, value);
            return result;
        }

        inline int get_power_of_two_minus_one(std::uint64_t value)
        {
            if (value == 0xFFFFFFFFFFFFFFFF)
            {
                return bits_per_uint64;
            }
            return get_power_of_two(value + 1);
        }

        inline int get_power_of_two_uint(const std::uint64_t *operand, int uint64_count)
        {
#ifdef SEAL_DEBUG
            if (operand == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("operand");
            }
            if (uint64_count < 0)
            {
                throw std::invalid_argument("uint64_count");
            }
#endif
            operand += uint64_count;
            int long_index = uint64_count, local_result = -1;
            for (; (long_index >= 1) && (local_result == -1); long_index--)
            {
                operand--;
                local_result = get_power_of_two(*operand);
            }

            // If local_result != -1, we've found a power-of-two highest order block,
            // in which case need to check that rest are zero.
            // If local_result == -1, operand is not power of two.
            if (local_result == -1)
            {
                return -1;
            }

            int zeros = 1;
            for (int j = long_index; j >= 1; j--)
            {
                zeros &= (*--operand == 0);
            }

            return zeros * (local_result + long_index * bits_per_uint64) + zeros - 1;
        }

        inline int get_power_of_two_minus_one_uint(const std::uint64_t *operand, int uint64_count)
        {
#ifdef SEAL_DEBUG
            if (operand == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("operand");
            }
            if (uint64_count < 0)
            {
                throw std::invalid_argument("uint64_count");
            }
#endif
            operand += uint64_count;
            int long_index = uint64_count, local_result = 0;
            for (; (long_index >= 1) && (local_result == 0); long_index--)
            {
                operand--;
                local_result = get_power_of_two_minus_one(*operand);
            }

            // If local_result != -1, we've found a power-of-two-minus-one highest order block,
            // in which case need to check that rest are ~0.
            // If local_result == -1, operand is not power of two minus one.
            if (local_result == -1)
            {
                return -1;
            }

            int ones = 1;
            for (int j = long_index; j >= 1; j--)
            {
                ones &= (~*--operand == 0ULL);
            }

            return ones * (local_result + long_index * bits_per_uint64) + ones - 1;
        }

        inline void filter_highbits_uint(std::uint64_t *operand, int uint64_count, int bit_count)
        {
#ifdef SEAL_DEBUG
            if (operand == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("operand");
            }
            if (bit_count < 0 || bit_count > uint64_count * bits_per_uint64)
            {
                throw std::invalid_argument("bit_count");
            }
            if (uint64_count < 0)
            {
                throw std::invalid_argument("uint64_count");
            }
#endif
            if (bit_count == uint64_count * bits_per_uint64)
            {
                return;
            }
            int uint64_index = bit_count / bits_per_uint64;
            int subbit_index = bit_count - uint64_index * bits_per_uint64;
            operand += uint64_index;
            *operand++ &= (1ULL << subbit_index) - 1;
            for (int long_index = uint64_index + 1; long_index < uint64_count; ++long_index)
            {
                *operand++ = 0;
            }
        }

        inline ConstPointer duplicate_uint_if_needed(const std::uint64_t *uint, int uint64_count, int new_uint64_count, bool force, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
            if (uint == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("uint");
            }
            if (uint64_count < 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (new_uint64_count < 0)
            {
                throw std::invalid_argument("new_uint64_count");
            }
#endif
            if (!force && uint64_count >= new_uint64_count)
            {
                return ConstPointer::Aliasing(uint);
            }
            Pointer allocation(pool.get_for_uint64_count(new_uint64_count));
            set_uint_uint(uint, uint64_count, new_uint64_count, allocation.get());
            ConstPointer const_allocation;
            const_allocation.acquire(allocation);
            return const_allocation;
        }

        inline int compare_uint_uint(const std::uint64_t *operand1, const std::uint64_t *operand2, int uint64_count)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("operand1");
            }
            if (operand2 == nullptr && uint64_count > 0)
            {
                throw std::invalid_argument("operand2");
            }
            if (uint64_count < 0)
            {
                throw std::invalid_argument("uint64_count");
            }
#endif
            int result = 0;
            operand1 += uint64_count - 1;
            operand2 += uint64_count - 1;

            for (int i = uint64_count - 1; (result == 0) && (i >= 0); i--)
            {
                result = (*operand1 > *operand2) - (*operand1 < *operand2);
                operand1--;
                operand2--;
            }
            return result;
        }

        inline int compare_uint_uint(const std::uint64_t *operand1, int operand1_uint64_count, const std::uint64_t *operand2, int operand2_uint64_count)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr && operand1_uint64_count > 0)
            {
                throw std::invalid_argument("operand1");
            }
            if (operand1_uint64_count < 0)
            {
                throw std::invalid_argument("operand1_uint64_count");
            }
            if (operand2 == nullptr && operand2_uint64_count > 0)
            {
                throw std::invalid_argument("operand2");
            }
            if (operand2_uint64_count < 0)
            {
                throw std::invalid_argument("operand2_uint64_count");
            }
#endif
            int result = 0;
            operand1 += operand1_uint64_count - 1;
            operand2 += operand2_uint64_count - 1;

            int min_uint64_count = std::min(operand1_uint64_count, operand2_uint64_count);
            for (int i = operand1_uint64_count - 1; (result == 0) && (i >= min_uint64_count); i--)
            {
                result = (*operand1-- > 0);
            }
            for (int i = operand2_uint64_count - 1; (result == 0) && (i >= min_uint64_count); i--)
            {
                result = -(*operand2-- > 0);
            }
            for (int i = min_uint64_count - 1; (result == 0) && (i >= 0); i--)
            {
                result = (*operand1 > *operand2) - (*operand1 < *operand2);
                operand1--;
                operand2--;
            }
            return result;
        }

        inline bool is_greater_than_uint_uint(const std::uint64_t *operand1, const std::uint64_t *operand2, int uint64_count)
        {
            return compare_uint_uint(operand1, operand2, uint64_count) > 0;
        }

        inline bool is_greater_than_or_equal_uint_uint(const std::uint64_t *operand1, const std::uint64_t *operand2, int uint64_count)
        {
            return compare_uint_uint(operand1, operand2, uint64_count) >= 0;
        }

        inline bool is_less_than_uint_uint(const std::uint64_t *operand1, const std::uint64_t *operand2, int uint64_count)
        {
            return compare_uint_uint(operand1, operand2, uint64_count) < 0;
        }

        inline bool is_less_than_or_equal_uint_uint(const std::uint64_t *operand1, const std::uint64_t *operand2, int uint64_count)
        {
            return compare_uint_uint(operand1, operand2, uint64_count) <= 0;
        }

        inline bool is_equal_uint_uint(const std::uint64_t *operand1, const std::uint64_t *operand2, int uint64_count)
        {
            return compare_uint_uint(operand1, operand2, uint64_count) == 0;
        }

        inline bool is_not_equal_uint_uint(const std::uint64_t *operand1, const std::uint64_t *operand2, int uint64_count)
        {
            return compare_uint_uint(operand1, operand2, uint64_count) != 0;
        }

        inline bool is_greater_than_uint_uint(const std::uint64_t *operand1, int operand1_uint64_count, const std::uint64_t *operand2, int operand2_uint64_count)
        {
            return compare_uint_uint(operand1, operand1_uint64_count, operand2, operand2_uint64_count) > 0;
        }

        inline bool is_greater_than_or_equal_uint_uint(const std::uint64_t *operand1, int operand1_uint64_count, const std::uint64_t *operand2, int operand2_uint64_count)
        {
            return compare_uint_uint(operand1, operand1_uint64_count, operand2, operand2_uint64_count) >= 0;
        }

        inline bool is_less_than_uint_uint(const std::uint64_t *operand1, int operand1_uint64_count, const std::uint64_t *operand2, int operand2_uint64_count)
        {
            return compare_uint_uint(operand1, operand1_uint64_count, operand2, operand2_uint64_count) < 0;
        }

        inline bool is_less_than_or_equal_uint_uint(const std::uint64_t *operand1, int operand1_uint64_count, const std::uint64_t *operand2, int operand2_uint64_count)
        {
            return compare_uint_uint(operand1, operand1_uint64_count, operand2, operand2_uint64_count) <= 0;
        }

        inline bool is_equal_uint_uint(const std::uint64_t *operand1, int operand1_uint64_count, const std::uint64_t *operand2, int operand2_uint64_count)
        {
            return compare_uint_uint(operand1, operand1_uint64_count, operand2, operand2_uint64_count) == 0;
        }

        inline bool is_not_equal_uint_uint(const std::uint64_t *operand1, int operand1_uint64_count, const std::uint64_t *operand2, int operand2_uint64_count)
        {
            return compare_uint_uint(operand1, operand1_uint64_count, operand2, operand2_uint64_count) != 0;
        }

        inline std::uint64_t hamming_weight(std::uint64_t value)
        {
            std::uint64_t res = 0;
            while (value)
            {
                res++;
                value &= value - 1;
            }
            return res;
        }

        inline std::uint64_t hamming_weight_split(std::uint64_t value)
        {
            std::uint64_t hwx = hamming_weight(value);
            std::uint64_t target = (hwx + 1) >> 1;
            std::uint64_t now = 0;
            std::uint64_t result = 0;

            for (int i = 0; i < bits_per_uint64; i++)
            {
                std::uint64_t xbit = value & 1;
                value = value >> 1;
                now += xbit;
                result += (xbit << i);

                if (now >= target)
                {
                    break;
                }
            }
            return result;
        }
    }
}
