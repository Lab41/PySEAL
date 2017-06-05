#pragma once

#include "utilities.h"
#include "EncryptionParamsWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class Simulation;

            public ref class Utilities abstract sealed
            {
            public:
                /**
                <summary>Computes the infinity-norm of a given polynomial with non-negative coefficients (represented by
                <see cref="BigPoly"/>).</summary>
                <param name="poly">The polynomial whose infinity-norm is to be computed</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                */
                static BigUInt ^PolyInftyNorm(BigPoly ^poly);

                /**
                <summary>Computes the infinity-norm of a given polynomial (represented by BigPoly).</summary>
                <remarks>
                Computes the infinity-norm of a given polynomial (represented by <see cref="BigPoly"/>). The coefficients of the polynomial are
                interpreted as integers modulo a given modulus. For each coefficient in the given polynomial, this function first
                finds a representative in the symmetric interval around zero, then computes its absolute value, and finally outputs
                the largest of them all.
                </remarks>
                <param name="poly">The polynomial whose infinity-norm is to be computed</param>
                <param name="modulus">The modulus used in the computation</param>
                <exception cref="System::ArgumentException">if poly or modulus is zero</exception>
                */
                static BigUInt ^PolyInftyNormCoeffmod(BigPoly ^poly, BigUInt ^modulus);

                /**
                <summary>Raises an unsigned integer (represented by BigUInt) to an unsigned integer power (represented by BigUInt)
                modulo a given modulus and stores the result in a given BigUInt.</summary>

                <remarks>
                Raises an unsigned integer (represented by <see cref="BigUInt"/>) to an unsigned integer power (represented by <see cref="BigUInt"/>)
                modulo a given modulus and stores the result in a given <see cref="BigUInt"/>. The input is expected to be already reduced modulo
                the modulus.
                </remarks>
                <param name="operand">The unsigned integer to exponentiate</param>
                <param name="exponent">The unsigned integer exponent</param>
                <param name="modulus">The modulus</param>
                <param name="destination">The unsigned integer to overwrite with the result</param>
                <exception cref="System::ArgumentNullException">if operand, exponent, modulus, or destination is null</exception>
                <exception cref="System::ArgumentException">if operand is not reduced modulo modulus</exception>
                <exception cref="System::ArgumentException">if both operand and exponent are zero</exception>
                */
                static void ExponentiateUIntMod(BigUInt ^operand, BigUInt ^exponent, BigUInt ^modulus, BigUInt ^destination);

                /**
                <summary>Raises an unsigned integer (represented by BigUInt) to an unsigned integer power (represented by BigUInt)
                modulo a given modulus and returns the result.</summary>

                <remarks>
                Raises an unsigned integer (represented by <see cref="BigUInt"/>) to an unsigned integer power (represented by <see cref="BigUInt"/>)
                modulo a given modulus and returns the result. The input is expected to be already reduced modulo
                the modulus.
                </remarks>
                <param name="operand">The unsigned integer to exponentiate</param>
                <param name="exponent">The unsigned integer exponent</param>
                <param name="modulus">The modulus</param>
                <exception cref="System::ArgumentNullException">if operand, exponent, or modulus is null</exception>
                <exception cref="System::ArgumentException">if operand is not reduced modulo modulus</exception>
                <exception cref="System::ArgumentException">if both operand and exponent are zero</exception>
                */
                static BigUInt ^ExponentiateUIntMod(BigUInt ^operand, BigUInt ^exponent, BigUInt ^modulus);

                /**
                <summary>Raises a polynomial (represented by BigPoly) to an unsigned integer power (represented by BigUInt) modulo a
                polynomial and a coefficient modulus, and stores the result in a given BigPoly.</summary>

                <remarks>
                Raises a polynomial (represented by <see cref="BigPoly"/>) to an unsigned integer power (represented by <see cref="BigUInt"/>) 
                modulo a polynomial and a coefficient modulus, and stores the result in a given <see cref="BigPoly"/>. The input is expected 
                to be already reduced both modulo the polynomial and the coefficient modulus.
                </remarks>
                <param name="operand">The polynomial to exponentiate</param>
                <param name="exponent">The unsigned integer exponent</param>
                <param name="polyModulus">The polynomial modulus</param>
                <param name="coeffModulus">The coefficient modulus</param>
                <param name="destination">The polynomial to overwrite with the result</param>
                <exception cref="System::ArgumentNullException">if operand, exponent, polyModulus, coeffModulus, or destination is null</exception>
                <exception cref="System::ArgumentException">if operand is not reduced modulo polyModulus and coeffModulus</exception>
                <exception cref="System::ArgumentException">if exponent is negative</exception>
                <exception cref="System::ArgumentException">if both operand and exponent are zero</exception>
                */
                static void ExponentiatePolyPolymodCoeffmod(BigPoly ^operand, BigUInt ^exponent, 
                    BigPoly ^polyModulus, BigUInt ^coeffModulus, BigPoly ^destination);

                /**
                <summary>Raises a polynomial (represented by BigPoly) to an unsigned integer power (represented by BigUInt) modulo a
                polynomial and a coefficient modulus, and returns the result.</summary>

                <remarks>
                Raises a polynomial (represented by <see cref="BigPoly"/>) to an unsigned integer power (represented by <see cref="BigUInt"/>)
                modulo a polynomial and a coefficient modulus, and returns the result. The input is expected to be already reduced both 
                modulo the polynomial and the coefficient modulus.
                </remarks>
                <param name="operand">The polynomial to exponentiate</param>
                <param name="exponent">The unsigned integer exponent</param>
                <param name="polyModulus">The polynomial modulus</param>
                <param name="coeffModulus">The coefficient modulus</param>
                <exception cref="System::ArgumentNullException">if operand, exponent, polyModulus, or coeffModulus is null</exception>
                <exception cref="System::ArgumentException">if operand is not reduced modulo polyModulus and coeffModulus</exception>
                <exception cref="System::ArgumentException">if exponent is negative</exception>
                <exception cref="System::ArgumentException">if both operand and exponent are zero</exception>
                */
                static BigPoly ^ExponentiatePolyPolymodCoeffmod(BigPoly ^operand, BigUInt ^exponent, 
                    BigPoly ^polyModulus, BigUInt ^coeffModulus);

                /**
                <summary>Evaluates a given polynomial at another given polynomial and stores the result in a given BigPoly.</summary>
                <param name="polyToEvaluate">The polynomial to be evaluated</param>
                <param name="polyToEvaluateAt">The polynomial polyToEvaluate will be evaluated by replacing its variable with this
                polynomial</param>
                <param name="destination">The polynomial to overwrite with the result</param>
                <exception cref="System::ArgumentNullException">if polyToEvaluate, polyToEvaluateAt, or destination is null</exception>
                */
                static void PolyEvalPoly(BigPoly ^polyToEvaluate, BigPoly ^polyToEvaluateAt, BigPoly ^destination);

                /**
                <summary>Evaluates a given polynomial at another given polynomial and returns the result.</summary>
                <param name="polyToEvaluate">The polynomial to be evaluated</param>
                <param name="polyToEvaluateAt">The polynomial polyToEvaluate will be evaluated by replacing its variable with this
                polynomial</param>
                <exception cref="System::ArgumentNullException">if polyToEvaluate, or polyToEvaluateAt is null</exception>
                */
                static BigPoly ^PolyEvalPoly(BigPoly ^polyToEvaluate, BigPoly ^polyToEvaluateAt);

                /**
                <summary>Evaluates a given polynomial at another given polynomial modulo a polynomial and a coefficient modulus and
                stores the result in a given BigPoly.</summary>

                <remarks>
                Evaluates a given polynomial at another given polynomial modulo a polynomial and a coefficient modulus and stores the
                result in a given <see cref="BigPoly"/>. The inputs are expected to be already reduced both modulo the polynomial and the
                coefficient modulus.
                </remarks>
                <param name="polyToEvaluate">The polynomial to be evaluated</param>
                <param name="polyToEvaluateAt">The polynomial polyToEvaluate will be evaluated by replacing its variable with this
                polynomial</param>
                <param name="polyModulus">The polynomial modulus</param>
                <param name="coeffModulus">The coefficient modulus</param>
                <param name="destination">The polynomial to overwrite with the result</param>
                <exception cref="System::ArgumentNullException">if polyToEvaluate, polyToEvaluateAt, polyModulus, coeffModulus, or destination is null</exception>
                <exception cref="System::ArgumentException">if polyToEvaluate is not reduced modulo polyModulus and
                coeffModulus</exception>
                <exception cref="System::ArgumentException">if polyToEvaluateAt is not reduced modulo polyModulus and
                coeffModulus</exception>
                */
                static void PolyEvalPolyPolymodCoeffmod(BigPoly ^polyToEvaluate, BigPoly ^polyToEvaluateAt, 
                    BigPoly ^polyModulus, BigUInt ^coeffModulus, BigPoly ^destination);

                /**
                <summary>Evaluates a given polynomial at another given polynomial modulo a polynomial and a coefficient 
                modulus and returns the result.</summary>

                <remarks>
                Evaluates a given polynomial at another given polynomial modulo a polynomial and a coefficient modulus and
                returns the result. The inputs are expected to be already reduced both modulo the polynomial and the
                coefficient modulus.
                </remarks>
                <param name="polyToEvaluate">The polynomial to be evaluated</param>
                <param name="polyToEvaluateAt">The polynomial polyToEvaluate will be evaluated by replacing its variable with this
                polynomial</param>
                <param name="polyModulus">The polynomial modulus</param>
                <param name="coeffModulus">The coefficient modulus</param>
                <exception cref="System::ArgumentNullException">if polyToEvaluate, polyToEvaluateAt, polyModulus, or coeffModulus is null</exception>
                <exception cref="System::ArgumentException">if polyToEvaluate is not reduced modulo polyModulus and
                coeffModulus</exception>
                <exception cref="System::ArgumentException">if polyToEvaluateAt is not reduced modulo polyModulus and
                coeffModulus</exception>
                */
                static BigPoly ^PolyEvalPolyPolymodCoeffmod(BigPoly ^polyToEvaluate, BigPoly ^polyToEvaluateAt, 
                    BigPoly ^polyModulus, BigUInt ^coeffModulus);

                /**
                <summary>Evaluates a given polynomial at an unsigned integer modulo a given modulus and stores the result 
                in a given BigUInt.</summary>

                <remarks>
                Evaluates a given polynomial at an unsigned integer modulo a given modulus and stores the result in a given <see cref="BigUInt"/>.
                The coefficients of polyToEvaluate and the value to evaluate at are expected to be already reduced modulo the
                modulus.
                </remarks>
                <param name="polyToEvaluate">The polynomial to be evaluated</param>
                <param name="value">The unsigned integer at which the polynomial is evaluated</param>
                <param name="modulus">The modulus</param>
                <param name="destination">The unsigned integer to overwrite with the result</param>
                <exception cref="System::ArgumentNullException">if polyToEvaluate, value, modulus, or destination is null</exception>
                <exception cref="System::ArgumentException">if the coefficients of polyToEvaluate are not reduced modulo
                modulus</exception>
                <exception cref="System::ArgumentException">if value is not reduced modulo modulus</exception>
                */
                static void PolyEvalUIntMod(BigPoly ^polyToEvaluate, BigUInt ^value, BigUInt ^modulus, BigUInt ^destination);

                /**
                <summary>Evaluates a given polynomial at an unsigned integer modulo a given modulus and returns the result.</summary>

                <remarks>
                Evaluates a given polynomial at an unsigned integer modulo a given modulus and returns the result.
                The coefficients of polyToEvaluate and the value to evaluate at are expected to be already reduced modulo the
                modulus.
                </remarks>
                <param name="polyToEvaluate">The polynomial to be evaluated</param>
                <param name="value">The unsigned integer at which the polynomial is evaluated</param>
                <param name="modulus">The modulus</param>
                <exception cref="System::ArgumentNullException">if polyToEvaluate, value, or modulus is null</exception>
                <exception cref="System::ArgumentException">if the coefficients of polyToEvaluate are not reduced modulo
                modulus</exception>
                <exception cref="System::ArgumentException">if value is not reduced modulo modulus</exception>
                */
                static BigUInt ^PolyEvalUIntMod(BigPoly ^polyToEvaluate, BigUInt ^value, BigUInt ^modulus);
            };
        }
    }
}
