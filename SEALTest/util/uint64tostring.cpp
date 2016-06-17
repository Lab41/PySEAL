#include "CppUnitTest.h"
#include "util/common.h"
#include "util/mempool.h"
#include <cstdint>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal::util;
using namespace std;

namespace SEALTest
{
    namespace util
    {
        TEST_CLASS(UInt64ToString)
        {
        public:
            TEST_METHOD(NibbleToUpperHexTest)
            {
                Assert::AreEqual('0', nibble_to_upper_hex(0));
                Assert::AreEqual('1', nibble_to_upper_hex(1));
                Assert::AreEqual('2', nibble_to_upper_hex(2));
                Assert::AreEqual('3', nibble_to_upper_hex(3));
                Assert::AreEqual('4', nibble_to_upper_hex(4));
                Assert::AreEqual('5', nibble_to_upper_hex(5));
                Assert::AreEqual('6', nibble_to_upper_hex(6));
                Assert::AreEqual('7', nibble_to_upper_hex(7));
                Assert::AreEqual('8', nibble_to_upper_hex(8));
                Assert::AreEqual('9', nibble_to_upper_hex(9));
                Assert::AreEqual('A', nibble_to_upper_hex(10));
                Assert::AreEqual('B', nibble_to_upper_hex(11));
                Assert::AreEqual('C', nibble_to_upper_hex(12));
                Assert::AreEqual('D', nibble_to_upper_hex(13));
                Assert::AreEqual('E', nibble_to_upper_hex(14));
                Assert::AreEqual('F', nibble_to_upper_hex(15));
            }

            TEST_METHOD(UInt64ToHexString)
            {
                uint64_t number[] = { 0, 0, 0 };
                string correct = "0";
                Assert::AreEqual(correct, uint64_to_hex_string(number, 3));
                Assert::AreEqual(correct, uint64_to_hex_string(number, 1));
                Assert::AreEqual(correct, uint64_to_hex_string(number, 0));
                Assert::AreEqual(correct, uint64_to_hex_string(nullptr, 0));

                number[0] = 1;
                correct = "1";
                Assert::AreEqual(correct, uint64_to_hex_string(number, 3));
                Assert::AreEqual(correct, uint64_to_hex_string(number, 1));

                number[0] = 0xF;
                correct = "F";
                Assert::AreEqual(correct, uint64_to_hex_string(number, 3));

                number[0] = 0x10;
                correct = "10";
                Assert::AreEqual(correct, uint64_to_hex_string(number, 3));

                number[0] = 0x100;
                correct = "100";
                Assert::AreEqual(correct, uint64_to_hex_string(number, 3));

                number[0] = 0x123;
                correct = "123";
                Assert::AreEqual(correct, uint64_to_hex_string(number, 3));

                number[0] = 0;
                number[1] = 1;
                correct = "10000000000000000";
                Assert::AreEqual(correct, uint64_to_hex_string(number, 3));

                number[0] = 0x1123456789ABCDEF;
                number[1] = 0x1;
                correct = "11123456789ABCDEF";
                Assert::AreEqual(correct, uint64_to_hex_string(number, 3));

                number[0] = 0x3456789ABCDEF123;
                number[1] = 0x23456789ABCDEF12;
                number[2] = 0x123456789ABCDEF1;
                correct = "123456789ABCDEF123456789ABCDEF123456789ABCDEF123";
                Assert::AreEqual(correct, uint64_to_hex_string(number, 3));

                number[0] = 0xFFFFFFFFFFFFFFFF;
                number[1] = 0xFFFFFFFFFFFFFFFF;
                number[2] = 0xFFFFFFFFFFFFFFFF;
                correct = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";
                Assert::AreEqual(correct, uint64_to_hex_string(number, 3));
            }

            TEST_METHOD(UInt64ToDecString)
            {
                uint64_t number[] = { 0, 0, 0 };
                string correct = "0";
                MemoryPool &pool = *MemoryPool::default_pool();
                Assert::AreEqual(correct, uint64_to_dec_string(number, 3, pool));
                Assert::AreEqual(correct, uint64_to_dec_string(number, 1, pool));
                Assert::AreEqual(correct, uint64_to_dec_string(number, 0, pool));
                Assert::AreEqual(correct, uint64_to_dec_string(nullptr, 0, pool));

                number[0] = 1;
                correct = "1";
                Assert::AreEqual(correct, uint64_to_dec_string(number, 3, pool));
                Assert::AreEqual(correct, uint64_to_dec_string(number, 1, pool));

                number[0] = 9;
                correct = "9";
                Assert::AreEqual(correct, uint64_to_dec_string(number, 3, pool));

                number[0] = 10;
                correct = "10";
                Assert::AreEqual(correct, uint64_to_dec_string(number, 3, pool));

                number[0] = 123;
                correct = "123";
                Assert::AreEqual(correct, uint64_to_dec_string(number, 3, pool));

                number[0] = 987654321;
                correct = "987654321";
                Assert::AreEqual(correct, uint64_to_dec_string(number, 3, pool));

                number[0] = 0;
                number[1] = 1;
                correct = "18446744073709551616";
                Assert::AreEqual(correct, uint64_to_dec_string(number, 3, pool));
            }
        };
    }
}