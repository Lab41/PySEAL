#include "CppUnitTest.h"
#include "util/mempool.h"
#include <memory>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal::util;
using namespace std;

namespace SEALTest
{
    namespace util
    {
        TEST_CLASS(MemoryPoolTests)
        {
        public:
            TEST_METHOD(TestMemoryPool)
            {
                MemoryPool pool;
                Assert::AreEqual(0, pool.pool_count());
                Assert::AreEqual(0, pool.total_byte_count());

                Pointer pointer = pool.get_for_uint64_count(0);
                Assert::IsFalse(pointer.is_set());
                pointer.release();
                Assert::AreEqual(0, pool.pool_count());
                Assert::AreEqual(0, pool.total_byte_count());

                pointer = pool.get_for_uint64_count(2);
                uint64_t *allocation1 = pointer.get();
                Assert::IsTrue(pointer.is_set());
                Assert::AreEqual(1, pool.pool_count());
                Assert::AreEqual(0, pool.total_byte_count());
                pointer.release();
                Assert::IsFalse(pointer.is_set());
                Assert::AreEqual(1, pool.pool_count());
                Assert::AreEqual(16, pool.total_byte_count());

                pointer = pool.get_for_uint64_count(2);
                Assert::IsTrue(allocation1 == pointer.get());
                Assert::IsTrue(pointer.is_set());
                Assert::AreEqual(1, pool.pool_count());
                Assert::AreEqual(0, pool.total_byte_count());
                pointer.release();
                Assert::IsFalse(pointer.is_set());
                Assert::AreEqual(1, pool.pool_count());
                Assert::AreEqual(16, pool.total_byte_count());

                pointer = pool.get_for_uint64_count(1);
                Assert::IsFalse(allocation1 == pointer.get());
                Assert::IsTrue(pointer.is_set());
                Assert::AreEqual(2, pool.pool_count());
                Assert::AreEqual(16, pool.total_byte_count());
                pointer.release();
                Assert::IsFalse(pointer.is_set());
                Assert::AreEqual(2, pool.pool_count());
                Assert::AreEqual(24, pool.total_byte_count());

                pointer = pool.get_for_uint64_count(2);
                Assert::IsTrue(allocation1 == pointer.get());
                Pointer pointer2 = pool.get_for_uint64_count(2);
                uint64_t *allocation2 = pointer2.get();
                Assert::IsFalse(allocation2 == pointer.get());
                Assert::IsTrue(pointer.is_set());
                Assert::AreEqual(2, pool.pool_count());
                Assert::AreEqual(8, pool.total_byte_count());
                pointer.release();
                pointer2.release();
                Assert::AreEqual(2, pool.pool_count());
                Assert::AreEqual(40, pool.total_byte_count());

                pointer = pool.get_for_uint64_count(2);
                Assert::IsTrue(allocation2 == pointer.get());
                pointer2 = pool.get_for_uint64_count(2);
                Assert::IsTrue(allocation1 == pointer2.get());
                Pointer pointer3 = pool.get_for_uint64_count(1);
                Assert::AreEqual(2, pool.pool_count());
                Assert::AreEqual(0, pool.total_byte_count());
                
                pointer.release();
                pointer2.release();
                pointer3.release();
                Assert::AreEqual(2, pool.pool_count());
                Assert::AreEqual(40, pool.total_byte_count());

                pool.free_all();
                Assert::AreEqual(0, pool.pool_count());
                Assert::AreEqual(0, pool.total_byte_count());
            }

            TEST_METHOD(PointerTests)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer p1;
                Assert::IsFalse(p1.is_set());
                Assert::IsTrue(p1.get() == nullptr);

                p1 = pool.get_for_uint64_count(1);
                uint64_t *allocation1 = p1.get();
                Assert::IsTrue(p1.is_set());
                Assert::IsTrue(p1.get() != nullptr);

                p1.release();
                Assert::IsFalse(p1.is_set());
                Assert::IsTrue(p1.get() == nullptr);

                p1 = pool.get_for_uint64_count(1);
                Assert::IsTrue(p1.is_set());
                Assert::IsTrue(p1.get() == allocation1);

                Pointer p2;
                p2.acquire(p1);
                Assert::IsFalse(p1.is_set());
                Assert::IsTrue(p2.is_set());
                Assert::IsTrue(p2.get() == allocation1);

                ConstPointer cp2;
                cp2.acquire(p2);
                Assert::IsFalse(p2.is_set());
                Assert::IsTrue(cp2.is_set());
                Assert::IsTrue(cp2.get() == allocation1);
                cp2.release();

                Pointer p3 = pool.get_for_uint64_count(1);
                Assert::IsTrue(p3.is_set());
                Assert::IsTrue(p3.get() == allocation1);

                Pointer p4 = pool.get_for_uint64_count(2);
                Assert::IsTrue(p4.is_set());
                uint64_t *allocation2 = p4.get();
                p3.swap_with(p4);
                Assert::IsTrue(p3.is_set());
                Assert::IsTrue(p3.get() == allocation2);
                Assert::IsTrue(p4.is_set());
                Assert::IsTrue(p4.get() == allocation1);
                p3.release();
                p4.release();
            }

            TEST_METHOD(DuplicateIfNeeded)
            {
                unique_ptr<uint64_t[]> allocation(new uint64_t[2]);
                allocation[0] = 0x1234567812345678;
                allocation[1] = 0x8765432187654321;

                MemoryPool pool;
                Pointer p1 = duplicate_if_needed(allocation.get(), 2, false, pool);
                Assert::IsTrue(p1.is_set());
                Assert::IsTrue(p1.get() == allocation.get());
                Assert::AreEqual(0, pool.pool_count());

                p1 = duplicate_if_needed(allocation.get(), 2, true, pool);
                Assert::IsTrue(p1.is_set());
                Assert::IsFalse(p1.get() == allocation.get());
                Assert::AreEqual(1, pool.pool_count());
                Assert::IsTrue(p1.get()[0] == 0x1234567812345678);
                Assert::IsTrue(p1.get()[1] == 0x8765432187654321);
                p1.release();
            }
        };
    }
}
