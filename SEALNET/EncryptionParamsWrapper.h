#pragma once

#include "encryptionparams.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class BigPoly;

            ref class BigUInt;

            ref class Encryptor;

            ref class Decryptor;

            ref class Evaluator;

            ref class ChooserPoly;

            ref class ChooserEvaluator;

            ref class EncryptionParameters;

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
            In general, reading from EncryptionParameters is thread-safe while mutating is not. Refer to the thread-safety of
            individual parameter types to assess their thread-safety.
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
                <summary>Returns a reference to the polynomial modulus (represented by a BigPoly) used for encryption.</summary>

                <remarks>
                Returns a reference to the polynomial modulus (represented by a <see cref="BigPoly"/>) used for encryption. Note
                that the polynomial modulus also directly determines the number of coefficients of encrypted polynomials and the
                maximum number of coefficients for plain-text polynomials that are representable by the library.
                </remarks>
                */
                property BigPoly ^PolyModulus {
                    BigPoly ^get();
                }

                /**
                <summary>Returns a reference to the coefficient modulus (represented by a BigUInt) used for encryption.</summary>

                <remarks>
                Returns a reference to the coefficient modulus (represented by a <see cref="BigUInt"/>) used for encryption. Note
                that the coefficient modulus directly determines the number of bits-per-coefficient of encrypted polynomials and
                the maximum value allowed for <see cref="PlainModulus"/> (which should be significantly smaller than the
                coefficient modulus).
                </remarks>
                */
                property BigUInt ^CoeffModulus {
                    BigUInt ^get();
                }

                property BigUInt ^AuxCoeffModulus {
                    BigUInt ^get();
                }

                /**
                <summary>Returns a reference to the plain-text modulus (represented by a BigUInt) used for encryption.</summary>

                <remarks>
                Returns a reference to the plain-text modulus (represented by a <see cref="BigUInt"/>) used for encryption. Note that
                the plain-text modulus is one greater than the maximum value allowed for any plain-text coefficient that the library
                can encrypt or represent.
                </remarks>
                */
                property BigUInt ^PlainModulus {
                    BigUInt ^get();
                }

                /**
                <summary>Gets/sets the standard-deviation of normalized noise used during key generation and encryption.</summary>

                <remarks>
                Gets/sets the standard-deviation of normalized noise used during key generation and encryption. Setting the noise
                standard deviation too small is not secure.
                </remarks>
                */
                property double NoiseStandardDeviation {
                    double get();
                    void set(double value);
                }

                /**
                <summary>Gets/sets the maximum deviation of normalized noise used during key generation and encryption.</summary>

                <remarks>
                Gets/sets the maximum deviation of normalized noise used during key generation and encryption. Setting the noise
                maximum deviation too small is not secure.
                </remarks>
                */
                property double NoiseMaxDeviation {
                    double get();
                    void set(double value);
                }

                /**
                <summary>Gets/sets the decomposition bit count which directly determines the number of evaluation keys required by
                the scheme.</summary>

                <remarks>
                Gets/sets the decomposition bit count which directly determines the number of evaluation keys required by the
                scheme. Smaller decomposition bit count reduces the accumulation of noise during multiplication operations, but
                can also significantly increase the time required to perform multiplication.
                </remarks>
                */
                property int DecompositionBitCount {
                    int get();
                    void set(int value);
                }

                /**
                <summary>Returns the set of qualifiers (as an instance of <see cref="EncryptionParameterQualifiers"/>) 
                for the current encryption parameters.</summary>
                <seealso cref="EncryptionParameterQualifiers">See EncryptionParameterQualifiers for more details.</seealso>
                */
                property EncryptionParameterQualifiers ^Qualifiers {
                    EncryptionParameterQualifiers ^get();
                }

                /**
                <summary>Computes and returns the maximum number of bits of inherent noise supported by the 
                current encryption parameters.</summary>

                <remarks>
                <para>
                Computes and returns the maximum number of bits of inherent noise supported by the current encryption parameters. Any
                ciphertext with larger inherent noise is impossible to decrypt, even with the correct secret key. The function
                Decryptor::InherentNoiseBits() can be used to compute the number of bits of inherent noise in a given ciphertext.
                </para>
                <para>
                Technically speaking, the inherent noise of a ciphertext is a polynomial, but the condition for decryption working
                depends on the size of the largest absolute value of its coefficients. It is this largest absolute value that we will
                call the "noise", the "inherent noise", or the "error", in this documentation. The reader is referred to the
                description of the encryption scheme for more details.
                </para>
                </remarks>
                <exception cref="System::InvalidOperationException">if the encryption parameters are not valid</exception>
                <seealso cref="InherentNoiseMax">See InherentNoiseMax for computing the exact maximum size of inherent noise.</seealso>
                */
                int InherentNoiseBitsMax();

                /**
                <summary>Computes the maximum amount of inherent noise supported by the current encryption parameters and stores 
                it in the given BigUInt.</summary>

                <remarks>
                <para>
                Computes the maximum amount of inherent noise supported by the current encryption parameters and stores it in 
                the given BigUInt. Any ciphertext with larger inherent noise is impossible to decrypt, even with the correct 
                secret key. The function Decryptor::InherentNoiseBits() can be used to compute the number of bits of inherent 
                noise in a given ciphertext.
                </para>
                <para>
                Technically speaking, the inherent noise of a ciphertext is a polynomial, but the condition for decryption working
                depends on the size of the largest absolute value of its coefficients. It is this largest absolute value that we will
                call the "noise", the "inherent noise", or the "error", in this documentation. The reader is referred to the
                description of the encryption scheme for more details.
                </para>
                </remarks>
                <param name="destination">The BigUInt to overwrite with the maximum inherent noise</param>
                <exception cref="System::InvalidOperationException">if the encryption parameters are not valid</exception>
                <seealso cref="InherentNoiseBitsMax">See InherentNoiseBitsMax for returning instead the significant bit count of the
                maximum size of inherent noise.</seealso>
                */
                void InherentNoiseMax(BigUInt ^destination);

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
                <param name="stream">The stream to load the EncryptionParameters from</param>
                <exception cref="System::ArgumentNullException">If stream is null</exception>
                <seealso cref="Save()">See Save() to save an EncryptionParameters instance.</seealso>
                */
                void Load(System::IO::Stream ^stream);

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