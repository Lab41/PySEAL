#include "biguint.h"
#include "util/common.h"
#include "util/uintcore.h"
#include <algorithm>
#include "util/uintarith.h"
#include "util/uintarithmod.h"

using namespace std;
using namespace seal::util;

namespace seal
{
    BigUInt::BigUInt() : value_(nullptr), bit_count_(0), is_alias_(false)
    {
    }

    BigUInt::BigUInt(int bit_count) : value_(nullptr), bit_count_(0), is_alias_(false)
    {
        resize(bit_count);
    }

    BigUInt::BigUInt(const string &hex_value) : value_(nullptr), bit_count_(0), is_alias_(false)
    {
        operator =(hex_value);
    }

    BigUInt::BigUInt(int bit_count, const string &hex_value) : value_(nullptr), bit_count_(0), is_alias_(false)
    {
        resize(bit_count);
        operator =(hex_value);
        if (bit_count != bit_count_)
        {
            resize(bit_count);
        }
    }

    BigUInt::BigUInt(int bit_count, uint64_t *value) : value_(nullptr), bit_count_(0), is_alias_(false)
    {
        alias(bit_count, value);
    } 

    BigUInt::BigUInt(int bit_count, uint64_t value) : value_(nullptr), bit_count_(0), is_alias_(false)
    {
        resize(bit_count);
        operator =(value);
        if (bit_count != bit_count_)
        {
            resize(bit_count);
        }
    }

    BigUInt::BigUInt(const BigUInt& copy) : value_(nullptr), bit_count_(0), is_alias_(false)
    {
        resize(copy.bit_count());
        operator =(copy);
    }

    BigUInt::~BigUInt()
    {
        reset();
    }

    int BigUInt::byte_count() const
    {
        return divide_round_up(bit_count_, bits_per_byte);
    }

    int BigUInt::uint64_count() const
    {
        return divide_round_up(bit_count_, bits_per_uint64);
    }

    int BigUInt::significant_bit_count() const
    {
        if (bit_count_ == 0)
        {
            return 0;
        }
        int uint64_count = divide_round_up(bit_count_, bits_per_uint64);
        return get_significant_bit_count_uint(value_, uint64_count);
    }

    double BigUInt::to_double() const
    {
        const double TwoToThe64 = 18446744073709551616.0;
        double result = 0;
        int uint64_count = divide_round_up(bit_count_, bits_per_uint64);
        for (int i = uint64_count - 1; i >= 0; --i)
        {
            result *= TwoToThe64;
            result += value_[i];
        }
        return result;
    }

    string BigUInt::to_string() const
    {
        int uint64_count = divide_round_up(bit_count_, bits_per_uint64);
        return uint64_to_hex_string(value_, uint64_count);
    }

    string BigUInt::to_dec_string() const
    {
        int uint64_count = divide_round_up(bit_count_, bits_per_uint64);
        return uint64_to_dec_string(value_, uint64_count, *MemoryPool::default_pool());
    }

    bool BigUInt::is_zero() const
    {
        if (bit_count_ == 0)
        {
            return true;
        }
        int uint64_count = divide_round_up(bit_count_, bits_per_uint64);
        return is_zero_uint(value_, uint64_count);
    }

    uint8_t BigUInt::operator[](int index) const
    {
        int byte_count = divide_round_up(bit_count_, bits_per_byte);
        if (index < 0 || index >= byte_count)
        {
            throw out_of_range("index must be within [0, byte count)");
        }
        return *get_uint64_byte(value_, index);
    }

    uint8_t &BigUInt::operator[](int index)
    {
        int byte_count = divide_round_up(bit_count_, bits_per_byte);
        if (index < 0 || index >= byte_count)
        {
            throw out_of_range("index must be within [0, byte count)");
        }
        return *get_uint64_byte(value_, index);
    }

    void BigUInt::set_zero()
    {
        if (bit_count_ > 0)
        {
            int uint64_count = divide_round_up(bit_count_, bits_per_uint64);
            return set_zero_uint(uint64_count, value_);
        }
    }

    void BigUInt::resize(int bit_count)
    {
        if (bit_count < 0)
        {
            throw invalid_argument("bit_count must be non-negative");
        }
        if (is_alias_)
        {
            throw logic_error("Cannot resize an aliased BigUInt");
        }
        if (bit_count == bit_count_)
        {
            return;
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
        uint64_t *new_value = new_uint64_count > 0 ? new uint64_t[new_uint64_count] : nullptr;

        // Copy over old value.
        if (new_uint64_count > 0)
        {
            set_uint_uint(value_, old_uint64_count, new_uint64_count, new_value);
            filter_highbits_uint(new_value, new_uint64_count, bit_count);
        }

        // Deallocate any owned pointers.
        reset();

        // Update class.
        value_ = new_value;
        bit_count_ = bit_count;
        is_alias_ = false;
    }

    void BigUInt::alias(int bit_count, std::uint64_t *value)
    {
        if (bit_count < 0)
        {
            throw invalid_argument("bit_count must be non-negative");
        }
        if (value == nullptr && bit_count > 0)
        {
            throw invalid_argument("value must be non-null for non-zero bit count");
        }

        // Deallocate any owned pointers.
        reset();

        // Update class.
        value_ = value;
        bit_count_ = bit_count;
        is_alias_ = true;
    }

    void BigUInt::unalias()
    {
        if (!is_alias_)
        {
            throw logic_error("BigUInt is not an alias");
        }

        // Reset class.
        reset();
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
            set_uint_uint(assign.value_, assign_uint64_count, uint64_count, value_);
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
            hex_string_to_uint64(hex_value.data(), hex_value_length, uint64_count, value_);
        }
        return *this;
    }

    BigUInt &BigUInt::operator =(uint64_t value)
    {
        int assign_bit_count = get_significant_bit_count(value);
        if (assign_bit_count > bit_count_)
        {
            // Size is too large to currently fit, so resize.
            resize(assign_bit_count);
        }
        if (bit_count_ > 0)
        {
            int uint64_count = divide_round_up(bit_count_, bits_per_uint64);
            set_uint(value, uint64_count, value_);
        }
        return *this;
    }

    BigUInt BigUInt::operator +() const
    {
        BigUInt result;
        result = *this;
        return result;
    }

    BigUInt BigUInt::operator -() const
    {
        BigUInt result(bit_count_);
        negate_uint(value_, result.uint64_count(), result.pointer());
        filter_highbits_uint(result.pointer(), result.uint64_count(), result.bit_count());
        return result;
    }

    BigUInt BigUInt::operator ~() const
    {
        BigUInt result(bit_count_);
        not_uint(value_, result.uint64_count(), result.pointer());
        filter_highbits_uint(result.pointer(), result.uint64_count(), result.bit_count());
        return result;
    }

    BigUInt &BigUInt::operator ++()
    {
        if (increment_uint(value_, uint64_count(), value_))
        {
            int carry_bit = bit_count_;
            resize(bit_count_ + 1);
            set_bit_uint(value_, uint64_count(), carry_bit);
        }
        bit_count_ = max(bit_count_, significant_bit_count());
        return *this;
    }

    BigUInt &BigUInt::operator --()
    {
        decrement_uint(value_, uint64_count(), value_);
        filter_highbits_uint(value_, uint64_count(), bit_count_);
        return *this;
    }

    BigUInt BigUInt::operator ++(int postfix)
    {
        BigUInt result;
        result = *this;
        if (increment_uint(value_, uint64_count(), value_))
        {
            int carry_bit = bit_count_;
            resize(bit_count_ + 1);
            set_bit_uint(value_, uint64_count(), carry_bit);
        }
        bit_count_ = max(bit_count_, significant_bit_count());
        return result;
    }

    BigUInt BigUInt::operator --(int postfix)
    {
        BigUInt result;
        result = *this;
        decrement_uint(value_, uint64_count(), value_);
        filter_highbits_uint(value_, uint64_count(), bit_count_);
        return result;
    }

    BigUInt BigUInt::operator +(const BigUInt& operand2) const
    {
        int result_bits = max(significant_bit_count(), operand2.significant_bit_count()) + 1;
        BigUInt result(result_bits);
        add_uint_uint(value_, uint64_count(), operand2.pointer(), operand2.uint64_count(), false, result.uint64_count(), result.pointer());
        return result;
    }

    BigUInt BigUInt::operator -(const BigUInt& operand2) const
    {
        int result_bits = max(bit_count_, operand2.bit_count());
        BigUInt result(result_bits);
        sub_uint_uint(value_, uint64_count(), operand2.pointer(), operand2.uint64_count(), false, result.uint64_count(), result.pointer());
        filter_highbits_uint(result.pointer(), result.uint64_count(), result_bits);
        return result;
    }

    BigUInt BigUInt::operator *(const BigUInt& operand2) const
    {
        int result_bits = significant_bit_count() + operand2.significant_bit_count();
        BigUInt result(result_bits);
        multiply_uint_uint(value_, uint64_count(), operand2.pointer(), operand2.uint64_count(), result.uint64_count(), result.pointer());
        return result;
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
        MemoryPool &pool = *MemoryPool::default_pool();
        int uint64_count = divide_round_up(result_bits, bits_per_uint64);
        if (uint64_count > operand2.uint64_count())
        {
            BigUInt operand2resized(result_bits);
            operand2resized = operand2;
            divide_uint_uint(value_, operand2resized.pointer(), uint64_count, result.pointer(), remainder.pointer(), pool);
        }
        else
        {
            divide_uint_uint(value_, operand2.pointer(), uint64_count, result.pointer(), remainder.pointer(), pool);
        }
        return result;
    }

    BigUInt BigUInt::operator %(const BigUInt& operand2) const
    {
        if (operand2.is_zero())
        {
            throw invalid_argument("operand2 must be positive");
        }
        MemoryPool &pool = *MemoryPool::default_pool();
        Modulus modulus(operand2.pointer(), operand2.uint64_count(), pool);
        int result_bits = significant_bit_count();
        BigUInt result(result_bits);
        result = *this;
        int uint64_count = divide_round_up(result_bits, bits_per_uint64);
        modulo_uint_inplace(result.pointer(), uint64_count, modulus, pool);
        return result;
    }

    BigUInt BigUInt::operator ^(const BigUInt& operand2) const
    {
        int result_bits = max(bit_count_, operand2.bit_count());
        BigUInt result(result_bits);
        int uint64_count = result.uint64_count();
        if (uint64_count != this->uint64_count())
        {
            result = *this;
            xor_uint_uint(result.pointer(), operand2.pointer(), uint64_count, result.pointer());
        }
        else if (uint64_count != operand2.uint64_count())
        {
            result = operand2;
            xor_uint_uint(result.pointer(), value_, uint64_count, result.pointer());
        }
        else
        {
            xor_uint_uint(value_, operand2.pointer(), uint64_count, result.pointer());
        }
        return result;
    }

    BigUInt BigUInt::operator &(const BigUInt& operand2) const
    {
        int result_bits = max(bit_count_, operand2.bit_count());
        BigUInt result(result_bits);
        int uint64_count = result.uint64_count();
        if (uint64_count != this->uint64_count())
        {
            result = *this;
            and_uint_uint(result.pointer(), operand2.pointer(), uint64_count, result.pointer());
        }
        else if (uint64_count != operand2.uint64_count())
        {
            result = operand2;
            and_uint_uint(result.pointer(), value_, uint64_count, result.pointer());
        }
        else
        {
            and_uint_uint(value_, operand2.pointer(), uint64_count, result.pointer());
        }
        return result;
    }

    BigUInt BigUInt::operator |(const BigUInt& operand2) const
    {
        int result_bits = max(bit_count_, operand2.bit_count());
        BigUInt result(result_bits);
        int uint64_count = result.uint64_count();
        if (uint64_count != this->uint64_count())
        {
            result = *this;
            or_uint_uint(result.pointer(), operand2.pointer(), uint64_count, result.pointer());
        }
        else if (uint64_count != operand2.uint64_count())
        {
            result = operand2;
            or_uint_uint(result.pointer(), value_, uint64_count, result.pointer());
        }
        else
        {
            or_uint_uint(value_, operand2.pointer(), uint64_count, result.pointer());
        }
        return result;
    }

    int BigUInt::compareto(const BigUInt &compare) const
    {
        int uint64_count = divide_round_up(bit_count_, bits_per_uint64);
        int compare_uint64_count = divide_round_up(compare.bit_count_, bits_per_uint64);
        return compare_uint_uint(value_, uint64_count, compare.value_, compare_uint64_count);
    }

    bool BigUInt::operator <(const BigUInt& compare) const
    {
        int uint64_count = divide_round_up(bit_count_, bits_per_uint64);
        int compare_uint64_count = divide_round_up(compare.bit_count_, bits_per_uint64);
        return compare_uint_uint(value_, uint64_count, compare.value_, compare_uint64_count) < 0;
    }

    bool BigUInt::operator >(const BigUInt& compare) const
    {
        int uint64_count = divide_round_up(bit_count_, bits_per_uint64);
        int compare_uint64_count = divide_round_up(compare.bit_count_, bits_per_uint64);
        return compare_uint_uint(value_, uint64_count, compare.value_, compare_uint64_count) > 0;
    }

    bool BigUInt::operator ==(const BigUInt& compare) const
    {
        int uint64_count = divide_round_up(bit_count_, bits_per_uint64);
        int compare_uint64_count = divide_round_up(compare.bit_count_, bits_per_uint64);
        return compare_uint_uint(value_, uint64_count, compare.value_, compare_uint64_count) == 0;
    }

    bool BigUInt::operator !=(const BigUInt& compare) const
    {
        return !(operator ==(compare));
    }

    bool BigUInt::operator <=(const BigUInt& compare) const
    {
        int uint64_count = divide_round_up(bit_count_, bits_per_uint64);
        int compare_uint64_count = divide_round_up(compare.bit_count_, bits_per_uint64);
        return compare_uint_uint(value_, uint64_count, compare.value_, compare_uint64_count) <= 0;
    }

    bool BigUInt::operator >=(const BigUInt& compare) const
    {
        int uint64_count = divide_round_up(bit_count_, bits_per_uint64);
        int compare_uint64_count = divide_round_up(compare.bit_count_, bits_per_uint64);
        return compare_uint_uint(value_, uint64_count, compare.value_, compare_uint64_count) >= 0;
    }

    BigUInt BigUInt::operator <<(int shift) const
    {
        if (shift < 0)
        {
            throw invalid_argument("shift must be non-negative");
        }
        int result_bits = significant_bit_count() + shift;
        BigUInt result(result_bits);
        result = *this;
        left_shift_uint(result.pointer(), shift, result.uint64_count(), result.pointer());
        return result;
    }

    BigUInt BigUInt::operator >>(int shift) const
    {
        if (shift < 0)
        {
            throw invalid_argument("shift must be non-negative");
        }
        int result_bits = significant_bit_count() - shift;
        if (result_bits <= 0)
        {
            BigUInt zero;
            return zero;
        }
        BigUInt result(result_bits);
        result = *this;
        right_shift_uint(result.pointer(), shift, result.uint64_count(), result.pointer());
        return result;
    }

    BigUInt &BigUInt::operator +=(const BigUInt& operand2)
    {
        int result_bits = max(significant_bit_count(), operand2.significant_bit_count()) + 1;
        if (bit_count_ < result_bits)
        {
            resize(result_bits);
        }
        add_uint_uint(value_, uint64_count(), operand2.pointer(), operand2.uint64_count(), false, uint64_count(), value_);
        return *this;
    }

    BigUInt &BigUInt::operator -=(const BigUInt& operand2)
    {
        int result_bits = max(bit_count_, operand2.bit_count());
        if (bit_count_ < result_bits)
        {
            resize(result_bits);
        }
        sub_uint_uint(value_, uint64_count(), operand2.pointer(), operand2.uint64_count(), false, uint64_count(), value_);
        filter_highbits_uint(value_, uint64_count(), result_bits);
        return *this;
    }

    BigUInt &BigUInt::operator *=(const BigUInt& operand2)
    {
        *this = *this * operand2;
        return *this;
    }

    BigUInt &BigUInt::operator /=(const BigUInt& operand2)
    {
        *this = *this / operand2;
        return *this;
    }

    BigUInt &BigUInt::operator %=(const BigUInt& operand2)
    {
        if (operand2.is_zero())
        {
            throw invalid_argument("operand2 must be positive");
        }
        MemoryPool &pool = *MemoryPool::default_pool();
        Modulus modulus(operand2.pointer(), operand2.uint64_count(), pool);
        modulo_uint_inplace(value_, uint64_count(), modulus, pool);
        return *this;
    }

    BigUInt &BigUInt::operator ^=(const BigUInt& operand2)
    {
        int result_bits = max(bit_count_, operand2.bit_count());
        if (bit_count_ != result_bits)
        {
            resize(result_bits);
        }
        xor_uint_uint(value_, operand2.pointer(), operand2.uint64_count(), value_);
        return *this;
    }

    BigUInt &BigUInt::operator &=(const BigUInt& operand2)
    {
        int result_bits = max(bit_count_, operand2.bit_count());
        if (bit_count_ != result_bits)
        {
            resize(result_bits);
        }
        and_uint_uint(value_, operand2.pointer(), operand2.uint64_count(), value_);
        return *this;
    }

    BigUInt &BigUInt::operator |=(const BigUInt& operand2)
    {
        int result_bits = max(bit_count_, operand2.bit_count());
        if (bit_count_ != result_bits)
        {
            resize(result_bits);
        }
        or_uint_uint(value_, operand2.pointer(), operand2.uint64_count(), value_);
        return *this;
    }

    BigUInt &BigUInt::operator <<=(int shift)
    {
        if (shift < 0)
        {
            throw invalid_argument("shift must be non-negative");
        }
        int result_bits = significant_bit_count() + shift;
        if (bit_count_ < result_bits)
        {
            resize(result_bits);
        }
        left_shift_uint(value_, shift, uint64_count(), value_);
        return *this;
    }

    BigUInt &BigUInt::operator >>=(int shift)
    {
        if (shift < 0)
        {
            throw invalid_argument("shift must be non-negative");
        }
        if (shift > bit_count_)
        {
            set_zero();
            return *this;
        }
        right_shift_uint(value_, shift, uint64_count(), value_);
        return *this;
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
        MemoryPool &pool = *MemoryPool::default_pool();
        BigUInt quotient(result_bits);
        int uint64_count = remainder.uint64_count();
        if (uint64_count > operand2.uint64_count())
        {
            BigUInt operand2resized(result_bits);
            operand2resized = operand2;
            divide_uint_uint_inplace(remainder.pointer(), operand2resized.pointer(), uint64_count, quotient.pointer(), pool);
        }
        else
        {
            divide_uint_uint_inplace(remainder.pointer(), operand2.pointer(), uint64_count, quotient.pointer(), pool);
        }
        return quotient;
    }

    BigUInt BigUInt::modinv(const BigUInt &modulus) const
    {
        if (modulus.is_zero())
        {
            throw invalid_argument("modulus must be positive");
        }
        int result_bits = modulus.significant_bit_count();
        if (*this >= modulus)
        {
            throw invalid_argument("modulus must be greater than BigUInt");
        }
        MemoryPool &pool = *MemoryPool::default_pool();
        BigUInt result(result_bits);
        result = *this;
        if (!try_invert_uint_mod(result.pointer(), modulus.pointer(), result.uint64_count(), result.pointer(), pool))
        {
            throw invalid_argument("BigUInt and modulus are not co-prime");
        }
        return result;
    }

    bool BigUInt::trymodinv(const BigUInt &modulus, BigUInt &inverse) const
    {
        if (modulus.is_zero())
        {
            throw invalid_argument("modulus must be positive");
        }
        int result_bits = modulus.significant_bit_count();
        if (*this >= modulus)
        {
            throw invalid_argument("modulus must be greater than BigUInt");
        }
        if (inverse.bit_count() < result_bits)
        {
            inverse.resize(result_bits);
        }
        inverse = *this;
        MemoryPool &pool = *MemoryPool::default_pool();
        return try_invert_uint_mod(inverse.pointer(), modulus.pointer(), inverse.uint64_count(), inverse.pointer(), pool);
    }

    void BigUInt::save(ostream &stream) const
    {
        int32_t bit_count32 = static_cast<int32_t>(bit_count_);
        stream.write(reinterpret_cast<const char*>(&bit_count32), sizeof(int32_t));
        int uint64_count = divide_round_up(bit_count_, bits_per_uint64);
        stream.write(reinterpret_cast<const char*>(value_), uint64_count * bytes_per_uint64);
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
        stream.read(reinterpret_cast<char*>(value_), read_uint64_count * bytes_per_uint64);

        // Zero any extra space.
        int uint64_count = divide_round_up(bit_count_, bits_per_uint64);
        if (uint64_count > read_uint64_count)
        {
            set_zero_uint(uint64_count - read_uint64_count, value_ + read_uint64_count);
        }
    }

    void BigUInt::reset()
    {
        if (!is_alias_ && value_ != nullptr)
        {
            delete[] value_;
        }
        value_ = nullptr;
        bit_count_ = 0;
        is_alias_ = false;
    }

    void BigUInt::duplicate_to(BigUInt &destination) const
    {
        destination.resize(this->bit_count_);
        destination = *this;
    }

    void BigUInt::duplicate_from(const BigUInt &value)
    {
        this->resize(value.bit_count_);
        *this = value;
    }

    BigUInt &BigUInt::operator =(BigUInt &&assign)
    {
        // Deallocate any memory allocated.
        reset();

        // Copy all member variables
        bit_count_ = assign.bit_count_;
        value_ = assign.value_;
        is_alias_ = assign.is_alias_;

        // Pointer in assign has been taken over so set it to nullptr
        assign.value_ = nullptr;
        assign.is_alias_ = false;
        assign.bit_count_ = 0;

        return *this;
    }

    BigUInt::BigUInt(BigUInt &&source) noexcept : value_(source.value_), bit_count_(source.bit_count_), is_alias_(source.is_alias_)
    {
        // Pointer in source has been taken over so set it to nullptr
        source.value_ = nullptr;
        source.is_alias_ = false;
        source.bit_count_ = 0;
    }
}
