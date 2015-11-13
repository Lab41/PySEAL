#include "CppUnitTest.h"
#include "encryptionparams.h"
#include "encryptor.h"
#include "decryptor.h"
#include "keygenerator.h"
#include "encoder.h"
#include <cstdint>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(EncryptorTest)
    {
    public:
        TEST_METHOD(EncryptAddsNoise)
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

            BalancedEncoder encoder(plain_modulus);

            KeyGenerator keygen(parms);
            keygen.generate();

            Encryptor encryptor(parms, keygen.public_key());
            BigPoly encrypted1 = encryptor.encrypt(encoder.encode(0x12345678));
            BigPoly encrypted2 = encryptor.encrypt(encoder.encode(0x12345678));
            Assert::IsTrue(encrypted1 != encrypted2);

            Decryptor decryptor(parms, keygen.secret_key());
            Assert::AreEqual(static_cast<uint64_t>(0x12345678), encoder.decode_uint64(decryptor.decrypt(encrypted1)));
            Assert::AreEqual(static_cast<uint64_t>(0x12345678), encoder.decode_uint64(decryptor.decrypt(encrypted2)));
        }

        TEST_METHOD(EncryptDecrypt)
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
            Assert::IsTrue(keygen.public_key() == encryptor.public_key());
            Decryptor decryptor(parms, keygen.secret_key());
            Assert::IsTrue(keygen.secret_key() == decryptor.secret_key());

            BigPoly encrypted = encryptor.encrypt(encoder.encode(0x12345678));
            Assert::AreEqual(static_cast<uint64_t>(0x12345678), encoder.decode_uint64(decryptor.decrypt(encrypted)));

            encrypted = encryptor.encrypt(encoder.encode(0));
            Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(decryptor.decrypt(encrypted)));

            encrypted = encryptor.encrypt(encoder.encode(1));
            Assert::AreEqual(static_cast<uint64_t>(1), encoder.decode_uint64(decryptor.decrypt(encrypted)));

            encrypted = encryptor.encrypt(encoder.encode(2));
            Assert::AreEqual(static_cast<uint64_t>(2), encoder.decode_uint64(decryptor.decrypt(encrypted)));

            encrypted = encryptor.encrypt(encoder.encode(0x7FFFFFFFFFFFFFFD));
            Assert::AreEqual(static_cast<uint64_t>(0x7FFFFFFFFFFFFFFD), encoder.decode_uint64(decryptor.decrypt(encrypted)));

            encrypted = encryptor.encrypt(encoder.encode(0x7FFFFFFFFFFFFFFE));
            Assert::AreEqual(static_cast<uint64_t>(0x7FFFFFFFFFFFFFFE), encoder.decode_uint64(decryptor.decrypt(encrypted)));

            encrypted = encryptor.encrypt(encoder.encode(0x7FFFFFFFFFFFFFFF));
            Assert::AreEqual(static_cast<uint64_t>(0x7FFFFFFFFFFFFFFF), encoder.decode_uint64(decryptor.decrypt(encrypted)));
        }

        TEST_METHOD(EncryptDecryptTestMode)
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

            BalancedEncoder encoder(plain_modulus);

            KeyGenerator keygen(parms);
            keygen.generate();

            Encryptor encryptor(parms, keygen.public_key());
            Assert::IsTrue(keygen.public_key() == encryptor.public_key());
            Decryptor decryptor(parms, keygen.secret_key());
            Assert::IsTrue(keygen.secret_key() == decryptor.secret_key());

            BigPoly plain(64, 48);
            plain = encoder.encode(0x12345678);
            BigPoly encrypted = encryptor.encrypt(plain);
            Assert::IsTrue(encrypted == plain);
            Assert::AreEqual(static_cast<uint64_t>(0x12345678), encoder.decode_uint64(decryptor.decrypt(encrypted)));

            plain = encoder.encode(0);
            encrypted = encryptor.encrypt(plain);
            Assert::IsTrue(encrypted == plain);
            Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(decryptor.decrypt(encrypted)));

            plain = encoder.encode(1);
            encrypted = encryptor.encrypt(plain);
            Assert::IsTrue(encrypted == plain);
            Assert::AreEqual(static_cast<uint64_t>(1), encoder.decode_uint64(decryptor.decrypt(encrypted)));

            plain = encoder.encode(2);
            encrypted = encryptor.encrypt(plain);
            Assert::IsTrue(encrypted == plain);
            Assert::AreEqual(static_cast<uint64_t>(2), encoder.decode_uint64(decryptor.decrypt(encrypted)));

            plain = encoder.encode(0x7FFFFFFFFFFFFFFD);
            encrypted = encryptor.encrypt(plain);
            Assert::IsTrue(encrypted == plain);
            Assert::AreEqual(static_cast<uint64_t>(0x7FFFFFFFFFFFFFFD), encoder.decode_uint64(decryptor.decrypt(encrypted)));

            plain = encoder.encode(0x7FFFFFFFFFFFFFFE);
            encrypted = encryptor.encrypt(plain);
            Assert::IsTrue(encrypted == plain);
            Assert::AreEqual(static_cast<uint64_t>(0x7FFFFFFFFFFFFFFE), encoder.decode_uint64(decryptor.decrypt(encrypted)));

            plain = encoder.encode(0x7FFFFFFFFFFFFFFF);
            encrypted = encryptor.encrypt(plain);
            Assert::IsTrue(encrypted == plain);
            Assert::AreEqual(static_cast<uint64_t>(0x7FFFFFFFFFFFFFFF), encoder.decode_uint64(decryptor.decrypt(encrypted)));
        }
    };
}
