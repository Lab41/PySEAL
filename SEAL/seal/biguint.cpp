#include "seal/biguint.h"
#include "seal/util/common.h"
#include "seal/util/uintcore.h"
#include "seal/util/uintarith.h"
#include "seal/util/uintarithmod.h"
#include <algorithm>

using namespace std;
using namespace seal::util;

namespace seal
{
    BigUInt::BigUInt(int bit_count)
    {
        resize(bit_count);
    }

    BigUInt::BigUInt(const string &hex_value)
    {
        operator =(hex_value);
    }

    BigUInt::BigUInt(int bit_count, const string &hex_value)
    {
        resize(bit_count);
        operator =(hex_value);
        if (bit_count != bit_count_)
        {
            resize(bit_count);
        }
    }

    BigUInt::BigUInt(int bit_count, uint64_t *value) :
        value_(value, true), bit_count_ (bit_count)
    {
        if (bit_count < 0)
        {
            throw invalid_argument("bit_count must be non-negative");
        }
        if (value == nullptr && bit_count > 0)
        {
            throw invalid_argument("value must be non-null for non-zero bit count");
        }
    } 

    BigUInt::BigUInt(int bit_count, uint64_t value)
    {
        resize(bit_count);
        operator =(value);
        if (bit_count != bit_count_)
        {
            resize(bit_count);
        }
    }

    BigUInt::BigUInt(const BigUInt &copy)
    {
        resize(copy.bit_count());
        operator =(copy);
    }

    BigUInt::BigUInt(BigUInt &&source) noexcept :
        pool_(move(source.pool_)),
        value_(move(source.value_)),
        bit_count_(source.bit_count_)
    {
        // Pointer in source has been taken over so set it to nullptr
        source.bit_count_ = 0;
    }

    BigUInt::~BigUInt()
    {
        reset();
    }

    string BigUInt::to_string() const
    {
        int uint64_count = divide_round_up(bit_count_, bits_per_uint64);
        return uint_to_hex_string(value_.get(), uint64_count);
    }

    string BigUInt::to_dec_string() const
    {
        int uint64_count = divide_round_up(bit_count_, bits_per_uint64);
        return uint_to_dec_string(value_.get(), uint64_count, pool_);
    }

    void BigUInt::resize(int bit_count)
    {
        if (bit_count < 0)
        {
            throw invalid_argument("bit_count must be non-negative");
        }
        if (value_.is_alias())
        {
            throw logic_error("Cannot resize an aliased BigUInt");
        }
        if (bit_count == bit_count_)
        {
            return;
        }

        // Lazy initialization of MemoryPoolHandle
        if (!pool_)
        {
            pool_ = MemoryPoolHandle::Global();
        }

        // Fast path if allocation size doesn't change.
        int old_uint64_count = divide_round_up(bit_count_, bits_per_uint64);
        int new_uint64_count = divide_round_up(bit_count, bits_per_uint64);
        if (old_uint64_count == new_uint64_count)
        {
            bit_count_ = bit_count;
            return;
        }

        // Allocate new space.
        Pointer new_value;
        if (new_uint64_count > 0)
        {
            new_value.swap_with(allocate_uint(new_uint64_count, pool_));
        }

        // Copy over old value.
        if (new_uint64_count > 0)
        {
            set_uint_uint(value_.get(), old_uint64_count, new_uint64_count, new_value.get());
            filter_highbits_uint(new_value.get(), new_uint64_count, bit_count);
        }

        // Deallocate any owned pointers.
        reset();

        // Update class.
        value_.swap_with(new_value);
        bit_count_ = bit_count;
    }

    BigUInt &BigUInt::operator =(const BigUInt& assign)
    {
        // Do nothing if same thing.
        if (&assign == this)
        {
            return *this;
        }

        // Verify assigned value will fit within bit count.
        int assign_sig_bit_count = assign.significant_bit_count();
        if (assign_sig_bit_count > bit_count_)
        {
            // Size is too large to currently fit, so resize.
            resize(assign_sig_bit_count);
        }

        // Copy over value.
        int assign_uint64_count = divide_round_up(assign_sig_bit_count, bits_per_uint64);
        int uint64_count = divide_round_up(bit_count_, bits_per_uint64);
        if (uint64_count > 0)
        {
            set_uint_uint(assign.value_.get(), assign_uint64_count, uint64_count, value_.get());
        }
        return *this;
    }

    BigUInt &BigUInt::operator =(const string &hex_value)
    {
        int hex_value_length = hex_value.length();
        int assign_bit_count = get_hex_string_bit_count(hex_value.data(), hex_value_length);
        if (assign_bit_count > bit_count_)
        {
            // Size is too large to currently fit, so resize.
            resize(assign_bit_count);
        }
        if (bit_count_ > 0)
        {
            // Copy over value.
            int uint64_count = divide_round_up(bit_count_, bits_per_uint64);
            hex_string_to_uint(hex_value.data(), hex_value_length, uint64_count, value_.get());
        }
        return *this;
    }

    BigUInt BigUInt::operator /(const BigUInt& operand2) const
    {
        int result_bits = significant_bit_count();
        int operand2_bits = operand2.significant_bit_count();
        if (operand2_bits == 0)
        {
            throw invalid_argument("operand2 must be positive");
        }
        if (operand2_bits > result_bits)
        {
            BigUInt zero(result_bits);
            return zero;
        }
        BigUInt result(result_bits);
        BigUInt remainder(result_bits);
        int uint64_count = divide_round_up(result_bits, bits_per_uint64);
        if (uint64_count > operand2.uint64_count())
        {
            BigUInt operand2resized(result_bits);
            operand2resized = operand2;
            divide_uint_uint(value_.get(), operand2resized.pointer(), uint64_count, result.pointer(), remainder.pointer(), pool_);
        }
        else
        {
            divide_uint_uint(value_.get(), operand2.pointer(), uint64_count, result.pointer(), remainder.pointer(), pool_);
        }
        return result;
    }

    BigUInt BigUInt::divrem(const BigUInt& operand2, BigUInt &remainder) const
    {
        int result_bits = significant_bit_count();
        remainder = *this;
        int operand2_bits = operand2.significant_bit_count();
        if (operand2_bits > result_bits)
        {
            BigUInt zero;
            return zero;
        }
        BigUInt quotient(result_bits);
        int uint64_count = remainder.uint64_count();
        if (uint64_count > operand2.uint64_count())
        {
            BigUInt operand2resized(result_bits);
            operand2resized = operand2;
            divide_uint_uint_inplace(remainder.pointer(), operand2resized.pointer(), uint64_count, quotient.pointer(), pool_);
        }
        else
        {
            divide_uint_uint_inplace(remainder.pointer(), operand2.pointer(), uint64_count, quotient.pointer(), pool_);
        }
        return quotient;
    }

    void BigUInt::save(ostream &stream) const
    {
        int32_t bit_count32 = static_cast<int32_t>(bit_count_);
        stream.write(reinterpret_cast<const char*>(&bit_count32), sizeof(int32_t));
        int uint64_count = divide_round_up(bit_count_, bits_per_uint64);
        stream.write(reinterpret_cast<const char*>(value_.get()), uint64_count * bytes_per_uint64);
    }

    void BigUInt::load(istream &stream)
    {
        int32_t read_bit_count = 0;
        stream.read(reinterpret_cast<char*>(&read_bit_count), sizeof(int32_t));
        if (read_bit_count > bit_count_)
        {
            // Size is too large to currently fit, so resize.
            resize(read_bit_count);
        }
        int read_uint64_count = divide_round_up(read_bit_count, bits_per_uint64);
        stream.read(reinterpret_cast<char*>(value_.get()), read_uint64_count * bytes_per_uint64);

        // Zero any extra space.
        int uint64_count = divide_round_up(bit_count_, bits_per_uint64);
        if (uint64_count > read_uint64_count)
        {
            set_zero_uint(uint64_count - read_uint64_count, value_.get() + read_uint64_count);
        }
    }
}
