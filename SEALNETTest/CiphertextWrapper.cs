using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.Research.SEAL;
using System.IO;
using System;
using System.Collections.Generic;

namespace SEALNETTest
{
    [TestClass]
    public class CiphertextWrapper
    {
        public void CiphertextBasicsNET()
        {
            var parms = new EncryptionParameters();

            parms.PolyModulus = "1x^2 + 1";
            parms.CoeffModulus = new List<SmallModulus> { 2 };
            parms.PlainModulus = 2;
            parms.NoiseStandardDeviation = 1.0;

            var ctxt = new Ciphertext(parms);
            ctxt.Reserve(10);
            Assert.AreEqual(2, ctxt.Size);
            Assert.AreEqual(2 * 3 * 1, ctxt.UInt64Count);
            Assert.AreEqual(10 * 3 * 1, ctxt.UInt64CountCapacity);
            Assert.IsTrue(ctxt.HashBlock.Equals(parms.HashBlock));

            ctxt.Reserve(5);
            Assert.AreEqual(2, ctxt.Size);
            Assert.AreEqual(2 * 3 * 1, ctxt.UInt64Count);
            Assert.AreEqual(5 * 3 * 1, ctxt.UInt64CountCapacity);
            Assert.IsTrue(ctxt.HashBlock.Equals(parms.HashBlock));

            ctxt.Reserve(10);
            Assert.AreEqual(2, ctxt.Size);
            Assert.AreEqual(2 * 3 * 1, ctxt.UInt64Count);
            Assert.AreEqual(10 * 3 * 1, ctxt.UInt64CountCapacity);
            Assert.IsTrue(ctxt.HashBlock.Equals(parms.HashBlock));

            ctxt.Reserve(2);
            Assert.AreEqual(2, ctxt.Size);
            Assert.AreEqual(2 * 3 * 1, ctxt.UInt64Count);
            Assert.AreEqual(2 * 3 * 1, ctxt.UInt64CountCapacity);
            Assert.IsTrue(ctxt.HashBlock.Equals(parms.HashBlock));

            ctxt.Reserve(5);
            Assert.AreEqual(2, ctxt.Size);
            Assert.AreEqual(2 * 3 * 1, ctxt.UInt64Count);
            Assert.AreEqual(5 * 3 * 1, ctxt.UInt64CountCapacity);
            Assert.IsTrue(ctxt.HashBlock.Equals(parms.HashBlock));
        }

        [TestMethod]
        public void SaveLoadCiphertextNET()
        {
            var stream = new MemoryStream();
            var parms = new EncryptionParameters();
            parms.PolyModulus = "1x^2 + 1";
            parms.CoeffModulus = new List<SmallModulus> { 2 };
            parms.PlainModulus = 2;
            parms.NoiseStandardDeviation = 1.0;

            var ctxt = new Ciphertext(parms);
            var ctxt2 = new Ciphertext();
            ctxt.Save(stream);
            stream.Seek(0, SeekOrigin.Begin);
            ctxt2.Load(stream);
            Assert.IsTrue(ctxt.HashBlock.Equals(ctxt2.HashBlock));
        }
    }
}