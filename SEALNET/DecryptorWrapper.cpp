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

            Decryptor::Decryptor(EncryptionParameters ^parms, BigPoly ^secretKey, int power) : decryptor_(nullptr)
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
                    decryptor_ = new seal::Decryptor(parms->GetParameters(), secretKey->GetPolynomial(), power);
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

            void Decryptor::Decrypt(BigPoly ^encrypted, BigPoly ^destination)
            {
                if (decryptor_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Decryptor is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    decryptor_->decrypt(encrypted->GetPolynomial(), destination->GetPolynomial());
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

            BigPoly ^Decryptor::Decrypt(BigPoly ^encrypted)
            {
                if (decryptor_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Decryptor is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                try
                {
                    return gcnew BigPoly(decryptor_->decrypt(encrypted->GetPolynomial()));
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