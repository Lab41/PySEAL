#include "CppUnitTest.h"
#include "util/uintcore.h"
#include "util/polycore.h"
#include "util/polyarith.h"
#include <cstdint>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal::util;
using namespace std;

namespace SEALTest
{
    namespace util
    {
        TEST_CLASS(PolyArith)
        {
        public:
            TEST_METHOD(ModuloPolyCoeffs)
            {
                MemoryPool pool;
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

                MemoryPool pool;
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
                MemoryPool pool;
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

                MemoryPool pool;
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

                MemoryPool pool;
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
                MemoryPool pool;
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
                MemoryPool pool;
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
                MemoryPool pool;
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
                MemoryPool pool;
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
                MemoryPool pool;
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
                MemoryPool pool;
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
        };
    }
}