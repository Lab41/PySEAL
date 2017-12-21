#include "CppUnitTest.h"
#include "seal/encoder.h"
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
            SmallModulus modulus(0xFFFFFFFFFFFFFFF);
            BinaryEncoder encoder(modulus);

            BigUInt value(64);
            value = "0";
            Plaintext poly = encoder.encode(value);
            Assert::AreEqual(0, poly.significant_coeff_count());
            Assert::IsTrue(poly.is_zero());
            Assert::IsTrue(value == encoder.decode_biguint(poly));

            value = "1";
            Plaintext poly1 = encoder.encode(value);
            Assert::AreEqual(1, poly1.coeff_count());
            Assert::IsTrue("1" == poly1.to_string());
            Assert::IsTrue(value == encoder.decode_biguint(poly1));

            value = "2";
            Plaintext poly2 = encoder.encode(value);
            Assert::AreEqual(2, poly2.coeff_count());
            Assert::IsTrue("1x^1" == poly2.to_string());
            Assert::IsTrue(value == encoder.decode_biguint(poly2));

            value = "3";
            Plaintext poly3 = encoder.encode(value);
            Assert::AreEqual(2, poly3.coeff_count());
            Assert::IsTrue("1x^1 + 1" == poly3.to_string());
            Assert::IsTrue(value == encoder.decode_biguint(poly3));

            value = "FFFFFFFFFFFFFFFF";
            Plaintext poly4 = encoder.encode(value);
            Assert::AreEqual(64, poly4.coeff_count());
            for (int i = 0; i < 64; ++i)
            {
                Assert::IsTrue(poly4[i] == 1);
            }
            Assert::IsTrue(value == encoder.decode_biguint(poly4));

            value = "80F02";
            Plaintext poly5 = encoder.encode(value);
            Assert::AreEqual(20, poly5.coeff_count());
            for (int i = 0; i < 20; ++i)
            {
                if (i == 19 || (i >= 8 && i <= 11) || i == 1)
                {
                    Assert::IsTrue(poly5[i] == 1);
                }
                else
                {
                    Assert::IsTrue(poly5[i] == 0);
                }
            }
            Assert::IsTrue(value == encoder.decode_biguint(poly5));

            Plaintext poly6(3);
            poly6[0] = 1;
            poly6[1] = 500;
            poly6[2] = 1023;
            value = 1 + 500 * 2 + 1023 * 4;
            Assert::IsTrue(value == encoder.decode_biguint(poly6));

            modulus = 1024;
            BinaryEncoder encoder2(modulus);
            Plaintext poly7(4);
            poly7[0] = 1023; // -1   (*1)
            poly7[1] = 512;  // -512 (*2)
            poly7[2] = 511;  // 511  (*4)
            poly7[3] = 1;    // 1    (*8)
            value = -1 + -512 * 2 + 511 * 4 + 1 * 8;
            Assert::IsTrue(value == encoder2.decode_biguint(poly7));
        }

        TEST_METHOD(BalancedEncodeDecodeBigUInt)
        {
            SmallModulus modulus(0x10000UL);
            BalancedEncoder encoder(modulus);

            BigUInt value(64);
            value = "0";
            Plaintext poly = encoder.encode(value);
            Assert::AreEqual(0, poly.significant_coeff_count());
            Assert::IsTrue(poly.is_zero());
            Assert::IsTrue(value == encoder.decode_biguint(poly));

            value = "1";
            Plaintext poly1 = encoder.encode(value);
            Assert::AreEqual(1, poly1.significant_coeff_count());
            Assert::IsTrue("1" == poly1.to_string());
            Assert::IsTrue(value == encoder.decode_biguint(poly1));

            value = "2";
            Plaintext poly2 = encoder.encode(value);
            Assert::AreEqual(2, poly2.significant_coeff_count());
            Assert::IsTrue("1x^1 + FFFF" == poly2.to_string());
            Assert::IsTrue(value == encoder.decode_biguint(poly2));

            value = "3";
            Plaintext poly3 = encoder.encode(value);
            Assert::AreEqual(2, poly3.significant_coeff_count());
            Assert::IsTrue("1x^1" == poly3.to_string());
            Assert::IsTrue(value == encoder.decode_biguint(poly3));

            value = "2671";
            Plaintext poly4 = encoder.encode(value);
            Assert::AreEqual(9, poly4.significant_coeff_count());
            for (int i = 0; i < 9; ++i)
            {
                Assert::IsTrue(poly4[i] == 1);
            }
            Assert::IsTrue(value == encoder.decode_biguint(poly4));

            value = "D4EB";
            Plaintext poly5 = encoder.encode(value);
            Assert::AreEqual(11, poly5.significant_coeff_count());
            for (int i = 0; i < 11; ++i)
            {
                if (i % 3 == 1)
                {
                    Assert::IsTrue(poly5[i] == 1);
                }
                else if (i % 3 == 0)
                {
                    Assert::IsTrue(poly5[i] == 0);
                }
                else
                {
                    Assert::IsTrue(poly5[i] == 0xFFFF);
                }
            }
            Assert::IsTrue(value == encoder.decode_biguint(poly5));

            Plaintext poly6(3);
            poly6[0] = 1;
            poly6[1] = 500;
            poly6[2] = 1023;
            value = 1 + 500 * 3 + 1023 * 9;
            Assert::IsTrue(value == encoder.decode_biguint(poly6));

            BalancedEncoder encoder2(modulus, 7);
            Plaintext poly7(4);
            poly7[0] = 123; // 123   (*1)
            poly7[1] = 0xFFFF;  // -1 (*7)
            poly7[2] = 511;  // 511  (*49)
            poly7[3] = 1;    // 1    (*343)
            value = 123 + -1 * 7 + 511 * 49 + 1 * 343;
            Assert::IsTrue(value == encoder2.decode_biguint(poly7));

            BalancedEncoder encoder3(modulus, 6);
            Plaintext poly8(4);
            poly8[0] = 5;
            poly8[1] = 4;
            poly8[2] = 3;
            poly8[3] = 2;
            value = 5 + 4 * 6 + 3 * 36 + 2 * 216;
            Assert::IsTrue(value == encoder3.decode_biguint(poly8));

            BalancedEncoder encoder4(modulus, 10);
            Plaintext poly9(4);
            poly9[0] = 1;
            poly9[1] = 2;
            poly9[2] = 3;
            poly9[3] = 4;
            value = 4321;
            Assert::IsTrue(value == encoder4.decode_biguint(poly9));

            value = "4D2";
            Plaintext poly10 = encoder2.encode(value);
            Assert::AreEqual(5, poly10.significant_coeff_count());
            Assert::IsTrue(value == encoder2.decode_biguint(poly10));

            value = "4D2";
            Plaintext poly11 = encoder3.encode(value);
            Assert::AreEqual(5, poly11.significant_coeff_count());
            Assert::IsTrue(value == encoder3.decode_biguint(poly11));

            value = "4D2";
            Plaintext poly12 = encoder4.encode(value);
            Assert::AreEqual(4, poly12.significant_coeff_count());
            Assert::IsTrue(value == encoder4.decode_biguint(poly12));
        }

        TEST_METHOD(BinaryEncodeDecodeUInt64)
        {
            int base = 2;
            SmallModulus modulus(0xFFFFFFFFFFFFFFF);
            BinaryEncoder encoder(modulus);

            Plaintext poly = encoder.encode(static_cast<uint64_t>(0));
            Assert::AreEqual(0, poly.significant_coeff_count());
            Assert::IsTrue(poly.is_zero());
            Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(poly));

            Plaintext poly1 = encoder.encode(1ULL);
            Assert::AreEqual(1, poly1.coeff_count());
            Assert::IsTrue("1" == poly1.to_string());
            Assert::AreEqual(1ULL, encoder.decode_uint64(poly1));

            Plaintext poly2 = encoder.encode(static_cast<uint64_t>(2));
            Assert::AreEqual(2, poly2.coeff_count());
            Assert::IsTrue("1x^1" == poly2.to_string());
            Assert::AreEqual(static_cast<uint64_t>(2), encoder.decode_uint64(poly2));

            Plaintext poly3 = encoder.encode(static_cast<uint64_t>(3));
            Assert::AreEqual(2, poly3.coeff_count());
            Assert::IsTrue("1x^1 + 1" == poly3.to_string());
            Assert::AreEqual(static_cast<uint64_t>(3), encoder.decode_uint64(poly3));

            Plaintext poly4 = encoder.encode(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF));
            Assert::AreEqual(64, poly4.coeff_count());
            for (int i = 0; i < 64; ++i)
            {
                Assert::IsTrue(poly4[i] == 1);
            }
            Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), encoder.decode_uint64(poly4));

            Plaintext poly5 = encoder.encode(static_cast<uint64_t>(0x80F02));
            Assert::AreEqual(20, poly5.coeff_count());
            for (int i = 0; i < 20; ++i)
            {
                if (i == 19 || (i >= 8 && i <= 11) || i == 1)
                {
                    Assert::IsTrue(poly5[i] == 1);
                }
                else
                {
                    Assert::IsTrue(poly5[i] == 0);
                }
            }
            Assert::AreEqual(static_cast<uint64_t>(0x80F02), encoder.decode_uint64(poly5));

            Plaintext poly6(3);
            poly6[0] = 1;
            poly6[1] = 500;
            poly6[2] = 1023;
            Assert::AreEqual(static_cast<uint64_t>(1 + 500 * 2 + 1023 * 4), encoder.decode_uint64(poly6));

            modulus = 1024;
            BinaryEncoder encoder2(modulus);
            Plaintext poly7(4);
            poly7[0] = 1023; // -1   (*1)
            poly7[1] = 512;  // -512 (*2)
            poly7[2] = 511;  // 511  (*4)
            poly7[3] = 1;    // 1    (*8)
            Assert::AreEqual(static_cast<uint64_t>(-1 + -512 * 2 + 511 * 4 + 1 * 8), encoder2.decode_uint64(poly7));
        }

        TEST_METHOD(BalancedEncodeDecodeUInt64)
        {
            SmallModulus modulus(0x10000UL);
            BalancedEncoder encoder(modulus);

            Plaintext poly = encoder.encode(static_cast<uint64_t>(0));
            Assert::AreEqual(0, poly.significant_coeff_count());
            Assert::IsTrue(poly.is_zero());
            Assert::AreEqual(static_cast<uint64_t>(0), encoder.decode_uint64(poly));

            Plaintext poly1 = encoder.encode(1ULL);
            Assert::AreEqual(1, poly1.significant_coeff_count());
            Assert::IsTrue("1" == poly1.to_string());
            Assert::AreEqual(1ULL, encoder.decode_uint64(poly1));

            Plaintext poly2 = encoder.encode(static_cast<uint64_t>(2));
            Assert::AreEqual(2, poly2.significant_coeff_count());
            Assert::IsTrue("1x^1 + FFFF" == poly2.to_string());
            Assert::AreEqual(static_cast<uint64_t>(2), encoder.decode_uint64(poly2));

            Plaintext poly3 = encoder.encode(static_cast<uint64_t>(3));
            Assert::AreEqual(2, poly3.significant_coeff_count());
            Assert::IsTrue("1x^1" == poly3.to_string());
            Assert::AreEqual(static_cast<uint64_t>(3), encoder.decode_uint64(poly3));

            Plaintext poly4 = encoder.encode(static_cast<uint64_t>(0x2671));
            Assert::AreEqual(9, poly4.significant_coeff_count());
            for (int i = 0; i < 9; ++i)
            {
                Assert::IsTrue(1 == poly4[i]);
            }
            Assert::AreEqual(static_cast<uint64_t>(0x2671), encoder.decode_uint64(poly4));

            Plaintext poly5 = encoder.encode(static_cast<uint64_t>(0xD4EB));
            Assert::AreEqual(11, poly5.significant_coeff_count());
            for (int i = 0; i < 11; ++i)
            {
                if (i % 3 == 1)
                {
                    Assert::IsTrue(1 == poly5[i]);
                }
                else if (i % 3 == 0)
                {
                    Assert::IsTrue(poly5[i] == 0);
                }
                else
                {
                    Assert::IsTrue(0xFFFF == poly5[i]);
                }
            }
            Assert::AreEqual(static_cast<uint64_t>(0xD4EB), encoder.decode_uint64(poly5));

            Plaintext poly6(3);
            poly6[0] = 1;
            poly6[1] = 500;
            poly6[2] = 1023;
            Assert::AreEqual(static_cast<uint64_t>(1 + 500 * 3 + 1023 * 9), encoder.decode_uint64(poly6));

            BalancedEncoder encoder2(modulus, 7);
            Plaintext poly7(4);
            poly7[0] = 123; // 123   (*1)
            poly7[1] = 0xFFFF;  // -1 (*7)
            poly7[2] = 511;  // 511  (*49)
            poly7[3] = 1;    // 1    (*343)
            Assert::AreEqual(static_cast<uint64_t>(123 + -1 * 7 + 511 * 49 + 1 * 343), encoder2.decode_uint64(poly7));

            BalancedEncoder encoder3(modulus, 6);
            Plaintext poly8(4);
            poly8[0] = 5;
            poly8[1] = 4;
            poly8[2] = 3;
            poly8[3] = 2;
            uint64_t value = 5 + 4 * 6 + 3 * 36 + 2 * 216;
            Assert::IsTrue(value == encoder3.decode_uint64(poly8));

            BalancedEncoder encoder4(modulus, 10);
            Plaintext poly9(4);
            poly9[0] = 1;
            poly9[1] = 2;
            poly9[2] = 3;
            poly9[3] = 4;
            value = 4321;
            Assert::IsTrue(value == encoder4.decode_uint64(poly9));

            value = 1234;
            Plaintext poly10 = encoder2.encode(value);
            Assert::AreEqual(5, poly10.significant_coeff_count());
            Assert::IsTrue(value == encoder2.decode_uint64(poly10));

            value = 1234;
            Plaintext poly11 = encoder3.encode(value);
            Assert::AreEqual(5, poly11.significant_coeff_count());
            Assert::IsTrue(value == encoder3.decode_uint64(poly11));

            value = 1234;
            Plaintext poly12 = encoder4.encode(value);
            Assert::AreEqual(4, poly12.significant_coeff_count());
            Assert::IsTrue(value == encoder4.decode_uint64(poly12));
        }

        TEST_METHOD(BinaryEncodeDecodeUInt32)
        {
            SmallModulus modulus(0xFFFFFFFFFFFFFFF);
            BinaryEncoder encoder(modulus);

            Plaintext poly = encoder.encode(static_cast<uint32_t>(0));
            Assert::AreEqual(0, poly.significant_coeff_count());
            Assert::IsTrue(poly.is_zero());
            Assert::AreEqual(static_cast<uint32_t>(0), encoder.decode_uint32(poly));

            Plaintext poly1 = encoder.encode(static_cast<uint32_t>(1));
            Assert::AreEqual(1, poly1.significant_coeff_count());
            Assert::IsTrue("1" == poly1.to_string());
            Assert::AreEqual(static_cast<uint32_t>(1), encoder.decode_uint32(poly1));

            Plaintext poly2 = encoder.encode(static_cast<uint32_t>(2));
            Assert::AreEqual(2, poly2.significant_coeff_count());
            Assert::IsTrue("1x^1" == poly2.to_string());
            Assert::AreEqual(static_cast<uint32_t>(2), encoder.decode_uint32(poly2));

            Plaintext poly3 = encoder.encode(static_cast<uint32_t>(3));
            Assert::AreEqual(2, poly3.significant_coeff_count());
            Assert::IsTrue("1x^1 + 1" == poly3.to_string());
            Assert::AreEqual(static_cast<uint32_t>(3), encoder.decode_uint32(poly3));

            Plaintext poly4 = encoder.encode(static_cast<uint32_t>(0xFFFFFFFF));
            Assert::AreEqual(32, poly4.significant_coeff_count());
            for (int i = 0; i < 32; ++i)
            {
                Assert::IsTrue(1 == poly4[i]);
            }
            Assert::AreEqual(static_cast<uint32_t>(0xFFFFFFFF), encoder.decode_uint32(poly4));

            Plaintext poly5 = encoder.encode(static_cast<uint32_t>(0x80F02));
            Assert::AreEqual(20, poly5.significant_coeff_count());
            for (int i = 0; i < 20; ++i)
            {
                if (i == 19 || (i >= 8 && i <= 11) || i == 1)
                {
                    Assert::IsTrue(1 == poly5[i]);
                }
                else
                {
                    Assert::IsTrue(poly5[i] == 0);
                }
            }
            Assert::AreEqual(static_cast<uint32_t>(0x80F02), encoder.decode_uint32(poly5));

            Plaintext poly6(3);
            poly6[0] = 1;
            poly6[1] = 500;
            poly6[2] = 1023;
            Assert::AreEqual(static_cast<uint32_t>(1 + 500 * 2 + 1023 * 4), encoder.decode_uint32(poly6));

            modulus = 1024;
            BinaryEncoder encoder2(modulus);
            Plaintext poly7(4);
            poly7[0] = 1023; // -1   (*1)
            poly7[1] = 512;  // -512 (*2)
            poly7[2] = 511;  // 511  (*4)
            poly7[3] = 1;    // 1    (*8)
            Assert::AreEqual(static_cast<uint32_t>(-1 + -512 * 2 + 511 * 4 + 1 * 8), encoder2.decode_uint32(poly7));
        }

        TEST_METHOD(BalancedEncodeDecodeUInt32)
        {
            SmallModulus modulus(0x10000UL);
            BalancedEncoder encoder(modulus);

            Plaintext poly = encoder.encode(static_cast<uint32_t>(0));
            Assert::AreEqual(0, poly.significant_coeff_count());
            Assert::IsTrue(poly.is_zero());
            Assert::AreEqual(static_cast<uint32_t>(0), encoder.decode_uint32(poly));

            Plaintext poly1 = encoder.encode(static_cast<uint32_t>(1));
            Assert::AreEqual(1, poly1.significant_coeff_count());
            Assert::IsTrue("1" == poly1.to_string());
            Assert::AreEqual(static_cast<uint32_t>(1), encoder.decode_uint32(poly1));

            Plaintext poly2 = encoder.encode(static_cast<uint32_t>(2));
            Assert::AreEqual(2, poly2.significant_coeff_count());
            Assert::IsTrue("1x^1 + FFFF" == poly2.to_string());
            Assert::AreEqual(static_cast<uint32_t>(2), encoder.decode_uint32(poly2));

            Plaintext poly3 = encoder.encode(static_cast<uint32_t>(3));
            Assert::AreEqual(2, poly3.significant_coeff_count());
            Assert::IsTrue("1x^1" == poly3.to_string());
            Assert::AreEqual(static_cast<uint32_t>(3), encoder.decode_uint32(poly3));

            Plaintext poly4 = encoder.encode(static_cast<uint32_t>(0x2671));
            Assert::AreEqual(9, poly4.significant_coeff_count());
            for (int i = 0; i < 9; ++i)
            {
                Assert::IsTrue(1 == poly4[i]);
            }
            Assert::AreEqual(static_cast<uint32_t>(0x2671), encoder.decode_uint32(poly4));

            Plaintext poly5 = encoder.encode(static_cast<uint32_t>(0xD4EB));
            Assert::AreEqual(11, poly5.significant_coeff_count());
            for (int i = 0; i < 11; ++i)
            {
                if (i % 3 == 1)
                {
                    Assert::IsTrue(1 == poly5[i]);
                }
                else if (i % 3 == 0)
                {
                    Assert::IsTrue(poly5[i] == 0);
                }
                else
                {
                    Assert::IsTrue(0xFFFF == poly5[i]);
                }
            }
            Assert::AreEqual(static_cast<uint32_t>(0xD4EB), encoder.decode_uint32(poly5));

            Plaintext poly6(3);
            poly6[0] = 1;
            poly6[1] = 500;
            poly6[2] = 1023;
            Assert::AreEqual(static_cast<uint32_t>(1 + 500 * 3 + 1023 * 9), encoder.decode_uint32(poly6));

            BalancedEncoder encoder2(modulus, 7);
            Plaintext poly7(4);
            poly7[0] = 123; // 123   (*1)
            poly7[1] = 0xFFFF;  // -1 (*7)
            poly7[2] = 511;  // 511  (*49)
            poly7[3] = 1;    // 1    (*343)
            Assert::AreEqual(static_cast<uint32_t>(123 + -1 * 7 + 511 * 49 + 1 * 343), encoder2.decode_uint32(poly7));

            BalancedEncoder encoder3(modulus, 6);
            Plaintext poly8(4);
            poly8[0] = 5;
            poly8[1] = 4;
            poly8[2] = 3;
            poly8[3] = 2;
            uint64_t value = 5 + 4 * 6 + 3 * 36 + 2 * 216;
            Assert::IsTrue(value == encoder3.decode_uint32(poly8));

            BalancedEncoder encoder4(modulus, 10);
            Plaintext poly9(4);
            poly9[0] = 1;
            poly9[1] = 2;
            poly9[2] = 3;
            poly9[3] = 4;
            value = 4321;
            Assert::IsTrue(value == encoder4.decode_uint32(poly9));

            value = 1234;
            Plaintext poly10 = encoder2.encode(value);
            Assert::AreEqual(5, poly10.significant_coeff_count());
            Assert::IsTrue(value == encoder2.decode_uint32(poly10));

            value = 1234;
            Plaintext poly11 = encoder3.encode(value);
            Assert::AreEqual(5, poly11.significant_coeff_count());
            Assert::IsTrue(value == encoder3.decode_uint32(poly11));

            value = 1234;
            Plaintext poly12 = encoder4.encode(value);
            Assert::AreEqual(4, poly12.significant_coeff_count());
            Assert::IsTrue(value == encoder4.decode_uint32(poly12));
        }

        TEST_METHOD(BinaryEncodeDecodeInt64)
        {
            SmallModulus modulus(0x7FFFFFFFFFFFF);
            BinaryEncoder encoder(modulus);

            Plaintext poly = encoder.encode(static_cast<int64_t>(0));
            Assert::AreEqual(0, poly.significant_coeff_count());
            Assert::IsTrue(poly.is_zero());
            Assert::AreEqual(static_cast<uint64_t>(0), static_cast<uint64_t>(encoder.decode_int64(poly)));

            Plaintext poly1 = encoder.encode(static_cast<int64_t>(1));
            Assert::AreEqual(1, poly1.significant_coeff_count());
            Assert::IsTrue("1" == poly1.to_string());
            Assert::AreEqual(1ULL, static_cast<uint64_t>(encoder.decode_int64(poly1)));

            Plaintext poly2 = encoder.encode(static_cast<int64_t>(2));
            Assert::AreEqual(2, poly2.significant_coeff_count());
            Assert::IsTrue("1x^1" == poly2.to_string());
            Assert::AreEqual(static_cast<uint64_t>(2), static_cast<uint64_t>(encoder.decode_int64(poly2)));

            Plaintext poly3 = encoder.encode(static_cast<int64_t>(3));
            Assert::AreEqual(2, poly3.significant_coeff_count());
            Assert::IsTrue("1x^1 + 1" == poly3.to_string());
            Assert::AreEqual(static_cast<uint64_t>(3), static_cast<uint64_t>(encoder.decode_int64(poly3)));

            Plaintext poly4 = encoder.encode(static_cast<int64_t>(-1));
            Assert::AreEqual(1, poly4.significant_coeff_count());
            Assert::IsTrue("7FFFFFFFFFFFE" == poly4.to_string());
            Assert::AreEqual(static_cast<uint64_t>(-1), static_cast<uint64_t>(encoder.decode_int64(poly4)));

            Plaintext poly5 = encoder.encode(static_cast<int64_t>(-2));
            Assert::AreEqual(2, poly5.significant_coeff_count());
            Assert::IsTrue("7FFFFFFFFFFFEx^1" == poly5.to_string());
            Assert::AreEqual(static_cast<uint64_t>(-2), static_cast<uint64_t>(encoder.decode_int64(poly5)));

            Plaintext poly6 = encoder.encode(static_cast<int64_t>(-3));
            Assert::AreEqual(2, poly6.significant_coeff_count());
            Assert::IsTrue("7FFFFFFFFFFFEx^1 + 7FFFFFFFFFFFE" == poly6.to_string());
            Assert::AreEqual(static_cast<uint64_t>(-3), static_cast<uint64_t>(encoder.decode_int64(poly6)));

            Plaintext poly7 = encoder.encode(static_cast<int64_t>(0x7FFFFFFFFFFFF));
            Assert::AreEqual(51, poly7.significant_coeff_count());
            for (int i = 0; i < 51; ++i)
            {
                Assert::IsTrue(1 == poly7[i]);
            }
            Assert::AreEqual(static_cast<uint64_t>(0x7FFFFFFFFFFFF), static_cast<uint64_t>(encoder.decode_int64(poly7)));

            Plaintext poly8 = encoder.encode(static_cast<int64_t>(0x8000000000000));
            Assert::AreEqual(52, poly8.significant_coeff_count());
            Assert::IsTrue(poly8[51] == 1);
            for (int i = 0; i < 51; ++i)
            {
                Assert::IsTrue(poly8[i] == 0);
            }
            Assert::AreEqual(static_cast<uint64_t>(0x8000000000000), static_cast<uint64_t>(encoder.decode_int64(poly8)));

            Plaintext poly9 = encoder.encode(static_cast<int64_t>(0x80F02));
            Assert::AreEqual(20, poly9.significant_coeff_count());
            for (int i = 0; i < 20; ++i)
            {
                if (i == 19 || (i >= 8 && i <= 11) || i == 1)
                {
                    Assert::IsTrue(1 == poly9[i]);
                }
                else
                {
                    Assert::IsTrue(poly9[i] == 0);
                }
            }
            Assert::AreEqual(static_cast<uint64_t>(0x80F02), static_cast<uint64_t>(encoder.decode_int64(poly9)));

            Plaintext poly10 = encoder.encode(static_cast<int64_t>(-1073));
            Assert::AreEqual(11, poly10.significant_coeff_count());
            Assert::IsTrue(0x7FFFFFFFFFFFE == poly10[10]);
            Assert::IsTrue(poly10[9] == 0);
            Assert::IsTrue(poly10[8] == 0);
            Assert::IsTrue(poly10[7] == 0);
            Assert::IsTrue(poly10[6] == 0);
            Assert::IsTrue(0x7FFFFFFFFFFFE == poly10[5]);
            Assert::IsTrue(0x7FFFFFFFFFFFE == poly10[4]);
            Assert::IsTrue(poly10[3] == 0);
            Assert::IsTrue(poly10[2] == 0);
            Assert::IsTrue(poly10[1] == 0);
            Assert::IsTrue(0x7FFFFFFFFFFFE == poly10[0]);
            Assert::AreEqual(static_cast<uint64_t>(-1073), static_cast<uint64_t>(encoder.decode_int64(poly10)));

            modulus = 0xFFFF;
            BinaryEncoder encoder2(modulus);
            Plaintext poly11(6);
            poly11[0] = 1;
            poly11[1] = 0xFFFE; // -1
            poly11[2] = 0xFFFD; // -2
            poly11[3] = 0x8000; // -32767
            poly11[4] = 0x7FFF; // 32767
            poly11[5] = 0x7FFE; // 32766
            Assert::AreEqual(static_cast<uint64_t>(1 + -1 * 2 + -2 * 4 + -32767 * 8 + 32767 * 16 + 32766 * 32), static_cast<uint64_t>(encoder2.decode_int64(poly11)));
        }

        TEST_METHOD(BalancedEncodeDecodeInt64)
        {
            SmallModulus modulus(0x10000UL);
            BalancedEncoder encoder(modulus);

            Plaintext poly = encoder.encode(static_cast<int64_t>(0));
            Assert::AreEqual(0, poly.significant_coeff_count());
            Assert::IsTrue(poly.is_zero());
            Assert::AreEqual(static_cast<uint64_t>(0), static_cast<uint64_t>(encoder.decode_int64(poly)));

            Plaintext poly1 = encoder.encode(static_cast<int64_t>(1));
            Assert::AreEqual(1, poly1.significant_coeff_count());
            Assert::IsTrue("1" == poly1.to_string());
            Assert::AreEqual(1ULL, static_cast<uint64_t>(encoder.decode_int64(poly1)));

            Plaintext poly2 = encoder.encode(static_cast<int64_t>(2));
            Assert::AreEqual(2, poly2.significant_coeff_count());
            Assert::IsTrue("1x^1 + FFFF" == poly2.to_string());
            Assert::AreEqual(static_cast<uint64_t>(2), static_cast<uint64_t>(encoder.decode_int64(poly2)));

            Plaintext poly3 = encoder.encode(static_cast<int64_t>(3));
            Assert::AreEqual(2, poly3.significant_coeff_count());
            Assert::IsTrue("1x^1" == poly3.to_string());
            Assert::AreEqual(static_cast<uint64_t>(3), static_cast<uint64_t>(encoder.decode_int64(poly3)));

            Plaintext poly4 = encoder.encode(static_cast<int64_t>(-1));
            Assert::AreEqual(1, poly4.significant_coeff_count());
            Assert::IsTrue("FFFF" == poly4.to_string());
            Assert::AreEqual(static_cast<uint64_t>(-1), static_cast<uint64_t>(encoder.decode_int64(poly4)));

            Plaintext poly5 = encoder.encode(static_cast<int64_t>(-2));
            Assert::AreEqual(2, poly5.significant_coeff_count());
            Assert::IsTrue("FFFFx^1 + 1" == poly5.to_string());
            Assert::AreEqual(static_cast<uint64_t>(-2), static_cast<uint64_t>(encoder.decode_int64(poly5)));

            Plaintext poly6 = encoder.encode(static_cast<int64_t>(-3));
            Assert::AreEqual(2, poly6.significant_coeff_count());
            Assert::IsTrue("FFFFx^1" == poly6.to_string());
            Assert::AreEqual(static_cast<uint64_t>(-3), static_cast<uint64_t>(encoder.decode_int64(poly6)));

            Plaintext poly7 = encoder.encode(static_cast<int64_t>(-0x2671));
            Assert::AreEqual(9, poly7.significant_coeff_count());
            for (int i = 0; i < 9; ++i)
            {
                Assert::IsTrue(0xFFFF == poly7[i]);
            }
            Assert::AreEqual(static_cast<uint64_t>(-0x2671), static_cast<uint64_t>(encoder.decode_int64(poly7)));

            Plaintext poly8 = encoder.encode(static_cast<int64_t>(-4374));
            Assert::AreEqual(9, poly8.significant_coeff_count());
            Assert::IsTrue(0xFFFF == poly8[8]);
            Assert::IsTrue(1 == poly8[7]);
            for (int i = 0; i < 7; ++i)
            {
                Assert::IsTrue(poly8[i] == 0);
            }
            Assert::AreEqual(static_cast<uint64_t>(-4374), static_cast<uint64_t>(encoder.decode_int64(poly8)));

            Plaintext poly9 = encoder.encode(static_cast<int64_t>(-0xD4EB));
            Assert::AreEqual(11, poly9.significant_coeff_count());
            for (int i = 0; i < 11; ++i)
            {
                if (i % 3 == 1)
                {
                    Assert::IsTrue(0xFFFF == poly9[i]);
                }
                else if (i % 3 == 0)
                {
                    Assert::IsTrue(poly9[i] == 0);
                }
                else
                {
                    Assert::IsTrue(1 == poly9[i]);
                }
            }
            Assert::AreEqual(static_cast<uint64_t>(-0xD4EB), static_cast<uint64_t>(encoder.decode_int64(poly9)));

            Plaintext poly10 = encoder.encode(static_cast<int64_t>(-30724));
            Assert::AreEqual(11, poly10.significant_coeff_count());
            Assert::IsTrue(0xFFFF == poly10[10]);
            Assert::IsTrue(1 == poly10[9]);
            Assert::IsTrue(1 == poly10[8]);
            Assert::IsTrue(1 == poly10[7]);
            Assert::IsTrue(poly10[6] == 0);
            Assert::IsTrue(poly10[5] == 0);
            Assert::IsTrue(0xFFFF == poly10[4]);
            Assert::IsTrue(0xFFFF == poly10[3]);
            Assert::IsTrue(poly10[2] == 0);
            Assert::IsTrue(1 == poly10[1]);
            Assert::IsTrue(0xFFFF == poly10[0]);
            Assert::AreEqual(static_cast<uint64_t>(-30724), static_cast<uint64_t>(encoder.decode_int64(poly10)));

            BalancedEncoder encoder2(modulus, 13);
            Plaintext poly11 = encoder2.encode(static_cast<int64_t>(-126375543984));
            Assert::AreEqual(11, poly11.significant_coeff_count());
            Assert::IsTrue(0xFFFF == poly11[10]);
            Assert::IsTrue(1 == poly11[9]);
            Assert::IsTrue(1 == poly11[8]);
            Assert::IsTrue(1 == poly11[7]);
            Assert::IsTrue(poly11[6] == 0);
            Assert::IsTrue(poly11[5] == 0);
            Assert::IsTrue(0xFFFF == poly11[4]);
            Assert::IsTrue(0xFFFF == poly11[3]);
            Assert::IsTrue(poly11[2] == 0);
            Assert::IsTrue(1 == poly11[1]);
            Assert::IsTrue(0xFFFF == poly11[0]);
            Assert::AreEqual(static_cast<uint64_t>(-126375543984), static_cast<uint64_t>(encoder2.decode_int64(poly11)));

            modulus = 0xFFFFUL;
            BalancedEncoder encoder3(modulus, 7);
            Plaintext poly12(6);
            poly12[0] = 1;
            poly12[1] = 0xFFFE; // -1
            poly12[2] = 0xFFFD; // -2
            poly12[3] = 0x8000; // -32767
            poly12[4] = 0x7FFF; // 32767
            poly12[5] = 0x7FFE; // 32766
            Assert::AreEqual(static_cast<uint64_t>(1 + -1 * 7 + -2 * 49 + -32767 * 343 + 32767 * 2401 + 32766 * 16807), static_cast<uint64_t>(encoder3.decode_int64(poly12)));
        
            BalancedEncoder encoder4(modulus, 6);
            poly8.resize(4);
            poly8[0] = 5;
            poly8[1] = 4;
            poly8[2] = 3;
            poly8[3] = *modulus.pointer() - 2;
            int64_t value = 5 + 4 * 6 + 3 * 36 - 2 * 216;
            Assert::IsTrue(value == encoder4.decode_int64(poly8));

            BalancedEncoder encoder5(modulus, 10);
            poly9.resize(4);
            poly9[0] = 1;
            poly9[1] = 2;
            poly9[2] = 3;
            poly9[3] = 4;
            value = 4321;
            Assert::IsTrue(value == encoder5.decode_int64(poly9));

            value = -1234;
            poly10 = encoder3.encode(value);
            Assert::AreEqual(5, poly10.significant_coeff_count());
            Assert::IsTrue(value == encoder3.decode_int64(poly10));

            value = -1234;
            poly11 = encoder4.encode(value);
            Assert::AreEqual(5, poly11.significant_coeff_count());
            Assert::IsTrue(value == encoder4.decode_int64(poly11));

            value = -1234;
            poly12 = encoder5.encode(value);
            Assert::AreEqual(4, poly12.significant_coeff_count());
            Assert::IsTrue(value == encoder5.decode_int64(poly12));
        }

        TEST_METHOD(EncodeDecodeInt32)
        {
            SmallModulus modulus(0x7FFFFFFFFFFFFF);
            BinaryEncoder encoder(modulus);

            Plaintext poly = encoder.encode(static_cast<int32_t>(0));
            Assert::AreEqual(0, poly.significant_coeff_count());
            Assert::IsTrue(poly.is_zero());
            Assert::AreEqual(static_cast<int32_t>(0), encoder.decode_int32(poly));

            Plaintext poly1 = encoder.encode(static_cast<int32_t>(1));
            Assert::AreEqual(1, poly1.significant_coeff_count());
            Assert::IsTrue("1" == poly1.to_string());
            Assert::AreEqual(static_cast<int32_t>(1), encoder.decode_int32(poly1));

            Plaintext poly2 = encoder.encode(static_cast<int32_t>(2));
            Assert::AreEqual(2, poly2.significant_coeff_count());
            Assert::IsTrue("1x^1" == poly2.to_string());
            Assert::AreEqual(static_cast<int32_t>(2), encoder.decode_int32(poly2));

            Plaintext poly3 = encoder.encode(static_cast<int32_t>(3));
            Assert::AreEqual(2, poly3.significant_coeff_count());
            Assert::IsTrue("1x^1 + 1" == poly3.to_string());
            Assert::AreEqual(static_cast<int32_t>(3), encoder.decode_int32(poly3));

            Plaintext poly4 = encoder.encode(static_cast<int32_t>(-1));
            Assert::AreEqual(1, poly4.significant_coeff_count());
            Assert::IsTrue("7FFFFFFFFFFFFE" == poly4.to_string());
            Assert::AreEqual(static_cast<int32_t>(-1), encoder.decode_int32(poly4));

            Plaintext poly5 = encoder.encode(static_cast<int32_t>(-2));
            Assert::AreEqual(2, poly5.significant_coeff_count());
            Assert::IsTrue("7FFFFFFFFFFFFEx^1" == poly5.to_string());
            Assert::AreEqual(static_cast<int32_t>(-2), encoder.decode_int32(poly5));

            Plaintext poly6 = encoder.encode(static_cast<int32_t>(-3));
            Assert::AreEqual(2, poly6.significant_coeff_count());
            Assert::IsTrue("7FFFFFFFFFFFFEx^1 + 7FFFFFFFFFFFFE" == poly6.to_string());
            Assert::AreEqual(static_cast<int32_t>(-3), encoder.decode_int32(poly6));

            Plaintext poly7 = encoder.encode(static_cast<int32_t>(0x7FFFFFFF));
            Assert::AreEqual(31, poly7.significant_coeff_count());
            for (int i = 0; i < 31; ++i)
            {
                Assert::IsTrue(1 == poly7[i]);
            }
            Assert::AreEqual(static_cast<int32_t>(0x7FFFFFFF), encoder.decode_int32(poly7));

            Plaintext poly8 = encoder.encode(static_cast<int32_t>(0x80000000));
            Assert::AreEqual(32, poly8.significant_coeff_count());
            Assert::IsTrue(0x7FFFFFFFFFFFFE == poly8[31]);
            for (int i = 0; i < 31; ++i)
            {
                Assert::IsTrue(poly8[i] == 0);
            }
            Assert::AreEqual(static_cast<int32_t>(0x80000000), encoder.decode_int32(poly8));

            Plaintext poly9 = encoder.encode(static_cast<int32_t>(0x80F02));
            Assert::AreEqual(20, poly9.significant_coeff_count());
            for (int i = 0; i < 20; ++i)
            {
                if (i == 19 || (i >= 8 && i <= 11) || i == 1)
                {
                    Assert::IsTrue(1 == poly9[i]);
                }
                else
                {
                    Assert::IsTrue(poly9[i] == 0);
                }
            }
            Assert::AreEqual(static_cast<int32_t>(0x80F02), encoder.decode_int32(poly9));

            Plaintext poly10 = encoder.encode(static_cast<int32_t>(-1073));
            Assert::AreEqual(11, poly10.significant_coeff_count());
            Assert::IsTrue(0x7FFFFFFFFFFFFE == poly10[10]);
            Assert::IsTrue(poly10[9] == 0);
            Assert::IsTrue(poly10[8] == 0);
            Assert::IsTrue(poly10[7] == 0);
            Assert::IsTrue(poly10[6] == 0);
            Assert::IsTrue(0x7FFFFFFFFFFFFE == poly10[5]);
            Assert::IsTrue(0x7FFFFFFFFFFFFE == poly10[4]);
            Assert::IsTrue(poly10[3] == 0);
            Assert::IsTrue(poly10[2] == 0);
            Assert::IsTrue(poly10[1] == 0);
            Assert::IsTrue(0x7FFFFFFFFFFFFE == poly10[0]);
            Assert::AreEqual(static_cast<int32_t>(-1073), encoder.decode_int32(poly10));

            modulus = 0xFFFF;
            BinaryEncoder encoder2(modulus);
            Plaintext poly11(6);
            poly11[0] = 1;
            poly11[1] = 0xFFFE; // -1
            poly11[2] = 0xFFFD; // -2
            poly11[3] = 0x8000; // -32767
            poly11[4] = 0x7FFF; // 32767
            poly11[5] = 0x7FFE; // 32766
            Assert::AreEqual(static_cast<int32_t>(1 + -1 * 2 + -2 * 4 + -32767 * 8 + 32767 * 16 + 32766 * 32), encoder2.decode_int32(poly11));
        }

        TEST_METHOD(BalancedEncodeDecodeInt32)
        {
            SmallModulus modulus(0x10000UL);
            BalancedEncoder encoder(modulus);

            Plaintext poly = encoder.encode(static_cast<int32_t>(0));
            Assert::AreEqual(0, poly.significant_coeff_count());
            Assert::IsTrue(poly.is_zero());
            Assert::AreEqual(static_cast<int32_t>(0), encoder.decode_int32(poly));

            Plaintext poly1 = encoder.encode(static_cast<int32_t>(1));
            Assert::AreEqual(1, poly1.significant_coeff_count());
            Assert::IsTrue("1" == poly1.to_string());
            Assert::AreEqual(static_cast<int32_t>(1), encoder.decode_int32(poly1));

            Plaintext poly2 = encoder.encode(static_cast<int32_t>(2));
            Assert::AreEqual(2, poly2.significant_coeff_count());
            Assert::IsTrue("1x^1 + FFFF" == poly2.to_string());
            Assert::AreEqual(static_cast<int32_t>(2), encoder.decode_int32(poly2));

            Plaintext poly3 = encoder.encode(static_cast<int32_t>(3));
            Assert::AreEqual(2, poly3.significant_coeff_count());
            Assert::IsTrue("1x^1" == poly3.to_string());
            Assert::AreEqual(static_cast<int32_t>(3), encoder.decode_int32(poly3));

            Plaintext poly4 = encoder.encode(static_cast<int32_t>(-1));
            Assert::AreEqual(1, poly4.significant_coeff_count());
            Assert::IsTrue("FFFF" == poly4.to_string());
            Assert::AreEqual(static_cast<int32_t>(-1), encoder.decode_int32(poly4));

            Plaintext poly5 = encoder.encode(static_cast<int32_t>(-2));
            Assert::AreEqual(2, poly5.significant_coeff_count());
            Assert::IsTrue("FFFFx^1 + 1" == poly5.to_string());
            Assert::AreEqual(static_cast<int32_t>(-2), encoder.decode_int32(poly5));

            Plaintext poly6 = encoder.encode(static_cast<int32_t>(-3));
            Assert::AreEqual(2, poly6.significant_coeff_count());
            Assert::IsTrue("FFFFx^1" == poly6.to_string());
            Assert::AreEqual(static_cast<int32_t>(-3), encoder.decode_int32(poly6));

            Plaintext poly7 = encoder.encode(static_cast<int32_t>(-0x2671));
            Assert::AreEqual(9, poly7.significant_coeff_count());
            for (int i = 0; i < 9; ++i)
            {
                Assert::IsTrue(0xFFFF == poly7[i]);
            }
            Assert::AreEqual(static_cast<int32_t>(-0x2671), encoder.decode_int32(poly7));

            Plaintext poly8 = encoder.encode(static_cast<int32_t>(-4374));
            Assert::AreEqual(9, poly8.significant_coeff_count());
            Assert::IsTrue(0xFFFF == poly8[8]);
            Assert::IsTrue(1 == poly8[7]);
            for (int i = 0; i < 7; ++i)
            {
                Assert::IsTrue(poly8[i] == 0);
            }
            Assert::AreEqual(static_cast<int32_t>(-4374), encoder.decode_int32(poly8));

            Plaintext poly9 = encoder.encode(static_cast<int32_t>(-0xD4EB));
            Assert::AreEqual(11, poly9.significant_coeff_count());
            for (int i = 0; i < 11; ++i)
            {
                if (i % 3 == 1)
                {
                    Assert::IsTrue(0xFFFF == poly9[i]);
                }
                else if (i % 3 == 0)
                {
                    Assert::IsTrue(poly9[i] == 0);
                }
                else
                {
                    Assert::IsTrue(1 == poly9[i]);
                }
            }
            Assert::AreEqual(static_cast<int32_t>(-0xD4EB), encoder.decode_int32(poly9));

            Plaintext poly10 = encoder.encode(static_cast<int32_t>(-30724));
            Assert::AreEqual(11, poly10.significant_coeff_count());
            Assert::IsTrue(0xFFFF == poly10[10]);
            Assert::IsTrue(1 == poly10[9]);
            Assert::IsTrue(1 == poly10[8]);
            Assert::IsTrue(1 == poly10[7]);
            Assert::IsTrue(poly10[6] == 0);
            Assert::IsTrue(poly10[5] == 0);
            Assert::IsTrue(0xFFFF == poly10[4]);
            Assert::IsTrue(0xFFFF == poly10[3]);
            Assert::IsTrue(poly10[2] == 0);
            Assert::IsTrue(1 == poly10[1]);
            Assert::IsTrue(0xFFFF == poly10[0]);
            Assert::AreEqual(static_cast<int32_t>(-30724), encoder.decode_int32(poly10));

            modulus = 0xFFFFUL;
            BalancedEncoder encoder2(modulus, 7);
            Plaintext poly12(6);
            poly12[0] = 1;
            poly12[1] = 0xFFFE; // -1
            poly12[2] = 0xFFFD; // -2
            poly12[3] = 0x8000; // -32767
            poly12[4] = 0x7FFF; // 32767
            poly12[5] = 0x7FFE; // 32766
            Assert::AreEqual(static_cast<int32_t>(1 + -1 * 7 + -2 * 49 + -32767 * 343 + 32767 * 2401 + 32766 * 16807), encoder2.decode_int32(poly12));
        
            BalancedEncoder encoder4(modulus, 6);
            poly8.resize(4);
            poly8[0] = 5;
            poly8[1] = 4;
            poly8[2] = 3;
            poly8[3] = *modulus.pointer() - 2;
            int32_t value = 5 + 4 * 6 + 3 * 36 - 2 * 216;
            Assert::IsTrue(value == encoder4.decode_int32(poly8));

            BalancedEncoder encoder5(modulus, 10);
            poly9.resize(4);
            poly9[0] = 1;
            poly9[1] = 2;
            poly9[2] = 3;
            poly9[3] = 4;
            value = 4321;
            Assert::IsTrue(value == encoder5.decode_int32(poly9));

            value = -1234;
            poly10 = encoder2.encode(value);
            Assert::AreEqual(5, poly10.significant_coeff_count());
            Assert::IsTrue(value == encoder2.decode_int32(poly10));

            value = -1234;
            Plaintext poly11 = encoder4.encode(value);
            Assert::AreEqual(5, poly11.significant_coeff_count());
            Assert::IsTrue(value == encoder4.decode_int32(poly11));

            value = -1234;
            poly12 = encoder5.encode(value);
            Assert::AreEqual(4, poly12.significant_coeff_count());
            Assert::IsTrue(value == encoder5.decode_int32(poly12));
        }

        TEST_METHOD(BinaryFractionalEncodeDecode)
        {
            BigPoly poly_modulus("1x^1024 + 1");
            SmallModulus modulus(0x10000UL);
            BinaryFractionalEncoder encoder(modulus, poly_modulus, 500, 50);

            Plaintext poly = encoder.encode(0.0);
            Assert::IsTrue(poly.is_zero());
            Assert::AreEqual(0.0, encoder.decode(poly));

            Plaintext poly1 = encoder.encode(-1.0);
            Assert::AreEqual(-1.0, encoder.decode(poly1));

            Plaintext poly2 = encoder.encode(0.1);
            Assert::IsTrue(fabs(encoder.decode(poly2) - 0.1) / 0.1 < 0.000001);

            Plaintext poly3 = encoder.encode(3.123);
            Assert::IsTrue(fabs(encoder.decode(poly3) - 3.123) / 3.123 < 0.000001);

            Plaintext poly4 = encoder.encode(-123.456);
            Assert::IsTrue(fabs(encoder.decode(poly4) + 123.456) / (-123.456) < 0.000001);

            Plaintext poly5 = encoder.encode(12345.98765);
            Assert::IsTrue(fabs(encoder.decode(poly5) - 12345.98765) / 12345.98765 < 0.000001);
        }

        TEST_METHOD(BalancedFractionalEncodeDecode)
        {
            BigPoly poly_modulus("1x^1024 + 1");
            {
                SmallModulus modulus(0x10000UL);
                for (uint64_t b = 3; b < 20; ++b)
                {
                    BalancedFractionalEncoder encoder(modulus, poly_modulus, 500, 50, b);

                    Plaintext poly = encoder.encode(0.0);
                    Assert::IsTrue(poly.is_zero());
                    Assert::AreEqual(0.0, encoder.decode(poly));

                    Plaintext poly1 = encoder.encode(-1.0);
                    Assert::AreEqual(-1.0, encoder.decode(poly1));

                    Plaintext poly2 = encoder.encode(0.1);
                    Assert::IsTrue(fabs(encoder.decode(poly2) - 0.1) / 0.1 < 0.000001);

                    Plaintext poly3 = encoder.encode(3.123);
                    Assert::IsTrue(fabs(encoder.decode(poly3) - 3.123) / 3.123 < 0.000001);

                    Plaintext poly4 = encoder.encode(-123.456);
                    Assert::IsTrue(fabs(encoder.decode(poly4) + 123.456) / (-123.456) < 0.000001);

                    Plaintext poly5 = encoder.encode(12345.98765);
                    Assert::IsTrue(fabs(encoder.decode(poly5) - 12345.98765) / 12345.98765 < 0.000001);

                    Plaintext poly6 = encoder.encode(-0.0);
                    Assert::AreEqual(0.0, encoder.decode(poly));

                    Plaintext poly7 = encoder.encode(0.115);
                    Assert::IsTrue(fabs(encoder.decode(poly7) - 0.115) / 0.115 < 0.000001);
                }
            }

            {
                SmallModulus modulus(0x100000000000);
                for (uint64_t b = 3; b < 20; ++b)
                {
                    BalancedFractionalEncoder encoder(modulus, poly_modulus, 500, 50, b);

                    Plaintext poly = encoder.encode(0.0);
                    Assert::IsTrue(poly.is_zero());
                    Assert::AreEqual(0.0, encoder.decode(poly));

                    Plaintext poly1 = encoder.encode(-1.0);
                    Assert::AreEqual(-1.0, encoder.decode(poly1));

                    Plaintext poly2 = encoder.encode(0.1);
                    Assert::IsTrue(fabs(encoder.decode(poly2) - 0.1) / 0.1 < 0.000001);

                    Plaintext poly3 = encoder.encode(3.123);
                    Assert::IsTrue(fabs(encoder.decode(poly3) - 3.123) / 3.123 < 0.000001);

                    Plaintext poly4 = encoder.encode(-123.456);
                    Assert::IsTrue(fabs(encoder.decode(poly4) + 123.456) / (-123.456) < 0.000001);

                    Plaintext poly5 = encoder.encode(12345.98765);
                    Assert::IsTrue(fabs(encoder.decode(poly5) - 12345.98765) / 12345.98765 < 0.000001);

                    Plaintext poly6 = encoder.encode(-0.0);
                    Assert::AreEqual(0.0, encoder.decode(poly));

                    Plaintext poly7 = encoder.encode(0.115);
                    Assert::IsTrue(fabs(encoder.decode(poly7) - 0.115) / 0.115 < 0.000001);
                }
            }
        }
    };
}