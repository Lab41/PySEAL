#pragma once

#include "seal/evaluationkeys.h"
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
            /**
            <summary>Class to store evaluation keys.</summary>

            <remarks>
            <para>
            Class to store evaluation keys. An evaluation key has type std::vector<Ciphertext> (C++).
            An instance of the EvaluationKeys class stores internally an std::vector (C++) of
            evaluation keys.
            </para>
            <para>
            Concretely, an evaluation key corresponding to a power K of the secret key can be used
            in the relinearization operation to change a ciphertext of size K+1 to size K. Recall
            that the smallest possible size for a ciphertext is 2, so the first evaluation key is
            corresponds to the square of the secret key. The second evaluation key corresponds to
            the cube of the secret key, and so on. For example, to relinearize a ciphertext of size
            7 back to size 2, one would need 5 evaluation keys, although it is hard to imagine
            a situation where it makes sense to have size 7 ciphertexts, as operating on such objects
            would be very slow. Most commonly only one evaluation key is needed, and relinearization
            is performed after every multiplication.
            </para>
            <para>
            Decomposition bit count (dbc) is a parameter that describes a performance trade-off in
            the relinearization process. Namely, in the relinearization process the polynomials in
            the ciphertexts (with large coefficients) get decomposed into a smaller base 2^dbc
            coefficient-wise. Each of the decomposition factors corresponds to a piece of data in
            the evaluation key, so the smaller the dbc is, the larger the evaluation keys are.
            Moreover, a smaller dbc results in less invariant noise budget being consumed in the
            relinearization process. However, using a large dbc results is much faster, and often
            one would want to optimize the dbc to be as large as possible for performance. The dbc
            is upper-bounded by the value of 60, and lower-bounded by the value of 1.
            </para>
            <para>
            In general, reading from EvaluationKeys is thread-safe as long as no other thread is
            concurrently mutating it. This is due to the underlying data structure storing the
            evaluation keys not being thread-safe.
            </para>
            </remarks>
            <seealso cref="SecretKey">See SecretKey for the class that stores the secret key.</seealso>
            <seealso cref="PublicKey">See PublicKey for the class that stores the public key.</seealso>
            <seealso cref="GaloisKeys">See GaloisKeys for the class that stores the Galois keys.</seealso>
            <seealso cref="KeyGenerator">See KeyGenerator for the class that generates the evaluation
            keys.</seealso>
            */
            public ref class EvaluationKeys
            {
            public:
                /**
                <summary>Creates an empty set of evaluation keys.</summary>
                */
                EvaluationKeys();

                /**
                <summary>Creates a new EvaluationKeys instance by copying a given instance.</summary>

                <param name="copy">The EvaluationKeys to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                EvaluationKeys(EvaluationKeys ^copy);

                /**
                <summary>Copies a given EvaluationKeys instance to the current one.</summary>

                <param name="assign">The EvaluationKeys to copy from</param>
                <exception cref="System::ArgumentNullException">if assign is null</exception>
                */
                void Set(EvaluationKeys ^assign);

                /**
                <summary>Returns the current number of evaluation keys.</summary>
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
                <summary>Returns a copy of the evaluation keys data.</summary>
                */
                property System::Collections::Generic::List<System::Collections::Generic::List<Ciphertext^> ^> ^Data {
                    System::Collections::Generic::List<System::Collections::Generic::List<Ciphertext^> ^> ^get();
                };

                /**
                <summary>Returns a copy of an evaluation key.</summary>
                
                <remarks>
                Returns a copy of an evaluation key. The returned evaluation key corresponds to the 
                given power of the secret key.
                </remarks>
                <param name="keyPower">The power of the secret key</param>
                <exception cref="System::ArgumentException">if the key corresponding to keyPower does not 
                exist</exception>
                */
                System::Collections::Generic::List<Ciphertext^> ^Key(int keyPower);

                /**
                <summary>Returns whether an evaluation key corresponding to a given power of the secret key 
                exists.</summary>

                <param name="keyPower">The power of the secret key</param>
                */
                bool HasKey(int keyPower);

                /**
                <summary>Returns a copy of the hash block.</summary>
                */
                property System::Tuple<System::UInt64, System::UInt64, System::UInt64, System::UInt64> ^HashBlock {
                    System::Tuple<System::UInt64, System::UInt64, System::UInt64, System::UInt64> ^get();
                }

                /**
                <summary>Saves the EvaluationKeys instance to an output stream.</summary>

                <remarks>
                Saves the EvaluationKeys instance to an output stream. The output is in binary format 
                and not human-readable. The output stream must have the "binary" flag set.
                </remarks>
                <param name="stream">The stream to save the EvaluationKeys to</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                <seealso cref="Load()">See Load() to load a saved EvaluationKeys instance.</seealso>
                */
                void Save(System::IO::Stream ^stream);

                /**
                <summary>Loads an EvaluationKeys instance from an input stream overwriting the current 
                EvaluationKeys instance.</summary>

                <param name="stream">The stream to load the EvaluationKeys instance from</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                <seealso cref="Save()">See Save() to save an EvaluationKeys instance.</seealso>
                */
                void Load(System::IO::Stream ^stream);

                /**
                <summary>Destroys the EvaluationKeys instance.</summary>
                */
                ~EvaluationKeys();

                /**
                <summary>Destroys the EvaluationKeys instance.</summary>
                */
                !EvaluationKeys();

            internal:
                /**
                <summary>Creates a deep copy of a C++ EvaluationKeys instance.</summary>
                <param name="value">The EvaluationKeys instance to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                EvaluationKeys(const seal::EvaluationKeys &value);

                /**
                <summary>Returns a reference to the underlying C++ EvaluationKeys.</summary>
                */
                seal::EvaluationKeys &GetKeys();

            private:
                seal::EvaluationKeys *keys_;
            };
        }
    }
}