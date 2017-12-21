#include "CppUnitTest.h"
#include "seal/util/uintcore.h"
#include "seal/util/polycore.h"
#include "seal/util/polyarithsmallmod.h"
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
        TEST_CLASS(PolyArithSmallMod)
        {
        public:
            TEST_METHOD(SmallModuloPolyCoeffs)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly(allocate_zero_poly(3, 1, pool));
                Pointer modulus(allocate_uint(2, pool));
                poly[0] = 2;
                poly[1] = 15;
                poly[2] = 77;
                SmallModulus mod(15);
                modulo_poly_coeffs(poly.get(), 3, mod, poly.get());
                Assert::AreEqual(2ULL, poly[0]);
                Assert::AreEqual(0ULL, poly[1]);
                Assert::AreEqual(2ULL, poly[2]);
            }

            TEST_METHOD(NegatePolyCoeffSmallMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly(allocate_zero_poly(3, 1, pool));
                poly[0] = 2;
                poly[1] = 3;
                poly[2] = 4;
                SmallModulus mod(15);
                negate_poly_coeffmod(poly.get(), 3, mod, poly.get());
                Assert::AreEqual(static_cast<uint64_t>(13), poly[0]);
                Assert::AreEqual(static_cast<uint64_t>(12), poly[1]);
                Assert::AreEqual(static_cast<uint64_t>(11), poly[2]);

                poly[0] = 2;
                poly[1] = 3;
                poly[2] = 4;
                mod = 0xFFFFFFFFFFFFFFULL;
                negate_poly_coeffmod(poly.get(), 3, mod, poly.get());
                Assert::AreEqual(0xFFFFFFFFFFFFFDULL, poly[0]);
                Assert::AreEqual(0xFFFFFFFFFFFFFCULL, poly[1]);
                Assert::AreEqual(0xFFFFFFFFFFFFFBULL, poly[2]);
            }

            TEST_METHOD(AddPolyPolyCoeffSmallMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly1(allocate_zero_poly(3, 1, pool));
                Pointer poly2(allocate_zero_poly(3, 1, pool));
                poly1[0] = 1;
                poly1[1] = 3;
                poly1[2] = 4;
                poly2[0] = 1;
                poly2[1] = 2;
                poly2[2] = 4;
                SmallModulus mod(5);
                add_poly_poly_coeffmod(poly1.get(), poly2.get(), 3, mod, poly1.get());
                Assert::AreEqual(2ULL, poly1[0]);
                Assert::AreEqual(0ULL, poly1[1]);
                Assert::AreEqual(3ULL, poly1[2]);
            }

            TEST_METHOD(SubPolyPolyCoeffSmallMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly1(allocate_zero_poly(3, 1, pool));
                Pointer poly2(allocate_zero_poly(3, 1, pool));
                poly1[0] = 4;
                poly1[1] = 3;
                poly1[2] = 2;
                poly2[0] = 2;
                poly2[1] = 3;
                poly2[2] = 4;
                SmallModulus mod(5);
                sub_poly_poly_coeffmod(poly1.get(), poly2.get(), 3, mod, poly1.get());
                Assert::AreEqual(2ULL, poly1[0]);
                Assert::AreEqual(0ULL, poly1[1]);
                Assert::AreEqual(3ULL, poly1[2]);
            }

            TEST_METHOD(MultiplyPolyScalarCoeffSmallMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly(allocate_zero_poly(3, 1, pool));
                poly[0] = 1;
                poly[1] = 3;
                poly[2] = 4;
                uint64_t scalar = 3;
                SmallModulus mod(5);
                multiply_poly_scalar_coeffmod(poly.get(), 3, scalar, mod, poly.get());
                Assert::AreEqual(3ULL, poly[0]);
                Assert::AreEqual(4ULL, poly[1]);
                Assert::AreEqual(2ULL, poly[2]);
            }

            TEST_METHOD(MultiplyPolyPolyCoeffSmallMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly1(allocate_zero_poly(3, 1, pool));
                Pointer poly2(allocate_zero_poly(3, 1, pool));
                Pointer result(allocate_zero_poly(5, 1, pool));
                poly1[0] = 1;
                poly1[1] = 2;
                poly1[2] = 3;
                poly2[0] = 2;
                poly2[1] = 3;
                poly2[2] = 4;
                SmallModulus mod(5);
                multiply_poly_poly_coeffmod(poly1.get(), 3, poly2.get(), 3, mod, 5, result.get());
                Assert::AreEqual(2ULL, result[0]);
                Assert::AreEqual(2ULL, result[1]);
                Assert::AreEqual(1ULL, result[2]);
                Assert::AreEqual(2ULL, result[3]);
                Assert::AreEqual(2ULL, result[4]);

                poly2[0] = 2;
                poly2[1] = 3;
                multiply_poly_poly_coeffmod(poly1.get(), 3, poly2.get(), 2, mod, 5, result.get());
                Assert::AreEqual(2ULL, result[0]);
                Assert::AreEqual(2ULL, result[1]);
                Assert::AreEqual(2ULL, result[2]);
                Assert::AreEqual(4ULL, result[3]);
                Assert::AreEqual(0ULL, result[4]);
            }

            TEST_METHOD(DividePolyPolyCoeffSmallMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly1(allocate_zero_poly(5, 1, pool));
                Pointer poly2(allocate_zero_poly(5, 1, pool));
                Pointer result(allocate_zero_poly(5, 1, pool));
                Pointer quotient(allocate_zero_poly(5, 1, pool));
                SmallModulus mod(5);

                poly1[0] = 2;
                poly1[1] = 2;
                poly2[0] = 2;
                poly2[1] = 3;
                poly2[2] = 4;
                
                divide_poly_poly_coeffmod_inplace(poly1.get(), poly2.get(), 5, mod, result.get(), pool);
                Assert::AreEqual(2ULL, poly1[0]);
                Assert::AreEqual(2ULL, poly1[1]);
                Assert::AreEqual(0ULL, poly1[2]);
                Assert::AreEqual(0ULL, poly1[3]);
                Assert::AreEqual(0ULL, poly1[4]);
                Assert::AreEqual(0ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);
                Assert::AreEqual(0ULL, result[2]);
                Assert::AreEqual(0ULL, result[3]);
                Assert::AreEqual(0ULL, result[4]);

                poly1[0] = 2;
                poly1[1] = 2;
                poly1[2] = 1;
                poly1[3] = 2;
                poly1[4] = 2;
                poly2[0] = 4;
                poly2[1] = 3;
                poly2[2] = 2;

                divide_poly_poly_coeffmod(poly1.get(), poly2.get(), 5, mod, quotient.get(), result.get(), pool);
                Assert::AreEqual(0ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);
                Assert::AreEqual(0ULL, result[2]);
                Assert::AreEqual(0ULL, result[3]);
                Assert::AreEqual(0ULL, result[4]);
                Assert::AreEqual(3ULL, quotient[0]);
                Assert::AreEqual(2ULL, quotient[1]);
                Assert::AreEqual(1ULL, quotient[2]);
                Assert::AreEqual(0ULL, quotient[3]);
                Assert::AreEqual(0ULL, quotient[4]);
            }

            TEST_METHOD(AddBigPolyArrayCoeffSmallMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                int coeff_uint64_count = divide_round_up(7, bits_per_uint64);
                SmallModulus mod(0x10);

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

            TEST_METHOD(DyadicProductCoeffSmallMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly1(allocate_zero_poly(3, 1, pool));
                Pointer poly2(allocate_zero_poly(3, 1, pool));
                Pointer result(allocate_zero_poly(3, 1, pool));
                SmallModulus mod(13);

                poly1[0] = 1;
                poly1[1] = 1;
                poly1[2] = 1;
                poly2[0] = 2;
                poly2[1] = 3;
                poly2[2] = 4;

                dyadic_product_coeffmod(poly1.get(), poly2.get(), 3, mod, result.get());
                Assert::AreEqual(2ULL, result[0]);
                Assert::AreEqual(3ULL, result[1]);
                Assert::AreEqual(4ULL, result[2]);

                poly1[0] = 0;
                poly1[1] = 0;
                poly1[2] = 0;
                poly2[0] = 2;
                poly2[1] = 3;
                poly2[2] = 4;

                dyadic_product_coeffmod(poly1.get(), poly2.get(), 3, mod, result.get());
                Assert::AreEqual(0ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);
                Assert::AreEqual(0ULL, result[2]);

                poly1[0] = 3;
                poly1[1] = 5;
                poly1[2] = 8;
                poly2[0] = 2;
                poly2[1] = 3;
                poly2[2] = 4;

                dyadic_product_coeffmod(poly1.get(), poly2.get(), 3, mod, result.get());
                Assert::AreEqual(6ULL, result[0]);
                Assert::AreEqual(2ULL, result[1]);
                Assert::AreEqual(6ULL, result[2]);
            }

            TEST_METHOD(SmallModuloPoly)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly1(allocate_zero_poly(5, 1, pool));
                Pointer poly2(allocate_zero_poly(3, 1, pool));
                Pointer result(allocate_zero_poly(3, 1, pool));
                SmallModulus mod(5);

                poly1[0] = 2;
                poly1[1] = 2;
                poly2[0] = 2;
                poly2[1] = 3;
                poly2[2] = 4;
                PolyModulus polymod(poly2.get(), 3, 1);

                modulo_poly(poly1.get(), 5, polymod, mod, result.get(), pool);
                Assert::AreEqual(2ULL, result[0]);
                Assert::AreEqual(2ULL, result[1]);
                Assert::AreEqual(0ULL, result[2]);

                modulo_poly_inplace(poly1.get(), 5, polymod, mod);
                Assert::AreEqual(2ULL, poly1[0]);
                Assert::AreEqual(2ULL, poly1[1]);
                Assert::AreEqual(0ULL, poly1[2]);
                Assert::AreEqual(0ULL, poly1[3]);
                Assert::AreEqual(0ULL, poly1[4]);

                poly1[0] = 3;
                poly1[1] = 3;
                poly1[2] = 1;
                poly1[3] = 2;
                poly1[4] = 2;
                poly2[0] = 4;
                poly2[1] = 3;
                poly2[2] = 2;
                PolyModulus polymod2(poly2.get(), 3, 1);

                modulo_poly(poly1.get(), 5, polymod2, mod, result.get(), pool);
                Assert::AreEqual(1ULL, result[0]);
                Assert::AreEqual(1ULL, result[1]);
                Assert::AreEqual(0ULL, result[2]);
                
                modulo_poly_inplace(poly1.get(), 5, polymod2, mod);
                Assert::AreEqual(1ULL, poly1[0]);
                Assert::AreEqual(1ULL, poly1[1]);
                Assert::AreEqual(0ULL, poly1[2]);
                Assert::AreEqual(0ULL, poly1[3]);
                Assert::AreEqual(0ULL, poly1[4]);
            }

            TEST_METHOD(NonFFTMultiplyPolyPolyPolyModCoeffSmallMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly1(allocate_zero_poly(5, 1, pool));
                Pointer poly2(allocate_zero_poly(5, 1, pool));
                Pointer polymod(allocate_zero_poly(5, 1, pool));
                Pointer result(allocate_zero_poly(5, 1, pool));
                SmallModulus mod(5);

                poly1[0] = 1;
                poly1[1] = 2;
                poly1[2] = 3;
                poly2[0] = 2;
                poly2[1] = 3;
                poly2[2] = 4;
                polymod[0] = 4;
                polymod[1] = 3;
                polymod[2] = 0;
                polymod[3] = 2;
                PolyModulus polym(polymod.get(), 4, 1);

                nonfft_multiply_poly_poly_polymod_coeffmod(poly1.get(), poly2.get(), polym, mod, result.get(), pool);
                Assert::AreEqual(3ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);
                Assert::AreEqual(3ULL, result[2]);
                Assert::AreEqual(0ULL, result[3]);

                polymod[0] = 1;
                polymod[1] = 0;
                polymod[2] = 0;
                polymod[3] = 0;
                polymod[4] = 1;
                polym = PolyModulus(polymod.get(), 5, 1);

                nonfft_multiply_poly_poly_polymod_coeffmod(poly1.get(), poly2.get(), polym, mod, result.get(), pool);
                Assert::AreEqual(0ULL, result[0]);
                Assert::AreEqual(2ULL, result[1]);
                Assert::AreEqual(1ULL, result[2]);
                Assert::AreEqual(2ULL, result[3]);
                Assert::AreEqual(0ULL, result[4]);
            }

            TEST_METHOD(TryInvertPolyCoeffSmallMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly(allocate_zero_poly(4, 1, pool));
                Pointer polymod(allocate_zero_poly(4, 1, pool));
                Pointer result(allocate_zero_poly(4, 1, pool));
                SmallModulus mod(5);

                polymod[0] = 4;
                polymod[1] = 3;
                polymod[2] = 0;
                polymod[3] = 2;
                
                Assert::IsFalse(try_invert_poly_coeffmod(poly.get(), polymod.get(), 4, mod, result.get(), pool));

                poly[0] = 1;
                Assert::IsTrue(try_invert_poly_coeffmod(poly.get(), polymod.get(), 4, mod, result.get(), pool));
                Assert::AreEqual(1ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);
                Assert::AreEqual(0ULL, result[2]);
                Assert::AreEqual(0ULL, result[3]);

                poly[0] = 1;
                poly[1] = 2;
                poly[2] = 3;
                Assert::IsTrue(try_invert_poly_coeffmod(poly.get(), polymod.get(), 4, mod, result.get(), pool));
                Assert::AreEqual(4ULL, result[0]);
                Assert::AreEqual(0ULL, result[1]);
                Assert::AreEqual(2ULL, result[2]);
                Assert::AreEqual(0ULL, result[3]);
            }

            TEST_METHOD(PolyInftyNormCoeffSmallMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly(allocate_zero_poly(4, 1, pool));
                SmallModulus mod(10);

                poly[0] = 0;
                poly[1] = 1;
                poly[2] = 2;
                poly[3] = 3;
                Assert::AreEqual(0x3ULL, poly_infty_norm_coeffmod(poly.get(), 4, mod));

                poly[0] = 0;
                poly[1] = 1;
                poly[2] = 2;
                poly[3] = 8;
                Assert::AreEqual(0x2ULL, poly_infty_norm_coeffmod(poly.get(), 4, mod));
            }

            TEST_METHOD(ExponentiatePolyPolyModCoeffSmallMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer poly(allocate_zero_poly(3, 1, pool));
                Pointer polymod_anchor(allocate_zero_poly(3, 1, pool));
                polymod_anchor[0] = 2;
                polymod_anchor[1] = 0;
                polymod_anchor[2] = 1;

                Pointer result(allocate_zero_poly(3, 1, pool));
                PolyModulus polymod(polymod_anchor.get(), 3, 1);
                SmallModulus mod(10);

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