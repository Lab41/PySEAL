using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.Research.SEAL;
using System.IO;
using System.Collections.Generic;

namespace SEALNETTest
{
    [TestClass]
    public class PublicKeyWrapper
    {
        [TestMethod]
        public void SaveLoadPublicKeyNET()
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

                var pk = keygen.PublicKey;
                Assert.IsTrue(pk.HashBlock.Equals(parms.HashBlock));
                stream.Seek(0, SeekOrigin.Begin);
                pk.Save(stream);

                var pk2 = new PublicKey();
                stream.Seek(0, SeekOrigin.Begin);
                pk2.Load(stream);
                Assert.AreEqual(pk.Data, pk2.Data);
                Assert.AreEqual(pk.HashBlock, pk2.HashBlock);
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

                var pk = keygen.PublicKey;
                Assert.IsTrue(pk.HashBlock.Equals(parms.HashBlock));
                stream.Seek(0, SeekOrigin.Begin);
                pk.Save(stream);

                var pk2 = new PublicKey();
                stream.Seek(0, SeekOrigin.Begin);
                pk2.Load(stream);
                Assert.AreEqual(pk.Data, pk2.Data);
                Assert.AreEqual(pk.HashBlock, pk2.HashBlock);
            }
        }
    }
}