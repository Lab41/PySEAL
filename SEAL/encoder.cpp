#include <stdexcept>
#include <algorithm>
#include "encoder.h"
#include "util/common.h"
#include "util/uintarith.h"
#include "util/polyarith.h"

using namespace std;
using namespace seal::util;

namespace seal
{
    BinaryEncoder::BinaryEncoder(const BigUInt &plain_modulus) : plain_modulus_(plain_modulus)
    {
        if (plain_modulus.significant_bit_count() <= 1)
        {
            throw invalid_argument("plain_modulus must be at least 2");
        }
        coeff_neg_threshold_.resize(plain_modulus_.bit_count());
        half_round_up_uint(plain_modulus_.pointer(), plain_modulus_.uint64_count(), coeff_neg_threshold_.pointer());
        neg_one_.resize(plain_modulus_.bit_count());
        decrement_uint(plain_modulus_.pointer(), plain_modulus_.uint64_count(), neg_one_.pointer());
    }

    BigPoly BinaryEncoder::encode(uint64_t value)
    {
        BigPoly result;
        encode(value, result);
        return result;
    }

    void BinaryEncoder::encode(uint64_t value, BigPoly &destination)
    {
        int encode_coeff_count = get_significant_bit_count(value);
        if (destination.coeff_count() < encode_coeff_count || destination.coeff_bit_count() == 0)
        {
            destination.resize(max(destination.coeff_count(), encode_coeff_count), max(destination.coeff_bit_count(), 1));
        }
        destination.set_zero();
        int coeff_index = 0;
        while (value != 0)
        {
            if ((value & 1) != 0)
            {
                destination[coeff_index] = 1;
            }
            value >>= 1;
            coeff_index++;
        }
    }

    BigPoly BinaryEncoder::encode(int64_t value)
    {
        BigPoly result;
        encode(value, result);
        return result;
    }

    void BinaryEncoder::encode(int64_t value, BigPoly &destination)
    {
        if (value < 0)
        {
            uint64_t pos_value = static_cast<uint64_t>(-value);
            int encode_coeff_count = get_significant_bit_count(pos_value);
            int encode_coeff_bit_count = neg_one_.significant_bit_count();
            if (destination.coeff_count() < encode_coeff_count || destination.coeff_bit_count() < encode_coeff_bit_count)
            {
                destination.resize(max(destination.coeff_count(), encode_coeff_count), max(destination.coeff_bit_count(), encode_coeff_bit_count));
            }
            destination.set_zero();

            int coeff_index = 0;
            while (pos_value != 0)
            {
                if ((pos_value & 1) != 0)
                {
                    destination[coeff_index] = neg_one_;
                }
                pos_value >>= 1;
                coeff_index++;
            }
        }
        else
        {
            encode(static_cast<uint64_t>(value), destination);
        }
    }

    BigPoly BinaryEncoder::encode(const BigUInt &value)
    {
        BigPoly result;
        encode(value, result);
        return result;
    }

    void BinaryEncoder::encode(const BigUInt &value, BigPoly &destination)
    {
        int encode_coeff_count = value.significant_bit_count();
        if (destination.coeff_count() < encode_coeff_count || destination.coeff_bit_count() == 0)
        {
            destination.resize(max(destination.coeff_count(), encode_coeff_count), max(destination.coeff_bit_count(), 1));
        }
        destination.set_zero();
        int coeff_index = 0;
        int coeff_count = value.significant_bit_count();
        int coeff_uint64_count = value.uint64_count();
        while (coeff_index < coeff_count)
        {
            if (is_bit_set_uint(value.pointer(), coeff_uint64_count, coeff_index))
            {
                destination[coeff_index] = 1;
            }
            coeff_index++;
        }
    }

    uint32_t BinaryEncoder::decode_uint32(const BigPoly &poly)
    {
        uint64_t value64 = decode_uint64(poly);
        if (value64 > UINT32_MAX)
        {
            throw invalid_argument("output out of range");
        }
        return static_cast<uint32_t>(value64);
    }

    uint64_t BinaryEncoder::decode_uint64(const BigPoly &poly)
    {
        BigUInt bigvalue = decode_biguint(poly);
        int bit_count = bigvalue.significant_bit_count();
        if (bit_count > bits_per_uint64)
        {
            // Decoded value has more bits than fit in a 64-bit uint.
            throw invalid_argument("output out of range");
        }
        return bit_count > 0 ? bigvalue.pointer()[0] : 0;
    }

    int32_t BinaryEncoder::decode_int32(const BigPoly &poly)
    {
        int64_t value64 = decode_int64(poly);
        if (value64 < INT32_MIN || value64 > INT32_MAX)
        {
            throw invalid_argument("output out of range");
        }
        return static_cast<int32_t>(value64);
    }

    int64_t BinaryEncoder::decode_int64(const BigPoly &poly)
    {
        // Determine plain_modulus width.
        int plain_modulus_bits = plain_modulus_.significant_bit_count();

        int sig_uint64_count = divide_round_up(plain_modulus_bits, bits_per_uint64);
        Pointer pos_value(allocate_uint(sig_uint64_count, pool_));

        // Determine coefficient threshold for negative numbers.
        int coeff_neg_threshold_bits = coeff_neg_threshold_.significant_bit_count();

        int64_t result = 0;
        for (int bit_index = poly.significant_coeff_count() - 1; bit_index >= 0; --bit_index)
        {
            const BigUInt &coeff = poly[bit_index];

            // Left shift result.
            int64_t next_result = result << 1;
            if ((next_result < 0) != (result < 0))
            {
                // Check for overflow.
                throw invalid_argument("output out of range");
            }

            // Get sign/magnitude of coefficient.
            int coeff_bit_count = coeff.significant_bit_count();
            if (coeff_bit_count > plain_modulus_bits)
            {
                // Coefficient has more bits than plain_modulus.
                throw invalid_argument("poly is not a valid plaintext polynomial");
            }
            bool coeff_is_negative = coeff_bit_count > coeff_neg_threshold_bits ||
                (coeff_bit_count == coeff_neg_threshold_bits && is_greater_than_or_equal_uint_uint(coeff.pointer(), coeff_neg_threshold_.pointer(), sig_uint64_count));
            const uint64_t *pos_pointer;
            if (coeff_is_negative)
            {
                if (sub_uint_uint(plain_modulus_.pointer(), coeff.pointer(), sig_uint64_count, pos_value.get()))
                {
                    // Check for borrow, which means value is greater than plain_modulus.
                    throw invalid_argument("poly is not a valid plaintext polynomial");
                }
                pos_pointer = pos_value.get();
                coeff_bit_count = get_significant_bit_count_uint(pos_value.get(), sig_uint64_count);
            }
            else
            {
                pos_pointer = coeff.pointer();
            }
            if (coeff_bit_count > bits_per_uint64 - 1)
            {
                // Absolute value of coefficient is too large to represent in a int64_t, so overflow.
                throw invalid_argument("output out of range");
            }
            int64_t coeff_value = static_cast<int64_t>(pos_pointer[0]);
            if (coeff_is_negative)
            {
                coeff_value = -coeff_value;
            }
            bool next_result_was_negative = next_result < 0;
            next_result += coeff_value;
            bool next_result_is_negative = next_result < 0;
            if (next_result_was_negative == coeff_is_negative && next_result_was_negative != next_result_is_negative)
            {
                // Accumulation and coefficient had same signs, but accumulator changed signs after addition, so must be overflow.
                throw invalid_argument("output out of range");
            }
            result = next_result;
        }
        return result;
    }

    BigUInt BinaryEncoder::decode_biguint(const BigPoly &poly)
    {
        // Determine plain_modulus width.
        int plain_modulus_bits = plain_modulus_.significant_bit_count();

        int sig_uint64_count = divide_round_up(plain_modulus_bits, bits_per_uint64);
        Pointer pos_value(allocate_uint(sig_uint64_count, pool_));

        // Determine coefficient threshold for negative numbers.
        int coeff_neg_threshold_bits = coeff_neg_threshold_.significant_bit_count();

        int result_uint64_count = 1;
        int result_bit_capacity = result_uint64_count * bits_per_uint64;
        BigUInt resultint(result_bit_capacity);
        bool result_is_negative = false;
        uint64_t *result = resultint.pointer();
        for (int bit_index = poly.significant_coeff_count() - 1; bit_index >= 0; --bit_index)
        {
            const BigUInt &coeff = poly[bit_index];

            // Left shift result, resizing if highest bit set.
            if (is_bit_set_uint(result, result_uint64_count, result_bit_capacity - 1))
            {
                // Resize to make bigger.
                result_uint64_count++;
                result_bit_capacity = result_uint64_count * bits_per_uint64;
                resultint.resize(result_bit_capacity);
                result = resultint.pointer();
            }
            left_shift_uint(result, 1, result_uint64_count, result);

            // Get sign/magnitude of coefficient.
            int coeff_bit_count = coeff.significant_bit_count();
            if (coeff_bit_count > plain_modulus_bits)
            {
                // Coefficient has more bits than plain_modulus.
                throw invalid_argument("poly is not a valid plaintext polynomial");
            }
            bool coeff_is_negative = coeff_bit_count > coeff_neg_threshold_bits ||
                (coeff_bit_count == coeff_neg_threshold_bits && is_greater_than_or_equal_uint_uint(coeff.pointer(), coeff_neg_threshold_.pointer(), sig_uint64_count));
            const uint64_t *pos_pointer;
            if (coeff_is_negative)
            {
                if (sub_uint_uint(plain_modulus_.pointer(), coeff.pointer(), sig_uint64_count, pos_value.get()))
                {
                    // Check for borrow, which means value is greater than plain_modulus.
                    throw invalid_argument("poly is not a valid plaintext polynomial");
                }
                pos_pointer = pos_value.get();
                coeff_bit_count = get_significant_bit_count_uint(pos_pointer, sig_uint64_count);
            }
            else
            {
                pos_pointer = coeff.pointer();
            }

            // Verify result is at least as wide as coefficient.
            int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
            if (coeff_bit_count > result_bit_capacity)
            {
                result_uint64_count = coeff_uint64_count;
                result_bit_capacity = result_uint64_count * bits_per_uint64;
                resultint.resize(result_bit_capacity);
                result = resultint.pointer();
            }

            // Add or subtract-in coefficient.
            if (result_is_negative == coeff_is_negative)
            {
                // Result and coefficient have same signs so add.
                if (add_uint_uint(result, result_uint64_count, pos_pointer, coeff_uint64_count, false, result_uint64_count, result))
                {
                    // Add produced a carry that didn't fit, so resize and put it in.
                    int carry_bit_index = result_uint64_count * bits_per_uint64;
                    result_uint64_count++;
                    result_bit_capacity = result_uint64_count * bits_per_uint64;
                    resultint.resize(result_bit_capacity);
                    result = resultint.pointer();
                    set_bit_uint(result, result_uint64_count, carry_bit_index);
                }
            }
            else
            {
                // Result and coefficient have opposite signs so subtract.
                if (sub_uint_uint(result, result_uint64_count, pos_pointer, coeff_uint64_count, false, result_uint64_count, result))
                {
                    // Subtraction produced a borrow so coefficient is larger (in magnitude) than result, so need to negate result.
                    negate_uint(result, result_uint64_count, result);
                    result_is_negative = !result_is_negative;
                }
            }
        }

        // Verify result is non-negative.
        if (result_is_negative && !resultint.is_zero())
        {
            throw invalid_argument("poly must decode to positive value");
        }
        return resultint;
    }

    void BinaryEncoder::decode_biguint(const BigPoly &poly, BigUInt &destination)
    {
        // Determine plain_modulus width.
        int plain_modulus_bits = plain_modulus_.significant_bit_count();

        int sig_uint64_count = divide_round_up(plain_modulus_bits, bits_per_uint64);
        Pointer pos_value(allocate_uint(sig_uint64_count, pool_));

        // Determine coefficient threshold for negative numbers.
        int coeff_neg_threshold_bits = coeff_neg_threshold_.significant_bit_count();

        destination.set_zero();
        int result_uint64_count = destination.uint64_count();
        int result_bit_capacity = result_uint64_count * bits_per_uint64;
        bool result_is_negative = false;
        uint64_t *result = destination.pointer();
        for (int bit_index = poly.significant_coeff_count() - 1; bit_index >= 0; --bit_index)
        {
            const BigUInt &coeff = poly[bit_index];

            // Left shift result, failing if highest bit set.
            if (is_bit_set_uint(result, result_uint64_count, result_bit_capacity - 1))
            {
                throw invalid_argument("poly is not a valid plaintext polynomial");
            }
            left_shift_uint(result, 1, result_uint64_count, result);

            // Get sign/magnitude of coefficient.
            int coeff_bit_count = coeff.significant_bit_count();
            if (coeff_bit_count > plain_modulus_bits)
            {
                // Coefficient has more bits than plain_modulus.
                throw invalid_argument("poly is not a valid plaintext polynomial");
            }
            bool coeff_is_negative = coeff_bit_count > coeff_neg_threshold_bits ||
                (coeff_bit_count == coeff_neg_threshold_bits && is_greater_than_or_equal_uint_uint(coeff.pointer(), coeff_neg_threshold_.pointer(), sig_uint64_count));
            const uint64_t *pos_pointer;
            if (coeff_is_negative)
            {
                if (sub_uint_uint(plain_modulus_.pointer(), coeff.pointer(), sig_uint64_count, pos_value.get()))
                {
                    // Check for borrow, which means value is greater than plain_modulus.
                    throw invalid_argument("poly is not a valid plaintext polynomial");
                }
                pos_pointer = pos_value.get();
                coeff_bit_count = get_significant_bit_count_uint(pos_pointer, sig_uint64_count);
            }
            else
            {
                pos_pointer = coeff.pointer();
            }

            // Verify result is at least as wide as coefficient.
            int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
            if (coeff_bit_count > result_bit_capacity)
            {
                // Coefficient has more bits than destination.
                throw invalid_argument("output out of range");
            }

            // Add or subtract-in coefficient.
            if (result_is_negative == coeff_is_negative)
            {
                // Result and coefficient have same signs so add.
                if (add_uint_uint(result, result_uint64_count, pos_pointer, coeff_uint64_count, false, result_uint64_count, result))
                {
                    // Add produced a carry that didn't fit.
                    throw invalid_argument("output out of range");
                }
            }
            else
            {
                // Result and coefficient have opposite signs so subtract.
                if (sub_uint_uint(result, result_uint64_count, pos_pointer, coeff_uint64_count, false, result_uint64_count, result))
                {
                    // Subtraction produced a borrow so coefficient is larger (in magnitude) than result, so need to negate result.
                    negate_uint(result, result_uint64_count, result);
                    result_is_negative = !result_is_negative;
                }
            }
        }

        // Verify result is non-negative.
        if (result_is_negative && !destination.is_zero())
        {
            throw invalid_argument("poly must decode to a positive value");
        }

        // Verify result fits in actual bit-width (as opposed to capacity) of destination.
        if (destination.significant_bit_count() > destination.bit_count())
        {
            throw invalid_argument("output out of range");
        }
    }

    BalancedEncoder::BalancedEncoder(const BigUInt &plain_modulus, uint64_t base) : plain_modulus_(plain_modulus), base_(base)
    {
        if (base <= 1 || base % 2 == 0)
        {
            throw invalid_argument("base must be an odd integer and at least 3");
        }
        if (plain_modulus <= (base - 1) / 2)
        {
            throw invalid_argument("plain_modulus must be at least (b+1)/2");
        }

        coeff_neg_threshold_.resize(plain_modulus_.bit_count());
        half_round_up_uint(plain_modulus_.pointer(), plain_modulus_.uint64_count(), coeff_neg_threshold_.pointer());
    }

    BigPoly BalancedEncoder::encode(uint64_t value)
    {
        BigPoly result;
        encode(value, result);

        return result;
    }

    void BalancedEncoder::encode(uint64_t value, BigPoly &destination)
    {
        // We estimate the number of coefficints in the expansion
        int encode_coeff_count = static_cast<int>(ceil(static_cast<double>(get_significant_bit_count(value)) / log2(base_)) + 1);
        
        if (destination.coeff_count() < encode_coeff_count || destination.coeff_bit_count() < plain_modulus_.significant_bit_count())
        {
            destination.resize(max(encode_coeff_count, destination.coeff_count()), plain_modulus_.significant_bit_count());
        }
        destination.set_zero();

        int coeff_index = 0;
        while (value != 0)
        {
            uint64_t remainder = value % base_;
            if (0 < remainder && remainder <= base_ / 2)
            {
                destination[coeff_index] = remainder;
            }
            else if (remainder > base_ / 2)
            {
                destination[coeff_index] = base_ - remainder;
                sub_uint_uint(plain_modulus_.pointer(), destination[coeff_index].pointer(), plain_modulus_.uint64_count(), destination[coeff_index].pointer());
            }
            value = (value + base_ / 2) / base_;

            ++coeff_index;
        }
    }

    BigPoly BalancedEncoder::encode(int64_t value)
    {
        BigPoly result;
        encode(value, result);
        return result;
    }

    void BalancedEncoder::encode(int64_t value, BigPoly &destination)
    {
        if (value < 0)
        {
            uint64_t pos_value = static_cast<uint64_t>(-value);
            
            // We estimate the number of coefficints in the expansion
            int encode_coeff_count = static_cast<int>(ceil(static_cast<double>(get_significant_bit_count(value)) / log2(base_)) + 1);
            if (destination.coeff_count() < encode_coeff_count || destination.coeff_bit_count() < plain_modulus_.significant_bit_count())
            {
                destination.resize(max(encode_coeff_count, destination.coeff_count()), plain_modulus_.significant_bit_count());
            }
            destination.set_zero();

            int coeff_index = 0;
            while (pos_value != 0)
            {
                uint64_t remainder = pos_value % base_;
                if (0 < remainder && remainder <= base_ / 2)
                {
                    destination[coeff_index] = remainder;
                    sub_uint_uint(plain_modulus_.pointer(), destination[coeff_index].pointer(), plain_modulus_.uint64_count(), destination[coeff_index].pointer());
                }
                else if (remainder > base_ / 2)
                {
                    destination[coeff_index] = base_ - remainder;
                }
                pos_value = (pos_value + (base_ / 2)) / base_;

                ++coeff_index;
            }
        }
        else
        {
            encode(static_cast<uint64_t>(value), destination);
        }
    }

    BigPoly BalancedEncoder::encode(const BigUInt &value)
    {
        BigPoly result;
        encode(value, result);
        return result;
    }

    void BalancedEncoder::encode(BigUInt value, BigPoly &destination)
    {
        if (value.is_zero())
        {
            destination.set_zero();
            return;
        }

        // We estimate the number of coefficints in the expansion
        int encode_coeff_count = static_cast<int>(ceil(static_cast<double>(value.significant_bit_count()) / log2(base_)) + 1);
        int encode_uint64_count = divide_round_up(encode_coeff_count, bits_per_uint64);

        if (destination.coeff_count() < encode_coeff_count || destination.coeff_bit_count() < plain_modulus_.significant_bit_count())
        {
            destination.resize(max(encode_coeff_count, destination.coeff_count()), plain_modulus_.significant_bit_count());
        }
        int dest_coeff_uint64_count = destination.coeff_uint64_count();
        destination.set_zero();

        Pointer base_uint(allocate_uint(encode_uint64_count, pool_));
        set_uint(base_, encode_uint64_count, base_uint.get());
        Pointer base_div_two_uint(allocate_uint(encode_uint64_count, pool_));
        right_shift_uint(base_uint.get(), 1, encode_uint64_count, base_div_two_uint.get());
        Pointer mod_minus_base(allocate_uint(dest_coeff_uint64_count, pool_));
        sub_uint_uint(plain_modulus_.pointer(), plain_modulus_.uint64_count(), base_uint.get(), encode_uint64_count, false, dest_coeff_uint64_count, mod_minus_base.get());

        Pointer quotient(allocate_uint(encode_uint64_count, pool_));
        Pointer remainder(allocate_uint(encode_uint64_count, pool_));

        int coeff_index = 0;
        while (!value.is_zero())
        {
            divide_uint_uint(value.pointer(), base_uint.get(), encode_uint64_count, quotient.get(), remainder.get(), pool_);
            uint64_t *dest_coeff = get_poly_coeff(destination.pointer(), coeff_index, dest_coeff_uint64_count);
            if (is_greater_than_uint_uint(remainder.get(), base_div_two_uint.get(), encode_uint64_count))
            {
                add_uint_uint(mod_minus_base.get(), dest_coeff_uint64_count, remainder.get(), encode_uint64_count, false, dest_coeff_uint64_count, dest_coeff);
            }
            else if (!is_zero_uint(remainder.get(), encode_uint64_count))
            {
                set_uint_uint(remainder.get(), encode_uint64_count, dest_coeff_uint64_count, dest_coeff);
            }
            add_uint_uint(value.pointer(), base_div_two_uint.get(), encode_uint64_count, value.pointer());
            divide_uint_uint(value.pointer(), base_uint.get(), encode_uint64_count, quotient.get(), remainder.get(), pool_);
            set_uint_uint(quotient.get(), encode_uint64_count, value.pointer());

            ++coeff_index;
        }
    }

    uint32_t BalancedEncoder::decode_uint32(const BigPoly &poly)
    {
        uint64_t value64 = decode_uint64(poly);
        if (value64 > UINT32_MAX)
        {
            throw invalid_argument("output out of range");
        }
        return static_cast<uint32_t>(value64);
    }

    uint64_t BalancedEncoder::decode_uint64(const BigPoly &poly)
    {
        BigUInt bigvalue = decode_biguint(poly);
        int bit_count = bigvalue.significant_bit_count();
        if (bit_count > bits_per_uint64)
        {
            // Decoded value has more bits than fit in a 64-bit uint.
            throw invalid_argument("output out of range");
        }
        return bit_count > 0 ? bigvalue.pointer()[0] : 0;
    }

    int32_t BalancedEncoder::decode_int32(const BigPoly &poly)
    {
        int64_t value64 = decode_int64(poly);
        if (value64 < INT32_MIN || value64 > INT32_MAX)
        {
            throw invalid_argument("output out of range");
        }
        return static_cast<int32_t>(value64);
    }

    int64_t BalancedEncoder::decode_int64(const BigPoly &poly)
    {
        // Determine plain_modulus width.
        int plain_modulus_bits = plain_modulus_.significant_bit_count();

        int sig_uint64_count = divide_round_up(plain_modulus_bits, bits_per_uint64);
        Pointer pos_value(allocate_uint(sig_uint64_count, pool_));

        // Determine coefficient threshold for negative numbers.
        int coeff_neg_threshold_bits = coeff_neg_threshold_.significant_bit_count();

        int64_t result = 0;
        for (int bit_index = poly.significant_coeff_count() - 1; bit_index >= 0; --bit_index)
        {
            const BigUInt &coeff = poly[bit_index];

            // Multiply result by base.
            int64_t next_result = result * base_;

            if ((next_result < 0) != (result < 0))
            {
                // Check for overflow.
                throw invalid_argument("output out of range");
            }

            // Get sign/magnitude of coefficient.
            int coeff_bit_count = coeff.significant_bit_count();
            if (coeff_bit_count > plain_modulus_bits)
            {
                // Coefficient has more bits than plain_modulus.
                throw invalid_argument("poly is not a valid plaintext polynomial");
            }
            bool coeff_is_negative = coeff_bit_count > coeff_neg_threshold_bits ||
                (coeff_bit_count == coeff_neg_threshold_bits && is_greater_than_or_equal_uint_uint(coeff.pointer(), coeff_neg_threshold_.pointer(), sig_uint64_count));
            const uint64_t *pos_pointer;
            if (coeff_is_negative)
            {
                if (sub_uint_uint(plain_modulus_.pointer(), coeff.pointer(), sig_uint64_count, pos_value.get()))
                {
                    // Check for borrow, which means value is greater than plain_modulus.
                    throw invalid_argument("poly is not a valid plaintext polynomial");
                }
                pos_pointer = pos_value.get();
                coeff_bit_count = get_significant_bit_count_uint(pos_value.get(), sig_uint64_count);
            }
            else
            {
                pos_pointer = coeff.pointer();
            }
            if (coeff_bit_count > bits_per_uint64 - 1)
            {
                // Absolute value of coefficient is too large to represent in a int64_t, so overflow.
                throw invalid_argument("output out of range");
            }
            int64_t coeff_value = static_cast<int64_t>(pos_pointer[0]);
            if (coeff_is_negative)
            {
                coeff_value = -coeff_value;
            }
            bool next_result_was_negative = next_result < 0;
            next_result += coeff_value;
            bool next_result_is_negative = next_result < 0;
            if (next_result_was_negative == coeff_is_negative && next_result_was_negative != next_result_is_negative)
            {
                // Accumulation and coefficient had same signs, but accumulator changed signs after addition, so must be overflow.
                throw invalid_argument("output out of range");
            }
            result = next_result;
        }
        return result;
    }

    BigUInt BalancedEncoder::decode_biguint(const BigPoly &poly)
    {
        // Determine plain_modulus width.
        int plain_modulus_bits = plain_modulus_.significant_bit_count();

        int sig_uint64_count = divide_round_up(plain_modulus_bits, bits_per_uint64);
        Pointer pos_value(allocate_uint(sig_uint64_count, pool_));

        // Determine coefficient threshold for negative numbers.
        int coeff_neg_threshold_bits = coeff_neg_threshold_.significant_bit_count();

        int result_uint64_count = 1;
        int result_bit_capacity = result_uint64_count * bits_per_uint64;
        BigUInt resultint(result_bit_capacity);
        bool result_is_negative = false;
        uint64_t *result = resultint.pointer();

        BigUInt base_uint(result_bit_capacity);
        base_uint = base_;
        BigUInt temp_result(result_bit_capacity);

        for (int bit_index = poly.significant_coeff_count() - 1; bit_index >= 0; --bit_index)
        {
            const BigUInt &coeff = poly[bit_index];

            // Multiply result by base. Resize if highest bit set.
            if (is_bit_set_uint(result, result_uint64_count, result_bit_capacity - 1))
            {
                // Resize to make bigger.
                result_uint64_count++;
                result_bit_capacity = result_uint64_count * bits_per_uint64;
                resultint.resize(result_bit_capacity);
                result = resultint.pointer();
            }
            set_uint_uint(result, result_uint64_count, temp_result.pointer());
            multiply_uint_uint(temp_result.pointer(), result_uint64_count, base_uint.pointer(), result_uint64_count, result_uint64_count, result);

            // Get sign/magnitude of coefficient.
            int coeff_bit_count = coeff.significant_bit_count();
            if (coeff_bit_count > plain_modulus_bits)
            {
                // Coefficient has more bits than plain_modulus.
                throw invalid_argument("poly is not a valid plaintext polynomial");
            }
            bool coeff_is_negative = coeff_bit_count > coeff_neg_threshold_bits ||
                (coeff_bit_count == coeff_neg_threshold_bits && is_greater_than_or_equal_uint_uint(coeff.pointer(), coeff_neg_threshold_.pointer(), sig_uint64_count));
            const uint64_t *pos_pointer;
            if (coeff_is_negative)
            {
                if (sub_uint_uint(plain_modulus_.pointer(), coeff.pointer(), sig_uint64_count, pos_value.get()))
                {
                    // Check for borrow, which means value is greater than plain_modulus.
                    throw invalid_argument("poly is not a valid plaintext polynomial");
                }
                pos_pointer = pos_value.get();
                coeff_bit_count = get_significant_bit_count_uint(pos_pointer, sig_uint64_count);
            }
            else
            {
                pos_pointer = coeff.pointer();
            }

            // Verify result is at least as wide as coefficient.
            int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
            if (coeff_bit_count > result_bit_capacity)
            {
                result_uint64_count = coeff_uint64_count;
                result_bit_capacity = result_uint64_count * bits_per_uint64;
                resultint.resize(result_bit_capacity);
                result = resultint.pointer();
            }

            // Add or subtract-in coefficient.
            if (result_is_negative == coeff_is_negative)
            {
                // Result and coefficient have same signs so add.
                if (add_uint_uint(result, result_uint64_count, pos_pointer, coeff_uint64_count, false, result_uint64_count, result))
                {
                    // Add produced a carry that didn't fit, so resize and put it in.
                    int carry_bit_index = result_uint64_count * bits_per_uint64;
                    result_uint64_count++;
                    result_bit_capacity = result_uint64_count * bits_per_uint64;
                    resultint.resize(result_bit_capacity);
                    result = resultint.pointer();
                    set_bit_uint(result, result_uint64_count, carry_bit_index);
                }
            }
            else
            {
                // Result and coefficient have opposite signs so subtract.
                if (sub_uint_uint(result, result_uint64_count, pos_pointer, coeff_uint64_count, false, result_uint64_count, result))
                {
                    // Subtraction produced a borrow so coefficient is larger (in magnitude) than result, so need to negate result.
                    negate_uint(result, result_uint64_count, result);
                    result_is_negative = !result_is_negative;
                }
            }
        }

        // Verify result is non-negative.
        if (result_is_negative && !resultint.is_zero())
        {
            throw invalid_argument("poly must decode to a positive value");
        }
        return resultint;
    }

    void BalancedEncoder::decode_biguint(const BigPoly &poly, BigUInt &destination)
    {
        // Determine plain_modulus width.
        int plain_modulus_bits = plain_modulus_.significant_bit_count();

        int sig_uint64_count = divide_round_up(plain_modulus_bits, bits_per_uint64);
        Pointer pos_value(allocate_uint(sig_uint64_count, pool_));

        // Determine coefficient threshold for negative numbers.
        int coeff_neg_threshold_bits = coeff_neg_threshold_.significant_bit_count();

        destination.set_zero();
        int result_uint64_count = destination.uint64_count();
        int result_bit_capacity = result_uint64_count * bits_per_uint64;
        bool result_is_negative = false;
        uint64_t *result = destination.pointer();

        BigUInt base_uint(result_bit_capacity);
        BigUInt temp_result(result_bit_capacity);
        base_uint = base_;

        for (int bit_index = poly.significant_coeff_count() - 1; bit_index >= 0; --bit_index)
        {
            const BigUInt &coeff = poly[bit_index];

            // Multiply result by base, failing if highest bit set.
            if (is_bit_set_uint(result, result_uint64_count, result_bit_capacity - 1))
            {
                throw invalid_argument("poly is not a valid plaintext polynomial");
            }
            set_uint_uint(result, result_uint64_count, temp_result.pointer());
            multiply_truncate_uint_uint(temp_result.pointer(), base_uint.pointer(), result_uint64_count, result);

            // Get sign/magnitude of coefficient.
            int coeff_bit_count = coeff.significant_bit_count();
            if (coeff_bit_count > plain_modulus_bits)
            {
                // Coefficient has more bits than plain_modulus.
                throw invalid_argument("poly is not a valid plaintext polynomial");
            }
            bool coeff_is_negative = coeff_bit_count > coeff_neg_threshold_bits ||
                (coeff_bit_count == coeff_neg_threshold_bits && is_greater_than_or_equal_uint_uint(coeff.pointer(), coeff_neg_threshold_.pointer(), sig_uint64_count));
            const uint64_t *pos_pointer;
            if (coeff_is_negative)
            {
                if (sub_uint_uint(plain_modulus_.pointer(), coeff.pointer(), sig_uint64_count, pos_value.get()))
                {
                    // Check for borrow, which means value is greater than plain_modulus.
                    throw invalid_argument("poly is not a valid plaintext polynomial");
                }
                pos_pointer = pos_value.get();
                coeff_bit_count = get_significant_bit_count_uint(pos_pointer, sig_uint64_count);
            }
            else
            {
                pos_pointer = coeff.pointer();
            }

            // Verify result is at least as wide as coefficient.
            int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
            if (coeff_bit_count > result_bit_capacity)
            {
                // Coefficient has more bits than destination.
                throw invalid_argument("output out of range");
            }

            // Add or subtract-in coefficient.
            if (result_is_negative == coeff_is_negative)
            {
                // Result and coefficient have same signs so add.
                if (add_uint_uint(result, result_uint64_count, pos_pointer, coeff_uint64_count, false, result_uint64_count, result))
                {
                    // Add produced a carry that didn't fit.
                    throw invalid_argument("output out of range");
                }
            }
            else
            {
                // Result and coefficient have opposite signs so subtract.
                if (sub_uint_uint(result, result_uint64_count, pos_pointer, coeff_uint64_count, false, result_uint64_count, result))
                {
                    // Subtraction produced a borrow so coefficient is larger (in magnitude) than result, so need to negate result.
                    negate_uint(result, result_uint64_count, result);
                    result_is_negative = !result_is_negative;
                }
            }
        }

        // Verify result is non-negative.
        if (result_is_negative && !destination.is_zero())
        {
            throw invalid_argument("poly must decode to a positive value");
        }

        // Verify result fits in actual bit-width (as opposed to capacity) of destination.
        if (destination.significant_bit_count() > destination.bit_count())
        {
            throw invalid_argument("output out of range");
        }
    }

    BinaryFractionalEncoder::BinaryFractionalEncoder(const BigUInt &plain_modulus, const BigPoly &poly_modulus, int integer_coeff_count, int fraction_coeff_count) : encoder_(plain_modulus),
        fraction_coeff_count_(fraction_coeff_count), integer_coeff_count_(integer_coeff_count), poly_modulus_(poly_modulus)
    {
        if (integer_coeff_count <= 0)
        {
            throw invalid_argument("integer_coeff_count must be positive");
        }
        if (fraction_coeff_count <= 0)
        {
            throw invalid_argument("fraction_coeff_count must be positive");
        }
        if (poly_modulus_.is_zero())
        {
            throw invalid_argument("poly_modulus cannot be zero");
        }
        if (integer_coeff_count_ + fraction_coeff_count_ >= poly_modulus_.coeff_count())
        {
            throw invalid_argument("integer/fractional parts are too large for poly_modulus");
        }
        if (poly_modulus_.coeff_count() != poly_modulus_.significant_coeff_count())
        {
            poly_modulus_.resize(poly_modulus_.significant_coeff_count(), poly_modulus.coeff_bit_count());
        }
    }

    BigPoly BinaryFractionalEncoder::encode(double value)
    {
        // Take care of the integral part
        int64_t value_int = static_cast<int64_t>(value);
        BigPoly encoded_int(poly_modulus_.coeff_count(), encoder_.plain_modulus().significant_bit_count());
        encoder_.encode(value_int, encoded_int);
        value -= value_int;

        // If the fractional part is zero, return encoded_int
        if (value == 0)
        {
            return encoded_int;
        }

        int plain_uint64_count = divide_round_up(encoder_.plain_modulus().significant_bit_count(), bits_per_uint64);

        bool is_negative = value < 0;

        //Extract the fractional part
        Pointer encoded_fract(allocate_zero_poly(poly_modulus_.coeff_count(), plain_uint64_count, pool_));
        for (int i = 0; i < fraction_coeff_count_; ++i)
        {
            value *= 2;
            value_int = static_cast<int64_t>(value);
            value -= value_int;

            // We want to encode the least significant bit of value_int to the least significant bit of encoded_fract.
            // First set it to 1 if it is to be set at all. Later we will negate them all if the number was negative.
            encoded_fract[0] = static_cast<uint64_t>(value_int & 1);

            // Shift encoded_fract by one coefficient unless we are at the last coefficient
            if (i < fraction_coeff_count_ - 1)
            {
                left_shift_uint(encoded_fract.get(), plain_uint64_count * bits_per_uint64, plain_uint64_count * poly_modulus_.coeff_count(), encoded_fract.get());
            }
        }

        // We negate the coefficients only if the number was NOT negative.
        // This is because the coefficients will have to be negated in any case (sign changes at "wrapping around"
        // the polynomial modulus).
        if (!is_negative)
        {
            for (int i = 0; i < fraction_coeff_count_; ++i)
            {
                if (encoded_fract[i * plain_uint64_count] != 0)
                {
                    set_uint_uint(encoder_.neg_one_.pointer(), plain_uint64_count, encoded_fract.get() + i*plain_uint64_count);
                }
            }
        }

        // Shift the fractional part to top of polynomial
        left_shift_uint(encoded_fract.get(), plain_uint64_count * bits_per_uint64 * (poly_modulus_.coeff_count() - 1 - fraction_coeff_count_), poly_modulus_.coeff_count() * plain_uint64_count, encoded_fract.get());

        // Combine everything together
        BigPoly result(poly_modulus_.coeff_count(), encoder_.plain_modulus().significant_bit_count());
        add_poly_poly(encoded_int.pointer(), encoded_fract.get(), poly_modulus_.coeff_count(), plain_uint64_count, result.pointer());

        return result;
    }

    double BinaryFractionalEncoder::decode(const BigPoly &poly)
    {
        if (poly.significant_coeff_count() >= poly_modulus_.significant_coeff_count())
        {
            throw invalid_argument("poly is not a valid plaintext polynomial");
        }

        int coeff_uint64_count = divide_round_up(poly.coeff_bit_count(), bits_per_uint64);

        // Extract the fractional and integral parts
        BigPoly encoded_int(poly_modulus_.coeff_count() - 1 - fraction_coeff_count_, poly.coeff_bit_count());
        BigPoly encoded_fract(fraction_coeff_count_, poly.coeff_bit_count());
        set_poly_poly(poly.pointer(), integer_coeff_count_, coeff_uint64_count, encoded_int.pointer());
        set_poly_poly(poly[poly_modulus_.coeff_count() - 1 - fraction_coeff_count_].pointer(), fraction_coeff_count_, coeff_uint64_count, encoded_fract.pointer());

        // Decode integral part
        int64_t integral_part = encoder_.decode_int64(encoded_int);

        // Decode fractional part (or rather negative of it), one coefficient at a time
        double fractional_part = 0;
        BigPoly temp_int_part(1, encoded_fract.coeff_bit_count());
        for (int i = 0; i < fraction_coeff_count_; ++i)
        {
            set_uint_uint(encoded_fract[i].pointer(), coeff_uint64_count, temp_int_part.pointer());
            fractional_part += encoder_.decode_int64(temp_int_part);
            fractional_part /= 2;
        }

        return static_cast<double>(integral_part) - fractional_part;
    }

    BalancedFractionalEncoder::BalancedFractionalEncoder(const BigUInt &plain_modulus, const BigPoly &poly_modulus, int integer_coeff_count, int fraction_coeff_count, uint64_t base) : encoder_(plain_modulus, base),
        fraction_coeff_count_(fraction_coeff_count), integer_coeff_count_(integer_coeff_count), poly_modulus_(poly_modulus)
    {
        if (integer_coeff_count <= 0)
        {
            throw invalid_argument("integer_coeff_count must be positive");
        }
        if (fraction_coeff_count <= 0)
        {
            throw invalid_argument("fraction_coeff_count must be positive");
        }
        if (poly_modulus_.is_zero())
        {
            throw invalid_argument("poly_modulus cannot be zero");
        }
        if (integer_coeff_count_ + fraction_coeff_count_ >= poly_modulus_.coeff_count())
        {
            throw invalid_argument("integer/fractional parts are too large for poly_modulus");
        }
        if (poly_modulus_.coeff_count() != poly_modulus_.significant_coeff_count())
        {
            poly_modulus_.resize(poly_modulus_.significant_coeff_count(), poly_modulus.coeff_bit_count());
        }
    }

    BigPoly BalancedFractionalEncoder::encode(double value)
    {
        // Take care of the integral part
        int64_t value_int = static_cast<int64_t>(round(value));
        BigPoly encoded_int(poly_modulus_.coeff_count(), encoder_.plain_modulus().significant_bit_count());
        encoder_.encode(value_int, encoded_int);
        value -= value_int;
        
        // If the fractional part is zero, return encoded_int
        if (value == 0)
        {
            return encoded_int;
        }

        int plain_uint64_count = divide_round_up(encoder_.plain_modulus().significant_bit_count(), bits_per_uint64);

        // Extract the fractional part
        Pointer encoded_fract(allocate_zero_poly(poly_modulus_.coeff_count(), plain_uint64_count, pool_));
        for (int i = 0; i < fraction_coeff_count_; ++i)
        {
            value *= encoder_.base();

            // When computing the next value_int we need to round e.g. 0.5 to 0 (not to 1) and
            // -0.5 to 0 (not to -1), i.e. always towards zero.
            int sign = (value >= 0 ? 1 : -1);
            value_int = static_cast<int64_t>(sign * ceil(abs(value) - 0.5));
            value -= value_int;

            // We store the representative of value_int modulo the base (symmetric representative)
            // as the absolute value (in value_int_mod_base) and as the sign (in is_negative).
            bool is_negative = false;
            // A slightly easier way to do the following would be:
            // int64_t value_int_mod_base = value_int % static_cast<int64_t>(encoder_.base_);
            // but then we need the cast work as expected so the base can't be 64 bits.
            // Warning: The sign of % was undefined behavior until C++11 for negative integers.
            int64_t value_int_mod_base = value_int - (value_int < 0 ? -1 : 1) * (abs(value_int) / encoder_.base_) * encoder_.base_;
            if (value_int_mod_base < 0)
            {
                is_negative = true;
                value_int_mod_base = -value_int_mod_base;
            }
            
            // Set the constant coefficient of encoded_fract to be the correct absolute value.
            encoded_fract[0] = value_int_mod_base;
            // And negate it modulo plain_modulus if it was NOT supposed to be negative, because the
            // fractional encoding requires the signs of the fractional coefficients to be negatives of
            // what one might naively expect, as they change sign when "wrapping around" the polynomial modulus.
            if (!is_negative && value_int_mod_base != 0)
            {
                sub_uint_uint(encoder_.plain_modulus_.pointer(), encoded_fract.get(), plain_uint64_count, encoded_fract.get());
            }

            // Shift encoded_fract by one coefficient unless we are at the last coefficient
            if (i < fraction_coeff_count_ - 1)
            {
                left_shift_uint(encoded_fract.get(), plain_uint64_count * bits_per_uint64, plain_uint64_count * poly_modulus_.coeff_count(), encoded_fract.get());
            }
        }

        // Shift the fractional part to top of polynomial
        left_shift_uint(encoded_fract.get(), plain_uint64_count * bits_per_uint64 * (poly_modulus_.coeff_count() - 1 - fraction_coeff_count_), poly_modulus_.coeff_count() * plain_uint64_count, encoded_fract.get());

        // Combine everything together
        BigPoly result(poly_modulus_.coeff_count(), encoder_.plain_modulus().significant_bit_count());
        add_poly_poly(encoded_int.pointer(), encoded_fract.get(), poly_modulus_.coeff_count(), plain_uint64_count, result.pointer());

        return result;
    }

    double BalancedFractionalEncoder::decode(const BigPoly &poly)
    {
        if (poly.significant_coeff_count() >= poly_modulus_.significant_coeff_count())
        {
            throw invalid_argument("poly is not a valid plaintext polynomial");
        }

        int coeff_uint64_count = divide_round_up(poly.coeff_bit_count(), bits_per_uint64);

        // Extract the fractional and integral parts
        BigPoly encoded_int(poly_modulus_.coeff_count() - 1 - fraction_coeff_count_, poly.coeff_bit_count());
        BigPoly encoded_fract(fraction_coeff_count_, poly.coeff_bit_count());
        set_poly_poly(poly.pointer(), integer_coeff_count_, coeff_uint64_count, encoded_int.pointer());
        set_poly_poly(poly[poly_modulus_.coeff_count() - 1 - fraction_coeff_count_].pointer(), fraction_coeff_count_, coeff_uint64_count, encoded_fract.pointer());

        // Decode integral part
        int64_t integral_part = encoder_.decode_int64(encoded_int);

        // Decode fractional part (or rather negative of it), one coefficient at a time
        double fractional_part = 0;
        BigPoly temp_int_part(1, encoded_fract.coeff_bit_count());
        for (int i = 0; i < fraction_coeff_count_; ++i)
        {
            set_uint_uint(encoded_fract[i].pointer(), coeff_uint64_count, temp_int_part.pointer());
            fractional_part += encoder_.decode_int64(temp_int_part);
            fractional_part /= encoder_.base();
        }

        return static_cast<double>(integral_part) - fractional_part;
    }
}
