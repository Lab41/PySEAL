#include "CppUnitTest.h"
#include "seal/context.h"
#include "seal/encryptor.h"
#include "seal/decryptor.h"
#include "seal/keygenerator.h"
#include "seal/encoder.h"
#include <cstdint>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(EncryptorTest)
    {
    public:
        TEST_METHOD(FVEncryptDecrypt)
        {
            EncryptionParameters parms;
            SmallModulus plain_modulus(1 << 6);
            parms.set_noise_standard_deviation(3.19);            
            parms.set_plain_modulus(plain_modulus);
            {
                parms.set_poly_modulus("1x^64 + 1");
                parms.set_coeff_modulus({ small_mods_40bit(0) });
                SEALContext context(parms);
                KeyGenerator keygen(context);

                BalancedEncoder encoder(plain_modulus);

                Encryptor encryptor(context, keygen.public_key());
                Decryptor decryptor(context, keygen.secret_key());

                Ciphertext encrypted;
                Plaintext plain;
                encryptor.encrypt(encoder.encode(0x12345678), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(0x12345678ULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(0), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(0ULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(1), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(1ULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(2), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(2ULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(0x7FFFFFFFFFFFFFFD), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(0x7FFFFFFFFFFFFFFDULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(0x7FFFFFFFFFFFFFFE), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(0x7FFFFFFFFFFFFFFEULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(0x7FFFFFFFFFFFFFFF), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(0x7FFFFFFFFFFFFFFFULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(314159265), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(314159265ULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());
            }
            {
                parms.set_poly_modulus("1x^128 + 1");
                parms.set_coeff_modulus({ small_mods_40bit(0), small_mods_40bit(1) });
                SEALContext context(parms);
                KeyGenerator keygen(context);

                BalancedEncoder encoder(plain_modulus);

                Encryptor encryptor(context, keygen.public_key());
                Decryptor decryptor(context, keygen.secret_key());

                Ciphertext encrypted;
                Plaintext plain;
                encryptor.encrypt(encoder.encode(0x12345678), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(0x12345678ULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(0), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(0ULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(1), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(1ULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(2), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(2ULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(0x7FFFFFFFFFFFFFFD), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(0x7FFFFFFFFFFFFFFDULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(0x7FFFFFFFFFFFFFFE), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(0x7FFFFFFFFFFFFFFEULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(0x7FFFFFFFFFFFFFFF), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(0x7FFFFFFFFFFFFFFFULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(314159265), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(314159265ULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());
            }

            {
                parms.set_poly_modulus("1x^256 + 1");
                parms.set_coeff_modulus({ small_mods_40bit(0), small_mods_40bit(1), small_mods_40bit(2) });
                SEALContext context(parms);
                KeyGenerator keygen(context);

                BalancedEncoder encoder(plain_modulus);

                Encryptor encryptor(context, keygen.public_key());
                Decryptor decryptor(context, keygen.secret_key());

                Ciphertext encrypted;
                Plaintext plain;
                encryptor.encrypt(encoder.encode(0x12345678), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(0x12345678ULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(0), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(0ULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(1), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(1ULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(2), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(2ULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(0x7FFFFFFFFFFFFFFD), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(0x7FFFFFFFFFFFFFFDULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(0x7FFFFFFFFFFFFFFE), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(0x7FFFFFFFFFFFFFFEULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(0x7FFFFFFFFFFFFFFF), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(0x7FFFFFFFFFFFFFFFULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());

                encryptor.encrypt(encoder.encode(314159265), encrypted);
                decryptor.decrypt(encrypted, plain);
                Assert::AreEqual(314159265ULL, encoder.decode_uint64(plain));
                Assert::IsTrue(encrypted.hash_block() == parms.hash_block());
            }
        }
    };
}
