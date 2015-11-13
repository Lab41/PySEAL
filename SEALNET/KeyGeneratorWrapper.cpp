#include <cstddef>
#include "EncryptionParamsWrapper.h"
#include "KeyGeneratorWrapper.h"
#include "BigPolyWrapper.h"
#include "BigUIntWrapper.h"
#include "EvaluationKeysWrapper.h"
#include "Common.h"

using namespace System;
using namespace std;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            KeyGenerator::KeyGenerator(EncryptionParameters ^parms) : generator_(nullptr)
            {
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                try
                {
                    generator_ = new seal::KeyGenerator(parms->GetParameters());
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

            void KeyGenerator::Generate()
            {
                if (generator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("KeyGenerator is disposed");
                }
                try
                {
                    generator_->generate();
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

            void KeyGenerator::Generate(BigPoly ^secretKey)
            {
                Generate(secretKey, 1);
            }

            void KeyGenerator::Generate(BigPoly ^secretKey, int power)
            {
                if (generator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("KeyGenerator is disposed");
                }
                if (secretKey == nullptr)
                {
                    throw gcnew ArgumentNullException("secretKey cannot be null");
                }
                try
                {
                    generator_->generate(secretKey->GetPolynomial(), power);
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

            BigPoly ^KeyGenerator::PublicKey::get()
            {
                if (generator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("KeyGenerator is disposed");
                }
                return gcnew BigPoly(generator_->public_key());
            }

            BigPoly ^KeyGenerator::SecretKey::get()
            {
                if (generator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("KeyGenerator is disposed");
                }
                return gcnew BigPoly(generator_->secret_key());
            }

            EvaluationKeys ^KeyGenerator::EvaluationKeys::get()
            {
                if (generator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("KeyGenerator is disposed");
                }
                return gcnew Microsoft::Research::SEAL::EvaluationKeys(generator_->evaluation_keys());
            }

            seal::KeyGenerator &KeyGenerator::GetGenerator()
            {
                if (generator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("KeyGenerator is disposed");
                }
                return *generator_;
            }

            KeyGenerator::~KeyGenerator()
            {
                this->!KeyGenerator();
            }

            KeyGenerator::!KeyGenerator()
            {
                if (generator_ != nullptr)
                {
                    delete generator_;
                    generator_ = nullptr;
                }
            }
        }
    }
}