using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.Research.SEAL;
using System.IO;

namespace SEALNETTest
{
    [TestClass]
    public class SmallModulusWrapper
    {
        [TestMethod]
        public void CreateSmallModulusNET()
        {
            var mod = new SmallModulus();
            Assert.IsTrue(mod.IsZero);
            Assert.AreEqual(0UL, mod.Value);
            Assert.AreEqual(0, mod.BitCount);
            Assert.AreEqual(1, mod.UInt64Count);
            Assert.AreEqual(0UL, mod.ConstRatio.Item1);
            Assert.AreEqual(0UL, mod.ConstRatio.Item2);
            Assert.AreEqual(0UL, mod.ConstRatio.Item3);

            mod.Set(3);
            Assert.IsFalse(mod.IsZero);
            Assert.AreEqual(3UL, mod.Value);
            Assert.AreEqual(2, mod.BitCount);
            Assert.AreEqual(1, mod.UInt64Count);
            Assert.AreEqual(6148914691236517205UL, mod.ConstRatio.Item1);
            Assert.AreEqual(6148914691236517205UL, mod.ConstRatio.Item2);
            Assert.AreEqual(1UL, mod.ConstRatio.Item3);

            var mod2 = new SmallModulus(2);
            var mod3 = new SmallModulus(3);
            Assert.IsFalse(mod.Equals(mod2));
            Assert.IsTrue(mod.Equals(mod3));

            mod.Set(0);
            Assert.IsTrue(mod.IsZero);
            Assert.AreEqual(0UL, mod.Value);
            Assert.AreEqual(0, mod.BitCount);
            Assert.AreEqual(1, mod.UInt64Count);
            Assert.AreEqual(0UL, mod.ConstRatio.Item1);
            Assert.AreEqual(0UL, mod.ConstRatio.Item2);
            Assert.AreEqual(0UL, mod.ConstRatio.Item3);

            mod.Set(0xF00000F00000F);
            Assert.IsFalse(mod.IsZero);
            Assert.AreEqual(0xF00000F00000FUL, mod.Value);
            Assert.AreEqual(52, mod.BitCount);
            Assert.AreEqual(1, mod.UInt64Count);
            Assert.AreEqual(1224979098644774929UL, mod.ConstRatio.Item1);
            Assert.AreEqual(4369UL, mod.ConstRatio.Item2);
            Assert.AreEqual(281470698520321UL, mod.ConstRatio.Item3);
        }

        [TestMethod]
        public void SaveLoadSmallModulusNET()
        {
            var stream = new MemoryStream();

            var mod = new SmallModulus();
            stream.Seek(0, SeekOrigin.Begin);
            mod.Save(stream);

            var mod2 = new SmallModulus();
            stream.Seek(0, SeekOrigin.Begin);
            mod2.Load(stream);
            Assert.AreEqual(mod2.Value, mod.Value);
            Assert.AreEqual(mod2.BitCount, mod.BitCount);
            Assert.AreEqual(mod2.UInt64Count, mod.UInt64Count);
            Assert.AreEqual(mod2.ConstRatio.Item1, mod.ConstRatio.Item1);
            Assert.AreEqual(mod2.ConstRatio.Item2, mod.ConstRatio.Item2);
            Assert.AreEqual(mod2.ConstRatio.Item3, mod.ConstRatio.Item3);

            mod.Set(3);
            stream.Seek(0, SeekOrigin.Begin);
            mod.Save(stream);
            stream.Seek(0, SeekOrigin.Begin);
            mod2.Load(stream);
            Assert.AreEqual(mod2.Value, mod.Value);
            Assert.AreEqual(mod2.BitCount, mod.BitCount);
            Assert.AreEqual(mod2.UInt64Count, mod.UInt64Count);
            Assert.AreEqual(mod2.ConstRatio.Item1, mod.ConstRatio.Item1);
            Assert.AreEqual(mod2.ConstRatio.Item2, mod.ConstRatio.Item2);
            Assert.AreEqual(mod2.ConstRatio.Item3, mod.ConstRatio.Item3);

            mod.Set(0xF00000F00000F);
            stream.Seek(0, SeekOrigin.Begin);
            mod.Save(stream);
            stream.Seek(0, SeekOrigin.Begin);
            mod2.Load(stream);
            Assert.AreEqual(mod2.Value, mod.Value);
            Assert.AreEqual(mod2.BitCount, mod.BitCount);
            Assert.AreEqual(mod2.UInt64Count, mod.UInt64Count);
            Assert.AreEqual(mod2.ConstRatio.Item1, mod.ConstRatio.Item1);
            Assert.AreEqual(mod2.ConstRatio.Item2, mod.ConstRatio.Item2);
            Assert.AreEqual(mod2.ConstRatio.Item3, mod.ConstRatio.Item3);
        }
    }
}