using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.Research.SEAL;
using System.Collections.Generic;

namespace SEALNETTest
{
    [TestClass]
    public class KeyGeneratorWrapper
    {
        [TestMethod]
        public void FVKeyGenerationNET()
        {
            var parms = new EncryptionParameters();
            {
                parms.NoiseStandardDeviation = 3.19;
                parms.PolyModulus = "1x^64 + 1";
                parms.CoeffModulus = new List<SmallModulus> { DefaultParams.SmallMods60Bit(0) };
                parms.PlainModulus = 1 << 6;

                var context = new SEALContext(parms);
                var keygen = new KeyGenerator(context);

                Assert.IsTrue(keygen.PublicKey.HashBlock.Equals(parms.HashBlock));
                Assert.IsTrue(keygen.SecretKey.HashBlock.Equals(parms.HashBlock));

                var evk = new EvaluationKeys();
                keygen.GenerateEvaluationKeys(60, evk);
                Assert.AreEqual(evk.HashBlock, parms.HashBlock);
                Assert.AreEqual(2, evk.Key(2)[0].Size);

                keygen.GenerateEvaluationKeys(30, 1, evk);
                Assert.AreEqual(evk.HashBlock, parms.HashBlock);
                Assert.AreEqual(4, evk.Key(2)[0].Size);

                keygen.GenerateEvaluationKeys(2, 2, evk);
                Assert.AreEqual(evk.HashBlock, parms.HashBlock);
                Assert.AreEqual(60, evk.Key(2)[0].Size);
            }
            {
                parms.NoiseStandardDeviation = 3.19;
                parms.PolyModulus = "1x^256 + 1";
                parms.CoeffModulus = new List<SmallModulus> {
                    DefaultParams.SmallMods60Bit(0), DefaultParams.SmallMods30Bit(0), DefaultParams.SmallMods30Bit(1) };
                parms.PlainModulus = 1 << 6;

                var context = new SEALContext(parms);
                var keygen = new KeyGenerator(context);

                Assert.AreEqual(keygen.PublicKey.HashBlock, parms.HashBlock);
                Assert.AreEqual(keygen.SecretKey.HashBlock, parms.HashBlock);

                var evk = new EvaluationKeys();
                keygen.GenerateEvaluationKeys(60, 2, evk);
                Assert.AreEqual(evk.HashBlock, parms.HashBlock);
                Assert.AreEqual(2, evk.Key(2)[0].Size);

                keygen.GenerateEvaluationKeys(30, 2, evk);
                Assert.AreEqual(evk.HashBlock, parms.HashBlock);
                Assert.AreEqual(4, evk.Key(2)[0].Size);

                keygen.GenerateEvaluationKeys(4, 1, evk);
                Assert.AreEqual(evk.HashBlock, parms.HashBlock);
                Assert.AreEqual(30, evk.Key(2)[0].Size);
            }
        }
    }
}