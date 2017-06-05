#pragma once

#include "evaluationkeys.h"
#include "EncryptionParamsWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            /**
            <summary>Class to store evaluation keys.</summary>
            
            <remarks>
            <para>
            Class to store evaluation keys. Each instance stores a list of objects of type System::Tuple<BigPolyArray^, BigPolyArray^>, 
            where the evaluation keys are stored. Each entry of the list is called an evaluation key. The number of evaluation keys to 
            store can be specified in the constructor of EvaluationKeys. Each entry of the list is the key associated with a particular
            power of the secret key, and can be repeatedly used by the relinearization operation typically performed either automatically 
            or manually by the user after homomorphic multiplication. 
            </para>

            <para>
            A reference to the evaluation keys vector can be obtained using <see cref="Keys" />. The evaluation keys can be cleared 
            (and the vector resized to 0) using <see cref="Clear()"/>. The evaluation keys can be saved and loaded from a stream with 
            the <see cref="Save()" /> and <see cref="Load()" /> functions.
            </para>

            <para>
            This class does not perform any sanity checks for the evaluation keys that it stores.
            </para>

            <para>
            In general, reading from EvaluationKeys is thread-safe as long as no other thread is concurrently mutating it.
            This is due to the underlying data structure storing the evaluation keys not being thread-safe.
            </para>
            </remarks>
            */
            public ref class EvaluationKeys
            {
            public:
                /**
                <summary>Creates an empty set of evaluation keys.</summary>
                */
                EvaluationKeys();

                /**
                <summary>Creates an EvaluationKeys instance initialized with a given list of evaluation keys.</summary>
                <param name="keys">The keys</param>
                <exception cref="System::ArgumentNullException">if keys or any of its elements is null</exception>
                */
                EvaluationKeys(System::Collections::Generic::List< System::Tuple<BigPolyArray^, BigPolyArray^> ^> ^keys);

                /**
                <summary>Returns the current number of evaluation keys.</summary>
                */
                property int Size {
                    int get();
                }

                /**
                <summary>Returns a reference to the evaluation key stored at the given index.</summary>
                <exception cref="System::ArgumentOutOfRangeException">if the given index is not within [0, Size)</exception>
                */
                property System::Tuple<BigPolyArray^, BigPolyArray^> ^default[int]{
                    System::Tuple<BigPolyArray^, BigPolyArray^> ^get(int index);
                }

                /**
                <summary>Returns a reference to the list of evaluation keys.</summary>
                */
                property System::Collections::Generic::List<System::Tuple<BigPolyArray^, BigPolyArray^> ^> ^Keys {
                    System::Collections::Generic::List<System::Tuple<BigPolyArray^, BigPolyArray^> ^> ^get();
                }

                /**
                <summary>Saves the EvaluationKeys instance to an output stream.</summary>

                <remarks>
                Saves the EvaluationKeys instance to an output stream. The output is in binary format and not human-readable. The
                output stream must have the "binary" flag set.
                </remarks>
                <param name="stream">The stream to save the EvaluationKeys to</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                <seealso cref="Load()">See <see cref="Load()" /> to load a saved EvaluationKeys instance.</seealso>
                */
                void Save(System::IO::Stream ^stream);

                /**
                <summary>Loads an EvaluationKeys instance from an input stream overwriting the current EvaluationKeys
                instance.</summary>
                <param name="stream">The stream to load the EvaluationKeys instance from</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                <seealso cref="Save()">See Save() to save an EvaluationKeys instance.</seealso>
                */
                void Load(System::IO::Stream ^stream);

                /**
                <summary>Destroys the EvaluationKeys instance.</summary>
                */
                ~EvaluationKeys();

                /**
                <summary>Destroys the EvaluationKeys instance.</summary>
                */
                !EvaluationKeys();

            internal:
                /**
                <summary>Creates a deep copy of a C++ EvaluationKeys instance.</summary>
                <param name="value">The EvaluationKeys instance to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                EvaluationKeys(const seal::EvaluationKeys &value);

                // get the underlying c++ keys
                seal::EvaluationKeys &GetKeys();

            private:
                seal::EvaluationKeys *keys_;
            };
        }
    }
}