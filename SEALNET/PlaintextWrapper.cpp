#include <msclr\marshal_cppstd.h>
#include "PlaintextWrapper.h"
#include "Common.h"

using namespace System;
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

            Plaintext::operator Plaintext ^(BigPoly ^poly)
            {
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    auto result = gcnew Plaintext(poly->GetPolynomial());
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

            void Plaintext::Set(System::String ^hexPoly)
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

            Plaintext::operator BigPoly^ (Plaintext ^plaintext)
            {
                if (plaintext == nullptr)
                {
                    throw gcnew ArgumentNullException("plaintext cannot be null");
                }
                try
                {
                    auto result = gcnew BigPoly(*plaintext->plaintext_);
                    GC::KeepAlive(plaintext);
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

            void Plaintext::Save(System::IO::Stream ^stream)
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                auto poly = gcnew BigPoly(*plaintext_);
                poly->Save(stream);
            }

            void Plaintext::Load(System::IO::Stream ^stream)
            {
                if (plaintext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Plaintext is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                auto poly = gcnew BigPoly();
                poly->Load(stream);
                *plaintext_ = poly->GetPolynomial();
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