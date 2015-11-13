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
    BinaryEncoder::BinaryEncoder(BigUInt plain_modulus) : plain_modulus_(plain_modulus)
    {
        if (plain_modulus.significant_bit_count() <= 1)
        {
            throw invalid_argument("plain_modulus");
        }
        coeff_neg_threshold_.resize(plain_modulus_.bit_count());
        half_round_up_uint(plain_modulus_.pointer(), plain_modulus_.uint64_count(), coeff_neg_threshold_.pointer());
        neg_one_.resize(plain_modulus_.bit_count());
        decrement_uint(plain_modulus_.pointer(), plain_modulus_.uint64_count(), neg_one_.pointer());
    }

    void BinaryEncoder::set_plain_modulus(BigUInt &value)
    {
        if (value.significant_bit_count() <= 1)
        {
            throw invalid_argument("value");
        }
        plain_modulus_.resize(value.bit_count());
        plain_modulus_ = value;
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

    BigPoly BinaryEncoder::encode(BigUInt value)
    {
        BigPoly result;
        encode(value, result);
        return result;
    }

    void BinaryEncoder::encode(BigUInt value, BigPoly &destination)
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
            throw out_of_range("poly");
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
            throw out_of_range("poly");
        }
        return bit_count > 0 ? bigvalue.pointer()[0] : 0;
    }

    int32_t BinaryEncoder::decode_int32(const BigPoly &poly)
    {
        int64_t value64 = decode_int64(poly);
        if (value64 < INT32_MIN || value64 > INT32_MAX)
        {
            throw out_of_range("poly");
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
                throw out_of_range("poly");
            }

            // Get sign/magnitude of coefficient.
            int coeff_bit_count = coeff.significant_bit_count();
            if (coeff_bit_count > plain_modulus_bits)
            {
                // Coefficient has more bits than plain_modulus.
                throw out_of_range("poly");
            }
            bool coeff_is_negative = coeff_bit_count > coeff_neg_threshold_bits ||
                (coeff_bit_count == coeff_neg_threshold_bits && is_greater_than_or_equal_uint_uint(coeff.pointer(), coeff_neg_threshold_.pointer(), sig_uint64_count));
            const uint64_t *pos_pointer;
            if (coeff_is_negative)
            {
                if (sub_uint_uint(plain_modulus_.pointer(), coeff.pointer(), sig_uint64_count, pos_value.get()))
                {
                    // Check for borrow, which means value is greater than plain_modulus.
                    throw out_of_range("poly");
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
                throw out_of_range("poly");
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
                throw out_of_range("poly");
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
                throw out_of_range("poly");
            }
            bool coeff_is_negative = coeff_bit_count > coeff_neg_threshold_bits ||
                (coeff_bit_count == coeff_neg_threshold_bits && is_greater_than_or_equal_uint_uint(coeff.pointer(), coeff_neg_threshold_.pointer(), sig_uint64_count));
            const uint64_t *pos_pointer;
            if (coeff_is_negative)
            {
                if (sub_uint_uint(plain_modulus_.pointer(), coeff.pointer(), sig_uint64_count, pos_value.get()))
                {
                    // Check for borrow, which means value is greater than plain_modulus.
                    throw out_of_range("poly");
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
            throw out_of_range("poly");
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
                throw out_of_range("poly");
            }
            left_shift_uint(result, 1, result_uint64_count, result);

            // Get sign/magnitude of coefficient.
            int coeff_bit_count = coeff.significant_bit_count();
            if (coeff_bit_count > plain_modulus_bits)
            {
                // Coefficient has more bits than plain_modulus.
                throw out_of_range("poly");
            }
            bool coeff_is_negative = coeff_bit_count > coeff_neg_threshold_bits ||
                (coeff_bit_count == coeff_neg_threshold_bits && is_greater_than_or_equal_uint_uint(coeff.pointer(), coeff_neg_threshold_.pointer(), sig_uint64_count));
            const uint64_t *pos_pointer;
            if (coeff_is_negative)
            {
                if (sub_uint_uint(plain_modulus_.pointer(), coeff.pointer(), sig_uint64_count, pos_value.get()))
                {
                    // Check for borrow, which means value is greater than plain_modulus.
                    throw out_of_range("poly");
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
                throw out_of_range("poly");
            }

            // Add or subtract-in coefficient.
            if (result_is_negative == coeff_is_negative)
            {
                // Result and coefficient have same signs so add.
                if (add_uint_uint(result, result_uint64_count, pos_pointer, coeff_uint64_count, false, result_uint64_count, result))
                {
                    // Add produced a carry that didn't fit.
                    throw out_of_range("poly");
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
            throw out_of_range("poly");
        }

        // Verify result fits in actual bit-width (as opposed to capacity) of destination.
        if (destination.significant_bit_count() > destination.bit_count())
        {
            throw out_of_range("poly");
        }
    }

    BalancedEncoder::BalancedEncoder(BigUInt plain_modulus, int base) : plain_modulus_(plain_modulus), base_(base)
    {
        if (plain_modulus.significant_bit_count() <= 1)
        {
            throw invalid_argument("plain_modulus must have significant bit count bigger than 1");
        }
        if (base <= 1 || base % 2 == 0)
        {
            throw invalid_argument("base must be an odd integer and at least 3");
        }
        coeff_neg_threshold_.resize(plain_modulus_.bit_count());
        half_round_up_uint(plain_modulus_.pointer(), plain_modulus_.uint64_count(), coeff_neg_threshold_.pointer());
    }

    void BalancedEncoder::set_plain_modulus(BigUInt &value)
    {
        if (value.significant_bit_count() <= 1)
        {
            throw invalid_argument("value");
        }
        plain_modulus_.resize(value.bit_count());
        plain_modulus_ = value;
        coeff_neg_threshold_.resize(plain_modulus_.bit_count());
        half_round_up_uint(plain_modulus_.pointer(), plain_modulus_.uint64_count(), coeff_neg_threshold_.pointer());
    }

    void BalancedEncoder::set_base(int base)
    {
        if (base <= 1 || base % 2 == 0)
        {
            throw invalid_argument("base must be an odd integer at least 3");
        }
        base_ = base;
    }

    BigPoly BalancedEncoder::encode(uint64_t value)
    {
        BigPoly result;
        encode(value, result);
        return result;
    }

    void BalancedEncoder::encode(uint64_t value, BigPoly &destination)
    {
        // We are using unnecessarily many coefficients here, but works for all bases.
        int encode_coeff_count = get_significant_bit_count(value);
        if (destination.coeff_count() < encode_coeff_count || destination.coeff_bit_count() < plain_modulus_.significant_bit_count())
        {
            destination.resize(encode_coeff_count, plain_modulus_.significant_bit_count());
        }
        destination.set_zero();

        int coeff_index = 0;
        while (value != 0)
        {
            int remainder = value % base_;
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

        // Resize destination
        int dest_number_of_coeffs = destination.significant_coeff_count();
        destination.resize(dest_number_of_coeffs, plain_modulus_.significant_bit_count());
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

            // We are using unnecessarily many coefficients here.
            int encode_coeff_count = get_significant_bit_count(value);
            if (destination.coeff_count() < encode_coeff_count || destination.coeff_bit_count() < plain_modulus_.significant_bit_count())
            {
                destination.resize(encode_coeff_count, plain_modulus_.significant_bit_count());
            }
            destination.set_zero();

            int coeff_index = 0;
            while (pos_value != 0)
            {
                int remainder = pos_value % base_;
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

            // Resize destination
            int dest_number_of_coeffs = destination.significant_coeff_count();
            destination.resize(dest_number_of_coeffs, plain_modulus_.significant_bit_count());
        }
        else
        {
            encode(static_cast<uint64_t>(value), destination);
        }
    }

    BigPoly BalancedEncoder::encode(BigUInt value)
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

        int encode_coeff_count = value.significant_bit_count();
        int encode_uint64_count = divide_round_up(encode_coeff_count, bits_per_uint64);

        if (destination.coeff_count() < encode_coeff_count || destination.coeff_bit_count() < plain_modulus_.significant_bit_count())
        {
            destination.resize(encode_coeff_count, plain_modulus_.significant_bit_count());
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

        // Resize destination
        int dest_number_of_coeffs = destination.significant_coeff_count();
        destination.resize(dest_number_of_coeffs, plain_modulus_.significant_bit_count());
    }

    uint32_t BalancedEncoder::decode_uint32(const BigPoly &poly)
    {
        uint64_t value64 = decode_uint64(poly);
        if (value64 > UINT32_MAX)
        {
            throw out_of_range("poly");
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
            throw out_of_range("poly");
        }
        return bit_count > 0 ? bigvalue.pointer()[0] : 0;
    }

    int32_t BalancedEncoder::decode_int32(const BigPoly &poly)
    {
        int64_t value64 = decode_int64(poly);
        if (value64 < INT32_MIN || value64 > INT32_MAX)
        {
            throw out_of_range("poly");
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
                throw out_of_range("poly");
            }

            // Get sign/magnitude of coefficient.
            int coeff_bit_count = coeff.significant_bit_count();
            if (coeff_bit_count > plain_modulus_bits)
            {
                // Coefficient has more bits than plain_modulus.
                throw out_of_range("poly");
            }
            bool coeff_is_negative = coeff_bit_count > coeff_neg_threshold_bits ||
                (coeff_bit_count == coeff_neg_threshold_bits && is_greater_than_or_equal_uint_uint(coeff.pointer(), coeff_neg_threshold_.pointer(), sig_uint64_count));
            const uint64_t *pos_pointer;
            if (coeff_is_negative)
            {
                if (sub_uint_uint(plain_modulus_.pointer(), coeff.pointer(), sig_uint64_count, pos_value.get()))
                {
                    // Check for borrow, which means value is greater than plain_modulus.
                    throw out_of_range("poly");
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
                throw out_of_range("poly");
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
                throw out_of_range("poly");
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
                throw out_of_range("poly");
            }
            bool coeff_is_negative = coeff_bit_count > coeff_neg_threshold_bits ||
                (coeff_bit_count == coeff_neg_threshold_bits && is_greater_than_or_equal_uint_uint(coeff.pointer(), coeff_neg_threshold_.pointer(), sig_uint64_count));
            const uint64_t *pos_pointer;
            if (coeff_is_negative)
            {
                if (sub_uint_uint(plain_modulus_.pointer(), coeff.pointer(), sig_uint64_count, pos_value.get()))
                {
                    // Check for borrow, which means value is greater than plain_modulus.
                    throw out_of_range("poly");
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
            throw out_of_range("poly");
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
                throw out_of_range("poly");
            }
            set_uint_uint(result, result_uint64_count, temp_result.pointer());
            multiply_truncate_uint_uint(temp_result.pointer(), base_uint.pointer(), result_uint64_count, result);

            // Get sign/magnitude of coefficient.
            int coeff_bit_count = coeff.significant_bit_count();
            if (coeff_bit_count > plain_modulus_bits)
            {
                // Coefficient has more bits than plain_modulus.
                throw out_of_range("poly");
            }
            bool coeff_is_negative = coeff_bit_count > coeff_neg_threshold_bits ||
                (coeff_bit_count == coeff_neg_threshold_bits && is_greater_than_or_equal_uint_uint(coeff.pointer(), coeff_neg_threshold_.pointer(), sig_uint64_count));
            const uint64_t *pos_pointer;
            if (coeff_is_negative)
            {
                if (sub_uint_uint(plain_modulus_.pointer(), coeff.pointer(), sig_uint64_count, pos_value.get()))
                {
                    // Check for borrow, which means value is greater than plain_modulus.
                    throw out_of_range("poly");
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
                throw out_of_range("poly");
            }

            // Add or subtract-in coefficient.
            if (result_is_negative == coeff_is_negative)
            {
                // Result and coefficient have same signs so add.
                if (add_uint_uint(result, result_uint64_count, pos_pointer, coeff_uint64_count, false, result_uint64_count, result))
                {
                    // Add produced a carry that didn't fit.
                    throw out_of_range("poly");
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
            throw out_of_range("poly");
        }

        // Verify result fits in actual bit-width (as opposed to capacity) of destination.
        if (destination.significant_bit_count() > destination.bit_count())
        {
            throw out_of_range("poly");
        }
    }

    BinaryFractionalEncoder::BinaryFractionalEncoder(const BigUInt &plain_modulus, const BigPoly &poly_modulus, int integer_coeff_count, int fraction_coeff_count) : encoder_(plain_modulus),
        fraction_coeff_count_(fraction_coeff_count), integer_coeff_count_(integer_coeff_count), poly_modulus_(poly_modulus)
    {
        if (integer_coeff_count <= 0)
        {
            throw out_of_range("integer_coeff_count");
        }
        if (fraction_coeff_count <= 0)
        {
            throw out_of_range("fraction_coeff_count");
        }
        if (poly_modulus_.is_zero())
        {
            throw invalid_argument("poly_modulus");
        }
        if (integer_coeff_count_ + fraction_coeff_count_ >= poly_modulus_.coeff_count())
        {
            throw invalid_argument("coeff_count");
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
        BigPoly encoded_int = encoder_.encode(value_int);
        encoded_int.resize(poly_modulus_.coeff_count(), encoder_.plain_modulus().bit_count());
        value -= value_int;

        int coeff_uint64_count = divide_round_up(encoded_int.coeff_bit_count(), bits_per_uint64);

        // Extract the fractional part
        BigPoly encoded_fract(poly_modulus_.coeff_count(), encoder_.plain_modulus().bit_count());
        encoded_fract.set_zero();

        for (int i = 0; i < fraction_coeff_count_; ++i)
        {
            value *= 2;
            value_int = static_cast<int64_t>(value);
            value -= value_int;

            // This is just one symbol. Write it as the constant coefficient of encoded_fract.
            BigPoly temp_int_part = encoder_.encode(value_int);
            if (!temp_int_part.is_zero())
            {
                set_uint_uint(temp_int_part[0].pointer(), temp_int_part[0].uint64_count(), encoded_fract[0].pointer());
            }

            // Shift encoded_fract by one coefficient unless we are at the last coefficient
            if (i < fraction_coeff_count_ - 1)
            {
                left_shift_uint(encoded_fract.pointer(), coeff_uint64_count * bits_per_uint64, encoded_fract.uint64_count(), encoded_fract.pointer());
            }
        }

        // Shift the fractional part to top of polynomial and negate
        left_shift_uint(encoded_fract.pointer(), coeff_uint64_count * bits_per_uint64 * (poly_modulus_.coeff_count() - 1 - fraction_coeff_count_), encoded_fract.coeff_count() * coeff_uint64_count, encoded_fract.pointer());
        negate_poly_coeffmod(encoded_fract.pointer(), encoded_fract.coeff_count(), encoder_.plain_modulus().pointer(), encoder_.plain_modulus().uint64_count(), encoded_fract.pointer());

        // Combine everything together
        BigPoly result(poly_modulus_.coeff_count(), encoder_.plain_modulus().bit_count());
        add_poly_poly(encoded_int.pointer(), encoded_fract.pointer(), result.coeff_count(), coeff_uint64_count, result.pointer());

        return result;
    }

    double BinaryFractionalEncoder::decode(const BigPoly &value)
    {
        if (value.significant_coeff_count() >= poly_modulus_.significant_coeff_count())
        {
            throw invalid_argument("value");
        }

        int coeff_uint64_count = divide_round_up(value.coeff_bit_count(), bits_per_uint64);

        // Extract the fractional and integral parts
        BigPoly encoded_int(poly_modulus_.coeff_count() - 1 - fraction_coeff_count_, value.coeff_bit_count());
        BigPoly encoded_fract(fraction_coeff_count_, value.coeff_bit_count());
        set_poly_poly(value.pointer(), integer_coeff_count_, coeff_uint64_count, encoded_int.pointer());
        set_poly_poly(value[poly_modulus_.coeff_count() - 1 - fraction_coeff_count_].pointer(), fraction_coeff_count_, coeff_uint64_count, encoded_fract.pointer());

        // Decode integral part
        int64_t integral_part = encoder_.decode_int64(encoded_int);

        // Decode fractional part (or rather negative of it), one coefficient at a time
        double fractional_part = 0;
        for (int i = 0; i < fraction_coeff_count_; ++i)
        {
            BigPoly temp_int_part(1, encoded_fract.coeff_bit_count());
            set_uint_uint(encoded_fract[i].pointer(), coeff_uint64_count, temp_int_part.pointer());
            fractional_part += encoder_.decode_int64(temp_int_part);
            fractional_part /= 2;
        }

        return static_cast<double>(integral_part) - fractional_part;
    }

    BalancedFractionalEncoder::BalancedFractionalEncoder(const BigUInt &plain_modulus, const BigPoly &poly_modulus, int integer_coeff_count, int fraction_coeff_count, int base) : encoder_(plain_modulus, base),
        fraction_coeff_count_(fraction_coeff_count), integer_coeff_count_(integer_coeff_count), poly_modulus_(poly_modulus)
    {
        if (integer_coeff_count <= 0)
        {
            throw out_of_range("integer_coeff_count");
        }
        if (fraction_coeff_count <= 0)
        {
            throw out_of_range("fraction_coeff_count");
        }
        if (poly_modulus_.is_zero())
        {
            throw invalid_argument("poly_modulus");
        }
        if (integer_coeff_count_ + fraction_coeff_count_ >= poly_modulus_.coeff_count())
        {
            throw invalid_argument("coeff_count");
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
        BigPoly encoded_int = encoder_.encode(value_int);
        encoded_int.resize(poly_modulus_.coeff_count(), encoder_.plain_modulus().bit_count());
        value -= value_int;

        int coeff_uint64_count = divide_round_up(encoded_int.coeff_bit_count(), bits_per_uint64);

        // Extract the fractional part
        BigPoly encoded_fract(poly_modulus_.coeff_count(), encoder_.plain_modulus().bit_count());
        encoded_fract.set_zero();

        for (int i = 0; i < fraction_coeff_count_; ++i)
        {
            value *= encoder_.base();
            // We need to round e.g. 0.5 to 0 (not to 1) and -0.5 to 0 (not to -1).
            int sign = (value >= 0 ? 1 : -1);
            value_int = static_cast<int64_t>(sign * ceil(abs(value) - 0.5));
            value -= value_int;

            // This is just one symbol. Write it as the constant coefficient of encoded_fract.
            BigPoly temp_int_part = encoder_.encode(value_int);
            if (!temp_int_part.is_zero())
            {
                set_uint_uint(temp_int_part[0].pointer(), temp_int_part[0].uint64_count(), encoded_fract[0].pointer());
            }

            // Shift encoded_fract by one coefficient unless we are at the last coefficient
            if (i < fraction_coeff_count_ - 1)
            {
                left_shift_uint(encoded_fract.pointer(), coeff_uint64_count * bits_per_uint64, encoded_fract.uint64_count(), encoded_fract.pointer());
            }
        }

        // Shift the fractional part to top of polynomial and negate
        left_shift_uint(encoded_fract.pointer(), coeff_uint64_count * bits_per_uint64 * (poly_modulus_.coeff_count() - 1 - fraction_coeff_count_), encoded_fract.coeff_count() * coeff_uint64_count, encoded_fract.pointer());
        negate_poly_coeffmod(encoded_fract.pointer(), encoded_fract.coeff_count(), encoder_.plain_modulus().pointer(), encoder_.plain_modulus().uint64_count(), encoded_fract.pointer());

        // Combine everything together
        BigPoly result(poly_modulus_.coeff_count(), encoder_.plain_modulus().bit_count());
        add_poly_poly(encoded_int.pointer(), encoded_fract.pointer(), result.coeff_count(), coeff_uint64_count, result.pointer());

        return result;
    }

    double BalancedFractionalEncoder::decode(const BigPoly &value)
    {
        if (value.significant_coeff_count() >= poly_modulus_.significant_coeff_count())
        {
            throw invalid_argument("value");
        }

        int coeff_uint64_count = divide_round_up(value.coeff_bit_count(), bits_per_uint64);

        // Extract the fractional and integral parts
        BigPoly encoded_int(poly_modulus_.coeff_count() - 1 - fraction_coeff_count_, value.coeff_bit_count());
        BigPoly encoded_fract(fraction_coeff_count_, value.coeff_bit_count());
        set_poly_poly(value.pointer(), integer_coeff_count_, coeff_uint64_count, encoded_int.pointer());
        set_poly_poly(value[poly_modulus_.coeff_count() - 1 - fraction_coeff_count_].pointer(), fraction_coeff_count_, coeff_uint64_count, encoded_fract.pointer());

        // Decode integral part
        int64_t integral_part = encoder_.decode_int64(encoded_int);

        // Decode fractional part (or rather negative of it), one coefficient at a time
        double fractional_part = 0;
        for (int i = 0; i < fraction_coeff_count_; ++i)
        {
            BigPoly temp_int_part(1, encoded_fract.coeff_bit_count());
            set_uint_uint(encoded_fract[i].pointer(), coeff_uint64_count, temp_int_part.pointer());
            fractional_part += encoder_.decode_int64(temp_int_part);
            fractional_part /= encoder_.base();
        }

        return static_cast<double>(integral_part) - fractional_part;
    }
}
