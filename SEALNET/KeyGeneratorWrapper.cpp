#include <cstddef>
#include "EncryptionParamsWrapper.h"
#include "KeyGeneratorWrapper.h"
#include "BigPolyWrapper.h"
#include "BigUIntWrapper.h"
#include "Common.h"
#include "EvaluationKeysWrapper.h"

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
                    GC::KeepAlive(parms);
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

            KeyGenerator::KeyGenerator(EncryptionParameters ^parms, MemoryPoolHandle ^pool) : generator_(nullptr)
            {
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    generator_ = new seal::KeyGenerator(parms->GetParameters(), pool->GetHandle());
                    GC::KeepAlive(parms);
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

            KeyGenerator::KeyGenerator(EncryptionParameters ^parms, BigPoly ^secretKey, BigPolyArray ^publicKey, Microsoft::Research::SEAL::EvaluationKeys ^evaluationKeys)
            {
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                if (secretKey == nullptr)
                {
                    throw gcnew ArgumentNullException("secretKey cannot be null");
                }
                if (publicKey == nullptr)
                {
                    throw gcnew ArgumentNullException("publicKey cannot be null");
                }
                if (evaluationKeys == nullptr)
                {
                    throw gcnew ArgumentNullException("evaluationKeys cannot be null");
                }
                try
                {
                    generator_ = new seal::KeyGenerator(parms->GetParameters(), secretKey->GetPolynomial(), publicKey->GetArray(), evaluationKeys->GetKeys());
                    GC::KeepAlive(parms);
                    GC::KeepAlive(secretKey);
                    GC::KeepAlive(publicKey);
                    GC::KeepAlive(evaluationKeys);
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

            KeyGenerator::KeyGenerator(EncryptionParameters ^parms, BigPoly ^secretKey, BigPolyArray ^publicKey, Microsoft::Research::SEAL::EvaluationKeys ^evaluationKeys, MemoryPoolHandle ^pool)
            {
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                if (secretKey == nullptr)
                {
                    throw gcnew ArgumentNullException("secretKey cannot be null");
                }
                if (publicKey == nullptr)
                {
                    throw gcnew ArgumentNullException("publicKey cannot be null");
                }
                if (evaluationKeys == nullptr)
                {
                    throw gcnew ArgumentNullException("evaluationKeys cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    generator_ = new seal::KeyGenerator(parms->GetParameters(), secretKey->GetPolynomial(), publicKey->GetArray(), evaluationKeys->GetKeys(), pool->GetHandle());
                    GC::KeepAlive(parms);
                    GC::KeepAlive(secretKey);
                    GC::KeepAlive(publicKey);
                    GC::KeepAlive(evaluationKeys);
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

            void KeyGenerator::Generate(int evaluationKeysCount)
            {
                if (generator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("KeyGenerator is disposed");
                }
                try
                {
                    generator_->generate(evaluationKeysCount);
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

            void KeyGenerator::GenerateEvaluationKeys(int count)
            {
                if (generator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("KeyGenerator is disposed");
                }
                try
                {
                    generator_->generate_evaluation_keys(count);
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

            bool KeyGenerator::Generated::get()
            {
                if (generator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("KeyGenerator is disposed");
                }
                return bool(generator_->is_generated());
            }


            BigPolyArray ^KeyGenerator::PublicKey::get()
            {
                if (generator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("KeyGenerator is disposed");
                }
                return gcnew BigPolyArray(generator_->public_key());
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