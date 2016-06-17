#include "CppUnitTest.h"
#include "util/polycore.h"
#include "util/uintarith.h"
#include <cstdint>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal::util;
using namespace std;

namespace SEALTest
{
    namespace util
    {
        TEST_CLASS(PolyCore)
        {
        public:
            TEST_METHOD(AllocatePoly)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer ptr(allocate_poly(0, 0, pool));
                Assert::IsTrue(nullptr == ptr.get());

                ptr = allocate_poly(1, 0, pool);
                Assert::IsTrue(nullptr == ptr.get());

                ptr = allocate_poly(0, 1, pool);
                Assert::IsTrue(nullptr == ptr.get());

                ptr = allocate_poly(1, 1, pool);
                Assert::IsTrue(nullptr != ptr.get());

                ptr = allocate_poly(2, 1, pool);
                Assert::IsTrue(nullptr != ptr.get());
            }

            TEST_METHOD(SetZeroPoly)
            {
                set_zero_poly(0, 0, nullptr);

                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer ptr(allocate_poly(1, 1, pool));
                ptr[0] = 0x1234567812345678;
                set_zero_poly(1, 1, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);

                ptr = allocate_poly(2, 3, pool);
                for (int i = 0; i < 6; ++i)
                {
                    ptr[i] = 0x1234567812345678;
                }
                set_zero_poly(2, 3, ptr.get());
                for (int i = 0; i < 6; ++i)
                {
                    Assert::AreEqual(static_cast<uint64_t>(0), ptr[i]);
                }
            }

            TEST_METHOD(AllocateZeroPoly)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer ptr(allocate_zero_poly(0, 0, pool));
                Assert::IsTrue(nullptr == ptr.get());

                ptr = allocate_zero_poly(1, 1, pool);
                Assert::IsTrue(nullptr != ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[0]);

                ptr = allocate_zero_poly(2, 3, pool);
                Assert::IsTrue(nullptr != ptr.get());
                for (int i = 0; i < 6; ++i)
                {
                    Assert::AreEqual(static_cast<uint64_t>(0), ptr[i]);
                }
            }

            TEST_METHOD(GetPolyCoeff)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer ptr(allocate_zero_poly(2, 3, pool));
                *get_poly_coeff(ptr.get(), 0, 3) = 1;
                *get_poly_coeff(ptr.get(), 1, 3) = 2;
                Assert::AreEqual(static_cast<uint64_t>(1), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(2), ptr[3]);
                Assert::AreEqual(static_cast<uint64_t>(1), *get_poly_coeff(ptr.get(), 0, 3));
                Assert::AreEqual(static_cast<uint64_t>(2), *get_poly_coeff(ptr.get(), 1, 3));
            }

            TEST_METHOD(SetPolyPoly)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer ptr1(allocate_poly(2, 3, pool));
                Pointer ptr2(allocate_zero_poly(2, 3, pool));
                for (int i = 0; i < 6; ++i)
                {
                    ptr1[i] = static_cast<uint64_t>(i + 1);
                }
                set_poly_poly(ptr1.get(), 2, 3, ptr2.get());
                for (int i = 0; i < 6; ++i)
                {
                    Assert::AreEqual(static_cast<uint64_t>(i + 1), ptr2[i]);
                }

                set_poly_poly(ptr1.get(), 2, 3, ptr1.get());
                for (int i = 0; i < 6; ++i)
                {
                    Assert::AreEqual(static_cast<uint64_t>(i + 1), ptr2[i]);
                }

                ptr2 = allocate_poly(3, 4, pool);
                for (int i = 0; i < 12; ++i)
                {
                    ptr2[i] = static_cast<uint64_t>(1);
                }
                set_poly_poly(ptr1.get(), 2, 3, 3, 4, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(1), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(2), ptr2[1]);
                Assert::AreEqual(static_cast<uint64_t>(3), ptr2[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[3]);
                Assert::AreEqual(static_cast<uint64_t>(4), ptr2[4]);
                Assert::AreEqual(static_cast<uint64_t>(5), ptr2[5]);
                Assert::AreEqual(static_cast<uint64_t>(6), ptr2[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[7]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[8]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[9]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[10]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr2[11]);

                ptr2 = allocate_poly(1, 2, pool);
                ptr2[0] = 1;
                ptr2[1] = 1;
                set_poly_poly(ptr1.get(), 2, 3, 1, 2, ptr2.get());
                Assert::AreEqual(static_cast<uint64_t>(1), ptr2[0]);
                Assert::AreEqual(static_cast<uint64_t>(2), ptr2[1]);
            }

            TEST_METHOD(IsZeroPoly)
            {
                Assert::IsTrue(is_zero_poly(nullptr, 0, 0));

                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer ptr(allocate_poly(2, 3, pool));
                for (int i = 0; i < 6; ++i)
                {
                    ptr[i] = 0;
                }
                Assert::IsTrue(is_zero_poly(ptr.get(), 2, 3));
                for (int i = 0; i < 6; ++i)
                {
                    ptr[i] = 1;
                    Assert::IsFalse(is_zero_poly(ptr.get(), 2, 3));
                    ptr[i] = 0;
                }
            }

            TEST_METHOD(IsEqualPolyPoly)
            {
                Assert::IsTrue(is_equal_poly_poly(nullptr, nullptr, 0, 0));

                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer ptr1(allocate_poly(2, 3, pool));
                Pointer ptr2(allocate_poly(2, 3, pool));
                for (int i = 0; i < 6; ++i)
                {
                    ptr2[i] = ptr1[i] = static_cast<uint64_t>(i + 1);
                }
                Assert::IsTrue(is_equal_poly_poly(ptr1.get(), ptr2.get(), 2, 3));
                for (int i = 0; i < 6; ++i)
                {
                    ptr2[i]--;
                    Assert::IsFalse(is_equal_poly_poly(ptr1.get(), ptr2.get(), 2, 3));
                    ptr2[i]++;
                }
            }

            TEST_METHOD(IsOneZeroOnePoly)
            {
                Assert::IsFalse(is_one_zero_one_poly(nullptr, 0, 0));

                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer poly(allocate_zero_poly(4, 2, pool));
                Assert::IsFalse(is_one_zero_one_poly(poly.get(), 0, 2));
                Assert::IsFalse(is_one_zero_one_poly(poly.get(), 1, 2));
                Assert::IsFalse(is_one_zero_one_poly(poly.get(), 2, 2));
                Assert::IsFalse(is_one_zero_one_poly(poly.get(), 3, 2));

                poly[0] = 2;
                Assert::IsFalse(is_one_zero_one_poly(poly.get(), 1, 2));
                Assert::IsFalse(is_one_zero_one_poly(poly.get(), 2, 2));

                poly[0] = 1;
                Assert::IsTrue(is_one_zero_one_poly(poly.get(), 1, 2));
                Assert::IsFalse(is_one_zero_one_poly(poly.get(), 2, 2));

                poly[2] = 2;
                Assert::IsFalse(is_one_zero_one_poly(poly.get(), 2, 2));
                Assert::IsFalse(is_one_zero_one_poly(poly.get(), 3, 2));

                poly[2] = 1;
                Assert::IsTrue(is_one_zero_one_poly(poly.get(), 2, 2));
                Assert::IsFalse(is_one_zero_one_poly(poly.get(), 3, 2));

                poly[4] = 1;
                Assert::IsFalse(is_one_zero_one_poly(poly.get(), 3, 2));
                Assert::IsFalse(is_one_zero_one_poly(poly.get(), 4, 2));

                poly[2] = 0;
                Assert::IsTrue(is_one_zero_one_poly(poly.get(), 3, 2));
                Assert::IsFalse(is_one_zero_one_poly(poly.get(), 4, 2));

                poly[6] = 2;
                Assert::IsFalse(is_one_zero_one_poly(poly.get(), 4, 2));

                poly[6] = 1;
                Assert::IsFalse(is_one_zero_one_poly(poly.get(), 4, 2));

                poly[4] = 0;
                Assert::IsTrue(is_one_zero_one_poly(poly.get(), 4, 2));
            }

            TEST_METHOD(GetSignificantCoeffCountPoly)
            {
                Assert::AreEqual(0, get_significant_coeff_count_poly(nullptr, 0, 0));

                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer ptr(allocate_zero_poly(3, 2, pool));
                Assert::AreEqual(0, get_significant_coeff_count_poly(ptr.get(), 3, 2));
                ptr[0] = 1;
                Assert::AreEqual(1, get_significant_coeff_count_poly(ptr.get(), 3, 2));
                ptr[1] = 1;
                Assert::AreEqual(1, get_significant_coeff_count_poly(ptr.get(), 3, 2));
                ptr[4] = 1;
                Assert::AreEqual(3, get_significant_coeff_count_poly(ptr.get(), 3, 2));
                ptr[4] = 0;
                ptr[5] = 1;
                Assert::AreEqual(3, get_significant_coeff_count_poly(ptr.get(), 3, 2));
            }

            TEST_METHOD(DuplicatePolyIfNeeded)
            {
                Assert::AreEqual(0, get_significant_coeff_count_poly(nullptr, 0, 0));

                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer poly(allocate_poly(3, 2, pool));
                for (int i = 0; i < 6; i++)
                {
                    poly[i] = i + 1;
                }

                ConstPointer ptr = duplicate_poly_if_needed(poly.get(), 3, 2, 3, 2, false, pool);
                Assert::IsTrue(ptr.get() == poly.get());
                ptr = duplicate_poly_if_needed(poly.get(), 3, 2, 2, 2, false, pool);
                Assert::IsTrue(ptr.get() == poly.get());
                ptr = duplicate_poly_if_needed(poly.get(), 3, 2, 2, 3, false, pool);
                Assert::IsTrue(ptr.get() != poly.get());
                Assert::AreEqual(static_cast<uint64_t>(1), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(2), ptr[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[2]);
                Assert::AreEqual(static_cast<uint64_t>(3), ptr[3]);
                Assert::AreEqual(static_cast<uint64_t>(4), ptr[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), ptr[5]);

                ptr = duplicate_poly_if_needed(poly.get(), 3, 2, 3, 2, true, pool);
                Assert::IsTrue(ptr.get() != poly.get());
                for (int i = 0; i < 6; i++)
                {
                    Assert::AreEqual(static_cast<uint64_t>(i + 1), ptr[i]);
                }
            }

            TEST_METHOD(ArePolyCoeffsLessThan)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer poly(allocate_zero_poly(3, 2, pool));
                poly[0] = 3;
                poly[2] = 5;
                poly[4] = 4;

                Pointer max(allocate_uint(1, pool));
                max[0] = 1;
                Assert::IsFalse(are_poly_coefficients_less_than(poly.get(), 3, 2, max.get(), 1));
                max[0] = 5;
                Assert::IsFalse(are_poly_coefficients_less_than(poly.get(), 3, 2, max.get(), 1));
                max[0] = 6;
                Assert::IsTrue(are_poly_coefficients_less_than(poly.get(), 3, 2, max.get(), 1));
                max[0] = 10;
                Assert::IsTrue(are_poly_coefficients_less_than(poly.get(), 3, 2, max.get(), 1));
            }
        };
    }
}