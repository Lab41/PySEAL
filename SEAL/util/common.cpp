#include <algorithm>
#include <string>
#include "util/common.h"
#include "util/uintarith.h"

using namespace std;

namespace seal
{
    namespace util
    {
        int get_power_of_two(uint64_t value)
        {
            if (value == 0 || (value & (value - 1)) != 0)
            {
                return -1;
            }
            int power = 0;
            while (value != 1)
            {
                power++;
                value >>= 1;
            }
            return power;
        }

        int get_power_of_two_minus_one(uint64_t value)
        {
            if (value == 0xFFFFFFFFFFFFFFFF)
            {
                return bits_per_uint64;
            }
            return get_power_of_two(value + 1);
        }

        string uint64_to_hex_string(const uint64_t *value, int uint64_count)
        {
#ifdef _DEBUG
            if (uint64_count < 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (uint64_count > 0 && value == nullptr)
            {
                throw invalid_argument("value");
            }
#endif
            // Start with a string with a zero for each nibble in the array.
            int num_nibbles = uint64_count * nibbles_per_uint64;
            string output(num_nibbles, '0');

            // Iterate through each uint64 in array and set string with correct nibbles in hex.
            int nibble_index = num_nibbles;
            int leftmost_non_zero_pos = num_nibbles;
            for (int i = 0; i < uint64_count; ++i)
            {
                uint64_t part = *value++;

                // Iterate through each nibble in the current uint64.
                for (int j = 0; j < nibbles_per_uint64; ++j)
                {
                    int nibble = static_cast<int>(part & 0x0F);
                    int pos = --nibble_index;
                    if (nibble != 0)
                    {
                        // If nibble is not zero, then update string and save this pos to determine
                        // number of leading zeros.
                        output[pos] = nibble_to_upper_hex(nibble);
                        leftmost_non_zero_pos = pos;
                    }
                    part >>= 4;
                }
            }

            // Trim string to remove leading zeros.
            output.erase(0, leftmost_non_zero_pos);

            // Return 0 if nothing remains.
            if (output.empty())
            {
                return "0";
            }

            return output;
        }

        string uint64_to_dec_string(const uint64_t *value, int uint64_count, MemoryPool &pool)
        {
#ifdef _DEBUG
            if (uint64_count < 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (uint64_count > 0 && value == nullptr)
            {
                throw invalid_argument("value");
            }
#endif
            if (uint64_count == 0)
            {
                return "0";
            }
            Pointer remainder(allocate_uint(uint64_count, pool));
            Pointer quotient(allocate_uint(uint64_count, pool));
            Pointer base(allocate_uint(uint64_count, pool));
            uint64_t *remainderptr = remainder.get();
            uint64_t *quotientptr = quotient.get();
            uint64_t *baseptr = base.get();
            set_uint(10, uint64_count, baseptr);
            set_uint_uint(value, uint64_count, remainderptr);
            string output;
            while (!is_zero_uint(remainderptr, uint64_count))
            {
                divide_uint_uint_inplace(remainderptr, baseptr, uint64_count, quotientptr, pool);
                char digit = static_cast<char>(remainderptr[0]) + '0';
                output += digit;
                swap(remainderptr, quotientptr);
            }
            reverse(output.begin(), output.end());

            // Return 0 if nothing remains.
            if (output.empty())
            {
                return "0";
            }

            return output;
        }

        void hex_string_to_uint64(const char *hex_string, int char_count, int uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (hex_string == nullptr && char_count > 0)
            {
                throw invalid_argument("hex_string");
            }
            if (char_count < 0)
            {
                throw invalid_argument("char_count");
            }
            if (uint64_count < 0)
            {
                throw invalid_argument("uint64_count");
            }
            if (uint64_count > 0 && result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (get_hex_string_bit_count(hex_string, char_count) > uint64_count * bits_per_uint64)
            {
                throw out_of_range("hex_string");
            }
#endif
            const char *hex_string_ptr = hex_string + char_count;
            for (int uint64_index = 0; uint64_index < uint64_count; ++uint64_index)
            {
                uint64_t value = 0;
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
                        throw invalid_argument("hex_value");
                    }
                    value |= static_cast<uint64_t>(nibble) << bit_index;
                }
                result[uint64_index] = value;
            }
        }

        int get_hex_string_bit_count(const char *hex_string, int char_count)
        {
#ifdef _DEBUG
            if (hex_string == nullptr && char_count > 0)
            {
                throw invalid_argument("hex_string");
            }
            if (char_count < 0)
            {
                throw invalid_argument("char_count");
            }
#endif
            for (int i = 0; i < char_count; ++i)
            {
                char hex = *hex_string++;
                int nibble = hex_to_nibble(hex);
                if (nibble != 0)
                {
                    int nibble_bits = get_significant_bit_count(nibble);
                    int remaining_nibbles = (char_count - i - 1) * bits_per_nibble;
                    return nibble_bits + remaining_nibbles;
                }
            }
            return 0;
        }
    }
}
