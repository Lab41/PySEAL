using System.IO;
using Microsoft.Research.SEAL;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace SEALNETTest
{
    [TestClass]
    public class EncryptionParamsWrapper
    {
        [TestMethod]
        public void EncryptionParamsWriteReadNET()
        {
            // Create encryption parameters.
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

            Assert.AreEqual(4, parms.DecompositionBitCount);
            Assert.AreEqual(3.19, parms.NoiseStandardDeviation);
            Assert.AreEqual(35.06, parms.NoiseMaxDeviation);
            Assert.AreEqual("FFFFFFFFC001", parms.CoeffModulus.ToString());
            Assert.AreEqual("40", parms.PlainModulus.ToString());
            Assert.AreEqual("1x^63 + 1", parms.PolyModulus.ToString());
            Assert.AreEqual(EncryptionMode.Test, parms.Mode);
        }

        [TestMethod]
        public void EncryptionParamsDefaultToNormalModeNET()
        {
            var parms = new EncryptionParameters();
            Assert.AreEqual(EncryptionMode.Normal, parms.Mode);
        }

        [TestMethod]
        public void SaveLoadEncryptionParamsNET()
        {
            var stream = new MemoryStream();

            // Create encryption parameters.
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

            var parms2 = new EncryptionParameters();
            stream.Seek(0, SeekOrigin.Begin);
            parms.Save(stream);
            stream.Seek(0, SeekOrigin.Begin);
            parms2.Load(stream);

            Assert.AreEqual(parms.DecompositionBitCount, parms2.DecompositionBitCount);
            Assert.AreEqual(parms.NoiseStandardDeviation, parms2.NoiseStandardDeviation);
            Assert.AreEqual(parms.NoiseMaxDeviation, parms2.NoiseMaxDeviation);
            Assert.AreEqual(parms.CoeffModulus, parms2.CoeffModulus);
            Assert.AreEqual(parms.PlainModulus, parms2.PlainModulus);
            Assert.AreEqual(parms.PolyModulus, parms2.PolyModulus);
        }
    }
}