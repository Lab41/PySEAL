#pragma once

#include "seal/ciphertext.h"
#include "sealnet/MemoryPoolHandleWrapper.h"
#include "sealnet/EncryptionParamsWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class Plaintext;

            ref class Evaluator;

            ref class MemoryPoolHandle;

            /**
            <summary>Class to store a ciphertext element.</summary>

            <remarks>
            <para>
            Class to store a ciphertext element. The data for a ciphertext consists of two or more polynomials,
            which are in SEAL stored in a CRT form with respect to the factors of the coefficient modulus. This
            data itself is not meant to be modified directly by the user, but is instead operated on by functions
            in the <see cref="Evaluator" /> class. The size of the backing array of a ciphertext depends on the
            encryption parameters and the size of the ciphertext (at least 2). If the degree of the PolyModulus 
            encryption parameter is N, and the number of primes in the CoeffModulus encryption parameter is K,
            then the ciphertext backing array requires precisely 8*N*K*size bytes of memory. A ciphertext also 
            carries with it the hash of its associated encryption parameters, which is used to check the validity 
            of the ciphertext for homomorphic operations and decryption.
            </para>
            <para>
            SEAL allocates the memory for a ciphertext by default from the global memory pool, but a user can
            alternatively specify a different memory pool to be used through the <see cref="MemoryPoolHandle" />
            class. This can be important, as constructing or resizing several ciphertexts at once allocated in the 
            global memory pool can quickly lead to contention and poor performance in multi-threaded applications.
            In addition to its size which denotes the number of polynomials, a ciphertext also has a capacity which 
            denotes the number of polynomials that fit in the current allocation. In high-performance applications 
            unnecessary re-allocations should also be avoided by reserving enough memory for the ciphertext to 
            begin with either by providing the capacity to the constructor as an extra argument, or by calling the 
            <see cref="Reserve(EncryptionParameters^, int, MemoryPoolHandle^)" /> function at any time.
            </para>
            <para>
            In general, reading from ciphertext is thread-safe as long as no other thread is concurrently
            mutating it. This is due to the underlying data structure storing the ciphertext not being
            thread-safe.
            </para>
            </remarks>
            <seealso cref="Plaintext">See Plaintext for the class that stores plaintexts.</seealso>
            */
            public ref class Ciphertext
            {
            public:
                /**
                <summary>Constructs an empty ciphertext allocating no memory.</summary>
                */
                Ciphertext();

                /**
                <summary>Constructs an empty ciphertext allocating no memory.</summary>

                <remarks>
                Constructs an empty ciphertext allocating no memory. The memory pool is set to the pool pointed 
                to by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if pool is null</exception>
                */
                Ciphertext(MemoryPoolHandle ^pool);

                /**
                <summary>Constructs an empty ciphertext with capacity 2.</summary>
                
                <remarks>
                Constructs an empty ciphertext with capacity 2. In addition to the capacity, the allocation 
                size is determined by the given encryption parameters. The memory pool is set to the pool 
                pointed to by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="parms">Encryption parameters determining the size characteristics of the ciphertext</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if parms or pool is null</exception>
                */
                Ciphertext(EncryptionParameters ^parms, MemoryPoolHandle ^pool);

                /**
                <summary>Constructs an empty ciphertext with capacity 2.</summary>

                <remarks>
                Constructs an empty ciphertext with capacity 2. In addition to the capacity, the allocation 
                size is determined by the given encryption parameters. The memory pool is set to the global 
                memory pool.
                </remarks>
                <param name="parms">Encryption parameters determining the size characteristics of the ciphertext</param>
                <exception cref="System::ArgumentNullException">if parms is null</exception>
                */
                Ciphertext(EncryptionParameters ^parms);

                /**
                <summary>Constructs an empty ciphertext with given capacity.</summary>

                <remarks>
                Constructs an empty ciphertext with given capacity. In addition to the capacity, the allocation 
                size is determined by the given encryption parameters. The memory pool is set to the pool pointed 
                to by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="parms">Encryption parameters determining the size characteristics of the ciphertext</param>
                <param name="sizeCapacity">The capacity</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if sizeCapacity is less than 2</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if parms or pool is null</exception>
                */
                Ciphertext(EncryptionParameters ^parms, int sizeCapacity, MemoryPoolHandle ^pool);

                /**
                <summary>Constructs an empty ciphertext with given capacity.</summary>

                <remarks>
                Constructs an empty ciphertext with given capacity. In addition to the capacity, the allocation 
                size is determined by the given encryption parameters. The memory pool is set to the global
                memory pool.
                </remarks>
                <param name="parms">Encryption parameters determining the size characteristics of the ciphertext</param>
                <param name="sizeCapacity">The capacity</param>
                <exception cref="System::ArgumentException">if sizeCapacity is less than 2</exception>
                <exception cref="System::ArgumentNullException">if parms is null</exception>
                */
                Ciphertext(EncryptionParameters ^parms, int sizeCapacity);

                /**
                <summary>Constructs a new ciphertext by copying a given one.</summary>

                <remarks>
                Constructs a new ciphertext by copying a given one. If copy carries an initialized memory pool,
                the allocation for the new ciphertext is made from that pool. Otherwise the global memory pool 
                is used instead.
                </remarks>
                <param name="copy">The ciphertext to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                Ciphertext(Ciphertext ^copy);

                /**
                <summar>Allocates enough memory to accommodate the backing array of a ciphertext with given
                capacity.</summary>
                
                <remarks>
                Allocates enough memory to accommodate the backing array of a ciphertext with given
                capacity. In addition to the capacity, the allocation size is determined by the given
                encryption parameters, and is made from the memory pool pointed to by the given
                <see cref="MemoryPoolHandle" />.
                </remarks>

                <param name="parms">Encryption parameters determining the size characteristics of the ciphertext</param>
                <param name="sizeCapacity">The capacity</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref=System::ArgumentException">if sizeCapacity is less than 2</exception>
                <exception cref=System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if parms or pool is null</exception>
                */
                void Reserve(EncryptionParameters ^parms, int sizeCapacity, MemoryPoolHandle ^pool);
                
                /**
                <summar>Allocates enough memory to accommodate the backing array of a ciphertext with given
                capacity.</summary>

                <remarks>
                Allocates enough memory to accommodate the backing array of a ciphertext with given
                capacity. In addition to the capacity, the allocation size is determined by the given
                encryption parameters, and is made from the memory pool pointed to by the currently held
                <see cref="MemoryPoolHandle" />. If the currently held <see cref="MemoryPoolHandle" />
                is uninitialized, it is set to point to the global memory pool instead, and the memory is 
                allocated from the global memory pool.
                </remarks>

                <param name="parms">Encryption parameters determining the size characteristics of the ciphertext</param>
                <param name="sizeCapacity">The capacity</param>
                <exception cref=System::ArgumentException">if sizeCapacity is less than 2</exception>
                <exception cref="System::ArgumentNullException">if parms is null</exception>
                */
                void Reserve(EncryptionParameters ^parms, int sizeCapacity);

                /**
                <summar>Allocates enough memory to accommodate the backing array of a ciphertext with given
                capacity.</summary>

                <remarks>
                Allocates enough memory to accommodate the backing array of a ciphertext with given
                capacity. In addition to the capacity, the allocation size is determined by the current
                encryption parameters, and is made from the memory pool pointed to by the given
                <see cref="MemoryPoolHandle" />.
                </remarks>

                <param name="sizeCapacity">The capacity</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref=System::ArgumentException">if sizeCapacity is less than 2</exception>
                <exception cref=System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if pool is null</exception>
                */
                void Reserve(int sizeCapacity, MemoryPoolHandle ^pool);
                
                /**
                <summar>Allocates enough memory to accommodate the backing array of a ciphertext with given
                capacity.</summary>

                <remarks>
                Allocates enough memory to accommodate the backing array of a ciphertext with given
                capacity. In addition to the capacity, the allocation size is determined by the current
                encryption parameters, and is made from the memory pool pointed to by the currently held
                <see cref="MemoryPoolHandle" />. If the currently held <see cref="MemoryPoolHandle" />
                is uninitialized, it is set to point to the global memory pool instead, and the memory is
                allocated from the global memory pool.
                </remarks>

                <param name="sizeCapacity">The capacity</param>
                <exception cref=System::ArgumentException">if sizeCapacity is less than 2</exception>
                */
                void Reserve(int sizeCapacity);

                /**
                <summary>Resets the ciphertext.</summary>
                
                <remarks>
                Resets the ciphertext. This function releases any memory allocated by the ciphertext, 
                returning it to the memory pool pointed to by the current <see cref="MemoryPoolHandle" />.
                It also sets all encryption parameter specific size information to zero.
                </remarks>
                */
                void Release();

                /**
                <summary>Copies a given ciphertext to the current one.</summary>

                <param name="assign">The ciphertext to copy from</param>
                <exception cref="System::ArgumentNullException">if assign is null</exception>
                */
                void Set(Ciphertext ^assign);

                /**
                Returns the polynomial coefficient at a particular index in the ciphertext data. If the 
                polynomial modulus has degree N, and the number of primes in the coefficient modulus is K, then 
                the ciphertext contains size*N*K coefficients. Thus, the index has a range of [0, size*N*K).

                <param name="coeffIndex">The index of the coefficient</param>
                <exception cref="System::ArgumentOutOfRangeException">if coeffIndex is less than 0 or too large</exception>
                */
                property System::UInt64 default[int]{
                    System::UInt64 get(int coeffIndex);                
                }

                /**
                <summary>Returns the number of primes in the coefficient modulus of the associated encryption
                parameters.</summary>
                
                <remarks>
                Returns the number of primes in the coefficient modulus of the associated encryption
                parameters. This directly affects the allocation size of the ciphertext.
                </remarks>
                */
                property int CoeffModCount {
                    int get();
                }

                /**
                <summary>Returns the coefficient count of the polynomial modulus of the associated encryption
                parameters.</summary>

                <remarks>
                Returns the coefficient count of the polynomial modulus of the associated encryption parameters. 
                This directly affects the allocation size of the ciphertext.
                </remarks>
                */
                property int PolyCoeffCount {
                    int get();
                }

                /**
                <summary>Returns the capacity of the allocation.</summary>

                <remarks>
                Returns the capacity of the allocation. This means the largest size of the ciphertext
                that can be stored in the current allocation with the current encryption parameters.
                </remarks>
                */
                property int SizeCapacity {
                    int get();
                }

                /**
                <summary>Returns the size of the ciphertext.</summary>
                */
                property int Size {
                    int get();
                }

                /**
                <summary>Returns the total size of the current allocation in 64-bit words.</summary>
                */
                property int UInt64CountCapacity {
                    int get();
                }

                /**
                <summary>Returns the total size of the current ciphertext in 64-bit words.</summary>
                */
                property int UInt64Count {
                    int get();
                }

                /**
                <summary>Saves the ciphertext to an output stream.</summary>
                <remarks>
                Saves the ciphertext to an output stream. The output is in binary format and not human-readable.
                The output stream must have the "binary" flag set.
                </remarks>

                <param name="stream">The stream to save the ciphertext to</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                <seealso cref="Load()">See Load() to load a saved ciphertext.</seealso>
                */
                void Save(System::IO::Stream ^stream);

                /**
                <summary>Loads a ciphertext from an input stream overwriting the current ciphertext.</summary>

                <param name="stream">The stream to load the ciphertext from</param>
                <exception cref="System::ArgumentNullException">if stream is null</exception>
                <seealso cref="Save()">See Save() to save a ciphertext.</seealso>
                */
                void Load(System::IO::Stream ^stream);

                /**
                <summary>Returns a copy of the hash block.</summary>
                */
                property System::Tuple<System::UInt64, System::UInt64, System::UInt64, System::UInt64> ^HashBlock {
                    System::Tuple<System::UInt64, System::UInt64, System::UInt64, System::UInt64> ^get();
                }

                /**
                <summary>Destroys the ciphertext.</summary>
                */
                ~Ciphertext();

                /**
                <summary>Destroys the ciphertext.</summary>
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
