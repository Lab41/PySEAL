#include "CppUnitTest.h"
#include "util/uintcore.h"
#include "util/polycore.h"
#include "util/polyarithmod.h"
#include <cstdint>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
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
                MemoryPool pool;
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

            TEST_METHOD(MultiplyPolyPolyPolyModCoeffMod)
            {
                MemoryPool pool;
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
                MemoryPool pool;
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
                MemoryPool pool;
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