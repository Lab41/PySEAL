#include "CppUnitTest.h"
#include "seal/util/uintcore.h"
#include "seal/util/uintarithsmallmod.h"
#include "seal/smallmodulus.h"
#include "seal/memorypoolhandle.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal::util;
using namespace seal;
using namespace std;

namespace SEALTest
{
    namespace util
    {
        TEST_CLASS(UIntArithSmallMod)
        {
        public:
            TEST_METHOD(IncrementUIntSmallMod)
            {
                SmallModulus mod(2);
                Assert::AreEqual(1ULL, increment_uint_mod(0, mod));
                Assert::AreEqual(0ULL, increment_uint_mod(1ULL, mod));

                mod = 0x10000;
                Assert::AreEqual(1ULL, increment_uint_mod(0, mod));
                Assert::AreEqual(2ULL, increment_uint_mod(1ULL, mod));
                Assert::AreEqual(0ULL, increment_uint_mod(0xFFFFULL, mod));

                mod = 4611686018427289601ULL;
                Assert::AreEqual(1ULL, increment_uint_mod(0, mod));
                Assert::AreEqual(0ULL, increment_uint_mod(4611686018427289600ULL, mod));                
                Assert::AreEqual(1ULL, increment_uint_mod(0, mod));
            }

            TEST_METHOD(DecrementUIntSmallMod)
            {
                SmallModulus mod(2);
                Assert::AreEqual(0ULL, decrement_uint_mod(1, mod));
                Assert::AreEqual(1ULL, decrement_uint_mod(0ULL, mod));

                mod = 0x10000;
                Assert::AreEqual(0ULL, decrement_uint_mod(1, mod));
                Assert::AreEqual(1ULL, decrement_uint_mod(2ULL, mod));
                Assert::AreEqual(0xFFFFULL, decrement_uint_mod(0ULL, mod));

                mod = 4611686018427289601ULL;
                Assert::AreEqual(0ULL, decrement_uint_mod(1, mod));
                Assert::AreEqual(4611686018427289600ULL, decrement_uint_mod(0ULL, mod));
                Assert::AreEqual(0ULL, decrement_uint_mod(1, mod));
            }

            TEST_METHOD(NegateUIntSmallMod)
            {
                SmallModulus mod(2);
                Assert::AreEqual(0ULL, negate_uint_mod(0, mod));
                Assert::AreEqual(1ULL, negate_uint_mod(1, mod));

                mod = 0xFFFFULL;
                Assert::AreEqual(0ULL, negate_uint_mod(0, mod));
                Assert::AreEqual(0xFFFEULL, negate_uint_mod(1, mod));
                Assert::AreEqual(0x1ULL, negate_uint_mod(0xFFFEULL, mod));

                mod = 0x10000ULL;
                Assert::AreEqual(0ULL, negate_uint_mod(0, mod));
                Assert::AreEqual(0xFFFFULL, negate_uint_mod(1, mod));
                Assert::AreEqual(0x1ULL, negate_uint_mod(0xFFFFULL, mod));

                mod = 4611686018427289601ULL;
                Assert::AreEqual(0ULL, negate_uint_mod(0, mod));
                Assert::AreEqual(4611686018427289600ULL, negate_uint_mod(1, mod));
            }

            TEST_METHOD(Div2UIntSmallMod)
            {
                SmallModulus mod(3);
                Assert::AreEqual(0ULL, div2_uint_mod(0ULL, mod));
                Assert::AreEqual(2ULL, div2_uint_mod(1ULL, mod));
                
                mod = 17;
                Assert::AreEqual(11ULL, div2_uint_mod(5ULL, mod));
                Assert::AreEqual(4ULL, div2_uint_mod(8ULL, mod));

                mod = 0xFFFFFFFFFFFFFFFULL;
                Assert::AreEqual(0x800000000000000ULL, div2_uint_mod(1ULL, mod));
                Assert::AreEqual(0x800000000000001ULL, div2_uint_mod(3ULL, mod));
            }

            TEST_METHOD(AddUIntSmallMod)
            {
                SmallModulus mod(2);
                Assert::AreEqual(0ULL, add_uint_uint_mod(0, 0, mod));
                Assert::AreEqual(1ULL, add_uint_uint_mod(0, 1, mod));
                Assert::AreEqual(1ULL, add_uint_uint_mod(1, 0, mod));
                Assert::AreEqual(0ULL, add_uint_uint_mod(1, 1, mod));

                mod = 10;
                Assert::AreEqual(0ULL, add_uint_uint_mod(0, 0, mod));
                Assert::AreEqual(1ULL, add_uint_uint_mod(0, 1, mod));
                Assert::AreEqual(1ULL, add_uint_uint_mod(1, 0, mod));
                Assert::AreEqual(2ULL, add_uint_uint_mod(1, 1, mod));
                Assert::AreEqual(4ULL, add_uint_uint_mod(7, 7, mod));
                Assert::AreEqual(3ULL, add_uint_uint_mod(6, 7, mod));

                mod = 4611686018427289601;
                Assert::AreEqual(0ULL, add_uint_uint_mod(0, 0, mod));
                Assert::AreEqual(1ULL, add_uint_uint_mod(0, 1, mod));
                Assert::AreEqual(1ULL, add_uint_uint_mod(1, 0, mod));
                Assert::AreEqual(2ULL, add_uint_uint_mod(1, 1, mod));
                Assert::AreEqual(0ULL, add_uint_uint_mod(2305843009213644800ULL, 2305843009213644801ULL, mod));
                Assert::AreEqual(1ULL, add_uint_uint_mod(2305843009213644801ULL, 2305843009213644801ULL, mod));
                Assert::AreEqual(4611686018427289599ULL, add_uint_uint_mod(4611686018427289600ULL, 4611686018427289600ULL, mod));
            }

            TEST_METHOD(SubUIntSmallMod)
            {
                SmallModulus mod(2);
                Assert::AreEqual(0ULL, sub_uint_uint_mod(0, 0, mod));
                Assert::AreEqual(1ULL, sub_uint_uint_mod(0, 1, mod));
                Assert::AreEqual(1ULL, sub_uint_uint_mod(1, 0, mod));
                Assert::AreEqual(0ULL, sub_uint_uint_mod(1, 1, mod));

                mod = 10;
                Assert::AreEqual(0ULL, sub_uint_uint_mod(0, 0, mod));
                Assert::AreEqual(9ULL, sub_uint_uint_mod(0, 1, mod));
                Assert::AreEqual(1ULL, sub_uint_uint_mod(1, 0, mod));
                Assert::AreEqual(0ULL, sub_uint_uint_mod(1, 1, mod));
                Assert::AreEqual(0ULL, sub_uint_uint_mod(7, 7, mod));
                Assert::AreEqual(9ULL, sub_uint_uint_mod(6, 7, mod));
                Assert::AreEqual(1ULL, sub_uint_uint_mod(7, 6, mod));

                mod = 4611686018427289601ULL;
                Assert::AreEqual(0ULL, sub_uint_uint_mod(0, 0, mod));
                Assert::AreEqual(4611686018427289600ULL, sub_uint_uint_mod(0, 1, mod));
                Assert::AreEqual(1ULL, sub_uint_uint_mod(1, 0, mod));
                Assert::AreEqual(0ULL, sub_uint_uint_mod(1, 1, mod));
                Assert::AreEqual(4611686018427289600ULL, sub_uint_uint_mod(2305843009213644800ULL, 2305843009213644801ULL, mod));
                Assert::AreEqual(1ULL, sub_uint_uint_mod(2305843009213644801ULL, 2305843009213644800ULL, mod));
                Assert::AreEqual(0ULL, sub_uint_uint_mod(2305843009213644801ULL, 2305843009213644801ULL, mod));
                Assert::AreEqual(0ULL, sub_uint_uint_mod(4611686018427289600ULL, 4611686018427289600ULL, mod));
            }

            TEST_METHOD(BarrettReduce128)
            {
                uint64_t input[2];

                SmallModulus mod(2);
                input[0] = 0;
                input[1] = 0;
                Assert::AreEqual(0ULL, barrett_reduce_128(input, mod));
                input[0] = 1;
                input[1] = 0;
                Assert::AreEqual(1ULL, barrett_reduce_128(input, mod));
                input[0] = 0xFFFFFFFFFFFFFFFFULL;
                input[1] = 0xFFFFFFFFFFFFFFFFULL;
                Assert::AreEqual(1ULL, barrett_reduce_128(input, mod));

                mod = 3;
                input[0] = 0;
                input[1] = 0;
                Assert::AreEqual(0ULL, barrett_reduce_128(input, mod));
                input[0] = 1;
                input[1] = 0;
                Assert::AreEqual(1ULL, barrett_reduce_128(input, mod));
                input[0] = 123;
                input[1] = 456;
                Assert::AreEqual(0ULL, barrett_reduce_128(input, mod));
                input[0] = 0xFFFFFFFFFFFFFFFFULL;
                input[1] = 0xFFFFFFFFFFFFFFFFULL;
                Assert::AreEqual(0ULL, barrett_reduce_128(input, mod));

                mod = 13131313131313ULL;
                input[0] = 0;
                input[1] = 0;
                Assert::AreEqual(0ULL, barrett_reduce_128(input, mod));
                input[0] = 1;
                input[1] = 0;
                Assert::AreEqual(1ULL, barrett_reduce_128(input, mod));
                input[0] = 123;
                input[1] = 456;
                Assert::AreEqual(8722750765283ULL, barrett_reduce_128(input, mod));
                input[0] = 24242424242424;
                input[1] = 79797979797979;
                Assert::AreEqual(1010101010101ULL, barrett_reduce_128(input, mod));
            }

            TEST_METHOD(MultiplyUIntUIntSmallMod)
            {
                SmallModulus mod(2);
                Assert::AreEqual(0ULL, multiply_uint_uint_mod(0, 0, mod));
                Assert::AreEqual(0ULL, multiply_uint_uint_mod(0, 1, mod));
                Assert::AreEqual(0ULL, multiply_uint_uint_mod(1, 0, mod));
                Assert::AreEqual(1ULL, multiply_uint_uint_mod(1, 1, mod));

                mod = 10;
                Assert::AreEqual(0ULL, multiply_uint_uint_mod(0, 0, mod));
                Assert::AreEqual(0ULL, multiply_uint_uint_mod(0, 1, mod));
                Assert::AreEqual(0ULL, multiply_uint_uint_mod(1, 0, mod));
                Assert::AreEqual(1ULL, multiply_uint_uint_mod(1, 1, mod));
                Assert::AreEqual(9ULL, multiply_uint_uint_mod(7, 7, mod));
                Assert::AreEqual(2ULL, multiply_uint_uint_mod(6, 7, mod));
                Assert::AreEqual(2ULL, multiply_uint_uint_mod(7, 6, mod));

                mod = 4611686018427289601ULL;
                Assert::AreEqual(0ULL, multiply_uint_uint_mod(0, 0, mod));
                Assert::AreEqual(0ULL, multiply_uint_uint_mod(0, 1, mod));
                Assert::AreEqual(0ULL, multiply_uint_uint_mod(1, 0, mod));
                Assert::AreEqual(1ULL, multiply_uint_uint_mod(1, 1, mod));
                Assert::AreEqual(1152921504606822400ULL, multiply_uint_uint_mod(2305843009213644800ULL, 2305843009213644801ULL, mod));
                Assert::AreEqual(1152921504606822400ULL, multiply_uint_uint_mod(2305843009213644801ULL, 2305843009213644800ULL, mod));
                Assert::AreEqual(3458764513820467201ULL, multiply_uint_uint_mod(2305843009213644801ULL, 2305843009213644801ULL, mod));
                Assert::AreEqual(1ULL, multiply_uint_uint_mod(4611686018427289600ULL, 4611686018427289600ULL, mod));
            }

            TEST_METHOD(ModuloUIntSmallMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                Pointer value(allocate_uint(4, pool));

                SmallModulus mod(2);
                value[0] = 0;
                value[1] = 0;
                value[2] = 0;
                modulo_uint_inplace(value.get(), 3, mod);
                Assert::AreEqual(0ULL, value[0]);
                Assert::AreEqual(0ULL, value[1]);
                Assert::AreEqual(0ULL, value[2]);

                value[0] = 1;
                value[1] = 0;
                value[2] = 0;
                modulo_uint_inplace(value.get(), 3, mod);
                Assert::AreEqual(1ULL, value[0]);
                Assert::AreEqual(0ULL, value[1]);
                Assert::AreEqual(0ULL, value[2]);

                value[0] = 2;
                value[1] = 0;
                value[2] = 0;
                modulo_uint_inplace(value.get(), 3, mod);
                Assert::AreEqual(0ULL, value[0]);
                Assert::AreEqual(0ULL, value[1]);
                Assert::AreEqual(0ULL, value[2]);

                value[0] = 3;
                value[1] = 0;
                value[2] = 0;
                modulo_uint_inplace(value.get(), 3, mod);
                Assert::AreEqual(1ULL, value[0]);
                Assert::AreEqual(0ULL, value[1]);
                Assert::AreEqual(0ULL, value[2]);

                mod = 0xFFFF;
                value[0] = 9585656442714717620;
                value[1] = 1817697005049051848;
                value[2] = 0;
                modulo_uint_inplace(value.get(), 3, mod);
                Assert::AreEqual(65143ULL, value[0]);
                Assert::AreEqual(0ULL, value[1]);
                Assert::AreEqual(0ULL, value[2]);

                mod = 0x1000;
                value[0] = 9585656442714717620;
                value[1] = 1817697005049051848;
                value[2] = 0;
                modulo_uint_inplace(value.get(), 3, mod);
                Assert::AreEqual(0xDB4ULL, value[0]);
                Assert::AreEqual(0ULL, value[1]);
                Assert::AreEqual(0ULL, value[2]);

                mod = 0xFFFFFFFFC001ULL;
                value[0] = 9585656442714717620;
                value[1] = 1817697005049051848;
                value[2] = 14447416709120365380;
                value[3] = 67450014862939159;
                modulo_uint_inplace(value.get(), 4, mod);
                Assert::AreEqual(124510066632001ULL, value[0]);
                Assert::AreEqual(0ULL, value[1]);
                Assert::AreEqual(0ULL, value[2]);
                Assert::AreEqual(0ULL, value[3]);
            }

            TEST_METHOD(TryInvertUIntSmallMod)
            {
                uint64_t result;
                SmallModulus mod(5);
                Assert::IsFalse(try_invert_uint_mod(0, mod, result));
                Assert::IsTrue(try_invert_uint_mod(1, mod, result));
                Assert::AreEqual(1ULL, result);
                Assert::IsTrue(try_invert_uint_mod(2, mod, result));
                Assert::AreEqual(3ULL, result);
                Assert::IsTrue(try_invert_uint_mod(3, mod, result));
                Assert::AreEqual(2ULL, result);
                Assert::IsTrue(try_invert_uint_mod(4, mod, result));
                Assert::AreEqual(4ULL, result);

                mod = 6;
                Assert::IsFalse(try_invert_uint_mod(2, mod, result));
                Assert::IsFalse(try_invert_uint_mod(3, mod, result));
                Assert::IsTrue(try_invert_uint_mod(5, mod, result));
                Assert::AreEqual(5ULL, result);

                mod = 1351315121;
                Assert::IsTrue(try_invert_uint_mod(331975426, mod, result));
                Assert::AreEqual(1052541512ULL, result);
            }

            TEST_METHOD(TryPrimitiveRootSmallMod)
            {
                uint64_t result;
                SmallModulus mod(11);

                Assert::IsTrue(try_primitive_root(2, mod, result));
                Assert::AreEqual(10ULL, result);

                mod = 29;
                Assert::IsTrue(try_primitive_root(2, mod, result));
                Assert::AreEqual(28ULL, result);

                vector<uint64_t> corrects{ 12, 17 };
                Assert::IsTrue(try_primitive_root(4, mod, result));
                Assert::IsTrue(std::find(corrects.begin(), corrects.end(), result) != corrects.end());

                mod = 1234565441;
                Assert::IsTrue(try_primitive_root(2, mod, result));
                Assert::AreEqual(1234565440ULL, result);
                corrects = { 984839708, 273658408, 249725733, 960907033 };
                Assert::IsTrue(try_primitive_root(8, mod, result));
                Assert::IsTrue(std::find(corrects.begin(), corrects.end(), result) != corrects.end());
            }

            TEST_METHOD(IsPrimitiveRootSmallMod)
            {
                MemoryPool &pool = *global_variables::global_memory_pool;
                SmallModulus mod(11);
                Assert::IsTrue(is_primitive_root(10, 2, mod));
                Assert::IsFalse(is_primitive_root(9, 2, mod));
                Assert::IsFalse(is_primitive_root(10, 4, mod));

                mod = 29;
                Assert::IsTrue(is_primitive_root(28, 2, mod));
                Assert::IsTrue(is_primitive_root(12, 4, mod));
                Assert::IsFalse(is_primitive_root(12, 2, mod));
                Assert::IsFalse(is_primitive_root(12, 8, mod));


                mod = 1234565441ULL;
                Assert::IsTrue(is_primitive_root(1234565440ULL, 2, mod));
                Assert::IsTrue(is_primitive_root(960907033ULL, 8, mod));
                Assert::IsTrue(is_primitive_root(1180581915ULL, 16, mod));
                Assert::IsFalse(is_primitive_root(1180581915ULL, 32, mod));
                Assert::IsFalse(is_primitive_root(1180581915ULL, 8, mod));
                Assert::IsFalse(is_primitive_root(1180581915ULL, 2, mod));
            }

            TEST_METHOD(TryMinimalPrimitiveRootSmallMod)
            {
                SmallModulus mod(11);

                uint64_t result;
                Assert::IsTrue(try_minimal_primitive_root(2, mod, result));
                Assert::AreEqual(10ULL, result);

                mod = 29;
                Assert::IsTrue(try_minimal_primitive_root(2, mod, result));
                Assert::AreEqual(28ULL, result);
                Assert::IsTrue(try_minimal_primitive_root(4, mod, result));
                Assert::AreEqual(12ULL, result);

                mod = 1234565441;
                Assert::IsTrue(try_minimal_primitive_root(2, mod, result));
                Assert::AreEqual(1234565440ULL, result);
                Assert::IsTrue(try_minimal_primitive_root(8, mod, result));
                Assert::AreEqual(249725733ULL, result);
            }

            TEST_METHOD(ExponentiateUIntSmallMod)
            {
                SmallModulus mod(5);
                Assert::AreEqual(1ULL, exponentiate_uint_mod(1, 0, mod));
                Assert::AreEqual(1ULL, exponentiate_uint_mod(1, 0xFFFFFFFFFFFFFFFFULL, mod));
                Assert::AreEqual(3ULL, exponentiate_uint_mod(2, 0xFFFFFFFFFFFFFFFFULL, mod));

                mod = 0x1000000000000000ULL;
                Assert::AreEqual(0ULL, exponentiate_uint_mod(2, 60, mod));
                Assert::AreEqual(0x800000000000000ULL, exponentiate_uint_mod(2, 59, mod));

                mod = 131313131313;
                Assert::AreEqual(39418477653ULL, exponentiate_uint_mod(2424242424, 16, mod));
            }
        };
    }
}