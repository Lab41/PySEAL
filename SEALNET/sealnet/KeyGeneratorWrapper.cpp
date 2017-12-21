#include <cstddef>
#include "sealnet/KeyGeneratorWrapper.h"
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
            KeyGenerator::KeyGenerator(SEALContext ^context)
            {
                if (context == nullptr)
                {
                    throw gcnew ArgumentNullException("context cannot be null");
                }
                try
                {
                    generator_ = new seal::KeyGenerator(context->GetContext());
                    GC::KeepAlive(context);
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

            KeyGenerator::KeyGenerator(SEALContext ^context, MemoryPoolHandle ^pool)
            {
                if (context == nullptr)
                {
                    throw gcnew ArgumentNullException("context cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    generator_ = new seal::KeyGenerator(context->GetContext(), pool->GetHandle());
                    GC::KeepAlive(context);
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

            KeyGenerator::KeyGenerator(SEALContext ^context, 
                Microsoft::Research::SEAL::SecretKey ^secretKey, 
                Microsoft::Research::SEAL::PublicKey ^publicKey)
            {
                if (context == nullptr)
                {
                    throw gcnew ArgumentNullException("context cannot be null");
                }
                if (secretKey == nullptr)
                {
                    throw gcnew ArgumentNullException("secretKey cannot be null");
                }
                if (publicKey == nullptr)
                {
                    throw gcnew ArgumentNullException("publicKey cannot be null");
                }
                try
                {
                    generator_ = new seal::KeyGenerator(context->GetContext(), secretKey->GetKey(), publicKey->GetKey());
                    GC::KeepAlive(context);
                    GC::KeepAlive(secretKey);
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

            KeyGenerator::KeyGenerator(SEALContext ^context, 
                Microsoft::Research::SEAL::SecretKey ^secretKey, 
                Microsoft::Research::SEAL::PublicKey ^publicKey, 
                MemoryPoolHandle ^pool)
            {
                if (context == nullptr)
                {
                    throw gcnew ArgumentNullException("context cannot be null");
                }
                if (secretKey == nullptr)
                {
                    throw gcnew ArgumentNullException("secretKey cannot be null");
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
                    generator_ = new seal::KeyGenerator(context->GetContext(), secretKey->GetKey(), 
                        publicKey->GetKey(), pool->GetHandle());
                    GC::KeepAlive(context);
                    GC::KeepAlive(secretKey);
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

            void KeyGenerator::GenerateEvaluationKeys(int decompositionBitCount, int count, EvaluationKeys ^evaluationKeys)
            {
                if (generator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("KeyGenerator is disposed");
                }
                if (evaluationKeys == nullptr)
                {
                    throw gcnew ArgumentNullException("evaluationKeys cannot be null");
                }
                try
                {
                    generator_->generate_evaluation_keys(decompositionBitCount, count, evaluationKeys->GetKeys());
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

            void KeyGenerator::GenerateEvaluationKeys(int decompositionBitCount, EvaluationKeys ^evaluationKeys)
            {
                if (generator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("KeyGenerator is disposed");
                }
                if (evaluationKeys == nullptr)
                {
                    throw gcnew ArgumentNullException("evaluationKeys cannot be null");
                }
                try
                {
                    generator_->generate_evaluation_keys(decompositionBitCount, evaluationKeys->GetKeys());
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

            void KeyGenerator::GenerateGaloisKeys(int decompositionBitCount, GaloisKeys ^galoisKeys)
            {
                if (generator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("KeyGenerator is disposed");
                }
                if (galoisKeys == nullptr)
                {
                    throw gcnew ArgumentNullException("galoisKeys cannot be null");
                }
                try
                {
                    generator_->generate_galois_keys(decompositionBitCount, galoisKeys->GetKeys());
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

            Microsoft::Research::SEAL::PublicKey ^KeyGenerator::PublicKey::get()
            {
                if (generator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("KeyGenerator is disposed");
                }
                return gcnew Microsoft::Research::SEAL::PublicKey(generator_->public_key());
            }

            Microsoft::Research::SEAL::SecretKey ^KeyGenerator::SecretKey::get()
            {
                if (generator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("KeyGenerator is disposed");
                }
                return gcnew Microsoft::Research::SEAL::SecretKey(generator_->secret_key());
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