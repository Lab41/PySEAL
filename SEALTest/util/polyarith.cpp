#include "CppUnitTest.h"
#include "util/uintcore.h"
#include "util/polycore.h"
#include "util/polyarith.h"
#include "bigpolyarray.h"
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
            TEST_METHOD(ModuloPolyCoeffs)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer poly(allocate_zero_poly(3, 2, pool));
                Pointer modulus(allocate_uint(2, pool));
                poly[0] = 2;
                poly[2] = 15;
                poly[4] = 77;
                modulus[0] = 15;
                modulus[1] = 0;
                Modulus mod(modulus.get(), 2);
                modulo_poly_coeffs(poly.get(), 3, mod, pool);
                Assert::AreEqual(static_cast<uint64_t>(2), poly[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly[3]);
                Assert::AreEqual(static_cast<uint64_t>(2), poly[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly[5]);
            }

            TEST_METHOD(NegatePoly)
            {
                negate_poly(nullptr, 0, 0, nullptr);

                MemoryPool &pool = *MemoryPool::default_pool();
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

            TEST_METHOD(NegatePolyCoeffMod)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer poly(allocate_zero_poly(3, 2, pool));
                Pointer modulus(allocate_uint(2, pool));
                poly[0] = 2;
                poly[2] = 3;
                poly[4] = 4;
                modulus[0] = 15;
                modulus[1] = 0;
                negate_poly_coeffmod(poly.get(), 3, modulus.get(), 2, poly.get());
                Assert::AreEqual(static_cast<uint64_t>(13), poly[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly[1]);
                Assert::AreEqual(static_cast<uint64_t>(12), poly[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly[3]);
                Assert::AreEqual(static_cast<uint64_t>(11), poly[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly[5]);

                poly[0] = 2;
                poly[2] = 3;
                poly[4] = 4;
                modulus[0] = 0xFFFFFFFFFFFFFFFF;
                modulus[1] = 0xFFFFFFFFFFFFFFFF;
                negate_poly_coeffmod(poly.get(), 3, modulus.get(), 2, poly.get());
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFD), poly[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), poly[1]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFC), poly[2]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), poly[3]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFB), poly[4]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), poly[5]);
            }

            TEST_METHOD(AddPolyPoly)
            {
                add_poly_poly(nullptr, nullptr, 0, 0, nullptr);

                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer poly1(allocate_zero_poly(3, 2, pool));
                Pointer poly2(allocate_zero_poly(3, 2, pool));
                poly1[0] = 2;
                poly1[2] = 3;
                poly1[4] = 0xFFFFFFFFFFFFFFFF;
                poly1[5] = 0xFFFFFFFFFFFFFFFF;
                poly2[0] = 5;
                poly2[2] = 6;
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

                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer poly1(allocate_zero_poly(3, 2, pool));
                Pointer poly2(allocate_zero_poly(3, 2, pool));
                poly1[0] = 5;
                poly1[2] = 6;
                poly1[4] = 0xFFFFFFFFFFFFFFFF;
                poly1[5] = 0xFFFFFFFFFFFFFFFF;
                poly2[0] = 2;
                poly2[2] = 8;
                poly2[4] = 0xFFFFFFFFFFFFFFFE;
                poly2[5] = 0xFFFFFFFFFFFFFFFF;
                sub_poly_poly(poly1.get(), poly2.get(), 3, 2, poly1.get());
                Assert::AreEqual(static_cast<uint64_t>(3), poly1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[1]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), poly1[2]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), poly1[3]);
                Assert::AreEqual(static_cast<uint64_t>(1), poly1[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[5]);
            }

            TEST_METHOD(AddPolyPolyCoeffMod)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer poly1(allocate_zero_poly(3, 2, pool));
                Pointer poly2(allocate_zero_poly(3, 2, pool));
                Pointer modulus(allocate_uint(2, pool));
                poly1[0] = 1;
                poly1[2] = 3;
                poly1[4] = 4;
                poly2[0] = 1;
                poly2[2] = 2;
                poly2[4] = 4;
                modulus[0] = 5;
                modulus[1] = 0;
                add_poly_poly_coeffmod(poly1.get(), poly2.get(), 3, modulus.get(), 2, poly1.get());
                Assert::AreEqual(static_cast<uint64_t>(2), poly1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[3]);
                Assert::AreEqual(static_cast<uint64_t>(3), poly1[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[5]);
            }

            TEST_METHOD(SubPolyPolyCoeffMod)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer poly1(allocate_zero_poly(3, 2, pool));
                Pointer poly2(allocate_zero_poly(3, 2, pool));
                Pointer modulus(allocate_uint(2, pool));
                poly1[0] = 4;
                poly1[2] = 3;
                poly1[4] = 2;
                poly2[0] = 2;
                poly2[2] = 3;
                poly2[4] = 4;
                modulus[0] = 5;
                modulus[1] = 0;
                sub_poly_poly_coeffmod(poly1.get(), poly2.get(), 3, modulus.get(), 2, poly1.get());
                Assert::AreEqual(static_cast<uint64_t>(2), poly1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[3]);
                Assert::AreEqual(static_cast<uint64_t>(3), poly1[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[5]);
            }

            TEST_METHOD(MultiplyPolyScalarCoeffMod)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer poly(allocate_zero_poly(3, 2, pool));
                Pointer scalar(allocate_uint(2, pool));
                Pointer modulus(allocate_uint(2, pool));
                poly[0] = 1;
                poly[2] = 3;
                poly[4] = 4;
                scalar[0] = 3;
                scalar[1] = 0;
                modulus[0] = 5;
                modulus[1] = 0;
                Modulus mod(modulus.get(), 2);
                multiply_poly_scalar_coeffmod(poly.get(), 3, scalar.get(), mod, poly.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(3), poly[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly[1]);
                Assert::AreEqual(static_cast<uint64_t>(4), poly[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly[3]);
                Assert::AreEqual(static_cast<uint64_t>(2), poly[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly[5]);
            }

            TEST_METHOD(MultiplyPolyPoly)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
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

            TEST_METHOD(MultiplyPolyPolyCoeffMod)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer poly1(allocate_zero_poly(3, 2, pool));
                Pointer poly2(allocate_zero_poly(3, 2, pool));
                Pointer result(allocate_zero_poly(5, 2, pool));
                Pointer modulus(allocate_uint(2, pool));
                poly1[0] = 1;
                poly1[2] = 2;
                poly1[4] = 3;
                poly2[0] = 2;
                poly2[2] = 3;
                poly2[4] = 4;
                modulus[0] = 5;
                modulus[1] = 0;
                Modulus mod(modulus.get(), 2);
                multiply_poly_poly_coeffmod(poly1.get(), 3, 2, poly2.get(), 3, 2, mod, 5, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(2), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                Assert::AreEqual(static_cast<uint64_t>(2), result[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[3]);
                Assert::AreEqual(static_cast<uint64_t>(1), result[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                Assert::AreEqual(static_cast<uint64_t>(2), result[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[7]);
                Assert::AreEqual(static_cast<uint64_t>(2), result[8]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[9]);
                set_zero_poly(5, 2, result.get());
                multiply_poly_poly_coeffmod(poly1.get(), poly2.get(), 3, mod, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(2), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                Assert::AreEqual(static_cast<uint64_t>(2), result[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[3]);
                Assert::AreEqual(static_cast<uint64_t>(1), result[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                Assert::AreEqual(static_cast<uint64_t>(2), result[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[7]);
                Assert::AreEqual(static_cast<uint64_t>(2), result[8]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[9]);
                set_zero_poly(5, 2, result.get());
                multiply_truncate_poly_poly_coeffmod(poly1.get(), poly2.get(), 3, mod, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(2), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                Assert::AreEqual(static_cast<uint64_t>(2), result[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[3]);
                Assert::AreEqual(static_cast<uint64_t>(1), result[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[7]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[8]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[9]);

                poly2[0] = 2;
                poly2[1] = 3;
                multiply_poly_poly_coeffmod(poly1.get(), 3, 2, poly2.get(), 2, 1, mod, 5, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(2), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                Assert::AreEqual(static_cast<uint64_t>(2), result[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[3]);
                Assert::AreEqual(static_cast<uint64_t>(2), result[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                Assert::AreEqual(static_cast<uint64_t>(4), result[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[7]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[8]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[9]);

                Modulus mod2(modulus.get(), 1);
                multiply_poly_poly_coeffmod(poly1.get(), 3, 2, poly2.get(), 2, 1, mod2, 5, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(2), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(2), result[1]);
                Assert::AreEqual(static_cast<uint64_t>(2), result[2]);
                Assert::AreEqual(static_cast<uint64_t>(4), result[3]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[4]);
            }

            TEST_METHOD(DividePolyPolyCoeffMod)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer poly1(allocate_zero_poly(5, 2, pool));
                Pointer poly2(allocate_zero_poly(5, 2, pool));
                Pointer result(allocate_zero_poly(5, 2, pool));
                Pointer quotient(allocate_zero_poly(5, 2, pool));
                Pointer modulus(allocate_uint(2, pool));

                poly1[0] = 2;
                poly1[2] = 2;
                poly2[0] = 2;
                poly2[2] = 3;
                poly2[4] = 4;
                modulus[0] = 5;
                modulus[1] = 0;
                Modulus mod(modulus.get(), 2);
                divide_poly_poly_coeffmod_inplace(poly1.get(), poly2.get(), 5, mod, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(2), poly1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[1]);
                Assert::AreEqual(static_cast<uint64_t>(2), poly1[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[3]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[5]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[7]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[8]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[9]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[3]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[7]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[8]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[9]);

                poly1[0] = 2;
                poly1[2] = 2;
                poly1[4] = 1;
                poly1[6] = 2;
                poly1[8] = 2;
                poly2[0] = 4;
                poly2[2] = 3;
                poly2[4] = 2;
                modulus[0] = 5;
                modulus[1] = 0;
                Modulus mod2(modulus.get(), 2);
                divide_poly_poly_coeffmod(poly1.get(), poly2.get(), 5, mod2, quotient.get(), result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(0), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[3]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[7]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[8]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[9]);
                Assert::AreEqual(static_cast<uint64_t>(3), quotient[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), quotient[1]);
                Assert::AreEqual(static_cast<uint64_t>(2), quotient[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), quotient[3]);
                Assert::AreEqual(static_cast<uint64_t>(1), quotient[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), quotient[5]);
                Assert::AreEqual(static_cast<uint64_t>(0), quotient[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), quotient[7]);
                Assert::AreEqual(static_cast<uint64_t>(0), quotient[8]);
                Assert::AreEqual(static_cast<uint64_t>(0), quotient[9]);
                divide_poly_poly_coeffmod_inplace(poly1.get(), poly2.get(), 5, mod2, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[3]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[5]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[7]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[8]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[9]);
                Assert::AreEqual(static_cast<uint64_t>(3), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                Assert::AreEqual(static_cast<uint64_t>(2), result[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[3]);
                Assert::AreEqual(static_cast<uint64_t>(1), result[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[7]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[8]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[9]);
            }


            TEST_METHOD(AddBigPolyArrayCoeffmod)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
                int coeff_uint64_count = divide_round_up(7, bits_per_uint64);
                BigUInt coeff_modulus("10");
                Modulus mod(coeff_modulus.pointer(), coeff_uint64_count, pool);
                
                // testing just addition, no mod reduction
                BigPolyArray result1(2, 5, 7);
                BigPolyArray arr1(2, 5, 7);
                BigPolyArray arr2(2, 5, 7);
                arr1[0] = "1x^1";
                arr1[1] = "1x^3";
                arr2[0] = "1";
                arr2[1] = "2x^1";
                add_bigpolyarray_coeffmod(arr1.pointer(0), arr2.pointer(0), 2, 5, mod, result1.pointer(0));
                Assert::IsTrue(result1[0].to_string() == "1x^1 + 1");
                Assert::IsTrue(result1[1].to_string() == "1x^3 + 2x^1");

                // expecting mod reduction
                BigPolyArray result2(3, 5, 7);
                BigPolyArray arr3(3, 5, 7);
                BigPolyArray arr4(3, 5, 7);
                arr3[0] = "1x^4 + A";
                arr3[1] = "3x^2 + 2x^1";
                arr3[2] = "Cx^1 + D";
                arr4[0] = "2x^4 + B";
                arr4[1] = "Fx^2 + 5x^1 + E";
                arr4[2] = "7x^1 + 8";
                add_bigpolyarray_coeffmod(arr3.pointer(0), arr4.pointer(0), 3, 5, mod, result2.pointer(0));
                Assert::IsTrue(result2[0].to_string() == "3x^4 + 5");
                Assert::IsTrue(result2[1].to_string() == "2x^2 + 7x^1 + E");
                Assert::IsTrue(result2[2].to_string() == "3x^1 + 5");

                // testing BPA's with only one entry, expecting mod reduction
                BigPolyArray result3(1, 5, 7);
                BigPolyArray singlepoly1(1, 5, 7);
                BigPolyArray singlepoly2(1, 5, 7);
                singlepoly1[0] = "Ax^3 + Bx^2 + Cx^1 + D";
                singlepoly2[0] = "1x^3 + 2x^1 + 3";
                add_bigpolyarray_coeffmod(singlepoly1.pointer(0), singlepoly2.pointer(0), 1, 5, mod, result3.pointer(0));
                Assert::IsTrue(result3[0].to_string() == "Bx^3 + Bx^2 + Ex^1");

                // testing addition of zero BPA
                BigPolyArray result4(2, 5, 7);
                BigPolyArray testzero1(2, 5, 7);
                BigPolyArray testzero2(2, 5, 7);
                testzero1[0] = "1x^2 + 2x^1 + 3";
                testzero1[1] = "8x^3 + 9x^2 + Ax^1 + B";
                testzero2[0].set_zero();
                testzero2[1].set_zero();
                add_bigpolyarray_coeffmod(testzero1.pointer(0), testzero2.pointer(0), 2, 5, mod, result4.pointer(0));
                Assert::IsTrue(result4[0].to_string() == "1x^2 + 2x^1 + 3");
                Assert::IsTrue(result4[1].to_string() == "8x^3 + 9x^2 + Ax^1 + B");

                // addition of both BPAs zero
                BigPolyArray result5(3, 5, 7);
                BigPolyArray testbothzero1(3, 5, 7);
                BigPolyArray testbothzero2(3, 5, 7);
                testbothzero1[0].set_zero();
                testbothzero1[1].set_zero();
                testbothzero1[2].set_zero();
                testbothzero2[0].set_zero();
                testbothzero2[1].set_zero();
                testbothzero2[2].set_zero();
                add_bigpolyarray_coeffmod(testbothzero1.pointer(0), testbothzero2.pointer(0), 3, 5, mod, result5.pointer(0));
                Assert::IsTrue(result5[0].to_string() == "0");
                Assert::IsTrue(result5[1].to_string() == "0");

                //  testing BPA's with only one entry, no mod reduction
                BigPolyArray result6(1, 5, 7);
                BigPolyArray singlepoly3(1, 5, 7);
                BigPolyArray singlepoly4(1, 5, 7);
                singlepoly3[0] = "1x^3 + 2x^2 + 3x^1 + 4";
                singlepoly4[0] = "5x^3 + 6x^1 + 7";
                add_bigpolyarray_coeffmod(singlepoly3.pointer(0), singlepoly4.pointer(0), 1, 5, mod, result6.pointer(0));
                Assert::IsTrue(result6[0].to_string() == "6x^3 + 2x^2 + 9x^1 + B");
            }
        };
    }
}