#pragma once

#include "seal/polycrt.h"
#include "sealnet/EncryptionParamsWrapper.h"
#include "sealnet/ContextWrapper.h"
#include "sealnet/PlaintextWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            /**
            <summary>Provides functionality for CRT batching.</summary>
            
            <remarks>
            Provides functionality for CRT batching. If the polynomial modulus is X^N+1, and the
            plaintext modulus is a prime number T such that T is congruent to 1 modulo 2N, then
            PolyCRTBuilder allows the SEAL plaintext elements to be viewed as 2-by-(N/2) matrices
            of integers modulo T. Homomorphic operations performed on such encrypted matrices are
            applied coefficient (slot) wise, enabling powerful SIMD functionality for computations
            that are vectorizable. This functionality is often called "batching" in the homomorphic
            encryption literature.
            </para>
            <para>
            Mathematically speaking, if PolyModulus is X^N+1, N is a power of two, and PlainModulus
            is a prime number T such that 2N divides T-1, then integers modulo T contain a primitive
            2N-th root of unity and the polynomial X^N+1 splits into n distinct linear factors as
            X^N+1 = (X-a_1)*...*(X-a_N) mod T, where the constants a_1, ..., a_n are all the distinct
            primitive 2N-th roots of unity in integers modulo T. The Chinese Remainder Theorem (CRT)
            states that the plaintext space Z_T[X]/(X^N+1) in this case is isomorphic (as an algebra)
            to the N-fold direct product of fields Z_T. The isomorphism is easy to compute explicitly
            in both directions, which is what this class does. Furthermore, the Galois group of the
            extension is (Z/2NZ)* ~= Z/2Z x Z/(N/2) whose action on the primitive roots of unity is
            easy to describe. Since the batching slots correspond 1-to-1 to the primitive roots of
            unity, applying Galois automorphisms on the plaintext act by permuting the slots. By
            applying generators of the two cyclic subgroups of the Galois group, we can effectively
            view the plaintext as a 2-by-(N/2) matrix, and enable cyclic row rotations, and column
            rotations (row swaps).
            </para>
            <para>
            Whether batching can be used depends on whether the plaintext modulus has been chosen
            appropriately. Thus, to construct a PolyCRTBuilder the user must provide an instance
            of <see cref="SEALContext" /> such that its associated EncryptionParameterQualifiers
            object has the flags ParametersSet and EnableBatching set to true.
            </para>
            <para>
            For the decompose function we provide two overloads concerning the memory pool used in
            allocations needed during the operation. In one overload the local memory pool of the
            PolyCRTBuilder (used to store pre-computation results and other member variables) is used
            for this purpose, and in another overload the user can supply a <see cref="MemoryPoolHandle" />
            to to be used instead. This is to allow one single PolyCRTBuilder to be used concurrently 
            by several threads without running into thread contention in allocations taking place 
            during operations. For example, one can share one single PolyCRTBuilder across any number 
            of threads, but in each thread call the encrypt function by giving it a thread-local
            <see cref="MemoryPoolHandle" /> to use. It is important for a developer to understand how 
            this works to avoid unnecessary performance bottlenecks.
            </remarks>
            <seealso cref="EncryptionParameters"/>See EncryptionParameters for more information about
            encryption parameters.</seealso>
            <seealso cref="EncryptionParameterQualifiers"/>See EncryptionParameterQualifiers for more
            information about parameter qualifiers.</seealso>
            <seealso cref="Evaluator"/>See Evaluator for rotating rows and columns of encrypted 
            matrices.</seealso>
            */
            public ref class PolyCRTBuilder
            {
            public:
                /**
                <summary>Creates a PolyCRTBuilder.</summary>

                <remarks>
                Creates a PolyCRTBuilder. It is necessary that the encryption parameters given through
                the <see cref="SEALContext" /> object support batching. Dynamically allocated member 
                variables are allocated from the global memory pool.
                </remarks>
                <param name="context">The SEALContext</param>
                <exception cref="System::ArgumentException">if the encryption parameters are not valid 
                for batching</exception>
                <exception cref="System::ArgumentNullException">if context is null</exception>
                */
                PolyCRTBuilder(SEALContext ^context);

                /**
                <summary>Creates a PolyCRTBuilder.</summary>

                <remarks>
                Creates a PolyCRTBuilder. It is necessary that the encryption parameters given through
                the <see cref="SEALContext" /> object support batching. Dynamically allocated member 
                variables are allocated from the memory pool pointed to by the given 
                <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="context">The SEALContext</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if the encryption parameters are not valid 
                for batching</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if context or pool is null</exception>
                */
                PolyCRTBuilder(SEALContext ^context, MemoryPoolHandle ^pool);

                /**
                <summary>Creates a deep copy of a given PolyCRTBuilder.</summary>

                <param name="copy">The PolyCRTBuilder to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                PolyCRTBuilder(PolyCRTBuilder ^copy);

                /**
                <summary>Creates a SEAL plaintext from a given matrix.</summary>

                <remarks>
                Creates a SEAL plaintext from a given matrix. This function "batches" a given matrix
                of integers modulo the plaintext modulus into a SEAL plaintext element, and stores
                the result in the destination parameter. The input System::List must have size at most 
                equal to the degree of the polynomial modulus. The first half of the elements represent
                the first row of the matrix, and the second half represent the second row. The numbers
                in the matrix can be at most equal to the plaintext modulus for it to represent
                a valid SEAL plaintext.
                </remarks>
                <param name="values">The matrix of integers modulo plaintext modulus to batch</param>
                <param name="destination">The plaintext polynomial to overwrite with the result</param>
                <exception cref="System::ArgumentException">if values is too large</exception>
                <exception cref="System::ArgumentNullException">if values or destination is 
                null</exception>
                */
                void Compose(System::Collections::Generic::List<System::UInt64> ^values, 
                    Plaintext ^destination);

                /**
                <summary>Creates a SEAL plaintext from a given matrix.</summary>

                <remarks>
                Creates a SEAL plaintext from a given matrix. This function "batches" a given matrix
                of integers modulo the plaintext modulus into a SEAL plaintext element, and stores
                the result in the destination parameter. The input System::List must have size at most
                equal to the degree of the polynomial modulus. The first half of the elements represent
                the first row of the matrix, and the second half represent the second row. The numbers
                in the matrix can be at most equal to the plaintext modulus for it to represent
                a valid SEAL plaintext.
                </remarks>
                <param name="values">The matrix of integers modulo plaintext modulus to batch</param>
                <param name="destination">The plaintext polynomial to overwrite with the result</param>
                <exception cref="System::ArgumentException">if values is too large</exception>
                <exception cref="System::ArgumentNullException">if values or destination is
                null</exception>
                */
                void Compose(System::Collections::Generic::List<System::Int64> ^values,
                    Plaintext ^destination);

                /**
                <summary>Creates a SEAL plaintext from a given matrix.</summary>

                <remarks>
                Creates a SEAL plaintext from a given matrix. This function "batches" a given matrix
                of integers modulo the plaintext modulus in-place into a SEAL plaintext ready to be
                encrypted. The matrix is given as a plaintext element whose first N/2 coefficients
                represent the first row of the matrix, and the second N/2 coefficients represent the
                second row, where N denotes the degree of the polynomial modulus. The input plaintext
                must have degress less than the polynomial modulus, and coefficients less than the
                plaintext modulus, i.e. it must be a valid plaintext for the encryption parameters.
                Dynamic memory allocations in the process are allocated from the memory pool pointed
                to by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="plain">The matrix of integers modulo plaintext modulus to batch</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if plain is not valid for the encryption
                parameters</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if plain or pool is null</exception>
                */
                void Compose(Plaintext ^plain, MemoryPoolHandle ^pool);

                /**
                <summary>Creates a SEAL plaintext from a given matrix.</summary>

                <remarks>
                Creates a SEAL plaintext from a given matrix. This function "batches" a given matrix
                of integers modulo the plaintext modulus in-place into a SEAL plaintext ready to be
                encrypted. The matrix is given as a plaintext element whose first N/2 coefficients
                represent the first row of the matrix, and the second N/2 coefficients represent the
                second row, where N denotes the degree of the polynomial modulus. The input plaintext
                must have degress less than the polynomial modulus, and coefficients less than the
                plaintext modulus, i.e. it must be a valid plaintext for the encryption parameters.
                Dynamic memory allocations in the process are allocated from the memory pool pointed
                to by the local <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="plain">The matrix of integers modulo plaintext modulus to batch</param>
                <exception cref="System::ArgumentException">if plain is not valid for the encryption
                parameters</exception>
                <exception cref="System::ArgumentNullException">if plain is null</exception>
                */
                void Compose(Plaintext ^plain);

                /**
                <summary>Inverse of compose.</summary>

                <remarks>
                Inverse of compose. This function "unbatches" a given SEAL plaintext into a matrix
                of integers modulo the plaintext modulus, and stores the result in the destination
                parameter. The input plaintext must have degress less than the polynomial modulus,
                and coefficients less than the plaintext modulus, i.e. it must be a valid plaintext
                for the encryption parameters. Dynamic memory allocations in the process are 
                allocated from the global memory pool.
                </remarks>
                <param name="plain">The plaintext polynomial to unbatch</param>
                <param name="destination">The vector to be overwritten with the values of the 
                slots</param>
                <exception cref="System::ArgumentException">if plain is not valid for the encryption
                parameters</exception>
                <exception cref="System::ArgumentNullException">if values or destination is 
                null</exception>
                */
                void Decompose(Plaintext ^plain, 
                    System::Collections::Generic::List<System::UInt64> ^destination);

                /**
                <summary>Inverse of compose.</summary>

                <remarks>
                Inverse of compose. This function "unbatches" a given SEAL plaintext into a matrix
                of integers modulo the plaintext modulus, and stores the result in the destination
                parameter. The input plaintext must have degress less than the polynomial modulus,
                and coefficients less than the plaintext modulus, i.e. it must be a valid plaintext
                for the encryption parameters. Dynamic memory allocations in the process are
                allocated from the global memory pool.
                </remarks>
                <param name="plain">The plaintext polynomial to unbatch</param>
                <param name="destination">The vector to be overwritten with the values of the
                slots</param>
                <exception cref="System::ArgumentException">if plain is not valid for the encryption
                parameters</exception>
                <exception cref="System::ArgumentNullException">if values or destination is
                null</exception>
                */
                void Decompose(Plaintext ^plain,
                    System::Collections::Generic::List<System::Int64> ^destination);

                /**
                <summary>Inverse of compose.</summary>

                <remarks>
                Inverse of compose. This function "unbatches" a given SEAL plaintext into a matrix
                of integers modulo the plaintext modulus, and stores the result in the destination
                parameter. The input plaintext must have degress less than the polynomial modulus,
                and coefficients less than the plaintext modulus, i.e. it must be a valid plaintext
                for the encryption parameters. Dynamic memory allocations in the process are allocated 
                from the memory pool pointed to by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="plain">The plaintext polynomial to unbatch</param>
                <param name="destination">The vector to be overwritten with the values of the
                slots</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if plain is not valid for the encryption
                parameters</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if values. destination, or pool is
                null</exception>
                */
                void Decompose(Plaintext ^plain, 
                    System::Collections::Generic::List<System::UInt64> ^destination,
                    MemoryPoolHandle ^pool);

                /**
                <summary>Inverse of compose.</summary>

                <remarks>
                Inverse of compose. This function "unbatches" a given SEAL plaintext into a matrix
                of integers modulo the plaintext modulus, and stores the result in the destination
                parameter. The input plaintext must have degress less than the polynomial modulus,
                and coefficients less than the plaintext modulus, i.e. it must be a valid plaintext
                for the encryption parameters. Dynamic memory allocations in the process are allocated
                from the memory pool pointed to by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="plain">The plaintext polynomial to unbatch</param>
                <param name="destination">The vector to be overwritten with the values of the
                slots</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if plain is not valid for the encryption
                parameters</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if values. destination, or pool is
                null</exception>
                */
                void Decompose(Plaintext ^plain,
                    System::Collections::Generic::List<System::Int64> ^destination,
                    MemoryPoolHandle ^pool);

                /**
                <summary>Inverse of compose.</summary>

                <remarks>
                Inverse of compose. This function "unbatches" a given SEAL plaintext in-place into
                a matrix of integers modulo the plaintext modulus. The input plaintext must have
                degress less than the polynomial modulus, and coefficients less than the plaintext
                modulus, i.e. it must be a valid plaintext for the encryption parameters. Dynamic
                memory allocations in the process are allocated from the memory pool pointed to by
                the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="plain">The matrix of integers modulo plaintext modulus to batch</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if plain is not valid for the encryption
                parameters</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if plain or pool is null</exception>
                */
                void Decompose(Plaintext ^plain, MemoryPoolHandle ^pool);

                /**
                <summary>Inverse of compose.</summary>

                <remarks>
                Inverse of compose. This function "unbatches" a given SEAL plaintext in-place into
                a matrix of integers modulo the plaintext modulus. The input plaintext must have
                degress less than the polynomial modulus, and coefficients less than the plaintext
                modulus, i.e. it must be a valid plaintext for the encryption parameters. Dynamic
                memory allocations in the process are allocated from the memory pool pointed to by
                the local <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="plain">The matrix of integers modulo plaintext modulus to batch</param>
                <exception cref="System::ArgumentException">if plain is not valid for the encryption
                parameters</exception>
                <exception cref="System::ArgumentNullException">if plain is null</exception>
                */
                void Decompose(Plaintext ^plain);

                /**
                <summary>Returns the number of slots.</summary>
                */
                property int SlotCount
                {
                    int get();
                }

                /**
                <summary>Destroys the PolyCRTBuilder</summary>
                */
                ~PolyCRTBuilder();

                /**
                <summary>Destroys the PolyCRTBuilder</summary>
                */
                !PolyCRTBuilder();

            private:
                /**
                <summary>Returns a reference to the underlying C++ PolyCRTBuilder.</summary>
                */
                seal::PolyCRTBuilder &GetPolyCRTBuilder();

                seal::PolyCRTBuilder *polyCRTBuilder_;
            };
        }
    }
}
