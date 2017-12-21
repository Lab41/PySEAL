#include "seal/smallmodulus.h"
#include "seal/util/uintarith.h"
#include <stdexcept>

using namespace seal::util;
using namespace std;

namespace seal
{
    void SmallModulus::save(ostream &stream) const
    {
        int bit_count32 = bit_count_;
        stream.write(reinterpret_cast<const char*>(&bit_count32), sizeof(int32_t));
        int uint64_count32 = uint64_count_;
        stream.write(reinterpret_cast<const char*>(&uint64_count32), sizeof(int32_t));
        stream.write(reinterpret_cast<const char*>(&value_), bytes_per_uint64);
        stream.write(reinterpret_cast<const char*>(const_ratio_.data()), 3 * bytes_per_uint64);
    }

    void SmallModulus::load(istream &stream)
    {
        int bit_count32;
        stream.read(reinterpret_cast<char*>(&bit_count32), sizeof(int32_t));
        bit_count_ = bit_count32;
        int uint64_count32;
        stream.read(reinterpret_cast<char*>(&uint64_count32), sizeof(int32_t));
        uint64_count_ = uint64_count32;
        stream.read(reinterpret_cast<char*>(&value_), bytes_per_uint64);
        stream.read(reinterpret_cast<char*>(const_ratio_.data()), 3 * bytes_per_uint64);
    }

    void SmallModulus::set_value(uint64_t value)
    {
        if (value == 0)
        {
            // Zero settings
            bit_count_ = 0;
            uint64_count_ = 1;
            value_ = 0;

            // C++11 compatibility
            const_ratio_ = { { 0 } };
        }
        else if (value >> 62 != 0 || value == 0x4000000000000000 || value == 1)
        {
            throw invalid_argument("value can be at most 62 bits and cannot be 1");
        }
        else
        {
            // All normal, compute const_ratio and set everything
            value_ = value;
            bit_count_ = get_significant_bit_count(value_);

            // Compute Barrett ratios for 64-bit words (barrett_reduce_128)
            uint64_t numerator[3]{ 0, 0, 1 };
            uint64_t quotient[3]{ 0 };

            // Use a special method to avoid using memory pool
            divide_uint192_uint64_inplace(numerator, value_, quotient);

            const_ratio_[0] = quotient[0];
            const_ratio_[1] = quotient[1];

            // We store also the remainder
            const_ratio_[2] = numerator[0];

            uint64_count_ = 1;
        }
    }
}
