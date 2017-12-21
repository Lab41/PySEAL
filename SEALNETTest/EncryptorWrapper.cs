using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.Research.SEAL;
using System.Collections.Generic;

namespace SEALNETTest
{
    [TestClass]
    public class EncryptorWrapper
    {
        [TestMethod]
        public void FVEncryptDecryptNET()
        {
            var parms = new EncryptionParameters();
            var plain_modulus = new SmallModulus(1 << 6);
            parms.NoiseStandardDeviation = 3.19;
            parms.PlainModulus = plain_modulus;
            {
                parms.PolyModulus = "1x^64 + 1";
                parms.CoeffModulus = new List<SmallModulus> { DefaultParams.SmallMods60Bit(0) };
                var context = new SEALContext(parms);

                var keygen = new KeyGenerator(context);
                var encoder = new BalancedEncoder(plain_modulus);

                var encryptor = new Encryptor(context, keygen.PublicKey);
                var decryptor = new Decryptor(context, keygen.SecretKey);

                var encrypted = new Ciphertext();
                var plain = new Plaintext();
                encryptor.Encrypt(encoder.Encode(0x12345678), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(0x12345678UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(0), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(0UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(1), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(1UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(2), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(2UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(0x7FFFFFFFFFFFFFFD), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(0x7FFFFFFFFFFFFFFDUL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(0x7FFFFFFFFFFFFFFE), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(0x7FFFFFFFFFFFFFFEUL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(0x7FFFFFFFFFFFFFFF), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(0x7FFFFFFFFFFFFFFFUL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(314159265), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(314159265UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);
            }
            {
                parms.PolyModulus = "1x^128 + 1";
                parms.CoeffModulus = new List<SmallModulus> { DefaultParams.SmallMods40Bit(0), DefaultParams.SmallMods40Bit(1) };
                var context = new SEALContext(parms);
                var keygen = new KeyGenerator(context);

                var encoder = new BalancedEncoder(plain_modulus);

                var encryptor = new Encryptor(context, keygen.PublicKey);
                var decryptor = new Decryptor(context, keygen.SecretKey);

                var encrypted = new Ciphertext();
                var plain = new Plaintext();
                encryptor.Encrypt(encoder.Encode(0x12345678), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(0x12345678UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(0), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(0UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(1), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(1UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(2), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(2UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(0x7FFFFFFFFFFFFFFD), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(0x7FFFFFFFFFFFFFFDUL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(0x7FFFFFFFFFFFFFFE), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(0x7FFFFFFFFFFFFFFEUL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(0x7FFFFFFFFFFFFFFF), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(0x7FFFFFFFFFFFFFFFUL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(314159265), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(314159265UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);
            }

            {
                parms.PolyModulus = "1x^256 + 1";
                parms.CoeffModulus = new List<SmallModulus> {
                    DefaultParams.SmallMods40Bit(0), DefaultParams.SmallMods40Bit(1), DefaultParams.SmallMods40Bit(2) };
                var context = new SEALContext(parms);
                var keygen = new KeyGenerator(context);

                var encoder = new BalancedEncoder(plain_modulus);

                var encryptor = new Encryptor(context, keygen.PublicKey);
                var decryptor = new Decryptor(context, keygen.SecretKey);

                var encrypted = new Ciphertext();
                var plain = new Plaintext();
                encryptor.Encrypt(encoder.Encode(0x12345678), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(0x12345678UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(0), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(0UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(1), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(1UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(2), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(2UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(0x7FFFFFFFFFFFFFFD), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(0x7FFFFFFFFFFFFFFDUL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(0x7FFFFFFFFFFFFFFE), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(0x7FFFFFFFFFFFFFFEUL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(0x7FFFFFFFFFFFFFFF), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(0x7FFFFFFFFFFFFFFFUL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(314159265), encrypted);
                decryptor.Decrypt(encrypted, plain);
                Assert.AreEqual(314159265UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);
            }
        }
    }
}