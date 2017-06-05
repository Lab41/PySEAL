#pragma once

#include "encoder.h"
#include "EncryptionParamsWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class BinaryFractionalEncoder;

            ref class BalancedFractionalEncoder;

            ref class BalancedEncoder;

            /**
            <summary>Encodes integers into plaintext polynomials that Encryptor can encrypt.</summary>
            <remarks>
            <para>
            Encodes integers into plaintext polynomials that Encryptor can encrypt. An instance of the BinaryEncoder class converts
            an integer into a plaintext polynomial by placing its binary digits as the coefficients of the polynomial. Decoding the
            integer amounts to evaluating the plaintext polynomial at X=2.
            </para>
            <para>
            Addition and multiplication on the integer side translate into addition and multiplication on the encoded plaintext
            polynomial side, provided that the length of the polynomial never grows to be of the size of the polynomial modulus
            (polyModulus), and that the coefficients of the plaintext polynomials appearing throughout the computations never
            experience coefficients larger than the plaintext modulus (plainModulus).
            </para>
            <para>
            Negative integers are represented by using -1 instead of 1 in the binary representation, and the negative coefficients
            are stored in the plaintext polynomials as unsigned integers that represent them modulo the plaintext modulus. Thus,
            for example, a coefficient of -1 would be stored as a polynomial coefficient plainModulus-1.
            </para>
            <para>
            In many cases it is a bad idea to throw an exception when decoding fails due to an overflow
            condition, e.g. <see cref="DecodeUInt32"> is given a polynomial that decodes into a value larger 
            than 32 bits. If such an exception is not handled silently, a malicious observer might notice
            this happening, and be able to extract one bit of information about the secret key.
            Thus, by default, the decoder functions do not throw exceptions in cases of overflow.
            However, the throws are automatically enabled if SEAL is compiled in debug mode in Visual Studio, 
            and more generally they can be directly controlled by the preprocessor define THROW_ON_DECODER_OVERFLOW
            in the file SEAL/util/defines.h.
            </para>
            </remarks>
            <seealso cref="BinaryFractionalEncoder">See BinaryFractionalEncoder for encoding real numbers.</seealso>
            <seealso cref="BalancedEncoder">See BalancedEncoder for encoding using base-b representation for b greater than 2.</seealso>
            <see also cref="IntegerEncoder">See IntegerEncoder for a common interface to all integer encoders.</seealso>
            */
            public ref class BinaryEncoder
            {
            public:
                /**
                <summary>Creates a BinaryEncoder object.</summary>
                <remarks>
                Creates a BinaryEncoder object. The constructor takes as input a reference to the plaintext modulus (represented by
                <see cref="BigUInt"/>).
                </remarks>
                <param name="plainModulus">The plaintext modulus (represented by BigUInt)</param>
                <exception cref="System::ArgumentNullException">if plainModulus is null</exception>
                <exception cref="System::ArgumentException">if plainModulus is not at least 2</exception>
                */
                BinaryEncoder(BigUInt ^plainModulus);

                /**
                <summary>Creates a BinaryEncoder object.</summary>
                <remarks>
                Creates a BinaryEncoder object. The constructor takes as input a reference to the plaintext modulus 
                (represented by <see cref="BigUInt"/>). The user can give a <see cref="MemoryPoolHandle "/> object 
                to use a custom memory pool instead of the global memory pool (default).
                </remarks>
                <param name="plainModulus">The plaintext modulus (represented by BigUInt)</param>
                <param name="pool">The memory pool handle</param>
                <exception cref="System::ArgumentNullException">if plainModulus or pool is null</exception>
                <exception cref="System::ArgumentException">if plainModulus is not at least 2</exception>
                <seealso cref="MemoryPoolHandle">See MemoryPoolHandle for more details on memory pool handles.</seealso>
                */
                BinaryEncoder(BigUInt ^plainModulus, MemoryPoolHandle ^pool);

                /**
                <summary>Creates a copy of a BinaryEncoder.</summary>

                <param name="copy">The BinaryEncoder to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                BinaryEncoder(BinaryEncoder ^copy);

                /**
                <summary>Destroys the BinaryEncoder.</summary>
                */
                ~BinaryEncoder();

                /**
                <summary>Destroys the BinaryEncoder.</summary>
                */
                !BinaryEncoder();

                /**
                <summary>Returns a reference to the plaintext modulus.</summary>
                */
                property BigUInt ^PlainModulus
                {
                    BigUInt ^get();
                }

                /**
                <summary>Returns the base used for encoding (2).</summary>
                */
                property System::UInt64 Base
                {
                    System::UInt64 get();
                }

                /**
                <summary>Encodes an unsigned integer (represented by System::UInt64) into a plaintext polynomial.</summary>
                <param name="value">The unsigned integer to encode</param>
                */
                BigPoly ^Encode(System::UInt64 value);

                /**
                <summary>Encodes an unsigned integer (represented by System::UInt64) into a plaintext polynomial.</summary>
                <param name="value">The unsigned integer to encode</param>
                <param name="destination">The polynomial to overwrite with the encoding</param>
                <exception cref="System::ArgumentNullException">if destination is null</exception>
                */
                void Encode(System::UInt64 value, BigPoly ^destination);

                /**
                <summary>Encodes an unsigned integer (represented by <see cref="BigUInt"/>) into a plaintext polynomial.</summary>
                <param name="value">The unsigned integer to encode</param>
                <exception cref="System::ArgumentNullException">if value is null</exception>
                */
                BigPoly ^Encode(BigUInt ^value);

                /**
                <summary>Encodes an unsigned integer (represented by <see cref="BigUInt"/>) into a plaintext polynomial.</summary>
                <param name="value">The unsigned integer to encode</param>
                <param name="destination">The polynomial to overwrite with the encoding</param>
                <exception cref="System::ArgumentNullException">if value or destination is null</exception>
                */
                void Encode(BigUInt ^value, BigPoly ^destination);

                /**
                <summary>Encodes a signed integer (represented by System::Int64) into a plaintext polynomial.</summary>
                <remarks>
                <para>
                Encodes a signed integer (represented by System::Int64) into a plaintext polynomial.
                </para>
                <para>
                Negative integers are represented by using -1 instead of 1 in the binary representation, and the negative coefficients
                are stored in the plaintext polynomials as unsigned integers that represent them modulo the plaintext modulus. Thus,
                for example, a coefficient of -1 would be stored as a polynomial coefficient plainModulus-1.
                </para>
                </remarks>
                <param name="value">The signed integer to encode</param>
                */
                BigPoly ^Encode(System::Int64 value);

                /**
                <summary>Encodes a signed integer (represented by System::Int64) into a plaintext polynomial.</summary>
                <remarks>
                <para>
                Encodes a signed integer (represented by System::Int64) into a plaintext polynomial.
                </para>
                <para>
                Negative integers are represented by using -1 instead of 1 in the binary representation, and the negative coefficients
                are stored in the plaintext polynomials as unsigned integers that represent them modulo the plaintext modulus. Thus,
                for example, a coefficient of -1 would be stored as a polynomial coefficient plainModulus-1.
                </para>
                </remarks>
                <param name="value">The signed integer to encode</param>
                <param name="destination">The polynomial to overwrite with the encoding</param>
                <exception cref="System::ArgumentNullException">if destination is null</exception>
                */
                void Encode(System::Int64 value, BigPoly ^destination);
                
                /**
                <summary>Decodes a plaintext polynomial and returns the result as System::UInt64.</summary>
                <remarks>
                Decodes a plaintext polynomial and returns the result as System::UInt64. Mathematically this amounts to evaluating the
                input polynomial at X=2.
                </remarks>
                <param name="poly">The polynomial to be decoded</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                <exception cref="System::ArgumentException">if the output does not fit in System::UInt64 (#ifdef THROW_ON_DECODER_OVERFLOW)</exception>
                */
                System::UInt64 DecodeUInt64(BigPoly ^poly);

                /**
                <summary>Decodes a plaintext polynomial and returns the result as System::UInt32.</summary>
                <remarks>
                Decodes a plaintext polynomial and returns the result as System::UInt32. Mathematically this amounts to evaluating the
                input polynomial at X=2.
                </remarks>
                <param name="poly">The polynomial to be decoded</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                <exception cref="System::ArgumentException">if the output does not fit in System::UInt32 (#ifdef THROW_ON_DECODER_OVERFLOW)</exception>
                */
                System::UInt32 DecodeUInt32(BigPoly ^poly);

                /**
                <summary>Decodes a plaintext polynomial and returns the result as System::Int64.</summary>
                <remarks>
                Decodes a plaintext polynomial and returns the result as System::Int64. Mathematically this amounts to evaluating the
                input polynomial at X=2.
                </remarks>
                <param name="poly">The polynomial to be decoded</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                <exception cref="System::ArgumentException">if poly is not a valid plaintext polynomial</exception>
                <exception cref="System::ArgumentException">if the output does not fit in System::Int64 (#ifdef THROW_ON_DECODER_OVERFLOW)</exception>
                */
                System::Int64 DecodeInt64(BigPoly ^poly);

                /**
                <summary>Decodes a plaintext polynomial and returns the result as System::Int32.</summary>
                <remarks>
                Decodes a plaintext polynomial and returns the result as System::Int32. Mathematically this amounts to evaluating the
                input polynomial at X=2.
                </remarks>
                <param name="poly">The polynomial to be decoded</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                <exception cref="System::ArgumentException">if poly is not a valid plaintext polynomial</exception>
                <exception cref="System::ArgumentException">if the output does not fit in System::Int32 (#ifdef THROW_ON_DECODER_OVERFLOW)</exception>
                */
                System::Int32 DecodeInt32(BigPoly ^poly);

                /**
                <summary>Decodes a plaintext polynomial and returns the result as BigUInt.</summary>
                <remarks>
                Decodes a plaintext polynomial and returns the result as <see cref="BigUInt"/>. Mathematically this amounts to evaluating the input
                polynomial at X=2.
                </remarks>
                <param name="poly">The polynomial to be decoded</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                <exception cref="System::ArgumentException">if poly is not a valid plaintext polynomial</exception>
                <exception cref="System::ArgumentException">if the output is negative (#ifdef THROW_ON_DECODER_OVERFLOW)</exception>
                */
                BigUInt ^DecodeBigUInt(BigPoly ^poly);

                /**
                <summary>Decodes a plaintext polynomial and stores the result in a given BigUInt.</summary>
                <remarks>
                Decodes a plaintext polynomial and stores the result in a given <see cref="BigUInt"/>. Mathematically this amounts to evaluating the
                input polynomial at X=2.
                </remarks>
                <param name="poly">The polynomial to be decoded</param>
                <param name="destination">The BigUInt to overwrite with the decoding</param>
                <exception cref="System::ArgumentNullException">if poly or destination is null</exception>
                <exception cref="System::ArgumentException">if poly is not a valid plaintext polynomial</exception>
                <exception cref="System::ArgumentException">if the output does not fit in destination</exception>
                <exception cref="System::ArgumentException">if the output is negative (#ifdef THROW_ON_DECODER_OVERFLOW)</exception>
                */
                void DecodeBigUInt(BigPoly ^poly, BigUInt ^destination);

                /**
                <summary>Encodes a signed integer (represented by System::Int32) into a plaintext polynomial.</summary>
                <remarks>
                <para>
                Encodes a signed integer (represented by System::Int32) into a plaintext polynomial.
                </para>
                <para>
                Negative integers are represented by using -1 instead of 1 in the binary representation, and the negative coefficients
                are stored in the plaintext polynomials as unsigned integers that represent them modulo the plaintext modulus. Thus,
                for example, a coefficient of -1 would be stored as a polynomial coefficient plainModulus-1.
                </para>
                </remarks>
                <param name="value">The signed integer to encode</param>
                */
                BigPoly ^Encode(System::Int32 value);

                /**
                <summary>Encodes an unsigned integer (represented by System::UInt32) into a plaintext polynomial.</summary>
                <param name="value">The unsigned integer to encode</param>
                */
                BigPoly ^Encode(System::UInt32 value);

                /**
                <summary>Encodes a signed integer (represented by System::Int32) into a plaintext polynomial.</summary>
                <remarks>
                <para>
                Encodes a signed integer (represented by System::Int32) into a plaintext polynomial.
                </para>
                <para>
                Negative integers are represented by using -1 instead of 1 in the binary representation, and the negative coefficients
                are stored in the plaintext polynomials as unsigned integers that represent them modulo the plaintext modulus. Thus,
                for example, a coefficient of -1 would be stored as a polynomial coefficient plainModulus-1.
                </para>
                </remarks>
                <param name="value">The signed integer to encode</param>
                <param name="destination">The polynomial to overwrite with the encoding</param>
                <exception cref="System::ArgumentNullException">if destination is null</exception>
                */
                void Encode(System::Int32 value, BigPoly ^destination);

                /**
                <summary>Encodes an unsigned integer (represented by System::UInt32) into a plaintext polynomial.</summary>
                <param name="value">The unsigned integer to encode</param>
                <param name="destination">The polynomial to overwrite with the encoding</param>
                <exception cref="System::ArgumentNullException">if destination is null</exception>
                */
                void Encode(System::UInt32 value, BigPoly ^destination);

            internal:
                /**
                <summary>Returns a reference to the underlying C++ BinaryEncoder.</summary>
                */
                seal::BinaryEncoder &GetEncoder();

            private:
                seal::BinaryEncoder *binaryEncoder_;
            };

            /**
            <summary>Encodes integers into plaintext polynomials that Encryptor can encrypt.</summary>
            <remarks>
            <para>
            Encodes integers into plaintext polynomials that Encryptor can encrypt. An instance of
            the BalancedEncoder class converts an integer into a plaintext polynomial by placing its
            digits in balanced base-b representation as the coefficients of the polynomial. The base
            b must be a positive integer at least 3 (which is the default value). When b is odd,
            digits in such a balanced representation are integers in the range -(b-1)/2,...,(b-1)/2. 
            When b is even, digits are integers in the range -b/2,...,b/2-1. Note that the default 
            value 3 for the base b allows for more compact representation than BinaryEncoder without 
            increasing the sizes of the coefficients of freshly encoded plaintext polynomials. A larger 
            base allows for an even more compact representation at the cost of having larger coefficients 
            in freshly encoded plaintext polynomials. Decoding the integer amounts to evaluating the plaintext 
            polynomial at X=b.
            </para>
            <para>
            Addition and multiplication on the integer side translate into addition and multiplication on the encoded plaintext
            polynomial side, provided that the length of the polynomial never grows to be of the size of the polynomial modulus
            (polyModulus), and that the coefficients of the plaintext polynomials appearing throughout the computations never
            experience coefficients larger than the plaintext modulus (plainModulus).
            </para>
            <para>
            Negative integers in the balanced base-b encoding are represented the same way as positive integers, namely, both
            positive and negative integers can have both positive and negative digits in their balanced base-b representation.
            Negative coefficients are stored in the plaintext polynomials as unsigned integers that represent them modulo the
            plaintext modulus. Thus, for example, a coefficient of -1 would be stored as a polynomial coefficient plainModulus-1.
            </para>
            <para>
            In many cases it is a bad idea to throw an exception when decoding fails due to an overflow
            condition, e.g. <see cref="DecodeUInt32"> is given a polynomial that decodes into a value larger
            than 32 bits. If such an exception is not handled silently, a malicious observer might notice
            this happening, and be able to extract one bit of information about the secret key.
            Thus, by default, the decoder functions do not throw exceptions in cases of overflow.
            However, the throws are automatically enabled if SEAL is compiled in debug mode in Visual Studio,
            and more generally they can be directly controlled by the preprocessor define THROW_ON_DECODER_OVERFLOW
            in the file SEAL/util/defines.h.
            </para>
            </remarks>
            <seealso cref="BalancedFractionalEncoder">See BalancedFractionalEncoder for encoding real numbers.</seealso>
            <seealso cref="BinaryEncoder">See BinaryEncoder for encoding using the binary representation.</seealso>
            <see also cref="IntegerEncoder">See IntegerEncoder for a common interface to all integer encoders.</seealso>
            */
            public ref class BalancedEncoder
            {
            public:
                /**
                <summary>Creates a BalancedEncoder object.</summary>
                <remarks>
                Creates a BalancedEncoder object with base set to 3. The constructor takes as input a reference to the 
                plaintext modulus (represented by <see cref="BigUInt"/>).
                </remarks>
                <param name="plainModulus">The plaintext modulus (represented by BigUInt)</param>
                <exception cref="System::ArgumentNullException">if plainModulus is null</exception>
                <exception cref="System::ArgumentException">if plainModulus is not at least 2</exception>
                */
                BalancedEncoder(BigUInt ^plainModulus);

                /**
                <summary>Creates a BalancedEncoder object.</summary>
                <remarks>
                Creates a BalancedEncoder object with base set to 3. The constructor takes as input a reference to the 
                plaintext modulus (represented by <see cref="BigUInt"/>). The user can give a <see cref="MemoryPoolHandle "/>
                object to use a custom memory pool instead of the global memory pool (default).
                </remarks>
                <param name="plainModulus">The plaintext modulus (represented by BigUInt)</param>
                <param name="pool">The memory pool handle</param>
                <exception cref="System::ArgumentNullException">if plainModulus or pool is null</exception>
                <exception cref="System::ArgumentException">if plainModulus is not at least 2</exception>
                <seealso cref="MemoryPoolHandle">See MemoryPoolHandle for more details on memory pool handles.</seealso>
                */
                BalancedEncoder(BigUInt ^plainModulus, MemoryPoolHandle ^pool);

                /**
                <summary>Creates a BalancedEncoder object.</summary>
                <remarks>
                Creates a BalancedEncoder object. The constructor takes as input a reference to the plaintext modulus 
                (represented by <see cref="BigUInt"/>), and an integer, at least 3, that is used as a base 
                in the encoding.
                </remarks>
                <param name="plainModulus">The plaintext modulus (represented by BigUInt)</param>
                <param name="base">The base to be used for encoding</param>
                <exception cref="System::ArgumentNullException">if plainModulus is null</exception>
                <exception cref="System::ArgumentException">if base is not an integer and at least 3</exception>
                <exception cref="System::ArgumentException">if plainModulus is not at least base</exception>
                */
                BalancedEncoder(BigUInt ^plainModulus, System::UInt64 base);

                /**
                <summary>Creates a BalancedEncoder object.</summary>
                <remarks>
                Creates a BalancedEncoder object. The constructor takes as input a reference to the plaintext modulus
                (represented by <see cref="BigUInt"/>), and an integer, at least 3, that is used as a base
                in the encoding. The user can give a <see cref="MemoryPoolHandle "/> object to use a custom
                memory pool instead of the global memory pool (default).
                </remarks>
                <param name="plainModulus">The plaintext modulus (represented by BigUInt)</param>
                <param name="base">The base to be used for encoding</param>
                <exception cref="System::ArgumentNullException">if plainModulus or pool is null</exception>
                <exception cref="System::ArgumentException">if base is not an integer and at least 3</exception>
                <exception cref="System::ArgumentException">if plainModulus is not at least base</exception>
                <seealso cref="MemoryPoolHandle">See MemoryPoolHandle for more details on memory pool handles.</seealso>
                */
                BalancedEncoder(BigUInt ^plainModulus, System::UInt64 base, MemoryPoolHandle ^pool);

                /**
                <summary>Creates a copy of a BalancedEncoder.</summary>

                <param name="copy">The BalancedEncoder to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                BalancedEncoder(BalancedEncoder ^copy);

                /**
                <summary>Destroys the BalancedEncoder.</summary>
                */
                ~BalancedEncoder();

                /**
                <summary>Destroys the BalancedEncoder.</summary>
                */
                !BalancedEncoder();

                /**
                <summary>Returns a reference to the plaintext modulus.</summary>
                */
                property BigUInt ^PlainModulus
                {
                    BigUInt ^get();
                }

                /**
                <summary>Returns the base used for encoding.</summary>
                */
                property System::UInt64 Base
                {
                    System::UInt64 get();
                }

                /**
                <summary>Encodes an unsigned integer (represented by System::UInt64) into a plaintext polynomial.</summary>
                <param name="value">The unsigned integer to encode</param>
                */
                BigPoly ^Encode(System::UInt64 value);

                /**
                <summary>Encodes an unsigned integer (represented by System::UInt64) into a plaintext polynomial.</summary>
                <param name="value">The unsigned integer to encode</param>
                <param name="destination">The polynomial to overwrite with the encoding</param>
                <exception cref="System::ArgumentNullException">if destination is null</exception>
                */
                void Encode(System::UInt64 value, BigPoly ^destination);

                /**
                <summary>Encodes an unsigned integer (represented by <see cref="BigUInt"/>) into a plaintext polynomial.</summary>
                <param name="value">The unsigned integer to encode</param>
                <exception cref="System::ArgumentNullException">if value is null</exception>
                */
                BigPoly ^Encode(BigUInt ^value);

                /**
                <summary>Encodes an unsigned integer (represented by <see cref="BigUInt"/>) into a plaintext polynomial.</summary>
                <param name="value">The unsigned integer to encode</param>
                <param name="destination">The polynomial to overwrite with the encoding</param>
                <exception cref="System::ArgumentNullException">if value or destination is null</exception>
                */
                void Encode(BigUInt ^value, BigPoly ^destination);

                /**
                <summary>Encodes a signed integer (represented by System::Int64) into a plaintext polynomial.</summary>
                <remarks>
                <para>
                Encodes a signed integer (represented by System::Int64) into a plaintext polynomial.
                </para>
                <para>
                Negative integers in the balanced base-b encoding are represented the same way as positive integers, namely, both
                positive and negative integers can have both positive and negative digits in their balanced base-b representation.
                Negative coefficients are stored in the plaintext polynomials as unsigned integers that represent them modulo the
                plaintext modulus. Thus, for example, a coefficient of -1 would be stored as a polynomial coefficient plainModulus-1.
                </para>
                </remarks>
                <param name="value">The signed integer to encode</param>
                */
                BigPoly ^Encode(System::Int64 value);

                /**
                <summary>Encodes a signed integer (represented by System::Int64) into a plaintext polynomial.</summary>
                <remarks>
                <para>
                Encodes a signed integer (represented by System::Int64) into a plaintext polynomial.
                </para>
                <para>
                Negative integers in the balanced base-b encoding are represented the same way as positive integers, namely, both
                positive and negative integers can have both positive and negative digits in their balanced base-b representation.
                Negative coefficients are stored in the plaintext polynomials as unsigned integers that represent them modulo the
                plaintext modulus. Thus, for example, a coefficient of -1 would be stored as a polynomial coefficient plainModulus-1.
                </para>
                </remarks>
                <param name="value">The signed integer to encode</param>
                <param name="destination">The polynomial to overwrite with the encoding</param>
                <exception cref="System::ArgumentNullException">if destination is null</exception>
                */
                void Encode(System::Int64 value, BigPoly ^destination);

                /**
                <summary>Decodes a plaintext polynomial and returns the result as System::UInt64.</summary>
                <remarks>
                Decodes a plaintext polynomial and returns the result as System::UInt64. Mathematically this amounts to evaluating the
                input polynomial at X=base.
                </remarks>
                <param name="poly">The polynomial to be decoded</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                <exception cref="System::ArgumentException">if poly is not a valid plaintext polynomial</exception>
                <exception cref="System::ArgumentException">if the output does not fit in System::UInt64 (#ifdef THROW_ON_DECODER_OVERFLOW)</exception>
                */
                System::UInt64 DecodeUInt64(BigPoly ^poly);

                /**
                <summary>Decodes a plaintext polynomial and returns the result as System::UInt32.</summary>
                <remarks>
                Decodes a plaintext polynomial and returns the result as System::UInt32. Mathematically this amounts to evaluating the
                input polynomial at X=base.
                </remarks>
                <param name="poly">The polynomial to be decoded</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                <exception cref="System::ArgumentException">if poly is not a valid plaintext polynomial</exception>
                <exception cref="System::ArgumentException">if the output does not fit in System::UInt32 (#ifdef THROW_ON_DECODER_OVERFLOW)</exception>
                */
                System::UInt32 DecodeUInt32(BigPoly ^poly);

                /**
                <summary>Decodes a plaintext polynomial and returns the result as System::Int64.</summary>
                <remarks>
                Decodes a plaintext polynomial and returns the result as System::Int64. Mathematically this amounts to evaluating the
                input polynomial at X=base.
                </remarks>
                <param name="poly">The polynomial to be decoded</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                <exception cref="System::ArgumentException">if poly is not a valid plaintext polynomial</exception>
                <exception cref="System::ArgumentException">if the output does not fit in System::Int64 (#ifdef THROW_ON_DECODER_OVERFLOW)</exception>
                */
                System::Int64 DecodeInt64(BigPoly ^poly);

                /**
                <summary>Decodes a plaintext polynomial and returns the result as System::Int32.</summary>
                <remarks>
                Decodes a plaintext polynomial and returns the result as System::Int32. Mathematically this amounts to evaluating the
                input polynomial at X=base.
                </remarks>
                <param name="poly">The polynomial to be decoded</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                <exception cref="System::ArgumentException">if poly is not a valid plaintext polynomial</exception>
                <exception cref="System::ArgumentException">if the output does not fit in System::Int32 (#ifdef THROW_ON_DECODER_OVERFLOW)</exception>
                */
                System::Int32 DecodeInt32(BigPoly ^poly);

                /**
                <summary>Decodes a plaintext polynomial and returns the result as BigUInt.</summary>
                <remarks>
                Decodes a plaintext polynomial and returns the result as <see cref="BigUInt"/>. Mathematically this amounts to evaluating the input
                polynomial at X=base.
                </remarks>
                <param name="poly">The polynomial to be decoded</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                <exception cref="System::ArgumentException">if poly is not a valid plaintext polynomial</exception>
                <exception cref="System::ArgumentException">if the output is negative (#ifdef THROW_ON_DECODER_OVERFLOW)</exception>
                */
                BigUInt ^DecodeBigUInt(BigPoly ^poly);

                /**
                <summary>Decodes a plaintext polynomial and stores the result in a given BigUInt.</summary>
                <remarks>
                Decodes a plaintext polynomial and stores the result in a given <see cref="BigUInt"/>. Mathematically this amounts to evaluating the
                input polynomial at X=base.
                </remarks>
                <param name="poly">The polynomial to be decoded</param>
                <param name="destination">The BigUInt to overwrite with the decoding</param>
                <exception cref="System::ArgumentNullException">if poly or destination is null</exception>
                <exception cref="System::ArgumentException">if poly is not a valid plaintext polynomial</exception>
                <exception cref="System::ArgumentException">if the output does not fit in destination</exception>
                <exception cref="System::ArgumentException">if the output is negative (#ifdef THROW_ON_DECODER_OVERFLOW)</exception>
                */
                void DecodeBigUInt(BigPoly ^poly, BigUInt ^destination);

                /**
                <summary>Encodes a signed integer (represented by System::Int32) into a plaintext polynomial.</summary>
                <remarks>
                <para>
                Encodes a signed integer (represented by System::Int32) into a plaintext polynomial.
                </para>
                <para>
                Negative integers in the balanced base-b encoding are represented the same way as positive integers, namely, both
                positive and negative integers can have both positive and negative digits in their balanced base-b representation.
                Negative coefficients are stored in the plaintext polynomials as unsigned integers that represent them modulo the
                plaintext modulus. Thus, for example, a coefficient of -1 would be stored as a polynomial coefficient plainModulus-1.
                </para>
                </remarks>
                <param name="value">The signed integer to encode</param>
                */
                BigPoly ^Encode(System::Int32 value);

                /**
                <summary>Encodes an unsigned integer (represented by System::UInt32) into a plaintext polynomial.</summary>
                <param name="value">The unsigned integer to encode</param>
                */
                BigPoly ^Encode(System::UInt32 value);

                /**
                <summary>Encodes a signed integer (represented by System::Int32) into a plaintext polynomial.</summary>
                <remarks>
                <para>
                Encodes a signed integer (represented by System::Int32) into a plaintext polynomial.
                </para>
                <para>
                Negative integers in the balanced base-b encoding are represented the same way as positive integers, namely, both
                positive and negative integers can have both positive and negative digits in their balanced base-b representation.
                Negative coefficients are stored in the plaintext polynomials as unsigned integers that represent them modulo the
                plaintext modulus. Thus, for example, a coefficient of -1 would be stored as a polynomial coefficient plainModulus-1.
                </para>
                </remarks>
                <param name="value">The signed integer to encode</param>
                <param name="destination">The polynomial to overwrite with the encoding</param>
                <exception cref="System::ArgumentNullException">if destination is null</exception>
                */
                void Encode(System::Int32 value, BigPoly ^destination);

                /**
                <summary>Encodes an unsigned integer (represented by System::UInt32) into a plaintext polynomial.</summary>
                <param name="value">The unsigned integer to encode</param>
                <param name="destination">The polynomial to overwrite with the encoding</param>
                <exception cref="System::ArgumentNullException">if destination is null</exception>
                */
                void Encode(System::UInt32 value, BigPoly ^destination);

            internal:
                /**
                <summary>Returns a reference to the underlying C++ BalancedEncoder.</summary>
                */
                seal::BalancedEncoder &GetEncoder();

            private:
                seal::BalancedEncoder *balancedEncoder_;
            };

            /**
            <summary>Encodes floating point numbers into plaintext polynomials that Encryptor can encrypt.</summary>
            <remarks>
            <para>
            Encodes floating point numbers into plaintext polynomials that Encryptor can encrypt. An instance of the
            BinaryFractionalEncoder class converts a double-precision floating-point number into a plaintext polynomial by
            computing its binary representation, encoding the integral part as in <see cref="BinaryEncoder"/>, and the fractional part as the
            highest degree terms of the plaintext polynomial, with signs inverted. Decoding the polynomial back into a double
            amounts to evaluating the low degree part at X=2, negating the coefficients of the high degree part and evaluating it
            at X=1/2.
            </para>
            <para>
            Addition and multiplication on the double side translate into addition and multiplication on the encoded plaintext
            polynomial side, provided that the integral part never mixes with the fractional part in the plaintext polynomials,
            and that the coefficients of the plaintext polynomials appearing throughout the computations never experience
            coefficients larger than the plaintext modulus (plainModulus).
            </para>
            <para>
            When homomorphic multiplications are performed, the integral part "grows up" to higher degree coefficients of the
            plaintext polynomial space, and the fractional part "grows down" from the top degree coefficients towards the lower
            degree coefficients. For decoding to work, these parts must not interfere with each other. When setting up the
            BinaryFractionalEncoder, onem ust specify how many coefficients of a plaintext polynomial are reserved for the
            integral part and how many for the fractional. The sum of these numbers can be at most equal to the degree of the
            polynomial modulus minus one. If homomorphic multiplications are performed, it is also necessary to leave enough room
            for the fractional part to "grow down".
            </para>
            <para>
            Negative integers are represented by using -1 instead of 1 in the binary representation, and the negative coefficients
            are stored in the plaintext polynomials as unsigned integers that represent them modulo the plaintext modulus. Thus,
            for example, a coefficient of -1 would be stored as a polynomial coefficient plainModulus-1.
            </para>
            <para>
            In many cases it is a bad idea to throw an exception when decoding fails due to an overflow
            condition, e.g. <see cref="Decode"> is given a polynomial whose integer part decodes into a value
            larger than 64 bits. If such an exception is not handled silently, a malicious observer might notice
            this happening, and be able to extract one bit of information about the secret key.
            Thus, by default, the decoder functions do not throw exceptions in cases of overflow.
            However, the throws are automatically enabled if SEAL is compiled in debug mode in Visual Studio,
            and more generally they can be directly controlled by the preprocessor define THROW_ON_DECODER_OVERFLOW
            in the file SEAL/util/defines.h.
            </para>
            </remarks>
            <seealso cref="BinaryEncoder">See BinaryEncoder for encoding integers.</seealso>
            <seealso cref="BalancedFractionalEncoder">See BalancedFractionalEncoder for encoding using base-b representation for b greater than 2.</seealso>
            <see also cref="FractionalEncoder">See FractionalEncoder for a common interface to all fractional encoders.</seealso>
            */
            public ref class BinaryFractionalEncoder
            {
            public:
                /**
                <summary>Creates a new BinaryFractionalEncoder object.</summary>
                <remarks>
                Creates a new BinaryFractionalEncoder object. The constructor takes as input a reference to the plaintext modulus,
                a reference to the polynomial modulus, and the numbers of coefficients that are reserved for
                the integral and fractional parts. The coefficients for the integral part are counted starting from the low-degree end
                of the polynomial, and the coefficients for the fractional part are counted from the high-degree end.
                </remarks>
                <param name="plainModulus">The plaintext modulus (represented by <see cref="BigUInt"/>)</param>
                <param name="polyModulus">The polynomial modulus (represented by <see cref="BigPoly"/>)</param>
                <param name="integerCoeffCount">The number of polynomial coefficients reserved for the integral part</param>
                <param name="fractionCoeffCount">The number of polynomial coefficients reserved for the fractional part</param>
                <exception cref="System::ArgumentNullException">if plainModulus or polyModulus is null</exception>
                <exception cref="System::ArgumentException">if plainModulus is not at least 2</exception>
                <exception cref="System::ArgumentException">if integerCoeffCount is not strictly positive</exception>
                <exception cref="System::ArgumentException">if fractionCoeffCount is not strictly positive</exception>
                <exception cref="System::ArgumentException">if polyModulus is zero</exception>
                <exception cref="System::ArgumentException">if polyModulus is too small for the integral and fractional
                parts</exception>
                */
                BinaryFractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, int fractionCoeffCount);

                /**
                <summary>Creates a new BinaryFractionalEncoder object.</summary>
                <remarks>
                Creates a new BinaryFractionalEncoder object. The constructor takes as input a reference to the plaintext modulus,
                a reference to the polynomial modulus, and the numbers of coefficients that are reserved for
                the integral and fractional parts. The coefficients for the integral part are counted starting from the low-degree 
                end of the polynomial, and the coefficients for the fractional part are counted from the high-degree end. 
                The user can give a <see cref="MemoryPoolHandle "/> object to use a custom memory pool instead of the 
                global memory pool (default).
                </remarks>
                <param name="plainModulus">The plaintext modulus (represented by <see cref="BigUInt"/>)</param>
                <param name="polyModulus">The polynomial modulus (represented by <see cref="BigPoly"/>)</param>
                <param name="integerCoeffCount">The number of polynomial coefficients reserved for the integral part</param>
                <param name="fractionCoeffCount">The number of polynomial coefficients reserved for the fractional part</param>
                <param name="pool">The memory pool handle</param>
                <exception cref="System::ArgumentNullException">if plainModulus, polyModulus, or pool is null</exception>
                <exception cref="System::ArgumentException">if plainModulus is not at least 2</exception>
                <exception cref="System::ArgumentException">if integerCoeffCount is not strictly positive</exception>
                <exception cref="System::ArgumentException">if fractionCoeffCount is not strictly positive</exception>
                <exception cref="System::ArgumentException">if polyModulus is zero</exception>
                <exception cref="System::ArgumentException">if polyModulus is too small for the integral and fractional
                parts</exception>
                <seealso cref="MemoryPoolHandle">See MemoryPoolHandle for more details on memory pool handles.</seealso>
                */
                BinaryFractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, 
                    int fractionCoeffCount, MemoryPoolHandle ^pool);

                /**
                <summary>Creates a copy of a BinaryFractionalEncoder.</summary>

                <param name="copy">The BinaryFractionalEncoder to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                BinaryFractionalEncoder(BinaryFractionalEncoder ^copy);

                /**
                <summary>Destroys the BinaryFractionalEncoder.</summar>
                */
                ~BinaryFractionalEncoder();

                /**
                <summary>Destroys the BinaryFractionalEncoder.</summar>
                */
                !BinaryFractionalEncoder();

                /**
                <summary>Encodes a double precision floating point number into a plaintext polynomial.</summary>
                <param name="value">The double-precision floating-point number to encode</param>
                */
                BigPoly ^Encode(double value);

                /**
                <summary>Decodes a plaintext polynomial and returns the result as a double-precision floating-point number.</summary>
                <param name="poly">The polynomial to be decoded</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                <exception cref="System::ArgumentException">if poly is not a valid plaintext polynomial</exception>
                <exception cref="System::ArgumentException">if the integral part does not fit in System::Int64 (#ifdef THROW_ON_DECODER_OVERFLOW)</exception>
                */
                double Decode(BigPoly ^poly);

                /**
                <summary>Returns a reference to the plaintext modulus.</summary>
                */
                property BigUInt ^PlainModulus
                {
                    BigUInt ^get();
                }

                /**
                <summary>Returns a reference to the polynomial modulus.</summary>
                */
                property BigPoly ^PolyModulus
                {
                    BigPoly ^get();
                }

                /**
                <summary>Returns the number of coefficients reserved for the fractional part.</summary>
                */
                property int FractionCoeffCount
                {
                    int get();
                }

                /**
                <summary>Returns the number of coefficients reserved for the integral part.</summary>
                */
                property int IntegerCoeffCount
                {
                    int get();
                }

                /**
                <summary>Returns the base used for encoding (2).</summary>
                */
                property System::UInt64 Base
                {
                    System::UInt64 get();
                }

            internal:
                /**
                <summary>Returns a reference to the underlying C++ BinaryFractionalEncoder.</summary>
                */
                seal::BinaryFractionalEncoder &GetEncoder();

            private:
                seal::BinaryFractionalEncoder *fractionalEncoder_;
            };

            /**
            <summary>Encodes floating point numbers into plaintext polynomials that Encryptor can encrypt.</summary>
            <remarks>
            <para>
            Encodes floating point numbers into plaintext polynomials that Encryptor can encrypt. An instance of the
            BalancedFractionalEncoder class converts a double-precision floating-point number into a plaintext polynomial by
            computing its balanced base-b representation, encoding the integral part as in <see cref="BalancedEncoder"/>, and the 
            fractional part as the highest degree terms of the plaintext polynomial, with signs inverted. Decoding the polynomial
            back into a double amounts to evaluating the low degree part at X=b, negating the coefficients of the high degree 
            part and evaluating it at X=1/b.
            </para>
            <para>
            Addition and multiplication on the double side translate into addition and multiplication on the encoded plaintext
            polynomial side, provided that the integral part never mixes with the fractional part in the plaintext polynomials,
            and that the coefficients of the plaintext polynomials appearing throughout the computations never experience
            coefficients larger than the plaintext modulus (plainModulus).
            </para>
            <para>
            When homomorphic multiplications are performed, the integral part "grows up" to higher degree coefficients of the
            plaintext polynomial space, and the fractional part "grows down" from the top degree coefficients towards the lower
            degree coefficients. For decoding to work, these parts must not interfere with each other. When setting up the
            BalancedFractionalEncoder, onem ust specify how many coefficients of a plaintext polynomial are reserved for the
            integral part and how many for the fractional. The sum of these numbers can be at most equal to the degree of the
            polynomial modulus minus one. If homomorphic multiplications are performed, it is also necessary to leave enough room
            for the fractional part to "grow down".
            </para>
            <para>
            Negative integers in the balanced base-b encoding are represented the same way as positive integers, namely, both
            positive and negative integers can have both positive and negative digits in their balanced base-b representation.
            Negative coefficients are stored in the plaintext polynomials as unsigned integers that represent them modulo the
            plaintext modulus. Thus, for example, a coefficient of -1 would be stored as a polynomial coefficient plainModulus-1.
            </para>
            <para>
            In many cases it is a bad idea to throw an exception when decoding fails due to an overflow
            condition, e.g. <see cref="Decode"> is given a polynomial whose integer part decodes into a value
            larger than 64 bits. If such an exception is not handled silently, a malicious observer might notice
            this happening, and be able to extract one bit of information about the secret key.
            Thus, by default, the decoder functions do not throw exceptions in cases of overflow.
            However, the throws are automatically enabled if SEAL is compiled in debug mode in Visual Studio,
            and more generally they can be directly controlled by the preprocessor define THROW_ON_DECODER_OVERFLOW
            in the file SEAL/util/defines.h.
            </para>
            </remarks>
            <seealso cref="BalancedEncoder">See BalancedEncoder for encoding integers.</seealso>
            <seealso cref="BinaryFractionalEncoder">See BinaryFractionalEncoder for encoding using the binary
            representation.</seealso>
            <see also cref="FractionalEncoder">See FractionalEncoder for a common interface to all fractional encoders.</seealso>
            */
            public ref class BalancedFractionalEncoder
            {
            public:
                /**
                <summary>Creates a new BalancedFractionalEncoder object.</summary>
                <remarks>
                Creates a new BalancedFractionalEncoder object. The constructor takes as input a reference to the plaintext modulus,
                a reference to the polynomial modulus, and the numbers of coefficients that are reserved for
                the integral and fractional parts. The base used for encoding is taken to be 3.
                The coefficients for the integral part are counted starting from the low-degree end of the polynomial, and the
                coefficients for the fractional part are counted from the high-degree end.
                </remarks>
                <param name="plainModulus">The plaintext modulus (represented by <see cref="BigUInt"/>)</param>
                <param name="polyModulus">The polynomial modulus (represented by <see cref="BigPoly"/>)</param>
                <param name="integerCoeffCount">The number of polynomial coefficients reserved for the integral part</param>
                <param name="fractionCoeffCount">The number of polynomial coefficients reserved for the fractional part</param>
                <exception cref="System::ArgumentNullException">if plainModulus or polyModulus is null</exception>
                <exception cref="System::ArgumentException">if plainModulus is not at least 2</exception>
                <exception cref="System::ArgumentException">if integerCoeffCount is not strictly positive</exception>
                <exception cref="System::ArgumentException">if fractionCoeffCount is not strictly positive</exception>
                <exception cref="System::ArgumentException">if polyModulus is zero</exception>
                <exception cref="System::ArgumentException">if polyModulus is too small for the integral and fractional
                parts</exception>
                */
                BalancedFractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, int fractionCoeffCount);

                /**
                <summary>Creates a new BalancedFractionalEncoder object.</summary>
                <remarks>
                Creates a new BalancedFractionalEncoder object. The constructor takes as input a reference to the plaintext modulus,
                a reference to the polynomial modulus, and the numbers of coefficients that are reserved for
                the integral and fractional parts. The base used for encoding is taken to be 3.
                The coefficients for the integral part are counted starting from the low-degree end of the polynomial, and the
                coefficients for the fractional part are counted from the high-degree end. The user can give a 
                <see cref="MemoryPoolHandle "/> object to use a custom memory pool instead of the global memory pool (default).
                </remarks>
                <param name="plainModulus">The plaintext modulus (represented by <see cref="BigUInt"/>)</param>
                <param name="polyModulus">The polynomial modulus (represented by <see cref="BigPoly"/>)</param>
                <param name="integerCoeffCount">The number of polynomial coefficients reserved for the integral part</param>
                <param name="fractionCoeffCount">The number of polynomial coefficients reserved for the fractional part</param>
                <param name="pool">The memory pool handle</param>
                <exception cref="System::ArgumentNullException">if plainModulus, polyModulus, or pool is null</exception>
                <exception cref="System::ArgumentException">if plainModulus is not at least 2</exception>
                <exception cref="System::ArgumentException">if integerCoeffCount is not strictly positive</exception>
                <exception cref="System::ArgumentException">if fractionCoeffCount is not strictly positive</exception>
                <exception cref="System::ArgumentException">if polyModulus is zero</exception>
                <exception cref="System::ArgumentException">if polyModulus is too small for the integral and fractional
                parts</exception>
                <seealso cref="MemoryPoolHandle">See MemoryPoolHandle for more details on memory pool handles.</seealso>
                */
                BalancedFractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, int fractionCoeffCount, MemoryPoolHandle ^pool);

                /**
                <summary>Creates a new BalancedFractionalEncoder object.</summary>
                <remarks>
                Creates a new BalancedFractionalEncoder object. The constructor takes as input a reference to the plaintext modulus,
                a reference to the polynomial modulus, and the numbers of coefficients that are reserved for
                the integral and fractional parts, and an odd integer, at least 3, that is used as the base in the encoding.
                The coefficients for the integral part are counted starting from the low-degree end of the polynomial, and the
                coefficients for the fractional part are counted from the high-degree end.
                </remarks>
                <param name="plainModulus">The plaintext modulus (represented by <see cref="BigUInt"/>)</param>
                <param name="polyModulus">The polynomial modulus (represented by <see cref="BigPoly"/>)</param>
                <param name="integerCoeffCount">The number of polynomial coefficients reserved for the integral part</param>
                <param name="fractionCoeffCount">The number of polynomial coefficients reserved for the fractional part</param>
                <param name="base">The base to be used for encoding</param>
                <exception cref="System::ArgumentNullException">if plainModulus or polyModulus is null</exception>
                <exception cref="System::ArgumentException">if plainModulus is not at least (base + 1) / 2</exception>
                <exception cref="System::ArgumentException">if integerCoeffCount is not strictly positive</exception>
                <exception cref="System::ArgumentException">if fractionCoeffCount is not strictly positive</exception>
                <exception cref="System::ArgumentException">if polyModulus is zero</exception>
                <exception cref="System::ArgumentException">if polyModulus is too small for the integral and fractional
                parts</exception>
                <exception cref="System::ArgumentException">if base is not an odd integer and at least 3</exception>
                */
                BalancedFractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, int fractionCoeffCount, System::UInt64 base);

                /**
                <summary>Creates a new BalancedFractionalEncoder object.</summary>
                <remarks>
                Creates a new BalancedFractionalEncoder object. The constructor takes as input a reference to the plaintext modulus,
                a reference to the polynomial modulus, and the numbers of coefficients that are reserved for
                the integral and fractional parts, and an odd integer, at least 3, that is used as the base in the encoding.
                The coefficients for the integral part are counted starting from the low-degree end of the polynomial, and the
                coefficients for the fractional part are counted from the high-degree end. The user can give a 
                <see cref="MemoryPoolHandle "/> object to use a custom memory pool instead of the global memory pool (default).
                </remarks>
                <param name="plainModulus">The plaintext modulus (represented by <see cref="BigUInt"/>)</param>
                <param name="polyModulus">The polynomial modulus (represented by <see cref="BigPoly"/>)</param>
                <param name="integerCoeffCount">The number of polynomial coefficients reserved for the integral part</param>
                <param name="fractionCoeffCount">The number of polynomial coefficients reserved for the fractional part</param>
                <param name="base">The base to be used for encoding</param>
                <param name="pool">The memory pool handle</param>
                <exception cref="System::ArgumentNullException">if plainModulus, polyModulus, or pool is null</exception>
                <exception cref="System::ArgumentException">if plainModulus is not at least (base + 1) / 2</exception>
                <exception cref="System::ArgumentException">if integerCoeffCount is not strictly positive</exception>
                <exception cref="System::ArgumentException">if fractionCoeffCount is not strictly positive</exception>
                <exception cref="System::ArgumentException">if polyModulus is zero</exception>
                <exception cref="System::ArgumentException">if polyModulus is too small for the integral and fractional
                parts</exception>
                <exception cref="System::ArgumentException">if base is not an odd integer and at least 3</exception>
                <seealso cref="MemoryPoolHandle">See MemoryPoolHandle for more details on memory pool handles.</seealso>
                */
                BalancedFractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, int fractionCoeffCount, System::UInt64 base, MemoryPoolHandle ^pool);

                /**
                <summary>Creates a copy of a BalancedFractionalEncoder.</summary>

                <param name="copy">The BalancedFractionalEncoder to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                BalancedFractionalEncoder(BalancedFractionalEncoder ^copy);

                /**
                <summary>Destroys the BalancedFractionalEncoder.</summary>
                */
                ~BalancedFractionalEncoder();

                /**
                <summary>Destroys the BalancedFractionalEncoder.</summary>
                */
                !BalancedFractionalEncoder();

                /**
                <summary>Encodes a double precision floating point number into a plaintext polynomial.</summary>
                <param name="value">The double-precision floating-point number to encode</param>
                */
                BigPoly ^Encode(double value);

                /**
                <summary>Decodes a plaintext polynomial and returns the result as a double-precision floating-point number.</summary>
                <param name="poly">The polynomial to be decoded</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                <exception cref="System::ArgumentException">if poly is not a valid plaintext polynomial</exception>
                <exception cref="System::ArgumentException">if either the integral part does not fit in System::Int64 (#ifdef THROW_ON_DECODER_OVERFLOW)</exception>
                */
                double Decode(BigPoly ^poly);

                /**
                <summary>Returns a reference to the plaintext modulus.</summary>
                */
                property BigUInt ^PlainModulus
                {
                    BigUInt ^get();
                }

                /**
                <summary>Returns a reference to the polynomial modulus.</summary>
                */
                property BigPoly ^PolyModulus
                {
                    BigPoly ^get();
                }

                /**
                <summary>Returns the number of coefficients reserved for the fractional part.</summary>
                */
                property int FractionCoeffCount
                {
                    int get();
                }

                /**
                <summary>Returns the number of coefficients reserved for the integral part.</summary>
                */
                property int IntegerCoeffCount
                {
                    int get();
                }

                /**
                <summary>Returns the base used for encoding.</summary>
                */
                property System::UInt64 Base
                {
                    System::UInt64 get();
                }

            internal:
                /**
                <summary>Returns a reference to the underlying C++ BalancedFractionalEncoder.</summary>
                */
                seal::BalancedFractionalEncoder &GetEncoder();

            private:
                seal::BalancedFractionalEncoder *fractionalEncoder_;
            };

            /**
            <summary>Encodes integers into plaintext polynomials that Encryptor can encrypt.</summary>
            <remarks>
            <para>
            Encodes integers into plaintext polynomials that Encryptor can encrypt. An instance of
            the IntegerEncoder class converts an integer into a plaintext polynomial by placing its
            digits in balanced base-b representation as the coefficients of the polynomial. The base
            b must be a positive integer at least 2 (which is the default value). When b is odd,
            digits in such a balanced representation are integers in the range -(b-1)/2,...,(b-1)/2.
            When b is even, digits are integers in the range -b/2,...,b/2-1. When b is 2, the
            coefficients are either all non-negative (0 and 1), or all non-positive (0 and -1). A larger
            base allows for more compact representation at the cost of having larger coefficients in
            freshly encoded plaintext polynomials. Decoding the integer amounts to evaluating the
            plaintext polynomial at X=b.
            </para>
            <para>
            Addition and multiplication on the integer side translate into addition and multiplication on the encoded plaintext
            polynomial side, provided that the length of the polynomial never grows to be of the size of the polynomial modulus
            (polyModulus), and that the coefficients of the plaintext polynomials appearing throughout the computations never
            experience coefficients larger than the plaintext modulus (plainModulus).
            </para>
            <para>
            Negative integers in the base-b encoding are represented the same way as positive integers,
            namely, both positive and negative integers can have both positive and negative digits in their
            base-b representation. Negative coefficients are stored in the plaintext polynomials as unsigned
            integers that represent them modulo the plaintext modulus. Thus, for example, a coefficient of -1
            would be stored as a polynomial coefficient plainModulus-1.
            </para>
            <para>
            In many cases it is a bad idea to throw an exception when decoding fails due to an overflow
            condition, e.g. <see cref="DecodeUInt32"> is given a polynomial that decodes into a value larger
            than 32 bits. If such an exception is not handled silently, a malicious observer might notice
            this happening, and be able to extract one bit of information about the secret key.
            Thus, by default, the decoder functions do not throw exceptions in cases of overflow.
            However, the throws are automatically enabled if SEAL is compiled in debug mode in Visual Studio,
            and more generally they can be directly controlled by the preprocessor define THROW_ON_DECODER_OVERFLOW
            in the file SEAL/util/defines.h.
            </para>
            <para>
            Under the hood IntegerEncoder uses either the <see cref="BinaryEncoder"/> or the <see cref="BalancedEncoder"/>
            classes to do the encoding. The first one is used when the base is 2, and the second one when the
            base is at least 3. Currently the <see cref="BinaryEncoder"/> and <see cref="BalancedEncoder"/> classes can 
            also be used directly, but this might change in future releases.
            </para>
            </remarks>
            <seealso cref="BinaryEncoder">See BinaryEncoder for encoding using the binary representation.</seealso>
            <seealso cref="BalancedEncoder">See BalancedEncoder for encoding using base-b representation for b greater than 2.</seealso>
            <see also cref="FractionalEncoder">See FractionalEncoder for encoding real numbers.</seealso>
            */
            public ref class IntegerEncoder
            {
            public:
                /**
                <summary>Creates an IntegerEncoder object.</summary>
                <remarks>
                Creates an IntegerEncoder object with base set to 2. The constructor takes as input a reference to the 
                plaintext modulus (represented by <see cref="BigUInt"/>).
                </remarks>
                <param name="plainModulus">The plaintext modulus (represented by BigUInt)</param>
                <exception cref="System::ArgumentNullException">if plainModulus is null</exception>
                <exception cref="System::ArgumentException">if plainModulus is not at least 2</exception>
                */
                IntegerEncoder(BigUInt ^plainModulus);

                /**
                <summary>Creates an IntegerEncoder object.</summary>
                <remarks>
                Creates an IntegerEncoder object with base set to 2. The constructor takes as input a reference to the
                plaintext modulus (represented by <see cref="BigUInt"/>). The user can give a <see cref="MemoryPoolHandle "/> 
                object to use a custom memory pool instead of the global memory pool (default).
                </remarks>
                <param name="plainModulus">The plaintext modulus (represented by BigUInt)</param>
                <param name="pool">The memory pool handle</param>
                <exception cref="System::ArgumentNullException">if plainModulus or pool is null</exception>
                <exception cref="System::ArgumentException">if plainModulus is not at least 2</exception>
                <seealso cref="MemoryPoolHandle">See MemoryPoolHandle for more details on memory pool handles.</seealso>
                */
                IntegerEncoder(BigUInt ^plainModulus, MemoryPoolHandle ^pool);

                /**
                <summary>Creates an IntegerEncoder object.</summary>
                <remarks>
                Creates an IntegerEncoder object. The constructor takes as input a reference to the plaintext modulus 
                (represented by <see cref="BigUInt"/>), and an integer, at least 2, that is used as a base in the encoding.
                </remarks>
                <param name="plainModulus">The plaintext modulus (represented by BigUInt)</param>
                <param name="base">The base to be used for encoding (default value is 2)</param>
                <exception cref="System::ArgumentNullException">if plainModulus is null</exception>
                <exception cref="System::ArgumentException">if base is not an integer and at least 2</exception>
                <exception cref="System::ArgumentException">if plainModulus is not at least base</exception>
                */
                IntegerEncoder(BigUInt ^plainModulus, System::UInt64 base);

                /**
                <summary>Creates an IntegerEncoder object.</summary>
                <remarks>
                Creates an IntegerEncoder object. The constructor takes as input a reference to the plaintext modulus 
                (represented by <see cref="BigUInt"/>), and an integer, at least 2, that is used as a base 
                in the encoding. The user can give a <see cref="MemoryPoolHandle "/> object to use a custom memory 
                pool instead of the global memory pool (default).
                </remarks>
                <param name="plainModulus">The plaintext modulus (represented by BigUInt)</param>
                <param name="base">The base to be used for encoding (default value is 2)</param>
                <param name="pool">The memory pool handle</param>
                <exception cref="System::ArgumentNullException">if plainModulus or pool is null</exception>
                <exception cref="System::ArgumentException">if base is not an integer and at least 2</exception>
                <exception cref="System::ArgumentException">if plainModulus is not at least base</exception>
                <seealso cref="MemoryPoolHandle">See MemoryPoolHandle for more details on memory pool handles.</seealso>
                */
                IntegerEncoder(BigUInt ^plainModulus, System::UInt64 base, MemoryPoolHandle ^pool);

                /**
                <summary>Creates a copy of a IntegerEncoder.</summary>

                <param name="copy">The IntegerEncoder to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                IntegerEncoder(IntegerEncoder ^copy);

                /**
                <summary>Destroys the IntegerEncoder.</summary>
                */
                ~IntegerEncoder();

                /**
                <summary>Destroys the IntegerEncoder.</summary>
                */
                !IntegerEncoder();

                /**
                <summary>Returns a reference to the plaintext modulus.</summary>
                */
                property BigUInt ^PlainModulus
                {
                    BigUInt ^get();
                }

                /**
                <summary>Returns the base used for encoding.</summary>
                */
                property System::UInt64 Base
                {
                    System::UInt64 get();
                }

                /**
                <summary>Encodes an unsigned integer (represented by System::UInt64) into a plaintext polynomial.</summary>
                <param name="value">The unsigned integer to encode</param>
                */
                BigPoly ^Encode(System::UInt64 value);

                /**
                <summary>Encodes an unsigned integer (represented by System::UInt64) into a plaintext polynomial.</summary>
                <param name="value">The unsigned integer to encode</param>
                <param name="destination">The polynomial to overwrite with the encoding</param>
                <exception cref="System::ArgumentNullException">if destination is null</exception>
                */
                void Encode(System::UInt64 value, BigPoly ^destination);

                /**
                <summary>Encodes an unsigned integer (represented by <see cref="BigUInt"/>) into a plaintext polynomial.</summary>
                <param name="value">The unsigned integer to encode</param>
                <exception cref="System::ArgumentNullException">if value is null</exception>
                */
                BigPoly ^Encode(BigUInt ^value);

                /**
                <summary>Encodes an unsigned integer (represented by <see cref="BigUInt"/>) into a plaintext polynomial.</summary>
                <param name="value">The unsigned integer to encode</param>
                <param name="destination">The polynomial to overwrite with the encoding</param>
                <exception cref="System::ArgumentNullException">if value or destination is null</exception>
                */
                void Encode(BigUInt ^value, BigPoly ^destination);

                /**
                <summary>Encodes a signed integer (represented by System::Int64) into a plaintext polynomial.</summary>
                <remarks>
                <para>
                Encodes a signed integer (represented by System::Int64) into a plaintext polynomial.
                </para>
                <para>
                Negative integers in the base-b encoding are represented the same way as positive integers,
                namely, both positive and negative integers can have both positive and negative digits in their
                base-b representation. Negative coefficients are stored in the plaintext polynomials as unsigned
                integers that represent them modulo the plaintext modulus. Thus, for example, a coefficient of -1
                would be stored as a polynomial coefficient plainModulus-1.
                </para>
                </remarks>
                <param name="value">The signed integer to encode</param>
                */
                BigPoly ^Encode(System::Int64 value);

                /**
                <summary>Encodes a signed integer (represented by System::Int64) into a plaintext polynomial.</summary>
                <remarks>
                <para>
                Encodes a signed integer (represented by System::Int64) into a plaintext polynomial.
                </para>
                <para>
                Negative integers in the base-b encoding are represented the same way as positive integers,
                namely, both positive and negative integers can have both positive and negative digits in their
                base-b representation. Negative coefficients are stored in the plaintext polynomials as unsigned
                integers that represent them modulo the plaintext modulus. Thus, for example, a coefficient of -1
                would be stored as a polynomial coefficient plainModulus-1.
                </para>
                </remarks>
                <param name="value">The signed integer to encode</param>
                <param name="destination">The polynomial to overwrite with the encoding</param>
                <exception cref="System::ArgumentNullException">if destination is null</exception>
                */
                void Encode(System::Int64 value, BigPoly ^destination);

                /**
                <summary>Decodes a plaintext polynomial and returns the result as System::UInt64.</summary>
                <remarks>
                Decodes a plaintext polynomial and returns the result as System::UInt64. Mathematically this amounts to evaluating the
                input polynomial at X=base.
                </remarks>
                <param name="poly">The polynomial to be decoded</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                <exception cref="System::ArgumentException">if poly is not a valid plaintext polynomial</exception>
                <exception cref="System::ArgumentException">if the output does not fit in System::UInt64</exception>
                */
                System::UInt64 DecodeUInt64(BigPoly ^poly);

                /**
                <summary>Decodes a plaintext polynomial and returns the result as System::UInt32.</summary>
                <remarks>
                Decodes a plaintext polynomial and returns the result as System::UInt32. Mathematically this amounts to evaluating the
                input polynomial at X=base.
                </remarks>
                <param name="poly">The polynomial to be decoded</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                <exception cref="System::ArgumentException">if poly is not a valid plaintext polynomial</exception>
                <exception cref="System::ArgumentException">if the output does not fit in System::UInt32</exception>
                */
                System::UInt32 DecodeUInt32(BigPoly ^poly);

                /**
                <summary>Decodes a plaintext polynomial and returns the result as System::Int64.</summary>
                <remarks>
                Decodes a plaintext polynomial and returns the result as System::Int64. Mathematically this amounts to evaluating the
                input polynomial at X=base.
                </remarks>
                <param name="poly">The polynomial to be decoded</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                <exception cref="System::ArgumentException">if poly is not a valid plaintext polynomial</exception>
                <exception cref="System::ArgumentException">if the output does not fit in System::Int32</exception>
                */
                System::Int64 DecodeInt64(BigPoly ^poly);

                /**
                <summary>Decodes a plaintext polynomial and returns the result as System::Int32.</summary>
                <remarks>
                Decodes a plaintext polynomial and returns the result as System::Int32. Mathematically this amounts to evaluating the
                input polynomial at X=base.
                </remarks>
                <param name="poly">The polynomial to be decoded</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                <exception cref="System::ArgumentException">if poly is not a valid plaintext polynomial</exception>
                <exception cref="System::ArgumentException">if the output does not fit in System::Int32</exception>
                */
                System::Int32 DecodeInt32(BigPoly ^poly);

                /**
                <summary>Decodes a plaintext polynomial and returns the result as BigUInt.</summary>
                <remarks>
                Decodes a plaintext polynomial and returns the result as <see cref="BigUInt"/>. Mathematically this amounts to evaluating the input
                polynomial at X=base.
                </remarks>
                <param name="poly">The polynomial to be decoded</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                <exception cref="System::ArgumentException">if poly is not a valid plaintext polynomial</exception>
                <exception cref="System::ArgumentException">if the output is negative</exception>
                */
                BigUInt ^DecodeBigUInt(BigPoly ^poly);

                /**
                <summary>Decodes a plaintext polynomial and stores the result in a given BigUInt.</summary>
                <remarks>
                Decodes a plaintext polynomial and stores the result in a given <see cref="BigUInt"/>. Mathematically this amounts to evaluating the
                input polynomial at X=base.
                </remarks>
                <param name="poly">The polynomial to be decoded</param>
                <param name="destination">The BigUInt to overwrite with the decoding</param>
                <exception cref="System::ArgumentNullException">if poly or destination is null</exception>
                <exception cref="System::ArgumentException">if poly is not a valid plaintext polynomial</exception>
                <exception cref="System::ArgumentException">if the output does not fit in destination</exception>
                <exception cref="System::ArgumentException">if the output is negative</exception>
                */
                void DecodeBigUInt(BigPoly ^poly, BigUInt ^destination);

                /**
                <summary>Encodes a signed integer (represented by System::Int32) into a plaintext polynomial.</summary>
                <remarks>
                <para>
                Encodes a signed integer (represented by System::Int32) into a plaintext polynomial.
                </para>
                <para>
                Negative integers in the base-b encoding are represented the same way as positive integers,
                namely, both positive and negative integers can have both positive and negative digits in their
                base-b representation. Negative coefficients are stored in the plaintext polynomials as unsigned
                integers that represent them modulo the plaintext modulus. Thus, for example, a coefficient of -1
                would be stored as a polynomial coefficient plainModulus-1.
                </para>
                </remarks>
                <param name="value">The signed integer to encode</param>
                */
                BigPoly ^Encode(System::Int32 value);

                /**
                <summary>Encodes an unsigned integer (represented by System::UInt32) into a plaintext polynomial.</summary>
                <param name="value">The unsigned integer to encode</param>
                */
                BigPoly ^Encode(System::UInt32 value);

                /**
                <summary>Encodes a signed integer (represented by System::Int32) into a plaintext polynomial.</summary>
                <remarks>
                <para>
                Encodes a signed integer (represented by System::Int32) into a plaintext polynomial.
                </para>
                <para>
                Negative integers in the base-b encoding are represented the same way as positive integers,
                namely, both positive and negative integers can have both positive and negative digits in their
                base-b representation. Negative coefficients are stored in the plaintext polynomials as unsigned
                integers that represent them modulo the plaintext modulus. Thus, for example, a coefficient of -1
                would be stored as a polynomial coefficient plainModulus-1.
                </para>
                </remarks>
                <param name="value">The signed integer to encode</param>
                <param name="destination">The polynomial to overwrite with the encoding</param>
                <exception cref="System::ArgumentNullException">if destination is null</exception>
                */
                void Encode(System::Int32 value, BigPoly ^destination);

                /**
                <summary>Encodes an unsigned integer (represented by System::UInt32) into a plaintext polynomial.</summary>
                <param name="value">The unsigned integer to encode</param>
                <param name="destination">The polynomial to overwrite with the encoding</param>
                <exception cref="System::ArgumentNullException">if destination is null</exception>
                */
                void Encode(System::UInt32 value, BigPoly ^destination);

            internal:
                /**
                <summary>Returns a reference to the underlying C++ IntegerEncoder.</summary>
                */
                seal::IntegerEncoder &GetEncoder();

            private:
                seal::IntegerEncoder *integerEncoder_;
            };

            /**
            <summary>Encodes floating point numbers into plaintext polynomials that Encryptor can encrypt.</summary>
            <remarks>
            <para>
            Encodes floating point numbers into plaintext polynomials that Encryptor can encrypt.
            An instance of the FractionalEncoder class converts a double-precision floating-point
            number into a plaintext polynomial by computing its balanced base-b representation,
            encoding the integral part as in IntegerEncoder, and the fractional part as the highest
            degree terms of the plaintext polynomial, with signs inverted. For an even base b, the
            coefficients of the polynomial are in the range -b/2,...,b/2-1. When b is 2, the
            coefficients are either all non-negative (0 and 1), or all non-positive (0 and -1).
            Decoding the polynomial back into a double amounts to evaluating the low degree part
            at X=b, negating the coefficients of the high degree part and evaluating it at X=1/b.
            </para>
            <para>
            Addition and multiplication on the double side translate into addition and multiplication on the encoded plaintext
            polynomial side, provided that the integral part never mixes with the fractional part in the plaintext polynomials,
            and that the coefficients of the plaintext polynomials appearing throughout the computations never experience
            coefficients larger than the plaintext modulus (plainModulus).
            </para>
            <para>
            When homomorphic multiplications are performed, the integral part "grows up" to higher degree coefficients of the
            plaintext polynomial space, and the fractional part "grows down" from the top degree coefficients towards the lower
            degree coefficients. For decoding to work, these parts must not interfere with each other. When setting up the
            BalancedFractionalEncoder, onem ust specify how many coefficients of a plaintext polynomial are reserved for the
            integral part and how many for the fractional. The sum of these numbers can be at most equal to the degree of the
            polynomial modulus minus one. If homomorphic multiplications are performed, it is also necessary to leave enough room
            for the fractional part to "grow down".
            </para>
            <para>
            Negative integers in the base-b encoding are represented the same way as positive integers,
            namely, both positive and negative integers can have both positive and negative digits in their
            base-b representation. Negative coefficients are stored in the plaintext polynomials as unsigned
            integers that represent them modulo the plaintext modulus. Thus, for example, a coefficient of -1
            would be stored as a polynomial coefficient plain_modulus-1.
            </para>
            <para>
            In many cases it is a bad idea to throw an exception when decoding fails due to an overflow
            condition, e.g. <see cref="Decode"> is given a polynomial whose integer part decodes into a value
            larger than 64 bits. If such an exception is not handled silently, a malicious observer might notice
            this happening, and be able to extract one bit of information about the secret key.
            Thus, by default, the decoder functions do not throw exceptions in cases of overflow.
            However, the throws are automatically enabled if SEAL is compiled in debug mode in Visual Studio,
            and more generally they can be directly controlled by the preprocessor define THROW_ON_DECODER_OVERFLOW
            in the file SEAL/util/defines.h.
            </para>
            <para>
            Under the hood FractionalEncoder uses either the <see cref="BinaryFractionalEncoder"/> or the
            <see cref="BalancedFractionalEncoder"/> classes to do the encoding. The first one is used when 
            the base is 2, and the second one when the base is at least 3. Currently the <see cref="BinaryFractionalEncoder"/>
            and <see cref="BalancedFractionalEncoder"/> classes can also be used directly, but this might change 
            in future releases.
            </para>
            </remarks>
            <seealso cref="BinaryFractionalEncoder">See BinaryFractionalEncoder for encoding using the binary representation.</seealso>
            <seealso cref="BalancedFractionalEncoder">See BalancedFractionalEncoder for encoding using base-b representation for b greater than 2.</seealso>
            <see also cref="IntegerEncoder">See IntegerEncoder for a common interface to all fractional encoders.</seealso>
            */
            public ref class FractionalEncoder
            {
            public:
                /**
                <summary>Creates a new FractionalEncoder object.</summary>
                <remarks>
                Creates a new FractionalEncoder object. The constructor takes as input a reference
                to the plaintext modulus, a reference to the polynomial modulus,
                and the numbers of coefficients that are reserved for the integral and fractional parts.
                The base used for encoding is taken to be 2. The coefficients for the integral part are 
                counted starting from the low-degree end of the polynomial, and the coefficients for the 
                fractional part are counted from the high-degree end.
                </remarks>
                <param name="plainModulus">The plaintext modulus (represented by <see cref="BigUInt"/>)</param>
                <param name="polyModulus">The polynomial modulus (represented by <see cref="BigPoly"/>)</param>
                <param name="integerCoeffCount">The number of polynomial coefficients reserved for the integral part</param>
                <param name="fractionCoeffCount">The number of polynomial coefficients reserved for the fractional part</param>
                <exception cref="System::ArgumentNullException">if plainModulus or polyModulus is null</exception>
                <exception cref="System::ArgumentException">if plainModulus is not at least 2</exception>
                <exception cref="System::ArgumentException">if integerCoeffCount is not strictly positive</exception>
                <exception cref="System::ArgumentException">if fractionCoeffCount is not strictly positive</exception>
                <exception cref="System::ArgumentException">if polyModulus is zero</exception>
                <exception cref="System::ArgumentException">if polyModulus is too small for the integral and fractional parts</exception>
                */
                FractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, int fractionCoeffCount);

                /**
                <summary>Creates a new FractionalEncoder object.</summary>
                <remarks>
                Creates a new FractionalEncoder object. The constructor takes as input a reference
                to the plaintext modulus, a reference to the polynomial modulus,
                and the numbers of coefficients that are reserved for the integral and fractional parts.
                The base used for encoding is taken to be 2. The coefficients for the integral part are
                counted starting from the low-degree end of the polynomial, and the coefficients for the
                fractional part are counted from the high-degree end. The user can give a <see cref="MemoryPoolHandle "/>
                object to use a custom memory pool instead of the global memory pool (default).
                </remarks>
                <param name="plainModulus">The plaintext modulus (represented by <see cref="BigUInt"/>)</param>
                <param name="polyModulus">The polynomial modulus (represented by <see cref="BigPoly"/>)</param>
                <param name="integerCoeffCount">The number of polynomial coefficients reserved for the integral part</param>
                <param name="fractionCoeffCount">The number of polynomial coefficients reserved for the fractional part</param>
                <param name="pool">The memory pool handle</param>
                <exception cref="System::ArgumentNullException">if plainModulus, polyModulus, or pool is null</exception>
                <exception cref="System::ArgumentException">if plainModulus is not at least 2</exception>
                <exception cref="System::ArgumentException">if integerCoeffCount is not strictly positive</exception>
                <exception cref="System::ArgumentException">if fractionCoeffCount is not strictly positive</exception>
                <exception cref="System::ArgumentException">if polyModulus is zero</exception>
                <exception cref="System::ArgumentException">if polyModulus is too small for the integral and fractional parts</exception>
                <seealso cref="MemoryPoolHandle">See MemoryPoolHandle for more details on memory pool handles.</seealso>
                */
                FractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, int fractionCoeffCount,
                    MemoryPoolHandle ^pool);

                /**
                <summary>Creates a new FractionalEncoder object.</summary>
                <remarks>
                Creates a new FractionalEncoder object. The constructor takes as input a reference
                to the plaintext modulus, a reference to the polynomial modulus,
                and the numbers of coefficients that are reserved for the integral and fractional parts,
                and an integer, at least 2, that is used as the base in the encoding. The coefficients 
                for the integral part are counted starting from the low-degree end of the polynomial, 
                and the coefficients for the fractional part are counted from the high-degree end.
                </remarks>
                <param name="plainModulus">The plaintext modulus (represented by <see cref="BigUInt"/>)</param>
                <param name="polyModulus">The polynomial modulus (represented by <see cref="BigPoly"/>)</param>
                <param name="integerCoeffCount">The number of polynomial coefficients reserved for the integral part</param>
                <param name="fractionCoeffCount">The number of polynomial coefficients reserved for the fractional part</param>
                <param name="base">The base to be used for encoding</param>
                <exception cref="System::ArgumentNullException">if plainModulus or polyModulus is null</exception>
                <exception cref="System::ArgumentException">if plainModulus is not at least base</exception>
                <exception cref="System::ArgumentException">if integerCoeffCount is not strictly positive</exception>
                <exception cref="System::ArgumentException">if fractionCoeffCount is not strictly positive</exception>
                <exception cref="System::ArgumentException">if polyModulus is zero</exception>
                <exception cref="System::ArgumentException">if polyModulus is too small for the integral and fractional
                parts</exception>
                <exception cref="System::ArgumentException">if base is not an integer and at least 2</exception>
                */
                FractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, int fractionCoeffCount, 
                    System::UInt64 base);

                /**
                <summary>Creates a new FractionalEncoder object.</summary>
                <remarks>
                Creates a new FractionalEncoder object. The constructor takes as input a reference
                to the plaintext modulus, a reference to the polynomial modulus,
                and the numbers of coefficients that are reserved for the integral and fractional parts,
                and an integer, at least 2, that is used as the base in the encoding. The coefficients
                for the integral part are counted starting from the low-degree end of the polynomial,
                and the coefficients for the fractional part are counted from the high-degree end.
                The user can give a <see cref="MemoryPoolHandle "/> object to use a custom memory pool 
                instead of the global memory pool (default).
                </remarks>
                <param name="plainModulus">The plaintext modulus (represented by <see cref="BigUInt"/>)</param>
                <param name="polyModulus">The polynomial modulus (represented by <see cref="BigPoly"/>)</param>
                <param name="integerCoeffCount">The number of polynomial coefficients reserved for the integral part</param>
                <param name="fractionCoeffCount">The number of polynomial coefficients reserved for the fractional part</param>
                <param name="base">The base to be used for encoding</param>
                <param name="pool">The memory pool handle</param>
                <exception cref="System::ArgumentNullException">if plainModulus, polyModulus, or pool is null</exception>
                <exception cref="System::ArgumentException">if plainModulus is not at least base</exception>
                <exception cref="System::ArgumentException">if integerCoeffCount is not strictly positive</exception>
                <exception cref="System::ArgumentException">if fractionCoeffCount is not strictly positive</exception>
                <exception cref="System::ArgumentException">if polyModulus is zero</exception>
                <exception cref="System::ArgumentException">if polyModulus is too small for the integral and fractional
                parts</exception>
                <exception cref="System::ArgumentException">if base is not an integer and at least 2</exception>
                <seealso cref="MemoryPoolHandle">See MemoryPoolHandle for more details on memory pool handles.</seealso>
                */
                FractionalEncoder(BigUInt ^plainModulus, BigPoly ^polyModulus, int integerCoeffCount, int fractionCoeffCount, 
                    System::UInt64 base, MemoryPoolHandle ^pool);

                /**
                <summary>Creates a copy of a FractionalEncoder.</summary>

                <param name="copy">The FractionalEncoder to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                FractionalEncoder(FractionalEncoder ^copy);

                /**
                <summary>Destroys the FractionalEncoder.</summary>
                */
                ~FractionalEncoder();

                /**
                <summary>Destroys the FractionalEncoder.</summary>
                */
                !FractionalEncoder();

                /**
                <summary>Encodes a double precision floating point number into a plaintext polynomial.</summary>
                <param name="value">The double-precision floating-point number to encode</param>
                */
                BigPoly ^Encode(double value);

                /**
                <summary>Decodes a plaintext polynomial and returns the result as a double-precision floating-point number.</summary>
                <param name="poly">The polynomial to be decoded</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                <exception cref="System::ArgumentException">if poly is not a valid plaintext polynomial</exception>
                <exception cref="System::ArgumentException">if either the integral part does not fit in System::Int64</exception>
                */
                double Decode(BigPoly ^poly);

                /**
                <summary>Returns a reference to the plaintext modulus.</summary>
                */
                property BigUInt ^PlainModulus
                {
                    BigUInt ^get();
                }

                /**
                <summary>Returns a reference to the polynomial modulus.</summary>
                */
                property BigPoly ^PolyModulus
                {
                    BigPoly ^get();
                }

                /**
                <summary>Returns the number of coefficients reserved for the fractional part.</summary>
                */
                property int FractionCoeffCount
                {
                    int get();
                }

                /**
                <summary>Returns the number of coefficients reserved for the integral part.</summary>
                */
                property int IntegerCoeffCount
                {
                    int get();
                }

                /**
                <summary>Returns the base used for encoding.</summary>
                */
                property System::UInt64 Base
                {
                    System::UInt64 get();
                }


            internal:
                /**
                <summary>Returns a reference to the underlying C++ FractionalEncoder.</summary>
                */
                seal::FractionalEncoder &GetEncoder();

            private:
                seal::FractionalEncoder *fractionalEncoder_;
            };
        }
    }
}