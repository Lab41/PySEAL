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
            var parms = new EncryptionParameters(MemoryPoolHandle.AcquireNew());
            parms.SetDecompositionBitCount(4);
            parms.SetNoiseStandardDeviation(3.19);
            parms.SetNoiseMaxDeviation(35.06);

            var coeffModulus = new BigUInt(48);
            coeffModulus.Set("FFFFFFFFC001");
            parms.SetCoeffModulus(coeffModulus);

            var plainModulus = new BigUInt(7);
            plainModulus.Set(1 << 6);
            parms.SetPlainModulus(plainModulus);

            var polyModulus = new BigPoly(65, 1);
            polyModulus[0].Set(1);
            polyModulus[64].Set(1);
            parms.SetPolyModulus(polyModulus);

            parms.Validate();

            var keygen = new KeyGenerator(parms, MemoryPoolHandle.AcquireNew());
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
            var parms = new EncryptionParameters(MemoryPoolHandle.AcquireNew());
            parms.SetDecompositionBitCount(0);
            parms.SetNoiseStandardDeviation(3.19);
            parms.SetNoiseMaxDeviation(35.06);

            var coeffModulus = new BigUInt(48);
            coeffModulus.Set("FFFFFFFFC001");
            parms.SetCoeffModulus(coeffModulus);

            var plainModulus = new BigUInt(7);
            plainModulus.Set(1 << 6);
            parms.SetPlainModulus(plainModulus);

            var polyModulus = new BigPoly(65, 1);
            polyModulus[0].Set(1);
            polyModulus[64].Set(1);
            parms.SetPolyModulus(polyModulus);

            parms.Validate();

            var keygen = new KeyGenerator(parms, MemoryPoolHandle.AcquireNew());
            keygen.Generate(0);

            Assert.IsFalse(keygen.PublicKey[0].IsZero);
            Assert.IsFalse(keygen.PublicKey[1].IsZero);
            Assert.IsFalse(keygen.SecretKey.IsZero);
        }
    }
}