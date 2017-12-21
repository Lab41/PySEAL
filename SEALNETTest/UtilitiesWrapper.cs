using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.Research.SEAL;

namespace SEALNETTest
{
    [TestClass]
    public class UtilitiesWrapper
    {
        [TestMethod]
        public void BigPolyInftyNormNET()
        {
            var poly = new BigPoly("1x^10 + 2x^9 + 5x^8 + Ax^7 + Bx^6 + 4x^5 + 1x^2 + 1");
            BigUInt value = new BigUInt();
            var inftynorm = Utilities.PolyInftyNorm(poly);
            value.Set("B");
            Assert.AreEqual(value, inftynorm);

            poly.Set("AAx^10 + ABx^9 + CAx^8 + CFx^7 + FEx^6 + F7x^5 + 1x^2 + 2");
            inftynorm = Utilities.PolyInftyNorm(poly);
            value.Set("FE");
            Assert.AreEqual(value, inftynorm);

            poly.Set("Ax^10 + ABx^9 + ABCx^8 + ABCDx^7 + ABCDEx^6 + ABCDEFx^5 + 1x^2 + 2");
            inftynorm = Utilities.PolyInftyNorm(poly);
            value.Set("ABCDEF");
            Assert.AreEqual(value, inftynorm);

            poly.Set("1x^5 + 2x^4 + 3x^3 + 4x^2 + 5x^1 + 6");
            inftynorm = Utilities.PolyInftyNorm(poly);
            value.Set("6");
            Assert.AreEqual(value, inftynorm);
        }

        [TestMethod]
        public void PolyInftyNormCoeffmodNET()
        {
            var poly = new BigPoly("1x^10 + 2x^9 + 5x^8 + Ax^7 + Bx^6 + 4x^5 + 1x^2 + 1");
            BigUInt value = new BigUInt();
            BigUInt modulus = new BigUInt();
            modulus.Set(5);
            var inftynorm = Utilities.PolyInftyNormCoeffmod(poly, modulus);
            value.Set("2");
            Assert.AreEqual(value, inftynorm);

            poly.Set("AAx^10 + ABx^9 + CAx^8 + CFx^7 + FEx^6 + F7x^5 + 1x^2 + 2");
            modulus.Set("10");
            inftynorm = Utilities.PolyInftyNormCoeffmod(poly, modulus);
            value.Set("7");
            Assert.AreEqual(value, inftynorm);

            poly.Set("Ax^10 + ABx^9 + ABCx^8 + ABCDx^7 + ABCDEx^6 + ABCDEFx^5 + 1x^2 + 2");
            modulus.Set("4");
            inftynorm = Utilities.PolyInftyNormCoeffmod(poly, modulus);
            value.Set("2");
            Assert.AreEqual(value, inftynorm);

            poly.Set("1x^5 + 2x^4 + 3x^3 + 4x^2 + 5x^1 + 6");
            modulus.Set("4");
            inftynorm = Utilities.PolyInftyNormCoeffmod(poly, modulus);
            value.Set("2");
            Assert.AreEqual(value, inftynorm);
        }

        [TestMethod]
        public void ExponentiateBigUIntModNET()
        {
            var myUInt = new BigUInt("ABABABAB");
            var modulus = new BigUInt("CAACAACAA");
            var exponent = new BigUInt("5");
            var result = Utilities.ExponentiateUIntMod(myUInt, exponent, modulus);
            Assert.AreEqual("33773505765", result.ToDecimalString());

            myUInt.Set("1");
            exponent.Set("F00F00F00F00F00");
            result = Utilities.ExponentiateUIntMod(myUInt, exponent, modulus);
            Assert.AreEqual("1", result.ToDecimalString());

            modulus.Set("AAAAAAAAAAAAAAAAAAAAA");
            myUInt.Set("F00F00F00F00F00");
            exponent.Set("1");
            result = Utilities.ExponentiateUIntMod(myUInt, exponent, modulus);
            Assert.AreEqual("F00F00F00F00F00", result.ToString());

            myUInt.Set("F00F00F00F00F00");
            exponent.Set("0");
            result = Utilities.ExponentiateUIntMod(myUInt, exponent, modulus);
            Assert.AreEqual("1", result.ToString());
        }

        [TestMethod]
        public void ExponentiatePolyPolyPolyModCoeffModNET()
        {
            var poly = new BigPoly("1x^2 + 2x^1 + 3");
            var modulus = new BigUInt("5");
            var exponent = new BigUInt("1");
            var polymod = new BigPoly("1x^3 + 3x^1 + 1");
            var result = Utilities.ExponentiatePolyPolymodCoeffmod(poly, exponent, polymod, modulus);
            Assert.AreEqual("1x^2 + 2x^1 + 3", result.ToString());
            exponent.Set("2");
            result = Utilities.ExponentiatePolyPolymodCoeffmod(poly, exponent, polymod, modulus);
            Assert.AreEqual("2x^2 + 4x^1", result.ToString());

            poly.Set("1");
            exponent.Set("2");
            result = Utilities.ExponentiatePolyPolymodCoeffmod(poly, exponent, polymod, modulus);
            Assert.AreEqual("1", result.ToString());
            exponent.Set("F00F000F00");
            result = Utilities.ExponentiatePolyPolymodCoeffmod(poly, exponent, polymod, modulus);
            Assert.AreEqual("1", result.ToString());
        }

        [TestMethod]
        public void BigPolyEvalPolyNET()
        {
            var polyToEval = new BigPoly("0");
            var poly = new BigPoly("0");
            var result = Utilities.PolyEvalPoly(polyToEval, poly);
            Assert.AreEqual("0", result.ToString());

            polyToEval.Set("1");
            poly.Set("0");
            result.Set(Utilities.PolyEvalPoly(polyToEval, poly));
            Assert.AreEqual("1", result.ToString());

            polyToEval.Set("12345ABCDE");
            poly.Set("0");
            result.Set(Utilities.PolyEvalPoly(polyToEval, poly));
            Assert.AreEqual("12345ABCDE", result.ToString());

            polyToEval.Set("12345ABCDE");
            poly.Set("1");
            result.Set(Utilities.PolyEvalPoly(polyToEval, poly));
            Assert.AreEqual("12345ABCDE", result.ToString());

            polyToEval.Set("0");
            poly.Set("1");
            result.Set(Utilities.PolyEvalPoly(polyToEval, poly));
            Assert.AreEqual("0", result.ToString());

            polyToEval.Set("1x^1 + 2");
            poly.Set("1");
            result.Set(Utilities.PolyEvalPoly(polyToEval, poly));
            Assert.AreEqual("3", result.ToString());

            polyToEval.Set("1x^1 + FFFFFFF");
            poly.Set("2x^1 + 1");
            result.Set(Utilities.PolyEvalPoly(polyToEval, poly));
            Assert.AreEqual("2x^1 + 10000000", result.ToString());

            polyToEval.Set("1x^1 + 1");
            poly.Set("1x^100 + 2x^90 + 3x^80 + 4x^70 + 5x^60 + 6x^50 + 7x^40 + 8x^30 + 9x^20 + Ax^10 + B");
            result.Set(Utilities.PolyEvalPoly(polyToEval, poly));
            Assert.AreEqual("1x^100 + 2x^90 + 3x^80 + 4x^70 + 5x^60 + 6x^50 + 7x^40 + 8x^30 + 9x^20 + Ax^10 + C", result.ToString());

            polyToEval.Set("1x^2 + 1");
            poly.Set("1x^10 + 2x^9 + 3x^8 + 4x^7 + 5x^6 + 6x^5 + 7x^4 + 8x^3 + 9x^2 + Ax^1 + B");
            result.Set(Utilities.PolyEvalPoly(polyToEval, poly));
            Assert.AreEqual("1x^20 + 4x^19 + Ax^18 + 14x^17 + 23x^16 + 38x^15 + 54x^14 + 78x^13 + A5x^12 + DCx^11 + 11Ex^10 + 154x^9 + 17Dx^8 + 198x^7 + 1A4x^6 + 1A0x^5 + 18Bx^4 + 164x^3 + 12Ax^2 + DCx^1 + 7A", result.ToString());

            polyToEval.Set("1x^3 + 1x^2 + 1");
            poly.Set("1x^2 + 1x^1 + 1");
            result.Set(Utilities.PolyEvalPoly(polyToEval, poly));
            Assert.AreEqual("1x^6 + 3x^5 + 7x^4 + 9x^3 + 9x^2 + 5x^1 + 3", result.ToString());

            polyToEval.Set("1x^100 + 2x^95 + 3x^90 + Ax^75 + Bx^70 + Cx^65 + Dx^60 + Fx^30 + Ex^20 + Dx^10 + 1x^9 + 2x^8 + 3x^7 + 4x^6 + 5x^5 + 1x^2 + 1");
            poly.Set("1");
            result.Set(Utilities.PolyEvalPoly(polyToEval, poly));
            Assert.AreEqual("6F", result.ToString());

            polyToEval.Set("1x^100 + 2x^95 + 3x^90 + Ax^75 + Bx^70 + Cx^65 + Dx^60 + Fx^30 + Ex^20 + Dx^10 + 1x^9 + 2x^8 + 3x^7 + 4x^6 + 5x^5 + 1x^2 + 1");
            poly.Set("3");
            result.Set(Utilities.PolyEvalPoly(polyToEval, poly));
            Assert.AreEqual("5B05B5BB47C5083385621FA57ACC77AAFD787C71", result.ToString());
        }

        [TestMethod]
        public void BigPolyEvalPolyPolyModCoeffModNET()
        {
            var modulus = new BigUInt("5");
            var polymod = new BigPoly("1x^3 + 3x^1 + 1");
            var poly_to_eval = new BigPoly("0");
            var poly = new BigPoly("0");
            var result = Utilities.PolyEvalPolyPolymodCoeffmod(poly_to_eval, poly, polymod, modulus);
            Assert.AreEqual("0", result.ToString());

            poly_to_eval.Set("1");
            poly.Set("0");
            result = Utilities.PolyEvalPolyPolymodCoeffmod(poly_to_eval, poly, polymod, modulus);
            Assert.AreEqual("1", result.ToString());

            poly_to_eval.Set("4");
            poly.Set("0");
            result = Utilities.PolyEvalPolyPolymodCoeffmod(poly_to_eval, poly, polymod, modulus);
            Assert.AreEqual("4", result.ToString());

            poly_to_eval.Set("4");
            poly.Set("1");
            result = Utilities.PolyEvalPolyPolymodCoeffmod(poly_to_eval, poly, polymod, modulus);
            Assert.AreEqual("4", result.ToString());

            poly_to_eval.Set("0");
            poly.Set("1");
            result = Utilities.PolyEvalPolyPolymodCoeffmod(poly_to_eval, poly, polymod, modulus);
            Assert.AreEqual("0", result.ToString());

            poly_to_eval.Set("1x^1 + 2");
            poly.Set("1");
            result = Utilities.PolyEvalPolyPolymodCoeffmod(poly_to_eval, poly, polymod, modulus);
            Assert.AreEqual("3", result.ToString());

            poly_to_eval.Set("1x^1 + 4");
            poly.Set("2x^1 + 1");
            result = Utilities.PolyEvalPolyPolymodCoeffmod(poly_to_eval, poly, polymod, modulus);
            Assert.AreEqual("2x^1", result.ToString());

            poly_to_eval.Set("1x^1 + 1");
            poly.Set("1x^2 + 2x^1 + 3");
            result = Utilities.PolyEvalPolyPolymodCoeffmod(poly_to_eval, poly, polymod, modulus);
            Assert.AreEqual("1x^2 + 2x^1 + 4", result.ToString());

            poly_to_eval.Set("1x^2 + 1");
            poly.Set("1x^2 + 2x^1 + 3");
            result = Utilities.PolyEvalPolyPolymodCoeffmod(poly_to_eval, poly, polymod, modulus);
            Assert.AreEqual("2x^2 + 4x^1 + 1", result.ToString());
        }

        [TestMethod]
        public void BigPolyEvalUIntModNET()
        {
            var modulus = new BigUInt("5");
            var poly = new BigPoly("1x^2 + 1x^1 + 1");
            var value = new BigUInt("0");
            var result = Utilities.PolyEvalUIntMod(poly, value, modulus);
            Assert.AreEqual("1", result.ToString());

            value.Set("1");
            result = Utilities.PolyEvalUIntMod(poly, value, modulus);
            Assert.AreEqual("3", result.ToString());

            value.Set("4");
            result = Utilities.PolyEvalUIntMod(poly, value, modulus);
            Assert.AreEqual("1", result.ToString());
        }
    }
}