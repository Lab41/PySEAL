#include "CppUnitTest.h"
#include "encryptionparams.h"
#include "encryptor.h"
#include "decryptor.h"
#include "evaluator.h"
#include "keygenerator.h"
#include "encoder.h"
#include <cstdint>
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(EvaluatorTest)
    {
    public:
        TEST_METHOD(FVEncryptNegateDecrypt)
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
            parms.set_poly_modulus(poly_modulus);
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus(coeff_modulus);
            parms.validate();
            KeyGenerator keygen(parms);
            keygen.generate();

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms);
            Decryptor decryptor(parms, keygen.secret_key());

            BigPolyArray encrypted = encryptor.encrypt(encoder.encode(0x12345678));
            BigPolyArray negated;
            evaluator.negate(encrypted, negated);
            Assert::AreEqual(static_cast<int32_t>(-0x12345678), encoder.decode_int32(decryptor.decrypt(negated)));

            encrypted = encryptor.encrypt(encoder.encode(0));
            evaluator.negate(encrypted, negated);
            Assert::AreEqual(static_cast<int32_t>(0), encoder.decode_int32(decryptor.decrypt(negated)));

            encrypted = encryptor.encrypt(encoder.encode(1));
            evaluator.negate(encrypted, negated);
            Assert::AreEqual(static_cast<int32_t>(-1), encoder.decode_int32(decryptor.decrypt(negated)));

            encrypted = encryptor.encrypt(encoder.encode(-1));
            BigPolyArray negate2 = evaluator.negate(encrypted);
            Assert::AreEqual(static_cast<int32_t>(1), encoder.decode_int32(decryptor.decrypt(negate2)));

            encrypted = encryptor.encrypt(encoder.encode(2));
            evaluator.negate(encrypted, negated);
            Assert::AreEqual(static_cast<int32_t>(-2), encoder.decode_int32(decryptor.decrypt(negated)));

            encrypted = encryptor.encrypt(encoder.encode(-5));
            evaluator.negate(encrypted, negated);
            Assert::AreEqual(static_cast<int32_t>(5), encoder.decode_int32(decryptor.decrypt(negated)));
        }

        TEST_METHOD(FVEncryptAddDecrypt)
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
            parms.set_poly_modulus(poly_modulus); 
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus(coeff_modulus);
            parms.validate();

            KeyGenerator keygen(parms);
            keygen.generate();

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms);
            Decryptor decryptor(parms, keygen.secret_key());

            BigPolyArray encrypted1 = encryptor.encrypt(encoder.encode(0x12345678));
            BigPolyArray encrypted2 = encryptor.encrypt(encoder.encode(0x54321));
            BigPolyArray sum = evaluator.add(encrypted1, encrypted2);
            Assert::AreEqual(static_cast<uint64_t>(0x12399999), encoder.decode_uint64(decryptor.decrypt(sum)));

            encrypted1 = encryptor.encrypt(encoder.encode(0));
            encrypted2 = encryptor.encrypt(encoder.encode(0));
            sum = evaluator.add(encrypted1, encrypted2);
            Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(decryptor.decrypt(sum)));

            encrypted1 = encryptor.encrypt(encoder.encode(0));
            encrypted2 = encryptor.encrypt(encoder.encode(5));
            sum = evaluator.add(encrypted1, encrypted2);
            Assert::AreEqual(static_cast<uint64_t>(5), encoder.decode_uint64(decryptor.decrypt(sum)));

            encrypted1 = encryptor.encrypt(encoder.encode(5));
            encrypted2 = encryptor.encrypt(encoder.encode(-3));
            sum = evaluator.add(encrypted1, encrypted2);
            Assert::AreEqual(static_cast<int32_t>(2), encoder.decode_int32(decryptor.decrypt(sum)));

            encrypted1 = encryptor.encrypt(encoder.encode(-7));
            encrypted2 = encryptor.encrypt(encoder.encode(2));
            sum = evaluator.add(encrypted1, encrypted2);
            Assert::AreEqual(static_cast<int32_t>(-5), encoder.decode_int32(decryptor.decrypt(sum)));

            BigPoly plain1 = "2x^2 + 1x^1 + 3";
            BigPoly plain2 = "3x^3 + 4x^2 + 5x^1 + 6";
            BigPolyArray encrypted3 = encryptor.encrypt(plain1);
            BigPolyArray encrypted4 = encryptor.encrypt(plain2);
            BigPolyArray sum2 = evaluator.add(encrypted3, encrypted4);
            BigPoly plain_sum2 = decryptor.decrypt(sum2);
            Assert::IsTrue(plain_sum2.to_string() == "3x^3 + 6x^2 + 6x^1 + 9");

            plain1 = "3x^5 + 1x^4 + 4x^3 + 1";
            plain2 = "5x^2 + 9x^1 + 2";
            encrypted3 = encryptor.encrypt(plain1);
            encrypted4 = encryptor.encrypt(plain2);
            sum2 = evaluator.add(encrypted3, encrypted4);
            plain_sum2 = decryptor.decrypt(sum2);
            Assert::IsTrue(plain_sum2.to_string() == "3x^5 + 1x^4 + 4x^3 + 5x^2 + 9x^1 + 3");

        }

        TEST_METHOD(FVEncryptSubDecrypt)
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
            parms.set_poly_modulus(poly_modulus);
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus(coeff_modulus);
            parms.validate();
            KeyGenerator keygen(parms);
            keygen.generate();

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms);
            Decryptor decryptor(parms, keygen.secret_key());

            BigPolyArray encrypted1 = encryptor.encrypt(encoder.encode(0x12345678));
            BigPolyArray encrypted2 = encryptor.encrypt(encoder.encode(0x54321));
            BigPolyArray diff = evaluator.sub(encrypted1, encrypted2);
            Assert::AreEqual(static_cast<int32_t>(0x122F1357), encoder.decode_int32(decryptor.decrypt(diff)));

            encrypted1 = encryptor.encrypt(encoder.encode(0));
            encrypted2 = encryptor.encrypt(encoder.encode(0));
            diff = evaluator.sub(encrypted1, encrypted2);
            Assert::AreEqual(static_cast<int32_t>(0), encoder.decode_int32(decryptor.decrypt(diff)));

            encrypted1 = encryptor.encrypt(encoder.encode(0));
            encrypted2 = encryptor.encrypt(encoder.encode(5));
            diff = evaluator.sub(encrypted1, encrypted2);
            Assert::AreEqual(static_cast<int32_t>(-5), encoder.decode_int32(decryptor.decrypt(diff)));

            encrypted1 = encryptor.encrypt(encoder.encode(5));
            encrypted2 = encryptor.encrypt(encoder.encode(-3));
            diff = evaluator.sub(encrypted1, encrypted2);
            Assert::AreEqual(static_cast<int32_t>(8), encoder.decode_int32(decryptor.decrypt(diff)));

            encrypted1 = encryptor.encrypt(encoder.encode(-7));
            encrypted2 = encryptor.encrypt(encoder.encode(2));
            diff = evaluator.sub(encrypted1, encrypted2);
            Assert::AreEqual(static_cast<int32_t>(-9), encoder.decode_int32(decryptor.decrypt(diff)));
        }

        TEST_METHOD(FVEncryptAddPlainDecrypt)
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
            parms.set_poly_modulus(poly_modulus);
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus(coeff_modulus);
            parms.validate();
            KeyGenerator keygen(parms);
            keygen.generate();

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms);
            Decryptor decryptor(parms, keygen.secret_key());

            BigPolyArray encrypted1 = encryptor.encrypt(encoder.encode(0x12345678));
            BigPoly plain2(64, 48);
            plain2 = encoder.encode(0x54321);
            BigPolyArray sum = evaluator.add_plain(encrypted1, plain2);
            Assert::AreEqual(static_cast<uint64_t>(0x12399999), encoder.decode_uint64(decryptor.decrypt(sum)));

            encrypted1 = encryptor.encrypt(encoder.encode(0));
            plain2 = encoder.encode(0);
            sum = evaluator.add_plain(encrypted1, plain2);
            Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(decryptor.decrypt(sum)));

            encrypted1 = encryptor.encrypt(encoder.encode(0));
            plain2 = encoder.encode(5);
            sum = evaluator.add_plain(encrypted1, plain2);
            Assert::AreEqual(static_cast<uint64_t>(5), encoder.decode_uint64(decryptor.decrypt(sum)));

            encrypted1 = encryptor.encrypt(encoder.encode(5));
            plain2 = encoder.encode(-3);
            sum = evaluator.add_plain(encrypted1, plain2);
            Assert::AreEqual(static_cast<int32_t>(2), encoder.decode_int32(decryptor.decrypt(sum)));

            encrypted1 = encryptor.encrypt(encoder.encode(-7));
            plain2 = encoder.encode(2);
            sum = evaluator.add_plain(encrypted1, plain2);
            Assert::AreEqual(static_cast<int32_t>(-5), encoder.decode_int32(decryptor.decrypt(sum)));
        }

        TEST_METHOD(FVEncryptSubPlainDecrypt)
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
            parms.set_poly_modulus(poly_modulus);
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus(coeff_modulus);
            parms.validate();
            KeyGenerator keygen(parms);
            keygen.generate();

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms);
            Decryptor decryptor(parms, keygen.secret_key());

            BigPolyArray encrypted1 = encryptor.encrypt(encoder.encode(0x12345678));
            BigPoly plain2(64, 48);
            plain2 = encoder.encode(0x54321);
            BigPolyArray diff = evaluator.sub_plain(encrypted1, plain2);
            Assert::AreEqual(static_cast<int32_t>(0x122F1357), encoder.decode_int32(decryptor.decrypt(diff)));

            encrypted1 = encryptor.encrypt(encoder.encode(0));
            plain2 = encoder.encode(0);
            diff = evaluator.sub_plain(encrypted1, plain2);
            Assert::AreEqual(static_cast<int32_t>(0), encoder.decode_int32(decryptor.decrypt(diff)));

            encrypted1 = encryptor.encrypt(encoder.encode(0));
            plain2 = encoder.encode(5);
            diff = evaluator.sub_plain(encrypted1, plain2);
            Assert::AreEqual(static_cast<int32_t>(-5), encoder.decode_int32(decryptor.decrypt(diff)));

            encrypted1 = encryptor.encrypt(encoder.encode(5));
            plain2 = encoder.encode(-3);
            diff = evaluator.sub_plain(encrypted1, plain2);
            Assert::AreEqual(static_cast<int32_t>(8), encoder.decode_int32(decryptor.decrypt(diff)));

            encrypted1 = encryptor.encrypt(encoder.encode(-7));
            plain2 = encoder.encode(2);
            diff = evaluator.sub_plain(encrypted1, plain2);
            Assert::AreEqual(static_cast<int32_t>(-9), encoder.decode_int32(decryptor.decrypt(diff)));
        }

        TEST_METHOD(FVEncryptMultiplyPlainDecrypt)
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
            parms.set_poly_modulus(poly_modulus);
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus(coeff_modulus);
            parms.validate();
            KeyGenerator keygen(parms);
            keygen.generate();

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms);
            Decryptor decryptor(parms, keygen.secret_key());

            BigPolyArray encrypted1 = encryptor.encrypt(encoder.encode(0x12345678));
            BigPoly plain2(64, 48);
            plain2 = encoder.encode(0x54321);
            BigPolyArray product = evaluator.multiply_plain(encrypted1, plain2);
            Assert::AreEqual(static_cast<uint64_t>(0x5FCBBBB88D78), encoder.decode_uint64(decryptor.decrypt(product)));

            encrypted1 = encryptor.encrypt(encoder.encode(0));
            plain2 = encoder.encode(5);
            product = evaluator.multiply_plain(encrypted1, plain2);
            Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(decryptor.decrypt(product)));

            encrypted1 = encryptor.encrypt(encoder.encode(7));
            plain2 = encoder.encode(1);
            product = evaluator.multiply_plain(encrypted1, plain2);
            Assert::AreEqual(static_cast<uint64_t>(7), encoder.decode_uint64(decryptor.decrypt(product)));

            encrypted1 = encryptor.encrypt(encoder.encode(5));
            plain2 = encoder.encode(-3);
            product = evaluator.multiply_plain(encrypted1, plain2);
            Assert::AreEqual(static_cast<int32_t>(-15), encoder.decode_int32(decryptor.decrypt(product)));

            encrypted1 = encryptor.encrypt(encoder.encode(-7));
            plain2 = encoder.encode(2);
            product = evaluator.multiply_plain(encrypted1, plain2);
            Assert::AreEqual(static_cast<int32_t>(-14), encoder.decode_int32(decryptor.decrypt(product)));
        }

        TEST_METHOD(FVEncryptMultiplyDecrypt)
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
                coeff_modulus = "FFFFFFFFC001";
                plain_modulus.resize(7);
                plain_modulus = 1 << 6;
                poly_modulus.resize(65, 1);
                poly_modulus[0] = 1;
                poly_modulus[64] = 1;
                parms.set_poly_modulus(poly_modulus);
                parms.set_plain_modulus(plain_modulus);
                parms.set_coeff_modulus(coeff_modulus);
                parms.validate();
                KeyGenerator keygen(parms);
                keygen.generate();

                BalancedEncoder encoder(plain_modulus);

                Encryptor encryptor(parms, keygen.public_key());
                Evaluator evaluator(parms);
                Decryptor decryptor(parms, keygen.secret_key());

                BigPolyArray encrypted1 = encryptor.encrypt(encoder.encode(0x12345678));
                BigPolyArray encrypted2 = encryptor.encrypt(encoder.encode(0x54321));
                BigPolyArray product = evaluator.multiply(encrypted1, encrypted2);
                Assert::AreEqual(static_cast<uint64_t>(0x5FCBBBB88D78), encoder.decode_uint64(decryptor.decrypt(product)));

                encrypted1 = encryptor.encrypt(encoder.encode(0));
                encrypted2 = encryptor.encrypt(encoder.encode(0));
                product = evaluator.multiply(encrypted1, encrypted2);
                Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(decryptor.decrypt(product)));

                encrypted1 = encryptor.encrypt(encoder.encode(0));
                encrypted2 = encryptor.encrypt(encoder.encode(5));
                product = evaluator.multiply(encrypted1, encrypted2);
                Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(decryptor.decrypt(product)));

                encrypted1 = encryptor.encrypt(encoder.encode(7));
                encrypted2 = encryptor.encrypt(encoder.encode(1));
                product = evaluator.multiply(encrypted1, encrypted2);
                Assert::AreEqual(static_cast<uint64_t>(7), encoder.decode_uint64(decryptor.decrypt(product)));

                encrypted1 = encryptor.encrypt(encoder.encode(5));
                encrypted2 = encryptor.encrypt(encoder.encode(-3));
                product = evaluator.multiply(encrypted1, encrypted2);
                Assert::AreEqual(static_cast<int32_t>(-15), encoder.decode_int32(decryptor.decrypt(product)));

                encrypted1 = encryptor.encrypt(encoder.encode(-7));
                encrypted2 = encryptor.encrypt(encoder.encode(2));
                product = evaluator.multiply(encrypted1, encrypted2);
                Assert::AreEqual(static_cast<int32_t>(-14), encoder.decode_int32(decryptor.decrypt(product)));
            }

            {
                EncryptionParameters parms;
                BigUInt coeff_modulus;
                BigUInt plain_modulus;
                BigPoly poly_modulus;
                parms.set_decomposition_bit_count(4);
                parms.set_noise_standard_deviation(3.19);
                parms.set_noise_max_deviation(35.06);
                coeff_modulus = "FFFFFFFFFFFFFFFFFFFF";
                plain_modulus.resize(7);
                plain_modulus = 1 << 6;
                poly_modulus.resize(129, 1);
                poly_modulus[0] = 1;
                poly_modulus[128] = 1;
                parms.set_poly_modulus(poly_modulus);
                parms.set_plain_modulus(plain_modulus);
                parms.set_coeff_modulus(coeff_modulus);
                parms.validate();
                KeyGenerator keygen(parms);
                keygen.generate();

                BalancedEncoder encoder(plain_modulus);

                Encryptor encryptor(parms, keygen.public_key());
                Evaluator evaluator(parms);
                Decryptor decryptor(parms, keygen.secret_key());

                BigPolyArray encrypted1 = encryptor.encrypt(encoder.encode(0x12345678));
                BigPolyArray encrypted2 = encryptor.encrypt(encoder.encode(0x54321));
                BigPolyArray product = evaluator.multiply(encrypted1, encrypted2);
                Assert::AreEqual(static_cast<uint64_t>(0x5FCBBBB88D78), encoder.decode_uint64(decryptor.decrypt(product)));

                encrypted1 = encryptor.encrypt(encoder.encode(0));
                encrypted2 = encryptor.encrypt(encoder.encode(0));
                product = evaluator.multiply(encrypted1, encrypted2);
                Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(decryptor.decrypt(product)));

                encrypted1 = encryptor.encrypt(encoder.encode(0));
                encrypted2 = encryptor.encrypt(encoder.encode(5));
                product = evaluator.multiply(encrypted1, encrypted2);
                Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(decryptor.decrypt(product)));

                encrypted1 = encryptor.encrypt(encoder.encode(7));
                encrypted2 = encryptor.encrypt(encoder.encode(1));
                product = evaluator.multiply(encrypted1, encrypted2);
                Assert::AreEqual(static_cast<uint64_t>(7), encoder.decode_uint64(decryptor.decrypt(product)));

                encrypted1 = encryptor.encrypt(encoder.encode(5));
                encrypted2 = encryptor.encrypt(encoder.encode(-3));
                product = evaluator.multiply(encrypted1, encrypted2);
                Assert::AreEqual(static_cast<int32_t>(-15), encoder.decode_int32(decryptor.decrypt(product)));

                encrypted1 = encryptor.encrypt(encoder.encode(-7));
                encrypted2 = encryptor.encrypt(encoder.encode(2));
                product = evaluator.multiply(encrypted1, encrypted2);
                Assert::AreEqual(static_cast<int32_t>(-14), encoder.decode_int32(decryptor.decrypt(product)));
            }
        }

        TEST_METHOD(FVEncryptSquareDecrypt)
        {
            EncryptionParameters parms;
            BigUInt coeff_modulus;
            BigUInt plain_modulus;
            BigPoly poly_modulus;
            parms.set_decomposition_bit_count(4);
            parms.set_noise_standard_deviation(3.19);
            parms.set_noise_max_deviation(35.06);
            coeff_modulus = "FFFFFFFFFFFFFFFFFFFF";
            plain_modulus.resize(7);
            plain_modulus = 1 << 6;
            poly_modulus.resize(65, 1);
            poly_modulus[0] = 1;
            poly_modulus[64] = 1;
            parms.set_poly_modulus(poly_modulus);
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus(coeff_modulus);
            parms.validate();
            KeyGenerator keygen(parms);
            keygen.generate();

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms);
            Decryptor decryptor(parms, keygen.secret_key());

            BigPolyArray encrypted1 = encryptor.encrypt(encoder.encode(1));
            BigPolyArray product = evaluator.square(encrypted1);
            Assert::AreEqual(static_cast<uint64_t>(1), encoder.decode_uint64(decryptor.decrypt(product)));

            encrypted1 = encryptor.encrypt(encoder.encode(0));
            product = evaluator.square(encrypted1);
            Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(decryptor.decrypt(product)));

            encrypted1 = encryptor.encrypt(encoder.encode(-5));
            product = evaluator.square(encrypted1);
            Assert::AreEqual(static_cast<uint64_t>(25), encoder.decode_uint64(decryptor.decrypt(product)));

            encrypted1 = encryptor.encrypt(encoder.encode(-1));
            product = evaluator.square(encrypted1);
            Assert::AreEqual(static_cast<uint64_t>(1), encoder.decode_uint64(decryptor.decrypt(product)));

            encrypted1 = encryptor.encrypt(encoder.encode(123));
            product = evaluator.square(encrypted1);
            Assert::AreEqual(static_cast<uint64_t>(15129), encoder.decode_uint64(decryptor.decrypt(product)));
        }

        TEST_METHOD(FVEncryptMultiplyManyDecrypt)
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
            parms.set_poly_modulus(poly_modulus);
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus(coeff_modulus);
            parms.validate();
            KeyGenerator keygen(parms);
            keygen.generate(3);

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms, keygen.evaluation_keys());

            EvaluationKeys evk = keygen.evaluation_keys();
            int evk_size = evk.size();
            for (int i = 0; i < evk_size; ++i)
            {
                for (int j = 0; j < evk[0].first.size(); ++j)
                {
                    Assert::IsTrue(keygen.evaluation_keys()[i].first[j] == evaluator.evaluation_keys()[i].first[j]);
                    Assert::IsTrue(keygen.evaluation_keys()[i].second[j] == evaluator.evaluation_keys()[i].second[j]);
                }
            }

            Decryptor decryptor(parms, keygen.secret_key());

            BigPolyArray encrypted1 = encryptor.encrypt(encoder.encode(5));
            BigPolyArray encrypted2 = encryptor.encrypt(encoder.encode(6));
            BigPolyArray encrypted3 = encryptor.encrypt(encoder.encode(7));
            vector<BigPolyArray> encrypteds = { encrypted1, encrypted2, encrypted3 };
            BigPolyArray product = evaluator.multiply_many(encrypteds);
            Assert::AreEqual(static_cast<uint64_t>(210), encoder.decode_uint64(decryptor.decrypt(product)));

            encrypted1 = encryptor.encrypt(encoder.encode(-9));
            encrypted2 = encryptor.encrypt(encoder.encode(-17));
            encrypteds = { encrypted1, encrypted2 };
            product = evaluator.multiply_many(encrypteds);
            Assert::AreEqual(static_cast<uint64_t>(153), encoder.decode_uint64(decryptor.decrypt(product)));

            encrypted1 = encryptor.encrypt(encoder.encode(2));
            encrypted2 = encryptor.encrypt(encoder.encode(-31));
            encrypted3 = encryptor.encrypt(encoder.encode(7));
            encrypteds = { encrypted1, encrypted2, encrypted3 };
            product = evaluator.multiply_many(encrypteds);
            Assert::AreEqual(static_cast<int32_t>(-434), encoder.decode_int32(decryptor.decrypt(product)));

            encrypted1 = encryptor.encrypt(encoder.encode(1));
            encrypted2 = encryptor.encrypt(encoder.encode(-1));
            encrypted3 = encryptor.encrypt(encoder.encode(1));
            BigPolyArray encrypted4 = encryptor.encrypt(encoder.encode(-1));
            encrypteds = { encrypted1, encrypted2, encrypted3, encrypted4 };
            product = evaluator.multiply_many(encrypteds);
            Assert::AreEqual(static_cast<int32_t>(1), encoder.decode_int32(decryptor.decrypt(product)));

            encrypted1 = encryptor.encrypt(encoder.encode(98765));
            encrypted2 = encryptor.encrypt(encoder.encode(0));
            encrypted3 = encryptor.encrypt(encoder.encode(12345));
            encrypted4 = encryptor.encrypt(encoder.encode(34567));
            encrypteds = { encrypted1, encrypted2, encrypted3, encrypted4 };
            product = evaluator.multiply_many(encrypteds);
            Assert::AreEqual(static_cast<int32_t>(0), encoder.decode_int32(decryptor.decrypt(product)));
        }

        TEST_METHOD(FVEncryptExponentiateDecrypt)
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
            plain_modulus = 1 << 4;
            poly_modulus.resize(65, 1);
            poly_modulus[0] = 1;
            poly_modulus[64] = 1;
            parms.set_poly_modulus(poly_modulus);
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus(coeff_modulus);
            parms.validate();
            KeyGenerator keygen(parms);
            keygen.generate(1);

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms, keygen.evaluation_keys());
            Decryptor decryptor(parms, keygen.secret_key());

            BigPolyArray encrypted = encryptor.encrypt(encoder.encode(5));
            BigPolyArray power = evaluator.exponentiate(encrypted, 1);
            Assert::AreEqual(static_cast<uint64_t>(5), encoder.decode_uint64(decryptor.decrypt(power)));

            encrypted = encryptor.encrypt(encoder.encode(7));
            power = evaluator.exponentiate(encrypted, 2);
            Assert::AreEqual(static_cast<uint64_t>(49), encoder.decode_uint64(decryptor.decrypt(power)));

            encrypted = encryptor.encrypt(encoder.encode(-7));
            power = evaluator.exponentiate(encrypted, 3);
            Assert::AreEqual(static_cast<int32_t>(-343), encoder.decode_int32(decryptor.decrypt(power)));
        }

        TEST_METHOD(FVEncryptAddManyDecrypt)
        {
            EncryptionParameters parms;
            BigUInt coeff_modulus;
            BigUInt plain_modulus;
            BigPoly poly_modulus; 
            parms.set_decomposition_bit_count(2);
            parms.set_noise_standard_deviation(3.19);
            parms.set_noise_max_deviation(35.06);
            coeff_modulus.resize(48);
            coeff_modulus = "FFFFFFFFC001";
            plain_modulus.resize(7);
            plain_modulus = 1 << 4;
            poly_modulus.resize(65, 1);
            poly_modulus[0] = 1;
            poly_modulus[64] = 1;
            parms.set_poly_modulus(poly_modulus);
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus(coeff_modulus);
            parms.validate();
            KeyGenerator keygen(parms);
            keygen.generate();

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms);
            Decryptor decryptor(parms, keygen.secret_key());

            BigPolyArray encrypted1 = encryptor.encrypt(encoder.encode(5));
            BigPolyArray encrypted2 = encryptor.encrypt(encoder.encode(6));
            BigPolyArray encrypted3 = encryptor.encrypt(encoder.encode(7));
            vector<BigPolyArray> encrypteds = { encrypted1, encrypted2, encrypted3 };
            BigPolyArray product = evaluator.add_many(encrypteds);
            Assert::AreEqual(static_cast<uint64_t>(18), encoder.decode_uint64(decryptor.decrypt(product)));

            encrypted1 = encryptor.encrypt(encoder.encode(-9));
            encrypted2 = encryptor.encrypt(encoder.encode(-17));
            encrypteds = { encrypted1, encrypted2 };
            product = evaluator.add_many(encrypteds);
            Assert::AreEqual(static_cast<int32_t>(-26), encoder.decode_int32(decryptor.decrypt(product)));

            encrypted1 = encryptor.encrypt(encoder.encode(2));
            encrypted2 = encryptor.encrypt(encoder.encode(-31));
            encrypted3 = encryptor.encrypt(encoder.encode(7));
            encrypteds = { encrypted1, encrypted2, encrypted3 };
            product = evaluator.add_many(encrypteds);
            Assert::AreEqual(static_cast<int32_t>(-22), encoder.decode_int32(decryptor.decrypt(product)));

            encrypted1 = encryptor.encrypt(encoder.encode(1));
            encrypted2 = encryptor.encrypt(encoder.encode(-1));
            encrypted3 = encryptor.encrypt(encoder.encode(1));
            BigPolyArray encrypted4 = encryptor.encrypt(encoder.encode(-1));
            encrypteds = { encrypted1, encrypted2, encrypted3, encrypted4 };
            product = evaluator.add_many(encrypteds);
            Assert::AreEqual(static_cast<int32_t>(0), encoder.decode_int32(decryptor.decrypt(product)));

            encrypted1 = encryptor.encrypt(encoder.encode(98765));
            encrypted2 = encryptor.encrypt(encoder.encode(0));
            encrypted3 = encryptor.encrypt(encoder.encode(12345));
            encrypted4 = encryptor.encrypt(encoder.encode(34567));
            encrypteds = { encrypted1, encrypted2, encrypted3, encrypted4 };
            product = evaluator.add_many(encrypteds);
            Assert::AreEqual(static_cast<int32_t>(145677), encoder.decode_int32(decryptor.decrypt(product)));

            BalancedFractionalEncoder frac_encoder(plain_modulus, poly_modulus, 10, 15);
            encrypted1 = encryptor.encrypt(frac_encoder.encode(3.1415));
            encrypted2 = encryptor.encrypt(frac_encoder.encode(12.345));
            encrypted3 = encryptor.encrypt(frac_encoder.encode(98.765));
            encrypted4 = encryptor.encrypt(frac_encoder.encode(1.1111));
            encrypteds = { encrypted1, encrypted2, encrypted3, encrypted4 };
            product = evaluator.add_many(encrypteds);
            Assert::IsTrue(abs(frac_encoder.decode(decryptor.decrypt(product)) - 115.3626) < 0.000001);
        }

        TEST_METHOD(TransformPlainToFromNTT)
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
            parms.set_poly_modulus(poly_modulus);
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus(coeff_modulus);
            parms.validate();

            Evaluator evaluator(parms);
            BigPoly plain;

            plain = "0";
            evaluator.transform_to_ntt(plain);
            Assert::IsTrue(plain.to_string() == "0");
            evaluator.transform_from_ntt(plain);
            Assert::IsTrue(plain.to_string() == "0");

            plain = "1";
            evaluator.transform_to_ntt(plain);
            for (int i = 0; i < 64; i++)
            {
                Assert::IsTrue(plain[i].to_string() == "1");
            }
            Assert::IsTrue(plain[64].to_string() == "0");
            evaluator.transform_from_ntt(plain);
            Assert::IsTrue(plain.to_string() == "1");

            plain = "2";
            evaluator.transform_to_ntt(plain);
            for (int i = 0; i < 64; i++)
            {
                Assert::IsTrue(plain[i].to_string() == "2");
            }
            Assert::IsTrue(plain[64].to_string() == "0");
            evaluator.transform_from_ntt(plain);
            Assert::IsTrue(plain.to_string() == "2");

            plain = "Fx^10 + Ex^9 + Dx^8 + Cx^7 + Bx^6 + Ax^5 + 1x^4 + 2x^3 + 3x^2 + 4x^1 + 5";
            evaluator.transform_to_ntt(plain);
            evaluator.transform_from_ntt(plain);
            Assert::IsTrue(plain.to_string() == "Fx^10 + Ex^9 + Dx^8 + Cx^7 + Bx^6 + Ax^5 + 1x^4 + 2x^3 + 3x^2 + 4x^1 + 5");
        }

        TEST_METHOD(TransformEncryptedToFromNTT)
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
            parms.set_poly_modulus(poly_modulus);
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus(coeff_modulus);
            parms.validate();

            KeyGenerator keygen(parms);
            keygen.generate();

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms);
            Decryptor decryptor(parms, keygen.secret_key());

            BigPoly plain;
            BigPolyArray cipher;

            plain = "0";
            encryptor.encrypt(plain, cipher);
            evaluator.transform_to_ntt(cipher);
            evaluator.transform_from_ntt(cipher);
            decryptor.decrypt(cipher, plain);
            Assert::IsTrue(plain.to_string() == "0");

            plain = "1";
            encryptor.encrypt(plain, cipher);
            evaluator.transform_to_ntt(cipher);
            evaluator.transform_from_ntt(cipher);
            decryptor.decrypt(cipher, plain);
            Assert::IsTrue(plain.to_string() == "1");

            plain = "Fx^10 + Ex^9 + Dx^8 + Cx^7 + Bx^6 + Ax^5 + 1x^4 + 2x^3 + 3x^2 + 4x^1 + 5";
            encryptor.encrypt(plain, cipher);
            evaluator.transform_to_ntt(cipher);
            evaluator.transform_from_ntt(cipher);
            decryptor.decrypt(cipher, plain);
            Assert::IsTrue(plain.to_string() == "Fx^10 + Ex^9 + Dx^8 + Cx^7 + Bx^6 + Ax^5 + 1x^4 + 2x^3 + 3x^2 + 4x^1 + 5");
        }

        TEST_METHOD(FVEncryptMultiplyPlainNTTDecrypt)
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
            parms.set_poly_modulus(poly_modulus);
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus(coeff_modulus);
            parms.validate();

            KeyGenerator keygen(parms);
            keygen.generate();

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms);
            Decryptor decryptor(parms, keygen.secret_key());

            BigPoly plain;
            BigPoly plain_multiplier;
            BigPolyArray cipher;

            plain = "0";
            encryptor.encrypt(plain, cipher);
            evaluator.transform_to_ntt(cipher);
            plain_multiplier = "1";
            evaluator.transform_to_ntt(plain_multiplier);
            evaluator.multiply_plain_ntt(cipher, plain_multiplier, cipher);
            evaluator.transform_from_ntt(cipher);
            decryptor.decrypt(cipher, plain);
            Assert::IsTrue(plain.to_string() == "0");

            plain = "2";
            encryptor.encrypt(plain, cipher);
            evaluator.transform_to_ntt(cipher);
            plain_multiplier = "3";
            evaluator.transform_to_ntt(plain_multiplier);
            evaluator.multiply_plain_ntt(cipher, plain_multiplier, cipher);
            evaluator.transform_from_ntt(cipher);
            decryptor.decrypt(cipher, plain);
            Assert::IsTrue(plain.to_string() == "6");

            plain = "1";
            encryptor.encrypt(plain, cipher);
            evaluator.transform_to_ntt(cipher);
            plain_multiplier = "Fx^10 + Ex^9 + Dx^8 + Cx^7 + Bx^6 + Ax^5 + 1x^4 + 2x^3 + 3x^2 + 4x^1 + 5";
            evaluator.transform_to_ntt(plain_multiplier);
            evaluator.multiply_plain_ntt(cipher, plain_multiplier, cipher);
            evaluator.transform_from_ntt(cipher);
            decryptor.decrypt(cipher, plain);
            Assert::IsTrue(plain.to_string() == "Fx^10 + Ex^9 + Dx^8 + Cx^7 + Bx^6 + Ax^5 + 1x^4 + 2x^3 + 3x^2 + 4x^1 + 5");

            plain = "1x^20";
            encryptor.encrypt(plain, cipher);
            evaluator.transform_to_ntt(cipher);
            plain_multiplier = "Fx^10 + Ex^9 + Dx^8 + Cx^7 + Bx^6 + Ax^5 + 1x^4 + 2x^3 + 3x^2 + 4x^1 + 5";
            evaluator.transform_to_ntt(plain_multiplier);
            evaluator.multiply_plain_ntt(cipher, plain_multiplier, cipher);
            evaluator.transform_from_ntt(cipher);
            decryptor.decrypt(cipher, plain);
            Assert::IsTrue(plain.to_string() == "Fx^30 + Ex^29 + Dx^28 + Cx^27 + Bx^26 + Ax^25 + 1x^24 + 2x^23 + 3x^22 + 4x^21 + 5x^20");
        }
    };
}