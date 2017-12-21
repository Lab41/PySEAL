#pragma once

#include "seal/smallmodulus.h"
#include "sealnet/EncryptionParamsWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class EncryptionParameters;

            /**
            <summary>Represent an integer modulus of up to 62 bits.</summary>

            <remarks>
            <para>
            Represent an integer modulus of up to 62 bits. An instance of the SmallModulus
            class represents a non-negative integer modulus up to 62 bits. In particular,
            the encryption parameter plain_modulus, and the primes in coeff_modulus, are
            represented by instances of SmallModulus. The purpose of this class is to
            perform and store the pre-computation required by Barrett reduction.
            </para>
            <para>
            In general, reading from SmallModulus is thread-safe as long as no other thread
            is  concurrently mutating it.
            </para>
            </remarks>
            <seealso cref="EncryptionParameters"/>See EncryptionParameters for a description 
            of the encryption parameters.</seealso>
            */
            public ref class SmallModulus : 
                System::IEquatable<SmallModulus^>, 
                System::IEquatable<System::UInt64>
            {
            public:
                /**
                <summary>Creates a SmallModulus instance.</summary>

                <remarks>
                Creates a SmallModulus instance. The value of the SmallModulus is set to 0.
                </remarks>
                */
                SmallModulus();

                /**
                <summary>Creates a SmallModulus instance.</summary>

                <remarks>
                Creates a SmallModulus instance. The value of the SmallModulus is set to
                the given value.
                </remarks>
                <param name="value">The integer modulus</param>
                <exception cref="System::ArgumentException">if value is 1 or more than
                62 bits</exception>
                */
                SmallModulus(System::UInt64 value);

                /**
                <summary>Creates a new SmallModulus by copying a given one.</summary>

                <param name="copy">The SmallModulus to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                SmallModulus(SmallModulus ^copy);

                /**
                <summary>Copies a given SmallModulus to the current one.</summary>

                <param name="assign">The SmallModulus to copy from</param>
                <exception cref="System::ArgumentNullException">if assign is null</exception>
                */
                void Set(SmallModulus ^assign);

                /**
                <summary>Sets the value of the SmallModulus.</summary>

                <param name="value">The new integer modulus</param>
                <exception cref="System::ArgumentException">if value is 1 or more than 
                62 bits</exception>
                */
                void Set(System::UInt64 value);

                /**
                <summary>Returns the significant bit count of the value of the current 
                SmallModulus.</summary>
                */
                property int BitCount {
                    int get();
                }

                /**
                <summary>Returns the size (in 64-bit words) of the value of the current 
                SmallModulus.</summary>
                */
                property int UInt64Count {
                    int get();
                }

                /**
                <summary>Returns the value of the current SmallModulus.</summary>
                */
                property System::UInt64 Value {
                    System::UInt64 get();
                }

                /**
                <summary>Returns the Barrett ratio computed for the value of the current 
                SmallModulus.</summary>

                <remarks>
                Returns the Barrett ratio computed for the value of the current SmallModulus.
                The first two components of the Barrett ratio are the floor of 2^128/value,
                and the third component is the remainder.
                </remarks>
                */
                property System::Tuple<System::UInt64, System::UInt64, System::UInt64> ^ConstRatio {
                    System::Tuple<System::UInt64, System::UInt64, System::UInt64> ^get();
                }

                /**
                <summary>Returns whether the value of the current SmallModulus is zero.</summary>
                */
                property bool IsZero {
                    bool get();
                }

                /**
                <summary>Saves the SmallModulus to an output stream.</summary>

                <remarks>
                Saves the SmallModulus to an output stream. The output is in binary format and 
                not human-readable. The output stream must have the "binary" flag set.
                </remarks>

                <param name="stream">The stream to save the SmallModulus to</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                */
                void Save(System::IO::Stream ^stream);

                /**
                <summary>Loads a SmallModulus from an input stream overwriting the current 
                SmallModulus.</summary>

                <param name="stream">The stream to load the SmallModulus from</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                */
                void Load(System::IO::Stream ^stream);

                /**
                <summary>Returns a hash-code based on the value of the SmallModulus.</summary>
                */
                int GetHashCode() override;

                /**
                <summary>Compares two SmallModulus instances.</summary>

                <param name="compare">The value to compare against</param>
                <exception cref="System::ArgumentNullException">If compare is null</exception>
                */
                virtual bool Equals(SmallModulus ^compare);

                /**
                <summary>Compares two SmallModulus instances.</summary>

                <param name="compare">The value to compare against</param>
                <exception cref="System::ArgumentNullException">If compare is null</exception>
                */
                bool Equals(System::Object ^compare) override;

                /**
                <summary>Compares value of the current SmallModulus to a given value.</summary>

                <param name="compare">The value to compare against</param>
                */
                virtual bool Equals(System::UInt64 compare);

                /**
                <summary>Destroys the SmallModulus.</summary>
                */
                ~SmallModulus();

                /**
                <summary>Destroys the SmallModulus.</summary>
                */
                !SmallModulus();

                /**
                <summary>Creates a SmallModulus instance.</summary>

                <remarks>
                Creates a SmallModulus instance. The value of the SmallModulus is set to
                the given value.
                </remarks>
                <param name="value">The integer modulus</param>
                <exception cref="System::ArgumentException">if value is 1 or more than
                62 bits</exception>
                */
                static operator SmallModulus ^(System::UInt64 value);

            internal:
                /**
                <summary>Creates a deep copy of a C++ SmallModulus instance.</summary>
                <param name="value">The SmallModulus instance to copy from</param>
                <exception cref="System::ArgumentNullException">If copy is null</exception>
                */
                SmallModulus(const seal::SmallModulus &value);

                /**
                <summary>Returns a reference to the underlying C++ SecretKey.</summary>
                */
                seal::SmallModulus &GetModulus();

            private:
                seal::SmallModulus *modulus_;
            };
        }
    }
}
