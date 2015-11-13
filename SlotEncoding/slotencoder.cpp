#include "slotencoder.h"
#include "internal\uintcore.h"
#include "internal\uintarith.h"
#include <algorithm>

using namespace std;
using namespace fhelib::internal;

namespace fhelib
{
	namespace slot_encoding
	{
		void SlotEncoder::set_base(int value)
		{
			balanced_encoder_.set_base(value);
		}

		void SlotEncoder::set_plain_modulus(BigUInt &value)
		{
			balanced_encoder_.set_plain_modulus(value);
		}

		void SlotEncoder::set_slot_size(int value)
		{
			if (value <= 0)
			{
				throw out_of_range("value");
			}
			slot_size_ = value;
		}

		SlotBigPoly SlotEncoder::encode(uint64_t value)
		{
			SlotBigPoly result(slot_size_);
			encode(value, result);
			return result;
		}

		void SlotEncoder::encode(uint64_t value, SlotBigPoly &destination)
		{
			if (destination.slot_size() != slot_size_)
			{
				throw invalid_argument("slot_size mismatch");
			}

			// We are using unnecessarily many coefficients here
			int encode_coeff_count = slot_size_ * get_significant_bit_count(value);
			if (destination.coeff_count() < encode_coeff_count || destination.coeff_bit_count() < balanced_encoder_.plain_modulus().bit_count())
			{
				destination.poly().resize(max(destination.coeff_count(), encode_coeff_count), max(destination.coeff_bit_count(), balanced_encoder_.plain_modulus().bit_count()));
			}
			destination.set_zero();

			int coeff_index = 0;
			while (value != 0)
			{
				int remainder = value % balanced_encoder_.base();
				if (0 < remainder && remainder <= balanced_encoder_.base() / 2)
				{
					destination[coeff_index] = remainder;
				}
				else if (remainder > balanced_encoder_.base() / 2)
				{
					destination[coeff_index] = balanced_encoder_.base() - remainder;
					sub_uint_uint(balanced_encoder_.plain_modulus().pointer(), destination[coeff_index].pointer(), balanced_encoder_.plain_modulus().uint64_count(), destination[coeff_index].pointer());
				}

				value = (value + balanced_encoder_.base() / 2) / balanced_encoder_.base();
				coeff_index += slot_size_;
			}

			// Make sure the number of terms is the smallest possible multiple of slot_size, and set shift to 0.
			destination.set_shift(0);
		}

		SlotBigPoly SlotEncoder::encode(int64_t value)
		{
			SlotBigPoly result(slot_size_);
			encode(value, result);
			return result;
		}

		void SlotEncoder::encode(int64_t value, SlotBigPoly &destination)
		{
			if (destination.slot_size() != slot_size_)
			{
				throw invalid_argument("slot_size mismatch");
			}

			if (value < 0)
			{
				uint64_t pos_value = static_cast<uint64_t>(-value);

				// We are using unnecessarily many coefficients here
				int encode_coeff_count = slot_size_ * get_significant_bit_count(pos_value);
				if (destination.coeff_count() < encode_coeff_count || destination.coeff_bit_count() < balanced_encoder_.plain_modulus().bit_count())
				{
					destination.poly().resize(max(destination.coeff_count(), encode_coeff_count), max(destination.coeff_bit_count(), balanced_encoder_.plain_modulus().bit_count()));
				}
				destination.set_zero();

				int coeff_index = 0;
				while (pos_value != 0)
				{
					int remainder = pos_value % balanced_encoder_.base();
					if (0 < remainder && remainder <= balanced_encoder_.base() / 2)
					{
						destination[coeff_index] = remainder;
						sub_uint_uint(balanced_encoder_.plain_modulus().pointer(), destination[coeff_index].pointer(), balanced_encoder_.plain_modulus().uint64_count(), destination[coeff_index].pointer());
					}
					else if (remainder > balanced_encoder_.base() / 2)
					{
						destination[coeff_index] = balanced_encoder_.base() - remainder;
					}

					pos_value = (pos_value + (balanced_encoder_.base() / 2)) / balanced_encoder_.base();

					coeff_index += slot_size_;
				}

				// Make sure the number of terms is the smallest possible multiple of slot_size.
				destination.set_shift(0);
			}
			else
			{
				encode(static_cast<uint64_t>(value), destination);
			}
		}

		SlotBigPoly SlotEncoder::encode(BigUInt value)
		{
			SlotBigPoly result(slot_size_);
			encode(value, result);
			return result;
		}

		void SlotEncoder::encode(BigUInt value, SlotBigPoly &destination)
		{
			if (destination.slot_size() != slot_size_)
			{
				throw invalid_argument("slot_size mismatch");
			}

			int encode_coeff_count = slot_size_ * value.significant_bit_count();
			int encode_uint64_count = divide_round_up(encode_coeff_count, bits_per_uint64);
			if (destination.coeff_count() < encode_coeff_count || destination.coeff_bit_count() < balanced_encoder_.plain_modulus().bit_count())
			{
				destination.poly().resize(max(destination.coeff_count(), encode_coeff_count), max(destination.coeff_bit_count(), balanced_encoder_.plain_modulus().bit_count()));
			}
			destination.set_zero();

			BigUInt base_uint(encode_coeff_count);
			base_uint = balanced_encoder_.base();
			BigUInt base_div_two_uint(encode_coeff_count);
			right_shift_uint(base_uint.pointer(), 1, encode_uint64_count, base_div_two_uint.pointer());

			BigUInt quotient(encode_coeff_count);
			BigUInt remainder(encode_coeff_count);

			int coeff_index = 0;
			while (!value.is_zero())
			{
				divide_uint_uint(value.pointer(), base_uint.pointer(), encode_uint64_count, quotient.pointer(), remainder.pointer(), pool_);
				if (!remainder.is_zero() && is_less_than_or_equal_uint_uint(remainder.pointer(), base_div_two_uint.pointer(), encode_uint64_count))
				{
					destination[coeff_index] = remainder;
					sub_uint_uint(balanced_encoder_.plain_modulus().pointer(), destination[coeff_index].pointer(), balanced_encoder_.plain_modulus().uint64_count(), destination[coeff_index].pointer());
				}
				else if (is_greater_than_uint_uint(remainder.pointer(), base_div_two_uint.pointer(), encode_uint64_count))
				{
					sub_uint_uint(base_uint.pointer(), remainder.pointer(), encode_uint64_count, destination[coeff_index].pointer());
				}
				add_uint_uint(value.pointer(), base_div_two_uint.pointer(), encode_uint64_count, value.pointer());
				divide_uint_uint(value.pointer(), base_uint.pointer(), encode_uint64_count, quotient.pointer(), remainder.pointer(), pool_);
				set_uint_uint(quotient.pointer(), encode_uint64_count, value.pointer()); 

				coeff_index += slot_size_;
			}

			// Resize destination
			destination.set_shift(0);
		}

		uint32_t SlotEncoder::decode_uint32(const SlotBigPoly &poly)
		{
			uint64_t value64 = decode_uint64(poly);
			if (value64 > UINT32_MAX)
			{
				throw out_of_range("poly");
			}
			return static_cast<uint32_t>(value64);
		}

		uint64_t SlotEncoder::decode_uint64(const SlotBigPoly &poly)
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

		int32_t SlotEncoder::decode_int32(const SlotBigPoly &poly)
		{
			int64_t value64 = decode_int64(poly);
			if (value64 < INT32_MIN || value64 > INT32_MAX)
			{
				throw out_of_range("poly");
			}
			return static_cast<int32_t>(value64);
		}

		int64_t SlotEncoder::decode_int64(const SlotBigPoly &poly)
		{
			if (poly.slot_size() != slot_size_)
			{
				throw invalid_argument("slot_size mismatch");
			}

			// Determine plain_modulus width.
			int plain_modulus_bits = balanced_encoder_.plain_modulus().significant_bit_count();
			if (plain_modulus_bits == 0)
			{
				throw invalid_argument("plain_modulus");
			}
			int sig_uint64_count = divide_round_up(plain_modulus_bits, bits_per_uint64);
			Pointer pos_value(allocate_uint(sig_uint64_count, pool_));

			// Construct threshold as same size.
			Pointer coeff_neg_threshold(allocate_uint(sig_uint64_count, pool_));

			// Set to value of (plain_modulus + 1) / 2
			half_round_up_uint(balanced_encoder_.plain_modulus().pointer(), sig_uint64_count, coeff_neg_threshold.get());
			int coeff_neg_threshold_bits = get_significant_bit_count_uint(coeff_neg_threshold.get(), sig_uint64_count);

			int64_t result = 0;
			for (int bit_index = poly.significant_coeff_count() - 1; bit_index >= 0; --bit_index)
			{
				const BigUInt &coeff = poly[bit_index];

				// Multiply result by base if starting next slot.
				int64_t next_result = result;
				if (bit_index % slot_size_ == slot_size_ - 1)
				{
					next_result *= balanced_encoder_.base();

					if ((next_result < 0) != (result < 0))
					{
						// Check for overflow.
						throw out_of_range("poly");
					}
				}

				// Get sign/magnitude of coefficient.
				int coeff_bit_count = coeff.significant_bit_count();
				if (coeff_bit_count > plain_modulus_bits)
				{
					// Coefficient has more bits than plain_modulus.
					throw out_of_range("poly");
				}
				bool coeff_is_negative = coeff_bit_count > coeff_neg_threshold_bits ||
					(coeff_bit_count == coeff_neg_threshold_bits && is_greater_than_or_equal_uint_uint(coeff.pointer(), coeff_neg_threshold.get(), sig_uint64_count));
				const uint64_t *pos_pointer;
				if (coeff_is_negative)
				{
					if (sub_uint_uint(balanced_encoder_.plain_modulus().pointer(), coeff.pointer(), sig_uint64_count, pos_value.get()))
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

		BigUInt SlotEncoder::decode_biguint(const SlotBigPoly &poly)
		{
			if (poly.slot_size() != slot_size_)
			{
				throw invalid_argument("slot_size mismatch");
			}

			// Determine plain_modulus width.
			int plain_modulus_bits = balanced_encoder_.plain_modulus().significant_bit_count();
			if (plain_modulus_bits == 0)
			{
				throw invalid_argument("plain_modulus");
			}
			int sig_uint64_count = divide_round_up(plain_modulus_bits, bits_per_uint64);
			Pointer pos_value(allocate_uint(sig_uint64_count, pool_));

			// Construct threshold as same size.
			Pointer coeff_neg_threshold(allocate_uint(sig_uint64_count, pool_));

			// Set to value of (plain_modulus + 1) / 2
			half_round_up_uint(balanced_encoder_.plain_modulus().pointer(), sig_uint64_count, coeff_neg_threshold.get());
			int coeff_neg_threshold_bits = get_significant_bit_count_uint(coeff_neg_threshold.get(), sig_uint64_count);

			int result_uint64_count = 1;
			int result_bit_capacity = result_uint64_count * bits_per_uint64;
			BigUInt resultint(result_bit_capacity);
			bool result_is_negative = false;
			uint64_t *result = resultint.pointer();
			for (int bit_index = poly.significant_coeff_count() - 1; bit_index >= 0; --bit_index)
			{
				const BigUInt &coeff = poly[bit_index];

				// Multiply result by base if starting next slot. Resize if highest bit set.
				if (is_bit_set_uint(result, result_uint64_count, result_bit_capacity - 1))
				{
					// Resize to make bigger.
					result_uint64_count++;
					result_bit_capacity = result_uint64_count * bits_per_uint64;
					resultint.resize(result_bit_capacity);
					result = resultint.pointer();
				}

				if (bit_index % slot_size_ == slot_size_ - 1)
				{
					BigUInt base_biguint(result_bit_capacity);
					BigUInt temp_result(result_bit_capacity);
					base_biguint = balanced_encoder_.base();
					set_uint_uint(result, result_uint64_count, temp_result.pointer());
					multiply_uint_uint(temp_result.pointer(), base_biguint.pointer(), result_uint64_count, result);
				}

				// Get sign/magnitude of coefficient.
				int coeff_bit_count = coeff.significant_bit_count();
				if (coeff_bit_count > plain_modulus_bits)
				{
					// Coefficient has more bits than plain_modulus.
					throw out_of_range("poly");
				}
				bool coeff_is_negative = coeff_bit_count > coeff_neg_threshold_bits ||
					(coeff_bit_count == coeff_neg_threshold_bits && is_greater_than_or_equal_uint_uint(coeff.pointer(), coeff_neg_threshold.get(), sig_uint64_count));
				const uint64_t *pos_pointer;
				if (coeff_is_negative)
				{
					if (sub_uint_uint(balanced_encoder_.plain_modulus().pointer(), coeff.pointer(), sig_uint64_count, pos_value.get()))
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

		void SlotEncoder::decode_biguint(const SlotBigPoly &poly, BigUInt &destination)
		{
			if (poly.slot_size() != slot_size_)
			{
				throw invalid_argument("slot_size mismatch");
			}

			// Determine plain_modulus width.
			int plain_modulus_bits = balanced_encoder_.plain_modulus().significant_bit_count();
			if (plain_modulus_bits == 0)
			{
				throw invalid_argument("plain_modulus");
			}
			int sig_uint64_count = divide_round_up(plain_modulus_bits, bits_per_uint64);
			Pointer pos_value(allocate_uint(sig_uint64_count, pool_));

			// Construct threshold as same size.
			Pointer coeff_neg_threshold(allocate_uint(sig_uint64_count, pool_));

			// Set to value of (plain_modulus + 1) / 2
			half_round_up_uint(balanced_encoder_.plain_modulus().pointer(), sig_uint64_count, coeff_neg_threshold.get());
			int coeff_neg_threshold_bits = get_significant_bit_count_uint(coeff_neg_threshold.get(), sig_uint64_count);

			destination.set_zero();
			int result_uint64_count = destination.uint64_count();
			int result_bit_capacity = result_uint64_count * bits_per_uint64;
			bool result_is_negative = false;
			uint64_t *result = destination.pointer();
			for (int bit_index = poly.significant_coeff_count() - 1; bit_index >= 0; --bit_index)
			{
				const BigUInt &coeff = poly[bit_index];

				// Multiply result by base if starting next slot, failing if highest bit set.
				if (is_bit_set_uint(result, result_uint64_count, result_bit_capacity - 1))
				{
					throw out_of_range("poly");
				}
				if (bit_index % slot_size_ == slot_size_ - 1)
				{
					BigUInt base_biguint(result_bit_capacity);
					BigUInt temp_result(result_bit_capacity);
					base_biguint = balanced_encoder_.base();
					set_uint_uint(result, result_uint64_count, temp_result.pointer());
					multiply_uint_uint(temp_result.pointer(), base_biguint.pointer(), result_uint64_count, result);
				}


				// Get sign/magnitude of coefficient.
				int coeff_bit_count = coeff.significant_bit_count();
				if (coeff_bit_count > plain_modulus_bits)
				{
					// Coefficient has more bits than plain_modulus.
					throw out_of_range("poly");
				}
				bool coeff_is_negative = coeff_bit_count > coeff_neg_threshold_bits ||
					(coeff_bit_count == coeff_neg_threshold_bits && is_greater_than_or_equal_uint_uint(coeff.pointer(), coeff_neg_threshold.get(), sig_uint64_count));
				const uint64_t *pos_pointer;
				if (coeff_is_negative)
				{
					if (sub_uint_uint(balanced_encoder_.plain_modulus().pointer(), coeff.pointer(), sig_uint64_count, pos_value.get()))
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

		SlotEncoder::SlotEncoder(int slot_size, const BigUInt &plain_modulus, int base) : slot_size_(slot_size), balanced_encoder_(plain_modulus, base)
		{
			if (slot_size <= 0)
			{
				throw out_of_range("slot_size");
			}
		}
	}
}