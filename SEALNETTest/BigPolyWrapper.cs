using System.IO;
using Microsoft.Research.SEAL;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace SEALNETTest
{
    [TestClass]
    public unsafe class BigPolyWrapper
    {
        [TestMethod]
        public void EmptyBigPolyNET()
        {
            var poly = new BigPoly();
            Assert.AreEqual(0, poly.CoeffCount);
            Assert.AreEqual(0, poly.CoeffBitCount);
            Assert.AreEqual(0, poly.UInt64Count);
            Assert.IsTrue(null == poly.Pointer);
            Assert.AreEqual(0, poly.GetSignificantCoeffCount());
            Assert.IsTrue(poly.IsZero);
            Assert.IsFalse(poly.IsAlias);
            Assert.AreEqual("0", poly.ToString());
            poly.SetZero();

            var poly2 = new BigPoly();
            Assert.IsTrue(poly.Equals(poly2));

            poly.Resize(1, 1);
            Assert.IsTrue(null != poly.Pointer);
            Assert.IsFalse(poly.IsAlias);

            poly.Resize(0, 0);
            Assert.IsTrue(null == poly.Pointer);
            Assert.IsFalse(poly.IsAlias);
        }

        [TestMethod]
        public void BigPoly3Coeff100BitsNET()
        {
            var poly = new BigPoly(3, 100);
            Assert.AreEqual(3, poly.CoeffCount);
            Assert.AreEqual(100, poly.CoeffBitCount);
            Assert.IsTrue(null != poly.Pointer);
            Assert.AreEqual(6, poly.UInt64Count);
            Assert.AreEqual(0, poly.GetSignificantCoeffCount());
            Assert.AreEqual("0", poly.ToString());
            Assert.IsTrue(poly.IsZero);
            Assert.AreEqual(0UL, poly.Pointer[0]);
            Assert.AreEqual(0UL, poly.Pointer[1]);
            Assert.AreEqual(0UL, poly.Pointer[2]);
            Assert.AreEqual(0UL, poly.Pointer[3]);
            Assert.AreEqual(0UL, poly.Pointer[4]);
            Assert.AreEqual(0UL, poly.Pointer[5]);

            poly[0].Set("1234");
            Assert.AreEqual("1234", poly[0].ToString());
            Assert.AreEqual(1, poly.GetSignificantCoeffCount());
            Assert.AreEqual("1234", poly.ToString());
            Assert.IsFalse(poly.IsZero);
            Assert.AreEqual(0x1234UL, poly.Pointer[0]);
            Assert.AreEqual(0UL, poly.Pointer[1]);
            Assert.AreEqual(0UL, poly.Pointer[2]);
            Assert.AreEqual(0UL, poly.Pointer[3]);
            Assert.AreEqual(0UL, poly.Pointer[4]);
            Assert.AreEqual(0UL, poly.Pointer[5]);

            poly[2].Set("3211234567812345678");
            Assert.AreEqual("3211234567812345678", poly[2].ToString());
            Assert.AreEqual(3, poly.GetSignificantCoeffCount());
            Assert.AreEqual("3211234567812345678x^2 + 1234", poly.ToString());
            Assert.IsFalse(poly.IsZero);
            Assert.AreEqual(0x1234UL, poly.Pointer[0]);
            Assert.AreEqual(0UL, poly.Pointer[1]);
            Assert.AreEqual(0UL, poly.Pointer[2]);
            Assert.AreEqual(0UL, poly.Pointer[3]);
            Assert.AreEqual(0x1234567812345678UL, poly.Pointer[4]);
            Assert.AreEqual(0x321UL, poly.Pointer[5]);

            poly.SetZero();
            Assert.IsTrue(poly.IsZero);
            Assert.AreEqual("0", poly.ToString());
            Assert.AreEqual(0UL, poly.Pointer[0]);
            Assert.AreEqual(0UL, poly.Pointer[1]);
            Assert.AreEqual(0UL, poly.Pointer[2]);
            Assert.AreEqual(0UL, poly.Pointer[3]);
            Assert.AreEqual(0UL, poly.Pointer[4]);
            Assert.AreEqual(0UL, poly.Pointer[5]);

            poly[0].Set("123");
            poly[1].Set("456");
            poly[2].Set("3211234567812345678");
            Assert.AreEqual("3211234567812345678x^2 + 456x^1 + 123", poly.ToString());
            Assert.AreEqual(3, poly.GetSignificantCoeffCount());
            Assert.AreEqual(0x123UL, poly.Pointer[0]);
            Assert.AreEqual(0UL, poly.Pointer[1]);
            Assert.AreEqual(0x456UL, poly.Pointer[2]);
            Assert.AreEqual(0UL, poly.Pointer[3]);
            Assert.AreEqual(0x1234567812345678UL, poly.Pointer[4]);
            Assert.AreEqual(0x321UL, poly.Pointer[5]);

            var poly2 = new BigPoly(3, 64);
            poly2[0].Set(1);
            poly2[1].Set(2);
            poly2[2].Set(3);
            Assert.IsFalse(poly.Equals(poly2));
            Assert.IsFalse(poly2.Equals(poly));
            Assert.AreNotEqual(poly.GetHashCode(), poly2.GetHashCode());
            poly.Set(poly2);
            Assert.IsTrue(poly.Equals(poly2));
            Assert.IsTrue(poly2.Equals(poly));
            Assert.AreEqual(poly.GetHashCode(), poly2.GetHashCode());
            Assert.AreEqual("3x^2 + 2x^1 + 1", poly.ToString());
            Assert.AreEqual(1UL, poly.Pointer[0]);
            Assert.AreEqual(0UL, poly.Pointer[1]);
            Assert.AreEqual(2UL, poly.Pointer[2]);
            Assert.AreEqual(0UL, poly.Pointer[3]);
            Assert.AreEqual(3UL, poly.Pointer[4]);
            Assert.AreEqual(0UL, poly.Pointer[5]);

            poly[0].Set("123");
            poly[1].Set("456");
            poly[2].Set("3211234567812345678");
            poly.Resize(3, 32);
            Assert.AreEqual(3, poly.CoeffCount);
            Assert.AreEqual(32, poly.CoeffBitCount);
            Assert.AreEqual(3, poly.UInt64Count);
            Assert.AreEqual("12345678x^2 + 456x^1 + 123", poly.ToString());
            Assert.AreEqual(0x123UL, poly.Pointer[0]);
            Assert.AreEqual(0x456UL, poly.Pointer[1]);
            Assert.AreEqual(0x12345678UL, poly.Pointer[2]);

            poly.Resize(4, 100);
            Assert.AreEqual(4, poly.CoeffCount);
            Assert.AreEqual(100, poly.CoeffBitCount);
            Assert.AreEqual("12345678x^2 + 456x^1 + 123", poly.ToString());
            Assert.AreEqual(0x123UL, poly.Pointer[0]);
            Assert.AreEqual(0UL, poly.Pointer[1]);
            Assert.AreEqual(0x456UL, poly.Pointer[2]);
            Assert.AreEqual(0UL, poly.Pointer[3]);
            Assert.AreEqual(0x12345678UL, poly.Pointer[4]);
            Assert.AreEqual(0UL, poly.Pointer[5]);
            Assert.AreEqual(0UL, poly.Pointer[6]);
            Assert.AreEqual(0UL, poly.Pointer[7]);

            poly.Resize(0, 0);
            Assert.AreEqual(0, poly.CoeffCount);
            Assert.AreEqual(0, poly.CoeffBitCount);
            Assert.AreEqual(0, poly.UInt64Count);
            Assert.IsTrue(null == poly.Pointer);
            Assert.AreEqual(0, poly.GetSignificantCoeffCount());
            Assert.IsTrue(poly.IsZero);
            Assert.IsFalse(poly.IsAlias);
            Assert.AreEqual("0", poly.ToString());
        }

        [TestMethod]
        public void BigPolyFromStringNET()
        {
            var poly = new BigPoly("9x^2 + 2");
            Assert.AreEqual("9x^2 + 2", poly.ToString());
            Assert.AreEqual(3, poly.CoeffCount);
            Assert.AreEqual(4, poly.CoeffBitCount);

            poly.Resize(0, 0);
            poly.Set("0");
            Assert.AreEqual("0", poly.ToString());
            Assert.AreEqual(0, poly.CoeffCount);
            Assert.AreEqual(0, poly.CoeffBitCount);

            poly.Set("1");
            Assert.AreEqual("1", poly.ToString());
            Assert.AreEqual(1, poly.CoeffCount);
            Assert.AreEqual(1, poly.CoeffBitCount);

            poly.Set("123");
            Assert.AreEqual("123", poly.ToString());
            Assert.AreEqual(1, poly.CoeffCount);
            Assert.AreEqual(9, poly.CoeffBitCount);

            poly.Set("23x^3");
            Assert.AreEqual("23x^3", poly.ToString());
            Assert.AreEqual(4, poly.CoeffCount);
            Assert.AreEqual(9, poly.CoeffBitCount);

            poly.Resize(0, 0);
            poly.Set("23x^3");
            Assert.AreEqual("23x^3", poly.ToString());
            Assert.AreEqual(4, poly.CoeffCount);
            Assert.AreEqual(6, poly.CoeffBitCount);

            poly.Set("3x^2 + 2x^1");
            Assert.AreEqual("3x^2 + 2x^1", poly.ToString());
            Assert.AreEqual(4, poly.CoeffCount);
            Assert.AreEqual(6, poly.CoeffBitCount);

            poly.Set("1x^2 + 2x^1 + 3");
            Assert.AreEqual("1x^2 + 2x^1 + 3", poly.ToString());
            Assert.AreEqual(4, poly.CoeffCount);
            Assert.AreEqual(6, poly.CoeffBitCount);

            poly.Set("4x^1");
            Assert.AreEqual("4x^1", poly.ToString());
            Assert.AreEqual(4, poly.CoeffCount);
            Assert.AreEqual(6, poly.CoeffBitCount);
        }

        [TestMethod]
        public void SaveLoadPolyNET()
        {
            var stream = new MemoryStream();

            var poly = new BigPoly();
            var poly2 = new BigPoly(1, 32);
            poly2[0].Set(1);
            stream.Seek(0, SeekOrigin.Begin);
            poly.Save(stream);
            stream.Seek(0, SeekOrigin.Begin);
            poly2.Load(stream);
            Assert.AreEqual(poly, poly2);

            poly.Resize(3, 96);
            poly[0].Set(2);
            poly[1].Set(3);
            poly[2].Set(4);
            stream.Seek(0, SeekOrigin.Begin);
            poly.Save(stream);
            stream.Seek(0, SeekOrigin.Begin);
            poly2.Load(stream);
            Assert.AreEqual(poly, poly2);

            poly[0].Set("FFFFFFFFFFFFFFFFFF");
            stream.Seek(0, SeekOrigin.Begin);
            poly.Save(stream);
            stream.Seek(0, SeekOrigin.Begin);
            poly2.Load(stream);
            Assert.AreEqual(poly, poly2);

            poly.Resize(2, 32);
            poly[0].Set(1);
            stream.Seek(0, SeekOrigin.Begin);
            poly.Save(stream);
            stream.Seek(0, SeekOrigin.Begin);
            poly2.Load(stream);
            Assert.AreEqual(poly, poly2);
        }

        [TestMethod]
        public void DuplicateToNET()
        {
            var original = new BigPoly(123, 456);
            original[0].Set(1);
            original[1].Set(2);
            original[2].Set(3);
            original[3].Set(4);
            original[4].Set(5);
            original[122].Set(123);

            BigPoly target = new BigPoly();

            original.DuplicateTo(target);
            Assert.AreEqual(target.CoeffCount, original.CoeffCount);
            Assert.AreEqual(target.CoeffBitCount, original.CoeffBitCount);
            Assert.IsTrue(target.Equals(original));
        }

        [TestMethod]
        public void DuplicateFromNET()
        {
            var original = new BigPoly(123, 456);
            original[0].Set(1);
            original[1].Set(2);
            original[2].Set(3);
            original[3].Set(4);
            original[4].Set(5);
            original[122].Set(123);

            BigPoly target = new BigPoly();

            target.DuplicateFrom(original);
            Assert.AreEqual(target.CoeffCount, original.CoeffCount);
            Assert.AreEqual(target.CoeffBitCount, original.CoeffBitCount);
            Assert.IsTrue(target.Equals(original));
        }
    }
}
