#include "CppUnitTest.h"
#include "util/uintcore.h"
#include "util/polycore.h"
#include "util/polyarithmod.h"
#include "bigpolyarray.h"
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
            TEST_METHOD(ModuloPoly)
            {


                MemoryPool &pool = *MemoryPool::default_pool();
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
                Assert::AreEqual(static_cast<uint64_t>(1), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                Assert::AreEqual(static_cast<uint64_t>(1), result[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[3]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                modulo_poly_inplace(poly1.get(), 5, polymod2, mod2, pool);
                Assert::AreEqual(static_cast<uint64_t>(1), poly1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[1]);
                Assert::AreEqual(static_cast<uint64_t>(1), poly1[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[3]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[5]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[7]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[8]);
                Assert::AreEqual(static_cast<uint64_t>(0), poly1[9]);
            }

            TEST_METHOD(DotProductBigPolyArray)
            {
                MemoryPool &pool = *MemoryPool::default_pool();

                int coeff_uint64_count = divide_round_up(7, bits_per_uint64);

                BigPoly poly_modulus("1x^4 + 1");
                PolyModulus polymod(poly_modulus.pointer(), 5, coeff_uint64_count);
                BigUInt coeff_modulus("40");
                Modulus mod(coeff_modulus.pointer(), coeff_uint64_count, pool);
                BigPoly result(5, 7);

                // testing with general BigPolyArray and zero BigPolyArray
                BigPolyArray testzero_arr1(3, 5, 7);
                BigPolyArray testzero_arr2(3, 5, 7);
                testzero_arr1.set_zero();
                testzero_arr2.set_zero();
                testzero_arr1[0] = "Ax^3 + Bx^2";  // OK
                testzero_arr1[1] = "Cx^1";
                testzero_arr1[2] = "Dx^2 + Ex^1 + F";
                dot_product_bigpolyarray_polymod_coeffmod(testzero_arr1.pointer(0), testzero_arr2.pointer(0), 3, polymod, mod, result.pointer(), pool);
                Assert::IsTrue(result.to_string() == "0");

                // testing with BigPolyArray that should extract the ith entry of the other BigPolyArray
                BigPolyArray test_arr1(3, 5, 7);
                BigPolyArray test_arr2(3, 5, 7);
                test_arr1.set_zero();
                test_arr2.set_zero();
                test_arr1[0] = "6x^1 + 5";  // OK
                test_arr1[1] = "4x^3";
                test_arr1[2] = "3x^2 + 2x^1 + 1";
                test_arr2[2] = "1";
                dot_product_bigpolyarray_polymod_coeffmod(test_arr1.pointer(0),test_arr2.pointer(0), 3, polymod, mod, result.pointer(), pool);
                Assert::IsTrue(result.to_string() == "3x^2 + 2x^1 + 1");

                // testing with BigPolys where a polymod reduction will occur
                BigPolyArray arr1(2, 5, 7);
                BigPolyArray arr2(2, 5, 7);
                arr1[0] = "1x^1";
                arr1[1] = "1x^3";
                arr2[0] = "1";
                arr2[1] = "2x^1";
                dot_product_bigpolyarray_polymod_coeffmod(arr1.pointer(0), arr2.pointer(0), 2, polymod, mod, result.pointer(), pool);
                Assert::IsTrue(result.to_string() == "1x^1 + 3E");

                // 1 BigPoly per array, each of which is a scalar, and mod reduction will occur
                BigPolyArray scalartest1(1, 5, 7);
                BigPolyArray scalartest2(1, 5, 7);
                scalartest1[0] = "17";
                scalartest2[0] = "4";
                dot_product_bigpolyarray_polymod_coeffmod(scalartest1.pointer(0), scalartest2.pointer(0), 1, polymod, mod, result.pointer(), pool);
                Assert::IsTrue(result.to_string() == "1C");

                // 1 BigPoly per array, each of which is a scalar, one of which is zero 
                BigPolyArray scalar_zero_test1(1, 5, 7);
                BigPolyArray scalar_zero_test2(1, 5, 7);
                scalar_zero_test1[0] = "17";
                scalar_zero_test2[0] = "0";
                dot_product_bigpolyarray_polymod_coeffmod(scalar_zero_test1.pointer(0), scalar_zero_test2.pointer(0), 1, polymod, mod, result.pointer(), pool);
                Assert::IsTrue(result.to_string() == "0");
           
                // General BigPolyArray, where we expect both mod and polymod reduction
                BigPolyArray general1(4, 5, 7);
                BigPolyArray general2(4, 5, 7);
                general1[0] = "3x^2 + 2x^1";
                general1[1] = "1x^1 + 5";
                general1[2] = "1x^2 + 27";
                general1[3] = "3x^2 + 1x^1";
                general2[0] = "1x^3";
                general2[1] = "2x^1 + 6";
                general2[2] = "3x^1 + A";
                general2[3] = "12x^2 + Bx^1";
                dot_product_bigpolyarray_polymod_coeffmod(general1.pointer(0), general2.pointer(0), 4, polymod, mod, result.pointer(), pool);
                Assert::IsTrue(result.to_string() == "36x^3 + 17x^2 + 2x^1 + 2C");

                // General BigPolyArray, where we expect both mod and polymod reduction
                BigPolyArray general3(2, 5, 7);
                BigPolyArray general4(2, 5, 7);
                general3[0] = "Ax^1 + 1";
                general3[1] = "Cx^2 + 3";
                general4[0] = "Bx^1 + 2";
                general4[1] = "Dx^3 + 4";
                dot_product_bigpolyarray_polymod_coeffmod(general3.pointer(0), general4.pointer(0), 2, polymod, mod, result.pointer(), pool);
                Assert::IsTrue(result.to_string() == "27x^3 + 1Ex^2 + 3x^1 + E");
            }

            TEST_METHOD(MultiplyPolyPolyPolyModCoeffMod)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer poly1(allocate_zero_poly(4, 2, pool));
                Pointer poly2(allocate_zero_poly(4, 2, pool));
                Pointer polymod(allocate_zero_poly(4, 2, pool));
                Pointer result(allocate_zero_poly(7, 2, pool));
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
                multiply_poly_poly_polymod_coeffmod_inplace(poly1.get(), poly2.get(), polym, mod, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(3), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[3]);
                Assert::AreEqual(static_cast<uint64_t>(3), result[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[7]);
                multiply_poly_poly_polymod_coeffmod(poly1.get(), poly2.get(), polym, mod, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(3), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[3]);
                Assert::AreEqual(static_cast<uint64_t>(3), result[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[7]);
            }

            TEST_METHOD(NonFFTMultiplyPolyPolyPolyModCoeffMod)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
                Pointer poly1(allocate_zero_poly(4, 2, pool));
                Pointer poly2(allocate_zero_poly(4, 2, pool));
                Pointer polymod(allocate_zero_poly(4, 2, pool));
                Pointer result(allocate_zero_poly(7, 2, pool));
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
                nonfftmultiply_poly_poly_polymod_coeffmod_inplace(poly1.get(), poly2.get(), polym, mod, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(3), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[3]);
                Assert::AreEqual(static_cast<uint64_t>(3), result[4]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[5]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[6]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[7]);
                nonfftmultiply_poly_poly_polymod_coeffmod(poly1.get(), poly2.get(), polym, mod, result.get(), pool);
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
                MemoryPool &pool = *MemoryPool::default_pool();
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
                Assert::AreEqual(static_cast<uint64_t>(1), result[0]);
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
        };
    }
}