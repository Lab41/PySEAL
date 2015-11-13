using Microsoft.Research.SEAL;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace SEALNETTest
{
    [TestClass]
    public class KeyGeneratorWrapper
    {
        [TestMethod]
        public void KeyGenerationNET()
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
            Assert.IsTrue(keygen.PublicKey.IsZero);
            Assert.IsTrue(keygen.SecretKey.IsZero);
            Assert.AreEqual(12, keygen.EvaluationKeys.Count);
            for (int i = 0; i < 12; ++i)
            {
                Assert.IsTrue(keygen.EvaluationKeys[i].IsZero);
            }

            keygen.Generate();
            Assert.IsFalse(keygen.PublicKey.IsZero);
            Assert.IsFalse(keygen.SecretKey.IsZero);
            Assert.AreEqual(12, keygen.EvaluationKeys.Count);
            for (int i = 0; i < 12; ++i)
            {
                Assert.IsFalse(keygen.EvaluationKeys[i].IsZero);
            }

            var publicKey = keygen.PublicKey;
            var secretKey = keygen.SecretKey;
            keygen.Generate();
            Assert.IsFalse(publicKey.Equals(keygen.PublicKey));
            Assert.IsFalse(secretKey.Equals(keygen.SecretKey));
        }
    }
}