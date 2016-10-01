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
            };
            var coeffModulus = parms.CoeffModulus;
            coeffModulus.Resize(48);
            coeffModulus.Set("7FFFFC801");
            var auxCoeffModulus = parms.AuxCoeffModulus;
            auxCoeffModulus.Resize(64);
            auxCoeffModulus.Set("FFFFFFFFC001");
            var plainModulus = parms.PlainModulus;
            plainModulus.Resize(7);
            plainModulus.Set(1 << 6);
            var polyModulus = parms.PolyModulus;
            polyModulus.Resize(65, 1);
            polyModulus[0].Set(1);
            polyModulus[64].Set(1);

            Assert.AreEqual(4, parms.DecompositionBitCount);
            Assert.AreEqual(3.19, parms.NoiseStandardDeviation);
            Assert.AreEqual(35.06, parms.NoiseMaxDeviation);
            Assert.AreEqual("7FFFFC801", parms.CoeffModulus.ToString());

            // Commented out due to dependence on the SEAL library #define DISABLE_NTT_IN_MULTIPLY
            //Assert.AreEqual("FFFFFFFFC001", parms.AuxCoeffModulus.ToString());

            Assert.AreEqual("40", parms.PlainModulus.ToString());
            Assert.AreEqual("1x^64 + 1", parms.PolyModulus.ToString());
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
            coeffModulus.Set("7FFFFC801");
            var auxCoeffModulus = parms.AuxCoeffModulus;
            auxCoeffModulus.Resize(64);
            auxCoeffModulus.Set("FFFFFFFFC001");
            var plainModulus = parms.PlainModulus;
            plainModulus.Resize(7);
            plainModulus.Set(1 << 6);
            var polyModulus = parms.PolyModulus;
            polyModulus.Resize(65, 1);
            polyModulus[0].Set(1);
            polyModulus[64].Set(1);

            var parms2 = new EncryptionParameters();
            stream.Seek(0, SeekOrigin.Begin);
            parms.Save(stream);
            stream.Seek(0, SeekOrigin.Begin);
            parms2.Load(stream);

            Assert.AreEqual(parms.DecompositionBitCount, parms2.DecompositionBitCount);
            Assert.AreEqual(parms.NoiseStandardDeviation, parms2.NoiseStandardDeviation);
            Assert.AreEqual(parms.NoiseMaxDeviation, parms2.NoiseMaxDeviation);
            Assert.AreEqual(parms.CoeffModulus, parms2.CoeffModulus);

            // Commented out due to dependence on the SEAL library #define DISABLE_NTT_IN_MULTIPLY
            //Assert.AreEqual(parms.AuxCoeffModulus, parms2.AuxCoeffModulus);

            Assert.AreEqual(parms.PlainModulus, parms2.PlainModulus);
            Assert.AreEqual(parms.PolyModulus, parms2.PolyModulus);
        }

        [TestMethod]
        public void GetQualifiersNET()
        {
            {
                var parms = new EncryptionParameters
                {
                    DecompositionBitCount = 4,
                    NoiseStandardDeviation = 3.19,
                    NoiseMaxDeviation = 35.06,
                };
                var coeffModulus = parms.CoeffModulus;
                coeffModulus.Resize(48);
                coeffModulus.Set("7FFFFC801");
                var auxCoeffModulus = parms.AuxCoeffModulus;
                auxCoeffModulus.Resize(64);
                auxCoeffModulus.Set("FFFFFFFFC001");
                var plainModulus = parms.PlainModulus;
                plainModulus.Resize(7);
                plainModulus.Set(1 << 6);
                var polyModulus = parms.PolyModulus;
                polyModulus.Resize(65, 1);
                polyModulus[0].Set(1);
                polyModulus[64].Set(1);

                var qualifiers = parms.Qualifiers;
                Assert.IsTrue(qualifiers.ParametersSet);
                Assert.IsTrue(qualifiers.EnableRelinearization);
                Assert.IsTrue(qualifiers.EnableNussbaumer);
                Assert.IsTrue(qualifiers.EnableNTT);

                // Commented out due to dependence on the SEAL library #define DISABLE_NTT_IN_MULTIPLY
                //Assert.IsTrue(qualifiers.EnableNTTInMultiply);

                Assert.IsFalse(qualifiers.EnableBatching);
            }

            {
                var parms = new EncryptionParameters
                {
                    DecompositionBitCount = 4,
                    NoiseStandardDeviation = 3.19,
                    NoiseMaxDeviation = 35.06,
                };
                var coeffModulus = parms.CoeffModulus;
                coeffModulus.Resize(48);
                coeffModulus.Set("7FFFFC801");
                var auxCoeffModulus = parms.AuxCoeffModulus;
                auxCoeffModulus.Resize(64);
                auxCoeffModulus.Set("FFFFFF");
                var plainModulus = parms.PlainModulus;
                plainModulus.Resize(7);
                plainModulus.Set(1 << 6);
                var polyModulus = parms.PolyModulus;
                polyModulus.Resize(65, 1);
                polyModulus[0].Set(1);
                polyModulus[64].Set(1);

                var qualifiers = parms.Qualifiers;
                Assert.IsTrue(qualifiers.ParametersSet);
                Assert.IsTrue(qualifiers.EnableRelinearization);
                Assert.IsTrue(qualifiers.EnableNussbaumer);
                Assert.IsTrue(qualifiers.EnableNTT);

                // Commented out due to dependence on the SEAL library #define DISABLE_NTT_IN_MULTIPLY
                //Assert.IsFalse(qualifiers.EnableNTTInMultiply);

                Assert.IsFalse(qualifiers.EnableBatching);
            }

            {
                var parms = new EncryptionParameters
                {
                    DecompositionBitCount = 4,
                    NoiseStandardDeviation = 3.19,
                    NoiseMaxDeviation = 35.06,
                };
                var coeffModulus = parms.CoeffModulus;
                coeffModulus.Resize(48);
                coeffModulus.Set("7FFFFC801");
                var auxCoeffModulus = parms.AuxCoeffModulus;
                auxCoeffModulus.Resize(64);
                auxCoeffModulus.Set("FFFFFFFFC001");
                var plainModulus = parms.PlainModulus;
                plainModulus.Resize(7);
                plainModulus.Set(1 << 6);
                var polyModulus = parms.PolyModulus;
                polyModulus.Resize(64, 1);
                polyModulus[0].Set(1);
                polyModulus[63].Set(1);

                var qualifiers = parms.Qualifiers;
                Assert.IsFalse(qualifiers.ParametersSet);
                Assert.IsFalse(qualifiers.EnableRelinearization);
                Assert.IsFalse(qualifiers.EnableNussbaumer);
                Assert.IsFalse(qualifiers.EnableNTT);

                // Commented out due to dependence on the SEAL library #define DISABLE_NTT_IN_MULTIPLY
                //Assert.IsFalse(qualifiers.EnableNTTInMultiply);

                Assert.IsFalse(qualifiers.EnableBatching);
            }

            {
                var parms = new EncryptionParameters
                {
                    DecompositionBitCount = 4,
                    NoiseStandardDeviation = 3.19,
                    NoiseMaxDeviation = 35.06,
                };
                var coeffModulus = parms.CoeffModulus;
                coeffModulus.Resize(48);
                coeffModulus.Set("0");
                var auxCoeffModulus = parms.AuxCoeffModulus;
                auxCoeffModulus.Resize(64);
                auxCoeffModulus.Set("FFFFFFFFC001");
                var plainModulus = parms.PlainModulus;
                plainModulus.Resize(7);
                plainModulus.Set(1 << 6);
                var polyModulus = parms.PolyModulus;
                polyModulus.Resize(65, 1);
                polyModulus[0].Set(1);
                polyModulus[64].Set(1);

                var qualifiers = parms.Qualifiers;
                Assert.IsFalse(qualifiers.ParametersSet);
                Assert.IsFalse(qualifiers.EnableRelinearization);
                Assert.IsFalse(qualifiers.EnableNussbaumer);
                Assert.IsFalse(qualifiers.EnableNTT);

                // Commented out due to dependence on the SEAL library #define DISABLE_NTT_IN_MULTIPLY
                //Assert.IsFalse(qualifiers.EnableNTTInMultiply);

                Assert.IsFalse(qualifiers.EnableBatching);
            }

            {
                var parms = new EncryptionParameters
                {
                    DecompositionBitCount = 0,
                    NoiseStandardDeviation = 3.19,
                    NoiseMaxDeviation = 35.06,
                };
                var coeffModulus = parms.CoeffModulus;
                coeffModulus.Resize(48);
                coeffModulus.Set("7FFFFC801");
                var auxCoeffModulus = parms.AuxCoeffModulus;
                auxCoeffModulus.Resize(64);
                auxCoeffModulus.Set("FFFFFFFFC001");
                var plainModulus = parms.PlainModulus;
                plainModulus.Resize(7);
                plainModulus.Set(1 << 6);
                var polyModulus = parms.PolyModulus;
                polyModulus.Resize(65, 1);
                polyModulus[0].Set(1);
                polyModulus[64].Set(1);

                var qualifiers = parms.Qualifiers;
                Assert.IsTrue(qualifiers.ParametersSet);
                Assert.IsFalse(qualifiers.EnableRelinearization);
                Assert.IsTrue(qualifiers.EnableNussbaumer);
                Assert.IsTrue(qualifiers.EnableNTT);

                // Commented out due to dependence on the SEAL library #define DISABLE_NTT_IN_MULTIPLY
                //Assert.IsTrue(qualifiers.EnableNTTInMultiply);

                Assert.IsFalse(qualifiers.EnableBatching);
            }

            {
                var parms = new EncryptionParameters
                {
                    DecompositionBitCount = 4,
                    NoiseStandardDeviation = -3.19,
                    NoiseMaxDeviation = 35.06,
                };
                var coeffModulus = parms.CoeffModulus;
                coeffModulus.Resize(48);
                coeffModulus.Set("7FFFFC801");
                var auxCoeffModulus = parms.AuxCoeffModulus;
                auxCoeffModulus.Resize(64);
                auxCoeffModulus.Set("FFFFFFFFC001");
                var plainModulus = parms.PlainModulus;
                plainModulus.Resize(7);
                plainModulus.Set(1 << 6);
                var polyModulus = parms.PolyModulus;
                polyModulus.Resize(65, 1);
                polyModulus[0].Set(1);
                polyModulus[64].Set(1);

                var qualifiers = parms.Qualifiers;
                Assert.IsFalse(qualifiers.ParametersSet);
                Assert.IsFalse(qualifiers.EnableRelinearization);
                Assert.IsFalse(qualifiers.EnableNussbaumer);
                Assert.IsFalse(qualifiers.EnableNTT);

                // Commented out due to dependence on the SEAL library #define DISABLE_NTT_IN_MULTIPLY
                //Assert.IsFalse(qualifiers.EnableNTTInMultiply);

                Assert.IsFalse(qualifiers.EnableBatching);
            }

            {
                var parms = new EncryptionParameters
                {
                    DecompositionBitCount = 0,
                    NoiseStandardDeviation = 3.19,
                    NoiseMaxDeviation = 35.06,
                };
                var coeffModulus = parms.CoeffModulus;
                coeffModulus.Resize(48);
                coeffModulus.Set("7FFFFC801");
                var auxCoeffModulus = parms.AuxCoeffModulus;
                auxCoeffModulus.Resize(64);
                auxCoeffModulus.Set("0");
                var plainModulus = parms.PlainModulus;
                plainModulus.Resize(7);
                plainModulus.Set(1 << 6);
                var polyModulus = parms.PolyModulus;
                polyModulus.Resize(65, 1);
                polyModulus[0].Set(1);
                polyModulus[64].Set(1);

                var qualifiers = parms.Qualifiers;
                Assert.IsTrue(qualifiers.ParametersSet);
                Assert.IsFalse(qualifiers.EnableRelinearization);
                Assert.IsTrue(qualifiers.EnableNussbaumer);
                Assert.IsTrue(qualifiers.EnableNTT);

                // Commented out due to dependence on the SEAL library #define DISABLE_NTT_IN_MULTIPLY
                //Assert.IsFalse(qualifiers.EnableNTTInMultiply);

                Assert.IsFalse(qualifiers.EnableBatching);
            }

            {
                var parms = new EncryptionParameters
                {
                    DecompositionBitCount = 0,
                    NoiseStandardDeviation = 3.19,
                    NoiseMaxDeviation = 35.06,
                };
                var coeffModulus = parms.CoeffModulus;
                coeffModulus.Resize(48);
                coeffModulus.Set("7FFFFFFFF");
                var auxCoeffModulus = parms.AuxCoeffModulus;
                auxCoeffModulus.Resize(64);
                auxCoeffModulus.Set("0");
                var plainModulus = parms.PlainModulus;
                plainModulus.Resize(7);
                plainModulus.Set(1 << 6);
                var polyModulus = parms.PolyModulus;
                polyModulus.Resize(65, 1);
                polyModulus[0].Set(1);
                polyModulus[64].Set(1);

                var qualifiers = parms.Qualifiers;
                Assert.IsTrue(qualifiers.ParametersSet);
                Assert.IsFalse(qualifiers.EnableRelinearization);
                Assert.IsTrue(qualifiers.EnableNussbaumer);
                Assert.IsFalse(qualifiers.EnableNTT);

                // Commented out due to dependence on the SEAL library #define DISABLE_NTT_IN_MULTIPLY
                //Assert.IsFalse(qualifiers.EnableNTTInMultiply);

                Assert.IsFalse(qualifiers.EnableBatching);
            }

            {
                var parms = new EncryptionParameters
                {
                    DecompositionBitCount = 0,
                    NoiseStandardDeviation = 3.19,
                    NoiseMaxDeviation = 35.06,
                };
                var coeffModulus = parms.CoeffModulus;
                coeffModulus.Resize(48);
                coeffModulus.Set("7FFFFFFFF");
                var auxCoeffModulus = parms.AuxCoeffModulus;
                auxCoeffModulus.Resize(64);
                auxCoeffModulus.Set("0");
                var plainModulus = parms.PlainModulus;
                plainModulus.Resize(7);
                plainModulus.Set(12289);
                var polyModulus = parms.PolyModulus;
                polyModulus.Resize(65, 1);
                polyModulus[0].Set(1);
                polyModulus[64].Set(1);

                var qualifiers = parms.Qualifiers;
                Assert.IsTrue(qualifiers.ParametersSet);
                Assert.IsFalse(qualifiers.EnableRelinearization);
                Assert.IsTrue(qualifiers.EnableNussbaumer);
                Assert.IsFalse(qualifiers.EnableNTT);

                // Commented out due to dependence on the SEAL library #define DISABLE_NTT_IN_MULTIPLY
                //Assert.IsFalse(qualifiers.EnableNTTInMultiply);

                Assert.IsTrue(qualifiers.EnableBatching);
            }
        }
    }
}