#include "SimulatorWrapper.h"
#include "EncryptionParamsWrapper.h"
#include "BigUIntWrapper.h"
#include "Common.h"
#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>

using namespace System;
using namespace System::Collections::Generic;
using namespace std;
using namespace msclr::interop;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            SimulationEvaluator::SimulationEvaluator() : simulationEvaluator_(nullptr)
            {
                simulationEvaluator_ = new seal::SimulationEvaluator();
            }

            SimulationEvaluator::~SimulationEvaluator()
            {
                this->!SimulationEvaluator();
            }

            SimulationEvaluator::!SimulationEvaluator()
            {
                if (simulationEvaluator_ != nullptr)
                {
                    delete simulationEvaluator_;
                    simulationEvaluator_ = nullptr;
                }
            }

            Simulation ^SimulationEvaluator::Relinearize(Simulation ^simulation)
            {
                if (simulationEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SimulationEvaluator is disposed");
                }
                if (simulation == nullptr)
                {
                    throw gcnew ArgumentNullException("simulation cannot be null");
                }
                try
                {
                    return gcnew Simulation(simulationEvaluator_->relinearize(simulation->GetSimulation()));
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
            
            Simulation ^SimulationEvaluator::Relinearize(Simulation ^simulation, int destinationSize)
            {
                if (simulationEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SimulationEvaluator is disposed");
                }
                if (simulation == nullptr)
                {
                    throw gcnew ArgumentNullException("simulation cannot be null");
                }
                try
                {
                    return gcnew Simulation(simulationEvaluator_->relinearize(simulation->GetSimulation(), destinationSize));
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

            Simulation ^SimulationEvaluator::Multiply(Simulation ^simulation1, Simulation ^simulation2)
            {
                if (simulationEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SimulationEvaluator is disposed");
                }
                if (simulation1 == nullptr)
                {
                    throw gcnew ArgumentNullException("simulation1 cannot be null");
                }
                if (simulation2 == nullptr)
                {
                    throw gcnew ArgumentNullException("simulation2 cannot be null");
                }
                try
                {
                    auto result = gcnew Simulation(simulationEvaluator_->multiply(simulation1->GetSimulation(), simulation2->GetSimulation()));
                    GC::KeepAlive(simulation1);
                    GC::KeepAlive(simulation2);
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

            Simulation ^SimulationEvaluator::Add(Simulation ^simulation1, Simulation ^simulation2)
            {
                if (simulationEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SimulationEvaluator is disposed");
                }
                if (simulation1 == nullptr)
                {
                    throw gcnew ArgumentNullException("simulation1 cannot be null");
                }
                if (simulation2 == nullptr)
                {
                    throw gcnew ArgumentNullException("simulation2 cannot be null");
                }
                try
                {
                    auto result = gcnew Simulation(simulationEvaluator_->add(simulation1->GetSimulation(), simulation2->GetSimulation()));
                    GC::KeepAlive(simulation1);
                    GC::KeepAlive(simulation2);
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

            Simulation ^SimulationEvaluator::AddMany(List<Simulation^> ^simulations)
            {
                if (simulationEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SimulationEvaluator is disposed");
                }
                if (simulations == nullptr)
                {
                    throw gcnew ArgumentNullException("simulations cannot be null");
                }
                try
                {
                    vector<seal::Simulation> v_simulations;
                    for each (Simulation ^simulation in simulations)
                    {
                        if (simulation == nullptr)
                        {
                            throw gcnew ArgumentNullException("simulations cannot be null");
                        }
                        v_simulations.push_back(simulation->GetSimulation());
                        GC::KeepAlive(simulation);
                    }

                    return gcnew Simulation(simulationEvaluator_->add_many(v_simulations));
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

            Simulation ^SimulationEvaluator::Sub(Simulation ^simulation1, Simulation ^simulation2)
            {
                if (simulationEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SimulationEvaluator is disposed");
                }
                if (simulation1 == nullptr)
                {
                    throw gcnew ArgumentNullException("simulation1 cannot be null");
                }
                if (simulation2 == nullptr)
                {
                    throw gcnew ArgumentNullException("simulation2 cannot be null");
                }
                try
                {
                    auto result = gcnew Simulation(simulationEvaluator_->sub(simulation1->GetSimulation(), simulation2->GetSimulation()));
                    GC::KeepAlive(simulation1);
                    GC::KeepAlive(simulation2);
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

            Simulation ^SimulationEvaluator::MultiplyPlain(Simulation ^simulation, int plainMaxCoeffCount, BigUInt ^plainMaxAbsValue)
            {
                if (simulationEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SimulationEvaluator is disposed");
                }
                if (simulation == nullptr)
                {
                    throw gcnew ArgumentNullException("simulation cannot be null");
                }
                if (plainMaxAbsValue == nullptr)
                {
                    throw gcnew ArgumentNullException("plainMaxAbsValue cannot be null");
                }
                try
                {
                   auto result = gcnew Simulation(simulationEvaluator_->multiply_plain(simulation->GetSimulation(), plainMaxCoeffCount, plainMaxAbsValue->GetUInt()));
                   GC::KeepAlive(simulation);
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

            Simulation ^SimulationEvaluator::MultiplyPlain(Simulation ^simulation, int plainMaxCoeffCount, UInt64 plainMaxAbsValue)
            {
                if (simulationEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SimulationEvaluator is disposed");
                }
                if (simulation == nullptr)
                {
                    throw gcnew ArgumentNullException("simulation cannot be null");
                }
                try
                {
                    auto result = gcnew Simulation(simulationEvaluator_->multiply_plain(simulation->GetSimulation(), plainMaxCoeffCount, plainMaxAbsValue));
                    GC::KeepAlive(simulation);
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

            Simulation ^SimulationEvaluator::AddPlain(Simulation ^simulation)
            {
                if (simulationEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SimulationEvaluator is disposed");
                }
                if (simulation == nullptr)
                {
                    throw gcnew ArgumentNullException("simulation cannot be null");
                }
                try
                {
                    auto result = gcnew Simulation(simulationEvaluator_->add_plain(simulation->GetSimulation()));
                    GC::KeepAlive(simulation);
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

            Simulation ^SimulationEvaluator::SubPlain(Simulation ^simulation)
            {
                if (simulationEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SimulationEvaluator is disposed");
                }
                if (simulation == nullptr)
                {
                    throw gcnew ArgumentNullException("simulation cannot be null");
                }
                try
                {
                    auto result = gcnew Simulation(simulationEvaluator_->sub_plain(simulation->GetSimulation()));
                    GC::KeepAlive(simulation);
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

            Simulation ^SimulationEvaluator::MultiplyMany(List<Simulation^> ^simulations)
            {
                if (simulationEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SimulationEvaluator is disposed");
                }
                if (simulations == nullptr)
                {
                    throw gcnew ArgumentNullException("simulations cannot be null");
                }
                try
                {
                    vector<seal::Simulation> v_simulations;
                    for each (Simulation ^simulation in simulations)
                    {
                        if (simulation == nullptr)
                        {
                            throw gcnew ArgumentNullException("simulations cannot be null");
                        }
                        v_simulations.push_back(simulation->GetSimulation());
                        GC::KeepAlive(simulation);
                    }

                    return gcnew Simulation(simulationEvaluator_->multiply_many(v_simulations));
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

            Simulation ^SimulationEvaluator::Exponentiate(Simulation ^simulation, UInt64 exponent)
            {
                if (simulationEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SimulationEvaluator is disposed");
                }
                if (simulation == nullptr)
                {
                    throw gcnew ArgumentNullException("simulation cannot be null");
                }
                try
                {
                    auto result = gcnew Simulation(simulationEvaluator_->exponentiate(simulation->GetSimulation(), exponent));
                    GC::KeepAlive(simulation);
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

            Simulation ^SimulationEvaluator::Negate(Simulation ^simulation)
            {
                if (simulationEvaluator_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SimulationEvaluator is disposed");
                }
                if (simulation == nullptr)
                {
                    throw gcnew ArgumentNullException("simulation cannot be null");
                }
                try
                {
                    auto result = gcnew Simulation(simulationEvaluator_->negate(simulation->GetSimulation()));
                    GC::KeepAlive(simulation);
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

            void Simulation::Set(Simulation ^assign)
            {
                if (assign == nullptr)
                {
                    throw gcnew ArgumentNullException("assign cannot be null");
                }
                try
                {
                    *simulation_ = assign->GetSimulation();
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

            BigUInt ^Simulation::Noise::get()
            {
                if (simulation_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Simulation is disposed");
                }
                return gcnew BigUInt(simulation_->noise());
            }

            BigUInt ^Simulation::MaxNoise::get()
            {
                if (simulation_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Simulation is disposed");
                }
                return gcnew BigUInt(simulation_->max_noise());
            }

            int Simulation::NoiseBits::get()
            {
                if (simulation_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Simulation is disposed");
                }
                return simulation_->noise_bits();
            }

            int Simulation::NoiseBitsLeft::get()
            {
                if (simulation_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Simulation is disposed");
                }
                return simulation_->noise_bits_left();
            }

            int Simulation::Size::get()
            {
                if (simulation_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Simulation is disposed");
                }
                return simulation_->size();
            }

            BigUInt ^Simulation::CoeffModulus::get()
            {
                if (simulation_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Simulation is disposed");
                }
                return gcnew BigUInt(simulation_->coeff_modulus());
            }

            BigUInt ^Simulation::PlainModulus::get()
            {
                if (simulation_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Simulation is disposed");
                }
                return gcnew BigUInt(simulation_->plain_modulus());
            }

            bool Simulation::Decrypts()
            {
                if (simulation_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Simulation is disposed");
                }
                try
                {
                    return simulation_->decrypts();
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

            bool Simulation::Decrypts(int noiseGap)
            {
                if (simulation_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Simulation is disposed");
                }
                try
                {
                    return simulation_->decrypts(noiseGap);
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

            Simulation::Simulation(EncryptionParameters ^parms) : simulation_(nullptr)
            {
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                try
                {
                    simulation_ = new seal::Simulation(parms->GetParameters());
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

            Simulation::Simulation(EncryptionParameters ^parms, BigUInt ^noise, int ciphertextSize)
            {
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                if (noise == nullptr)
                {
                    throw gcnew ArgumentNullException("noise cannot be null");
                }
                try
                {
                    simulation_ = new seal::Simulation(parms->GetParameters(), noise->GetUInt(), ciphertextSize);
                    GC::KeepAlive(parms);
                    GC::KeepAlive(noise);
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

            Simulation::Simulation(const seal::Simulation &copy) : simulation_(nullptr)
            {
                try
                {
                    simulation_ = new seal::Simulation(copy);
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

            seal::Simulation &Simulation::GetSimulation()
            {
                if (simulation_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Simulation is disposed");
                }
                return *simulation_;
            }

            Simulation::~Simulation()
            {
                this->!Simulation();
            }

            Simulation::!Simulation()
            {
                if (simulation_ != nullptr)
                {
                    delete simulation_;
                    simulation_ = nullptr;
                }
            }
        }
    }
}