using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.IO;
using Microsoft.Research.SEAL;
using System.Collections.Generic;

namespace SEALNETTest
{
    [TestClass]
    public class EvaluationKeysWrapper
    {
        [TestMethod]
        public void EvaluationKeysSaveLoadNET()
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

                var keys = new EvaluationKeys();
                Assert.AreEqual(keys.DecompositionBitCount, 0);
                var test_keys = new EvaluationKeys();
                keys.Save(stream);
                stream.Seek(0, SeekOrigin.Begin);
                test_keys.Load(stream);
                Assert.AreEqual(keys.Size, test_keys.Size);
                Assert.IsTrue(keys.HashBlock.Equals(test_keys.HashBlock));
                Assert.AreEqual(keys.DecompositionBitCount, test_keys.DecompositionBitCount);
                Assert.AreEqual(0, keys.Size);

                keygen.GenerateEvaluationKeys(1, 1, keys);
                Assert.AreEqual(keys.DecompositionBitCount, 1);
                stream.Seek(0, SeekOrigin.Begin);
                keys.Save(stream);
                stream.Seek(0, SeekOrigin.Begin);
                test_keys.Load(stream);
                Assert.AreEqual(keys.Size, test_keys.Size);
                Assert.IsTrue(keys.HashBlock.Equals(test_keys.HashBlock));
                Assert.AreEqual(keys.DecompositionBitCount, test_keys.DecompositionBitCount);
                for (int j = 0; j < test_keys.Size; j++)
                {
                    for (int i = 0; i < test_keys.Key(j + 2).Count; i++)
                    {
                        Assert.AreEqual(keys.Key(j + 2)[i].Size, test_keys.Key(j + 2)[i].Size);
                        Assert.AreEqual(keys.Key(j + 2)[i].UInt64Count, test_keys.Key(j + 2)[i].UInt64Count);
                    }
                }

                keygen.GenerateEvaluationKeys(2, 1, keys);
                Assert.AreEqual(keys.DecompositionBitCount, 2);
                stream.Seek(0, SeekOrigin.Begin);
                keys.Save(stream);
                stream.Seek(0, SeekOrigin.Begin);
                test_keys.Load(stream);
                Assert.AreEqual(keys.Size, test_keys.Size);
                Assert.IsTrue(keys.HashBlock.Equals(test_keys.HashBlock));
                Assert.AreEqual(keys.DecompositionBitCount, test_keys.DecompositionBitCount);
                for (int j = 0; j < test_keys.Size; j++)
                {
                    for (int i = 0; i < test_keys.Key(j + 2).Count; i++)
                    {
                        Assert.AreEqual(keys.Key(j + 2)[i].Size, test_keys.Key(j + 2)[i].Size);
                        Assert.AreEqual(keys.Key(j + 2)[i].UInt64Count, test_keys.Key(j + 2)[i].UInt64Count);
                    }
                }

                keygen.GenerateEvaluationKeys(59, 2, keys);
                Assert.AreEqual(keys.DecompositionBitCount, 59);
                stream.Seek(0, SeekOrigin.Begin);
                keys.Save(stream);
                stream.Seek(0, SeekOrigin.Begin);
                test_keys.Load(stream);
                Assert.AreEqual(keys.Size, test_keys.Size);
                Assert.IsTrue(keys.HashBlock.Equals(test_keys.HashBlock));
                Assert.AreEqual(keys.DecompositionBitCount, test_keys.DecompositionBitCount);
                for (int j = 0; j < test_keys.Size; j++)
                {
                    for (int i = 0; i < test_keys.Key(j + 2).Count; i++)
                    {
                        Assert.AreEqual(keys.Key(j + 2)[i].Size, test_keys.Key(j + 2)[i].Size);
                        Assert.AreEqual(keys.Key(j + 2)[i].UInt64Count, test_keys.Key(j + 2)[i].UInt64Count);
                    }
                }

                keygen.GenerateEvaluationKeys(60, 5, keys);
                Assert.AreEqual(keys.DecompositionBitCount, 60);
                stream.Seek(0, SeekOrigin.Begin);
                keys.Save(stream);
                stream.Seek(0, SeekOrigin.Begin);
                test_keys.Load(stream);
                Assert.AreEqual(keys.Size, test_keys.Size);
                Assert.IsTrue(keys.HashBlock.Equals(test_keys.HashBlock));
                Assert.AreEqual(keys.DecompositionBitCount, test_keys.DecompositionBitCount);
                for (int j = 0; j < test_keys.Size; j++)
                {
                    for (int i = 0; i < test_keys.Key(j + 2).Count; i++)
                    {
                        Assert.AreEqual(keys.Key(j + 2)[i].Size, test_keys.Key(j + 2)[i].Size);
                        Assert.AreEqual(keys.Key(j + 2)[i].UInt64Count, test_keys.Key(j + 2)[i].UInt64Count);
                    }
                }
            }
            {
                var parms = new EncryptionParameters();
                parms.NoiseStandardDeviation = 3.19;
                parms.PolyModulus = "1x^256 + 1";
                parms.PlainModulus = 1 << 6;
                parms.CoeffModulus = new List<SmallModulus> {
                    DefaultParams.SmallMods60Bit(0), DefaultParams.SmallMods50Bit(0) };
                var context = new SEALContext(parms);
                var keygen = new KeyGenerator(context);

                var keys = new EvaluationKeys();
                Assert.AreEqual(keys.DecompositionBitCount, 0);
                var test_keys = new EvaluationKeys();
                stream.Seek(0, SeekOrigin.Begin);
                keys.Save(stream);
                stream.Seek(0, SeekOrigin.Begin);
                test_keys.Load(stream);
                Assert.AreEqual(keys.Size, test_keys.Size);
                Assert.IsTrue(keys.HashBlock.Equals(test_keys.HashBlock));
                Assert.AreEqual(keys.DecompositionBitCount, test_keys.DecompositionBitCount);
                Assert.AreEqual(0, keys.Size);

                keygen.GenerateEvaluationKeys(8, 1, keys);
                Assert.AreEqual(keys.DecompositionBitCount, 8);
                stream.Seek(0, SeekOrigin.Begin);
                keys.Save(stream);
                stream.Seek(0, SeekOrigin.Begin);
                test_keys.Load(stream);
                Assert.AreEqual(keys.Size, test_keys.Size);
                Assert.IsTrue(keys.HashBlock.Equals(test_keys.HashBlock));
                Assert.AreEqual(keys.DecompositionBitCount, test_keys.DecompositionBitCount);
                for (int j = 0; j < test_keys.Size; j++)
                {
                    for (int i = 0; i < test_keys.Key(j + 2).Count; i++)
                    {
                        Assert.AreEqual(keys.Key(j + 2)[i].Size, test_keys.Key(j + 2)[i].Size);
                        Assert.AreEqual(keys.Key(j + 2)[i].UInt64Count, test_keys.Key(j + 2)[i].UInt64Count);
                    }
                }

                keygen.GenerateEvaluationKeys(8, 2, keys);
                Assert.AreEqual(keys.DecompositionBitCount, 8);
                stream.Seek(0, SeekOrigin.Begin);
                keys.Save(stream);
                stream.Seek(0, SeekOrigin.Begin);
                test_keys.Load(stream);
                Assert.AreEqual(keys.Size, test_keys.Size);
                Assert.IsTrue(keys.HashBlock.Equals(test_keys.HashBlock));
                Assert.AreEqual(keys.DecompositionBitCount, test_keys.DecompositionBitCount);
                for (int j = 0; j < test_keys.Size; j++)
                {
                    for (int i = 0; i < test_keys.Key(j + 2).Count; i++)
                    {
                        Assert.AreEqual(keys.Key(j + 2)[i].Size, test_keys.Key(j + 2)[i].Size);
                        Assert.AreEqual(keys.Key(j + 2)[i].UInt64Count, test_keys.Key(j + 2)[i].UInt64Count);
                    }
                }

                keygen.GenerateEvaluationKeys(59, 2, keys);
                Assert.AreEqual(keys.DecompositionBitCount, 59);
                stream.Seek(0, SeekOrigin.Begin);
                keys.Save(stream);
                stream.Seek(0, SeekOrigin.Begin);
                test_keys.Load(stream);
                Assert.AreEqual(keys.Size, test_keys.Size);
                Assert.IsTrue(keys.HashBlock.Equals(test_keys.HashBlock));
                Assert.AreEqual(keys.DecompositionBitCount, test_keys.DecompositionBitCount);
                for (int j = 0; j < test_keys.Size; j++)
                {
                    for (int i = 0; i < test_keys.Key(j + 2).Count; i++)
                    {
                        Assert.AreEqual(keys.Key(j + 2)[i].Size, test_keys.Key(j + 2)[i].Size);
                        Assert.AreEqual(keys.Key(j + 2)[i].UInt64Count, test_keys.Key(j + 2)[i].UInt64Count);
                    }
                }

                keygen.GenerateEvaluationKeys(60, 5, keys);
                Assert.AreEqual(keys.DecompositionBitCount, 60);
                stream.Seek(0, SeekOrigin.Begin);
                keys.Save(stream);
                stream.Seek(0, SeekOrigin.Begin);
                test_keys.Load(stream);
                Assert.AreEqual(keys.Size, test_keys.Size);
                Assert.IsTrue(keys.HashBlock.Equals(test_keys.HashBlock));
                Assert.AreEqual(keys.DecompositionBitCount, test_keys.DecompositionBitCount);
                for (int j = 0; j < test_keys.Size; j++)
                {
                    for (int i = 0; i < test_keys.Key(j + 2).Count; i++)
                    {
                        Assert.AreEqual(keys.Key(j + 2)[i].Size, test_keys.Key(j + 2)[i].Size);
                        Assert.AreEqual(keys.Key(j + 2)[i].UInt64Count, test_keys.Key(j + 2)[i].UInt64Count);
                    }
                }
            }
        }
    }
}