#include "CppUnitTest.h"
#include "seal/util/uintcore.h"
#include <cstdint>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal::util;
using namespace std;

namespace SEALTest
{
    namespace util
    {
        TEST_CLASS(UIntCore)
        {
        public:
            TEST_METHOD(AllocateUInt)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(0, pool));
                Assert::IsTrue(nullptr == ptr.get());

                ptr = allocate_uint(1, pool);
                Assert::IsTrue(nullptr != ptr.get());

                ptr = allocate_uint(2, pool);
                Assert::IsTrue(nullptr != ptr.get());
            }

            TEST_METHOD(SetZeroUInt)
            {
                set_zero_uint(0, nullptr);

                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(1, pool));
                ptr[0] = 0x1234567812345678;
                set_zero_uint(1, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);

                ptr = allocate_uint(2, pool);
                ptr[0] = 0x1234567812345678;
                ptr[1] = 0x1234567812345678;
                set_zero_uint(2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);
            }

            TEST_METHOD(AllocateZeroUInt)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_zero_uint(0, pool));
                Assert::IsTrue(nullptr == ptr.get());

                ptr = allocate_zero_uint(1, pool);
                Assert::IsTrue(nullptr != ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);

                ptr = allocate_zero_uint(2, pool);
                Assert::IsTrue(nullptr != ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);
            }

            TEST_METHOD(SetUInt)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(1, pool));
                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                set_uint(1, 1, ptr.get());
                Assert::AreEqual(1ULL, ptr[0]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                set_uint(0x1234567812345678, 1, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0x1234567812345678), ptr[0]);

                ptr = allocate_uint(2, pool);
                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                set_uint(1, 2, ptr.get());
                Assert::AreEqual(1ULL, ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                set_uint(0x1234567812345678, 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0x1234567812345678), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);
            }

            TEST_METHOD(SetUIntUInt)
            {
                set_uint_uint(nullptr, 0, nullptr);

                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr1(allocate_uint(1, pool));
                ptr1[0] = 0x1234567887654321;
                Pointer ptr2(allocate_uint(1, pool));
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                set_uint_uint(ptr1.get(), 1, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0x1234567887654321), ptr2[0]);

                ptr1[0] = 0x1231231231231231;
                set_uint_uint(ptr1.get(), 1, ptr1.get());
                Assert::AreEqual(static_cast<uint64_t>(0x1231231231231231), ptr1[0]);

                ptr1 = allocate_uint(2, pool);
                ptr2 = allocate_uint(2, pool);
                ptr1[0] = 0x1234567887654321;
                ptr1[1] = 0x8765432112345678;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                set_uint_uint(ptr1.get(), 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0x1234567887654321), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x8765432112345678), ptr2[1]);

                ptr1[0] = 0x1231231231231321;
                ptr1[1] = 0x3213213213213211;
                set_uint_uint(ptr1.get(), 2, ptr1.get());
                Assert::AreEqual(static_cast<uint64_t>(0x1231231231231321), ptr1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x3213213213213211), ptr1[1]);
            }

            TEST_METHOD(SetUIntUInt2)
            {
                set_uint_uint(nullptr, 0, 0, nullptr);

                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr1(allocate_uint(1, pool));
                ptr1[0] = 0x1234567887654321;
                set_uint_uint(nullptr, 0, 1, ptr1.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr1[0]);

                Pointer ptr2(allocate_uint(1, pool));
                ptr1[0] = 0x1234567887654321;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                set_uint_uint(ptr1.get(), 1, 1, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0x1234567887654321), ptr2[0]);

                ptr1[0] = 0x1231231231231231;
                set_uint_uint(ptr1.get(), 1, 1, ptr1.get());
                Assert::AreEqual(static_cast<uint64_t>(0x1231231231231231), ptr1[0]);

                ptr1 = allocate_uint(2, pool);
                ptr2 = allocate_uint(2, pool);
                ptr1[0] = 0x1234567887654321;
                ptr1[1] = 0x8765432112345678;
                set_uint_uint(nullptr, 0, 2, ptr1.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr1[1]);

                ptr1[0] = 0x1234567887654321;
                ptr1[1] = 0x8765432112345678;
                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                set_uint_uint(ptr1.get(), 1, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0x1234567887654321), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[1]);

                ptr2[0] = 0xFFFFFFFFFFFFFFFF;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                set_uint_uint(ptr1.get(), 2, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(0x1234567887654321), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x8765432112345678), ptr2[1]);

                ptr1[0] = 0x1231231231231321;
                ptr1[1] = 0x3213213213213211;
                set_uint_uint(ptr1.get(), 2, 2, ptr1.get());
                Assert::AreEqual(static_cast<uint64_t>(0x1231231231231321), ptr1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x3213213213213211), ptr1[1]);

                set_uint_uint(ptr1.get(), 1, 2, ptr1.get());
                Assert::AreEqual(static_cast<uint64_t>(0x1231231231231321), ptr1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr1[1]);
            }

            TEST_METHOD(IsZeroUInt)
            {
                Assert::IsTrue(is_zero_uint(nullptr, 0));

                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(1, pool));
                ptr[0] = 1;
                Assert::IsFalse(is_zero_uint(ptr.get(), 1));
                ptr[0] = 0;
                Assert::IsTrue(is_zero_uint(ptr.get(), 1));

                ptr = allocate_uint(2, pool);
                ptr[0] = 0x8000000000000000;
                ptr[1] = 0x8000000000000000;
                Assert::IsFalse(is_zero_uint(ptr.get(), 2));
                ptr[0] = 0;
                ptr[1] = 0x8000000000000000;
                Assert::IsFalse(is_zero_uint(ptr.get(), 2));
                ptr[0] = 0x8000000000000000;
                ptr[1] = 0;
                Assert::IsFalse(is_zero_uint(ptr.get(), 2));
                ptr[0] = 0;
                ptr[1] = 0;
                Assert::IsTrue(is_zero_uint(ptr.get(), 2));
            }

            TEST_METHOD(IsEqualUInt)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(1, pool));
                ptr[0] = 1;
                Assert::IsTrue(is_equal_uint(ptr.get(), 1, 1));
                Assert::IsFalse(is_equal_uint(ptr.get(), 1, 0));
                Assert::IsFalse(is_equal_uint(ptr.get(), 1, 2));

                ptr = allocate_uint(2, pool);
                ptr[0] = 1;
                ptr[1] = 1;
                Assert::IsFalse(is_equal_uint(ptr.get(), 2, 1));
                ptr[0] = 1;
                ptr[1] = 0;
                Assert::IsTrue(is_equal_uint(ptr.get(), 2, 1));
                ptr[0] = 0x1234567887654321;
                ptr[1] = 0;
                Assert::IsTrue(is_equal_uint(ptr.get(), 2, 0x1234567887654321));
                Assert::IsFalse(is_equal_uint(ptr.get(), 2, 0x2234567887654321));
            }

            TEST_METHOD(IsBitSetUInt)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(2, pool));
                ptr[0] = 0;
                ptr[1] = 0;
                for (int i = 0; i < 128; ++i)
                {
                    Assert::IsFalse(is_bit_set_uint(ptr.get(), 2, i));
                }
                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                for (int i = 0; i < 128; ++i)
                {
                    Assert::IsTrue(is_bit_set_uint(ptr.get(), 2, i));
                }

                ptr[0] = 0x0000000000000001;
                ptr[1] = 0x8000000000000000;
                for (int i = 0; i < 128; ++i)
                {
                    if (i == 0 || i == 127)
                    {
                        Assert::IsTrue(is_bit_set_uint(ptr.get(), 2, i));
                    }
                    else
                    {
                        Assert::IsFalse(is_bit_set_uint(ptr.get(), 2, i));
                    }
                }
            }

            TEST_METHOD(IsHighBitSetUInt)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(2, pool));
                ptr[0] = 0;
                ptr[1] = 0;
                Assert::IsFalse(is_high_bit_set_uint(ptr.get(), 2));

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                Assert::IsTrue(is_high_bit_set_uint(ptr.get(), 2));

                ptr[0] = 0;
                ptr[1] = 0x8000000000000000;
                Assert::IsTrue(is_high_bit_set_uint(ptr.get(), 2));

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0x7FFFFFFFFFFFFFFF;
                Assert::IsFalse(is_high_bit_set_uint(ptr.get(), 2));
            }

            TEST_METHOD(SetBitUInt)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(2, pool));
                ptr[0] = 0;
                ptr[1] = 0;
                set_bit_uint(ptr.get(), 2, 0);
                Assert::AreEqual(1ULL, ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);

                set_bit_uint(ptr.get(), 2, 127);
                Assert::AreEqual(1ULL, ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x8000000000000000), ptr[1]);

                set_bit_uint(ptr.get(), 2, 63);
                Assert::AreEqual(static_cast<uint64_t>(0x8000000000000001), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x8000000000000000), ptr[1]);

                set_bit_uint(ptr.get(), 2, 64);
                Assert::AreEqual(static_cast<uint64_t>(0x8000000000000001), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x8000000000000001), ptr[1]);

                set_bit_uint(ptr.get(), 2, 3);
                Assert::AreEqual(static_cast<uint64_t>(0x8000000000000009), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x8000000000000001), ptr[1]);
            }

            TEST_METHOD(GetSignificantBitCountUInt)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(2, pool));
                ptr[0] = 0;
                ptr[1] = 0;
                Assert::AreEqual(0, get_significant_bit_count_uint(ptr.get(), 2));

                ptr[0] = 1;
                ptr[1] = 0;
                Assert::AreEqual(1, get_significant_bit_count_uint(ptr.get(), 2));

                ptr[0] = 2;
                ptr[1] = 0;
                Assert::AreEqual(2, get_significant_bit_count_uint(ptr.get(), 2));

                ptr[0] = 3;
                ptr[1] = 0;
                Assert::AreEqual(2, get_significant_bit_count_uint(ptr.get(), 2));

                ptr[0] = 29;
                ptr[1] = 0;
                Assert::AreEqual(5, get_significant_bit_count_uint(ptr.get(), 2));

                ptr[0] = 4;
                ptr[1] = 0;
                Assert::AreEqual(3, get_significant_bit_count_uint(ptr.get(), 2));

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0;
                Assert::AreEqual(64, get_significant_bit_count_uint(ptr.get(), 2));

                ptr[0] = 0;
                ptr[1] = 1;
                Assert::AreEqual(65, get_significant_bit_count_uint(ptr.get(), 2));

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 1;
                Assert::AreEqual(65, get_significant_bit_count_uint(ptr.get(), 2));

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0x7000000000000000;
                Assert::AreEqual(127, get_significant_bit_count_uint(ptr.get(), 2));

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0x8000000000000000;
                Assert::AreEqual(128, get_significant_bit_count_uint(ptr.get(), 2));

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                Assert::AreEqual(128, get_significant_bit_count_uint(ptr.get(), 2));
            }

            TEST_METHOD(GetSignificantUInt64CountUInt)
            {   
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(2, pool));
                ptr[0] = 0;
                ptr[1] = 0;
                Assert::AreEqual(0, get_significant_uint64_count_uint(ptr.get(), 2));

                ptr[0] = 1;
                ptr[1] = 0;
                Assert::AreEqual(1, get_significant_uint64_count_uint(ptr.get(), 2));

                ptr[0] = 2;
                ptr[1] = 0;
                Assert::AreEqual(1, get_significant_uint64_count_uint(ptr.get(), 2));

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0;
                Assert::AreEqual(1, get_significant_uint64_count_uint(ptr.get(), 2));

                ptr[0] = 0;
                ptr[1] = 1;
                Assert::AreEqual(2, get_significant_uint64_count_uint(ptr.get(), 2));

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 1;
                Assert::AreEqual(2, get_significant_uint64_count_uint(ptr.get(), 2));

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0x8000000000000000;
                Assert::AreEqual(2, get_significant_uint64_count_uint(ptr.get(), 2));

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                Assert::AreEqual(2, get_significant_uint64_count_uint(ptr.get(), 2));
            }

            TEST_METHOD(GetPowerOfTwoUInt)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_zero_uint(2, pool));
                Assert::AreEqual(-1, get_power_of_two_uint(ptr.get(), 1));
                Assert::AreEqual(-1, get_power_of_two_uint(ptr.get(), 2));

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                Assert::AreEqual(-1, get_power_of_two_uint(ptr.get(), 1));
                Assert::AreEqual(-1, get_power_of_two_uint(ptr.get(), 2));

                ptr[0] = 0x0000000000000001;
                ptr[1] = 0x0000000000000000;
                Assert::AreEqual(0, get_power_of_two_uint(ptr.get(), 1));
                Assert::AreEqual(0, get_power_of_two_uint(ptr.get(), 2));

                ptr[0] = 0x0000000000000001;
                ptr[1] = 0x8000000000000000;
                Assert::AreEqual(-1, get_power_of_two_uint(ptr.get(), 2));

                ptr[0] = 0x0000000000000000;
                ptr[1] = 0x8000000000000000;
                Assert::AreEqual(127, get_power_of_two_uint(ptr.get(), 2));

                ptr[0] = 0x8000000000000000;
                ptr[1] = 0x0000000000000000;
                Assert::AreEqual(63, get_power_of_two_uint(ptr.get(), 2));

                ptr[0] = 0x9000000000000000;
                ptr[1] = 0x0000000000000000;
                Assert::AreEqual(-1, get_power_of_two_uint(ptr.get(), 2));

                ptr[0] = 0x8000000000000001;
                ptr[1] = 0x0000000000000000;
                Assert::AreEqual(-1, get_power_of_two_uint(ptr.get(), 2));

                ptr[0] = 0x0000000000000000;
                ptr[1] = 0x0000000000000001;
                Assert::AreEqual(64, get_power_of_two_uint(ptr.get(), 2));
            }

            TEST_METHOD(GetPowerOfTwoMinusOneUInt)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_zero_uint(2, pool));
                Assert::AreEqual(0, get_power_of_two_minus_one_uint(ptr.get(), 1));
                Assert::AreEqual(0, get_power_of_two_minus_one_uint(ptr.get(), 2));

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                Assert::AreEqual(64, get_power_of_two_minus_one_uint(ptr.get(), 1));
                Assert::AreEqual(128, get_power_of_two_minus_one_uint(ptr.get(), 2));

                ptr[0] = 0x0000000000000001;
                ptr[1] = 0x0000000000000000;
                Assert::AreEqual(1, get_power_of_two_minus_one_uint(ptr.get(), 1));
                Assert::AreEqual(1, get_power_of_two_minus_one_uint(ptr.get(), 2));

                ptr[0] = 0x0000000000000001;
                ptr[1] = 0x8000000000000000;
                Assert::AreEqual(-1, get_power_of_two_minus_one_uint(ptr.get(), 2));

                ptr[0] = 0x0000000000000000;
                ptr[1] = 0x8000000000000000;
                Assert::AreEqual(-1, get_power_of_two_minus_one_uint(ptr.get(), 2));

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0x7FFFFFFFFFFFFFFF;
                Assert::AreEqual(127, get_power_of_two_minus_one_uint(ptr.get(), 2));

                ptr[0] = 0xFFFFFFFFFFFFFFFE;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                Assert::AreEqual(-1, get_power_of_two_minus_one_uint(ptr.get(), 2));

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0x0000000000000000;
                Assert::AreEqual(64, get_power_of_two_minus_one_uint(ptr.get(), 2));

                ptr[0] = 0xFFFFFFFFFFFFFFFE;
                ptr[1] = 0x0000000000000000;
                Assert::AreEqual(-1, get_power_of_two_minus_one_uint(ptr.get(), 2));

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0x0000000000000001;
                Assert::AreEqual(65, get_power_of_two_minus_one_uint(ptr.get(), 2));

                ptr[0] = 0xFFFFFFFFFFFFFFFE;
                ptr[1] = 0x0000000000000001;
                Assert::AreEqual(-1, get_power_of_two_minus_one_uint(ptr.get(), 2));
            }

            TEST_METHOD(FilterHighBitsUInt)
            {
                filter_highbits_uint(nullptr, 0, 0);

                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(2, pool));
                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                filter_highbits_uint(ptr.get(), 2, 0);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                filter_highbits_uint(ptr.get(), 2, 128);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[1]);
                filter_highbits_uint(ptr.get(), 2, 127);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x7FFFFFFFFFFFFFFF), ptr[1]);
                filter_highbits_uint(ptr.get(), 2, 126);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0x3FFFFFFFFFFFFFFF), ptr[1]);
                filter_highbits_uint(ptr.get(), 2, 64);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);
                filter_highbits_uint(ptr.get(), 2, 63);
                Assert::AreEqual(static_cast<uint64_t>(0x7FFFFFFFFFFFFFFF), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);
                filter_highbits_uint(ptr.get(), 2, 2);
                Assert::AreEqual(static_cast<uint64_t>(0x3), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);
                filter_highbits_uint(ptr.get(), 2, 1);
                Assert::AreEqual(static_cast<uint64_t>(0x1), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);
                filter_highbits_uint(ptr.get(), 2, 0);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);

                filter_highbits_uint(ptr.get(), 2, 128);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[1]);
            }

            TEST_METHOD(CompareUIntUInt)
            {
                Assert::AreEqual(0, compare_uint_uint(nullptr, nullptr, 0));
                Assert::IsTrue(is_equal_uint_uint(nullptr, nullptr, 0));
                Assert::IsFalse(is_not_equal_uint_uint(nullptr, nullptr, 0));
                Assert::IsFalse(is_greater_than_uint_uint(nullptr, nullptr, 0));
                Assert::IsFalse(is_less_than_uint_uint(nullptr, nullptr, 0));
                Assert::IsTrue(is_greater_than_or_equal_uint_uint(nullptr, nullptr, 0));
                Assert::IsTrue(is_less_than_or_equal_uint_uint(nullptr, nullptr, 0));

                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr1(allocate_uint(2, pool));
                Pointer ptr2(allocate_uint(2, pool));
                ptr1[0] = 0;
                ptr1[1] = 0;
                ptr2[0] = 0;
                ptr2[1] = 0;
                Assert::AreEqual(0, compare_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_not_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_greater_than_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_less_than_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_greater_than_or_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_less_than_or_equal_uint_uint(ptr1.get(), ptr2.get(), 2));

                ptr1[0] = 0x1234567887654321;
                ptr1[1] = 0x8765432112345678;
                ptr2[0] = 0x1234567887654321;
                ptr2[1] = 0x8765432112345678;
                Assert::AreEqual(0, compare_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_not_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_greater_than_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_less_than_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_greater_than_or_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_less_than_or_equal_uint_uint(ptr1.get(), ptr2.get(), 2));

                ptr1[0] = 1;
                ptr1[1] = 0;
                ptr2[0] = 2;
                ptr2[1] = 0;
                Assert::AreEqual(-1, compare_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_not_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_greater_than_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_less_than_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_greater_than_or_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_less_than_or_equal_uint_uint(ptr1.get(), ptr2.get(), 2));

                ptr1[0] = 1;
                ptr1[1] = 0xFFFFFFFFFFFFFFFF;
                ptr2[0] = 2;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                Assert::AreEqual(-1, compare_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_not_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_greater_than_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_less_than_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_greater_than_or_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_less_than_or_equal_uint_uint(ptr1.get(), ptr2.get(), 2));

                ptr1[0] = 0xFFFFFFFFFFFFFFFF;
                ptr1[1] = 0x0000000000000001;
                ptr2[0] = 0x0000000000000000;
                ptr2[1] = 0x0000000000000002;
                Assert::AreEqual(-1, compare_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_not_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_greater_than_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_less_than_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_greater_than_or_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_less_than_or_equal_uint_uint(ptr1.get(), ptr2.get(), 2));

                ptr1[0] = 2;
                ptr1[1] = 0;
                ptr2[0] = 1;
                ptr2[1] = 0;
                Assert::AreEqual(1, compare_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_not_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_greater_than_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_less_than_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_greater_than_or_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_less_than_or_equal_uint_uint(ptr1.get(), ptr2.get(), 2));

                ptr1[0] = 2;
                ptr1[1] = 0xFFFFFFFFFFFFFFFF;
                ptr2[0] = 1;
                ptr2[1] = 0xFFFFFFFFFFFFFFFF;
                Assert::AreEqual(1, compare_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_not_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_greater_than_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_less_than_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_greater_than_or_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_less_than_or_equal_uint_uint(ptr1.get(), ptr2.get(), 2));

                ptr1[0] = 0xFFFFFFFFFFFFFFFF;
                ptr1[1] = 0x0000000000000003;
                ptr2[0] = 0x0000000000000000;
                ptr2[1] = 0x0000000000000002;
                Assert::AreEqual(1, compare_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_not_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_greater_than_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_less_than_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsTrue(is_greater_than_or_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
                Assert::IsFalse(is_less_than_or_equal_uint_uint(ptr1.get(), ptr2.get(), 2));
            }

            TEST_METHOD(GetPowerOfTwo)
            {
                Assert::AreEqual(-1, get_power_of_two(0));
                Assert::AreEqual(0, get_power_of_two(1));
                Assert::AreEqual(1, get_power_of_two(2));
                Assert::AreEqual(-1, get_power_of_two(3));
                Assert::AreEqual(2, get_power_of_two(4));
                Assert::AreEqual(-1, get_power_of_two(5));
                Assert::AreEqual(-1, get_power_of_two(6));
                Assert::AreEqual(-1, get_power_of_two(7));
                Assert::AreEqual(3, get_power_of_two(8));
                Assert::AreEqual(-1, get_power_of_two(15));
                Assert::AreEqual(4, get_power_of_two(16));
                Assert::AreEqual(-1, get_power_of_two(17));
                Assert::AreEqual(-1, get_power_of_two(255));
                Assert::AreEqual(8, get_power_of_two(256));
                Assert::AreEqual(-1, get_power_of_two(257));
                Assert::AreEqual(10, get_power_of_two(1 << 10));
                Assert::AreEqual(30, get_power_of_two(1 << 30));
                Assert::AreEqual(32, get_power_of_two(1ULL << 32));
                Assert::AreEqual(62, get_power_of_two(1ULL << 62));
                Assert::AreEqual(63, get_power_of_two(1ULL << 63));
            }

            TEST_METHOD(GetPowerOfTwoMinusOne)
            {
                Assert::AreEqual(0, get_power_of_two_minus_one(0));
                Assert::AreEqual(1, get_power_of_two_minus_one(1));
                Assert::AreEqual(-1, get_power_of_two_minus_one(2));
                Assert::AreEqual(2, get_power_of_two_minus_one(3));
                Assert::AreEqual(-1, get_power_of_two_minus_one(4));
                Assert::AreEqual(-1, get_power_of_two_minus_one(5));
                Assert::AreEqual(-1, get_power_of_two_minus_one(6));
                Assert::AreEqual(3, get_power_of_two_minus_one(7));
                Assert::AreEqual(-1, get_power_of_two_minus_one(8));
                Assert::AreEqual(-1, get_power_of_two_minus_one(14));
                Assert::AreEqual(4, get_power_of_two_minus_one(15));
                Assert::AreEqual(-1, get_power_of_two_minus_one(16));
                Assert::AreEqual(8, get_power_of_two_minus_one(255));
                Assert::AreEqual(10, get_power_of_two_minus_one((1 << 10) - 1));
                Assert::AreEqual(30, get_power_of_two_minus_one((1 << 30) - 1));
                Assert::AreEqual(32, get_power_of_two_minus_one((1ULL << 32) - 1));
                Assert::AreEqual(63, get_power_of_two_minus_one((1ULL << 63) - 1));
                Assert::AreEqual(64, get_power_of_two_minus_one(~static_cast<uint64_t>(0)));
            }

            TEST_METHOD(DuplicateUIntIfNeeded)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_uint(2, pool));
                ptr[0] = 0xF0F0F0F0F0;
                ptr[1] = 0xABABABABAB;
                ConstPointer ptr2 = duplicate_uint_if_needed(ptr.get(), 0, 0, false, pool);
                // No forcing and sizes are same (although zero) so just alias
                Assert::IsTrue(ptr2.get() == ptr.get());

                ptr2 = duplicate_uint_if_needed(ptr.get(), 0, 0, true, pool);
                // Forcing and size is zero so return nullptr
                Assert::IsTrue(ptr2.get() == nullptr);

                ptr2 = duplicate_uint_if_needed(ptr.get(), 1, 0, false, pool);
                Assert::IsTrue(ptr2.get() == ptr.get());

                ptr2 = duplicate_uint_if_needed(ptr.get(), 1, 0, true, pool);
                Assert::IsTrue(ptr2.get() == nullptr);

                ptr2 = duplicate_uint_if_needed(ptr.get(), 1, 1, false, pool);
                Assert::IsTrue(ptr2.get() == ptr.get());

                ptr2 = duplicate_uint_if_needed(ptr.get(), 1, 1, true, pool);
                Assert::IsTrue(ptr2.get() != ptr.get());
                Assert::AreEqual(ptr[0], ptr2[0]);

                ptr2 = duplicate_uint_if_needed(ptr.get(), 2, 2, true, pool);
                Assert::IsTrue(ptr2.get() != ptr.get());
                Assert::AreEqual(ptr[0], ptr2[0]);
                Assert::AreEqual(ptr[1], ptr2[1]);

                ptr2 = duplicate_uint_if_needed(ptr.get(), 2, 2, false, pool);
                Assert::IsTrue(ptr2.get() == ptr.get());

                ptr2 = duplicate_uint_if_needed(ptr.get(), 2, 1, false, pool);
                Assert::IsTrue(ptr2.get() == ptr.get());
                
                ptr2 = duplicate_uint_if_needed(ptr.get(), 1, 2, false, pool);
                Assert::IsTrue(ptr2.get() != ptr.get());
                Assert::AreEqual(ptr[0], ptr2[0]);
                Assert::AreEqual(0ULL, ptr2[1]);

                ptr2 = duplicate_uint_if_needed(ptr.get(), 1, 2, true, pool);
                Assert::IsTrue(ptr2.get() != ptr.get());
                Assert::AreEqual(ptr[0], ptr2[0]);
                Assert::AreEqual(0ULL, ptr2[1]);
            }

            TEST_METHOD(HammingWeight)
            {
                Assert::AreEqual(0ULL, hamming_weight(0ULL));
                Assert::AreEqual(1ULL, hamming_weight(1ULL));
                Assert::AreEqual(1ULL, hamming_weight(0x10000ULL));
                Assert::AreEqual(2ULL, hamming_weight(0x10001ULL));
                Assert::AreEqual(32ULL, hamming_weight(0xFFFFFFFFULL));
                Assert::AreEqual(64ULL, hamming_weight(0xFFFFFFFFFFFFFFFFULL));
                Assert::AreEqual(32ULL, hamming_weight(0xF0F0F0F0F0F0F0F0ULL));
                Assert::AreEqual(16ULL, hamming_weight(0xA0A0A0A0A0A0A0A0ULL)); 
            }

            TEST_METHOD(HammingWeightSplit)
            {
                Assert::AreEqual(0ULL, hamming_weight_split(0ULL));
                Assert::AreEqual(1ULL, hamming_weight_split(1ULL));
                Assert::AreEqual(0x10000ULL, hamming_weight_split(0x10000ULL));
                Assert::AreEqual(1ULL, hamming_weight_split(0x10001ULL));
                Assert::AreEqual(0xFFFFULL, hamming_weight_split(0xFFFFFFFFULL));
                Assert::AreEqual(0xFFFFFFFFULL, hamming_weight_split(0xFFFFFFFFFFFFFFFFULL));
                Assert::AreEqual(0xF0F0F00ULL, hamming_weight_split(0xF0F0F0000F0F0F00ULL));
                Assert::AreEqual(0xA0A0A0A0ULL, hamming_weight_split(0xA0A0A0A0A0A0A0A0ULL));
            }
        };
    }
}