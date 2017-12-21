#include "CppUnitTest.h"
#include "seal/util/polyarithmod.h"
#include "seal/util/polyfftmultmod.h"
#include "seal/util/ntt.h"
#include "seal/util/mempool.h"
#include "seal/util/uintcore.h"
#include "seal/util/polycore.h"
#include "seal/util/uintarithmod.h"
#include "seal/bigpoly.h"
#include "seal/bigpolyarray.h"
#include <random>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace seal::util;
using namespace std;


namespace SEALTest
{
    namespace util
    {
        TEST_CLASS(PolyFFTMultMod)
        {
        public:
            TEST_METHOD(NussbaumerMultiplyPolyPolyCoeffMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                BigPoly poly1(5, 128);
                BigPoly poly2(5, 128);
                BigPoly result(5, 128);
                BigUInt modulus(128);
                poly1[0] = 5;
                poly1[1] = 1;
                poly1[2] = 3;
                poly1[3] = 2;
                poly2[0] = 7;
                poly2[1] = 7;
                poly2[3] = 2;
                modulus = 27;
                Modulus mod(modulus.pointer(), 2);
                nussbaumer_multiply_poly_poly_coeffmod(poly1.pointer(), poly2.pointer(), 2, mod, result.pointer(), pool);
                Assert::IsTrue("12x^3 + 18x^2 + 9x^1 + 13" == result.to_string());

                const int coeff_power = 8;
                const int coeff_count = (1 << coeff_power) + 1;
                BigPoly poly3(coeff_count, 128);
                BigPoly poly4(coeff_count, 128);
                BigPoly polymod(coeff_count, 128);
                BigPoly correct(coeff_count, 128);
                BigPoly result2(coeff_count, 128);
                random_device rd;
                for (int i = 0; i < coeff_count - 1; ++i)
                {
                    poly3[i] = rd() % 15;
                    poly4[i] = rd() % 15;
                }
                polymod[0] = 1;
                polymod[coeff_count - 1] = 1;
                PolyModulus polym(polymod.pointer(), coeff_count, 2);
                nonfft_multiply_poly_poly_polymod_coeffmod(poly3.pointer(), poly4.pointer(), polym, mod, correct.pointer(), pool);
                nussbaumer_multiply_poly_poly_coeffmod(poly3.pointer(), poly4.pointer(), coeff_power, mod, result2.pointer(), pool);
                for (int i = 0; i < coeff_count; ++i)
                {
                    Assert::IsTrue(correct[i] == result2[i]);
                }
            }

            TEST_METHOD(NTTMultiplyPolyPoly)
            {
                MemoryPoolHandle pool = MemoryPoolHandle::New();
                {
                    BigPoly poly1(4, 64);
                    BigPoly poly2(4, 64);
                    BigUInt modulus(64);
                    poly1[0] = 5;
                    poly1[1] = 1;
                    poly1[2] = 3;
                    poly1[3] = 2;

                    poly2[0] = 7;
                    poly2[1] = 7;
                    poly2[2] = 0;
                    poly2[3] = 2;

                    modulus = 17;

                    Pointer result(allocate_poly(4, 1, pool));
                    Modulus mod(modulus.pointer(), 1);

                    int coeff_count_power = 2;
                    NTTTables tables(pool);
                    tables.generate(coeff_count_power, mod);
                    ntt_multiply_poly_poly(poly1.pointer(), poly2.pointer(), tables, result.get(), pool);
                    Assert::AreEqual(result.get()[0], static_cast<uint64_t>(2));
                    Assert::AreEqual(result.get()[1], static_cast<uint64_t>(2));
                    Assert::AreEqual(result.get()[2], static_cast<uint64_t>(7));
                    Assert::AreEqual(result.get()[3], static_cast<uint64_t>(11));
                }

                {
                    Pointer modulus_anchor(allocate_uint(1, pool));
                    modulus_anchor[0] = 65537;
                    Modulus mod(modulus_anchor.get(), 1);
                    int coeff_count_power = 10;
                    int coeff_count = (1 << coeff_count_power) + 1;
                    Pointer poly1(allocate_zero_poly(coeff_count, 1, pool));
                    Pointer poly2(allocate_zero_poly(coeff_count, 1, pool));
                    Pointer result(allocate_zero_poly(coeff_count, 1, pool));
                    Pointer correct(allocate_zero_poly(coeff_count, 1, pool));

                    random_device rd;

                    for (int i = 0; i < coeff_count - 1; ++i)
                    {
                        poly1[i] = rd() % *mod.get();
                        poly2[i] = rd() % *mod.get();
                    }

                    BigPoly polymod("1x^1024 + 1");
                    PolyModulus polym(polymod.pointer(), coeff_count, 1);
                    NTTTables tables(coeff_count_power, mod, pool);

                    nonfft_multiply_poly_poly_polymod_coeffmod(poly1.get(), poly2.get(), polym, mod, correct.get(), pool);
                    ntt_multiply_poly_poly(poly1.get(), poly2.get(), tables, result.get(), pool);

                    for (int i = 0; i < coeff_count; ++i)
                    {
                        Assert::IsTrue(correct[i] == result[i]);
                    }
                }
            }

            TEST_METHOD(NTTDotProductBigPolyArrayNTTBigPolyArray)
            {
                MemoryPoolHandle pool = MemoryPoolHandle::New();
                NTTTables tables(pool);
                int coeff_uint64_count = divide_round_up(7, bits_per_uint64);

                BigPoly poly_modulus("1x^4 + 1");
                PolyModulus polymod(poly_modulus.pointer(), 5, coeff_uint64_count);
                BigUInt coeff_modulus("61"); // decimal number 97 
                Modulus mod(coeff_modulus.pointer(), coeff_uint64_count, pool);
                BigPoly result(5, 7);

                // testing with general BigPolyArray and zero BigPolyArray
                BigPolyArray testzero_arr1(3, 5, 7);
                BigPolyArray testzero_arr2(3, 5, 7);
                testzero_arr1.set_zero();
                testzero_arr2.set_zero();
                BigPoly(testzero_arr1.coeff_count(), testzero_arr1.coeff_bit_count(), testzero_arr1.pointer(0)) = "Ax^3 + Bx^2";  // OK
                BigPoly(testzero_arr1.coeff_count(), testzero_arr1.coeff_bit_count(), testzero_arr1.pointer(1)) = "Cx^1";
                BigPoly(testzero_arr1.coeff_count(), testzero_arr1.coeff_bit_count(), testzero_arr1.pointer(2)) = "Dx^2 + Ex^1 + F";

                tables.generate(2, mod); 

                ntt_dot_product_bigpolyarray_nttbigpolyarray(testzero_arr1.pointer(0), testzero_arr2.pointer(0), 3, coeff_uint64_count*5, tables, result.pointer(), pool);
                Assert::IsTrue(result.to_string() == "0");

                // testing with BigPolyArray that should extract the ith entry of the other BigPolyArray
                BigPolyArray test_arr1(3, 5, 7);
                BigPolyArray test_arr2(3, 5, 7);
                test_arr1.set_zero();
                test_arr2.set_zero();
                BigPoly(test_arr1.coeff_count(), test_arr1.coeff_bit_count(), test_arr1.pointer(0)) = "6x^1 + 5";  // OK
                BigPoly(test_arr1.coeff_count(), test_arr1.coeff_bit_count(), test_arr1.pointer(1)) = "4x^3";
                BigPoly(test_arr1.coeff_count(), test_arr1.coeff_bit_count(), test_arr1.pointer(2)) = "3x^2 + 2x^1 + 1";
                BigPoly(test_arr2.coeff_count(), test_arr2.coeff_bit_count(), test_arr2.pointer(2)) = "1x^3 + 1x^2 + 1x^1 + 1";
                ntt_dot_product_bigpolyarray_nttbigpolyarray(test_arr1.pointer(0),test_arr2.pointer(0), 3, coeff_uint64_count*5, tables, result.pointer(), pool);
                Assert::IsTrue(result.to_string() == "3x^2 + 2x^1 + 1");

                // testing with BigPolys where a polymod reduction will occur
                BigPolyArray arr1(2, 5, 7);
                BigPolyArray arr2(2, 5, 7);
                BigPoly(arr1.coeff_count(), arr1.coeff_bit_count(), arr1.pointer(0)) = "1x^1";
                BigPoly(arr1.coeff_count(), arr1.coeff_bit_count(), arr1.pointer(1)) = "1x^3";
                BigPoly(arr2.coeff_count(), arr2.coeff_bit_count(), arr2.pointer(0)) = "1";
                BigPoly(arr2.coeff_count(), arr2.coeff_bit_count(), arr2.pointer(1)) = "2x^1";
                for (int i = 0; i < 2; i++)
                {
                    ntt_negacyclic_harvey(arr2.pointer(i), tables, pool); 
                }
                ntt_dot_product_bigpolyarray_nttbigpolyarray(arr1.pointer(0), arr2.pointer(0), 2, 5, tables, result.pointer(), pool);
                Assert::IsTrue(result.to_string() == "1x^1 + 5F");

                // 1 BigPoly per array, each of which is a scalar, and mod reduction will occur
                BigPolyArray scalartest1(1, 5, 7);
                BigPolyArray scalartest2(1, 5, 7);
                BigPoly(scalartest1.coeff_count(), scalartest1.coeff_bit_count(), scalartest1.pointer(0)) = "2";
                BigPoly(scalartest2.coeff_count(), scalartest2.coeff_bit_count(), scalartest2.pointer(0)) = "4x^3 + 4x^2 + 4x^1 + 4";
                ntt_dot_product_bigpolyarray_nttbigpolyarray(scalartest1.pointer(0), scalartest2.pointer(0), 1, coeff_uint64_count*5, tables, result.pointer(), pool);
                Assert::IsTrue(result.to_string() == "8");

                //// 1 BigPoly per array, each of which is a scalar, one of which is zero 
                BigPolyArray scalar_zero_test1(1, 5, 7);
                BigPolyArray scalar_zero_test2(1, 5, 7);
                BigPoly(scalar_zero_test1.coeff_count(), scalar_zero_test1.coeff_bit_count(), scalar_zero_test1.pointer(0)) = "17";
                BigPoly(scalar_zero_test2.coeff_count(), scalar_zero_test2.coeff_bit_count(), scalar_zero_test2.pointer(0)) = "0";
                ntt_dot_product_bigpolyarray_nttbigpolyarray(scalar_zero_test1.pointer(0), scalar_zero_test2.pointer(0), 1, coeff_uint64_count*5, tables, result.pointer(), pool);
                Assert::IsTrue(result.to_string() == "0");
            }
        };
    }
}