#include "slotencryptor.h"

using namespace std;

namespace fhelib
{
	namespace slot_encoding
	{
		SlotBigPoly SlotEncryptor::encrypt(const SlotBigPoly &plain)
		{
			SlotBigPoly result(plain.slot_size());
			result.set(encryptor_.encrypt(plain.poly()), plain.shift());

			return result;
		}

		SlotEncryptor::SlotEncryptor(const EncryptionParameters &params, const BigPoly &public_key) : encryptor_(params, public_key)
		{
		}
	}
}