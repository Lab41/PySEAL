using Microsoft.Research.SEAL;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Collections.Generic;

namespace SEALNETTest
{
    [TestClass]
    public class EvaluatorWrapper
    {
        [TestMethod]
        public void FVEncryptNegateDecryptNET()
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
            polyModulus.Resize(65, 1);
            polyModulus[0].Set(1);
            polyModulus[64].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted = encryptor.Encrypt(encoder.Encode(0x12345678));
            var negated = evaluator.Negate(encrypted);
            Assert.AreEqual(-0x12345678, encoder.DecodeInt32(decryptor.Decrypt(negated)));

            encrypted = encryptor.Encrypt(encoder.Encode(0));
            negated = evaluator.Negate(encrypted);
            Assert.AreEqual(0, encoder.DecodeInt32(decryptor.Decrypt(negated)));

            encrypted = encryptor.Encrypt(encoder.Encode(1));
            negated = evaluator.Negate(encrypted);
            Assert.AreEqual(-1, encoder.DecodeInt32(decryptor.Decrypt(negated)));

            encrypted = encryptor.Encrypt(encoder.Encode(-1));
            negated = evaluator.Negate(encrypted);
            Assert.AreEqual(1, encoder.DecodeInt32(decryptor.Decrypt(negated)));

            encrypted = encryptor.Encrypt(encoder.Encode(2));
            negated = evaluator.Negate(encrypted);
            Assert.AreEqual(-2, encoder.DecodeInt32(decryptor.Decrypt(negated)));

            encrypted = encryptor.Encrypt(encoder.Encode(-5));
            negated = evaluator.Negate(encrypted);
            Assert.AreEqual(5, encoder.DecodeInt32(decryptor.Decrypt(negated)));
        }

        [TestMethod]
        public void FVEncryptAddDecryptNET()
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
            polyModulus.Resize(65, 1);
            polyModulus[0].Set(1);
            polyModulus[64].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted1 = encryptor.Encrypt(encoder.Encode(0x12345678));
            var encrypted2 = encryptor.Encrypt(encoder.Encode(0x54321));
            var sum = evaluator.Add(encrypted1, encrypted2);
            Assert.AreEqual(0x12399999UL, encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(0));
            encrypted2 = encryptor.Encrypt(encoder.Encode(0));
            sum = evaluator.Add(encrypted1, encrypted2);
            Assert.AreEqual(0UL, encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(0));
            encrypted2 = encryptor.Encrypt(encoder.Encode(5));
            sum = evaluator.Add(encrypted1, encrypted2);
            Assert.AreEqual(5UL, encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(5));
            encrypted2 = encryptor.Encrypt(encoder.Encode(-3));
            sum = evaluator.Add(encrypted1, encrypted2);
            Assert.AreEqual(2, encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(-7));
            encrypted2 = encryptor.Encrypt(encoder.Encode(2));
            sum = evaluator.Add(encrypted1, encrypted2);
            Assert.AreEqual(-5, encoder.DecodeInt32(decryptor.Decrypt(sum)));
        }        

        [TestMethod]
        public void FVEncryptSubDecryptNET()
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
            polyModulus.Resize(65, 1);
            polyModulus[0].Set(1);
            polyModulus[64].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted1 = encryptor.Encrypt(encoder.Encode(0x12345678));
            var encrypted2 = encryptor.Encrypt(encoder.Encode(0x54321));
            var sum = evaluator.Sub(encrypted1, encrypted2);
            Assert.AreEqual(0x122F1357, encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(0));
            encrypted2 = encryptor.Encrypt(encoder.Encode(0));
            sum = evaluator.Sub(encrypted1, encrypted2);
            Assert.AreEqual(0, encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(0));
            encrypted2 = encryptor.Encrypt(encoder.Encode(5));
            sum = evaluator.Sub(encrypted1, encrypted2);
            Assert.AreEqual(-5, encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(5));
            encrypted2 = encryptor.Encrypt(encoder.Encode(-3));
            sum = evaluator.Sub(encrypted1, encrypted2);
            Assert.AreEqual(8, encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(-7));
            encrypted2 = encryptor.Encrypt(encoder.Encode(2));
            sum = evaluator.Sub(encrypted1, encrypted2);
            Assert.AreEqual(-9, encoder.DecodeInt32(decryptor.Decrypt(sum)));
        }

        [TestMethod]
        public void FVEncryptAddPlainDecryptNET()
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
            polyModulus.Resize(65, 1);
            polyModulus[0].Set(1);
            polyModulus[64].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted1 = encryptor.Encrypt(encoder.Encode(0x12345678));
            var plain2 = encoder.Encode(0x54321);
            var sum = evaluator.AddPlain(encrypted1, plain2);
            Assert.AreEqual(0x12399999UL, encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(0));
            plain2 = encoder.Encode(0);
            sum = evaluator.AddPlain(encrypted1, plain2);
            Assert.AreEqual(0UL, encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(0));
            plain2 = encoder.Encode(5);
            sum = evaluator.AddPlain(encrypted1, plain2);
            Assert.AreEqual(5UL, encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(5));
            plain2 = encoder.Encode(-3);
            sum = evaluator.AddPlain(encrypted1, plain2);
            Assert.AreEqual(2, encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(-7));
            plain2 = encoder.Encode(2);
            sum = evaluator.AddPlain(encrypted1, plain2);
            Assert.AreEqual(-5, encoder.DecodeInt32(decryptor.Decrypt(sum)));
        }

        [TestMethod]
        public void FVEncryptSubPlainDecryptNET()
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
            polyModulus.Resize(65, 1);
            polyModulus[0].Set(1);
            polyModulus[64].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted1 = encryptor.Encrypt(encoder.Encode(0x12345678));
            var plain2 = encoder.Encode(0x54321);
            var sum = evaluator.SubPlain(encrypted1, plain2);
            Assert.AreEqual(0x122F1357, encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(0));
            plain2 = encoder.Encode(0);
            sum = evaluator.SubPlain(encrypted1, plain2);
            Assert.AreEqual(0, encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(0));
            plain2 = encoder.Encode(5);
            sum = evaluator.SubPlain(encrypted1, plain2);
            Assert.AreEqual(-5, encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(5));
            plain2 = encoder.Encode(-3);
            sum = evaluator.SubPlain(encrypted1, plain2);
            Assert.AreEqual(8, encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(-7));
            plain2 = encoder.Encode(2);
            sum = evaluator.SubPlain(encrypted1, plain2);
            Assert.AreEqual(-9, encoder.DecodeInt32(decryptor.Decrypt(sum)));
        }

        [TestMethod]
        public void FVEncryptMultiplyPlainDecryptNET()
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
            polyModulus.Resize(65, 1);
            polyModulus[0].Set(1);
            polyModulus[64].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted1 = encryptor.Encrypt(encoder.Encode(0x12345678));
            var plain2 = encoder.Encode(0x54321);
            var sum = evaluator.MultiplyPlain(encrypted1, plain2);
            Assert.AreEqual(0x5FCBBBB88D78UL, encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(0));
            plain2 = encoder.Encode(5);
            sum = evaluator.MultiplyPlain(encrypted1, plain2);
            Assert.AreEqual(0UL, encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(7));
            plain2 = encoder.Encode(1);
            sum = evaluator.MultiplyPlain(encrypted1, plain2);
            Assert.AreEqual(7UL, encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(5));
            plain2 = encoder.Encode(-3);
            sum = evaluator.MultiplyPlain(encrypted1, plain2);
            Assert.AreEqual(-15, encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(-7));
            plain2 = encoder.Encode(2);
            sum = evaluator.MultiplyPlain(encrypted1, plain2);
            Assert.AreEqual(-14, encoder.DecodeInt32(decryptor.Decrypt(sum)));
        }

        [TestMethod]
        public void FVEncryptMultiplyManyDecryptNET()
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
            polyModulus.Resize(65, 1);
            polyModulus[0].Set(1);
            polyModulus[64].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate(1);

            var encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms, keygen.EvaluationKeys);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted1 = encryptor.Encrypt(encoder.Encode(2));
            var encrypted2 = encryptor.Encrypt(encoder.Encode(-3));
            var encrypted3 = encryptor.Encrypt(encoder.Encode(4));
            var encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2, encrypted3 };
            var product = evaluator.MultiplyMany(encrypteds);
            Assert.AreEqual(-24, encoder.DecodeInt32(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(-9));
            encrypted2 = encryptor.Encrypt(encoder.Encode(-17));
            encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2 };
            product = evaluator.MultiplyMany(encrypteds);
            Assert.AreEqual(153, encoder.DecodeInt32(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(2));
            encrypted2 = encryptor.Encrypt(encoder.Encode(-31));
            encrypted3 = encryptor.Encrypt(encoder.Encode(7));
            encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2, encrypted3 };
            product = evaluator.MultiplyMany(encrypteds);
            Assert.AreEqual(-434, encoder.DecodeInt32(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(1));
            encrypted2 = encryptor.Encrypt(encoder.Encode(-1));
            encrypted3 = encryptor.Encrypt(encoder.Encode(1));
            var encrypted4 = encryptor.Encrypt(encoder.Encode(-1));
            encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2, encrypted3, encrypted4 };
            product = evaluator.MultiplyMany(encrypteds);
            Assert.AreEqual(1, encoder.DecodeInt32(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(98765));
            encrypted2 = encryptor.Encrypt(encoder.Encode(0));
            encrypted3 = encryptor.Encrypt(encoder.Encode(12345));
            encrypted4 = encryptor.Encrypt(encoder.Encode(34567));
            encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2, encrypted3, encrypted4 };
            product = evaluator.MultiplyMany(encrypteds);
            Assert.AreEqual(0, encoder.DecodeInt32(decryptor.Decrypt(product)));
        }

        [TestMethod]
        public void FVEncryptAddManyDecryptNET()
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
            polyModulus.Resize(65, 1);
            polyModulus[0].Set(1);
            polyModulus[64].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted1 = encryptor.Encrypt(encoder.Encode(5));
            var encrypted2 = encryptor.Encrypt(encoder.Encode(6));
            var encrypted3 = encryptor.Encrypt(encoder.Encode(7));
            var encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2, encrypted3 };
            var product = evaluator.AddMany(encrypteds);
            Assert.AreEqual(18, encoder.DecodeInt32(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(-9));
            encrypted2 = encryptor.Encrypt(encoder.Encode(-17));
            encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2 };
            product = evaluator.AddMany(encrypteds);
            Assert.AreEqual(-26, encoder.DecodeInt32(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(2));
            encrypted2 = encryptor.Encrypt(encoder.Encode(-31));
            encrypted3 = encryptor.Encrypt(encoder.Encode(7));
            encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2, encrypted3 };
            product = evaluator.AddMany(encrypteds);
            Assert.AreEqual(-22, encoder.DecodeInt32(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(1));
            encrypted2 = encryptor.Encrypt(encoder.Encode(-1));
            encrypted3 = encryptor.Encrypt(encoder.Encode(1));
            var encrypted4 = encryptor.Encrypt(encoder.Encode(-1));
            encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2, encrypted3, encrypted4 };
            product = evaluator.AddMany(encrypteds);
            Assert.AreEqual(0, encoder.DecodeInt32(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(98765));
            encrypted2 = encryptor.Encrypt(encoder.Encode(0));
            encrypted3 = encryptor.Encrypt(encoder.Encode(12345));
            encrypted4 = encryptor.Encrypt(encoder.Encode(34567));
            encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2, encrypted3, encrypted4 };
            product = evaluator.AddMany(encrypteds);
            Assert.AreEqual(145677, encoder.DecodeInt32(decryptor.Decrypt(product)));

            BalancedFractionalEncoder fracEncoder = new BalancedFractionalEncoder(plainModulus, polyModulus, 10, 15);
            encrypted1 = encryptor.Encrypt(fracEncoder.Encode(3.1415));
            encrypted2 = encryptor.Encrypt(fracEncoder.Encode(12.345));
            encrypted3 = encryptor.Encrypt(fracEncoder.Encode(98.765));
            encrypted4 = encryptor.Encrypt(fracEncoder.Encode(1.1111));
            encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2, encrypted3, encrypted4 };
            product = evaluator.AddMany(encrypteds);
            Assert.IsTrue(System.Math.Abs(fracEncoder.Decode(decryptor.Decrypt(product)) - 115.3626) < 0.000001);
        }

        [TestMethod]
        public void FVEncryptExponentiateDecryptNET()
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
            polyModulus.Resize(65, 1);
            polyModulus[0].Set(1);
            polyModulus[64].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate(1);

            var encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms, keygen.EvaluationKeys);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted = encryptor.Encrypt(encoder.Encode(5));
            var power = evaluator.Exponentiate(encrypted, 1);
            Assert.AreEqual(5, encoder.DecodeInt32(decryptor.Decrypt(power)));

            encrypted = encryptor.Encrypt(encoder.Encode(7));
            power = evaluator.Exponentiate(encrypted, 2);
            Assert.AreEqual(49, encoder.DecodeInt32(decryptor.Decrypt(power)));

            encrypted = encryptor.Encrypt(encoder.Encode(-7));
            power = evaluator.Exponentiate(encrypted, 3);
            Assert.AreEqual(-343, encoder.DecodeInt32(decryptor.Decrypt(power)));
        }

        [TestMethod]
        public void FVEncryptFFTMultiplyDecryptNET()
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
            polyModulus.Resize(65, 1);
            polyModulus[0].Set(1);
            polyModulus[64].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted1 = encryptor.Encrypt(encoder.Encode(0x12345678));
            var encrypted2 = encryptor.Encrypt(encoder.Encode(0x54321));
            var sum = evaluator.Multiply(encrypted1, encrypted2);
            Assert.AreEqual(0x5FCBBBB88D78UL, encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(0));
            encrypted2 = encryptor.Encrypt(encoder.Encode(0));
            sum = evaluator.Multiply(encrypted1, encrypted2);
            Assert.AreEqual(0UL, encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(0));
            encrypted2 = encryptor.Encrypt(encoder.Encode(5));
            sum = evaluator.Multiply(encrypted1, encrypted2);
            Assert.AreEqual(0UL, encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(7));
            encrypted2 = encryptor.Encrypt(encoder.Encode(1));
            sum = evaluator.Multiply(encrypted1, encrypted2);
            Assert.AreEqual(7UL, encoder.DecodeUInt64(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(5));
            encrypted2 = encryptor.Encrypt(encoder.Encode(-3));
            sum = evaluator.Multiply(encrypted1, encrypted2);
            Assert.AreEqual(-15, encoder.DecodeInt32(decryptor.Decrypt(sum)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(-7));
            encrypted2 = encryptor.Encrypt(encoder.Encode(2));
            sum = evaluator.Multiply(encrypted1, encrypted2);
            Assert.AreEqual(-14, encoder.DecodeInt32(decryptor.Decrypt(sum)));
        }

        [TestMethod]
        public void FVEncryptMultiplyDecryptNET()
        {
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
                polyModulus.Resize(65, 1);
                polyModulus[0].Set(1);
                polyModulus[64].Set(1);

                var keygen = new KeyGenerator(parms);
                keygen.Generate();

                var encoder = new BinaryEncoder(parms.PlainModulus);

                var encryptor = new Encryptor(parms, keygen.PublicKey);
                var evaluator = new Evaluator(parms);
                var decryptor = new Decryptor(parms, keygen.SecretKey);

                var encrypted1 = encryptor.Encrypt(encoder.Encode(0x12345678));
                var encrypted2 = encryptor.Encrypt(encoder.Encode(0x54321));
                var sum = evaluator.Multiply(encrypted1, encrypted2);
                Assert.AreEqual(0x5FCBBBB88D78UL, encoder.DecodeUInt64(decryptor.Decrypt(sum)));

                encrypted1 = encryptor.Encrypt(encoder.Encode(0));
                encrypted2 = encryptor.Encrypt(encoder.Encode(0));
                sum = evaluator.Multiply(encrypted1, encrypted2);
                Assert.AreEqual(0UL, encoder.DecodeUInt64(decryptor.Decrypt(sum)));

                encrypted1 = encryptor.Encrypt(encoder.Encode(0));
                encrypted2 = encryptor.Encrypt(encoder.Encode(5));
                sum = evaluator.Multiply(encrypted1, encrypted2);
                Assert.AreEqual(0UL, encoder.DecodeUInt64(decryptor.Decrypt(sum)));

                encrypted1 = encryptor.Encrypt(encoder.Encode(7));
                encrypted2 = encryptor.Encrypt(encoder.Encode(1));
                sum = evaluator.Multiply(encrypted1, encrypted2);
                Assert.AreEqual(7UL, encoder.DecodeUInt64(decryptor.Decrypt(sum)));

                encrypted1 = encryptor.Encrypt(encoder.Encode(5));
                encrypted2 = encryptor.Encrypt(encoder.Encode(-3));
                sum = evaluator.Multiply(encrypted1, encrypted2);
                Assert.AreEqual(-15, encoder.DecodeInt32(decryptor.Decrypt(sum)));

                encrypted1 = encryptor.Encrypt(encoder.Encode(-7));
                encrypted2 = encryptor.Encrypt(encoder.Encode(2));
                sum = evaluator.Multiply(encrypted1, encrypted2);
                Assert.AreEqual(-14, encoder.DecodeInt32(decryptor.Decrypt(sum)));
            }

            {
                var parms = new EncryptionParameters
                {
                    DecompositionBitCount = 4,
                    NoiseStandardDeviation = 3.19,
                    NoiseMaxDeviation = 35.06
                };
                var coeffModulus = parms.CoeffModulus;
                coeffModulus.Resize(48);
                coeffModulus.Set("FFFFFFFFFFFFFFFFFFFF");
                var plainModulus = parms.PlainModulus;
                plainModulus.Resize(7);
                plainModulus.Set(1 << 6);
                var polyModulus = parms.PolyModulus;
                polyModulus.Resize(129, 1);
                polyModulus[0].Set(1);
                polyModulus[128].Set(1);

                var keygen = new KeyGenerator(parms);
                keygen.Generate();

                var encoder = new BinaryEncoder(parms.PlainModulus);

                var encryptor = new Encryptor(parms, keygen.PublicKey);
                var evaluator = new Evaluator(parms);
                var decryptor = new Decryptor(parms, keygen.SecretKey);

                var encrypted1 = encryptor.Encrypt(encoder.Encode(0x12345678));
                var encrypted2 = encryptor.Encrypt(encoder.Encode(0x54321));
                var sum = evaluator.Multiply(encrypted1, encrypted2);
                Assert.AreEqual(0x5FCBBBB88D78UL, encoder.DecodeUInt64(decryptor.Decrypt(sum)));

                encrypted1 = encryptor.Encrypt(encoder.Encode(0));
                encrypted2 = encryptor.Encrypt(encoder.Encode(0));
                sum = evaluator.Multiply(encrypted1, encrypted2);
                Assert.AreEqual(0UL, encoder.DecodeUInt64(decryptor.Decrypt(sum)));

                encrypted1 = encryptor.Encrypt(encoder.Encode(0));
                encrypted2 = encryptor.Encrypt(encoder.Encode(5));
                sum = evaluator.Multiply(encrypted1, encrypted2);
                Assert.AreEqual(0UL, encoder.DecodeUInt64(decryptor.Decrypt(sum)));

                encrypted1 = encryptor.Encrypt(encoder.Encode(7));
                encrypted2 = encryptor.Encrypt(encoder.Encode(1));
                sum = evaluator.Multiply(encrypted1, encrypted2);
                Assert.AreEqual(7UL, encoder.DecodeUInt64(decryptor.Decrypt(sum)));

                encrypted1 = encryptor.Encrypt(encoder.Encode(5));
                encrypted2 = encryptor.Encrypt(encoder.Encode(-3));
                sum = evaluator.Multiply(encrypted1, encrypted2);
                Assert.AreEqual(-15, encoder.DecodeInt32(decryptor.Decrypt(sum)));

                encrypted1 = encryptor.Encrypt(encoder.Encode(-7));
                encrypted2 = encryptor.Encrypt(encoder.Encode(2));
                sum = evaluator.Multiply(encrypted1, encrypted2);
                Assert.AreEqual(-14, encoder.DecodeInt32(decryptor.Decrypt(sum)));
            }
        }

        [TestMethod]
        public void FVEncryptSquareDecrypt()
        {
            var parms = new EncryptionParameters
            {
                DecompositionBitCount = 4,
                NoiseStandardDeviation = 3.19,
                NoiseMaxDeviation = 35.06
            };
            var coeffModulus = parms.CoeffModulus;
            coeffModulus.Resize(48);
            coeffModulus.Set("FFFFFFFFFFFFFFFFFFFF");
            var plainModulus = parms.PlainModulus;
            plainModulus.Resize(7);
            plainModulus.Set(1 << 6);
            var polyModulus = parms.PolyModulus;
            polyModulus.Resize(65, 1);
            polyModulus[0].Set(1);
            polyModulus[64].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var encoder = new BalancedEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted1 = encryptor.Encrypt(encoder.Encode(1));
            var product = evaluator.Square(encrypted1);
            Assert.AreEqual(1UL, encoder.DecodeUInt64(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(0));
            product = evaluator.Square(encrypted1);
            Assert.AreEqual(0UL, encoder.DecodeUInt64(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(-5));
            product = evaluator.Square(encrypted1);
            Assert.AreEqual(25UL, encoder.DecodeUInt64(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(-1));
            product = evaluator.Square(encrypted1);
            Assert.AreEqual(1UL, encoder.DecodeUInt64(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(encoder.Encode(123));
            product = evaluator.Square(encrypted1);
            Assert.AreEqual(15129UL, encoder.DecodeUInt64(decryptor.Decrypt(product)));
        }
    }
}