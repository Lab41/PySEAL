#include <msclr\marshal_cppstd.h>
#include "sealnet/PlaintextWrapper.h"
#include "sealnet/Common.h"
#include "seal/util/common.h"

using namespace System;
using namespace System::IO;
using namespace std;
using namespace msclr::interop;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            Plaintext::Plaintext()
            {
                try
                {
                    plaintext_ = new seal::Plaintext();
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

            Plaintext::Plaintext(MemoryPoolHandle ^pool)
            {
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    plaintext_ = new seal::Plaintext(pool->GetHandle());
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

            Plaintext::Plaintext(int coeffCount, MemoryPoolHandle ^pool)
            {
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    plaintext_ = new seal::Plaintext(coeffCount, pool->GetHandle());
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

            Plaintext::Plaintext(int coeffCount)
            {
                try
                {
                    plaintext_ = new seal::Plaintext(coeffCount);
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

            Plaintext::Plaintext(int capacity, int coeffCount, MemoryPoolHandle ^pool)
            {
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    plaintext_ = new seal::Plaintext(capacity, coeffCount, pool->GetHandle());
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

            Plaintext::Plaintext(int capacity, int coeffCount)
            {
                try
                {
                    plaintext_ = new seal::Plaintext(capacity, coeffCount);
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

            Plaintext::Plaintext(BigPoly ^poly, MemoryPoolHandle ^pool)
            {
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    plaintext_ = new seal::Plaintext(poly->GetPolynomial(), pool->GetHandle());
                    GC::KeepAlive(poly);
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
            Plaintext::Plaintext(BigPoly ^poly)
            {
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    plaintext_ = new seal::Plaintext(poly->GetPolynomial());
                    GC::KeepAlive(poly);
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

            Plaintext::Plaintext(String ^hexPoly, MemoryPoolHandle ^pool)
            {
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                if (hexPoly == nullptr)
                {
                    throw gcnew ArgumentNullException("hexPoly cannot be null");
                }
                marshal_context ^context = gcnew marshal_context();
                try
                {
                    plaintext_ = new seal::Plaintext(context->marshal_as<const char*>(hexPoly), pool->GetHandle());
                    GC::KeepAlive(hexPoly);
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
                finally
                {
                    delete context;
                }
            }

            Plaintext::Plaintext(String ^hexPoly)
            {
                if (hexPoly == nullptr)
                {
                    throw gcnew ArgumentNullException("hexPoly cannot be null");
                }
                marshal_context ^context = gcnew marshal_context();
                try
                {
                    plaintext_ = new seal::Plaintext(context->marshal_as<const char*>(hexPoly));
                    GC::KeepAlive(hexPoly);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                finally
                {
                    delete context;
                }
            }

            Plaintext::Plaintext(Plaintext ^copy)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    plaintext_ = new seal::Plaintext(copy->GetPlaintext());
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

            void Plaintext::Reserve(int capacity, MemoryPoolHandle ^pool)
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    plaintext_->reserve(capacity, pool->GetHandle());
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

            void Plaintext::Reserve(int capacity)
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                try
                {
                    plaintext_->reserve(capacity);
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

            void Plaintext::Release()
            {
                try
                {
                    plaintext_->release();
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

            void Plaintext::Resize(int coeffCount, MemoryPoolHandle ^pool)
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    plaintext_->resize(coeffCount, pool->GetHandle());
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

            void Plaintext::Resize(int coeffCount)
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                try
                {
                    plaintext_->resize(coeffCount);
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

            void Plaintext::Set(Plaintext ^assign)
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                if (assign == nullptr)
                {
                    throw gcnew ArgumentNullException("assign cannot be null");
                }
                try
                {
                    *plaintext_ = *assign->plaintext_;
                    GC::KeepAlive(assign);
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

            void Plaintext::Set(BigPoly ^poly)
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    *plaintext_ = poly->GetPolynomial();
                    GC::KeepAlive(poly);
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

            void Plaintext::Set(String ^hexPoly)
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                if (hexPoly == nullptr)
                {
                    throw gcnew ArgumentNullException("hexPoly cannot be null");
                }
                marshal_context ^context = gcnew marshal_context();
                try
                {
                    *plaintext_ = context->marshal_as<const char*>(hexPoly);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                finally
                {
                    delete context;
                }
            }

            void Plaintext::Set(UInt64 constCoeff)
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                try
                {
                    *plaintext_ = constCoeff;
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

            void Plaintext::SetZero(int startCoeff, int length)
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                try
                {
                    plaintext_->set_zero(startCoeff, length);
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

            void Plaintext::SetZero(int startCoeff)
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                try
                {
                    plaintext_->set_zero(startCoeff);
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

            void Plaintext::SetZero()
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                try
                {
                    plaintext_->set_zero();
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

            UInt64 Plaintext::default::get(int index)
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                try
                {
                    return plaintext_->operator[](index);
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

            void Plaintext::default::set(int index, UInt64 value)
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                try
                {
                    plaintext_->operator[](index) = value;
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

            bool Plaintext::Equals(Plaintext ^compare)
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                if (compare == nullptr)
                {
                    return false;
                }
                try
                {
                    auto result = *plaintext_ == compare->GetPlaintext();
                    GC::KeepAlive(compare);
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

            bool Plaintext::Equals(Object ^compare)
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                Plaintext ^comparepoly = dynamic_cast<Plaintext^>(compare);
                if (comparepoly != nullptr)
                {
                    return Equals(comparepoly);
                }
                return false;
            }

            bool Plaintext::IsZero::get()
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                try
                {
                    return plaintext_->is_zero();
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

            int Plaintext::Capacity::get()
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                return plaintext_->capacity();
            }

            int Plaintext::CoeffCount::get()
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                return plaintext_->coeff_count();
            }

            int Plaintext::SignificantCoeffCount()
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                try
                {
                    return plaintext_->significant_coeff_count();
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

            String ^Plaintext::ToString()
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                return marshal_as<String^>(plaintext_->to_string());
            }

            int Plaintext::GetHashCode()
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                return ComputeArrayHashCode(plaintext_->pointer(), plaintext_->coeff_count());
            }

            void Plaintext::Save(Stream ^stream)
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                try
                {
                    int32_t coeff_count32 = static_cast<int32_t>(plaintext_->coeff_count());
                    Write(stream, reinterpret_cast<const char*>(&coeff_count32), sizeof(int32_t));
                    Write(stream, reinterpret_cast<const char*>(plaintext_->pointer()), 
                        plaintext_->coeff_count() * seal::util::bytes_per_uint64);
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

            void Plaintext::Load(Stream ^stream)
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                try
                {
                    int32_t coeff_count32 = 0;
                    Read(stream, reinterpret_cast<char*>(&coeff_count32), sizeof(int32_t));
                    plaintext_->resize(coeff_count32);
                    Read(stream, reinterpret_cast<char*>(plaintext_->pointer()), 
                        plaintext_->coeff_count() * seal::util::bytes_per_uint64);
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

            Plaintext::~Plaintext()
            {
                this->!Plaintext();
            }

            Plaintext::!Plaintext()
            {
                if (plaintext_ != nullptr)
                {
                    delete plaintext_;
                    plaintext_ = nullptr;
                }
            }

            Plaintext::Plaintext(const seal::Plaintext &plaintext) : plaintext_(nullptr)
            {
                try
                {
                    plaintext_ = new seal::Plaintext(plaintext);
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

            seal::Plaintext &Plaintext::GetPlaintext()
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                } 
                return *plaintext_;
            }
        }
    }
}