#include <cstddef>
#include "EncoderWrapper.h"
#include "BigPolyWrapper.h"
#include "BigUIntWrapper.h"
#include "Common.h"

using namespace System;
using namespace std;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            BinaryEncoder::BinaryEncoder(BigUInt ^plainModulus) : binaryEncoder_(nullptr)
            {
                if (plainModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("plainModulus cannot be null");
                }
                binaryEncoder_ = new seal::BinaryEncoder(plainModulus->GetUInt());
            }

            BinaryEncoder::~BinaryEncoder()
            {
                this->!BinaryEncoder();
            }

            BinaryEncoder::!BinaryEncoder()
            {
                if (binaryEncoder_ == nullptr)
                {
                    delete binaryEncoder_;
                    binaryEncoder_ = nullptr;
                }
            }

            BigUInt ^BinaryEncoder::PlainModulus::get()
            {
                if (binaryEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryEncoder is disposed");
                }
                return gcnew BigUInt(binaryEncoder_->plain_modulus());
            }

            BigPoly ^BinaryEncoder::Encode(UInt64 value)
            {
                if (binaryEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryEncoder is disposed");
                }
                try
                {
                    return gcnew BigPoly(binaryEncoder_->encode(value));
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

            void BinaryEncoder::Encode(UInt64 value, BigPoly ^destination)
            {
                if (binaryEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryEncoder is disposed");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    binaryEncoder_->encode(value, destination->GetPolynomial());
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

            void BinaryEncoder::Encode(UInt32 value, BigPoly ^destination)
            {
                if (binaryEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryEncoder is disposed");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    binaryEncoder_->encode(value, destination->GetPolynomial());
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

            BigPoly ^BinaryEncoder::Encode(BigUInt ^value)
            {
                if (binaryEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryEncoder is disposed");
                }
                if (value == nullptr)
                {
                    throw gcnew ArgumentNullException("value cannot be null");
                }
                try
                {
                    return gcnew BigPoly(binaryEncoder_->encode(value->GetUInt()));
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

            void BinaryEncoder::Encode(BigUInt ^value, BigPoly ^destination)
            {
                if (binaryEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryEncoder is disposed");
                }
                if (value == nullptr)
                {
                    throw gcnew ArgumentNullException("value cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    binaryEncoder_->encode(value->GetUInt(), destination->GetPolynomial());
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

            BigPoly ^BinaryEncoder::Encode(Int64 value)
            {
                if (binaryEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryEncoder is disposed");
                }
                try
                {
                    return gcnew BigPoly(binaryEncoder_->encode(value));
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

            void BinaryEncoder::Encode(Int64 value, BigPoly ^destination)
            {
                if (binaryEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryEncoder is disposed");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    binaryEncoder_->encode(value, destination->GetPolynomial());
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

            void BinaryEncoder::Encode(Int32 value, BigPoly ^destination)
            {
                if (binaryEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryEncoder is disposed");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    binaryEncoder_->encode(value, destination->GetPolynomial());
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

            UInt64 BinaryEncoder::DecodeUInt64(BigPoly ^poly)
            {
                if (binaryEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryEncoder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    return binaryEncoder_->decode_uint64(poly->GetPolynomial());
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

            UInt32 BinaryEncoder::DecodeUInt32(BigPoly ^poly)
            {
                if (binaryEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryEncoder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    return binaryEncoder_->decode_uint32(poly->GetPolynomial());
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

            Int64 BinaryEncoder::DecodeInt64(BigPoly ^poly)
            {
                if (binaryEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryEncoder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    return binaryEncoder_->decode_int64(poly->GetPolynomial());
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

            Int32 BinaryEncoder::DecodeInt32(BigPoly ^poly)
            {
                if (binaryEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryEncoder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    return binaryEncoder_->decode_int32(poly->GetPolynomial());
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

            BigUInt ^BinaryEncoder::DecodeBigUInt(BigPoly ^poly)
            {
                if (binaryEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryEncoder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    return gcnew BigUInt(binaryEncoder_->decode_biguint(poly->GetPolynomial()));
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

            void BinaryEncoder::DecodeBigUInt(BigPoly ^poly, BigUInt ^destination)
            {
                if (binaryEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryEncoder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    binaryEncoder_->decode_biguint(poly->GetPolynomial(), destination->GetUInt());
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

            BigPoly ^BinaryEncoder::Encode(System::Int32 value)
            {
                if (binaryEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryEncoder is disposed");
                }
                try
                {
                    return gcnew BigPoly(binaryEncoder_->encode(value));
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

            BigPoly ^BinaryEncoder::Encode(System::UInt32 value)
            {
                if (binaryEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryEncoder is disposed");
                }
                try
                {
                    return gcnew BigPoly(binaryEncoder_->encode(value));
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

            seal::BinaryEncoder &BinaryEncoder::GetEncoder()
            {
                if (binaryEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryEncoder is disposed");
                }
                return *binaryEncoder_;
            }

            BalancedEncoder::BalancedEncoder(BigUInt ^plainModulus, UInt64 base) : balancedEncoder_(nullptr)
            {
                if (plainModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("plainModulus cannot be null");
                }
                balancedEncoder_ = new seal::BalancedEncoder(plainModulus->GetUInt(), base);
            }

            BalancedEncoder::BalancedEncoder(BigUInt ^plainModulus) : balancedEncoder_(nullptr)
            {
                if (plainModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("plainModulus cannot be null");
                }
                balancedEncoder_ = new seal::BalancedEncoder(plainModulus->GetUInt());
            }

            BalancedEncoder::~BalancedEncoder()
            {
                this->!BalancedEncoder();
            }

            BalancedEncoder::!BalancedEncoder()
            {
                if (balancedEncoder_ == nullptr)
                {
                    delete balancedEncoder_;
                    balancedEncoder_ = nullptr;
                }
            }

            BigUInt ^BalancedEncoder::PlainModulus::get()
            {
                if (balancedEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedEncoder is disposed");
                }
                return gcnew BigUInt(balancedEncoder_->plain_modulus());
            }

            UInt64 BalancedEncoder::Base::get()
            {
                if (balancedEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedEncoder is disposed");
                }
                return balancedEncoder_->base();
            }

            BigPoly ^BalancedEncoder::Encode(UInt64 value)
            {
                if (balancedEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedEncoder is disposed");
                }
                try
                {
                    return gcnew BigPoly(balancedEncoder_->encode(value));
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

            void BalancedEncoder::Encode(UInt64 value, BigPoly ^destination)
            {
                if (balancedEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedEncoder is disposed");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    balancedEncoder_->encode(value, destination->GetPolynomial());
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

            void BalancedEncoder::Encode(UInt32 value, BigPoly ^destination)
            {
                if (balancedEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedEncoder is disposed");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    balancedEncoder_->encode(value, destination->GetPolynomial());
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

            BigPoly ^BalancedEncoder::Encode(BigUInt ^value)
            {
                if (balancedEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedEncoder is disposed");
                }
                if (value == nullptr)
                {
                    throw gcnew ArgumentNullException("value cannot be null");
                }
                try
                {
                    return gcnew BigPoly(balancedEncoder_->encode(value->GetUInt()));
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

            void BalancedEncoder::Encode(BigUInt ^value, BigPoly ^destination)
            {
                if (balancedEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedEncoder is disposed");
                }
                if (value == nullptr)
                {
                    throw gcnew ArgumentNullException("value cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    balancedEncoder_->encode(value->GetUInt(), destination->GetPolynomial());
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

            BigPoly ^BalancedEncoder::Encode(Int64 value)
            {
                if (balancedEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedEncoder is disposed");
                }
                try
                {
                    return gcnew BigPoly(balancedEncoder_->encode(value));
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

            void BalancedEncoder::Encode(Int64 value, BigPoly ^destination)
            {
                if (balancedEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedEncoder is disposed");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    balancedEncoder_->encode(value, destination->GetPolynomial());
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

            void BalancedEncoder::Encode(Int32 value, BigPoly ^destination)
            {
                if (balancedEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedEncoder is disposed");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    balancedEncoder_->encode(value, destination->GetPolynomial());
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

            UInt64 BalancedEncoder::DecodeUInt64(BigPoly ^poly)
            {
                if (balancedEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedEncoder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    return balancedEncoder_->decode_uint64(poly->GetPolynomial());
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

            UInt32 BalancedEncoder::DecodeUInt32(BigPoly ^poly)
            {
                if (balancedEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedEncoder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    return balancedEncoder_->decode_uint32(poly->GetPolynomial());
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

            Int64 BalancedEncoder::DecodeInt64(BigPoly ^poly)
            {
                if (balancedEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedEncoder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    return balancedEncoder_->decode_int64(poly->GetPolynomial());
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

            Int32 BalancedEncoder::DecodeInt32(BigPoly ^poly)
            {
                if (balancedEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedEncoder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    return balancedEncoder_->decode_int32(poly->GetPolynomial());
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

            BigUInt ^BalancedEncoder::DecodeBigUInt(BigPoly ^poly)
            {
                if (balancedEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedEncoder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    return gcnew BigUInt(balancedEncoder_->decode_biguint(poly->GetPolynomial()));
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

            void BalancedEncoder::DecodeBigUInt(BigPoly ^poly, BigUInt ^destination)
            {
                if (balancedEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedEncoder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    balancedEncoder_->decode_biguint(poly->GetPolynomial(), destination->GetUInt());
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

            BigPoly ^BalancedEncoder::Encode(System::Int32 value)
            {
                if (balancedEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedEncoder is disposed");
                }
                try
                {
                    return gcnew BigPoly(balancedEncoder_->encode(value));
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

            BigPoly ^BalancedEncoder::Encode(System::UInt32 value)
            {
                if (balancedEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedEncoder is disposed");
                }
                try
                {
                    return gcnew BigPoly(balancedEncoder_->encode(value));
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

            seal::BalancedEncoder &BalancedEncoder::GetEncoder()
            {
                if (balancedEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedEncoder is disposed");
                }
                return *balancedEncoder_;
            }

            BinaryFractionalEncoder::BinaryFractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, int fractionCoeffCount) : fractionalEncoder_(nullptr)
            {
                if (plainModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("plainModulus cannot be null");
                }
                if (polyModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("polyModulus cannot be null");
                }
                try
                {
                    fractionalEncoder_ = new seal::BinaryFractionalEncoder(plainModulus->GetUInt(), polyModulus->GetPolynomial(), integerCoeffCount, fractionCoeffCount);
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

            BinaryFractionalEncoder::~BinaryFractionalEncoder()
            {
                this->!BinaryFractionalEncoder();
            }

            BinaryFractionalEncoder::!BinaryFractionalEncoder()
            {
                if (fractionalEncoder_ != nullptr)
                {
                    delete fractionalEncoder_;
                    fractionalEncoder_ = nullptr;
                }
            }

            BigPoly ^BinaryFractionalEncoder::Encode(double value)
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryFractionalEncoder is disposed");
                }
                try
                {
                    return gcnew BigPoly(fractionalEncoder_->encode(value));
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

            double BinaryFractionalEncoder::Decode(BigPoly ^poly)
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryFractionalEncoder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("value cannot be null");
                }
                try
                {
                    return fractionalEncoder_->decode(poly->GetPolynomial());
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

            BigUInt ^BinaryFractionalEncoder::PlainModulus::get()
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryFractionalEncoder is disposed");
                }
                return gcnew BigUInt(fractionalEncoder_->plain_modulus());
            }

            seal::BinaryFractionalEncoder &BinaryFractionalEncoder::GetEncoder()
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryFractionalEncoder is disposed");
                }
                return *fractionalEncoder_;
            }

            BalancedFractionalEncoder::BalancedFractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, int fractionCoeffCount, UInt64 base) : fractionalEncoder_(nullptr)
            {
                if (plainModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("plainModulus cannot be null");
                }
                if (polyModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("polyModulus cannot be null");
                }
                try
                {
                    fractionalEncoder_ = new seal::BalancedFractionalEncoder(plainModulus->GetUInt(), polyModulus->GetPolynomial(), integerCoeffCount, fractionCoeffCount, base);
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

            BalancedFractionalEncoder::BalancedFractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, int fractionCoeffCount) : fractionalEncoder_(nullptr)
            {
                if (plainModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("plainModulus cannot be null");
                }
                if (polyModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("polyModulus cannot be null");
                }
                try
                {
                    fractionalEncoder_ = new seal::BalancedFractionalEncoder(plainModulus->GetUInt(), polyModulus->GetPolynomial(), integerCoeffCount, fractionCoeffCount);
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

            BalancedFractionalEncoder::~BalancedFractionalEncoder()
            {
                this->!BalancedFractionalEncoder();
            }

            BalancedFractionalEncoder::!BalancedFractionalEncoder()
            {
                if (fractionalEncoder_ != nullptr)
                {
                    delete fractionalEncoder_;
                    fractionalEncoder_ = nullptr;
                }
            }

            BigPoly ^BalancedFractionalEncoder::Encode(double value)
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedFractionalEncoder is disposed");
                }
                try
                {
                    return gcnew BigPoly(fractionalEncoder_->encode(value));
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

            double BalancedFractionalEncoder::Decode(BigPoly ^poly)
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedFractionalEncoder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    return fractionalEncoder_->decode(poly->GetPolynomial());
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

            BigUInt ^BalancedFractionalEncoder::PlainModulus::get()
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedFractionalEncoder is disposed");
                }
                return gcnew BigUInt(fractionalEncoder_->plain_modulus());
            }

            UInt64 BalancedFractionalEncoder::Base::get()
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedFractionalEncoder is disposed");
                }
                return fractionalEncoder_->base();
            }

            seal::BalancedFractionalEncoder &BalancedFractionalEncoder::GetEncoder()
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedFractionalEncoder is disposed");
                }
                return *fractionalEncoder_;
            }
        }
    }
}