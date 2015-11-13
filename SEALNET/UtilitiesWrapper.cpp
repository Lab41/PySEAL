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
            BigUInt ^Utilities::InherentNoise(BigPoly ^encrypted, BigPoly ^plain, EncryptionParameters ^parms, BigPoly ^secret_key)
            {
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain");
                }
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms");
                }
                if (secret_key == nullptr)
                {
                    throw gcnew ArgumentNullException("secret_key");
                }
                try
                {
                    return gcnew BigUInt(seal::inherent_noise(encrypted->GetPolynomial(), plain->GetPolynomial(), parms->GetParameters(), secret_key->GetPolynomial()));
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

            BigUInt ^Utilities::InherentNoise(BigPoly ^encrypted, EncryptionParameters ^parms, BigPoly ^secret_key)
            {
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted");
                }
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms");
                }
                if (secret_key == nullptr)
                {
                    throw gcnew ArgumentNullException("secret_key");
                }
                try
                {
                    return gcnew BigUInt(seal::inherent_noise(encrypted->GetPolynomial(), parms->GetParameters(), secret_key->GetPolynomial()));
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

            void Utilities::InherentNoise(BigPoly ^encrypted, BigPoly ^plain, EncryptionParameters ^parms, BigPoly ^secret_key, BigUInt ^result)
            {
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain");
                }
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms");
                }
                if (secret_key == nullptr)
                {
                    throw gcnew ArgumentNullException("secret_key");
                }
                if (result == nullptr)
                {
                    throw gcnew ArgumentNullException("result");
                }
                try
                {
                    seal::inherent_noise(encrypted->GetPolynomial(), plain->GetPolynomial(), parms->GetParameters(), secret_key->GetPolynomial(), result->GetUInt());
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
                    throw gcnew ArgumentNullException("parms");
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
                    throw gcnew ArgumentNullException("poly");
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
                    throw gcnew ArgumentNullException("poly");
                }
                if (modulus == nullptr)
                {
                    throw gcnew ArgumentNullException("modulus");
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
                    throw gcnew ArgumentNullException("parms");
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

            BigUInt ^Utilities::ExponentiateUInt(BigUInt ^operand, int exponent)
            {
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand");
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

            BigPoly ^Utilities::ExponentiatePoly(BigPoly ^operand, int exponent)
            {
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand");
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

            BigPoly ^Utilities::PolyEvalPoly(BigPoly ^poly_to_eval, BigPoly ^value)
            {
                if (poly_to_eval == nullptr)
                {
                    throw gcnew ArgumentNullException("poly_to_eval");
                }
                if (value == nullptr)
                {
                    throw gcnew ArgumentNullException("value");
                }
                try
                {
                    return gcnew BigPoly(seal::poly_eval_poly(poly_to_eval->GetPolynomial(), value->GetPolynomial()));
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