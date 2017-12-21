#include <cstddef>
#include "sealnet/EncryptionParamsWrapper.h"
#include "sealnet/EncryptorWrapper.h"
#include "sealnet/BigPolyWrapper.h"
#include "sealnet/BigUIntWrapper.h"
#include "sealnet/Common.h"

using namespace System;
using namespace std;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            Encryptor::Encryptor(SEALContext ^context, PublicKey ^publicKey)
            {
                if (context == nullptr)
                {
                    throw gcnew ArgumentNullException("context cannot be null");
                }
                if (publicKey == nullptr)
                {
                    throw gcnew ArgumentNullException("publicKey cannot be null");
                }
                try
                {
                    encryptor_ = new seal::Encryptor(context->GetContext(), publicKey->GetKey());
                    GC::KeepAlive(context);
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

            Encryptor::Encryptor(SEALContext ^context, PublicKey ^publicKey, MemoryPoolHandle ^pool)
            {
                if (context == nullptr)
                {
                    throw gcnew ArgumentNullException("context cannot be null");
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
                    encryptor_ = new seal::Encryptor(context->GetContext(), publicKey->GetKey(), pool->GetHandle());
                    GC::KeepAlive(context);
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

            Encryptor::Encryptor(Encryptor ^copy)
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

            void Encryptor::Encrypt(Plaintext ^plain, Ciphertext ^destination)
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
                    encryptor_->encrypt(plain->GetPlaintext(), destination->GetCiphertext());
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

            void Encryptor::Encrypt(Plaintext ^plain, Ciphertext ^destination, MemoryPoolHandle ^pool)
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
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    encryptor_->encrypt(plain->GetPlaintext(), destination->GetCiphertext(), pool->GetHandle());
                    GC::KeepAlive(plain);
                    GC::KeepAlive(destination);
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