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
        TEST_METHOD(FVEncryptAddsNoise)
        {
            EncryptionParameters parms;
            BigUInt coeff_modulus;
            BigUInt plain_modulus;
            BigPoly poly_modulus; 
            {
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
                BalancedEncoder encoder(plain_modulus);

                KeyGenerator keygen(parms);
                keygen.generate();

                Encryptor encryptor(parms, keygen.public_key());

                Assert::IsTrue(encryptor.public_key()[0].to_string() == keygen.public_key()[0].to_string());
                Assert::IsTrue(encryptor.public_key()[1].to_string() == keygen.public_key()[1].to_string());

                BigPolyArray encrypted1 = encryptor.encrypt(encoder.encode(0x12345678));
                BigPolyArray encrypted2 = encryptor.encrypt(encoder.encode(0x12345678));
                Assert::IsTrue(encrypted1[0] != encrypted2[0]);
                Assert::IsTrue(encrypted1[1] != encrypted2[1]);

                Decryptor decryptor(parms, keygen.secret_key());
                Assert::AreEqual(static_cast<uint64_t>(0x12345678), encoder.decode_uint64(decryptor.decrypt(encrypted1)));
                Assert::AreEqual(static_cast<uint64_t>(0x12345678), encoder.decode_uint64(decryptor.decrypt(encrypted2)));
            }

            {
                // Testing that decryption is correct when the coeff_modulus has more than 1 uint64 count and 
                // the Nussbaumer path.
                parms.set_noise_standard_deviation(3.19);
                parms.set_noise_max_deviation(35.06);
                coeff_modulus = "FFFFFFFFFFFFFFFFFFFFFFFF";
                plain_modulus.resize(7);
                plain_modulus = 1 << 6;
                poly_modulus.resize(65, 1);
                poly_modulus[0] = 1;
                poly_modulus[64] = 1;
                parms.set_poly_modulus(const_cast<const BigPoly &>(poly_modulus));
                parms.set_plain_modulus(const_cast<const BigUInt &>(plain_modulus));
                parms.set_coeff_modulus(const_cast<const BigUInt &>(coeff_modulus));
                parms.validate();

                BalancedEncoder encoder(plain_modulus);

                KeyGenerator keygen(parms);
                keygen.generate();

                Encryptor encryptor(parms, keygen.public_key());

                auto qualifiers = parms.get_qualifiers();
                Assert::IsFalse(qualifiers.enable_ntt);
                Assert::IsTrue(qualifiers.enable_nussbaumer);
                Assert::IsTrue(encryptor.public_key()[0].to_string() == keygen.public_key()[0].to_string());
                Assert::IsTrue(encryptor.public_key()[1].to_string() == keygen.public_key()[1].to_string());

                BigPolyArray encrypted1 = encryptor.encrypt(encoder.encode(0x12345679));
                BigPolyArray encrypted2 = encryptor.encrypt(encoder.encode(0x12345679));
                Assert::IsTrue(encrypted1[0] != encrypted2[0]);
                Assert::IsTrue(encrypted1[1] != encrypted2[1]);

                Decryptor decryptor(parms, keygen.secret_key());

                Assert::AreEqual(static_cast<uint64_t>(0x12345679), encoder.decode_uint64(decryptor.decrypt(encrypted1)));
                Assert::AreEqual(static_cast<uint64_t>(0x12345679), encoder.decode_uint64(decryptor.decrypt(encrypted2)));
            }

            {
                // Testing that decryption is correct when the plain modulus has more than 1 uint64 count and 
                // the Nussbaumer path.
                parms.set_noise_standard_deviation(3.19);
                parms.set_noise_max_deviation(35.06);
                coeff_modulus = "FFFFFFFFFFFFFFFFFFFFFFFF";
                plain_modulus = "FFFFFFFFFFFFFFFFF";
                poly_modulus.resize(65, 1);
                poly_modulus[0] = 1;
                poly_modulus[64] = 1;
                parms.set_poly_modulus(const_cast<const BigPoly &>(poly_modulus));
                parms.set_plain_modulus(const_cast<const BigUInt &>(plain_modulus));
                parms.set_coeff_modulus(const_cast<const BigUInt &>(coeff_modulus));
                parms.validate();
                BalancedEncoder encoder(plain_modulus);

                KeyGenerator keygen(parms);
                keygen.generate();

                Encryptor encryptor(parms, keygen.public_key());

                auto qualifiers = parms.get_qualifiers();
                Assert::IsFalse(qualifiers.enable_ntt);
                Assert::IsTrue(qualifiers.enable_nussbaumer);
                Assert::IsTrue(encryptor.public_key()[0].to_string() == keygen.public_key()[0].to_string());
                Assert::IsTrue(encryptor.public_key()[1].to_string() == keygen.public_key()[1].to_string());

                BigPolyArray encrypted1 = encryptor.encrypt(encoder.encode(0x12345679));
                BigPolyArray encrypted2 = encryptor.encrypt(encoder.encode(0x12345679));
                Assert::IsTrue(encrypted1[0] != encrypted2[0]);
                Assert::IsTrue(encrypted1[1] != encrypted2[1]);

                Decryptor decryptor(parms, keygen.secret_key());

                Assert::AreEqual(static_cast<uint64_t>(0x12345679), encoder.decode_uint64(decryptor.decrypt(encrypted1)));
                Assert::AreEqual(static_cast<uint64_t>(0x12345679), encoder.decode_uint64(decryptor.decrypt(encrypted2)));
            }
        }

        TEST_METHOD(FVEncryptDecrypt)
        {
            EncryptionParameters parms;
            BigUInt coeff_modulus; 
            BigUInt plain_modulus;
            BigPoly poly_modulus;
            parms.set_decomposition_bit_count(4);
            parms.set_noise_standard_deviation(3.19);
            parms.set_noise_max_deviation(35.06);
            
            plain_modulus.resize(7);
            plain_modulus = 1 << 6;
            poly_modulus.resize(65, 1);
            poly_modulus[0] = 1;
            poly_modulus[64] = 1;
            parms.set_poly_modulus(const_cast<const BigPoly &>(poly_modulus));
            parms.set_plain_modulus(const_cast<const BigUInt &>(plain_modulus));
            {
                coeff_modulus.resize(48);
                coeff_modulus = "FFFFFFFFC001";
                parms.set_coeff_modulus(const_cast<const BigUInt &>(coeff_modulus));
                parms.validate();

                KeyGenerator keygen(parms);
                keygen.generate();

                BalancedEncoder encoder(plain_modulus);

                Encryptor encryptor(parms, keygen.public_key());
                Assert::IsTrue(keygen.public_key()[0] == encryptor.public_key()[0]);
                Assert::IsTrue(keygen.public_key()[1] == encryptor.public_key()[1]);

                Decryptor decryptor(parms, keygen.secret_key());
                Assert::IsTrue(keygen.secret_key() == decryptor.secret_key());

                BigPolyArray encrypted = encryptor.encrypt(encoder.encode(0x12345678));
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

                encrypted = encryptor.encrypt(encoder.encode(314159265));
                Assert::AreEqual(static_cast<uint64_t>(314159265), encoder.decode_uint64(decryptor.decrypt(encrypted)));
            }

            //Testing the Nussbaumer path.
            {
                coeff_modulus = "FFFFFFFFFFFFFFFF";
                parms.set_coeff_modulus(const_cast<const BigUInt &>(coeff_modulus));

                parms.validate();

                KeyGenerator keygen(parms);
                keygen.generate();

                BalancedEncoder encoder(plain_modulus);

                Encryptor encryptor(parms, keygen.public_key());
                Assert::IsTrue(keygen.public_key()[0] == encryptor.public_key()[0]);
                Assert::IsTrue(keygen.public_key()[1] == encryptor.public_key()[1]);

                Decryptor decryptor(parms, keygen.secret_key());
                Assert::IsTrue(keygen.secret_key() == decryptor.secret_key());
                
                auto qualifiers = parms.get_qualifiers();
                Assert::IsFalse(qualifiers.enable_ntt);
                Assert::IsTrue(qualifiers.enable_nussbaumer);

                BigPolyArray encrypted = encryptor.encrypt(encoder.encode(0x12345678));
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

                encrypted = encryptor.encrypt(encoder.encode(314159265));
                Assert::AreEqual(static_cast<uint64_t>(314159265), encoder.decode_uint64(decryptor.decrypt(encrypted)));
            }
        }
    };
}
