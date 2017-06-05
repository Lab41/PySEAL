#include <stdexcept>
#include "ChooserWrapper.h"
#include "Common.h"
#include "BigUIntWrapper.h"
#include "SimulatorWrapper.h"
#include "EncryptionParamsWrapper.h"
#include <map>

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
                    GC::KeepAlive(maxAbsValue);
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

            ChooserPoly::ChooserPoly(int maxCoeffCount, UInt64 maxAbsValue) : chooserPoly_(nullptr)
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
                if (assign == nullptr)
                {
                    throw gcnew ArgumentNullException("assign cannot be null");
                }
                try
                {
                    *chooserPoly_ = assign->GetChooserPoly();
                    GC::KeepAlive(assign);
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
                if (value == nullptr)
                {
                    throw gcnew ArgumentNullException("value cannot be null");
                }
                try
                {
                    chooserPoly_->max_abs_value() = value->GetUInt();
                    GC::KeepAlive(value);
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

            bool ChooserPoly::TestParameters(EncryptionParameters ^parms, int budgetGap)
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
                    auto result = chooserPoly_->test_parameters(parms->GetParameters(), budgetGap);
                    GC::KeepAlive(parms);
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
                    auto result = gcnew Simulation(chooserPoly_->simulate(parms->GetParameters()));
                    GC::KeepAlive(parms);
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

            bool ChooserEvaluator::SelectParameters(List<ChooserPoly^> ^operands, int budgetGap, double noiseStandardDeviation, double noiseMaxDeviation, Dictionary<int, BigUInt^> ^parameterOptions, EncryptionParameters ^destination)
            {
                if (chooserEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEvaluator is disposed");
                }
                if (operands == nullptr)
                {
                    throw gcnew ArgumentNullException("operands cannot be null");
                }
                if (parameterOptions == nullptr)
                {
                    throw gcnew ArgumentNullException("parameterOptions cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    vector<seal::ChooserPoly> operands_vector;
                    for each (ChooserPoly ^operand in operands)
                    {
                        if (operand == nullptr)
                        {
                            throw gcnew ArgumentNullException("operand cannot be null");
                        }
                        operands_vector.emplace_back(operand->GetChooserPoly());
                        GC::KeepAlive(operand);
                    }

                    map<int, seal::BigUInt> parameter_options_map;
                    for each (KeyValuePair<int, BigUInt^> ^paramPair in parameterOptions)
                    {
                        if (paramPair->Value == nullptr)
                        {
                            throw gcnew ArgumentNullException("parameterOptions cannot contain null values");
                        }
                        parameter_options_map[paramPair->Key] = paramPair->Value->GetUInt();
                        GC::KeepAlive(paramPair);
                    }

                    auto result = chooserEvaluator_->select_parameters(operands_vector, budgetGap, noiseStandardDeviation, noiseMaxDeviation, parameter_options_map, destination->GetParameters());
                    GC::KeepAlive(destination);
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

            bool ChooserEvaluator::SelectParameters(List<ChooserPoly^> ^operands, int budgetGap, EncryptionParameters ^destination)
            {
                if (chooserEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEvaluator is disposed");
                }
                if (operands == nullptr)
                {
                    throw gcnew ArgumentNullException("operands cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    vector<seal::ChooserPoly> operands_vector;
                    for each (ChooserPoly ^operand in operands)
                    {
                        if (operand == nullptr)
                        {
                            throw gcnew ArgumentNullException("operand cannot be null");
                        }
                        operands_vector.emplace_back(operand->GetChooserPoly());
                        GC::KeepAlive(operand);
                    }

                    auto result = chooserEvaluator_->select_parameters(operands_vector, budgetGap, destination->GetParameters());
                    GC::KeepAlive(destination);
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

            Dictionary<int, BigUInt^> ^ChooserEvaluator::DefaultParameterOptions::get()
            {
                Dictionary<int, BigUInt^> ^defaultParametersDict = gcnew Dictionary<int, BigUInt^>;

                // Copy the key-value pairs from the map to the Dictionary.
                for (pair<int, seal::BigUInt> param_pair : seal::ChooserEvaluator::default_parameter_options())
                {
                    defaultParametersDict->Add(param_pair.first, gcnew BigUInt(param_pair.second));
                }
                return defaultParametersDict;
            }

            double ChooserEvaluator::DefaultNoiseStandardDeviation::get()
            {
                return seal::ChooserEvaluator::default_noise_standard_deviation();
            }

            double ChooserEvaluator::DefaultNoiseMaxDeviation::get()
            {
                return seal::ChooserEvaluator::default_noise_max_deviation();
            }

            ChooserEvaluator::ChooserEvaluator() : chooserEvaluator_(nullptr)
            {
                chooserEvaluator_ = new seal::ChooserEvaluator;
            }

            ChooserEvaluator::ChooserEvaluator(MemoryPoolHandle ^pool) : chooserEvaluator_(nullptr)
            {
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                chooserEvaluator_ = new seal::ChooserEvaluator(pool->GetHandle());
                GC::KeepAlive(pool);
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
                    auto result = gcnew ChooserPoly(chooserEvaluator_->multiply(operand1->GetChooserPoly(), operand2->GetChooserPoly()));
                    GC::KeepAlive(operand1);
                    GC::KeepAlive(operand2);
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

            ChooserPoly ^ChooserEvaluator::Square(ChooserPoly ^operand)
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
                    auto result = gcnew ChooserPoly(chooserEvaluator_->square(operand->GetChooserPoly()));
                    GC::KeepAlive(operand);
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
                    auto result = gcnew ChooserPoly(chooserEvaluator_->relinearize(operand->GetChooserPoly()));
                    GC::KeepAlive(operand);
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

            ChooserPoly ^ChooserEvaluator::Relinearize(ChooserPoly ^operand, int destinationSize)
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
                    auto result = gcnew ChooserPoly(chooserEvaluator_->relinearize(operand->GetChooserPoly(), destinationSize));
                    GC::KeepAlive(operand);
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
                    auto result = gcnew ChooserPoly(chooserEvaluator_->add(operand1->GetChooserPoly(), operand2->GetChooserPoly()));
                    GC::KeepAlive(operand1);
                    GC::KeepAlive(operand2);
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

            ChooserPoly ^ChooserEvaluator::AddMany(List<ChooserPoly^> ^operands)
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
                    vector<seal::ChooserPoly> v_simulations;
                    for each (ChooserPoly ^operand in operands)
                    {
                        if (operand == nullptr)
                        {
                            throw gcnew ArgumentNullException("operand cannot be null");
                        }
                        v_simulations.emplace_back(operand->GetChooserPoly());
                        GC::KeepAlive(operand);
                    }

                    return gcnew ChooserPoly(chooserEvaluator_->add_many(v_simulations));
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
                    auto result = gcnew ChooserPoly(chooserEvaluator_->sub(operand1->GetChooserPoly(), operand2->GetChooserPoly()));
                    GC::KeepAlive(operand1);
                    GC::KeepAlive(operand2);
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
                if (plainMaxAbsValue == nullptr)
                {
                    throw gcnew ArgumentNullException("plainMaxAbsValue cannot be null");
                }
                try
                {
                    auto result = gcnew ChooserPoly(chooserEvaluator_->multiply_plain(operand->GetChooserPoly(), plainMaxCoeffCount, plainMaxAbsValue->GetUInt()));
                    GC::KeepAlive(operand);
                    GC::KeepAlive(plainMaxAbsValue);
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
                    auto result = gcnew ChooserPoly(chooserEvaluator_->multiply_plain(operand->GetChooserPoly(), plainMaxCoeffCount, plainMaxAbsValue));
                    GC::KeepAlive(operand);
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
                    auto result = gcnew ChooserPoly(chooserEvaluator_->multiply_plain(operand->GetChooserPoly(), plainChooserPoly->GetChooserPoly()));
                    GC::KeepAlive(operand);
                    GC::KeepAlive(plainChooserPoly);
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
                    auto result = gcnew ChooserPoly(chooserEvaluator_->add_plain(operand->GetChooserPoly(), plainMaxCoeffCount, plainMaxAbsValue->GetUInt()));
                    GC::KeepAlive(operand);
                    GC::KeepAlive(plainMaxAbsValue);
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
                    auto result = gcnew ChooserPoly(chooserEvaluator_->add_plain(operand->GetChooserPoly(), plainMaxCoeffCount, plainMaxAbsValue));
                    GC::KeepAlive(operand);
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
                    auto result = gcnew ChooserPoly(chooserEvaluator_->add_plain(operand->GetChooserPoly(), plainChooserPoly->GetChooserPoly()));
                    GC::KeepAlive(operand);
                    GC::KeepAlive(plainChooserPoly);
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
                    auto result = gcnew ChooserPoly(chooserEvaluator_->sub_plain(operand->GetChooserPoly(), plainMaxCoeffCount, plainMaxAbsValue->GetUInt()));
                    GC::KeepAlive(operand);
                    GC::KeepAlive(plainMaxAbsValue);
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
                    auto result = gcnew ChooserPoly(chooserEvaluator_->sub_plain(operand->GetChooserPoly(), plainMaxCoeffCount, plainMaxAbsValue));
                    GC::KeepAlive(operand);
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
                    auto result = gcnew ChooserPoly(chooserEvaluator_->sub_plain(operand->GetChooserPoly(), plainChooserPoly->GetChooserPoly()));
                    GC::KeepAlive(operand);
                    GC::KeepAlive(plainChooserPoly);
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

            ChooserPoly ^ChooserEvaluator::MultiplyMany(List<ChooserPoly^> ^operands)
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
                        v_operands.emplace_back(operand->GetChooserPoly());
                        GC::KeepAlive(operand);
                    }

                    return gcnew ChooserPoly(chooserEvaluator_->multiply_many(v_operands));
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

            ChooserPoly ^ChooserEvaluator::Exponentiate(ChooserPoly ^operand, UInt64 exponent)
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
                    auto result = gcnew ChooserPoly(chooserEvaluator_->exponentiate(operand->GetChooserPoly(), exponent));
                    GC::KeepAlive(operand);
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
                    auto result = gcnew ChooserPoly(chooserEvaluator_->negate(operand->GetChooserPoly()));
                    GC::KeepAlive(operand);
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

            ChooserEncoder::ChooserEncoder(UInt64 base) : chooserEncoder_(nullptr)
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

            ChooserPoly ^ChooserEncoder::Encode(UInt64 value)
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

            void ChooserEncoder::Encode(UInt64 value, ChooserPoly ^destination)
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

            ChooserPoly ^ChooserEncoder::Encode(Int64 value)
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

            void ChooserEncoder::Encode(Int64 value, ChooserPoly ^destination)
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

            ChooserPoly ^ChooserEncoder::Encode(BigUInt ^value)
            {
                if (chooserEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEncoder is disposed");
                }
                if (value == nullptr)
                {
                    throw gcnew ArgumentNullException("value cannot be null");
                }
                try
                {
                    auto result = gcnew ChooserPoly(chooserEncoder_->encode(value->GetUInt()));
                    GC::KeepAlive(value);
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
                if (value == nullptr)
                {
                    throw gcnew ArgumentNullException("value cannot be null");
                }
                try
                {
                    chooserEncoder_->encode(value->GetUInt(), destination->GetChooserPoly());
                    GC::KeepAlive(value);
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

            ChooserPoly ^ChooserEncoder::Encode(Int32 value)
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

            ChooserPoly ^ChooserEncoder::Encode(UInt32 value)
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

            UInt64 ChooserEncoder::Base::get()
            {
                if (chooserEncoder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("ChooserEncoder is disposed");
                }
                return chooserEncoder_->base();
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

            ChooserPoly ^ChooserEncryptor::Encrypt(ChooserPoly ^plain)
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
                    auto result = gcnew ChooserPoly(chooserEncryptor_->encrypt(plain->GetChooserPoly()));
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

            ChooserPoly ^ChooserEncryptor::Decrypt(ChooserPoly ^encrypted)
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
                    auto result =gcnew ChooserPoly(chooserEncryptor_->decrypt(encrypted->GetChooserPoly()));
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