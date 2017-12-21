#pragma once

#include "seal/util/defines.h"
#include "seal/context.h"
#include "sealnet/EncryptionParamsWrapper.h"
#include "sealnet/BigUIntWrapper.h"
#include "sealnet/BigPolyWrapper.h"
#include "sealnet/SmallModulusWrapper.h"
#include "sealnet/MemoryPoolHandleWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class EncryptionParameters;

            ref class SEALContext;

            /**
            <summary>Stores a set of attributes (qualifiers) of a set of encryption parameters.</summary>

            <remarks>
            Stores a set of attributes (qualifiers) of a set of encryption parameters. These 
            parameters are mainly used internally in various parts of the library, e.g. to 
            determine which algorithmic optimizations the current support. The qualifiers are 
            automatically created by the <see cref="SEALContext" /> class, silently passed 
            on to classes such as <see cref="Encryptor" />, <see cref="Evaluator" />, and 
            <see cref="Decryptor" />, and the only way to change them is by changing the 
            encryption parameters themselves. In other words, a user will never have to create 
            their own instance of EncryptionParameterQualifiers, and in most cases never have 
            to worry about them at all.
            </remarks>
            <seealso>See EncryptionParameters::GetQualifiers() for obtaining the 
            EncryptionParameterQualifiers corresponding to a certain parameter set.</seealso>
            */
            public ref class EncryptionParameterQualifiers
            {
            public:
                /**
                <summary>If the encryption parameters are set in a way that is considered valid by SEAL,
                the variable parameters_set is set to true.</summary>
                */
                property bool ParametersSet {
                    bool get();
                }

                /**
                <summary>Tells whether FFT can be used for polynomial multiplication.</summary>

                <remarks>
                Tells whether FFT can be used for polynomial multiplication. If the polynomial modulus 
                is of the form X^N+1, where N is a power of two, then FFT can be used for fast 
                multiplication of polynomials modulo the polynomial modulus. In this case the variable 
                EnableFFT will be set to true. However, currently SEAL requires the polynomial modulus
                to be of this form for the parameters to be valid. Therefore, ParametersSet can only 
                be true if EnableFFT is true.
                </remarks>
                */
                property bool EnableFFT {
                    bool get();
                }

                /**
                <summary>Tells whether NTT can be used for polynomial multiplication.</summary>

                <remarks>
                Tells whether NTT can be used for polynomial multiplication. If the primes in the
                coefficient modulus are congruent to 1 modulo 2N, where X^N+1 is the polynomial
                modulus and N is a power of two, then the number-theoretic transform (NTT) can be
                used for fast multiplications of polynomials modulo the polynomial modulus and
                coefficient modulus. In this case the variable EnableNTT will be set to true. However,
                currently SEAL requires this to be the case for the parameters to be valid. Therefore,
                ParametersSet can only be true if EnableNTT is true.
                </remarks>
                */
                property bool EnableNTT {
                    bool get();
                }

                /**
                <summary>Tells whether batching is supported by the encryption parameters.</summary>

                <remarks>
                Tells whether batching is supported by the encryption parameters. If the plaintext
                modulus is congruent to 1 modulo 2N, where X^N+1 is the polynomial modulus and N is
                a power of two, then it is possible to use the PolyCRTBuilder class to view plaintext
                elements as 2-by-(N/2) matrices of integers modulo the plaintext modulus. This is
                called batching, and allows the user to operate on the matrix elements (slots) in
                a SIMD fashion, and rotate the matrix rows and columns. When the computation is
                easily vectorizable, using batching can yield a huge performance boost. If the
                encryption parameters support batching, the variable EnableBatching is set to true.
                </remarks>
                */
                property bool EnableBatching {
                    bool get();
                }

                /**
                <summary>Tells whether fast plain lift is supported by the encryption parameters.</summary>

                <remarks>
                Tells whether fast plain lift is supported by the encryption parameters. A certain
                performance optimization in multiplication of a ciphertext by a plaintext
                (Evaluator::MultiplyPlain) and in transforming a plaintext element to NTT domain
                (Evaluator::TransformToNTT) can be used when the plaintext modulus is smaller than
                each prime in the coefficient modulus. In this case the variable EnableFastPlainLift
                is set to true.
                </remarks>
                */
                property bool EnableFastPlainLift {
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
            <summary>Performs sanity checks (validation) and pre-computations for a given set
            of encryption parameters.</summary>

            <remarks>
            <para>
            Performs sanity checks (validation) and pre-computations for a given set of encryption
            parameters. While the <see cref="EncryptionParameters" /> class is intended to be 
            a light-weight class to store the encryption parameters, the SEALContext class is 
            a heavy-weight class that is constructed from a given set of encryption parameters. 
            It validates the parameters for correctness, evaluates their properties, and performs 
            and stores the results of several costly pre-computations.
            </para>
            <para>
            After the user has set at least the PolyModulus, CoeffModulus, and PlainModulus
            parameters in a given <see cref="EncryptionParameters" /> instance, the parameters 
            can be validated for correctness and functionality by constructing an instance of 
            SEALContext. The constructor of SEALContext does all of its work automatically, 
            and concludes by constructing and storing an instance of the <see cref="EncryptionParameterQualifiers" />
            class, with its flags set according to the properties of the given parameters. If 
            the created instance of <see cref="EncryptionParameterQualifiers" /> has the ParametersSet 
            flag set to true, the given parameter set has been deemed valid and is ready to be 
            used. If the parameters were for some reason not appropriately set, the ParametersSet 
            flag will be false, and a new SEALContext will have to be created after the parameters 
            are corrected.
            </para>
            </remarks>
            <seealso>See <see cref="EncryptionParameters" /> for more details on the parameters.</seealso>
            <seealso>See <see cref="EncryptionParameterQualifiers" /> for more details on the qualifiers.</seealso>
            */
            public ref class SEALContext
            {
            public:
                /**
                <summary>Creates an instance of SEALContext, and performs several pre-computations
                on the given EncryptionParameters.</summary>

                <remarks>
                Creates an instance of SEALContext, and performs several pre-computations on the 
                given <see cref="EncryptionParameters" />. The results of the pre-computations are stored 
                in allocations from the global memory pool.
                </remarks>
                <param name="parms">The encryption parameters</param>
                <exception cref="System::ArgumentNullException">if parms is null</exception>
                */
                SEALContext(EncryptionParameters ^parms);

                /**
                <summary>Creates an instance of SEALContext, and performs several pre-computations
                on the given EncryptionParameters.</summary>

                <remarks>
                Creates an instance of SEALContext, and performs several pre-computations on the 
                given <see cref="EncryptionParameters" />. The results of the pre-computations are stored
                in allocations from the memory pool pointed to by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="parms">The encryption parameters</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory poole</param>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if parms or pool is null</exception>
                <seealso>See <see cref="EncryptionParameters" /> for more details on the parameters.</seealso>
                <seealso cref="MemoryPoolHandle">See MemoryPoolHandle for more details on memory pool 
                handles.</seealso>
                */
                SEALContext(EncryptionParameters ^parms, MemoryPoolHandle ^pool);

                /**
                <summary>Creates a new SEALContext instance by creating a deep copy of a given 
                instance.</summary>

                <param name="copy">The SEALContext to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                SEALContext(SEALContext ^copy);

                /**
                <summary>Returns a copy of the underlying encryption parameters.</summary>
                */
                property EncryptionParameters ^Parms {
                    EncryptionParameters ^get();
                }

                /**
                <summary>Returns a copy of EncryptionParameterQualifiers corresponding to the current
                encryption parameters.</summary>
                
                </remarks>
                Returns a copy of <see cref="EncryptionParameterQualifiers" /> corresponding to 
                the current encryption parameters. Note that to change the qualifiers it is necessary 
                to create a new instance of SEALContext once appropriate changes to the encryption 
                parameters have been made.
                */
                property EncryptionParameterQualifiers ^Qualifiers {
                    EncryptionParameterQualifiers ^get();
                }

                /**
                <summary>Returns a copy of the polynomial modulus that was given in the encryption 
                parameters.</summary>
                */
                property BigPoly ^PolyModulus {
                    BigPoly ^get();
                }

                /**
                <summary>Returns a copy of the coefficient modulus that was given in the encryption
                parameters.</summary>
                */
                property System::Collections::Generic::List<SmallModulus^> ^CoeffModulus {
                    System::Collections::Generic::List<SmallModulus^> ^get();
                }

                /**
                <summary>Returns a copy of the plaintext modulus that was given in the encryption
                parameters.</summary>
                */
                property SmallModulus ^PlainModulus {
                    SmallModulus ^get();
                }

                /**
                <summary>Returns the standard deviation of the noise distribution that was given 
                in the encryption parameters.</summary>
                */
                property double NoiseStandardDeviation {
                    double get();
                }

                /**
                <summary>Returns the maximum deviation of the noise distribution that was given
                in the encryption parameters.</summary>
                */
                property double NoiseMaxDeviation {
                    double get();
                }

                /**
                <summary>Returns a constant reference to a pre-computed product of all primes 
                in the coefficient modulus.</summary>

                <remarks>
                Returns a constant reference to a pre-computed product of all primes in the
                coefficient modulus. The security of the encryption parameters largely depends
                on the bit-length of this product, and on the degree of the polynomial modulus.
                </remarks>
                */
                property BigUInt ^TotalCoeffModulus {
                    BigUInt ^get();
                }

                /**
                <summary>Destroys the SEALContext.</summary>
                */
                ~SEALContext();

                /**
                <summary>Destroys the SEALContext.</summary>
                */
                !SEALContext();

            internal:
                /**
                <summary>Returns a reference to the underlying C++ SEALContext.</summary>
                */
                seal::SEALContext &GetContext();

                /**
                <summary>Creates a deep copy of the C++ SEALContext.</summary>

                <remarks>
                Creates a deep copy of a C++ SEALContext. The created SEALContext
                will have the same settings as the original.
                </remarks>
                <param name="context">The SEALContext to copy from</param>
                */
                SEALContext(const seal::SEALContext &context);

            private:
                seal::SEALContext *context_;
            };
        }
    }
}