using Microsoft.Research.SEAL;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.IO;
using System;
using System.Collections.Generic;
using std;

namespace SEALNETTest
{
    [TestClass]
    public class EvaluationKeysWrapper
    {
        [TestMethod]
        public void EvaluationKeysNETSaveLoad()
        {
            var stream = new MemoryStream();

            var arr1 = new BigPolyArray(3, 5, 10);
            arr1[0][0].Set(1);
            arr1[0][1].Set(2);
            arr1[0][2].Set(3);
            arr1[1][0].Set(4);
            arr1[1][1].Set(5);
            arr1[2][0].Set(6);

            var arr2 = new BigPolyArray(3, 5, 10);
            arr2[0][0].Set(7);
            arr2[0][1].Set(8);
            arr2[0][2].Set(9);
            arr2[1][0].Set(10);
            arr2[1][1].Set(11);
            arr2[2][0].Set(12);

            var pair1 = new Tuple<BigPolyArray, BigPolyArray>(arr1, arr2);
            var list1 = new List<System.Tuple<Microsoft.Research.SEAL.BigPolyArray, Microsoft.Research.SEAL.BigPolyArray>>();
            list1.Add(pair1);
            var keys1 = new EvaluationKeys(list1);

            stream.Seek(0, SeekOrigin.Begin);
            keys1.Save(stream);

            var keys2 = new EvaluationKeys();
            stream.Seek(0, SeekOrigin.Begin);
            keys2.Load(stream);

            Assert.AreEqual(keys1[0].Item1[0].ToString(), keys2[0].Item1[0].ToString());
        }
    }
}