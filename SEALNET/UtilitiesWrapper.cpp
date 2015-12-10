#include "UtilitiesWrapper.h"
#include "BigUIntWrapper.h"
#include "BigPolyWrapper.h"
#include "EncryptionParamsWrapper.h"
#include "Common.h"

using namespace System;
using namespace std;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            BigUInt ^Utilities::InherentNoise(BigPoly ^encrypted, BigPoly ^plain, EncryptionParameters ^parms, BigPoly ^secretKey)
            {
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                if (secretKey == nullptr)
                {
                    throw gcnew ArgumentNullException("secretKey cannot be null");
                }
                try
                {
                    return gcnew BigUInt(seal::inherent_noise(encrypted->GetPolynomial(), plain->GetPolynomial(), parms->GetParameters(), secretKey->GetPolynomial()));
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                throw gcnew Exception("Unexpected exception");
            }

            BigUInt ^Utilities::InherentNoise(BigPoly ^encrypted, EncryptionParameters ^parms, BigPoly ^secretKey)
            {
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                if (secretKey == nullptr)
                {
                    throw gcnew ArgumentNullException("secretKey cannot be null");
                }
                try
                {
                    return gcnew BigUInt(seal::inherent_noise(encrypted->GetPolynomial(), parms->GetParameters(), secretKey->GetPolynomial()));
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                throw gcnew Exception("Unexpected exception");
            }

            void Utilities::InherentNoise(BigPoly ^encrypted, BigPoly ^plain, EncryptionParameters ^parms, BigPoly ^secretKey, BigUInt ^result)
            {
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                if (secretKey == nullptr)
                {
                    throw gcnew ArgumentNullException("secretKey cannot be null");
                }
                if (result == nullptr)
                {
                    throw gcnew ArgumentNullException("result cannot be null");
                }
                try
                {
                    seal::inherent_noise(encrypted->GetPolynomial(), plain->GetPolynomial(), parms->GetParameters(), secretKey->GetPolynomial(), result->GetUInt());
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            BigUInt ^Utilities::InherentNoiseMax(EncryptionParameters ^parms)
            {
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                try
                {
                    return gcnew BigUInt(seal::inherent_noise_max(parms->GetParameters()));
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                throw gcnew Exception("Unexpected exception");
            }

            BigUInt ^Utilities::PolyInftyNorm(BigPoly ^poly)
            {
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    return gcnew BigUInt(seal::poly_infty_norm(poly->GetPolynomial()));
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                throw gcnew Exception("Unexpected exception");
            }

            BigUInt ^Utilities::PolyInftyNormCoeffmod(BigPoly ^poly, BigUInt ^modulus)
            {
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                if (modulus == nullptr)
                {
                    throw gcnew ArgumentNullException("modulus cannot be null");
                }
                try
                {
                    return gcnew BigUInt(seal::poly_infty_norm_coeffmod(poly->GetPolynomial(), modulus->GetUInt()));
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                throw gcnew Exception("Unexpected exception");
            }

            int Utilities::EstimateLevelMax(EncryptionParameters ^parms)
            {
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                try
                {
                    return seal::estimate_level_max(parms->GetParameters());
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                throw gcnew Exception("Unexpected exception");
            }

            void Utilities::ExponentiateUInt(BigUInt ^operand, int exponent, BigUInt ^result)
            {
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                if (result == nullptr)
                {
                    throw gcnew ArgumentNullException("result cannot be null");
                }
                try
                {
                    seal::exponentiate_uint(operand->GetUInt(), exponent, result->GetUInt());
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                throw gcnew Exception("Unhandled exception");
            }

            BigUInt ^Utilities::ExponentiateUInt(BigUInt ^operand, int exponent)
            {
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                try
                {
                    return gcnew BigUInt(seal::exponentiate_uint(operand->GetUInt(), exponent));
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                throw gcnew Exception("Unhandled exception");
            }

            void Utilities::ExponentiatePoly(BigPoly ^operand, int exponent, BigPoly ^result)
            {
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                if (result == nullptr)
                {
                    throw gcnew ArgumentNullException("result cannot be null");
                }
                try
                {
                    seal::exponentiate_poly(operand->GetPolynomial(), exponent, result->GetPolynomial());
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                throw gcnew Exception("Unhandled exception");
            }

            BigPoly ^Utilities::ExponentiatePoly(BigPoly ^operand, int exponent)
            {
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                try
                {
                    return gcnew BigPoly(seal::exponentiate_poly(operand->GetPolynomial(), exponent));
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                throw gcnew Exception("Unhandled exception");
            }

            BigPoly ^Utilities::PolyEvalPoly(BigPoly ^polyToEvaluate, BigPoly ^polyToEvaluateAt)
            {
                if (polyToEvaluate == nullptr)
                {
                    throw gcnew ArgumentNullException("polyToEvaluate cannot be null");
                }
                if (polyToEvaluateAt == nullptr)
                {
                    throw gcnew ArgumentNullException("polyToEvaluateAt cannot be null");
                }
                try
                {
                    return gcnew BigPoly(seal::poly_eval_poly(polyToEvaluate->GetPolynomial(), polyToEvaluateAt->GetPolynomial()));
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                throw gcnew Exception("Unhandled exception");
            }
        }
    }
}