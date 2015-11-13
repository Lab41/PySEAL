#include "CppUnitTest.h"
#include "util/common.h"
#include <cstdint>
#include <cstring>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal::util;
using namespace std;

namespace SEALTest
{
    namespace util
    {
        TEST_CLASS(StringToUInt64)
        {
        public:
            TEST_METHOD(IsHexCharTest)
            {
                Assert::IsTrue(is_hex_char('0'));
                Assert::IsTrue(is_hex_char('1'));
                Assert::IsTrue(is_hex_char('2'));
                Assert::IsTrue(is_hex_char('3'));
                Assert::IsTrue(is_hex_char('4'));
                Assert::IsTrue(is_hex_char('5'));
                Assert::IsTrue(is_hex_char('6'));
                Assert::IsTrue(is_hex_char('7'));
                Assert::IsTrue(is_hex_char('8'));
                Assert::IsTrue(is_hex_char('9'));
                Assert::IsTrue(is_hex_char('A'));
                Assert::IsTrue(is_hex_char('B'));
                Assert::IsTrue(is_hex_char('C'));
                Assert::IsTrue(is_hex_char('D'));
                Assert::IsTrue(is_hex_char('E'));
                Assert::IsTrue(is_hex_char('F'));
                Assert::IsTrue(is_hex_char('a'));
                Assert::IsTrue(is_hex_char('b'));
                Assert::IsTrue(is_hex_char('c'));
                Assert::IsTrue(is_hex_char('d'));
                Assert::IsTrue(is_hex_char('e'));
                Assert::IsTrue(is_hex_char('f'));

                Assert::IsFalse(is_hex_char('/'));
                Assert::IsFalse(is_hex_char(' '));
                Assert::IsFalse(is_hex_char('+'));
                Assert::IsFalse(is_hex_char('\\'));
                Assert::IsFalse(is_hex_char('G'));
                Assert::IsFalse(is_hex_char('g'));
                Assert::IsFalse(is_hex_char('Z'));
                Assert::IsFalse(is_hex_char('Z'));
            }

            TEST_METHOD(HexToNibbleTest)
            {
                Assert::AreEqual(0, hex_to_nibble('0'));
                Assert::AreEqual(1, hex_to_nibble('1'));
                Assert::AreEqual(2, hex_to_nibble('2'));
                Assert::AreEqual(3, hex_to_nibble('3'));
                Assert::AreEqual(4, hex_to_nibble('4'));
                Assert::AreEqual(5, hex_to_nibble('5'));
                Assert::AreEqual(6, hex_to_nibble('6'));
                Assert::AreEqual(7, hex_to_nibble('7'));
                Assert::AreEqual(8, hex_to_nibble('8'));
                Assert::AreEqual(9, hex_to_nibble('9'));
                Assert::AreEqual(10, hex_to_nibble('A'));
                Assert::AreEqual(11, hex_to_nibble('B'));
                Assert::AreEqual(12, hex_to_nibble('C'));
                Assert::AreEqual(13, hex_to_nibble('D'));
                Assert::AreEqual(14, hex_to_nibble('E'));
                Assert::AreEqual(15, hex_to_nibble('F'));
                Assert::AreEqual(10, hex_to_nibble('a'));
                Assert::AreEqual(11, hex_to_nibble('b'));
                Assert::AreEqual(12, hex_to_nibble('c'));
                Assert::AreEqual(13, hex_to_nibble('d'));
                Assert::AreEqual(14, hex_to_nibble('e'));
                Assert::AreEqual(15, hex_to_nibble('f'));
            }

            TEST_METHOD(GetHexStringBitCount)
            {
                Assert::AreEqual(0, get_hex_string_bit_count(nullptr, 0));
                Assert::AreEqual(0, get_hex_string_bit_count("0", 1));
                Assert::AreEqual(0, get_hex_string_bit_count("000000000", 9));
                Assert::AreEqual(1, get_hex_string_bit_count("1", 1));
                Assert::AreEqual(1, get_hex_string_bit_count("00001", 5));
                Assert::AreEqual(2, get_hex_string_bit_count("2", 1));
                Assert::AreEqual(2, get_hex_string_bit_count("00002", 5));
                Assert::AreEqual(2, get_hex_string_bit_count("3", 1));
                Assert::AreEqual(2, get_hex_string_bit_count("0003", 4));
                Assert::AreEqual(3, get_hex_string_bit_count("4", 1));
                Assert::AreEqual(3, get_hex_string_bit_count("5", 1));
                Assert::AreEqual(3, get_hex_string_bit_count("6", 1));
                Assert::AreEqual(3, get_hex_string_bit_count("7", 1));
                Assert::AreEqual(4, get_hex_string_bit_count("8", 1));
                Assert::AreEqual(4, get_hex_string_bit_count("9", 1));
                Assert::AreEqual(4, get_hex_string_bit_count("A", 1));
                Assert::AreEqual(4, get_hex_string_bit_count("B", 1));
                Assert::AreEqual(4, get_hex_string_bit_count("C", 1));
                Assert::AreEqual(4, get_hex_string_bit_count("D", 1));
                Assert::AreEqual(4, get_hex_string_bit_count("E", 1));
                Assert::AreEqual(4, get_hex_string_bit_count("F", 1));
                Assert::AreEqual(5, get_hex_string_bit_count("10", 2));
                Assert::AreEqual(5, get_hex_string_bit_count("00010", 5));
                Assert::AreEqual(5, get_hex_string_bit_count("11", 2));
                Assert::AreEqual(5, get_hex_string_bit_count("1F", 2));
                Assert::AreEqual(6, get_hex_string_bit_count("20", 2));
                Assert::AreEqual(6, get_hex_string_bit_count("2F", 2));
                Assert::AreEqual(7, get_hex_string_bit_count("7F", 2));
                Assert::AreEqual(7, get_hex_string_bit_count("0007F", 5));
                Assert::AreEqual(8, get_hex_string_bit_count("80", 2));
                Assert::AreEqual(8, get_hex_string_bit_count("FF", 2));
                Assert::AreEqual(8, get_hex_string_bit_count("00FF", 4));
                Assert::AreEqual(9, get_hex_string_bit_count("100", 3));
                Assert::AreEqual(9, get_hex_string_bit_count("000100", 6));
                Assert::AreEqual(22, get_hex_string_bit_count("200000", 6));
                Assert::AreEqual(35, get_hex_string_bit_count("7FFF30001", 9));

                Assert::AreEqual(15, get_hex_string_bit_count("7FFF30001", 4));
                Assert::AreEqual(3, get_hex_string_bit_count("7FFF30001", 1));
                Assert::AreEqual(0, get_hex_string_bit_count("7FFF30001", 0));
            }

            TEST_METHOD(HexStringToUInt64)
            {
                uint64_t correct[3];
                uint64_t parsed[3];

                correct[0] = 0;
                correct[1] = 0;
                correct[2] = 0;
                parsed[0] = 0x123;
                parsed[1] = 0x123;
                parsed[2] = 0x123;
                hex_string_to_uint64("0", 1, 3, parsed);
                Assert::AreEqual(0, memcmp(correct, parsed, 3 * sizeof(uint64_t)));
                parsed[0] = 0x123;
                parsed[1] = 0x123;
                parsed[2] = 0x123;
                hex_string_to_uint64("0", 1, 1, parsed);
                Assert::AreEqual(0, memcmp(correct, parsed, 1 * sizeof(uint64_t)));
                parsed[0] = 0x123;
                parsed[1] = 0x123;
                parsed[2] = 0x123;
                hex_string_to_uint64(nullptr, 0, 3, parsed);
                Assert::AreEqual(0, memcmp(correct, parsed, 3 * sizeof(uint64_t)));

                correct[0] = 1;
                parsed[0] = 0x123;
                parsed[1] = 0x123;
                parsed[2] = 0x123;
                hex_string_to_uint64("1", 1, 3, parsed);
                Assert::AreEqual(0, memcmp(correct, parsed, 3 * sizeof(uint64_t)));
                parsed[0] = 0x123;
                parsed[1] = 0x123;
                parsed[2] = 0x123;
                hex_string_to_uint64("01", 2, 3, parsed);
                Assert::AreEqual(0, memcmp(correct, parsed, 3 * sizeof(uint64_t)));
                parsed[0] = 0x123;
                parsed[1] = 0x123;
                parsed[2] = 0x123;
                hex_string_to_uint64("001", 3, 1, parsed);
                Assert::AreEqual(0, memcmp(correct, parsed, 1 * sizeof(uint64_t)));

                correct[0] = 0xF;
                parsed[0] = 0x123;
                parsed[1] = 0x123;
                parsed[2] = 0x123;
                hex_string_to_uint64("F", 1, 3, parsed);
                Assert::AreEqual(0, memcmp(correct, parsed, 3 * sizeof(uint64_t)));

                correct[0] = 0x10;
                parsed[0] = 0x123;
                parsed[1] = 0x123;
                parsed[2] = 0x123;
                hex_string_to_uint64("10", 2, 3, parsed);
                Assert::AreEqual(0, memcmp(correct, parsed, 3 * sizeof(uint64_t)));
                parsed[0] = 0x123;
                parsed[1] = 0x123;
                parsed[2] = 0x123;
                hex_string_to_uint64("010", 3, 3, parsed);
                Assert::AreEqual(0, memcmp(correct, parsed, 3 * sizeof(uint64_t)));

                correct[0] = 0x100;
                parsed[0] = 0x123;
                parsed[1] = 0x123;
                parsed[2] = 0x123;
                hex_string_to_uint64("100", 3, 3, parsed);
                Assert::AreEqual(0, memcmp(correct, parsed, 3 * sizeof(uint64_t)));

                correct[0] = 0x123;
                parsed[0] = 0x123;
                parsed[1] = 0x123;
                parsed[2] = 0x123;
                hex_string_to_uint64("123", 3, 3, parsed);
                Assert::AreEqual(0, memcmp(correct, parsed, 3 * sizeof(uint64_t)));
                parsed[0] = 0x123;
                parsed[1] = 0x123;
                parsed[2] = 0x123;
                hex_string_to_uint64("00000123", 8, 3, parsed);
                Assert::AreEqual(0, memcmp(correct, parsed, 3 * sizeof(uint64_t)));

                correct[0] = 0;
                correct[1] = 1;
                parsed[0] = 0x123;
                parsed[1] = 0x123;
                parsed[2] = 0x123;
                hex_string_to_uint64("10000000000000000", 17, 3, parsed);
                Assert::AreEqual(0, memcmp(correct, parsed, 3 * sizeof(uint64_t)));

                correct[0] = 0x1123456789ABCDEF;
                correct[1] = 0x1;
                parsed[0] = 0x123;
                parsed[1] = 0x123;
                parsed[2] = 0x123;
                hex_string_to_uint64("11123456789ABCDEF", 17, 3, parsed);
                Assert::AreEqual(0, memcmp(correct, parsed, 3 * sizeof(uint64_t)));
                parsed[0] = 0x123;
                parsed[1] = 0x123;
                parsed[2] = 0x123;
                hex_string_to_uint64("000011123456789ABCDEF", 21, 3, parsed);
                Assert::AreEqual(0, memcmp(correct, parsed, 3 * sizeof(uint64_t)));

                correct[0] = 0x3456789ABCDEF123;
                correct[1] = 0x23456789ABCDEF12;
                correct[2] = 0x123456789ABCDEF1;
                parsed[0] = 0x123;
                parsed[1] = 0x123;
                parsed[2] = 0x123;
                hex_string_to_uint64("123456789ABCDEF123456789ABCDEF123456789ABCDEF123", 48, 3, parsed);
                Assert::AreEqual(0, memcmp(correct, parsed, 3 * sizeof(uint64_t)));

                correct[0] = 0xFFFFFFFFFFFFFFFF;
                correct[1] = 0xFFFFFFFFFFFFFFFF;
                correct[2] = 0xFFFFFFFFFFFFFFFF;
                parsed[0] = 0x123;
                parsed[1] = 0x123;
                parsed[2] = 0x123;
                hex_string_to_uint64("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", 48, 3, parsed);
                Assert::AreEqual(0, memcmp(correct, parsed, 3 * sizeof(uint64_t)));

                correct[0] = 0x100;
                correct[1] = 0;
                correct[2] = 0;
                parsed[0] = 0x123;
                parsed[1] = 0x123;
                parsed[2] = 0x123;
                hex_string_to_uint64("100", 3, 3, parsed);
                Assert::AreEqual(0, memcmp(correct, parsed, 3 * sizeof(uint64_t)));

                correct[0] = 0x10;
                parsed[0] = 0x123;
                parsed[1] = 0x123;
                parsed[2] = 0x123;
                hex_string_to_uint64("100", 2, 3, parsed);
                Assert::AreEqual(0, memcmp(correct, parsed, 3 * sizeof(uint64_t)));

                correct[0] = 0x1;
                parsed[0] = 0x123;
                parsed[1] = 0x123;
                parsed[2] = 0x123;
                hex_string_to_uint64("100", 1, 3, parsed);
                Assert::AreEqual(0, memcmp(correct, parsed, 3 * sizeof(uint64_t)));

                correct[0] = 0;
                parsed[0] = 0x123;
                parsed[1] = 0x123;
                parsed[2] = 0x123;
                hex_string_to_uint64("100", 0, 3, parsed);
                Assert::AreEqual(0, memcmp(correct, parsed, 3 * sizeof(uint64_t)));
            }
        };
    }
}
