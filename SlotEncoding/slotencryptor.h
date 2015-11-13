#ifndef SLOTENCRYPTOR_H
#define SLOTENCRYPTOR_H

#include "slotbigpoly.h"
#include "encryptor.h"

namespace fhelib
{
	namespace slot_encoding
	{
		class SlotEncryptor
		{
		public:
			SlotEncryptor(const EncryptionParameters &params, const BigPoly &public_key);

			SlotBigPoly encrypt(const SlotBigPoly &plain);

			inline const BigPoly &public_key() const
			{
				return encryptor_.public_key();
			}

		private:
			Encryptor encryptor_;

			inline SlotEncryptor &operator =(const SlotEncryptor &assign)
			{
				return *this;
			}
		};
	}
}


#endif // SLOTENCRYPTOR_H