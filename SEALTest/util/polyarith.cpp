#include "CppUnitTest.h"
#include "seal/util/uintcore.h"
#include "seal/util/polyarith.h"
#include "seal/bigpolyarray.h"
#include <cstdint>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal::util;
using namespace std;
using namespace seal;

namespace SEALTest
{
    namespace util
    {
        TEST_CLASS(PolyArith)
        {
        public:
            TEST_METHOD(RightShiftPolyCoeffs)
            {
                right_shift_poly_coeffs(nullptr, 0, 0, 0, nullptr);
                right_shift_poly_coeffs(nullptr, 0, 0, 1, nullptr);

                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_zero_poly(3, 2, pool));
                ptr[0] = 2;
                ptr[1] = 4;
                ptr[2] = 8;
                right_shift_poly_coeffs(ptr.get(), 3, 1, 0, ptr.get());
                Assert::AreEqual(2ULL, ptr[0]);
                Assert::AreEqual(4ULL, ptr[1]);
                Assert::AreEqual(8ULL, ptr[2]);

                right_shift_poly_coeffs(ptr.get(), 3, 1, 1, ptr.get());
                Assert::AreEqual(1ULL, ptr[0]);
                Assert::AreEqual(2ULL, ptr[1]);
                Assert::AreEqual(4ULL, ptr[2]);

                right_shift_poly_coeffs(ptr.get(), 3, 1, 1, ptr.get());
                Assert::AreEqual(0ULL, ptr[0]);
                Assert::AreEqual(1ULL, ptr[1]);
                Assert::AreEqual(2ULL, ptr[2]);

                ptr[0] = 3;
                ptr[1] = 5;
                ptr[2] = 9;
                right_shift_poly_coeffs(ptr.get(), 3, 1, 2, ptr.get());
                Assert::AreEqual(0ULL, ptr[0]);
                Assert::AreEqual(1ULL, ptr[1]);
                Assert::AreEqual(2ULL, ptr[2]);

                ptr[0] = 3;
                ptr[1] = 5;
                ptr[2] = 9;
                right_shift_poly_coeffs(ptr.get(), 3, 1, 4, ptr.get());
                Assert::AreEqual(0ULL, ptr[0]);
                Assert::AreEqual(0ULL, ptr[1]);
                Assert::AreEqual(0ULL, ptr[2]);

                ptr[0] = 1;
                ptr[1] = 1;
                ptr[2] = 1;
                right_shift_poly_coeffs(ptr.get(), 1, 2, 64, ptr.get());
                Assert::AreEqual(1ULL, ptr[0]);
                Assert::AreEqual(0ULL, ptr[1]);
                Assert::AreEqual(1ULL, ptr[2]);

                ptr[0] = 3;
                ptr[1] = 5;
                ptr[2] = 9;
                right_shift_poly_coeffs(ptr.get(), 1, 3, 128, ptr.get());
                Assert::AreEqual(9ULL, ptr[0]);
                Assert::AreEqual(0ULL, ptr[1]);
                Assert::AreEqual(0ULL, ptr[2]);

                ptr[0] = 0xFFFFFFFFFFFFFFFF;
                ptr[1] = 0xFFFFFFFFFFFFFFFF;
                ptr[2] = 0xFFFFFFFFFFFFFFFF;
                right_shift_poly_coeffs(ptr.get(), 1, 3, 191, ptr.get());
                Assert::AreEqual(1ULL, ptr[0]);
                Assert::AreEqual(0ULL, ptr[1]);
                Assert::AreEqual(0ULL, ptr[2]);
            }

            TEST_METHOD(NegatePoly)
            {
                negate_poly(nullptr, 0, 0, nullptr);

                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer ptr(allocate_zero_poly(3, 2, pool));
                ptr[0] = 2;
                ptr[2] = 3;
                ptr[4] = 4;
                negate_poly(ptr.get(), 3, 2, ptr.get());
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), ptr[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[1]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFD), ptr[2]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[3]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFC), ptr[4]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), ptr[5]);
            }

            TEST_METHOD(AddPolyPoly)
            {
                add_poly_poly(nullptr, nullptr, 0, 0, nullptr);

                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly1(allocate_zero_poly(3, 2, pool));
                Pointer poly2(allocate_zero_poly(3, 2, pool));

                poly1[0] = 0;
                poly1[1] = 0xFFFFFFFFFFFFFFFF;
                poly1[2] = 1;
                poly1[3] = 0;
                poly1[4] = 0xFFFFFFFFFFFFFFFF;
                poly1[5] = 1;
                poly2[0] = 1;
                poly2[1] = 1;
                poly2[2] = 1;
                poly2[3] = 1;
                poly2[4] = 0xFFFFFFFFFFFFFFFF;
                poly2[5] = 1;
                add_poly_poly(poly1.get(), poly2.get(), 3, 2, poly1.get());
                Assert::AreEqual(static_cast<uint64_t>(1), poly1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[1]);
                Assert::AreEqual(static_cast<uint64_t>(2), poly1[2]);
                Assert::AreEqual(static_cast<uint64_t>(1), poly1[3]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), poly1[4]);
                Assert::AreEqual(static_cast<uint64_t>(3), poly1[5]);

                poly1[0] = 2;
                poly1[1] = 0;
                poly1[2] = 3;
                poly1[3] = 0;
                poly1[4] = 0xFFFFFFFFFFFFFFFF;
                poly1[5] = 0xFFFFFFFFFFFFFFFF;
                poly2[0] = 5;
                poly2[1] = 0;
                poly2[2] = 6;
                poly2[3] = 0;
                poly2[4] = 0xFFFFFFFFFFFFFFFF;
                poly2[5] = 0xFFFFFFFFFFFFFFFF;
                add_poly_poly(poly1.get(), poly2.get(), 3, 2, poly1.get());
                Assert::AreEqual(static_cast<uint64_t>(7), poly1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[1]);
                Assert::AreEqual(static_cast<uint64_t>(9), poly1[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[3]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), poly1[4]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), poly1[5]);
            }

            TEST_METHOD(SubPolyPoly)
            {
                sub_poly_poly(nullptr, nullptr, 0, 0, nullptr);

                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly1(allocate_zero_poly(3, 2, pool));
                Pointer poly2(allocate_zero_poly(3, 2, pool));

                poly1[0] = 0;
                poly1[1] = 0xFFFFFFFFFFFFFFFF;
                poly1[2] = 1;
                poly1[3] = 0;
                poly1[4] = 0xFFFFFFFFFFFFFFFF;
                poly1[5] = 1;
                poly2[0] = 1;
                poly2[1] = 1;
                poly2[2] = 1;
                poly2[3] = 1;
                poly2[4] = 0xFFFFFFFFFFFFFFFF;
                poly2[5] = 1;
                sub_poly_poly(poly1.get(), poly2.get(), 6, 1, poly1.get());
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), poly1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), poly1[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[2]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), poly1[3]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[5]);

                poly1[0] = 5;
                poly1[1] = 0;
                poly1[2] = 6;
                poly1[3] = 0;
                poly1[4] = 0xFFFFFFFFFFFFFFFF;
                poly1[5] = 0xFFFFFFFFFFFFFFFF;
                poly2[0] = 2;
                poly2[1] = 0;
                poly2[2] = 8;
                poly2[3] = 0;
                poly2[4] = 0xFFFFFFFFFFFFFFFE;
                poly2[5] = 0xFFFFFFFFFFFFFFFF;
                sub_poly_poly(poly1.get(), poly2.get(), 3, 2, poly1.get());
                Assert::AreEqual(static_cast<uint64_t>(3), poly1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[1]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), poly1[2]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), poly1[3]);
                Assert::AreEqual(1ULL, poly1[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[5]);
            }

            TEST_METHOD(MultiplyPolyPoly)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly1(allocate_zero_poly(3, 2, pool));
                Pointer poly2(allocate_zero_poly(3, 2, pool));
                Pointer result(allocate_zero_poly(5, 2, pool));
                poly1[0] = 1;
                poly1[2] = 2;
                poly1[4] = 3;
                poly2[0] = 2;
                poly2[2] = 3;
                poly2[4] = 4;
                multiply_poly_poly(poly1.get(), 3, 2, poly2.get(), 3, 2, 5, 2, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(2), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                Assert::AreEqual(static_cast<uint64_t>(7), result[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[3]);
                Assert::AreEqual(static_cast<uint64_t>(16), result[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                Assert::AreEqual(static_cast<uint64_t>(17), result[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[7]);
                Assert::AreEqual(static_cast<uint64_t>(12), result[8]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[9]);

                poly2[0] = 2;
                poly2[1] = 3;
                multiply_poly_poly(poly1.get(), 3, 2, poly2.get(), 2, 1, 5, 2, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(2), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                Assert::AreEqual(static_cast<uint64_t>(7), result[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[3]);
                Assert::AreEqual(static_cast<uint64_t>(12), result[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                Assert::AreEqual(static_cast<uint64_t>(9), result[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[7]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[8]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[9]);

                multiply_poly_poly(poly1.get(), 3, 2, poly2.get(), 2, 1, 5, 1, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(2), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(7), result[1]);
                Assert::AreEqual(static_cast<uint64_t>(12), result[2]);
                Assert::AreEqual(static_cast<uint64_t>(9), result[3]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[4]);
            }

            TEST_METHOD(PolyInftyNorm)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly(allocate_zero_poly(10, 1, pool));
                uint64_t result[2];

                poly[0] = 1, poly[1] = 0, poly[2] = 1, poly[3] = 0, poly[4] = 0;
                poly[5] = 4, poly[6] = 0xB, poly[7] = 0xA, poly[8] = 5, poly[9] = 2;
                poly_infty_norm(poly.get(), 10, 1, result);
                Assert::AreEqual(result[0], 0xBULL);

                poly[0] = 2, poly[1] = 0, poly[2] = 1, poly[3] = 0, poly[4] = 0;
                poly[5] = 0xF7, poly[6] = 0xFE, poly[7] = 0xCF, poly[8] = 0xCA, poly[9] = 0xAB;
                poly_infty_norm(poly.get(), 10, 1, result);
                Assert::AreEqual(result[0], 0xFEULL);

                poly[0] = 2, poly[1] = 0, poly[2] = 1, poly[3] = 0, poly[4] = 0;
                poly[5] = 0xABCDEF, poly[6] = 0xABCDE, poly[7] = 0xABCD, poly[8] = 0xABC, poly[9] = 0xAB;
                poly_infty_norm(poly.get(), 10, 1, result);
                Assert::AreEqual(result[0], 0xABCDEFULL);

                poly[0] = 6, poly[1] = 5, poly[2] = 4, poly[3] = 3, poly[4] = 2;
                poly[5] = 1, poly[6] = 0;
                poly_infty_norm(poly.get(), 6, 1, result);
                Assert::AreEqual(result[0], 6ULL);

                poly[0] = 1, poly[1] = 0, poly[2] = 1, poly[3] = 0, poly[4] = 0;
                poly[5] = 4, poly[6] = 0xB, poly[7] = 0xA, poly[8] = 5, poly[9] = 2;
                poly_infty_norm(poly.get(), 5, 2, result);
                Assert::AreEqual(result[0], 0xBULL);
                Assert::AreEqual(result[1], 0xAULL);

                poly[0] = 2, poly[1] = 0, poly[2] = 1, poly[3] = 0, poly[4] = 0;
                poly[5] = 0xF7, poly[6] = 0xFE, poly[7] = 0xCF, poly[8] = 0xCA, poly[9] = 0xAB;
                poly_infty_norm(poly.get(), 5, 2, result);
                Assert::AreEqual(result[0], 0x0ULL);
                Assert::AreEqual(result[1], 0xF7ULL);

                poly[0] = 2, poly[1] = 0, poly[2] = 1, poly[3] = 0, poly[4] = 0;
                poly[5] = 0xABCDEF, poly[6] = 0xABCDE, poly[7] = 0xABCD, poly[8] = 0xABC, poly[9] = 0xAB;
                poly_infty_norm(poly.get(), 5, 2, result);
                Assert::AreEqual(result[0], 0ULL);
                Assert::AreEqual(result[1], 0xABCDEFULL);

                poly[0] = 6, poly[1] = 5, poly[2] = 4, poly[3] = 3, poly[4] = 2;
                poly[5] = 1, poly[6] = 0;
                poly_infty_norm(poly.get(), 3, 2, result);
                Assert::AreEqual(result[0], 6ULL);
                Assert::AreEqual(result[1], 5ULL);
            }

            TEST_METHOD(PolyEvalPoly)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly1(allocate_zero_poly(4, 1, pool));
                Pointer poly2(allocate_zero_poly(4, 1, pool));
                Pointer poly3(allocate_zero_poly(8, 1, pool));

                poly_eval_poly(poly1.get(), 4, 1, poly2.get(), 4, 1, 8, 1, poly3.get(), pool);
                Assert::AreEqual(poly3[0], 0ULL);
                Assert::AreEqual(poly3[1], 0ULL);
                Assert::AreEqual(poly3[2], 0ULL);
                Assert::AreEqual(poly3[3], 0ULL);
                Assert::AreEqual(poly3[4], 0ULL);
                Assert::AreEqual(poly3[5], 0ULL);
                Assert::AreEqual(poly3[6], 0ULL);
                Assert::AreEqual(poly3[7], 0ULL);

                poly1[0] = 1;
                poly_eval_poly(poly1.get(), 4, 1, poly2.get(), 4, 1, 8, 1, poly3.get(), pool);
                Assert::AreEqual(poly3[0], 1ULL);
                Assert::AreEqual(poly3[1], 0ULL);
                Assert::AreEqual(poly3[2], 0ULL);
                Assert::AreEqual(poly3[3], 0ULL);
                Assert::AreEqual(poly3[4], 0ULL);
                Assert::AreEqual(poly3[5], 0ULL);
                Assert::AreEqual(poly3[6], 0ULL);
                Assert::AreEqual(poly3[7], 0ULL);

                poly1[0] = 2;
                poly2[0] = 1;
                poly_eval_poly(poly1.get(), 4, 1, poly2.get(), 4, 1, 8, 1, poly3.get(), pool);
                Assert::AreEqual(poly3[0], 2ULL);
                Assert::AreEqual(poly3[1], 0ULL);
                Assert::AreEqual(poly3[2], 0ULL);
                Assert::AreEqual(poly3[3], 0ULL);
                Assert::AreEqual(poly3[4], 0ULL);
                Assert::AreEqual(poly3[5], 0ULL);
                Assert::AreEqual(poly3[6], 0ULL);
                Assert::AreEqual(poly3[7], 0ULL);

                poly1[0] = 1;
                poly1[1] = 1;
                poly2[0] = 1;
                poly_eval_poly(poly1.get(), 4, 1, poly2.get(), 4, 1, 8, 1, poly3.get(), pool);
                Assert::AreEqual(poly3[0], 2ULL);
                Assert::AreEqual(poly3[1], 0ULL);
                Assert::AreEqual(poly3[2], 0ULL);
                Assert::AreEqual(poly3[3], 0ULL);
                Assert::AreEqual(poly3[4], 0ULL);
                Assert::AreEqual(poly3[5], 0ULL);
                Assert::AreEqual(poly3[6], 0ULL);
                Assert::AreEqual(poly3[7], 0ULL);

                poly1[0] = 1;
                poly1[1] = 1;
                poly2[0] = 2;
                poly2[1] = 0;
                poly2[2] = 1;
                poly_eval_poly(poly1.get(), 4, 1, poly2.get(), 4, 1, 8, 1, poly3.get(), pool);
                Assert::AreEqual(poly3[0], 3ULL);
                Assert::AreEqual(poly3[1], 0ULL);
                Assert::AreEqual(poly3[2], 1ULL);
                Assert::AreEqual(poly3[3], 0ULL);
                Assert::AreEqual(poly3[4], 0ULL);
                Assert::AreEqual(poly3[5], 0ULL);
                Assert::AreEqual(poly3[6], 0ULL);
                Assert::AreEqual(poly3[7], 0ULL);                
                
                poly1[0] = 2;
                poly1[1] = 0;
                poly1[2] = 1;
                poly2[0] = 1;
                poly2[1] = 1;
                poly2[2] = 0;
                poly_eval_poly(poly1.get(), 4, 1, poly2.get(), 4, 1, 8, 1, poly3.get(), pool);
                Assert::AreEqual(poly3[0], 3ULL);
                Assert::AreEqual(poly3[1], 2ULL);
                Assert::AreEqual(poly3[2], 1ULL);
                Assert::AreEqual(poly3[3], 0ULL);
                Assert::AreEqual(poly3[4], 0ULL);
                Assert::AreEqual(poly3[5], 0ULL);
                Assert::AreEqual(poly3[6], 0ULL);
                Assert::AreEqual(poly3[7], 0ULL);

                poly1[0] = 0;
                poly1[1] = 0;
                poly1[2] = 0;
                poly1[3] = 1;
                poly2[0] = 2;
                poly2[1] = 0;
                poly2[2] = 0;
                poly2[3] = 0;
                poly_eval_poly(poly1.get(), 4, 1, poly2.get(), 4, 1, 8, 1, poly3.get(), pool);
                Assert::AreEqual(poly3[0], 8ULL);
                Assert::AreEqual(poly3[1], 0ULL);
                Assert::AreEqual(poly3[2], 0ULL);
                Assert::AreEqual(poly3[3], 0ULL);
                Assert::AreEqual(poly3[4], 0ULL);
                Assert::AreEqual(poly3[5], 0ULL);
                Assert::AreEqual(poly3[6], 0ULL);
                Assert::AreEqual(poly3[7], 0ULL);

                poly1[0] = 0;
                poly1[1] = 0;
                poly1[2] = 0;
                poly1[3] = 1;
                poly2[0] = 0;
                poly2[1] = 0;
                poly2[2] = 2;
                poly2[3] = 0;
                poly_eval_poly(poly1.get(), 4, 1, poly2.get(), 4, 1, 8, 1, poly3.get(), pool);
                Assert::AreEqual(poly3[0], 0ULL);
                Assert::AreEqual(poly3[1], 0ULL);
                Assert::AreEqual(poly3[2], 0ULL);
                Assert::AreEqual(poly3[3], 0ULL);
                Assert::AreEqual(poly3[4], 0ULL);
                Assert::AreEqual(poly3[5], 0ULL);
                Assert::AreEqual(poly3[6], 8ULL);
                Assert::AreEqual(poly3[7], 0ULL);
            }

            TEST_METHOD(ExponentiatePoly)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly1(allocate_zero_poly(4, 1, pool));
                Pointer poly2(allocate_zero_poly(12, 1, pool));

                uint64_t exponent = 1;
                exponentiate_poly(poly1.get(), 4, 1, &exponent, 1, 12, 1, poly2.get(), pool);
                Assert::AreEqual(poly2[0], 0ULL);
                Assert::AreEqual(poly2[1], 0ULL);
                Assert::AreEqual(poly2[2], 0ULL);
                Assert::AreEqual(poly2[3], 0ULL);
                Assert::AreEqual(poly2[4], 0ULL);
                Assert::AreEqual(poly2[5], 0ULL);
                Assert::AreEqual(poly2[6], 0ULL);
                Assert::AreEqual(poly2[7], 0ULL);
                Assert::AreEqual(poly2[8], 0ULL);
                Assert::AreEqual(poly2[9], 0ULL);
                Assert::AreEqual(poly2[10], 0ULL);
                Assert::AreEqual(poly2[11], 0ULL);

                exponent = 0;
                exponentiate_poly(poly1.get(), 4, 1, &exponent, 1, 12, 1, poly2.get(), pool);
                Assert::AreEqual(poly2[0], 1ULL);
                Assert::AreEqual(poly2[1], 0ULL);
                Assert::AreEqual(poly2[2], 0ULL);
                Assert::AreEqual(poly2[3], 0ULL);
                Assert::AreEqual(poly2[4], 0ULL);
                Assert::AreEqual(poly2[5], 0ULL);
                Assert::AreEqual(poly2[6], 0ULL);
                Assert::AreEqual(poly2[7], 0ULL);
                Assert::AreEqual(poly2[8], 0ULL);
                Assert::AreEqual(poly2[9], 0ULL);
                Assert::AreEqual(poly2[10], 0ULL);
                Assert::AreEqual(poly2[11], 0ULL);

                exponent = 3;
                poly1[1] = 2;
                exponentiate_poly(poly1.get(), 4, 1, &exponent, 1, 12, 1, poly2.get(), pool);
                Assert::AreEqual(poly2[0], 0ULL);
                Assert::AreEqual(poly2[1], 0ULL);
                Assert::AreEqual(poly2[2], 0ULL);
                Assert::AreEqual(poly2[3], 8ULL);
                Assert::AreEqual(poly2[4], 0ULL);
                Assert::AreEqual(poly2[5], 0ULL);
                Assert::AreEqual(poly2[6], 0ULL);
                Assert::AreEqual(poly2[7], 0ULL);
                Assert::AreEqual(poly2[8], 0ULL);
                Assert::AreEqual(poly2[9], 0ULL);
                Assert::AreEqual(poly2[10], 0ULL);
                Assert::AreEqual(poly2[11], 0ULL);

                exponent = 3;
                poly1[0] = 1;
                poly1[1] = 1;
                exponentiate_poly(poly1.get(), 4, 1, &exponent, 1, 12, 1, poly2.get(), pool);
                Assert::AreEqual(poly2[0], 1ULL);
                Assert::AreEqual(poly2[1], 3ULL);
                Assert::AreEqual(poly2[2], 3ULL);
                Assert::AreEqual(poly2[3], 1ULL);
                Assert::AreEqual(poly2[4], 0ULL);
                Assert::AreEqual(poly2[5], 0ULL);
                Assert::AreEqual(poly2[6], 0ULL);
                Assert::AreEqual(poly2[7], 0ULL);
                Assert::AreEqual(poly2[8], 0ULL);
                Assert::AreEqual(poly2[9], 0ULL);
                Assert::AreEqual(poly2[10], 0ULL);
                Assert::AreEqual(poly2[11], 0ULL);

                exponent = 5;
                poly1[0] = 0;
                poly1[1] = 0;
                poly1[2] = 2;
                exponentiate_poly(poly1.get(), 4, 1, &exponent, 1, 12, 1, poly2.get(), pool);
                Assert::AreEqual(poly2[0], 0ULL);
                Assert::AreEqual(poly2[1], 0ULL);
                Assert::AreEqual(poly2[2], 0ULL);
                Assert::AreEqual(poly2[3], 0ULL);
                Assert::AreEqual(poly2[4], 0ULL);
                Assert::AreEqual(poly2[5], 0ULL);
                Assert::AreEqual(poly2[6], 0ULL);
                Assert::AreEqual(poly2[7], 0ULL);
                Assert::AreEqual(poly2[8], 0ULL);
                Assert::AreEqual(poly2[9], 0ULL);
                Assert::AreEqual(poly2[10], 32ULL);
                Assert::AreEqual(poly2[11], 0ULL);
            }
        };
    }
}