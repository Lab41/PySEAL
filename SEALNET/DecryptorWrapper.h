#pragma once

#include "decryptor.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class EncryptionParameters;

            ref class BigPoly;

            /**
            <summary>Decrypts encrypted polynomials (represented by BigPoly) into plain-text polynomials.</summary>

            <remarks>
            <para>
            Decrypts encrypted polynomials (represented by <see cref="BigPoly"/>) into plain-text polynomials. Constructing a
            Decryptor requires the encryption parameters (set through an <see cref="EncryptionParameters"/> object) and the
            secret key polynomial. The public and evaluation keys are not needed for decryption.
            </para>
            <para>
            The Decrypt() functions are not thread safe and a separate Decryptor instance is needed for each potentially concurrent
            decrypt operation.
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
                <exception cref="System::ArgumentNullException">If parms or secretKey is null</exception>
                <exception cref="System::ArgumentException">if encryption parameters or secret key are not valid</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption
                parameters.</seealso>
                */
                Decryptor(EncryptionParameters ^parms, BigPoly ^secretKey);

                /**
                <summary>Creates a Decryptor instance initialized with the specified encryption parameters and secret key.</summary>

                <remarks>
                Creates a Decryptor instance initialized with the specified encryption parameters and secret key. The power
                parameter raises the secret key to the specified power during initialization. Raising the secret key to a power
                enables more efficient operations in some advanced cases.
                </remarks>
                <param name="parms">The encryption parameters</param>
                <param name="secretKey">The secret key</param>
                <param name="power">The power to raise the secret key to</param>
                <exception cref="System::ArgumentNullException">If parms or secretKey is null</exception>
                <exception cref="System::ArgumentException">if encryption parameters or secret key are not valid</exception>
                <exception cref="System::ArgumentException">if power is not positive</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption
                parameters.</seealso>
                */
                Decryptor(EncryptionParameters ^parms, BigPoly ^secretKey, int power);

                /**
                <summary>Returns the secret key used by the Decryptor.</summary>
                */
                property BigPoly ^SecretKey {
                    BigPoly ^get();
                }

                /**
                <summary>Decrypts an encrypted polynomial and stores the result in the destination parameter.</summary>

                <remarks>
                Decrypts an encrypted polynomial and stores the result in the destination parameter. The destination parameter is
                resized if and only if its coefficient count or coefficient bit count does not match the encryption parameters.
                </remarks>
                <param name="encrypted">The encrypted polynomial to decrypt</param>
                <param name="destination">The polynomial to overwrite with the plain-text polynomial</param>
                <exception cref="System::ArgumentNullException">If encrypted or destination is null</exception>
                <exception cref="System::ArgumentException">if the encrypted polynomial is not a valid encrypted polynomial for the
                encryption parameters</exception>
                <exception cref="System::InvalidOperationException">If destination is an alias but needs to be resized</exception>
                */
                void Decrypt(BigPoly ^encrypted, BigPoly ^destination);

                /**
                <summary>Decrypts an encrypted polynomial and returns the result.</summary>
                <param name="encrypted">The encrypted polynomial to decrypt</param>
                <exception cref="System::ArgumentNullException">If encrypted is null</exception>
                <exception cref="System::ArgumentException">if the encrypted polynomial is not a valid encrypted polynomial for the
                encryption parameters</exception>
                */
                BigPoly ^Decrypt(BigPoly ^encrypted);

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
