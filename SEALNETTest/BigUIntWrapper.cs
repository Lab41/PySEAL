using System.IO;
using Microsoft.Research.SEAL;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace SEALNETTest
{
    [TestClass]
    public unsafe class BigUIntWrapper
    {
        [TestMethod]
        public void EmptyBigUIntNET()
        {
            var biguint = new BigUInt();
            Assert.AreEqual(0, biguint.BitCount);
            Assert.AreEqual(0, biguint.ByteCount);
            Assert.AreEqual(0, biguint.UInt64Count);
            Assert.IsTrue(null == biguint.Pointer);
            Assert.AreEqual(0, biguint.GetSignificantBitCount());
            Assert.AreEqual("0", biguint.ToString());
            Assert.IsTrue(biguint.IsZero);
            Assert.IsFalse(biguint.IsAlias);
            biguint.SetZero();

            var biguint2 = new BigUInt();
            Assert.IsTrue(biguint.Equals(biguint2));

            biguint.Resize(1);
            Assert.AreEqual(1, biguint.BitCount);
            Assert.IsTrue(null != biguint.Pointer);
            Assert.IsFalse(biguint.IsAlias);

            biguint.Resize(0);
            Assert.AreEqual(0, biguint.BitCount);
            Assert.IsTrue(null == biguint.Pointer);
            Assert.IsFalse(biguint.IsAlias);
        }

        [TestMethod]
        public void BigUInt64BitsNET()
        {
            var biguint = new BigUInt(64);
            Assert.AreEqual(64, biguint.BitCount);
            Assert.AreEqual(8, biguint.ByteCount);
            Assert.AreEqual(1, biguint.UInt64Count);
            Assert.AreEqual(0, biguint.GetSignificantBitCount());
            Assert.AreEqual("0", biguint.ToString());
            Assert.IsTrue(biguint.IsZero);
            Assert.IsTrue(null != biguint.Pointer);
            Assert.AreEqual(0UL, biguint.Pointer[0]);
            Assert.AreEqual(0, biguint[0]);
            Assert.AreEqual(0, biguint[1]);
            Assert.AreEqual(0, biguint[2]);
            Assert.AreEqual(0, biguint[3]);
            Assert.AreEqual(0, biguint[4]);
            Assert.AreEqual(0, biguint[5]);
            Assert.AreEqual(0, biguint[6]);
            Assert.AreEqual(0, biguint[7]);

            biguint.Set(1);
            Assert.AreEqual(1, biguint.GetSignificantBitCount());
            Assert.AreEqual("1", biguint.ToString());
            Assert.IsFalse(biguint.IsZero);
            Assert.AreEqual(1UL, biguint.Pointer[0]);
            Assert.AreEqual(1, biguint[0]);
            Assert.AreEqual(0, biguint[1]);
            Assert.AreEqual(0, biguint[2]);
            Assert.AreEqual(0, biguint[3]);
            Assert.AreEqual(0, biguint[4]);
            Assert.AreEqual(0, biguint[5]);
            Assert.AreEqual(0, biguint[6]);
            Assert.AreEqual(0, biguint[7]);
            biguint.SetZero();
            Assert.IsTrue(biguint.IsZero);
            Assert.AreEqual(0UL, biguint.Pointer[0]);

            biguint.Set("7FFFFFFFFFFFFFFF");
            Assert.AreEqual(63, biguint.GetSignificantBitCount());
            Assert.IsFalse(biguint.IsZero);
            Assert.AreEqual("7FFFFFFFFFFFFFFF", biguint.ToString());
            Assert.AreEqual(0x7FFFFFFFFFFFFFFFUL, biguint.Pointer[0]);
            Assert.AreEqual(0xFF, biguint[0]);
            Assert.AreEqual(0xFF, biguint[1]);
            Assert.AreEqual(0xFF, biguint[2]);
            Assert.AreEqual(0xFF, biguint[3]);
            Assert.AreEqual(0xFF, biguint[4]);
            Assert.AreEqual(0xFF, biguint[5]);
            Assert.AreEqual(0xFF, biguint[6]);
            Assert.AreEqual(0x7F, biguint[7]);

            biguint.Set("FFFFFFFFFFFFFFFF");
            Assert.AreEqual(64, biguint.GetSignificantBitCount());
            Assert.IsFalse(biguint.IsZero);
            Assert.AreEqual("FFFFFFFFFFFFFFFF", biguint.ToString());
            Assert.AreEqual(0xFFFFFFFFFFFFFFFFUL, biguint.Pointer[0]);
            Assert.AreEqual(0xFF, biguint[0]);
            Assert.AreEqual(0xFF, biguint[1]);
            Assert.AreEqual(0xFF, biguint[2]);
            Assert.AreEqual(0xFF, biguint[3]);
            Assert.AreEqual(0xFF, biguint[4]);
            Assert.AreEqual(0xFF, biguint[5]);
            Assert.AreEqual(0xFF, biguint[6]);
            Assert.AreEqual(0xFF, biguint[7]);

            biguint.Set(0x8001);
            Assert.AreEqual(16, biguint.GetSignificantBitCount());
            Assert.AreEqual("8001", biguint.ToString());
            Assert.AreEqual(0x8001UL, biguint.Pointer[0]);
            Assert.AreEqual(0x01, biguint[0]);
            Assert.AreEqual(0x80, biguint[1]);
            Assert.AreEqual(0x00, biguint[2]);
            Assert.AreEqual(0x00, biguint[3]);
            Assert.AreEqual(0x00, biguint[4]);
            Assert.AreEqual(0x00, biguint[5]);
            Assert.AreEqual(0x00, biguint[6]);
            Assert.AreEqual(0x00, biguint[7]);
        }

        [TestMethod]
        public void BigUInt99BitsNET()
        {
            var biguint = new BigUInt(99);
            Assert.AreEqual(99, biguint.BitCount);
            Assert.IsTrue(null != biguint.Pointer);
            Assert.AreEqual(13, biguint.ByteCount);
            Assert.AreEqual(2, biguint.UInt64Count);
            Assert.AreEqual(0, biguint.GetSignificantBitCount());
            Assert.AreEqual("0", biguint.ToString());
            Assert.IsTrue(biguint.IsZero);
            Assert.AreEqual(0UL, biguint.Pointer[0]);
            Assert.AreEqual(0UL, biguint.Pointer[1]);
            Assert.AreEqual(0, biguint[0]);
            Assert.AreEqual(0, biguint[1]);
            Assert.AreEqual(0, biguint[2]);
            Assert.AreEqual(0, biguint[3]);
            Assert.AreEqual(0, biguint[4]);
            Assert.AreEqual(0, biguint[5]);
            Assert.AreEqual(0, biguint[6]);
            Assert.AreEqual(0, biguint[7]);
            Assert.AreEqual(0, biguint[8]);
            Assert.AreEqual(0, biguint[9]);
            Assert.AreEqual(0, biguint[10]);
            Assert.AreEqual(0, biguint[11]);
            Assert.AreEqual(0, biguint[12]);

            biguint.Set(1);
            Assert.AreEqual(1, biguint.GetSignificantBitCount());
            Assert.AreEqual("1", biguint.ToString());
            Assert.IsFalse(biguint.IsZero);
            Assert.AreEqual(1UL, biguint.Pointer[0]);
            Assert.AreEqual(0UL, biguint.Pointer[1]);
            Assert.AreEqual(1, biguint[0]);
            Assert.AreEqual(0, biguint[1]);
            Assert.AreEqual(0, biguint[2]);
            Assert.AreEqual(0, biguint[3]);
            Assert.AreEqual(0, biguint[4]);
            Assert.AreEqual(0, biguint[5]);
            Assert.AreEqual(0, biguint[6]);
            Assert.AreEqual(0, biguint[7]);
            Assert.AreEqual(0, biguint[8]);
            Assert.AreEqual(0, biguint[9]);
            Assert.AreEqual(0, biguint[10]);
            Assert.AreEqual(0, biguint[11]);
            Assert.AreEqual(0, biguint[12]);
            biguint.SetZero();
            Assert.IsTrue(biguint.IsZero);
            Assert.AreEqual(0UL, biguint.Pointer[0]);
            Assert.AreEqual(0UL, biguint.Pointer[1]);

            biguint.Set("7FFFFFFFFFFFFFFFFFFFFFFFF");
            Assert.AreEqual(99, biguint.GetSignificantBitCount());
            Assert.AreEqual("7FFFFFFFFFFFFFFFFFFFFFFFF", biguint.ToString());
            Assert.IsFalse(biguint.IsZero);
            Assert.AreEqual(0xFFFFFFFFFFFFFFFFUL, biguint.Pointer[0]);
            Assert.AreEqual(0x7FFFFFFFFUL, biguint.Pointer[1]);
            Assert.AreEqual(0xFF, biguint[0]);
            Assert.AreEqual(0xFF, biguint[1]);
            Assert.AreEqual(0xFF, biguint[2]);
            Assert.AreEqual(0xFF, biguint[3]);
            Assert.AreEqual(0xFF, biguint[4]);
            Assert.AreEqual(0xFF, biguint[5]);
            Assert.AreEqual(0xFF, biguint[6]);
            Assert.AreEqual(0xFF, biguint[7]);
            Assert.AreEqual(0xFF, biguint[8]);
            Assert.AreEqual(0xFF, biguint[9]);
            Assert.AreEqual(0xFF, biguint[10]);
            Assert.AreEqual(0xFF, biguint[11]);
            Assert.AreEqual(0x07, biguint[12]);
            biguint.SetZero();
            Assert.IsTrue(biguint.IsZero);
            Assert.AreEqual(0UL, biguint.Pointer[0]);
            Assert.AreEqual(0UL, biguint.Pointer[1]);

            biguint.Set("4000000000000000000000000");
            Assert.AreEqual(99, biguint.GetSignificantBitCount());
            Assert.AreEqual("4000000000000000000000000", biguint.ToString());
            Assert.IsFalse(biguint.IsZero);
            Assert.AreEqual(0UL, biguint.Pointer[0]);
            Assert.AreEqual(0x400000000UL, biguint.Pointer[1]);
            Assert.AreEqual(0, biguint[0]);
            Assert.AreEqual(0, biguint[1]);
            Assert.AreEqual(0, biguint[2]);
            Assert.AreEqual(0, biguint[3]);
            Assert.AreEqual(0, biguint[4]);
            Assert.AreEqual(0, biguint[5]);
            Assert.AreEqual(0, biguint[6]);
            Assert.AreEqual(0, biguint[7]);
            Assert.AreEqual(0, biguint[8]);
            Assert.AreEqual(0, biguint[9]);
            Assert.AreEqual(0, biguint[10]);
            Assert.AreEqual(0, biguint[11]);
            Assert.AreEqual(0x04, biguint[12]);

            biguint.Set(0x8001);
            Assert.AreEqual(16, biguint.GetSignificantBitCount());
            Assert.AreEqual("8001", biguint.ToString());
            Assert.IsFalse(biguint.IsZero);
            Assert.AreEqual(0x8001UL, biguint.Pointer[0]);
            Assert.AreEqual(0UL, biguint.Pointer[1]);
            Assert.AreEqual(0x01, biguint[0]);
            Assert.AreEqual(0x80, biguint[1]);
            Assert.AreEqual(0, biguint[2]);
            Assert.AreEqual(0, biguint[3]);
            Assert.AreEqual(0, biguint[4]);
            Assert.AreEqual(0, biguint[5]);
            Assert.AreEqual(0, biguint[6]);
            Assert.AreEqual(0, biguint[7]);
            Assert.AreEqual(0, biguint[8]);
            Assert.AreEqual(0, biguint[9]);
            Assert.AreEqual(0, biguint[10]);
            Assert.AreEqual(0, biguint[11]);
            Assert.AreEqual(0, biguint[12]);

            var biguint2 = new BigUInt("123");
            Assert.IsFalse(biguint.Equals(biguint2));
            Assert.IsFalse(biguint2.Equals(biguint));
            Assert.AreNotEqual(biguint.GetHashCode(), biguint2.GetHashCode());

            biguint.Set(biguint2);
            Assert.IsTrue(biguint.Equals(biguint2));
            Assert.IsTrue(biguint2.Equals(biguint));
            Assert.AreEqual(biguint.GetHashCode(), biguint2.GetHashCode());
            Assert.AreEqual(9, biguint.GetSignificantBitCount());
            Assert.AreEqual("123", biguint.ToString());
            Assert.AreEqual(0x123UL, biguint.Pointer[0]);
            Assert.AreEqual(0UL, biguint.Pointer[1]);
            Assert.AreEqual(0x23, biguint[0]);
            Assert.AreEqual(0x01, biguint[1]);
            Assert.AreEqual(0, biguint[2]);
            Assert.AreEqual(0, biguint[3]);
            Assert.AreEqual(0, biguint[4]);
            Assert.AreEqual(0, biguint[5]);
            Assert.AreEqual(0, biguint[6]);
            Assert.AreEqual(0, biguint[7]);
            Assert.AreEqual(0, biguint[8]);
            Assert.AreEqual(0, biguint[9]);
            Assert.AreEqual(0, biguint[10]);
            Assert.AreEqual(0, biguint[11]);
            Assert.AreEqual(0, biguint[12]);

            biguint.Resize(8);
            Assert.AreEqual(8, biguint.BitCount);
            Assert.AreEqual(1, biguint.UInt64Count);
            Assert.AreEqual("23", biguint.ToString());

            biguint.Resize(100);
            Assert.AreEqual(100, biguint.BitCount);
            Assert.AreEqual(2, biguint.UInt64Count);
            Assert.AreEqual("23", biguint.ToString());

            biguint.Resize(0);
            Assert.AreEqual(0, biguint.BitCount);
            Assert.AreEqual(0, biguint.UInt64Count);
            Assert.IsTrue(null == biguint.Pointer);
        }

        [TestMethod]
        public void SaveLoadUIntNET()
        {
            var stream = new MemoryStream();

            var value = new BigUInt();
            var value2 = new BigUInt("100");
            stream.Seek(0, SeekOrigin.Begin);
            value.Save(stream);
            stream.Seek(0, SeekOrigin.Begin);
            value2.Load(stream);
            Assert.AreEqual(value, value2);

            value.Set("123");
            stream.Seek(0, SeekOrigin.Begin);
            value.Save(stream);
            stream.Seek(0, SeekOrigin.Begin);
            value2.Load(stream);
            Assert.AreEqual(value, value2);

            value.Set("FFFFFFFFFFFFFFFFFFFFFFFFFF");
            stream.Seek(0, SeekOrigin.Begin);
            value.Save(stream);
            stream.Seek(0, SeekOrigin.Begin);
            value2.Load(stream);
            Assert.AreEqual(value, value2);

            value.Set("0");
            stream.Seek(0, SeekOrigin.Begin);
            value.Save(stream);
            stream.Seek(0, SeekOrigin.Begin);
            value2.Load(stream);
            Assert.AreEqual(value, value2);
        }

        [TestMethod]
        public void DuplicateToNET()
        {
            var original = new BigUInt(123);
            original.Set(56789);

            var target = new BigUInt();

            original.DuplicateTo(target);
            Assert.AreEqual(target.BitCount, original.BitCount);
            Assert.IsTrue(target.Equals(original));
        }

        [TestMethod]
        public void DuplicateFromNET()
        {
            var original = new BigUInt(123);
            original.Set(56789);

            var target = new BigUInt();

            target.DuplicateFrom(original);
            Assert.AreEqual(target.BitCount, original.BitCount);
            Assert.IsTrue(target.Equals(original));
        }
    }
}

