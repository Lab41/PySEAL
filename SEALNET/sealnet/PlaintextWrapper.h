#pragma once

#include "seal/plaintext.h"
#include "sealnet/BigPolyWrapper.h"
#include "sealnet/MemoryPoolHandleWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class Ciphertext;

            /**
            <summary>Class to store a plaintext element.</summary>

            <remarks>
            <para>
            Class to store a plaintext element. The data for the plaintext is a polynomial with coefficients
            modulo the plaintext modulus. The degree of the plaintext polynomial must be one less than the
            degree of the polynomial modulus. The backing array always allocates one 64-bit word per each
            coefficient of the polynoimal.
            </para>
            <para>
            SEAL allocates the memory for a plaintext by default from the global memory pool, but a user can
            alternatively specify a different memory pool to be used through the MemoryPoolHandle class. This
            can be important, as constructing or resizing several plaintexts at once allocated in the global
            memory pool can quickly lead to contention and poor performance in multi-threaded applications.
            In addition to its coefficient count, a plaintext also has a capacity which denotes the coefficient
            count that fits in the current allocation. Since each coefficient is a 64-bit word, this is exactly
            the number of 64-bit words that are allocated. In high-performance applications unnecessary
            re-allocations should also be avoided by reserving enough memory for the plaintext to begin with
            either by providing the capacity to the constructor as an extra argument, or by calling the reserve
            function at any time.
            </para>
            <para>
            In general, reading from plaintext is thread-safe as long as no other thread is concurrently
            mutating it. This is due to the underlying data structure storing the plaintext not being
            thread-safe.
            </para>
            </remarks>
            <seealso cref="Ciphertext">See Ciphertext for the class that stores ciphertexts.</seealso>
            */
            public ref class Plaintext : System::IEquatable<Plaintext^>
            {
            public:
                /**
                <summary>Constructs an empty plaintext allocating no memory.</summary>
                */
                Plaintext();

                /**
                <summary>Constructs an empty plaintext allocating no memory.</summary>

                <remarks>
                Constructs an empty plaintext allocating no memory. The memory pool is set to the pool pointed
                to by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if pool is null</exception>
                */
                Plaintext(MemoryPoolHandle ^pool);

                /**
                <summary>Constructs a plaintext representing a constant polynomial 0.</summary>

                <remarks>
                Constructs a plaintext representing a constant polynomial 0. The coefficient count of the
                polynomial is set to the given value. The capacity is set to the same value. The memory
                pool is set to the pool pointed to by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="coeffCount">The number of (zeroed) coefficients in the plaintext polynomial</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if coeffCount is negative</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if pool is null</exception>
                */
                Plaintext(int coeffCount, MemoryPoolHandle ^pool);
                
                /**
                <summary>Constructs a plaintext representing a constant polynomial 0.</summary>

                <remarks>
                Constructs a plaintext representing a constant polynomial 0. The coefficient count of the
                polynomial is set to the given value. The capacity is set to the same value. The memory
                pool is set to the global memory pool.
                </remarks>
                <param name="coeffCount">The number of (zeroed) coefficients in the plaintext polynomial</param>
                <exception cref="System::ArgumentException">if coeffCount is negative</exception>
                */
                Plaintext(int coeffCount);

                /**
                <summary>Constructs a plaintext representing a constant polynomial 0.</summary>

                <remarks>
                Constructs a plaintext representing a constant polynomial 0. The coefficient count of 
                the polynomial and the capacity are set to the given values. The memory pool is set to the
                pool pointed to by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="capacity">The capacity</param>
                <param name="coeffCount">The number of (zeroed) coefficients in the plaintext polynomial</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if capacity is less than coeffCount</exception>
                <exception cref="System::ArgumentException">if coeffCount is negative</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if pool is null</exception>
                */
                Plaintext(int capacity, int coeffCount, MemoryPoolHandle ^pool);

                /**
                <summary>Constructs a plaintext representing a constant polynomial 0.</summary>

                <remarks>
                Constructs a plaintext representing a constant polynomial 0. The coefficient count of
                the polynomial and the capacity are set to the given values. The memory pool is set to the
                global memory pool.
                </remarks>
                <param name="capacity">The capacity</param>
                <param name="coeffCount">The number of (zeroed) coefficients in the plaintext polynomial</param>
                <exception cref="System::ArgumentException">if capacity is less than coeffCount</exception>
                <exception cref="System::ArgumentException">if coeffCount is negative</exception>
                */
                Plaintext(int capacity, int coeffCount);

                /**
                <summary>Constructs a plaintext and sets its value to the polynomial represented by the given 
                BigPoly.</summary>
                
                <remarks>
                Constructs a plaintext and sets its value to the polynomial represented by the given 
                <see cref="BigPoly" />. The memory pool is set to the pool pointed to by the given 
                <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="poly">The plaintext polynomial</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if pool is unitialized</exception>
                <exception cref="System::ArgumentNullException">if poly or pool is null</exception>
                */
                Plaintext(BigPoly ^poly, MemoryPoolHandle ^pool);

                /**
                <summary>Constructs a plaintext and sets its value to the polynomial represented by the given
                BigPoly.</summary>

                <remarks>
                Constructs a plaintext and sets its value to the polynomial represented by the given
                <see cref="BigPoly" />. The memory pool is set to the global memory pool.
                </remarks>
                <param name="poly">The plaintext polynomial</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                */
                Plaintext(BigPoly ^poly);

                /**
                <summary>Constructs a plaintext from a given hexadecimal string describing the plaintext polynomial.</summary>

                <remarks>
                <para>
                Constructs a plaintext from a given hexadecimal string describing the plaintext polynomial.
                The memory pool is set to the pool pointed to by the given <see cref="MemoryPoolHandle" />.
                </para>
                <para>
                The string description of the polynomial must adhere to the format returned by <see cref="ToString()"/>,
                which is of the form "7FFx^3 + 1x^1 + 3" and summarized by the following rules:
                <list type="number">
                <item><description>Terms are listed in order of strictly decreasing exponent</description></item>
                <item><description>Coefficient values are non-negative and in hexadecimal format (upper and lower case letters are both supported)</description></item>
                <item><description>Exponents are positive and in decimal format</description></item>
                <item><description>Zero coefficient terms (including the constant term) may be (but do not have to be) omitted</description></item>
                <item><description>Term with the exponent value of one is written as x^1</description></item>
                <item><description>Term with the exponent value of zero (the constant term) is written as just a hexadecimal number without x or exponent</description></item>
                <item><description>Terms are separated exactly by &lt;space&gt;+&lt;space&gt;</description></item>
                <item><description>Other than the +, no other terms have whitespace</description></item>
                </list>
                </para>
                </remarks>
                <param name="hexPoly">The formatted polynomial string specifying the plaintext polynomial</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if hexPoly does not adhere to the expected format</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if hexPoly or pool is null</exception>
                */
                Plaintext(System::String ^hexPoly, MemoryPoolHandle ^pool);

                /**
                <summary>Constructs a plaintext from a given hexadecimal string describing the plaintext polynomial.</summary>

                <remarks>
                <para>
                Constructs a plaintext from a given hexadecimal string describing the plaintext polynomial.
                The memory pool is set to the global memory pool.
                </para>
                <para>
                The string description of the polynomial must adhere to the format returned by <see cref="ToString()"/>,
                which is of the form "7FFx^3 + 1x^1 + 3" and summarized by the following rules:
                <list type="number">
                <item><description>Terms are listed in order of strictly decreasing exponent</description></item>
                <item><description>Coefficient values are non-negative and in hexadecimal format (upper and lower case letters are both supported)</description></item>
                <item><description>Exponents are positive and in decimal format</description></item>
                <item><description>Zero coefficient terms (including the constant term) may be (but do not have to be) omitted</description></item>
                <item><description>Term with the exponent value of one is written as x^1</description></item>
                <item><description>Term with the exponent value of zero (the constant term) is written as just a hexadecimal number without x or exponent</description></item>
                <item><description>Terms are separated exactly by &lt;space&gt;+&lt;space&gt;</description></item>
                <item><description>Other than the +, no other terms have whitespace</description></item>
                </list>
                </para>
                </remarks>
                <param name="hexPoly">The formatted polynomial string specifying the plaintext polynomial</param>
                <exception cref="System::ArgumentException">if hexPoly does not adhere to the expected format</exception>
                <exception cref="System::ArgumentNullException">if hexPoly is null</exception>
                */
                Plaintext(System::String ^hexPoly);

                /**
                <summary>Constructs a new plaintext by copying a given one.</summary>

                <param name="copy">The plaintext to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                Plaintext(Plaintext ^copy);

                /**
                <summary>Allocates enough memory to accommodate the backing array of a plaintext with given capacity.</summary>

                <remarks>
                Allocates enough memory to accommodate the backing array of a plaintext with given capacity.
                The allocation is made from the memory pool pointed to by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="capacity">The capacity</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if capacity is negative</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if pool is null</exception>
                */
                void Reserve(int capacity, MemoryPoolHandle ^pool);

                /**
                <summary>Allocates enough memory to accommodate the backing array of a plaintext with given capacity.</summary>

                <remarks>
                Allocates enough memory to accommodate the backing array of a plaintext with given
                capacity. The allocation is made from the memory pool pointed to by the currently held
                <see cref="MemoryPoolHandle" />. If the currently held <see cref="MemoryPoolHandle " />
                is uninitialized, it is set to point to the global memory pool instead, and the memory 
                is allocated from the global memory pool.
                </remarks>
                <param name="capacity">The capacity</param>
                <exception cref="System::ArgumentException">if capacity is negative</exception>
                */
                void Reserve(int capacity);

                /**
                <summary>Resets the plaintext.</summary>

                <remarks>
                Resets the plaintext. This function releases any memory allocated by the plaintext,
                returning it to the memory pool pointed to by the current <see cref="MemoryPoolHandle" />.
                </remarks>
                */
                void Release();

                /**
                <summary>Resizes the plaintext to have a given coefficient count.</summary>

                <remarks>
                Resizes the plaintext to have a given coefficient count. The plaintext is automatically
                reallocated if the new coefficient count does not fit in the current capacity. The allocation 
                is made from the memory pool pointed to by the given <see cref="MemoryPoolHandle" />. 
                </remarks>
                <param name="coeffCount">The number of coefficients in the plaintext polynomial</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if coeffCount is negative</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if pool is null</exception>
                */
                void Resize(int coeffCount, MemoryPoolHandle ^pool);

                /**
                <summary>Resizes the plaintext to have a given coefficient count.</summary>

                <remarks>
                Resizes the plaintext to have a given coefficient count. The plaintext is automatically
                reallocated if the new coefficient count does not fit in the current capacity. The allocation 
                is made from the memory pool pointed to by the currently held <see cref="MemoryPoolHandle" />. 
                If the currently held <see cref="MemoryPoolHandle" /> is uninitialized, it is set to point 
                to the global memory pool instead, and the memory is allocated from the global memory pool. 
                </remarks>
                <param name="coeffCount">The number of coefficients in the plaintext polynomial</param>
                <exception cref="System::ArgumentException">if coeffCount is negative</exception>
                */
                void Resize(int coeffCount);

                /**
                <summary>Copies a given plaintext to the current one.</summary>

                <param name="assign">The plaintext to copy from</param>
                <exception cref="System::ArgumentNullException">if assign is null</exception>
                */
                void Set(Plaintext ^assign);

                /**
                <summary>Sets the value of the current plaintext to the polynomial represented by the given <see cref="BigPoly" />.</summary>

                <param name="poly">The plaintext polynomial</param> 
                <exception cref="System::ArgumentException">if the coefficients of poly are too wide</exception>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                */
                void Set(BigPoly ^poly);

                /**
                <summary>Sets the value of the current plaintext to the polynomial represented by the a given hexadecimal string.</summary>

                <remarks>
                <para>
                Sets the value of the current plaintext to the polynomial represented by the a given hexadecimal string.
                </para>
                <para>
                The string description of the polynomial must adhere to the format returned by <see cref="ToString()"/>,
                which is of the form "7FFx^3 + 1x^1 + 3" and summarized by the following rules:
                <list type="number">
                <item><description>Terms are listed in order of strictly decreasing exponent</description></item>
                <item><description>Coefficient values are non-negative and in hexadecimal format (upper and lower case letters are both supported)</description></item>
                <item><description>Exponents are positive and in decimal format</description></item>
                <item><description>Zero coefficient terms (including the constant term) may be (but do not have to be) omitted</description></item>
                <item><description>Term with the exponent value of one is written as x^1</description></item>
                <item><description>Term with the exponent value of zero (the constant term) is written as just a hexadecimal number without x or exponent</description></item>
                <item><description>Terms are separated exactly by &lt;space&gt;+&lt;space&gt;</description></item>
                <item><description>Other than the +, no other terms have whitespace</description></item>
                </list>
                </para>
                </remarks>
                <param name="hexPoly">The formatted polynomial string specifying the plaintext polynomial</param>
                <exception cref="System::ArgumentException">if hexPoly does not adhere to the expected format</exception>
                <exception cref="System::ArgumentException">if the coefficients of hexPoly are too wide</exception>
                <exception cref="System::ArgumentNullException">if hexPoly is null</exception>
                */
                void Set(System::String ^hexPoly);

                /**
                <summary>Sets the value of the current plaintext to a given constant polynomial.</summary>
                
                <remarks>
                Sets the value of the current plaintext to a given constant polynomial. The coefficient count
                is set to one.
                </remarks>
                <param name="constCoeff">The constant coefficient</param>
                */
                void Set(System::UInt64 constCoeff);

                /**
                <summary>Sets a given range of coefficients of a plaintext polynomial to zero.</summary>

                <param name="startCoeff">The index of the first coefficient to set to zero</param>
                <param name="length">The number of coefficients to set to zero</param>
                <exception cref="System::ArgumentOutOfRangeException">if start_coeff is not within [0, CoeffCount)</exception>
                <exception cref="System::ArgumentOutOfRangeException">if length is negative or start_coeff + length is not within [0, CoeffCount)</exception>
                */
                void SetZero(int startCoeff, int length);

                /**
                <summary>Sets the plaintext polynomial coefficients to zero starting at a given index.</summary>

                <param name="startCoeff">The index of the first coefficient to set to zero</param>
                <exception cref="System::ArgumentOutOfRangeException">if start_coeff is not within [0, CoeffCount)</exception>
                */
                void SetZero(int startCoeff);

                /**
                <summary>Sets the plaintext polynomial to zero.</summary>
                */
                void SetZero();

                /**
                <summary>Gets/set the value of a given coefficient of the plaintext polynomial.</summary>

                <param name="coeffIndex">The index of the coefficient in the plaintext polynomial</param>
                <exception cref="System::ArgumentOutOfRangeException">if coeffIndex is not within [0, CoeffCount)</exception>
                */
                property System::UInt64 default[int]{
                    System::UInt64 get(int index);

                    void set(int index, System::UInt64 value);
                }

                /**
                <summary>Returns whether or not the plaintext has the same semantic value as a given plaintext.</summary>

                <remarks>
                Returns whether or not the plaintext has the same semantic value as a given plaintext. Leading
                zero coefficients are ignored by the comparison.
                </remarks>
                <param name="compare">The plaintext to compare against</param>
                */
                virtual bool Equals(Plaintext ^compare);

                /**
                <summary>Returns whether or not the plaintext has the same semantic value as a given plaintext.</summary>

                <remarks>
                Returns whether or not the plaintext has the same semantic value as a given plaintext. Leading
                zero coefficients are ignored by the comparison.
                </remarks>
                <param name="compare">The plaintext to compare against</param>
                */
                bool Equals(System::Object ^compare) override;

                /**
                <summary>Returns whether the plaintext polynomial has all zero coefficients.</summary>
                */
                property bool IsZero {
                    bool get();
                }

                /**
                <summary>Returns the capacity of the current allocation.</summary>
                */
                property int Capacity {
                    int get();
                }

                /**
                <summary>Returns the coefficient count of the current plaintext polynomial.</summary>
                */
                property int CoeffCount {
                    int get();
                }

                /**
                <summary>Returns the significant coefficient count of the current plaintext polynomial.</summary>
                */
                int SignificantCoeffCount();

                /**
                <summary>Returns a human-readable string description of the plaintext polynomial.</summary>
                <remarks>
                <para>
                Returns a human-readable string description of the plaintext polynomial.
                </para>
                <para>
                The returned string is of the form "7FFx^3 + 1x^1 + 3" with a format summarized by the following:
                <list type="number">
                <item><description>Terms are listed in order of strictly decreasing exponent</description></item>
                <item><description>Coefficient values are non-negative and in hexadecimal format (hexadecimal letters are in upper-case)</description></item>
                <item><description>Exponents are positive and in decimal format</description></item>
                <item><description>Zero coefficient terms (including the constant term) are omitted unless the polynomial is exactly 0 (see rule 9)</description></item>
                <item><description>Term with the exponent value of one is written as x^1</description></item>
                <item><description>Term with the exponent value of zero (the constant term) is written as just a hexadecimal number without x or exponent</description></item>
                <item><description>Terms are separated exactly by &lt;space&gt;+&lt;space&gt;</description></item>
                <item><description>Other than the +, no other terms have whitespace</description></item>
                <item><description>If the polynomial is exactly 0, the string "0" is returned</description></item>
                </list>
                </para>
                </remarks>
                */
                System::String ^ToString() override;

                /**
                <summary>Returns a hash-code based on the value of the plaintext polynomial.</summary>
                */
                int GetHashCode() override;

                /**
                <summary>Saves the plaintext to an output stream.</summary>
                
                <remarks>
                Saves the plaintext to an output stream. The output is in binary format and not human-readable. 
                The output stream must have the "binary" flag set.
                </remarks>
                <param name="stream">The stream to save the plaintext to</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                <seealso cref="Load()">See Load() to load a saved plaintext.</seealso>
                */
                void Save(System::IO::Stream ^stream);

                /**
                <summary>Loads a plaintext from an input stream overwriting the current plaintext.</summary>

                <param name="stream">The stream to load the plaintext from</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                <seealso cref="Save()">See Save() to save a plaintext.</seealso>
                */
                void Load(System::IO::Stream ^stream);

                /**
                <summary>Destroys the plaintext.</summary>
                */
                ~Plaintext();

                /**
                <summary>Destroys the plaintext.</summary>
                */
                !Plaintext();

            internal:
                /**
                <summary>Creates a deep copy of a C++ Plaintext instance.</summary>
                <param name="value">The Plaintext instance to copy from</param>
                <exception cref="System::ArgumentNullException">If copy is null</exception>
                */
                Plaintext(const seal::Plaintext &value);

                /**
                <summary>Returns a reference to the underlying C++ Plaintext.</summary>
                */
                seal::Plaintext &GetPlaintext();

            private:
                seal::Plaintext *plaintext_;
            };
        }
    }
}