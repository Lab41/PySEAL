#include "slotdecryptor.h"

using namespace std;

namespace fhelib
{
	namespace slot_encoding
	{
		SlotBigPoly SlotDecryptor::decrypt(const SlotBigPoly &encrypted)
		{
			SlotBigPoly result(encrypted.slot_size());
			result.set(decryptor_.decrypt(encrypted.poly()), encrypted.shift());

			return result;
		}

		SlotDecryptor::SlotDecryptor(const EncryptionParameters &params, const BigPoly &secret_key) : decryptor_(params, secret_key)
		{
		}
	}
}