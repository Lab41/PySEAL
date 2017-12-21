#include "CppUnitTest.h"
#include "seal/plaintext.h"
#include "seal/memorypoolhandle.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace seal::util;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(PlaintextTest)
    {
    public:
        TEST_METHOD(PlaintextBasics)
        {
            Plaintext plain(2);
            Assert::AreEqual(2, plain.capacity());
            Assert::AreEqual(2, plain.coeff_count());
            Assert::AreEqual(0, plain.significant_coeff_count());
            uint64_t *ptr = plain.pointer();
            plain[0] = 1;
            plain[1] = 2;

            plain.reserve(10);
            Assert::AreEqual(10, plain.capacity());
            Assert::AreEqual(2, plain.coeff_count());
            Assert::AreEqual(2, plain.significant_coeff_count());
            Assert::IsTrue(ptr != plain.pointer());
            Assert::AreEqual(1ULL, plain[0]);
            Assert::AreEqual(2ULL, plain[1]);

            ptr = plain.pointer();

            plain.resize(5);
            Assert::AreEqual(10, plain.capacity());
            Assert::AreEqual(5, plain.coeff_count());
            Assert::AreEqual(2, plain.significant_coeff_count());
            Assert::IsTrue(ptr == plain.pointer());
            Assert::AreEqual(1ULL, plain[0]);
            Assert::AreEqual(2ULL, plain[1]);            
            Assert::AreEqual(0ULL, plain[2]);
            Assert::AreEqual(0ULL, plain[3]);
            Assert::AreEqual(0ULL, plain[4]);
            Assert::IsFalse(plain.is_alias());

            MemoryPoolHandle pool = MemoryPoolHandle::Global();
            Pointer plain_alloc(allocate_zero_uint(20, pool));
            Plaintext plain2;
            plain2.alias(20, 0, plain_alloc.get());
            Assert::AreEqual(20, plain2.capacity());
            Assert::AreEqual(0, plain2.coeff_count());
            Assert::AreEqual(0, plain2.significant_coeff_count());
            Assert::IsTrue(plain_alloc.get() == plain2.pointer());
            Assert::IsTrue(plain2.is_alias());

            plain2.resize(15);
            Assert::AreEqual(20, plain2.capacity());
            Assert::AreEqual(15, plain2.coeff_count());
            Assert::AreEqual(0, plain2.significant_coeff_count());
            Assert::IsTrue(plain_alloc.get() == plain2.pointer());

            plain2 = plain;
            Assert::AreEqual(20, plain2.capacity());
            Assert::AreEqual(5, plain2.coeff_count());
            Assert::AreEqual(2, plain2.significant_coeff_count());
            Assert::AreEqual(1ULL, plain2[0]);
            Assert::AreEqual(2ULL, plain2[1]);
            Assert::AreEqual(0ULL, plain2[2]);
            Assert::AreEqual(0ULL, plain2[3]);
            Assert::AreEqual(0ULL, plain2[4]);
            Assert::IsTrue(plain2.is_alias());

            ptr = plain2.pointer();

            plain2.unalias();
            Assert::AreEqual(20, plain2.capacity());
            Assert::AreEqual(5, plain2.coeff_count());
            Assert::AreEqual(2, plain2.significant_coeff_count());
            Assert::AreEqual(1ULL, plain2[0]);
            Assert::AreEqual(2ULL, plain2[1]);
            Assert::AreEqual(0ULL, plain2[2]);
            Assert::AreEqual(0ULL, plain2[3]);
            Assert::AreEqual(0ULL, plain2[4]);
            Assert::IsFalse(plain2.is_alias());
            Assert::IsTrue(ptr != plain2.pointer());
        }

        TEST_METHOD(SaveLoadPlaintext)
        {
            stringstream stream;

            Plaintext plain;
            Plaintext plain2;
            plain.save(stream);
            plain2.load(stream);
            Assert::IsTrue(plain.pointer() == plain2.pointer());
            Assert::IsTrue(plain2.pointer() == nullptr);
            Assert::AreEqual(0, plain2.capacity());
            Assert::AreEqual(0, plain2.coeff_count());

            plain.reserve(20);
            plain.resize(5);
            plain[0] = 1;
            plain[1] = 2;
            plain[2] = 3;
            plain.save(stream);
            plain2.load(stream);
            Assert::IsTrue(plain.pointer() != plain2.pointer());
            Assert::AreEqual(5, plain2.capacity());
            Assert::AreEqual(5, plain2.coeff_count());
            Assert::AreEqual(1ULL, plain2[0]);
            Assert::AreEqual(2ULL, plain2[1]);
            Assert::AreEqual(3ULL, plain2[2]);
            Assert::AreEqual(0ULL, plain2[3]);
            Assert::AreEqual(0ULL, plain2[4]);


            MemoryPoolHandle pool = MemoryPoolHandle::Global();
            Pointer plain_alloc(allocate_zero_uint(20, pool));
            plain.alias(20, 7, plain_alloc.get());
            Assert::IsTrue(plain.is_alias());
            plain[0] = 5;
            plain[1] = 4;
            plain[2] = 3;
            plain[3] = 2;
            plain[4] = 1;
            plain[5] = 9;
            plain[6] = 8;
            plain.save(stream);
            plain2.load(stream);
            Assert::IsFalse(plain2.is_alias());
            Assert::AreEqual(7, plain2.capacity());
            Assert::AreEqual(7, plain2.coeff_count());
            Assert::IsTrue(plain.pointer() != plain2.pointer());
            Assert::AreEqual(5ULL, plain2[0]);
            Assert::AreEqual(4ULL, plain2[1]);
            Assert::AreEqual(3ULL, plain2[2]);
            Assert::AreEqual(2ULL, plain2[3]);
            Assert::AreEqual(1ULL, plain2[4]);
            Assert::AreEqual(9ULL, plain2[5]);
            Assert::AreEqual(8ULL, plain2[6]);
        }
    };
}