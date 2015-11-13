#include "CppUnitTest.h"
#include "util/common.h"
#include <cstdint>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal::util;
using namespace std;

namespace SEALTest
{
    namespace util
    {
        TEST_CLASS(Common)
        {
        public:
            TEST_METHOD(Constants)
            {
                Assert::AreEqual(4, bits_per_nibble);
                Assert::AreEqual(8, bits_per_byte);
                Assert::AreEqual(4, bytes_per_uint32);
                Assert::AreEqual(8, bytes_per_uint64);
                Assert::AreEqual(32, bits_per_uint32);
                Assert::AreEqual(64, bits_per_uint64);
                Assert::AreEqual(2, nibbles_per_byte);
                Assert::AreEqual(2, uint32_per_uint64);
                Assert::AreEqual(16, nibbles_per_uint64);
                Assert::AreEqual(static_cast<uint64_t>(INT64_MAX)+1, uint64_high_bit);
            }

            TEST_METHOD(DivideRoundUp)
            {
                Assert::AreEqual(0, divide_round_up(0, 4));
                Assert::AreEqual(1, divide_round_up(1, 4));
                Assert::AreEqual(1, divide_round_up(2, 4));
                Assert::AreEqual(1, divide_round_up(3, 4));
                Assert::AreEqual(1, divide_round_up(4, 4));
                Assert::AreEqual(2, divide_round_up(5, 4));
                Assert::AreEqual(2, divide_round_up(6, 4));
                Assert::AreEqual(2, divide_round_up(7, 4));
                Assert::AreEqual(2, divide_round_up(8, 4));
                Assert::AreEqual(3, divide_round_up(9, 4));
                Assert::AreEqual(3, divide_round_up(12, 4));
                Assert::AreEqual(4, divide_round_up(13, 4));
            }

            TEST_METHOD(GetPowerOfTwo)
            {
                Assert::AreEqual(-1, get_power_of_two(0));
                Assert::AreEqual(0, get_power_of_two(1));
                Assert::AreEqual(1, get_power_of_two(2));
                Assert::AreEqual(-1, get_power_of_two(3));
                Assert::AreEqual(2, get_power_of_two(4));
                Assert::AreEqual(-1, get_power_of_two(5));
                Assert::AreEqual(-1, get_power_of_two(6));
                Assert::AreEqual(-1, get_power_of_two(7));
                Assert::AreEqual(3, get_power_of_two(8));
                Assert::AreEqual(-1, get_power_of_two(15));
                Assert::AreEqual(4, get_power_of_two(16));
                Assert::AreEqual(-1, get_power_of_two(17));
                Assert::AreEqual(-1, get_power_of_two(255));
                Assert::AreEqual(8, get_power_of_two(256));
                Assert::AreEqual(-1, get_power_of_two(257));
                Assert::AreEqual(10, get_power_of_two(1 << 10));
                Assert::AreEqual(30, get_power_of_two(1 << 30));
                Assert::AreEqual(32, get_power_of_two(static_cast<uint64_t>(1) << 32));
                Assert::AreEqual(62, get_power_of_two(static_cast<uint64_t>(1) << 62));
                Assert::AreEqual(63, get_power_of_two(static_cast<uint64_t>(1) << 63));
            }

            TEST_METHOD(GetPowerOfTwoMinusOne)
            {
                Assert::AreEqual(0, get_power_of_two_minus_one(0));
                Assert::AreEqual(1, get_power_of_two_minus_one(1));
                Assert::AreEqual(-1, get_power_of_two_minus_one(2));
                Assert::AreEqual(2, get_power_of_two_minus_one(3));
                Assert::AreEqual(-1, get_power_of_two_minus_one(4));
                Assert::AreEqual(-1, get_power_of_two_minus_one(5));
                Assert::AreEqual(-1, get_power_of_two_minus_one(6));
                Assert::AreEqual(3, get_power_of_two_minus_one(7));
                Assert::AreEqual(-1, get_power_of_two_minus_one(8));
                Assert::AreEqual(-1, get_power_of_two_minus_one(14));
                Assert::AreEqual(4, get_power_of_two_minus_one(15));
                Assert::AreEqual(-1, get_power_of_two_minus_one(16));
                Assert::AreEqual(8, get_power_of_two_minus_one(255));
                Assert::AreEqual(10, get_power_of_two_minus_one((1 << 10) - 1));
                Assert::AreEqual(30, get_power_of_two_minus_one((1 << 30) - 1));
                Assert::AreEqual(32, get_power_of_two_minus_one((static_cast<uint64_t>(1) << 32) - 1));
                Assert::AreEqual(63, get_power_of_two_minus_one((static_cast<uint64_t>(1) << 63) - 1));
                Assert::AreEqual(64, get_power_of_two_minus_one(~static_cast<uint64_t>(0)));
            }

            TEST_METHOD(GetUInt64Byte)
            {
                uint64_t number[2];
                number[0] = 0x3456789ABCDEF121;
                number[1] = 0x23456789ABCDEF12;
                Assert::AreEqual(static_cast<uint8_t>(0x21), *get_uint64_byte(number, 0));
                Assert::AreEqual(static_cast<uint8_t>(0xF1), *get_uint64_byte(number, 1));
                Assert::AreEqual(static_cast<uint8_t>(0xDE), *get_uint64_byte(number, 2));
                Assert::AreEqual(static_cast<uint8_t>(0xBC), *get_uint64_byte(number, 3));
                Assert::AreEqual(static_cast<uint8_t>(0x9A), *get_uint64_byte(number, 4));
                Assert::AreEqual(static_cast<uint8_t>(0x78), *get_uint64_byte(number, 5));
                Assert::AreEqual(static_cast<uint8_t>(0x56), *get_uint64_byte(number, 6));
                Assert::AreEqual(static_cast<uint8_t>(0x34), *get_uint64_byte(number, 7));
                Assert::AreEqual(static_cast<uint8_t>(0x12), *get_uint64_byte(number, 8));
                Assert::AreEqual(static_cast<uint8_t>(0xEF), *get_uint64_byte(number, 9));
                Assert::AreEqual(static_cast<uint8_t>(0xCD), *get_uint64_byte(number, 10));
                Assert::AreEqual(static_cast<uint8_t>(0xAB), *get_uint64_byte(number, 11));
                Assert::AreEqual(static_cast<uint8_t>(0x89), *get_uint64_byte(number, 12));
                Assert::AreEqual(static_cast<uint8_t>(0x67), *get_uint64_byte(number, 13));
                Assert::AreEqual(static_cast<uint8_t>(0x45), *get_uint64_byte(number, 14));
                Assert::AreEqual(static_cast<uint8_t>(0x23), *get_uint64_byte(number, 15));
            }

            TEST_METHOD(GetUInt32Byte)
            {
                uint64_t number[2];
                number[0] = 0x3456789ABCDEF121;
                number[1] = 0x23456789ABCDEF12;
                Assert::AreEqual(static_cast<uint32_t>(0xBCDEF121), *get_uint64_uint32(number, 0));
                Assert::AreEqual(static_cast<uint32_t>(0x3456789A), *get_uint64_uint32(number, 1));
                Assert::AreEqual(static_cast<uint32_t>(0xABCDEF12), *get_uint64_uint32(number, 2));
                Assert::AreEqual(static_cast<uint32_t>(0x23456789), *get_uint64_uint32(number, 3));
            }

            TEST_METHOD(GetSignificantBitCount)
            {
                Assert::AreEqual(0, get_significant_bit_count(0));
                Assert::AreEqual(1, get_significant_bit_count(1));
                Assert::AreEqual(2, get_significant_bit_count(2));
                Assert::AreEqual(2, get_significant_bit_count(3));
                Assert::AreEqual(3, get_significant_bit_count(4));
                Assert::AreEqual(3, get_significant_bit_count(5));
                Assert::AreEqual(3, get_significant_bit_count(6));
                Assert::AreEqual(3, get_significant_bit_count(7));
                Assert::AreEqual(4, get_significant_bit_count(8));
                Assert::AreEqual(63, get_significant_bit_count(0x7000000000000000));
                Assert::AreEqual(63, get_significant_bit_count(0x7FFFFFFFFFFFFFFF));
                Assert::AreEqual(64, get_significant_bit_count(0x8000000000000000));
                Assert::AreEqual(64, get_significant_bit_count(0xFFFFFFFFFFFFFFFF));
            }

            TEST_METHOD(ReverseBits)
            {
                Assert::AreEqual(static_cast<uint32_t>(0), reverse_bits(static_cast<uint32_t>(0)));
                Assert::AreEqual(static_cast<uint32_t>(0x80000000), reverse_bits(static_cast<uint32_t>(1)));
                Assert::AreEqual(static_cast<uint32_t>(0x40000000), reverse_bits(static_cast<uint32_t>(2)));
                Assert::AreEqual(static_cast<uint32_t>(0xC0000000), reverse_bits(static_cast<uint32_t>(3)));
                Assert::AreEqual(static_cast<uint32_t>(0x00010000), reverse_bits(static_cast<uint32_t>(0x00008000)));
                Assert::AreEqual(static_cast<uint32_t>(0xFFFF0000), reverse_bits(static_cast<uint32_t>(0x0000FFFF)));
                Assert::AreEqual(static_cast<uint32_t>(0x0000FFFF), reverse_bits(static_cast<uint32_t>(0xFFFF0000)));
                Assert::AreEqual(static_cast<uint32_t>(0x00008000), reverse_bits(static_cast<uint32_t>(0x00010000)));
                Assert::AreEqual(static_cast<uint32_t>(3), reverse_bits(static_cast<uint32_t>(0xC0000000)));
                Assert::AreEqual(static_cast<uint32_t>(2), reverse_bits(static_cast<uint32_t>(0x40000000)));
                Assert::AreEqual(static_cast<uint32_t>(1), reverse_bits(static_cast<uint32_t>(0x80000000)));
                Assert::AreEqual(static_cast<uint32_t>(0xFFFFFFFF), reverse_bits(static_cast<uint32_t>(0xFFFFFFFF)));
            }
        };
    }
}