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
            BigUInt coeff_modulus;
            BigPoly poly_modulus;
            BigUInt plain_modulus = parms.plain_modulus();
            parms.set_decomposition_bit_count(4);
            parms.set_noise_standard_deviation(3.19);
            parms.set_noise_max_deviation(35.06);
            coeff_modulus.resize(48);
            coeff_modulus = "7FFFFC801";
            parms.set_coeff_modulus(const_cast<const BigUInt &>(coeff_modulus)); 
            // aux_coeff_modulus = "FFFFFFFFC001";
            plain_modulus.resize(7);
            parms.set_plain_modulus(1 << 6);
            poly_modulus.resize(65, 1);
            poly_modulus[0] = 1;
            poly_modulus[64] = 1;
            parms.set_poly_modulus(const_cast<const BigPoly &>(poly_modulus));

            parms.set_random_generator(UniformRandomGeneratorFactory::default_factory());
            parms.validate();

            Assert::AreEqual(4, parms.decomposition_bit_count());
            Assert::AreEqual(3.19, parms.noise_standard_deviation());
            Assert::AreEqual(35.06, parms.noise_max_deviation());
            Assert::IsTrue("7FFFFC801" == parms.coeff_modulus().to_string());
            //Assert::IsTrue("FFFFFFFFC001" == parms.aux_coeff_modulus().to_string());
            Assert::IsTrue("40" == parms.plain_modulus().to_string());
            Assert::IsTrue("1x^64 + 1" == parms.poly_modulus().to_string());
            Assert::IsTrue(parms.random_generator() == UniformRandomGeneratorFactory::default_factory());
        }

        TEST_METHOD(SaveLoadEncryptionParams)
        {
            stringstream stream;

            EncryptionParameters parms;
            BigUInt coeff_modulus = parms.coeff_modulus();
            BigUInt plain_modulus = parms.plain_modulus();
            BigPoly poly_modulus = parms.poly_modulus();
            parms.set_decomposition_bit_count(4);
            parms.set_noise_standard_deviation(3.19);
            parms.set_noise_max_deviation(35.06);
            coeff_modulus.resize(48);
            coeff_modulus = "7FFFFC801";
            //aux_coeff_modulus = "FFFFFFFFC001";
            plain_modulus.resize(7);
            plain_modulus = 1 << 6;
            poly_modulus.resize(65, 1);
            poly_modulus[0] = 1;
            poly_modulus[64] = 1;

            EncryptionParameters params2;
            parms.save(stream);
            params2.load(stream);

            Assert::AreEqual(parms.decomposition_bit_count(), params2.decomposition_bit_count());
            Assert::AreEqual(parms.noise_standard_deviation(), params2.noise_standard_deviation());
            Assert::AreEqual(parms.noise_max_deviation(), params2.noise_max_deviation());
            Assert::IsTrue(parms.coeff_modulus() == params2.coeff_modulus());
            //Assert::IsTrue(parms.aux_coeff_modulus() == params2.aux_coeff_modulus());
            Assert::IsTrue(parms.plain_modulus() == params2.plain_modulus());
            Assert::IsTrue(parms.poly_modulus() == params2.poly_modulus());
        }

        TEST_METHOD(GetQualifiers)
        {
            {
                EncryptionParameters parms;
                BigUInt coeff_modulus;
                BigUInt plain_modulus;
                BigPoly poly_modulus;
                parms.set_decomposition_bit_count(4);
                parms.set_noise_standard_deviation(3.19);
                parms.set_noise_max_deviation(35.06);
                coeff_modulus.resize(48);
                coeff_modulus = "7FFFFC801";
                // aux_coeff_modulus = "FFFFFFFFC001";
                plain_modulus.resize(7);
                plain_modulus = 1 << 6;
                poly_modulus.resize(65, 1);
                poly_modulus[0] = 1;
                poly_modulus[64] = 1;
                parms.set_random_generator(UniformRandomGeneratorFactory::default_factory());
                parms.set_poly_modulus(const_cast<const BigPoly &>(poly_modulus));
                parms.set_plain_modulus(const_cast<const BigUInt &>(plain_modulus));
                parms.set_coeff_modulus(const_cast<const BigUInt &>(coeff_modulus));
                parms.validate();
                auto qualifiers = parms.get_qualifiers();
                Assert::IsTrue(qualifiers.parameters_set);
                Assert::IsTrue(qualifiers.enable_relinearization);
                Assert::IsTrue(qualifiers.enable_nussbaumer);
                Assert::IsTrue(qualifiers.enable_ntt);
#ifndef DISABLE_NTT_IN_MULTIPLY

                Assert::IsTrue(qualifiers.enable_ntt_in_multiply);
#endif
                Assert::IsFalse(qualifiers.enable_batching);
            }

            {
                EncryptionParameters parms;
                BigUInt coeff_modulus;
                BigUInt plain_modulus;
                BigPoly poly_modulus;
                parms.set_decomposition_bit_count(4);
                parms.set_noise_standard_deviation(3.19);
                parms.set_noise_max_deviation(35.06);
                coeff_modulus.resize(48);
                coeff_modulus = "7FFFFC801";
                // aux_coeff_modulus = "FFFFFF";
                plain_modulus.resize(7);
                plain_modulus = 1 << 6;
                poly_modulus.resize(65, 1);
                poly_modulus[0] = 1;
                poly_modulus[64] = 1;
                parms.set_poly_modulus(const_cast<const BigPoly &>(poly_modulus));
                parms.set_plain_modulus(const_cast<const BigUInt &>(plain_modulus));
                parms.set_coeff_modulus(const_cast<const BigUInt &>(coeff_modulus));
                parms.set_random_generator(UniformRandomGeneratorFactory::default_factory());
                parms.validate();

                auto qualifiers = parms.get_qualifiers();
                Assert::IsTrue(qualifiers.enable_relinearization);
                Assert::IsTrue(qualifiers.enable_nussbaumer);
                Assert::IsTrue(qualifiers.enable_ntt);
                Assert::IsFalse(qualifiers.enable_ntt_in_multiply);
                Assert::IsFalse(qualifiers.enable_batching);
                Assert::IsTrue(qualifiers.parameters_set);
            }

            {
                EncryptionParameters parms;
                BigUInt coeff_modulus;
                BigUInt plain_modulus;
                BigPoly poly_modulus;
                parms.set_decomposition_bit_count(4);
                parms.set_noise_standard_deviation(3.19);
                parms.set_noise_max_deviation(35.06);
                coeff_modulus.resize(48);
                coeff_modulus = "7FFFFC801";
                plain_modulus.resize(7);
                plain_modulus = 1 << 6;
                poly_modulus.resize(64, 1);
                poly_modulus[0] = 1;
                poly_modulus[63] = 1;
                parms.set_poly_modulus(const_cast<const BigPoly &>(poly_modulus));
                parms.set_plain_modulus(const_cast<const BigUInt &>(plain_modulus));
                parms.set_coeff_modulus(const_cast<const BigUInt &>(coeff_modulus));
                parms.set_random_generator(UniformRandomGeneratorFactory::default_factory());
                parms.validate();

                auto qualifiers = parms.get_qualifiers();
                Assert::IsFalse(qualifiers.parameters_set);
                Assert::IsFalse(qualifiers.enable_relinearization);
                Assert::IsFalse(qualifiers.enable_nussbaumer);
                Assert::IsFalse(qualifiers.enable_ntt);
                Assert::IsFalse(qualifiers.enable_ntt_in_multiply);
                Assert::IsFalse(qualifiers.enable_batching);
            }

            {
                EncryptionParameters parms;
                BigUInt coeff_modulus;
                BigUInt plain_modulus;
                BigPoly poly_modulus;
                parms.set_decomposition_bit_count(4);
                parms.set_noise_standard_deviation(3.19);
                parms.set_noise_max_deviation(35.06);
                coeff_modulus.resize(48);
                coeff_modulus = "0";
                plain_modulus.resize(7);
                plain_modulus = 1 << 6;
                poly_modulus.resize(65, 1);
                poly_modulus[0] = 1;
                poly_modulus[64] = 1;
                parms.set_poly_modulus(const_cast<const BigPoly &>(poly_modulus));
                parms.set_plain_modulus(const_cast<const BigUInt &>(plain_modulus));
                parms.set_coeff_modulus(const_cast<const BigUInt &>(coeff_modulus));
                parms.set_random_generator(UniformRandomGeneratorFactory::default_factory());
                parms.validate();
                auto qualifiers = parms.get_qualifiers();
                Assert::IsFalse(qualifiers.parameters_set);
                Assert::IsFalse(qualifiers.enable_relinearization);
                Assert::IsFalse(qualifiers.enable_nussbaumer);
                Assert::IsFalse(qualifiers.enable_ntt);
                Assert::IsFalse(qualifiers.enable_ntt_in_multiply);
                Assert::IsFalse(qualifiers.enable_batching);
            }

            {
                EncryptionParameters parms;
                BigUInt coeff_modulus;
                BigUInt plain_modulus;
                BigPoly poly_modulus;
                parms.set_noise_standard_deviation(3.19);
                parms.set_noise_max_deviation(35.06);
                coeff_modulus.resize(48);
                coeff_modulus = "7FFFFC801";
                plain_modulus.resize(7);
                plain_modulus = 1 << 6;
                poly_modulus.resize(65, 1);
                poly_modulus[0] = 1;
                poly_modulus[64] = 1;
                parms.set_random_generator(UniformRandomGeneratorFactory::default_factory());
                parms.set_poly_modulus(const_cast<const BigPoly &>(poly_modulus));
                parms.set_plain_modulus(const_cast<const BigUInt &>(plain_modulus));
                parms.set_coeff_modulus(const_cast<const BigUInt &>(coeff_modulus));
                parms.validate();
                auto qualifiers = parms.get_qualifiers();
                Assert::IsTrue(qualifiers.parameters_set);
                Assert::IsFalse(qualifiers.enable_relinearization);
                Assert::IsTrue(qualifiers.enable_nussbaumer);
                Assert::IsTrue(qualifiers.enable_ntt);
#ifndef DISABLE_NTT_IN_MULTIPLY
                Assert::IsTrue(qualifiers.enable_ntt_in_multiply);
#endif
                Assert::IsFalse(qualifiers.enable_batching);
            }

            {
                EncryptionParameters parms;
                BigUInt coeff_modulus;
                BigUInt plain_modulus;
                BigPoly poly_modulus;
                parms.set_decomposition_bit_count(-4);
                parms.set_noise_standard_deviation(-3.19);
                parms.set_noise_max_deviation(35.06);
                coeff_modulus.resize(48);
                coeff_modulus = "7FFFFC801";
                plain_modulus.resize(7);
                plain_modulus = 1 << 6;
                poly_modulus.resize(65, 1);
                poly_modulus[0] = 1;
                poly_modulus[64] = 1;
                parms.set_random_generator(UniformRandomGeneratorFactory::default_factory());
                parms.set_poly_modulus(const_cast<const BigPoly &>(poly_modulus));
                parms.set_plain_modulus(const_cast<const BigUInt &>(plain_modulus));
                parms.set_coeff_modulus(const_cast<const BigUInt &>(coeff_modulus));
                parms.validate();
                auto qualifiers = parms.get_qualifiers();
                Assert::IsFalse(qualifiers.parameters_set);
                Assert::IsFalse(qualifiers.enable_relinearization);
                Assert::IsFalse(qualifiers.enable_nussbaumer);
                Assert::IsFalse(qualifiers.enable_ntt);
                Assert::IsFalse(qualifiers.enable_ntt_in_multiply);
                Assert::IsFalse(qualifiers.enable_batching);
            }

            {
                EncryptionParameters parms;
                BigUInt coeff_modulus;
                BigUInt plain_modulus;
                BigPoly poly_modulus;
                parms.set_decomposition_bit_count(0);
                parms.set_noise_standard_deviation(3.19);
                parms.set_noise_max_deviation(35.06);
                coeff_modulus.resize(48);
                coeff_modulus = "7FFFFC801";
                plain_modulus.resize(7);
                plain_modulus = 1 << 6;
                poly_modulus.resize(65, 1);
                poly_modulus[0] = 1;
                poly_modulus[64] = 1;
                parms.set_random_generator(UniformRandomGeneratorFactory::default_factory());
                parms.set_poly_modulus(const_cast<const BigPoly &>(poly_modulus));
                parms.set_plain_modulus(const_cast<const BigUInt &>(plain_modulus));
                parms.set_coeff_modulus(const_cast<const BigUInt &>(coeff_modulus));
                parms.validate();
                auto qualifiers = parms.get_qualifiers();
                Assert::IsTrue(qualifiers.parameters_set);
                Assert::IsFalse(qualifiers.enable_relinearization);
                Assert::IsTrue(qualifiers.enable_nussbaumer);
                Assert::IsTrue(qualifiers.enable_ntt);
                Assert::IsFalse(qualifiers.enable_ntt_in_multiply);
                Assert::IsFalse(qualifiers.enable_batching);
            }

            {
                EncryptionParameters parms;
                BigUInt coeff_modulus;
                BigUInt plain_modulus;
                BigPoly poly_modulus;
                parms.set_decomposition_bit_count(0);
                parms.set_noise_standard_deviation(3.19);
                parms.set_noise_max_deviation(35.06);
                coeff_modulus.resize(48);
                coeff_modulus = "7FFFFFFFF";
                plain_modulus.resize(7);
                plain_modulus = 1 << 6;
                poly_modulus.resize(65, 1);
                poly_modulus[0] = 1;
                poly_modulus[64] = 1;
                parms.set_random_generator(UniformRandomGeneratorFactory::default_factory());
                parms.set_poly_modulus(const_cast<const BigPoly &>(poly_modulus));
                parms.set_plain_modulus(const_cast<const BigUInt &>(plain_modulus));
                parms.set_coeff_modulus(const_cast<const BigUInt &>(coeff_modulus));
                parms.validate();
                auto qualifiers = parms.get_qualifiers();
                Assert::IsTrue(qualifiers.parameters_set);
                Assert::IsFalse(qualifiers.enable_relinearization);
                Assert::IsTrue(qualifiers.enable_nussbaumer);
                Assert::IsFalse(qualifiers.enable_ntt);
                Assert::IsFalse(qualifiers.enable_ntt_in_multiply);
                Assert::IsFalse(qualifiers.enable_batching);
            }

            {
                EncryptionParameters parms;
                BigUInt coeff_modulus;
                BigUInt plain_modulus;
                BigPoly poly_modulus;
                parms.set_decomposition_bit_count(0);
                parms.set_noise_standard_deviation(3.19);
                parms.set_noise_max_deviation(35.06);
                coeff_modulus.resize(48);
                coeff_modulus = "7FFFFFFFF";
                plain_modulus.resize(7);
                plain_modulus = 12289;
                poly_modulus.resize(65, 1);
                poly_modulus[0] = 1;
                poly_modulus[64] = 1;
                parms.set_random_generator(UniformRandomGeneratorFactory::default_factory());
                parms.set_poly_modulus(const_cast<const BigPoly &>(poly_modulus));
                parms.set_plain_modulus(const_cast<const BigUInt &>(plain_modulus));
                parms.set_coeff_modulus(const_cast<const BigUInt &>(coeff_modulus));
                parms.validate();
                auto qualifiers = parms.get_qualifiers();
                Assert::IsTrue(qualifiers.parameters_set);
                Assert::IsFalse(qualifiers.enable_relinearization);
                Assert::IsTrue(qualifiers.enable_nussbaumer);
                Assert::IsFalse(qualifiers.enable_ntt);
                Assert::IsFalse(qualifiers.enable_ntt_in_multiply);
                Assert::IsTrue(qualifiers.enable_batching);
            }
        }
    };
}