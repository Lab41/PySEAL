using Microsoft.Research.SEAL;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.IO;
using System;
using System.Collections.Generic;
using std;


namespace SEALNETTest
{
    [TestClass]
    public class KeyGeneratorWrapper
    {
        [TestMethod]
        public void FVKeyGenerationNET()
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
            keygen.Generate(1);

            Assert.IsFalse(keygen.PublicKey[0].IsZero);
            Assert.IsFalse(keygen.PublicKey[1].IsZero);
            Assert.IsFalse(keygen.SecretKey.IsZero);
            Assert.AreEqual(1, keygen.EvaluationKeys.Size);
            Assert.AreEqual(12, keygen.EvaluationKeys.Keys[0].Item1.Size);
            Assert.AreEqual(12, keygen.EvaluationKeys.Keys[0].Item2.Size);
            for (int i = 0; i < 12; ++i)
            {
                Assert.IsFalse(keygen.EvaluationKeys[0].Item1[i].IsZero);
                Assert.IsFalse(keygen.EvaluationKeys[0].Item2[i].IsZero);
            }

            var publicKey = keygen.PublicKey;
            var secretKey = keygen.SecretKey;
            var evaluationKeys = keygen.EvaluationKeys;

            keygen.Generate(1);
            Assert.IsFalse(publicKey[0].Equals(keygen.PublicKey[0]));
            Assert.IsFalse(publicKey[1].Equals(keygen.PublicKey[1]));
            Assert.IsFalse(secretKey.Equals(keygen.SecretKey));
            for (int i = 0; i < 12; ++i)
            {
                Assert.IsFalse(keygen.EvaluationKeys[0].Item1[i].Equals(evaluationKeys[0].Item1[i]));
                Assert.IsFalse(keygen.EvaluationKeys[0].Item2[i].Equals(evaluationKeys[0].Item2[i]));
            }

            keygen.GenerateEvaluationKeys(3);
            Assert.AreEqual(3, keygen.EvaluationKeys.Size);
            for (int i = 0; i < 12; ++i)
            {
                Assert.AreEqual(12, keygen.EvaluationKeys.Keys[0].Item1.Size);
                Assert.AreEqual(12, keygen.EvaluationKeys.Keys[0].Item2.Size);
                Assert.AreEqual(12, keygen.EvaluationKeys.Keys[1].Item1.Size);
                Assert.AreEqual(12, keygen.EvaluationKeys.Keys[1].Item2.Size);
                Assert.AreEqual(12, keygen.EvaluationKeys.Keys[2].Item1.Size);
                Assert.AreEqual(12, keygen.EvaluationKeys.Keys[2].Item2.Size);
                Assert.IsFalse(keygen.EvaluationKeys[0].Item1[i].IsZero);
                Assert.IsFalse(keygen.EvaluationKeys[0].Item2[i].IsZero);
                Assert.IsFalse(keygen.EvaluationKeys[1].Item1[i].IsZero);
                Assert.IsFalse(keygen.EvaluationKeys[1].Item1[i].IsZero);
                Assert.IsFalse(keygen.EvaluationKeys[2].Item2[i].IsZero);
                Assert.IsFalse(keygen.EvaluationKeys[2].Item2[i].IsZero);
            }
        }
        [TestMethod]
        public void FVKeyGenerationNoEVKNET()
        {
            var parms = new EncryptionParameters
            {
                DecompositionBitCount = 0,
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
            keygen.Generate(0);

            Assert.IsFalse(keygen.PublicKey[0].IsZero);
            Assert.IsFalse(keygen.PublicKey[1].IsZero);
            Assert.IsFalse(keygen.SecretKey.IsZero);
        }
    }
}