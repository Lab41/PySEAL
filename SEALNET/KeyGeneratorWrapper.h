#pragma once

#include "keygenerator.h"
#include "EncryptionParamsWrapper.h"
#include "EvaluationKeysWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            /**
            <summary>Generates matching secret key, public key, and evaluation keys for encryption, decryption, and evaluation
            functions.</summary>

            <remarks>
            <para>
            Generates matching secret key, public key, and evaluation keys for encryption, decryption, and evaluation functions.
            Constructing a KeyGenerator requires the encryption parameters (set through an <see cref="EncryptionParameters"/>
            object). Invoking the <see cref="Generate()"/> function will generate a new secret key (which can be read from
            <see cref="SecretKey"/>), public key (which can be read from <see cref="PublicKey"/>), and evaluation keys (which
            can be read from <see cref="EvaluationKeys"/>).
            </para>
            <para>
            KeyGenerator is not thread-safe and a separate instance is needed for each potentially concurrent usage.
            </para>
            </remarks>
            */
            public ref class KeyGenerator
            {
            public:
                /**
                <summary>Creates a KeyGenerator instance initialized with the specified encryption parameters.</summary>

                <param name="parms">The encryption parameters</param>
                <exception cref="System::ArgumentNullException">if parms is null</exception>
                <exception cref="System::ArgumentException">if encryption parameters are not valid</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption
                parameters.</seealso>
                */
                KeyGenerator(EncryptionParameters ^parms);

                /**
                <summary>Creates a KeyGenerator instance initialized with the specified encryption parameters.</summary>

                <remarks>
                Creates a KeyGenerator instance initialized with the specified encryption parameters. The user can 
                give a <see cref="MemoryPoolHandle "/> object to use a custom memory pool instead of the global 
                memory pool (default).
                </remarks>
                <param name="parms">The encryption parameters</param>
                <param name="pool">The memory pool handle</param>
                <exception cref="System::ArgumentNullException">if parms or pool is null</exception>
                <exception cref="System::ArgumentException">if encryption parameters are not valid</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption parameters.</seealso>
                <seealso cref="MemoryPoolHandle">See MemoryPoolHandle for more details on memory pool handles.</seealso>
                */
                KeyGenerator(EncryptionParameters ^parms, MemoryPoolHandle ^pool);

                /**
                <summary>Creates an KeyGenerator instance initialized with the specified encryption parameters and specified
                previously generated keys.</summary>

                <remarks>
                <para>
                Creates an KeyGenerator instance initialized with the specified encryption parameters and specified
                previously generated keys. This can be used to increase the number of evaluation keys (using
                <see cref="GenerateEvaluationKeys()" />) from what had earlier been generated. If no evaluation keys
                had been generated earlier, one can simply pass a newly created empty instance of <see cref="EvaluationKeys" />
                to the function.
                </para>
                </remarks>
                <param name="parms">The encryption parameters</param>
                <param name="secretKey">A previously generated secret key</param>
                <param name="publicKey">A previously generated public key</param>
                <param name="evaluationKeys">A previously generated set of evaluation keys</param>
                <exception cref="System::ArgumentNullException">if parms, secretKey, publicKey or evaluationKeys is null</exception>
                <exception cref="System::ArgumentException">if encryption parameters are not valid</exception>
                <exception cref="System::ArgumentException">if secret, public or evaluation keys are not valid</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption
                parameters.</seealso>
                */
                KeyGenerator(EncryptionParameters ^parms, BigPoly ^secretKey, BigPolyArray ^publicKey, EvaluationKeys ^evaluationKeys);

                /**
                <summary>Creates an KeyGenerator instance initialized with the specified encryption parameters and specified
                previously generated keys.</summary>

                <remarks>
                <para>
                Creates an KeyGenerator instance initialized with the specified encryption parameters and specified
                previously generated keys. This can be used to increase the number of evaluation keys (using
                <see cref="GenerateEvaluationKeys()" />) from what had earlier been generated. If no evaluation keys
                had been generated earlier, one can simply pass a newly created empty instance of <see cref="EvaluationKeys" />
                to the function. The user can give a <see cref="MemoryPoolHandle "/> object to use a custom memory pool 
                instead of the global memory pool (default).
                </para>
                </remarks>
                <param name="parms">The encryption parameters</param>
                <param name="secretKey">A previously generated secret key</param>
                <param name="publicKey">A previously generated public key</param>
                <param name="evaluationKeys">A previously generated set of evaluation keys</param>
                <param name="pool">The memory pool handle</param>
                <exception cref="System::ArgumentNullException">if parms, secretKey, publicKey, evaluationKeys, or pool is null</exception>
                <exception cref="System::ArgumentException">if encryption parameters are not valid</exception>
                <exception cref="System::ArgumentException">if secret, public or evaluation keys are not valid</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption parameters.</seealso>
                <seealso cref="MemoryPoolHandle">See MemoryPoolHandle for more details on memory pool handles.</seealso>
                */
                KeyGenerator(EncryptionParameters ^parms, BigPoly ^secretKey, BigPolyArray ^publicKey, EvaluationKeys ^evaluationKeys, MemoryPoolHandle ^pool);

                /**
                <summary>Generates new matching set of secret key and public key.</summary>
                <remarks>
                Generates new matching set of secret key and public key. This function does not generate any evaluation keys.
                </remarks>
                <seealso cref="SecretKey">See SecretKey to read generated secret key</seealso>
                <seealso cref="PublicKey">See PublicKey to read generated public key</seealso>
                <seealso cref="Generate(int)">See Generate(int) to also create evaluation keys</seealso>
                */
                void Generate();

                /**
                <summary>Generates new matching set of secret key, public key, and any number of evaluation keys.</summary>

                <remarks>
                Generates new matching set of secret key, public key, and any number of evaluation keys. The number of evaluation keys
                that will be generated can be specified by the input parameter evaluationKeysCount.
                </remarks>
                <param name="evaluationKeysCount">The number of evaluation keys to generate</param>
                <exception cref="System::ArgumentException">if evaluation keys cannot be generated for specified encryption
                parameters</exception>
                <seealso cref="SecretKey">See SecretKey to read generated secret key</seealso>
                <seealso cref="PublicKey">See PublicKey to read generated public key</seealso>
                <seealso cref="EvaluationKeys">See EvaluationKeys to read generated evaluation keys</seealso>
                */
                void Generate(int evaluationKeysCount);

                /**
                <summary>Generates evaluation keys so that there are count many in total.</summary>

                <remarks>
                Generates evaluation keys so that there are count many in total. Each key is added as a new entry to the System::List of
                evaluation keys. This function is automatically called by <see cref="Generate()" /> to generate evaluation keys, but can
                be later called by the user to increase the number of evaluation keys on top of what has already been generated. 
                An error is thrown if the user tries to generate evaluation keys before a secret key and public key have been generated.
                </remarks>
                <param name="count">The total number of evaluation keys to have been generated.</param>
                <exception cref="System::ArgumentException">if count is less than 0</exception>
                <exception cref="System::ArgumentException">if evaluation keys cannot be generated for specified encryption
                parameters</exception>
                <exception cref="System::InvalidOperationException">if called before the secret key and public key have been generated</exception>
                <seealso cref="EvaluationKeys">See EvaluationKeys to read generated evaluation keys.</seealso>
                <seealso cref="Generated">See Generated to see if the secret and public keys have already been generated.</seealso>
                */
                void GenerateEvaluationKeys(int count);

                /**
                <summary>Returns true or false depending on whether secret key and public key have been generated.</summary>
                */
                property bool Generated
                {
                    bool get();
                }

                /**
                <summary>Returns the generated public key after a <see cref="Generate()"/> invocation.</summary>

                <exception cref="System::InvalidOperationException">if encryption keys have not been generated</exception>
                */
                property BigPolyArray ^PublicKey {
                    BigPolyArray ^get();
                }

                /**
                <summary>Returns the generated secret key after a <see cref="Generate()"/> invocation.</summary>

                <exception cref="System::InvalidOperationException">if encryption keys have not been generated</exception>
                */
                property BigPoly ^SecretKey {
                    BigPoly ^get();
                }

                /**
                <summary>Returns the generated evaluation keys after a <see cref="Generate()"/> invocation.</summary>

                <exception cref="System::InvalidOperationException">if encryption keys have not been generated</exception>
                <exception cref="System::InvalidOperationException">if evaluation keys have not been generated</exception>
                */
                property EvaluationKeys ^EvaluationKeys {
                    Microsoft::Research::SEAL::EvaluationKeys ^get();
                }

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