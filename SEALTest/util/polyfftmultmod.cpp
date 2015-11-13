#include "CppUnitTest.h"
#include "util/polyarithmod.h"
#include "util/polyfftmultmod.h"
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
        TEST_CLASS(PolyFFTMultMod)
        {
        public:
            TEST_METHOD(FFTMultiplyPolyPolyPolyModCoeffModBase)
            {
                MemoryPool pool;
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
                fftmultiply_poly_poly_polymod_coeffmod(poly1.pointer(), poly2.pointer(), 2, mod, result.pointer(), pool);
                Assert::IsTrue("12x^3 + 18x^2 + 9x^1 + 13" == result.to_string());
            }

            TEST_METHOD(FFTMultiplyPolyPolyPolyModCoeffMod)
            {
                const int coeff_power = 8;
                const int coeff_count = (1 << coeff_power) + 1;
                MemoryPool pool;
                BigPoly poly1(coeff_count, 128);
                BigPoly poly2(coeff_count, 128);
                BigPoly polymod(coeff_count, 128);
                BigPoly correct(coeff_count, 128);
                BigPoly result(coeff_count, 128);
                BigUInt modulus(128);
                default_random_engine random;
                random.seed(0);
                for (int i = 0; i < coeff_count - 1; ++i)
                {
                    poly1[i] = random() % 15;
                    poly2[i] = random() % 15;
                }
                polymod[0] = 1;
                polymod[coeff_count - 1] = 1;
                modulus = 27;
                PolyModulus polym(polymod.pointer(), coeff_count, 2);
                Modulus mod(modulus.pointer(), 2);
                nonfftmultiply_poly_poly_polymod_coeffmod(poly1.pointer(), poly2.pointer(), polym, mod, correct.pointer(), pool);
                fftmultiply_poly_poly_polymod_coeffmod(poly1.pointer(), poly2.pointer(), coeff_power, mod, result.pointer(), pool);
                for (int i = 0; i < coeff_count; ++i)
                {
                    Assert::IsTrue(correct[i] == result[i]);
                }
            }
        };
    }
}