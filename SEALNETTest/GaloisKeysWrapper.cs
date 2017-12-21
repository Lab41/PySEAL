using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.Research.SEAL;
using System.IO;
using System.Collections.Generic;

namespace SEALNETTest
{
    [TestClass]
    public class GaloisKeysWrapper
    {
        [TestMethod]
        public void GaloisKeysSaveLoadNET()
        {
            var stream = new MemoryStream();
            {
                var parms = new EncryptionParameters();
                parms.NoiseStandardDeviation = 3.19;
                parms.PolyModulus = "1x^64 + 1";
                parms.PlainModulus = 65537;
                parms.CoeffModulus = new List<SmallModulus> { DefaultParams.SmallMods60Bit(0) };
                var context = new SEALContext(parms);
                var keygen = new KeyGenerator(context);

                var keys = new GaloisKeys();
                Assert.AreEqual(keys.DecompositionBitCount, 0);
                var test_keys = new GaloisKeys();
                keys.Save(stream);
                stream.Seek(0, SeekOrigin.Begin);
                test_keys.Load(stream);
                Assert.AreEqual(keys.Size, test_keys.Size);
                Assert.AreEqual(keys.HashBlock, test_keys.HashBlock);
                Assert.AreEqual(keys.DecompositionBitCount, test_keys.DecompositionBitCount);
                Assert.AreEqual(0, keys.Size);

                keygen.GenerateGaloisKeys(1, keys);
                Assert.AreEqual(keys.DecompositionBitCount, 1);
                stream.Seek(0, SeekOrigin.Begin);
                keys.Save(stream);
                stream.Seek(0, SeekOrigin.Begin);
                test_keys.Load(stream);
                Assert.AreEqual(keys.Size, test_keys.Size);
                Assert.AreEqual(keys.HashBlock, test_keys.HashBlock);
                Assert.AreEqual(keys.DecompositionBitCount, test_keys.DecompositionBitCount);
                for (int j = 0; j < test_keys.Size; j++)
                {
                    for (int i = 0; i < test_keys.Data[j].Count; i++)
                    {
                        Assert.AreEqual(keys.Data[j][i].Size, test_keys.Data[j][i].Size);
                        Assert.AreEqual(keys.Data[j][i].UInt64Count, test_keys.Data[j][i].UInt64Count);
                    }
                }
                Assert.AreEqual(10, keys.Size);

                keygen.GenerateGaloisKeys(8, keys);
                Assert.AreEqual(keys.DecompositionBitCount, 8);
                stream.Seek(0, SeekOrigin.Begin);
                keys.Save(stream);
                stream.Seek(0, SeekOrigin.Begin);
                test_keys.Load(stream);
                Assert.AreEqual(keys.Size, test_keys.Size);
                Assert.AreEqual(keys.HashBlock, test_keys.HashBlock);
                Assert.AreEqual(keys.DecompositionBitCount, test_keys.DecompositionBitCount);
                for (int j = 0; j < test_keys.Size; j++)
                {
                    for (int i = 0; i < test_keys.Data[j].Count; i++)
                    {
                        Assert.AreEqual(keys.Data[j][i].Size, test_keys.Data[j][i].Size);
                        Assert.AreEqual(keys.Data[j][i].UInt64Count, test_keys.Data[j][i].UInt64Count);
                    }
                }
                Assert.AreEqual(10, keys.Size);

                keygen.GenerateGaloisKeys(60, keys);
                Assert.AreEqual(keys.DecompositionBitCount, 60);
                stream.Seek(0, SeekOrigin.Begin);
                keys.Save(stream);
                stream.Seek(0, SeekOrigin.Begin);
                test_keys.Load(stream);
                Assert.AreEqual(keys.Size, test_keys.Size);
                Assert.AreEqual(keys.HashBlock, test_keys.HashBlock);
                Assert.AreEqual(keys.DecompositionBitCount, test_keys.DecompositionBitCount);
                for (int j = 0; j < test_keys.Size; j++)
                {
                    for (int i = 0; i < test_keys.Data[j].Count; i++)
                    {
                        Assert.AreEqual(keys.Data[j][i].Size, test_keys.Data[j][i].Size);
                        Assert.AreEqual(keys.Data[j][i].UInt64Count, test_keys.Data[j][i].UInt64Count);
                    }
                }
                Assert.AreEqual(10, keys.Size);
            }
            {
                var parms = new EncryptionParameters();
                parms.NoiseStandardDeviation = 3.19;
                parms.PolyModulus = "1x^256 + 1";
                parms.PlainModulus = 65537;
                parms.CoeffModulus = new List<SmallModulus> {
                    DefaultParams.SmallMods60Bit(0), DefaultParams.SmallMods50Bit(0) };
                var context = new SEALContext(parms);
                var keygen = new KeyGenerator(context);

                var keys = new GaloisKeys();
                Assert.AreEqual(keys.DecompositionBitCount, 0);
                var test_keys = new GaloisKeys();
                stream.Seek(0, SeekOrigin.Begin);
                keys.Save(stream);
                stream.Seek(0, SeekOrigin.Begin);
                test_keys.Load(stream);
                Assert.AreEqual(keys.Size, test_keys.Size);
                Assert.AreEqual(keys.HashBlock, test_keys.HashBlock);
                Assert.AreEqual(keys.DecompositionBitCount, test_keys.DecompositionBitCount);
                Assert.AreEqual(0, keys.Size);

                keygen.GenerateGaloisKeys(8, keys);
                Assert.AreEqual(keys.DecompositionBitCount, 8);
                stream.Seek(0, SeekOrigin.Begin);
                keys.Save(stream);
                stream.Seek(0, SeekOrigin.Begin);
                test_keys.Load(stream);
                Assert.AreEqual(keys.Size, test_keys.Size);
                Assert.AreEqual(keys.HashBlock, test_keys.HashBlock);
                Assert.AreEqual(keys.DecompositionBitCount, test_keys.DecompositionBitCount);
                for (int j = 0; j < test_keys.Size; j++)
                {
                    for (int i = 0; i < test_keys.Data[j].Count; i++)
                    {
                        Assert.AreEqual(keys.Data[j][i].Size, test_keys.Data[j][i].Size);
                        Assert.AreEqual(keys.Data[j][i].UInt64Count, test_keys.Data[j][i].UInt64Count);
                    }
                }
                Assert.AreEqual(14, keys.Size);

                keygen.GenerateGaloisKeys(60, keys);
                Assert.AreEqual(keys.DecompositionBitCount, 60);
                stream.Seek(0, SeekOrigin.Begin);
                keys.Save(stream);
                stream.Seek(0, SeekOrigin.Begin);
                test_keys.Load(stream);
                Assert.AreEqual(keys.Size, test_keys.Size);
                Assert.AreEqual(keys.HashBlock, test_keys.HashBlock);
                Assert.AreEqual(keys.DecompositionBitCount, test_keys.DecompositionBitCount);
                for (int j = 0; j < test_keys.Size; j++)
                {
                    for (int i = 0; i < test_keys.Data[j].Count; i++)
                    {
                        Assert.AreEqual(keys.Data[j][i].Size, test_keys.Data[j][i].Size);
                        Assert.AreEqual(keys.Data[j][i].UInt64Count, test_keys.Data[j][i].UInt64Count);
                    }
                }
                Assert.AreEqual(14, keys.Size);
            }
        }
    }
}