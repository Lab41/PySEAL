#include <cstddef>
#include <vector>
#include "sealnet/EvaluatorWrapper.h"
#include "sealnet/Common.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace std;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            Evaluator::Evaluator(SEALContext ^context)
            {
                if (context == nullptr)
                {
                    throw gcnew ArgumentNullException("context cannot be null");
                }
                try
                {
                    evaluator_ = new seal::Evaluator(context->GetContext());
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

            Evaluator::Evaluator(SEALContext ^context, MemoryPoolHandle ^pool)
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
                    evaluator_ = new seal::Evaluator(context->GetContext(), pool->GetHandle());
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

            Evaluator::Evaluator(Evaluator ^copy)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    evaluator_ = new seal::Evaluator(copy->GetEvaluator());
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

            void Evaluator::Negate(Ciphertext ^encrypted)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                try
                {
                    evaluator_->negate(encrypted->GetCiphertext());
                    GC::KeepAlive(encrypted);
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

            void Evaluator::Negate(Ciphertext ^encrypted, Ciphertext ^destination)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
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
                    evaluator_->negate(encrypted->GetCiphertext(), destination->GetCiphertext());
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

            void Evaluator::Add(Ciphertext ^encrypted1, Ciphertext ^encrypted2)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted1 == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted1 cannot be null");
                }
                if (encrypted2 == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted2 cannot be null");
                }
                try
                {
                    evaluator_->add(encrypted1->GetCiphertext(), encrypted2->GetCiphertext());
                    GC::KeepAlive(encrypted1);
                    GC::KeepAlive(encrypted2);
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

            void Evaluator::Add(Ciphertext ^encrypted1, Ciphertext ^encrypted2, Ciphertext ^destination)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted1 == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted1 cannot be null");
                }
                if (encrypted2 == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted2 cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    evaluator_->add(encrypted1->GetCiphertext(), encrypted2->GetCiphertext(), destination->GetCiphertext());
                    GC::KeepAlive(encrypted1);
                    GC::KeepAlive(encrypted2);
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

            void Evaluator::AddMany(List<Ciphertext^> ^encrypteds, Ciphertext ^destination)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypteds == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypteds cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    vector<seal::Ciphertext> v_encrypteds;
                    for each (Ciphertext ^poly in encrypteds)
                    {
                        if (poly == nullptr)
                        {
                            throw gcnew ArgumentNullException("encrypteds cannot be null");
                        }
                        v_encrypteds.emplace_back(poly->GetCiphertext());
                        GC::KeepAlive(poly);
                    }

                    evaluator_->add_many(v_encrypteds, destination->GetCiphertext());
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

            void Evaluator::Sub(Ciphertext ^encrypted1, Ciphertext ^encrypted2)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted1 == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted1 cannot be null");
                }
                if (encrypted2 == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted2 cannot be null");
                }
                try
                {
                    evaluator_->sub(encrypted1->GetCiphertext(), encrypted2->GetCiphertext());
                    GC::KeepAlive(encrypted1);
                    GC::KeepAlive(encrypted2);
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

            void Evaluator::Sub(Ciphertext ^encrypted1, Ciphertext ^encrypted2, Ciphertext ^destination)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted1 == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted1 cannot be null");
                }
                if (encrypted2 == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted2 cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    evaluator_->sub(encrypted1->GetCiphertext(), encrypted2->GetCiphertext(), destination->GetCiphertext());
                    GC::KeepAlive(encrypted1);
                    GC::KeepAlive(encrypted2);
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

            void Evaluator::Multiply(Ciphertext ^encrypted1, Ciphertext ^encrypted2)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted1 == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted1 cannot be null");
                }
                if (encrypted2 == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted2 cannot be null");
                }
                try
                {
                    evaluator_->multiply(encrypted1->GetCiphertext(), encrypted2->GetCiphertext());
                    GC::KeepAlive(encrypted1);
                    GC::KeepAlive(encrypted2);
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

            void Evaluator::Multiply(Ciphertext ^encrypted1, Ciphertext ^encrypted2, Ciphertext ^destination)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted1 == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted1 cannot be null");
                }
                if (encrypted2 == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted2 cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    evaluator_->multiply(encrypted1->GetCiphertext(), encrypted2->GetCiphertext(), destination->GetCiphertext());
                    GC::KeepAlive(encrypted1);
                    GC::KeepAlive(encrypted2);
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

            void Evaluator::Multiply(Ciphertext ^encrypted1, Ciphertext ^encrypted2, Ciphertext ^destination, MemoryPoolHandle ^pool)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted1 == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted1 cannot be null");
                }
                if (encrypted2 == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted2 cannot be null");
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
                    evaluator_->multiply(encrypted1->GetCiphertext(), encrypted2->GetCiphertext(), 
                        destination->GetCiphertext(), pool->GetHandle());
                    GC::KeepAlive(encrypted1);
                    GC::KeepAlive(encrypted2);
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

            void Evaluator::Multiply(Ciphertext ^encrypted1, Ciphertext ^encrypted2, MemoryPoolHandle ^pool)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted1 == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted1 cannot be null");
                }
                if (encrypted2 == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted2 cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    evaluator_->multiply(encrypted1->GetCiphertext(), encrypted2->GetCiphertext(), pool->GetHandle());
                    GC::KeepAlive(encrypted1);
                    GC::KeepAlive(encrypted2);
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

            void Evaluator::Square(Ciphertext ^encrypted)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                try
                {
                    evaluator_->square(encrypted->GetCiphertext());
                    GC::KeepAlive(encrypted);
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

            void Evaluator::Square(Ciphertext ^encrypted, Ciphertext ^destination)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
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
                    evaluator_->square(encrypted->GetCiphertext(), destination->GetCiphertext());
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

            void Evaluator::Square(Ciphertext ^encrypted, Ciphertext ^destination, MemoryPoolHandle ^pool)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
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
                    evaluator_->square(encrypted->GetCiphertext(), destination->GetCiphertext(), pool->GetHandle());
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

            void Evaluator::Square(Ciphertext ^encrypted, MemoryPoolHandle ^pool)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
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
                    evaluator_->square(encrypted->GetCiphertext(), pool->GetHandle());
                    GC::KeepAlive(encrypted);
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

            void Evaluator::Relinearize(Ciphertext ^encrypted, EvaluationKeys ^evaluationKeys, MemoryPoolHandle ^pool)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
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
                    evaluator_->relinearize(encrypted->GetCiphertext(), evaluationKeys->GetKeys(), pool->GetHandle());
                    GC::KeepAlive(encrypted);
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

            void Evaluator::Relinearize(Ciphertext ^encrypted, EvaluationKeys ^evaluationKeys)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                if (evaluationKeys == nullptr)
                {
                    throw gcnew ArgumentNullException("evaluationKeys cannot be null");
                }
                try
                {
                    evaluator_->relinearize(encrypted->GetCiphertext(), evaluationKeys->GetKeys());
                    GC::KeepAlive(encrypted);
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

            void Evaluator::Relinearize(Ciphertext ^encrypted, EvaluationKeys ^evaluationKeys, Ciphertext ^destination)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                if (evaluationKeys == nullptr)
                {
                    throw gcnew ArgumentNullException("evaluationKeys cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    evaluator_->relinearize(encrypted->GetCiphertext(), evaluationKeys->GetKeys(), destination->GetCiphertext());
                    GC::KeepAlive(encrypted);
                    GC::KeepAlive(evaluationKeys);
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

            void Evaluator::Relinearize(Ciphertext ^encrypted, EvaluationKeys ^evaluationKeys, Ciphertext ^destination, 
                MemoryPoolHandle ^pool)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                if (evaluationKeys == nullptr)
                {
                    throw gcnew ArgumentNullException("evaluationKeys cannot be null");
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
                    evaluator_->relinearize(encrypted->GetCiphertext(), evaluationKeys->GetKeys(), 
                        destination->GetCiphertext(), pool->GetHandle());
                    GC::KeepAlive(encrypted);
                    GC::KeepAlive(evaluationKeys);
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

            void Evaluator::AddPlain(Ciphertext ^encrypted, Plaintext ^plain)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                try
                {
                    evaluator_->add_plain(encrypted->GetCiphertext(), plain->GetPlaintext());
                    GC::KeepAlive(encrypted);
                    GC::KeepAlive(plain);
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

            void Evaluator::AddPlain(Ciphertext ^encrypted, Plaintext ^plain, Ciphertext ^destination)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
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
                    evaluator_->add_plain(encrypted->GetCiphertext(), plain->GetPlaintext(), destination->GetCiphertext());
                    GC::KeepAlive(encrypted);
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

            void Evaluator::SubPlain(Ciphertext ^encrypted, Plaintext ^plain)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                try
                {
                    evaluator_->sub_plain(encrypted->GetCiphertext(), plain->GetPlaintext());
                    GC::KeepAlive(encrypted);
                    GC::KeepAlive(plain);
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

            void Evaluator::SubPlain(Ciphertext ^encrypted, Plaintext ^plain, Ciphertext ^destination)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
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
                    evaluator_->sub_plain(encrypted->GetCiphertext(), plain->GetPlaintext(), destination->GetCiphertext());
                    GC::KeepAlive(encrypted);
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

            void Evaluator::MultiplyPlain(Ciphertext ^encrypted, Plaintext ^plain)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                try
                {
                    evaluator_->multiply_plain(encrypted->GetCiphertext(), plain->GetPlaintext());
                    GC::KeepAlive(encrypted);
                    GC::KeepAlive(plain);
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

            void Evaluator::MultiplyPlain(Ciphertext ^encrypted, Plaintext ^plain, Ciphertext ^destination)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
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
                    evaluator_->multiply_plain(encrypted->GetCiphertext(), plain->GetPlaintext(), destination->GetCiphertext());
                    GC::KeepAlive(encrypted);
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

            void Evaluator::MultiplyPlain(Ciphertext ^encrypted, Plaintext ^plain, Ciphertext ^destination, MemoryPoolHandle ^pool)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
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
                    evaluator_->multiply_plain(encrypted->GetCiphertext(), plain->GetPlaintext(), 
                        destination->GetCiphertext(), pool->GetHandle());
                    GC::KeepAlive(encrypted);
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

            void Evaluator::MultiplyPlain(Ciphertext ^encrypted, Plaintext ^plain, MemoryPoolHandle ^pool)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    evaluator_->multiply_plain(encrypted->GetCiphertext(), plain->GetPlaintext(), pool->GetHandle());
                    GC::KeepAlive(encrypted);
                    GC::KeepAlive(plain);
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

            void Evaluator::MultiplyMany(List<Ciphertext^> ^encrypteds, EvaluationKeys ^evaluationKeys, Ciphertext ^destination)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypteds == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypteds cannot be null");
                }
                if (evaluationKeys == nullptr)
                {
                    throw gcnew ArgumentNullException("evaluationKeys cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    vector<seal::Ciphertext> v_encrypteds;
                    for each (Ciphertext ^polyarray in encrypteds)
                    {
                        if (polyarray == nullptr)
                        {
                            throw gcnew ArgumentNullException("encrypteds cannot be null");
                        }
                        v_encrypteds.emplace_back(polyarray->GetCiphertext());
                        GC::KeepAlive(polyarray);
                    }
                    evaluator_->multiply_many(v_encrypteds, evaluationKeys->GetKeys(), destination->GetCiphertext());
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

            void Evaluator::MultiplyMany(List<Ciphertext^> ^encrypteds, EvaluationKeys ^evaluationKeys, 
                Ciphertext ^destination, MemoryPoolHandle ^pool)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypteds == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypteds cannot be null");
                }
                if (evaluationKeys == nullptr)
                {
                    throw gcnew ArgumentNullException("evaluationKeys cannot be null");
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
                    vector<seal::Ciphertext> v_encrypteds;
                    for each (Ciphertext ^polyarray in encrypteds)
                    {
                        if (polyarray == nullptr)
                        {
                            throw gcnew ArgumentNullException("encrypteds cannot be null");
                        }
                        v_encrypteds.emplace_back(polyarray->GetCiphertext());
                        GC::KeepAlive(polyarray);
                    }
                    evaluator_->multiply_many(v_encrypteds, evaluationKeys->GetKeys(), destination->GetCiphertext(), pool->GetHandle());
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

            void Evaluator::Exponentiate(Ciphertext ^encrypted, UInt64 exponent, EvaluationKeys ^evaluationKeys)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypteds cannot be null");
                }
                if (evaluationKeys == nullptr)
                {
                    throw gcnew ArgumentNullException("evaluationKeys cannot be null");
                }
                try
                {
                    evaluator_->exponentiate(encrypted->GetCiphertext(), exponent, evaluationKeys->GetKeys());
                    GC::KeepAlive(encrypted);
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

            void Evaluator::Exponentiate(Ciphertext ^encrypted, UInt64 exponent, EvaluationKeys ^evaluationKeys, 
                Ciphertext ^destination)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypteds cannot be null");
                }
                if (evaluationKeys == nullptr)
                {
                    throw gcnew ArgumentNullException("evaluationKeys cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    evaluator_->exponentiate(encrypted->GetCiphertext(), exponent, evaluationKeys->GetKeys(), 
                        destination->GetCiphertext());
                    GC::KeepAlive(encrypted);
                    GC::KeepAlive(evaluationKeys);
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

            void Evaluator::Exponentiate(Ciphertext ^encrypted, UInt64 exponent, EvaluationKeys ^evaluationKeys,
                Ciphertext ^destination, MemoryPoolHandle ^pool)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypteds cannot be null");
                }
                if (evaluationKeys == nullptr)
                {
                    throw gcnew ArgumentNullException("evaluationKeys cannot be null");
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
                    evaluator_->exponentiate(encrypted->GetCiphertext(), exponent, evaluationKeys->GetKeys(),
                        destination->GetCiphertext(), pool->GetHandle());
                    GC::KeepAlive(encrypted);
                    GC::KeepAlive(evaluationKeys);
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

            void Evaluator::Exponentiate(Ciphertext ^encrypted, UInt64 exponent, EvaluationKeys ^evaluationKeys, 
                MemoryPoolHandle ^pool)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypteds cannot be null");
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
                    evaluator_->exponentiate(encrypted->GetCiphertext(), exponent, evaluationKeys->GetKeys(),
                        pool->GetHandle());
                    GC::KeepAlive(encrypted);
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

            void Evaluator::TransformToNTT(Plaintext ^plain)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                try
                {
                    evaluator_->transform_to_ntt(plain->GetPlaintext());
                    GC::KeepAlive(plain);
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

            void Evaluator::TransformToNTT(Plaintext ^plain, MemoryPoolHandle ^pool)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    evaluator_->transform_to_ntt(plain->GetPlaintext(), pool->GetHandle());
                    GC::KeepAlive(plain);
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

            void Evaluator::TransformToNTT(Plaintext ^plain, Plaintext ^destinationNTT)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                if (destinationNTT == nullptr)
                {
                    throw gcnew ArgumentNullException("destinationNTT cannot be null");
                }
                try
                {
                    evaluator_->transform_to_ntt(plain->GetPlaintext(), destinationNTT->GetPlaintext());
                    GC::KeepAlive(plain);
                    GC::KeepAlive(destinationNTT);
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

            void Evaluator::TransformToNTT(Plaintext ^plain, Plaintext ^destinationNTT, MemoryPoolHandle ^pool)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                if (destinationNTT == nullptr)
                {
                    throw gcnew ArgumentNullException("destinationNTT cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    evaluator_->transform_to_ntt(plain->GetPlaintext(), destinationNTT->GetPlaintext(), pool->GetHandle());
                    GC::KeepAlive(plain);
                    GC::KeepAlive(destinationNTT);
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

            void Evaluator::TransformToNTT(Ciphertext ^encrypted)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                try
                {
                    evaluator_->transform_to_ntt(encrypted->GetCiphertext());
                    GC::KeepAlive(encrypted);
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

            void Evaluator::TransformToNTT(Ciphertext ^encrypted, Ciphertext ^destinationNTT)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                if (destinationNTT == nullptr)
                {
                    throw gcnew ArgumentNullException("destinationNTT cannot be null");
                }
                try
                {
                    evaluator_->transform_to_ntt(encrypted->GetCiphertext(), destinationNTT->GetCiphertext());
                    GC::KeepAlive(encrypted);
                    GC::KeepAlive(destinationNTT);
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

            void Evaluator::TransformFromNTT(Ciphertext ^encryptedNTT)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encryptedNTT == nullptr)
                {
                    throw gcnew ArgumentNullException("encryptedNTT cannot be null");
                }
                try
                {
                    evaluator_->transform_from_ntt(encryptedNTT->GetCiphertext());
                    GC::KeepAlive(encryptedNTT);
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

            void Evaluator::TransformFromNTT(Ciphertext ^encryptedNTT, Ciphertext ^destination)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encryptedNTT == nullptr)
                {
                    throw gcnew ArgumentNullException("encryptedNTT cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    evaluator_->transform_from_ntt(encryptedNTT->GetCiphertext(), destination->GetCiphertext());
                    GC::KeepAlive(encryptedNTT);
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

            void Evaluator::MultiplyPlainNTT(Ciphertext ^encryptedNTT, Plaintext ^plainNTT, Ciphertext ^destinationNTT)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encryptedNTT == nullptr)
                {
                    throw gcnew ArgumentNullException("encryptedNTT cannot be null");
                }
                if (plainNTT == nullptr)
                {
                    throw gcnew ArgumentNullException("plainNTT cannot be null");
                }
                if (destinationNTT == nullptr)
                {
                    throw gcnew ArgumentNullException("destinationNTT cannot be null");
                }
                try
                {
                    evaluator_->multiply_plain_ntt(encryptedNTT->GetCiphertext(), plainNTT->GetPlaintext(), 
                        destinationNTT->GetCiphertext());
                    GC::KeepAlive(encryptedNTT);
                    GC::KeepAlive(plainNTT);
                    GC::KeepAlive(destinationNTT);
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

            void Evaluator::MultiplyPlainNTT(Ciphertext ^encryptedNTT, Plaintext ^plainNTT)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encryptedNTT == nullptr)
                {
                    throw gcnew ArgumentNullException("encryptedNTT cannot be null");
                }
                if (plainNTT == nullptr)
                {
                    throw gcnew ArgumentNullException("plainNTT cannot be null");
                }
                try
                {
                    evaluator_->multiply_plain_ntt(encryptedNTT->GetCiphertext(), plainNTT->GetPlaintext());
                    GC::KeepAlive(encryptedNTT);
                    GC::KeepAlive(plainNTT);
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

            void Evaluator::RotateRows(Ciphertext ^encrypted, int steps, GaloisKeys ^galoisKeys)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                if (galoisKeys == nullptr)
                {
                    throw gcnew ArgumentNullException("galoisKeys cannot be null");
                }
                try
                {
                    evaluator_->rotate_rows(encrypted->GetCiphertext(), steps, galoisKeys->GetKeys());
                    GC::KeepAlive(encrypted);
                    GC::KeepAlive(galoisKeys);
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

            void Evaluator::RotateRows(Ciphertext ^encrypted, int steps, GaloisKeys ^galoisKeys, MemoryPoolHandle ^pool)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                if (galoisKeys == nullptr)
                {
                    throw gcnew ArgumentNullException("galoisKeys cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    evaluator_->rotate_rows(encrypted->GetCiphertext(), steps, galoisKeys->GetKeys(), pool->GetHandle());
                    GC::KeepAlive(encrypted);
                    GC::KeepAlive(galoisKeys);
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

            void Evaluator::RotateRows(Ciphertext ^encrypted, int steps, GaloisKeys ^galoisKeys, Ciphertext ^destination)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                if (galoisKeys == nullptr)
                {
                    throw gcnew ArgumentNullException("galoisKeys cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    evaluator_->rotate_rows(encrypted->GetCiphertext(), steps, galoisKeys->GetKeys(), destination->GetCiphertext());
                    GC::KeepAlive(encrypted);
                    GC::KeepAlive(galoisKeys);
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

            void Evaluator::RotateRows(Ciphertext ^encrypted, int steps, GaloisKeys ^galoisKeys, Ciphertext ^destination, 
                MemoryPoolHandle ^pool)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                if (galoisKeys == nullptr)
                {
                    throw gcnew ArgumentNullException("galoisKeys cannot be null");
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
                    evaluator_->rotate_rows(encrypted->GetCiphertext(), steps, galoisKeys->GetKeys(), 
                        destination->GetCiphertext(), pool->GetHandle());
                    GC::KeepAlive(encrypted);
                    GC::KeepAlive(galoisKeys);
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

            void Evaluator::RotateColumns(Ciphertext ^encrypted, GaloisKeys ^galoisKeys)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                if (galoisKeys == nullptr)
                {
                    throw gcnew ArgumentNullException("galoisKeys cannot be null");
                }
                try
                {
                    evaluator_->rotate_columns(encrypted->GetCiphertext(), galoisKeys->GetKeys());
                    GC::KeepAlive(encrypted);
                    GC::KeepAlive(galoisKeys);
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

            void Evaluator::RotateColumns(Ciphertext ^encrypted, GaloisKeys ^galoisKeys, MemoryPoolHandle ^pool)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                if (galoisKeys == nullptr)
                {
                    throw gcnew ArgumentNullException("galoisKeys cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    evaluator_->rotate_columns(encrypted->GetCiphertext(), galoisKeys->GetKeys(), pool->GetHandle());
                    GC::KeepAlive(encrypted);
                    GC::KeepAlive(galoisKeys);
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

            void Evaluator::RotateColumns(Ciphertext ^encrypted, GaloisKeys ^galoisKeys, Ciphertext ^destination)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                if (galoisKeys == nullptr)
                {
                    throw gcnew ArgumentNullException("galoisKeys cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    evaluator_->rotate_columns(encrypted->GetCiphertext(), galoisKeys->GetKeys(), destination->GetCiphertext());
                    GC::KeepAlive(encrypted);
                    GC::KeepAlive(galoisKeys);
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

            void Evaluator::RotateColumns(Ciphertext ^encrypted, GaloisKeys ^galoisKeys, Ciphertext ^destination, MemoryPoolHandle ^pool)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                if (galoisKeys == nullptr)
                {
                    throw gcnew ArgumentNullException("galoisKeys cannot be null");
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
                    evaluator_->rotate_columns(encrypted->GetCiphertext(), galoisKeys->GetKeys(),
                        destination->GetCiphertext(), pool->GetHandle());
                    GC::KeepAlive(encrypted);
                    GC::KeepAlive(galoisKeys);
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

            seal::Evaluator &Evaluator::GetEvaluator()
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                return *evaluator_;
            }

            Evaluator::~Evaluator()
            {
                this->!Evaluator();
            }

            Evaluator::!Evaluator()
            {
                if (evaluator_ != nullptr)
                {
                    delete evaluator_;
                    evaluator_ = nullptr;
                }
            }
        }
    }
}