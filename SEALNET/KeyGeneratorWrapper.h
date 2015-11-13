#pragma once

#include "keygenerator.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class EncryptionParameters;

            ref class BigPoly;

            ref class EvaluationKeys;

            /**
            <summary>Generates matching secret key, public key, and evaluation keys for encryption, decryption, and evaluation
            functions.</summary>

            <remarks>
            <para>
            Generates matching secret key, public key, and evaluation keys for encryption, decryption, and evaluation functions.
            Constructing a KeyGenerator requires the encryption parameters (set through an <see cref="EncryptionParameters"/>
            object). Invoking the <see cref="Generate()"/> function will generate a new secret key (which can be read from
            <see cref="SecretKey"/>), public key (which can be read from <see cref="PublicKey"/>), and evaluation keys (which
            can be read from <see cref="EvaluationKeys"/>). Invoking the <see cref="Generate(BigPoly^, int)"/> variant will
            generate new public key and evaluation keys for a specified secret key, raised to an optional power.
            </para>
            <para>
            The Generate() functions are not thread safe and a separate KeyGenerator instance is needed for each potentially
            concurrent key generation operation.
            </para>
            </remarks>
            */
            public ref class KeyGenerator
            {
            public:
                /**
                <summary>Creates a KeyGenerator instance initialized with the specified encryption parameters.</summary>
                <param name="parms">The encryption parameters</param>
                <exception cref="System::ArgumentNullException">If parms is null</exception>
                <exception cref="System::ArgumentException">if encryption parameters are not valid</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption
                parameters.</seealso>
                */
                KeyGenerator(EncryptionParameters ^parms);

                /**
                <summary>Generates new matching secret key, public key, and evaluation keys.</summary>

                <seealso cref="SecretKey">See SecretKey to read generated secret key.</seealso>
                <seealso cref="PublicKey">See PublicKey to read generated public key.</seealso>
                <seealso cref="EvaluationKeys">See EvaluationKeys to read generated evaluation keys.</seealso>
                */
                void Generate();

                /**
                <summary>Generates new matching public key and evaluation keys for a specified secret key.</summary>

                <remarks>
                Generates new matching public key and evaluation keys for a specified secret key. Note that due to randomness
                during the generation process, the generated public key and evaluation keys may not match prior generated
                public key and evaluation keys for the same secret key.
                </remarks>
                <param name="secretKey">The secret key</param>
                <exception cref="System::ArgumentNullException">If secretKey is null</exception>
                <exception cref="System::ArgumentException">if secret key is not valid</exception>
                <exception cref="System::ArgumentException">if power is not positive</exception>
                <seealso cref="PublicKey">See PublicKey to read generated public key.</seealso>
                <seealso cref="EvaluationKeys">See EvaluationKeys to read generated evaluation keys.</seealso>
                */
                void Generate(BigPoly ^secretKey);

                /**
                <summary>Generates new matching public key and evaluation keys for a specified secret key, raised to a specified
                power.</summary>

                <remarks>
                Generates new matching public key and evaluation keys for a specified secret key, raised to a specified power. The
                power parameter raises the secret key to the specified power prior to generating the keys. Raising the secret
                key to a power enables more efficient operations in some advanced cases. Note that due to randomness during the
                generation process, the generated public key and evaluation keys may not match prior generated public key and
                evaluation keys for the same secret key.
                </remarks>
                <param name="secretKey">The secret key</param>
                <param name="power">The power to raise the secret key to</param>
                <exception cref="System::ArgumentException">if secret key is not valid</exception>
                <exception cref="System::ArgumentException">if power is not positive</exception>
                <exception cref="System::ArgumentNullException">If secretKey is null</exception>
                <seealso cref="SecretKey">See SecretKey to read specified secret key, raised to the specified power.</seealso>
                <seealso cref="PublicKey">See PublicKey to read generated public key.</seealso>
                <seealso cref="EvaluationKeys">See EvaluationKeys to read generated evaluation keys.</seealso>
                */
                void Generate(BigPoly ^secretKey, int power);

                /**
                <summary>Returns the generated public key after a Generate() invocation.</summary>
                */
                property BigPoly ^PublicKey {
                    BigPoly ^get();
                }

                /**
                <summary>Returns the generated secret key after a Generate() invocation.</summary>
                */
                property BigPoly ^SecretKey {
                    BigPoly ^get();
                }

                /**
                <summary>Returns the generated evaluation keys after a Generate() invocation.</summary>
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