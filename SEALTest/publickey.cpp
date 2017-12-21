#include "CppUnitTest.h"
#include "seal/publickey.h"
#include "seal/context.h"
#include "seal/keygenerator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(PublicKeyTest)
    {
    public:
        TEST_METHOD(SaveLoadPublicKey)
        {
            stringstream stream;
            {
                EncryptionParameters parms;
                parms.set_noise_standard_deviation(3.19);
                parms.set_poly_modulus("1x^64 + 1");
                parms.set_plain_modulus(1 << 6);
                parms.set_coeff_modulus({ small_mods_60bit(0) });
                SEALContext context(parms);
                KeyGenerator keygen(context);

                PublicKey pk = keygen.public_key();
                Assert::IsTrue(pk.hash_block() == parms.hash_block());
                pk.save(stream);

                PublicKey pk2;
                pk2.load(stream);

                Assert::IsTrue(pk.data() == pk2.data());
                Assert::IsTrue(pk.hash_block() == pk2.hash_block());
            }
            {
                EncryptionParameters parms;
                parms.set_noise_standard_deviation(3.19);
                parms.set_poly_modulus("1x^256 + 1");
                parms.set_plain_modulus(1 << 20);
                parms.set_coeff_modulus({ small_mods_30bit(0), small_mods_40bit(0) });
                SEALContext context(parms);
                KeyGenerator keygen(context);

                PublicKey pk = keygen.public_key();
                Assert::IsTrue(pk.hash_block() == parms.hash_block());
                pk.save(stream);

                PublicKey pk2;
                pk2.load(stream);

                Assert::IsTrue(pk.data() == pk2.data());
                Assert::IsTrue(pk.hash_block() == pk2.hash_block());
            }
        }
    };
}