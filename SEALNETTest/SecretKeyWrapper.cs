using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.Research.SEAL;
using System.IO;
using System.Collections.Generic;

namespace SEALNETTest
{
    [TestClass]
    public class SecretKeyWrapper
    {
        [TestMethod]
        public void SaveLoadSecretKeyNET()
        {
            var stream = new MemoryStream();
            {
                var parms = new EncryptionParameters();
                parms.NoiseStandardDeviation = 3.19;
                parms.PolyModulus = "1x^64 + 1";
                parms.PlainModulus = 1 << 6;
                parms.CoeffModulus = new List<SmallModulus> { DefaultParams.SmallMods60Bit(0) };

                var context = new SEALContext(parms);
                var keygen = new KeyGenerator(context);

                var sk = keygen.SecretKey;
                Assert.AreEqual(sk.HashBlock, parms.HashBlock);
                stream.Seek(0, SeekOrigin.Begin);
                sk.Save(stream);

                var sk2 = new SecretKey();
                stream.Seek(0, SeekOrigin.Begin);
                sk2.Load(stream);
                Assert.AreEqual(sk.Data, sk2.Data);
                Assert.AreEqual(sk.HashBlock, sk2.HashBlock);
            }
            {
                var parms = new EncryptionParameters();
                parms.NoiseStandardDeviation = 3.19;
                parms.PolyModulus = "1x^256 + 1";
                parms.PlainModulus = 1 << 20;
                parms.CoeffModulus = new List<SmallModulus> {
                    DefaultParams.SmallMods30Bit(0), DefaultParams.SmallMods40Bit(0) };

                var context = new SEALContext(parms);
                var keygen = new KeyGenerator(context);

                var sk = keygen.SecretKey;
                Assert.AreEqual(sk.HashBlock, parms.HashBlock);
                stream.Seek(0, SeekOrigin.Begin);
                sk.Save(stream);

                var sk2 = new SecretKey();
                stream.Seek(0, SeekOrigin.Begin);
                sk2.Load(stream);
                Assert.AreEqual(sk.Data, sk2.Data);
                Assert.AreEqual(sk.HashBlock, sk2.HashBlock);
            }
        }
    }
}