#include "CppUnitTest.h"
#include "util/polyfftmult.h"
#include "util/polyarith.h"
#include "util/uintarith.h"
#include <random>
#include "bigpoly.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace seal::util;
using namespace std;

namespace SEALTest
{
    namespace util
    {
        TEST_CLASS(PolyFFTMult)
        {
        public:
            TEST_METHOD(NussbaumerMultiplyPolyPolyBase)
            {
                BigPoly poly1(5, 128);
                BigPoly poly2(5, 128);
                BigPoly result(5, 128);
                poly1[0] = 5;
                poly1[1] = 1;
                poly1[2] = 3;
                poly1[3] = 8;
                poly2[0] = 7;
                poly2[1] = 7;
                poly2[3] = 2;
                MemoryPool &pool = *MemoryPool::default_pool();
                nussbaumer_multiply_poly_poly(poly1.pointer(), poly2.pointer(), 2, 2, 2, 2, result.pointer(), pool);
                Assert::IsTrue("57x^3 + Cx^2 + 24x^1 + FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE9" == result.to_string());

                result.resize(5, 64);
                result.set_zero();
                nussbaumer_multiply_poly_poly(poly1.pointer(), poly2.pointer(), 2, 2, 2, 1, result.pointer(), pool);
                Assert::IsTrue("57x^3 + Cx^2 + 24x^1 + FFFFFFFFFFFFFFE9" == result.to_string());

                result.resize(5, 128);
                result.set_zero();
                poly1.resize(5, 64);
                poly2.resize(5, 64);
                nussbaumer_multiply_poly_poly(poly1.pointer(), poly2.pointer(), 2, 1, 2,2, result.pointer(), pool);
                Assert::IsTrue("57x^3 + Cx^2 + 24x^1 + FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE9" == result.to_string());
            }

            TEST_METHOD(NussbaumerMultiplyPolyPoly)
            {
                const int coeff_power = 8;
                const int coeff_count = (1 << coeff_power) + 1;
                BigPoly poly1(coeff_count, 128);
                BigPoly poly2(coeff_count, 128);
                BigPoly polymod(coeff_count, 128);
                BigPoly correct(2 * coeff_count, 128);
                BigPoly result(coeff_count, 128);
                default_random_engine random;
                random.seed(0);
                for (int i = 0; i < coeff_count - 1; ++i)
                {
                    poly1[i] = random() % 100;
                    poly2[i] = random() % 100;
                }
                polymod[0] = 1;
                polymod[coeff_count - 1] = 1;
                MemoryPool &pool = *MemoryPool::default_pool();
                multiply_poly_poly(poly1.pointer(), coeff_count, 2, poly2.pointer(), coeff_count, 2, 2 * coeff_count, 2, correct.pointer(), pool);
                for (int i = coeff_count - 1; i < 2 * coeff_count; ++i)
                {
                    uint64_t *lower_coeff = get_poly_coeff(correct.pointer(), i - (coeff_count - 1), 2);
                    uint64_t *upper_coeff = get_poly_coeff(correct.pointer(), i, 2);
                    sub_uint_uint(lower_coeff, upper_coeff, 2, lower_coeff);
                    set_zero_uint(2, upper_coeff);
                }
                nussbaumer_multiply_poly_poly(poly1.pointer(), poly2.pointer(), coeff_power, 2, 2, 2, result.pointer(), pool);
                for (int i = 0; i < coeff_count; ++i)
                {
                    Assert::IsTrue(correct[i] == result[i]);
                }
            }

            TEST_METHOD(NussbaumerCrossMultiplyPolyPolyBase)
            {
                BigPoly poly1(5, 64);
                BigPoly poly2(5, 64);
                BigPoly result11(5, 128);
                BigPoly result12(5, 128);
                BigPoly result22(5, 128);

                poly1[0] = 5;
                poly1[1] = 1;
                poly1[2] = 3;
                poly1[3] = 1;

                poly2[0] = 7;
                poly2[1] = 7;
                poly2[2] = 3;
                poly2[3] = 2;

                int coeff_count_power = 2; 

                MemoryPool &pool = *MemoryPool::default_pool();
                nussbaumer_cross_multiply_poly_poly(poly1.pointer(), poly2.pointer(), coeff_count_power, 1, 1, 2, result11.pointer(), result22.pointer(), result12.pointer(),pool);

                BigPoly result11_correct(5, 128); 
                nussbaumer_multiply_poly_poly(poly1.pointer(), poly1.pointer(), coeff_count_power, 1, 1, 2, result11_correct.pointer(), pool);
                Assert::AreEqual(result11_correct.pointer()[0], result11.pointer()[0]);
                Assert::AreEqual(result11.pointer()[1], result11_correct.pointer()[1]);
                Assert::AreEqual(result11.pointer()[2], result11_correct.pointer()[2]);
                Assert::AreEqual(result11.pointer()[3], result11_correct.pointer()[3]);

                BigPoly result22_correct(5, 128);
                nussbaumer_multiply_poly_poly(poly2.pointer(), poly2.pointer(), coeff_count_power, 1, 1, 2, result22_correct.pointer(), pool);
                for (int i = 0; i < 4; i++)
                {
                    Assert::AreEqual(result22_correct.pointer()[i], result22.pointer()[i]);
                }

                BigPoly result12_correct(5, 128);
                nussbaumer_multiply_poly_poly(poly1.pointer(), poly2.pointer(), coeff_count_power, 1, 1, 2, result12_correct.pointer(), pool);
                for (int i = 0; i < 4; i++)
                {
                    Assert::AreEqual(result12_correct.pointer()[i], result12.pointer()[i]);
                }

            }

            TEST_METHOD(NussbaumerCrossMultiplyPolyPoly)
            {
                const int coeff_power = 8;
                const int coeff_count = (1 << coeff_power) + 1;
                BigPoly poly1(coeff_count, 128);
                BigPoly poly2(coeff_count, 128);
                BigPoly polymod(coeff_count, 128);
                BigPoly correct11(coeff_count, 128);
                BigPoly correct12(coeff_count, 128);
                BigPoly correct22(coeff_count, 128);
                BigPoly result11(coeff_count, 128);
                BigPoly result12(coeff_count, 128);
                BigPoly result22(coeff_count, 128);

                default_random_engine random;
                random.seed(0);
                for (int i = 0; i < coeff_count - 1; ++i)
                {
                    poly1[i] = random() % 100;
                    poly2[i] = random() % 100;
                }
                polymod[0] = 1;
                polymod[coeff_count - 1] = 1;
                MemoryPool &pool = *MemoryPool::default_pool();

                nussbaumer_cross_multiply_poly_poly(poly1.pointer(), poly2.pointer(), coeff_power, 1, 2, 2, result11.pointer(), result22.pointer(), result12.pointer(), pool);


                nussbaumer_multiply_poly_poly(poly1.pointer(), poly1.pointer(), coeff_power, 1, 2, 2, correct11.pointer(), pool);
                nussbaumer_multiply_poly_poly(poly1.pointer(), poly2.pointer(), coeff_power, 1, 2, 2, correct12.pointer(), pool);
                nussbaumer_multiply_poly_poly(poly2.pointer(), poly2.pointer(), coeff_power, 1, 2, 2, correct22.pointer(), pool);

                for (int i = 0; i < coeff_count - 1; ++i)
                {
                    Assert::IsTrue(correct11[i] == result11[i]);
                    Assert::IsTrue(correct12[i] == result12[i]);
                    Assert::IsTrue(correct22[i] == result22[i]);
                }
            }
        };
    }
}