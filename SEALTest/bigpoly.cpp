#include "CppUnitTest.h"
#include "bigpoly.h"
#include <cstdint>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(BigPolynomial)
    {
    public:
        TEST_METHOD(EmptyBigPoly)
        {
            BigPoly poly;
            Assert::AreEqual(0, poly.coeff_count());
            Assert::AreEqual(0, poly.coeff_bit_count());
            Assert::IsTrue(nullptr == poly.pointer());
            Assert::AreEqual(0, poly.uint64_count());
            Assert::AreEqual(0, poly.significant_coeff_count());
            Assert::IsTrue("0" == poly.to_string());
            Assert::IsTrue(poly.is_zero());
            Assert::IsFalse(poly.is_alias());
            poly.set_zero();

            BigPoly poly2;
            Assert::IsTrue(poly == poly2);
            Assert::IsFalse(poly != poly2);

            poly.resize(1, 1);
            Assert::IsTrue(nullptr != poly.pointer());
            Assert::IsFalse(poly.is_alias());

            poly.resize(0, 0);
            Assert::IsTrue(nullptr == poly.pointer());
            Assert::IsFalse(poly.is_alias());
        }

        TEST_METHOD(BigPoly3Coeff100Bits)
        {
            BigPoly poly(3, 100);
            Assert::AreEqual(3, poly.coeff_count());
            Assert::AreEqual(100, poly.coeff_bit_count());
            Assert::IsTrue(nullptr != poly.pointer());
            Assert::AreEqual(6, poly.uint64_count());
            Assert::AreEqual(0, poly.significant_coeff_count());
            Assert::IsTrue("0" == poly.to_string());
            Assert::IsTrue(poly.is_zero());
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[0]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[1]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[2]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[3]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[4]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[5]);

            poly[0] = "1234";
            Assert::IsTrue("1234" == poly[0].to_string());
            Assert::AreEqual(1, poly.significant_coeff_count());
            Assert::IsTrue("1234" == poly.to_string());
            Assert::IsFalse(poly.is_zero());
            Assert::AreEqual(static_cast<uint64_t>(0x1234), poly.pointer()[0]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[1]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[2]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[3]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[4]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[5]);

            poly[2] = "3211234567812345678";
            Assert::IsTrue("3211234567812345678" == poly[2].to_string());
            Assert::AreEqual(3, poly.significant_coeff_count());
            Assert::IsTrue("3211234567812345678x^2 + 1234" == poly.to_string());
            Assert::IsFalse(poly.is_zero());
            Assert::AreEqual(static_cast<uint64_t>(0x1234), poly.pointer()[0]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[1]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[2]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[3]);
            Assert::AreEqual(static_cast<uint64_t>(0x1234567812345678), poly.pointer()[4]);
            Assert::AreEqual(static_cast<uint64_t>(0x321), poly.pointer()[5]);

            poly.set_zero();
            Assert::IsTrue(poly.is_zero());
            Assert::IsTrue("0" == poly.to_string());
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[0]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[1]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[2]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[3]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[4]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[5]);

            poly[0] = "123";
            poly[1] = "456";
            poly[2] = "3211234567812345678";
            Assert::IsTrue("3211234567812345678x^2 + 456x^1 + 123" == poly.to_string());
            Assert::AreEqual(3, poly.significant_coeff_count());
            Assert::AreEqual(static_cast<uint64_t>(0x123), poly.pointer()[0]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[1]);
            Assert::AreEqual(static_cast<uint64_t>(0x456), poly.pointer()[2]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[3]);
            Assert::AreEqual(static_cast<uint64_t>(0x1234567812345678), poly.pointer()[4]);
            Assert::AreEqual(static_cast<uint64_t>(0x321), poly.pointer()[5]);

            BigPoly poly2(3, 64);
            poly2[0] = "1";
            poly2[1] = "2";
            poly2[2] = "3";
            Assert::IsFalse(poly == poly2);
            Assert::IsFalse(poly2 == poly);
            Assert::IsTrue(poly != poly2);
            Assert::IsTrue(poly2 != poly);
            poly = poly2;
            Assert::IsTrue(poly == poly2);
            Assert::IsTrue(poly2 == poly);
            Assert::IsFalse(poly != poly2);
            Assert::IsFalse(poly2 != poly);
            Assert::IsTrue("3x^2 + 2x^1 + 1" == poly.to_string());
            Assert::AreEqual(static_cast<uint64_t>(1), poly.pointer()[0]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[1]);
            Assert::AreEqual(static_cast<uint64_t>(2), poly.pointer()[2]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[3]);
            Assert::AreEqual(static_cast<uint64_t>(3), poly.pointer()[4]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[5]);

            poly[0] = "123";
            poly[1] = "456";
            poly[2] = "3211234567812345678";
            poly.resize(3, 32);
            Assert::AreEqual(3, poly.coeff_count());
            Assert::AreEqual(32, poly.coeff_bit_count());
            Assert::AreEqual(3, poly.uint64_count());
            Assert::IsTrue("12345678x^2 + 456x^1 + 123" == poly.to_string());
            Assert::AreEqual(static_cast<uint64_t>(0x123), poly.pointer()[0]);
            Assert::AreEqual(static_cast<uint64_t>(0x456), poly.pointer()[1]);
            Assert::AreEqual(static_cast<uint64_t>(0x12345678), poly.pointer()[2]);

            poly.resize(4, 100);
            Assert::AreEqual(4, poly.coeff_count());
            Assert::AreEqual(100, poly.coeff_bit_count());
            Assert::AreEqual(8, poly.uint64_count());
            Assert::IsTrue("12345678x^2 + 456x^1 + 123" == poly.to_string());
            Assert::AreEqual(static_cast<uint64_t>(0x123), poly.pointer()[0]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[1]);
            Assert::AreEqual(static_cast<uint64_t>(0x456), poly.pointer()[2]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[3]);
            Assert::AreEqual(static_cast<uint64_t>(0x12345678), poly.pointer()[4]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[5]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[6]);
            Assert::AreEqual(static_cast<uint64_t>(0), poly.pointer()[7]);

            poly.resize(0, 0);
            Assert::AreEqual(0, poly.coeff_count());
            Assert::AreEqual(0, poly.coeff_bit_count());
            Assert::IsTrue(nullptr == poly.pointer());
            Assert::AreEqual(0, poly.uint64_count());
            Assert::AreEqual(0, poly.significant_coeff_count());
            Assert::IsTrue("0" == poly.to_string());
            Assert::IsTrue(poly.is_zero());
            Assert::IsFalse(poly.is_alias());
        }

        TEST_METHOD(BigPolyFromString)
        {
            BigPoly poly("9x^2 + 2");
            Assert::IsTrue("9x^2 + 2" == poly.to_string());
            Assert::AreEqual(3, poly.coeff_count());
            Assert::AreEqual(4, poly.coeff_bit_count());
            
            poly.resize(0, 0);
            poly = "0";
            Assert::IsTrue("0" == poly.to_string());
            Assert::AreEqual(0, poly.coeff_count());
            Assert::AreEqual(0, poly.coeff_bit_count());

            poly = "1";
            Assert::IsTrue("1" == poly.to_string());
            Assert::AreEqual(1, poly.coeff_count());
            Assert::AreEqual(1, poly.coeff_bit_count());

            poly = "123";
            Assert::IsTrue("123" == poly.to_string());
            Assert::AreEqual(1, poly.coeff_count());
            Assert::AreEqual(9, poly.coeff_bit_count());

            poly = "23x^3";
            Assert::IsTrue("23x^3" == poly.to_string());
            Assert::AreEqual(4, poly.coeff_count());
            Assert::AreEqual(9, poly.coeff_bit_count());

            poly.resize(0, 0);
            poly = "23x^3";
            Assert::IsTrue("23x^3" == poly.to_string());
            Assert::AreEqual(4, poly.coeff_count());
            Assert::AreEqual(6, poly.coeff_bit_count());

            poly = "3x^2 + 2x^1";
            Assert::IsTrue("3x^2 + 2x^1" == poly.to_string());
            Assert::AreEqual(4, poly.coeff_count());
            Assert::AreEqual(6, poly.coeff_bit_count());

            poly = "1x^2 + 2x^1 + 3";
            Assert::IsTrue("1x^2 + 2x^1 + 3" == poly.to_string());
            Assert::AreEqual(4, poly.coeff_count());
            Assert::AreEqual(6, poly.coeff_bit_count());

            poly = "4x^1";
            Assert::IsTrue("4x^1" == poly.to_string());
            Assert::AreEqual(4, poly.coeff_count());
            Assert::AreEqual(6, poly.coeff_bit_count());
        }

        TEST_METHOD(SaveLoadPoly)
        {
            stringstream stream;

            BigPoly poly;
            BigPoly poly2(1, 32);
            poly2[0] = 1;
            poly.save(stream);
            poly2.load(stream);
            Assert::IsTrue(poly == poly2);

            poly.resize(3, 96);
            poly[0] = 2;
            poly[1] = 3;
            poly[2] = 4;
            poly.save(stream);
            poly2.load(stream);
            Assert::IsTrue(poly == poly2);

            poly[0] = "FFFFFFFFFFFFFFFFFF";
            poly.save(stream);
            poly2.load(stream);
            Assert::IsTrue(poly == poly2);

            poly.resize(2, 32);
            poly[0] = 1;
            poly.save(stream);
            poly2.load(stream);
            Assert::IsTrue(poly == poly2);
        }

        TEST_METHOD(DuplicateTo)
        {
            BigPoly original(123, 456);
            *(original[0].pointer()) = 1;
            *(original[1].pointer()) = 2;
            *(original[2].pointer()) = 3;
            *(original[3].pointer()) = 4;
            *(original[4].pointer()) = 5;
            *(original[122].pointer()) = 123;

            BigPoly target;
            
            original.duplicate_to(target);
            Assert::AreEqual(target.coeff_count(), original.coeff_count());
            Assert::AreEqual(target.coeff_bit_count(), original.coeff_bit_count());
            Assert::IsTrue(target == original);
        }

        TEST_METHOD(DuplicateFrom)
        {
            BigPoly original(123, 456);
            *(original[0].pointer()) = 1;
            *(original[1].pointer()) = 2;
            *(original[2].pointer()) = 3;
            *(original[3].pointer()) = 4;
            *(original[4].pointer()) = 5;
            *(original[122].pointer()) = 123;

            BigPoly target;

            target.duplicate_from(original);
            Assert::AreEqual(target.coeff_count(), original.coeff_count());
            Assert::AreEqual(target.coeff_bit_count(), original.coeff_bit_count());
            Assert::IsTrue(target == original);
        }
    };
}