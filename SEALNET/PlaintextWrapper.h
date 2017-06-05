#pragma once

#include "plaintext.h"
#include "BigPolyWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            public ref class Plaintext
            {
            public:
                /**
                <summary>Creates a Plaintext wrapping a constant polynomial 0.</summary>
                */
                Plaintext();

                /**
                <summary>Creates a Plaintext by copying a given BigPoly instance.</summary> 
                
                <remarks>
                Creates a Plaintext by copying a given <see cref="BigPoly" /> instance. The created 
                Plaintext will wrap a duplicate of the given <see cref="BigPoly" />.
                </remarks>
                <param name="poly">The plaintext polynomial</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                */
                static operator Plaintext ^(BigPoly ^poly);

                /**
                <summary>Creates a Plaintext from a given hexadecimal string describing the plaintext polynomial.</summary>

                <remarks>
                <para>
                Creates a Plaintext from a given hexadecimal string describing the plaintext polynomial.
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
                <exception cref="System::ArgumentNullException">if hexPoly is null</exception>
                <exception cref="System::ArgumentException">if hexPoly does not adhere to the expected format</exception>
                */
                Plaintext(System::String ^hexPoly);

                /**
                <summary>Creates a copy of a Plaintext.</summary>

                <param name="copy">The Plaintext to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                Plaintext(Plaintext ^copy);

                /**
                <summary>Copies an old Plaintext to the current one.</summary>

                <param name="assign">The Plaintext to copy from</param>
                <exception cref="System::ArgumentNullException">if assign is null</exception>
                */
                void Set(Plaintext ^assign);

                /**
                <summary>Sets the current Plaintext to wrap a given <see cref="BigPoly" /> by creating a copy of it.</summary>

                <param name="poly">The polynomial to copy from</param> 
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                */
                void Set(BigPoly ^poly);

                /**
                <summary>Sets the underlying plaintext polynomial from a given hexadecimal string.</summary>

                <remarks>
                <para>
                Sets the underlying plaintext polynomial from a given hexadecimal string.
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
                <exception cref="System::ArgumentNullException">if hexPoly is null</exception>
                <exception cref="System::ArgumentException">if hexPoly does not adhere to the expected format</exception>
                */
                void Set(System::String ^hexPoly);

                /**
                <summary>Returns a copy of the underlying plaintext polynomial.</summary>

                <param name="plaintext">The plaintext object</param>
                <exception cref="System::ArgumentNullException">if plaintext is null</exception>
                */
                static operator BigPoly^ (Plaintext ^plaintext);

                /**
                <summary>Saves the Plaintext to an output stream.</summary>
                <remarks>
                Saves the Plaintext to an output stream. The output is in binary format and not human-readable. 
                The output stream must have the "binary" flag set.
                </remarks>

                <param name="stream">The stream to save the Plaintext to</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                <seealso cref="Load()">See Load() to load a saved Plaintext.</seealso>
                */
                void Save(System::IO::Stream ^stream);

                /**
                <summary>Loads a Plaintext from an input stream overwriting the current Plaintext.</summary>

                <param name="stream">The stream to load the Plaintext from</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                <seealso cref="Save()">See Save() to save a Plaintext.</seealso>
                */
                void Load(System::IO::Stream ^stream);

                /**
                <summary>Destroys the Plaintext.</summary>
                */
                ~Plaintext();

                /**
                <summary>Destroys the Plaintext.</summary>
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