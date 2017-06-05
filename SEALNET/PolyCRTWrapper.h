#pragma once

#include "polycrt.h"
#include "EncryptionParamsWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            /**
            <summary>Provides functionality for encrypting several plaintext numbers into one ciphertext 
            for improved memory efficiency and efficient vector operations (SIMD).</summary>

            <remarks>
            <para>
            Provides functionality for encrypting several plaintext numbers into one ciphertext for improved 
            memory efficiency and efficient vector operations (SIMD). Multiplying and adding such ciphertexts 
            together performs the respective operation on each of the slots independently and simultaneously.
            This functionality is often called "batching" in homomorphic encryption literature.
            </para>
            <para>
            Mathematically speaking, if PolyModulus is X^N+1, N is a power of two, and PlainModulus
            is a prime number t such that 2N divides t-1, then integers modulo t contain a primitive
            2N-th root of unity and the polynomial X^N+1 splits into n distinct linear factors as
            X^N+1 = (X-a_1)*...*(X-a_N) mod t, where the constants a_1, ..., a_n are all the distinct
            primitive 2N-th roots of unity in integers modulo t. The Chinese Remainder Theorem (CRT)
            states that the plaintext space Z_t[X]/(X^N+1) in this case is isomorphic (as an algebra)
            to the N-fold direct product of fields Z_t. The isomorphism is easy to compute explicitly
            in both directions, which is what an instance of this class can be used for.
            </para>
            <para>
            Whether batching can be used depends strongly on the encryption parameters. Thus, to
            construct a PolyCRTBuilder the user must provide an instance of <see cref="EncryptionParameters"/>
            such that the <see cref="EncryptionParameterQualifiers"/> object returned by the function
            EncryptionParameters::GetQualifiers() has the flags ParametersSet and EnableBatching
            set to true.
            </para>
            </remarks>
            <seealso cref="EncryptionParameters"/>See EncryptionParameters for more information about encryption parameters.</seealso>
            <seealso cref="EncryptionParameterQualifiers"/>See EncryptionParameterQualifiers for more information about encryption parameters that support batching.</seealso>
            */
            public ref class PolyCRTBuilder
            {
            public:
                /**
                <summary>Creates a PolyCRTBuilder instance given a set of encryption parameters.</summary>

                <remarks>
                Creates a PolyCRTBuilder instance given a set of encryption parameters. It is necessary that the given set of
                encryption parameters supports batching.
                </remarks>
                <param name="parms">The encryption parameters</param>
                <exception cref="System::ArgumentNullException">if parms is null</exception>
                <exception cref="System::ArgumentException">if parms are not valid or do not support batching</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more information about encryption
                parameters.</seealso>
                <seealso cref="EncryptionParameterQualifiers">See EncryptionParameterQualifiers for more information about encryption
                parameters that support batching.</seealso>
                */
                PolyCRTBuilder(EncryptionParameters ^parms);

                /**
                <summary>Creates a PolyCRTBuilder instance given a set of encryption parameters.</summary>

                <remarks>
                Creates a PolyCRTBuilder instance given a set of encryption parameters. It is necessary that the given set of
                encryption parameters supports batching. The user can give a <see cref="MemoryPoolHandle "/> object to use 
                a custom memory pool instead of the global memory pool (default).
                </remarks>
                <param name="parms">The encryption parameters</param>
                <param name="pool">The memory pool handle</param>
                <exception cref="System::ArgumentNullException">if parms or pool is null</exception>
                <exception cref="System::ArgumentException">if parms are not valid or do not support batching</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more information about encryption
                parameters.</seealso>
                <seealso cref="EncryptionParameterQualifiers">See EncryptionParameterQualifiers for more information about encryption
                parameters that support batching.</seealso>
                <seealso cref="MemoryPoolHandle">See MemoryPoolHandle for more details on memory pool handles.</seealso>
                */
                PolyCRTBuilder(EncryptionParameters ^parms, MemoryPoolHandle ^pool);

                /**
                <summary>Creates a copy of a PolyCRTBuilder.</summary>

                <param name="copy">The PolyCRTBuilder to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                PolyCRTBuilder(PolyCRTBuilder ^copy);

                /**
                <summary>Destroys the PolyCRTBuilder</summary>
                */
                ~PolyCRTBuilder();

                /**
                <summary>Destroys the PolyCRTBuilder</summary>
                */
                !PolyCRTBuilder();

                /**
                <summary>Writes a given list of unsigned integers modulo the plaintext modulus into the slots of 
                a given plaintext polynomial.</summary>

                <remarks>
                <para>
                Writes a given list of unsigned integers modulo the plaintext modulus into the slots of a given 
                plaintext polynomial.
                </para>
                <para>
                The number of elements in the list of inputs must be equal to the number of slots, which is
                equal to the degree of the polynomial modulus. Each entry in the list of inputs must have
                bit count equal to that of the plaintext modulus and is expected to be already reduced modulo
                the plaintext modulus. The destination polynomial will automatically be resized to have correct
                size, i.e. the same number of coefficients as the polynomial modulus, and each coefficient of the
                same bit count as the plaintext modulus.
                </para>
                </remarks>
                <param name="values">The list of values to write into the slots</param>
                <param name="destination">The plaintext polynomial to overwrite with the result</param>
                <exception cref="System::ArgumentNullException">if values or destination is null</exception>
                <exception cref="System::ArgumentException">if the values list has incorrect size</exception>
                <exception cref="System::ArgumentException">if the entries in the values list have incorrect size</exception>
                */
                void Compose(System::Collections::Generic::List<BigUInt^> ^values, BigPoly ^destination);

                /**
                <summary>Writes a given list of unsigned integers (represented by System::UInt64) modulo the 
                plaintext modulus into the slots of a given plaintext polynomial.</summary>

                <remarks>
                <para>
                Writes a given list of unsigned integers (represented by System::UInt64) modulo the plaintext 
                modulus into the slots of a given plaintext polynomial.
                </para>
                <para>
                The number of elements in the list of inputs must be equal to the number of slots, which is
                equal to the degree of the polynomial modulus. Each entry in the vector of inputs must have value
                less than the plaintext modulus. The destination polynomial will automatically be resized to have 
                correct size, i.e. the same number of coefficients as the polynomial modulus, and each coefficient 
                of the same bit count as the plaintext modulus.
                </para>
                </remarks>
                <param name="values">The list of values to write into the slots</param>
                <param name="destination">The plaintext polynomial to overwrite with the result</param>
                <exception cref="System::ArgumentNullException">if values or destination is null</exception>
                <exception cref="System::InvalidOperationException">if the plaintext modulus is bigger than 64 bits</exception>
                <exception cref="System::ArgumentException">if the values list has incorrect size</exception>
                <exception cref="System::ArgumentException">if the entries in the values list have incorrect size</exception>
                */
                void Compose(System::Collections::Generic::List<System::UInt64> ^values, BigPoly ^destination);

                /**
                <summary>Writes a given list of unsigned integers modulo the plaintext modulus into the slots of 
                a plaintext polynomial, and returns it.</summary>

                <remarks>
                <para>
                Writes a given list of unsigned integers modulo the plaintext modulus into the slots of
                a plaintext polynomial, and returns it.
                </para>
                <para>
                The number of elements in the list of inputs must be equal to the number of slots, which is
                equal to the degree of the polynomial modulus. Each entry in the list of inputs must have
                bit count equal to that of the plaintext modulus and is expected to be already reduced modulo
                the plaintext modulus. The returned polynomial will automatically be resized to have correct
                size, i.e. the same number of coefficients as the polynomial modulus, and each coefficient of the
                same bit count as the plaintext modulus.
                </para>
                </remarks>
                <param name="values">The list of values to write into the slots</param>
                <exception cref="System::ArgumentNullException">if values is null</exception>
                <exception cref="System::ArgumentException">if the values list has incorrect size</exception>
                <exception cref="System::ArgumentException">if the entries in the values list have incorrect size</exception>
                */
                BigPoly ^Compose(System::Collections::Generic::List<BigUInt^> ^values);

                /**
                <summary>Writes a given list of unsigned integers (represented by System::UInt64) modulo the 
                plaintext modulus into the slots of a plaintext polynomial, and returns it.</summary>

                <remarks>
                <para>
                Writes a given list of unsigned integers modulo the plaintext modulus into the slots of
                a plaintext polynomial, and returns it.
                </para>
                <para>
                The number of elements in the vector of inputs must be equal to the number of slots, which is
                equal to the degree of the polynomial modulus. Each entry in the vector of inputs must have value
                less than the plaintext modulus. The returned polynomial will automatically be resized to have correct
                size, i.e. the same number of coefficients as the polynomial modulus, and each coefficient of the
                same bit count as the plaintext modulus.
                </para>
                </remarks>
                <param name="values">The list of values to write into the slots</param>
                <exception cref="System::ArgumentNullException">if values is null</exception>
                <exception cref="System::InvalidOperationException">if the plaintext modulus is bigger than 64 bits</exception>
                <exception cref="System::ArgumentException">if the values list has incorrect size</exception>
                <exception cref="System::ArgumentException">if the entries in the values list have incorrect size</exception>
                */
                BigPoly ^Compose(System::Collections::Generic::List<System::UInt64> ^values);

                /**
                <summary>Reads the values in the slots of a given plaintext polynomial and writes them as the entries of 
                a given list.</summary>
                <remarks>
                <para>Reads the values in the slots of a given plaintext polynomial and writes them as the entries of 
                a given list. This is the inverse of what <see cref="Compose(System::Collections::Generic::List<BigUInt^>^,BigPoly^)"/> 
                does.
                </para>
                <para>
                The plaintext polynomial poly must have the same number of coefficients as the polynomial modulus,
                and its coefficients must have bit count equal to that of the plaintext modulus. Moreover, poly is
                expected to be already reduced modulo polynomial modulus, and its coefficients are expected
                to be already reduced modulo the plaintext modulus. The number of elements in the destination
                list will be set to equal the number of slots, the elements themselves will be set to have
                bit count equal to that of the plaintext modulus, and their values will be reduced modulo the
                plaintext modulus.
                </para>
                </remarks>
                <param name="poly">The plaintext polynomial from which the slots will be read</param>
                <param name="destination">The list to be overwritten with the values of the slots</param>
                <exception cref="System::ArgumentNullException">if poly or destination is null</exception>
                <exception cref="System::ArgumentException">if poly has incorrect size</exception>
                */
                void Decompose(BigPoly ^poly, System::Collections::Generic::List<BigUInt^> ^destination);

                /**
                <summary>Reads the values in the slots of a given plaintext polynomial and writes them as the entries of a list,
                which is then returned.</summary>
                <remarks>
                <para>Reads the values in the slots of a given plaintext polynomial and writes them as the entries of a list,
                which is then returned. This is the inverse of what <see cref="Compose(System::Collections::Generic::List<BigUInt^>^)"/>
                does.
                </para>
                <para>
                The plaintext polynomial poly must have the same number of coefficients as the polynomial modulus, and its coefficients
                must have bit count equal to that of the plaintext modulus. Moreover, poly is expected to be already reduced modulo
                polynomial modulus, and its coefficients are expected to be already reduced modulo the plaintext modulus. The number
                of elements in the returned list will be equal the number of slots, the elements themselves will have bit count
                equal to that of the plaintext modulus, and their values will be reduced modulo the plaintext modulus.
                </para>
                </remarks>
                <param name="poly">The plaintext polynomial from which the slots will be read</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                <exception cref="System::ArgumentException">if poly has incorrect size</exception>
                */
                System::Collections::Generic::List<BigUInt^> ^Decompose(BigPoly ^poly);

                /**
                <summary>Returns the number of slots.</summary>
                */
                property int SlotCount
                {
                    int get();
                }

                /**
                <summary>Returns a reference to the underlying C++ PolyCRTBuilder.</summary>
                */
                seal::PolyCRTBuilder &GetPolyCRTBuilder();

            private:
                seal::PolyCRTBuilder *polyCRTBuilder_;
            };
        }
    }
}
