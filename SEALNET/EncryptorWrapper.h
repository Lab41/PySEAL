#pragma once

#include "encryptor.h"
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
            <summary>Encrypts BigPoly objects into BigPolyArray objects.</summary>

            <remarks>
            <para>
            Encrypts BigPoly objects into BigPolyArray objects. Constructing an Encryptor requires the encryption parameters 
            (set through an <see cref="EncryptionParameters"/> object) and an BigPolyArray. The private and evaluation keys are not 
            needed for encryption.
            </para>
            </remarks>
            */
            public ref class Encryptor
            {
            public:
                /**
                <summary>Creates a Encryptor instances initialized with the specified encryption parameters and public key.</summary>
                <param name="parms">The encryption parameters</param>
                <param name="publicKey">The public key</param>
                <exception cref="System::ArgumentNullException">if parms or publicKey is null</exception>
                <exception cref="System::ArgumentException">if encryption parameters or public key are not valid</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption
                parameters.</seealso>
                */
                Encryptor(EncryptionParameters ^parms, BigPolyArray ^publicKey);

                /**
                <summary>Returns the public key used by the Encryptor.</summary>
                */
                property BigPolyArray ^PublicKey {
                    BigPolyArray ^get();
                }

                /**
                <summary>Encrypts a plaintext and stores the result in the destination parameter.</summary>

                <remarks>
                Encrypts a plaintext and stores the result in the destination parameter. The destination parameter is
                resized if and only if its coefficient count or coefficient bit count does not match the encryption parameters. The
                plaintext polynomial must have a significant coefficient count smaller than the coefficient count specified by the
                encryption parameters, and with coefficient values less-than the plaintext modulus
                (<see cref="EncryptionParameters::PlainModulus"/>).
                </remarks>
                <param name="plain">The plaintext to encrypt</param>
                <param name="destination">The ciphertext to overwrite with the encrypted plaintext</param>
                <exception cref="System::ArgumentNullException">if plain or destination is null</exception>
                <exception cref="System::ArgumentException">if the plaintext polynomial's significant coefficient count or coefficient
                values are too large to represent with the encryption parameters</exception>
                <exception cref="System::InvalidOperationException">If destination is an alias but needs to be resized</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on plain text limits for encryption
                parameters.</seealso>
                */
                void Encrypt(BigPoly ^plain, BigPolyArray ^destination);

                /**
                <summary>Encrypts a plaintext and returns the result.</summary>

                <remarks>
                Encrypts a plaintext and returns the result. The destination parameter is
                resized if and only if its coefficient count or coefficient bit count does not match the encryption parameters. The
                plaintext polynomial must have a significant coefficient count smaller than the coefficient count specified by the
                encryption parameters, and with coefficient values less-than the plaintext modulus
                (<see cref="EncryptionParameters::PlainModulus"/>).
                </remarks>
                <param name="plain">The plaintext to encrypt</param>
                <exception cref="System::ArgumentNullException">if plain is null</exception>
                <exception cref="System::ArgumentException">if the plaintext polynomial's significant coefficient count or coefficient
                values are too large to represent with the encryption parameters</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on plaintext limits for encryption
                parameters.</seealso>
                */
                BigPolyArray ^Encrypt(BigPoly ^plain);

                /**
                <summary>Destroys the Encryptor.</summary>
                */
                ~Encryptor();

                /**
                <summary>Destroys the Encryptor.</summary>
                */
                !Encryptor();

                /**
                <summary>Returns a reference to the underlying C++ Encryptor.</summary>
                */
                seal::Encryptor &GetEncryptor();

            internal:

            private:
                seal::Encryptor *encryptor_;
            };

        }
    }
}