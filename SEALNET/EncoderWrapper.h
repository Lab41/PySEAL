#pragma once

#include "encoder.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class BigPoly;

            ref class BigUInt;

            public ref class BinaryEncoder
            {
            public:
                BinaryEncoder(BigUInt ^plainModulus);

                ~BinaryEncoder();

                !BinaryEncoder();

                property BigUInt ^PlainModulus
                {
                    BigUInt ^get();

                    void set(BigUInt ^value);
                }

                BigPoly ^Encode(System::UInt64 value);

                void Encode(System::UInt64 value, BigPoly ^destination);

                BigPoly ^Encode(BigUInt ^value);

                void Encode(BigUInt ^value, BigPoly ^destination);

                BigPoly ^Encode(System::Int64 value);

                void Encode(System::Int64 value, BigPoly ^destination);

                System::UInt64 DecodeUInt64(BigPoly ^poly);

                System::UInt32 DecodeUInt32(BigPoly ^poly);

                System::Int64 DecodeInt64(BigPoly ^poly);

                System::Int32 DecodeInt32(BigPoly ^poly);

                BigUInt ^DecodeBigUInt(BigPoly ^poly);

                void DecodeBigUInt(BigPoly ^poly, BigUInt ^destination);

                BigPoly ^Encode(System::Int32 value);

                BigPoly ^Encode(System::UInt32 value);

                seal::BinaryEncoder &GetEncoder();

            private:
                seal::BinaryEncoder *binaryEncoder_;
            };

            public ref class BalancedEncoder
            {
            public:
                BalancedEncoder(BigUInt ^plainModulus);

                BalancedEncoder(BigUInt ^plainModulus, int base);

                ~BalancedEncoder();

                !BalancedEncoder();

                property BigUInt ^PlainModulus
                {
                    BigUInt ^get();

                    void set(BigUInt ^value);
                }

                property int Base
                {
                    int get();

                    void set(int base);
                }

                BigPoly ^Encode(System::UInt64 value);

                void Encode(System::UInt64 value, BigPoly ^destination);

                BigPoly ^Encode(BigUInt ^value);

                void Encode(BigUInt ^value, BigPoly ^destination);

                BigPoly ^Encode(System::Int64 value);

                void Encode(System::Int64 value, BigPoly ^destination);

                System::UInt64 DecodeUInt64(BigPoly ^poly);

                System::UInt32 DecodeUInt32(BigPoly ^poly);

                System::Int64 DecodeInt64(BigPoly ^poly);

                System::Int32 DecodeInt32(BigPoly ^poly);

                BigUInt ^DecodeBigUInt(BigPoly ^poly);

                void DecodeBigUInt(BigPoly ^poly, BigUInt ^destination);

                BigPoly ^Encode(System::Int32 value);

                BigPoly ^Encode(System::UInt32 value);

                seal::BalancedEncoder &GetEncoder();

            private:
                seal::BalancedEncoder *balancedEncoder_;
            };

            public ref class BinaryFractionalEncoder
            {
            public:
                BinaryFractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, int fractionCoeffCount);

                ~BinaryFractionalEncoder();

                !BinaryFractionalEncoder();

                BigPoly ^Encode(double value);

                double Decode(BigPoly ^value);

                property BigUInt ^PlainModulus
                {
                    BigUInt ^get();

                    void set(BigUInt ^value);
                }

                seal::BinaryFractionalEncoder &GetEncoder();

            private:
                seal::BinaryFractionalEncoder *fractionalEncoder_;
            };

            public ref class BalancedFractionalEncoder
            {
            public:
                BalancedFractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, int fractionCoeffCount);

                BalancedFractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, int fractionCoeffCount, int base);

                ~BalancedFractionalEncoder();

                !BalancedFractionalEncoder();

                BigPoly ^Encode(double value);

                double Decode(BigPoly ^value);

                property BigUInt ^PlainModulus
                {
                    BigUInt ^get();

                    void set(BigUInt ^value);
                }

                property int Base
                {
                    int get();

                    void set(int value);
                }

                seal::BalancedFractionalEncoder &GetEncoder();

            private:
                seal::BalancedFractionalEncoder *fractionalEncoder_;
            };
        }
    }
}