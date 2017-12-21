#pragma once

#include "seal/keygenerator.h"
#include "sealnet/ContextWrapper.h"
#include "sealnet/SecretKeyWrapper.h"
#include "sealnet/PublicKeyWrapper.h"
#include "sealnet/EncryptionParamsWrapper.h"
#include "sealnet/EvaluationKeysWrapper.h"
#include "sealnet/GaloisKeysWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            /**
            <summary>Generates matching secret key and public key.</summary>

            <remarks>
            Generates matching secret key and public key. An existing KeyGenerator can also at any time
            be used to generate evaluation keys and Galois keys. Constructing a KeyGenerator requires
            only a <see cref="SEALContext" />.
            </remarks>
            <seealso cref="EncryptionParameters">See EncryptionParameters for more details on encryption
            parameters.</seealso>
            <seealso cref="SecretKey">See SecretKey for more details on secret key.</seealso>
            <seealso cref="PublicKey">See PublicKey for more details on public key.</seealso>
            <seealso cref="EvaluationKeys">See EvaluationKeys for more details on evaluation keys.</seealso>
            <seealso cref="GaloisKeys">See GaloisKeys for more details on Galois keys.</seealso>
            */
            public ref class KeyGenerator
            {
            public:
                /**
                <summary>Creates a KeyGenerator initialized with the specified SEALContext.</summary>

                <remarks>
                Creates a KeyGenerator initialized with the specified <see cref="SEALContext" />.
                Dynamically allocated member variables are allocated from the global memory pool.
                </remarks>
                <param name="context">The SEALContext</param>
                <exception cref="System::ArgumentException">if encryption parameters are not 
                valid</exception>
                <exception cref="System::ArgumentNullException">if context is null</exception>
                */
                KeyGenerator(SEALContext ^context);

                /**
                <summary>Creates a KeyGenerator initialized with the specified SEALContext.</summary>

                <remarks>
                Creates a KeyGenerator initialized with the specified <see cref="SEALContext" />.
                Dynamically allocated member variables are allocated from the memory pool pointed to 
                by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="context">The SEALContext</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if encryption parameters are not 
                valid</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if context or pool is null</exception>
                */
                KeyGenerator(SEALContext ^context, MemoryPoolHandle ^pool);

                /**
                <summary>Creates an KeyGenerator instance initialized with the specified SEALContext
                and specified previously secret and public keys.</summary>

                <remarks>
                Creates an KeyGenerator instance initialized with the specified
                <see cref="SEALContext" /> and specified previously secret and public keys. This
                can e.g. be used to increase the number of evaluation keys from what had earlier
                been generated, or to generate Galois keys in case they had not been generated
                earlier. Dynamically allocated member variables are allocated from the global 
                memory pool.
                </remarks>
                <param name="context">The SEALContext</param>
                <param name="secretKey">A previously generated secret key</param>
                <param name="publicKey">A previously generated public key</param>
                <exception cref="System::ArgumentException">if encryption parameters are not
                valid</exception>
                <exception cref="System::ArgumentException">if secretKey or publicKey is not valid
                for encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if context, secretKey, or publicKey
                is null</exception>
                */
                KeyGenerator(SEALContext ^context, SecretKey ^secretKey, PublicKey ^publicKey);

                /**
                <summary>Creates an KeyGenerator instance initialized with the specified SEALContext
                and specified previously secret and public keys.</summary>

                <remarks>
                Creates an KeyGenerator instance initialized with the specified
                <see cref="SEALContext" /> and specified previously secret and public keys. This
                can e.g. be used to increase the number of evaluation keys from what had earlier
                been generated, or to generate Galois keys in case they had not been generated
                earlier. Dynamically allocated member variables are allocated from the memory pool
                pointed to by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="context">The SEALContext</param>
                <param name="secretKey">A previously generated secret key</param>
                <param name="publicKey">A previously generated public key</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if encryption parameters are not 
                valid</exception>
                <exception cref="System::ArgumentException">if secretKey or publicKey is not valid
                for encryption parameters</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if context, secretKey, publicKey, 
                or pool is null</exception>
                */
                KeyGenerator(SEALContext ^context, SecretKey ^secretKey, PublicKey ^publicKey, 
                    MemoryPoolHandle ^pool);

                /**
                <summary>Returns a copy of the public key.</summary>
                */
                property Microsoft::Research::SEAL::PublicKey ^PublicKey {
                    Microsoft::Research::SEAL::PublicKey ^get();
                }

                /**
                <summary>Returns a copy of the secret key.</summary>
                */
                property Microsoft::Research::SEAL::SecretKey ^SecretKey {
                    Microsoft::Research::SEAL::SecretKey ^get();
                }

                /**
                <summary>Generates the specified number of evaluation keys.</summary>

                <param name="decompositionBitCount">The decomposition bit count</param>
                <param name="count">The number of evaluation keys to generate</param>
                <param name="evaluationKeys">The evaluation keys instance to overwrite with the 
                generated keys</param>
                <exception cref="System::ArgumentException">if decompositionBitCount is not 
                within [0, 60]</exception>
                <exception cref="System::ArgumentException">if count is negative</exception>
                <exception cref="System::ArgumentNullException">if evaluationKeys is null</exception>
                */
                void GenerateEvaluationKeys(int decompositionBitCount, int count, 
                    EvaluationKeys ^evaluationKeys);

                /**
                <summary>Generates evaluation keys containing one key.</summary>

                <param name="decompositionBitCount">The decomposition bit count</param>
                <param name="evaluationKeys">The evaluation keys instance to overwrite with the
                generated keys</param>
                <exception cref="System::ArgumentException">if decompositionBitCount is not
                within [0, 60]</exception>
                <exception cref="System::ArgumentNullException">if evaluationKeys is null</exception>
                */
                void GenerateEvaluationKeys(int decompositionBitCount, EvaluationKeys ^evaluationKeys);

                /**
                <summary>Generates Galois keys.</summary>

                <param name="decompositionBitCount">The decomposition bit count</param>
                <param name="galoisKeys">The Galois keys instance to overwrite with the generated
                keys</param>
                <exception cref="System::ArgumentException">if decompositionBitCount is not
                within [0, 60]</exception>
                <exception cref="System::ArgumentNullException">if galoisKeys is null</exception>
                */
                void GenerateGaloisKeys(int decompositionBitCount, GaloisKeys ^galoisKeys);

                /**
                <summary>Destroys the KeyGenerator.</summary>
                */
                ~KeyGenerator();

                /**
                <summary>Destroys the KeyGenerator.</summary>
                */
                !KeyGenerator();

                /**
                <summary>Returns a reference to the underlying C++ KeyGenerator.</summary>
                */
                seal::KeyGenerator &GetGenerator();

            internal:

            private:
                seal::KeyGenerator *generator_;
            };
        }
    }
}