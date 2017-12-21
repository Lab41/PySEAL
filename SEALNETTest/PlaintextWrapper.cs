using Microsoft.Research.SEAL;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.IO;

namespace SEALNETTest
{
    [TestClass]
    public class PlaintextWrapper
    {
        public void PlaintextBasicsNET()
        {
            var plain = new Plaintext(2);
            Assert.AreEqual(2, plain.Capacity);
            Assert.AreEqual(2, plain.CoeffCount);
            Assert.AreEqual(0, plain.SignificantCoeffCount());
            plain[0] = 1;
            plain[1] = 2;

            plain.Reserve(10);
            Assert.AreEqual(10, plain.Capacity);
            Assert.AreEqual(2, plain.CoeffCount);
            Assert.AreEqual(2, plain.SignificantCoeffCount());
            Assert.AreEqual(1UL, plain[0]);
            Assert.AreEqual(2UL, plain[1]);

            plain.Resize(5);
            Assert.AreEqual(10, plain.Capacity);
            Assert.AreEqual(5, plain.CoeffCount);
            Assert.AreEqual(2, plain.SignificantCoeffCount());
            Assert.AreEqual(1UL, plain[0]);
            Assert.AreEqual(2UL, plain[1]);
            Assert.AreEqual(0UL, plain[2]);
            Assert.AreEqual(0UL, plain[3]);
            Assert.AreEqual(0UL, plain[4]);
        }

        [TestMethod]
        public void SaveLoadPlaintextNET()
        {
            var stream = new MemoryStream();

            var plain = new Plaintext();
            var plain2 = new Plaintext();
            plain.Save(stream);
            stream.Seek(0, SeekOrigin.Begin);
            plain2.Load(stream);
            Assert.AreEqual(0, plain2.Capacity);
            Assert.AreEqual(0, plain2.CoeffCount);

            plain.Reserve(20);
            plain.Resize(5);
            plain[0] = 1;
            plain[1] = 2;
            plain[2] = 3;
            stream.Seek(0, SeekOrigin.Begin);
            plain.Save(stream);
            stream.Seek(0, SeekOrigin.Begin);
            plain2.Load(stream);
            Assert.AreEqual(5, plain2.Capacity);
            Assert.AreEqual(5, plain2.CoeffCount);
            Assert.AreEqual(1UL, plain2[0]);
            Assert.AreEqual(2UL, plain2[1]);
            Assert.AreEqual(3UL, plain2[2]);
            Assert.AreEqual(0UL, plain2[3]);
            Assert.AreEqual(0UL, plain2[4]);
        }
    }
}