using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.Research.SEAL;
using System.Collections.Generic;
using System;
using System.Linq;

namespace SEALNETTest
{
    [TestClass]
    public class PolyCRTBuilderWrapper
    {
        [TestMethod]
        public void BatchUnbatchUIntVectorNET()
        {
            var parms = new EncryptionParameters();
            parms.PolyModulus = "1x^64 + 1";
            parms.CoeffModulus = new List<SmallModulus> { DefaultParams.SmallMods60Bit(0) };
            parms.PlainModulus = 257;

            var context = new SEALContext(parms);
            Assert.IsTrue(context.Qualifiers.EnableBatching);

            var crtbuilder = new PolyCRTBuilder(context);
            Assert.AreEqual(64, crtbuilder.SlotCount);
            var plain_vec = new List<UInt64>();
            for (int i = 0; i < crtbuilder.SlotCount; i++)
            {
                plain_vec.Add((UInt64)i);
            }

            var plain = new Plaintext();
            crtbuilder.Compose(plain_vec, plain);
            var plain_vec2 = new List<UInt64>();
            crtbuilder.Decompose(plain, plain_vec2);
            Assert.IsTrue(plain_vec.SequenceEqual(plain_vec2));

            for (int i = 0; i < crtbuilder.SlotCount; i++)
            {
                plain_vec[i] = 5;
            }
            crtbuilder.Compose(plain_vec, plain);
            Assert.IsTrue(plain.ToString().Equals("5"));
            crtbuilder.Decompose(plain, plain_vec2);
            Assert.IsTrue(plain_vec.SequenceEqual(plain_vec2));

            var short_plain_vec = new List<UInt64>();
            for (int i = 0; i < 20; i++)
            {
                short_plain_vec.Add((UInt64)i);
            }
            crtbuilder.Compose(short_plain_vec, plain);
            var short_plain_vec2 = new List<UInt64>();
            crtbuilder.Decompose(plain, short_plain_vec2);
            Assert.AreEqual(20, short_plain_vec.Count);
            Assert.AreEqual(64, short_plain_vec2.Count);
            for (int i = 0; i < 20; i++)
            {
                Assert.AreEqual(short_plain_vec[i], short_plain_vec2[i]);
            }
            for (int i = 20; i < crtbuilder.SlotCount; i++)
            {
                Assert.AreEqual(0UL, short_plain_vec2[i]);
            }
        }

        [TestMethod]
        public void BatchUnbatchIntVectorNET()
        {
            var parms = new EncryptionParameters();
            parms.PolyModulus = "1x^64 + 1";
            parms.CoeffModulus = new List<SmallModulus> { DefaultParams.SmallMods60Bit(0) };
            parms.PlainModulus = 257;

            var context = new SEALContext(parms);
            Assert.IsTrue(context.Qualifiers.EnableBatching);

            var crtbuilder = new PolyCRTBuilder(context);
            Assert.AreEqual(64, crtbuilder.SlotCount);
            var plain_vec = new List<Int64>();
            for (int i = 0; i < crtbuilder.SlotCount; i++)
            {
                plain_vec.Add((Int64)i * (1 - 2 * (i % 2)));
            }

            var plain = new Plaintext();
            crtbuilder.Compose(plain_vec, plain);
            var plain_vec2 = new List<Int64>();
            crtbuilder.Decompose(plain, plain_vec2);
            Assert.IsTrue(plain_vec.SequenceEqual(plain_vec2));

            for (int i = 0; i < crtbuilder.SlotCount; i++)
            {
                plain_vec[i] = -5;
            }
            crtbuilder.Compose(plain_vec, plain);
            Assert.IsTrue(plain.ToString().Equals("FC"));
            crtbuilder.Decompose(plain, plain_vec2);
            Assert.IsTrue(plain_vec.SequenceEqual(plain_vec2));

            var short_plain_vec = new List<Int64>();
            for (int i = 0; i < 20; i++)
            {
                short_plain_vec.Add((Int64)i * (1 - 2 * (i % 2)));
            }
            crtbuilder.Compose(short_plain_vec, plain);
            var short_plain_vec2 = new List<Int64>();
            crtbuilder.Decompose(plain, short_plain_vec2);
            Assert.AreEqual(20, short_plain_vec.Count);
            Assert.AreEqual(64, short_plain_vec2.Count);
            for (int i = 0; i < 20; i++)
            {
                Assert.AreEqual(short_plain_vec[i], short_plain_vec2[i]);
            }
            for (int i = 20; i < crtbuilder.SlotCount; i++)
            {
                Assert.AreEqual(0L, short_plain_vec2[i]);
            }
        }

        [TestMethod]
        public void BatchUnbatchPlaintextNET()
        {
            var parms = new EncryptionParameters();
            parms.PolyModulus = "1x^64 + 1";
            parms.CoeffModulus = new List<SmallModulus> { DefaultParams.SmallMods60Bit(0) };
            parms.PlainModulus = 257;

            var context = new SEALContext(parms);
            Assert.IsTrue(context.Qualifiers.EnableBatching);

            var crtbuilder = new PolyCRTBuilder(context);
            Assert.AreEqual(64, crtbuilder.SlotCount);
            var plain = new Plaintext(crtbuilder.SlotCount);
            for (int i = 0; i < crtbuilder.SlotCount; i++)
            {
                plain[i] = (UInt64)i;
            }

            crtbuilder.Compose(plain);
            crtbuilder.Decompose(plain);
            for (int i = 0; i < crtbuilder.SlotCount; i++)
            {
                Assert.IsTrue(plain[i] == (UInt64)i);
            }

            for (int i = 0; i < crtbuilder.SlotCount; i++)
            {
                plain[i] = (UInt64)5;
            }
            crtbuilder.Compose(plain);
            Assert.IsTrue(plain.ToString().Equals("5"));
            crtbuilder.Decompose(plain);
            for (int i = 0; i < crtbuilder.SlotCount; i++)
            {
                Assert.IsTrue(plain[i] == (UInt64)5);
            }

            var short_plain = new Plaintext(20);
            for (int i = 0; i < 20; i++)
            {
                short_plain[i] = (UInt64)i;
            }
            crtbuilder.Compose(short_plain);
            crtbuilder.Decompose(short_plain);
            for (int i = 0; i < 20; i++)
            {
                Assert.IsTrue(short_plain[i] == (UInt64)i);
            }
            for (int i = 20; i < crtbuilder.SlotCount; i++)
            {
                Assert.IsTrue(short_plain[i] == 0UL);
            }
        }
    }
}