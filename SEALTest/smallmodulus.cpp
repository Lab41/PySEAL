#include "CppUnitTest.h"
#include "seal/smallmodulus.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(SmallModulusTest)
    {
    public:
        TEST_METHOD(CreateSmallModulus)
        {
            SmallModulus mod;
            Assert::IsTrue(mod.is_zero());
            Assert::AreEqual(0ULL, mod.value());
            Assert::AreEqual(0, mod.bit_count());
            Assert::AreEqual(1, mod.uint64_count());
            Assert::AreEqual(0ULL, mod.const_ratio()[0]);
            Assert::AreEqual(0ULL, mod.const_ratio()[1]);
            Assert::AreEqual(0ULL, mod.const_ratio()[2]);

            mod = 3;
            Assert::IsFalse(mod.is_zero());
            Assert::AreEqual(3ULL, mod.value());
            Assert::AreEqual(2, mod.bit_count());
            Assert::AreEqual(1, mod.uint64_count());
            Assert::AreEqual(6148914691236517205ULL, mod.const_ratio()[0]);
            Assert::AreEqual(6148914691236517205ULL, mod.const_ratio()[1]);
            Assert::AreEqual(1ULL, mod.const_ratio()[2]);

            SmallModulus mod2(2);
            SmallModulus mod3(3);
            Assert::IsTrue(mod != mod2);
            Assert::IsTrue(mod == mod3);

            mod = 0;
            Assert::IsTrue(mod.is_zero());
            Assert::AreEqual(0ULL, mod.value());
            Assert::AreEqual(0, mod.bit_count());
            Assert::AreEqual(1, mod.uint64_count());
            Assert::AreEqual(0ULL, mod.const_ratio()[0]);
            Assert::AreEqual(0ULL, mod.const_ratio()[1]);
            Assert::AreEqual(0ULL, mod.const_ratio()[2]);

            mod = 0xF00000F00000F;
            Assert::IsFalse(mod.is_zero());
            Assert::AreEqual(0xF00000F00000FULL, mod.value());
            Assert::AreEqual(52, mod.bit_count());
            Assert::AreEqual(1, mod.uint64_count());
            Assert::AreEqual(1224979098644774929ULL, mod.const_ratio()[0]);
            Assert::AreEqual(4369ULL, mod.const_ratio()[1]);
            Assert::AreEqual(281470698520321ULL, mod.const_ratio()[2]);
        }

        TEST_METHOD(SaveLoadSmallModulus)
        {
            stringstream stream;

            SmallModulus mod;
            mod.save(stream);

            SmallModulus mod2;
            mod2.load(stream);
            Assert::AreEqual(mod2.value(), mod.value());
            Assert::AreEqual(mod2.bit_count(), mod.bit_count());
            Assert::AreEqual(mod2.uint64_count(), mod.uint64_count());
            Assert::AreEqual(mod2.const_ratio()[0], mod.const_ratio()[0]);
            Assert::AreEqual(mod2.const_ratio()[1], mod.const_ratio()[1]);
            Assert::AreEqual(mod2.const_ratio()[2], mod.const_ratio()[2]);

            mod = 3;
            mod.save(stream);
            mod2.load(stream);
            Assert::AreEqual(mod2.value(), mod.value());
            Assert::AreEqual(mod2.bit_count(), mod.bit_count());
            Assert::AreEqual(mod2.uint64_count(), mod.uint64_count());
            Assert::AreEqual(mod2.const_ratio()[0], mod.const_ratio()[0]);
            Assert::AreEqual(mod2.const_ratio()[1], mod.const_ratio()[1]);
            Assert::AreEqual(mod2.const_ratio()[2], mod.const_ratio()[2]);

            mod = 0xF00000F00000F;
            mod.save(stream);
            mod2.load(stream);
            Assert::AreEqual(mod2.value(), mod.value());
            Assert::AreEqual(mod2.bit_count(), mod.bit_count());
            Assert::AreEqual(mod2.uint64_count(), mod.uint64_count());
            Assert::AreEqual(mod2.const_ratio()[0], mod.const_ratio()[0]);
            Assert::AreEqual(mod2.const_ratio()[1], mod.const_ratio()[1]);
            Assert::AreEqual(mod2.const_ratio()[2], mod.const_ratio()[2]);
        }
    };
}