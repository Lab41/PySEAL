#include "CppUnitTest.h"
#include "encryptionparams.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(EncryptionParams)
    {
    public:
        TEST_METHOD(EncryptionParamsWriteRead)
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
            parms.mode() = TEST_MODE;
            parms.random_generator() = UniformRandomGeneratorFactory::default_factory();

            Assert::AreEqual(4, parms.decomposition_bit_count());
            Assert::AreEqual(3.19, parms.noise_standard_deviation());
            Assert::AreEqual(35.06, parms.noise_max_deviation());
            Assert::IsTrue("FFFFFFFFC001" == parms.coeff_modulus().to_string());
            Assert::IsTrue("40" == parms.plain_modulus().to_string());
            Assert::IsTrue("1x^63 + 1" == parms.poly_modulus().to_string());
            Assert::IsTrue(parms.random_generator() == UniformRandomGeneratorFactory::default_factory());
            Assert::IsTrue(TEST_MODE == parms.mode());
        }

        TEST_METHOD(EncryptionParamsDefaultToNormalMode)
        {
            EncryptionParameters parms;
            Assert::IsTrue(NORMAL_MODE == parms.mode());
        }

        TEST_METHOD(SaveLoadEncryptionParams)
        {
            stringstream stream;

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

            EncryptionParameters params2;
            parms.save(stream);
            params2.load(stream);

            Assert::AreEqual(parms.decomposition_bit_count(), params2.decomposition_bit_count());
            Assert::AreEqual(parms.noise_standard_deviation(), params2.noise_standard_deviation());
            Assert::AreEqual(parms.noise_max_deviation(), params2.noise_max_deviation());
            Assert::IsTrue(parms.coeff_modulus() == params2.coeff_modulus());
            Assert::IsTrue(parms.plain_modulus() == params2.plain_modulus());
            Assert::IsTrue(parms.poly_modulus() == params2.poly_modulus());
        }
    };
}