#ifndef SLOTEVALUATOR_H
#define SLOTEVALUATOR_H

#include "slotbigpoly.h"
#include "evaluator.h"

namespace fhelib
{
	namespace slot_encoding
	{
		class SlotEvaluator
		{
		public:
			SlotEvaluator(const EncryptionParameters &params, const EvaluationKeys &evaluation_keys);

			void negate(const SlotBigPoly &encrypted, SlotBigPoly &destination);

			inline SlotBigPoly negate(const SlotBigPoly &encrypted)
			{
				return SlotBigPoly(encrypted.slot_size(), evaluator_.negate(encrypted.poly()));
			}

			void add(int shift_by, const SlotBigPoly &encrypted1, const SlotBigPoly &encrypted2, SlotBigPoly &destination);

			SlotBigPoly add(int shift_by, const SlotBigPoly &encrypted1, const SlotBigPoly &encrypted2);

			void sub(int shift_by, const SlotBigPoly &encrypted1, const SlotBigPoly &encrypted2, SlotBigPoly &destination);

			SlotBigPoly sub(int shift_by, const SlotBigPoly &encrypted1, const SlotBigPoly &encrypted2);

			void multiply(const SlotBigPoly &encrypted1, const SlotBigPoly &encrypted2, SlotBigPoly &destination);

			SlotBigPoly multiply(const SlotBigPoly &encrypted1, const SlotBigPoly &encrypted2);

			void tree_multiply(const std::vector<SlotBigPoly> &encrypteds, SlotBigPoly &destination);

			SlotBigPoly tree_multiply(const std::vector<SlotBigPoly> &encrypteds);

			void tree_exponentiate(const SlotBigPoly &encrypted, int exponent, SlotBigPoly &destination);

			void binary_exponentiate(const SlotBigPoly &encrypted, int exponent, SlotBigPoly &destination);

			SlotBigPoly tree_exponentiate(const SlotBigPoly &encrypted, int exponent);

			SlotBigPoly binary_exponentiate(const SlotBigPoly &encrypted, int exponent);

			void add_plain(int shift_by, const SlotBigPoly &encrypted1, const SlotBigPoly &plain2, SlotBigPoly &destination);

			SlotBigPoly add_plain(int shift_by, const SlotBigPoly &encrypted1, const SlotBigPoly &plain2);

			void sub_plain(int shift_by, const SlotBigPoly &encrypted1, const SlotBigPoly &plain2, SlotBigPoly &destination);

			SlotBigPoly sub_plain(int shift_by, const SlotBigPoly &encrypted1, const SlotBigPoly &plain2);

			void multiply_plain(int shift_by, const SlotBigPoly &encrypted1, SlotBigPoly plain2, SlotBigPoly &destination);

			SlotBigPoly multiply_plain(int shift_by, const SlotBigPoly &encrypted1, SlotBigPoly plain2);

			inline const EvaluationKeys &evaluation_keys() const
			{
				return evaluator_.evaluation_keys();
			}

		private:
			Evaluator evaluator_;
			
			BigPoly poly_modulus_;

			BigUInt coeff_modulus_;

			BigUInt plain_modulus_;

		    internal::MemoryPool pool_;

            internal::PolyModulus polymod_;

            internal::Modulus mod_;

			inline SlotEvaluator &operator =(const SlotEvaluator &assign)
			{
				return *this;
			}
		};
	}
}

#endif // SLOTEVALUATOR_H