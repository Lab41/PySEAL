#include "CppUnitTest.h"
#include "seal/util/numth.h"
#include <cstdint>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal::util;
using namespace std;

namespace SEALTest
{
    namespace util
    {
        TEST_CLASS(NumberTheoryTest)
        {
        public:
            TEST_METHOD(GCD)
            {
                Assert::AreEqual(1ULL, gcd(1, 1));
                Assert::AreEqual(1ULL, gcd(2, 1));
                Assert::AreEqual(1ULL, gcd(1, 2));
                Assert::AreEqual(2ULL, gcd(2, 2));
                Assert::AreEqual(3ULL, gcd(6, 15));
                Assert::AreEqual(3ULL, gcd(15, 6));
                Assert::AreEqual(1ULL, gcd(7, 15));
                Assert::AreEqual(1ULL, gcd(15, 7));
                Assert::AreEqual(1ULL, gcd(7, 15));
                Assert::AreEqual(3ULL, gcd(11112, 44445));
            }

            TEST_METHOD(ExtendedGCD)
            {
                tuple<uint64_t, int64_t, int64_t> result;

                // Corner case behavior
                result = xgcd(7, 7);
                Assert::IsTrue(result == make_tuple<>(7, 0, 1));
                result = xgcd(2, 2);
                Assert::IsTrue(result == make_tuple<>(2, 0, 1));

                result = xgcd(1, 1);
                Assert::IsTrue(result == make_tuple<>(1, 0, 1));
                result = xgcd(1, 2);
                Assert::IsTrue(result == make_tuple<>(1, 1, 0));
                result = xgcd(5, 6);
                Assert::IsTrue(result == make_tuple<>(1, -1, 1));
                result = xgcd(13, 19);
                Assert::IsTrue(result == make_tuple<>(1, 3, -2));
                result = xgcd(14, 21);
                Assert::IsTrue(result == make_tuple<>(7, -1, 1));

                result = xgcd(2, 1);
                Assert::IsTrue(result == make_tuple<>(1, 0, 1));
                result = xgcd(6, 5);
                Assert::IsTrue(result == make_tuple<>(1, 1, -1));
                result = xgcd(19, 13);
                Assert::IsTrue(result == make_tuple<>(1, -2, 3));
                result = xgcd(21, 14);
                Assert::IsTrue(result == make_tuple<>(7, 1, -1));
            }

            TEST_METHOD(TryModInverse)
            {
                uint64_t input, modulus, result;

                input = 1, modulus = 2;
                Assert::IsTrue(try_mod_inverse(input, modulus, result));
                Assert::AreEqual(result, 1ULL);

                input = 2, modulus = 2;
                Assert::IsFalse(try_mod_inverse(input, modulus, result));
                
                input = 3, modulus = 2;
                Assert::IsTrue(try_mod_inverse(input, modulus, result));
                Assert::AreEqual(result, 1ULL);

                input = 0xFFFFFF, modulus = 2;
                Assert::IsTrue(try_mod_inverse(input, modulus, result));
                Assert::AreEqual(result, 1ULL);

                input = 0xFFFFFE, modulus = 2;
                Assert::IsFalse(try_mod_inverse(input, modulus, result));

                input = 12345, modulus = 3;
                Assert::IsFalse(try_mod_inverse(input, modulus, result));

                input = 5, modulus = 19;
                Assert::IsTrue(try_mod_inverse(input, modulus, result));
                Assert::AreEqual(result, 4ULL);

                input = 4, modulus = 19;
                Assert::IsTrue(try_mod_inverse(input, modulus, result));
                Assert::AreEqual(result, 5ULL);
            }
        };
    }
}