#pragma once

#include "seal/util/defines.h"
#include "seal/encryptionparams.h"
#include "sealnet/SmallModulusWrapper.h"
#include "sealnet/BigPolyWrapper.h"
#include "sealnet/MemoryPoolHandleWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class SmallModulus;

            ref class EncryptionParameters;

            ref class Encryptor;

            ref class Decryptor;

            ref class Evaluator;

            ref class ChooserPoly;

            ref class ChooserEvaluator;

            ref class SecretKey;

            ref class PublicKey;

            ref class EvaluationKeys;

            ref class Ciphertext;

            ref class SEALContext;

            ref class KeyGenerator;

            /**
            <summary>Represents the user-customizable encryption scheme settings.</summary> 
            
            <remarks>
            <para>
            Represents user-customizable encryption scheme settings. The parameters (most
            importantly PolyModulus, CoeffModulus, PlainModulus) significantly affect the 
            performance, capabilities, and security of the encryption scheme. Once an 
            instance of EncryptionParameters is populated with appropriate parameters, it 
            can be used to create an instance of the <see cref="SEALContext" /> class, 
            which verifies the validity of the parameters, and performs necessary 
            pre-computations.
            </para>
            <para>
            Picking appropriate encryption parameters is essential to enable a particular
            application while balancing performance and security. Some encryption settings
            will not allow some inputs (e.g. attempting to encrypt a polynomial with more
            coefficients than PolyModulus or larger coefficients than PlainModulus) or 
            support the desired computations (with noise growing too fast due to too large 
            PlainModulus and too small CoeffModulus).
            </para>
            <para>
            The EncryptionParameters class maintains at all times a 256-bit SHA-3 hash of
            the currently set encryption parameters. This hash is then stored by all further
            objects created for these encryption parameters, e.g. <see cref="SEALContext" />,
            <see cref="KeyGenerator" />, <see cref="Encryptor" />, <see cref="Decryptor" />,
            <see cref="Evaluator" />, all secret and public keys, and ciphertexts. The hash 
            block is not intended to be directly modified by the user, and is used internally 
            to perform quick input compatibility checks.
            </para>
            <para>
            In general, reading from EncryptionParameters is thread-safe, while mutating is not.
            </para>
            <para>
            Choosing inappropriate encryption parameters may lead to an encryption scheme 
            that is not secure, does not perform well, and/or does not support the input 
            and computation of the desired application. We highly recommend consulting an 
            expert in RLWE-based encryption when selecting parameters, as this is where
            inexperienced users seem to most    often make critical mistakes.
            </para>
            </remarks>
            */
            public ref class EncryptionParameters : System::IEquatable<EncryptionParameters^>
            {
            public:
                /**
                <summary>Creates an empty encryption parameters.</summary>
                
                <remarks>
                Creates an empty encryption parameters. At a minimum, the user needs to specify 
                the parameters <see cref="PolyModulus"/>, <see cref="CoeffModulus"/>, and 
                <see cref="PlainModulus"/> for the parameters to be valid.
                </remarks>
                */
                EncryptionParameters();

                /** 
                <summary>Creates a copy of a given instance of EncryptionParameters.</summary>

                <param name="copy">The EncryptionParameters to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                EncryptionParameters(EncryptionParameters ^copy);

                /**
                <summary>Overwrites the EncryptionParameters instance with a copy of a given 
                instance.</summary>

                <param name="assign">The EncryptionParameters to copy from</param>
                <exception cref="System::ArgumentNullException">if assign is null</exception>
                */
                void Set(EncryptionParameters ^assign);

                property BigPoly ^PolyModulus {
                    /**
                    <summary>Returns a copy of the currently set polynomial modulus parameter.</summary>
                    */
                    BigPoly ^get();

                    /**
                    <summary>Sets the polynomial modulus parameter to the specified value.</summary>

                    <remarks>
                    Sets the polynomial modulus parameter to the specified value (represented by
                    <see cref="BigPoly" />). The polynomial modulus directly affects the number
                    of coefficients in plaintext polynomials, the size of ciphertext elements, the
                    computational performance of the scheme (bigger is worse), and the security
                    level (bigger is better). In SEAL the polynomial modulus must be of the form
                    "1x^N + 1", where N is a power of 2 (e.g. 1024, 2048, 4096, 8192, 16384, or 32768).
                    </remarks>
                    <param name="polyModulus">The new polynomial modulus</param>
                    <exception cref="System::ArgumentNullException">if polyModulus is null</exception>
                    */
                    void set(BigPoly ^polyModulus);
                }

                property System::Collections::Generic::List<SmallModulus^> ^CoeffModulus {
                    /**
                    <summary>Returns a copy of the currently set coefficient modulus parameter.</summary>
                    */
                    System::Collections::Generic::List<SmallModulus^> ^get();

                    /**
                    <summary>Sets the coefficient modulus parameter.</summary>

                    <remarks>
                    Sets the coefficient modulus parameter. The coefficient modulus consists of a list
                    of distinct prime numbers, and is represented by a list of <see cref="SmallModulus" />
                    objects. The coefficient modulus directly affects the size of ciphertext elements,
                    the amount of computation that the scheme can perform (bigger is better), and the
                    security level (bigger is worse). In SEAL each of the prime numbers in the coefficient
                    modulus must be at most 60 bits, and must be congruent to 1 modulo 2*degree(PolyModulus).
                    </remarks>
                    <param name="coeffModulus">The new coefficient modulus</param>
                    <exception cref="System::ArgumentNullException">if coeffModulus is null</exception>
                    */
                    void set(System::Collections::Generic::List<SmallModulus^> ^coeffModulus);
                }

                property SmallModulus ^PlainModulus {
                    /**
                    <summary>Returns a copy of the currently set plaintext modulus parameter.</summary>
                    */
                    SmallModulus ^get();

                    /**
                    <summary>Sets the plaintext modulus parameter.</summary>

                    <remarks>
                    Sets the plaintext modulus parameter. The plaintext modulus is an integer modulus
                    represented by the <see cref="SmallModulus" /> class. The plaintext modulus determines
                    the largest coefficient that plaintext polynomials can represent. It also affects the
                    amount of computation that the scheme can perform (bigger is worse). In SEAL the
                    plaintext modulus can be at most 60 bits long, but can otherwise be any integer. Note,
                    however, that some features (e.g. batching) require the plaintext modulus to be of
                    a particular form.
                    </remarks>
                    <param name="plainModulus">The new plaintext modulus</param>
                    <exception cref="System::ArgumentNullException">if plainModulus is null</exception>
                    */
                    void set(SmallModulus ^plainModulus);
                }

                property double NoiseStandardDeviation {
                    /**
                    <summary>Returns the currently set standard deviation of the noise distribution.</summary>
                    */
                    double get();

                    /**
                    <summary>Sets the standard deviation of the noise distribution used for error
                    sampling.</summary>

                    <remarks>
                    Sets the standard deviation of the noise distribution used for error sampling. This
                    parameter directly affects the security level of the scheme. However, it should not be
                    necessary for most users to change this parameter from its default value.
                    </remarks>
                    <param name="value">The new standard deviation</param>
                    */
                    void set(double value);
                }

                /**
                <summary>Returns the currently set maximum deviation of the noise distribution.</summary>

                <remarks>
                Returns the currently set maximum deviation of the noise distribution. This value
                cannot be directly controlled by the user, and is automatically set to be an
                appropriate multiple of the NoiseStandardDeviation parameter.
                </remarks>
                */
                property double NoiseMaxDeviation {
                    double get();
                }

                /**
                <summary>Saves the EncryptionParameters to an output stream.</summary>

                <remarks>
                Saves the EncryptionParameters to an output stream. The output is in binary format
                and is not human-readable. The output stream must have the "Binary" flag set.
                </remarks>
                <param name="stream">The stream to save the EncryptionParameters to</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                <seealso cref="Load()">See Load() to load a saved EncryptionParameters instance.</seealso>
                */
                void Save(System::IO::Stream ^stream);

                /**
                <summary>Loads the EncryptionParameters from an input stream overwriting the current
                EncryptionParameters.</summary>

                <param name="stream">The stream to load the EncryptionParameters from</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                <seealso cref="Save()">See Save() to save an EncryptionParameters instance.</seealso>
                */
                void Load(System::IO::Stream ^stream);

                /**
                <summary>Returns the hash block of the current parameters.</summary>

                <remarks>
                Returns the hash block of the current parameters. This function is intended
                mainly for internal use.
                </remarks>
                */
                property System::Tuple<System::UInt64, System::UInt64, System::UInt64, System::UInt64> ^HashBlock {
                    System::Tuple<System::UInt64, System::UInt64, System::UInt64, System::UInt64> ^get();
                }

                /**
                <summary>Compares a given set of encryption parameters to the current set of 
                encryption parameters.</summary>

                <remarks>
                Compares a given set of encryption parameters to the current set of encryption
                parameters. The comparison is performed by comparing hash blocks of the parameter
                sets rather than comparing the parameters individually.
                </remarks>
                <param name="compare">The EncryptionParameters to compare against</param>
                */
                virtual bool Equals(EncryptionParameters ^compare);

                /**
                <summary>Compares a given set of encryption parameters to the current set of
                encryption parameters.</summary>

                <remarks>
                Compares a given set of encryption parameters to the current set of encryption
                parameters. The comparison is performed by comparing hash blocks of the parameter
                sets rather than comparing the parameters individually.
                </remarks>
                <param name="compare">The EncryptionParameters to compare against</param>
                */
                bool Equals(System::Object ^compare) override;

                /**
                <summary>Returns a hash-code based on the EncryptionParameters.</summary>
                */
                int GetHashCode() override;

                /**
                <summary>Destroys the EncryptionParameters.</summary>
                */
                ~EncryptionParameters();

                /**
                <summary>Destroys the EncryptionParameters.</summary>
                */
                !EncryptionParameters();

            internal:
                /**
                <summary>Returns a reference to the underlying C++ EncryptionParameters.</summary>
                */
                seal::EncryptionParameters &GetParms();

                /**
                <summary>Creates a deep copy of the C++ EncryptionParameters.</summary>
                <remarks>
                Creates a deep copy of a EncryptionParameters. The created EncryptionParameters 
                will have the same settings as the original.
                </remarks>
                <param name="parms">The EncryptionParameters to copy from</param>
                */
                EncryptionParameters(const seal::EncryptionParameters &parms);

            private:
                seal::EncryptionParameters *parms_;
            };
        }
    }
}