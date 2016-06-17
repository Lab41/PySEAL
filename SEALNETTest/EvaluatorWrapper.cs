using Microsoft.Research.SEAL;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Collections.Generic;

using YASHECiphertext = Microsoft.Research.SEAL.BigPoly;
using BigPolyArray = Microsoft.Research.SEAL.BigPolyArray;

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
            polyModulus.Resize(64, 1);
            polyModulus[0].Set(1);
            polyModulus[63].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms);
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
            polyModulus.Resize(64, 1);
            polyModulus[0].Set(1);
            polyModulus[63].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms);
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
            polyModulus.Resize(64, 1);
            polyModulus[0].Set(1);
            polyModulus[63].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms);
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
            polyModulus.Resize(64, 1);
            polyModulus[0].Set(1);
            polyModulus[63].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms);
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
            polyModulus.Resize(64, 1);
            polyModulus[0].Set(1);
            polyModulus[63].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms);
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
            polyModulus.Resize(64, 1);
            polyModulus[0].Set(1);
            polyModulus[63].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted1 = encryptor.Encrypt(Encoder.Encode(0x12345678));
            var plain2 = Encoder.Encode(0x54321);
            var sum = evaluator.MultiplyPlain(encrypted1, plain2);
            Assert.AreEqual(0x5FCBBBB88D78UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

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
            keygen.Generate();

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted1 = encryptor.Encrypt(Encoder.Encode(2));
            var encrypted2 = encryptor.Encrypt(Encoder.Encode(-3));
            var encrypted3 = encryptor.Encrypt(Encoder.Encode(4));
            var encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2, encrypted3 };
            var product = evaluator.MultiplyMany(encrypteds);
            Assert.AreEqual(-24, Encoder.DecodeInt32(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(-9));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(-17));
            encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2 };
            product = evaluator.MultiplyMany(encrypteds);
            Assert.AreEqual(153, Encoder.DecodeInt32(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(2));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(-31));
            encrypted3 = encryptor.Encrypt(Encoder.Encode(7));
            encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2, encrypted3 };
            product = evaluator.MultiplyMany(encrypteds);
            Assert.AreEqual(-434, Encoder.DecodeInt32(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(1));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(-1));
            encrypted3 = encryptor.Encrypt(Encoder.Encode(1));
            var encrypted4 = encryptor.Encrypt(Encoder.Encode(-1));
            encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2, encrypted3, encrypted4 };
            product = evaluator.MultiplyMany(encrypteds);
            Assert.AreEqual(1, Encoder.DecodeInt32(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(98765));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(0));
            encrypted3 = encryptor.Encrypt(Encoder.Encode(12345));
            encrypted4 = encryptor.Encrypt(Encoder.Encode(34567));
            encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2, encrypted3, encrypted4 };
            product = evaluator.MultiplyMany(encrypteds);
            Assert.AreEqual(0, Encoder.DecodeInt32(decryptor.Decrypt(product)));
        }

        //[TestMethod]
        //public void FVEncryptMultiplyNoRelinManyDecryptNET()
        //{
        //    var parms = new EncryptionParameters
        //    {
        //        DecompositionBitCount = 4,
        //        NoiseStandardDeviation = 3.19,
        //        NoiseMaxDeviation = 35.06
        //    };
        //    var coeffModulus = parms.CoeffModulus;
        //    coeffModulus.Resize(48);
        //    coeffModulus.Set("FFFFFFFFC001");
        //    var plainModulus = parms.PlainModulus;
        //    plainModulus.Resize(7);
        //    plainModulus.Set(1 << 4);
        //    var polyModulus = parms.PolyModulus;
        //    polyModulus.Resize(65, 1);
        //    polyModulus[0].Set(1);
        //    polyModulus[64].Set(1);

        //    var keygen = new KeyGenerator(parms);
        //    keygen.Generate(0);

        //    var Encoder = new BinaryEncoder(parms.PlainModulus);

        //    var encryptor = new Encryptor(parms, keygen.PublicKey);
        //    var evaluator = new Evaluator(parms, keygen.EvaluationKeys);
        //    var decryptor = new Decryptor(parms, keygen.SecretKey);

        //    var encrypted1 = encryptor.Encrypt(Encoder.Encode(2));
        //    var encrypted2 = encryptor.Encrypt(Encoder.Encode(-3));
        //    var encrypted3 = encryptor.Encrypt(Encoder.Encode(4));
        //    var encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2, encrypted3 };
        //    var product = evaluator.MultiplyNoRelinMany(encrypteds);
        //    Assert.AreEqual(-24, Encoder.DecodeInt32(decryptor.Decrypt(product)));

        //    encrypted1 = encryptor.Encrypt(Encoder.Encode(-9));
        //    encrypted2 = encryptor.Encrypt(Encoder.Encode(-17));
        //    encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2 };
        //    product = evaluator.MultiplyNoRelinMany(encrypteds);
        //    Assert.AreEqual(153, Encoder.DecodeInt32(decryptor.Decrypt(product)));

        //    encrypted1 = encryptor.Encrypt(Encoder.Encode(2));
        //    encrypted2 = encryptor.Encrypt(Encoder.Encode(-31));
        //    encrypted3 = encryptor.Encrypt(Encoder.Encode(7));
        //    encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2, encrypted3 };
        //    product = evaluator.MultiplyNoRelinMany(encrypteds);
        //    Assert.AreEqual(-434, Encoder.DecodeInt32(decryptor.Decrypt(product)));

        //    encrypted1 = encryptor.Encrypt(Encoder.Encode(1));
        //    encrypted2 = encryptor.Encrypt(Encoder.Encode(-1));
        //    encrypted3 = encryptor.Encrypt(Encoder.Encode(1));
        //    var encrypted4 = encryptor.Encrypt(Encoder.Encode(-1));
        //    encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2, encrypted3, encrypted4 };
        //    product = evaluator.MultiplyNoRelinMany(encrypteds);
        //    Assert.AreEqual(1, Encoder.DecodeInt32(decryptor.Decrypt(product)));

        //    encrypted1 = encryptor.Encrypt(Encoder.Encode(98765));
        //    encrypted2 = encryptor.Encrypt(Encoder.Encode(0));
        //    encrypted3 = encryptor.Encrypt(Encoder.Encode(12345));
        //    encrypted4 = encryptor.Encrypt(Encoder.Encode(34567));
        //    encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2, encrypted3, encrypted4 };
        //    product = evaluator.MultiplyNoRelinMany(encrypteds);
        //    Assert.AreEqual(0, Encoder.DecodeInt32(decryptor.Decrypt(product)));
        //}

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
            polyModulus.Resize(64, 1);
            polyModulus[0].Set(1);
            polyModulus[63].Set(1);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted1 = encryptor.Encrypt(Encoder.Encode(5));
            var encrypted2 = encryptor.Encrypt(Encoder.Encode(6));
            var encrypted3 = encryptor.Encrypt(Encoder.Encode(7));
            var encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2, encrypted3 };
            var product = evaluator.AddMany(encrypteds);
            Assert.AreEqual(18, Encoder.DecodeInt32(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(-9));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(-17));
            encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2 };
            product = evaluator.AddMany(encrypteds);
            Assert.AreEqual(-26, Encoder.DecodeInt32(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(2));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(-31));
            encrypted3 = encryptor.Encrypt(Encoder.Encode(7));
            encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2, encrypted3 };
            product = evaluator.AddMany(encrypteds);
            Assert.AreEqual(-22, Encoder.DecodeInt32(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(1));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(-1));
            encrypted3 = encryptor.Encrypt(Encoder.Encode(1));
            var encrypted4 = encryptor.Encrypt(Encoder.Encode(-1));
            encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2, encrypted3, encrypted4 };
            product = evaluator.AddMany(encrypteds);
            Assert.AreEqual(0, Encoder.DecodeInt32(decryptor.Decrypt(product)));

            encrypted1 = encryptor.Encrypt(Encoder.Encode(98765));
            encrypted2 = encryptor.Encrypt(Encoder.Encode(0));
            encrypted3 = encryptor.Encrypt(Encoder.Encode(12345));
            encrypted4 = encryptor.Encrypt(Encoder.Encode(34567));
            encrypteds = new List<BigPolyArray>() { encrypted1, encrypted2, encrypted3, encrypted4 };
            product = evaluator.AddMany(encrypteds);
            Assert.AreEqual(145677, Encoder.DecodeInt32(decryptor.Decrypt(product)));

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
            keygen.Generate();

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms);
            var decryptor = new Decryptor(parms, keygen.SecretKey);

            var encrypted = encryptor.Encrypt(Encoder.Encode(5));
            var power = evaluator.Exponentiate(encrypted, 1);
            Assert.AreEqual(5, Encoder.DecodeInt32(decryptor.Decrypt(power)));

            encrypted = encryptor.Encrypt(Encoder.Encode(7));
            power = evaluator.Exponentiate(encrypted, 2);
            Assert.AreEqual(49, Encoder.DecodeInt32(decryptor.Decrypt(power)));

            encrypted = encryptor.Encrypt(Encoder.Encode(-7));
            power = evaluator.Exponentiate(encrypted, 3);
            Assert.AreEqual(-343, Encoder.DecodeInt32(decryptor.Decrypt(power)));
        }

        //[TestMethod]
        //public void FVEncryptExponentiateNoRelinDecryptNET()
        //{
        //    var parms = new EncryptionParameters
        //    {
        //        DecompositionBitCount = 4,
        //        NoiseStandardDeviation = 3.19,
        //        NoiseMaxDeviation = 35.06
        //    };
        //    var coeffModulus = parms.CoeffModulus;
        //    coeffModulus.Resize(48);
        //    coeffModulus.Set("FFFFFFFFC001");
        //    var plainModulus = parms.PlainModulus;
        //    plainModulus.Resize(7);
        //    plainModulus.Set(1 << 4);
        //    var polyModulus = parms.PolyModulus;
        //    polyModulus.Resize(65, 1);
        //    polyModulus[0].Set(1);
        //    polyModulus[64].Set(1);

        //    var keygen = new KeyGenerator(parms);
        //    keygen.Generate();

        //    var Encoder = new BinaryEncoder(parms.PlainModulus);

        //    var encryptor = new Encryptor(parms, keygen.PublicKey);
        //    var evaluator = new Evaluator(parms, keygen.EvaluationKeys);
        //    var keygenEvals = keygen.EvaluationKeys;
        //    var evaluatorEvals = keygen.EvaluationKeys;
        //    for (int i = 0; i < keygen.EvaluationKeys.Size; ++i)
        //    {
        //        for (int j = 0; j < keygenEvals[0].Item1.Size; ++j)
        //        {
        //            Assert.AreEqual(keygenEvals[i].Item1[j], evaluatorEvals[i].Item1[j]);
        //            Assert.AreEqual(keygenEvals[i].Item2[j], evaluatorEvals[i].Item2[j]);
        //        }
        //    }
        //    var decryptor = new Decryptor(parms, keygen.SecretKey);

        //    var encrypted = encryptor.Encrypt(Encoder.Encode(5));
        //    var power = evaluator.ExponentiateNoRelin(encrypted, 1);
        //    Assert.AreEqual(5, Encoder.DecodeInt32(decryptor.Decrypt(power)));

        //    encrypted = encryptor.Encrypt(Encoder.Encode(7));
        //    power = evaluator.ExponentiateNoRelin(encrypted, 2);
        //    Assert.AreEqual(49, Encoder.DecodeInt32(decryptor.Decrypt(power)));

        //    encrypted = encryptor.Encrypt(Encoder.Encode(-7));
        //    power = evaluator.ExponentiateNoRelin(encrypted, 3);
        //    Assert.AreEqual(-343, Encoder.DecodeInt32(decryptor.Decrypt(power)));
        //}

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

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var evaluator = new Evaluator(parms);
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
        public void FVEncryptNonFFTMultiplyDecryptNET()
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
            var evaluator = new Evaluator(parms);
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

        //[TestMethod]
        //public void FVEncryptFFTMultiplyNoRelinDecryptNET()
        //{
        //    var parms = new EncryptionParameters
        //    {
        //        DecompositionBitCount = 4,
        //        NoiseStandardDeviation = 3.19,
        //        NoiseMaxDeviation = 35.06
        //    };
        //    var coeffModulus = parms.CoeffModulus;
        //    coeffModulus.Resize(48);
        //    coeffModulus.Set("FFFFFFFFC001");
        //    var plainModulus = parms.PlainModulus;
        //    plainModulus.Resize(7);
        //    plainModulus.Set(1 << 4);
        //    var polyModulus = parms.PolyModulus;
        //    polyModulus.Resize(65, 1);
        //    polyModulus[0].Set(1);
        //    polyModulus[64].Set(1);

        //    var keygen = new KeyGenerator(parms);
        //    keygen.Generate();

        //    var Encoder = new BinaryEncoder(parms.PlainModulus);

        //    var encryptor = new Encryptor(parms, keygen.PublicKey);
        //    var evaluator = new Evaluator(parms, keygen.EvaluationKeys);
        //    var keygenEvals = keygen.EvaluationKeys;
        //    var evaluatorEvals = evaluator.EvaluationKeys;
        //    for (int i = 0; i < keygen.EvaluationKeys.Size; ++i)
        //    {
        //        for (int j = 0; j < keygenEvals[0].Item1.Size; ++j)
        //        {
        //            Assert.AreEqual(keygenEvals[i].Item1[j], evaluatorEvals[i].Item1[j]);
        //            Assert.AreEqual(keygenEvals[i].Item2[j], evaluatorEvals[i].Item2[j]);
        //        }
        //    }
        //    var decryptor = new Decryptor(parms, keygen.SecretKey);

        //    var encrypted1 = encryptor.Encrypt(Encoder.Encode(0x12345678));
        //    var encrypted2 = encryptor.Encrypt(Encoder.Encode(0x54321));
        //    var sum = evaluator.MultiplyNoRelin(encrypted1, encrypted2);
        //    Assert.AreEqual(0x5FCBBBB88D78UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

        //    encrypted1 = encryptor.Encrypt(Encoder.Encode(0));
        //    encrypted2 = encryptor.Encrypt(Encoder.Encode(0));
        //    sum = evaluator.MultiplyNoRelin (encrypted1, encrypted2);
        //    Assert.AreEqual(0UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

        //    encrypted1 = encryptor.Encrypt(Encoder.Encode(0));
        //    encrypted2 = encryptor.Encrypt(Encoder.Encode(5));
        //    sum = evaluator.MultiplyNoRelin(encrypted1, encrypted2);
        //    Assert.AreEqual(0UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

        //    encrypted1 = encryptor.Encrypt(Encoder.Encode(7));
        //    encrypted2 = encryptor.Encrypt(Encoder.Encode(1));
        //    sum = evaluator.MultiplyNoRelin (encrypted1, encrypted2);
        //    Assert.AreEqual(7UL, Encoder.DecodeUInt64(decryptor.Decrypt(sum)));

        //    encrypted1 = encryptor.Encrypt(Encoder.Encode(5));
        //    encrypted2 = encryptor.Encrypt(Encoder.Encode(-3));
        //    sum = evaluator.MultiplyNoRelin(encrypted1, encrypted2);
        //    Assert.AreEqual(-15, Encoder.DecodeInt32(decryptor.Decrypt(sum)));

        //    encrypted1 = encryptor.Encrypt(Encoder.Encode(-7));
        //    encrypted2 = encryptor.Encrypt(Encoder.Encode(2));
        //    sum = evaluator.MultiplyNoRelin(encrypted1, encrypted2);
        //    Assert.AreEqual(-14, Encoder.DecodeInt32(decryptor.Decrypt(sum)));

        //    var encrypted3 = evaluator.MultiplyNoRelin(encrypted1, encrypted2);
        //    var encrypted4 = encryptor.Encrypt(Encoder.Encode(0));
        //    var encrypted5 = evaluator.MultiplyNoRelin(encrypted3, encrypted4);
        //    Assert.AreEqual(0, Encoder.DecodeInt32(decryptor.Decrypt(encrypted5)));
        //    var encrypted6 = encryptor.Encrypt(Encoder.Encode(1));
        //    sum = evaluator.MultiplyNoRelin(encrypted5, encrypted6);
        //    Assert.IsTrue(sum.Size == 5);
        //    Assert.AreEqual(0, Encoder.DecodeInt32(decryptor.Decrypt(sum)));
        //}

        //[TestMethod]
        //public void FVEncryptMultNoRelinRelinearizeDecryptNET()
        //{
        //    var parms = new EncryptionParameters
        //    {
        //        DecompositionBitCount = 4,
        //        NoiseStandardDeviation = 3.19,
        //        NoiseMaxDeviation = 35.06
        //    };
        //    var coeffModulus = parms.CoeffModulus;
        //    coeffModulus.Resize(48);
        //    coeffModulus.Set("FFFFFFFFC001");
        //    var plainModulus = parms.PlainModulus;
        //    plainModulus.Resize(7);
        //    plainModulus.Set(1 << 4);
        //    var polyModulus = parms.PolyModulus;
        //    polyModulus.Resize(65, 1);
        //    polyModulus[0].Set(1);
        //    polyModulus[64].Set(1);

        //    var keygen = new KeyGenerator(parms);
        //    keygen.Generate(3);

        //    var Encoder = new BinaryEncoder(parms.PlainModulus);

        //    var encryptor = new Encryptor(parms, keygen.PublicKey);
        //    var evaluator = new Evaluator(parms, keygen.EvaluationKeys);
        //    var keygenEvals = keygen.EvaluationKeys;
        //    var evaluatorEvals = keygen.EvaluationKeys;
        //    for (int i = 0; i < keygen.EvaluationKeys.Size; ++i)
        //    {
        //        for (int j = 0; j < keygenEvals[0].Item1.Size; ++j)
        //        {
        //            Assert.AreEqual(keygenEvals[i].Item1[j], evaluatorEvals[i].Item1[j]);
        //            Assert.AreEqual(keygenEvals[i].Item2[j], evaluatorEvals[i].Item2[j]);
        //        }
        //    }
        //    var decryptor = new Decryptor(parms, keygen.SecretKey);

        //    var encrypted1 = encryptor.Encrypt(Encoder.Encode(-7));
        //    var encrypted2 = encryptor.Encrypt(Encoder.Encode(2));
        //    var encrypted3 = evaluator.MultiplyNoRelin(encrypted1, encrypted2);
        //    Assert.AreEqual(encrypted3.Size, 3);
        //    var norelinencrypted3 = evaluator.Relinearize(encrypted3, 3);
        //    Assert.AreEqual(norelinencrypted3.Size, 3);
        //    Assert.AreEqual(-14, Encoder.DecodeInt32(decryptor.Decrypt(norelinencrypted3)));
        //    var encrypted4 = encryptor.Encrypt(Encoder.Encode(0)); 
        //    var encrypted5 = evaluator.MultiplyNoRelin(encrypted3, encrypted4); 
        //    Assert.AreEqual(0, Encoder.DecodeInt32(decryptor.Decrypt(encrypted5)));
        //    var encrypted6 = encryptor.Encrypt(Encoder.Encode(1));
        //    var sum = evaluator.MultiplyNoRelin(encrypted5, encrypted6);
        //    Assert.AreEqual(0, Encoder.DecodeInt32(decryptor.Decrypt(sum)));
        //    Assert.AreEqual(sum.Size, 5);
        //    var relin = evaluator.Relinearize(sum);
        //    Assert.AreEqual(0, Encoder.DecodeInt32(decryptor.Decrypt(relin)));
        //    var rerelin = evaluator.Relinearize(relin);
        //    Assert.AreEqual(0, Encoder.DecodeInt32(decryptor.Decrypt(rerelin)));

        //    var encryptedplain1 = encryptor.Encrypt(Encoder.Encode(2));
        //    Assert.AreEqual(encryptedplain1.Size, 2);
        //    var relinencryptedplain1 = evaluator.Relinearize(encryptedplain1);
        //    Assert.AreEqual(relinencryptedplain1.Size, 2);
        //    Assert.AreEqual(2, Encoder.DecodeInt32(decryptor.Decrypt(relinencryptedplain1)));
        //}
    }
}