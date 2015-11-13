#include "CppUnitTest.h"
#include "encryptionparams.h"
#include "keygenerator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(KeyGeneratorTest)
    {
    public:
        TEST_METHOD(KeyGeneration)
        {
            EncryptionParameters parms;
            BigUInt &coeff_modulus = parms.coeff_modulus();
            BigUInt &plain_modulus = parms.plain_modulus();
            BigPoly &poly_modulus = parms.poly_modulus();
            parms.decomposition_bit_count() = 4;
            parms.noise_standard_deviation() = 3.19;
            parms.noise_max_deviation() = 35.06;
            coeff_modulus.resize(48);
            coeff_modulus = "FFFFFFFFC001";
            plain_modulus.resize(7);
            plain_modulus = 1 << 6;
            poly_modulus.resize(64, 1);
            poly_modulus[0] = 1;
            poly_modulus[63] = 1;

            KeyGenerator keygen(parms);
            Assert::IsTrue(keygen.public_key().is_zero());
            Assert::IsTrue(keygen.secret_key().is_zero());
            Assert::AreEqual(12, keygen.evaluation_keys().count());
            for (int i = 0; i < 12; ++i)
            {
                Assert::IsTrue(keygen.evaluation_keys()[i].is_zero());
            }

            keygen.generate();
            Assert::IsFalse(keygen.public_key().is_zero());
            Assert::IsFalse(keygen.secret_key().is_zero());
            Assert::AreEqual(12, keygen.evaluation_keys().count());
            for (int i = 0; i < 12; ++i)
            {
                Assert::IsFalse(keygen.evaluation_keys()[i].is_zero());
            }

            BigPoly public_key = keygen.public_key();
            BigPoly secret_key = keygen.secret_key();
            keygen.generate();
            Assert::IsTrue(public_key != keygen.public_key());
            Assert::IsTrue(secret_key != keygen.secret_key());
        }
    };
}
