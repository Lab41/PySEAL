#include "CppUnitTest.h"
#include "seal/bigpolyarray.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(BigPolyArr)
    {
    public:
        TEST_METHOD(BigPolyArrayTest)
        {
            BigPolyArray arr(0, 0, 0);
            Assert::AreEqual(arr.size(), 0);
            Assert::AreEqual(arr.coeff_count(), 0);
            Assert::AreEqual(arr.coeff_bit_count(), 0);
            Assert::AreEqual(arr.coeff_uint64_count(), 0);
            Assert::IsTrue(arr.is_zero());

            arr.reset();
            Assert::AreEqual(arr.size(), 0);
            Assert::AreEqual(arr.coeff_count(), 0);
            Assert::AreEqual(arr.coeff_bit_count(), 0);
            Assert::AreEqual(arr.coeff_uint64_count(), 0);
            Assert::IsTrue(arr.is_zero());

            arr.resize(2, 5, 10);
            Assert::AreEqual(arr.size(), 2);
            Assert::AreEqual(arr.coeff_count(), 5);
            Assert::AreEqual(arr.coeff_bit_count(), 10);
            Assert::AreEqual(arr.coeff_uint64_count(), 1);
            Assert::IsTrue(arr.is_zero());

            arr.resize(3, 13, 70);
            Assert::AreEqual(arr.size(), 3);
            Assert::AreEqual(arr.coeff_count(), 13);
            Assert::AreEqual(arr.coeff_bit_count(), 70);
            Assert::AreEqual(arr.coeff_uint64_count(), 2);
            Assert::IsTrue(arr.is_zero());

            arr.pointer(0)[0] = 1;
            arr.pointer(0)[1] = 0;
            arr.pointer(0)[2] = 2;
            arr.pointer(0)[3] = 0;
            arr.pointer(0)[4] = 3;
            arr.pointer(0)[5] = 0;
            arr.pointer(0)[6] = 4;
            arr.pointer(0)[7] = 0;
            arr.pointer(0)[8] = 5;
            arr.pointer(1)[0] = 6;
            arr.pointer(1)[1] = 0;
            arr.pointer(1)[2] = 7;
            arr.pointer(1)[3] = 0;
            arr.pointer(1)[4] = 8;
            arr.pointer(2)[0] = 9;
            arr.pointer(2)[1] = 0;
            arr.pointer(2)[2] = 10;

            Assert::IsTrue("5x^4 + 4x^3 + 3x^2 + 2x^1 + 1" == BigPoly(arr.coeff_count(), arr.coeff_bit_count(), arr.pointer(0)).to_string());
            Assert::IsTrue("8x^2 + 7x^1 + 6" == BigPoly(arr.coeff_count(), arr.coeff_bit_count(), arr.pointer(1)).to_string());
            Assert::IsTrue("Ax^1 + 9" == BigPoly(arr.coeff_count(), arr.coeff_bit_count(), arr.pointer(2)).to_string());
            Assert::IsFalse(arr.is_zero());

            BigPolyArray arr2;
            Assert::AreEqual(arr2.size(), 0);
            Assert::AreEqual(arr2.coeff_count(), 0);
            Assert::AreEqual(arr2.coeff_bit_count(), 0);
            Assert::AreEqual(arr2.coeff_uint64_count(), 0);
            Assert::IsTrue(arr2.is_zero());

            arr2 = arr;
            Assert::AreEqual(arr2.size(), 3);
            Assert::AreEqual(arr2.coeff_count(), 13);
            Assert::AreEqual(arr2.coeff_bit_count(), 70);
            Assert::AreEqual(arr2.coeff_uint64_count(), 2);
            Assert::IsFalse(arr2.is_zero());
            Assert::IsTrue(arr == arr2);

            Assert::IsTrue("5x^4 + 4x^3 + 3x^2 + 2x^1 + 1" == BigPoly(arr2.coeff_count(), arr2.coeff_bit_count(), arr2.pointer(0)).to_string());
            Assert::IsTrue("8x^2 + 7x^1 + 6" == BigPoly(arr2.coeff_count(), arr2.coeff_bit_count(), arr2.pointer(1)).to_string());
            Assert::IsTrue("Ax^1 + 9" == BigPoly(arr2.coeff_count(), arr2.coeff_bit_count(), arr2.pointer(2)).to_string());

            BigPoly(arr2.coeff_count(), arr2.coeff_bit_count(), arr2.pointer(1)).set_zero();
            Assert::IsTrue("5x^4 + 4x^3 + 3x^2 + 2x^1 + 1" == BigPoly(arr2.coeff_count(), arr2.coeff_bit_count(), arr2.pointer(0)).to_string());
            Assert::IsTrue("Ax^1 + 9" == BigPoly(arr2.coeff_count(), arr2.coeff_bit_count(), arr2.pointer(2)).to_string());

            arr.resize(2, 3, 10);
            Assert::AreEqual(arr.size(), 2);
            Assert::AreEqual(arr.coeff_count(), 3);
            Assert::AreEqual(arr.coeff_bit_count(), 10);
            Assert::AreEqual(arr.coeff_uint64_count(), 1);
            Assert::IsFalse(arr.is_zero());
            Assert::IsTrue(arr != arr2);

            Assert::IsTrue("3x^2 + 2x^1 + 1" == BigPoly(arr.coeff_count(), arr.coeff_bit_count(), arr.pointer(0)).to_string());
            Assert::IsTrue("8x^2 + 7x^1 + 6" == BigPoly(arr.coeff_count(), arr.coeff_bit_count(), arr.pointer(1)).to_string());

            arr.resize(1,1,10);
            Assert::AreEqual(arr.size(), 1);
            Assert::AreEqual(arr.coeff_count(), 1);
            Assert::AreEqual(arr.coeff_bit_count(), 10);
            Assert::AreEqual(arr.coeff_uint64_count(), 1);
            Assert::IsFalse(arr.is_zero());

            Assert::IsTrue("1" == BigPoly(arr.coeff_count(), arr.coeff_bit_count(), arr.pointer(0)).to_string());

            arr.reset();
            Assert::AreEqual(arr.size(), 0);
            Assert::AreEqual(arr.coeff_count(), 0);
            Assert::AreEqual(arr.coeff_bit_count(), 0);
            Assert::AreEqual(arr.coeff_uint64_count(), 0);
            Assert::IsTrue(arr.is_zero());
        }

        TEST_METHOD(SaveLoadBigPolyArray)
        {
            stringstream stream;

            BigPolyArray arr(3, 5, 10);
            arr.pointer(0)[0] = 1;
            arr.pointer(0)[1] = 2;
            arr.pointer(0)[2] = 3;
            arr.pointer(1)[0] = 4;
            arr.pointer(1)[1] = 5;
            arr.pointer(2)[0] = 6;

            BigPolyArray arr2;
            arr.save(stream);
            arr2.load(stream);

            Assert::AreEqual(3, arr2.size());
            Assert::AreEqual(arr2.size(), 3);
            Assert::AreEqual(arr2.coeff_count(), 5);
            Assert::AreEqual(arr2.coeff_bit_count(), 10);
            Assert::AreEqual(arr2.coeff_uint64_count(), 1);

            Assert::IsTrue(arr == arr2);
        }
    };
}
