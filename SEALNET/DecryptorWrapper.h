#pragma once

#include "decryptor.h"
#include "BigPolyArrayWrapper.h"
#include "EncryptionParamsWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
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
                <summary>Creates a Decryptor instance initialized with the specified encryption parameters and secret key.</summary>

                <remarks>
                Creates a Decryptor instance initialized with the specified encryption parameters and secret key.
                The user can give a <see cref="MemoryPoolHandle "/> object to use a custom memory pool instead 
                of the global memory pool (default).
                </remarks>
                <param name="parms">The encryption parameters</param>
                <param name="secretKey">The secret key</param>
                <param name="pool">The memory pool handle</param>
                <exception cref="System::ArgumentNullException">if parms, secretKey, or pool is null</exception>
                <exception cref="System::ArgumentException">if encryption parameters or secret key are not valid</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption parameters.</seealso>
                <seealso cref="MemoryPoolHandle">See MemoryPoolHandle for more details on memory pool handles.</seealso>
                */
                Decryptor(EncryptionParameters ^parms, BigPoly ^secretKey, MemoryPoolHandle ^pool);

                /**
                <summary>Creates a copy of a Decryptor.</summary>

                <param name="copy">The Decryptor to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                Decryptor(Decryptor ^copy);

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
                <summary>Computes the invariant noise budget (in bits) of a ciphertext.</summary>
                
                <remarks>
                <para>
                Computes the invariant noise budget (in bits) of a ciphertext. The invariant noise budget measures 
                the amount of room there is for the noise to grow while ensuring correct decryptions.
                </para>
                <para>
                The invariant noise polynomial of a ciphertext is a rational coefficient polynomial, such that
                a ciphertext decrypts correctly as long as the coefficients of the invariant noise polynomial are
                of absolute value less than 1/2. Thus, we call the infinity-norm of the invariant noise polynomial
                the invariant noise, and for correct decryption require it to be less than 1/2. If v denotes the
                invariant noise, we define the invariant noise budget as -log2(2v). Thus, the invariant noise budget
                starts from some initial value, which depends on the encryption parameters, and decreases to 0 when
                computations are performed. When the budget reaches 0, the ciphertext becomes too noisy to decrypt
                correctly.
                </para>
                </remarks>
                <param name="encrypted">The ciphertext</param>
                <exception cref="System::ArgumentNullException">if encrypted is null</exception>
                <exception cref="System::ArgumentException">if encrypted is not a valid ciphertext for the encryption
                parameters</exception>
                */
                int InvariantNoiseBudget(BigPolyArray ^encrypted);

                /**
                <summary>Computes and returns the number of bits of inherent noise in a ciphertext.</summary>

                <remarks>
                Computes and returns the number of bits of inherent noise in a ciphertext. The user can easily compare this 
                with the maximum possible value returned by the function EncryptionParameters::InherentNoiseBitsMax(). 
                Instead of this function, consider using <see cref="InvariantNoiseBudget()"/>, which has several subtle 
                advantages.
                </remarks>
                <param name="encrypted">The ciphertext</param>
                <exception cref="System::ArgumentNullException">if encrypted is null</exception>
                <exception cref="System::ArgumentException">if encrypted is not a valid ciphertext for the encryption
                parameters</exception>
                <seealso cref="InherentNoise">See InherentNoise for computing the exact size of inherent noise.</seealso>
                */
                int InherentNoiseBits(BigPolyArray ^encrypted);

                /**
                <summary>Computes the inherent noise in a ciphertext.</summary>

                <remarks>
                Computes the inherent noise in a ciphertext. The result is written in a BigUInt given as a parameter. The user can
                easily compare this with the maximum possible value returned by the function EncryptionParameters::inherentNoiseMax().
                It is often easier to analyze the size of the inherent noise by using the functions <see cref="InherentNoiseBits"/> 
                and EncryptionParameters::InherentNoiseMax(). Instead of this function, consider using 
                <see cref="InvariantNoiseBudget()"/>, which has several subtle advantages.
                </remarks>
                <param name="encrypted">The ciphertext</param>
                <param name="destination">The BigUInt to overwrite with the inherent noise</param>
                <exception cref="System::ArgumentNullException">if encrypted or destination is null</exception>
                <exception cref="System::ArgumentException">if encrypted is not a valid ciphertext for the encryption
                parameters</exception>
                <seealso cref="InherentNoiseBits">See InherentNoiseBits for returning the significant bit count of the 
                inherent noise instead.</seealso>
                */
                void InherentNoise(BigPolyArray ^encrypted, BigUInt ^destination);

                /**
                <summary>Destroys the Decryptor.</summary>
                */
                ~Decryptor();

                /**
                <summary>Destroys the Decryptor.</summary>
                */
                !Decryptor();

            internal:
                /**
                <summary>Returns a reference to the underlying C++ Decryptor.</summary>
                */
                seal::Decryptor &GetDecryptor();

            private:
                seal::Decryptor *decryptor_;
            };
        }
    }
}
