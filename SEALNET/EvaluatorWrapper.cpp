#include <cstddef>
#include <vector>
#include "EncryptionParamsWrapper.h"
#include "EvaluatorWrapper.h"
#include "BigPolyWrapper.h"
#include "BigUIntWrapper.h"
#include "Common.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace std;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            Evaluator::Evaluator(EncryptionParameters ^parms, Microsoft::Research::SEAL::EvaluationKeys ^evaluationKeys) : evaluator_(nullptr)
            {
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                if (evaluationKeys == nullptr)
                {
                    throw gcnew ArgumentNullException("evaluationKeys cannot be null");
                }
                try
                {
                    evaluator_ = new seal::Evaluator(parms->GetParameters(), evaluationKeys->GetKeys());
                    GC::KeepAlive(parms);
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

            Evaluator::Evaluator(EncryptionParameters ^parms, Microsoft::Research::SEAL::EvaluationKeys ^evaluationKeys, MemoryPoolHandle ^pool) : evaluator_(nullptr)
            {
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
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
                    evaluator_ = new seal::Evaluator(parms->GetParameters(), evaluationKeys->GetKeys(), pool->GetHandle());
                    GC::KeepAlive(parms);
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

            Evaluator::Evaluator(EncryptionParameters ^parms) : evaluator_(nullptr)
            {
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                try
                {
                    evaluator_ = new seal::Evaluator(parms->GetParameters());
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

            Evaluator::Evaluator(EncryptionParameters ^parms, MemoryPoolHandle ^pool) : evaluator_(nullptr)
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
                    evaluator_ = new seal::Evaluator(parms->GetParameters(), pool->GetHandle());
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

            Evaluator::Evaluator(Evaluator ^copy) : evaluator_(nullptr)
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

            EvaluationKeys ^Evaluator::EvaluationKeys::get()
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                return gcnew Microsoft::Research::SEAL::EvaluationKeys(evaluator_->evaluation_keys());
            }

            void Evaluator::Negate(BigPolyArray ^encrypted, BigPolyArray ^destination)
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
                    evaluator_->negate(encrypted->GetArray(), destination->GetArray());
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

            BigPolyArray ^Evaluator::Negate(BigPolyArray ^encrypted)
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
                    auto result = gcnew BigPolyArray(evaluator_->negate(encrypted->GetArray()));
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

            void Evaluator::Add(BigPolyArray ^encrypted1, BigPolyArray ^encrypted2, BigPolyArray ^destination)
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
                    evaluator_->add(encrypted1->GetArray(), encrypted2->GetArray(), destination->GetArray());
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

            BigPolyArray ^Evaluator::Add(BigPolyArray ^encrypted1, BigPolyArray ^encrypted2)
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
                    auto result = gcnew BigPolyArray(evaluator_->add(encrypted1->GetArray(), encrypted2->GetArray()));
                    GC::KeepAlive(encrypted1);
                    GC::KeepAlive(encrypted2);
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

            void Evaluator::AddMany(List<BigPolyArray^> ^encrypteds, BigPolyArray ^destination)
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
                    vector<seal::BigPolyArray> v_encrypteds;
                    for each (BigPolyArray ^poly in encrypteds)
                    {
                        if (poly == nullptr)
                        {
                            throw gcnew ArgumentNullException("encrypteds cannot be null");
                        }
                        v_encrypteds.emplace_back(poly->GetArray());
                        GC::KeepAlive(poly);
                    }

                    evaluator_->add_many(v_encrypteds, destination->GetArray());
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

            BigPolyArray ^Evaluator::AddMany(List<BigPolyArray^> ^encrypteds)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypteds == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypteds cannot be null");
                }
                try
                {
                    vector<seal::BigPolyArray> v_encrypteds;
                    for each (BigPolyArray ^poly in encrypteds)
                    {
                        if (poly == nullptr)
                        {
                            throw gcnew ArgumentNullException("encrypteds cannot be null");
                        }
                        v_encrypteds.emplace_back(poly->GetArray());
                        GC::KeepAlive(poly);
                    }

                    return gcnew BigPolyArray(evaluator_->add_many(v_encrypteds));
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

            Ciphertext ^Evaluator::AddMany(List<Ciphertext^> ^encrypteds)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypteds == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypteds cannot be null");
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

                    return gcnew Ciphertext(evaluator_->add_many(v_encrypteds));
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

            void Evaluator::Sub(BigPolyArray ^encrypted1, BigPolyArray ^encrypted2, BigPolyArray ^destination)
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
                    evaluator_->sub(encrypted1->GetArray(), encrypted2->GetArray(), destination->GetArray());
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

            BigPolyArray ^Evaluator::Sub(BigPolyArray ^encrypted1, BigPolyArray ^encrypted2)
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
                    auto result = gcnew BigPolyArray(evaluator_->sub(encrypted1->GetArray(), encrypted2->GetArray()));
                    GC::KeepAlive(encrypted1);
                    GC::KeepAlive(encrypted2);
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

            void Evaluator::Multiply(BigPolyArray ^encrypted1, BigPolyArray ^encrypted2, BigPolyArray ^destination)
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
                    evaluator_->multiply(encrypted1->GetArray(), encrypted2->GetArray(), destination->GetArray());
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

            BigPolyArray ^Evaluator::Multiply(BigPolyArray ^encrypted1, BigPolyArray ^encrypted2)
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
                    auto result = gcnew BigPolyArray(evaluator_->multiply(encrypted1->GetArray(), encrypted2->GetArray()));
                    GC::KeepAlive(encrypted1);
                    GC::KeepAlive(encrypted2);
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

            void Evaluator::Square(BigPolyArray ^encrypted, BigPolyArray ^destination)
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
                    evaluator_->square(encrypted->GetArray(), destination->GetArray());
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

            BigPolyArray ^Evaluator::Square(BigPolyArray ^encrypted)
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
                    auto result = gcnew BigPolyArray(evaluator_->square(encrypted->GetArray()));
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

            void Evaluator::Relinearize(BigPolyArray ^encrypted, BigPolyArray ^destination)
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
                    evaluator_->relinearize(encrypted->GetArray(), destination->GetArray());
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

            void Evaluator::Relinearize(BigPolyArray ^encrypted, BigPolyArray ^destination, int destinationSize)
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
                    evaluator_->relinearize(encrypted->GetArray(), destination->GetArray(), destinationSize);
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

            BigPolyArray ^Evaluator::Relinearize(BigPolyArray ^encrypted)
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
                    auto result = gcnew BigPolyArray(evaluator_->relinearize(encrypted->GetArray()));
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

            BigPolyArray ^Evaluator::Relinearize(BigPolyArray ^encrypted, int destinationSize)
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
                    auto result = gcnew BigPolyArray(evaluator_->relinearize(encrypted->GetArray(), destinationSize));
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

            void Evaluator::AddPlain(BigPolyArray ^encrypted, BigPoly ^plain, BigPolyArray ^destination)
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
                    evaluator_->add_plain(encrypted->GetArray(), plain->GetPolynomial(), destination->GetArray());
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

            BigPolyArray ^Evaluator::AddPlain(BigPolyArray ^encrypted, BigPoly ^plain)
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
                    auto result = gcnew BigPolyArray(evaluator_->add_plain(encrypted->GetArray(), plain->GetPolynomial()));
                    GC::KeepAlive(encrypted);
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

            void Evaluator::SubPlain(BigPolyArray ^encrypted, BigPoly ^plain, BigPolyArray ^destination)
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
                    evaluator_->sub_plain(encrypted->GetArray(), plain->GetPolynomial(), destination->GetArray());
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

            BigPolyArray ^Evaluator::SubPlain(BigPolyArray ^encrypted, BigPoly ^plain)
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
                    auto result = gcnew BigPolyArray(evaluator_->sub_plain(encrypted->GetArray(), plain->GetPolynomial()));
                    GC::KeepAlive(encrypted);
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

            void Evaluator::MultiplyPlain(BigPolyArray ^encrypted, BigPoly ^plain, BigPolyArray ^destination)
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
                    evaluator_->multiply_plain(encrypted->GetArray(), plain->GetPolynomial(), destination->GetArray());
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

            BigPolyArray ^Evaluator::MultiplyPlain(BigPolyArray ^encrypted, BigPoly ^plain)
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
                    auto result = gcnew BigPolyArray(evaluator_->multiply_plain(encrypted->GetArray(), plain->GetPolynomial()));
                    GC::KeepAlive(encrypted);
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

            void Evaluator::MultiplyMany(List<BigPolyArray^> ^encrypteds, BigPolyArray ^destination)
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
                    vector<seal::BigPolyArray> v_encrypteds;
                    for each (BigPolyArray ^polyarray in encrypteds)
                    {
                        if (polyarray == nullptr)
                        {
                            throw gcnew ArgumentNullException("encrypteds cannot be null");
                        }
                        v_encrypteds.emplace_back(polyarray->GetArray());
                        GC::KeepAlive(polyarray);
                    }

                    evaluator_->multiply_many(v_encrypteds, destination->GetArray());
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

            void Evaluator::MultiplyMany(List<Ciphertext^> ^encrypteds, Ciphertext ^destination)
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
                    for each (Ciphertext ^polyarray in encrypteds)
                    {
                        if (polyarray == nullptr)
                        {
                            throw gcnew ArgumentNullException("encrypteds cannot be null");
                        }
                        v_encrypteds.emplace_back(polyarray->GetCiphertext());
                        GC::KeepAlive(polyarray);
                    }

                    evaluator_->multiply_many(v_encrypteds, destination->GetCiphertext());
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

            BigPolyArray ^Evaluator::MultiplyMany(List<BigPolyArray^> ^encrypteds)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypteds == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypteds cannot be null");
                }
                try
                {
                    vector<seal::BigPolyArray> v_encrypteds;
                    for each (BigPolyArray ^polyarray in encrypteds)
                    {
                        if (polyarray == nullptr)
                        {
                            throw gcnew ArgumentNullException("encrypteds cannot be null");
                        }
                        v_encrypteds.emplace_back(polyarray->GetArray());
                        GC::KeepAlive(polyarray);
                    }

                    return gcnew BigPolyArray(evaluator_->multiply_many(v_encrypteds));
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

            Ciphertext ^Evaluator::MultiplyMany(List<Ciphertext^> ^encrypteds)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypteds == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypteds cannot be null");
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

                    return gcnew Ciphertext(evaluator_->multiply_many(v_encrypteds));
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

            void Evaluator::Exponentiate(BigPolyArray ^encrypted, UInt64 exponent, BigPolyArray ^destination)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypteds cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    evaluator_->exponentiate(encrypted->GetArray(), exponent, destination->GetArray());
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

            BigPolyArray ^Evaluator::Exponentiate(BigPolyArray ^encrypted, UInt64 exponent)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypteds cannot be null");
                }
                try
                {
                    auto result = gcnew BigPolyArray(evaluator_->exponentiate(encrypted->GetArray(), exponent));
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

            void Evaluator::TransformToNTT(BigPoly ^plain)
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
                    evaluator_->transform_to_ntt(plain->GetPolynomial());
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

            void Evaluator::TransformToNTT(BigPolyArray ^encrypted)
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
                    evaluator_->transform_to_ntt(encrypted->GetArray());
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

            void Evaluator::TransformFromNTT(BigPoly ^plainNTT)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (plainNTT == nullptr)
                {
                    throw gcnew ArgumentNullException("plainNTT cannot be null");
                }
                try
                {
                    evaluator_->transform_from_ntt(plainNTT->GetPolynomial());
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

            void Evaluator::TransformFromNTT(BigPolyArray ^encryptedNTT)
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
                    evaluator_->transform_from_ntt(encryptedNTT->GetArray());
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

            void Evaluator::MultiplyPlainNTT(BigPolyArray ^encryptedNTT, BigPoly ^plainNTT, BigPolyArray ^destinationNTT)
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
                    evaluator_->multiply_plain_ntt(encryptedNTT->GetArray(), plainNTT->GetPolynomial(), destinationNTT->GetArray());
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

            BigPolyArray ^ Evaluator::MultiplyPlainNTT(BigPolyArray ^encryptedNTT, BigPoly ^plainNTT)
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
                    auto  result = gcnew BigPolyArray(evaluator_->multiply_plain_ntt(encryptedNTT->GetArray(), plainNTT->GetPolynomial()));
                    GC::KeepAlive(encryptedNTT);
                    GC::KeepAlive(plainNTT);
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