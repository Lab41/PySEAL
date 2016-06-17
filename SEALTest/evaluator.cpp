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
        TEST_METHOD(FVEncryptNegateDecrypt)
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

        TEST_METHOD(FVEncryptFFTMultiplyDecrypt)
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

        TEST_METHOD(FVEncryptMultiplyDecrypt)
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

        //TEST_METHOD(FVEncryptFFTMultiplyNoRelinDecrypt)
        //{
        //    EncryptionParameters parms;
        //    BigUInt &coeff_modulus = parms.coeff_modulus();
        //    BigUInt &plain_modulus = parms.plain_modulus();
        //    BigPoly &poly_modulus = parms.poly_modulus();
        //    parms.decomposition_bit_count() = 4;
        //    parms.noise_standard_deviation() = 3.19;
        //    parms.noise_max_deviation() = 35.06;
        //    coeff_modulus.resize(48);
        //    coeff_modulus = "FFFFFFFFC001";
        //    plain_modulus.resize(7);
        //    plain_modulus = 1 << 6;
        //    poly_modulus.resize(65, 1);
        //    poly_modulus[0] = 1;
        //    poly_modulus[64] = 1;

        //    KeyGenerator keygen(parms);
        //    keygen.generate();

        //    BalancedEncoder encoder(plain_modulus);

        //    Encryptor encryptor(parms, keygen.public_key());
        //    Evaluator evaluator(parms, keygen.evaluation_keys());

        //    EvaluationKeys evk = keygen.evaluation_keys();
        //    int evk_size = static_cast<int>(evk.size());
        //    for (int i = 0; i < evk_size; ++i)
        //    {
        //        for (int j = 0; j < evk[0].first.size(); ++j)
        //        {
        //            Assert::IsTrue(keygen.evaluation_keys()[i].first[j] == evaluator.evaluation_keys()[i].first[j]);
        //            Assert::IsTrue(keygen.evaluation_keys()[i].second[j] == evaluator.evaluation_keys()[i].second[j]);
        //        }
        //    }

        //    Decryptor decryptor(parms, keygen.secret_key());

        //    BigPolyArray encrypted1 = encryptor.encrypt(encoder.encode(0x12345678));
        //    BigPolyArray encrypted2 = encryptor.encrypt(encoder.encode(0x54321));
        //    BigPolyArray product = evaluator.multiply_norelin(encrypted1, encrypted2);
        //    Assert::AreEqual(static_cast<uint64_t>(0x5FCBBBB88D78), encoder.decode_uint64(decryptor.decrypt(product)));

        //    encrypted1 = encryptor.encrypt(encoder.encode(0));
        //    encrypted2 = encryptor.encrypt(encoder.encode(0));
        //    product = evaluator.multiply_norelin(encrypted1, encrypted2);
        //    Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(decryptor.decrypt(product)));

        //    encrypted1 = encryptor.encrypt(encoder.encode(0));
        //    encrypted2 = encryptor.encrypt(encoder.encode(5));
        //    product = evaluator.multiply_norelin(encrypted1, encrypted2);
        //    Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(decryptor.decrypt(product)));

        //    encrypted1 = encryptor.encrypt(encoder.encode(7));
        //    encrypted2 = encryptor.encrypt(encoder.encode(1));
        //    product = evaluator.multiply_norelin(encrypted1, encrypted2);
        //    Assert::AreEqual(static_cast<uint64_t>(7), encoder.decode_uint64(decryptor.decrypt(product)));

        //    encrypted1 = encryptor.encrypt(encoder.encode(5));
        //    encrypted2 = encryptor.encrypt(encoder.encode(-3));
        //    product = evaluator.multiply_norelin(encrypted1, encrypted2);
        //    Assert::AreEqual(static_cast<int32_t>(-15), encoder.decode_int32(decryptor.decrypt(product)));

        //    encrypted1 = encryptor.encrypt(encoder.encode(-7));
        //    encrypted2 = encryptor.encrypt(encoder.encode(2));
        //    product = evaluator.multiply_norelin(encrypted1, encrypted2);
        //    Assert::AreEqual(static_cast<int32_t>(-14), encoder.decode_int32(decryptor.decrypt(product)));

        //    BigPolyArray encrypted3 = encryptor.encrypt(encoder.encode(0));
        //    BigPolyArray encrypted4 = evaluator.multiply_norelin(encrypted3, product);
        //    Assert::IsTrue(encrypted4.size() == 4);
        //    Assert::AreEqual(static_cast<int32_t>(0), encoder.decode_int32(decryptor.decrypt(encrypted4)));
        //}

        TEST_METHOD(FVEncryptMultiplyManyDecrypt)
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
            keygen.generate(3);

            BalancedEncoder encoder(plain_modulus);

            Encryptor encryptor(parms, keygen.public_key());
            Evaluator evaluator(parms, keygen.evaluation_keys());

            EvaluationKeys evk = keygen.evaluation_keys();
            int evk_size = static_cast<int>(evk.size());
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

        //TEST_METHOD(FVEncryptMultiplyNoRelinManyDecrypt)
        //{
        //    EncryptionParameters parms;
        //    BigUInt &coeff_modulus = parms.coeff_modulus();
        //    BigUInt &plain_modulus = parms.plain_modulus();
        //    BigPoly &poly_modulus = parms.poly_modulus();
        //    parms.decomposition_bit_count() = 4;
        //    parms.noise_standard_deviation() = 3.19;
        //    parms.noise_max_deviation() = 35.06;
        //    coeff_modulus.resize(48);
        //    coeff_modulus = "FFFFFFFFC001";
        //    plain_modulus.resize(7);
        //    plain_modulus = 1 << 6;
        //    poly_modulus.resize(65, 1);
        //    poly_modulus[0] = 1;
        //    poly_modulus[64] = 1;

        //    KeyGenerator keygen(parms);
        //    keygen.generate(0);

        //    BalancedEncoder encoder(plain_modulus);

        //    Encryptor encryptor(parms, keygen.public_key());
        //    Evaluator evaluator(parms, keygen.evaluation_keys());
        //    Decryptor decryptor(parms, keygen.secret_key());

        //    BigPolyArray encrypted1 = encryptor.encrypt(encoder.encode(5));
        //    BigPolyArray encrypted2 = encryptor.encrypt(encoder.encode(6));
        //    BigPolyArray encrypted3 = encryptor.encrypt(encoder.encode(7));
        //    vector<BigPolyArray> encrypteds = { encrypted1, encrypted2, encrypted3 };
        //    BigPolyArray product = evaluator.multiply_norelin_many(encrypteds);
        //    Assert::AreEqual(static_cast<uint64_t>(210), encoder.decode_uint64(decryptor.decrypt(product)));

        //    encrypted1 = encryptor.encrypt(encoder.encode(-9));
        //    encrypted2 = encryptor.encrypt(encoder.encode(-17));
        //    encrypteds = { encrypted1, encrypted2 };
        //    product = evaluator.multiply_norelin_many(encrypteds);
        //    Assert::AreEqual(static_cast<uint64_t>(153), encoder.decode_uint64(decryptor.decrypt(product)));

        //    encrypted1 = encryptor.encrypt(encoder.encode(2));
        //    encrypted2 = encryptor.encrypt(encoder.encode(-31));
        //    encrypted3 = encryptor.encrypt(encoder.encode(7));
        //    encrypteds = { encrypted1, encrypted2, encrypted3 };
        //    product = evaluator.multiply_norelin_many(encrypteds);
        //    Assert::AreEqual(static_cast<int32_t>(-434), encoder.decode_int32(decryptor.decrypt(product)));

        //    encrypted1 = encryptor.encrypt(encoder.encode(1));
        //    encrypted2 = encryptor.encrypt(encoder.encode(-1));
        //    encrypted3 = encryptor.encrypt(encoder.encode(1));
        //    BigPolyArray encrypted4 = encryptor.encrypt(encoder.encode(-1));
        //    encrypteds = { encrypted1, encrypted2, encrypted3, encrypted4 };
        //    product = evaluator.multiply_norelin_many(encrypteds);
        //    Assert::AreEqual(static_cast<int32_t>(1), encoder.decode_int32(decryptor.decrypt(product)));

        //    encrypted1 = encryptor.encrypt(encoder.encode(98765));
        //    encrypted2 = encryptor.encrypt(encoder.encode(0));
        //    encrypted3 = encryptor.encrypt(encoder.encode(12345));
        //    encrypted4 = encryptor.encrypt(encoder.encode(34567));
        //    encrypteds = { encrypted1, encrypted2, encrypted3, encrypted4 };
        //    product = evaluator.multiply_norelin_many(encrypteds);
        //    Assert::AreEqual(static_cast<int32_t>(0), encoder.decode_int32(decryptor.decrypt(product)));
        //}

        TEST_METHOD(FVEncryptExponentiateDecrypt)
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
            Evaluator evaluator(parms);
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


        //TEST_METHOD(FVEncryptExponentiateNoRelinDecrypt)
        //{
        //    EncryptionParameters parms;
        //    BigUInt &coeff_modulus = parms.coeff_modulus();
        //    BigUInt &plain_modulus = parms.plain_modulus();
        //    BigPoly &poly_modulus = parms.poly_modulus();
        //    parms.decomposition_bit_count() = 2;
        //    parms.noise_standard_deviation() = 3.19;
        //    parms.noise_max_deviation() = 35.06;
        //    coeff_modulus.resize(48);
        //    coeff_modulus = "FFFFFFFFC001";
        //    plain_modulus.resize(7);
        //    plain_modulus = 1 << 4;
        //    poly_modulus.resize(64, 1);
        //    poly_modulus[0] = 1;
        //    poly_modulus[63] = 1;

        //    KeyGenerator keygen(parms);
        //    keygen.generate();

        //    BalancedEncoder encoder(plain_modulus);

        //    Encryptor encryptor(parms, keygen.public_key());
        //    Evaluator evaluator(parms, keygen.evaluation_keys());

        //    EvaluationKeys evk = keygen.evaluation_keys();
        //    int evk_size = static_cast<int>(evk.size());
        //    for (int i = 0; i < evk_size; ++i)
        //    {
        //        for (int j = 0; j < evk[0].first.size(); ++j)
        //        {
        //            Assert::IsTrue(keygen.evaluation_keys()[i].first[j] == evaluator.evaluation_keys()[i].first[j]);
        //            Assert::IsTrue(keygen.evaluation_keys()[i].second[j] == evaluator.evaluation_keys()[i].second[j]);
        //        }
        //    }

        //    Decryptor decryptor(parms, keygen.secret_key());

        //    BigPolyArray encrypted = encryptor.encrypt(encoder.encode(5));
        //    BigPolyArray power = evaluator.exponentiate_norelin(encrypted, 1);
        //    Assert::AreEqual(static_cast<uint64_t>(5), encoder.decode_uint64(decryptor.decrypt(power)));

        //    encrypted = encryptor.encrypt(encoder.encode(7));
        //    power = evaluator.exponentiate_norelin(encrypted, 2);
        //    Assert::AreEqual(static_cast<uint64_t>(49), encoder.decode_uint64(decryptor.decrypt(power)));

        //    encrypted = encryptor.encrypt(encoder.encode(-7));
        //    power = evaluator.exponentiate_norelin(encrypted, 3);
        //    Assert::AreEqual(static_cast<int32_t>(-343), encoder.decode_int32(decryptor.decrypt(power)));
        //}


        TEST_METHOD(FVEncryptAddManyDecrypt)
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


        //TEST_METHOD(FVEncryptMultNoRelinRelinearizeDecrypt)
        //{
        //    EncryptionParameters parms;
        //    BigUInt &coeff_modulus = parms.coeff_modulus();
        //    BigUInt &plain_modulus = parms.plain_modulus();
        //    BigPoly &poly_modulus = parms.poly_modulus();
        //    parms.decomposition_bit_count() = 2;
        //    parms.noise_standard_deviation() = 3.19;
        //    parms.noise_max_deviation() = 35.06;
        //    coeff_modulus.resize(48);
        //    coeff_modulus = "FFFFFFFFC001";
        //    plain_modulus.resize(7);
        //    plain_modulus = 1 << 4;
        //    poly_modulus.resize(65, 1);
        //    poly_modulus[0] = 1;
        //    poly_modulus[64] = 1;

        //    KeyGenerator keygen(parms);

        //    // Remember to generate enough evaluation keys
        //    keygen.generate(3);

        //    BalancedEncoder encoder(plain_modulus);

        //    Encryptor encryptor(parms, keygen.public_key());
        //    Evaluator evaluator(parms, keygen.evaluation_keys());

        //    EvaluationKeys evk = keygen.evaluation_keys();
        //    int evk_size = static_cast<int>(evk.size());
        //    for (int i = 0; i < evk_size; ++i)
        //    {
        //        for (int j = 0; j < evk[0].first.size(); ++j)
        //        {
        //            Assert::IsTrue(keygen.evaluation_keys()[i].first[j] == evaluator.evaluation_keys()[i].first[j]);
        //            Assert::IsTrue(keygen.evaluation_keys()[i].second[j] == evaluator.evaluation_keys()[i].second[j]);
        //        }
        //    }

        //    Decryptor decryptor(parms, keygen.secret_key());

        //    BigPolyArray encrypted1 = encryptor.encrypt(encoder.encode(98765));
        //    BigPolyArray encrypted2 = encryptor.encrypt(encoder.encode(0));
        //    BigPolyArray encrypted3 = encryptor.encrypt(encoder.encode(12345));
        //    BigPolyArray encrypted4 = encryptor.encrypt(encoder.encode(34567));

        //    BigPolyArray encrypted5 = evaluator.multiply_norelin(encrypted1, encrypted2);
        //    BigPolyArray encrypted6 = evaluator.multiply_norelin(encrypted3, encrypted4);

        //    BigPolyArray encrypted7 = evaluator.multiply_norelin(encrypted5, encrypted6);

        //    // Check that multiply worked properly and the current ciphertext still decrypts successfully
        //    int expected_count = 5;
        //    Assert::AreEqual(expected_count, encrypted7.size());
        //    Assert::AreEqual(encoder.decode_uint64(decryptor.decrypt(encrypted7)), static_cast<uint64_t>(0));

        //    // Check each possible final count that relinearization gives the correct size and decrypts correctly
        //    BigPolyArray encrypted8 = evaluator.relinearize(encrypted7, 4);
        //    expected_count = 4;
        //    Assert::AreEqual(expected_count, encrypted8.size());
        //    Assert::AreEqual(encoder.decode_uint64(decryptor.decrypt(encrypted8)), static_cast<uint64_t>(0));
        //    BigPolyArray encrypted9 = evaluator.relinearize(encrypted7, 3);
        //    expected_count = 3;
        //    Assert::AreEqual(expected_count, encrypted9.size());
        //    Assert::AreEqual(encoder.decode_uint64(decryptor.decrypt(encrypted9)), static_cast<uint64_t>(0));
        //    BigPolyArray encrypted10 = evaluator.relinearize(encrypted7, 2);
        //    expected_count = 2;
        //    Assert::AreEqual(expected_count, encrypted10.size());
        //    Assert::AreEqual(encoder.decode_uint64(decryptor.decrypt(encrypted10)), static_cast<uint64_t>(0));

        //    // Check that an intermediate ciphertext can also be relinearized later fully and retains correct decryption
        //    BigPolyArray encrypted11 = evaluator.relinearize(encrypted8, 2);
        //    expected_count = 2;
        //    Assert::AreEqual(expected_count, encrypted11.size());
        //    Assert::AreEqual(encoder.decode_uint64(decryptor.decrypt(encrypted11)), static_cast<uint64_t>(0));

        //    //Check a nonzero example
        //    BigPoly plain1 = "1x^1 + 1";
        //    BigPoly plain2 = "2x^2 + 2";
        //    BigPoly plain3 = "3";
        //    encrypted1 = encryptor.encrypt(plain1);
        //    encrypted2 = encryptor.encrypt(plain2);
        //    encrypted3 = encryptor.encrypt(plain3);

        //    //do a mix of multiply and multiply no relin
        //    encrypted4 = evaluator.multiply(encrypted1, encrypted2);
        //    encrypted5 = evaluator.multiply_norelin(encrypted4, encrypted3);

        //    //check it decrypts correctly
        //    BigPoly plain5 = decryptor.decrypt(encrypted5);
        //    Assert::IsTrue(plain5.to_string() == "6x^3 + 6x^2 + 6x^1 + 6");
        //    expected_count = 3;
        //    Assert::AreEqual(expected_count, encrypted5.size());

        //    // quick test of 'relinearizing' with no expected effect
        //    BigPolyArray norelinencrypted5 = evaluator.relinearize(encrypted5, 3);
        //    expected_count = 3;
        //    Assert::AreEqual(expected_count, norelinencrypted5.size());
        //    BigPoly norelinplain5 = decryptor.decrypt(norelinencrypted5);
        //    Assert::IsTrue(norelinplain5.to_string() == "6x^3 + 6x^2 + 6x^1 + 6");

        //    // now relinearize fully
        //    encrypted6 = evaluator.relinearize(encrypted5);
        //    expected_count = 2;
        //    Assert::AreEqual(expected_count, encrypted6.size());
        //    BigPoly plain6 = decryptor.decrypt(encrypted6);
        //    Assert::IsTrue(plain6.to_string() == "6x^3 + 6x^2 + 6x^1 + 6");

        //    //test how to handle already relinearized ciphertexts
        //    BigPolyArray encrypted12 = evaluator.relinearize(encrypted6);
        //    BigPoly plain12 = decryptor.decrypt(encrypted6);            
        //    expected_count = 2;
        //    Assert::AreEqual(expected_count, encrypted6.size());
        //    Assert::IsTrue(plain12.to_string() == "6x^3 + 6x^2 + 6x^1 + 6");
        //    BigPoly plain4 = "0";
        //    BigPolyArray encplain4 = encryptor.encrypt(plain4);
        //    BigPolyArray relinencplain4 = evaluator.relinearize(encplain4);
        //    expected_count = 2;
        //    Assert::AreEqual(expected_count, relinencplain4.size());
        //    BigPoly plain4candidate = decryptor.decrypt(relinencplain4);
        //    Assert::IsTrue(plain4candidate.to_string() == "0");
        //    BigPoly plain7 = "2x^11";
        //    BigPolyArray encplain7 = encryptor.encrypt(plain7);
        //    BigPolyArray relinencplain7 = evaluator.relinearize(encplain7);
        //    expected_count = 2;
        //    Assert::AreEqual(expected_count, relinencplain7.size());
        //    BigPoly plain7candidate = decryptor.decrypt(relinencplain7);
        //    Assert::IsTrue(plain7candidate.to_string() == "2x^11");
        //}
    };
}