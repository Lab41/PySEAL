#pragma once

#include "seal/galoiskeys.h"
#include "sealnet/EncryptionParamsWrapper.h"
#include "sealnet/CiphertextWrapper.h"
#include <vector>
#include <utility>

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class PolyCRTBuilder;

            ref class KeyGenerator;

            /**
            <summary>Class to store Galois keys.</summary>

            <remarks>
            <para>
            Galois keys are used together with batching (<see cref="PolyCRTBuilder" />). If the
            polynomial modulus is a polynomial of degree N, in batching the idea is to view
            a plaintext polynomial as a 2-by-(N/2) matrix of integers modulo plaintext modulus.
            Normal homomorphic computations operate on such encrypted matrices element (slot) wise.
            However, special rotation operations allow us to also rotate the matrix rows cyclically
            in either direction, and rotate the columns (swap the rows). These operations require
            the Galois keys.
            </para>
            <para>
            Decomposition bit count (dbc) is a parameter that describes a performance trade-off in
            the rotation operation. Its function is exactly the same as in relinearization. Namely,
            the polynomials in the ciphertexts (with large coefficients) get decomposed into a smaller
            base 2^dbc, coefficient-wise. Each of the decomposition factors corresponds to a piece of
            data in the Galois keys, so the smaller the dbc is, the larger the Galois keys are.
            Moreover, a smaller dbc results in less invariant noise budget being consumed in the
            rotation operation. However, using a large dbc is much faster, and often one would want
            to optimize the dbc to be as large as possible for performance. The dbc is upper-bounded
            by the value of 60, and lower-bounded by the value of 1.
            </para>
            <para>
            In general, reading from GaloisKeys is thread-safe as long as no other thread is
            concurrently mutating it. This is due to the underlying data structure storing the
            Galois keys not being thread-safe.
            </para>
            </remarks>
            <seealso cref="SecretKey">See SecretKey for the class that stores the secret key.</seealso>
            <seealso cref="PublicKey">See PublicKey for the class that stores the public key.</seealso>
            <seealso cref="GaloisKeys">See EvaluationKeys for the class that stores the evaluation keys.</seealso>
            <seealso cref="KeyGenerator">See KeyGenerator for the class that generates the Galois keys.</seealso>
            */
            public ref class GaloisKeys
            {
            public:
                /**
                <summary>Creates an empty set of Galois keys.</summary>
                */
                GaloisKeys();

                /**
                <summary>Creates a new GaloisKeys instance by copying a given instance.</summary>

                <param name="copy">The GaloisKeys to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                GaloisKeys(GaloisKeys ^copy);

                /**
                <summary>Copies a given GaloisKeys instance to the current one.</summary>

                <param name="assign">The GaloisKeys to copy from</param>
                <exception cref="System::ArgumentNullException">if assign is null</exception>
                */
                void Set(GaloisKeys ^assign);

                /**
                <summary>Returns the current number of Galois keys.</summary>
                */
                property int Size {
                    int get();
                }

                /**
                <summary>Returns the decomposition bit count.</summary>
                */
                property int DecompositionBitCount {
                    int get();
                }

                /**
                <summary>Returns a copy of the Galois keys data.</summary>
                */
                property System::Collections::Generic::List<System::Collections::Generic::List<Ciphertext^> ^> ^Data {
                    System::Collections::Generic::List<System::Collections::Generic::List<Ciphertext^> ^> ^get();
                };

                /**
                <summary>Returns a copy of a Galois key.</summary>

                <remarks>
                Returns a copy of a Galois key. The returned Galois key corresponds to the given 
                Galois element.
                </remarks>
                <param name="galoisElt">The Galois element</param>
                <exception cref="System::ArgumentException">if the key corresponding to galoisElt does 
                not exist</exception>
                */
                System::Collections::Generic::List<Ciphertext^> ^Key(System::UInt64 galoisElt);

                /**
                <summary>Returns whether a Galois key corresponding to a given Galois key element
                exists.</summary>

                <param name="galoisElt">The Galois element</param>
                <exception cref="System::ArgumentException">if Galois element is not valid</exception>
                */
                bool HasKey(System::UInt64 galoisElt);

                /**
                <summary>Returns a copy of the hash block.</summary>
                */
                property System::Tuple<System::UInt64, System::UInt64, System::UInt64, System::UInt64> ^HashBlock {
                    System::Tuple<System::UInt64, System::UInt64, System::UInt64, System::UInt64> ^get();
                }

                /**
                <summary>Saves the GaloisKeys instance to an output stream.</summary>

                <remarks>
                Saves the GaloisKeys instance to an output stream. The output is in binary format 
                and not human-readable. The output stream must have the "binary" flag set.
                </remarks>
                <param name="stream">The stream to save the GaloisKeys to</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                <seealso cref="Load()">See Load() to load a saved GaloisKeys instance.</seealso>
                */
                void Save(System::IO::Stream ^stream);

                /**
                <summary>Loads an GaloisKeys instance from an input stream overwriting the current 
                GaloisKeys instance.</summary>

                <param name="stream">The stream to load the GaloisKeys instance from</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                <seealso cref="Save()">See Save() to save an GaloisKeys instance.</seealso>
                */
                void Load(System::IO::Stream ^stream);

                /**
                <summary>Destroys the GaloisKeys instance.</summary>
                */
                ~GaloisKeys();

                /**
                <summary>Destroys the GaloisKeys instance.</summary>
                */
                !GaloisKeys();

            internal:
                /**
                <summary>Creates a deep copy of a C++ GaloisKeys instance.</summary>

                <param name="value">The GaloisKeys instance to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                GaloisKeys(const seal::GaloisKeys &value);

                /**
                <summary>Returns a reference to the underlying C++ GaloisKeys.</summary>
                */
                seal::GaloisKeys &GetKeys();

            private:
                seal::GaloisKeys *keys_;
            };
        }
    }
}
