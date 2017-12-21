using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.Research.SEAL;
using System.Collections.Generic;
using System;
using System.Linq;

namespace SEALNETTest
{
    [TestClass]
    public class EvaluatorWrapper
    {
        [TestMethod]
        public void FVEncryptNegateDecryptNET()
        {
            var parms = new EncryptionParameters();
            var plain_modulus = new SmallModulus(1 << 6);
            parms.NoiseStandardDeviation = 3.19;
            parms.PlainModulus = plain_modulus;
            parms.PolyModulus = "1x^64 + 1";
            parms.CoeffModulus = new List<SmallModulus> { DefaultParams.SmallMods40Bit(0) };
            var context = new SEALContext(parms);
            var keygen = new KeyGenerator(context);
            var encoder = new BalancedEncoder(plain_modulus);

            var encryptor = new Encryptor(context, keygen.PublicKey);
            var evaluator = new Evaluator(context);
            var decryptor = new Decryptor(context, keygen.SecretKey);

            var encrypted = new Ciphertext();
            var plain = new Plaintext();

            encryptor.Encrypt(encoder.Encode(0x12345678), encrypted);
            evaluator.Negate(encrypted);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual(-0x12345678, encoder.DecodeInt32(plain));
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(0), encrypted);
            evaluator.Negate(encrypted);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual((0), encoder.DecodeInt32(plain));
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(1), encrypted);
            evaluator.Negate(encrypted);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual((-1), encoder.DecodeInt32(plain));
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(-1), encrypted);
            evaluator.Negate(encrypted);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual((1), encoder.DecodeInt32(plain));
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(2), encrypted);
            evaluator.Negate(encrypted);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual((-2), encoder.DecodeInt32(plain));
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(-5), encrypted);
            evaluator.Negate(encrypted);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual((5), encoder.DecodeInt32(plain));
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);
        }

        [TestMethod]
        public void FVEncryptAddDecryptNET()
        {
            var parms = new EncryptionParameters();
            var plain_modulus = new SmallModulus(1 << 6);
            parms.NoiseStandardDeviation = 3.19;
            parms.PlainModulus = plain_modulus;
            parms.PolyModulus = "1x^64 + 1";
            parms.CoeffModulus = new List<SmallModulus> { DefaultParams.SmallMods40Bit(0) };
            var context = new SEALContext(parms);
            var keygen = new KeyGenerator(context);
            var encoder = new BalancedEncoder(plain_modulus);

            var encryptor = new Encryptor(context, keygen.PublicKey);
            var evaluator = new Evaluator(context);
            var decryptor = new Decryptor(context, keygen.SecretKey);

            var encrypted1 = new Ciphertext();
            var encrypted2 = new Ciphertext();
            var plain = new Plaintext();

            encryptor.Encrypt(encoder.Encode(0x12345678), encrypted1);
            encryptor.Encrypt(encoder.Encode(0x54321), encrypted2);
            evaluator.Add(encrypted1, encrypted2);
            decryptor.Decrypt(encrypted1, plain);
            Assert.AreEqual((0x12399999UL), encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
            Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(0), encrypted1);
            encryptor.Encrypt(encoder.Encode(0), encrypted2);
            evaluator.Add(encrypted1, encrypted2);
            decryptor.Decrypt(encrypted1, plain);
            Assert.AreEqual(0UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
            Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(0), encrypted1);
            encryptor.Encrypt(encoder.Encode(5), encrypted2);
            evaluator.Add(encrypted1, encrypted2);
            decryptor.Decrypt(encrypted1, plain);
            Assert.AreEqual(5UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
            Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(5), encrypted1);
            encryptor.Encrypt(encoder.Encode(-3), encrypted2);
            evaluator.Add(encrypted1, encrypted2);
            decryptor.Decrypt(encrypted1, plain);
            Assert.AreEqual(2, encoder.DecodeInt32(plain));
            Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
            Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(-7), encrypted1);
            encryptor.Encrypt(encoder.Encode(2), encrypted2);
            evaluator.Add(encrypted1, encrypted2);
            decryptor.Decrypt(encrypted1, plain);
            Assert.AreEqual((-5), encoder.DecodeInt32(plain));
            Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
            Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

            var plain1 = new Plaintext("2x^2 + 1x^1 + 3");
            var plain2 = new Plaintext("3x^3 + 4x^2 + 5x^1 + 6");
            encryptor.Encrypt(plain1, encrypted1);
            encryptor.Encrypt(plain2, encrypted2);
            evaluator.Add(encrypted1, encrypted2);
            decryptor.Decrypt(encrypted1, plain);
            Assert.IsTrue(plain.ToString().Equals("3x^3 + 6x^2 + 6x^1 + 9"));
            Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
            Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

            plain1.Set("3x^5 + 1x^4 + 4x^3 + 1");
            plain2.Set("5x^2 + 9x^1 + 2");
            encryptor.Encrypt(plain1, encrypted1);
            encryptor.Encrypt(plain2, encrypted2);
            evaluator.Add(encrypted1, encrypted2);
            decryptor.Decrypt(encrypted1, plain);
            Assert.IsTrue(plain.ToString().Equals("3x^5 + 1x^4 + 4x^3 + 5x^2 + 9x^1 + 3"));
            Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
            Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);
        }

        [TestMethod]
        public void FVEncryptSubDecryptNET()
        {
            var parms = new EncryptionParameters();
            var plain_modulus = new SmallModulus(1 << 6);
            parms.NoiseStandardDeviation = 3.19;
            parms.PlainModulus = plain_modulus;
            parms.PolyModulus = "1x^64 + 1";
            parms.CoeffModulus = new List<SmallModulus> { DefaultParams.SmallMods40Bit(0) };
            var context = new SEALContext(parms);
            var keygen = new KeyGenerator(context);
            var encoder = new BalancedEncoder(plain_modulus);

            var encryptor = new Encryptor(context, keygen.PublicKey);
            var evaluator = new Evaluator(context);
            var decryptor = new Decryptor(context, keygen.SecretKey);

            var encrypted1 = new Ciphertext();
            var encrypted2 = new Ciphertext();
            var plain = new Plaintext();

            encryptor.Encrypt(encoder.Encode(0x12345678), encrypted1);
            encryptor.Encrypt(encoder.Encode(0x54321), encrypted2);
            evaluator.Sub(encrypted1, encrypted2);
            decryptor.Decrypt(encrypted1, plain);
            Assert.AreEqual(0x122F1357, encoder.DecodeInt32(plain));
            Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
            Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(0), encrypted1);
            encryptor.Encrypt(encoder.Encode(0), encrypted2);
            evaluator.Sub(encrypted1, encrypted2);
            decryptor.Decrypt(encrypted1, plain);
            Assert.AreEqual(0, encoder.DecodeInt32(plain));
            Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
            Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(0), encrypted1);
            encryptor.Encrypt(encoder.Encode(5), encrypted2);
            evaluator.Sub(encrypted1, encrypted2);
            decryptor.Decrypt(encrypted1, plain);
            Assert.AreEqual(-5, encoder.DecodeInt32(plain));
            Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
            Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(5), encrypted1);
            encryptor.Encrypt(encoder.Encode(-3), encrypted2);
            evaluator.Sub(encrypted1, encrypted2);
            decryptor.Decrypt(encrypted1, plain);
            Assert.AreEqual(8, encoder.DecodeInt32(plain));
            Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
            Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(-7), encrypted1);
            encryptor.Encrypt(encoder.Encode(2), encrypted2);
            evaluator.Sub(encrypted1, encrypted2);
            decryptor.Decrypt(encrypted1, plain);
            Assert.AreEqual(-9, encoder.DecodeInt32(plain));
            Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
            Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);
        }

        [TestMethod]
        public void FVEncryptAddPlainDecryptNET()
        {
            var parms = new EncryptionParameters();
            var plain_modulus = new SmallModulus(1 << 6);
            parms.NoiseStandardDeviation = 3.19;
            parms.PlainModulus = plain_modulus;
            parms.PolyModulus = "1x^64 + 1";
            parms.CoeffModulus = new List<SmallModulus> { DefaultParams.SmallMods40Bit(0) };
            var context = new SEALContext(parms);
            var keygen = new KeyGenerator(context);
            var encoder = new BalancedEncoder(plain_modulus);

            var encryptor = new Encryptor(context, keygen.PublicKey);
            var evaluator = new Evaluator(context);
            var decryptor = new Decryptor(context, keygen.SecretKey);

            var encrypted1 = new Ciphertext();
            var plain = new Plaintext();

            encryptor.Encrypt(encoder.Encode(0x12345678), encrypted1);
            plain = encoder.Encode(0x54321);
            evaluator.AddPlain(encrypted1, plain);
            decryptor.Decrypt(encrypted1, plain);
            Assert.AreEqual(0x12399999UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(0), encrypted1);
            plain = encoder.Encode(0);
            evaluator.AddPlain(encrypted1, plain);
            decryptor.Decrypt(encrypted1, plain);
            Assert.AreEqual(0UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);
            encryptor.Encrypt(encoder.Encode(0), encrypted1);
            plain = encoder.Encode(5);
            evaluator.AddPlain(encrypted1, plain);
            decryptor.Decrypt(encrypted1, plain);
            Assert.AreEqual(5UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);
            encryptor.Encrypt(encoder.Encode(5), encrypted1);
            plain = encoder.Encode(-3);
            evaluator.AddPlain(encrypted1, plain);
            decryptor.Decrypt(encrypted1, plain);
            Assert.AreEqual(2UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);
            encryptor.Encrypt(encoder.Encode(-7), encrypted1);
            plain = encoder.Encode(7);
            evaluator.AddPlain(encrypted1, plain);
            decryptor.Decrypt(encrypted1, plain);
            Assert.AreEqual(0UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);
        }

        [TestMethod]
        public void FVEncryptSubPlainDecryptNET()
        {
            var parms = new EncryptionParameters();
            var plain_modulus = new SmallModulus(1 << 6);
            parms.NoiseStandardDeviation = 3.19;
            parms.PlainModulus = plain_modulus;
            parms.PolyModulus = "1x^64 + 1";
            parms.CoeffModulus = new List<SmallModulus> { DefaultParams.SmallMods40Bit(0) };
            var context = new SEALContext(parms);
            var keygen = new KeyGenerator(context);
            var encoder = new BalancedEncoder(plain_modulus);

            var encryptor = new Encryptor(context, keygen.PublicKey);
            var evaluator = new Evaluator(context);
            var decryptor = new Decryptor(context, keygen.SecretKey);

            var encrypted1 = new Ciphertext();
            var encrypted2 = new Ciphertext();
            var plain = new Plaintext();

            encryptor.Encrypt(encoder.Encode(0x12345678), encrypted1);
            plain = encoder.Encode(0x54321);
            evaluator.SubPlain(encrypted1, plain);
            decryptor.Decrypt(encrypted1, plain);
            Assert.AreEqual(0x122F1357UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(0), encrypted1);
            plain = encoder.Encode(0);
            evaluator.SubPlain(encrypted1, plain);
            decryptor.Decrypt(encrypted1, plain);
            Assert.AreEqual(0UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(0), encrypted1);
            plain = encoder.Encode(5);
            evaluator.SubPlain(encrypted1, plain);
            decryptor.Decrypt(encrypted1, plain);
            Assert.AreEqual(-5, encoder.DecodeInt64(plain));
            Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(5), encrypted1);
            plain = encoder.Encode(-3);
            evaluator.SubPlain(encrypted1, plain);
            decryptor.Decrypt(encrypted1, plain);
            Assert.AreEqual(8UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(-7), encrypted1);
            plain = encoder.Encode(2);
            evaluator.SubPlain(encrypted1, plain);
            decryptor.Decrypt(encrypted1, plain);
            Assert.AreEqual(-9, encoder.DecodeInt64(plain));
            Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);
        }

        [TestMethod]
        public void FVEncryptMultiplyPlainDecryptNET()
        {
            var parms = new EncryptionParameters();
            var plain_modulus = new SmallModulus(1 << 6);
            parms.NoiseStandardDeviation = 3.19;
            parms.PlainModulus = plain_modulus;
            parms.PolyModulus = "1x^64 + 1";
            parms.CoeffModulus = new List<SmallModulus> { DefaultParams.SmallMods40Bit(0) };
            var context = new SEALContext(parms);
            var keygen = new KeyGenerator(context);
            var encoder = new BalancedEncoder(plain_modulus);

            var encryptor = new Encryptor(context, keygen.PublicKey);
            var evaluator = new Evaluator(context);
            var decryptor = new Decryptor(context, keygen.SecretKey);

            var encrypted = new Ciphertext();
            var plain = new Plaintext();

            encryptor.Encrypt(encoder.Encode(0x12345678), encrypted);
            plain = encoder.Encode(0x54321);
            evaluator.MultiplyPlain(encrypted, plain);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual(0x5FCBBBB88D78UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(0), encrypted);
            plain = encoder.Encode(5);
            evaluator.MultiplyPlain(encrypted, plain);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual(0UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(7), encrypted);
            plain = encoder.Encode(1);
            evaluator.MultiplyPlain(encrypted, plain);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual(7UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(5), encrypted);
            plain = encoder.Encode(-3);
            evaluator.MultiplyPlain(encrypted, plain);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual(-15, encoder.DecodeInt64(plain));
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(-7), encrypted);
            plain = encoder.Encode(2);
            evaluator.MultiplyPlain(encrypted, plain);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual(-14, encoder.DecodeInt64(plain));
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);
        }

        [TestMethod]
        public void FVEncryptMultiplyDecryptNET()
        {
            {
                var parms = new EncryptionParameters();
                var plain_modulus = new SmallModulus(1 << 6);
                parms.NoiseStandardDeviation = 3.19;
                parms.PlainModulus = plain_modulus;
                parms.PolyModulus = "1x^64 + 1";
                parms.CoeffModulus = new List<SmallModulus> { DefaultParams.SmallMods40Bit(0) };
                var context = new SEALContext(parms);
                var keygen = new KeyGenerator(context);
                var encoder = new BalancedEncoder(plain_modulus);

                var encryptor = new Encryptor(context, keygen.PublicKey);
                var evaluator = new Evaluator(context);
                var decryptor = new Decryptor(context, keygen.SecretKey);

                var encrypted1 = new Ciphertext();
                var encrypted2 = new Ciphertext();
                var plain = new Plaintext();

                encryptor.Encrypt(encoder.Encode(0x12345678), encrypted1);
                encryptor.Encrypt(encoder.Encode(0x54321), encrypted2);
                evaluator.Multiply(encrypted1, encrypted2);
                decryptor.Decrypt(encrypted1, plain);
                Assert.AreEqual(0x5FCBBBB88D78UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
                Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(0), encrypted1);
                encryptor.Encrypt(encoder.Encode(0), encrypted2);
                evaluator.Multiply(encrypted1, encrypted2);
                decryptor.Decrypt(encrypted1, plain);
                Assert.AreEqual(0UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
                Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(0), encrypted1);
                encryptor.Encrypt(encoder.Encode(5), encrypted2);
                evaluator.Multiply(encrypted1, encrypted2);
                decryptor.Decrypt(encrypted1, plain);
                Assert.AreEqual(0UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
                Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(7), encrypted1);
                encryptor.Encrypt(encoder.Encode(1), encrypted2);
                evaluator.Multiply(encrypted1, encrypted2);
                decryptor.Decrypt(encrypted1, plain);
                Assert.AreEqual(7UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
                Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(5), encrypted1);
                encryptor.Encrypt(encoder.Encode(-3), encrypted2);
                evaluator.Multiply(encrypted1, encrypted2);
                decryptor.Decrypt(encrypted1, plain);
                Assert.AreEqual(-15, encoder.DecodeInt64(plain));
                Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
                Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(0x10000), encrypted1);
                encryptor.Encrypt(encoder.Encode(0x100), encrypted2);
                evaluator.Multiply(encrypted1, encrypted2);
                decryptor.Decrypt(encrypted1, plain);
                Assert.AreEqual(0x1000000UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
                Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);
            }
            {
                var parms = new EncryptionParameters();
                var plain_modulus = new SmallModulus(1 << 6);
                parms.NoiseStandardDeviation = 3.19;
                parms.PlainModulus = plain_modulus;
                parms.PolyModulus = "1x^128 + 1";
                parms.CoeffModulus = new List<SmallModulus> {
                    DefaultParams.SmallMods40Bit(0), DefaultParams.SmallMods40Bit(1)
                };
                var context = new SEALContext(parms);
                var keygen = new KeyGenerator(context);
                var encoder = new BalancedEncoder(plain_modulus);

                var encryptor = new Encryptor(context, keygen.PublicKey);
                var evaluator = new Evaluator(context);
                var decryptor = new Decryptor(context, keygen.SecretKey);

                var encrypted1 = new Ciphertext();
                var encrypted2 = new Ciphertext();
                var plain = new Plaintext();

                encryptor.Encrypt(encoder.Encode(0x12345678), encrypted1);
                encryptor.Encrypt(encoder.Encode(0x54321), encrypted2);
                evaluator.Multiply(encrypted1, encrypted2);
                decryptor.Decrypt(encrypted1, plain);
                Assert.AreEqual(0x5FCBBBB88D78UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
                Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(0), encrypted1);
                encryptor.Encrypt(encoder.Encode(0), encrypted2);
                evaluator.Multiply(encrypted1, encrypted2);
                decryptor.Decrypt(encrypted1, plain);
                Assert.AreEqual(0UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
                Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(0), encrypted1);
                encryptor.Encrypt(encoder.Encode(5), encrypted2);
                evaluator.Multiply(encrypted1, encrypted2);
                decryptor.Decrypt(encrypted1, plain);
                Assert.AreEqual(0UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
                Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(7), encrypted1);
                encryptor.Encrypt(encoder.Encode(1), encrypted2);
                evaluator.Multiply(encrypted1, encrypted2);
                decryptor.Decrypt(encrypted1, plain);
                Assert.AreEqual(7UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
                Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(5), encrypted1);
                encryptor.Encrypt(encoder.Encode(-3), encrypted2);
                evaluator.Multiply(encrypted1, encrypted2);
                decryptor.Decrypt(encrypted1, plain);
                Assert.AreEqual(-15, encoder.DecodeInt64(plain));
                Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
                Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);

                encryptor.Encrypt(encoder.Encode(0x10000), encrypted1);
                encryptor.Encrypt(encoder.Encode(0x100), encrypted2);
                evaluator.Multiply(encrypted1, encrypted2);
                decryptor.Decrypt(encrypted1, plain);
                Assert.AreEqual(0x1000000UL, encoder.DecodeUInt64(plain));
                Assert.AreEqual(encrypted2.HashBlock, encrypted1.HashBlock);
                Assert.AreEqual(encrypted1.HashBlock, parms.HashBlock);
            }
        }

        [TestMethod]
        public void FVEncryptSquareDecryptNET()
        {
            var parms = new EncryptionParameters();
            var plain_modulus = new SmallModulus(1 << 6);
            parms.NoiseStandardDeviation = 3.19;
            parms.PlainModulus = plain_modulus;
            parms.PolyModulus = "1x^128 + 1";
            parms.CoeffModulus = new List<SmallModulus> {
                    DefaultParams.SmallMods40Bit(0), DefaultParams.SmallMods40Bit(1)
                };
            var context = new SEALContext(parms);
            var keygen = new KeyGenerator(context);
            var encoder = new BalancedEncoder(plain_modulus);

            var encryptor = new Encryptor(context, keygen.PublicKey);
            var evaluator = new Evaluator(context);
            var decryptor = new Decryptor(context, keygen.SecretKey);

            var encrypted = new Ciphertext();
            var plain = new Plaintext();

            encryptor.Encrypt(encoder.Encode(1), encrypted);
            evaluator.Square(encrypted);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual(1UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(0), encrypted);
            evaluator.Square(encrypted);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual(0UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(-5), encrypted);
            evaluator.Square(encrypted);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual(25UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(-1), encrypted);
            evaluator.Square(encrypted);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual(1UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(123), encrypted);
            evaluator.Square(encrypted);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual(15129UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(0x10000), encrypted);
            evaluator.Square(encrypted);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual(0x100000000UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);
        }

        [TestMethod]
        public void FVEncryptMultiplyManyDecryptNET()
        {
            var parms = new EncryptionParameters();
            var plain_modulus = new SmallModulus(1 << 6);
            parms.NoiseStandardDeviation = 3.19;
            parms.PlainModulus = plain_modulus;
            parms.PolyModulus = "1x^128 + 1";
            parms.CoeffModulus = new List<SmallModulus> {
                    DefaultParams.SmallMods40Bit(0), DefaultParams.SmallMods40Bit(1)
                };
            var context = new SEALContext(parms);
            var keygen = new KeyGenerator(context);
            var encoder = new BalancedEncoder(plain_modulus);

            var encryptor = new Encryptor(context, keygen.PublicKey);
            var evaluator = new Evaluator(context);
            var decryptor = new Decryptor(context, keygen.SecretKey);

            var evk = new EvaluationKeys();
            keygen.GenerateEvaluationKeys(4, evk);

            var encrypted1 = new Ciphertext();
            var encrypted2 = new Ciphertext();
            var encrypted3 = new Ciphertext();
            var encrypted4 = new Ciphertext();
            var product = new Ciphertext();
            var plain = new Plaintext();

            encryptor.Encrypt(encoder.Encode(5), encrypted1);
            encryptor.Encrypt(encoder.Encode(6), encrypted2);
            encryptor.Encrypt(encoder.Encode(7), encrypted3);
            var encrypteds = new List<Ciphertext> { encrypted1, encrypted2, encrypted3 };

            evaluator.MultiplyMany(encrypteds, evk, product);
            decryptor.Decrypt(product, plain);
            Assert.AreEqual(210UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted1.HashBlock, product.HashBlock);
            Assert.AreEqual(encrypted2.HashBlock, product.HashBlock);
            Assert.AreEqual(encrypted3.HashBlock, product.HashBlock);
            Assert.AreEqual(product.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(-9), encrypted1);
            encryptor.Encrypt(encoder.Encode(-17), encrypted2);
            encrypteds = new List<Ciphertext> { encrypted1, encrypted2 };
            evaluator.MultiplyMany(encrypteds, evk, product);
            decryptor.Decrypt(product, plain);
            Assert.AreEqual(153UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted1.HashBlock, product.HashBlock);
            Assert.AreEqual(encrypted2.HashBlock, product.HashBlock);
            Assert.AreEqual(product.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(2), encrypted1);
            encryptor.Encrypt(encoder.Encode(-31), encrypted2);
            encryptor.Encrypt(encoder.Encode(7), encrypted3);
            encrypteds = new List<Ciphertext> { encrypted1, encrypted2, encrypted3 };
            evaluator.MultiplyMany(encrypteds, evk, product);
            decryptor.Decrypt(product, plain);
            Assert.AreEqual(-434, encoder.DecodeInt64(plain));
            Assert.AreEqual(encrypted1.HashBlock, product.HashBlock);
            Assert.AreEqual(encrypted2.HashBlock, product.HashBlock);
            Assert.AreEqual(encrypted3.HashBlock, product.HashBlock);
            Assert.AreEqual(product.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(1), encrypted1);
            encryptor.Encrypt(encoder.Encode(-1), encrypted2);
            encryptor.Encrypt(encoder.Encode(1), encrypted3);
            encryptor.Encrypt(encoder.Encode(-1), encrypted4);
            encrypteds = new List<Ciphertext> { encrypted1, encrypted2, encrypted3, encrypted4 };
            evaluator.MultiplyMany(encrypteds, evk, product);
            decryptor.Decrypt(product, plain);
            Assert.AreEqual(1UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted1.HashBlock, product.HashBlock);
            Assert.AreEqual(encrypted2.HashBlock, product.HashBlock);
            Assert.AreEqual(encrypted3.HashBlock, product.HashBlock);
            Assert.AreEqual(encrypted4.HashBlock, product.HashBlock);
            Assert.AreEqual(product.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(98765), encrypted1);
            encryptor.Encrypt(encoder.Encode(0), encrypted2);
            encryptor.Encrypt(encoder.Encode(12345), encrypted3);
            encryptor.Encrypt(encoder.Encode(34567), encrypted4);
            encrypteds = new List<Ciphertext> { encrypted1, encrypted2, encrypted3, encrypted4 };
            evaluator.MultiplyMany(encrypteds, evk, product);
            decryptor.Decrypt(product, plain);
            Assert.AreEqual(0UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted1.HashBlock, product.HashBlock);
            Assert.AreEqual(encrypted2.HashBlock, product.HashBlock);
            Assert.AreEqual(encrypted3.HashBlock, product.HashBlock);
            Assert.AreEqual(encrypted4.HashBlock, product.HashBlock);
            Assert.AreEqual(product.HashBlock, parms.HashBlock);
        }

        [TestMethod]
        public void FVEncryptExponentiateDecryptNET()
        {
            var parms = new EncryptionParameters();
            var plain_modulus = new SmallModulus(1 << 6);
            parms.NoiseStandardDeviation = 3.19;
            parms.PlainModulus = plain_modulus;
            parms.PolyModulus = "1x^128 + 1";
            parms.CoeffModulus = new List<SmallModulus> {
                    DefaultParams.SmallMods40Bit(0), DefaultParams.SmallMods40Bit(1)
                };
            var context = new SEALContext(parms);
            var keygen = new KeyGenerator(context);
            var encoder = new BalancedEncoder(plain_modulus);

            var encryptor = new Encryptor(context, keygen.PublicKey);
            var evaluator = new Evaluator(context);
            var decryptor = new Decryptor(context, keygen.SecretKey);

            var evk = new EvaluationKeys();
            keygen.GenerateEvaluationKeys(4, evk);

            var encrypted = new Ciphertext();
            var plain = new Plaintext();

            encryptor.Encrypt(encoder.Encode(5), encrypted);
            evaluator.Exponentiate(encrypted, 1, evk);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual(5UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(7), encrypted);
            evaluator.Exponentiate(encrypted, 2, evk);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual(49UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(-7), encrypted);
            evaluator.Exponentiate(encrypted, 3, evk);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual(-343, encoder.DecodeInt64(plain));
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(0x100), encrypted);
            evaluator.Exponentiate(encrypted, 4, evk);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual(0x100000000UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);
        }

        [TestMethod]
        public void FVEncryptAddManyDecryptNET()
        {
            var parms = new EncryptionParameters();
            var plain_modulus = new SmallModulus(1 << 6);
            var poly_modulus = new BigPoly("1x^128 + 1");
            parms.NoiseStandardDeviation = 3.19;
            parms.PlainModulus = plain_modulus;
            parms.PolyModulus = poly_modulus;
            parms.CoeffModulus = new List<SmallModulus> {
                    DefaultParams.SmallMods40Bit(0), DefaultParams.SmallMods40Bit(1)
                };
            var context = new SEALContext(parms);
            var keygen = new KeyGenerator(context);
            var encoder = new BalancedEncoder(plain_modulus);

            var encryptor = new Encryptor(context, keygen.PublicKey);
            var evaluator = new Evaluator(context);
            var decryptor = new Decryptor(context, keygen.SecretKey);

            var evk = new EvaluationKeys();
            keygen.GenerateEvaluationKeys(4, evk);

            var encrypted1 = new Ciphertext();
            var encrypted2 = new Ciphertext();
            var encrypted3 = new Ciphertext();
            var encrypted4 = new Ciphertext();
            var sum = new Ciphertext();
            var plain = new Plaintext();

            encryptor.Encrypt(encoder.Encode(5), encrypted1);
            encryptor.Encrypt(encoder.Encode(6), encrypted2);
            encryptor.Encrypt(encoder.Encode(7), encrypted3);
            var encrypteds = new List<Ciphertext> { encrypted1, encrypted2, encrypted3 };

            evaluator.AddMany(encrypteds, sum);
            decryptor.Decrypt(sum, plain);
            Assert.AreEqual(18UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted1.HashBlock, sum.HashBlock);
            Assert.AreEqual(encrypted2.HashBlock, sum.HashBlock);
            Assert.AreEqual(encrypted3.HashBlock, sum.HashBlock);
            Assert.AreEqual(sum.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(-9), encrypted1);
            encryptor.Encrypt(encoder.Encode(-17), encrypted2);
            encrypteds = new List<Ciphertext> { encrypted1, encrypted2, };
            evaluator.AddMany(encrypteds, sum);
            decryptor.Decrypt(sum, plain);
            Assert.AreEqual(-26, encoder.DecodeInt64(plain));
            Assert.AreEqual(encrypted1.HashBlock, sum.HashBlock);
            Assert.AreEqual(encrypted2.HashBlock, sum.HashBlock);
            Assert.AreEqual(sum.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(2), encrypted1);
            encryptor.Encrypt(encoder.Encode(-31), encrypted2);
            encryptor.Encrypt(encoder.Encode(7), encrypted3);
            encrypteds = new List<Ciphertext> { encrypted1, encrypted2, encrypted3 };
            evaluator.AddMany(encrypteds, sum);
            decryptor.Decrypt(sum, plain);
            Assert.AreEqual(-22, encoder.DecodeInt64(plain));
            Assert.AreEqual(encrypted1.HashBlock, sum.HashBlock);
            Assert.AreEqual(encrypted2.HashBlock, sum.HashBlock);
            Assert.AreEqual(encrypted3.HashBlock, sum.HashBlock);
            Assert.AreEqual(sum.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(1), encrypted1);
            encryptor.Encrypt(encoder.Encode(-1), encrypted2);
            encryptor.Encrypt(encoder.Encode(1), encrypted3);
            encryptor.Encrypt(encoder.Encode(-1), encrypted4);
            encrypteds = new List<Ciphertext> { encrypted1, encrypted2, encrypted3, encrypted4 };
            evaluator.AddMany(encrypteds, sum);
            decryptor.Decrypt(sum, plain);
            Assert.AreEqual(0UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted1.HashBlock, sum.HashBlock);
            Assert.AreEqual(encrypted2.HashBlock, sum.HashBlock);
            Assert.AreEqual(encrypted3.HashBlock, sum.HashBlock);
            Assert.AreEqual(encrypted4.HashBlock, sum.HashBlock);
            Assert.AreEqual(sum.HashBlock, parms.HashBlock);

            encryptor.Encrypt(encoder.Encode(98765), encrypted1);
            encryptor.Encrypt(encoder.Encode(0), encrypted2);
            encryptor.Encrypt(encoder.Encode(12345), encrypted3);
            encryptor.Encrypt(encoder.Encode(34567), encrypted4);
            encrypteds = new List<Ciphertext> { encrypted1, encrypted2, encrypted3, encrypted4 };
            evaluator.AddMany(encrypteds, sum);
            decryptor.Decrypt(sum, plain);
            Assert.AreEqual(145677UL, encoder.DecodeUInt64(plain));
            Assert.AreEqual(encrypted1.HashBlock, sum.HashBlock);
            Assert.AreEqual(encrypted2.HashBlock, sum.HashBlock);
            Assert.AreEqual(encrypted3.HashBlock, sum.HashBlock);
            Assert.AreEqual(encrypted4.HashBlock, sum.HashBlock);
            Assert.AreEqual(sum.HashBlock, parms.HashBlock);

            var frac_encoder = new BalancedFractionalEncoder(plain_modulus, poly_modulus, 10, 15);
            encryptor.Encrypt(frac_encoder.Encode(3.1415), encrypted1);
            encryptor.Encrypt(frac_encoder.Encode(12.345), encrypted2);
            encryptor.Encrypt(frac_encoder.Encode(98.765), encrypted3);
            encryptor.Encrypt(frac_encoder.Encode(1.1111), encrypted4);
            encrypteds = new List<Ciphertext> { encrypted1, encrypted2, encrypted3, encrypted4 };
            evaluator.AddMany(encrypteds, sum);
            decryptor.Decrypt(sum, plain);
            Assert.IsTrue(Math.Abs(frac_encoder.Decode(plain) - 115.3626) < 0.000001);
            Assert.AreEqual(encrypted1.HashBlock, sum.HashBlock);
            Assert.AreEqual(encrypted2.HashBlock, sum.HashBlock);
            Assert.AreEqual(encrypted3.HashBlock, sum.HashBlock);
            Assert.AreEqual(encrypted4.HashBlock, sum.HashBlock);
            Assert.AreEqual(sum.HashBlock, parms.HashBlock);
        }

        [TestMethod]
        public void TransformPlainToNTTNET()
        {
            var parms = new EncryptionParameters();
            var plain_modulus = new SmallModulus(1 << 6);
            parms.NoiseStandardDeviation = 3.19;
            parms.PlainModulus = plain_modulus;
            parms.PolyModulus = "1x^128 + 1";
            parms.CoeffModulus = new List<SmallModulus> {
                    DefaultParams.SmallMods40Bit(0), DefaultParams.SmallMods40Bit(1)
                };
            var context = new SEALContext(parms);
            var keygen = new KeyGenerator(context);
            var evaluator = new Evaluator(context);

            var plain = new Plaintext("0");
            evaluator.TransformToNTT(plain);
            Assert.IsTrue(plain.ToString().Equals("0"));

            plain.Release();
            plain.Set("1");
            evaluator.TransformToNTT(plain);
            for (int i = 0; i < 128; i++)
            {
                Assert.IsTrue(plain[i] == 1);
            }
            Assert.IsTrue(plain[128] == 0);

            plain.Release();
            plain.Set("2");
            evaluator.TransformToNTT(plain);
            for (int i = 0; i < 128; i++)
            {
                Assert.IsTrue(plain[i] == 2);
            }
            Assert.IsTrue(plain[128] == 0);
        }

        [TestMethod]
        public void TransformEncryptedToFromNTTNET()
        {
            var parms = new EncryptionParameters();
            var plain_modulus = new SmallModulus(1 << 6);
            parms.NoiseStandardDeviation = 3.19;
            parms.PlainModulus = plain_modulus;
            parms.PolyModulus = "1x^128 + 1";
            parms.CoeffModulus = new List<SmallModulus> {
                    DefaultParams.SmallMods40Bit(0), DefaultParams.SmallMods40Bit(1)
                };
            var context = new SEALContext(parms);
            var keygen = new KeyGenerator(context);

            var encryptor = new Encryptor(context, keygen.PublicKey);
            var evaluator = new Evaluator(context);
            var decryptor = new Decryptor(context, keygen.SecretKey);

            var plain = new Plaintext();
            var encrypted = new Ciphertext();

            plain.Set("0");
            encryptor.Encrypt(plain, encrypted);
            evaluator.TransformToNTT(encrypted);
            evaluator.TransformFromNTT(encrypted);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual("0", plain.ToString());
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

            plain.Set("1");
            encryptor.Encrypt(plain, encrypted);
            evaluator.TransformToNTT(encrypted);
            evaluator.TransformFromNTT(encrypted);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual("1", plain.ToString());
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

            plain.Set("Fx^10 + Ex^9 + Dx^8 + Cx^7 + Bx^6 + Ax^5 + 1x^4 + 2x^3 + 3x^2 + 4x^1 + 5");
            encryptor.Encrypt(plain, encrypted);
            evaluator.TransformToNTT(encrypted);
            evaluator.TransformFromNTT(encrypted);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual("Fx^10 + Ex^9 + Dx^8 + Cx^7 + Bx^6 + Ax^5 + 1x^4 + 2x^3 + 3x^2 + 4x^1 + 5", plain.ToString());
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);
        }

        [TestMethod]
        public void FVEncryptMultiplyPlainNTTDecrypt()
        {
            var parms = new EncryptionParameters();
            var plain_modulus = new SmallModulus(1 << 6);
            parms.NoiseStandardDeviation = 3.19;
            parms.PlainModulus = plain_modulus;
            parms.PolyModulus = "1x^128 + 1";
            parms.CoeffModulus = new List<SmallModulus> {
                    DefaultParams.SmallMods40Bit(0), DefaultParams.SmallMods40Bit(1)
                };
            var context = new SEALContext(parms);
            var keygen = new KeyGenerator(context);

            var encryptor = new Encryptor(context, keygen.PublicKey);
            var evaluator = new Evaluator(context);
            var decryptor = new Decryptor(context, keygen.SecretKey);

            var plain = new Plaintext();
            var plain_multiplier = new Plaintext();
            var encrypted = new Ciphertext();

            plain.Set(0);
            encryptor.Encrypt(plain, encrypted);
            evaluator.TransformToNTT(encrypted);
            plain_multiplier.Set(1);
            evaluator.TransformToNTT(plain_multiplier);
            evaluator.MultiplyPlainNTT(encrypted, plain_multiplier);
            evaluator.TransformFromNTT(encrypted);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual("0", plain.ToString());
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

            plain.Set(2);
            encryptor.Encrypt(plain, encrypted);
            evaluator.TransformToNTT(encrypted);
            plain_multiplier.Release();
            plain_multiplier.Set(3);
            evaluator.TransformToNTT(plain_multiplier);
            evaluator.MultiplyPlainNTT(encrypted, plain_multiplier);
            evaluator.TransformFromNTT(encrypted);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual("6", plain.ToString());
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

            plain.Set(1);
            encryptor.Encrypt(plain, encrypted);
            evaluator.TransformToNTT(encrypted);
            plain_multiplier.Release();
            plain_multiplier.Set("Fx^10 + Ex^9 + Dx^8 + Cx^7 + Bx^6 + Ax^5 + 1x^4 + 2x^3 + 3x^2 + 4x^1 + 5");
            evaluator.TransformToNTT(plain_multiplier);
            evaluator.MultiplyPlainNTT(encrypted, plain_multiplier);
            evaluator.TransformFromNTT(encrypted);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual("Fx^10 + Ex^9 + Dx^8 + Cx^7 + Bx^6 + Ax^5 + 1x^4 + 2x^3 + 3x^2 + 4x^1 + 5", plain.ToString());
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);

            plain.Set("1x^20");
            encryptor.Encrypt(plain, encrypted);
            evaluator.TransformToNTT(encrypted);
            plain_multiplier.Release();
            plain_multiplier.Set("Fx^10 + Ex^9 + Dx^8 + Cx^7 + Bx^6 + Ax^5 + 1x^4 + 2x^3 + 3x^2 + 4x^1 + 5");
            evaluator.TransformToNTT(plain_multiplier);
            evaluator.MultiplyPlainNTT(encrypted, plain_multiplier);
            evaluator.TransformFromNTT(encrypted);
            decryptor.Decrypt(encrypted, plain);
            Assert.AreEqual("Fx^30 + Ex^29 + Dx^28 + Cx^27 + Bx^26 + Ax^25 + 1x^24 + 2x^23 + 3x^22 + 4x^21 + 5x^20", plain.ToString());
            Assert.AreEqual(encrypted.HashBlock, parms.HashBlock);
        }

        [TestMethod]
        public void FVEncryptRotateMatrixDecryptNET()
        {
            var parms = new EncryptionParameters();
            var plain_modulus = new SmallModulus(257);
            parms.NoiseStandardDeviation = 3.19;
            parms.PlainModulus = plain_modulus;
            parms.PolyModulus = "1x^8 + 1";
            parms.CoeffModulus = new List<SmallModulus> {
                    DefaultParams.SmallMods40Bit(0), DefaultParams.SmallMods40Bit(1)
                };
            var context = new SEALContext(parms);
            var keygen = new KeyGenerator(context);
            var glk = new GaloisKeys();
            keygen.GenerateGaloisKeys(24, glk);

            var encryptor = new Encryptor(context, keygen.PublicKey);
            var evaluator = new Evaluator(context);
            var decryptor = new Decryptor(context, keygen.SecretKey);
            var crtbuilder = new PolyCRTBuilder(context);

            var plain = new Plaintext();
            var plain_vec = new List<UInt64> {
                1, 2, 3, 4,
                5, 6, 7, 8
            };
            crtbuilder.Compose(plain_vec, plain);
            var encrypted = new Ciphertext();
            encryptor.Encrypt(plain, encrypted);

            evaluator.RotateColumns(encrypted, glk);
            decryptor.Decrypt(encrypted, plain);
            crtbuilder.Decompose(plain, plain_vec);
            Assert.IsTrue(plain_vec.SequenceEqual(new List<UInt64> {
                5, 6, 7, 8,
                1, 2, 3, 4
            }));

            evaluator.RotateRows(encrypted, -1, glk);
            decryptor.Decrypt(encrypted, plain);
            crtbuilder.Decompose(plain, plain_vec);
            Assert.IsTrue(plain_vec.SequenceEqual(new List<UInt64> {
                8, 5, 6, 7,
                4, 1, 2, 3
            }));

            evaluator.RotateRows(encrypted, 2, glk);
            decryptor.Decrypt(encrypted, plain);
            crtbuilder.Decompose(plain, plain_vec);
            Assert.IsTrue(plain_vec.SequenceEqual(new List<UInt64> {
                6, 7, 8, 5,
                2, 3, 4, 1
            }));

            evaluator.RotateColumns(encrypted, glk);
            decryptor.Decrypt(encrypted, plain);
            crtbuilder.Decompose(plain, plain_vec);
            Assert.IsTrue(plain_vec.SequenceEqual(new List<UInt64> {
                2, 3, 4, 1,
                6, 7, 8, 5
            }));

            evaluator.RotateRows(encrypted, 0, glk);
            decryptor.Decrypt(encrypted, plain);
            crtbuilder.Decompose(plain, plain_vec);
            Assert.IsTrue(plain_vec.SequenceEqual(new List<UInt64> {
                2, 3, 4, 1,
                6, 7, 8, 5
            }));
        }
    }
}