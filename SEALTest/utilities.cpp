#include "CppUnitTest.h"
#include "util/polyextras.h"
#include "bigpoly.h"
#include "utilities.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal::util;
using namespace seal;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(Utilities)
    {
    public:
        TEST_METHOD(PolyInftyNorm)
        {
            BigPoly poly("1x^10 + 2x^9 + 5x^8 + Ax^7 + Bx^6 + 4x^5 + 1x^2 + 1");
            BigUInt inftynorm = poly_infty_norm(poly);
            Assert::IsTrue(inftynorm == BigUInt("B"));

            poly = BigPoly("AAx^10 + ABx^9 + CAx^8 + CFx^7 + FEx^6 + F7x^5 + 1x^2 + 2");
            inftynorm = poly_infty_norm(poly);
            Assert::IsTrue(inftynorm == BigUInt("FE"));

            poly = BigPoly("Ax^10 + ABx^9 + ABCx^8 + ABCDx^7 + ABCDEx^6 + ABCDEFx^5 + 1x^2 + 2");
            inftynorm = poly_infty_norm(poly);
            Assert::IsTrue(inftynorm == BigUInt("ABCDEF"));

            poly = BigPoly("1x^5 + 2x^4 + 3x^3 + 4x^2 + 5x^1 + 6");
            inftynorm = poly_infty_norm(poly);
            Assert::IsTrue(inftynorm == BigUInt("6"));
        }

        TEST_METHOD(PolyInftyNormCoeffmod)
        {
            BigPoly poly("1x^10 + 2x^9 + 5x^8 + Ax^7 + Bx^6 + 4x^5 + 1x^2 + 1");
            BigUInt modulus(poly.coeff_bit_count());
            modulus = 5;
            BigUInt inftynorm = poly_infty_norm_coeffmod(poly, modulus);
            Assert::IsTrue(inftynorm == BigUInt("2"));

            poly = BigPoly("AAx^10 + ABx^9 + CAx^8 + CFx^7 + FEx^6 + F7x^5 + 1x^2 + 2");
            modulus = "10";
            inftynorm = poly_infty_norm_coeffmod(poly, modulus);
            Assert::IsTrue(inftynorm == BigUInt("7"));

            poly = BigPoly("Ax^10 + ABx^9 + ABCx^8 + ABCDx^7 + ABCDEx^6 + ABCDEFx^5 + 1x^2 + 2");
            modulus = "4";
            inftynorm = poly_infty_norm_coeffmod(poly, modulus);
            Assert::IsTrue(inftynorm == BigUInt("2"));

            poly = BigPoly("1x^5 + 2x^4 + 3x^3 + 4x^2 + 5x^1 + 6");
            modulus = "4";
            inftynorm = poly_infty_norm_coeffmod(poly, modulus);
            Assert::IsTrue(inftynorm == BigUInt("2"));
        }

        TEST_METHOD(PolyEvalPoly)
        {
            BigPoly poly_to_eval("0");
            BigPoly poly("0");
            BigPoly result = poly_eval_poly(poly_to_eval, poly);
            Assert::IsTrue(result.to_string() == "0");

            poly_to_eval = "1";
            poly = "0";
            result = poly_eval_poly(poly_to_eval, poly);
            Assert::IsTrue(result.to_string() == "1");

            poly_to_eval = "12345ABCDE";
            poly = "0";
            result = poly_eval_poly(poly_to_eval, poly);
            Assert::IsTrue(result.to_string() == "12345ABCDE");

            poly_to_eval = "12345ABCDE";
            poly = "1";
            result = poly_eval_poly(poly_to_eval, poly);
            Assert::IsTrue(result.to_string() == "12345ABCDE");

            poly_to_eval = "0";
            poly = "1";
            result = poly_eval_poly(poly_to_eval, poly);
            Assert::IsTrue(result.to_string() == "0");

            poly_to_eval = "1x^1 + 2";
            poly = "1";
            result = poly_eval_poly(poly_to_eval, poly);
            Assert::IsTrue(result.to_string() == "3");

            poly_to_eval = "1x^1 + FFFFFFF";
            poly = "2x^1 + 1";
            result = poly_eval_poly(poly_to_eval, poly);
            Assert::IsTrue(result.to_string() == "2x^1 + 10000000");

            poly_to_eval = "1x^1 + 1";
            poly = "1x^100 + 2x^90 + 3x^80 + 4x^70 + 5x^60 + 6x^50 + 7x^40 + 8x^30 + 9x^20 + Ax^10 + B";
            result = poly_eval_poly(poly_to_eval, poly);
            Assert::IsTrue(result.to_string() == "1x^100 + 2x^90 + 3x^80 + 4x^70 + 5x^60 + 6x^50 + 7x^40 + 8x^30 + 9x^20 + Ax^10 + C");

            poly_to_eval = "1x^2 + 1";
            poly = "1x^10 + 2x^9 + 3x^8 + 4x^7 + 5x^6 + 6x^5 + 7x^4 + 8x^3 + 9x^2 + Ax^1 + B";
            result = poly_eval_poly(poly_to_eval, poly);
            Assert::IsTrue(result.to_string() == "1x^20 + 4x^19 + Ax^18 + 14x^17 + 23x^16 + 38x^15 + 54x^14 + 78x^13 + A5x^12 + DCx^11 + 11Ex^10 + 154x^9 + 17Dx^8 + 198x^7 + 1A4x^6 + 1A0x^5 + 18Bx^4 + 164x^3 + 12Ax^2 + DCx^1 + 7A");

            poly_to_eval = "1x^3 + 1x^2 + 1";
            poly = "1x^2 + 1x^1 + 1";
            result = poly_eval_poly(poly_to_eval, poly);
            Assert::IsTrue(result.to_string() == "1x^6 + 3x^5 + 7x^4 + 9x^3 + 9x^2 + 5x^1 + 3");

            poly_to_eval = "1x^100 + 2x^95 + 3x^90 + Ax^75 + Bx^70 + Cx^65 + Dx^60 + Fx^30 + Ex^20 + Dx^10 + 1x^9 + 2x^8 + 3x^7 + 4x^6 + 5x^5 + 1x^2 + 1";
            poly = "1";
            result = poly_eval_poly(poly_to_eval, poly);
            Assert::IsTrue(result.to_string() == "6F");

            poly_to_eval = "1x^100 + 2x^95 + 3x^90 + Ax^75 + Bx^70 + Cx^65 + Dx^60 + Fx^30 + Ex^20 + Dx^10 + 1x^9 + 2x^8 + 3x^7 + 4x^6 + 5x^5 + 1x^2 + 1";
            poly = "3";
            result = poly_eval_poly(poly_to_eval, poly);
            Assert::IsTrue(result.to_string() == "5B05B5BB47C5083385621FA57ACC77AAFD787C71");
        }
    };
}