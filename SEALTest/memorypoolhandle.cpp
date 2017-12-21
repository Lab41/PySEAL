#include "CppUnitTest.h"
#include "seal/memorypoolhandle.h"
#include "seal/util/uintcore.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace seal::util;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(MemoryPoolHandleTest)
    {
    public:
        TEST_METHOD(MemoryPoolHandleConstructAssign)
        {
            MemoryPoolHandle pool;
            Assert::IsFalse(pool);
            pool = MemoryPoolHandle::Global();
            Assert::IsTrue(&static_cast<MemoryPool&>(pool) == global_variables::global_memory_pool);
            pool = MemoryPoolHandle::New(true);
            Assert::IsFalse(&pool.operator seal::util::MemoryPool &() == global_variables::global_memory_pool);
            MemoryPoolHandle pool2 = MemoryPoolHandle::New(true);
            Assert::IsFalse(pool == pool2);

            pool = pool2;
            Assert::IsTrue(pool == pool2);
            pool = MemoryPoolHandle::Global();
            Assert::IsFalse(pool == pool2);
            pool2 = MemoryPoolHandle::Global();
            Assert::IsTrue(pool == pool2);
        }

        TEST_METHOD(MemoryPoolHandleAllocate)
        {
            MemoryPoolHandle pool = MemoryPoolHandle::New();
            Assert::AreEqual(0ULL, pool.alloc_byte_count());
            Assert::AreEqual(0ULL, pool.alloc_uint64_count());
            {
                Pointer ptr(allocate_uint(5, pool));
                Assert::AreEqual(40ULL, pool.alloc_byte_count());
                Assert::AreEqual(5ULL, pool.alloc_uint64_count());
            }

            pool = MemoryPoolHandle::New();
            Assert::AreEqual(0ULL, pool.alloc_byte_count());
            Assert::AreEqual(0ULL, pool.alloc_uint64_count());
            {
                Pointer ptr(allocate_uint(5, pool));
                Assert::AreEqual(40ULL, pool.alloc_byte_count());
                Assert::AreEqual(5ULL, pool.alloc_uint64_count());

                ptr = allocate_uint(8, pool);
                Assert::AreEqual(104ULL, pool.alloc_byte_count());
                Assert::AreEqual(13ULL, pool.alloc_uint64_count());

                Pointer ptr2(allocate_uint(2, pool));
                Assert::AreEqual(120ULL, pool.alloc_byte_count());
                Assert::AreEqual(15ULL, pool.alloc_uint64_count());
            }
        }
    };
}