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
        TEST_METHOD(FVKeyGeneration)
        {
            EncryptionParameters parms;
            BigUInt coeff_modulus;
            BigUInt plain_modulus;
            BigPoly poly_modulus;
            parms.set_decomposition_bit_count(4);
            parms.set_noise_standard_deviation(3.19);
            parms.set_noise_max_deviation(35.06);
            coeff_modulus.resize(48);
            coeff_modulus = "FFFFFFFFC001";
            plain_modulus.resize(7);
            plain_modulus = 1 << 6;
            poly_modulus.resize(65, 1);
            poly_modulus[0] = 1;
            poly_modulus[64] = 1;
            parms.set_poly_modulus(const_cast<const BigPoly &>(poly_modulus));
            parms.set_plain_modulus(const_cast<const BigUInt &>(plain_modulus));
            parms.set_coeff_modulus(const_cast<const BigUInt &>(coeff_modulus));
            parms.validate();
            KeyGenerator keygen(parms);
            keygen.generate(1);
            Assert::IsFalse(keygen.public_key()[0].is_zero());
            Assert::IsFalse(keygen.public_key()[1].is_zero());
            Assert::IsFalse(keygen.secret_key().is_zero());
            Assert::AreEqual(12, keygen.evaluation_keys()[0].first.size());
            Assert::AreEqual(12, keygen.evaluation_keys()[0].second.size());
            for (int i = 0; i < 12; ++i)
            {
                Assert::IsFalse(keygen.evaluation_keys()[0].first[i].is_zero());
                Assert::IsFalse(keygen.evaluation_keys()[0].second[i].is_zero());
            }

            BigPolyArray public_key = keygen.public_key();
            BigPoly secret_key = keygen.secret_key();
            keygen.generate(1);
            Assert::IsTrue(public_key[0] != keygen.public_key()[0]);
            Assert::IsTrue(public_key[1] != keygen.public_key()[1]);
            Assert::IsTrue(secret_key != keygen.secret_key());

            keygen.generate_evaluation_keys(2);
            Assert::AreEqual(12, keygen.evaluation_keys()[0].first.size());
            Assert::AreEqual(12, keygen.evaluation_keys()[0].second.size());
            Assert::AreEqual(12, keygen.evaluation_keys()[1].first.size());
            Assert::AreEqual(12, keygen.evaluation_keys()[1].second.size());

            for (int i = 0; i < 12; ++i)
            {
                Assert::IsFalse(keygen.evaluation_keys()[0].first[i].is_zero());
                Assert::IsFalse(keygen.evaluation_keys()[0].second[i].is_zero());
                Assert::IsFalse(keygen.evaluation_keys()[1].first[i].is_zero());
                Assert::IsFalse(keygen.evaluation_keys()[1].second[i].is_zero());
            }
        }
    };
}
