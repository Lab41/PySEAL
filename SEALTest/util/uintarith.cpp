#include "CppUnitTest.h"
#include "util/uintarith.h"
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
            TEST_METHOD(IncrementUInt)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer ptr(allocate_uint(2, pool));
                ptr[0] = 0;
                ptr[1] = 0;
                Assert::IsFalse(increment_uint(ptr.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(1), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);
                Assert::IsFalse(increment_uint(ptr.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(2), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0;
                Assert::IsFalse(increment_uint(ptr.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(1), ptr[1]);
                Assert::IsFalse(increment_uint(ptr.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(1), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(1), ptr[1]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 1;
                Assert::IsFalse(increment_uint(ptr.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(2), ptr[1]);
                Assert::IsFalse(increment_uint(ptr.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(1), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(2), ptr[1]);

                ptr[0] = 0xFFFFFFFFFFFFFFFE;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                Assert::IsFalse(increment_uint(ptr.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[1]);
                Assert::IsTrue(increment_uint(ptr.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);
                Assert::IsFalse(increment_uint(ptr.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(1), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);
            }

            TEST_METHOD(DecrementUInt)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer ptr(allocate_uint(2, pool));
                ptr[0] = 2;
                ptr[1] = 2;
                Assert::IsFalse(decrement_uint(ptr.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(1), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(2), ptr[1]);
                Assert::IsFalse(decrement_uint(ptr.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(2), ptr[1]);
                Assert::IsFalse(decrement_uint(ptr.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(1), ptr[1]);
                Assert::IsFalse(decrement_uint(ptr.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(1), ptr[1]);

                ptr[0] = 2;
                ptr[1] = 1;
                Assert::IsFalse(decrement_uint(ptr.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(1), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(1), ptr[1]);
                Assert::IsFalse(decrement_uint(ptr.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(1), ptr[1]);
                Assert::IsFalse(decrement_uint(ptr.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);
                Assert::IsFalse(decrement_uint(ptr.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);

                ptr[0] = 2;
                ptr[1] = 0;
                Assert::IsFalse(decrement_uint(ptr.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(1), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);
                Assert::IsFalse(decrement_uint(ptr.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);
                Assert::IsTrue(decrement_uint(ptr.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[1]);
                Assert::IsFalse(decrement_uint(ptr.get(), 2, ptr.get()) != 0);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[1]);
            }

            TEST_METHOD(NegateUInt)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
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
                Assert::AreEqual(static_cast<uint64_t>(1), ptr[0]);
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
                Assert::AreEqual(static_cast<uint64_t>(1), ptr[1]);

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
                Assert::AreEqual(static_cast<uint64_t>(1), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(1), ptr[1]);
            }

            TEST_METHOD(LeftShiftUInt)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
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
                MemoryPool &pool = *MemoryPool::default_pool();
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
                Assert::AreEqual(static_cast<uint64_t>(1), ptr2[0]);
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

            TEST_METHOD(RightShiftSignExtendUInt)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer ptr(allocate_uint(2, pool));
                Pointer ptr2(allocate_uint(2, pool));
                ptr[0] = 0;
                ptr[1] = 0;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                right_shift_sign_extend_uint(ptr.get(), 0, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                right_shift_sign_extend_uint(ptr.get(), 10, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);
                right_shift_sign_extend_uint(ptr.get(), 10, 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);

                ptr[0] = 0x5555555555555555;
                ptr[1] = 0xAAAAAAAAAAAAAAAA;
                right_shift_sign_extend_uint(ptr.get(), 0, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0x5555555555555555), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xAAAAAAAAAAAAAAAA), ptr2[1]);
                right_shift_sign_extend_uint(ptr.get(), 0, 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0x5555555555555555), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xAAAAAAAAAAAAAAAA), ptr[1]);
                right_shift_sign_extend_uint(ptr.get(), 1, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0x2AAAAAAAAAAAAAAA), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xD555555555555555), ptr2[1]);
                right_shift_sign_extend_uint(ptr.get(), 2, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0x9555555555555555), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xEAAAAAAAAAAAAAAA), ptr2[1]);
                right_shift_sign_extend_uint(ptr.get(), 64, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0xAAAAAAAAAAAAAAAA), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr2[1]);
                right_shift_sign_extend_uint(ptr.get(), 65, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0xD555555555555555), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr2[1]);
                right_shift_sign_extend_uint(ptr.get(), 127, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr2[1]);
                right_shift_sign_extend_uint(ptr.get(), 128, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr2[1]);

                right_shift_sign_extend_uint(ptr.get(), 2, 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0x9555555555555555), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xEAAAAAAAAAAAAAAA), ptr[1]);
                right_shift_sign_extend_uint(ptr.get(), 64, 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0xEAAAAAAAAAAAAAAA), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[1]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0x7FFFFFFFFFFFFFFF;
                right_shift_sign_extend_uint(ptr.get(), 0, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x7FFFFFFFFFFFFFFF), ptr2[1]);
                right_shift_sign_extend_uint(ptr.get(), 1, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x3FFFFFFFFFFFFFFF), ptr2[1]);
                right_shift_sign_extend_uint(ptr.get(), 64, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0x7FFFFFFFFFFFFFFF), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);
                right_shift_sign_extend_uint(ptr.get(), 126, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(1), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);
                right_shift_sign_extend_uint(ptr.get(), 127, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);
                right_shift_sign_extend_uint(ptr.get(), 128, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);
            }

            TEST_METHOD(HalfRoundUpUInt)
            {
                half_round_up_uint(nullptr, 0, nullptr);

                MemoryPool &pool = *MemoryPool::default_pool();
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
                Assert::AreEqual(static_cast<uint64_t>(1), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);
                half_round_up_uint(ptr.get(), 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(1), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);

                ptr[0] = 2;
                ptr[1] = 0;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                half_round_up_uint(ptr.get(), 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(1), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);
                half_round_up_uint(ptr.get(), 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(1), ptr[0]);
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

                MemoryPool &pool = *MemoryPool::default_pool();
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

                MemoryPool &pool = *MemoryPool::default_pool();
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

                MemoryPool &pool = *MemoryPool::default_pool();
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

                MemoryPool &pool = *MemoryPool::default_pool();
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

            TEST_METHOD(AddUIntUInt)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
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
                Assert::AreEqual(static_cast<uint64_t>(1), ptr3[1]);

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
                Assert::AreEqual(static_cast<uint64_t>(1), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(6), ptr3[1]);
            }

            TEST_METHOD(SubUIntUInt)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
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

            TEST_METHOD(MultiplyUIntUInt)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
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
                Assert::AreEqual(static_cast<uint64_t>(1), ptr3[0]);
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
                Assert::AreEqual(static_cast<uint64_t>(1), ptr3[0]);
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
                Assert::AreEqual(static_cast<uint64_t>(1), ptr3[0]);
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
                Assert::AreEqual(static_cast<uint64_t>(1), ptr3[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr3[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr3[3]);
            }

            TEST_METHOD(DivideUIntUInt)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
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
                Assert::AreEqual(static_cast<uint64_t>(1), ptr3[0]);
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
        };
    }
}