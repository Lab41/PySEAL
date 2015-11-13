#ifndef SLOTENCODER_H
#define SLOTENCODER_H

#include "slotbigpoly.h"
#include "encoder.h"

namespace fhelib
{
	namespace slot_encoding
	{
		class SlotEncoder
		{
		public:
			SlotEncoder(int slot_size, const BigUInt &plain_modulus, int base = 3);

			SlotBigPoly encode(std::uint64_t value);

			void encode(std::uint64_t value, SlotBigPoly &destination);

			// To prevent problems with automatic type conversion from int32_t
			inline SlotBigPoly encode(std::int32_t value)
			{
				return encode(static_cast<int64_t>(value));
			}

			// To prevent problems with automatic type conversion from uint32_t
			inline SlotBigPoly encode(std::uint32_t value)
			{
				return encode(static_cast<uint64_t>(value));
			}

			SlotBigPoly encode(std::int64_t value);

			void encode(std::int64_t value, SlotBigPoly &destination);

			SlotBigPoly encode(BigUInt value);

			void encode(BigUInt value, SlotBigPoly &destination);

			std::uint32_t decode_uint32(const SlotBigPoly &poly);

			std::uint64_t decode_uint64(const SlotBigPoly &poly);

			std::int32_t decode_int32(const SlotBigPoly &poly);

			std::int64_t decode_int64(const SlotBigPoly &poly);

			BigUInt decode_biguint(const SlotBigPoly &poly);

			void decode_biguint(const SlotBigPoly &poly, BigUInt &destination);

			inline int base() const
			{
				return balanced_encoder_.base();
			}

			void set_base(int value);

			void set_plain_modulus(BigUInt &value);

			void set_slot_size(int value);

			inline int slot_size() const
			{
				return slot_size_;
			}

			inline const BigUInt &plain_modulus() const
			{
				return balanced_encoder_.plain_modulus();
			}

		private:
			BalancedEncoder balanced_encoder_;

			int slot_size_;

		    internal::MemoryPool pool_;

			inline SlotEncoder &operator =(const SlotEncoder &assign)
			{
				return *this;
			}
		};

	}
}

#endif // SLOTENCODER_H