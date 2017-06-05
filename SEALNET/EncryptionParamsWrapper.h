#pragma once

#include "util/defines.h"
#include "encryptionparams.h"
#include "BigUIntWrapper.h"
#include "BigPolyWrapper.h"
#include "BigPolyArrayWrapper.h"
#include "MemoryPoolHandleWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class EncryptionParameters;

            ref class Encryptor;

            ref class Decryptor;

            ref class Evaluator;

            ref class ChooserPoly;

            ref class ChooserEvaluator;

            /**
            <summary>Stores a set of attributes (qualifiers) of a set of encryption parameteres.</summary>

            <remarks>
            Stores a set of attributes (qualifiers) of a set of encryption parameteres. These parameters are used in various parts
            of the library, e.g. to determine which algorithms can be used. The qualifiers are silently passed on to classes such
            as <see cref="Encryptor"/>, <see cref="Evaluator"/>, and <see cref="Decryptor"/>, and the only way to change them is 
            by changing the encryption parameters accordingly.
            </remarks>
            <seealso cref="EncryptionParameters">See EncryptionParameters for the class that stores the encryption parameters
            themselves.</seealso>
            <seealso>See EncryptionParameters::GetQualifiers() for obtaining the EncryptionParameterQualifiers 
            corresponding to a certain parameter set.</seealso>
            */
            public ref class EncryptionParameterQualifiers
            {
            public:
                /**
                <summary>If the encryption parameters are set in a way that is considered valid by SEAL, the 
                variable ParametersSet will be true.</summary>
                */
                property bool ParametersSet {
                    bool get();
                }

                /**
                <summary>If EncryptionParameters::DecompositionBitCount is set to a positive value, the variable 
                EnableRelinearization will be true.</summary>
                */
                property bool EnableRelinearization {
                    bool get();
                }

                /**
                <summary>If the polynomial modulus is of the form X^N+1, where N is a power of two, then Nussbaumer 
                convolution can be used for fast multiplication of polynomials modulo the polynomial modulus.</summary>

                <remarks>
                If the polynomial modulus is of the form X^N+1, where N is a power of two, then Nussbaumer convolution can be 
                used for fast multiplication of polynomials modulo the polynomial modulus. In this case the variable EnableNussbaumer 
                will be true. However, currently SEAL requires the polynomial modulus to be of this form to even consider the
                parameters to be valid. Therefore, ParametersSet can only be true if EnableNussbaumer is true.
                </remarks>
                */
                property bool EnableNussbaumer {
                    bool get();
                }

                /**
                <summary>If the coefficient modulus is congruent to 1 modulo 2N, where X^N+1 is the polynomial modulus and N is
                a power of two, then the number-theoretic transform (NTT) can be used for fast multiplications of polynomials 
                modulo the polynomial modulus and coefficient modulus.</summary>

                <remarks>
                If the coefficient modulus is congruent to 1 modulo 2N, where X^N+1 is the polynomial modulus and N is a power of two,
                then the number-theoretic transform (NTT) can be used for fast multiplications of polynomials modulo the polynomial
                modulus and coefficient modulus. In this case the variable EnableNtt will be true.
                </remarks>
                */
                property bool EnableNTT {
                    bool get();
                }

                /**
                <summary>If the plaintext modulus is congruent to 1 modulo 2N, where X^N+1 is the polynomial modulus and N is a power
                of two, then it is possible to use PolyCRTBuilder to do batching, which is a fundamental technique in homomorphic
                encryption to enable powerful SIMD functionality.</summary>

                <remarks>
                If the plaintext modulus is congruent to 1 modulo 2N, where X^N+1 is the polynomial
                modulus and N is a power of two, then it is possible to use PolyCRTBuilder to do batching,
                which is a fundamental technique in homomorphic encryption to enable powerful SIMD
                functionality, often called "batching" in homomorphic encryption literature. In this
                case the variable enable_batching will be set to true.
                </remarks>
                */
                property bool EnableBatching {
                    bool get();
                }

                /**
                <summary>This variable has currently no effect.</summary>

                <remarks>
                This variable has currently no effect (see SEAL/util/defines.h).
                </remarks>
                */
                property bool EnableNTTInMultiply {
                    bool get();
                }

                /**
                <summary>Destroys the EncryptionParameterQualifiers.</summary>
                */
                ~EncryptionParameterQualifiers();

                /**
                <summary>Destroys the EncryptionParameterQualifiers.</summary>
                */
                !EncryptionParameterQualifiers();

            internal:
                /**
                <summary>Returns a reference to the underlying C++ EncryptionParameterQualifiers.</summary>
                */
                seal::EncryptionParameterQualifiers &GetQualifiers();

                /**
                <summary>Creates a deep copy of the C++ EncryptionParameterQualifiers.</summary>
                <remarks>
                Creates a deep copy of a EncryptionParameterQualifiers. The created EncryptionParameterQualifiers 
                will have the same values as the original.
                </remarks>

                <param name="value">The EncryptionParameterQualifiers to copy from</param>
                */
                EncryptionParameterQualifiers(const seal::EncryptionParameterQualifiers &value);


            private:
                seal::EncryptionParameterQualifiers *qualifiers_;
            };

            /**
            <summary>Represents the user-customizable encryption scheme settings.</summary>

            <remarks>
            <para>
            Represents the user-customizable encryption scheme settings. Several settings (e.g., <see cref="PolyModulus"/>,
            <see cref="CoeffModulus"/>, <see cref="PlainModulus"/>) significantly affect the performance, capabilities, and
            security of the encryption scheme. KeyGenerator, Encryptor, Decryptor, Evaluator, and other objects in the library 
            all require the EncryptionParameters object to specify and agree on the encryption scheme settings.
            </para>
            <para>
            Picking appropriate encryption parameters is essential to enable a particular application while balancing performance
            and security. Some encryption settings will not allow some inputs (e.g., attempting to encrypt a polynomial with more
            coefficients than <see cref="PolyModulus"/> or larger coefficients than <see cref="PlainModulus"/>) or support some
            computations (with noise growing too fast as determined by <see cref="CoeffModulus"/> and
            <see cref="DecompositionBitCount"/>). The <see cref="ChooserPoly"/> and <see cref="ChooserEvaluator"/> classes provide
            functionality to help determine the best encryption parameters for an application. Additionally, please refer to
            external documentation for more details on how to determine the best parameters.
            </para>
            <para>
            After the user has set at least the <see cref="PolyModulus"/>, the <see cref="CoeffModulus"/>, and the 
            <see cref="PlainModulus"/>, and in many cases also the <see cref="DecompositionBitCount"/>, the parameters need to 
            be validated for correctness and functionality. This can be done using the function <see cref="Validate()"/>, which 
            returns an instance of EncryptionParameterQualifiers. If the returned instance of EncryptionParameterQualifiers has 
            the EncryptionParameterQualifiers::ParametersSet flag set to true, the parameter set is valid and ready to be used. 
            If the parameters were for some reason not appropriately set, the returned EncryptionParameterQualifiers instance 
            will have EncryptionParameterQualifiers::ParametersSet set to false. Any change to an already validated instance 
            of EncryptionParameters immediately invalidates it, requiring another call to <see cref="Validate()"/> before the
            parameters can be used.
            </para>
            <para>
            In general, reading from EncryptionParameters is thread-safe, while mutating is not. 
            </para>
            <para>
            Choosing inappropriate EncryptionParameters may load to an encryption scheme that is not secure, does not perform
            well, and/or does not support the input and computation of the application.
            </para>
            </remarks>
            */
            public ref class EncryptionParameters
            {
            public:
                /**
                <summary>Creates an empty EncryptionParameters instance.</summary>
                */
                EncryptionParameters();

                /**
                <summary>Creates an empty EncryptionParameters instance.</summary>

                <remarks>
                Creates an empty EncryptionParameters instance. The user can give a reference to a <see cref="MemoryPoolHandle"/> 
                object to use a custom memory pool instead of the global memory pool (default).
                </remarks>
                <param name="pool">The memory pool handle</param>
                <exception cref="System::ArgumentNullException">if pool is null</exception>
                <seealso cref="MemoryPoolHandle">See MemoryPoolHandle for more details on memory pool handles.</seealso>
                */
                EncryptionParameters(MemoryPoolHandle ^pool);
                
                /**
                <summary>Creates a copy of a EncryptionParameters.</summary>

                <param name="copy">The EncryptionParameters to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                EncryptionParameters(EncryptionParameters ^copy);

                /**
                <summary>Overwrites the EncryptionParameters instance with the specified instance.</summary>

                <param name="assign">The EncryptionParameters instance that should be assigned to the current instance</param>
                <exception cref="System::ArgumentNullException">if assign is null</exception>
                */
                void Set(EncryptionParameters ^assign);

                /**
                <summary>Returns a reference to the polynomial modulus (represented by <see cref="BigPoly"/>).</summary>

                <remarks>
                Returns a copy of the polynomial modulus parameter (represented by <see cref="BigPoly"/>). Note that
                the polynomial modulus directly determines the number of coefficients of encrypted polynomials, and the
                maximum number of coefficients for plaintext polynomials that are representable by the library.
                </remarks>
                */
                property BigPoly ^PolyModulus {
                    BigPoly ^get();
                }

                /**
                <summary>Sets the polynomial modulus parameter to the specified value (represented by <see cref="BigPoly"/>).</summary>
                
                <remarks>
                Sets the polynomial modulus parameter to the specified value (represented by <see cref="BigPoly"/>).
                Note that the polynomial modulus directly determines the number of coefficients of encrypted polynomials, 
                and the maximum number of coefficients for plaintext polynomials that are representable by the library. 
                This function automatically invalidates the EncryptionParameters instance, so calling <see cref="Validate()"/>
                is necessary before the parameters can be used.
                </remarks>
                <param name="polyModulus">The new polynomial modulus</param>
                <exception cref="System::ArgumentNullException">if polyModulus is null</exception>
                */
                void SetPolyModulus(BigPoly ^polyModulus);

                /**
                <summary>Sets the polynomial modulus parameter to the specified value (represented by System::String).</summary>

                <remarks>
                Sets the polynomial modulus parameter to the specified value (represented by System::String).
                Note that the polynomial modulus directly determines the number of coefficients of encrypted polynomials,
                and the maximum number of coefficients for plaintext polynomials that are representable by the library.
                This function automatically invalidates the EncryptionParameters instance, so calling <see cref="Validate()"/>
                is necessary before the parameters can be used.
                </remarks>
                <param name="polyModulus">The new polynomial modulus</param>
                <exception cref="System::ArgumentNullException">if polyModulus is null</exception>
                */
                void SetPolyModulus(System::String ^polyModulus);

                /**
                <summary>Returns a copy of the coefficient modulus parameter (represented by a <see cref="BigUInt"/>).</summary>

                <remarks>
                Returns a copy of the coefficient modulus parameter (represented by a <see cref="BigUInt"/>). Note that the
                coefficient modulus directly determines the number of bits-per-coefficient of encrypted polynomials and the
                maximum value allowed for <see cref="PlainModulus"/> (which should be significantly smaller than CoeffModulus).
                </remarks>
                */
                property BigUInt ^CoeffModulus {
                    BigUInt ^get();
                }

                /**
                <summary>Sets the coefficient modulus parameter to the specified value (represented by <see cref="BigUInt"/>).</summary>

                <remarks>
                Sets the coefficient modulus parameter to the specified value (represented by <see cref="BigUInt"/>).
                Note that the coefficient modulus directly determines the number of bits-per-coefficient of encrypted 
                polynomials and the maximum value allowed for <see cref="PlainModulus"/> (which should be significantly 
                smaller than CoeffModulus). This function automatically invalidates the EncryptionParameters instance, so 
                calling <see cref="Validate()"/> is necessary before the parameters can be used.
                </remarks>
                <param name="coeffModulus">The new coefficient modulus</param>
                <exception cref="System::ArgumentNullException">if coeffModulus is null</exception>
                */
                void SetCoeffModulus(BigUInt ^coeffModulus);

                /**
                <summary>Sets the coefficient modulus parameter to the specified value (represented by System::UInt64).</summary>

                <remarks>
                Sets the coefficient modulus parameter to the specified value (represented by System::UInt64).
                Note that the coefficient modulus directly determines the number of bits-per-coefficient of encrypted
                polynomials and the maximum value allowed for <see cref="PlainModulus"/> (which should be significantly
                smaller than CoeffModulus). This function automatically invalidates the EncryptionParameters instance, so
                calling <see cref="Validate()"/> is necessary before the parameters can be used.
                </remarks>
                <param name="coeffModulus">The new coefficient modulus</param>
                */
                void SetCoeffModulus(System::UInt64 coeffModulus);

                /**
                <summary>Sets the coefficient modulus parameter to the specified value (represented by System::String).</summary>

                <remarks>
                Sets the coefficient modulus parameter to the specified value (represented by <see System::String).
                Note that the coefficient modulus directly determines the number of bits-per-coefficient of encrypted
                polynomials and the maximum value allowed for <see cref="PlainModulus"/> (which should be significantly
                smaller than CoeffModulus). This function automatically invalidates the EncryptionParameters instance, so
                calling <see cref="Validate()"/> is necessary before the parameters can be used.
                </remarks>
                <param name="coeffModulus">The new coefficient modulus</param>
                <exception cref="System::ArgumentNullException">if coeffModulus is null</exception>
                */
                void SetCoeffModulus(System::String ^coeffModulus);
#ifndef DISABLE_NTT_IN_MULTIPLY
                // Not currently used
                property BigUInt ^AuxCoeffModulus {
                    BigUInt ^get();
                }

                // Not currently used
                void SetAuxCoeffModulus(System::UInt64 auxCoeffModulus);

                // Not currently used
                void SetAuxCoeffModulus(System::String ^auxCoeffModulus);
#endif
                /**
                <summary>Returns a copy of the plaintext modulus parameter (represented by a <see cref="BigUInt"/>).</summary>

                <remarks>
                Returns a copy of the plaintext modulus parameter (represented by a <see cref="BigUInt"/>). Note that the
                plaintext modulus is one greater than the maximum value allowed for any plaintext coefficient that the library
                can encrypt or represent.
                </remarks>
                */
                property BigUInt ^PlainModulus {
                    BigUInt ^get();
                }

                /**
                <summary>Sets the plaintext modulus parameter to the specified value (represented by <see cref="BigUInt"/>).</summary>

                <remarks>
                Sets the plaintext modulus parameter to the specified value (represented by <see cref="BigUInt"/>).
                Note that the plaintext modulus is one greater than the maximum value allowed for any plaintext 
                coefficient that the library can encrypt or represent. This function automatically invalidates 
                the EncryptionParameters instance, so calling <see cref="Validate()"/> is necessary before the 
                parameters can be used.
                </remarks>
                <param name="plainModulus">The new plaintext modulus</param>
                <exception cref="System::ArgumentNullException">if plainModulus is null</exception>
                */
                void SetPlainModulus(BigUInt ^plainModulus);

                /**
                <summary>Sets the plaintext modulus parameter to the specified value (represented by System::UInt64).</summary>

                <remarks>
                Sets the plaintext modulus parameter to the specified value (represented by System::UInt64).
                Note that the plaintext modulus is one greater than the maximum value allowed for any plaintext
                coefficient that the library can encrypt or represent. This function automatically invalidates
                the EncryptionParameters instance, so calling <see cref="Validate()"/> is necessary before the
                parameters can be used.
                </remarks>
                <param name="plainModulus">The new plaintext modulus</param>
                */
                void SetPlainModulus(System::UInt64 plainModulus);

                /**
                <summary>Sets the plaintext modulus parameter to the specified value (represented by System::String).</summary>

                <remarks>
                Sets the plaintext modulus parameter to the specified value (represented by System::String).
                Note that the plaintext modulus is one greater than the maximum value allowed for any plaintext
                coefficient that the library can encrypt or represent. This function automatically invalidates
                the EncryptionParameters instance, so calling <see cref="Validate()"/> is necessary before the
                parameters can be used.
                </remarks>
                <param name="plainModulus">The new plaintext modulus</param>
                <exception cref="System::ArgumentNullException">if plainModulus is null</exception>
                */
                void SetPlainModulus(System::String ^plainModulus);

                /**
                <summary>Returns the standard deviation of normalized noise used during key generation and encryption.</summary>
                */
                property double NoiseStandardDeviation {
                    double get();
                }

                /**
                <summary>Sets the standard deviation of normalized noise used during key generation and encryption.</summary>
                
                <remarks>
                Sets the standard deviation of normalized noise used during key generation and encryption. This function
                automatically invalidates the EncryptionParameters instance, so calling <see cref="Validate()"/> is necessary 
                before the parameters can be used.
                </remarks>
                <param name="value">The new standard deviation</param>
                */
                void SetNoiseStandardDeviation(double value);

                /**
                <summary>Returns the maximum deviation of normalized noise used during key generation and encryption.</summary>
                */
                property double NoiseMaxDeviation {
                    double get();
                }

                /**
                <summary>Sets the maximum deviation of normalized noise used during key generation and encryption.</summary>

                <remarks>
                Sets the maximum deviation of normalized noise used during key generation and encryption. This function
                automatically invalidates the EncryptionParameters instance, so calling <see cref="Validate()"/> is necessary 
                before the parameters can be used.
                </remarks>
                <param name="value">The new maximum deviation</param>
                */
                void SetNoiseMaxDeviation(double value);

                /**
                <summary> Returns the decomposition bit count which directly determines the number of evaluation keys required by
                the scheme.</summary>

                <remarks>
                Returns the decomposition bit count which directly determines the number of evaluation keys required by
                the scheme. Smaller decomposition bit count reduces the accumulation of noise during multiplication
                operations, but can also significantly increase the time required to perform multiplication.
                </remarks>
                */
                property int DecompositionBitCount {
                    int get();
                }

                /**
                <summary>Sets the decomposition bit count parameter to the specified value.</summary>

                <remarks>
                Sets the decomposition bit count parameter to the specified value. The decomposition bit count directly
                determines the number of evaluation keys required by the scheme. Smaller decomposition bit count reduces
                the accumulation of noise during multiplication operations, but can also significantly increase the time
                required to perform multiplication. This function automatically invalidates the EncryptionParameters
                instance, so calling <see cref="Validate()"/> is necessary before the parameters can be used.
                </remarks>
                <param name="value">The new decomposition bit count</param>
                */
                void SetDecompositionBitCount(int value);

                /**
                <summary>Validates the parameters set in the current EncryptionParameters instance, and returns a populated 
                instance of <see cref="EncryptionParameterQualifiers"/>.</summary>

                <remarks>
                Validates the parameters set in the current EncryptionParameters instance, and returns a populated instance
                of <see cref="EncryptionParameterQualifiers"/>. It is necessary to call this function before using the 
                the encryption parameters to create other classes such as <see cref="KeyGenerator"/>, <see cref="Encryptor"/>, 
                or <see cref="Evaluator"/>. Note also that any change to the parameters requires the Validate() function to be 
                called again before the parameters can be used.
                </remarks>
                <seealso cref="EncryptionParameterQualifiers"/>See EncryptionParameterQualifiers for more details on the returned qualifiers.</seealso>
                */
                EncryptionParameterQualifiers ^Validate();

                /**
                <summary>Saves the EncryptionParameters to an output stream.</summary>

                <remarks>
                Saves the EncryptionParameters to an output stream. The output is in binary format and is not human-readable. The
                output stream must have the "Binary" flag set.
                </remarks>
                <param name="stream">The stream to save the EncryptionParameters to</param>
                <exception cref="System::ArgumentNullException">If stream is null</exception>
                <seealso cref="Load()">See Load() to load a saved EncryptionParameters instance.</seealso>
                */
                void Save(System::IO::Stream ^stream);

                /**
                <summary>Loads the EncryptionParameters from an input stream overwriting the current EncryptionParameters.</summary>
                
                <remarks>
                Loads the EncryptionParameters from an input stream overwriting the current EncryptionParameters.
                This function automatically invalidates the EncryptionParameters instance, so calling <see cref="Validate()"/>
                is necessary before the parameters can be used.
                </remarks>
                <param name="stream">The stream to load the EncryptionParameters from</param>
                <exception cref="System::ArgumentNullException">If stream is null</exception>
                <seealso cref="Save()">See Save() to save an EncryptionParameters instance.</seealso>
                */
                void Load(System::IO::Stream ^stream);

                /**
                <summary>Returns the set of qualifiers (as an instance of <see cref="EncryptionParameterQualifiers"/>) 
                for the current encryption parameters.</summary>

                <remarks>Returns the set of qualifiers (as an instance of <see cref="EncryptionParameterQualifiers"/>)
                for the current encryption parameters. Note that to get an updated set of qualifiers it is necessary to call
                <see cref="Validate()"/> after any change to the encryption parameters.
                </remarks>
                <seealso cref="EncryptionParameterQualifiers">See EncryptionParameterQualifiers for more details.</seealso>
                */
                property EncryptionParameterQualifiers ^Qualifiers {
                    EncryptionParameterQualifiers ^get();
                }

                /**
                <summary>Computes and returns the maximum number of bits of inherent noise supported by the 
                current encryption parameters.</summary>

                <remarks>
                Computes and returns the maximum number of bits of inherent noise supported by the current encryption parameters. Any
                ciphertext with larger inherent noise is impossible to decrypt, even with the correct secret key. The function
                Decryptor::InherentNoiseBits() can be used to compute the number of bits of inherent noise in a given ciphertext.
                </remarks>
                <exception cref="System::InvalidOperationException">if the encryption parameters are not valid</exception>
                <seealso cref="InherentNoiseMax">See InherentNoiseMax for computing the exact maximum size of inherent noise.</seealso>
                */
                int InherentNoiseBitsMax();

                /**
                <summary>Computes the maximum amount of inherent noise supported by the current encryption parameters and stores 
                it in the given BigUInt.</summary>

                <remarks>
                Computes the maximum amount of inherent noise supported by the current encryption parameters and stores it in 
                the given BigUInt. Any ciphertext with larger inherent noise is impossible to decrypt, even with the correct 
                secret key. The function Decryptor::InherentNoiseBits() can be used to compute the number of bits of inherent 
                noise in a given ciphertext.
                </remarks>
                <param name="destination">The BigUInt to overwrite with the maximum inherent noise</param>
                <exception cref="System::ArgumentNullException">if destination is null</exception>
                <exception cref="System::InvalidOperationException">if the encryption parameters are not valid</exception>
                <seealso cref="InherentNoiseBitsMax">See InherentNoiseBitsMax for returning instead the significant bit count of the
                maximum size of inherent noise.</seealso>
                */
                void InherentNoiseMax(BigUInt ^destination);

                /**
                <summary>Invalidates the current encryption parameters.</summary>
                
                <remarks>
                Invalidates the current encryption parameters. Calling <see cref="Validate()" /> is necessary before the 
                parameters can be used again.
                </remarks>
                */
                void Invalidate();

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
                seal::EncryptionParameters &GetParameters();

                /**
                <summary>Creates a deep copy of the C++ EncryptionParameters.</summary>
                <remarks>
                Creates a deep copy of a EncryptionParameters. The created EncryptionParameters will have the same settings as the
                original.
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