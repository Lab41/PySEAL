#include "CppUnitTest.h"
#include "seal/util/uintcore.h"
#include "seal/util/polycore.h"
#include "seal/util/polyarithmod.h"
#include "seal/bigpolyarray.h"
#include <cstdint>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace seal::util;
using namespace std;

namespace SEALTest
{
    namespace util
    {
        TEST_CLASS(PolyArithMod)
        {
        public:
            TEST_METHOD(ModuloPolyCoeffs)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
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

            TEST_METHOD(NegatePolyCoeffMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
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

            TEST_METHOD(AddPolyPolyCoeffMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
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
                MemoryPool &pool = *global_variables::global_memory_pool;
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
                MemoryPool &pool = *global_variables::global_memory_pool;
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

            TEST_METHOD(MultiplyPolyPolyCoeffMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
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
                Assert::AreEqual(1ULL, result[4]);
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
                Assert::AreEqual(1ULL, result[4]);
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
                Assert::AreEqual(1ULL, result[4]);
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
                MemoryPool &pool = *global_variables::global_memory_pool;
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
                Assert::AreEqual(1ULL, quotient[4]);
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
                Assert::AreEqual(1ULL, result[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[7]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[8]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[9]);
            }

            TEST_METHOD(AddBigPolyArrayCoeffMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                int coeff_uint64_count = divide_round_up(7, bits_per_uint64);
                BigUInt coeff_modulus("10");
                Modulus mod(coeff_modulus.pointer(), coeff_uint64_count, pool);

                // testing just addition, no mod reduction
                BigPolyArray result1(2, 5, 7);
                BigPolyArray arr1(2, 5, 7);
                BigPolyArray arr2(2, 5, 7);
                BigPoly(arr1.coeff_count(), arr1.coeff_bit_count(), arr1.pointer(0)) = "1x^1";
                BigPoly(arr1.coeff_count(), arr1.coeff_bit_count(), arr1.pointer(1)) = "1x^3";
                BigPoly(arr2.coeff_count(), arr2.coeff_bit_count(), arr2.pointer(0)) = "1";
                BigPoly(arr2.coeff_count(), arr2.coeff_bit_count(), arr2.pointer(1)) = "2x^1";
                add_bigpolyarray_coeffmod(arr1.pointer(0), arr2.pointer(0), 2, 5, mod, result1.pointer(0));
                Assert::IsTrue(BigPoly(result1.coeff_count(), result1.coeff_bit_count(), result1.pointer(0)).to_string() == "1x^1 + 1");
                Assert::IsTrue(BigPoly(result1.coeff_count(), result1.coeff_bit_count(), result1.pointer(1)).to_string() == "1x^3 + 2x^1");

                // expecting mod reduction
                BigPolyArray result2(3, 5, 7);
                BigPolyArray arr3(3, 5, 7);
                BigPolyArray arr4(3, 5, 7);
                BigPoly(arr3.coeff_count(), arr3.coeff_bit_count(), arr3.pointer(0)) = "1x^4 + A";
                BigPoly(arr3.coeff_count(), arr3.coeff_bit_count(), arr3.pointer(1)) = "3x^2 + 2x^1";
                BigPoly(arr3.coeff_count(), arr3.coeff_bit_count(), arr3.pointer(2)) = "Cx^1 + D";
                BigPoly(arr4.coeff_count(), arr4.coeff_bit_count(), arr4.pointer(0)) = "2x^4 + B";
                BigPoly(arr4.coeff_count(), arr4.coeff_bit_count(), arr4.pointer(1)) = "Fx^2 + 5x^1 + E";
                BigPoly(arr4.coeff_count(), arr4.coeff_bit_count(), arr4.pointer(2)) = "7x^1 + 8";
                add_bigpolyarray_coeffmod(arr3.pointer(0), arr4.pointer(0), 3, 5, mod, result2.pointer(0));
                Assert::IsTrue(BigPoly(result2.coeff_count(), result2.coeff_bit_count(), result2.pointer(0)).to_string() == "3x^4 + 5");
                Assert::IsTrue(BigPoly(result2.coeff_count(), result2.coeff_bit_count(), result2.pointer(1)).to_string() == "2x^2 + 7x^1 + E");
                Assert::IsTrue(BigPoly(result2.coeff_count(), result2.coeff_bit_count(), result2.pointer(2)).to_string() == "3x^1 + 5");

                // testing BPA's with only one entry, expecting mod reduction
                BigPolyArray result3(1, 5, 7);
                BigPolyArray singlepoly1(1, 5, 7);
                BigPolyArray singlepoly2(1, 5, 7);
                BigPoly(singlepoly1.coeff_count(), singlepoly1.coeff_bit_count(), singlepoly1.pointer(0)) = "Ax^3 + Bx^2 + Cx^1 + D";
                BigPoly(singlepoly2.coeff_count(), singlepoly2.coeff_bit_count(), singlepoly2.pointer(0)) = "1x^3 + 2x^1 + 3";
                add_bigpolyarray_coeffmod(singlepoly1.pointer(0), singlepoly2.pointer(0), 1, 5, mod, result3.pointer(0));
                Assert::IsTrue(BigPoly(result3.coeff_count(), result3.coeff_bit_count(), result3.pointer(0)).to_string() == "Bx^3 + Bx^2 + Ex^1");

                // testing addition of zero BPA
                BigPolyArray result4(2, 5, 7);
                BigPolyArray testzero1(2, 5, 7);
                BigPolyArray testzero2(2, 5, 7);
                BigPoly(testzero1.coeff_count(), testzero1.coeff_bit_count(), testzero1.pointer(0)) = "1x^2 + 2x^1 + 3";
                BigPoly(testzero1.coeff_count(), testzero1.coeff_bit_count(), testzero1.pointer(1)) = "8x^3 + 9x^2 + Ax^1 + B";
                BigPoly(testzero2.coeff_count(), testzero2.coeff_bit_count(), testzero2.pointer(0)).set_zero();
                BigPoly(testzero2.coeff_count(), testzero2.coeff_bit_count(), testzero2.pointer(1)).set_zero();
                add_bigpolyarray_coeffmod(testzero1.pointer(0), testzero2.pointer(0), 2, 5, mod, result4.pointer(0));
                Assert::IsTrue(BigPoly(result4.coeff_count(), result4.coeff_bit_count(), result4.pointer(0)).to_string() == "1x^2 + 2x^1 + 3");
                Assert::IsTrue(BigPoly(result4.coeff_count(), result4.coeff_bit_count(), result4.pointer(1)).to_string() == "8x^3 + 9x^2 + Ax^1 + B");

                // addition of both BPAs zero
                BigPolyArray result5(3, 5, 7);
                BigPolyArray testbothzero1(3, 5, 7);
                BigPolyArray testbothzero2(3, 5, 7);
                testbothzero1.set_zero();
                testbothzero2.set_zero();
                add_bigpolyarray_coeffmod(testbothzero1.pointer(0), testbothzero2.pointer(0), 3, 5, mod, result5.pointer(0));
                Assert::IsTrue(BigPoly(result5.coeff_count(), result5.coeff_bit_count(), result5.pointer(0)).to_string() == "0");
                Assert::IsTrue(BigPoly(result5.coeff_count(), result5.coeff_bit_count(), result5.pointer(1)).to_string() == "0");

                //  testing BPA's with only one entry, no mod reduction
                BigPolyArray result6(1, 5, 7);
                BigPolyArray singlepoly3(1, 5, 7);
                BigPolyArray singlepoly4(1, 5, 7);
                BigPoly(singlepoly3.coeff_count(), singlepoly3.coeff_bit_count(), singlepoly3.pointer(0)) = "1x^3 + 2x^2 + 3x^1 + 4";
                BigPoly(singlepoly4.coeff_count(), singlepoly4.coeff_bit_count(), singlepoly4.pointer(0)) = "5x^3 + 6x^1 + 7";
                add_bigpolyarray_coeffmod(singlepoly3.pointer(0), singlepoly4.pointer(0), 1, 5, mod, result6.pointer(0));
                Assert::IsTrue(BigPoly(result6.coeff_count(), result6.coeff_bit_count(), result6.pointer(0)).to_string() == "6x^3 + 2x^2 + 9x^1 + B");
            }

            TEST_METHOD(DyadicProductCoeffMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly1(allocate_zero_poly(3, 2, pool));
                Pointer poly2(allocate_zero_poly(3, 2, pool));
                Pointer result(allocate_zero_poly(3, 2, pool));
                Pointer modulus(allocate_uint(2, pool));

                poly1[0] = 1;
                poly1[2] = 1;
                poly1[4] = 1;
                poly2[0] = 2;
                poly2[2] = 3;
                poly2[4] = 4;
                modulus[0] = 13;
                modulus[1] = 0;
                {
                    Modulus mod(modulus.get(), 2);
                    dyadic_product_coeffmod(poly1.get(), poly2.get(), 3, mod, result.get(), pool);
                    Assert::AreEqual(static_cast<uint64_t>(2), result[0]);
                    Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                    Assert::AreEqual(static_cast<uint64_t>(3), result[2]);
                    Assert::AreEqual(static_cast<uint64_t>(0), result[3]);
                    Assert::AreEqual(static_cast<uint64_t>(4), result[4]);
                    Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                }

                poly1[0] = 0;
                poly1[2] = 0;
                poly1[4] = 0;
                poly2[0] = 2;
                poly2[2] = 3;
                poly2[4] = 4;
                modulus[0] = 13;
                modulus[1] = 0;
                {
                    Modulus mod(modulus.get(), 2);
                    dyadic_product_coeffmod(poly1.get(), poly2.get(), 3, mod, result.get(), pool);
                    Assert::AreEqual(static_cast<uint64_t>(0), result[0]);
                    Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                    Assert::AreEqual(static_cast<uint64_t>(0), result[2]);
                    Assert::AreEqual(static_cast<uint64_t>(0), result[3]);
                    Assert::AreEqual(static_cast<uint64_t>(0), result[4]);
                    Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                }

                poly1[0] = 3;
                poly1[2] = 5;
                poly1[4] = 8;
                poly2[0] = 2;
                poly2[2] = 3;
                poly2[4] = 4;
                modulus[0] = 13;
                modulus[1] = 0;
                {
                    Modulus mod(modulus.get(), 2);
                    dyadic_product_coeffmod(poly1.get(), poly2.get(), 3, mod, result.get(), pool);
                    Assert::AreEqual(static_cast<uint64_t>(6), result[0]);
                    Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                    Assert::AreEqual(static_cast<uint64_t>(2), result[2]);
                    Assert::AreEqual(static_cast<uint64_t>(0), result[3]);
                    Assert::AreEqual(static_cast<uint64_t>(6), result[4]);
                    Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                }

                poly1.acquire(allocate_zero_poly(4, 2, pool));
                poly2.acquire(allocate_zero_poly(4, 2, pool));
                result.acquire(allocate_zero_poly(4, 2, pool));

                poly1[0] = 1;
                poly1[1] = 0;
                poly1[2] = 0;
                poly1[3] = 1;
                poly1[4] = 0xFFFFFFFFFFFFFFFF;
                poly1[5] = 0;
                poly1[6] = 0xABCDEF;
                poly1[7] = 0xFEDCBA;
                poly2[0] = 0x1111;
                poly2[1] = 0;
                poly2[2] = 0x2222;
                poly2[3] = 1;
                poly2[4] = 0xFFFF;
                poly2[5] = 0;
                poly2[6] = 0xFEDCBA;
                poly2[7] = 0xABCDEF;
                modulus[0] = 0x1111111111111111;
                modulus[1] = 0xAAAAAAAAAAAAAAAA;
                {
                    cout << modulus[1] << " " << modulus[0] << endl;
                    Modulus mod(modulus.get(), 2);
                    dyadic_product_coeffmod(poly1.get(), poly2.get(), 4, mod, result.get(), pool);
                    Assert::AreEqual(static_cast<uint64_t>(4369), result[0]);
                    Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                    Assert::AreEqual(static_cast<uint64_t>(17216961135462248175), result[2]);
                    Assert::AreEqual(static_cast<uint64_t>(6148914691236525943), result[3]);
                    Assert::AreEqual(static_cast<uint64_t>(18446744073709486081), result[4]);
                    Assert::AreEqual(static_cast<uint64_t>(65534), result[5]);
                    Assert::AreEqual(static_cast<uint64_t>(206867539828125), result[6]);
                    Assert::AreEqual(static_cast<uint64_t>(575007302272500), result[7]);
                }
            }

            TEST_METHOD(ModuloPoly)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly1(allocate_zero_poly(5, 2, pool));
                Pointer poly2(allocate_zero_poly(3, 2, pool));
                Pointer result(allocate_zero_poly(3, 2, pool));
                Pointer modulus(allocate_uint(2, pool));

                poly1[0] = 2;
                poly1[2] = 2;
                poly2[0] = 2;
                poly2[2] = 3;
                poly2[4] = 4;
                modulus[0] = 5;
                modulus[1] = 0;
                PolyModulus polymod(poly2.get(), 3, 2);
                Modulus mod(modulus.get(), 2);
                modulo_poly(poly1.get(), 5, polymod, mod, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(2), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                Assert::AreEqual(static_cast<uint64_t>(2), result[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[3]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                modulo_poly_inplace(poly1.get(), 5, polymod, mod, pool);
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

                poly1[0] = 3;
                poly1[2] = 3;
                poly1[4] = 1;
                poly1[6] = 2;
                poly1[8] = 2;
                poly2[0] = 4;
                poly2[2] = 3;
                poly2[4] = 2;
                modulus[0] = 5;
                modulus[1] = 0;
                PolyModulus polymod2(poly2.get(), 3, 2);
                Modulus mod2(modulus.get(), 2);
                modulo_poly(poly1.get(), 5, polymod2, mod2, result.get(), pool);
                Assert::AreEqual(1ULL, result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                Assert::AreEqual(1ULL, result[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[3]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                modulo_poly_inplace(poly1.get(), 5, polymod2, mod2, pool);
                Assert::AreEqual(1ULL, poly1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[1]);
                Assert::AreEqual(1ULL, poly1[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[3]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[5]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[7]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[8]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[9]);
            }

            TEST_METHOD(NonFFTMultiplyPolyPolyPolyModCoeffMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly1(allocate_zero_poly(4, 2, pool));
                Pointer poly2(allocate_zero_poly(4, 2, pool));
                Pointer polymod(allocate_zero_poly(4, 2, pool));
                Pointer result(allocate_zero_poly(4, 2, pool));
                Pointer modulus(allocate_uint(2, pool));
                poly1[0] = 1;
                poly1[2] = 2;
                poly1[4] = 3;
                poly2[0] = 2;
                poly2[2] = 3;
                poly2[4] = 4;
                polymod[0] = 4;
                polymod[2] = 3;
                polymod[4] = 0;
                polymod[6] = 2;
                modulus[0] = 5;
                modulus[1] = 0;
                PolyModulus polym(polymod.get(), 4, 2);
                Modulus mod(modulus.get(), 2);
                nonfft_multiply_poly_poly_polymod_coeffmod(poly1.get(), poly2.get(), polym, mod, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(3), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[3]);
                Assert::AreEqual(static_cast<uint64_t>(3), result[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[7]);
                nonfft_multiply_poly_poly_polymod_coeffmod(poly1.get(), poly2.get(), polym, mod, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(3), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[3]);
                Assert::AreEqual(static_cast<uint64_t>(3), result[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[7]);
            }

            TEST_METHOD(TryInvertPolyCoeffMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly(allocate_zero_poly(4, 2, pool));
                Pointer polymod(allocate_zero_poly(4, 2, pool));
                Pointer modulus(allocate_uint(2, pool));
                Pointer result(allocate_zero_poly(4, 2, pool));

                polymod[0] = 4;
                polymod[2] = 3;
                polymod[4] = 0;
                polymod[6] = 2;
                modulus[0] = 5;
                modulus[1] = 0;
                Modulus mod(modulus.get(), 2);
                Assert::IsFalse(try_invert_poly_coeffmod(poly.get(), polymod.get(), 4, mod, result.get(), pool));

                poly[0] = 1;
                Assert::IsTrue(try_invert_poly_coeffmod(poly.get(), polymod.get(), 4, mod, result.get(), pool));
                Assert::AreEqual(1ULL, result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[3]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[7]);

                poly[0] = 1;
                poly[2] = 2;
                poly[4] = 3;
                Assert::IsTrue(try_invert_poly_coeffmod(poly.get(), polymod.get(), 4, mod, result.get(), pool));
                Assert::AreEqual(static_cast<uint64_t>(4), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[3]);
                Assert::AreEqual(static_cast<uint64_t>(2), result[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[7]);
            }

            TEST_METHOD(PolyInftyNormCoeffMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly(allocate_zero_poly(4, 1, pool));
                Pointer modulus(allocate_uint(2, pool));
                modulus[0] = 10;
                modulus[1] = 0;
                Modulus mod(modulus.get(), 1);

                uint64_t result[2];

                poly[0] = 0, poly[1] = 1, poly[2] = 2, poly[3] = 3;
                poly_infty_norm_coeffmod(poly.get(), 4, 1, mod, result, pool);
                Assert::AreEqual(result[0], 0x3ULL);

                poly[0] = 0, poly[1] = 1, poly[2] = 2, poly[3] = 8;
                poly_infty_norm_coeffmod(poly.get(), 4, 1, mod, result, pool);
                Assert::AreEqual(result[0], 0x2ULL);

                modulus[0] = 0;
                modulus[1] = 1;
                Modulus mod2(modulus.get(), 2);

                poly[0] = 1, poly[1] = 0, poly[2] = 2, poly[3] = 0;
                poly_infty_norm_coeffmod(poly.get(), 2, 2, mod2, result, pool);
                Assert::AreEqual(0x2ULL, result[0]);
                Assert::AreEqual(0x0ULL, result[1]);

                poly[0] = 0xFFFFFFFFFFFFFFFF, poly[1] = 0, poly[2] = 2, poly[3] = 0;
                poly_infty_norm_coeffmod(poly.get(), 2, 2, mod2, result, pool);
                Assert::AreEqual(0x2ULL, result[0]);
                Assert::AreEqual(0x0ULL, result[1]);
            }

            TEST_METHOD(PolyEvalPolyPolyModCoeffMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly1(allocate_zero_poly(3, 1, pool));
                Pointer poly2(allocate_zero_poly(3, 1, pool));
                Pointer polymod_anchor(allocate_zero_poly(3, 1, pool));
                polymod_anchor[0] = 2;
                polymod_anchor[1] = 0;
                polymod_anchor[2] = 1;

                Pointer result(allocate_zero_poly(3, 1, pool));
                PolyModulus polymod(polymod_anchor.get(), 3, 1);

                uint64_t modulus = 10;
                Modulus mod(&modulus, 1);

                poly1[0] = 1;
                poly1[1] = 2;
                poly1[2] = 2;
                poly2[0] = 5;
                poly2[1] = 8;
                poly2[2] = 0;
                poly_eval_poly_polymod_coeffmod(poly1.get(), poly2.get(), polymod, mod, result.get(), pool);
                Assert::AreEqual(5ULL, result[0]);
                Assert::AreEqual(6ULL, result[1]);
                Assert::AreEqual(0ULL, result[2]);
            }

            TEST_METHOD(PolyEvalUIntMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly(allocate_zero_poly(3, 1, pool));
                poly[0] = 1;
                poly[1] = 2;
                poly[2] = 3;

                uint64_t modulus = 10;
                Modulus mod(&modulus, 1);

                uint64_t value = 5;
                uint64_t result;

                poly_eval_uint_mod(poly.get(), 3, &value, mod, &result, pool);
                Assert::AreEqual(6ULL, result);

                modulus = 0xFFFFFF;
                Modulus mod2(&modulus, 1);

                poly_eval_uint_mod(poly.get(), 3, &value, mod, &result, pool);
                Assert::AreEqual(86ULL, result);
            }

            TEST_METHOD(ExponentiatePolyPolyModCoeffMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly(allocate_zero_poly(3, 1, pool));
                Pointer polymod_anchor(allocate_zero_poly(3, 1, pool));
                polymod_anchor[0] = 2;
                polymod_anchor[1] = 0;
                polymod_anchor[2] = 1;

                Pointer result(allocate_zero_poly(3, 1, pool));
                PolyModulus polymod(polymod_anchor.get(), 3, 1);

                uint64_t modulus = 10;
                Modulus mod(&modulus, 1);

                uint64_t exponent = 5;

                poly[0] = 1;
                poly[1] = 1;
                poly[2] = 0;

                exponentiate_poly_polymod_coeffmod(poly.get(), &exponent, 1, polymod, mod, result.get(), pool);
                Assert::AreEqual(1ULL, result[0]);
                Assert::AreEqual(9ULL, result[1]);
                Assert::AreEqual(0ULL, result[2]);
            }
        };
    }
}