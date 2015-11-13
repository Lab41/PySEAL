#include "Common.h"
#include <stdexcept>
#include "BigPolyArithWrapper.h"
#include "BigUIntWrapper.h"
#include "BigPolyWrapper.h"
#include "bigpolyarith.h"

using namespace System;
using namespace std;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            BigPolyArithmetic::BigPolyArithmetic() : arith_(nullptr)
            {
                try
                {
                    arith_ = new seal::BigPolyArith();
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

            void BigPolyArithmetic::Negate(BigPoly ^poly, BigUInt ^coeffMod, BigPoly ^result)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                if (coeffMod == nullptr)
                {
                    throw gcnew ArgumentNullException("coeffMod cannot be null");
                }
                if (result == nullptr)
                {
                    throw gcnew ArgumentNullException("result cannot be null");
                }
                try
                {
                    arith_->negate(poly->GetPolynomial(), coeffMod->GetUInt(), result->GetPolynomial());
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

            BigPoly ^BigPolyArithmetic::Negate(BigPoly ^poly, BigUInt ^coeffMod)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                if (coeffMod == nullptr)
                {
                    throw gcnew ArgumentNullException("coeffMod cannot be null");
                }
                try
                {
                    return gcnew BigPoly(arith_->negate(poly->GetPolynomial(), coeffMod->GetUInt()));
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

            void BigPolyArithmetic::Negate(BigPoly ^poly, BigPoly ^result)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                if (result == nullptr)
                {
                    throw gcnew ArgumentNullException("result cannot be null");
                }
                try
                {
                    arith_->negate(poly->GetPolynomial(), result->GetPolynomial());
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

            BigPoly ^BigPolyArithmetic::Negate(BigPoly ^poly)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    return gcnew BigPoly(arith_->negate(poly->GetPolynomial()));
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

            void BigPolyArithmetic::Add(BigPoly ^poly1, BigPoly ^poly2, BigUInt ^coeffMod, BigPoly ^result)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly1 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly1 cannot be null");
                }
                if (poly2 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly2 cannot be null");
                }
                if (coeffMod == nullptr)
                {
                    throw gcnew ArgumentNullException("coeffMod cannot be null");
                }
                if (result == nullptr)
                {
                    throw gcnew ArgumentNullException("result cannot be null");
                }
                try
                {
                    arith_->add(poly1->GetPolynomial(), poly2->GetPolynomial(), coeffMod->GetUInt(), result->GetPolynomial());
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

            BigPoly ^BigPolyArithmetic::Add(BigPoly ^poly1, BigPoly ^poly2, BigUInt ^coeffMod)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly1 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly1 cannot be null");
                }
                if (poly2 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly2 cannot be null");
                }
                if (coeffMod == nullptr)
                {
                    throw gcnew ArgumentNullException("coeffMod cannot be null");
                }
                try
                {
                    return gcnew BigPoly(arith_->add(poly1->GetPolynomial(), poly2->GetPolynomial(), coeffMod->GetUInt()));
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

            void BigPolyArithmetic::Sub(BigPoly ^poly1, BigPoly ^poly2, BigUInt ^coeffMod, BigPoly ^result)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly1 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly1 cannot be null");
                }
                if (poly2 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly2 cannot be null");
                }
                if (coeffMod == nullptr)
                {
                    throw gcnew ArgumentNullException("coeffMod cannot be null");
                }
                if (result == nullptr)
                {
                    throw gcnew ArgumentNullException("result cannot be null");
                }
                try
                {
                    arith_->sub(poly1->GetPolynomial(), poly2->GetPolynomial(), coeffMod->GetUInt(), result->GetPolynomial());
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

            BigPoly ^BigPolyArithmetic::Sub(BigPoly ^poly1, BigPoly ^poly2, BigUInt ^coeffMod)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly1 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly1 cannot be null");
                }
                if (poly2 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly2 cannot be null");
                }
                if (coeffMod == nullptr)
                {
                    throw gcnew ArgumentNullException("coeffMod cannot be null");
                }
                try
                {
                    return gcnew BigPoly(arith_->sub(poly1->GetPolynomial(), poly2->GetPolynomial(), coeffMod->GetUInt()));
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

            void BigPolyArithmetic::Add(BigPoly ^poly1, BigPoly ^poly2, BigPoly ^result)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly1 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly1 cannot be null");
                }
                if (poly2 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly2 cannot be null");
                }
                if (result == nullptr)
                {
                    throw gcnew ArgumentNullException("result cannot be null");
                }
                try
                {
                    arith_->add(poly1->GetPolynomial(), poly2->GetPolynomial(), result->GetPolynomial());
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

            BigPoly ^BigPolyArithmetic::Add(BigPoly ^poly1, BigPoly ^poly2)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly1 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly1 cannot be null");
                }
                if (poly2 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly2 cannot be null");
                }
                try
                {
                    return gcnew BigPoly(arith_->add(poly1->GetPolynomial(), poly2->GetPolynomial()));
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

            void BigPolyArithmetic::Sub(BigPoly ^poly1, BigPoly ^poly2, BigPoly ^result)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly1 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly1 cannot be null");
                }
                if (poly2 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly2 cannot be null");
                }
                if (result == nullptr)
                {
                    throw gcnew ArgumentNullException("result cannot be null");
                }
                try
                {
                    arith_->sub(poly1->GetPolynomial(), poly2->GetPolynomial(), result->GetPolynomial());
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

            BigPoly ^BigPolyArithmetic::Sub(BigPoly ^poly1, BigPoly ^poly2)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly1 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly1 cannot be null");
                }
                if (poly2 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly2 cannot be null");
                }
                try
                {
                    return gcnew BigPoly(arith_->sub(poly1->GetPolynomial(), poly2->GetPolynomial()));
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

            void BigPolyArithmetic::Multiply(BigPoly ^poly1, BigPoly ^poly2, BigPoly ^polyMod, BigUInt ^coeffMod, BigPoly ^result)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly1 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly1 cannot be null");
                }
                if (poly2 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly2 cannot be null");
                }
                if (polyMod == nullptr)
                {
                    throw gcnew ArgumentNullException("polyMod cannot be null");
                }
                if (coeffMod == nullptr)
                {
                    throw gcnew ArgumentNullException("coeffMod cannot be null");
                }
                if (result == nullptr)
                {
                    throw gcnew ArgumentNullException("result cannot be null");
                }
                try
                {
                    arith_->multiply(poly1->GetPolynomial(), poly2->GetPolynomial(), polyMod->GetPolynomial(), coeffMod->GetUInt(), result->GetPolynomial());
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

            BigPoly ^BigPolyArithmetic::Multiply(BigPoly ^poly1, BigPoly ^poly2, BigPoly ^polyMod, BigUInt ^coeffMod)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly1 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly1 cannot be null");
                }
                if (poly2 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly2 cannot be null");
                }
                if (polyMod == nullptr)
                {
                    throw gcnew ArgumentNullException("polyMod cannot be null");
                }
                if (coeffMod == nullptr)
                {
                    throw gcnew ArgumentNullException("coeffMod cannot be null");
                }
                try
                {
                    return gcnew BigPoly(arith_->multiply(poly1->GetPolynomial(), poly2->GetPolynomial(), polyMod->GetPolynomial(), coeffMod->GetUInt()));
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

            void BigPolyArithmetic::Multiply(BigPoly ^poly1, BigPoly ^poly2, BigUInt ^coeffMod, BigPoly ^result)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly1 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly1 cannot be null");
                }
                if (poly2 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly2 cannot be null");
                }
                if (coeffMod == nullptr)
                {
                    throw gcnew ArgumentNullException("coeffMod cannot be null");
                }
                if (result == nullptr)
                {
                    throw gcnew ArgumentNullException("result cannot be null");
                }
                try
                {
                    arith_->multiply(poly1->GetPolynomial(), poly2->GetPolynomial(), coeffMod->GetUInt(), result->GetPolynomial());
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

            BigPoly ^BigPolyArithmetic::Multiply(BigPoly ^poly1, BigPoly ^poly2, BigUInt ^coeffMod)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly1 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly1 cannot be null");
                }
                if (poly2 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly2 cannot be null");
                }
                if (coeffMod == nullptr)
                {
                    throw gcnew ArgumentNullException("coeffMod cannot be null");
                }
                try
                {
                    return gcnew BigPoly(arith_->multiply(poly1->GetPolynomial(), poly2->GetPolynomial(), coeffMod->GetUInt()));
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

            void BigPolyArithmetic::Multiply(BigPoly ^poly1, BigPoly ^poly2, BigPoly ^result)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly1 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly1 cannot be null");
                }
                if (poly2 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly2 cannot be null");
                }
                if (result == nullptr)
                {
                    throw gcnew ArgumentNullException("result cannot be null");
                }
                try
                {
                    arith_->multiply(poly1->GetPolynomial(), poly2->GetPolynomial(), result->GetPolynomial());
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

            BigPoly ^BigPolyArithmetic::Multiply(BigPoly ^poly1, BigPoly ^poly2)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly1 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly1 cannot be null");
                }
                if (poly2 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly2 cannot be null");
                }
                try
                {
                    return gcnew BigPoly(arith_->multiply(poly1->GetPolynomial(), poly2->GetPolynomial()));
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

            void BigPolyArithmetic::Multiply(BigPoly ^poly1, BigUInt ^uint2, BigUInt ^coeffMod, BigPoly ^result)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly1 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly1 cannot be null");
                }
                if (uint2 == nullptr)
                {
                    throw gcnew ArgumentNullException("uint2 cannot be null");
                }
                if (coeffMod == nullptr)
                {
                    throw gcnew ArgumentNullException("coeffMod cannot be null");
                }
                if (result == nullptr)
                {
                    throw gcnew ArgumentNullException("result cannot be null");
                }
                try
                {
                    arith_->multiply(poly1->GetPolynomial(), uint2->GetUInt(), coeffMod->GetUInt(), result->GetPolynomial());
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

            BigPoly ^BigPolyArithmetic::Multiply(BigPoly ^poly1, BigUInt ^uint2, BigUInt ^coeffMod)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly1 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly1 cannot be null");
                }
                if (uint2 == nullptr)
                {
                    throw gcnew ArgumentNullException("uint2 cannot be null");
                }
                if (coeffMod == nullptr)
                {
                    throw gcnew ArgumentNullException("coeffMod cannot be null");
                }
                try
                {
                    return gcnew BigPoly(arith_->multiply(poly1->GetPolynomial(), uint2->GetUInt(), coeffMod->GetUInt()));
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

            void BigPolyArithmetic::Multiply(BigPoly ^poly1, BigUInt ^uint2, BigPoly ^result)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly1 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly1 cannot be null");
                }
                if (uint2 == nullptr)
                {
                    throw gcnew ArgumentNullException("uint2 cannot be null");
                }
                if (result == nullptr)
                {
                    throw gcnew ArgumentNullException("result cannot be null");
                }
                try
                {
                    arith_->multiply(poly1->GetPolynomial(), uint2->GetUInt(), result->GetPolynomial());
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

            BigPoly ^BigPolyArithmetic::Multiply(BigPoly ^poly1, BigUInt ^uint2)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly1 == nullptr)
                {
                    throw gcnew ArgumentNullException("poly1 cannot be null");
                }
                if (uint2 == nullptr)
                {
                    throw gcnew ArgumentNullException("uint2 cannot be null");
                }
                try
                {
                    return gcnew BigPoly(arith_->multiply(poly1->GetPolynomial(), uint2->GetUInt()));
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

            void BigPolyArithmetic::Divide(BigPoly ^numerator, BigPoly ^denominator, BigUInt ^coeffMod, BigPoly ^quotient, BigPoly ^remainder)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (numerator == nullptr)
                {
                    throw gcnew ArgumentNullException("numerator cannot be null");
                }
                if (denominator == nullptr)
                {
                    throw gcnew ArgumentNullException("denominator cannot be null");
                }
                if (coeffMod == nullptr)
                {
                    throw gcnew ArgumentNullException("coeffMod cannot be null");
                }
                if (quotient == nullptr)
                {
                    throw gcnew ArgumentNullException("quotient cannot be null");
                }
                if (remainder == nullptr)
                {
                    throw gcnew ArgumentNullException("remainder cannot be null");
                }
                try
                {
                    arith_->divide(numerator->GetPolynomial(), denominator->GetPolynomial(), coeffMod->GetUInt(), quotient->GetPolynomial(), remainder->GetPolynomial());
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

            void BigPolyArithmetic::Modulo(BigPoly ^numerator, BigPoly ^denominator, BigUInt ^coeffMod, BigPoly ^remainder)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (numerator == nullptr)
                {
                    throw gcnew ArgumentNullException("numerator cannot be null");
                }
                if (denominator == nullptr)
                {
                    throw gcnew ArgumentNullException("denominator cannot be null");
                }
                if (coeffMod == nullptr)
                {
                    throw gcnew ArgumentNullException("coeffMod cannot be null");
                }
                if (remainder == nullptr)
                {
                    throw gcnew ArgumentNullException("remainder cannot be null");
                }
                try
                {
                    arith_->modulo(numerator->GetPolynomial(), denominator->GetPolynomial(), coeffMod->GetUInt(), remainder->GetPolynomial());
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

            BigPoly ^BigPolyArithmetic::Modulo(BigPoly ^numerator, BigPoly ^denominator, BigUInt ^coeffMod)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (numerator == nullptr)
                {
                    throw gcnew ArgumentNullException("numerator cannot be null");
                }
                if (denominator == nullptr)
                {
                    throw gcnew ArgumentNullException("denominator cannot be null");
                }
                if (coeffMod == nullptr)
                {
                    throw gcnew ArgumentNullException("coeffMod cannot be null");
                }
                try
                {
                    return gcnew BigPoly(arith_->modulo(numerator->GetPolynomial(), denominator->GetPolynomial(), coeffMod->GetUInt()));
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

            bool BigPolyArithmetic::TryInvert(BigPoly ^poly, BigPoly ^polyMod, BigUInt ^coeffMod, BigPoly ^result)
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                if (polyMod == nullptr)
                {
                    throw gcnew ArgumentNullException("polyMod cannot be null");
                }
                if (coeffMod == nullptr)
                {
                    throw gcnew ArgumentNullException("coeffMod cannot be null");
                }
                if (result == nullptr)
                {
                    throw gcnew ArgumentNullException("result cannot be null");
                }
                try
                {
                    return arith_->try_invert(poly->GetPolynomial(), polyMod->GetPolynomial(), coeffMod->GetUInt(), result->GetPolynomial());
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

            seal::BigPolyArith &BigPolyArithmetic::GetBigPolyArith()
            {
                if (arith_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArithmetic is disposed");
                }
                return *arith_;
            }

            BigPolyArithmetic::~BigPolyArithmetic()
            {
                this->!BigPolyArithmetic();
            }

            BigPolyArithmetic::!BigPolyArithmetic()
            {
                if (arith_ != nullptr)
                {
                    delete arith_;
                    arith_ = nullptr;
                }
            }
        }
    }
}