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
            ref class EncryptionParameters;
            ref class Encryptor;
            ref class Decryptor;
            ref class Evaluator;
            ref class KeyGenerator;
            ref class Chooser;
            ref class Simulator;

            /**
            <summary>Enumeration of the different modes for the library.</summary>

            <remarks>
            <para>
            Enumeration of the different modes for the library. Mode is set through the Mode property of <see cref="EncryptionParameters"/>.
            EncryptionMode::Normal performs all encryption, evaluation, and decryption operations as normal. EncryptionMode::Test disables
            actual encryption while allowing the API to still function for the application. EncryptionMode::Test enables significantly
            faster library calls and may be very useful for testing and debugging applications.
            </para>
            <para>
            EncryptionMode::Test disables actual encryption in the library, so only use this mode for testing and debugging purposes when
            no security is required.
            </para>
            </remarks>
            <seealso cref="EncryptionParameters">See EncryptionParameters to set mode for the library.</seealso>
            */
            public enum struct EncryptionMode
            {
                /**
                <summary>EncryptionMode::Normal enables encryption and the normal functioning of the library.</summary>
                */
                Normal,

                /**
                <summary>EncryptionMode::Test disables encryption in the library.</summary>

                <remarks>
                <para>
                EncryptionMode::Test disables encryption in the library. The benefit of this mode is that most library functions
                (e.g., encryption, decryption, all Evaluator functions) complete significantly faster as they are not secure.
                This mode is useful when testing and debugging applications when security is not important.
                </para>
                <para>
                EncryptionMode::Test disables actual encryption in the library, so only use EncryptionMode::Test for testing and
                debugging purposes when no security is required.
                </para>
                </remarks>
                */
                Test
            };

            /**
            <summary>Represents the user-customizable encryption scheme settings.</summary>

            <remarks>
            <para>
            Represents the user-customizable encryption scheme settings. Several settings (e.g., <see cref="PolyModulus"/>,
            <see cref="CoeffModulus"/>, <see cref="PlainModulus"/>) significantly affect the performance, capabilities, and
            security of the encryption scheme. <see cref="KeyGenerator"/>, <see cref="Encryptor"/>, <see cref="Decryptor"/>,
            <see cref="Evaluator"/>, and other objects in the library all require the EncryptionParameters object to specify and
            agree on the encryption scheme settings.
            </para>
            <para>
            Picking appropriate encryption parameters is essential to enable a particular application while balancing performance
            and security. Some encryption settings will not allow some inputs (e.g., attempting to encrypt a polynomial with more
            coefficients than <see cref="PolyModulus"/> or larger coefficients than <see cref="PlainModulus"/>) or support some
            computations (with noise growing too fast as determined by <see cref="CoeffModulus"/> and
            <see cref="DecompositionBitCount"/>). The <see cref="Chooser"/> and <see cref="Simulator"/> classes provide
            functionality to help determine the best encryption parameters for an application. Additionally, please refer to
            external documentation for more details on how to determine the best parameters.
            </para>
            <para>
            In general, reading from EncryptionParameters is thread-safe while mutating is not. Refer to the thread safety of
            individual parameter types to assess their thread safety.
            </para>
            </remarks>
            <seealso cref="Chooser">See Chooser to help determine the best encryption parameters for an application.</seealso>
            <seealso cref="Simulator">See Simulator to help determine the best encryption parameters for an application.</seealso>
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
                <summary>Gets/sets the operating mode of the library, which is set to either EncryptionMode::Normal or
                EncryptionMode::Test.</summary>

                <remarks>
                Gets/sets the operating mode of the library, which is set to either EncryptionMode::Normal or EncryptionMode::Test.
                Setting the mode to EncryptionMode::Test disables all encryption but may significantly improve performance for
                cases where security is not important (e.g., testing and debugging).
                </remarks>
                */
                property EncryptionMode Mode {
                    EncryptionMode get();
                    void set(EncryptionMode mode);
                }

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

                <param name="value">The EncryptionParameters to copy from</param>
                */
                EncryptionParameters(const seal::EncryptionParameters &value);

            private:
                seal::EncryptionParameters *parms_;
            };
        }
    }
}