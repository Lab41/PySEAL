#include "CppUnitTest.h"
#include "util/polyfftmult.h"
#include "util/polyarith.h"
#include "util/uintarith.h"
#include <random>
#include <bigpoly.h>

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
            TEST_METHOD(FFTMultiplyPolyPolyPolyModBase)
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
                MemoryPool pool;
                fftmultiply_poly_poly_polymod(poly1.pointer(), poly2.pointer(), 2, 2, 2, result.pointer(), pool);
                Assert::IsTrue("57x^3 + Cx^2 + 24x^1 + FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE9" == result.to_string());

                result.resize(5, 64);
                result.set_zero();
                fftmultiply_poly_poly_polymod(poly1.pointer(), poly2.pointer(), 2, 2, 1, result.pointer(), pool);
                Assert::IsTrue("57x^3 + Cx^2 + 24x^1 + FFFFFFFFFFFFFFE9" == result.to_string());

                result.resize(5, 128);
                result.set_zero();
                poly1.resize(5, 64);
                poly2.resize(5, 64);
                fftmultiply_poly_poly_polymod(poly1.pointer(), poly2.pointer(), 2, 1, 2, result.pointer(), pool);
                Assert::IsTrue("57x^3 + Cx^2 + 24x^1 + FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE9" == result.to_string());
            }

            TEST_METHOD(FFTMultiplyPolyPolyPolyMod)
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
                    poly1[i] = random() % 15;
                    poly2[i] = random() % 15;
                }
                polymod[0] = 1;
                polymod[coeff_count - 1] = 1;
                MemoryPool pool;
                multiply_poly_poly(poly1.pointer(), coeff_count, 2, poly2.pointer(), coeff_count, 2, 2 * coeff_count, 2, correct.pointer(), pool);
                for (int i = coeff_count - 1; i < 2 * coeff_count; ++i)
                {
                    uint64_t *lower_coeff = get_poly_coeff(correct.pointer(), i - (coeff_count - 1), 2);
                    uint64_t *upper_coeff = get_poly_coeff(correct.pointer(), i, 2);
                    sub_uint_uint(lower_coeff, upper_coeff, 2, lower_coeff);
                    set_zero_uint(2, upper_coeff);
                }
                fftmultiply_poly_poly_polymod(poly1.pointer(), poly2.pointer(), coeff_power, 2, 2, result.pointer(), pool);
                for (int i = 0; i < coeff_count; ++i)
                {
                    Assert::IsTrue(correct[i] == result[i]);
                }
            }
        };
    }
}