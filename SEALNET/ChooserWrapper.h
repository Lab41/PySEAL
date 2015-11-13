#pragma once

#include "encryptionparams.h"
#include "simulator.h"
#include "encoder.h"
#include "chooser.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class BigUInt;

            ref class Simulation;

            ref class EncryptionParameters;

            public ref class ChooserPoly
            {
            public:
                ChooserPoly();

                ChooserPoly(int maxCoeffCount, BigUInt ^maxAbsValue);

                ChooserPoly(int maxCoeffCount, System::UInt64 maxAbsValue);

                ~ChooserPoly();

                !ChooserPoly();

                ChooserPoly(ChooserPoly ^copy);

                void Set(ChooserPoly ^assign);

                property int MaxCoeffCount
                {
                    int get();

                    void set(int value);
                }

                property BigUInt ^MaxAbsValue
                {
                    BigUInt ^get();

                    void set(BigUInt ^value);
                }

                bool TestParameters(EncryptionParameters ^parms);

                Simulation ^Simulate(EncryptionParameters ^parms);

                EncryptionParameters ^SelectParameters();

                void Reset();

                void SetFresh();

                BigUInt ^Noise(EncryptionParameters ^parms);

                BigUInt ^MaxNoise(EncryptionParameters ^parms);

                int NoiseBits(EncryptionParameters ^parms);

                int NoiseBitsLeft(EncryptionParameters ^parms);

                bool Decrypts(EncryptionParameters ^parms);

                seal::ChooserPoly &GetChooserPoly();

            internal:
                ChooserPoly(const seal::ChooserPoly &value);

            private:
                seal::ChooserPoly *chooserPoly_;
            };

            public ref class ChooserEvaluator
            {
            public:
                ChooserEvaluator();

                ~ChooserEvaluator();

                !ChooserEvaluator();

                ChooserPoly ^TreeMultiply(System::Collections::Generic::List<ChooserPoly^> ^operands);

                ChooserPoly ^Add(ChooserPoly ^operand1, ChooserPoly ^operand2);

                ChooserPoly ^Sub(ChooserPoly ^operand1, ChooserPoly ^operand2);

                ChooserPoly ^Multiply(ChooserPoly ^operand1, ChooserPoly ^operand2);

                ChooserPoly ^MultiplyNoRelin(ChooserPoly ^operand1, ChooserPoly ^operand2);

                ChooserPoly ^Relinearize(ChooserPoly ^operand);

                ChooserPoly ^MultiplyPlain(ChooserPoly ^operand, int plainMaxCoeffCount, BigUInt ^plainMaxAbsValue);

                ChooserPoly ^MultiplyPlain(ChooserPoly ^operand, int plainMaxCoeffCount, System::UInt64 plainMaxAbsValue);

                ChooserPoly ^MultiplyPlain(ChooserPoly ^operand, ChooserPoly ^plainChooserPoly);

                ChooserPoly ^AddPlain(ChooserPoly ^operand, int plainMaxCoeffCount, BigUInt ^plainMaxAbsValue);

                ChooserPoly ^AddPlain(ChooserPoly ^operand, int plainMaxCoeffCount, System::UInt64 plainMaxAbsValue);

                ChooserPoly ^AddPlain(ChooserPoly ^operand, ChooserPoly ^plainChooserPoly);

                ChooserPoly ^SubPlain(ChooserPoly ^operand, int plainMaxCoeffCount, BigUInt ^plainMaxAbsValue);

                ChooserPoly ^SubPlain(ChooserPoly ^operand, int plainMaxCoeffCount, System::UInt64 plainMaxAbsValue);

                ChooserPoly ^SubPlain(ChooserPoly ^operand, ChooserPoly ^plainChooserPoly);

                ChooserPoly ^BinaryExponentiate(ChooserPoly ^operand, int exponent);

                ChooserPoly ^TreeExponentiate(ChooserPoly ^operand, int exponent);

                ChooserPoly ^Negate(ChooserPoly ^operand);

                seal::ChooserEvaluator &GetEvaluator();

            private:
                seal::ChooserEvaluator *chooserEvaluator_;
            };

            public ref class ChooserEncoder
            {
            public:
                ChooserEncoder();

                ChooserEncoder(int base);

                ~ChooserEncoder();

                !ChooserEncoder();

                ChooserPoly ^Encode(System::UInt64 value);

                void Encode(System::UInt64 value, ChooserPoly ^destination);

                ChooserPoly ^Encode(System::Int64 value);

                void Encode(System::Int64 value, ChooserPoly ^destination);

                ChooserPoly ^Encode(BigUInt ^value);

                void Encode(BigUInt ^value, ChooserPoly ^destination);

                ChooserPoly ^Encode(System::Int32 value);

                ChooserPoly ^Encode(System::UInt32 value);

                property int Base
                {
                    int get();

                    void set(int value);
                }

                seal::ChooserEncoder &GetEncoder();

            private:
                seal::ChooserEncoder *chooserEncoder_;
            };

            public ref class ChooserEncryptor
            {
            public:
                ChooserEncryptor();

                ~ChooserEncryptor();

                !ChooserEncryptor();

                void Encrypt(ChooserPoly ^plain, ChooserPoly ^destination);

                inline ChooserPoly ^Encrypt(ChooserPoly ^plain);

                void Decrypt(ChooserPoly ^encrypted, ChooserPoly ^destination);

                inline ChooserPoly ^Decrypt(ChooserPoly ^encrypted);

                seal::ChooserEncryptor &GetEncryptor();

            private:
                seal::ChooserEncryptor *chooserEncryptor_;
            };
        }
    }
}