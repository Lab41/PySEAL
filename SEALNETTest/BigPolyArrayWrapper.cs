using System.IO;
using Microsoft.Research.SEAL;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace SEALNETTest
{
    [TestClass]
    public class BigPolyArrayWrapper
    {
        [TestMethod]
        public void BigPolyArrayTestNET()
        {
            var arr = new BigPolyArray();

            Assert.AreEqual(arr.Size, 0);
            Assert.AreEqual(arr.CoeffCount, 0);
            Assert.AreEqual(arr.CoeffBitCount, 0);
            Assert.AreEqual(arr.CoeffUInt64Count(), 0);

            arr.Reset();
            Assert.AreEqual(arr.Size, 0);
            Assert.AreEqual(arr.CoeffCount, 0);
            Assert.AreEqual(arr.CoeffBitCount, 0);
            Assert.AreEqual(arr.CoeffUInt64Count(), 0);

            arr.Resize(2, 5, 10);
            Assert.AreEqual(arr.Size, 2);
            Assert.AreEqual(arr.CoeffCount, 5);
            Assert.AreEqual(arr.CoeffBitCount, 10);
            Assert.AreEqual(arr.CoeffUInt64Count(), 1);

            arr.Resize(3, 13, 70);
            Assert.AreEqual(arr.Size, 3);
            Assert.AreEqual(arr.CoeffCount, 13);
            Assert.AreEqual(arr.CoeffBitCount, 70);
            Assert.AreEqual(arr.CoeffUInt64Count(), 2);

            arr[0][0].Set(1);
            arr[0][1].Set(2);
            arr[0][2].Set(3);
            arr[0][3].Set(4);
            arr[0][4].Set(5);
            arr[1][0].Set(6);
            arr[1][1].Set(7);
            arr[1][2].Set(8);
            arr[2][0].Set(9);
            arr[2][1].Set(10);

            Assert.IsTrue("5x^4 + 4x^3 + 3x^2 + 2x^1 + 1" == arr[0].ToString());
            Assert.IsTrue("8x^2 + 7x^1 + 6" == arr[1].ToString());
            Assert.IsTrue("Ax^1 + 9" == arr[2].ToString());

            var arr2 = new BigPolyArray();
            Assert.AreEqual(arr2.Size, 0);
            Assert.AreEqual(arr2.CoeffCount, 0);
            Assert.AreEqual(arr2.CoeffBitCount, 0);
            Assert.AreEqual(arr2.CoeffUInt64Count(), 0);

            arr2.Set(arr);
            Assert.AreEqual(arr.Size, 3);
            Assert.AreEqual(arr.CoeffCount, 13);
            Assert.AreEqual(arr.CoeffBitCount, 70);

            Assert.IsTrue("5x^4 + 4x^3 + 3x^2 + 2x^1 + 1" == arr2[0].ToString());
            Assert.IsTrue("8x^2 + 7x^1 + 6" == arr2[1].ToString());
            Assert.IsTrue("Ax^1 + 9" == arr2[2].ToString());

            arr2[1].SetZero();
            Assert.IsTrue("5x^4 + 4x^3 + 3x^2 + 2x^1 + 1" == arr2[0].ToString());
            Assert.IsTrue("Ax^1 + 9" == arr2[2].ToString());

            arr.Resize(2, 3, 10);
            Assert.AreEqual(arr.Size, 2);
            Assert.AreEqual(arr.CoeffCount, 3);
            Assert.AreEqual(arr.CoeffBitCount, 10);

            Assert.IsTrue("3x^2 + 2x^1 + 1" == arr[0].ToString());
            Assert.IsTrue("8x^2 + 7x^1 + 6" == arr[1].ToString());

            arr.Resize(1, 1, 10);
            Assert.AreEqual(arr.Size, 1);
            Assert.AreEqual(arr.CoeffCount, 1);
            Assert.AreEqual(arr.CoeffBitCount, 10);

            Assert.IsTrue("1" == arr[0].ToString());

            arr.Reset();
            Assert.AreEqual(arr.Size, 0);
            Assert.AreEqual(arr.CoeffCount, 0);
            Assert.AreEqual(arr.CoeffBitCount, 0);
        }

        [TestMethod]
        public void SaveLoadBigPolyArrayNET()
        {
            var stream = new MemoryStream();

            var arr = new BigPolyArray(3, 5, 10);

            arr[0][0].Set(1);
            arr[0][1].Set(2);
            arr[0][2].Set(3);
            arr[1][0].Set(4);
            arr[1][1].Set(5);
            arr[2][0].Set(6);

            var arr2 = new BigPolyArray();
            stream.Seek(0, SeekOrigin.Begin);
            arr.Save(stream);
            stream.Seek(0, SeekOrigin.Begin);
            arr2.Load(stream);

            Assert.AreEqual(3, arr2.Size);
            Assert.AreEqual(arr2.Size, 3);
            Assert.AreEqual(arr2.CoeffCount, 5);
            Assert.AreEqual(arr2.CoeffBitCount, 10);

            Assert.IsTrue(arr[0].Equals(arr2[0]));
            Assert.IsTrue(arr[1].Equals(arr2[1]));
            Assert.IsTrue(arr[2].Equals(arr2[2]));
        }
    }
}
