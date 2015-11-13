using Microsoft.Research.SEAL;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Collections.Generic;

namespace SEALNETTest
{
    [TestClass]
    public class EvaluatorWrapper
    {
        [TestMethod]
        public void EncryptNegateDecryptNET()
        {
            var parms = new EncryptionParameters
            {
                DecompositionBitCount = 4,
                NoiseStandardDeviation = 3.19,
                NoiseMaxDeviation = 35.06
            };
            var coeffModulus = parms.CoeffModulus;
            coeffModulus.Resize(48);
            coeffModulus.Set("FFFFFFFFC001");
            var plainModulus = parms.PlainModulus;
            plainModulus.Resize(7);
            plainModulus.Set(1 << 6);
            var polyModulus = parms.PolyModulus;
            polyModulus.Resize(64, 1);
            polyModulus[0].Set(1);
            polyModulus[63].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms, keygen.EvaluationKeys);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted = encryptor.Encrypt(Encoder.Encode(0x12345678));
            var negated = evaluator.Negate(encrypted);
            Assert.AreEqual(-0x12345678, Encoder.DecodeInt32(decryptor.Decrypt(negated)));

            encrypted = encryptor.Encrypt(Encoder.Encode(0));
            negated = evaluator.Negate(encrypted);
            Assert.AreEqual(0, Encoder.DecodeInt32(decryptor.Decrypt(negated)));

            encrypted = encryptor.Encrypt(Encoder.Encode(1));
            negated = evaluator.Negate(encrypted);
            Assert.AreEqual(-1, Encoder.DecodeInt32(decryptor.Decrypt(negated)));

            encrypted = encryptor.Encrypt(Encoder.Encode(-1));
            negated = evaluator.Negate(encrypted);
            Assert.AreEqual(1, Encoder.DecodeInt32(decryptor.Decrypt(negated)));

            encrypted = encryptor.Encrypt(Encoder.Encode(2));
            negated = evaluator.Negate(encrypted);
            Assert.AreEqual(-2, Encoder.DecodeInt32(decryptor.Decrypt(negated)));

            encrypted = encryptor.Encrypt(Encoder.Encode(-5));
            negated = evaluator.Negate(encrypted);
            Assert.AreEqual(5, Encoder.DecodeInt32(decryptor.Decrypt(negated)));
        }

        [TestMethod]
        public void EncryptAddDecryptNET()
        {
            var parms = new EncryptionParameters
            {
                DecompositionBitCount = 4,
                NoiseStandardDeviation = 3.19,
                NoiseMaxDeviation = 35.06
            };
            var coeffModulus = parms.CoeffModulus;
            coeffModulus.Resize(48);
            coeffModulus.Set("FFFFFFFFC001");
            var plainModulus = parms.PlainModulus;
            plainModulus.Resize(7);
            plainModulus.Set(1 << 6);
            var polyModulus = parms.PolyModulus;
            polyModulus.Resize(64, 1);
            polyModulus[0].Set(1);
            polyModulus[63].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms, keygen.EvaluationKeys);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted1 = encryptor.Encrypt(Encoder.Encode(0x12345678));
            var encrypted2 = encryptor.Encrypt(Encoder.Encode(0x54321));
            var sum = evaluator.Add(encrypted1, encrypted2);
            Assert.AreEqual(0x12399999UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(0));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(0));
            sum = evaluator.Add(encrypted1, encrypted2);
            Assert.AreEqual(0UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(0));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(5));
            sum = evaluator.Add(encrypted1, encrypted2);
            Assert.AreEqual(5UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(5));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(-3));
            sum = evaluator.Add(encrypted1, encrypted2);
            Assert.AreEqual(2, Encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(-7));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(2));
            sum = evaluator.Add(encrypted1, encrypted2);
            Assert.AreEqual(-5, Encoder.DecodeInt32(decryptor.Decrypt(sum)));
        }

        [TestMethod]
        public void EncryptAddPlainDecryptNET()
        {
            var parms = new EncryptionParameters
            {
                DecompositionBitCount = 4,
                NoiseStandardDeviation = 3.19,
                NoiseMaxDeviation = 35.06
            };
            var coeffModulus = parms.CoeffModulus;
            coeffModulus.Resize(48);
            coeffModulus.Set("FFFFFFFFC001");
            var plainModulus = parms.PlainModulus;
            plainModulus.Resize(7);
            plainModulus.Set(1 << 6);
            var polyModulus = parms.PolyModulus;
            polyModulus.Resize(64, 1);
            polyModulus[0].Set(1);
            polyModulus[63].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms, keygen.EvaluationKeys);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted1 = encryptor.Encrypt(Encoder.Encode(0x12345678));
            var plain2 = Encoder.Encode(0x54321);
            var sum = evaluator.AddPlain(encrypted1, plain2);
            Assert.AreEqual(0x12399999UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(0));
            plain2 = Encoder.Encode(0);
            sum = evaluator.AddPlain(encrypted1, plain2);
            Assert.AreEqual(0UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(0));
            plain2 = Encoder.Encode(5);
            sum = evaluator.AddPlain(encrypted1, plain2);
            Assert.AreEqual(5UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(5));
            plain2 = Encoder.Encode(-3);
            sum = evaluator.AddPlain(encrypted1, plain2);
            Assert.AreEqual(2, Encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(-7));
            plain2 = Encoder.Encode(2);
            sum = evaluator.AddPlain(encrypted1, plain2);
            Assert.AreEqual(-5, Encoder.DecodeInt32(decryptor.Decrypt(sum)));
        }

        [TestMethod]
        public void EncryptSubDecryptNET()
        {
            var parms = new EncryptionParameters
            {
                DecompositionBitCount = 4,
                NoiseStandardDeviation = 3.19,
                NoiseMaxDeviation = 35.06
            };
            var coeffModulus = parms.CoeffModulus;
            coeffModulus.Resize(48);
            coeffModulus.Set("FFFFFFFFC001");
            var plainModulus = parms.PlainModulus;
            plainModulus.Resize(7);
            plainModulus.Set(1 << 6);
            var polyModulus = parms.PolyModulus;
            polyModulus.Resize(64, 1);
            polyModulus[0].Set(1);
            polyModulus[63].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms, keygen.EvaluationKeys);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted1 = encryptor.Encrypt(Encoder.Encode(0x12345678));
            var encrypted2 = encryptor.Encrypt(Encoder.Encode(0x54321));
            var sum = evaluator.Sub(encrypted1, encrypted2);
            Assert.AreEqual(0x122F1357, Encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(0));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(0));
            sum = evaluator.Sub(encrypted1, encrypted2);
            Assert.AreEqual(0, Encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(0));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(5));
            sum = evaluator.Sub(encrypted1, encrypted2);
            Assert.AreEqual(-5, Encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(5));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(-3));
            sum = evaluator.Sub(encrypted1, encrypted2);
            Assert.AreEqual(8, Encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(-7));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(2));
            sum = evaluator.Sub(encrypted1, encrypted2);
            Assert.AreEqual(-9, Encoder.DecodeInt32(decryptor.Decrypt(sum)));
        }

        [TestMethod]
        public void EncryptSubPlainDecryptNET()
        {
            var parms = new EncryptionParameters
            {
                DecompositionBitCount = 4,
                NoiseStandardDeviation = 3.19,
                NoiseMaxDeviation = 35.06
            };
            var coeffModulus = parms.CoeffModulus;
            coeffModulus.Resize(48);
            coeffModulus.Set("FFFFFFFFC001");
            var plainModulus = parms.PlainModulus;
            plainModulus.Resize(7);
            plainModulus.Set(1 << 6);
            var polyModulus = parms.PolyModulus;
            polyModulus.Resize(64, 1);
            polyModulus[0].Set(1);
            polyModulus[63].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms, keygen.EvaluationKeys);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted1 = encryptor.Encrypt(Encoder.Encode(0x12345678));
            var plain2 = Encoder.Encode(0x54321);
            var sum = evaluator.SubPlain(encrypted1, plain2);
            Assert.AreEqual(0x122F1357, Encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(0));
            plain2 = Encoder.Encode(0);
            sum = evaluator.SubPlain(encrypted1, plain2);
            Assert.AreEqual(0, Encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(0));
            plain2 = Encoder.Encode(5);
            sum = evaluator.SubPlain(encrypted1, plain2);
            Assert.AreEqual(-5, Encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(5));
            plain2 = Encoder.Encode(-3);
            sum = evaluator.SubPlain(encrypted1, plain2);
            Assert.AreEqual(8, Encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(-7));
            plain2 = Encoder.Encode(2);
            sum = evaluator.SubPlain(encrypted1, plain2);
            Assert.AreEqual(-9, Encoder.DecodeInt32(decryptor.Decrypt(sum)));
        }

        [TestMethod]
        public void EncryptMultiplyDecryptNET()
        {
            var parms = new EncryptionParameters
            {
                DecompositionBitCount = 4,
                NoiseStandardDeviation = 3.19,
                NoiseMaxDeviation = 35.06
            };
            var coeffModulus = parms.CoeffModulus;
            coeffModulus.Resize(48);
            coeffModulus.Set("FFFFFFFFC001");
            var plainModulus = parms.PlainModulus;
            plainModulus.Resize(7);
            plainModulus.Set(1 << 6);
            var polyModulus = parms.PolyModulus;
            polyModulus.Resize(64, 1);
            polyModulus[0].Set(1);
            polyModulus[63].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms, keygen.EvaluationKeys);
            var keygenEvals = keygen.EvaluationKeys;
            var evaluatorEvals = keygen.EvaluationKeys;
            for (int i = 0; i < keygen.EvaluationKeys.Count; ++i)
            {
                Assert.AreEqual(keygenEvals[i], evaluatorEvals[i]);
            }
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted1 = encryptor.Encrypt(Encoder.Encode(0x12345678));
            var encrypted2 = encryptor.Encrypt(Encoder.Encode(0x54321));
            var sum = evaluator.Multiply(encrypted1, encrypted2);
            Assert.AreEqual(0x5FCBBBB88D78UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(0));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(0));
            sum = evaluator.Multiply(encrypted1, encrypted2);
            Assert.AreEqual(0UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(0));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(5));
            sum = evaluator.Multiply(encrypted1, encrypted2);
            Assert.AreEqual(0UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(7));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(1));
            sum = evaluator.Multiply(encrypted1, encrypted2);
            Assert.AreEqual(7UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(5));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(-3));
            sum = evaluator.Multiply(encrypted1, encrypted2);
            Assert.AreEqual(-15, Encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(-7));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(2));
            sum = evaluator.Multiply(encrypted1, encrypted2);
            Assert.AreEqual(-14, Encoder.DecodeInt32(decryptor.Decrypt(sum)));
        }

        [TestMethod]
        public void EncryptMultiplyPlainDecryptNET()
        {
            var parms = new EncryptionParameters
            {
                DecompositionBitCount = 4,
                NoiseStandardDeviation = 3.19,
                NoiseMaxDeviation = 35.06
            };
            var coeffModulus = parms.CoeffModulus;
            coeffModulus.Resize(48);
            coeffModulus.Set("FFFFFFFFC001");
            var plainModulus = parms.PlainModulus;
            plainModulus.Resize(7);
            plainModulus.Set(1 << 6);
            var polyModulus = parms.PolyModulus;
            polyModulus.Resize(64, 1);
            polyModulus[0].Set(1);
            polyModulus[63].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms, keygen.EvaluationKeys);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted1 = encryptor.Encrypt(Encoder.Encode(0x12345678));
            var plain2 = Encoder.Encode(0x54321);
            var sum = evaluator.MultiplyPlain(encrypted1, plain2);
            Assert.AreEqual(0x5FCBBBB88D78UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(0));
            plain2 = Encoder.Encode(0);
            sum = evaluator.MultiplyPlain(encrypted1, plain2);
            Assert.AreEqual(0UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(0));
            plain2 = Encoder.Encode(5);
            sum = evaluator.MultiplyPlain(encrypted1, plain2);
            Assert.AreEqual(0UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(7));
            plain2 = Encoder.Encode(1);
            sum = evaluator.MultiplyPlain(encrypted1, plain2);
            Assert.AreEqual(7UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(5));
            plain2 = Encoder.Encode(-3);
            sum = evaluator.MultiplyPlain(encrypted1, plain2);
            Assert.AreEqual(-15, Encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(-7));
            plain2 = Encoder.Encode(2);
            sum = evaluator.MultiplyPlain(encrypted1, plain2);
            Assert.AreEqual(-14, Encoder.DecodeInt32(decryptor.Decrypt(sum)));
        }

        public void EncryptMultiplyPlain2DecryptNET()
        {
            var parms = new EncryptionParameters
            {
                DecompositionBitCount = 4,
                NoiseStandardDeviation = 3.19,
                NoiseMaxDeviation = 35.06
            };
            var coeffModulus = parms.CoeffModulus;
            coeffModulus.Resize(48);
            coeffModulus.Set("FFFFFFFFC001");
            var plainModulus = parms.PlainModulus;
            plainModulus.Resize(7);
            plainModulus.Set(1 << 6);
            var polyModulus = parms.PolyModulus;
            polyModulus.Resize(64, 1);
            polyModulus[0].Set(1);
            polyModulus[63].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms, keygen.EvaluationKeys);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted1 = encryptor.Encrypt(Encoder.Encode(0x12345678));
            var plain2 = Encoder.Encode(0x54321);
            var sum = evaluator.MultiplyPlain(encrypted1, plain2);
            Assert.AreEqual(0x5FCBBBB88D78UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(0));
            plain2 = Encoder.Encode(0);
            sum = evaluator.MultiplyPlain(encrypted1, plain2);
            Assert.AreEqual(0UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(0));
            plain2 = Encoder.Encode(5);
            sum = evaluator.MultiplyPlain(encrypted1, plain2);
            Assert.AreEqual(0UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(7));
            plain2 = Encoder.Encode(1);
            sum = evaluator.MultiplyPlain(encrypted1, plain2);
            Assert.AreEqual(7UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(5));
            plain2 = Encoder.Encode(-3);
            sum = evaluator.MultiplyPlain(encrypted1, plain2);
            Assert.AreEqual(-15, Encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(-7));
            plain2 = Encoder.Encode(2);
            sum = evaluator.MultiplyPlain(encrypted1, plain2);
            Assert.AreEqual(-14, Encoder.DecodeInt32(decryptor.Decrypt(sum)));
        }

        [TestMethod]
        public void EncryptTreeMultiplyDecryptNET()
        {
            var parms = new EncryptionParameters
            {
                DecompositionBitCount = 4,
                NoiseStandardDeviation = 3.19,
                NoiseMaxDeviation = 35.06
            };
            var coeffModulus = parms.CoeffModulus;
            coeffModulus.Resize(48);
            coeffModulus.Set("FFFFFFFFC001");
            var plainModulus = parms.PlainModulus;
            plainModulus.Resize(7);
            plainModulus.Set(1 << 4);
            var polyModulus = parms.PolyModulus;
            polyModulus.Resize(64, 1);
            polyModulus[0].Set(1);
            polyModulus[63].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms, keygen.EvaluationKeys);
            var keygenEvals = keygen.EvaluationKeys;
            var evaluatorEvals = keygen.EvaluationKeys;
            for (int i = 0; i < keygen.EvaluationKeys.Count; ++i)
            {
                Assert.AreEqual(keygenEvals[i], evaluatorEvals[i]);
            }
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted1 = encryptor.Encrypt(Encoder.Encode(2));
            var encrypted2 = encryptor.Encrypt(Encoder.Encode(-3));
            var encrypted3 = encryptor.Encrypt(Encoder.Encode(4));
            var encrypteds = new List<BigPoly>() { encrypted1, encrypted2, encrypted3 };
			var product = evaluator.TreeMultiply(encrypteds);
            Assert.AreEqual(-24, Encoder.DecodeInt32(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(-9));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(-17));
            encrypteds = new List<BigPoly>() { encrypted1, encrypted2 };
            product = evaluator.TreeMultiply(encrypteds);
            Assert.AreEqual(153, Encoder.DecodeInt32(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(2));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(-31));
            encrypted3 = encryptor.Encrypt(Encoder.Encode(7));
            encrypteds = new List<BigPoly>() { encrypted1, encrypted2, encrypted3 };
            product = evaluator.TreeMultiply(encrypteds);
            Assert.AreEqual(-434, Encoder.DecodeInt32(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(1));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(-1));
            encrypted3 = encryptor.Encrypt(Encoder.Encode(1));
            var encrypted4 = encryptor.Encrypt(Encoder.Encode(-1));
            encrypteds = new List<BigPoly>() { encrypted1, encrypted2, encrypted3, encrypted4 };
            product = evaluator.TreeMultiply(encrypteds);
            Assert.AreEqual(1, Encoder.DecodeInt32(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(98765));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(0));
            encrypted3 = encryptor.Encrypt(Encoder.Encode(12345));
            encrypted4 = encryptor.Encrypt(Encoder.Encode(34567));
            encrypteds = new List<BigPoly>() { encrypted1, encrypted2, encrypted3, encrypted4 };
            product = evaluator.TreeMultiply(encrypteds);
            Assert.AreEqual(0, Encoder.DecodeInt32(decryptor.Decrypt(product)));
        }

        [TestMethod]
        public void EncryptTreeExponentiateDecryptNET()
        {
            var parms = new EncryptionParameters
            {
                DecompositionBitCount = 4,
                NoiseStandardDeviation = 3.19,
                NoiseMaxDeviation = 35.06
            };
            var coeffModulus = parms.CoeffModulus;
            coeffModulus.Resize(48);
            coeffModulus.Set("FFFFFFFFC001");
            var plainModulus = parms.PlainModulus;
            plainModulus.Resize(7);
            plainModulus.Set(1 << 4);
            var polyModulus = parms.PolyModulus;
            polyModulus.Resize(64, 1);
            polyModulus[0].Set(1);
            polyModulus[63].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms, keygen.EvaluationKeys);
            var keygenEvals = keygen.EvaluationKeys;
            var evaluatorEvals = keygen.EvaluationKeys;
            for (int i = 0; i < keygen.EvaluationKeys.Count; ++i)
            {
                Assert.AreEqual(keygenEvals[i], evaluatorEvals[i]);
            }
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted = encryptor.Encrypt(Encoder.Encode(5));
			var power = evaluator.TreeExponentiate(encrypted, 1);
			Assert.AreEqual(5, Encoder.DecodeInt32(decryptor.Decrypt(power)));

            encrypted = encryptor.Encrypt(Encoder.Encode(5));
			power = evaluator.TreeExponentiate(encrypted, 0);
			Assert.AreEqual(1, Encoder.DecodeInt32(decryptor.Decrypt(power)));

            encrypted = encryptor.Encrypt(Encoder.Encode(5));
			power = evaluator.TreeExponentiate(encrypted, 0);
			Assert.AreEqual(1, Encoder.DecodeInt32(decryptor.Decrypt(power)));

            encrypted = encryptor.Encrypt(Encoder.Encode(7));
			power = evaluator.TreeExponentiate(encrypted, 2);
			Assert.AreEqual(49, Encoder.DecodeInt32(decryptor.Decrypt(power)));

            encrypted = encryptor.Encrypt(Encoder.Encode(-7));
			power = evaluator.TreeExponentiate(encrypted, 3);
			Assert.AreEqual(-343, Encoder.DecodeInt32(decryptor.Decrypt(power)));
        }

        [TestMethod]
        public void EncryptBinaryExponentiateDecryptNET()
        {
            var parms = new EncryptionParameters
            {
                DecompositionBitCount = 4,
                NoiseStandardDeviation = 3.19,
                NoiseMaxDeviation = 35.06
            };
            var coeffModulus = parms.CoeffModulus;
            coeffModulus.Resize(48);
            coeffModulus.Set("FFFFFFFFC001");
            var plainModulus = parms.PlainModulus;
            plainModulus.Resize(7);
            plainModulus.Set(1 << 4);
            var polyModulus = parms.PolyModulus;
            polyModulus.Resize(64, 1);
            polyModulus[0].Set(1);
            polyModulus[63].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms, keygen.EvaluationKeys);
            var keygenEvals = keygen.EvaluationKeys;
            var evaluatorEvals = keygen.EvaluationKeys;
            for (int i = 0; i < keygen.EvaluationKeys.Count; ++i)
            {
                Assert.AreEqual(keygenEvals[i], evaluatorEvals[i]);
            }
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted = encryptor.Encrypt(Encoder.Encode(5));
            var power = evaluator.BinaryExponentiate(encrypted, 1);
            Assert.AreEqual(5, Encoder.DecodeInt32(decryptor.Decrypt(power)));

            encrypted = encryptor.Encrypt(Encoder.Encode(5));
            power = evaluator.BinaryExponentiate(encrypted, 0);
            Assert.AreEqual(1, Encoder.DecodeInt32(decryptor.Decrypt(power)));

            encrypted = encryptor.Encrypt(Encoder.Encode(5));
            power = evaluator.BinaryExponentiate(encrypted, 0);
            Assert.AreEqual(1, Encoder.DecodeInt32(decryptor.Decrypt(power)));

            encrypted = encryptor.Encrypt(Encoder.Encode(7));
            power = evaluator.BinaryExponentiate(encrypted, 2);
            Assert.AreEqual(49, Encoder.DecodeInt32(decryptor.Decrypt(power)));

            encrypted = encryptor.Encrypt(Encoder.Encode(-7));
            power = evaluator.BinaryExponentiate(encrypted, 3);
            Assert.AreEqual(-343, Encoder.DecodeInt32(decryptor.Decrypt(power)));
        }
        

        [TestMethod]
        public void EvaluateTestModeNET()
        {
            var parms = new EncryptionParameters
            {
                DecompositionBitCount = 4,
                NoiseStandardDeviation = 3.19,
                NoiseMaxDeviation = 35.06,
                Mode = EncryptionMode.Test
            };
            var coeffModulus = parms.CoeffModulus;
            coeffModulus.Resize(48);
            coeffModulus.Set("FFFFFFFFC001");
            var plainModulus = parms.PlainModulus;
            plainModulus.Resize(7);
            plainModulus.Set(1 << 6);
            var polyModulus = parms.PolyModulus;
            polyModulus.Resize(64, 1);
            polyModulus[0].Set(1);
            polyModulus[63].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms, keygen.EvaluationKeys);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted = encryptor.Encrypt(Encoder.Encode(0x12345678));
            var negated = evaluator.Negate(encrypted);
            Assert.AreEqual(-0x12345678, Encoder.DecodeInt32(negated));
            Assert.AreEqual(-0x12345678, Encoder.DecodeInt32(decryptor.Decrypt(negated)));

            var plain2 = Encoder.Encode(0x54321);
            var encrypted2 = encryptor.Encrypt(plain2);
            var sum = evaluator.Add(encrypted, encrypted2);
            Assert.AreEqual(0x12399999UL, Encoder.DecodeUInt64(sum));
            Assert.AreEqual(0x12399999UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            sum = evaluator.AddPlain(encrypted, plain2);
            Assert.AreEqual(0x12399999UL, Encoder.DecodeUInt64(sum));
            Assert.AreEqual(0x12399999UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            var diff = evaluator.Sub(encrypted, encrypted2);
            Assert.AreEqual(0x122F1357, Encoder.DecodeInt32(diff));
            Assert.AreEqual(0x122F1357, Encoder.DecodeInt32(decryptor.Decrypt(diff)));

            diff = evaluator.SubPlain(encrypted, plain2);
            Assert.AreEqual(0x122F1357, Encoder.DecodeInt32(diff));
            Assert.AreEqual(0x122F1357, Encoder.DecodeInt32(decryptor.Decrypt(diff)));

            var prod = evaluator.Multiply(encrypted, encrypted2);
            Assert.AreEqual(0x5FCBBBB88D78UL, Encoder.DecodeUInt64(prod));
            Assert.AreEqual(0x5FCBBBB88D78UL, Encoder.DecodeUInt64(decryptor.Decrypt(prod)));

            prod = evaluator.MultiplyPlain(encrypted, plain2);
            Assert.AreEqual(0x5FCBBBB88D78UL, Encoder.DecodeUInt64(prod));
            Assert.AreEqual(0x5FCBBBB88D78UL, Encoder.DecodeUInt64(decryptor.Decrypt(prod)));
        }
    }
}