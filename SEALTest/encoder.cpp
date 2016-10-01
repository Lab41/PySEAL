#include "CppUnitTest.h"
#include "encoder.h"
#include <cstdint>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(Encoder)
    {
    public:
        TEST_METHOD(BinaryEncodeDecodeBigUInt)
        {
            BigUInt modulus("FFFFFFFFFFFFFFFF");
            BinaryEncoder encoder(modulus);

            BigUInt value(64);
            value = "0";
            BigPoly poly = encoder.encode(value);
            Assert::AreEqual(0, poly.coeff_count());
            Assert::IsTrue(poly.is_zero());
            Assert::IsTrue(value == encoder.decode_biguint(poly));

            value = "1";
            BigPoly poly1 = encoder.encode(value);
            Assert::AreEqual(1, poly1.coeff_count());
            Assert::AreEqual(1, poly1.coeff_bit_count());
            Assert::IsTrue("1" == poly1.to_string());
            Assert::IsTrue(value == encoder.decode_biguint(poly1));

            value = "2";
            BigPoly poly2 = encoder.encode(value);
            Assert::AreEqual(2, poly2.coeff_count());
            Assert::AreEqual(1, poly2.coeff_bit_count());
            Assert::IsTrue("1x^1" == poly2.to_string());
            Assert::IsTrue(value == encoder.decode_biguint(poly2));

            value = "3";
            BigPoly poly3 = encoder.encode(value);
            Assert::AreEqual(2, poly3.coeff_count());
            Assert::AreEqual(1, poly3.coeff_bit_count());
            Assert::IsTrue("1x^1 + 1" == poly3.to_string());
            Assert::IsTrue(value == encoder.decode_biguint(poly3));

            value = "FFFFFFFFFFFFFFFF";
            BigPoly poly4 = encoder.encode(value);
            Assert::AreEqual(64, poly4.coeff_count());
            Assert::AreEqual(1, poly4.coeff_bit_count());
            for (int i = 0; i < 64; ++i)
            {
                Assert::IsTrue("1" == poly4[i].to_string());
            }
            Assert::IsTrue(value == encoder.decode_biguint(poly4));

            value = "80F02";
            BigPoly poly5 = encoder.encode(value);
            Assert::AreEqual(20, poly5.coeff_count());
            Assert::AreEqual(1, poly5.coeff_bit_count());
            for (int i = 0; i < 20; ++i)
            {
                if (i == 19 || (i >= 8 && i <= 11) || i == 1)
                {
                    Assert::IsTrue("1" == poly5[i].to_string());
                }
                else
                {
                    Assert::IsTrue(poly5[i].is_zero());
                }
            }
            Assert::IsTrue(value == encoder.decode_biguint(poly5));

            BigPoly poly6(3, 10);
            poly6[0] = 1;
            poly6[1] = 500;
            poly6[2] = 1023;
            value = 1 + 500 * 2 + 1023 * 4;
            Assert::IsTrue(value == encoder.decode_biguint(poly6));

            modulus = 1024;
            BinaryEncoder encoder2(modulus);
            BigPoly poly7(4, 10);
            poly7[0] = 1023; // -1   (*1)
            poly7[1] = 512;  // -512 (*2)
            poly7[2] = 511;  // 511  (*4)
            poly7[3] = 1;    // 1    (*8)
            value = -1 + -512 * 2 + 511 * 4 + 1 * 8;
            Assert::IsTrue(value == encoder2.decode_biguint(poly7));
        }

        TEST_METHOD(BalancedEncodeDecodeBigUInt)
        {
            BigUInt modulus("10000");
            BalancedEncoder encoder(modulus);

            BigUInt value(64);
            value = "0";
            BigPoly poly = encoder.encode(value);
            Assert::AreEqual(0, poly.coeff_count());
            Assert::IsTrue(poly.is_zero());
            Assert::IsTrue(value == encoder.decode_biguint(poly));

            value = "1";
            BigPoly poly1 = encoder.encode(value);
            Assert::AreEqual(1, poly1.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly1.coeff_bit_count());
            Assert::IsTrue("1" == poly1.to_string());
            Assert::IsTrue(value == encoder.decode_biguint(poly1));

            value = "2";
            BigPoly poly2 = encoder.encode(value);
            Assert::AreEqual(2, poly2.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly2.coeff_bit_count());
            Assert::IsTrue("1x^1 + FFFF" == poly2.to_string());
            Assert::IsTrue(value == encoder.decode_biguint(poly2));

            value = "3";
            BigPoly poly3 = encoder.encode(value);
            Assert::AreEqual(2, poly3.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly3.coeff_bit_count());
            Assert::IsTrue("1x^1" == poly3.to_string());
            Assert::IsTrue(value == encoder.decode_biguint(poly3));

            value = "2671";
            BigPoly poly4 = encoder.encode(value);
            Assert::AreEqual(9, poly4.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly4.coeff_bit_count());
            for (int i = 0; i < 9; ++i)
            {
                Assert::IsTrue("1" == poly4[i].to_string());
            }
            Assert::IsTrue(value == encoder.decode_biguint(poly4));

            value = "D4EB";
            BigPoly poly5 = encoder.encode(value);
            Assert::AreEqual(11, poly5.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly5.coeff_bit_count());
            for (int i = 0; i < 11; ++i)
            {
                if (i % 3 == 1)
                {
                    Assert::IsTrue("1" == poly5[i].to_string());
                }
                else if (i % 3 == 0)
                {
                    Assert::IsTrue(poly5[i].is_zero());
                }
                else
                {
                    Assert::IsTrue("FFFF" == poly5[i].to_string());
                }
            }
            Assert::IsTrue(value == encoder.decode_biguint(poly5));

            BigPoly poly6(3, 10);
            poly6[0] = 1;
            poly6[1] = 500;
            poly6[2] = 1023;
            value = 1 + 500 * 3 + 1023 * 9;
            Assert::IsTrue(value == encoder.decode_biguint(poly6));

            BalancedEncoder encoder2(modulus, 7);
            BigPoly poly7(4, 16);
            poly7[0] = 123; // 123   (*1)
            poly7[1] = "FFFF";  // -1 (*7)
            poly7[2] = 511;  // 511  (*49)
            poly7[3] = 1;    // 1    (*343)
            value = 123 + -1 * 7 + 511 * 49 + 1 * 343;
            Assert::IsTrue(value == encoder2.decode_biguint(poly7));

            BalancedEncoder encoder3(modulus, 6);
            BigPoly poly8(4, 16);
            poly8[0] = 5;
            poly8[1] = 4;
            poly8[2] = 3;
            poly8[3] = 2;
            value = 5 + 4 * 6 + 3 * 36 + 2 * 216;
            Assert::IsTrue(value == encoder3.decode_biguint(poly8));

            BalancedEncoder encoder4(modulus, 10);
            BigPoly poly9(4, 16);
            poly9[0] = 1;
            poly9[1] = 2;
            poly9[2] = 3;
            poly9[3] = 4;
            value = 4321;
            Assert::IsTrue(value == encoder4.decode_biguint(poly9));

            value = "4D2";
            BigPoly poly10 = encoder2.encode(value);
            Assert::AreEqual(5, poly10.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly10.coeff_bit_count());
            Assert::IsTrue(value == encoder2.decode_biguint(poly10));

            value = "4D2";
            BigPoly poly11 = encoder3.encode(value);
            Assert::AreEqual(5, poly11.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly11.coeff_bit_count());
            Assert::IsTrue(value == encoder3.decode_biguint(poly11));

            value = "4D2";
            BigPoly poly12 = encoder4.encode(value);
            Assert::AreEqual(4, poly12.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly12.coeff_bit_count());
            Assert::IsTrue(value == encoder4.decode_biguint(poly12));
        }

        TEST_METHOD(BinaryEncodeDecodeUInt64)
        {
            int base = 2;
            BigUInt modulus("FFFFFFFFFFFFFFFF");
            BinaryEncoder encoder(modulus);

            BigPoly poly = encoder.encode(static_cast<uint64_t>(0));
            Assert::AreEqual(0, poly.coeff_count());
            Assert::IsTrue(poly.is_zero());
            Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(poly));

            BigPoly poly1 = encoder.encode(static_cast<uint64_t>(1));
            Assert::AreEqual(1, poly1.coeff_count());
            Assert::AreEqual(1, poly1.coeff_bit_count());
            Assert::IsTrue("1" == poly1.to_string());
            Assert::AreEqual(static_cast<uint64_t>(1), encoder.decode_uint64(poly1));

            BigPoly poly2 = encoder.encode(static_cast<uint64_t>(2));
            Assert::AreEqual(2, poly2.coeff_count());
            Assert::AreEqual(1, poly2.coeff_bit_count());
            Assert::IsTrue("1x^1" == poly2.to_string());
            Assert::AreEqual(static_cast<uint64_t>(2), encoder.decode_uint64(poly2));

            BigPoly poly3 = encoder.encode(static_cast<uint64_t>(3));
            Assert::AreEqual(2, poly3.coeff_count());
            Assert::AreEqual(1, poly3.coeff_bit_count());
            Assert::IsTrue("1x^1 + 1" == poly3.to_string());
            Assert::AreEqual(static_cast<uint64_t>(3), encoder.decode_uint64(poly3));

            BigPoly poly4 = encoder.encode(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF));
            Assert::AreEqual(64, poly4.coeff_count());
            Assert::AreEqual(1, poly4.coeff_bit_count());
            for (int i = 0; i < 64; ++i)
            {
                Assert::IsTrue("1" == poly4[i].to_string());
            }
            Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), encoder.decode_uint64(poly4));

            BigPoly poly5 = encoder.encode(static_cast<uint64_t>(0x80F02));
            Assert::AreEqual(20, poly5.coeff_count());
            Assert::AreEqual(1, poly5.coeff_bit_count());
            for (int i = 0; i < 20; ++i)
            {
                if (i == 19 || (i >= 8 && i <= 11) || i == 1)
                {
                    Assert::IsTrue("1" == poly5[i].to_string());
                }
                else
                {
                    Assert::IsTrue(poly5[i].is_zero());
                }
            }
            Assert::AreEqual(static_cast<uint64_t>(0x80F02), encoder.decode_uint64(poly5));

            BigPoly poly6(3, 10);
            poly6[0] = 1;
            poly6[1] = 500;
            poly6[2] = 1023;
            Assert::AreEqual(static_cast<uint64_t>(1 + 500 * 2 + 1023 * 4), encoder.decode_uint64(poly6));

            modulus = 1024;
            BinaryEncoder encoder2(modulus);
            BigPoly poly7(4, 10);
            poly7[0] = 1023; // -1   (*1)
            poly7[1] = 512;  // -512 (*2)
            poly7[2] = 511;  // 511  (*4)
            poly7[3] = 1;    // 1    (*8)
            Assert::AreEqual(static_cast<uint64_t>(-1 + -512 * 2 + 511 * 4 + 1 * 8), encoder2.decode_uint64(poly7));
        }

        TEST_METHOD(BalancedEncodeDecodeUInt64)
        {
            BigUInt modulus("10000");
            BalancedEncoder encoder(modulus);

            BigPoly poly = encoder.encode(static_cast<uint64_t>(0));
            Assert::AreEqual(0, poly.significant_coeff_count());
            Assert::IsTrue(poly.is_zero());
            Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(poly));

            BigPoly poly1 = encoder.encode(static_cast<uint64_t>(1));
            Assert::AreEqual(1, poly1.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly1.coeff_bit_count());
            Assert::IsTrue("1" == poly1.to_string());
            Assert::AreEqual(static_cast<uint64_t>(1), encoder.decode_uint64(poly1));

            BigPoly poly2 = encoder.encode(static_cast<uint64_t>(2));
            Assert::AreEqual(2, poly2.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly2.coeff_bit_count());
            Assert::IsTrue("1x^1 + FFFF" == poly2.to_string());
            Assert::AreEqual(static_cast<uint64_t>(2), encoder.decode_uint64(poly2));

            BigPoly poly3 = encoder.encode(static_cast<uint64_t>(3));
            Assert::AreEqual(2, poly3.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly3.coeff_bit_count());
            Assert::IsTrue("1x^1" == poly3.to_string());
            Assert::AreEqual(static_cast<uint64_t>(3), encoder.decode_uint64(poly3));

            BigPoly poly4 = encoder.encode(static_cast<uint64_t>(0x2671));
            Assert::AreEqual(9, poly4.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly4.coeff_bit_count());
            for (int i = 0; i < 9; ++i)
            {
                Assert::IsTrue("1" == poly4[i].to_string());
            }
            Assert::AreEqual(static_cast<uint64_t>(0x2671), encoder.decode_uint64(poly4));

            BigPoly poly5 = encoder.encode(static_cast<uint64_t>(0xD4EB));
            Assert::AreEqual(11, poly5.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly5.coeff_bit_count());
            for (int i = 0; i < 11; ++i)
            {
                if (i % 3 == 1)
                {
                    Assert::IsTrue("1" == poly5[i].to_string());
                }
                else if (i % 3 == 0)
                {
                    Assert::IsTrue(poly5[i].is_zero());
                }
                else
                {
                    Assert::IsTrue("FFFF" == poly5[i].to_string());
                }
            }
            Assert::AreEqual(static_cast<uint64_t>(0xD4EB), encoder.decode_uint64(poly5));

            BigPoly poly6(3, 10);
            poly6[0] = 1;
            poly6[1] = 500;
            poly6[2] = 1023;
            Assert::AreEqual(static_cast<uint64_t>(1 + 500 * 3 + 1023 * 9), encoder.decode_uint64(poly6));

            BalancedEncoder encoder2(modulus, 7);
            BigPoly poly7(4, 16);
            poly7[0] = 123; // 123   (*1)
            poly7[1] = "FFFF";  // -1 (*7)
            poly7[2] = 511;  // 511  (*49)
            poly7[3] = 1;    // 1    (*343)
            Assert::AreEqual(static_cast<uint64_t>(123 + -1 * 7 + 511 * 49 + 1 * 343), encoder2.decode_uint64(poly7));

            BalancedEncoder encoder3(modulus, 6);
            BigPoly poly8(4, 16);
            poly8[0] = 5;
            poly8[1] = 4;
            poly8[2] = 3;
            poly8[3] = 2;
            uint64_t value = 5 + 4 * 6 + 3 * 36 + 2 * 216;
            Assert::IsTrue(value == encoder3.decode_uint64(poly8));

            BalancedEncoder encoder4(modulus, 10);
            BigPoly poly9(4, 16);
            poly9[0] = 1;
            poly9[1] = 2;
            poly9[2] = 3;
            poly9[3] = 4;
            value = 4321;
            Assert::IsTrue(value == encoder4.decode_uint64(poly9));

            value = 1234;
            BigPoly poly10 = encoder2.encode(value);
            Assert::AreEqual(5, poly10.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly10.coeff_bit_count());
            Assert::IsTrue(value == encoder2.decode_uint64(poly10));

            value = 1234;
            BigPoly poly11 = encoder3.encode(value);
            Assert::AreEqual(5, poly11.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly11.coeff_bit_count());
            Assert::IsTrue(value == encoder3.decode_uint64(poly11));

            value = 1234;
            BigPoly poly12 = encoder4.encode(value);
            Assert::AreEqual(4, poly12.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly12.coeff_bit_count());
            Assert::IsTrue(value == encoder4.decode_uint64(poly12));
        }

        TEST_METHOD(BinaryEncodeDecodeUInt32)
        {
            BigUInt modulus("FFFFFFFFFFFFFFFF");
            BinaryEncoder encoder(modulus);

            BigPoly poly = encoder.encode(static_cast<uint32_t>(0));
            Assert::AreEqual(0, poly.significant_coeff_count());
            Assert::IsTrue(poly.is_zero());
            Assert::AreEqual(static_cast<uint32_t>(0), encoder.decode_uint32(poly));

            BigPoly poly1 = encoder.encode(static_cast<uint32_t>(1));
            Assert::AreEqual(1, poly1.significant_coeff_count());
            Assert::AreEqual(1, poly1.coeff_bit_count());
            Assert::IsTrue("1" == poly1.to_string());
            Assert::AreEqual(static_cast<uint32_t>(1), encoder.decode_uint32(poly1));

            BigPoly poly2 = encoder.encode(static_cast<uint32_t>(2));
            Assert::AreEqual(2, poly2.significant_coeff_count());
            Assert::AreEqual(1, poly2.coeff_bit_count());
            Assert::IsTrue("1x^1" == poly2.to_string());
            Assert::AreEqual(static_cast<uint32_t>(2), encoder.decode_uint32(poly2));

            BigPoly poly3 = encoder.encode(static_cast<uint32_t>(3));
            Assert::AreEqual(2, poly3.significant_coeff_count());
            Assert::AreEqual(1, poly3.coeff_bit_count());
            Assert::IsTrue("1x^1 + 1" == poly3.to_string());
            Assert::AreEqual(static_cast<uint32_t>(3), encoder.decode_uint32(poly3));

            BigPoly poly4 = encoder.encode(static_cast<uint32_t>(0xFFFFFFFF));
            Assert::AreEqual(32, poly4.significant_coeff_count());
            Assert::AreEqual(1, poly4.coeff_bit_count());
            for (int i = 0; i < 32; ++i)
            {
                Assert::IsTrue("1" == poly4[i].to_string());
            }
            Assert::AreEqual(static_cast<uint32_t>(0xFFFFFFFF), encoder.decode_uint32(poly4));

            BigPoly poly5 = encoder.encode(static_cast<uint32_t>(0x80F02));
            Assert::AreEqual(20, poly5.significant_coeff_count());
            Assert::AreEqual(1, poly5.coeff_bit_count());
            for (int i = 0; i < 20; ++i)
            {
                if (i == 19 || (i >= 8 && i <= 11) || i == 1)
                {
                    Assert::IsTrue("1" == poly5[i].to_string());
                }
                else
                {
                    Assert::IsTrue(poly5[i].is_zero());
                }
            }
            Assert::AreEqual(static_cast<uint32_t>(0x80F02), encoder.decode_uint32(poly5));

            BigPoly poly6(3, 10);
            poly6[0] = 1;
            poly6[1] = 500;
            poly6[2] = 1023;
            Assert::AreEqual(static_cast<uint32_t>(1 + 500 * 2 + 1023 * 4), encoder.decode_uint32(poly6));

            modulus = 1024;
            BinaryEncoder encoder2(modulus);
            BigPoly poly7(4, 10);
            poly7[0] = 1023; // -1   (*1)
            poly7[1] = 512;  // -512 (*2)
            poly7[2] = 511;  // 511  (*4)
            poly7[3] = 1;    // 1    (*8)
            Assert::AreEqual(static_cast<uint32_t>(-1 + -512 * 2 + 511 * 4 + 1 * 8), encoder2.decode_uint32(poly7));
        }

        TEST_METHOD(BalancedEncodeDecodeUInt32)
        {
            BigUInt modulus("10000");
            BalancedEncoder encoder(modulus);

            BigPoly poly = encoder.encode(static_cast<uint32_t>(0));
            Assert::AreEqual(0, poly.significant_coeff_count());
            Assert::IsTrue(poly.is_zero());
            Assert::AreEqual(static_cast<uint32_t>(0), encoder.decode_uint32(poly));

            BigPoly poly1 = encoder.encode(static_cast<uint32_t>(1));
            Assert::AreEqual(1, poly1.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly1.coeff_bit_count());
            Assert::IsTrue("1" == poly1.to_string());
            Assert::AreEqual(static_cast<uint32_t>(1), encoder.decode_uint32(poly1));

            BigPoly poly2 = encoder.encode(static_cast<uint32_t>(2));
            Assert::AreEqual(2, poly2.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly2.coeff_bit_count());
            Assert::IsTrue("1x^1 + FFFF" == poly2.to_string());
            Assert::AreEqual(static_cast<uint32_t>(2), encoder.decode_uint32(poly2));

            BigPoly poly3 = encoder.encode(static_cast<uint32_t>(3));
            Assert::AreEqual(2, poly3.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly3.coeff_bit_count());
            Assert::IsTrue("1x^1" == poly3.to_string());
            Assert::AreEqual(static_cast<uint32_t>(3), encoder.decode_uint32(poly3));

            BigPoly poly4 = encoder.encode(static_cast<uint32_t>(0x2671));
            Assert::AreEqual(9, poly4.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly4.coeff_bit_count());
            for (int i = 0; i < 9; ++i)
            {
                Assert::IsTrue("1" == poly4[i].to_string());
            }
            Assert::AreEqual(static_cast<uint32_t>(0x2671), encoder.decode_uint32(poly4));

            BigPoly poly5 = encoder.encode(static_cast<uint32_t>(0xD4EB));
            Assert::AreEqual(11, poly5.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly5.coeff_bit_count());
            for (int i = 0; i < 11; ++i)
            {
                if (i % 3 == 1)
                {
                    Assert::IsTrue("1" == poly5[i].to_string());
                }
                else if (i % 3 == 0)
                {
                    Assert::IsTrue(poly5[i].is_zero());
                }
                else
                {
                    Assert::IsTrue("FFFF" == poly5[i].to_string());
                }
            }
            Assert::AreEqual(static_cast<uint32_t>(0xD4EB), encoder.decode_uint32(poly5));

            BigPoly poly6(3, 10);
            poly6[0] = 1;
            poly6[1] = 500;
            poly6[2] = 1023;
            Assert::AreEqual(static_cast<uint32_t>(1 + 500 * 3 + 1023 * 9), encoder.decode_uint32(poly6));

            BalancedEncoder encoder2(modulus, 7);
            BigPoly poly7(4, 16);
            poly7[0] = 123; // 123   (*1)
            poly7[1] = "FFFF";  // -1 (*7)
            poly7[2] = 511;  // 511  (*49)
            poly7[3] = 1;    // 1    (*343)
            Assert::AreEqual(static_cast<uint32_t>(123 + -1 * 7 + 511 * 49 + 1 * 343), encoder2.decode_uint32(poly7));

            BalancedEncoder encoder3(modulus, 6);
            BigPoly poly8(4, 16);
            poly8[0] = 5;
            poly8[1] = 4;
            poly8[2] = 3;
            poly8[3] = 2;
            uint64_t value = 5 + 4 * 6 + 3 * 36 + 2 * 216;
            Assert::IsTrue(value == encoder3.decode_uint32(poly8));

            BalancedEncoder encoder4(modulus, 10);
            BigPoly poly9(4, 16);
            poly9[0] = 1;
            poly9[1] = 2;
            poly9[2] = 3;
            poly9[3] = 4;
            value = 4321;
            Assert::IsTrue(value == encoder4.decode_uint32(poly9));

            value = 1234;
            BigPoly poly10 = encoder2.encode(value);
            Assert::AreEqual(5, poly10.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly10.coeff_bit_count());
            Assert::IsTrue(value == encoder2.decode_uint32(poly10));

            value = 1234;
            BigPoly poly11 = encoder3.encode(value);
            Assert::AreEqual(5, poly11.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly11.coeff_bit_count());
            Assert::IsTrue(value == encoder3.decode_uint32(poly11));

            value = 1234;
            BigPoly poly12 = encoder4.encode(value);
            Assert::AreEqual(4, poly12.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly12.coeff_bit_count());
            Assert::IsTrue(value == encoder4.decode_uint32(poly12));
        }

        TEST_METHOD(BinaryEncodeDecodeInt64)
        {
            BigUInt modulus("FFFFFFFFFFFFFFFF");
            BinaryEncoder encoder(modulus);

            BigPoly poly = encoder.encode(static_cast<int64_t>(0));
            Assert::AreEqual(0, poly.significant_coeff_count());
            Assert::IsTrue(poly.is_zero());
            Assert::AreEqual(static_cast<uint64_t>(0), static_cast<uint64_t>(encoder.decode_int64(poly)));

            BigPoly poly1 = encoder.encode(static_cast<int64_t>(1));
            Assert::AreEqual(1, poly1.significant_coeff_count());
            Assert::AreEqual(1, poly1.coeff_bit_count());
            Assert::IsTrue("1" == poly1.to_string());
            Assert::AreEqual(static_cast<uint64_t>(1), static_cast<uint64_t>(encoder.decode_int64(poly1)));

            BigPoly poly2 = encoder.encode(static_cast<int64_t>(2));
            Assert::AreEqual(2, poly2.significant_coeff_count());
            Assert::AreEqual(1, poly2.coeff_bit_count());
            Assert::IsTrue("1x^1" == poly2.to_string());
            Assert::AreEqual(static_cast<uint64_t>(2), static_cast<uint64_t>(encoder.decode_int64(poly2)));

            BigPoly poly3 = encoder.encode(static_cast<int64_t>(3));
            Assert::AreEqual(2, poly3.significant_coeff_count());
            Assert::AreEqual(1, poly3.coeff_bit_count());
            Assert::IsTrue("1x^1 + 1" == poly3.to_string());
            Assert::AreEqual(static_cast<uint64_t>(3), static_cast<uint64_t>(encoder.decode_int64(poly3)));

            BigPoly poly4 = encoder.encode(static_cast<int64_t>(-1));
            Assert::AreEqual(1, poly4.significant_coeff_count());
            Assert::AreEqual(64, poly4.coeff_bit_count());
            Assert::IsTrue("FFFFFFFFFFFFFFFE" == poly4.to_string());
            Assert::AreEqual(static_cast<uint64_t>(-1), static_cast<uint64_t>(encoder.decode_int64(poly4)));

            BigPoly poly5 = encoder.encode(static_cast<int64_t>(-2));
            Assert::AreEqual(2, poly5.significant_coeff_count());
            Assert::AreEqual(64, poly5.coeff_bit_count());
            Assert::IsTrue("FFFFFFFFFFFFFFFEx^1" == poly5.to_string());
            Assert::AreEqual(static_cast<uint64_t>(-2), static_cast<uint64_t>(encoder.decode_int64(poly5)));

            BigPoly poly6 = encoder.encode(static_cast<int64_t>(-3));
            Assert::AreEqual(2, poly6.significant_coeff_count());
            Assert::AreEqual(64, poly6.coeff_bit_count());
            Assert::IsTrue("FFFFFFFFFFFFFFFEx^1 + FFFFFFFFFFFFFFFE" == poly6.to_string());
            Assert::AreEqual(static_cast<uint64_t>(-3), static_cast<uint64_t>(encoder.decode_int64(poly6)));

            BigPoly poly7 = encoder.encode(static_cast<int64_t>(0x7FFFFFFFFFFFFFFF));
            Assert::AreEqual(63, poly7.significant_coeff_count());
            Assert::AreEqual(1, poly7.coeff_bit_count());
            for (int i = 0; i < 63; ++i)
            {
                Assert::IsTrue("1" == poly7[i].to_string());
            }
            Assert::AreEqual(static_cast<uint64_t>(0x7FFFFFFFFFFFFFFF), static_cast<uint64_t>(encoder.decode_int64(poly7)));

            BigPoly poly8 = encoder.encode(static_cast<int64_t>(0x8000000000000000));
            Assert::AreEqual(64, poly8.significant_coeff_count());
            Assert::AreEqual(64, poly8.coeff_bit_count());
            Assert::IsTrue("FFFFFFFFFFFFFFFE" == poly8[63].to_string());
            for (int i = 0; i < 63; ++i)
            {
                Assert::IsTrue(poly8[i].is_zero());
            }
            Assert::AreEqual(static_cast<uint64_t>(0x8000000000000000), static_cast<uint64_t>(encoder.decode_int64(poly8)));

            BigPoly poly9 = encoder.encode(static_cast<int64_t>(0x80F02));
            Assert::AreEqual(20, poly9.significant_coeff_count());
            Assert::AreEqual(1, poly9.coeff_bit_count());
            for (int i = 0; i < 20; ++i)
            {
                if (i == 19 || (i >= 8 && i <= 11) || i == 1)
                {
                    Assert::IsTrue("1" == poly9[i].to_string());
                }
                else
                {
                    Assert::IsTrue(poly9[i].is_zero());
                }
            }
            Assert::AreEqual(static_cast<uint64_t>(0x80F02), static_cast<uint64_t>(encoder.decode_int64(poly9)));

            BigPoly poly10 = encoder.encode(static_cast<int64_t>(-1073));
            Assert::AreEqual(11, poly10.significant_coeff_count());
            Assert::AreEqual(64, poly10.coeff_bit_count());
            Assert::IsTrue("FFFFFFFFFFFFFFFE" == poly10[10].to_string());
            Assert::IsTrue(poly10[9].is_zero());
            Assert::IsTrue(poly10[8].is_zero());
            Assert::IsTrue(poly10[7].is_zero());
            Assert::IsTrue(poly10[6].is_zero());
            Assert::IsTrue("FFFFFFFFFFFFFFFE" == poly10[5].to_string());
            Assert::IsTrue("FFFFFFFFFFFFFFFE" == poly10[4].to_string());
            Assert::IsTrue(poly10[3].is_zero());
            Assert::IsTrue(poly10[2].is_zero());
            Assert::IsTrue(poly10[1].is_zero());
            Assert::IsTrue("FFFFFFFFFFFFFFFE" == poly10[0].to_string());
            Assert::AreEqual(static_cast<uint64_t>(-1073), static_cast<uint64_t>(encoder.decode_int64(poly10)));

            modulus = "FFFF";
            BinaryEncoder encoder2(modulus);
            BigPoly poly11(6, 64);
            poly11[0] = 1;
            poly11[1] = "FFFE"; // -1
            poly11[2] = "FFFD"; // -2
            poly11[3] = "8000"; // -32767
            poly11[4] = "7FFF"; // 32767
            poly11[5] = "7FFE"; // 32766
            Assert::AreEqual(static_cast<uint64_t>(1 + -1 * 2 + -2 * 4 + -32767 * 8 + 32767 * 16 + 32766 * 32), static_cast<uint64_t>(encoder2.decode_int64(poly11)));
        }

        TEST_METHOD(BalancedEncodeDecodeInt64)
        {
            BigUInt modulus("10000");
            BalancedEncoder encoder(modulus);

            BigPoly poly = encoder.encode(static_cast<int64_t>(0));
            Assert::AreEqual(0, poly.significant_coeff_count());
            Assert::IsTrue(poly.is_zero());
            Assert::AreEqual(static_cast<uint64_t>(0), static_cast<uint64_t>(encoder.decode_int64(poly)));

            BigPoly poly1 = encoder.encode(static_cast<int64_t>(1));
            Assert::AreEqual(1, poly1.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly1.coeff_bit_count());
            Assert::IsTrue("1" == poly1.to_string());
            Assert::AreEqual(static_cast<uint64_t>(1), static_cast<uint64_t>(encoder.decode_int64(poly1)));

            BigPoly poly2 = encoder.encode(static_cast<int64_t>(2));
            Assert::AreEqual(2, poly2.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly2.coeff_bit_count());
            Assert::IsTrue("1x^1 + FFFF" == poly2.to_string());
            Assert::AreEqual(static_cast<uint64_t>(2), static_cast<uint64_t>(encoder.decode_int64(poly2)));

            BigPoly poly3 = encoder.encode(static_cast<int64_t>(3));
            Assert::AreEqual(2, poly3.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly3.coeff_bit_count());
            Assert::IsTrue("1x^1" == poly3.to_string());
            Assert::AreEqual(static_cast<uint64_t>(3), static_cast<uint64_t>(encoder.decode_int64(poly3)));

            BigPoly poly4 = encoder.encode(static_cast<int64_t>(-1));
            Assert::AreEqual(1, poly4.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly4.coeff_bit_count());
            Assert::IsTrue("FFFF" == poly4.to_string());
            Assert::AreEqual(static_cast<uint64_t>(-1), static_cast<uint64_t>(encoder.decode_int64(poly4)));

            BigPoly poly5 = encoder.encode(static_cast<int64_t>(-2));
            Assert::AreEqual(2, poly5.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly5.coeff_bit_count());
            Assert::IsTrue("FFFFx^1 + 1" == poly5.to_string());
            Assert::AreEqual(static_cast<uint64_t>(-2), static_cast<uint64_t>(encoder.decode_int64(poly5)));

            BigPoly poly6 = encoder.encode(static_cast<int64_t>(-3));
            Assert::AreEqual(2, poly6.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly6.coeff_bit_count());
            Assert::IsTrue("FFFFx^1" == poly6.to_string());
            Assert::AreEqual(static_cast<uint64_t>(-3), static_cast<uint64_t>(encoder.decode_int64(poly6)));

            BigPoly poly7 = encoder.encode(static_cast<int64_t>(-0x2671));
            Assert::AreEqual(9, poly7.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly7.coeff_bit_count());
            for (int i = 0; i < 9; ++i)
            {
                Assert::IsTrue("FFFF" == poly7[i].to_string());
            }
            Assert::AreEqual(static_cast<uint64_t>(-0x2671), static_cast<uint64_t>(encoder.decode_int64(poly7)));

            BigPoly poly8 = encoder.encode(static_cast<int64_t>(-4374));
            Assert::AreEqual(9, poly8.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly8.coeff_bit_count());
            Assert::IsTrue("FFFF" == poly8[8].to_string());
            Assert::IsTrue("1" == poly8[7].to_string());
            for (int i = 0; i < 7; ++i)
            {
                Assert::IsTrue(poly8[i].is_zero());
            }
            Assert::AreEqual(static_cast<uint64_t>(-4374), static_cast<uint64_t>(encoder.decode_int64(poly8)));

            BigPoly poly9 = encoder.encode(static_cast<int64_t>(-0xD4EB));
            Assert::AreEqual(11, poly9.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly9.coeff_bit_count());
            for (int i = 0; i < 11; ++i)
            {
                if (i % 3 == 1)
                {
                    Assert::IsTrue("FFFF" == poly9[i].to_string());
                }
                else if (i % 3 == 0)
                {
                    Assert::IsTrue(poly9[i].is_zero());
                }
                else
                {
                    Assert::IsTrue("1" == poly9[i].to_string());
                }
            }
            Assert::AreEqual(static_cast<uint64_t>(-0xD4EB), static_cast<uint64_t>(encoder.decode_int64(poly9)));

            BigPoly poly10 = encoder.encode(static_cast<int64_t>(-30724));
            Assert::AreEqual(11, poly10.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly10.coeff_bit_count());
            Assert::IsTrue("FFFF" == poly10[10].to_string());
            Assert::IsTrue("1" == poly10[9].to_string());
            Assert::IsTrue("1" == poly10[8].to_string());
            Assert::IsTrue("1" == poly10[7].to_string());
            Assert::IsTrue(poly10[6].is_zero());
            Assert::IsTrue(poly10[5].is_zero());
            Assert::IsTrue("FFFF" == poly10[4].to_string());
            Assert::IsTrue("FFFF" == poly10[3].to_string());
            Assert::IsTrue(poly10[2].is_zero());
            Assert::IsTrue("1" == poly10[1].to_string());
            Assert::IsTrue("FFFF" == poly10[0].to_string());
            Assert::AreEqual(static_cast<uint64_t>(-30724), static_cast<uint64_t>(encoder.decode_int64(poly10)));

            BalancedEncoder encoder2(modulus, 13);
            BigPoly poly11 = encoder2.encode(static_cast<int64_t>(-126375543984));
            Assert::AreEqual(11, poly11.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly11.coeff_bit_count());
            Assert::IsTrue("FFFF" == poly11[10].to_string());
            Assert::IsTrue("1" == poly11[9].to_string());
            Assert::IsTrue("1" == poly11[8].to_string());
            Assert::IsTrue("1" == poly11[7].to_string());
            Assert::IsTrue(poly11[6].is_zero());
            Assert::IsTrue(poly11[5].is_zero());
            Assert::IsTrue("FFFF" == poly11[4].to_string());
            Assert::IsTrue("FFFF" == poly11[3].to_string());
            Assert::IsTrue(poly11[2].is_zero());
            Assert::IsTrue("1" == poly11[1].to_string());
            Assert::IsTrue("FFFF" == poly11[0].to_string());
            Assert::AreEqual(static_cast<uint64_t>(-126375543984), static_cast<uint64_t>(encoder2.decode_int64(poly11)));

            modulus = "FFFF";
            BalancedEncoder encoder3(modulus, 7);
            BigPoly poly12(6, 16);
            poly12[0] = 1;
            poly12[1] = "FFFE"; // -1
            poly12[2] = "FFFD"; // -2
            poly12[3] = "8000"; // -32767
            poly12[4] = "7FFF"; // 32767
            poly12[5] = "7FFE"; // 32766
            Assert::AreEqual(static_cast<uint64_t>(1 + -1 * 7 + -2 * 49 + -32767 * 343 + 32767 * 2401 + 32766 * 16807), static_cast<uint64_t>(encoder3.decode_int64(poly12)));
        
            BalancedEncoder encoder4(modulus, 6);
            poly8.resize(4, 16);
            poly8[0] = 5;
            poly8[1] = 4;
            poly8[2] = 3;
            poly8[3] = *modulus.pointer() - 2;
            int64_t value = 5 + 4 * 6 + 3 * 36 - 2 * 216;
            Assert::IsTrue(value == encoder4.decode_int64(poly8));

            BalancedEncoder encoder5(modulus, 10);
            poly9.resize(4, 16);
            poly9[0] = 1;
            poly9[1] = 2;
            poly9[2] = 3;
            poly9[3] = 4;
            value = 4321;
            Assert::IsTrue(value == encoder5.decode_int64(poly9));

            value = -1234;
            poly10 = encoder3.encode(value);
            Assert::AreEqual(5, poly10.significant_coeff_count());
            Assert::AreEqual(modulus.significant_bit_count(), poly10.coeff_bit_count());
            Assert::IsTrue(value == encoder3.decode_int64(poly10));

            value = -1234;
            poly11 = encoder4.encode(value);
            Assert::AreEqual(5, poly11.significant_coeff_count());
            Assert::AreEqual(modulus.significant_bit_count(), poly11.coeff_bit_count());
            Assert::IsTrue(value == encoder4.decode_int64(poly11));

            value = -1234;
            poly12 = encoder5.encode(value);
            Assert::AreEqual(4, poly12.significant_coeff_count());
            Assert::AreEqual(modulus.significant_bit_count(), poly12.coeff_bit_count());
            Assert::IsTrue(value == encoder5.decode_int64(poly12));
        }

        TEST_METHOD(EncodeDecodeInt32)
        {
            BigUInt modulus("FFFFFFFFFFFFFFFF");
            BinaryEncoder encoder(modulus);

            BigPoly poly = encoder.encode(static_cast<int32_t>(0));
            Assert::AreEqual(0, poly.significant_coeff_count());
            Assert::IsTrue(poly.is_zero());
            Assert::AreEqual(static_cast<int32_t>(0), encoder.decode_int32(poly));

            BigPoly poly1 = encoder.encode(static_cast<int32_t>(1));
            Assert::AreEqual(1, poly1.significant_coeff_count());
            Assert::AreEqual(1, poly1.coeff_bit_count());
            Assert::IsTrue("1" == poly1.to_string());
            Assert::AreEqual(static_cast<int32_t>(1), encoder.decode_int32(poly1));

            BigPoly poly2 = encoder.encode(static_cast<int32_t>(2));
            Assert::AreEqual(2, poly2.significant_coeff_count());
            Assert::AreEqual(1, poly2.coeff_bit_count());
            Assert::IsTrue("1x^1" == poly2.to_string());
            Assert::AreEqual(static_cast<int32_t>(2), encoder.decode_int32(poly2));

            BigPoly poly3 = encoder.encode(static_cast<int32_t>(3));
            Assert::AreEqual(2, poly3.significant_coeff_count());
            Assert::AreEqual(1, poly3.coeff_bit_count());
            Assert::IsTrue("1x^1 + 1" == poly3.to_string());
            Assert::AreEqual(static_cast<int32_t>(3), encoder.decode_int32(poly3));

            BigPoly poly4 = encoder.encode(static_cast<int32_t>(-1));
            Assert::AreEqual(1, poly4.significant_coeff_count());
            Assert::AreEqual(64, poly4.coeff_bit_count());
            Assert::IsTrue("FFFFFFFFFFFFFFFE" == poly4.to_string());
            Assert::AreEqual(static_cast<int32_t>(-1), encoder.decode_int32(poly4));

            BigPoly poly5 = encoder.encode(static_cast<int32_t>(-2));
            Assert::AreEqual(2, poly5.significant_coeff_count());
            Assert::AreEqual(64, poly5.coeff_bit_count());
            Assert::IsTrue("FFFFFFFFFFFFFFFEx^1" == poly5.to_string());
            Assert::AreEqual(static_cast<int32_t>(-2), encoder.decode_int32(poly5));

            BigPoly poly6 = encoder.encode(static_cast<int32_t>(-3));
            Assert::AreEqual(2, poly6.significant_coeff_count());
            Assert::AreEqual(64, poly6.coeff_bit_count());
            Assert::IsTrue("FFFFFFFFFFFFFFFEx^1 + FFFFFFFFFFFFFFFE" == poly6.to_string());
            Assert::AreEqual(static_cast<int32_t>(-3), encoder.decode_int32(poly6));

            BigPoly poly7 = encoder.encode(static_cast<int32_t>(0x7FFFFFFF));
            Assert::AreEqual(31, poly7.significant_coeff_count());
            Assert::AreEqual(1, poly7.coeff_bit_count());
            for (int i = 0; i < 31; ++i)
            {
                Assert::IsTrue("1" == poly7[i].to_string());
            }
            Assert::AreEqual(static_cast<int32_t>(0x7FFFFFFF), encoder.decode_int32(poly7));

            BigPoly poly8 = encoder.encode(static_cast<int32_t>(0x80000000));
            Assert::AreEqual(32, poly8.significant_coeff_count());
            Assert::AreEqual(64, poly8.coeff_bit_count());
            Assert::IsTrue("FFFFFFFFFFFFFFFE" == poly8[31].to_string());
            for (int i = 0; i < 31; ++i)
            {
                Assert::IsTrue(poly8[i].is_zero());
            }
            Assert::AreEqual(static_cast<int32_t>(0x80000000), encoder.decode_int32(poly8));

            BigPoly poly9 = encoder.encode(static_cast<int32_t>(0x80F02));
            Assert::AreEqual(20, poly9.significant_coeff_count());
            Assert::AreEqual(1, poly9.coeff_bit_count());
            for (int i = 0; i < 20; ++i)
            {
                if (i == 19 || (i >= 8 && i <= 11) || i == 1)
                {
                    Assert::IsTrue("1" == poly9[i].to_string());
                }
                else
                {
                    Assert::IsTrue(poly9[i].is_zero());
                }
            }
            Assert::AreEqual(static_cast<int32_t>(0x80F02), encoder.decode_int32(poly9));

            BigPoly poly10 = encoder.encode(static_cast<int32_t>(-1073));
            Assert::AreEqual(11, poly10.significant_coeff_count());
            Assert::AreEqual(64, poly10.coeff_bit_count());
            Assert::IsTrue("FFFFFFFFFFFFFFFE" == poly10[10].to_string());
            Assert::IsTrue(poly10[9].is_zero());
            Assert::IsTrue(poly10[8].is_zero());
            Assert::IsTrue(poly10[7].is_zero());
            Assert::IsTrue(poly10[6].is_zero());
            Assert::IsTrue("FFFFFFFFFFFFFFFE" == poly10[5].to_string());
            Assert::IsTrue("FFFFFFFFFFFFFFFE" == poly10[4].to_string());
            Assert::IsTrue(poly10[3].is_zero());
            Assert::IsTrue(poly10[2].is_zero());
            Assert::IsTrue(poly10[1].is_zero());
            Assert::IsTrue("FFFFFFFFFFFFFFFE" == poly10[0].to_string());
            Assert::AreEqual(static_cast<int32_t>(-1073), encoder.decode_int32(poly10));

            modulus = "FFFF";
            BinaryEncoder encoder2(modulus);
            BigPoly poly11(6, 64);
            poly11[0] = 1;
            poly11[1] = "FFFE"; // -1
            poly11[2] = "FFFD"; // -2
            poly11[3] = "8000"; // -32767
            poly11[4] = "7FFF"; // 32767
            poly11[5] = "7FFE"; // 32766
            Assert::AreEqual(static_cast<int32_t>(1 + -1 * 2 + -2 * 4 + -32767 * 8 + 32767 * 16 + 32766 * 32), encoder2.decode_int32(poly11));
        }

        TEST_METHOD(BalancedEncodeDecodeInt32)
        {
            BigUInt modulus("10000");
            BalancedEncoder encoder(modulus);

            BigPoly poly = encoder.encode(static_cast<int32_t>(0));
            Assert::AreEqual(0, poly.significant_coeff_count());
            Assert::IsTrue(poly.is_zero());
            Assert::AreEqual(static_cast<int32_t>(0), encoder.decode_int32(poly));

            BigPoly poly1 = encoder.encode(static_cast<int32_t>(1));
            Assert::AreEqual(1, poly1.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly1.coeff_bit_count());
            Assert::IsTrue("1" == poly1.to_string());
            Assert::AreEqual(static_cast<int32_t>(1), encoder.decode_int32(poly1));

            BigPoly poly2 = encoder.encode(static_cast<int32_t>(2));
            Assert::AreEqual(2, poly2.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly2.coeff_bit_count());
            Assert::IsTrue("1x^1 + FFFF" == poly2.to_string());
            Assert::AreEqual(static_cast<int32_t>(2), encoder.decode_int32(poly2));

            BigPoly poly3 = encoder.encode(static_cast<int32_t>(3));
            Assert::AreEqual(2, poly3.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly3.coeff_bit_count());
            Assert::IsTrue("1x^1" == poly3.to_string());
            Assert::AreEqual(static_cast<int32_t>(3), encoder.decode_int32(poly3));

            BigPoly poly4 = encoder.encode(static_cast<int32_t>(-1));
            Assert::AreEqual(1, poly4.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly4.coeff_bit_count());
            Assert::IsTrue("FFFF" == poly4.to_string());
            Assert::AreEqual(static_cast<int32_t>(-1), encoder.decode_int32(poly4));

            BigPoly poly5 = encoder.encode(static_cast<int32_t>(-2));
            Assert::AreEqual(2, poly5.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly5.coeff_bit_count());
            Assert::IsTrue("FFFFx^1 + 1" == poly5.to_string());
            Assert::AreEqual(static_cast<int32_t>(-2), encoder.decode_int32(poly5));

            BigPoly poly6 = encoder.encode(static_cast<int32_t>(-3));
            Assert::AreEqual(2, poly6.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly6.coeff_bit_count());
            Assert::IsTrue("FFFFx^1" == poly6.to_string());
            Assert::AreEqual(static_cast<int32_t>(-3), encoder.decode_int32(poly6));

            BigPoly poly7 = encoder.encode(static_cast<int32_t>(-0x2671));
            Assert::AreEqual(9, poly7.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly7.coeff_bit_count());
            for (int i = 0; i < 9; ++i)
            {
                Assert::IsTrue("FFFF" == poly7[i].to_string());
            }
            Assert::AreEqual(static_cast<int32_t>(-0x2671), encoder.decode_int32(poly7));

            BigPoly poly8 = encoder.encode(static_cast<int32_t>(-4374));
            Assert::AreEqual(9, poly8.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly8.coeff_bit_count());
            Assert::IsTrue("FFFF" == poly8[8].to_string());
            Assert::IsTrue("1" == poly8[7].to_string());
            for (int i = 0; i < 7; ++i)
            {
                Assert::IsTrue(poly8[i].is_zero());
            }
            Assert::AreEqual(static_cast<int32_t>(-4374), encoder.decode_int32(poly8));

            BigPoly poly9 = encoder.encode(static_cast<int32_t>(-0xD4EB));
            Assert::AreEqual(11, poly9.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly9.coeff_bit_count());
            for (int i = 0; i < 11; ++i)
            {
                if (i % 3 == 1)
                {
                    Assert::IsTrue("FFFF" == poly9[i].to_string());
                }
                else if (i % 3 == 0)
                {
                    Assert::IsTrue(poly9[i].is_zero());
                }
                else
                {
                    Assert::IsTrue("1" == poly9[i].to_string());
                }
            }
            Assert::AreEqual(static_cast<int32_t>(-0xD4EB), encoder.decode_int32(poly9));

            BigPoly poly10 = encoder.encode(static_cast<int32_t>(-30724));
            Assert::AreEqual(11, poly10.significant_coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly10.coeff_bit_count());
            Assert::IsTrue("FFFF" == poly10[10].to_string());
            Assert::IsTrue("1" == poly10[9].to_string());
            Assert::IsTrue("1" == poly10[8].to_string());
            Assert::IsTrue("1" == poly10[7].to_string());
            Assert::IsTrue(poly10[6].is_zero());
            Assert::IsTrue(poly10[5].is_zero());
            Assert::IsTrue("FFFF" == poly10[4].to_string());
            Assert::IsTrue("FFFF" == poly10[3].to_string());
            Assert::IsTrue(poly10[2].is_zero());
            Assert::IsTrue("1" == poly10[1].to_string());
            Assert::IsTrue("FFFF" == poly10[0].to_string());
            Assert::AreEqual(static_cast<int32_t>(-30724), encoder.decode_int32(poly10));

            modulus = "FFFF";
            BalancedEncoder encoder2(modulus, 7);
            BigPoly poly12(6, 16);
            poly12[0] = 1;
            poly12[1] = "FFFE"; // -1
            poly12[2] = "FFFD"; // -2
            poly12[3] = "8000"; // -32767
            poly12[4] = "7FFF"; // 32767
            poly12[5] = "7FFE"; // 32766
            Assert::AreEqual(static_cast<int32_t>(1 + -1 * 7 + -2 * 49 + -32767 * 343 + 32767 * 2401 + 32766 * 16807), encoder2.decode_int32(poly12));
        
            BalancedEncoder encoder4(modulus, 6);
            poly8.resize(4, 16);
            poly8[0] = 5;
            poly8[1] = 4;
            poly8[2] = 3;
            poly8[3] = *modulus.pointer() - 2;
            int32_t value = 5 + 4 * 6 + 3 * 36 - 2 * 216;
            Assert::IsTrue(value == encoder4.decode_int32(poly8));

            BalancedEncoder encoder5(modulus, 10);
            poly9.resize(4, 16);
            poly9[0] = 1;
            poly9[1] = 2;
            poly9[2] = 3;
            poly9[3] = 4;
            value = 4321;
            Assert::IsTrue(value == encoder5.decode_int32(poly9));

            value = -1234;
            poly10 = encoder2.encode(value);
            Assert::AreEqual(5, poly10.significant_coeff_count());
            Assert::AreEqual(modulus.significant_bit_count(), poly10.coeff_bit_count());
            Assert::IsTrue(value == encoder2.decode_int32(poly10));

            value = -1234;
            BigPoly poly11 = encoder4.encode(value);
            Assert::AreEqual(5, poly11.significant_coeff_count());
            Assert::AreEqual(modulus.significant_bit_count(), poly11.coeff_bit_count());
            Assert::IsTrue(value == encoder4.decode_int32(poly11));

            value = -1234;
            poly12 = encoder5.encode(value);
            Assert::AreEqual(4, poly12.significant_coeff_count());
            Assert::AreEqual(modulus.significant_bit_count(), poly12.coeff_bit_count());
            Assert::IsTrue(value == encoder5.decode_int32(poly12));
        }

        TEST_METHOD(BinaryFractionalEncodeDecode)
        {
            BigPoly poly_modulus("1x^1024 + 1");
            BigUInt modulus("10000");
            BinaryFractionalEncoder encoder(modulus, poly_modulus, 500, 50);

            BigPoly poly = encoder.encode(0.0);
            Assert::AreEqual(poly_modulus.coeff_count(), poly.coeff_count());
            Assert::IsTrue(poly.is_zero());
            Assert::AreEqual(0.0, encoder.decode(poly));

            BigPoly poly1 = encoder.encode(-1.0);
            Assert::AreEqual(poly_modulus.coeff_count(), poly1.coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly1.coeff_bit_count());
            Assert::AreEqual(-1.0, encoder.decode(poly1));

            BigPoly poly2 = encoder.encode(0.1);
            Assert::AreEqual(poly_modulus.coeff_count(), poly2.coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly2.coeff_bit_count());
            Assert::IsTrue(fabs(encoder.decode(poly2) - 0.1) / 0.1 < 0.000001);

            BigPoly poly3 = encoder.encode(3.123);
            Assert::AreEqual(poly_modulus.coeff_count(), poly3.coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly3.coeff_bit_count());
            Assert::IsTrue(fabs(encoder.decode(poly3) - 3.123) / 3.123 < 0.000001);

            BigPoly poly4 = encoder.encode(-123.456);
            Assert::AreEqual(poly_modulus.coeff_count(), poly4.coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly4.coeff_bit_count());
            Assert::IsTrue(fabs(encoder.decode(poly4) + 123.456) / (-123.456) < 0.000001);

            BigPoly poly5 = encoder.encode(12345.98765);
            Assert::AreEqual(poly_modulus.coeff_count(), poly5.coeff_count());
            Assert::AreEqual(modulus.bit_count(), poly5.coeff_bit_count());
            Assert::IsTrue(fabs(encoder.decode(poly5) - 12345.98765) / 12345.98765 < 0.000001);
        }

        TEST_METHOD(BalancedFractionalEncodeDecode)
        {
            BigPoly poly_modulus("1x^1024 + 1");
            {
                BigUInt modulus("10000");
                for (uint64_t b = 3; b < 20; ++b)
                {
                    BalancedFractionalEncoder encoder(modulus, poly_modulus, 500, 50, b);

                    BigPoly poly = encoder.encode(0.0);
                    Assert::AreEqual(poly_modulus.coeff_count(), poly.coeff_count());
                    Assert::IsTrue(poly.is_zero());
                    Assert::AreEqual(0.0, encoder.decode(poly));

                    BigPoly poly1 = encoder.encode(-1.0);
                    Assert::AreEqual(poly_modulus.coeff_count(), poly1.coeff_count());
                    Assert::AreEqual(modulus.bit_count(), poly1.coeff_bit_count());
                    Assert::AreEqual(-1.0, encoder.decode(poly1));

                    BigPoly poly2 = encoder.encode(0.1);
                    Assert::AreEqual(poly_modulus.coeff_count(), poly2.coeff_count());
                    Assert::AreEqual(modulus.bit_count(), poly2.coeff_bit_count());
                    Assert::IsTrue(fabs(encoder.decode(poly2) - 0.1) / 0.1 < 0.000001);

                    BigPoly poly3 = encoder.encode(3.123);
                    Assert::AreEqual(poly_modulus.coeff_count(), poly3.coeff_count());
                    Assert::AreEqual(modulus.bit_count(), poly3.coeff_bit_count());
                    Assert::IsTrue(fabs(encoder.decode(poly3) - 3.123) / 3.123 < 0.000001);

                    BigPoly poly4 = encoder.encode(-123.456);
                    Assert::AreEqual(poly_modulus.coeff_count(), poly4.coeff_count());
                    Assert::AreEqual(modulus.bit_count(), poly4.coeff_bit_count());
                    Assert::IsTrue(fabs(encoder.decode(poly4) + 123.456) / (-123.456) < 0.000001);

                    BigPoly poly5 = encoder.encode(12345.98765);
                    Assert::AreEqual(poly_modulus.coeff_count(), poly5.coeff_count());
                    Assert::AreEqual(modulus.bit_count(), poly5.coeff_bit_count());
                    Assert::IsTrue(fabs(encoder.decode(poly5) - 12345.98765) / 12345.98765 < 0.000001);

                    BigPoly poly6 = encoder.encode(-0.0);
                    Assert::AreEqual(poly_modulus.coeff_count(), poly6.coeff_count());
                    Assert::AreEqual(modulus.bit_count(), poly6.coeff_bit_count());
                    Assert::AreEqual(0.0, encoder.decode(poly));

                    BigPoly poly7 = encoder.encode(0.115);
                    Assert::AreEqual(poly_modulus.coeff_count(), poly7.coeff_count());
                    Assert::AreEqual(modulus.bit_count(), poly7.coeff_bit_count());
                    Assert::IsTrue(fabs(encoder.decode(poly7) - 0.115) / 0.115 < 0.000001);
                }
            }

            {
                BigUInt modulus("100000000000000000000000000");
                for (uint64_t b = 3; b < 20; ++b)
                {
                    BalancedFractionalEncoder encoder(modulus, poly_modulus, 500, 50, b);

                    BigPoly poly = encoder.encode(0.0);
                    Assert::AreEqual(poly_modulus.coeff_count(), poly.coeff_count());
                    Assert::IsTrue(poly.is_zero());
                    Assert::AreEqual(0.0, encoder.decode(poly));

                    BigPoly poly1 = encoder.encode(-1.0);
                    Assert::AreEqual(poly_modulus.coeff_count(), poly1.coeff_count());
                    Assert::AreEqual(modulus.bit_count(), poly1.coeff_bit_count());
                    Assert::AreEqual(-1.0, encoder.decode(poly1));

                    BigPoly poly2 = encoder.encode(0.1);
                    Assert::AreEqual(poly_modulus.coeff_count(), poly2.coeff_count());
                    Assert::AreEqual(modulus.bit_count(), poly2.coeff_bit_count());
                    Assert::IsTrue(fabs(encoder.decode(poly2) - 0.1) / 0.1 < 0.000001);

                    BigPoly poly3 = encoder.encode(3.123);
                    Assert::AreEqual(poly_modulus.coeff_count(), poly3.coeff_count());
                    Assert::AreEqual(modulus.bit_count(), poly3.coeff_bit_count());
                    Assert::IsTrue(fabs(encoder.decode(poly3) - 3.123) / 3.123 < 0.000001);

                    BigPoly poly4 = encoder.encode(-123.456);
                    Assert::AreEqual(poly_modulus.coeff_count(), poly4.coeff_count());
                    Assert::AreEqual(modulus.bit_count(), poly4.coeff_bit_count());
                    Assert::IsTrue(fabs(encoder.decode(poly4) + 123.456) / (-123.456) < 0.000001);

                    BigPoly poly5 = encoder.encode(12345.98765);
                    Assert::AreEqual(poly_modulus.coeff_count(), poly5.coeff_count());
                    Assert::AreEqual(modulus.bit_count(), poly5.coeff_bit_count());
                    Assert::IsTrue(fabs(encoder.decode(poly5) - 12345.98765) / 12345.98765 < 0.000001);

                    BigPoly poly6 = encoder.encode(-0.0);
                    Assert::AreEqual(poly_modulus.coeff_count(), poly6.coeff_count());
                    Assert::AreEqual(modulus.bit_count(), poly6.coeff_bit_count());
                    Assert::AreEqual(0.0, encoder.decode(poly));

                    BigPoly poly7 = encoder.encode(0.115);
                    Assert::AreEqual(poly_modulus.coeff_count(), poly7.coeff_count());
                    Assert::AreEqual(modulus.bit_count(), poly7.coeff_bit_count());
                    Assert::IsTrue(fabs(encoder.decode(poly7) - 0.115) / 0.115 < 0.000001);
                }
            }
        }
    };
}