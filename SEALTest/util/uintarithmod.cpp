#include "CppUnitTest.h"
#include "util/uintcore.h"
#include "util/uintarithmod.h"
#include <cstdint>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal::util;
using namespace std;

namespace SEALTest
{
    namespace util
    {
        TEST_CLASS(UIntArithMod)
        {
        public:
            TEST_METHOD(ModuloUInt)
            {
                MemoryPool pool;
                Pointer value(allocate_uint(4, pool));
                Pointer modulus(allocate_uint(2, pool));
                Pointer result(allocate_uint(2, pool));
                value[0] = 0;
                value[1] = 0;
                value[2] = 0;
                modulus[0] = 2;
                modulus[1] = 0;
                Modulus mod(modulus.get(), 2);
                modulo_uint_inplace(value.get(), 3, mod, pool);
                Assert::AreEqual(static_cast<uint64_t>(0), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[2]);

                value[0] = 1;
                value[1] = 0;
                value[2] = 0;
                modulo_uint_inplace(value.get(), 3, mod, pool);
                Assert::AreEqual(static_cast<uint64_t>(1), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[2]);

                value[0] = 2;
                value[1] = 0;
                value[2] = 0;
                modulo_uint_inplace(value.get(), 3, mod, pool);
                Assert::AreEqual(static_cast<uint64_t>(0), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[2]);

                value[0] = 3;
                value[1] = 0;
                value[2] = 0;
                modulo_uint_inplace(value.get(), 3, mod, pool);
                Assert::AreEqual(static_cast<uint64_t>(1), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[2]);

                value[0] = 9585656442714717620;
                value[1] = 1817697005049051848;
                value[2] = 0;
                modulus[0] = 0xFFFF;
                modulus[1] = 0;
                Modulus mod2(modulus.get(), 2);
                modulo_uint_inplace(value.get(), 3, mod2, pool);
                Assert::AreEqual(static_cast<uint64_t>(65143), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[2]);

                // Number: 423390605348012810342478682114936219074165532129103049974799020242001591732
                // value[0] = 9585656442714717620;  0x850717BF66F1FDB4
                // value[1] = 1817697005049051848;  0x1939C1CBA73D7AC8
                // value[2] = 14447416709120365380; 0xC87F88F385299344
                // value[3] = 67450014862939159;    0xEFA16E60001417
                value[0] = 9585656442714717620;
                value[1] = 1817697005049051848;
                value[2] = 0;
                modulus[0] = 0x1000;
                modulus[1] = 0;
                Modulus mod3(modulus.get(), 2);
                modulo_uint_inplace(value.get(), 3, mod3, pool);
                Assert::AreEqual(static_cast<uint64_t>(0xDB4), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[2]);

                value[0] = 9585656442714717620;
                value[1] = 1817697005049051848;
                value[2] = 14447416709120365380;
                value[3] = 67450014862939159;
                modulus[0] = 0xFFFFFFFFC001;
                modulus[1] = 0;
                Modulus mod4(modulus.get(), 2);
                modulo_uint_inplace(value.get(), 4, mod4, pool);
                Assert::AreEqual(static_cast<uint64_t>(124510066632001), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[3]);

                value[0] = 9585656442714717620;
                value[1] = 1817697005049051848;
                value[2] = 14447416709120365380;
                value[3] = 67450014862939159;
                modulus[0] = 0x9ABCDEF000000000;
                modulus[1] = 0xFFFFFFFF12345678;
                Modulus mod5(modulus.get(), 2);
                modulo_uint_inplace(value.get(), 4, mod5, pool);
                Assert::AreEqual(static_cast<uint64_t>(0xAEF527BF66F1FDB4), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xD1BF92146640262A), value[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[3]);

                value[0] = 9585656442714717620;
                value[1] = 1817697005049051848;
                value[2] = 14447416709120365380;
                value[3] = 67450014862939159;
                modulus[0] = 0xFFFFFFFFF0000FFF;
                modulus[1] = 0x000000000000000F;
                Modulus mod6(modulus.get(), 2);
                modulo_uint_inplace(value.get(), 4, mod6, pool);
                Assert::AreEqual(static_cast<uint64_t>(0xE60ABB34A2D9096E), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[3]);

                value[0] = 9585656442714717620;
                value[1] = 1817697005049051848;
                value[2] = 14447416709120365380;
                value[3] = 67450014862939159;
                modulus[0] = 701538366196406307;
                modulus[1] = 1699883529753102283;
                result[0] = 0xFFFFFFFFFFFFFFFF;
                result[1] = 0xFFFFFFFFFFFFFFFF;
                Modulus mod7(modulus.get(), 2);
                modulo_uint(value.get(), 4, mod7, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(2), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                modulo_uint_inplace(value.get(), 4, mod7, pool);
                Assert::AreEqual(static_cast<uint64_t>(2), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[3]);

                value[0] = 9585656442714717618;
                value[1] = 1817697005049051848;
                value[2] = 14447416709120365380;
                value[3] = 67450014862939159;
                modulus[0] = 701538366196406307;
                modulus[1] = 1699883529753102283;
                result[0] = 0xFFFFFFFFFFFFFFFF;
                result[1] = 0xFFFFFFFFFFFFFFFF;
                Modulus mod8(modulus.get(), 2);
                modulo_uint(value.get(), 4, mod8, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(0), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                modulo_uint_inplace(value.get(), 4, mod8, pool);
                Assert::AreEqual(static_cast<uint64_t>(0), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[2]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[3]);
            }

            TEST_METHOD(IncrementUIntMod)
            {
                MemoryPool pool;
                Pointer value(allocate_uint(2, pool));
                Pointer modulus(allocate_uint(2, pool));
                value[0] = 0;
                value[1] = 0;
                modulus[0] = 3;
                modulus[1] = 0;
                increment_uint_mod(value.get(), modulus.get(), 2, value.get());
                Assert::AreEqual(static_cast<uint64_t>(1), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);
                increment_uint_mod(value.get(), modulus.get(), 2, value.get());
                Assert::AreEqual(static_cast<uint64_t>(2), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);
                increment_uint_mod(value.get(), modulus.get(), 2, value.get());
                Assert::AreEqual(static_cast<uint64_t>(0), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);

                value[0] = 0xFFFFFFFFFFFFFFFD;
                value[1] = 0xFFFFFFFFFFFFFFFF;
                modulus[0] = 0xFFFFFFFFFFFFFFFF;
                modulus[1] = 0xFFFFFFFFFFFFFFFF;
                increment_uint_mod(value.get(), modulus.get(), 2, value.get());
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), value[1]);
                increment_uint_mod(value.get(), modulus.get(), 2, value.get());
                Assert::AreEqual(static_cast<uint64_t>(0), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);
                increment_uint_mod(value.get(), modulus.get(), 2, value.get());
                Assert::AreEqual(static_cast<uint64_t>(1), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);
            }

            TEST_METHOD(DecrementUIntMod)
            {
                MemoryPool pool;
                Pointer value(allocate_uint(2, pool));
                Pointer modulus(allocate_uint(2, pool));
                value[0] = 2;
                value[1] = 0;
                modulus[0] = 3;
                modulus[1] = 0;
                decrement_uint_mod(value.get(), modulus.get(), 2, value.get());
                Assert::AreEqual(static_cast<uint64_t>(1), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);
                decrement_uint_mod(value.get(), modulus.get(), 2, value.get());
                Assert::AreEqual(static_cast<uint64_t>(0), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);
                decrement_uint_mod(value.get(), modulus.get(), 2, value.get());
                Assert::AreEqual(static_cast<uint64_t>(2), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);

                value[0] = 1;
                value[1] = 0;
                modulus[0] = 0xFFFFFFFFFFFFFFFF;
                modulus[1] = 0xFFFFFFFFFFFFFFFF;
                decrement_uint_mod(value.get(), modulus.get(), 2, value.get());
                Assert::AreEqual(static_cast<uint64_t>(0), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);
                decrement_uint_mod(value.get(), modulus.get(), 2, value.get());
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFE), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), value[1]);
                decrement_uint_mod(value.get(), modulus.get(), 2, value.get());
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFD), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), value[1]);
            }

            TEST_METHOD(NegateUIntMod)
            {
                MemoryPool pool;
                Pointer value(allocate_uint(2, pool));
                Pointer modulus(allocate_uint(2, pool));
                value[0] = 0;
                value[1] = 0;
                modulus[0] = 3;
                modulus[1] = 0;
                negate_uint_mod(value.get(), modulus.get(), 2, value.get());
                Assert::AreEqual(static_cast<uint64_t>(0), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);

                value[0] = 1;
                value[1] = 0;
                modulus[0] = 3;
                modulus[1] = 0;
                negate_uint_mod(value.get(), modulus.get(), 2, value.get());
                Assert::AreEqual(static_cast<uint64_t>(2), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);
                negate_uint_mod(value.get(), modulus.get(), 2, value.get());
                Assert::AreEqual(static_cast<uint64_t>(1), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);

                value[0] = 2;
                value[1] = 0;
                modulus[0] = 0xFFFFFFFFFFFFFFFF;
                modulus[1] = 0xFFFFFFFFFFFFFFFF;
                negate_uint_mod(value.get(), modulus.get(), 2, value.get());
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFD), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), value[1]);
                negate_uint_mod(value.get(), modulus.get(), 2, value.get());
                Assert::AreEqual(static_cast<uint64_t>(2), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);
            }

            TEST_METHOD(AddUIntUIntMod)
            {
                MemoryPool pool;
                Pointer value1(allocate_uint(2, pool));
                Pointer value2(allocate_uint(2, pool));
                Pointer modulus(allocate_uint(2, pool));
                value1[0] = 0;
                value1[1] = 0;
                value2[0] = 0;
                value2[1] = 0;
                modulus[0] = 3;
                modulus[1] = 0;
                add_uint_uint_mod(value1.get(), value2.get(), modulus.get(), 2, value1.get());
                Assert::AreEqual(static_cast<uint64_t>(0), value1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value1[1]);

                value1[0] = 1;
                value1[1] = 0;
                value2[0] = 1;
                value2[1] = 0;
                modulus[0] = 3;
                modulus[1] = 0;
                add_uint_uint_mod(value1.get(), value2.get(), modulus.get(), 2, value1.get());
                Assert::AreEqual(static_cast<uint64_t>(2), value1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value1[1]);

                value1[0] = 1;
                value1[1] = 0;
                value2[0] = 2;
                value2[1] = 0;
                modulus[0] = 3;
                modulus[1] = 0;
                add_uint_uint_mod(value1.get(), value2.get(), modulus.get(), 2, value1.get());
                Assert::AreEqual(static_cast<uint64_t>(0), value1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value1[1]);

                value1[0] = 2;
                value1[1] = 0;
                value2[0] = 2;
                value2[1] = 0;
                modulus[0] = 3;
                modulus[1] = 0;
                add_uint_uint_mod(value1.get(), value2.get(), modulus.get(), 2, value1.get());
                Assert::AreEqual(static_cast<uint64_t>(1), value1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value1[1]);

                value1[0] = 0xFFFFFFFFFFFFFFFE;
                value1[1] = 0xFFFFFFFFFFFFFFFF;
                value2[0] = 0xFFFFFFFFFFFFFFFE;
                value2[1] = 0xFFFFFFFFFFFFFFFF;
                modulus[0] = 0xFFFFFFFFFFFFFFFF;
                modulus[1] = 0xFFFFFFFFFFFFFFFF;
                add_uint_uint_mod(value1.get(), value2.get(), modulus.get(), 2, value1.get());
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFD), value1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), value1[1]);
            }

            TEST_METHOD(SubUIntUIntMod)
            {
                MemoryPool pool;
                Pointer value1(allocate_uint(2, pool));
                Pointer value2(allocate_uint(2, pool));
                Pointer modulus(allocate_uint(2, pool));
                value1[0] = 0;
                value1[1] = 0;
                value2[0] = 0;
                value2[1] = 0;
                modulus[0] = 3;
                modulus[1] = 0;
                sub_uint_uint_mod(value1.get(), value2.get(), modulus.get(), 2, value1.get());
                Assert::AreEqual(static_cast<uint64_t>(0), value1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value1[1]);

                value1[0] = 2;
                value1[1] = 0;
                value2[0] = 1;
                value2[1] = 0;
                modulus[0] = 3;
                modulus[1] = 0;
                sub_uint_uint_mod(value1.get(), value2.get(), modulus.get(), 2, value1.get());
                Assert::AreEqual(static_cast<uint64_t>(1), value1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value1[1]);

                value1[0] = 1;
                value1[1] = 0;
                value2[0] = 2;
                value2[1] = 0;
                modulus[0] = 3;
                modulus[1] = 0;
                sub_uint_uint_mod(value1.get(), value2.get(), modulus.get(), 2, value1.get());
                Assert::AreEqual(static_cast<uint64_t>(2), value1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value1[1]);

                value1[0] = 2;
                value1[1] = 0;
                value2[0] = 2;
                value2[1] = 0;
                modulus[0] = 3;
                modulus[1] = 0;
                sub_uint_uint_mod(value1.get(), value2.get(), modulus.get(), 2, value1.get());
                Assert::AreEqual(static_cast<uint64_t>(0), value1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value1[1]);

                value1[0] = 1;
                value1[1] = 0;
                value2[0] = 0xFFFFFFFFFFFFFFFE;
                value2[1] = 0xFFFFFFFFFFFFFFFF;
                modulus[0] = 0xFFFFFFFFFFFFFFFF;
                modulus[1] = 0xFFFFFFFFFFFFFFFF;
                sub_uint_uint_mod(value1.get(), value2.get(), modulus.get(), 2, value1.get());
                Assert::AreEqual(static_cast<uint64_t>(2), value1[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value1[1]);
            }

            TEST_METHOD(MultiplyUIntUIntMod)
            {
                MemoryPool pool;
                Pointer value1(allocate_uint(2, pool));
                Pointer value2(allocate_uint(2, pool));
                Pointer modulus(allocate_uint(2, pool));
                Pointer result(allocate_uint(4, pool));
                value1[0] = 0;
                value1[1] = 0;
                value2[0] = 0;
                value2[1] = 0;
                modulus[0] = 3;
                modulus[1] = 0;
                result[0] = 0xFFFFFFFFFFFFFFFF;
                result[1] = 0xFFFFFFFFFFFFFFFF;
                Modulus mod(modulus.get(), 2);
                multiply_uint_uint_mod_inplace(value1.get(), value2.get(), mod, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(0), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);

                value1[0] = 2;
                value1[1] = 0;
                value2[0] = 1;
                value2[1] = 0;
                result[0] = 0xFFFFFFFFFFFFFFFF;
                result[1] = 0xFFFFFFFFFFFFFFFF;
                multiply_uint_uint_mod_inplace(value1.get(), value2.get(), mod, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(2), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);

                value1[0] = 2;
                value1[1] = 0;
                value2[0] = 2;
                value2[1] = 0;
                result[0] = 0xFFFFFFFFFFFFFFFF;
                result[1] = 0xFFFFFFFFFFFFFFFF;
                multiply_uint_uint_mod_inplace(value1.get(), value2.get(), mod, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(1), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);

                value1[0] = 0xFFFFFFFFFFFFFFFF;
                value1[1] = 0x7FFFFFFFFFFFFFFF;
                value2[0] = 0xFFFFFFFFFFFFFFFF;
                value2[1] = 0x7FFFFFFFFFFFFFFF;
                modulus[0] = 0;
                modulus[1] = 0x8000000000000000;
                result[0] = 0xFFFFFFFFFFFFFFFF;
                result[1] = 0xFFFFFFFFFFFFFFFF;
                Modulus mod2(modulus.get(), 2);
                multiply_uint_uint_mod_inplace(value1.get(), value2.get(), mod2, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(1), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
                result[0] = 0xFFFFFFFFFFFFFFFF;
                result[1] = 0xFFFFFFFFFFFFFFFF;
                multiply_uint_uint_mod(value1.get(), value2.get(), mod2, result.get(), pool);
                Assert::AreEqual(static_cast<uint64_t>(1), result[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), result[1]);
            }

            TEST_METHOD(TryInvertUIntMod)
            {
                MemoryPool pool;
                Pointer value(allocate_uint(2, pool));
                Pointer modulus(allocate_uint(2, pool));
                value[0] = 0;
                value[1] = 0;
                modulus[0] = 5;
                modulus[1] = 0;
                Assert::IsFalse(try_invert_uint_mod(value.get(), modulus.get(), 2, value.get(), pool));

                value[0] = 1;
                value[1] = 0;
                modulus[0] = 5;
                modulus[1] = 0;
                Assert::IsTrue(try_invert_uint_mod(value.get(), modulus.get(), 2, value.get(), pool));
                Assert::AreEqual(static_cast<uint64_t>(1), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);

                value[0] = 2;
                value[1] = 0;
                modulus[0] = 5;
                modulus[1] = 0;
                Assert::IsTrue(try_invert_uint_mod(value.get(), modulus.get(), 2, value.get(), pool));
                Assert::AreEqual(static_cast<uint64_t>(3), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);

                value[0] = 3;
                value[1] = 0;
                modulus[0] = 5;
                modulus[1] = 0;
                Assert::IsTrue(try_invert_uint_mod(value.get(), modulus.get(), 2, value.get(), pool));
                Assert::AreEqual(static_cast<uint64_t>(2), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);

                value[0] = 4;
                value[1] = 0;
                modulus[0] = 5;
                modulus[1] = 0;
                Assert::IsTrue(try_invert_uint_mod(value.get(), modulus.get(), 2, value.get(), pool));
                Assert::AreEqual(static_cast<uint64_t>(4), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);

                value[0] = 2;
                value[1] = 0;
                modulus[0] = 6;
                modulus[1] = 0;
                Assert::IsFalse(try_invert_uint_mod(value.get(), modulus.get(), 2, value.get(), pool));

                value[0] = 3;
                value[1] = 0;
                modulus[0] = 6;
                modulus[1] = 0;
                Assert::IsFalse(try_invert_uint_mod(value.get(), modulus.get(), 2, value.get(), pool));

                value[0] = 331975426;
                value[1] = 0;
                modulus[0] = 1351315121;
                modulus[1] = 0;
                Assert::IsTrue(try_invert_uint_mod(value.get(), modulus.get(), 2, value.get(), pool));
                Assert::AreEqual(static_cast<uint64_t>(1052541512), value[0]);
                Assert::AreEqual(static_cast<uint64_t>(0), value[1]);
            }
        };
    }
}