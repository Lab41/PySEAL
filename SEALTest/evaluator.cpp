#include "CppUnitTest.h"
#include "encryptionparams.h"
#include "encryptor.h"
#include "decryptor.h"
#include "evaluator.h"
#include "keygenerator.h"
#include "encoder.h"
#include <cstdint>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(EvaluatorTest)
    {
    public:
        TEST_METHOD(EncryptNegateDecrypt)
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
            keygen.generate();

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms, keygen.evaluation_keys());
            Decryptor decryptor(parms, keygen.secret_key());

            BigPoly encrypted = encryptor.encrypt(encoder.encode(0x12345678));
            BigPoly negated = evaluator.negate(encrypted);
            Assert::AreEqual(static_cast<int32_t>(-0x12345678), encoder.decode_int32(decryptor.decrypt(negated)));

            encrypted = encryptor.encrypt(encoder.encode(0));
            negated = evaluator.negate(encrypted);
            Assert::AreEqual(static_cast<int32_t>(0), encoder.decode_int32(decryptor.decrypt(negated)));

            encrypted = encryptor.encrypt(encoder.encode(1));
            negated = evaluator.negate(encrypted);
            Assert::AreEqual(static_cast<int32_t>(-1), encoder.decode_int32(decryptor.decrypt(negated)));

            encrypted = encryptor.encrypt(encoder.encode(-1));
            negated = evaluator.negate(encrypted);
            Assert::AreEqual(static_cast<int32_t>(1), encoder.decode_int32(decryptor.decrypt(negated)));

            encrypted = encryptor.encrypt(encoder.encode(2));
            negated = evaluator.negate(encrypted);
            Assert::AreEqual(static_cast<int32_t>(-2), encoder.decode_int32(decryptor.decrypt(negated)));

            encrypted = encryptor.encrypt(encoder.encode(-5));
            negated = evaluator.negate(encrypted);
            Assert::AreEqual(static_cast<int32_t>(5), encoder.decode_int32(decryptor.decrypt(negated)));
        }

        TEST_METHOD(EncryptAddDecrypt)
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
            keygen.generate();

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms, keygen.evaluation_keys());
            Decryptor decryptor(parms, keygen.secret_key());

            BigPoly encrypted1 = encryptor.encrypt(encoder.encode(0x12345678));
            BigPoly encrypted2 = encryptor.encrypt(encoder.encode(0x54321));
            BigPoly sum = evaluator.add(encrypted1, encrypted2);
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
        }

        TEST_METHOD(EncryptAddPlainDecrypt)
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
            keygen.generate();

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms, keygen.evaluation_keys());
            Decryptor decryptor(parms, keygen.secret_key());

            BigPoly encrypted1 = encryptor.encrypt(encoder.encode(0x12345678));
            BigPoly plain2(64, 48);
            plain2 = encoder.encode(0x54321);
            BigPoly sum = evaluator.add_plain(encrypted1, plain2);
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

        TEST_METHOD(EncryptSubDecrypt)
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
            keygen.generate();

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms, keygen.evaluation_keys());
            Decryptor decryptor(parms, keygen.secret_key());

            BigPoly encrypted1 = encryptor.encrypt(encoder.encode(0x12345678));
            BigPoly encrypted2 = encryptor.encrypt(encoder.encode(0x54321));
            BigPoly diff = evaluator.sub(encrypted1, encrypted2);
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

        TEST_METHOD(EncryptSubPlainDecrypt)
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
            keygen.generate();

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms, keygen.evaluation_keys());
            Decryptor decryptor(parms, keygen.secret_key());

            BigPoly encrypted1 = encryptor.encrypt(encoder.encode(0x12345678));
            BigPoly plain2(64, 48);
            plain2 = encoder.encode(0x54321);
            BigPoly diff = evaluator.sub_plain(encrypted1, plain2);
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

        TEST_METHOD(EncryptMultiplyDecrypt)
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
            keygen.generate();

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms, keygen.evaluation_keys());
            for (int i = 0; i < keygen.evaluation_keys().count(); ++i)
            {
                Assert::IsTrue(keygen.evaluation_keys()[i] == evaluator.evaluation_keys()[i]);
            }
            Decryptor decryptor(parms, keygen.secret_key());

            BigPoly encrypted1 = encryptor.encrypt(encoder.encode(0x12345678));
            BigPoly encrypted2 = encryptor.encrypt(encoder.encode(0x54321));
            BigPoly product = evaluator.multiply(encrypted1, encrypted2);
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

        TEST_METHOD(EncryptFFTMultiplyDecrypt)
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
            poly_modulus.resize(65, 1);
            poly_modulus[0] = 1;
            poly_modulus[64] = 1;

            KeyGenerator keygen(parms);
            keygen.generate();

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms, keygen.evaluation_keys());
            for (int i = 0; i < keygen.evaluation_keys().count(); ++i)
            {
                Assert::IsTrue(keygen.evaluation_keys()[i] == evaluator.evaluation_keys()[i]);
            }
            Decryptor decryptor(parms, keygen.secret_key());

            BigPoly encrypted1 = encryptor.encrypt(encoder.encode(0x12345678));
            BigPoly encrypted2 = encryptor.encrypt(encoder.encode(0x54321));
            BigPoly product = evaluator.multiply(encrypted1, encrypted2);
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

        TEST_METHOD(EncryptMultiplyPlainDecrypt)
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
            keygen.generate();

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms, keygen.evaluation_keys());
            for (int i = 0; i < keygen.evaluation_keys().count(); ++i)
            {
                Assert::IsTrue(keygen.evaluation_keys()[i] == evaluator.evaluation_keys()[i]);
            }
            Decryptor decryptor(parms, keygen.secret_key());

            BigPoly encrypted1 = encryptor.encrypt(encoder.encode(0x12345678));
            BigPoly plain2(64, 48);
            plain2 = encoder.encode(0x54321);
            BigPoly product = evaluator.multiply_plain(encrypted1, plain2);
            Assert::AreEqual(static_cast<uint64_t>(0x5FCBBBB88D78), encoder.decode_uint64(decryptor.decrypt(product)));

            encrypted1 = encryptor.encrypt(encoder.encode(0));
            plain2 = encoder.encode(0);
            product = evaluator.multiply_plain(encrypted1, plain2);
            Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(decryptor.decrypt(product)));

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

        TEST_METHOD(EncryptMultiplyPlain2Decrypt)
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
            keygen.generate();

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms, keygen.evaluation_keys());
            for (int i = 0; i < keygen.evaluation_keys().count(); ++i)
            {
                Assert::IsTrue(keygen.evaluation_keys()[i] == evaluator.evaluation_keys()[i]);
            }
            Decryptor decryptor(parms, keygen.secret_key());

            BigPoly encrypted1 = encryptor.encrypt(encoder.encode(0x12345678));
            BigPoly plain2(64, 48);
            plain2 = encoder.encode(0x54321);
            BigPoly product = evaluator.multiply_plain(encrypted1, plain2);
            Assert::AreEqual(static_cast<uint64_t>(0x5FCBBBB88D78), encoder.decode_uint64(decryptor.decrypt(product)));

            encrypted1 = encryptor.encrypt(encoder.encode(0));
            plain2 = encoder.encode(0);
            product = evaluator.multiply_plain(encrypted1, plain2);
            Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(decryptor.decrypt(product)));

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

        TEST_METHOD(EncryptMultiplyManyDecrypt)
        {
            EncryptionParameters parms;
            BigUInt &coeff_modulus = parms.coeff_modulus();
            BigUInt &plain_modulus = parms.plain_modulus();
            BigPoly &poly_modulus = parms.poly_modulus();
            parms.decomposition_bit_count() = 2;
            parms.noise_standard_deviation() = 3.19;
            parms.noise_max_deviation() = 35.06;
            coeff_modulus.resize(48);
            coeff_modulus = "FFFFFFFFC001";
            plain_modulus.resize(7);
            plain_modulus = 1 << 4;
            poly_modulus.resize(64, 1);
            poly_modulus[0] = 1;
            poly_modulus[63] = 1;

            KeyGenerator keygen(parms);
            keygen.generate();

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms, keygen.evaluation_keys());
            for (int i = 0; i < keygen.evaluation_keys().count(); ++i)
            {
                Assert::IsTrue(keygen.evaluation_keys()[i] == evaluator.evaluation_keys()[i]);
            }
            Decryptor decryptor(parms, keygen.secret_key());

            BigPoly encrypted1 = encryptor.encrypt(encoder.encode(5));
            BigPoly encrypted2 = encryptor.encrypt(encoder.encode(6));
            BigPoly encrypted3 = encryptor.encrypt(encoder.encode(7));
            vector<BigPoly> encrypteds = { encrypted1, encrypted2, encrypted3 };
            BigPoly product = evaluator.multiply_many(encrypteds);
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
            BigPoly encrypted4 = encryptor.encrypt(encoder.encode(-1));
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

        TEST_METHOD(EncryptExponentiateDecrypt)
        {
            EncryptionParameters parms;
            BigUInt &coeff_modulus = parms.coeff_modulus();
            BigUInt &plain_modulus = parms.plain_modulus();
            BigPoly &poly_modulus = parms.poly_modulus();
            parms.decomposition_bit_count() = 2;
            parms.noise_standard_deviation() = 3.19;
            parms.noise_max_deviation() = 35.06;
            coeff_modulus.resize(48);
            coeff_modulus = "FFFFFFFFC001";
            plain_modulus.resize(7);
            plain_modulus = 1 << 4;
            poly_modulus.resize(64, 1);
            poly_modulus[0] = 1;
            poly_modulus[63] = 1;

            KeyGenerator keygen(parms);
            keygen.generate();

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms, keygen.evaluation_keys());
            for (int i = 0; i < keygen.evaluation_keys().count(); ++i)
            {
                Assert::IsTrue(keygen.evaluation_keys()[i] == evaluator.evaluation_keys()[i]);
            }
            Decryptor decryptor(parms, keygen.secret_key());

            BigPoly encrypted = encryptor.encrypt(encoder.encode(5));
            BigPoly power = evaluator.exponentiate(encrypted, 1);
            Assert::AreEqual(static_cast<uint64_t>(5), encoder.decode_uint64(decryptor.decrypt(power)));

            encrypted = encryptor.encrypt(encoder.encode(5));
            power = evaluator.exponentiate(encrypted, 0);
            Assert::AreEqual(static_cast<uint64_t>(1), encoder.decode_uint64(decryptor.decrypt(power)));

            encrypted = encryptor.encrypt(encoder.encode(7));
            power = evaluator.exponentiate(encrypted, 2);
            Assert::AreEqual(static_cast<uint64_t>(49), encoder.decode_uint64(decryptor.decrypt(power)));

            encrypted = encryptor.encrypt(encoder.encode(-7));
            power = evaluator.exponentiate(encrypted, 3);
            Assert::AreEqual(static_cast<int32_t>(-343), encoder.decode_int32(decryptor.decrypt(power)));
        }

        TEST_METHOD(EncryptAddManyDecrypt)
        {
            EncryptionParameters parms;
            BigUInt &coeff_modulus = parms.coeff_modulus();
            BigUInt &plain_modulus = parms.plain_modulus();
            BigPoly &poly_modulus = parms.poly_modulus();
            parms.decomposition_bit_count() = 2;
            parms.noise_standard_deviation() = 3.19;
            parms.noise_max_deviation() = 35.06;
            coeff_modulus.resize(48);
            coeff_modulus = "FFFFFFFFC001";
            plain_modulus.resize(7);
            plain_modulus = 1 << 4;
            poly_modulus.resize(64, 1);
            poly_modulus[0] = 1;
            poly_modulus[63] = 1;

            KeyGenerator keygen(parms);
            keygen.generate();

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms, keygen.evaluation_keys());
            for (int i = 0; i < keygen.evaluation_keys().count(); ++i)
            {
                Assert::IsTrue(keygen.evaluation_keys()[i] == evaluator.evaluation_keys()[i]);
            }
            Decryptor decryptor(parms, keygen.secret_key());

            BigPoly encrypted1 = encryptor.encrypt(encoder.encode(5));
            BigPoly encrypted2 = encryptor.encrypt(encoder.encode(6));
            BigPoly encrypted3 = encryptor.encrypt(encoder.encode(7));
            vector<BigPoly> encrypteds = { encrypted1, encrypted2, encrypted3 };
            BigPoly product = evaluator.add_many(encrypteds);
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
            BigPoly encrypted4 = encryptor.encrypt(encoder.encode(-1));
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
            Assert::IsTrue(abs(frac_encoder.decode(decryptor.decrypt(product))-115.3626) < 0.000001);
        }

        TEST_METHOD(EvaluateTestMode)
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

            KeyGenerator keygen(parms);
            keygen.generate();

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms, keygen.evaluation_keys());
            Decryptor decryptor(parms, keygen.secret_key());

            BigPoly encrypted = encryptor.encrypt(encoder.encode(0x12345678));
            BigPoly negated = evaluator.negate(encrypted);
            Assert::AreEqual(static_cast<int32_t>(-0x12345678), encoder.decode_int32(negated));
            Assert::AreEqual(static_cast<int32_t>(-0x12345678), encoder.decode_int32(decryptor.decrypt(negated)));

            BigPoly plain2(64, 48);
            plain2 = encoder.encode(0x54321);
            BigPoly encrypted2 = encryptor.encrypt(plain2);
            BigPoly sum = evaluator.add(encrypted, encrypted2);
            Assert::AreEqual(static_cast<uint64_t>(0x12399999), encoder.decode_uint64(sum));
            Assert::AreEqual(static_cast<uint64_t>(0x12399999), encoder.decode_uint64(decryptor.decrypt(sum)));

            sum = evaluator.add_plain(encrypted, plain2);
            Assert::AreEqual(static_cast<uint64_t>(0x12399999), encoder.decode_uint64(sum));
            Assert::AreEqual(static_cast<uint64_t>(0x12399999), encoder.decode_uint64(decryptor.decrypt(sum)));

            BigPoly diff = evaluator.sub(encrypted, encrypted2);
            Assert::AreEqual(static_cast<int32_t>(0x122F1357), encoder.decode_int32(diff));
            Assert::AreEqual(static_cast<int32_t>(0x122F1357), encoder.decode_int32(decryptor.decrypt(diff)));

            diff = evaluator.sub_plain(encrypted, plain2);
            Assert::AreEqual(static_cast<int32_t>(0x122F1357), encoder.decode_int32(diff));
            Assert::AreEqual(static_cast<int32_t>(0x122F1357), encoder.decode_int32(decryptor.decrypt(diff)));

            BigPoly prod = evaluator.multiply(encrypted, encrypted2);
            Assert::AreEqual(static_cast<uint64_t>(0x5FCBBBB88D78), encoder.decode_uint64(prod));
            Assert::AreEqual(static_cast<uint64_t>(0x5FCBBBB88D78), encoder.decode_uint64(decryptor.decrypt(prod)));

            prod = evaluator.multiply_plain(encrypted, plain2);
            Assert::AreEqual(static_cast<uint64_t>(0x5FCBBBB88D78), encoder.decode_uint64(prod));
            Assert::AreEqual(static_cast<uint64_t>(0x5FCBBBB88D78), encoder.decode_uint64(decryptor.decrypt(prod)));
        }
    };
}