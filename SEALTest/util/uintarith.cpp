#include "CppUnitTest.h"
#include "seal/util/uintarith.h"
#include <cstdint>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal::util;
using namespace std;

namespace SEALTest
{
    namespace util
    {
        TEST_CLASS(UIntArith)
        {
        public:
            TEST_METHOD(AddUInt64Generic)
            {
                uint64_t result;
                Assert::IsFalse(add_uint64_generic(0ULL, 0ULL, 0, &result));
                Assert::AreEqual(0ULL, result);
                Assert::IsFalse(add_uint64_generic(1ULL, 1ULL, 0, &result));
                Assert::AreEqual(2ULL, result);
                Assert::IsFalse(add_uint64_generic(1ULL, 0ULL, 1, &result));
                Assert::AreEqual(2ULL, result);
                Assert::IsFalse(add_uint64_generic(0ULL, 1ULL, 1, &result));
                Assert::AreEqual(2ULL, result);
                Assert::IsFalse(add_uint64_generic(1ULL, 1ULL, 1, &result));
                Assert::AreEqual(3ULL, result);
                Assert::IsTrue(add_uint64_generic(0xFFFFFFFFFFFFFFFFULL, 1ULL, 0, &result));
                Assert::AreEqual(0ULL, result);
                Assert::IsTrue(add_uint64_generic(1ULL, 0xFFFFFFFFFFFFFFFFULL, 0, &result));
                Assert::AreEqual(0ULL, result);
                Assert::IsTrue(add_uint64_generic(1ULL, 0xFFFFFFFFFFFFFFFFULL, 1, &result));
                Assert::AreEqual(1ULL, result);
                Assert::IsTrue(add_uint64_generic(2ULL, 0xFFFFFFFFFFFFFFFEULL, 0, &result));
                Assert::AreEqual(0ULL, result);
                Assert::IsTrue(add_uint64_generic(2ULL, 0xFFFFFFFFFFFFFFFEULL, 1, &result));
                Assert::AreEqual(1ULL, result);
                Assert::IsFalse(add_uint64_generic(0xF00F00F00F00F00FULL, 0x0FF0FF0FF0FF0FF0ULL, 0, &result));
                Assert::AreEqual(0xFFFFFFFFFFFFFFFFULL, result);
                Assert::IsTrue(add_uint64_generic(0xF00F00F00F00F00FULL, 0x0FF0FF0FF0FF0FF0ULL, 1, &result));
                Assert::AreEqual(0x0ULL, result);
            }

            TEST_METHOD(AddUInt64)
            {
                uint64_t result;
                Assert::IsFalse(add_uint64(0ULL, 0ULL, 0, &result));
                Assert::AreEqual(0ULL, result);
                Assert::IsFalse(add_uint64(1ULL, 1ULL, 0, &result));
                Assert::AreEqual(2ULL, result);
                Assert::IsFalse(add_uint64(1ULL, 0ULL, 1, &result));
                Assert::AreEqual(2ULL, result);
                Assert::IsFalse(add_uint64(0ULL, 1ULL, 1, &result));
                Assert::AreEqual(2ULL, result);
                Assert::IsFalse(add_uint64(1ULL, 1ULL, 1, &result));
                Assert::AreEqual(3ULL, result);
                Assert::IsTrue(add_uint64(0xFFFFFFFFFFFFFFFFULL, 1ULL, 0, &result));
                Assert::AreEqual(0ULL, result);
                Assert::IsTrue(add_uint64(1ULL, 0xFFFFFFFFFFFFFFFFULL, 0, &result));
                Assert::AreEqual(0ULL, result);
                Assert::IsTrue(add_uint64(1ULL, 0xFFFFFFFFFFFFFFFFULL, 1, &result));
                Assert::AreEqual(1ULL, result);
                Assert::IsTrue(add_uint64(2ULL, 0xFFFFFFFFFFFFFFFEULL, 0, &result));
                Assert::AreEqual(0ULL, result);
                Assert::IsTrue(add_uint64(2ULL, 0xFFFFFFFFFFFFFFFEULL, 1, &result));
                Assert::AreEqual(1ULL, result);
                Assert::IsFalse(add_uint64(0xF00F00F00F00F00FULL, 0x0FF0FF0FF0FF0FF0ULL, 0, &result));
                Assert::AreEqual(0xFFFFFFFFFFFFFFFFULL, result);
                Assert::IsTrue(add_uint64(0xF00F00F00F00F00FULL, 0x0FF0FF0FF0FF0FF0ULL, 1, &result));
                Assert::AreEqual(0x0ULL, result);
            }

            TEST_METHOD(SubUInt64Generic)
            {
                uint64_t result;
                Assert::IsFalse(sub_uint64_generic(0ULL, 0ULL, 0, &result));
                Assert::AreEqual(0ULL, result);
                Assert::IsFalse(sub_uint64_generic(1ULL, 1ULL, 0, &result));
                Assert::AreEqual(0ULL, result);
                Assert::IsFalse(sub_uint64_generic(1ULL, 0ULL, 1, &result));
                Assert::AreEqual(0ULL, result);
                Assert::IsTrue(sub_uint64_generic(0ULL, 1ULL, 1, &result));
                Assert::AreEqual(0xFFFFFFFFFFFFFFFEULL, result);
                Assert::IsTrue(sub_uint64_generic(1ULL, 1ULL, 1, &result));
                Assert::AreEqual(0xFFFFFFFFFFFFFFFFULL, result);
                Assert::IsFalse(sub_uint64_generic(0xFFFFFFFFFFFFFFFFULL, 1ULL, 0, &result));
                Assert::AreEqual(0xFFFFFFFFFFFFFFFEULL, result);
                Assert::IsTrue(sub_uint64_generic(1ULL, 0xFFFFFFFFFFFFFFFFULL, 0, &result));
                Assert::AreEqual(2ULL, result);
                Assert::IsTrue(sub_uint64_generic(1ULL, 0xFFFFFFFFFFFFFFFFULL, 1, &result));
                Assert::AreEqual(1ULL, result);
                Assert::IsTrue(sub_uint64_generic(2ULL, 0xFFFFFFFFFFFFFFFEULL, 0, &result));
                Assert::AreEqual(4ULL, result);
                Assert::IsTrue(sub_uint64_generic(2ULL, 0xFFFFFFFFFFFFFFFEULL, 1, &result));
                Assert::AreEqual(3ULL, result);
                Assert::IsFalse(sub_uint64_generic(0xF00F00F00F00F00FULL, 0x0FF0FF0FF0FF0FF0ULL, 0, &result));
                Assert::AreEqual(0xE01E01E01E01E01FULL, result);
                Assert::IsFalse(sub_uint64_generic(0xF00F00F00F00F00FULL, 0x0FF0FF0FF0FF0FF0ULL, 1, &result));
                Assert::AreEqual(0xE01E01E01E01E01EULL, result);
            }

            TEST_METHOD(SubUInt64)
            {
                uint64_t result;
                Assert::IsFalse(sub_uint64(0ULL, 0ULL, 0, &result));
                Assert::AreEqual(0ULL, result);
                Assert::IsFalse(sub_uint64(1ULL, 1ULL, 0, &result));
                Assert::AreEqual(0ULL, result);
                Assert::IsFalse(sub_uint64(1ULL, 0ULL, 1, &result));
                Assert::AreEqual(0ULL, result);
                Assert::IsTrue(sub_uint64(0ULL, 1ULL, 1, &result));
                Assert::AreEqual(0xFFFFFFFFFFFFFFFEULL, result);
                Assert::IsTrue(sub_uint64(1ULL, 1ULL, 1, &result));
                Assert::AreEqual(0xFFFFFFFFFFFFFFFFULL, result);
                Assert::IsFalse(sub_uint64(0xFFFFFFFFFFFFFFFFULL, 1ULL, 0, &result));
                Assert::AreEqual(0xFFFFFFFFFFFFFFFEULL, result);
                Assert::IsTrue(sub_uint64(1ULL, 0xFFFFFFFFFFFFFFFFULL, 0, &result));
                Assert::AreEqual(2ULL, result);
                Assert::IsTrue(sub_uint64(1ULL, 0xFFFFFFFFFFFFFFFFULL, 1, &result));
                Assert::AreEqual(1ULL, result);
                Assert::IsTrue(sub_uint64(2ULL, 0xFFFFFFFFFFFFFFFEULL, 0, &result));
                Assert::AreEqual(4ULL, result);
                Assert::IsTrue(sub_uint64(2ULL, 0xFFFFFFFFFFFFFFFEULL, 1, &result));
                Assert::AreEqual(3ULL, result);
                Assert::IsFalse(sub_uint64(0xF00F00F00F00F00FULL, 0x0FF0FF0FF0FF0FF0ULL, 0, &result));
                Assert::AreEqual(0xE01E01E01E01E01FULL, result);
                Assert::IsFalse(sub_uint64(0xF00F00F00F00F00FULL, 0x0FF0FF0FF0FF0FF0ULL, 1, &result));
                Assert::AreEqual(0xE01E01E01E01E01EULL, result);
            }

            TEST_METHOD(AddUIntUInt)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(2, pool));
                Pointer ptr2(allocate_uint(2, pool));
                Pointer ptr3(allocate_uint(2, pool));
                ptr[0] = 0;
                ptr[1] = 0;
                ptr2[0] = 0;
                ptr2[1] = 0;
                ptr3[0] = 0xFFFFFFFFFFFFFFFF;
                ptr3[1] = 0xFFFFFFFFFFFFFFFF;
                Assert::IsFalse(add_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[1]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                ptr2[0] = 0;
                ptr2[1] = 0;
                ptr3[0] = 0;
                ptr3[1] = 0;
                Assert::IsFalse(add_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[1]);

                ptr[0] = 0xFFFFFFFFFFFFFFFE;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                ptr2[0] = 1;
                ptr2[1] = 0;
                ptr3[0] = 0;
                ptr3[1] = 0;
                Assert::IsFalse(add_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[1]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                ptr2[0] = 1;
                ptr2[1] = 0;
                ptr3[0] = 0xFFFFFFFFFFFFFFFF;
                ptr3[1] = 0xFFFFFFFFFFFFFFFF;
                Assert::IsTrue(add_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[1]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                ptr3[0] = 0;
                ptr3[1] = 0;

                Assert::IsTrue(add_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[1]);
                Assert::IsTrue(add_uint_uint(ptr.get(), ptr2.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[1]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0;
                ptr2[0] = 1;
                ptr2[1] = 0;
                ptr3[0] = 0;
                ptr3[1] = 0;
                Assert::IsFalse(add_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[0]);
                Assert::AreEqual(1ULL, ptr3[1]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 5;
                ptr2[0] = 1;
                ptr2[1] = 0;
                ptr3[0] = 0;
                ptr3[1] = 0;
                Assert::IsFalse(add_uint_uint(ptr.get(), 2, ptr2.get(), 1, false, 2, ptr3.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(6), ptr3[1]);
                Assert::IsFalse(add_uint_uint(ptr.get(), 2, ptr2.get(), 1, true, 2, ptr3.get()) != 0);
                Assert::AreEqual(1ULL, ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(6), ptr3[1]);
            }

            TEST_METHOD(SubUIntUInt)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(2, pool));
                Pointer ptr2(allocate_uint(2, pool));
                Pointer ptr3(allocate_uint(2, pool));
                ptr[0] = 0;
                ptr[1] = 0;
                ptr2[0] = 0;
                ptr2[1] = 0;
                ptr3[0] = 0xFFFFFFFFFFFFFFFF;
                ptr3[1] = 0xFFFFFFFFFFFFFFFF;
                Assert::IsFalse(sub_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[1]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                ptr2[0] = 0;
                ptr2[1] = 0;
                ptr3[0] = 0;
                ptr3[1] = 0;
                Assert::IsFalse(sub_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[1]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                ptr2[0] = 1;
                ptr2[1] = 0;
                ptr3[0] = 0;
                ptr3[1] = 0;
                Assert::IsFalse(sub_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[1]);

                ptr[0] = 0;
                ptr[1] = 0;
                ptr2[0] = 1;
                ptr2[1] = 0;
                ptr3[0] = 0;
                ptr3[1] = 0;
                Assert::IsTrue(sub_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[1]);
                Assert::IsTrue(sub_uint_uint(ptr.get(), ptr2.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[1]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                ptr3[0] = 0;
                ptr3[1] = 0;
                Assert::IsFalse(sub_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[1]);
                Assert::IsFalse(sub_uint_uint(ptr.get(), ptr2.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);

                ptr[0] = 0xFFFFFFFFFFFFFFFE;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                ptr3[0] = 0;
                ptr3[1] = 0;
                Assert::IsTrue(sub_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[1]);

                ptr[0] = 0;
                ptr[1] = 1;
                ptr2[0] = 1;
                ptr2[1] = 0;
                ptr3[0] = 0;
                ptr3[1] = 0;
                Assert::IsFalse(sub_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[1]);

                ptr[0] = 0;
                ptr[1] = 1;
                ptr2[0] = 1;
                ptr2[1] = 0;
                ptr3[0] = 0;
                ptr3[1] = 0;
                Assert::IsFalse(sub_uint_uint(ptr.get(), 2, ptr2.get(), 1, false, 2, ptr3.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[1]);
                Assert::IsFalse(sub_uint_uint(ptr.get(), 2, ptr2.get(), 1, true, 2, ptr3.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[1]);
            }

            TEST_METHOD(AddUIntUInt64)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(2, pool));
                Pointer ptr2(allocate_uint(2, pool));

                ptr[0] = 0ULL;
                ptr[1] = 0ULL;
                Assert::IsFalse(add_uint_uint64(ptr.get(), 0ULL, 2, ptr2.get()));
                Assert::AreEqual(0ULL, ptr2[0]);
                Assert::AreEqual(0ULL, ptr2[1]);

                ptr[0] = 0xFFFFFFFF00000000ULL;
                ptr[1] = 0ULL;
                Assert::IsFalse(add_uint_uint64(ptr.get(), 0xFFFFFFFFULL, 2, ptr2.get()));
                Assert::AreEqual(0xFFFFFFFFFFFFFFFFULL, ptr2[0]);
                Assert::AreEqual(0ULL, ptr2[1]);

                ptr[0] = 0xFFFFFFFF00000000ULL;
                ptr[1] = 0xFFFFFFFF00000000ULL;
                Assert::IsFalse(add_uint_uint64(ptr.get(), 0x100000000ULL, 2, ptr2.get()));
                Assert::AreEqual(0ULL, ptr2[0]);
                Assert::AreEqual(0xFFFFFFFF00000001ULL, ptr2[1]);

                ptr[0] = 0xFFFFFFFFFFFFFFFFULL;
                ptr[1] = 0xFFFFFFFFFFFFFFFFULL;
                Assert::IsTrue(add_uint_uint64(ptr.get(), 1ULL, 2, ptr2.get()));
                Assert::AreEqual(0ULL, ptr2[0]);
                Assert::AreEqual(0ULL, ptr2[1]);
            }

            TEST_METHOD(SubUIntUInt64)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(2, pool));
                Pointer ptr2(allocate_uint(2, pool));

                ptr[0] = 0ULL;
                ptr[1] = 0ULL;
                Assert::IsFalse(sub_uint_uint64(ptr.get(), 0ULL, 2, ptr2.get()));
                Assert::AreEqual(0ULL, ptr2[0]);
                Assert::AreEqual(0ULL, ptr2[1]);

                ptr[0] = 0ULL;
                ptr[1] = 0ULL;
                Assert::IsTrue(sub_uint_uint64(ptr.get(), 1ULL, 2, ptr2.get()));
                Assert::AreEqual(0xFFFFFFFFFFFFFFFFULL, ptr2[0]);
                Assert::AreEqual(0xFFFFFFFFFFFFFFFFULL, ptr2[1]);

                ptr[0] = 1ULL;
                ptr[1] = 0ULL;
                Assert::IsTrue(sub_uint_uint64(ptr.get(), 2ULL, 2, ptr2.get()));
                Assert::AreEqual(0xFFFFFFFFFFFFFFFFULL, ptr2[0]);
                Assert::AreEqual(0xFFFFFFFFFFFFFFFFULL, ptr2[1]);

                ptr[0] = 0xFFFFFFFF00000000ULL;
                ptr[1] = 0ULL;
                Assert::IsFalse(sub_uint_uint64(ptr.get(), 0xFFFFFFFFULL, 2, ptr2.get()));
                Assert::AreEqual(0xFFFFFFFE00000001ULL, ptr2[0]);
                Assert::AreEqual(0ULL, ptr2[1]);

                ptr[0] = 0xFFFFFFFF00000000ULL;
                ptr[1] = 0xFFFFFFFF00000000ULL;
                Assert::IsFalse(sub_uint_uint64(ptr.get(), 0x100000000ULL, 2, ptr2.get()));
                Assert::AreEqual(0xFFFFFFFE00000000ULL, ptr2[0]);
                Assert::AreEqual(0xFFFFFFFF00000000ULL, ptr2[1]);

                ptr[0] = 0xFFFFFFFFFFFFFFFFULL;
                ptr[1] = 0xFFFFFFFFFFFFFFFFULL;
                Assert::IsFalse(sub_uint_uint64(ptr.get(), 1ULL, 2, ptr2.get()));
                Assert::AreEqual(0xFFFFFFFFFFFFFFFEULL, ptr2[0]);
                Assert::AreEqual(0xFFFFFFFFFFFFFFFFULL, ptr2[1]);
            }

            TEST_METHOD(IncrementUInt)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr1(allocate_uint(2, pool));
                Pointer ptr2(allocate_uint(2, pool));
                ptr1[0] = 0;
                ptr1[1] = 0;
                Assert::IsFalse(increment_uint(ptr1.get(), 2, ptr2.get()) != 0);
                Assert::AreEqual(1ULL, ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);
                Assert::IsFalse(increment_uint(ptr2.get(), 2, ptr1.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(2), ptr1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr1[1]);

                ptr1[0] = 0xFFFFFFFFFFFFFFFF;
                ptr1[1] = 0;
                Assert::IsFalse(increment_uint(ptr1.get(), 2, ptr2.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[0]);
                Assert::AreEqual(1ULL, ptr2[1]);
                Assert::IsFalse(increment_uint(ptr2.get(), 2, ptr1.get()) != 0);
                Assert::AreEqual(1ULL, ptr1[0]);
                Assert::AreEqual(1ULL, ptr1[1]);

                ptr1[0] = 0xFFFFFFFFFFFFFFFF;
                ptr1[1] = 1;
                Assert::IsFalse(increment_uint(ptr1.get(), 2, ptr2.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(2), ptr2[1]);
                Assert::IsFalse(increment_uint(ptr2.get(), 2, ptr1.get()) != 0);
                Assert::AreEqual(1ULL, ptr1[0]);
                Assert::AreEqual(static_cast<uint64_t>(2), ptr1[1]);

                ptr1[0] = 0xFFFFFFFFFFFFFFFE;
                ptr1[1] = 0xFFFFFFFFFFFFFFFF;
                Assert::IsFalse(increment_uint(ptr1.get(), 2, ptr2.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr2[1]);
                Assert::IsTrue(increment_uint(ptr2.get(), 2, ptr1.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr1[1]);
                Assert::IsFalse(increment_uint(ptr1.get(), 2, ptr2.get()) != 0);
                Assert::AreEqual(1ULL, ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);
            }

            TEST_METHOD(DecrementUInt)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr1(allocate_uint(2, pool));
                Pointer ptr2(allocate_uint(2, pool));
                ptr1[0] = 2;
                ptr1[1] = 2;
                Assert::IsFalse(decrement_uint(ptr1.get(), 2, ptr2.get()) != 0);
                Assert::AreEqual(1ULL, ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(2), ptr2[1]);
                Assert::IsFalse(decrement_uint(ptr2.get(), 2, ptr1.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr1[0]);
                Assert::AreEqual(static_cast<uint64_t>(2), ptr1[1]);
                Assert::IsFalse(decrement_uint(ptr1.get(), 2, ptr2.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr2[0]);
                Assert::AreEqual(1ULL, ptr2[1]);
                Assert::IsFalse(decrement_uint(ptr2.get(), 2, ptr1.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), ptr1[0]);
                Assert::AreEqual(1ULL, ptr1[1]);

                ptr1[0] = 2;
                ptr1[1] = 1;
                Assert::IsFalse(decrement_uint(ptr1.get(), 2, ptr2.get()) != 0);
                Assert::AreEqual(1ULL, ptr2[0]);
                Assert::AreEqual(1ULL, ptr2[1]);
                Assert::IsFalse(decrement_uint(ptr2.get(), 2, ptr1.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr1[0]);
                Assert::AreEqual(1ULL, ptr1[1]);
                Assert::IsFalse(decrement_uint(ptr1.get(), 2, ptr2.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);
                Assert::IsFalse(decrement_uint(ptr2.get(), 2, ptr1.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), ptr1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr1[1]);

                ptr1[0] = 2;
                ptr1[1] = 0;
                Assert::IsFalse(decrement_uint(ptr1.get(), 2, ptr2.get()) != 0);
                Assert::AreEqual(1ULL, ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);
                Assert::IsFalse(decrement_uint(ptr2.get(), 2, ptr1.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr1[1]);
                Assert::IsTrue(decrement_uint(ptr1.get(), 2, ptr2.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr2[1]);
                Assert::IsFalse(decrement_uint(ptr2.get(), 2, ptr1.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), ptr1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr1[1]);
            }

            TEST_METHOD(NegateUInt)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(2, pool));
                ptr[0] = 0;
                ptr[1] = 0;
                negate_uint(ptr.get(), 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);

                ptr[0] = 1;
                ptr[1] = 0;
                negate_uint(ptr.get(), 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[1]);
                negate_uint(ptr.get(), 2, ptr.get());
                Assert::AreEqual(1ULL, ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);

                ptr[0] = 2;
                ptr[1] = 0;
                negate_uint(ptr.get(), 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[1]);
                negate_uint(ptr.get(), 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(2), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);

                ptr[0] = 0;
                ptr[1] = 1;
                negate_uint(ptr.get(), 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[1]);
                negate_uint(ptr.get(), 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(1ULL, ptr[1]);

                ptr[0] = 0;
                ptr[1] = 2;
                negate_uint(ptr.get(), 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), ptr[1]);
                negate_uint(ptr.get(), 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(2), ptr[1]);

                ptr[0] = 1;
                ptr[1] = 1;
                negate_uint(ptr.get(), 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), ptr[1]);
                negate_uint(ptr.get(), 2, ptr.get());
                Assert::AreEqual(1ULL, ptr[0]);
                Assert::AreEqual(1ULL, ptr[1]);
            }

            TEST_METHOD(LeftShiftUInt)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(2, pool));
                Pointer ptr2(allocate_uint(2, pool));
                ptr[0] = 0;
                ptr[1] = 0;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                left_shift_uint(ptr.get(), 0, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                left_shift_uint(ptr.get(), 10, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);
                left_shift_uint(ptr.get(), 10, 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);

                ptr[0] = 0x5555555555555555;
                ptr[1] = 0xAAAAAAAAAAAAAAAA;
                left_shift_uint(ptr.get(), 0, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0x5555555555555555), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xAAAAAAAAAAAAAAAA), ptr2[1]);
                left_shift_uint(ptr.get(), 0, 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0x5555555555555555), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xAAAAAAAAAAAAAAAA), ptr[1]);
                left_shift_uint(ptr.get(), 1, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0xAAAAAAAAAAAAAAAA), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x5555555555555554), ptr2[1]);
                left_shift_uint(ptr.get(), 2, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0x5555555555555554), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xAAAAAAAAAAAAAAA9), ptr2[1]);
                left_shift_uint(ptr.get(), 64, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x5555555555555555), ptr2[1]);
                left_shift_uint(ptr.get(), 65, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xAAAAAAAAAAAAAAAA), ptr2[1]);
                left_shift_uint(ptr.get(), 127, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x8000000000000000), ptr2[1]);
                left_shift_uint(ptr.get(), 128, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);

                left_shift_uint(ptr.get(), 2, 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0x5555555555555554), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xAAAAAAAAAAAAAAA9), ptr[1]);
                left_shift_uint(ptr.get(), 64, 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x5555555555555554), ptr[1]);
            }

            TEST_METHOD(RightShiftUInt)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(2, pool));
                Pointer ptr2(allocate_uint(2, pool));
                ptr[0] = 0;
                ptr[1] = 0;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                right_shift_uint(ptr.get(), 0, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                right_shift_uint(ptr.get(), 10, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);
                right_shift_uint(ptr.get(), 10, 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);

                ptr[0] = 0x5555555555555555;
                ptr[1] = 0xAAAAAAAAAAAAAAAA;
                right_shift_uint(ptr.get(), 0, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0x5555555555555555), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xAAAAAAAAAAAAAAAA), ptr2[1]);
                right_shift_uint(ptr.get(), 0, 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0x5555555555555555), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xAAAAAAAAAAAAAAAA), ptr[1]);
                right_shift_uint(ptr.get(), 1, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0x2AAAAAAAAAAAAAAA), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x5555555555555555), ptr2[1]);
                right_shift_uint(ptr.get(), 2, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0x9555555555555555), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x2AAAAAAAAAAAAAAA), ptr2[1]);
                right_shift_uint(ptr.get(), 64, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0xAAAAAAAAAAAAAAAA), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);
                right_shift_uint(ptr.get(), 65, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0x5555555555555555), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);
                right_shift_uint(ptr.get(), 127, 2, ptr2.get());
                Assert::AreEqual(1ULL, ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);
                right_shift_uint(ptr.get(), 128, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);

                right_shift_uint(ptr.get(), 2, 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0x9555555555555555), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x2AAAAAAAAAAAAAAA), ptr[1]);
                right_shift_uint(ptr.get(), 64, 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0x2AAAAAAAAAAAAAAA), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);
            }

            TEST_METHOD(HalfRoundUpUInt)
            {
                half_round_up_uint(nullptr, 0, nullptr);

                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(2, pool));
                Pointer ptr2(allocate_uint(2, pool));
                ptr[0] = 0;
                ptr[1] = 0;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                half_round_up_uint(ptr.get(), 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);
                half_round_up_uint(ptr.get(), 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);

                ptr[0] = 1;
                ptr[1] = 0;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                half_round_up_uint(ptr.get(), 2, ptr2.get());
                Assert::AreEqual(1ULL, ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);
                half_round_up_uint(ptr.get(), 2, ptr.get());
                Assert::AreEqual(1ULL, ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);

                ptr[0] = 2;
                ptr[1] = 0;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                half_round_up_uint(ptr.get(), 2, ptr2.get());
                Assert::AreEqual(1ULL, ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);
                half_round_up_uint(ptr.get(), 2, ptr.get());
                Assert::AreEqual(1ULL, ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);

                ptr[0] = 3;
                ptr[1] = 0;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                half_round_up_uint(ptr.get(), 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(2), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);

                ptr[0] = 4;
                ptr[1] = 0;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                half_round_up_uint(ptr.get(), 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(2), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                half_round_up_uint(ptr.get(), 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x8000000000000000), ptr2[1]);
                half_round_up_uint(ptr.get(), 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x8000000000000000), ptr[1]);
            }

            TEST_METHOD(NotUInt)
            {
                not_uint(nullptr, 0, nullptr);

                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(2, pool));
                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0;
                not_uint(ptr.get(), 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[1]);

                ptr[0] = 0xFFFFFFFF00000000;
                ptr[1] = 0xFFFF0000FFFF0000;
                not_uint(ptr.get(), 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0x00000000FFFFFFFF), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x0000FFFF0000FFFF), ptr[1]);
            }

            TEST_METHOD(AndUIntUInt)
            {
                and_uint_uint(nullptr, nullptr, 0, nullptr);

                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(2, pool));
                Pointer ptr2(allocate_uint(2, pool));
                Pointer ptr3(allocate_uint(2, pool));
                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0;
                ptr2[0] = 0;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                ptr3[0] = 0xFFFFFFFFFFFFFFFF;
                ptr3[1] = 0xFFFFFFFFFFFFFFFF;
                and_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[1]);

                ptr[0] = 0xFFFFFFFF00000000;
                ptr[1] = 0xFFFF0000FFFF0000;
                ptr2[0] = 0x0000FFFF0000FFFF;
                ptr2[1] = 0xFF00FF00FF00FF00;
                ptr3[0] = 0;
                ptr3[1] = 0;
                and_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get());
                Assert::AreEqual(static_cast<uint64_t>(0x0000FFFF00000000), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFF000000FF000000), ptr3[1]);
                and_uint_uint(ptr.get(), ptr2.get(), 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0x0000FFFF00000000), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFF000000FF000000), ptr[1]);
            }

            TEST_METHOD(OrUIntUInt)
            {
                or_uint_uint(nullptr, nullptr, 0, nullptr);

                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(2, pool));
                Pointer ptr2(allocate_uint(2, pool));
                Pointer ptr3(allocate_uint(2, pool));
                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0;
                ptr2[0] = 0;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                ptr3[0] = 0;
                ptr3[1] = 0;
                or_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get());
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[1]);

                ptr[0] = 0xFFFFFFFF00000000;
                ptr[1] = 0xFFFF0000FFFF0000;
                ptr2[0] = 0x0000FFFF0000FFFF;
                ptr2[1] = 0xFF00FF00FF00FF00;
                ptr3[0] = 0;
                ptr3[1] = 0;
                or_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get());
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFF0000FFFF), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFF00FFFFFF00), ptr3[1]);
                or_uint_uint(ptr.get(), ptr2.get(), 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFF0000FFFF), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFF00FFFFFF00), ptr[1]);
            }

            TEST_METHOD(XorUIntUInt)
            {
                xor_uint_uint(nullptr, nullptr, 0, nullptr);

                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(2, pool));
                Pointer ptr2(allocate_uint(2, pool));
                Pointer ptr3(allocate_uint(2, pool));
                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0;
                ptr2[0] = 0;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                ptr3[0] = 0;
                ptr3[1] = 0;
                xor_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get());
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[1]);

                ptr[0] = 0xFFFFFFFF00000000;
                ptr[1] = 0xFFFF0000FFFF0000;
                ptr2[0] = 0x0000FFFF0000FFFF;
                ptr2[1] = 0xFF00FF00FF00FF00;
                ptr3[0] = 0;
                ptr3[1] = 0;
                xor_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get());
                Assert::AreEqual(static_cast<uint64_t>(0xFFFF00000000FFFF), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x00FFFF0000FFFF00), ptr3[1]);
                xor_uint_uint(ptr.get(), ptr2.get(), 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0xFFFF00000000FFFF), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x00FFFF0000FFFF00), ptr[1]);
            }

            TEST_METHOD(MultiplyUInt64Generic)
            {
                uint64_t result[2];

                multiply_uint64_generic(0ULL, 0ULL, result);
                Assert::AreEqual(0ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);
                multiply_uint64_generic(0ULL, 1ULL, result);
                Assert::AreEqual(0ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);
                multiply_uint64_generic(1ULL, 0ULL, result);
                Assert::AreEqual(0ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);
                multiply_uint64_generic(1ULL, 1ULL, result);
                Assert::AreEqual(1ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);
                multiply_uint64_generic(0x100000000, 0xFAFABABA, result);
                Assert::AreEqual(0xFAFABABA00000000ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);
                multiply_uint64_generic(0x1000000000, 0xFAFABABA, result);
                Assert::AreEqual(0xAFABABA000000000ULL, result[0]);
                Assert::AreEqual(0xFULL, result[1]);
                multiply_uint64_generic(1111222233334444ULL, 5555666677778888ULL, result);
                Assert::AreEqual(4140785562324247136ULL, result[0]);
                Assert::AreEqual(334670460471ULL, result[1]);
            }

            TEST_METHOD(MultiplyUInt64)
            {
                uint64_t result[2];

                multiply_uint64(0ULL, 0ULL, result);
                Assert::AreEqual(0ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);
                multiply_uint64(0ULL, 1ULL, result);
                Assert::AreEqual(0ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);
                multiply_uint64(1ULL, 0ULL, result);
                Assert::AreEqual(0ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);
                multiply_uint64(1ULL, 1ULL, result);
                Assert::AreEqual(1ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);
                multiply_uint64(0x100000000, 0xFAFABABA, result);
                Assert::AreEqual(0xFAFABABA00000000ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);
                multiply_uint64(0x1000000000, 0xFAFABABA, result);
                Assert::AreEqual(0xAFABABA000000000ULL, result[0]);
                Assert::AreEqual(0xFULL, result[1]);
                multiply_uint64(1111222233334444ULL, 5555666677778888ULL, result);
                Assert::AreEqual(4140785562324247136ULL, result[0]);
                Assert::AreEqual(334670460471ULL, result[1]);
            }

            TEST_METHOD(MultiplyUInt64HW64Generic)
            {
                uint64_t result;

                multiply_uint64_hw64_generic(0ULL, 0ULL, &result);
                Assert::AreEqual(0ULL, result);
                multiply_uint64_hw64_generic(0ULL, 1ULL, &result);
                Assert::AreEqual(0ULL, result);
                multiply_uint64_hw64_generic(1ULL, 0ULL, &result);
                Assert::AreEqual(0ULL, result);
                multiply_uint64_hw64_generic(1ULL, 1ULL, &result);
                Assert::AreEqual(0ULL, result);
                multiply_uint64_hw64_generic(0x100000000, 0xFAFABABA, &result);
                Assert::AreEqual(0ULL, result);
                multiply_uint64_hw64_generic(0x1000000000, 0xFAFABABA, &result);
                Assert::AreEqual(0xFULL, result);
                multiply_uint64_hw64_generic(1111222233334444ULL, 5555666677778888ULL, &result);
                Assert::AreEqual(334670460471ULL, result);
            }

            TEST_METHOD(MultiplyUInt64HW64)
            {
                uint64_t result;

                multiply_uint64_hw64(0ULL, 0ULL, &result);
                Assert::AreEqual(0ULL, result);
                multiply_uint64_hw64(0ULL, 1ULL, &result);
                Assert::AreEqual(0ULL, result);
                multiply_uint64_hw64(1ULL, 0ULL, &result);
                Assert::AreEqual(0ULL, result);
                multiply_uint64_hw64(1ULL, 1ULL, &result);
                Assert::AreEqual(0ULL, result);
                multiply_uint64_hw64(0x100000000, 0xFAFABABA, &result);
                Assert::AreEqual(0ULL, result);
                multiply_uint64_hw64(0x1000000000, 0xFAFABABA, &result);
                Assert::AreEqual(0xFULL, result);
                multiply_uint64_hw64(1111222233334444ULL, 5555666677778888ULL, &result);
                Assert::AreEqual(334670460471ULL, result);
            }

            TEST_METHOD(MultiplyUIntUInt)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(2, pool));
                Pointer ptr2(allocate_uint(2, pool));
                Pointer ptr3(allocate_uint(4, pool));
                ptr[0] = 0;
                ptr[1] = 0;
                ptr2[0] = 0;
                ptr2[1] = 0;
                ptr3[0] = 0xFFFFFFFFFFFFFFFF;
                ptr3[1] = 0xFFFFFFFFFFFFFFFF;
                ptr3[2] = 0xFFFFFFFFFFFFFFFF;
                ptr3[3] = 0xFFFFFFFFFFFFFFFF;
                multiply_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[3]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                ptr2[0] = 0;
                ptr2[1] = 0;
                ptr3[0] = 0xFFFFFFFFFFFFFFFF;
                ptr3[1] = 0xFFFFFFFFFFFFFFFF;
                ptr3[2] = 0xFFFFFFFFFFFFFFFF;
                ptr3[3] = 0xFFFFFFFFFFFFFFFF;
                multiply_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[3]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                ptr2[0] = 1;
                ptr2[1] = 0;
                ptr3[0] = 0;
                ptr3[1] = 0;
                ptr3[2] = 0;
                ptr3[3] = 0;
                multiply_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get());
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[3]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                ptr2[0] = 0;
                ptr2[1] = 1;
                ptr3[0] = 0;
                ptr3[1] = 0;
                ptr3[2] = 0;
                ptr3[3] = 0;
                multiply_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[1]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[3]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                ptr3[0] = 0;
                ptr3[1] = 0;
                ptr3[2] = 0;
                ptr3[3] = 0;
                multiply_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get());
                Assert::AreEqual(1ULL, ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[1]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), ptr3[2]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[3]);

                ptr[0] = 9756571004902751654;
                ptr[1] = 731952007397389984;
                ptr2[0] = 701538366196406307;
                ptr2[1] = 1699883529753102283;
                ptr3[0] = 0;
                ptr3[1] = 0;
                ptr3[2] = 0;
                ptr3[3] = 0;
                multiply_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get());
                Assert::AreEqual(static_cast<uint64_t>(9585656442714717618), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(1817697005049051848), ptr3[1]);
                Assert::AreEqual(static_cast<uint64_t>(14447416709120365380), ptr3[2]);
                Assert::AreEqual(static_cast<uint64_t>(67450014862939159), ptr3[3]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                ptr3[0] = 0;
                ptr3[1] = 0;
                ptr3[2] = 0;
                ptr3[3] = 0;
                multiply_uint_uint(ptr.get(), 2, ptr2.get(), 1, 2, ptr3.get());
                Assert::AreEqual(1ULL, ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[3]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                ptr3[0] = 0;
                ptr3[1] = 0;
                ptr3[2] = 0;
                ptr3[3] = 0;
                multiply_uint_uint(ptr.get(), 2, ptr2.get(), 1, 3, ptr3.get());
                Assert::AreEqual(1ULL, ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[1]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), ptr3[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[3]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                ptr3[0] = 0;
                ptr3[1] = 0;
                ptr3[2] = 0;
                ptr3[3] = 0;
                multiply_truncate_uint_uint(ptr.get(), ptr2.get(), 2, ptr3.get());
                Assert::AreEqual(1ULL, ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[3]);
            }

            TEST_METHOD(MultiplyUIntUInt64)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(3, pool));
                Pointer result(allocate_uint(4, pool));

                ptr[0] = 0;
                ptr[1] = 0;
                ptr[2] = 0;
                multiply_uint_uint64(ptr.get(), 3, 0ULL, 4, result.get());
                Assert::AreEqual(0ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);
                Assert::AreEqual(0ULL, result[2]);
                Assert::AreEqual(0ULL, result[3]);

                ptr[0] = 0xFFFFFFFFF;
                ptr[1] = 0xAAAAAAAAA;
                ptr[2] = 0x111111111;
                multiply_uint_uint64(ptr.get(), 3, 0ULL, 4, result.get());
                Assert::AreEqual(0ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);
                Assert::AreEqual(0ULL, result[2]);
                Assert::AreEqual(0ULL, result[3]);

                ptr[0] = 0xFFFFFFFFF;
                ptr[1] = 0xAAAAAAAAA;
                ptr[2] = 0x111111111;
                multiply_uint_uint64(ptr.get(), 3, 1ULL, 4, result.get());
                Assert::AreEqual(0xFFFFFFFFFULL, result[0]);
                Assert::AreEqual(0xAAAAAAAAAULL, result[1]);
                Assert::AreEqual(0x111111111ULL, result[2]);
                Assert::AreEqual(0ULL, result[3]);

                ptr[0] = 0xFFFFFFFFF;
                ptr[1] = 0xAAAAAAAAA;
                ptr[2] = 0x111111111;
                multiply_uint_uint64(ptr.get(), 3, 0x10000ULL, 4, result.get());
                Assert::AreEqual(0xFFFFFFFFF0000ULL, result[0]);
                Assert::AreEqual(0xAAAAAAAAA0000ULL, result[1]);
                Assert::AreEqual(0x1111111110000ULL, result[2]);
                Assert::AreEqual(0ULL, result[3]);

                ptr[0] = 0xFFFFFFFFF;
                ptr[1] = 0xAAAAAAAAA;
                ptr[2] = 0x111111111;
                multiply_uint_uint64(ptr.get(), 3, 0x100000000ULL, 4, result.get());
                Assert::AreEqual(0xFFFFFFFF00000000ULL, result[0]);
                Assert::AreEqual(0xAAAAAAAA0000000FULL, result[1]);
                Assert::AreEqual(0x111111110000000AULL, result[2]);
                Assert::AreEqual(1ULL, result[3]);

                ptr[0] = 5656565656565656ULL;
                ptr[1] = 3434343434343434ULL;
                ptr[2] = 1212121212121212ULL;
                multiply_uint_uint64(ptr.get(), 3, 7878787878787878ULL, 4, result.get());
                Assert::AreEqual(8891370032116156560ULL, result[0]);
                Assert::AreEqual(127835914414679452ULL, result[1]);
                Assert::AreEqual(9811042505314082702ULL, result[2]);
                Assert::AreEqual(517709026347ULL, result[3]);
            }

            TEST_METHOD(DivideUIntUInt)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                divide_uint_uint_inplace(nullptr, nullptr, 0, nullptr, pool);
                divide_uint_uint(nullptr, nullptr, 0, nullptr, nullptr, pool);

                Pointer ptr(allocate_uint(4, pool));
                Pointer ptr2(allocate_uint(4, pool));
                Pointer ptr3(allocate_uint(4, pool));
                Pointer ptr4(allocate_uint(4, pool));
                ptr[0] = 0;
                ptr[1] = 0;
                ptr2[0] = 0;
                ptr2[1] = 1;
                ptr3[0] = 0xFFFFFFFFFFFFFFFF;
                ptr3[1] = 0xFFFFFFFFFFFFFFFF;
                divide_uint_uint_inplace(ptr.get(), ptr2.get(), 2, ptr3.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[1]);

                ptr[0] = 0;
                ptr[1] = 0;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                ptr3[0] = 0xFFFFFFFFFFFFFFFF;
                ptr3[1] = 0xFFFFFFFFFFFFFFFF;
                divide_uint_uint_inplace(ptr.get(), ptr2.get(), 2, ptr3.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[1]);

                ptr[0] = 0xFFFFFFFFFFFFFFFE;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                ptr3[0] = 0xFFFFFFFFFFFFFFFF;
                ptr3[1] = 0xFFFFFFFFFFFFFFFF;
                divide_uint_uint_inplace(ptr.get(), ptr2.get(), 2, ptr3.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[1]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                ptr3[0] = 0xFFFFFFFFFFFFFFFF;
                ptr3[1] = 0xFFFFFFFFFFFFFFFF;
                divide_uint_uint_inplace(ptr.get(), ptr2.get(), 2, ptr3.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);
                Assert::AreEqual(1ULL, ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[1]);

                ptr[0] = 14;
                ptr[1] = 0;
                ptr2[0] = 3;
                ptr2[1] = 0;
                ptr3[0] = 0xFFFFFFFFFFFFFFFF;
                ptr3[1] = 0xFFFFFFFFFFFFFFFF;
                divide_uint_uint_inplace(ptr.get(), ptr2.get(), 2, ptr3.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(2), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);
                Assert::AreEqual(static_cast<uint64_t>(4), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[1]);

                ptr[0] = 9585656442714717620;
                ptr[1] = 1817697005049051848;
                ptr[2] = 14447416709120365380;
                ptr[3] = 67450014862939159;
                ptr2[0] = 701538366196406307;
                ptr2[1] = 1699883529753102283;
                ptr2[2] = 0;
                ptr2[3] = 0;
                ptr3[0] = 0xFFFFFFFFFFFFFFFF;
                ptr3[1] = 0xFFFFFFFFFFFFFFFF;
                ptr3[2] = 0xFFFFFFFFFFFFFFFF;
                ptr3[3] = 0xFFFFFFFFFFFFFFFF;
                ptr4[0] = 0xFFFFFFFFFFFFFFFF;
                ptr4[1] = 0xFFFFFFFFFFFFFFFF;
                ptr4[2] = 0xFFFFFFFFFFFFFFFF;
                ptr4[3] = 0xFFFFFFFFFFFFFFFF;
                divide_uint_uint(ptr.get(), ptr2.get(), 4, ptr3.get(), ptr4.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(2), ptr4[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr4[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr4[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr4[3]);
                Assert::AreEqual(static_cast<uint64_t>(9756571004902751654), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(731952007397389984), ptr3[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[3]);

                divide_uint_uint_inplace(ptr.get(), ptr2.get(), 4, ptr3.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(2), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[3]);
                Assert::AreEqual(static_cast<uint64_t>(9756571004902751654), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(731952007397389984), ptr3[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[3]);
            }

            TEST_METHOD(DivideUInt128UInt64)
            {
                uint64_t input[2];
                uint64_t quotient[2];

                input[0] = 0;
                input[1] = 0;
                divide_uint128_uint64_inplace(input, 1ULL, quotient);
                Assert::AreEqual(0ULL, input[0]);
                Assert::AreEqual(0ULL, input[1]);
                Assert::AreEqual(0ULL, quotient[0]);
                Assert::AreEqual(0ULL, quotient[1]);

                input[0] = 1;
                input[1] = 0;
                divide_uint128_uint64_inplace(input, 1ULL, quotient);
                Assert::AreEqual(0ULL, input[0]);
                Assert::AreEqual(0ULL, input[1]);
                Assert::AreEqual(1ULL, quotient[0]);
                Assert::AreEqual(0ULL, quotient[1]);

                input[0] = 0x10101010;
                input[1] = 0x2B2B2B2B;
                divide_uint128_uint64_inplace(input, 0x1000ULL, quotient);
                Assert::AreEqual(0x10ULL, input[0]);
                Assert::AreEqual(0ULL, input[1]);
                Assert::AreEqual(0xB2B0000000010101ULL, quotient[0]);
                Assert::AreEqual(0x2B2B2ULL, quotient[1]);

                input[0] = 1212121212121212ULL;
                input[1] = 3434343434343434ULL;
                divide_uint128_uint64_inplace(input, 5656565656565656ULL, quotient);
                Assert::AreEqual(5252525252525252ULL, input[0]);
                Assert::AreEqual(0ULL, input[1]);
                Assert::AreEqual(11199808901895084909ULL, quotient[0]);
                Assert::AreEqual(0ULL, quotient[1]);
            }

            TEST_METHOD(DivideUInt192UInt64)
            {
                uint64_t input[3];
                uint64_t quotient[3];

                input[0] = 0;
                input[1] = 0;
                input[2] = 0;
                divide_uint192_uint64_inplace(input, 1ULL, quotient);
                Assert::AreEqual(0ULL, input[0]);
                Assert::AreEqual(0ULL, input[1]);
                Assert::AreEqual(0ULL, input[2]);
                Assert::AreEqual(0ULL, quotient[0]);
                Assert::AreEqual(0ULL, quotient[1]);
                Assert::AreEqual(0ULL, quotient[2]);

                input[0] = 1;
                input[1] = 0;
                input[2] = 0;
                divide_uint192_uint64_inplace(input, 1ULL, quotient);
                Assert::AreEqual(0ULL, input[0]);
                Assert::AreEqual(0ULL, input[1]);
                Assert::AreEqual(0ULL, input[2]);
                Assert::AreEqual(1ULL, quotient[0]);
                Assert::AreEqual(0ULL, quotient[1]);
                Assert::AreEqual(0ULL, quotient[2]);

                input[0] = 0x10101010;
                input[1] = 0x2B2B2B2B;
                input[2] = 0xF1F1F1F1;
                divide_uint192_uint64_inplace(input, 0x1000ULL, quotient);
                Assert::AreEqual(0x10ULL, input[0]);
                Assert::AreEqual(0ULL, input[1]);
                Assert::AreEqual(0ULL, input[2]);
                Assert::AreEqual(0xB2B0000000010101ULL, quotient[0]);
                Assert::AreEqual(0x1F1000000002B2B2ULL, quotient[1]);
                Assert::AreEqual(0xF1F1FULL, quotient[2]);

                input[0] = 1212121212121212ULL;
                input[1] = 3434343434343434ULL;
                input[2] = 5656565656565656ULL;
                divide_uint192_uint64_inplace(input, 7878787878787878ULL, quotient);
                Assert::AreEqual(7272727272727272ULL, input[0]);
                Assert::AreEqual(0ULL, input[1]);
                Assert::AreEqual(0ULL, input[2]);
                Assert::AreEqual(17027763760347278414ULL, quotient[0]);
                Assert::AreEqual(13243816258047883211ULL, quotient[1]);
                Assert::AreEqual(0ULL, quotient[2]);
            }

            TEST_METHOD(ExponentiateUInt)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer input(allocate_zero_uint(2, pool));
                Pointer result(allocate_zero_uint(8, pool));

                result[0] = 1, result[1] = 2, result[2] = 3, result[3] = 4;
                result[4] = 5, result[5] = 6, result[6] = 7, result[7] = 8;

                uint64_t exponent[2]{ 0 };
                
                input[0] = 0xFFF;
                input[1] = 0;
                exponentiate_uint(input.get(), 2, exponent, 1, 1, result.get(), pool);
                Assert::AreEqual(1ULL, result[0]);
                Assert::AreEqual(2ULL, result[1]);

                exponentiate_uint(input.get(), 2, exponent, 1, 2, result.get(), pool);
                Assert::AreEqual(1ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);

                exponentiate_uint(input.get(), 1, exponent, 1, 4, result.get(), pool);
                Assert::AreEqual(1ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);
                Assert::AreEqual(0ULL, result[2]);
                Assert::AreEqual(0ULL, result[3]);

                input[0] = 123;
                exponent[0] = 5;
                exponentiate_uint(input.get(), 1, exponent, 2, 2, result.get(), pool);
                Assert::AreEqual(28153056843ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);

                input[0] = 1;
                exponent[0] = 1;
                exponent[1] = 1;
                exponentiate_uint(input.get(), 1, exponent, 2, 2, result.get(), pool);
                Assert::AreEqual(1ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);

                input[0] = 0;
                input[1] = 1;
                exponent[0] = 7;
                exponent[1] = 0;
                exponentiate_uint(input.get(), 2, exponent, 2, 8, result.get(), pool);
                Assert::AreEqual(0ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);
                Assert::AreEqual(0ULL, result[2]);
                Assert::AreEqual(0ULL, result[3]);
                Assert::AreEqual(0ULL, result[4]);
                Assert::AreEqual(0ULL, result[5]);
                Assert::AreEqual(0ULL, result[6]);
                Assert::AreEqual(1ULL, result[7]);

                input[0] = 121212;
                input[1] = 343434;
                exponent[0] = 3;
                exponent[1] = 0;
                exponentiate_uint(input.get(), 2, exponent, 2, 8, result.get(), pool);
                Assert::AreEqual(1780889000200128ULL, result[0]);
                Assert::AreEqual(15137556501701088ULL, result[1]);
                Assert::AreEqual(42889743421486416ULL, result[2]);
                Assert::AreEqual(40506979898070504ULL, result[3]);
                Assert::AreEqual(0ULL, result[4]);
                Assert::AreEqual(0ULL, result[5]);
                Assert::AreEqual(0ULL, result[6]);
                Assert::AreEqual(0ULL, result[7]);
            }

            TEST_METHOD(ExponentiateUInt64)
            {
                Assert::AreEqual(0ULL, exponentiate_uint64(0ULL, 1ULL));
                Assert::AreEqual(1ULL, exponentiate_uint64(1ULL, 0ULL));
                Assert::AreEqual(0ULL, exponentiate_uint64(0ULL, 0xFFFFFFFFFFFFFFFFULL));
                Assert::AreEqual(1ULL, exponentiate_uint64(0xFFFFFFFFFFFFFFFFULL, 0ULL));
                Assert::AreEqual(25ULL, exponentiate_uint64(5ULL, 2ULL));
                Assert::AreEqual(32ULL, exponentiate_uint64(2ULL, 5ULL));
                Assert::AreEqual(0x1000000000000000ULL, exponentiate_uint64(0x10ULL, 15ULL));
                Assert::AreEqual(0ULL, exponentiate_uint64(0x10ULL, 16ULL));
                Assert::AreEqual(12389286314587456613ULL, exponentiate_uint64(123456789ULL, 13ULL));
            }
        };
    }
}