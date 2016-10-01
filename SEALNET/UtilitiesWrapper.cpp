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
            BigUInt ^Utilities::PolyInftyNorm(BigPoly ^poly)
            {
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(seal::poly_infty_norm(poly->GetPolynomial()));
                    GC::KeepAlive(poly);
                    return result;
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
                    auto result = gcnew BigUInt(seal::poly_infty_norm_coeffmod(poly->GetPolynomial(), modulus->GetUInt()));
                    GC::KeepAlive(poly);
                    GC::KeepAlive(modulus);
                    return result;
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

            void Utilities::ExponentiateUIntMod(BigUInt ^operand, BigUInt ^exponent, BigUInt ^modulus, BigUInt ^destination)
            {
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                if (exponent == nullptr)
                {
                    throw gcnew ArgumentNullException("exponent cannot be null");
                }
                if (modulus == nullptr)
                {
                    throw gcnew ArgumentNullException("modulus cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    seal::exponentiate_uint_mod(operand->GetUInt(), exponent->GetUInt(), modulus->GetUInt(), destination->GetUInt());
                    GC::KeepAlive(operand);
                    GC::KeepAlive(exponent);
                    GC::KeepAlive(modulus);
                    GC::KeepAlive(destination);
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

            BigUInt ^Utilities::ExponentiateUIntMod(BigUInt ^operand, BigUInt ^exponent, BigUInt ^modulus)
            {
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                if (exponent == nullptr)
                {
                    throw gcnew ArgumentNullException("exponent cannot be null");
                }
                if (modulus == nullptr)
                {
                    throw gcnew ArgumentNullException("modulus cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(seal::exponentiate_uint_mod(operand->GetUInt(), exponent->GetUInt(), modulus->GetUInt()));
                    GC::KeepAlive(operand);
                    GC::KeepAlive(exponent);
                    GC::KeepAlive(modulus);
                    return result;
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

            void Utilities::ExponentiatePolyPolymodCoeffmod(BigPoly ^operand, BigUInt ^exponent,
                BigPoly ^polyModulus, BigUInt ^coeffModulus, BigPoly ^destination)
            {
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                if (exponent == nullptr)
                {
                    throw gcnew ArgumentNullException("exponent cannot be null");
                }
                if (polyModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("polyModulus cannot be null");
                }
                if (coeffModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("coeffModulus cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    seal::exponentiate_poly_polymod_coeffmod(operand->GetPolynomial(), exponent->GetUInt(), polyModulus->GetPolynomial(), coeffModulus->GetUInt(), destination->GetPolynomial());
                    GC::KeepAlive(operand);
                    GC::KeepAlive(exponent);
                    GC::KeepAlive(polyModulus);
                    GC::KeepAlive(coeffModulus);
                    GC::KeepAlive(destination);
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

            BigPoly ^Utilities::ExponentiatePolyPolymodCoeffmod(BigPoly ^operand, BigUInt ^exponent,
                BigPoly ^polyModulus, BigUInt ^coeffModulus)
            {
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                if (exponent == nullptr)
                {
                    throw gcnew ArgumentNullException("exponent cannot be null");
                }
                if (polyModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("polyModulus cannot be null");
                }
                if (coeffModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("coeffModulus cannot be null");
                }
                try
                {
                    auto result = gcnew BigPoly(seal::exponentiate_poly_polymod_coeffmod(operand->GetPolynomial(), exponent->GetUInt(), polyModulus->GetPolynomial(), coeffModulus->GetUInt()));
                    GC::KeepAlive(operand);
                    GC::KeepAlive(exponent);
                    GC::KeepAlive(polyModulus);
                    GC::KeepAlive(coeffModulus);
                    return result;
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

            void Utilities::PolyEvalPoly(BigPoly ^polyToEvaluate, BigPoly ^polyToEvaluateAt, BigPoly ^destination)
            {
                if (polyToEvaluate == nullptr)
                {
                    throw gcnew ArgumentNullException("polyToEvaluate cannot be null");
                }
                if (polyToEvaluateAt == nullptr)
                {
                    throw gcnew ArgumentNullException("polyToEvaluateAt cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    seal::poly_eval_poly(polyToEvaluate->GetPolynomial(), polyToEvaluateAt->GetPolynomial(), destination->GetPolynomial());
                    GC::KeepAlive(polyToEvaluate);
                    GC::KeepAlive(polyToEvaluateAt);
                    GC::KeepAlive(destination);
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
                    auto result = gcnew BigPoly(seal::poly_eval_poly(polyToEvaluate->GetPolynomial(), polyToEvaluateAt->GetPolynomial()));
                    GC::KeepAlive(polyToEvaluate);
                    GC::KeepAlive(polyToEvaluateAt);
                    return result;
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

            void Utilities::PolyEvalPolyPolymodCoeffmod(BigPoly ^polyToEvaluate, BigPoly ^polyToEvaluateAt, 
                BigPoly ^polyModulus, BigUInt ^coeffModulus, BigPoly ^destination)
            {
                if (polyToEvaluate == nullptr)
                {
                    throw gcnew ArgumentNullException("polyToEvaluate cannot be null");
                }
                if (polyToEvaluateAt == nullptr)
                {
                    throw gcnew ArgumentNullException("polyToEvaluateAt cannot be null");
                }
                if (polyModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("polyModulus cannot be null");
                }
                if (coeffModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("coeffModulus cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    seal::poly_eval_poly_polymod_coeffmod(polyToEvaluate->GetPolynomial(), polyToEvaluateAt->GetPolynomial(), polyModulus->GetPolynomial(), coeffModulus->GetUInt(), destination->GetPolynomial());
                    GC::KeepAlive(polyToEvaluate);
                    GC::KeepAlive(polyToEvaluateAt);
                    GC::KeepAlive(polyModulus);
                    GC::KeepAlive(coeffModulus);
                    GC::KeepAlive(destination);
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

            BigPoly ^Utilities::PolyEvalPolyPolymodCoeffmod(BigPoly ^polyToEvaluate, BigPoly ^polyToEvaluateAt,
                BigPoly ^polyModulus, BigUInt ^coeffModulus)
            {
                if (polyToEvaluate == nullptr)
                {
                    throw gcnew ArgumentNullException("polyToEvaluate cannot be null");
                }
                if (polyToEvaluateAt == nullptr)
                {
                    throw gcnew ArgumentNullException("polyToEvaluateAt cannot be null");
                }
                if (polyModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("polyModulus cannot be null");
                }
                if (coeffModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("coeffModulus cannot be null");
                }
                try
                {
                    auto result = gcnew BigPoly(seal::poly_eval_poly_polymod_coeffmod(polyToEvaluate->GetPolynomial(), polyToEvaluateAt->GetPolynomial(), polyModulus->GetPolynomial(), coeffModulus->GetUInt()));
                    GC::KeepAlive(polyToEvaluate);
                    GC::KeepAlive(polyToEvaluateAt);
                    GC::KeepAlive(polyModulus);
                    GC::KeepAlive(coeffModulus);
                    return result;
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

            void Utilities::PolyEvalUIntMod(BigPoly ^polyToEvaluate, BigUInt ^value, BigUInt ^modulus, BigUInt ^destination)
            {
                if (polyToEvaluate == nullptr)
                {
                    throw gcnew ArgumentNullException("polyToEvaluate cannot be null");
                }
                if (value == nullptr)
                {
                    throw gcnew ArgumentNullException("value cannot be null");
                }
                if (modulus == nullptr)
                {
                    throw gcnew ArgumentNullException("modulus cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    seal::poly_eval_uint_mod(polyToEvaluate->GetPolynomial(), value->GetUInt(), modulus->GetUInt(), destination->GetUInt());
                    GC::KeepAlive(polyToEvaluate);
                    GC::KeepAlive(value);
                    GC::KeepAlive(modulus);
                    GC::KeepAlive(destination);
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

            BigUInt ^Utilities::PolyEvalUIntMod(BigPoly ^polyToEvaluate, BigUInt ^value, BigUInt ^modulus)
            {
                if (polyToEvaluate == nullptr)
                {
                    throw gcnew ArgumentNullException("polyToEvaluate cannot be null");
                }
                if (value == nullptr)
                {
                    throw gcnew ArgumentNullException("value cannot be null");
                }
                if (modulus == nullptr)
                {
                    throw gcnew ArgumentNullException("modulus cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(seal::poly_eval_uint_mod(polyToEvaluate->GetPolynomial(), value->GetUInt(), modulus->GetUInt()));
                    GC::KeepAlive(polyToEvaluate);
                    GC::KeepAlive(value);
                    GC::KeepAlive(modulus);
                    return result;
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
        }
    }
}