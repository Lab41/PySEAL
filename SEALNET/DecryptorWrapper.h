#pragma once

#include "decryptor.h"
#include "BigPolyArrayWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class EncryptionParameters;

            ref class BigPoly;

            /**
            <summary>Decrypts BigPolyArray objects into BigPoly objects.</summary>

            <remarks>
            <para>
            Decrypts <see cref="BigPolyArray" /> objects into <see cref="BigPoly" /> objects. Constructing a
            Decryptor requires the encryption parameters (set through an <see cref="EncryptionParameters"/> object) and the
            secret key polynomial. The public and evaluation keys are not needed for decryption.
            </para>
            </remarks>
            */
            public ref class Decryptor
            {
            public:
                /**
                <summary>Creates a Decryptor instance initialized with the specified encryption parameters and secret key.</summary>
                <param name="parms">The encryption parameters</param>
                <param name="secretKey">The secret key</param>
                <exception cref="System::ArgumentNullException">if parms or secretKey is null</exception>
                <exception cref="System::ArgumentException">if encryption parameters or secret key are not valid</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption
                parameters.</seealso>
                */
                Decryptor(EncryptionParameters ^parms, BigPoly ^secretKey);

                /**
                <summary>Returns the secret key used by the Decryptor.</summary>
                */
                property BigPoly ^SecretKey {
                    BigPoly ^get();
                }

                /**
                <summary>Decrypts an BigPolyArray and stores the result in the destination parameter.</summary>

                <remarks>
                Decrypts an BigPolyArray and stores the result in the destination parameter.
                </remarks>
                <param name="encrypted">The ciphertext to decrypt</param>
                <param name="destination">The plaintext to overwrite with the decrypted ciphertext</param>
                <exception cref="System::ArgumentNullException">if encrypted or destination is null</exception>
                <exception cref="System::ArgumentException">if the ciphertext is not a valid ciphertext for the
                encryption parameters</exception>
                <exception cref="System::InvalidOperationException">If destination is an alias but needs to be resized</exception>
                */
                void Decrypt(BigPolyArray ^encrypted, BigPoly ^destination);

                /**
                <summary>Decrypts an BigPolyArray and returns the result.</summary>
                <param name="encrypted">The ciphertext to decrypt</param>
                <exception cref="System::ArgumentNullException">if encrypted is null</exception>
                <exception cref="System::ArgumentException">if the ciphertext is not a valid ciphertext for the
                encryption parameters</exception>
                */
                BigPoly ^Decrypt(BigPolyArray ^encrypted);

                /**
                <summary>Destroys the Decryptor.</summary>
                */
                ~Decryptor();

                /**
                <summary>Destroys the Decryptor.</summary>
                */
                !Decryptor();

                /**
                <summary>Returns a reference to the underlying C++ Decryptor.</summary>
                */
                seal::Decryptor &GetDecryptor();

            internal:

            private:
                seal::Decryptor *decryptor_;
            };




        }
    }
}
