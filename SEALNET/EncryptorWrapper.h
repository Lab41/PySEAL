#pragma once

#include "encryptor.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class EncryptionParameters;

            ref class BigPoly;

            /**
            <summary>Encrypts plain-text polynomials (represented by BigPoly) into encrypted polynomials.</summary>

            <remarks>
            <para>
            Encrypts plain-text polynomials (represented by <see cref="BigPoly"/>) into encrypted polynomials. Constructing an
            Encryptor requires the encryption parameters (set through an <see cref="EncryptionParameters"/> object) and the
            public key polynomial. The private and evaluation keys are not needed for encryption.
            </para>
            <para>
            The Encrypt() functions are not thread safe and a separate Encryptor instance is needed for each potentially concurrent
            encrypt operation.
            </para>
            </remarks>
            */
            public ref class Encryptor
            {
            public:
                /**
                <summary>Creates an Encryptor instances initialized with the specified encryption parameters and public key.</summary>
                <param name="parms">The encryption parameters</param>
                <param name="publicKey">The public key</param>
                <exception cref="System::ArgumentNullException">if parms or publicKey is null</exception>
                <exception cref="System::ArgumentException">if encryption parameters or public key are not valid</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption
                parameters.</seealso>
                */
                Encryptor(EncryptionParameters ^parms, BigPoly ^publicKey);

                /**
                <summary>Returns the public key used by the Encryptor.</summary>
                */
                property BigPoly ^PublicKey {
                    BigPoly ^get();
                }

                /**
                <summary>Encrypts a plain-text polynomial and stores the result in the destination parameter.</summary>

                <remarks>
                Encrypts a plaintext polynomial and stores the result in the destination parameter. The destination parameter is
                resized if and only if its coefficient count or coefficient bit count does not match the encryption parameters. The
                plain polynomial must have a significant coefficient count smaller than the coefficient count specified by the
                encryption parameters, and with coefficient values less-than the plaintext modulus
                (<see cref="EncryptionParameters::PlainModulus"/>).
                </remarks>
                <param name="plain">The plaintext polynomial to encrypt</param>
                <param name="destination">The polynomial to overwrite with the encrypted polynomial</param>
                <exception cref="System::ArgumentNullException">If plain or destination is null</exception>
                <exception cref="System::ArgumentException">if the plain polynomial's significant coefficient count or coefficient
                values are too large to represent with the encryption parameters</exception>
                <exception cref="System::InvalidOperationException">If destination is an alias but needs to be resized</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on plain text limits for encryption
                parameters.</seealso>
                */
                void Encrypt(BigPoly ^plain, BigPoly ^destination);

                /**
                <summary>Encrypts a plain-text polynomial and returns the result.</summary>

                <remarks>
                Encrypts a plain-text polynomial and returns the result. The plain polynomial must have a significant coefficient count
                smaller than the coefficient count specified by the encryption parameters, and with coefficient values less-than the
                plaintext modulus (<see cref="EncryptionParameters::PlainModulus"/>).
                </remarks>
                <param name="plain">The plaintext polynomial to encrypt</param>
                <exception cref="System::ArgumentNullException">If plain is null</exception>
                <exception cref="System::ArgumentException">if the plain polynomial's significant coefficient count or coefficient
                values are too large to represent with the encryption parameters</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on plain text limits for encryption
                parameters.</seealso>
                */
                BigPoly ^Encrypt(BigPoly ^plain);

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