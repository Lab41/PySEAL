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
            TEST_METHOD(DyadicProductCoeffMod)
            {
                MemoryPool &pool = *MemoryPool::default_pool();
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
                nonfft_multiply_poly_poly_polymod_coeffmod_inplace(poly1.get(), poly2.get(), polym, mod, result.get(), pool);
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