#include <stdexcept>
#include "ChooserWrapper.h"
#include "Common.h"
#include "BigUIntWrapper.h"
#include "SimulatorWrapper.h"
#include "EncryptionParamsWrapper.h"

using namespace System;
using namespace seal;
using namespace std;
using namespace System::Collections::Generic;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ChooserPoly::ChooserPoly() : chooserPoly_(nullptr)
            {
                try
                {
                    chooserPoly_ = new seal::ChooserPoly();
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

            ChooserPoly::ChooserPoly(int maxCoeffCount, BigUInt ^maxAbsValue) : chooserPoly_(nullptr)
            {
                if (maxAbsValue == nullptr)
                {
                    throw gcnew ArgumentNullException("maxAbsValue cannot be null");
                }
                try
                {
                    chooserPoly_ = new seal::ChooserPoly(maxCoeffCount, maxAbsValue->GetUInt());
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

            ChooserPoly::ChooserPoly(int maxCoeffCount, System::UInt64 maxAbsValue) : chooserPoly_(nullptr)
            {
                try
                {
                    chooserPoly_ = new seal::ChooserPoly(maxCoeffCount, maxAbsValue);
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

            ChooserPoly::ChooserPoly(const seal::ChooserPoly &value) : chooserPoly_(nullptr)
            {
                try
                {
                    chooserPoly_ = new seal::ChooserPoly(value);
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

            ChooserPoly::~ChooserPoly()
            {
                this->!ChooserPoly();
            }

            ChooserPoly::!ChooserPoly()
            {
                if (chooserPoly_ != nullptr)
                {
                    delete chooserPoly_;
                    chooserPoly_ = nullptr;
                }
            }

            ChooserPoly::ChooserPoly(ChooserPoly ^copy) : chooserPoly_(nullptr)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    chooserPoly_ = new seal::ChooserPoly(copy->GetChooserPoly());
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

            seal::ChooserPoly &ChooserPoly::GetChooserPoly()
            {
                if (chooserPoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserPoly is disposed");
                }
                return *chooserPoly_;
            }

            void ChooserPoly::Set(ChooserPoly ^assign)
            {
                if (chooserPoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserPoly is disposed");
                }
                if (assign == nullptr)
                {
                    throw gcnew ArgumentNullException("assign cannot be null");
                }
                try
                {
                    *chooserPoly_ = assign->GetChooserPoly();
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

            int ChooserPoly::MaxCoeffCount::get()
            {
                if (chooserPoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserPoly is disposed");
                }
                return chooserPoly_->max_coeff_count();
            }

            BigUInt ^ChooserPoly::MaxAbsValue::get()
            {
                if (chooserPoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserPoly is disposed");
                }
                return gcnew BigUInt(chooserPoly_->max_abs_value());
            }

            void ChooserPoly::MaxCoeffCount::set(int value)
            {
                if (chooserPoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserPoly is disposed");
                }
                chooserPoly_->max_coeff_count() = value;
            }

            void ChooserPoly::MaxAbsValue::set(BigUInt ^value)
            {
                if (chooserPoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserPoly is disposed");
                }
                try
                {
                    chooserPoly_->max_abs_value() = value->GetUInt();
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

            bool ChooserPoly::TestParameters(EncryptionParameters ^parms)
            {
                if (chooserPoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserPoly is disposed");
                }
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                try
                {
                    return chooserPoly_->test_parameters(parms->GetParameters());
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

            Simulation ^ChooserPoly::Simulate(EncryptionParameters ^parms)
            {
                if (chooserPoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserPoly is disposed");
                }
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                try
                {
                    return gcnew Simulation(chooserPoly_->simulate(parms->GetParameters()));
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

            EncryptionParameters ^ChooserPoly::SelectParameters()
            {
                if (chooserPoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserPoly is disposed");
                }
                try
                {
                    return gcnew EncryptionParameters(chooserPoly_->select_parameters());
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

            void ChooserPoly::SetFresh()
            {
                if (chooserPoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserPoly is disposed");
                }
                try
                {
                    chooserPoly_->set_fresh();
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

            void ChooserPoly::Reset()
            {
                if (chooserPoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserPoly is disposed");
                }
                try
                {
                    chooserPoly_->reset();
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

            BigUInt ^ChooserPoly::Noise(EncryptionParameters ^parms)
            {
                if (chooserPoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserPoly is disposed");
                }
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                try
                {
                    return gcnew BigUInt(chooserPoly_->noise(parms->GetParameters()));
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

            BigUInt ^ChooserPoly::MaxNoise(EncryptionParameters ^parms)
            {
                if (chooserPoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserPoly is disposed");
                }
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                try
                {
                    return gcnew BigUInt(chooserPoly_->noise_bits(parms->GetParameters()));
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

            int ChooserPoly::NoiseBits(EncryptionParameters ^parms)
            {
                if (chooserPoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserPoly is disposed");
                }
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                try
                {
                    return chooserPoly_->noise_bits(parms->GetParameters());
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

            int ChooserPoly::NoiseBitsLeft(EncryptionParameters ^parms)
            {
                if (chooserPoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserPoly is disposed");
                }
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                try
                {
                    return chooserPoly_->noise_bits_left(parms->GetParameters());
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

            bool ChooserPoly::Decrypts(EncryptionParameters ^parms)
            {
                if (chooserPoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserPoly is disposed");
                }
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                try
                {
                    return chooserPoly_->decrypts(parms->GetParameters());
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

            ChooserEvaluator::ChooserEvaluator() : chooserEvaluator_(nullptr)
            {
                chooserEvaluator_ = new seal::ChooserEvaluator;
            }

            ChooserEvaluator::~ChooserEvaluator()
            {
                this->!ChooserEvaluator();
            }

            ChooserEvaluator::!ChooserEvaluator()
            {
                if (chooserEvaluator_ == nullptr)
                {
                    delete chooserEvaluator_;
                    chooserEvaluator_ = nullptr;
                }
            }

            ChooserPoly ^ChooserEvaluator::Multiply(ChooserPoly ^operand1, ChooserPoly ^operand2)
            {
                if (chooserEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEvaluator is disposed");
                }
                if (operand1 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand1 cannot be null");
                }
                if (operand2 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand2 cannot be null");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEvaluator_->multiply(operand1->GetChooserPoly(), operand2->GetChooserPoly()));
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

            ChooserPoly ^ChooserEvaluator::MultiplyNoRelin(ChooserPoly ^operand1, ChooserPoly ^operand2)
            {
                if (chooserEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEvaluator is disposed");
                }
                if (operand1 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand1 cannot be null");
                }
                if (operand2 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand2 cannot be null");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEvaluator_->multiply_norelin(operand1->GetChooserPoly(), operand2->GetChooserPoly()));
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

            ChooserPoly ^ChooserEvaluator::Relinearize(ChooserPoly ^operand)
            {
                if (chooserEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEvaluator is disposed");
                }
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEvaluator_->relinearize(operand->GetChooserPoly()));
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

            ChooserPoly ^ChooserEvaluator::Add(ChooserPoly ^operand1, ChooserPoly ^operand2)
            {
                if (chooserEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEvaluator is disposed");
                }
                if (operand1 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand1 cannot be null");
                }
                if (operand2 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand2 cannot be null");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEvaluator_->add(operand1->GetChooserPoly(), operand2->GetChooserPoly()));
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

            ChooserPoly ^ChooserEvaluator::Sub(ChooserPoly ^operand1, ChooserPoly ^operand2)
            {
                if (chooserEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEvaluator is disposed");
                }
                if (operand1 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand1 cannot be null");
                }
                if (operand2 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand2 cannot be null");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEvaluator_->sub(operand1->GetChooserPoly(), operand2->GetChooserPoly()));
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

            ChooserPoly ^ChooserEvaluator::MultiplyPlain(ChooserPoly ^operand, int plainMaxCoeffCount, BigUInt ^plainMaxAbsValue)
            {
                if (chooserEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEvaluator is disposed");
                }
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEvaluator_->multiply_plain(operand->GetChooserPoly(), plainMaxCoeffCount, plainMaxAbsValue->GetUInt()));
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

            ChooserPoly ^ChooserEvaluator::MultiplyPlain(ChooserPoly ^operand, int plainMaxCoeffCount, UInt64 plainMaxAbsValue)
            {
                if (chooserEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEvaluator is disposed");
                }
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEvaluator_->multiply_plain(operand->GetChooserPoly(), plainMaxCoeffCount, plainMaxAbsValue));
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

            ChooserPoly ^ChooserEvaluator::MultiplyPlain(ChooserPoly ^operand, ChooserPoly ^plainChooserPoly)
            {
                if (chooserEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEvaluator is disposed");
                }
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                if (plainChooserPoly == nullptr)
                {
                    throw gcnew ArgumentNullException("plainChooserPoly cannot be null");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEvaluator_->multiply_plain(operand->GetChooserPoly(), plainChooserPoly->GetChooserPoly()));
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

            ChooserPoly ^ChooserEvaluator::AddPlain(ChooserPoly ^operand, int plainMaxCoeffCount, BigUInt ^plainMaxAbsValue)
            {
                if (chooserEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEvaluator is disposed");
                }
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                if (plainMaxAbsValue == nullptr)
                {
                    throw gcnew ArgumentNullException("plainMaxAbsValue cannot be null");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEvaluator_->add_plain(operand->GetChooserPoly(), plainMaxCoeffCount, plainMaxAbsValue->GetUInt()));
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

            ChooserPoly ^ChooserEvaluator::AddPlain(ChooserPoly ^operand, int plainMaxCoeffCount, UInt64 plainMaxAbsValue)
            {
                if (chooserEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEvaluator is disposed");
                }
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEvaluator_->add_plain(operand->GetChooserPoly(), plainMaxCoeffCount, plainMaxAbsValue));
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

            ChooserPoly ^ChooserEvaluator::AddPlain(ChooserPoly ^operand, ChooserPoly ^plainChooserPoly)
            {
                if (chooserEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEvaluator is disposed");
                }
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                if (plainChooserPoly == nullptr)
                {
                    throw gcnew ArgumentNullException("plainChooserPoly cannot be null");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEvaluator_->add_plain(operand->GetChooserPoly(), plainChooserPoly->GetChooserPoly()));
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

            ChooserPoly ^ChooserEvaluator::SubPlain(ChooserPoly ^operand, int plainMaxCoeffCount, BigUInt ^plainMaxAbsValue)
            {
                if (chooserEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEvaluator is disposed");
                }
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                if (plainMaxAbsValue == nullptr)
                {
                    throw gcnew ArgumentNullException("plainMaxAbsValue cannot be null");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEvaluator_->sub_plain(operand->GetChooserPoly(), plainMaxCoeffCount, plainMaxAbsValue->GetUInt()));
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

            ChooserPoly ^ChooserEvaluator::SubPlain(ChooserPoly ^operand, int plainMaxCoeffCount, UInt64 plainMaxAbsValue)
            {
                if (chooserEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEvaluator is disposed");
                }
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEvaluator_->sub_plain(operand->GetChooserPoly(), plainMaxCoeffCount, plainMaxAbsValue));
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

            ChooserPoly ^ChooserEvaluator::SubPlain(ChooserPoly ^operand, ChooserPoly ^plainChooserPoly)
            {
                if (chooserEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEvaluator is disposed");
                }
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                if (plainChooserPoly == nullptr)
                {
                    throw gcnew ArgumentNullException("plainChooserPoly cannot be null");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEvaluator_->sub_plain(operand->GetChooserPoly(), plainChooserPoly->GetChooserPoly()));
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

            ChooserPoly ^ChooserEvaluator::TreeMultiply(List<ChooserPoly^> ^operands)
            {
                if (chooserEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEvaluator is disposed");
                }
                if (operands == nullptr)
                {
                    throw gcnew ArgumentNullException("operands cannot be null");
                }
                try
                {
                    vector<seal::ChooserPoly> v_operands;
                    for each (ChooserPoly ^operand in operands)
                    {
                        if (operand == nullptr)
                        {
                            throw gcnew ArgumentNullException("operand cannot be null");
                        }
                        v_operands.push_back(operand->GetChooserPoly());
                    }

                    return gcnew ChooserPoly(chooserEvaluator_->tree_multiply(v_operands));
                }
                catch (const exception& e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                throw gcnew Exception("Unexpected exception");
            }

            ChooserPoly ^ChooserEvaluator::TreeExponentiate(ChooserPoly ^operand, int exponent)
            {
                if (chooserEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEvaluator is disposed");
                }
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEvaluator_->tree_exponentiate(operand->GetChooserPoly(), exponent));
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

            ChooserPoly ^ChooserEvaluator::BinaryExponentiate(ChooserPoly ^operand, int exponent)
            {
                if (chooserEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEvaluator is disposed");
                }
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEvaluator_->binary_exponentiate(operand->GetChooserPoly(), exponent));
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

            ChooserPoly ^ChooserEvaluator::Negate(ChooserPoly ^operand)
            {
                if (chooserEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEvaluator is disposed");
                }
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEvaluator_->negate(operand->GetChooserPoly()));
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

            seal::ChooserEvaluator &ChooserEvaluator::GetEvaluator()
            {
                if (chooserEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEvaluator is disposed");
                }
                return *chooserEvaluator_;
            }

            ChooserEncoder::ChooserEncoder() : chooserEncoder_(nullptr)
            {
                try
                {
                    chooserEncoder_ = new seal::ChooserEncoder();
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

            ChooserEncoder::ChooserEncoder(int base) : chooserEncoder_(nullptr)
            {
                try
                {
                    chooserEncoder_ = new seal::ChooserEncoder(base);
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

            ChooserEncoder::~ChooserEncoder()
            {
                this->!ChooserEncoder();
            }

            ChooserEncoder::!ChooserEncoder()
            {
                if (chooserEncoder_ != nullptr)
                {
                    delete chooserEncoder_;
                    chooserEncoder_ = nullptr;
                }
            }

            ChooserPoly ^ChooserEncoder::Encode(System::UInt64 value)
            {
                if (chooserEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEncoder is disposed");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEncoder_->encode(value));
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

            void ChooserEncoder::Encode(System::UInt64 value, ChooserPoly ^destination)
            {
                if (chooserEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEncoder is disposed");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    chooserEncoder_->encode(value, destination->GetChooserPoly());
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

            ChooserPoly ^ChooserEncoder::Encode(System::Int64 value)
            {
                if (chooserEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEncoder is disposed");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEncoder_->encode(value));
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

            void ChooserEncoder::Encode(System::Int64 value, ChooserPoly ^destination)
            {
                if (chooserEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEncoder is disposed");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    chooserEncoder_->encode(value, destination->GetChooserPoly());
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

            ChooserPoly ^ChooserEncoder::Encode(BigUInt ^value)
            {
                if (chooserEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEncoder is disposed");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEncoder_->encode(value->GetUInt()));
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

            void ChooserEncoder::Encode(BigUInt ^value, ChooserPoly ^destination)
            {
                if (chooserEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEncoder is disposed");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    chooserEncoder_->encode(value->GetUInt(), destination->GetChooserPoly());
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

            ChooserPoly ^ChooserEncoder::Encode(System::Int32 value)
            {
                if (chooserEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEncoder is disposed");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEncoder_->encode(value));
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

            ChooserPoly ^ChooserEncoder::Encode(System::UInt32 value)
            {
                if (chooserEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEncoder is disposed");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEncoder_->encode(value));
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

            int ChooserEncoder::Base::get()
            {
                if (chooserEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEncoder is disposed");
                }
                return chooserEncoder_->base();
            }

            void ChooserEncoder::Base::set(int value)
            {
                if (chooserEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEncoder is disposed");
                }
                try
                {
                    chooserEncoder_->set_base(value);
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

            seal::ChooserEncoder &ChooserEncoder::GetEncoder()
            {
                if (chooserEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEncoder is disposed");
                }
                return *chooserEncoder_;
            }

            ChooserEncryptor::ChooserEncryptor()
            {
                try
                {
                    chooserEncryptor_ = new seal::ChooserEncryptor();
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

            ChooserEncryptor::~ChooserEncryptor()
            {
                this->!ChooserEncryptor();
            }

            ChooserEncryptor::!ChooserEncryptor()
            {
                if (chooserEncryptor_ != nullptr)
                {
                    delete chooserEncryptor_;
                    chooserEncryptor_ = nullptr;
                }
            }

            void ChooserEncryptor::Encrypt(ChooserPoly ^plain, ChooserPoly ^destination)
            {
                if (chooserEncryptor_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEncryptor is disposed");
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
                    chooserEncryptor_->encrypt(plain->GetChooserPoly());
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

            inline ChooserPoly ^ChooserEncryptor::Encrypt(ChooserPoly ^plain)
            {
                if (chooserEncryptor_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEncryptor is disposed");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEncryptor_->encrypt(plain->GetChooserPoly()));
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

            void ChooserEncryptor::Decrypt(ChooserPoly ^encrypted, ChooserPoly ^destination)
            {
                if (chooserEncryptor_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEncryptor is disposed");
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
                    chooserEncryptor_->decrypt(encrypted->GetChooserPoly());
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

            inline ChooserPoly ^ChooserEncryptor::Decrypt(ChooserPoly ^encrypted)
            {
                if (chooserEncryptor_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEncryptor is disposed");
                }
                if (encrypted == nullptr)
                {
                    throw gcnew ArgumentNullException("encrypted cannot be null");
                }
                try
                {
                    return gcnew ChooserPoly(chooserEncryptor_->decrypt(encrypted->GetChooserPoly()));
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

            seal::ChooserEncryptor &ChooserEncryptor::GetEncryptor()
            {
                if (chooserEncryptor_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEncryptor is disposed");
                }
                return *chooserEncryptor_;
            }
        }
    }
}