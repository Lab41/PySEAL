using System.IO;
using Microsoft.Research.SEAL;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace SEALNETTest
{
    [TestClass]
    public class EvaluationKeysWrapper
    {
        [TestMethod]
        public void EvaluationKeysTestNET()
        {
            var keys = new EvaluationKeys();
            Assert.AreEqual(0, keys.Count);

            keys.Clear();
            Assert.AreEqual(0, keys.Count);

            keys.Resize(2);
            Assert.AreEqual(2, keys.Count);
            keys[0].Resize(2, 32);
            keys[0][0].Set(1);
            keys[0][1].Set(2);
            keys[1].Resize(3, 32);
            keys[1][0].Set(3);
            keys[1][1].Set(4);
            keys[1][2].Set(5);
            Assert.AreEqual("2x^1 + 1", keys[0].ToString());
            Assert.AreEqual("5x^2 + 4x^1 + 3", keys[1].ToString());

            var keys2 = new EvaluationKeys();
            Assert.AreEqual(0, keys2.Count);
            keys2.Set(keys);
            Assert.AreEqual(2, keys2.Count);
            Assert.AreEqual("2x^1 + 1", keys2[0].ToString());
            Assert.AreEqual("5x^2 + 4x^1 + 3", keys2[1].ToString());
            keys2[1].SetZero();
            Assert.AreEqual("5x^2 + 4x^1 + 3", keys[1].ToString());

            keys.Resize(3);
            Assert.AreEqual(3, keys.Count);
            Assert.AreEqual("2x^1 + 1", keys[0].ToString());
            Assert.AreEqual("5x^2 + 4x^1 + 3", keys[1].ToString());
            Assert.IsTrue(keys[2].IsZero);

            keys.Resize(1);
            Assert.AreEqual(1, keys.Count);
            Assert.AreEqual("2x^1 + 1", keys[0].ToString());

            keys.Clear();
            Assert.AreEqual(0, keys.Count);
        }

        [TestMethod]
        public void SaveLoadEvaluationKeysNET()
        {
            var stream = new MemoryStream();

            var keys = new EvaluationKeys(3);
            keys[0].Resize(3, 32);
            keys[0][0].Set(1);
            keys[0][1].Set(2);
            keys[0][2].Set(3);
            keys[1].Resize(2, 96);
            keys[1][0].Set(4);
            keys[1][1].Set(5);
            keys[2].Resize(1, 24);
            keys[2][0].Set(6);

            var keys2 = new EvaluationKeys();
            stream.Seek(0, SeekOrigin.Begin);
            keys.Save(stream);
            stream.Seek(0, SeekOrigin.Begin);
            keys2.Load(stream);

            Assert.AreEqual(3, keys2.Count);
            Assert.AreEqual(keys[0], keys2[0]);
            Assert.AreEqual(keys[1], keys2[1]);
            Assert.AreEqual(keys[2], keys2[2]);
        }
    }
}
