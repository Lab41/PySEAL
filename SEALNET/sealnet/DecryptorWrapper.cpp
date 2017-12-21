#include <cstddef>
#include "sealnet/EncryptionParamsWrapper.h"
#include "sealnet/DecryptorWrapper.h"
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
            Decryptor::Decryptor(SEALContext ^context, SecretKey ^secretKey) : decryptor_(nullptr)
            {
                if (context == nullptr)
                {
                    throw gcnew ArgumentNullException("context cannot be null");
                }
                if (secretKey == nullptr)
                {
                    throw gcnew ArgumentNullException("secretKey cannot be null");
                }
                try
                {
                    decryptor_ = new seal::Decryptor(context->GetContext(), secretKey->GetKey());
                    GC::KeepAlive(context);
                    GC::KeepAlive(secretKey);
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

            Decryptor::Decryptor(SEALContext ^context, SecretKey ^secretKey, MemoryPoolHandle ^pool) : decryptor_(nullptr)
            {
                if (context == nullptr)
                {
                    throw gcnew ArgumentNullException("context cannot be null");
                }
                if (secretKey == nullptr)
                {
                    throw gcnew ArgumentNullException("secretKey cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    decryptor_ = new seal::Decryptor(context->GetContext(), secretKey->GetKey(), pool->GetHandle());
                    GC::KeepAlive(context);
                    GC::KeepAlive(secretKey);
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

            Decryptor::Decryptor(Decryptor ^copy)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    decryptor_ = new seal::Decryptor(copy->GetDecryptor());
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

            void Decryptor::Decrypt(Ciphertext ^encrypted, Plaintext ^destination)
            {
                if (decryptor_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Decryptor is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {   
                    decryptor_->decrypt(encrypted->GetCiphertext(), destination->GetPlaintext());
                    GC::KeepAlive(encrypted);
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

            void Decryptor::Decrypt(Ciphertext ^encrypted, Plaintext ^destination, MemoryPoolHandle ^pool)
            {
                if (decryptor_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Decryptor is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
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
                    decryptor_->decrypt(encrypted->GetCiphertext(), destination->GetPlaintext(), pool->GetHandle());
                    GC::KeepAlive(encrypted);
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

            int Decryptor::InvariantNoiseBudget(Ciphertext ^encrypted)
            {
                if (decryptor_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Decryptor is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                try
                {
                    return decryptor_->invariant_noise_budget(encrypted->GetCiphertext());
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

            int Decryptor::InvariantNoiseBudget(Ciphertext ^encrypted, MemoryPoolHandle ^pool)
            {
                if (decryptor_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Decryptor is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    return decryptor_->invariant_noise_budget(encrypted->GetCiphertext(), pool->GetHandle());
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

            seal::Decryptor &Decryptor::GetDecryptor()
            {
                if (decryptor_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Decryptor is disposed");
                }
                return *decryptor_;
            }

            Decryptor::~Decryptor()
            {
                this->!Decryptor();
            }

            Decryptor::!Decryptor()
            {
                if (decryptor_ != nullptr)
                {
                    delete decryptor_;
                    decryptor_ = nullptr;
                }
            }
        }
    }
}