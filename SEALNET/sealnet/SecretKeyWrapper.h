#pragma once

#include "seal/secretkey.h"
#include "sealnet/BigPolyWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class KeyGenerator;

            ref class PublicKey;

            ref class EvaluationKeys;

            ref class GaloisKeys;

            /**
            <summary>Class to store a secret key.</summary>

            <remarks>
            <para>
            Class to store a secret key. Internally, the secret key is represented
            by a BigPoly object, and is created by KeyGenerator.
            </para>
            <para>
            In general, reading from SecretKey is thread-safe as long as no other
            thread is concurrently mutating it. This is due to the underlying data
            structure storing the secret key not being thread-safe.
            </para>
            </remarks>
            <seealso cref="KeyGenerator">See KeyGenerator for the class that generates
            the public key.</seealso>
            <seealso cref="PublicKey">See PublicKey for the class that stores the public
            key.</seealso>
            <seealso cref="EvaluationKeys">See EvaluationKeys for the class that stores
            the evaluation keys.</seealso>
            <seealso cref="GaloisKeys">See GaloisKeys for the class that stores the Galois
            keys.</seealso>
            */
            public ref class SecretKey
            {
            public:
                /**
                <summary>Creates an empty secret key.</summary>
                */
                SecretKey();

                /**
                <summary>Creates a new SecretKey by copying an old one.</summary>

                <param name="copy">The SecretKey to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                SecretKey(SecretKey ^copy);

                /**
                <summary>Copies an old SecretKey to the current one.</summary>

                <param name="assign">The SecretKey to copy from</param>
                <exception cref="System::ArgumentNullException">if assign is null</exception>
                */
                void Set(SecretKey ^assign);

                /**
                <summary>Returns a copy of the underlying polynomial.</summary>
                */
                property BigPoly ^Data {
                    BigPoly ^get();
                };

                /**
                <summary>Saves the SecretKey to an output stream.</summary>

                <remarks>
                Saves the SecretKey to an output stream. The output is in binary format and 
                not human-readable. The output stream must have the "binary" flag set.
                </remarks>
                <param name="stream">The stream to save the SecretKey to</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                */
                void Save(System::IO::Stream ^stream);

                /**
                <summary>Loads a SecretKey from an input stream overwriting the current 
                SecretKey.</summary>

                <param name="stream">The stream to load the SecretKey from</param>
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
                <summary>Destroys the SecretKey.</summary>
                */
                ~SecretKey();

                /**
                <summary>Destroys the SecretKey.</summary>
                */
                !SecretKey();

            internal:
                /**
                <summary>Creates a deep copy of a C++ SecretKey instance.</summary>
                <param name="value">The SecretKey instance to copy from</param>
                <exception cref="System::ArgumentNullException">If copy is null</exception>
                */
                SecretKey(const seal::SecretKey &value);

                /**
                <summary>Returns a reference to the underlying C++ SecretKey.</summary>
                */
                seal::SecretKey &GetKey();

            private:
                seal::SecretKey *sk_;
            };
        }
    }
}
