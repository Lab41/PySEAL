#include "CppUnitTest.h"
#include "util/modulus.h"
#include <cstdint>
#include "biguint.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace seal::util;
using namespace std;

namespace SEALTest
{
    namespace util
    {
        TEST_CLASS(TestModulus)
        {
        public:
            TEST_METHOD(ModulusTest)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
                Modulus mod;
                Assert::IsFalse(mod.is_power_of_two_minus_one());
                Assert::IsFalse(mod.has_inverse());
                Assert::IsTrue(nullptr == mod.get());
                Assert::IsTrue(nullptr == mod.get_inverse());
                Assert::AreEqual(0, mod.uint64_count());
                Assert::AreEqual(0, mod.significant_bit_count());
                Assert::AreEqual(-1, mod.power_of_two_minus_one());

                BigUInt value("7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
                mod = Modulus(value.pointer(), 2);
                Assert::IsTrue(mod.is_power_of_two_minus_one());
                Assert::IsTrue(mod.has_inverse());
                Assert::IsTrue(value.pointer() == mod.get());
                Assert::AreEqual(static_cast<uint64_t>(0), mod.get_inverse()[1]);
                Assert::AreEqual(static_cast<uint64_t>(1), mod.get_inverse()[0]);
                Assert::AreEqual(2, mod.uint64_count());
                Assert::AreEqual(127, mod.significant_bit_count());
                Assert::AreEqual(127, mod.power_of_two_minus_one());

                value = "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE00000001";
                mod = Modulus(value.pointer(), 6);
                Assert::IsFalse(mod.is_power_of_two_minus_one());
                Assert::IsTrue(mod.has_inverse());
                Assert::IsTrue(value.pointer() == mod.get());
                Assert::AreEqual(static_cast<uint64_t>(0), mod.get_inverse()[5]);
                Assert::AreEqual(static_cast<uint64_t>(0), mod.get_inverse()[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), mod.get_inverse()[3]);
                Assert::AreEqual(static_cast<uint64_t>(0), mod.get_inverse()[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), mod.get_inverse()[1]);
                Assert::AreEqual(static_cast<uint64_t>(0x1FFFFFFFF), mod.get_inverse()[0]);
                Assert::AreEqual(6, mod.uint64_count());
                Assert::AreEqual(383, mod.significant_bit_count());
                Assert::AreEqual(-1, mod.power_of_two_minus_one());

                value = "9FFF";
                mod = Modulus(value.pointer(), 1);
                Assert::IsFalse(mod.is_power_of_two_minus_one());
                Assert::IsFalse(mod.has_inverse());
                Assert::IsTrue(value.pointer() == mod.get());
                Assert::IsTrue(nullptr == mod.get_inverse());
                Assert::AreEqual(1, mod.uint64_count());
                Assert::AreEqual(16, mod.significant_bit_count());
                Assert::AreEqual(-1, mod.power_of_two_minus_one());
            }
        };
    }
}