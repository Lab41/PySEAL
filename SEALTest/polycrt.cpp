#include "CppUnitTest.h"
#include "encryptionparams.h"
#include "bigpoly.h"
#include "bigpolyarith.h"
#include "polycrt.h"
#include <cstdint>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace seal::util;
using namespace std;


namespace SEALTest
{
    TEST_CLASS(PolyCRTBuilderTEST)
    {
    public:
        TEST_METHOD(PolyCRTZBuilderEncodeDecodeTEST)
        {
            EncryptionParameters parms;
            parms.poly_modulus() = "1x^4096 + 1";
            parms.coeff_modulus() = "FFFFFFFFFFF"; 
            parms.plain_modulus() = 40961;
            PolyCRTBuilder crtbuilder(parms); 
            uint64_t slot_count = crtbuilder.get_slot_count();
            Assert::AreEqual(slot_count, static_cast<uint64_t>(4096)); 
            vector<BigUInt> values(slot_count, BigUInt(parms.plain_modulus().bit_count(), static_cast<uint64_t>(0)));


            values[2] = 2;
            values[3] = 3;
            values[5] = 5;
            values[7] = 7;
 

            BigPoly plain_coeff_poly = crtbuilder.compose(values);
            vector<BigUInt> values_dec = crtbuilder.decompose(plain_coeff_poly);
            for (int i = 0; i < 4096; i++) {
                if (i == 2 || i == 3 || i == 5 || i == 7)
                {
                    Assert::AreEqual(values_dec[i][0], static_cast<uint8_t>(i));
                }
                else {
                    Assert::AreEqual(values_dec[i][0], static_cast<uint8_t>(0));
                }
            }
        }

        TEST_METHOD(PolyCRTZBuilderAddTEST)
        {
            EncryptionParameters parms;
            parms.poly_modulus() = "1x^16 + 1";
            parms.coeff_modulus() = "FFFFFFFFFFF";
            parms.plain_modulus() = 97;
            PolyCRTBuilder crtbuilder(parms);
            uint64_t slot_count = crtbuilder.get_slot_count();
            vector<BigUInt> value_vec1(slot_count, BigUInt(parms.plain_modulus().bit_count(), static_cast<uint64_t>(0)));

            value_vec1[0] = 1;
            value_vec1[3] = 3;
            value_vec1[5] = 4;
            value_vec1[15] = 9;

            vector<BigUInt> value_vec2(slot_count, BigUInt(parms.plain_modulus().bit_count(), static_cast<uint64_t>(0)));

            value_vec2[0] = 2;
            value_vec2[3] = 5;
            value_vec2[5] = 7;
            value_vec2[15] = 90;


            BigPoly plain_coeff_poly1 = crtbuilder.compose(value_vec1);
            BigPoly plain_coeff_poly2 = crtbuilder.compose(value_vec2);
            BigPolyArith arith;
            BigPoly plain_coeff_poly_sum = arith.add(plain_coeff_poly1, plain_coeff_poly2, BigUInt(7, "61"));

            for (int i = 0; i < 16; i++) {
                if (i == 0)
                {
                    Assert::AreEqual(crtbuilder.get_slot(plain_coeff_poly_sum, i)[0], static_cast<uint8_t>(3));
                }
                else if ( i == 3) {
                    Assert::AreEqual(crtbuilder.get_slot(plain_coeff_poly_sum, i)[0], static_cast<uint8_t>(8));
                }
                else if ( i == 5)
                {
                    Assert::AreEqual(crtbuilder.get_slot(plain_coeff_poly_sum, i)[0], static_cast<uint8_t>(11));
                }
                else if (i == 15)
                {
                    Assert::AreEqual(crtbuilder.get_slot(plain_coeff_poly_sum, i)[0], static_cast<uint8_t>(2));
                }
                else
                {
                    Assert::AreEqual(crtbuilder.get_slot(plain_coeff_poly_sum, i)[0], static_cast<uint8_t>(0));
                }
            }
        }

        TEST_METHOD(PolyCRTZBuilderMultiplyTEST)
        {
            EncryptionParameters parms;
            parms.poly_modulus() = "1x^1024 + 1";
            parms.coeff_modulus() = "FFFFFFFFFFF";
            parms.plain_modulus() = 12289;
            PolyCRTBuilder crtbuilder(parms);
            uint64_t slot_count = crtbuilder.get_slot_count();
            vector<BigUInt> value_vec1(slot_count, BigUInt(parms.plain_modulus().bit_count(), static_cast<uint64_t>(0)));

            value_vec1[0] = 0;
            value_vec1[1] = 1;
            value_vec1[2] = 2;
            value_vec1[3] = 3;

            vector<BigUInt> value_vec2(slot_count, BigUInt(parms.plain_modulus().bit_count(), static_cast<uint64_t>(0)));

            value_vec2[0] = 5000;
            value_vec2[1] = 6000;
            value_vec2[2] = 7000;
            value_vec2[3] = 8000;


            BigPoly plain_coeff_poly1 = crtbuilder.compose(value_vec1);
            BigPoly plain_coeff_poly2 = crtbuilder.compose(value_vec2);
            
            BigPoly polymod = "1x^1024 + 1";
            
            BigPolyArith arith;


            BigPoly plain_coeff_poly_mult = arith.multiply(plain_coeff_poly1, plain_coeff_poly2, polymod, BigUInt(14,static_cast<uint64_t>(12289)));

            for (int i = 0; i < 1024; i++) {
                if (i == 0)
                {
                    Assert::IsTrue(crtbuilder.get_slot(plain_coeff_poly_mult, i).to_dec_string() == "0");
                }
                else if (i == 1) {
                    Assert::IsTrue(crtbuilder.get_slot(plain_coeff_poly_mult, i).to_dec_string() == "6000"); 
                }
                else if (i == 2)
                {
                    Assert::IsTrue(crtbuilder.get_slot(plain_coeff_poly_mult, i).to_dec_string() == "1711");
                }
                else if (i == 3)
                {
                    Assert::IsTrue(crtbuilder.get_slot(plain_coeff_poly_mult, i).to_dec_string() == "11711");
                }
                else
                {
                    Assert::IsTrue(crtbuilder.get_slot(plain_coeff_poly_mult, i).to_dec_string() == "0");
                }
            }
        }



    };
}