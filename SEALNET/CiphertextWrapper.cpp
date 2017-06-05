#include "CiphertextWrapper.h"
#include "Common.h"

using namespace System;
using namespace std;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            Ciphertext::operator Ciphertext ^(BigPolyArray ^polyArray)
            {
                if (polyArray == nullptr)
                {
                    throw gcnew ArgumentNullException("polyArray cannot be null");
                }
                try
                {
                    auto result = gcnew Ciphertext(polyArray->GetArray());
                    GC::KeepAlive(polyArray);
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

            Ciphertext::Ciphertext(Ciphertext ^copy)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    ciphertext_ = new seal::Ciphertext(copy->GetCiphertext());
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

            void Ciphertext::Set(Ciphertext ^assign)
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                if (assign == nullptr)
                {
                    throw gcnew ArgumentNullException("assign cannot be null");
                }
                try
                {
                    *ciphertext_ = *assign->ciphertext_;
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

            void Ciphertext::Set(BigPolyArray ^polyArray)
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                if (polyArray == nullptr)
                {
                    throw gcnew ArgumentNullException("polyArray cannot be null");
                }
                try
                {
                    *ciphertext_ = polyArray->GetArray();
                    GC::KeepAlive(polyArray);
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

            Ciphertext::operator BigPolyArray^ (Ciphertext ^ciphertext)
            {
                if (ciphertext == nullptr)
                {
                    throw gcnew ArgumentNullException("ciphertext cannot be null");
                }
                try
                {
                    auto result = gcnew BigPolyArray(*ciphertext->ciphertext_);
                    GC::KeepAlive(ciphertext);
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
                throw gcnew Exception("Unknown exception");
            }

            int Ciphertext::Size::get()
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                return ciphertext_->size();
            }

            void Ciphertext::Save(System::IO::Stream ^stream)
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                auto poly_array = gcnew BigPolyArray(*ciphertext_);
                poly_array->Save(stream);
            }

            void Ciphertext::Load(System::IO::Stream ^stream)
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                auto poly_array = gcnew BigPolyArray();
                poly_array->Load(stream);
                *ciphertext_ = poly_array->GetArray();
            }

            Ciphertext::~Ciphertext()
            {
                this->!Ciphertext();
            }

            Ciphertext::!Ciphertext()
            {
                if (ciphertext_ != nullptr)
                {
                    delete ciphertext_;
                    ciphertext_ = nullptr;
                }
            }

            Ciphertext::Ciphertext(const seal::Ciphertext &ciphertext) : ciphertext_(nullptr)
            {
                try
                {
                    ciphertext_ = new seal::Ciphertext(ciphertext);
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

            seal::Ciphertext &Ciphertext::GetCiphertext()
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                return *ciphertext_;
            }
        }
    }
}