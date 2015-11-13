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

            public ref class Utilities abstract sealed
            {
            public:
                static BigUInt ^InherentNoise(BigPoly ^encrypted, BigPoly ^plain, EncryptionParameters ^parms, BigPoly ^secret_key);

                static BigUInt ^InherentNoise(BigPoly ^encrypted, EncryptionParameters ^parms, BigPoly ^secret_key);

                static void InherentNoise(BigPoly ^encrypted, BigPoly ^plain, EncryptionParameters ^parms, BigPoly ^secret_key, BigUInt ^result);

                static BigUInt ^InherentNoiseMax(EncryptionParameters ^parms);

                static BigUInt ^PolyInftyNorm(BigPoly ^poly);

                static BigUInt ^PolyInftyNormCoeffmod(BigPoly ^poly, BigUInt ^modulus);

                static int EstimateLevelMax(EncryptionParameters ^parms);

                static BigUInt ^ExponentiateUInt(BigUInt ^operand, int exponent);

                static BigPoly ^ExponentiatePoly(BigPoly ^operand, int exponent);

                static BigPoly ^PolyEvalPoly(BigPoly ^poly_to_eval, BigPoly ^value);
            };
        }
    }
}
