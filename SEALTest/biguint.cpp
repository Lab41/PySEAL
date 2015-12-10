#include "CppUnitTest.h"
#include "biguint.h"
#include <cstdint>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(BigUnsignedInt)
    {
    public:
        TEST_METHOD(EmptyBigUInt)
        {
            BigUInt uint;
            Assert::AreEqual(0, uint.bit_count());
            Assert::IsTrue(nullptr == uint.pointer());
            Assert::AreEqual(0, uint.byte_count());
            Assert::AreEqual(0, uint.uint64_count());
            Assert::AreEqual(0, uint.significant_bit_count());
            Assert::IsTrue("0" == uint.to_string());
            Assert::IsTrue(uint.is_zero());
            Assert::IsFalse(uint.is_alias());
            uint.set_zero();

            BigUInt uint2;
            Assert::IsTrue(uint == uint2);
            Assert::IsFalse(uint != uint2);

            uint.resize(1);
            Assert::AreEqual(1, uint.bit_count());
            Assert::IsTrue(nullptr != uint.pointer());
            Assert::IsFalse(uint.is_alias());

            uint.resize(0);
            Assert::AreEqual(0, uint.bit_count());
            Assert::IsTrue(nullptr == uint.pointer());
            Assert::IsFalse(uint.is_alias());
        }

        TEST_METHOD(BigUInt64Bits)
        {
            BigUInt uint(64);
            Assert::AreEqual(64, uint.bit_count());
            Assert::IsTrue(nullptr != uint.pointer());
            Assert::AreEqual(8, uint.byte_count());
            Assert::AreEqual(1, uint.uint64_count());
            Assert::AreEqual(0, uint.significant_bit_count());
            Assert::IsTrue("0" == uint.to_string());
            Assert::IsTrue(uint.is_zero());
            Assert::AreEqual(static_cast<uint64_t>(0), *uint.pointer());
            Assert::AreEqual(static_cast<uint8_t>(0), uint[0]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[1]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[2]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[3]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[4]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[5]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[6]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[7]);

            uint = "1";
            Assert::AreEqual(1, uint.significant_bit_count());
            Assert::IsTrue("1" == uint.to_string());
            Assert::IsFalse(uint.is_zero());
            Assert::AreEqual(static_cast<uint64_t>(1), *uint.pointer());
            Assert::AreEqual(static_cast<uint8_t>(1), uint[0]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[1]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[2]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[3]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[4]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[5]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[6]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[7]);
            uint.set_zero();
            Assert::IsTrue(uint.is_zero());
            Assert::AreEqual(static_cast<uint64_t>(0), *uint.pointer());

            uint = "7FFFFFFFFFFFFFFF";
            Assert::AreEqual(63, uint.significant_bit_count());
            Assert::IsTrue("7FFFFFFFFFFFFFFF" == uint.to_string());
            Assert::AreEqual(static_cast<uint64_t>(0x7FFFFFFFFFFFFFFF), *uint.pointer());
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[0]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[1]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[2]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[3]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[4]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[5]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[6]);
            Assert::AreEqual(static_cast<uint8_t>(0x7F), uint[7]);
            Assert::IsFalse(uint.is_zero());

            uint = "FFFFFFFFFFFFFFFF";
            Assert::AreEqual(64, uint.significant_bit_count());
            Assert::IsTrue("FFFFFFFFFFFFFFFF" == uint.to_string());
            Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), *uint.pointer());
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[0]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[1]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[2]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[3]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[4]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[5]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[6]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[7]);
            Assert::IsFalse(uint.is_zero());

            uint = 0x8001;
            Assert::AreEqual(16, uint.significant_bit_count());
            Assert::IsTrue("8001" == uint.to_string());
            Assert::AreEqual(static_cast<uint64_t>(0x8001), *uint.pointer());
            Assert::AreEqual(static_cast<uint8_t>(0x01), uint[0]);
            Assert::AreEqual(static_cast<uint8_t>(0x80), uint[1]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[2]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[3]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[4]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[5]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[6]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[7]);
        }

        TEST_METHOD(BigUInt99Bits)
        {
            BigUInt uint(99);
            Assert::AreEqual(99, uint.bit_count());
            Assert::IsTrue(nullptr != uint.pointer());
            Assert::AreEqual(13, uint.byte_count());
            Assert::AreEqual(2, uint.uint64_count());
            Assert::AreEqual(0, uint.significant_bit_count());
            Assert::IsTrue("0" == uint.to_string());
            Assert::IsTrue(uint.is_zero());
            Assert::AreEqual(static_cast<uint64_t>(0), uint.pointer()[0]);
            Assert::AreEqual(static_cast<uint64_t>(0), uint.pointer()[1]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[0]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[1]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[2]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[3]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[4]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[5]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[6]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[7]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[8]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[9]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[10]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[11]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[12]);

            uint = "1";
            Assert::AreEqual(1, uint.significant_bit_count());
            Assert::IsTrue("1" == uint.to_string());
            Assert::IsFalse(uint.is_zero());
            Assert::AreEqual(static_cast<uint64_t>(1), uint.pointer()[0]);
            Assert::AreEqual(static_cast<uint64_t>(0), uint.pointer()[1]);
            Assert::AreEqual(static_cast<uint8_t>(1), uint[0]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[1]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[2]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[3]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[4]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[5]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[6]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[7]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[8]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[9]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[10]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[11]);
            Assert::AreEqual(static_cast<uint8_t>(0), uint[12]);
            uint.set_zero();
            Assert::IsTrue(uint.is_zero());
            Assert::AreEqual(static_cast<uint64_t>(0), uint.pointer()[0]);
            Assert::AreEqual(static_cast<uint64_t>(0), uint.pointer()[1]);

            uint = "7FFFFFFFFFFFFFFFFFFFFFFFF";
            Assert::AreEqual(99, uint.significant_bit_count());
            Assert::IsTrue("7FFFFFFFFFFFFFFFFFFFFFFFF" == uint.to_string());
            Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), uint.pointer()[0]);
            Assert::AreEqual(static_cast<uint64_t>(0x7FFFFFFFF), uint.pointer()[1]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[0]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[1]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[2]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[3]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[4]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[5]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[6]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[7]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[8]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[9]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[10]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), uint[11]);
            Assert::AreEqual(static_cast<uint8_t>(0x07), uint[12]);
            Assert::IsFalse(uint.is_zero());
            uint.set_zero();
            Assert::IsTrue(uint.is_zero());
            Assert::AreEqual(static_cast<uint64_t>(0), uint.pointer()[0]);
            Assert::AreEqual(static_cast<uint64_t>(0), uint.pointer()[1]);

            uint = "4000000000000000000000000";
            Assert::AreEqual(99, uint.significant_bit_count());
            Assert::IsTrue("4000000000000000000000000" == uint.to_string());
            Assert::AreEqual(static_cast<uint64_t>(0x0000000000000000), uint.pointer()[0]);
            Assert::AreEqual(static_cast<uint64_t>(0x400000000), uint.pointer()[1]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[0]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[1]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[2]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[3]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[4]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[5]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[6]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[7]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[8]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[9]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[10]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[11]);
            Assert::AreEqual(static_cast<uint8_t>(0x04), uint[12]);
            Assert::IsFalse(uint.is_zero());

            uint = 0x8001;
            Assert::AreEqual(16, uint.significant_bit_count());
            Assert::IsTrue("8001" == uint.to_string());
            Assert::AreEqual(static_cast<uint64_t>(0x8001), uint.pointer()[0]);
            Assert::AreEqual(static_cast<uint64_t>(0), uint.pointer()[1]);
            Assert::AreEqual(static_cast<uint8_t>(0x01), uint[0]);
            Assert::AreEqual(static_cast<uint8_t>(0x80), uint[1]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[2]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[3]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[4]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[5]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[6]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[7]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[8]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[9]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[10]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[11]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[12]);

            BigUInt uint2("123");
            Assert::IsFalse(uint == uint2);
            Assert::IsFalse(uint2 == uint);
            Assert::IsTrue(uint != uint2);
            Assert::IsTrue(uint2 != uint);

            uint = uint2;
            Assert::IsTrue(uint == uint2);
            Assert::IsFalse(uint != uint2);
            Assert::AreEqual(9, uint.significant_bit_count());
            Assert::IsTrue("123" == uint.to_string());
            Assert::AreEqual(static_cast<uint64_t>(0x123), uint.pointer()[0]);
            Assert::AreEqual(static_cast<uint64_t>(0), uint.pointer()[1]);
            Assert::AreEqual(static_cast<uint8_t>(0x23), uint[0]);
            Assert::AreEqual(static_cast<uint8_t>(0x01), uint[1]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[2]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[3]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[4]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[5]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[6]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[7]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[8]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[9]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[10]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[11]);
            Assert::AreEqual(static_cast<uint8_t>(0x00), uint[12]);

            uint.resize(8);
            Assert::AreEqual(8, uint.bit_count());
            Assert::AreEqual(1, uint.uint64_count());
            Assert::IsTrue("23" == uint.to_string());

            uint.resize(100);
            Assert::AreEqual(100, uint.bit_count());
            Assert::AreEqual(2, uint.uint64_count());
            Assert::IsTrue("23" == uint.to_string());

            uint.resize(0);
            Assert::AreEqual(0, uint.bit_count());
            Assert::AreEqual(0, uint.uint64_count());
            Assert::IsTrue(nullptr == uint.pointer());
        }

        TEST_METHOD(SaveLoadUInt)
        {
            stringstream stream;

            BigUInt value;
            BigUInt value2("100");
            value.save(stream);
            value2.load(stream);
            Assert::IsTrue(value == value2);

            value = "123";
            value.save(stream);
            value2.load(stream);
            Assert::IsTrue(value == value2);

            value = "FFFFFFFFFFFFFFFFFFFFFFFFFF";
            value.save(stream);
            value2.load(stream);
            Assert::IsTrue(value == value2);

            value = "0";
            value.save(stream);
            value2.load(stream);
            Assert::IsTrue(value == value2);
        }

        TEST_METHOD(DuplicateTo)
        {
            BigUInt original(123);
            original = 56789;

            BigUInt target;

            original.duplicate_to(target);
            Assert::AreEqual(target.bit_count(), original.bit_count());
            Assert::IsTrue(target == original);
        }

        TEST_METHOD(DuplicateFrom)
        {
            BigUInt original(123);
            original = 56789;

            BigUInt target;

            target.duplicate_from(original);
            Assert::AreEqual(target.bit_count(), original.bit_count());
            Assert::IsTrue(target == original);
        }
    };
}
