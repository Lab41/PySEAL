#ifndef SLOTDECRYPTOR_H
#define SLOTDECRYPTOR_H

#include "slotbigpoly.h"
#include "decryptor.h"

namespace fhelib
{
	namespace slot_encoding
	{
		class SlotDecryptor
		{
		public:
			SlotDecryptor(const EncryptionParameters &params, const BigPoly &secret_key);

			SlotBigPoly decrypt(const SlotBigPoly &encrypted);

			inline const BigPoly &secret_key() const
			{
				return decryptor_.secret_key();
			}

		private:
			Decryptor decryptor_;

			inline SlotDecryptor &operator =(const SlotDecryptor &assign)
			{
				return *this;
			}
		};
	}
}

#endif // SLOTDECRYPTOR_H