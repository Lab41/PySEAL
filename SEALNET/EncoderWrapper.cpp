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
                GC::KeepAlive(plainModulus);
            }

            BinaryEncoder::BinaryEncoder(BigUInt ^plainModulus, MemoryPoolHandle ^pool) : binaryEncoder_(nullptr)
            {
                if (plainModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("plainModulus cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                binaryEncoder_ = new seal::BinaryEncoder(plainModulus->GetUInt(), pool->GetHandle());
                GC::KeepAlive(plainModulus);
                GC::KeepAlive(pool);
            }

            BinaryEncoder::BinaryEncoder(BinaryEncoder ^copy) : binaryEncoder_(nullptr)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    binaryEncoder_ = new seal::BinaryEncoder(copy->GetEncoder());
                    GC::KeepAlive(copy);
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

            UInt64 BinaryEncoder::Base::get()
            {
                if (binaryEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryEncoder is disposed");
                }
                return binaryEncoder_->base();
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
                    auto result = gcnew BigPoly(binaryEncoder_->encode(value->GetUInt()));
                    GC::KeepAlive(value);
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
                    GC::KeepAlive(value);
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
                    auto result = binaryEncoder_->decode_uint64(poly->GetPolynomial());
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
                    auto result = binaryEncoder_->decode_uint32(poly->GetPolynomial());
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
                    auto result = binaryEncoder_->decode_int64(poly->GetPolynomial());
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
                    auto result = binaryEncoder_->decode_int32(poly->GetPolynomial());
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
                    auto result = gcnew BigUInt(binaryEncoder_->decode_biguint(poly->GetPolynomial()));
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
                    GC::KeepAlive(poly);
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
                GC::KeepAlive(plainModulus);
            }

            BalancedEncoder::BalancedEncoder(BigUInt ^plainModulus, UInt64 base, MemoryPoolHandle ^pool) : balancedEncoder_(nullptr)
            {
                if (plainModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("plainModulus cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                balancedEncoder_ = new seal::BalancedEncoder(plainModulus->GetUInt(), base, pool->GetHandle());
                GC::KeepAlive(plainModulus);
                GC::KeepAlive(pool);
            }

            BalancedEncoder::BalancedEncoder(BigUInt ^plainModulus) : balancedEncoder_(nullptr)
            {
                if (plainModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("plainModulus cannot be null");
                }
                balancedEncoder_ = new seal::BalancedEncoder(plainModulus->GetUInt());
                GC::KeepAlive(plainModulus);
            }

            BalancedEncoder::BalancedEncoder(BigUInt ^plainModulus, MemoryPoolHandle ^pool) : balancedEncoder_(nullptr)
            {
                if (plainModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("plainModulus cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                balancedEncoder_ = new seal::BalancedEncoder(plainModulus->GetUInt(), 3, pool->GetHandle());
                GC::KeepAlive(plainModulus);
                GC::KeepAlive(pool);
            }

            BalancedEncoder::BalancedEncoder(BalancedEncoder ^copy) : balancedEncoder_(nullptr)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    balancedEncoder_ = new seal::BalancedEncoder(copy->GetEncoder());
                    GC::KeepAlive(copy);
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
                    auto result = gcnew BigPoly(balancedEncoder_->encode(value->GetUInt()));
                    GC::KeepAlive(value);
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
                    GC::KeepAlive(value);
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
                    auto result = balancedEncoder_->decode_uint64(poly->GetPolynomial());
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
                    auto result = balancedEncoder_->decode_uint32(poly->GetPolynomial());
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
                    auto result = balancedEncoder_->decode_int64(poly->GetPolynomial());
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
                    auto result = balancedEncoder_->decode_int32(poly->GetPolynomial());
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
                    auto result = gcnew BigUInt(balancedEncoder_->decode_biguint(poly->GetPolynomial()));
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
                    GC::KeepAlive(poly);
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
                    GC::KeepAlive(plainModulus);
                    GC::KeepAlive(polyModulus);
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

            BinaryFractionalEncoder::BinaryFractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, int fractionCoeffCount, MemoryPoolHandle ^pool) : fractionalEncoder_(nullptr)
            {
                if (plainModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("plainModulus cannot be null");
                }
                if (polyModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("polyModulus cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    fractionalEncoder_ = new seal::BinaryFractionalEncoder(plainModulus->GetUInt(), polyModulus->GetPolynomial(), integerCoeffCount, fractionCoeffCount, pool->GetHandle());
                    GC::KeepAlive(plainModulus);
                    GC::KeepAlive(polyModulus);
                    GC::KeepAlive(pool);
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

            BinaryFractionalEncoder::BinaryFractionalEncoder(BinaryFractionalEncoder ^copy) : fractionalEncoder_(nullptr)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    fractionalEncoder_ = new seal::BinaryFractionalEncoder(copy->GetEncoder());
                    GC::KeepAlive(copy);
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
                    auto result = fractionalEncoder_->decode(poly->GetPolynomial());
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

            BigUInt ^BinaryFractionalEncoder::PlainModulus::get()
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryFractionalEncoder is disposed");
                }
                return gcnew BigUInt(fractionalEncoder_->plain_modulus());
            }

            BigPoly ^BinaryFractionalEncoder::PolyModulus::get()
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryFractionalEncoder is disposed");
                }
                return gcnew BigPoly(fractionalEncoder_->poly_modulus());
            }

            int BinaryFractionalEncoder::FractionCoeffCount::get()
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryFractionalEncoder is disposed");
                }
                return fractionalEncoder_->fraction_coeff_count();
            }

            int BinaryFractionalEncoder::IntegerCoeffCount::get()
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryFractionalEncoder is disposed");
                }
                return fractionalEncoder_->integer_coeff_count();
            }

            UInt64 BinaryFractionalEncoder::Base::get()
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BinaryFractionalEncoder is disposed");
                }
                return fractionalEncoder_->base();
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
                    GC::KeepAlive(plainModulus);
                    GC::KeepAlive(polyModulus);
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

            BalancedFractionalEncoder::BalancedFractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, int fractionCoeffCount, UInt64 base, MemoryPoolHandle ^pool) : fractionalEncoder_(nullptr)
            {
                if (plainModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("plainModulus cannot be null");
                }
                if (polyModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("polyModulus cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    fractionalEncoder_ = new seal::BalancedFractionalEncoder(plainModulus->GetUInt(), polyModulus->GetPolynomial(), integerCoeffCount, fractionCoeffCount, base, pool->GetHandle());
                    GC::KeepAlive(plainModulus);
                    GC::KeepAlive(polyModulus);
                    GC::KeepAlive(pool);
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

            BalancedFractionalEncoder::BalancedFractionalEncoder(BalancedFractionalEncoder ^copy) : fractionalEncoder_(nullptr)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    fractionalEncoder_ = new seal::BalancedFractionalEncoder(copy->GetEncoder());
                    GC::KeepAlive(copy);
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
                    GC::KeepAlive(plainModulus);
                    GC::KeepAlive(polyModulus);
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

            BalancedFractionalEncoder::BalancedFractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, int fractionCoeffCount, MemoryPoolHandle ^pool) : fractionalEncoder_(nullptr)
            {
                if (plainModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("plainModulus cannot be null");
                }
                if (polyModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("polyModulus cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    fractionalEncoder_ = new seal::BalancedFractionalEncoder(plainModulus->GetUInt(), polyModulus->GetPolynomial(), integerCoeffCount, fractionCoeffCount, 3, pool->GetHandle());
                    GC::KeepAlive(plainModulus);
                    GC::KeepAlive(polyModulus);
                    GC::KeepAlive(pool);
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
                   auto result = fractionalEncoder_->decode(poly->GetPolynomial());
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

            BigUInt ^BalancedFractionalEncoder::PlainModulus::get()
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedFractionalEncoder is disposed");
                }
                return gcnew BigUInt(fractionalEncoder_->plain_modulus());
            }

            BigPoly ^BalancedFractionalEncoder::PolyModulus::get()
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedFractionalEncoder is disposed");
                }
                return gcnew BigPoly(fractionalEncoder_->poly_modulus());
            }

            int BalancedFractionalEncoder::FractionCoeffCount::get()
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedFractionalEncoder is disposed");
                }
                return fractionalEncoder_->fraction_coeff_count();
            }

            int BalancedFractionalEncoder::IntegerCoeffCount::get()
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BalancedFractionalEncoder is disposed");
                }
                return fractionalEncoder_->integer_coeff_count();
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

            IntegerEncoder::IntegerEncoder(BigUInt ^plainModulus, UInt64 base) : integerEncoder_(nullptr)
            {
                if (plainModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("plainModulus cannot be null");
                }
                integerEncoder_ = new seal::IntegerEncoder(plainModulus->GetUInt(), base);
                GC::KeepAlive(plainModulus);
            }

            IntegerEncoder::IntegerEncoder(BigUInt ^plainModulus, UInt64 base, MemoryPoolHandle ^pool) : integerEncoder_(nullptr)
            {
                if (plainModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("plainModulus cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                integerEncoder_ = new seal::IntegerEncoder(plainModulus->GetUInt(), base, pool->GetHandle());
                GC::KeepAlive(plainModulus);
                GC::KeepAlive(pool);
            }

            IntegerEncoder::IntegerEncoder(BigUInt ^plainModulus) : integerEncoder_(nullptr)
            {
                if (plainModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("plainModulus cannot be null");
                }
                integerEncoder_ = new seal::IntegerEncoder(plainModulus->GetUInt());
                GC::KeepAlive(plainModulus);
            }

            IntegerEncoder::IntegerEncoder(BigUInt ^plainModulus, MemoryPoolHandle ^pool) : integerEncoder_(nullptr)
            {
                if (plainModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("plainModulus cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                integerEncoder_ = new seal::IntegerEncoder(plainModulus->GetUInt(), 2, pool->GetHandle());
                GC::KeepAlive(plainModulus);
                GC::KeepAlive(pool);
            }

            IntegerEncoder::IntegerEncoder(IntegerEncoder ^copy) : integerEncoder_(nullptr)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    integerEncoder_ = new seal::IntegerEncoder(copy->GetEncoder());
                    GC::KeepAlive(copy);
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

            IntegerEncoder::~IntegerEncoder()
            {
                this->!IntegerEncoder();
            }

            IntegerEncoder::!IntegerEncoder()
            {
                if (integerEncoder_ == nullptr)
                {
                    delete integerEncoder_;
                    integerEncoder_ = nullptr;
                }
            }

            BigUInt ^IntegerEncoder::PlainModulus::get()
            {
                if (integerEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("IntegerEncoder is disposed");
                }
                return gcnew BigUInt(integerEncoder_->plain_modulus());
            }

            UInt64 IntegerEncoder::Base::get()
            {
                if (integerEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("IntegerEncoder is disposed");
                }
                return integerEncoder_->base();
            }

            BigPoly ^IntegerEncoder::Encode(UInt64 value)
            {
                if (integerEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("IntegerEncoder is disposed");
                }
                try
                {
                    return gcnew BigPoly(integerEncoder_->encode(value));
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

            void IntegerEncoder::Encode(UInt64 value, BigPoly ^destination)
            {
                if (integerEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("IntegerEncoder is disposed");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    integerEncoder_->encode(value, destination->GetPolynomial());
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
            }

            void IntegerEncoder::Encode(UInt32 value, BigPoly ^destination)
            {
                if (integerEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("IntegerEncoder is disposed");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    integerEncoder_->encode(value, destination->GetPolynomial());
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
            }

            BigPoly ^IntegerEncoder::Encode(BigUInt ^value)
            {
                if (integerEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("IntegerEncoder is disposed");
                }
                if (value == nullptr)
                {
                    throw gcnew ArgumentNullException("value cannot be null");
                }
                try
                {
                    auto result = gcnew BigPoly(integerEncoder_->encode(value->GetUInt()));
                    GC::KeepAlive(value);
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

            void IntegerEncoder::Encode(BigUInt ^value, BigPoly ^destination)
            {
                if (integerEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("IntegerEncoder is disposed");
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
                    integerEncoder_->encode(value->GetUInt(), destination->GetPolynomial());
                    GC::KeepAlive(value);
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
            }

            BigPoly ^IntegerEncoder::Encode(Int64 value)
            {
                if (integerEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("IntegerEncoder is disposed");
                }
                try
                {
                    return gcnew BigPoly(integerEncoder_->encode(value));
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

            void IntegerEncoder::Encode(Int64 value, BigPoly ^destination)
            {
                if (integerEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("IntegerEncoder is disposed");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    integerEncoder_->encode(value, destination->GetPolynomial());
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
            }

            void IntegerEncoder::Encode(Int32 value, BigPoly ^destination)
            {
                if (integerEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("IntegerEncoder is disposed");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    integerEncoder_->encode(value, destination->GetPolynomial());
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
            }

            UInt64 IntegerEncoder::DecodeUInt64(BigPoly ^poly)
            {
                if (integerEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("IntegerEncoder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    auto result = integerEncoder_->decode_uint64(poly->GetPolynomial());
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

            UInt32 IntegerEncoder::DecodeUInt32(BigPoly ^poly)
            {
                if (integerEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("IntegerEncoder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    auto result = integerEncoder_->decode_uint32(poly->GetPolynomial());
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

            Int64 IntegerEncoder::DecodeInt64(BigPoly ^poly)
            {
                if (integerEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("IntegerEncoder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    auto result = integerEncoder_->decode_int64(poly->GetPolynomial());
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

            Int32 IntegerEncoder::DecodeInt32(BigPoly ^poly)
            {
                if (integerEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("IntegerEncoder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    auto result = integerEncoder_->decode_int32(poly->GetPolynomial());
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

            BigUInt ^IntegerEncoder::DecodeBigUInt(BigPoly ^poly)
            {
                if (integerEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("IntegerEncoder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(integerEncoder_->decode_biguint(poly->GetPolynomial()));
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

            void IntegerEncoder::DecodeBigUInt(BigPoly ^poly, BigUInt ^destination)
            {
                if (integerEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("IntegerEncoder is disposed");
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
                    integerEncoder_->decode_biguint(poly->GetPolynomial(), destination->GetUInt());
                    GC::KeepAlive(poly);
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
            }

            BigPoly ^IntegerEncoder::Encode(System::Int32 value)
            {
                if (integerEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("IntegerEncoder is disposed");
                }
                try
                {
                    return gcnew BigPoly(integerEncoder_->encode(value));
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

            BigPoly ^IntegerEncoder::Encode(System::UInt32 value)
            {
                if (integerEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("IntegerEncoder is disposed");
                }
                try
                {
                    return gcnew BigPoly(integerEncoder_->encode(value));
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

            seal::IntegerEncoder &IntegerEncoder::GetEncoder()
            {
                if (integerEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("IntegerEncoder is disposed");
                }
                return *integerEncoder_;
            }

            FractionalEncoder::FractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, int fractionCoeffCount, UInt64 base) : fractionalEncoder_(nullptr)
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
                    fractionalEncoder_ = new seal::FractionalEncoder(plainModulus->GetUInt(), polyModulus->GetPolynomial(), integerCoeffCount, fractionCoeffCount, base);
                    GC::KeepAlive(plainModulus);
                    GC::KeepAlive(polyModulus);
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

            FractionalEncoder::FractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, int fractionCoeffCount, UInt64 base, MemoryPoolHandle ^pool) : fractionalEncoder_(nullptr)
            {
                if (plainModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("plainModulus cannot be null");
                }
                if (polyModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("polyModulus cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    fractionalEncoder_ = new seal::FractionalEncoder(plainModulus->GetUInt(), polyModulus->GetPolynomial(), integerCoeffCount, fractionCoeffCount, base, pool->GetHandle());
                    GC::KeepAlive(plainModulus);
                    GC::KeepAlive(polyModulus);
                    GC::KeepAlive(pool);
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

            FractionalEncoder::FractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, int fractionCoeffCount) : fractionalEncoder_(nullptr)
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
                    fractionalEncoder_ = new seal::FractionalEncoder(plainModulus->GetUInt(), polyModulus->GetPolynomial(), integerCoeffCount, fractionCoeffCount);
                    GC::KeepAlive(plainModulus);
                    GC::KeepAlive(polyModulus);
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

            FractionalEncoder::FractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, int fractionCoeffCount, MemoryPoolHandle ^pool) : fractionalEncoder_(nullptr)
            {
                if (plainModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("plainModulus cannot be null");
                }
                if (polyModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("polyModulus cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    fractionalEncoder_ = new seal::FractionalEncoder(plainModulus->GetUInt(), polyModulus->GetPolynomial(), integerCoeffCount, fractionCoeffCount, 2, pool->GetHandle());
                    GC::KeepAlive(plainModulus);
                    GC::KeepAlive(polyModulus);
                    GC::KeepAlive(pool);
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

            FractionalEncoder::FractionalEncoder(FractionalEncoder ^copy) : fractionalEncoder_(nullptr)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    fractionalEncoder_ = new seal::FractionalEncoder(copy->GetEncoder());
                    GC::KeepAlive(copy);
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

            FractionalEncoder::~FractionalEncoder()
            {
                this->!FractionalEncoder();
            }

            FractionalEncoder::!FractionalEncoder()
            {
                if (fractionalEncoder_ != nullptr)
                {
                    delete fractionalEncoder_;
                    fractionalEncoder_ = nullptr;
                }
            }

            BigPoly ^FractionalEncoder::Encode(double value)
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("FractionalEncoder is disposed");
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

            double FractionalEncoder::Decode(BigPoly ^poly)
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("FractionalEncoder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    auto result = fractionalEncoder_->decode(poly->GetPolynomial());
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

            BigUInt ^FractionalEncoder::PlainModulus::get()
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("FractionalEncoder is disposed");
                }
                return gcnew BigUInt(fractionalEncoder_->plain_modulus());
            }

            BigPoly ^FractionalEncoder::PolyModulus::get()
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("FractionalEncoder is disposed");
                }
                return gcnew BigPoly(fractionalEncoder_->poly_modulus());
            }

            int FractionalEncoder::FractionCoeffCount::get()
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("FractionalEncoder is disposed");
                }
                return fractionalEncoder_->fraction_coeff_count();
            }

            int FractionalEncoder::IntegerCoeffCount::get()
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("FractionalEncoder is disposed");
                }
                return fractionalEncoder_->integer_coeff_count();
            }

            UInt64 FractionalEncoder::Base::get()
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("FractionalEncoder is disposed");
                }
                return fractionalEncoder_->base();
            }

            seal::FractionalEncoder &FractionalEncoder::GetEncoder()
            {
                if (fractionalEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("FractionalEncoder is disposed");
                }
                return *fractionalEncoder_;
            }
        }
    }
}