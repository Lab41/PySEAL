#include <cstddef>
#include "EncryptionParamsWrapper.h"
#include "EncryptorWrapper.h"
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
            Encryptor::Encryptor(EncryptionParameters ^parms, BigPolyArray ^publicKey) : encryptor_(nullptr)
            {
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                if (publicKey == nullptr)
                {
                    throw gcnew ArgumentNullException("publicKey cannot be null");
                }
                try
                {
                    encryptor_ = new seal::Encryptor(parms->GetParameters(), publicKey->GetArray());
                    GC::KeepAlive(parms);
                    GC::KeepAlive(publicKey);
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

            Encryptor::Encryptor(EncryptionParameters ^parms, BigPolyArray ^publicKey, MemoryPoolHandle ^pool) : encryptor_(nullptr)
            {
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                if (publicKey == nullptr)
                {
                    throw gcnew ArgumentNullException("publicKey cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    encryptor_ = new seal::Encryptor(parms->GetParameters(), publicKey->GetArray(), pool->GetHandle());
                    GC::KeepAlive(parms);
                    GC::KeepAlive(publicKey);
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

            Encryptor::Encryptor(Encryptor ^copy) : encryptor_(nullptr)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    encryptor_ = new seal::Encryptor(copy->GetEncryptor());
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

            BigPolyArray ^Encryptor::PublicKey::get()
            {
                if (encryptor_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Encryptor is disposed");
                }
                return gcnew BigPolyArray(encryptor_->public_key());
            }

            void Encryptor::Encrypt(BigPoly ^plain, BigPolyArray ^destination)
            {
                if (encryptor_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Encryptor is disposed");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    encryptor_->encrypt(plain->GetPolynomial(), destination->GetArray());
                    GC::KeepAlive(plain);
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

            BigPolyArray ^Encryptor::Encrypt(BigPoly ^plain)
            {
                if (encryptor_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Encryptor is disposed");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                try
                {
                    auto result = gcnew BigPolyArray(encryptor_->encrypt(plain->GetPolynomial()));
                    GC::KeepAlive(plain);
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

            seal::Encryptor &Encryptor::GetEncryptor()
            {
                if (encryptor_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Encryptor is disposed");
                }
                return *encryptor_;
            }

            Encryptor::~Encryptor()
            {
                this->!Encryptor();
            }

            Encryptor::!Encryptor()
            {
                if (encryptor_ != nullptr)
                {
                    delete encryptor_;
                    encryptor_ = nullptr;
                }
            }
        }
    }
}