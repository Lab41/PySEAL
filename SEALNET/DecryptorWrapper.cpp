#include <cstddef>
#include "EncryptionParamsWrapper.h"
#include "DecryptorWrapper.h"
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
            Decryptor::Decryptor(EncryptionParameters ^parms, BigPoly ^secretKey) : decryptor_(nullptr)
            {
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                if (secretKey == nullptr)
                {
                    throw gcnew ArgumentNullException("secretKey cannot be null");
                }
                try
                {
                    decryptor_ = new seal::Decryptor(parms->GetParameters(), secretKey->GetPolynomial());
                    GC::KeepAlive(parms);
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

            Decryptor::Decryptor(EncryptionParameters ^parms, BigPoly ^secretKey, MemoryPoolHandle ^pool) : decryptor_(nullptr)
            {
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
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
                    decryptor_ = new seal::Decryptor(parms->GetParameters(), secretKey->GetPolynomial(), pool->GetHandle());
                    GC::KeepAlive(parms);
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

            Decryptor::Decryptor(Decryptor ^copy) : decryptor_(nullptr)
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

            BigPoly ^Decryptor::SecretKey::get()
            {
                if (decryptor_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Decryptor is disposed");
                }
                return gcnew BigPoly(decryptor_->secret_key());
            }

            void Decryptor::Decrypt(BigPolyArray ^encrypted, BigPoly ^destination)
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
                    decryptor_->decrypt(encrypted->GetArray(), destination->GetPolynomial());
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

            BigPoly ^Decryptor::Decrypt(BigPolyArray ^encrypted)
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
                    auto result = gcnew BigPoly(decryptor_->decrypt(encrypted->GetArray())); 
                    GC::KeepAlive(encrypted);
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

            int Decryptor::InvariantNoiseBudget(BigPolyArray ^encrypted)
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
                    return decryptor_->invariant_noise_budget(encrypted->GetArray());
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

            int Decryptor::InherentNoiseBits(BigPolyArray ^encrypted)
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
                    return decryptor_->inherent_noise_bits(encrypted->GetArray());
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

            void Decryptor::InherentNoise(BigPolyArray ^encrypted, BigUInt ^destination)
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
                    decryptor_->inherent_noise(encrypted->GetArray(), destination->GetUInt());
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