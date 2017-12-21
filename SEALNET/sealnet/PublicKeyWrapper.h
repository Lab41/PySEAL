#pragma once

#include "seal/publickey.h"
#include "sealnet/BigPolyArrayWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class KeyGenerator;

            ref class SecretKey;
            
            ref class EvaluationKeys;

            ref class GaloisKeys;

            /**
            <summary>Class to store a public key.</summary> 

            <remarks>
            <para>
            Class to store a public key. Internally, the public key is represented by
            a BigPolyArray object, and is created by KeyGenerator.
            </para>
            <para>
            In general, reading from PublicKey is thread-safe as long as no other thread
            is concurrently mutating it. This is due to the underlying data structure
            storing the public key not being thread-safe.
            </para>
            </remarks>
            <seealso cref="KeyGenerator">See KeyGenerator for the class that generates
            the public key.</seealso>
            <seealso cref="SecretKey">See SecretKey for the class that stores the secret
            key.</seealso>
            <seealso cref="EvaluationKeys">See EvaluationKeys for the class that stores 
            the evaluation keys.</seealso>
            <seealso cref="GaloisKeys">See GaloisKeys for the class that stores the Galois 
            keys.</seealso>
            */
            public ref class PublicKey
            {
            public:
                /**
                <summary>Creates an empty public key.</summary>
                */
                PublicKey();

                /**
                <summary>Creates a new PublicKey by copying an old one.</summary>

                <param name="copy">The PublicKey to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                PublicKey(PublicKey ^copy);

                /**
                <summary>Copies an old PublicKey to the current one.</summary>

                <param name="assign">The PublicKey to copy from</param>
                <exception cref="System::ArgumentNullException">if assign is null</exception>
                */
                void Set(PublicKey ^assign);

                /**
                <summary>Returns a copy of the underlying polynomial array.</summary>
                */
                property BigPolyArray ^Data {
                    BigPolyArray ^get();
                };

                /**
                <summary>Saves the PublicKey to an output stream.</summary>

                <remarks>
                Saves the PublicKey to an output stream. The output is in binary format and 
                not human-readable. The output stream must have the "binary" flag set.
                </remarks>
                <param name="stream">The stream to save the PublicKey to</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                */
                void Save(System::IO::Stream ^stream);

                /**
                <summary>Loads a PublicKey from an input stream overwriting the current 
                PublicKey.</summary>

                <param name="stream">The stream to load the PublicKey from</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                */
                void Load(System::IO::Stream ^stream);

                /**
                <summary>Returns a copy of the hash block.</summary>
                */
                property System::Tuple<System::UInt64, System::UInt64, System::UInt64, System::UInt64> ^HashBlock {
                    System::Tuple<System::UInt64, System::UInt64, System::UInt64, System::UInt64> ^get();
                }

                /**
                <summary>Destroys the PublicKey.</summary>
                */
                ~PublicKey();

                /**
                <summary>Destroys the PublicKey.</summary>
                */
                !PublicKey();

            internal:
                /**
                <summary>Creates a deep copy of a C++ PublicKey instance.</summary>

                <param name="value">The PublicKey instance to copy from</param>
                <exception cref="System::ArgumentNullException">If copy is null</exception>
                */
                PublicKey(const seal::PublicKey &value);

                /**
                <summary>Returns a reference to the underlying C++ PublicKey.</summary>
                */
                seal::PublicKey &GetKey();

            private:
                seal::PublicKey *pk_;
            };
        }
    }
}
