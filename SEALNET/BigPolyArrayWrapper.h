#pragma once

#include "bigpolyarray.h"
#include "BigPolyWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            /**
            <summary>Represents an array of BigPoly objects.</summary>

            <remarks>
            <para>
            Represents an array of BigPoly objects. The BigPolyArray class provides all of the functionality of a BigPoly array.
            The size of the array (which can be read with <see cref="Size" />) is set initially by the constructor and can be 
            resized either with the <see cref="Resize()"/> function or with the <see cref="Set()"/> function. 
            Each polynomial in the array must have the same coefficient count and coefficient bit count, which can be set in the 
            constructor or with the <see cref="Resize" /> function, and read with <see cref="CoeffCount" /> and <see cref="CoeffBitCount"/>.
            The index functions allow reading/writing individual BigPoly's in the array by returning references to them. 
            The array can be saved and loaded from a stream with the <see cref="Save()"/> and <see cref="Load()"/> functions.
            </para>
            <para>
            All of the polynomials in the array are stored in one continuous block in memory.
            </para>
            <para>
            In general, reading from a BigPolyArray instance is thread-safe while mutating is not. Specifically, the array may be freed
            whenever a <see cref="Resize()"/> occurs, the BigPolyArray instance is destroyed, or an assignment operation occurs, which 
            will invalidate the aliased BigPolys returned by the index functions.
            </para>
            </remarks>
            */
            public ref class BigPolyArray
            {
            public:
                /**
                <summary>Creates an empty BigPolyArray instance with a size of zero.</summary>

                <remarks>
                Creates an empty BigPolyArray instance with a size of zero. No memory is allocated by this constructor.
                </remarks>
                */
                BigPolyArray();

                /**
                <summary>Creates a zero-initialized BigPolyArray instance with the specified size.</summary>
                <param name="size">The number of polynomials to allocate space for</param>
                <param name="coeffCount">The number of coefficients allocated for each polynomial in the array</param>
                <param name="coeffBitCount">The bit count of each polynomial in the array</param>
                <exception cref="System::ArgumentException">if size is negative</exception>
                <exception cref="System::ArgumentException">if coeffCount is negative</exception>
                <exception cref="System::ArgumentException">if coeffBitCount is negative</exception>
                */
                BigPolyArray(int size, int coeffCount, int coeffBitCount);

                /**
                <summary>Creates a deep copy of an BigPolyArray instance.</summary>

                <remarks>Creates a deep copy of an BigPolyArray instance.</remarks>
                <param name="copy">The BigPolyArray instance to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                BigPolyArray(BigPolyArray ^copy);

                /**
                <summary>Returns the number of polynomials.</summary>
                */
                property int Size {
                    int get();
                }

                /**
                <summary>Returns the number of coefficients in the polynomials.</summary>
                */
                property int CoeffCount {
                    int get();
                }

                /**
                <summary>Returns the coefficient bit count of the polynomials.</summary>
                */
                property int CoeffBitCount {
                    int get();
                }

                /**
                <summary>Returns the number of System::UInt64 allocated for each coefficient of each polynomial.</summary>
                */
                int CoeffUInt64Count();

                /**
                <summary>Returns the number of System::UInt64 allocated for each polynomial.</summary>
                */
                int PolyUInt64Count();

                /**
                <summary>Returns the number of System::UInt64 allocated for the entire array of polynomials.</summary>
                */
                int UInt64Count();

                /**
                <summary>Returns a reference to the BigPoly at index polyIndex in the array.</summary>

                <remarks>
                <para>
                Returns a reference to the BigPoly at index polyIndex in the array. The returned BigPoly is an alias backed by 
                the BigPolyArray internal array. As such, it is only valid until the BigPolyArray instance is resized or destroyed.
                </para>
                </remarks>
                <exception cref="System::ArgumentOutOfRangeException">if polyIndex is not within [0, <see cref="Size"/>)</exception>
                */
                property BigPoly ^default[int] {
                    BigPoly ^get(int polyIndex);
                }

                /**
                <summary>Sets all polynomials to have a value of zero.</summary>

                <remarks>
                <para>
                Sets all polynomials to have a value of zero. This does not resize the BigPolyArray.
                </para>
                </remarks>
                */
                void SetZero();

                /**
                <summary>Sets the polynomial with index polyIndex to have value zero.</summary>

                <remarks>
                Sets the polynomial with index polyIndex to have value zero. This does not resize the BigPolyArray or modify the other
                polynomials.
                </remarks>
                <param name="polyIndex">The index of the polynomial to set to zero</param>
                <exception cref="System::ArgumentOutOfRangeException">If polyIndex is not within [0, size())</exception>
                */
                void SetZero(int polyIndex);

                /**
                <summary>Resizes the BigPolyArray internal array to store the specified number of polynomials of the specified size,
                copying over the old polynomials as much as will fit.</summary>
                <param name="size">The number of polynomials to allocate space for</param>
                <param name="coeffCount">The number of coefficients allocated for each polynomial in the array</param>
                <param name="coeffBitCount">The bit count of each polynomial in the array</param>
                <exception cref="System::ArgumentException">if size is negative</exception>
                <exception cref="System::ArgumentException">if coeffCount is negative</exception>
                <exception cref="System::ArgumentException">if coeffBitCount is negative</exception>
                */
                void Resize(int size, int coeffCount, int coeffBitCount);

                /**
                <summary>Resets the BigPolyArray instance to an empty, zero-sized instance.</summary>

                <remarks>
                Resets the BigPolyArray instance to an empty, zero-sized instance. Any space allocated by the BigPolyArray instance
                is deallocated.
                </remarks>
                */
                void Reset();

                /**
                <summary>Overwrites the BigPolyArray instance with a specified BigPolyArray instance.</summary>

                <remarks>
                Overwrites the BigPolyArray instance with the BigPolys in the specified BigPolyArray instance. After assignment,
                the size of BigPolyArray matches the size of the assigned BigPolyArray instance.
                </remarks>
                <param name="assign">The BigPolyArray instance to whose value should be assigned to the current BigPolyArray
                instance</param>
                <exception cref="System::ArgumentNullException">if assign is null</exception>
                */
                void Set(BigPolyArray ^assign);

                /**
                <summary>Saves the BigPolyArray instance to an output stream.</summary>

                <remarks>
                Saves the BigPolyArray instance to an output stream. The output is in binary format and not human-readable. The
                output stream must have the "binary" flag set.
                </remarks>
                <param name="stream">The stream to save the BigPolyArray to</param>
                <exception cref="System::ArgumentNullException">If stream is null</exception>
                <seealso cref="Load()">See Load() to load a saved BigPolyArray instance.</seealso>
                */
                void Save(System::IO::Stream ^stream);

                /**
                <summary>Loads an BigPolyArray instance from an input stream overwriting the current BigPolyArray
                instance.</summary>
                <param name="stream">The stream to load the BigPolyArray instance from</param>
                <exception cref="System::ArgumentNullException">If stream is null</exception>
                <seealso cref="Save()">See Save() to save an BigPolyArray instance.</seealso>
                */
                void Load(System::IO::Stream ^stream);

                /**
                <summary>Destroys the BigPolyArray, including deallocating any internally allocated space.</summary>
                */
                ~BigPolyArray();

                /**
                <summary>Destroys the BigPolyArray, including deallocating any internally allocated space.</summary>
                */
                !BigPolyArray();

                /**
                <summary>Returns a reference to the underlying C++ BigPolyArray.</summary>
                */
                seal::BigPolyArray &GetArray();

            internal:
                /**
                <summary>Creates a deep copy of a C++ BigPolyArray instance.</summary>
                <param name="value">The BigPolyArray instance to copy from</param>
                <exception cref="System::ArgumentNullException">If copy is null</exception>
                */
                BigPolyArray(const seal::BigPolyArray &value);

                /**
                <summary>Initializes the BigPolyArray to use the specified C++ BigPolyArray.</summary>
                <remarks>
                Initializes the BigPolyArray to use the specified C++ BigPolyArray. This constructor does not copy the C++ BigPolyArray but actually
                uses the specified C++ BigPolyArray as the backing data. Upon destruction, the managed BigPolyArray will not destroy the C++
                BigPolyArray.
                </remarks>
                <param name="value">The BigPolyArray to use as the backing BigPolyArray</param>
                */
                BigPolyArray(seal::BigPolyArray *value);

            private:
                seal::BigPolyArray *array_;

                bool owned_;
            };
        }
    }
}
