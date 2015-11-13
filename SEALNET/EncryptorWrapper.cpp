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
            Encryptor::Encryptor(EncryptionParameters ^parms, BigPoly ^publicKey) : encryptor_(nullptr)
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
                    encryptor_ = new seal::Encryptor(parms->GetParameters(), publicKey->GetPolynomial());
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

            BigPoly ^Encryptor::PublicKey::get()
            {
                if (encryptor_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Encryptor is disposed");
                }
                return gcnew BigPoly(encryptor_->public_key());
            }

            void Encryptor::Encrypt(BigPoly ^plain, BigPoly ^destination)
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
                    encryptor_->encrypt(plain->GetPolynomial(), destination->GetPolynomial());
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

            BigPoly ^Encryptor::Encrypt(BigPoly ^plain)
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
                    return gcnew BigPoly(encryptor_->encrypt(plain->GetPolynomial()));
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