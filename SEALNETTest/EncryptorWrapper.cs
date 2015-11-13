using Microsoft.Research.SEAL;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace SEALNETTest
{
    [TestClass]
    public class EncryptorWrapper
    {
        [TestMethod]
        public void EncryptAddsNoiseNET()
        {
            var parms = new EncryptionParameters
            {
                DecompositionBitCount = 4,
                NoiseStandardDeviation = 3.19,
                NoiseMaxDeviation = 35.06
            };
            var coeffModulus = parms.CoeffModulus;
            coeffModulus.Resize(48);
            coeffModulus.Set("FFFFFFFFC001");
            var plainModulus = parms.PlainModulus;
            plainModulus.Resize(7);
            plainModulus.Set(1 << 6);
            var polyModulus = parms.PolyModulus;
            polyModulus.Resize(64, 1);
            polyModulus[0].Set(1);
            polyModulus[63].Set(1);

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            var encrypted1 = encryptor.Encrypt(Encoder.Encode(0x12345678));
            var encrypted2 = encryptor.Encrypt(Encoder.Encode(0x12345678));
            Assert.AreNotEqual(encrypted1, encrypted2);

            var decryptor = new Decryptor(parms, keygen.SecretKey);
            Assert.AreEqual(0x12345678U, Encoder.DecodeUInt32(decryptor.Decrypt(encrypted1)));
            Assert.AreEqual(0x12345678U, Encoder.DecodeUInt32(decryptor.Decrypt(encrypted2)));
        }

        [TestMethod]
        public void EncryptDecryptNET()
        {
            var parms = new EncryptionParameters
            {
                DecompositionBitCount = 4,
                NoiseStandardDeviation = 3.19,
                NoiseMaxDeviation = 35.06
            };
            var coeffModulus = parms.CoeffModulus;
            coeffModulus.Resize(48);
            coeffModulus.Set("FFFFFFFFC001");
            var plainModulus = parms.PlainModulus;
            plainModulus.Resize(7);
            plainModulus.Set(1 << 6);
            var polyModulus = parms.PolyModulus;
            polyModulus.Resize(64, 1);
            polyModulus[0].Set(1);
            polyModulus[63].Set(1);

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            Assert.AreEqual(encryptor.PublicKey, keygen.PublicKey);
            var decryptor = new Decryptor(parms, keygen.SecretKey);
            Assert.AreEqual(decryptor.SecretKey, keygen.SecretKey);

            var encrypted = encryptor.Encrypt(Encoder.Encode(0x12345678));
            Assert.AreEqual(0x12345678U, Encoder.DecodeUInt32(decryptor.Decrypt(encrypted)));

            encrypted = encryptor.Encrypt(Encoder.Encode(0));
            Assert.AreEqual(0U, Encoder.DecodeUInt32(decryptor.Decrypt(encrypted)));

            encrypted = encryptor.Encrypt(Encoder.Encode(1));
            Assert.AreEqual(1U, Encoder.DecodeUInt32(decryptor.Decrypt(encrypted)));

            encrypted = encryptor.Encrypt(Encoder.Encode(2));
            Assert.AreEqual(2U, Encoder.DecodeUInt32(decryptor.Decrypt(encrypted)));

            encrypted = encryptor.Encrypt(Encoder.Encode(0x7FFFFFFFFFFFFFFDUL));
            Assert.AreEqual(0x7FFFFFFFFFFFFFFDUL, Encoder.DecodeUInt64(decryptor.Decrypt(encrypted)));

            encrypted = encryptor.Encrypt(Encoder.Encode(0x7FFFFFFFFFFFFFFEUL));
            Assert.AreEqual(0x7FFFFFFFFFFFFFFEUL, Encoder.DecodeUInt64(decryptor.Decrypt(encrypted)));

            encrypted = encryptor.Encrypt(Encoder.Encode(0x7FFFFFFFFFFFFFFFUL));
            Assert.AreEqual(0x7FFFFFFFFFFFFFFFUL, Encoder.DecodeUInt64(decryptor.Decrypt(encrypted)));
        }

        [TestMethod]
        public void EncryptDecryptTestModeNET()
        {
            var parms = new EncryptionParameters
            {
                DecompositionBitCount = 4,
                NoiseStandardDeviation = 3.19,
                NoiseMaxDeviation = 35.06,
                Mode = EncryptionMode.Test
            };
            var coeffModulus = parms.CoeffModulus;
            coeffModulus.Resize(48);
            coeffModulus.Set("FFFFFFFFC001");
            var plainModulus = parms.PlainModulus;
            plainModulus.Resize(7);
            plainModulus.Set(1 << 6);
            var polyModulus = parms.PolyModulus;
            polyModulus.Resize(64, 1);
            polyModulus[0].Set(1);
            polyModulus[63].Set(1);

            var Encoder = new BinaryEncoder(parms.PlainModulus);

            var keygen = new KeyGenerator(parms);
            keygen.Generate();

            var encryptor = new Encryptor(parms, keygen.PublicKey);
            Assert.AreEqual(encryptor.PublicKey, keygen.PublicKey);
            var decryptor = new Decryptor(parms, keygen.SecretKey);
            Assert.AreEqual(decryptor.SecretKey, keygen.SecretKey);

            var plain = Encoder.Encode(0x12345678);
            var encrypted = encryptor.Encrypt(plain);
            Assert.AreEqual(plain, encrypted);
            Assert.AreEqual(0x12345678U, Encoder.DecodeUInt32(decryptor.Decrypt(encrypted)));

            plain = Encoder.Encode(0);
            encrypted = encryptor.Encrypt(plain);
            Assert.AreEqual(plain, encrypted);
            Assert.AreEqual(0U, Encoder.DecodeUInt32(decryptor.Decrypt(encrypted)));

            plain = Encoder.Encode(1);
            encrypted = encryptor.Encrypt(plain);
            Assert.AreEqual(plain, encrypted);
            Assert.AreEqual(1U, Encoder.DecodeUInt32(decryptor.Decrypt(encrypted)));

            plain = Encoder.Encode(2);
            encrypted = encryptor.Encrypt(plain);
            Assert.AreEqual(plain, encrypted);
            Assert.AreEqual(2U, Encoder.DecodeUInt32(decryptor.Decrypt(encrypted)));

            plain = Encoder.Encode(0x7FFFFFFFFFFFFFFDUL);
            encrypted = encryptor.Encrypt(plain);
            Assert.AreEqual(plain, encrypted);
            Assert.AreEqual(0x7FFFFFFFFFFFFFFDUL, Encoder.DecodeUInt64(decryptor.Decrypt(encrypted)));

            plain = Encoder.Encode(0x7FFFFFFFFFFFFFFEUL);
            encrypted = encryptor.Encrypt(plain);
            Assert.AreEqual(plain, encrypted);
            Assert.AreEqual(0x7FFFFFFFFFFFFFFEUL, Encoder.DecodeUInt64(decryptor.Decrypt(encrypted)));

            plain = Encoder.Encode(0x7FFFFFFFFFFFFFFFUL);
            encrypted = encryptor.Encrypt(plain);
            Assert.AreEqual(plain, encrypted);
            Assert.AreEqual(0x7FFFFFFFFFFFFFFFUL, Encoder.DecodeUInt64(decryptor.Decrypt(encrypted)));
        }
    }
}