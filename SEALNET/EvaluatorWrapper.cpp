#include <cstddef>
#include <vector>
#include "EncryptionParamsWrapper.h"
#include "EvaluatorWrapper.h"
#include "BigPolyWrapper.h"
#include "BigUIntWrapper.h"
#include "EvaluationKeysWrapper.h"
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

            void Evaluator::Negate(BigPoly ^encrypted, BigPoly ^destination)
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
                    evaluator_->negate(encrypted->GetPolynomial(), destination->GetPolynomial());
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

            BigPoly ^Evaluator::Negate(BigPoly ^encrypted)
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
                    return gcnew BigPoly(evaluator_->negate(encrypted->GetPolynomial()));
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

            void Evaluator::Add(BigPoly ^encrypted1, BigPoly ^encrypted2, BigPoly ^destination)
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
                    evaluator_->add(encrypted1->GetPolynomial(), encrypted2->GetPolynomial(), destination->GetPolynomial());
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

            BigPoly ^Evaluator::Add(BigPoly ^encrypted1, BigPoly ^encrypted2)
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
                    return gcnew BigPoly(evaluator_->add(encrypted1->GetPolynomial(), encrypted2->GetPolynomial()));
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

            void Evaluator::AddMany(List<BigPoly^> ^encrypteds, BigPoly ^destination)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypteds == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypteds");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    vector<seal::BigPoly> v_encrypteds;
                    for each (BigPoly ^poly in encrypteds)
                    {
                        if (poly == nullptr)
                        {
                            throw gcnew ArgumentNullException("encrypted cannot be null");
                        }
                        v_encrypteds.push_back(poly->GetPolynomial());
                    }

                    evaluator_->add_many(v_encrypteds, destination->GetPolynomial());
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

            BigPoly ^Evaluator::AddMany(List<BigPoly^> ^encrypteds)
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
                    vector<seal::BigPoly> v_encrypteds;
                    for each (BigPoly ^poly in encrypteds)
                    {
                        if (poly == nullptr)
                        {
                            throw gcnew ArgumentNullException("encrypteds cannot contain null values null");
                        }
                        v_encrypteds.push_back(poly->GetPolynomial());
                    }

                    return gcnew BigPoly(evaluator_->add_many(v_encrypteds));
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

            void Evaluator::Sub(BigPoly ^encrypted1, BigPoly ^encrypted2, BigPoly ^destination)
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
                    evaluator_->sub(encrypted1->GetPolynomial(), encrypted2->GetPolynomial(), destination->GetPolynomial());
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

            BigPoly ^Evaluator::Sub(BigPoly ^encrypted1, BigPoly ^encrypted2)
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
                    return gcnew BigPoly(evaluator_->sub(encrypted1->GetPolynomial(), encrypted2->GetPolynomial()));
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

            void Evaluator::Multiply(BigPoly ^encrypted1, BigPoly ^encrypted2, BigPoly ^destination)
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
                    evaluator_->multiply(encrypted1->GetPolynomial(), encrypted2->GetPolynomial(), destination->GetPolynomial());
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

            BigPoly ^Evaluator::Multiply(BigPoly ^encrypted1, BigPoly ^encrypted2)
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
                    return gcnew BigPoly(evaluator_->multiply(encrypted1->GetPolynomial(), encrypted2->GetPolynomial()));
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

            void Evaluator::MultiplyNoRelin(BigPoly ^encrypted1, BigPoly ^encrypted2, BigPoly ^destination)
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
                    evaluator_->multiply_norelin(encrypted1->GetPolynomial(), encrypted2->GetPolynomial(), destination->GetPolynomial());
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

            BigPoly ^Evaluator::MultiplyNoRelin(BigPoly ^encrypted1, BigPoly ^encrypted2)
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
                    return gcnew BigPoly(evaluator_->multiply_norelin(encrypted1->GetPolynomial(), encrypted2->GetPolynomial()));
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

            void Evaluator::Relinearize(BigPoly ^encrypted, BigPoly ^destination)
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
                    evaluator_->relinearize(encrypted->GetPolynomial(), destination->GetPolynomial());
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

            BigPoly ^Evaluator::Relinearize(BigPoly ^encrypted)
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
                    return gcnew BigPoly(evaluator_->relinearize(encrypted->GetPolynomial()));
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

            void Evaluator::AddPlain(BigPoly ^encrypted1, BigPoly ^plain2, BigPoly ^destination)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted1 == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted1 cannot be null");
                }
                if (plain2 == nullptr)
                {
                    throw gcnew ArgumentNullException("plain2 cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    evaluator_->add_plain(encrypted1->GetPolynomial(), plain2->GetPolynomial(), destination->GetPolynomial());
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

            BigPoly ^Evaluator::AddPlain(BigPoly ^encrypted1, BigPoly ^plain2)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted1 == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted1 cannot be null");
                }
                if (plain2 == nullptr)
                {
                    throw gcnew ArgumentNullException("plain2 cannot be null");
                }
                try
                {
                    return gcnew BigPoly(evaluator_->add_plain(encrypted1->GetPolynomial(), plain2->GetPolynomial()));
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

            void Evaluator::SubPlain(BigPoly ^encrypted1, BigPoly ^plain2, BigPoly ^destination)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted1 == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted1 cannot be null");
                }
                if (plain2 == nullptr)
                {
                    throw gcnew ArgumentNullException("plain2 cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    evaluator_->sub_plain(encrypted1->GetPolynomial(), plain2->GetPolynomial(), destination->GetPolynomial());
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

            BigPoly ^Evaluator::SubPlain(BigPoly ^encrypted1, BigPoly ^plain2)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted1 == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted1 cannot be null");
                }
                if (plain2 == nullptr)
                {
                    throw gcnew ArgumentNullException("plain2 cannot be null");
                }
                try
                {
                    return gcnew BigPoly(evaluator_->sub_plain(encrypted1->GetPolynomial(), plain2->GetPolynomial()));
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

            void Evaluator::MultiplyPlain(BigPoly ^encrypted1, BigPoly ^plain2, BigPoly ^destination)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted1 == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted1 cannot be null");
                }
                if (plain2 == nullptr)
                {
                    throw gcnew ArgumentNullException("plain2 cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    evaluator_->multiply_plain(encrypted1->GetPolynomial(), plain2->GetPolynomial(), destination->GetPolynomial());
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

            BigPoly ^Evaluator::MultiplyPlain(BigPoly ^encrypted1, BigPoly ^plain2)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypted1 == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted1 cannot be null");
                }
                if (plain2 == nullptr)
                {
                    throw gcnew ArgumentNullException("plain2 cannot be null");
                }
                try
                {
                    return gcnew BigPoly(evaluator_->multiply_plain(encrypted1->GetPolynomial(), plain2->GetPolynomial()));
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

            void Evaluator::MultiplyMany(List<BigPoly^> ^encrypteds, BigPoly ^destination)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypteds == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypteds");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    vector<seal::BigPoly> v_encrypteds;
                    for each (BigPoly ^poly in encrypteds)
                    {
                        if (poly == nullptr)
                        {
                            throw gcnew ArgumentNullException("encrypted cannot be null");
                        }
                        v_encrypteds.push_back(poly->GetPolynomial());
                    }

                    evaluator_->multiply_many(v_encrypteds, destination->GetPolynomial());
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

            BigPoly ^Evaluator::MultiplyMany(List<BigPoly^> ^encrypteds)
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
                    vector<seal::BigPoly> v_encrypteds;
                    for each (BigPoly ^poly in encrypteds)
                    {
                        if (poly == nullptr)
                        {
                            throw gcnew ArgumentNullException("encrypteds cannot contain null values null");
                        }
                        v_encrypteds.push_back(poly->GetPolynomial());
                    }

                    return gcnew BigPoly(evaluator_->multiply_many(v_encrypteds));
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

            void Evaluator::MultiplyNoRelinMany(List<BigPoly^> ^encrypteds, BigPoly ^destination)
            {
                if (evaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Evaluator is disposed");
                }
                if (encrypteds == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypteds");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    vector<seal::BigPoly> v_encrypteds;
                    for each (BigPoly ^poly in encrypteds)
                    {
                        if (poly == nullptr)
                        {
                            throw gcnew ArgumentNullException("encrypted cannot be null");
                        }
                        v_encrypteds.push_back(poly->GetPolynomial());
                    }

                    evaluator_->multiply_norelin_many(v_encrypteds, destination->GetPolynomial());
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

            BigPoly ^Evaluator::MultiplyNoRelinMany(List<BigPoly^> ^encrypteds)
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
                    vector<seal::BigPoly> v_encrypteds;
                    for each (BigPoly ^poly in encrypteds)
                    {
                        if (poly == nullptr)
                        {
                            throw gcnew ArgumentNullException("encrypteds cannot contain null values null");
                        }
                        v_encrypteds.push_back(poly->GetPolynomial());
                    }

                    return gcnew BigPoly(evaluator_->multiply_norelin_many(v_encrypteds));
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

            void Evaluator::Exponentiate(BigPoly ^encrypted, int exponent, BigPoly ^destination)
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
                    evaluator_->exponentiate(encrypted->GetPolynomial(), exponent, destination->GetPolynomial());
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

            BigPoly ^Evaluator::Exponentiate(BigPoly ^encrypted, int exponent)
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
                    return gcnew BigPoly(evaluator_->exponentiate(encrypted->GetPolynomial(), exponent));
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

            void Evaluator::ExponentiateNoRelin(BigPoly ^encrypted, int exponent, BigPoly ^destination)
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
                    evaluator_->exponentiate_norelin(encrypted->GetPolynomial(), exponent, destination->GetPolynomial());
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

            BigPoly ^Evaluator::ExponentiateNoRelin(BigPoly ^encrypted, int exponent)
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
                    return gcnew BigPoly(evaluator_->exponentiate_norelin(encrypted->GetPolynomial(), exponent));
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