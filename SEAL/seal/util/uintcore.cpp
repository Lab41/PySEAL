#include "seal/util/common.h"
#include "seal/util/uintcore.h"
#include "seal/util/uintarith.h"
#include <algorithm>
#include <string>

using namespace std;

namespace seal
{
    namespace util
    {
        string uint_to_hex_string(const uint64_t *value, int uint64_count)
        {
#ifdef SEAL_DEBUG
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
                return string("0");
            }

            return output;
        }

        string uint_to_dec_string(const uint64_t *value, int uint64_count, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
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
                return string("0");
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
                return string("0");
            }

            return output;
        }
    }
}