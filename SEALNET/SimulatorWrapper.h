#pragma once

#include "simulator.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class EncryptionParameters;

            ref class BigUInt;

            public ref class Simulation
            {
            public:
                Simulation(EncryptionParameters ^parms);

                Simulation(EncryptionParameters ^parms, BigUInt ^noise);

                Simulation(const seal::Simulation &copy);

                void Set(Simulation ^assign);

                property BigUInt ^Noise
                {
                    BigUInt ^get();
                }

                property BigUInt ^MaxNoise
                {
                    BigUInt ^get();
                }

                property int NoiseBits
                {
                    int get();
                }

                property int NoiseBitsLeft
                {
                    int get();
                }

                property BigUInt ^CoeffModulus
                {
                    BigUInt ^get();
                }

                property BigUInt ^PlainModulus
                {
                    BigUInt ^get();
                }

                bool Decrypts();

                System::String ^ToString() override;

                ~Simulation();

                !Simulation();

                seal::Simulation &GetSimulation();

            internal:

            private:
                seal::Simulation *simulation_;
            };

            public ref class SimulationEvaluator
            {
            public:
                SimulationEvaluator();

                ~SimulationEvaluator();

                !SimulationEvaluator();

                Simulation ^Relinearize(Simulation ^simulation);

                Simulation ^MultiplyNoRelin(Simulation ^simulation1, Simulation ^simulation2);

                Simulation ^Multiply(Simulation ^simulation1, Simulation ^simulation2);

                Simulation ^Add(Simulation ^simulation1, Simulation ^simulation2);

                Simulation ^Sub(Simulation ^simulation1, Simulation ^simulation2);

                Simulation ^MultiplyPlain(Simulation ^simulation, int plainMaxCoeffCount, BigUInt ^plainMaxAbsValue);

                Simulation ^MultiplyPlain(Simulation ^simulation, int plainMaxCoeffCount, System::UInt64 plainMaxAbsValue);

                Simulation ^AddPlain(Simulation ^simulation);

                Simulation ^SubPlain(Simulation ^simulation);

                Simulation ^TreeMultiply(System::Collections::Generic::List<Simulation^> ^simulations);

                Simulation ^TreeExponentiate(Simulation ^simulation, int exponent);

                Simulation ^BinaryExponentiate(Simulation ^simulation, int exponent);

                Simulation ^Negate(Simulation ^simulation);

            private:
                seal::SimulationEvaluator *simulationEvaluator_;
            };
        }
    }
}
