#include "CppUnitTest.h"
#include "seal/context.h"
#include "seal/encryptor.h"
#include "seal/decryptor.h"
#include "seal/evaluator.h"
#include "seal/keygenerator.h"
#include "seal/polycrt.h"
#include "seal/encoder.h"
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
            SmallModulus plain_modulus(1 << 6);
            parms.set_poly_modulus("1x^64 + 1");
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus({ small_mods_40bit(0) });
            SEALContext context(parms);
            KeyGenerator keygen(context);

            BalancedEncoder encoder(plain_modulus);
            Encryptor encryptor(context, keygen.public_key());
            Evaluator evaluator(context);
            Decryptor decryptor(context, keygen.secret_key());

            Ciphertext encrypted;
            encryptor.encrypt(encoder.encode(0x12345678), encrypted);
            evaluator.negate(encrypted);
            Plaintext plain;
            decryptor.decrypt(encrypted, plain);
            Assert::AreEqual(static_cast<int32_t>(-0x12345678), encoder.decode_int32(plain));
            Assert::IsTrue(encrypted.hash_block() == encrypted.hash_block());
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(0), encrypted);
            evaluator.negate(encrypted);
            decryptor.decrypt(encrypted, plain);
            Assert::AreEqual(static_cast<int32_t>(0), encoder.decode_int32(plain));
            Assert::IsTrue(encrypted.hash_block() == encrypted.hash_block());
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(1), encrypted);
            evaluator.negate(encrypted);
            decryptor.decrypt(encrypted, plain);
            Assert::AreEqual(static_cast<int32_t>(-1), encoder.decode_int32(plain));
            Assert::IsTrue(encrypted.hash_block() == encrypted.hash_block());
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(-1), encrypted);
            evaluator.negate(encrypted);
            decryptor.decrypt(encrypted, plain);
            Assert::AreEqual(static_cast<int32_t>(1), encoder.decode_int32(plain));
            Assert::IsTrue(encrypted.hash_block() == encrypted.hash_block());
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(2), encrypted);
            evaluator.negate(encrypted);
            decryptor.decrypt(encrypted, plain);
            Assert::AreEqual(static_cast<int32_t>(-2), encoder.decode_int32(plain));
            Assert::IsTrue(encrypted.hash_block() == encrypted.hash_block());
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(-5), encrypted);
            evaluator.negate(encrypted);
            decryptor.decrypt(encrypted, plain);
            Assert::AreEqual(static_cast<int32_t>(5), encoder.decode_int32(plain));
            Assert::IsTrue(encrypted.hash_block() == encrypted.hash_block());
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());
        }

        TEST_METHOD(FVEncryptAddDecrypt)
        {
            EncryptionParameters parms;
            SmallModulus plain_modulus(1 << 6);
            parms.set_poly_modulus("1x^64 + 1");
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus({ small_mods_40bit(0) });
            SEALContext context(parms);
            KeyGenerator keygen(context);

            BalancedEncoder encoder(plain_modulus);
            Encryptor encryptor(context, keygen.public_key());
            Evaluator evaluator(context);
            Decryptor decryptor(context, keygen.secret_key());

            Ciphertext encrypted1;
            encryptor.encrypt(encoder.encode(0x12345678), encrypted1);
            Ciphertext encrypted2;
            encryptor.encrypt(encoder.encode(0x54321), encrypted2);
            evaluator.add(encrypted1, encrypted2);
            Plaintext plain;
            decryptor.decrypt(encrypted1, plain);
            Assert::AreEqual(static_cast<uint64_t>(0x12399999), encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
            Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(0), encrypted1);
            encryptor.encrypt(encoder.encode(0), encrypted2);
            evaluator.add(encrypted1, encrypted2);
            decryptor.decrypt(encrypted1, plain);
            Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
            Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(0), encrypted1);
            encryptor.encrypt(encoder.encode(5), encrypted2);
            evaluator.add(encrypted1, encrypted2);
            decryptor.decrypt(encrypted1, plain);
            Assert::AreEqual(static_cast<uint64_t>(5), encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
            Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(5), encrypted1);
            encryptor.encrypt(encoder.encode(-3), encrypted2);
            evaluator.add(encrypted1, encrypted2);
            decryptor.decrypt(encrypted1, plain);
            Assert::AreEqual(static_cast<int32_t>(2), encoder.decode_int32(plain));
            Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
            Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(-7), encrypted1);
            encryptor.encrypt(encoder.encode(2), encrypted2);
            evaluator.add(encrypted1, encrypted2);
            decryptor.decrypt(encrypted1, plain);
            Assert::AreEqual(static_cast<int32_t>(-5), encoder.decode_int32(plain));
            Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
            Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

            Plaintext plain1 = "2x^2 + 1x^1 + 3";
            Plaintext plain2 = "3x^3 + 4x^2 + 5x^1 + 6";
            encryptor.encrypt(plain1, encrypted1);
            encryptor.encrypt(plain2, encrypted2);
            evaluator.add(encrypted1, encrypted2);
            decryptor.decrypt(encrypted1, plain);
            Assert::IsTrue(plain.to_string() == "3x^3 + 6x^2 + 6x^1 + 9");
            Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
            Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

            plain1 = "3x^5 + 1x^4 + 4x^3 + 1";
            plain2 = "5x^2 + 9x^1 + 2";
            encryptor.encrypt(plain1, encrypted1);
            encryptor.encrypt(plain2, encrypted2);
            evaluator.add(encrypted1, encrypted2);
            decryptor.decrypt(encrypted1, plain);
            Assert::IsTrue(plain.to_string() == "3x^5 + 1x^4 + 4x^3 + 5x^2 + 9x^1 + 3");
            Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
            Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());
        }

        TEST_METHOD(FVEncryptSubDecrypt)
        {
            EncryptionParameters parms;
            SmallModulus plain_modulus(1 << 6);
            parms.set_poly_modulus("1x^64 + 1");
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus({ small_mods_40bit(0) });
            SEALContext context(parms);
            KeyGenerator keygen(context);

            BalancedEncoder encoder(plain_modulus);
            Encryptor encryptor(context, keygen.public_key());
            Evaluator evaluator(context);
            Decryptor decryptor(context, keygen.secret_key());

            Ciphertext encrypted1;
            encryptor.encrypt(encoder.encode(0x12345678), encrypted1);
            Ciphertext encrypted2;
            encryptor.encrypt(encoder.encode(0x54321), encrypted2);
            evaluator.sub(encrypted1, encrypted2);
            Plaintext plain;
            decryptor.decrypt(encrypted1, plain);
            Assert::AreEqual(static_cast<int32_t>(0x122F1357), encoder.decode_int32(plain));
            Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
            Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(0), encrypted1);
            encryptor.encrypt(encoder.encode(0), encrypted2);
            evaluator.sub(encrypted1, encrypted2);
            decryptor.decrypt(encrypted1, plain);
            Assert::AreEqual(static_cast<int32_t>(0), encoder.decode_int32(plain));
            Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
            Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(0), encrypted1);
            encryptor.encrypt(encoder.encode(5), encrypted2);
            evaluator.sub(encrypted1, encrypted2);
            decryptor.decrypt(encrypted1, plain);
            Assert::AreEqual(static_cast<int32_t>(-5), encoder.decode_int32(plain));
            Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
            Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(5), encrypted1);
            encryptor.encrypt(encoder.encode(-3), encrypted2);
            evaluator.sub(encrypted1, encrypted2);
            decryptor.decrypt(encrypted1, plain);
            Assert::AreEqual(static_cast<int32_t>(8), encoder.decode_int32(plain));
            Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
            Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(-7), encrypted1);
            encryptor.encrypt(encoder.encode(2), encrypted2);
            evaluator.sub(encrypted1, encrypted2);
            decryptor.decrypt(encrypted1, plain);
            Assert::AreEqual(static_cast<int32_t>(-9), encoder.decode_int32(plain));
            Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
            Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());
        }

        TEST_METHOD(FVEncryptAddPlainDecrypt)
        {
            EncryptionParameters parms;
            SmallModulus plain_modulus(1 << 6);
            parms.set_poly_modulus("1x^64 + 1");
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus({ small_mods_40bit(0) });
            SEALContext context(parms);
            KeyGenerator keygen(context);

            BalancedEncoder encoder(plain_modulus);
            Encryptor encryptor(context, keygen.public_key());
            Evaluator evaluator(context);
            Decryptor decryptor(context, keygen.secret_key());

            Ciphertext encrypted1;
            Ciphertext encrypted2;
            Plaintext plain;
            encryptor.encrypt(encoder.encode(0x12345678), encrypted1);
            plain = encoder.encode(0x54321);
            evaluator.add_plain(encrypted1, plain);
            decryptor.decrypt(encrypted1, plain);
            Assert::AreEqual(static_cast<uint64_t>(0x12399999), encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(0), encrypted1);
            plain = encoder.encode(0);
            evaluator.add_plain(encrypted1, plain);
            decryptor.decrypt(encrypted1, plain);
            Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(0), encrypted1);
            plain = encoder.encode(5);
            evaluator.add_plain(encrypted1, plain);
            decryptor.decrypt(encrypted1, plain);
            Assert::AreEqual(static_cast<uint64_t>(5), encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(5), encrypted1);
            plain = encoder.encode(-3);
            evaluator.add_plain(encrypted1, plain);
            decryptor.decrypt(encrypted1, plain);
            Assert::AreEqual(static_cast<uint64_t>(2), encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(-7), encrypted1);
            plain = encoder.encode(7);
            evaluator.add_plain(encrypted1, plain);
            decryptor.decrypt(encrypted1, plain);
            Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());
        }

        TEST_METHOD(FVEncryptSubPlainDecrypt)
        {
            EncryptionParameters parms;
            SmallModulus plain_modulus(1 << 6);
            parms.set_poly_modulus("1x^64 + 1");
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus({ small_mods_40bit(0) });
            SEALContext context(parms);
            KeyGenerator keygen(context);

            BalancedEncoder encoder(plain_modulus);
            Encryptor encryptor(context, keygen.public_key());
            Evaluator evaluator(context);
            Decryptor decryptor(context, keygen.secret_key());

            Ciphertext encrypted1;
            Plaintext plain;
            encryptor.encrypt(encoder.encode(0x12345678), encrypted1);
            plain = encoder.encode(0x54321);
            evaluator.sub_plain(encrypted1, plain);
            decryptor.decrypt(encrypted1, plain);
            Assert::AreEqual(static_cast<uint64_t>(0x122F1357), encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(0), encrypted1);
            plain = encoder.encode(0);
            evaluator.sub_plain(encrypted1, plain);
            decryptor.decrypt(encrypted1, plain);
            Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(0), encrypted1);
            plain = encoder.encode(5);
            evaluator.sub_plain(encrypted1, plain);
            decryptor.decrypt(encrypted1, plain);
            Assert::IsTrue(static_cast<int64_t>(-5) == encoder.decode_int64(plain));
            Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(5), encrypted1);
            plain = encoder.encode(-3);
            evaluator.sub_plain(encrypted1, plain);
            decryptor.decrypt(encrypted1, plain);
            Assert::AreEqual(static_cast<uint64_t>(8), encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(-7), encrypted1);
            plain = encoder.encode(2);
            evaluator.sub_plain(encrypted1, plain);
            decryptor.decrypt(encrypted1, plain);
            Assert::IsTrue(static_cast<int64_t>(-9) == encoder.decode_int64(plain));
            Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());
        }

        TEST_METHOD(FVEncryptMultiplyPlainDecrypt)
        {
            EncryptionParameters parms;
            SmallModulus plain_modulus(1 << 6);
            parms.set_poly_modulus("1x^64 + 1");
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus({ small_mods_40bit(0) });
            SEALContext context(parms);
            KeyGenerator keygen(context);

            BalancedEncoder encoder(plain_modulus);
            Encryptor encryptor(context, keygen.public_key());
            Evaluator evaluator(context);
            Decryptor decryptor(context, keygen.secret_key());

            Ciphertext encrypted;
            Plaintext plain;
            encryptor.encrypt(encoder.encode(0x12345678), encrypted);
            plain = encoder.encode(0x54321);
            evaluator.multiply_plain(encrypted, plain);
            decryptor.decrypt(encrypted, plain);
            Assert::AreEqual(static_cast<uint64_t>(0x5FCBBBB88D78), encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(0), encrypted);
            plain = encoder.encode(5);
            evaluator.multiply_plain(encrypted, plain);
            decryptor.decrypt(encrypted, plain);
            Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(7), encrypted);
            plain = encoder.encode(1);
            evaluator.multiply_plain(encrypted, plain);
            decryptor.decrypt(encrypted, plain);
            Assert::AreEqual(static_cast<uint64_t>(7), encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(5), encrypted);
            plain = encoder.encode(-3);
            evaluator.multiply_plain(encrypted, plain);
            decryptor.decrypt(encrypted, plain);
            Assert::IsTrue(static_cast<int64_t>(-15) == encoder.decode_int64(plain));
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(-7), encrypted);
            plain = encoder.encode(2);
            evaluator.multiply_plain(encrypted, plain);
            decryptor.decrypt(encrypted, plain);
            Assert::IsTrue(static_cast<int64_t>(-14) == encoder.decode_int64(plain));
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());
        }

        TEST_METHOD(FVEncryptMultiplyDecrypt)
        {
            {
                EncryptionParameters parms;
                SmallModulus plain_modulus(1 << 6);
                parms.set_poly_modulus("1x^64 + 1");
                parms.set_plain_modulus(plain_modulus);
                parms.set_coeff_modulus({ small_mods_40bit(0) });
                SEALContext context(parms);
                KeyGenerator keygen(context);

                BalancedEncoder encoder(plain_modulus);
                Encryptor encryptor(context, keygen.public_key());
                Evaluator evaluator(context);
                Decryptor decryptor(context, keygen.secret_key());

                Ciphertext encrypted1;
                Ciphertext encrypted2;
                Plaintext plain;
                encryptor.encrypt(encoder.encode(0x12345678), encrypted1);
                encryptor.encrypt(encoder.encode(0x54321), encrypted2);
                evaluator.multiply(encrypted1, encrypted2);
                decryptor.decrypt(encrypted1, plain);
                Assert::AreEqual(static_cast<uint64_t>(0x5FCBBBB88D78), encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
                Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(0), encrypted1);
                encryptor.encrypt(encoder.encode(0), encrypted2);
                evaluator.multiply(encrypted1, encrypted2);
                decryptor.decrypt(encrypted1, plain);
                Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
                Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(0), encrypted1);
                encryptor.encrypt(encoder.encode(5), encrypted2);
                evaluator.multiply(encrypted1, encrypted2);
                decryptor.decrypt(encrypted1, plain);
                Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
                Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(7), encrypted1);
                encryptor.encrypt(encoder.encode(1), encrypted2);
                evaluator.multiply(encrypted1, encrypted2);
                decryptor.decrypt(encrypted1, plain);
                Assert::AreEqual(static_cast<uint64_t>(7), encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
                Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(5), encrypted1);
                encryptor.encrypt(encoder.encode(-3), encrypted2);
                evaluator.multiply(encrypted1, encrypted2);
                decryptor.decrypt(encrypted1, plain);
                Assert::IsTrue(static_cast<int64_t>(-15) == encoder.decode_int64(plain));
                Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
                Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(0x10000), encrypted1);
                encryptor.encrypt(encoder.encode(0x100), encrypted2);
                evaluator.multiply(encrypted1, encrypted2);
                decryptor.decrypt(encrypted1, plain);
                Assert::AreEqual(static_cast<uint64_t>(0x1000000), encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
                Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());
            }
            {
                EncryptionParameters parms;
                SmallModulus plain_modulus(1 << 6);
                parms.set_poly_modulus("1x^128 + 1");
                parms.set_plain_modulus(plain_modulus);
                parms.set_coeff_modulus({ small_mods_40bit(0), small_mods_40bit(1) });
                SEALContext context(parms);
                KeyGenerator keygen(context);

                BalancedEncoder encoder(plain_modulus);
                Encryptor encryptor(context, keygen.public_key());
                Evaluator evaluator(context);
                Decryptor decryptor(context, keygen.secret_key());

                Ciphertext encrypted1;
                Ciphertext encrypted2;
                Plaintext plain;
                encryptor.encrypt(encoder.encode(0x12345678), encrypted1);
                encryptor.encrypt(encoder.encode(0x54321), encrypted2);
                evaluator.multiply(encrypted1, encrypted2);
                decryptor.decrypt(encrypted1, plain);
                Assert::AreEqual(static_cast<uint64_t>(0x5FCBBBB88D78), encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
                Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(0), encrypted1);
                encryptor.encrypt(encoder.encode(0), encrypted2);
                evaluator.multiply(encrypted1, encrypted2);
                decryptor.decrypt(encrypted1, plain);
                Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
                Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(0), encrypted1);
                encryptor.encrypt(encoder.encode(5), encrypted2);
                evaluator.multiply(encrypted1, encrypted2);
                decryptor.decrypt(encrypted1, plain);
                Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
                Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(7), encrypted1);
                encryptor.encrypt(encoder.encode(1), encrypted2);
                evaluator.multiply(encrypted1, encrypted2);
                decryptor.decrypt(encrypted1, plain);
                Assert::AreEqual(static_cast<uint64_t>(7), encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
                Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(5), encrypted1);
                encryptor.encrypt(encoder.encode(-3), encrypted2);
                evaluator.multiply(encrypted1, encrypted2);
                decryptor.decrypt(encrypted1, plain);
                Assert::IsTrue(static_cast<int64_t>(-15) == encoder.decode_int64(plain));
                Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
                Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(0x10000), encrypted1);
                encryptor.encrypt(encoder.encode(0x100), encrypted2);
                evaluator.multiply(encrypted1, encrypted2);
                decryptor.decrypt(encrypted1, plain);
                Assert::AreEqual(static_cast<uint64_t>(0x1000000), encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted2.hash_block() == encrypted1.hash_block());
                Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());
            }
            {
                EncryptionParameters parms;
                SmallModulus plain_modulus(1 << 6);
                parms.set_poly_modulus("1x^128 + 1");
                parms.set_plain_modulus(plain_modulus);
                parms.set_coeff_modulus({ small_mods_40bit(0), small_mods_40bit(1) });
                SEALContext context(parms);
                KeyGenerator keygen(context);

                BalancedEncoder encoder(plain_modulus);
                Encryptor encryptor(context, keygen.public_key());
                Evaluator evaluator(context);
                Decryptor decryptor(context, keygen.secret_key());

                Ciphertext encrypted1;
                Plaintext plain;
                encryptor.encrypt(encoder.encode(123), encrypted1);
                evaluator.multiply(encrypted1, encrypted1, encrypted1);
                evaluator.multiply(encrypted1, encrypted1, encrypted1);
                decryptor.decrypt(encrypted1, plain);
                Assert::AreEqual(static_cast<uint64_t>(228886641), encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted1.hash_block() == parms.hash_block());
            }
        }

        TEST_METHOD(FVEncryptSquareDecrypt)
        {
            EncryptionParameters parms;
            SmallModulus plain_modulus(1 << 6);
            parms.set_poly_modulus("1x^128 + 1");
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus({ small_mods_40bit(0), small_mods_40bit(1) });
            SEALContext context(parms);
            KeyGenerator keygen(context);

            BalancedEncoder encoder(plain_modulus);
            Encryptor encryptor(context, keygen.public_key());
            Evaluator evaluator(context);
            Decryptor decryptor(context, keygen.secret_key());

            Ciphertext encrypted;
            Plaintext plain;
            encryptor.encrypt(encoder.encode(1), encrypted);
            evaluator.square(encrypted);
            decryptor.decrypt(encrypted, plain);
            Assert::AreEqual(1ULL, encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(0), encrypted);
            evaluator.square(encrypted);
            decryptor.decrypt(encrypted, plain);
            Assert::AreEqual(0ULL, encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(-5), encrypted);
            evaluator.square(encrypted);
            decryptor.decrypt(encrypted, plain);
            Assert::AreEqual(25ULL, encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(-1), encrypted);
            evaluator.square(encrypted);
            decryptor.decrypt(encrypted, plain);
            Assert::AreEqual(1ULL, encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(123), encrypted);
            evaluator.square(encrypted);
            decryptor.decrypt(encrypted, plain);
            Assert::AreEqual(15129ULL, encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(0x10000), encrypted);
            evaluator.square(encrypted);
            decryptor.decrypt(encrypted, plain);
            Assert::AreEqual(0x100000000ULL, encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(123), encrypted);
            evaluator.square(encrypted);
            evaluator.square(encrypted);
            decryptor.decrypt(encrypted, plain);
            Assert::AreEqual(228886641ULL, encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());
        }

        TEST_METHOD(FVEncryptMultiplyManyDecrypt)
        {
            EncryptionParameters parms;
            SmallModulus plain_modulus(1 << 6);
            parms.set_poly_modulus("1x^128 + 1");
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus({ small_mods_40bit(0), small_mods_40bit(1) });
            SEALContext context(parms);
            KeyGenerator keygen(context);

            BalancedEncoder encoder(plain_modulus);
            Encryptor encryptor(context, keygen.public_key());
            Evaluator evaluator(context);
            Decryptor decryptor(context, keygen.secret_key());
            EvaluationKeys evk;
            keygen.generate_evaluation_keys(4, evk);

            Ciphertext encrypted1, encrypted2, encrypted3, encrypted4, product;
            Plaintext plain;
            encryptor.encrypt(encoder.encode(5), encrypted1);
            encryptor.encrypt(encoder.encode(6), encrypted2);
            encryptor.encrypt(encoder.encode(7), encrypted3);
            vector<Ciphertext> encrypteds{ encrypted1, encrypted2, encrypted3 };
                evaluator.multiply_many(encrypteds, evk, product);
                decryptor.decrypt(product, plain);
                Assert::AreEqual(static_cast<uint64_t>(210), encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted1.hash_block() == product.hash_block());
                Assert::IsTrue(encrypted2.hash_block() == product.hash_block());
                Assert::IsTrue(encrypted3.hash_block() == product.hash_block());
                Assert::IsTrue(product.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(-9), encrypted1);
                encryptor.encrypt(encoder.encode(-17), encrypted2);
                encrypteds = { encrypted1, encrypted2 };
                evaluator.multiply_many(encrypteds, evk, product);
                decryptor.decrypt(product, plain);
                Assert::AreEqual(static_cast<uint64_t>(153), encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted1.hash_block() == product.hash_block());
                Assert::IsTrue(encrypted2.hash_block() == product.hash_block());
                Assert::IsTrue(product.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(2), encrypted1);
                encryptor.encrypt(encoder.encode(-31), encrypted2);
                encryptor.encrypt(encoder.encode(7), encrypted3);
                encrypteds = { encrypted1, encrypted2, encrypted3 };
                evaluator.multiply_many(encrypteds, evk, product);
                decryptor.decrypt(product, plain);
                Assert::IsTrue(static_cast<int64_t>(-434) == encoder.decode_int64(plain));
                Assert::IsTrue(encrypted1.hash_block() == product.hash_block());
                Assert::IsTrue(encrypted2.hash_block() == product.hash_block());
                Assert::IsTrue(encrypted3.hash_block() == product.hash_block());
                Assert::IsTrue(product.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(1), encrypted1);
                encryptor.encrypt(encoder.encode(-1), encrypted2);
                encryptor.encrypt(encoder.encode(1), encrypted3);
                encryptor.encrypt(encoder.encode(-1), encrypted4);
                encrypteds = { encrypted1, encrypted2, encrypted3, encrypted4 };
                evaluator.multiply_many(encrypteds, evk, product);
                decryptor.decrypt(product, plain);
                Assert::AreEqual(static_cast<uint64_t>(1), encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted1.hash_block() == product.hash_block());
                Assert::IsTrue(encrypted2.hash_block() == product.hash_block());
                Assert::IsTrue(encrypted3.hash_block() == product.hash_block());
                Assert::IsTrue(encrypted4.hash_block() == product.hash_block());
                Assert::IsTrue(product.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(98765), encrypted1);
                encryptor.encrypt(encoder.encode(0), encrypted2);
                encryptor.encrypt(encoder.encode(12345), encrypted3);
                encryptor.encrypt(encoder.encode(34567), encrypted4);
                encrypteds = { encrypted1, encrypted2, encrypted3, encrypted4 };
                evaluator.multiply_many(encrypteds, evk, product);
                decryptor.decrypt(product, plain);
                Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted1.hash_block() == product.hash_block());
                Assert::IsTrue(encrypted2.hash_block() == product.hash_block());
                Assert::IsTrue(encrypted3.hash_block() == product.hash_block());
                Assert::IsTrue(encrypted4.hash_block() == product.hash_block());
                Assert::IsTrue(product.hash_block() == parms.hash_block());
        }

        TEST_METHOD(FVEncryptExponentiateDecrypt)
        {
            EncryptionParameters parms;
            SmallModulus plain_modulus(1 << 6);
            parms.set_poly_modulus("1x^128 + 1");
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus({ small_mods_40bit(0), small_mods_40bit(1) });
            SEALContext context(parms);
            KeyGenerator keygen(context);

            BalancedEncoder encoder(plain_modulus);
            Encryptor encryptor(context, keygen.public_key());
            Evaluator evaluator(context);
            Decryptor decryptor(context, keygen.secret_key());
            EvaluationKeys evk;
            keygen.generate_evaluation_keys(4, evk);

            Ciphertext encrypted;
            Plaintext plain;
            encryptor.encrypt(encoder.encode(5), encrypted);
            evaluator.exponentiate(encrypted, 1, evk);
            decryptor.decrypt(encrypted, plain);
            Assert::AreEqual(static_cast<uint64_t>(5), encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted.hash_block() == encrypted.hash_block());
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(7), encrypted);
            evaluator.exponentiate(encrypted, 2, evk);
            decryptor.decrypt(encrypted, plain);
            Assert::AreEqual(static_cast<uint64_t>(49), encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted.hash_block() == encrypted.hash_block());
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(-7), encrypted);
            evaluator.exponentiate(encrypted, 3, evk);
            decryptor.decrypt(encrypted, plain);
            Assert::IsTrue(static_cast<int64_t>(-343) == encoder.decode_int64(plain));
            Assert::IsTrue(encrypted.hash_block() == encrypted.hash_block());
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(0x100), encrypted);
            evaluator.exponentiate(encrypted, 4, evk);
            decryptor.decrypt(encrypted, plain);
            Assert::AreEqual(static_cast<uint64_t>(0x100000000), encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted.hash_block() == encrypted.hash_block());
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());
        }

        TEST_METHOD(FVEncryptAddManyDecrypt)
        {
            EncryptionParameters parms;
            SmallModulus plain_modulus(1 << 6);
            BigPoly poly_modulus("1x^128 + 1");
            parms.set_poly_modulus(poly_modulus);
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus({ small_mods_40bit(0), small_mods_40bit(1) });
            SEALContext context(parms);
            KeyGenerator keygen(context);

            BalancedEncoder encoder(plain_modulus);
            Encryptor encryptor(context, keygen.public_key());
            Evaluator evaluator(context);
            Decryptor decryptor(context, keygen.secret_key());

            Ciphertext encrypted1, encrypted2, encrypted3, encrypted4, sum;
            Plaintext plain;
            encryptor.encrypt(encoder.encode(5), encrypted1);
            encryptor.encrypt(encoder.encode(6), encrypted2);
            encryptor.encrypt(encoder.encode(7), encrypted3);
            vector<Ciphertext> encrypteds = { encrypted1, encrypted2, encrypted3 };
            evaluator.add_many(encrypteds, sum);
            decryptor.decrypt(sum, plain);
            Assert::AreEqual(static_cast<uint64_t>(18), encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted1.hash_block() == sum.hash_block());
            Assert::IsTrue(encrypted2.hash_block() == sum.hash_block());
            Assert::IsTrue(encrypted3.hash_block() == sum.hash_block());
            Assert::IsTrue(sum.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(-9), encrypted1);
            encryptor.encrypt(encoder.encode(-17), encrypted2);
            encrypteds = { encrypted1, encrypted2, };
            evaluator.add_many(encrypteds, sum);
            decryptor.decrypt(sum, plain);
            Assert::IsTrue(static_cast<int64_t>(-26) == encoder.decode_int64(plain));
            Assert::IsTrue(encrypted1.hash_block() == sum.hash_block());
            Assert::IsTrue(encrypted2.hash_block() == sum.hash_block());
            Assert::IsTrue(sum.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(2), encrypted1);
            encryptor.encrypt(encoder.encode(-31), encrypted2);
            encryptor.encrypt(encoder.encode(7), encrypted3);
            encrypteds = { encrypted1, encrypted2, encrypted3 };
            evaluator.add_many(encrypteds, sum);
            decryptor.decrypt(sum, plain);
            Assert::IsTrue(static_cast<int64_t>(-22) == encoder.decode_int64(plain));
            Assert::IsTrue(encrypted1.hash_block() == sum.hash_block());
            Assert::IsTrue(encrypted2.hash_block() == sum.hash_block());
            Assert::IsTrue(encrypted3.hash_block() == sum.hash_block());
            Assert::IsTrue(sum.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(1), encrypted1);
            encryptor.encrypt(encoder.encode(-1), encrypted2);
            encryptor.encrypt(encoder.encode(1), encrypted3);
            encryptor.encrypt(encoder.encode(-1), encrypted4);
            encrypteds = { encrypted1, encrypted2, encrypted3, encrypted4 };
            evaluator.add_many(encrypteds, sum);
            decryptor.decrypt(sum, plain);
            Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted1.hash_block() == sum.hash_block());
            Assert::IsTrue(encrypted2.hash_block() == sum.hash_block());
            Assert::IsTrue(encrypted3.hash_block() == sum.hash_block());
            Assert::IsTrue(encrypted4.hash_block() == sum.hash_block());
            Assert::IsTrue(sum.hash_block() == parms.hash_block());

            encryptor.encrypt(encoder.encode(98765), encrypted1);
            encryptor.encrypt(encoder.encode(0), encrypted2);
            encryptor.encrypt(encoder.encode(12345), encrypted3);
            encryptor.encrypt(encoder.encode(34567), encrypted4);
            encrypteds = { encrypted1, encrypted2, encrypted3, encrypted4 };
            evaluator.add_many(encrypteds, sum);
            decryptor.decrypt(sum, plain);
            Assert::AreEqual(static_cast<uint64_t>(145677), encoder.decode_uint64(plain));
            Assert::IsTrue(encrypted1.hash_block() == sum.hash_block());
            Assert::IsTrue(encrypted2.hash_block() == sum.hash_block());
            Assert::IsTrue(encrypted3.hash_block() == sum.hash_block());
            Assert::IsTrue(encrypted4.hash_block() == sum.hash_block());
            Assert::IsTrue(sum.hash_block() == parms.hash_block());

            BalancedFractionalEncoder frac_encoder(plain_modulus, poly_modulus, 10, 15);
            encryptor.encrypt(frac_encoder.encode(3.1415), encrypted1);
            encryptor.encrypt(frac_encoder.encode(12.345), encrypted2);
            encryptor.encrypt(frac_encoder.encode(98.765), encrypted3);
            encryptor.encrypt(frac_encoder.encode(1.1111), encrypted4);
            encrypteds = { encrypted1, encrypted2, encrypted3, encrypted4 };
            evaluator.add_many(encrypteds, sum);
            decryptor.decrypt(sum, plain);
            Assert::IsTrue(abs(frac_encoder.decode(plain) - 115.3626) < 0.000001);
            Assert::IsTrue(encrypted1.hash_block() == sum.hash_block());
            Assert::IsTrue(encrypted2.hash_block() == sum.hash_block());
            Assert::IsTrue(encrypted3.hash_block() == sum.hash_block());
            Assert::IsTrue(encrypted4.hash_block() == sum.hash_block());
            Assert::IsTrue(sum.hash_block() == parms.hash_block());
        }

        TEST_METHOD(TransformPlainToNTT)
        {
            EncryptionParameters parms;
            SmallModulus plain_modulus(1 << 6);
            BigPoly poly_modulus("1x^128 + 1");
            parms.set_poly_modulus(poly_modulus);
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus({ small_mods_40bit(0), small_mods_40bit(1) });
            SEALContext context(parms);
            KeyGenerator keygen(context);

            Evaluator evaluator(context);
            Plaintext plain = "0";
            evaluator.transform_to_ntt(plain);
            Assert::IsTrue(plain.to_string() == "0");

            plain.release();
            plain = "1";
            evaluator.transform_to_ntt(plain);
            for (int i = 0; i < 128; i++)
            {
                Assert::IsTrue(plain[i] == 1);
            }
            Assert::IsTrue(plain[128] == 0);

            plain.release();
            plain = "2";
            evaluator.transform_to_ntt(plain);
            for (int i = 0; i < 128; i++)
            {
                Assert::IsTrue(plain[i] == 2);
            }
            Assert::IsTrue(plain[128] == 0);
        }

        TEST_METHOD(TransformEncryptedToFromNTT)
        {
            EncryptionParameters parms;
            SmallModulus plain_modulus(1 << 6);
            BigPoly poly_modulus("1x^128 + 1");
            parms.set_poly_modulus(poly_modulus);
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus({ small_mods_40bit(0), small_mods_40bit(1) });
            SEALContext context(parms);
            KeyGenerator keygen(context);

            Encryptor encryptor(context, keygen.public_key());
            Evaluator evaluator(context);
            Decryptor decryptor(context, keygen.secret_key());

            Plaintext plain;
            Ciphertext encrypted;
            plain = "0";
            encryptor.encrypt(plain, encrypted);
            evaluator.transform_to_ntt(encrypted);
            evaluator.transform_from_ntt(encrypted);
            decryptor.decrypt(encrypted, plain);
            Assert::IsTrue(plain.to_string() == "0");
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            plain = "1";
            encryptor.encrypt(plain, encrypted);
            evaluator.transform_to_ntt(encrypted);
            evaluator.transform_from_ntt(encrypted);
            decryptor.decrypt(encrypted, plain);
            Assert::IsTrue(plain.to_string() == "1");
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            plain = "Fx^10 + Ex^9 + Dx^8 + Cx^7 + Bx^6 + Ax^5 + 1x^4 + 2x^3 + 3x^2 + 4x^1 + 5";
            encryptor.encrypt(plain, encrypted);
            evaluator.transform_to_ntt(encrypted);
            evaluator.transform_from_ntt(encrypted);
            decryptor.decrypt(encrypted, plain);
            Assert::IsTrue(plain.to_string() == "Fx^10 + Ex^9 + Dx^8 + Cx^7 + Bx^6 + Ax^5 + 1x^4 + 2x^3 + 3x^2 + 4x^1 + 5");
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());
        }

        TEST_METHOD(FVEncryptMultiplyPlainNTTDecrypt)
        {
            EncryptionParameters parms;
            SmallModulus plain_modulus(1 << 6);
            BigPoly poly_modulus("1x^128 + 1");
            parms.set_poly_modulus(poly_modulus);
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus({ small_mods_40bit(0), small_mods_40bit(1) });
            SEALContext context(parms);
            KeyGenerator keygen(context);

            Encryptor encryptor(context, keygen.public_key());
            Evaluator evaluator(context);
            Decryptor decryptor(context, keygen.secret_key());

            Plaintext plain;
            Plaintext plain_multiplier;
            Ciphertext encrypted;

            plain = 0;
            encryptor.encrypt(plain, encrypted);
            evaluator.transform_to_ntt(encrypted);
            plain_multiplier = 1;
            evaluator.transform_to_ntt(plain_multiplier);
            evaluator.multiply_plain_ntt(encrypted, plain_multiplier);
            evaluator.transform_from_ntt(encrypted);
            decryptor.decrypt(encrypted, plain);
            Assert::IsTrue(plain.to_string() == "0");
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            plain = 2;
            encryptor.encrypt(plain, encrypted);
            evaluator.transform_to_ntt(encrypted);
            plain_multiplier.release();
            plain_multiplier = 3;
            evaluator.transform_to_ntt(plain_multiplier);
            evaluator.multiply_plain_ntt(encrypted, plain_multiplier);
            evaluator.transform_from_ntt(encrypted);
            decryptor.decrypt(encrypted, plain);
            Assert::IsTrue(plain.to_string() == "6");
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            plain = 1;
            encryptor.encrypt(plain, encrypted);
            evaluator.transform_to_ntt(encrypted);
            plain_multiplier.release();
            plain_multiplier = "Fx^10 + Ex^9 + Dx^8 + Cx^7 + Bx^6 + Ax^5 + 1x^4 + 2x^3 + 3x^2 + 4x^1 + 5";
            evaluator.transform_to_ntt(plain_multiplier);
            evaluator.multiply_plain_ntt(encrypted, plain_multiplier);
            evaluator.transform_from_ntt(encrypted);
            decryptor.decrypt(encrypted, plain);
            Assert::IsTrue(plain.to_string() == "Fx^10 + Ex^9 + Dx^8 + Cx^7 + Bx^6 + Ax^5 + 1x^4 + 2x^3 + 3x^2 + 4x^1 + 5");
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

            plain = "1x^20";
            encryptor.encrypt(plain, encrypted);
            evaluator.transform_to_ntt(encrypted);
            plain_multiplier.release();
            plain_multiplier = "Fx^10 + Ex^9 + Dx^8 + Cx^7 + Bx^6 + Ax^5 + 1x^4 + 2x^3 + 3x^2 + 4x^1 + 5";
            evaluator.transform_to_ntt(plain_multiplier);
            evaluator.multiply_plain_ntt(encrypted, plain_multiplier);
            evaluator.transform_from_ntt(encrypted);
            decryptor.decrypt(encrypted, plain);
            Assert::IsTrue(plain.to_string() == "Fx^30 + Ex^29 + Dx^28 + Cx^27 + Bx^26 + Ax^25 + 1x^24 + 2x^23 + 3x^22 + 4x^21 + 5x^20");
            Assert::IsTrue(encrypted.hash_block() == parms.hash_block());
        }

        TEST_METHOD(FVEncryptRotateMatrixDecrypt)
        {
            EncryptionParameters parms;
            SmallModulus plain_modulus(257);
            BigPoly poly_modulus("1x^8 + 1");
            parms.set_poly_modulus(poly_modulus);
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus({ small_mods_40bit(0), small_mods_40bit(1) });
            SEALContext context(parms);
            KeyGenerator keygen(context);
            GaloisKeys glk;
            keygen.generate_galois_keys(24, glk);

            Encryptor encryptor(context, keygen.public_key());
            Evaluator evaluator(context);
            Decryptor decryptor(context, keygen.secret_key());
            PolyCRTBuilder crtbuilder(context);

            Plaintext plain;
            vector<uint64_t> plain_vec{
                1, 2, 3, 4,
                5, 6, 7, 8
            };
            crtbuilder.compose(plain_vec, plain);
            Ciphertext encrypted;
            encryptor.encrypt(plain, encrypted);

            evaluator.rotate_columns(encrypted, glk);
            decryptor.decrypt(encrypted, plain);
            crtbuilder.decompose(plain, plain_vec);
            Assert::IsTrue(plain_vec == vector<uint64_t>{
                5, 6, 7, 8,
                1, 2, 3, 4
            });

            evaluator.rotate_rows(encrypted, -1, glk);
            decryptor.decrypt(encrypted, plain);
            crtbuilder.decompose(plain, plain_vec);
            Assert::IsTrue(plain_vec == vector<uint64_t>{
                8, 5, 6, 7,
                4, 1, 2, 3
            });

            evaluator.rotate_rows(encrypted, 2, glk);
            decryptor.decrypt(encrypted, plain);
            crtbuilder.decompose(plain, plain_vec);
            Assert::IsTrue(plain_vec == vector<uint64_t>{
                6, 7, 8, 5,
                2, 3, 4, 1
            });

            evaluator.rotate_columns(encrypted, glk);
            decryptor.decrypt(encrypted, plain);
            crtbuilder.decompose(plain, plain_vec);
            Assert::IsTrue(plain_vec == vector<uint64_t>{
                2, 3, 4, 1,
                6, 7, 8, 5
            });

            evaluator.rotate_rows(encrypted, 0, glk);
            decryptor.decrypt(encrypted, plain);
            crtbuilder.decompose(plain, plain_vec);
            Assert::IsTrue(plain_vec == vector<uint64_t>{
                2, 3, 4, 1,
                6, 7, 8, 5
            });
        }
    };
}