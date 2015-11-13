#include "slotevaluator.h"
#include "internal\polyarithmod.h"
#include "internal\uintarith.h"
#include <algorithm>

using namespace std;
using namespace fhelib::internal;

namespace fhelib
{
	namespace slot_encoding
	{
		void SlotEvaluator::negate(const SlotBigPoly &encrypted, SlotBigPoly &destination)
		{
			if (encrypted.slot_size() != destination.slot_size())
			{
				throw invalid_argument("slot_size mismatch");
			}
			destination.set(evaluator_.negate(encrypted.poly()), encrypted.shift());
		}

		void SlotEvaluator::add(int shift_by, const SlotBigPoly &encrypted1, const SlotBigPoly &encrypted2, SlotBigPoly &destination)
		{
			if (shift_by < 0)
			{
				throw out_of_range("shift_by");
			}

			int slot_size = encrypted1.slot_size();
			if (slot_size != encrypted2.slot_size() || slot_size != destination.slot_size())
			{
				throw invalid_argument("slot_size mismatch");
			}

			// We need to shift the one that has already a larger shift
			SlotBigPoly encrypted_smaller_shift = encrypted1;
			SlotBigPoly encrypted_bigger_shift = encrypted2;
			if (encrypted1.shift() > encrypted2.shift())
			{
				swap(encrypted_smaller_shift, encrypted_bigger_shift);
			}

			// Shift if needed and add.
			if (shift_by != 0)
			{
				encrypted_bigger_shift.left_shift(shift_by);
				modulo_poly_inplace(encrypted_bigger_shift.pointer(), encrypted_bigger_shift.coeff_count(), polymod_, mod_, pool_);
				encrypted_bigger_shift.poly().resize(poly_modulus_.coeff_count(), coeff_modulus_.significant_bit_count());
			}

			destination.set(evaluator_.add(encrypted_bigger_shift.poly(), encrypted_smaller_shift.poly()), encrypted_bigger_shift.shift());
		}

		SlotBigPoly SlotEvaluator::add(int shift_by, const SlotBigPoly &encrypted1, const SlotBigPoly &encrypted2)
		{
			int slot_size = encrypted1.slot_size();
			SlotBigPoly result(slot_size);
			add(shift_by, encrypted1, encrypted2, result);

			return result;
		}

		void SlotEvaluator::sub(int shift_by, const SlotBigPoly &encrypted1, const SlotBigPoly &encrypted2, SlotBigPoly &destination)
		{
			if (shift_by < 0)
			{
				throw out_of_range("shift_by");
			}

			int slot_size = encrypted1.slot_size();
			if (slot_size != encrypted2.slot_size() || slot_size != destination.slot_size())
			{
				throw invalid_argument("slot_size mismatch");
			}

			// We need to shift the one that has already a larger shift
			SlotBigPoly encrypted_smaller_shift = encrypted1;
			SlotBigPoly encrypted_bigger_shift = encrypted2;
			if (encrypted1.shift() > encrypted2.shift())
			{
				swap(encrypted_smaller_shift, encrypted_bigger_shift);
			}

			// Shift if needed and subtract.
			if (shift_by != 0)
			{
				encrypted_bigger_shift.left_shift(shift_by);
				modulo_poly_inplace(encrypted_bigger_shift.pointer(), encrypted_bigger_shift.coeff_count(), polymod_, mod_, pool_);
				encrypted_bigger_shift.poly().resize(poly_modulus_.coeff_count(), coeff_modulus_.significant_bit_count());
			}

			destination.set(evaluator_.sub(encrypted_bigger_shift.poly(), encrypted_smaller_shift.poly()), encrypted_bigger_shift.shift());
		}

		SlotBigPoly SlotEvaluator::sub(int shift_by, const SlotBigPoly &encrypted1, const SlotBigPoly &encrypted2)
		{
			int slot_size = encrypted1.slot_size();
			SlotBigPoly result(slot_size);
			sub(shift_by, encrypted1, encrypted2, result);

			return result;
		}

		void SlotEvaluator::multiply(const SlotBigPoly &encrypted1, const SlotBigPoly &encrypted2, SlotBigPoly &destination)
		{
			int slot_size = encrypted1.slot_size();
			if (slot_size != encrypted2.slot_size() || slot_size != destination.slot_size())
			{
				throw invalid_argument("slot_size mismatch");
			}
			if (encrypted1.shift() + encrypted2.shift() >= slot_size)
			{
				throw out_of_range("shift");
			}

			destination.set(evaluator_.multiply(encrypted1.poly(), encrypted2.poly()), encrypted1.shift() + encrypted2.shift());
		}

		SlotBigPoly SlotEvaluator::multiply(const SlotBigPoly &encrypted1, const SlotBigPoly &encrypted2)
		{
			int slot_size = encrypted1.slot_size();
			SlotBigPoly result(slot_size);
			multiply(encrypted1, encrypted2, result);

			return result;
		}

		void SlotEvaluator::tree_multiply(const vector<SlotBigPoly> &encrypteds, SlotBigPoly &destination)
		{
			int slot_size = destination.slot_size();
			int total_shift = 0;
			vector<BigPoly> encrypted_polys;
			for (int i = 0; i < encrypteds.size(); ++i)
			{
				if (encrypteds[i].slot_size() != slot_size)
				{
					throw invalid_argument("slot_size mismatch");
				}
				total_shift += encrypteds[i].shift();
				encrypted_polys.push_back(encrypteds[i].poly());
			}
			if (total_shift >= slot_size)
			{
				throw out_of_range("shift");
			}

			destination.set(evaluator_.tree_multiply(encrypted_polys), total_shift);
		}

		SlotBigPoly SlotEvaluator::tree_multiply(const std::vector<SlotBigPoly> &encrypteds)
		{
			if (encrypteds.empty())
			{
				throw invalid_argument("encrypteds");
			}
			int slot_size = encrypteds[0].slot_size();
			SlotBigPoly result(slot_size);
			tree_multiply(encrypteds, result);

			return result;
		}

		void SlotEvaluator::tree_exponentiate(const SlotBigPoly &encrypted, int exponent, SlotBigPoly &destination)
		{
			int slot_size = encrypted.slot_size();
			if (slot_size != destination.slot_size())
			{
				throw invalid_argument("slot_size mismatch");
			}
			if (exponent * encrypted.shift() >= slot_size)
			{
				throw out_of_range("shift");
			}

			destination.set(evaluator_.tree_exponentiate(encrypted.poly(), exponent), exponent * encrypted.shift());
		}

		void SlotEvaluator::binary_exponentiate(const SlotBigPoly &encrypted, int exponent, SlotBigPoly &destination)
		{
			int slot_size = encrypted.slot_size();
			if (slot_size != destination.slot_size())
			{
				throw invalid_argument("slot_size mismatch");
			}
			if (exponent * encrypted.shift() >= slot_size)
			{
				throw out_of_range("shift");
			}

			destination.set(evaluator_.binary_exponentiate(encrypted.poly(), exponent), exponent * encrypted.shift());
		}

		SlotBigPoly SlotEvaluator::tree_exponentiate(const SlotBigPoly &encrypted, int exponent)
		{
			int slot_size = encrypted.slot_size();
			SlotBigPoly result(slot_size);
			tree_exponentiate(encrypted, exponent, result);

			return result;
		}

		SlotBigPoly SlotEvaluator::binary_exponentiate(const SlotBigPoly &encrypted, int exponent)
		{
			int slot_size = encrypted.slot_size();
			SlotBigPoly result(slot_size);
			binary_exponentiate(encrypted, exponent, result);

			return result;
		}

		void SlotEvaluator::add_plain(int shift_by, const SlotBigPoly &encrypted1, const SlotBigPoly &plain2, SlotBigPoly &destination)
		{
			if (shift_by < 0)
			{
				throw out_of_range("shift_by");
			}

			int slot_size = encrypted1.slot_size();
			if (slot_size != plain2.slot_size() || slot_size != destination.slot_size())
			{
				throw invalid_argument("slot_size mismatch");
			}
			if (plain2.shift() != 0)
			{
				throw invalid_argument("plain2");
			}

			// We need to shift encrypted1 since it has always the larger shift (compare to add(...))
			destination = encrypted1;

			// Shift if needed and add.
			if (shift_by != 0)
			{
				destination.left_shift(shift_by);
				modulo_poly_inplace(destination.pointer(), destination.coeff_count(), polymod_, mod_, pool_);
				destination.poly().resize(poly_modulus_.coeff_count(), coeff_modulus_.significant_bit_count());
			}

			destination.set(evaluator_.add_plain(destination.poly(), plain2.poly()), destination.shift());
		}

		SlotBigPoly SlotEvaluator::add_plain(int shift_by, const SlotBigPoly &encrypted1, const SlotBigPoly &plain2)
		{
			int slot_size = encrypted1.slot_size();
			SlotBigPoly result(slot_size);
			add_plain(shift_by, encrypted1, plain2, result);

			return result;
		}

		void SlotEvaluator::sub_plain(int shift_by, const SlotBigPoly &encrypted1, const SlotBigPoly &plain2, SlotBigPoly &destination)
		{
			if (shift_by < 0)
			{
				throw out_of_range("shift_by");
			}

			int slot_size = encrypted1.slot_size();
			if (slot_size != plain2.slot_size() || slot_size != destination.slot_size())
			{
				throw invalid_argument("slot_size mismatch");
			}
			if (plain2.shift() != 0)
			{
				throw invalid_argument("plain2");
			}

			// We need to shift encrypted1 since it has always the larger shift (compare to add(...))
			destination = encrypted1;

			// Shift if needed and subtract.
			if (shift_by != 0)
			{
				destination.left_shift(shift_by);
				modulo_poly_inplace(destination.pointer(), destination.coeff_count(), polymod_, mod_, pool_);
				destination.poly().resize(poly_modulus_.coeff_count(), coeff_modulus_.significant_bit_count());
			}

			destination.set(evaluator_.sub_plain(destination.poly(), plain2.poly()), destination.shift());
		}

		SlotBigPoly SlotEvaluator::sub_plain(int shift_by, const SlotBigPoly &encrypted1, const SlotBigPoly &plain2)
		{
			int slot_size = encrypted1.slot_size();
			SlotBigPoly result(slot_size);
			sub_plain(shift_by, encrypted1, plain2, result);

			return result;
		}

		void SlotEvaluator::multiply_plain(int shift_by, const SlotBigPoly &encrypted1, SlotBigPoly plain2, SlotBigPoly &destination)
		{
			// Shift only every nth term by 0, shift_by, 2*shift_by, ...
			// Could be made to be a parameter
			int shift_every_nth = 1;

			if (shift_by < 0)
			{
				throw out_of_range("shift_by");
			}

			int slot_size = encrypted1.slot_size();
			if (slot_size != plain2.slot_size() || slot_size != destination.slot_size())
			{
				throw invalid_argument("slot_size mismatch");
			}

			// Make sure plain2 is of valid form, i.e. no shift in the polynomial. set_shift() also sets the number of coefficients allocated
			// to be a multiple of slot_size, which we will need.
			plain2.set_shift();
			if (plain2.shift() != 0)
			{
				throw invalid_argument("plain2");
			}

			// We need to know the number of non-zero terms in plain2. At the same time we shift coefficients appropriately within slots.
			int plain_terms = 0;
			int shift_amount = shift_by;
			int last_shift_amount = 0;
			int poly_coeff_bit_count = plain_modulus_.significant_bit_count();
			int poly_coeff_uint64_count = divide_round_up(poly_coeff_bit_count, bits_per_uint64);
			uint64_t *coeff_pointer = plain2.pointer();
			for (int slot_index = 1; slot_index < plain2.slot_count(); ++slot_index)
			{
				coeff_pointer += slot_size * poly_coeff_uint64_count;
				if (!plain2[slot_index * slot_size].is_zero())
				{
					left_shift_uint(coeff_pointer, shift_amount * poly_coeff_uint64_count * bits_per_uint64, slot_size * poly_coeff_uint64_count, coeff_pointer);

					// Set the next shift
					last_shift_amount = shift_amount;
					shift_amount += (plain_terms % shift_every_nth == shift_every_nth - 1) ? shift_by : 0;

					++plain_terms;
				}
			}
			
			// Check if things went horribly wrong. If not, set shift_ in plain2 to be correct.
			if (last_shift_amount >= slot_size)
			{
				throw out_of_range("shift");
			}
			plain2.set_shift(last_shift_amount);

			// Finally do the multiplication
			if (encrypted1.shift() + plain2.shift() >= slot_size)
			{
				throw out_of_range("shift");
			}

			destination.set(evaluator_.multiply_plain(encrypted1.poly(), plain2.poly()), encrypted1.shift() + plain2.shift());
		}

		SlotBigPoly SlotEvaluator::multiply_plain(int shift_by, const SlotBigPoly &encrypted1, SlotBigPoly plain2)
		{
			int slot_size = encrypted1.slot_size();
			SlotBigPoly result(slot_size);
			multiply_plain(shift_by, encrypted1, plain2, result);

			return result;
		}

		SlotEvaluator::SlotEvaluator(const EncryptionParameters &params, const EvaluationKeys &evaluation_keys) : 
			evaluator_(params, evaluation_keys), poly_modulus_(params.poly_modulus()), coeff_modulus_(params.coeff_modulus()), plain_modulus_(params.plain_modulus())
		{
            // Resize encryption parameters to consistent size.
            int coeff_count = poly_modulus_.coeff_count();
            int coeff_bit_count = max(max(poly_modulus_.coeff_bit_count(), coeff_modulus_.bit_count()), plain_modulus_.bit_count());
            int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
            if (poly_modulus_.coeff_count() != coeff_count || poly_modulus_.coeff_bit_count() != coeff_bit_count)
            {
                poly_modulus_.resize(coeff_count, coeff_bit_count);
            }
            if (coeff_modulus_.bit_count() != coeff_bit_count)
            {
                coeff_modulus_.resize(coeff_bit_count);
            }
            if (plain_modulus_.bit_count() != coeff_bit_count)
            {
                plain_modulus_.resize(coeff_bit_count);
            }

            // Initialize moduli.
            polymod_ = PolyModulus(poly_modulus_.pointer(), coeff_count, coeff_uint64_count);
            mod_ = Modulus(coeff_modulus_.pointer(), coeff_uint64_count, pool_);
		}
	}
}