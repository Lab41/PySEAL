#pragma once

#include "utilities.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class BigUInt;

            ref class BigPoly;

            ref class EncryptionParameters;

            ref class Simulation;

            public ref class Utilities abstract sealed
            {
            public:
                /**
                <summary>Computes the exact inherent noise in a ciphertext and returns it as a BigUInt.</summary>
                <remarks>
                <para>
                Computes the exact inherent noise in a ciphertext and returns it as a <see cref="BigUInt"/>. Computing the inherent noise requires
                the ciphertext, the secret key, and the plaintext.
                </para>
                <para>
                Technically speaking, the inherent noise of a ciphertext is a polynomial, but the condition for decryption working
                depends on the size of the largest absolute value of its coefficients. It is this largest absolute value that we will
                call the "noise", the "inherent noise", or the "error", in this documentation. The reader is referred to the
                description of the encryption scheme for more details.
                </para>
                </remarks>
                <param name="encrypted">The encrypted polynomial</param>
                <param name="plain">The corresponding plaintext polynomial</param>
                <param name="parms">The encryption parameters</param>
                <param name="secretKey">The secret key</param>
                <exception cref="System::ArgumentNullException">if encrypted, plain, parms, or secretKey is null</exception>
                <exception cref="System::ArgumentException">if encryption parameters are not valid</exception>
                <exception cref="System::ArgumentException">if the encrypted polynomial is not a valid encrypted polynomial for the
                encryption parameters</exception>
                <exception cref="System::ArgumentException">if the plain polynomial's significant coefficient count or coefficient
                values are too large to represent with the encryption parameters</exception>
                <exception cref="System::ArgumentException">if the secret key is not valid</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption
                parameters.</seealso>
                <seealso>See InherentNoiseMax() for computing the maximum value of inherent noise supported
                by given encryption parameters.</seealso>
                */
                static BigUInt ^InherentNoise(BigPoly ^encrypted, BigPoly ^plain, EncryptionParameters ^parms, BigPoly ^secretKey);

                /**
                <summary>Computes the exact inherent noise in a ciphertext and returns it as a BigUInt.</summary>
                <remarks>
                <para>
                Computes the exact inherent noise in a ciphertext and returns it as a <see cref="BigUInt"/>. Computing the inherent noise requires
                the ciphertext, the secret key, and the plaintext, which this function obtains by decrypting the ciphertext with the
                secret key.
                </para>
                <para>
                Technically speaking, the inherent noise of a ciphertext is a polynomial, but the condition for decryption working
                depends on the size of the largest absolute value of its coefficients. It is this largest absolute value that we will
                call the "noise", the "inherent noise", or the "error", in this documentation. The reader is referred to the
                description of the encryption scheme for more details.
                </para>
                </remarks>
                <param name="encrypted">The encrypted polynomial</param>
                <param name="parms">The encryption parameters</param>
                <param name="secretKey">The secret key</param>
                <exception cref="System::ArgumentNullException">if encrypted, parms, or secretKey is null</exception>
                <exception cref="System::ArgumentException">if encryption parameters are not valid</exception>
                <exception cref="System::ArgumentException">if the encrypted polynomial is not a valid encrypted polynomial for the
                encryption parameters</exception>
                <exception cref="System::ArgumentException">if the secret key is not valid</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption
                parameters.</seealso>
                <seealso>See InherentNoiseMax() for computing the maximum value of inherent noise supported
                by given encryption parameters.</seealso>
                */
                static BigUInt ^InherentNoise(BigPoly ^encrypted, EncryptionParameters ^parms, BigPoly ^secretKey);

                /**
                <summary>Computes the exact inherent noise in a ciphertext and stores it in a BigUInt given by reference.</summary>
                <remarks>
                <para>
                Computes the exact inherent noise in a ciphertext and stores it in a <see cref="BigUInt"/> given by reference. Computing the inherent
                noise requires the ciphertext, the secret key, and the plaintext.
                </para>
                <para>
                Technically speaking, the inherent noise of a ciphertext is a polynomial, but the condition for decryption working
                depends on the size of the largest absolute value of its coefficients. It is this largest absolute value that we will
                call the "noise", the "inherent noise", or the "error", in this documentation. The reader is referred to the
                description of the encryption scheme for more details.
                </para>
                </remarks>
                <param name="encrypted">The encrypted polynomial</param>
                <param name="plain">The corresponding plaintext polynomial</param>
                <param name="parms">The encryption parameters</param>
                <param name="secretKey">The secret key</param>
                <param name="result">The <see cref="BigUInt"/> to overwrite with the inherent noise</param>
                <exception cref="System::ArgumentNullException">if encrypted, plain, parms, secretKey, or result is null</exception>
                <exception cref="System::ArgumentException">if encryption parameters are not valid</exception>
                <exception cref="System::ArgumentException">if the encrypted polynomial is not a valid encrypted polynomial for the
                encryption parameters</exception>
                <exception cref="System::ArgumentException">if the plain polynomial's significant coefficient count or coefficient
                values are too large to represent with the encryption parameters</exception>
                <exception cref="System::ArgumentException">if the secret key is not valid</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption
                parameters.</seealso>
                <seealso>See InherentNoiseMax() for computing the maximum value of inherent noise supported
                by given encryption parameters.</seealso>
                */
                static void InherentNoise(BigPoly ^encrypted, BigPoly ^plain, EncryptionParameters ^parms, BigPoly ^secretKey, BigUInt ^result);

                /**
                <summary>Computes the maximum value of inherent noise supported by given encryption parameters.</summary>
                <remarks>
                <para>
                Computes the maximum value of inherent noise supported by given encryption parameters. Any ciphertext with larger
                inherent noise is impossible to decrypt, even with the correct secret key.
                </para>
                <para>
                Technically speaking, the inherent noise of a ciphertext is a polynomial, but the condition for decryption working
                depends on the size of the largest absolute value of its coefficients. It is this largest absolute value that we will
                call the "noise", the "inherent noise", or the "error", in this documentation. The reader is referred to the
                description of the encryption scheme for more details.
                </para>
                </remarks>
                <param name="parms">The encryption parameters</param>
                <exception cref="System::ArgumentNullException">if parms is null</exception>
                <exception cref="System::ArgumentException">if encryption parameters are not valid</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption
                parameters.</seealso>
                <seealso>See InherentNoise() for computing the exact value of inherent noise in a given ciphertext.</seealso>
                */
                static BigUInt ^InherentNoiseMax(EncryptionParameters ^parms);

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
                <summary>Estimates using <see cref="Simulation"/> the maximum "level" that can be achieved by given encryption parameters.</summary>
                <param name="parms">The encryption parameters</param>
                <exception cref="System::ArgumentNullException">if parms is null</exception>
                <exception cref="System::ArgumentException">if encryption parameters are not valid</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption parameters.</seealso>
                */
                static int EstimateLevelMax(EncryptionParameters ^parms);

                /**
                <summary>Raises an unsigned integer (represented by <see cref="BigUInt"/>) to a non-negative integer power
                and stores the result in a given BigUInt.</summary>
                <param name="operand">The unsigned integer to exponentiate</param>
                <param name="exponent">The non-negative integer exponent</param>
                <param name="result">The unsigned integer to overwrite with the result</param>
                <exception cref="System::ArgumentNullException">if operand or result is null</exception>
                <exception cref="System::ArgumentException">if exponent is negative</exception>
                <exception cref="System::ArgumentException">if both operand and exponent are zero</exception>
                */
                static void ExponentiateUInt(BigUInt ^operand, int exponent, BigUInt ^result);

                /**
                <summary>Raises an unsigned integer (represented by <see cref="BigUInt"/>) to a given non-negative integer power.</summary>
                <param name="operand">The unsigned integer to exponentiate</param>
                <param name="exponent">The non-negative integer exponent</param>
                <exception cref="System::ArgumentNullException">if operand is null</exception>
                <exception cref="System::ArgumentException">if exponent is negative</exception>
                <exception cref="System::ArgumentException">if both operand and exponent are zero</exception>
                */
                static BigUInt ^ExponentiateUInt(BigUInt ^operand, int exponent);

                /**
                <summary>Raises a polynomial (represented by <see cref="BigPoly"/>) to a non-negative integer power and stores the result in a given
                BigPoly.</summary>
                <param name="operand">The polynomial to exponentiate</param>
                <param name="exponent">The non-negative integer exponent</param>
                <param name="result">The polynomial to overwrite with the result</param>
                <exception cref="System::ArgumentNullException">if operand or result is null</exception>
                <exception cref="System::ArgumentException">if exponent is negative</exception>
                <exception cref="System::ArgumentException">if both operand and exponent are zero</exception>
                */
                static void ExponentiatePoly(BigPoly ^operand, int exponent, BigPoly ^result);

                /**
                <summary>Raises a polynomial (represented by BigPoly) to a non-negative integer power.</summary>
                <param name="operand">The polynomial to exponentiate</param>
                <param name="exponent">The non-negative integer exponent</param>
                <exception cref="System::ArgumentNullException">if operand is null</exception>
                <exception cref="System::ArgumentException">if exponent is negative</exception>
                <exception cref="System::ArgumentException">if both operand and exponent are zero</exception>
                */
                static BigPoly ^ExponentiatePoly(BigPoly ^operand, int exponent);

                /**
                <summary>Evaluates a given polynomial at another given polynomial.</summary>
                <param name="polyToEvaluate">The polynomial to be evaluated</param>
                <param name="polyToEvaluateAt">The polynomial polyToEvaluate will be evaluated by replacing its variable with this
                polynomial</param>
                <exception cref="System::ArgumentNullException">if polyToEvaluate or polyToEvaluateAt is null</exception>
                */
                static BigPoly ^PolyEvalPoly(BigPoly ^polyToEvaluate, BigPoly ^polyToEvaluateAt);
            };
        }
    }
}
