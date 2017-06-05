#pragma once

#include "ciphertext.h"
#include "BigPolyArrayWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            public ref class Ciphertext
            {
            public:
                /**
                <summary>Creates a Ciphertext by copying a given <see cref="BigPolyArray"/> instance.</summary>

                <remarks>
                Creates a Ciphertext by copying a given <see cref="BigPolyArray"/> instance. The created Ciphertext
                will wrap a duplicate of the given <see cref="BigPolyArray"/>.
                </remarks>
                <param name="polyArray">The polynomial array</param>
                <exception cref="System::ArgumentNullException">if polyArray is null</exception>
                */
                static operator Ciphertext ^(BigPolyArray ^polyArray);

                /**
                <summary>Creates a copy of a Ciphertext.</summary>

                <param name="copy">The Ciphertext to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                Ciphertext(Ciphertext ^copy);

                /**
                <summary>Copies an old Ciphertext to the current one.</summary>

                <param name="assign">The Ciphertext to copy from</param>
                <exception cref="System::ArgumentNullException">if assign is null</exception>
                */
                void Set(Ciphertext ^assign);

                /**
                <summary>Sets the current Ciphertext to wrap a given <see cref="BigPolyArray" /> by creating a copy of it.</summary>

                <param name="polyArray">The polynomial array to copy from</param>
                <exception cref="System::ArgumentNullException">if polyArray is null</exception>
                */
                void Set(BigPolyArray ^polyArray);

                /**
                <summary>Returns a copy of the underlying polynomial array.</summary>

                <param name="ciphertext">The ciphertext object</param>
                <exception cref="System::ArgumentNullException">if ciphertext is null</exception>
                */
                static operator BigPolyArray^ (Ciphertext ^ciphertext);

                /**
                <summary>Returns the size of the ciphertext.</summary>
                */
                property int Size {
                    int get();
                }

                /**
                <summary>Saves the Ciphertext to an output stream.</summary>
                <remarks>
                Saves the Ciphertext to an output stream. The output is in binary format and not human-readable.
                The output stream must have the "binary" flag set.
                </remarks>

                <param name="stream">The stream to save the Ciphertext to</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                <seealso cref="Load()">See Load() to load a saved Ciphertext.</seealso>
                */
                void Save(System::IO::Stream ^stream);

                /**
                <summary>Loads a Ciphertext from an input stream overwriting the current Ciphertext.</summary>

                <param name="stream">The stream to load the Ciphertext from</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                <seealso cref="Save()">See Save() to save a Ciphertext.</seealso>
                */
                void Load(System::IO::Stream ^stream);

                /**
                <summary>Destroys the Ciphertext.</summary>
                */
                ~Ciphertext();

                /**
                <summary>Destroys the Ciphertext.</summary>
                */
                !Ciphertext();

            internal:
                /**
                <summary>Creates a deep copy of a C++ Ciphertext instance.</summary>
                <param name="value">The Ciphertext instance to copy from</param>
                <exception cref="System::ArgumentNullException">If copy is null</exception>
                */
                Ciphertext(const seal::Ciphertext &value);

                /**
                <summary>Returns a reference to the underlying C++ Ciphertext.</summary>
                */
                seal::Ciphertext &GetCiphertext();

            private:
                seal::Ciphertext *ciphertext_;
            };
        }
    }
}
