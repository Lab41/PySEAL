#pragma once

#include "bigpoly.h"
#include "BigUIntWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class BigPolyArray;

            ref class BigPolyArithmetic;

            ref class Evaluator;

            ref class Decryptor;

            /**
            <summary>Represents a polynomial consisting of a set of unsigned integer coefficients with a specified bit width.</summary>
            <remarks>
            <para>
            Represents a polynomial consisting of a set of unsigned integer coefficients with a specified bit width.
            BigPolys are mutable and able to be resized. Individual coefficients can be read/written through the
            indexer property. A BigPoly has a set coefficient count (which can be read with <see cref="CoeffCount"/>)
            and coefficient bit width (which can be read with <see cref="CoeffBitCount"/>), and all coefficients in a BigPoly have
            the same bit width. The coefficient count and bit width of a BigPoly is set initially by the constructor, and
            can be resized either explicitly with the <see cref="Resize()"/> function, or implicitly with for example 
            assignment.
            </para>

            <para>
            A BigPoly's coefficients are stored sequentially, index-zero coefficient first, in a contiguous System::UInt64
            array. The width of each coefficient is rounded up to the next System::UInt64 width (i.e., to the next 64-bit
            boundary). The <see cref="CoeffUInt64Count"/> function returns the number of System::UInt64 values used per
            coefficient. The <see cref="UInt64Count"/> function returns the number of System::UInt64 values used to store
            all coefficients. Each coefficient is stored in an identical format to <see cref="BigUInt"/>, with the least quad word first
            and the order of bits for each quad word dependent on the architecture's System::UInt64 representation. For
            each coefficient, the bits higher than the coefficient bit count must be set to zero to prevent undefined behavior. The
            <see cref="Pointer"/> function returns a pointer to the first System::UInt64 of the array.
            </para>

            <para>
            Both the copy constructor and the Set function allocate more memory for the backing array when needed, 
            i.e. when the source polynomial has a larger backing array than the destination. Conversely, when the destination 
            backing array is already large enough, the data is only copied and the unnecessary higher degree coefficients 
            are set to zero. When new memory has to be allocated, only the significant coefficients of the source polynomial
            are taken into account. This is is important, because it avoids unnecessary zero coefficients to be included
            in the destination, which in some cases could accumulate and result in very large unnecessary allocations.
            However, sometimes it is necessary to preserve the original coefficient count, even if some of the
            leading coefficients are zero. This comes up for example when copying individual polynomials of ciphertext
            <see cref="BigPolyArray" /> objects, as these polynomials need to have the leading coefficient equal to zero to 
            be considered valid by classes such as <see cref="Evaluator"/> and <see cref="Decryptor"/>. For this purpose 
            BigPoly contains functions <see cref="DuplicateFrom" /> and <see cref="DuplicateTo"/>, which create an exact 
            copy of the source BigPoly.
            </para>

            <para>
            An aliased BigPoly (which can be determined with <see cref="IsAlias"/>) is a special type of BigPoly that does not manage
            its underlying System::UInt64 pointer that stores the coefficients. An aliased BigPoly supports most of the
            same operations as a non-aliased BigPoly, including reading/writing the coefficients and assignment, however an
            aliased BigPoly does not internally allocate or deallocate its backing array and, therefore, does not support resizing.
            Any attempt, either explicitly or implicitly, to resize the BigPoly will result in an exception being thrown. Aliased
            BigPoly's are only created internally.
            </para>

            <para>
            In general, reading a BigPoly is thread-safe while mutating is not. Specifically, the backing array may be freed
            whenever a resize occurs or the BigPoly is destroyed, which would invalidate the address returned by <see cref="Pointer"/>
            and the coefficients returned by the indexer property. When it is known a resize will not occur, concurrent reading and
            mutating is safe as long as it is known that concurrent reading and mutating for the same coefficient will not occur,
            as the reader may see a partially updated coefficient value.
            </para>
            </remarks>

            <seealso cref="BigPolyArithmetic">See BigPolyArithmetic for arithmetic and modular functions on BigPolys.</seealso>
            <seealso cref="BigUInt">See BigUInt for more details on the coefficients returned by this[int].</seealso>
            */
            public ref class BigPoly : System::IEquatable<BigPoly^>
            {
            public:
                /**
                <summary>Creates an empty BigPoly with zero coefficients and zero coefficient bit width.</summary>
                <remarks>
                Creates an empty BigPoly with zero coefficients and zero coefficient bit width. No memory is allocated by this
                constructor.
                </remarks>
                */
                BigPoly();

                /**
                <summary>Creates a zero-initialized BigPoly of the specified coefficient count and bit width.</summary>

                <param name="coeffCount">The number of coefficients</param>
                <param name="coeffBitCount">The bit width of each coefficient</param>
                <exception cref="System::ArgumentException">if either coeffCount or coeffBitCount is negative</exception>
                */
                BigPoly(int coeffCount, int coeffBitCount);

                /**
                <summary>Creates a BigPoly populated and minimally sized to fit the polynomial described by the formatted string.</summary>
                <remarks>
                <para>
                Creates a BigPoly populated and minimally sized to fit the polynomial described by the formatted string.
                </para>

                <para>
                The string description of the polynomial must adhere to the format returned by <see cref="ToString()"/>, which is of the form
                "7FFx^3 + 1x^1 + 3" and summarized by the following rules:
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

                <param name="hexPoly">The formatted polynomial string specifying the initial value</param>
                <exception cref="System::ArgumentNullException">if hexPoly is null</exception>
                <exception cref="System::ArgumentException">if hexPoly does not adhere to the expected format</exception>
                */
                BigPoly(System::String ^hexPoly);

                /**
                <summary>Creates a BigPoly of the specified coefficient count and bit width and initializes it with the polynomial described by
                the formatted string.</summary>
                <remarks>
                <para>
                Creates a BigPoly of the specified coefficient count and bit width and initializes it with the polynomial described by
                the formatted string.
                </para>
                <para>
                The string description of the polynomial must adhere to the format returned by <see cref="ToString()"/>, which is of the form
                "7FFx^3 + 1x^1 + 3" and summarized by the following rules:
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

                <param name="coeffCount">The number of coefficients</param>
                <param name="coeffBitCount">The bit width of each coefficient</param>
                <param name="hexPoly">The formatted polynomial string specifying the initial value</param>
                <exception cref="System::ArgumentException">if either coeffCount or coeffBitCount is negative</exception>
                <exception cref="System::ArgumentNullException">if hexPoly is null</exception>
                <exception cref="System::ArgumentException">if hexPoly does not adhere to the expected format</exception>
                */
                BigPoly(int coeffCount, int coeffBitCount, System::String ^hexPoly);

                /**
                <summary>Creates a deep copy of a BigPoly.</summary>
                <remarks>
                Creates a deep copy of a BigPoly. The created BigPoly will have the same coefficient count, coefficient bit count,
                and coefficient values as the original.
                </remarks>

                <param name="copy">The BigPoly to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                BigPoly(BigPoly ^copy);

                /**
                <summary>Returns whether or not the BigPoly is an alias.</summary>

                <seealso cref="BigPoly">See BigPoly for a detailed description of aliased BigPolys.</seealso>
                */
                property bool IsAlias {
                    bool get();
                }

                /**
                <summary>Returns the coefficient count for the BigPoly.</summary>

                <seealso cref="GetSignificantCoeffCount()">See GetSignificantCoeffCount() to instead ignore the leading coefficients that have a value
                of zero.</seealso>
                */
                property int CoeffCount {
                    int get();
                }

                /**
                <summary>Returns the number of bits per coefficient.</summary>

                <seealso cref="GetSignificantCoeffBitCount()">See GetSignificantCoeffBitCount() to instead get the number of significant bits of the
                largest coefficient in the BigPoly.</seealso>
                */
                property int CoeffBitCount {
                    int get();
                }

                /**
                <summary>Returns the total number of System::UInt64 in the backing array to store all of the coefficients of the BigPoly.</summary>

                <seealso cref="CoeffUInt64Count">See CoeffUInt64Count to determine the number of System::UInt64 values used for each
                individual coefficient.</seealso>
                <seealso cref="BigPoly">See BigPoly for a more detailed description of the format of the backing array.</seealso>
                */
                property int UInt64Count {
                    int get();
                }

                /**
                <summary>Returns the number of System::UInt64 in the backing array used to store each coefficient.</summary>

                <seealso cref="UInt64Count">See UInt64Count to determine the total number of System::UInt64 values used to
                store all of the coefficients.</seealso>
                <seealso cref="BigPoly">See BigPoly for a more detailed description of the format of the backing array.</seealso>
                */
                property int CoeffUInt64Count {
                    int get();
                }

                /**
                <summary>Returns a pointer to the backing array storing all of the coefficient values.</summary>
                <remarks>
                <para>
                Returns a pointer to the backing array storing all of the coefficient values. The pointer points to the beginning of the
                backing array where all coefficients are stored sequentially. The pointer points to the coefficient for the constant
                (degree 0) term of the BigPoly. Note that the return value will be null if the coefficient count and/or bit count is zero.
                </para>

                <para>
                The pointer is valid only until the backing array is freed, which occurs when the BigPoly is resized or destroyed.
                </para>
                </remarks>
                <seealso cref="CoeffUInt64Count">See CoeffUInt64Count to determine the number of System::UInt64 values used for each
                coefficient in the backing array.</seealso>
                <seealso cref="BigPoly">See BigPoly for a more detailed description of the format of the backing array.</seealso>
                */
                property System::UInt64 *Pointer {
                    System::UInt64 *get();
                }

                /**
                <summary>Returns whether or not the BigPoly has all zero coefficients.</summary>
                */
                property bool IsZero {
                    bool get();
                }

                /**
                <summary>Returns a BigUInt that can read or write the coefficient at the specified index.</summary>
                <remarks>
                <para>
                Returns a BigUInt that can read or write the coefficient at the specified index. The BigUInt is an aliased BigUInt that points
                directly to the backing array of the BigPoly.
                </para>

                <para>
                The returned BigUInt is an alias backed by a region of the BigPoly's backing array. As such, it is only valid until
                the BigPoly is resized or destroyed.
                </para>
                </remarks>

                <param name="index">The index of the coefficient to read/write</param>
                <exception cref="System::ArgumentOutOfRangeException">if index is not within [0, <see cref="CoeffCount"/>)</exception>
                <seealso cref="BigUInt">See BigUInt for operations that can be performed on the coefficients.</seealso>
                */
                property BigUInt ^default[int]{
                    BigUInt ^get(int index);
                }

                /**
                <summary>Returns the coefficient count for the BigPoly ignoring all of the highest coefficients that have value zero.</summary>

                <seealso cref="CoeffCount">See CoeffCount to instead return the coefficient count regardless of the number of leading zero terms.
                </seealso>
                */
                int GetSignificantCoeffCount();

                /**
                <summary>Returns the number of significant bits of the largest coefficient in the BigPoly.</summary>

                <seealso cref="CoeffBitCount">See CoeffBitCount to instead return the coefficient bit count regardless of leading zero bits.</seealso>
                */
                int GetSignificantCoeffBitCount();

                /**
                <summary>Overwrites the BigPoly with the value of the specified BigPoly, enlarging if needed to fit the assigned value.</summary>
                <remarks>
                Overwrites the BigPoly with the value of the specified BigPoly, enlarging if needed to fit the assigned value.
                Only significant coefficients and significant coefficient bit counts are used to size the BigPoly.
                </remarks>

                <param name="assign">The BigPoly whose value should be assigned to the current BigPoly</param>
                <exception cref="System::ArgumentNullException">if assign is null</exception>
                <exception cref="System::InvalidOperationException">if BigPoly is an alias and the assigned BigPoly is too large to fit the current
                coefficient count and/or bit width</exception>
                */
                void Set(BigPoly ^assign);

                /**
                <summary>Overwrites the BigPoly with the value of the polynomial described by the formatted string, enlarging if needed to fit
                the assigned value.</summary>
                <remarks>
                <para>
                Overwrites the BigPoly with the value of the polynomial described by the formatted string, enlarging if needed to fit
                the assigned value.
                </para>

                <para>
                The string description of the polynomial must adhere to the format returned by <see cref="ToString()"/>, which is of the form
                "7FFx^3 + 1x^1 + 3" and summarized by the following rules:
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

                <param name="assign">The formatted polynomial string specifying the value to set</param>
                <exception cref="System::ArgumentNullException">if assign is null</exception>
                <exception cref="System::ArgumentException">if assign does not adhere to the expected format</exception>
                <exception cref="System::InvalidOperationException">if BigPoly is an alias and the assigned polynomial is too large to fit the
                current coefficient count and/or bit width</exception>
                */
                void Set(System::String ^assign);

                /**
                <summary>Sets all coefficients to have a value of zero.</summary>
                <remarks>Sets all coefficients to have a value of zero. This does not resize the BigPoly.</remarks>
                */
                void SetZero();

                /**
                <summary>Sets all coefficients within [startCoeff, coefficient count) to have a value of zero.</summary>
                <remarks>
                Sets all coefficients within [startCoeff, coefficient count) to have a value of zero. This does not resize the BigPoly or
                modify the other coefficients.
                </remarks>

                <param name="startCoeff">The index of the first coefficient to set to zero</param>
                <exception cref="System::ArgumentOutOfRangeException">if startCoeff is not within [0, <see cref="CoeffCount"/>]</exception>
                */
                void SetZero(int startCoeff);

                /**
                <summary>Sets all coefficients within [startCoeff, startCoeff + length) to have a value of zero.</summary>
                <remarks>
                Sets all coefficients within [startCoeff, startCoeff + length) to have a value of zero. This does not resize the BigPoly
                or modify the other coefficients.
                </remarks>

                <param name="startCoeff">The index of the first coefficient to set to zero</param>
                <param name="coeffCount">The number of coefficients to set to zero</param>
                <exception cref="System::ArgumentOutOfRangeException">if startCoeff is not within [0, <see cref="CoeffCount"/>]</exception>
                <exception cref="System::ArgumentOutOfRangeException">if length is negative or start_coeff + length is not within
                [0, <see cref="CoeffCount"/>]</exception>
                */
                void SetZero(int startCoeff, int coeffCount);

                /**
                <summary>Saves the BigPoly to an output stream.</summary>
                <remarks>
                Saves the BigPoly to an output stream. The full state of the BigPoly is serialized, including leading zero
                coefficients and insignificant coefficient bits. The output is in binary format and not human-readable. The
                output stream must have the "binary" flag set.
                </remarks>

                <param name="stream">The stream to save the BigPoly to</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                <seealso cref="Load()">See Load() to load a saved BigPoly.</seealso>
                */
                void Save(System::IO::Stream ^stream);

                /**
                <summary>Loads a BigPoly from an input stream overwriting the current BigPoly and enlarging if needed to fit the loaded
                BigPoly.</summary>

                <param name="stream">The stream to load the BigPoly from</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                <exception cref="System::InvalidOperationException">if BigPoly is an alias and the loaded polynomial is too large to fit the
                current coefficient count and/or bit width</exception>
                <seealso cref="Save()">See Save() to save a BigPoly.</seealso>
                */
                void Load(System::IO::Stream ^stream);

                /**
                <summary>Resizes a BigPoly to the specified coefficient count and bit width, copying over and resizing existing coefficient values
                as much as will fit.</summary>
                <remarks>
                Resizes a BigPoly to the specified coefficient count and bit width, copying over and resizing existing coefficient values
                as much as will fit. If coefficient count is reduced, the leading coefficients are dropped. If coefficient count increases,
                the new coefficients are initialized to zero.
                </remarks>

                <param name="coeffCount">The number of coefficients</param>
                <param name="coeffBitCount">The bit width of each coefficient</param>
                <exception cref="System::ArgumentException">if either coeffCount or coeffBitCount is negative</exception>
                <exception cref="System::InvalidOperationException">if the BigPoly is an alias</exception>
                */
                void Resize(int coeffCount, int coeffBitCount);

                /**
                <summary>Returns whether or not the BigPoly has the same value as a specified BigPoly.</summary>
                <remarks>
                Returns whether or not the BigPoly has the same value as a specified BigPoly. Value equality is not determined by the
                raw coefficient count or bit count, but rather if the significant bits of non-zero coefficients have the same value.
                </remarks>

                <param name="compare">The BigPoly to compare against</param>
                <exception cref="System::ArgumentNullException">if compare is null</exception>
                */
                virtual bool Equals(BigPoly ^compare);

                /**
                <summary>Returns whether or not the BigPoly has the same value as a specified BigPoly.</summary>
                <remarks>
                Returns whether or not the BigPoly has the same value as a specified BigPoly. Value equality is not determined by the
                raw coefficient count or bit count, but rather if the significant bits of non-zero coefficients have the same value.
                </remarks>

                <param name="compare">The BigPoly to compare against</param>
                */
                bool Equals(System::Object ^compare) override;

                /**
                <summary>Returns a human-readable string description of the BigPoly.</summary>
                <remarks>
                <para>
                Returns a human-readable string description of the BigPoly.
                </para>

                <para>
                The returned string is of the form "7FFx^3 + 1x^1 + 3" with a format summarized by the following:
                <list type="number">
                <item><description>Terms are listed in order of strictly decreasing exponent</description></item>
                <item><description>Coefficient values are non-negative and in hexadecimal format (hexadecimal letters are in upper-case)</description></item>
                <item><description>Exponents are positive and in decimal format</description></item>
                <item><description>Zero coefficient terms (including the constant term) are omitted unless the BigPoly is exactly 0 (see rule 9)</description></item>
                <item><description>Term with the exponent value of one is written as x^1</description></item>
                <item><description>Term with the exponent value of zero (the constant term) is written as just a hexadecimal number without x or exponent</description></item>
                <item><description>Terms are separated exactly by &lt;space&gt;+&lt;space&gt;</description></item>
                <item><description>Other than the +, no other terms have whitespace</description></item>
                <item><description>If the BigPoly is exactly 0, the string "0" is returned</description></item>
                </list>
                </para>
                </remarks>
                */
                System::String ^ToString() override;

                /**
                <summary>Returns a hash-code based on the value of the BigPoly.</summary>
                */
                int GetHashCode() override;

                /**
                <summary>Destroys the BigPoly, including deallocating any internally allocated space.</summary>
                */
                ~BigPoly();

                /**
                <summary>Destroys the BigPoly, including deallocating any internally allocated space.</summary>
                */
                !BigPoly();

                /**
                <summary>Returns a reference to the underlying C++ BigPoly.</summary>
                */
                seal::BigPoly &GetPolynomial();

                /**
                <summary>Duplicates the current BigPoly.</summary>
                <remarks>
                Duplicates the current BigPoly. The coefficient count, the coefficient bit count, and the value of the given BigPoly
                are set to be exactly the same as in the current one.
                </remarks>
                <param name="destination">The BigPoly to overwrite with the duplicate</param>
                <exception cref="System::ArgumentNullException">if destination is null</exception>
                <exception cref="System::InvalidOperationException">if the destination BigPoly is an alias</exception>
                */
                void DuplicateTo(BigPoly ^destination);

                /**
                <summary>Duplicates a given BigPoly.</summary>
                <remarks>
                Duplicates a given BigPoly. The coefficient count, the coefficient bit count,
                and the value of the current BigPoly are set to be exactly the same as in the given one.
                </remarks>
                <param name="value">The BigPoly to duplicate</param>
                <exception cref="System::ArgumentNullException">if value is null</exception>
                <exception cref="System::InvalidOperationException">if the current BigPoly is an alias</exception>
                */
                void DuplicateFrom(BigPoly ^value);

            internal:
                /**
                <summary>Creates a deep copy of a C++ BigPoly.</summary>
                <remarks>
                Creates a deep copy of a C++ BigPoly. The created BigPoly will have the same coefficient count, coefficient bit count,
                and coefficient values as the original.
                </remarks>

                <param name="value">The BigPoly to copy from</param>
                */
                BigPoly(const seal::BigPoly &value);

                /**
                <summary>Initializes the BigPoly to use the specified C++ BigPoly.</summary>
                <remarks>
                Initializes the BigPoly to use the specified C++ BigPoly. This constructor does not copy the C++ BigPoly but actually
                uses the specified C++ BigPoly as the backing data. Upon destruction, the managed BigPoly will not destroy the C++
                BigPoly.
                </remarks>
                <param name="value">The BigPoly to use as the backing BigPoly</param>
                */
                BigPoly(seal::BigPoly *value);

            private:
                seal::BigPoly *bigpoly_;

                bool owned_;
            };
        }
    }
}
