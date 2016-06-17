#include "CppUnitTest.h"
#include "bigpolyarray.h"

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
            BigPolyArray arr;
            Assert::AreEqual(arr.size(), 0);
            Assert::AreEqual(arr.coeff_count(), 0);
            Assert::AreEqual(arr.coeff_bit_count(), 0);
            Assert::AreEqual(arr.coeff_uint64_count(), 0);

            arr.reset();
            Assert::AreEqual(arr.size(), 0);
            Assert::AreEqual(arr.coeff_count(), 0);
            Assert::AreEqual(arr.coeff_bit_count(), 0);
            Assert::AreEqual(arr.coeff_uint64_count(), 0);

            arr.resize(2, 5, 10);
            Assert::AreEqual(arr.size(), 2);
            Assert::AreEqual(arr.coeff_count(), 5);
            Assert::AreEqual(arr.coeff_bit_count(), 10);
            Assert::AreEqual(arr.coeff_uint64_count(), 1);

            arr.resize(3, 13, 70);
            Assert::AreEqual(arr.size(), 3);
            Assert::AreEqual(arr.coeff_count(), 13);
            Assert::AreEqual(arr.coeff_bit_count(), 70);
            Assert::AreEqual(arr.coeff_uint64_count(), 2);

            arr[0][0] = 1;
            arr[0][1] = 2;
            arr[0][2] = 3;
            arr[0][3] = 4;
            arr[0][4] = 5;
            arr[1][0] = 6;
            arr[1][1] = 7;
            arr[1][2] = 8;
            arr[2][0] = 9;
            arr[2][1] = 10;

            Assert::IsTrue("5x^4 + 4x^3 + 3x^2 + 2x^1 + 1" == arr[0].to_string());
            Assert::IsTrue("8x^2 + 7x^1 + 6" == arr[1].to_string());
            Assert::IsTrue("Ax^1 + 9" == arr[2].to_string());

            BigPolyArray arr2;
            Assert::AreEqual(arr2.size(), 0);
            Assert::AreEqual(arr2.coeff_count(), 0);
            Assert::AreEqual(arr2.coeff_bit_count(), 0);
            Assert::AreEqual(arr2.coeff_uint64_count(), 0);

            arr2 = arr;
            Assert::AreEqual(arr.size(), 3);
            Assert::AreEqual(arr.coeff_count(), 13);
            Assert::AreEqual(arr.coeff_bit_count(), 70);

            Assert::IsTrue("5x^4 + 4x^3 + 3x^2 + 2x^1 + 1" == arr2[0].to_string());
            Assert::IsTrue("8x^2 + 7x^1 + 6" == arr2[1].to_string());
            Assert::IsTrue("Ax^1 + 9" == arr2[2].to_string());

            arr2[1].set_zero();
            Assert::IsTrue("5x^4 + 4x^3 + 3x^2 + 2x^1 + 1" == arr2[0].to_string());
            Assert::IsTrue("Ax^1 + 9" == arr2[2].to_string());

            arr.resize(2, 3, 10);
            Assert::AreEqual(arr.size(), 2);
            Assert::AreEqual(arr.coeff_count(), 3);
            Assert::AreEqual(arr.coeff_bit_count(), 10);

            Assert::IsTrue("3x^2 + 2x^1 + 1" == arr[0].to_string());
            Assert::IsTrue("8x^2 + 7x^1 + 6" == arr[1].to_string());

            arr.resize(1,1,10);
            Assert::AreEqual(arr.size(), 1);
            Assert::AreEqual(arr.coeff_count(), 1);
            Assert::AreEqual(arr.coeff_bit_count(), 10);

            Assert::IsTrue("1" == arr[0].to_string());

            arr.reset();
            Assert::AreEqual(arr.size(), 0);
            Assert::AreEqual(arr.coeff_count(), 0);
            Assert::AreEqual(arr.coeff_bit_count(), 0);
        }

        TEST_METHOD(SaveLoadBigPolyArray)
        {
            stringstream stream;

            BigPolyArray arr(3, 5, 10);
            arr[0][0] = 1;
            arr[0][1] = 2;
            arr[0][2] = 3;
            arr[1][0] = 4;
            arr[1][1] = 5;
            arr[2][0] = 6;

            BigPolyArray arr2;
            arr.save(stream);
            arr2.load(stream);

            Assert::AreEqual(3, arr2.size());
            Assert::AreEqual(arr2.size(), 3);
            Assert::AreEqual(arr2.coeff_count(), 5);
            Assert::AreEqual(arr2.coeff_bit_count(), 10);

            Assert::IsTrue(arr[0] == arr2[0]);
            Assert::IsTrue(arr[1] == arr2[1]);
            Assert::IsTrue(arr[2] == arr2[2]);
        }
    };
}
