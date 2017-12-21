#pragma once

#include <cstdint>
#include <vector>
#include "seal/util/common.h"
#include "seal/util/uintcore.h"
#include "seal/encryptionparams.h"
#include "seal/util/polymodulus.h"
#include "seal/util/smallntt.h"
#include "seal/smallmodulus.h"
#include "seal/plaintext.h"
#include "seal/context.h"

namespace seal
{
    /**
    Provides functionality for CRT batching. If the polynomial modulus is X^N+1, and the 
    plaintext modulus is a prime number T such that T is congruent to 1 modulo 2N, then 
    PolyCRTBuilder allows the SEAL plaintext elements to be viewed as 2-by-(N/2) matrices 
    of integers modulo T. Homomorphic operations performed on such encrypted matrices are 
    applied coefficient (slot) wise, enabling powerful SIMD functionality for computations 
    that are vectorizable. This functionality is often called "batching" in the homomorphic 
    encryption literature.

    @par Mathematical Background
    Mathematically speaking, if poly_modulus is X^N+1, N is a power of two, and plain_modulus
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

    @par Valid Parameters
    Whether batching can be used depends on whether the plaintext modulus has been chosen
    appropriately. Thus, to construct a PolyCRTBuilder the user must provide an instance 
    of SEALContext such that its associated EncryptionParameterQualifiers object has the 
    flags parameters_set and enable_batching set to true.

    @par Overloads
    For the decompose function we provide two overloads concerning the memory pool used in 
    allocations needed during the operation. In one overload the local memory pool of the 
    PolyCRTBuilder (used to store pre-computation results and other member variables) is used 
    for this purpose, and in another overload the user can supply a MemoryPoolHandle to to be 
    used instead. This is to allow one single PolyCRTBuilder to be used concurrently by 
    several threads without running into thread contention in allocations taking place during 
    operations. For example, one can share one single PolyCRTBuilder across any number of 
    threads, but in each thread call the encrypt function by giving it a thread-local 
    MemoryPoolHandle to use. It is important for a developer to understand how this works to 
    avoid unnecessary performance bottlenecks.

    @see EncryptionParameters for more information about encryption parameters.
    @see EncryptionParameterQualifiers for more information about parameter qualifiers.
    @see Evaluator for rotating rows and columns of encrypted matrices.
    */
    class PolyCRTBuilder
    {
    public:
        /**
        Creates a PolyCRTBuilder. It is necessary that the encryption parameters given through
        the SEALContext object support batching. Dynamically allocated member variables are 
        allocated from the memory pool pointed to by the given MemoryPoolHandle. By default 
        the global memory pool is used.

        @param[in] context The SEALContext
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if the encryption parameters are not valid for batching
        @throws std::invalid_argument if pool is uninitialized
        */
        PolyCRTBuilder(const SEALContext &context, 
            const MemoryPoolHandle &pool = MemoryPoolHandle::Global());

        /**
        Creates a deep copy of a given PolyCRTBuilder.

        @param[in] copy The PolyCRTBuilder to copy from
        */
        PolyCRTBuilder(const PolyCRTBuilder &copy);

        /**
        Creates a new PolyCRTBuilder by moving a given one.

        @param[in] source The PolyCRTBuilder to move from
        */
        PolyCRTBuilder(PolyCRTBuilder &&source) = default;

        /**
        Creates a SEAL plaintext from a given matrix. This function "batches" a given matrix
        of integers modulo the plaintext modulus into a SEAL plaintext element, and stores 
        the result in the destination parameter. The input vector must have size at most equal
        to the degree of the polynomial modulus. The first half of the elements represent the
        first row of the matrix, and the second half represent the second row. The numbers
        in the matrix can be at most equal to the plaintext modulus for it to represent 
        a valid SEAL plaintext.

        @param[in] values The matrix of integers modulo plaintext modulus to batch
        @param[out] destination The plaintext polynomial to overwrite with the result
        @throws std::invalid_argument if values is too large
        */
        void compose(const std::vector<std::uint64_t> &values, Plaintext &destination);

        /**
        Creates a SEAL plaintext from a given matrix. This function "batches" a given matrix
        of integers modulo the plaintext modulus into a SEAL plaintext element, and stores
        the result in the destination parameter. The input vector must have size at most equal
        to the degree of the polynomial modulus. The first half of the elements represent the
        first row of the matrix, and the second half represent the second row. The numbers
        in the matrix can be at most equal to the plaintext modulus for it to represent
        a valid SEAL plaintext.

        @param[in] values The matrix of integers modulo plaintext modulus to batch
        @param[out] destination The plaintext polynomial to overwrite with the result
        @throws std::invalid_argument if values is too large
        */
        void compose(const std::vector<std::int64_t> &values, Plaintext &destination);
        
        /**
        Creates a SEAL plaintext from a given matrix. This function "batches" a given matrix
        of integers modulo the plaintext modulus in-place into a SEAL plaintext ready to be
        encrypted. The matrix is given as a plaintext element whose first N/2 coefficients
        represent the first row of the matrix, and the second N/2 coefficients represent the
        second row, where N denotes the degree of the polynomial modulus. The input plaintext
        must have degress less than the polynomial modulus, and coefficients less than the 
        plaintext modulus, i.e. it must be a valid plaintext for the encryption parameters. 
        Dynamic memory allocations in the process are allocated from the memory pool pointed 
        to by the given MemoryPoolHandle.

        @param[in] plain The matrix of integers modulo plaintext modulus to batch
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if plain is not valid for the encryption parameters
        @throws std::invalid_argument if pool is uninitialized
        */
        void compose(Plaintext &plain, const MemoryPoolHandle &pool);

        /**
        Creates a SEAL plaintext from a given matrix. This function "batches" a given matrix
        of integers modulo the plaintext modulus in-place into a SEAL plaintext ready to be
        encrypted. The matrix is given as a plaintext element whose first N/2 coefficients
        represent the first row of the matrix, and the second N/2 coefficients represent the
        second row, where N denotes the degree of the polynomial modulus. The input plaintext
        must have degress less than the polynomial modulus, and coefficients less than the
        plaintext modulus, i.e. it must be a valid plaintext for the encryption parameters.
        Dynamic memory allocations in the process are allocated from the memory pool pointed
        to by the local MemoryPoolHandle.

        @param[in] plain The matrix of integers modulo plaintext modulus to batch
        @throws std::invalid_argument if plain is not valid for the encryption parameters
        */
        void compose(Plaintext &plain)
        {
            compose(plain, pool_);
        }

        /**
        Inverse of compose. This function "unbatches" a given SEAL plaintext into a matrix
        of integers modulo the plaintext modulus, and stores the result in the destination 
        parameter. The input plaintext must have degress less than the polynomial modulus, 
        and coefficients less than the plaintext modulus, i.e. it must be a valid plaintext
        for the encryption parameters. Dynamic memory allocations in the process are 
        allocated from the memory pool pointed to by the given MemoryPoolHandle.

        @param[in] plain The plaintext polynomial to unbatch
        @param[out] destination The vector to be overwritten with the values of the slots
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if plain is not valid for the encryption parameters
        @throws std::invalid_argument if pool is uninitialized
        */
        void decompose(const Plaintext &plain, std::vector<std::uint64_t> &destination, 
            const MemoryPoolHandle &pool);

        /**
        Inverse of compose. This function "unbatches" a given SEAL plaintext into a matrix
        of integers modulo the plaintext modulus, and stores the result in the destination
        parameter. The input plaintext must have degress less than the polynomial modulus,
        and coefficients less than the plaintext modulus, i.e. it must be a valid plaintext
        for the encryption parameters. Dynamic memory allocations in the process are
        allocated from the memory pool pointed to by the local MemoryPoolHandle.

        @param[in] plain The plaintext polynomial to unbatch
        @param[out] destination The vector to be overwritten with the values of the slots
        @throws std::invalid_argument if plain is not valid for the encryption parameters
        */
        inline void decompose(const Plaintext &plain, std::vector<std::uint64_t> &destination)
        {
            decompose(plain, destination, pool_);
        }

        /**
        Inverse of compose. This function "unbatches" a given SEAL plaintext into a matrix
        of integers modulo the plaintext modulus, and stores the result in the destination
        parameter. The input plaintext must have degress less than the polynomial modulus,
        and coefficients less than the plaintext modulus, i.e. it must be a valid plaintext
        for the encryption parameters. Dynamic memory allocations in the process are
        allocated from the memory pool pointed to by the given MemoryPoolHandle.

        @param[in] plain The plaintext polynomial to unbatch
        @param[out] destination The vector to be overwritten with the values of the slots
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if plain is not valid for the encryption parameters
        @throws std::invalid_argument if pool is uninitialized
        */
        void decompose(const Plaintext &plain, std::vector<std::int64_t> &destination,
            const MemoryPoolHandle &pool);

        /**
        Inverse of compose. This function "unbatches" a given SEAL plaintext into a matrix
        of integers modulo the plaintext modulus, and stores the result in the destination
        parameter. The input plaintext must have degress less than the polynomial modulus,
        and coefficients less than the plaintext modulus, i.e. it must be a valid plaintext
        for the encryption parameters. Dynamic memory allocations in the process are
        allocated from the memory pool pointed to by the local MemoryPoolHandle.

        @param[in] plain The plaintext polynomial to unbatch
        @param[out] destination The vector to be overwritten with the values of the slots
        @throws std::invalid_argument if plain is not valid for the encryption parameters
        */
        inline void decompose(const Plaintext &plain, std::vector<std::int64_t> &destination)
        {
            decompose(plain, destination, pool_);
        }

        /**
        Inverse of compose. This function "unbatches" a given SEAL plaintext in-place into 
        a matrix of integers modulo the plaintext modulus. The input plaintext must have 
        degress less than the polynomial modulus, and coefficients less than the plaintext 
        modulus, i.e. it must be a valid plaintext for the encryption parameters. Dynamic 
        memory allocations in the process are allocated from the memory pool pointed to by 
        the given MemoryPoolHandle.

        @param[in] plain The plaintext polynomial to unbatch
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if plain is not valid for the encryption parameters
        @throws std::invalid_argument if pool is uninitialized
        */
        void decompose(Plaintext &plain, const MemoryPoolHandle &pool);

        /**
        Inverse of compose. This function "unbatches" a given SEAL plaintext in-place into
        a matrix of integers modulo the plaintext modulus. The input plaintext must have
        degress less than the polynomial modulus, and coefficients less than the plaintext
        modulus, i.e. it must be a valid plaintext for the encryption parameters. Dynamic
        memory allocations in the process are allocated from the memory pool pointed to by
        the local MemoryPoolHandle.

        @param[in] plain The plaintext polynomial to unbatch
        @throws std::invalid_argument if plain is not valid for the encryption parameters
        */
        void decompose(Plaintext &plain)
        {
            decompose(plain, pool_);
        }

        /**
        Returns the number of slots.
        */
        inline int slot_count() const
        {
            return slots_;
        }

    private:
        PolyCRTBuilder &operator =(const PolyCRTBuilder &assign) = delete;

        PolyCRTBuilder &operator =(PolyCRTBuilder &&assign) = delete;

        void populate_roots_of_unity_vector();

        void populate_matrix_reps_index_map();

        inline void reverse_bits(std::uint64_t *input)
        {
#ifdef SEAL_DEBUG
            if (input == nullptr)
            {
                throw std::invalid_argument("input cannot be null");
            }
#endif
            std::uint32_t n = parms_.poly_modulus().coeff_count() - 1;
            int logn = util::get_power_of_two(n);
            for (std::uint32_t i = 0; i < n; i++)
            {
                std::uint32_t reversed_i = util::reverse_bits(i, logn);
                if (i < reversed_i)
                {
                    std::swap(input[i], input[reversed_i]);
                }
            }
        }

        MemoryPoolHandle pool_;

        EncryptionParameters parms_;

        util::SmallNTTTables ntt_tables_;

        SmallModulus mod_;

        util::PolyModulus polymod_;

        int slots_;

        util::Pointer roots_of_unity_;

        EncryptionParameterQualifiers qualifiers_;

        std::vector<std::uint64_t> matrix_reps_index_map_;
    };
}