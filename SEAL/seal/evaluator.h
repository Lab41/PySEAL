#pragma once

#include <vector>
#include <utility>
#include <map>
#include "seal/encryptionparams.h"
#include "seal/context.h"
#include "seal/evaluationkeys.h"
#include "seal/smallmodulus.h"
#include "seal/memorypoolhandle.h"
#include "seal/ciphertext.h"
#include "seal/plaintext.h"
#include "seal/galoiskeys.h"
#include "seal/util/polymodulus.h"
#include "seal/util/baseconverter.h"
#include "seal/util/uintarithsmallmod.h"

using namespace std;

namespace seal
{
    /**
    Provides operations on ciphertexts. Due to the properties of the encryption scheme,
    the arithmetic operations pass through the encryption layer to the underlying plaintext,
    changing it according to the type of the operation. Since the plaintext elements are
    fundamentally polynomials in the polynomial quotient ring Z_T[x]/(X^N+1), where T is
    the plaintext modulus and X^N+1 is the polynomial modulus, this is the ring where the
    arithmetic operations will take place. PolyCRTBuilder (batching) provider an alternative
    possibly more convenient view of the plaintext elements as 2-by-(N2/2) matrices of
    integers modulo the plaintext modulus. In the batching view the arithmetic operations
    act on the matrices element-wise. Some of the operations only apply in the batching
    view, such as matrix row and column rotations. Other operations such as relinearization
    have no semantic meaning but are necessary for performance reasons.
    
    @par Arithmetic Operations
    The core operations are arithmetic operations, in particular multiplication and addition 
    of ciphertexts. In addition to these, we also provide negation, subtraction, squaring,
    exponentiation, and multiplication and addition of several ciphertexts for convenience.
    in many cases some of the inputs to a computation are plaintext elements rather than
    ciphertexts. For this we provide fast "plain" operations: plain addition, plain subtraction,
    aand plain multiplication.

    @par Relinearization
    One of the most important non-arithmetic operations is relinearization, which takes
    as input a ciphertext of size K+1 and evaluation keys (at least K-1 keys are needed),
    and changes the size of the ciphertext down to 2 (minimum size). For most use-cases only 
    one evaluation key suffices, in which case relinearization should be performed after every 
    multiplication. Homomorphic multiplication of ciphertexts of size K+1 and L+1 outputs 
    a ciphertext of size K+L+1, and the computational cost of multiplication is proportional
    to K*L. Plain multiplication and addition operations of any type do not change the size.
    The performance of relinearization is determined by the decomposition bit count that the 
    evaluation keys were generated with.

    @par Rotations
    When batching is enabled, we provide operations for rotating the plaintext matrix rows
    cyclically left or right, and for rotating the columns (swapping the rows). Rotations
    require Galois keys to have been generated, and their performance depends on the
    decomposition bit count that the Galois keys were generated with.

    @par Other Operations
    We also provide operations for transforming ciphertexts to NTT form and back, and for
    transforming plaintext polynomials to NTT form. These can be used in a very fast plain
    multiplication variant, that assumes the inputs to be in NTT transformed form. Since
    the NTT has to be done in any case in plain multiplication, this function can be used
    when e.g. one plaintext input is used in several plain multiplication, and transforming
    it several times would not make sense.

    @par Overloads
    For many functions we provide two flavors of overloads. In one set of overloads the
    operations act on the inputs "in place", overwriting typically the first of the input
    parameters with the result, whereas the opposite set of overloads take a destination
    parameter where the result is stored. The first of these is always faster, and should
    be preferred when performance is critical. Another flavor of overloads concerns the
    memory pool used in allocations needed during the operation. In one set of overloads
    the local memory pool of the Evaluator (used to store pre-computation results and
    other member variables) is used for this purpose, and in another set of overloads the
    user can supply a MemoryPoolHandle to to be used instead. This is to allow one single 
    Evaluator to be used concurrently by several threads without running into thread 
    contention in allocations taking place during operations. For example, one can share 
    one single Evaluator across any number of threads, but in each thread call the 
    operations by giving them a thread-local MemoryPoolHandle to use. Thus, for many 
    operations we provide up to four different overloads, and it is important for 
    a developer to understand how these work to avoid unnecessary performance bottlenecks.

    @see EncryptionParameters for more details on encryption parameters.
    @see PolyCRTBuilder for more details on batching
    @see EvaluationKeys for more details on evaluation keys.
    @see GaloisKeys for more details on Galois keys.
    */
    class Evaluator
    {
    public:
        /**
        Creates an Evaluator instance initialized with the specified SEALContext. Dynamically 
        allocated member variables are allocated from the memory pool pointed to by the given 
        MemoryPoolHandle. By default the global memory pool is used.

        @param[in] context The SEALContext
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if encryption parameters are not valid
        @throws std::invalid_argument if pool is uninitialized
        */
        Evaluator(const SEALContext &context, 
            const MemoryPoolHandle &pool = MemoryPoolHandle::Global());

        /**
        Creates a deep copy of a given Evaluator.

        @param[in] copy The Evaluator to copy from
        */
        Evaluator(const Evaluator &copy);

        /**
        Creates a new Evaluator by moving a given one.

        @param[in] source The Evaluator to move from
        */
        Evaluator(Evaluator &&source) = default;

        /**
        Negates a ciphertext.

        @param[in] encrypted The ciphertext to negate
        @throws std::invalid_argument if encrypted is not valid for the encryption parameters
        */
        void negate(Ciphertext &encrypted);

        /**
        Negates a ciphertext and stores the result in the destination parameter.

        @param[in] encrypted The ciphertext to negate
        @param[out] destination The ciphertext to overwrite with the negated result
        @throws std::invalid_argument if encrypted is not valid for the encryption parameters
        @throws std::logic_error if destination is aliased and needs to be reallocated
        */
        inline void negate(const Ciphertext &encrypted, Ciphertext &destination)
        {
            destination = encrypted;
            negate(destination);
        }

        /**
        Adds two ciphertexts. This function adds together encrypted1 and encrypted2 and 
        stores the result in encrypted1.

        @param[in] encrypted1 The first ciphertext to add
        @param[in] encrypted2 The second ciphertext to add
        @throws std::invalid_argument if encrypted1 or encrypted2 is not valid for the encryption 
        parameters
        @throws std::logic_error if encrypted1 is aliased and needs to be reallocated
        */
        void add(Ciphertext &encrypted1, const Ciphertext &encrypted2);

        /**
        Adds two ciphertexts. This function adds together encrypted1 and encrypted2 and 
        stores the result in the destination parameter.

        @param[in] encrypted1 The first ciphertext to add
        @param[in] encrypted2 The second ciphertext to add
        @param[out] destination The ciphertext to overwrite with the addition result
        @throws std::invalid_argument if encrypted1 or encrypted2 is not valid for the encryption 
        parameters
        @throws std::logic_error if destination is aliased and needs to be reallocated
        */
        inline void add(const Ciphertext &encrypted1, const Ciphertext &encrypted2, 
            Ciphertext &destination)
        {
            destination = encrypted1;
            add(destination, encrypted2);
        }

        /**
        Adds together a vector of ciphertexts and stores the result in the destination 
        parameter.

        @param[in] encrypteds The ciphertexts to add
        @param[out] destination The ciphertext to overwrite with the addition result
        @throws std::invalid_argument if encrypteds is empty
        @throws std::invalid_argument if the ciphertexts are not valid for the encryption 
        parameters
        @throws std::logic_error if destination is aliased and needs to be reallocated
        */
        void add_many(const std::vector<Ciphertext> &encrypteds, Ciphertext &destination);

        /**
        Subtracts two ciphertexts. This function computes the difference of encrypted1 and
        encrypted2, and stores the result in encrypted1.

        @param[in] encrypted1 The ciphertext to subtract from
        @param[in] encrypted2 The ciphertext to subtract
        @throws std::invalid_argument if encrypted1 or encrypted2 is not valid for the encryption 
        parameters
        @throws std::logic_error if encrypted1 is aliased and needs to be reallocated
        */
        void sub(Ciphertext &encrypted1, const Ciphertext &encrypted2);

        /**
        Subtracts two ciphertexts. This function computes the difference of encrypted1 and 
        encrypted2 and stores the result in the destination parameter.

        @param[in] encrypted1 The ciphertext to subtract from
        @param[in] encrypted2 The ciphertext to subtract
        @param[out] destination The ciphertext to overwrite with the subtraction result
        @throws std::invalid_argument if encrypted1 or encrypted2 is not valid for the encryption 
        parameters
        @throws std::logic_error if destination is aliased and needs to be reallocated
        */
        inline void sub(const Ciphertext &encrypted1, const Ciphertext &encrypted2, 
            Ciphertext &destination)
        {
            destination = encrypted1;
            sub(destination, encrypted2);
        }

        /**
        Multiplies two ciphertexts. This functions computes the product of encrypted1 and
        encrypted2 and stores the result in encrypted1. Dynamic memory allocations in the 
        process are allocated from the memory pool pointed to by the given MemoryPoolHandle.

        @param[in] encrypted1 The first ciphertext to multiply
        @param[in] encrypted2 The second ciphertext to multiply
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if encrypted1 or encrypted2 is not valid for the encryption 
        parameters
        @throws std::logic_error if encrypted1 is aliased and needs to be reallocated
        @throws std::invalid_argument if pool is uninitialized
        */
        void multiply(Ciphertext &encrypted1, const Ciphertext &encrypted2, 
            const MemoryPoolHandle &pool);

        /**
        Multiplies two ciphertexts. This functions computes the product of encrypted1 and
        encrypted2 and stores the result in encrypted1. Dynamic memory allocations in the 
        process are allocated from the memory pool pointed to by the local MemoryPoolHandle.

        @param[in] encrypted1 The first ciphertext to multiply
        @param[in] encrypted2 The second ciphertext to multiply
        @throws std::invalid_argument if encrypted1 or encrypted2 is not valid for the encryption 
        parameters
        @throws std::logic_error if encrypted1 is aliased and needs to be reallocated
        */
        inline void multiply(Ciphertext &encrypted1, const Ciphertext &encrypted2)
        {
            multiply(encrypted1, encrypted2, pool_);
        }

        /**
        Multiplies two ciphertexts. This functions computes the product of encrypted1 and
        encrypted2 and stores the result in the destination parameter. Dynamic memory
        allocations in the process are allocated from the memory pool pointed to by the
        given MemoryPoolHandle.

        @param[in] encrypted1 The first ciphertext to multiply
        @param[in] encrypted2 The second ciphertext to multiply
        @param[out] destination The ciphertext to overwrite with the multiplication result
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if encrypted1 or encrypted2 is not valid for the encryption 
        parameters
        @throws std::logic_error if destination is aliased and needs to be reallocated
        @throws std::invalid_argument if pool is uninitialized
        */
        inline void multiply(const Ciphertext &encrypted1, const Ciphertext &encrypted2, 
            Ciphertext &destination, const MemoryPoolHandle &pool)
        {
            destination = encrypted1;
            multiply(destination, encrypted2, pool);
        }

        /**
        Multiplies two ciphertexts. This functions computes the product of encrypted1 and
        encrypted2 and stores the result in the destination parameter. Dynamic memory
        allocations in the process are allocated from the memory pool pointed to by the
        local MemoryPoolHandle.

        @param[in] encrypted1 The first ciphertext to multiply
        @param[in] encrypted2 The second ciphertext to multiply
        @param[out] destination The ciphertext to overwrite with the multiplication result
        @throws std::invalid_argument if encrypted1 or encrypted2 is not valid for the 
        encryption parameters
        @throws std::logic_error if destination is aliased and needs to be reallocated
        */
        inline void multiply(const Ciphertext &encrypted1, const Ciphertext &encrypted2, 
            Ciphertext &destination)
        {
            multiply(encrypted1, encrypted2, destination, pool_);
        }

        /**
        Squares a ciphertext. This functions computes the square of encrypted. Dynamic memory 
        allocations in the process are allocated from the memory pool pointed to by the given 
        MemoryPoolHandle.

        @param[in] encrypted The ciphertext to square
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if encrypted is not valid for the encryption 
        parameters
        @throws std::logic_error if encrypted is aliased and needs to be reallocated
        @throws std::invalid_argument if pool is uninitialized
        */
        void square(Ciphertext &encrypted, const MemoryPoolHandle &pool);

        /**
        Squares a ciphertext. This functions computes the square of encrypted. Dynamic memory 
        allocations in the process are allocated from the memory pool pointed to by the local 
        MemoryPoolHandle.

        @param[in] encrypted The ciphertext to square
        @throws std::invalid_argument if encrypted is not valid for the encryption
        parameters
        @throws std::logic_error if encrypted is aliased and needs to be reallocated
        */
        inline void square(Ciphertext &encrypted)
        {
            square(encrypted, pool_);
        }

        /**
        Squares a ciphertext. This functions computes the square of encrypted and stores 
        the result in the destination parameter. Dynamic memory allocations in the process 
        are allocated from the memory pool pointed to by the given MemoryPoolHandle.

        @param[in] encrypted The ciphertext to square
        @param[out] destination The ciphertext to overwrite with the square
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if encrypted is not valid for the encryption 
        parameters
        @throws std::logic_error if destination is aliased and needs to be reallocated
        @throws std::invalid_argument if pool is uninitialized
        */
        inline void square(const Ciphertext &encrypted, Ciphertext &destination, 
            const MemoryPoolHandle &pool)
        {
            destination = encrypted;
            square(destination, pool);
        }

        /**
        Squares a ciphertext. This functions computes the square of encrypted and stores 
        the result in the destination parameter. Dynamic memory allocations in the process 
        are allocated from the memory pool pointed to by the local MemoryPoolHandle.

        @param[in] encrypted The ciphertext to square
        @param[out] destination The ciphertext to overwrite with the square
        @throws std::invalid_argument if encrypted is not valid for the encryption
        parameters
        @throws std::logic_error if destination is aliased and needs to be reallocated
        */
        inline void square(const Ciphertext &encrypted, Ciphertext &destination)
        {
            square(encrypted, destination, pool_);
        }

        /**
        Relinearizes a ciphertext. This functions relinearizes encrypted, reducing its size 
        down to 2. If the size of encrypted is K+1, the given evaluation keys need to have 
        size at least K-1. Dynamic memory allocations in the process are allocated from the 
        memory pool pointed to by the given MemoryPoolHandle.

        @param[in] encrypted The ciphertext to relinearize
        @param[in] evaluation_keys The evaluation keys
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if encrypted or evaluation_keys is not valid for the 
        encryption parameters
        @throws std::invalid_argument if the size of evaluation_keys is too small
        @throws std::invalid_argument if pool is uninitialized
        */
        inline void relinearize(Ciphertext &encrypted, const EvaluationKeys &evaluation_keys, 
            const MemoryPoolHandle &pool)
        {
            relinearize(encrypted, evaluation_keys, 2, pool);
        }

        /**
        Relinearizes a ciphertext. This functions relinearizes encrypted, reducing its size
        down to 2. If the size of encrypted is K+1, the given evaluation keys need to have
        size at least K-1. Dynamic memory allocations in the process are allocated from the
        memory pool pointed to by the local MemoryPoolHandle.

        @param[in] encrypted The ciphertext to relinearize
        @param[in] evaluation_keys The evaluation keys
        @throws std::invalid_argument if encrypted or evaluation_keys is not valid for the
        encryption parameters
        @throws std::invalid_argument if the size of evaluation_keys is too small
        */
        inline void relinearize(Ciphertext &encrypted, const EvaluationKeys &evaluation_keys)
        {
            relinearize(encrypted, evaluation_keys, pool_);
        }

        /**
        Relinearizes a ciphertext. This functions relinearizes encrypted, reducing its size
        down to 2, and stores the result in the destination parameter. If the size of encrypted 
        is K+1, the given evaluation keys need to have size at least K-1. Dynamic memory 
        allocations in the process are allocated from the memory pool pointed to by the given 
        MemoryPoolHandle.

        @param[in] encrypted The ciphertext to relinearize
        @param[in] evaluation_keys The evaluation keys
        @param[out] destination The ciphertext to overwrite with the relinearized result
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if encrypted or evaluation_keys is not valid for the
        encryption parameters
        @throws std::invalid_argument if the size of evaluation_keys is too small
        @throws std::logic_error if destination is aliased and needs to be reallocated
        @throws std::invalid_argument if pool is uninitialized
        */
        inline void relinearize(const Ciphertext &encrypted, 
            const EvaluationKeys &evaluation_keys, Ciphertext &destination, 
            const MemoryPoolHandle &pool)
        {
            destination = encrypted;
            relinearize(destination, evaluation_keys, 2, pool);
        }

        /**
        Relinearizes a ciphertext. This functions relinearizes encrypted, reducing its size
        down to 2, and stores the result in the destination parameter. If the size of encrypted
        is K+1, the given evaluation keys need to have size at least K-1. Dynamic memory
        allocations in the process are allocated from the memory pool pointed to by the local
        MemoryPoolHandle.

        @param[in] encrypted The ciphertext to relinearize
        @param[in] evaluation_keys The evaluation keys
        @param[out] destination The ciphertext to overwrite with the relinearized result
        @throws std::invalid_argument if encrypted or evaluation_keys is not valid for the
        encryption parameters
        @throws std::invalid_argument if the size of evaluation_keys is too small
        @throws std::logic_error if destination is aliased and needs to be reallocated
        */
        inline void relinearize(const Ciphertext &encrypted, 
            const EvaluationKeys &evaluation_keys, Ciphertext &destination)
        {
            relinearize(encrypted, evaluation_keys, destination, pool_);
        }

        /**
        Multiplies several ciphertexts together. This function computes the product of several
        ciphertext given as an std::vector and stores the result in the destination parameter.
        The multiplication is done in a depth-optimal order, and relinearization is performed
        automatically after every multiplication in the process. In relinearization the given
        evaluation keys are used. Dynamic memory allocations in the process are allocated from 
        the memory pool pointed to by the given MemoryPoolHandle.

        @param[in] encrypteds The ciphertexts to multiply
        @param[in] evaluation_keys The evaluation keys
        @param[out] destination The ciphertext to overwrite with the multiplication result
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if encrypteds is empty
        @throws std::invalid_argument if the ciphertexts or evaluation_keys are not valid for
        the encryption parameters
        @throws std::invalid_argument if the size of evaluation_keys is too small
        @throws std::logic_error if destination is aliased and needs to be reallocated
        @throws std::invalid_argument if pool is uninitialized
        */
        void multiply_many(std::vector<Ciphertext> &encrypteds, 
            const EvaluationKeys &evaluation_keys, Ciphertext &destination, 
            const MemoryPoolHandle &pool);

        /**
        Multiplies several ciphertexts together. This function computes the product of several
        ciphertext given as an std::vector and stores the result in the destination parameter.
        The multiplication is done in a depth-optimal order, and relinearization is performed
        automatically after every multiplication in the process. In relinearization the given
        evaluation keys are used. Dynamic memory allocations in the process are allocated from
        the memory pool pointed to by the local MemoryPoolHandle.

        @param[in] encrypteds The ciphertexts to multiply
        @param[in] evaluation_keys The evaluation keys
        @param[out] destination The ciphertext to overwrite with the multiplication result
        @throws std::invalid_argument if encrypteds is empty
        @throws std::invalid_argument if the ciphertexts or evaluation_keys are not valid for
        the encryption parameters
        @throws std::invalid_argument if the size of evaluation_keys is too small
        @throws std::logic_error if destination is aliased and needs to be reallocated
        */
        inline void multiply_many(std::vector<Ciphertext> &encrypteds, 
            const EvaluationKeys &evaluation_keys, Ciphertext &destination)
        {
            multiply_many(encrypteds, evaluation_keys, destination, pool_);
        }

        /**
        Exponentiates a ciphertext. This functions raises encrypted to a power. Dynamic 
        memory allocations in the process are allocated from the memory pool pointed to by 
        the given MemoryPoolHandle. The exponentiation is done in a depth-optimal order, 
        and relinearization is performed automatically after every multiplication in the 
        process. In relinearization the given evaluation keys are used.

        @param[in] encrypted The ciphertext to exponentiate
        @param[in] exponent The power to raise the ciphertext to
        @param[in] evaluation_keys The evaluation keys
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if encrypted or evaluation_keys is not valid for the
        encryption parameters
        @throws std::invalid_argument if exponent is zero
        @throws std::invalid_argument if the size of evaluation_keys is too small
        @throws std::logic_error if encrypted is aliased and needs to be reallocated
        @throws std::invalid_argument if pool is uninitialized
        */
        void exponentiate(Ciphertext &encrypted, std::uint64_t exponent, 
            const EvaluationKeys &evaluation_keys, const MemoryPoolHandle &pool);

        /**
        Exponentiates a ciphertext. This functions raises encrypted to a power. Dynamic
        memory allocations in the process are allocated from the memory pool pointed to by
        the local MemoryPoolHandle. The exponentiation is done in a depth-optimal order,
        and relinearization is performed automatically after every multiplication in the
        process. In relinearization the given evaluation keys are used.

        @param[in] encrypted The ciphertext to exponentiate
        @param[in] exponent The power to raise the ciphertext to
        @param[in] evaluation_keys The evaluation keys
        @throws std::invalid_argument if encrypted or evaluation_keys is not valid for the
        encryption parameters
        @throws std::invalid_argument if exponent is zero
        @throws std::invalid_argument if the size of evaluation_keys is too small
        @throws std::logic_error if encrypted is aliased and needs to be reallocated
        */
        inline void exponentiate(Ciphertext &encrypted, std::uint64_t exponent, 
            const EvaluationKeys &evaluation_keys)
        {
            exponentiate(encrypted, exponent, evaluation_keys, pool_);
        }

        /**
        Exponentiates a ciphertext. This functions raises encrypted to a power and stores
        the result in the destination parameter. Dynamic memory allocations in the process
        are allocated from the memory pool pointed to by the given MemoryPoolHandle. The 
        exponentiation is done in a depth-optimal order, and relinearization is performed
        automatically after every multiplication in the process. In relinearization the 
        given evaluation keys are used. 

        @param[in] encrypted The ciphertext to exponentiate
        @param[in] exponent The power to raise the ciphertext to
        @param[in] evaluation_keys The evaluation keys
        @param[out] destination The ciphertext to overwrite with the power
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if encrypted or evaluation_keys is not valid for the 
        encryption parameters
        @throws std::invalid_argument if exponent is zero
        @throws std::invalid_argument if the size of evaluation_keys is too small
        @throws std::logic_error if destination is aliased and needs to be reallocated
        @throws std::invalid_argument if pool is uninitialized
        */
        inline void exponentiate(const Ciphertext &encrypted, std::uint64_t exponent, 
            const EvaluationKeys &evaluation_keys, Ciphertext &destination, 
            const MemoryPoolHandle &pool)
        {
            destination = encrypted;
            exponentiate(destination, exponent, evaluation_keys, pool);
        }

        /**
        Exponentiates a ciphertext. TThis functions raises encrypted to a power and stores
        the result in the destination parameter. Dynamic memory allocations in the process
        are allocated from the memory pool pointed to by the local MemoryPoolHandle. The
        exponentiation is done in a depth-optimal order, and relinearization is performed
        automatically after every multiplication in the process. In relinearization the 
        given evaluation keys are used.

        @param[in] encrypted The ciphertext to exponentiate
        @param[in] exponent The power to raise the ciphertext to
        @param[in] evaluation_keys The evaluation keys
        @param[out] destination The ciphertext to overwrite with the power
        @throws std::invalid_argument if encrypted or evaluation_keys is not valid for the
        encryption parameters
        @throws std::invalid_argument if exponent is zero
        @throws std::invalid_argument if the size of evaluation_keys is too small
        @throws std::logic_error if destination is aliased and needs to be reallocated
        */
        inline void exponentiate(const Ciphertext &encrypted, std::uint64_t exponent,
                const EvaluationKeys &evaluation_keys, Ciphertext &destination)
        {
            exponentiate(encrypted, exponent, evaluation_keys, destination, pool_);
        }

        /**
        Adds a ciphertext and a plaintext. This function adds a plaintext to a ciphertext.
        For the operation to be valid, the plaintext must have less than degree(poly_modulus)
        many non-zero coefficients, and each coefficient must be less than the plaintext
        modulus, i.e. the plaintext must be a valid plaintext under the current encryption
        parameters.

        @param[in] encrypted The ciphertext to add
        @param[in] plain The plaintext to add
        @throws std::invalid_argument if encrypted or plain is not valid for the encryption 
        parameters
        */
        void add_plain(Ciphertext &encrypted, const Plaintext &plain);

        /**
        Adds a ciphertext and a plaintext. This function adds a plaintext to a ciphertext
        and stores the result in the destination parameter. For the operation to be valid, 
        the plaintext must have less than degree(poly_modulus) many non-zero coefficients, 
        and each coefficient must be less than the plaintext modulus, i.e. the plaintext 
        must be a valid plaintext under the current encryption parameters.

        @param[in] encrypted The ciphertext to add
        @param[in] plain The plaintext to add
        @param[out] destination The ciphertext to overwrite with the addition result
        @throws std::invalid_argument if encrypted or plain is not valid for the encryption
        parameters
        @throws std::logic_error if destination is aliased and needs to be reallocated
        */
        inline void add_plain(const Ciphertext &encrypted, const Plaintext &plain, 
            Ciphertext &destination)
        {
            destination = encrypted;
            add_plain(destination, plain);
        }

        /**
        Subtracts a plaintext from a ciphertext. This function subtracts a plaintext from
        a ciphertext. For the operation to be valid, the plaintext must have less than 
        degree(poly_modulus) many non-zero coefficients, and each coefficient must be less
        than the plaintext modulus, i.e. the plaintext must be a valid plaintext under the 
        current encryption parameters.

        @param[in] encrypted The ciphertext to subtract from
        @param[in] plain The plaintext to subtract
        @throws std::invalid_argument if encrypted or plain is not valid for the encryption
        parameters
        */
        void sub_plain(Ciphertext &encrypted, const Plaintext &plain);

        /**
        Subtracts a plaintext from a ciphertext. This function subtracts a plaintext from
        a ciphertext and stores the result in the destination parameter. For the operation
        to be valid, the plaintext must have less than degree(poly_modulus) many non-zero 
        coefficients, and each coefficient must be less than the plaintext modulus, i.e. 
        the plaintext must be a valid plaintext under the current encryption parameters.

        @param[in] encrypted The ciphertext to subtract from
        @param[in] plain The plaintext to subtract
        @param[out] destination The ciphertext to overwrite with the subtraction result
        @throws std::invalid_argument if encrypted or plain is not valid for the encryption
        parameters
        @throws std::logic_error if destination is aliased and needs to be reallocated
        */
        inline void sub_plain(const Ciphertext &encrypted, const Plaintext &plain, 
            Ciphertext &destination)
        {
            destination = encrypted;
            sub_plain(destination, plain);
        }

        /**
        Multiplies a ciphertext with a plaintext. This function multiplies a ciphertext with
        a plaintext. For the operation to be valid, the plaintext must have less than
        degree(poly_modulus) many non-zero coefficients, and each coefficient must be less 
        than the plaintext modulus, i.e. the plaintext must be a valid plaintext under the
        current encryption parameters. Moreover, the plaintext cannot be identially 0.
        Dynamic memory allocations in the process are allocated from the memory pool pointed 
        to by the given MemoryPoolHandle.

        @param[in] encrypted The ciphertext to multiply
        @param[in] plain The plaintext to multiply
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if the encrypted or plain is not valid for the encryption
        parameters
        @throws std::invalid_argument if plain is zero
        @throws std::invalid_argument if pool is uninitialized
        */
        void multiply_plain(Ciphertext &encrypted, const Plaintext &plain, 
            const MemoryPoolHandle &pool);

        /**
        Multiplies a ciphertext with a plaintext. This function multiplies a ciphertext with
        a plaintext. For the operation to be valid, the plaintext must have less than
        degree(poly_modulus) many non-zero coefficients, and each coefficient must be less
        than the plaintext modulus, i.e. the plaintext must be a valid plaintext under the
        current encryption parameters. Moreover, the plaintext cannot be identially 0.
        Dynamic memory allocations in the process are allocated from the memory pool pointed
        to by the local MemoryPoolHandle.

        @param[in] encrypted The ciphertext to multiply
        @param[in] plain The plaintext to multiply
        @throws std::invalid_argument if the encrypted or plain is not valid for the encryption
        parameters
        @throws std::invalid_argument if plain is zero
        */
        inline void multiply_plain(Ciphertext &encrypted, const Plaintext &plain)
        {
            multiply_plain(encrypted, plain, pool_);
        }

        /**
        Multiplies a ciphertext with a plaintext. This function multiplies a ciphertext with
        a plaintext and stores the result in the destination parameter. For the operation
        to be valid, the plaintext must have less than degree(poly_modulus) many non-zero 
        coefficients, and each coefficient must be less than the plaintext modulus, i.e. 
        the plaintext must be a valid plaintext under the current encryption parameters.
        Moreover, the plaintext cannot be identially 0. Dynamic memory allocations in the 
        process are allocated from the memory pool pointed to by the given MemoryPoolHandle. 

        @param[in] encrypted The ciphertext to multiply
        @param[in] plain The plaintext to multiply
        @param[out] destination The ciphertext to overwrite with the multiplication result
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if the encrypted or plain is not valid for the encryption
        parameters
        @throws std::invalid_argument if plain is zero
        @throws std::logic_error if destination is aliased and needs to be reallocated
        @throws std::invalid_argument if pool is uninitialized
        */
        inline void multiply_plain(const Ciphertext &encrypted, const Plaintext &plain, 
            Ciphertext &destination, const MemoryPoolHandle &pool)
        {
            destination = encrypted;
            multiply_plain(destination, plain, pool);
        }

        /**
        Multiplies a ciphertext with a plaintext. This function multiplies a ciphertext with
        a plaintext and stores the result in the destination parameter. For the operation
        to be valid, the plaintext must have less than degree(poly_modulus) many non-zero
        coefficients, and each coefficient must be less than the plaintext modulus, i.e.
        the plaintext must be a valid plaintext under the current encryption parameters.
        Moreover, the plaintext cannot be identially 0. Dynamic memory allocations in the
        process are allocated from the memory pool pointed to by the local MemoryPoolHandle.

        @param[in] encrypted The ciphertext to multiply
        @param[in] plain The plaintext to multiply
        @param[out] destination The ciphertext to overwrite with the multiplication result
        @throws std::invalid_argument if the encrypted or plain is not valid for the encryption
        parameters
        @throws std::invalid_argument if plain is zero
        @throws std::logic_error if destination is aliased and needs to be reallocated
        */
        inline void multiply_plain(const Ciphertext &encrypted, const Plaintext &plain, 
            Ciphertext &destination)
        {
            multiply_plain(encrypted, plain, destination, pool_);
        }

        /**
        Transforms a plaintext to NTT domain. This functions applies the Number Theoretic
        Transform to a plaintext by first embedding integers modulo the plaintext modulus
        to integers modulo the coefficient modulus, and then performing David Harvey's NTT
        on the resulting polynomial.For the operation to be valid, the plaintext must have 
        less than degree(poly_modulus) many non-zero coefficients, and each coefficient must
        be less than the plaintext modulus, i.e. the plaintext must be a valid plaintext 
        under the current encryption parameters. Dynamic memory allocations in the process
        are allocated from the memory pool pointed to by the given MemoryPoolHandle.

        @param[in] plain The plaintext to transform
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if plain is not valid for the encryption parameters
        @throws std::logic_error if plain is aliased and needs to be reallocated
        @throws std::invalid_argument if pool is uninitialized
        */
        void transform_to_ntt(Plaintext &plain, const MemoryPoolHandle &pool);

        /**
        Transforms a plaintext to NTT domain. This functions applies the Number Theoretic
        Transform to a plaintext by first embedding integers modulo the plaintext modulus
        to integers modulo the coefficient modulus, and then performing David Harvey's NTT
        on the resulting polynomial.For the operation to be valid, the plaintext must have
        less than degree(poly_modulus) many non-zero coefficients, and each coefficient must
        be less than the plaintext modulus, i.e. the plaintext must be a valid plaintext
        under the current encryption parameters. Dynamic memory allocations in the process
        are allocated from the memory pool pointed to by the local MemoryPoolHandle.

        @param[in] plain The plaintext to transform
        @throws std::invalid_argument if plain is not valid for the encryption parameters
        @throws std::logic_error if plain is aliased and needs to be reallocated
        */
        inline void transform_to_ntt(Plaintext &plain)
        {
            transform_to_ntt(plain, pool_);
        }

        /**
        Transforms a plaintext to NTT domain. This functions applies the Number Theoretic
        Transform to a plaintext by first embedding integers modulo the plaintext modulus
        to integers modulo the coefficient modulus, and then performing David Harvey's NTT
        on the resulting polynomial. The result is stored in the destination_ntt parameter.
        For the operation to be valid, the plaintext must have less than degree(poly_modulus)
        many non-zero coefficients, and each coefficient must be less than the plaintext 
        modulus, i.e. the plaintext must be a valid plaintext under the current encryption 
        parameters. Dynamic memory allocations in the process are allocated from the memory 
        pool pointed to by the given MemoryPoolHandle.

        @param[in] plain The plaintext to transform
        @param[out] destinationNTT The plaintext to overwrite with the transformed result
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if plain is not valid for the encryption parameters
        @throws std::logic_error if destination_ntt is aliased and needs to be reallocated
        @throws std::invalid_argument if pool is uninitialized
        */
        inline void transform_to_ntt(const Plaintext &plain, Plaintext &destination_ntt,
            const MemoryPoolHandle &pool)
        {
            destination_ntt = plain;
            transform_to_ntt(destination_ntt, pool);
        }

        /**
        Transforms a plaintext to NTT domain. This functions applies the Number Theoretic
        Transform to a plaintext by first embedding integers modulo the plaintext modulus
        to integers modulo the coefficient modulus, and then performing David Harvey's NTT
        on the resulting polynomial. The result is stored in the destination_ntt parameter.
        For the operation to be valid, the plaintext must have less than degree(poly_modulus)
        many non-zero coefficients, and each coefficient must be less than the plaintext
        modulus, i.e. the plaintext must be a valid plaintext under the current encryption
        parameters. Dynamic memory allocations in the process are allocated from the memory
        pool pointed to by the local MemoryPoolHandle.

        @param[in] plain The plaintext to transform
        @param[out] destinationNTT The plaintext to overwrite with the transformed result
        @throws std::invalid_argument if plain is not valid for the encryption parameters
        @throws std::logic_error if destination_ntt is aliased and needs to be reallocated
        */
        inline void transform_to_ntt(const Plaintext &plain, Plaintext &destination_ntt)
        {
            transform_to_ntt(plain, destination_ntt, pool_);
        }

        /**
        Transforms a ciphertext to NTT domain. This functions applies David Harvey's Number
        Theoretic Transform separately to each polynomial of a ciphertext.

        @param[in] encrypted The ciphertext to transform
        @throws std::invalid_argument if encrypted is not valid for the encryption parameters
        */
        void transform_to_ntt(Ciphertext &encrypted);

        /**
        Transforms a ciphertext to NTT domain. This functions applies David Harvey's Number 
        Theoretic Transform separately to each polynomial of a ciphertext. The result is 
        stored in the destination_ntt parameter.

        @param[in] encrypted The ciphertext to transform
        @param[out] destination_ntt The ciphertext to overwrite with the transformed result
        @throws std::invalid_argument if encrypted is not valid for the encryption parameters
        @throws std::logic_error if destination_ntt is aliased and needs to be reallocated
        */
        inline void transform_to_ntt(const Ciphertext &encrypted, Ciphertext &destination_ntt)
        {
            destination_ntt = encrypted;
            transform_to_ntt(destination_ntt);
        }

        /**
        Transforms a ciphertext back from NTT domain. This functions applies the inverse of
        David Harvey's Number Theoretic Transform separately to each polynomial of a ciphertext.

        @param[in] encrypted_ntt The ciphertext to transform
        @throws std::invalid_argument if encrypted_ntt is not valid for the encryption parameters
        */
        void transform_from_ntt(Ciphertext &encrypted_ntt);

        /**
        Transforms a ciphertext back from NTT domain. This functions applies the inverse of 
        David Harvey's Number Theoretic Transform separately to each polynomial of a ciphertext.
        The result is stored in the destination parameter.

        @param[in] encrypted_ntt The ciphertext to transform
        @param[out] destination The ciphertext to overwrite with the transformed result
        @throws std::invalid_argument if encrypted_ntt is not valid for the encryption parameters
        @throws std::logic_error if destination is aliased and needs to be reallocated
        */
        inline void transform_from_ntt(const Ciphertext &encrypted_ntt, Ciphertext &destination)
        {
            destination = encrypted_ntt;
            transform_from_ntt(destination);
        }

        /*
        Multiplies a ciphertext with a plaintext. This function multiplies an NTT transformed
        ciphertext with an NTT transformed plaintext. The result ciphertext remains in the NTT
        domain, and can be subsequently transformed back to coefficient domain. The plaintext
        cannot be identially 0.

        @param[in] encrypted_ntt The ciphertext to multiply
        @param[in] plain_ntt The plaintext to multiply
        @throws std::invalid_argument if encrypted_ntt or plain_ntt is not valid for the
        encryption parameters
        @throws std::invalid_argument if plain_ntt is zero
        */
        void multiply_plain_ntt(Ciphertext &encrypted_ntt, const Plaintext &plain_ntt);

        /*
        Multiplies a ciphertext with a plaintext. This function multiplies an NTT transformed
        ciphertext with an NTT transformed plaintext. The result ciphertext remains in the NTT 
        domain, and can be subsequently transformed back to coefficient domain. The result is
        stored in the destination_ntt parameter. The plaintext cannot be identially 0.

        @param[in] encrypted_ntt The ciphertext to multiply
        @param[in] plain_ntt The plaintext to multiply
        @param[out] destination_ntt The ciphertext to overwrite with the multiplication result
        @throws std::invalid_argument if encrypted_ntt or plain_ntt is not valid for the
        encryption parameters
        @throws std::invalid_argument if plain_ntt is zero
        @throws std::logic_error if destination_ntt is aliased and needs to be reallocated
        */
        inline void multiply_plain_ntt(const Ciphertext &encrypted_ntt, const Plaintext &plain_ntt, 
            Ciphertext &destination_ntt)
        {
            destination_ntt = encrypted_ntt;
            multiply_plain_ntt(destination_ntt, plain_ntt);
        }

        /**
        Rotates plaintext matrix rows cyclically. When batching is used, this function rotates
        the encrypted plaintext matrix rows cyclically to the left (steps > 0) or to the right
        (steps < 0). Since the size of the batched matrix is 2-by-(N/2), where N is the degree
        of the polynomial modulus, the number of steps to rotate must have absolute value at 
        most N/2-1. Dynamic memory allocations in the process are allocated from the memory
        pool pointed to by the given MemoryPoolHandle.

        @param[in] encrypted The ciphertext to rotate
        @param[in] steps The number of steps to rotate (negative left, positive right)
        @param[in] galois_keys The Galois keys
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if encrypted or galois_keys is not valid for the
        encryption parameters
        @throws std::invalid_argument if encrypted has size greater than two
        @throws std::invalid_argument if steps has too big absolute value
        @throws std::invalid_argument if necessary Galois keys are not present
        @throws std::invalid_argument if pool is uninitialized
        */
        void rotate_rows(Ciphertext &encrypted, int steps, const GaloisKeys &galois_keys, 
            const MemoryPoolHandle &pool);

        /**
        Rotates plaintext matrix rows cyclically. When batching is used, this function rotates
        the encrypted plaintext matrix rows cyclically to the left (steps > 0) or to the right
        (steps < 0). Since the size of the batched matrix is 2-by-(N/2), where N is the degree
        of the polynomial modulus, the number of steps to rotate must have absolute value at
        most N/2-1. Dynamic memory allocations in the process are allocated from the memory
        pool pointed to by the local MemoryPoolHandle.

        @param[in] encrypted The ciphertext to rotate
        @param[in] steps The number of steps to rotate (negative left, positive right)
        @param[in] galois_keys The Galois keys
        @throws std::invalid_argument if encrypted or galois_keys is not valid for the
        encryption parameters
        @throws std::invalid_argument if encrypted has size greater than two
        @throws std::invalid_argument if steps has too big absolute value
        @throws std::invalid_argument if necessary Galois keys are not present
        */
        inline void rotate_rows(Ciphertext &encrypted, int steps, const GaloisKeys &galois_keys)
        {
            rotate_rows(encrypted, steps, galois_keys, pool_);
        }

        /**
        Rotates plaintext matrix rows cyclically. When batching is used, this function rotates
        the encrypted plaintext matrix rows cyclically to the left (steps > 0) or to the right
        (steps < 0) and writes the result to the destination parameter. Since the size of the
        batched matrix is 2-by-(N/2), where N is the degree of the polynomial modulus, the
        number of steps to rotate must have absolute value at most N/2-1. Dynamic memory
        allocations in the process are allocated from the memory pool pointed to by the given
        MemoryPoolHandle. 

        @param[in] encrypted The ciphertext to rotate
        @param[in] steps The number of steps to rotate (negative left, positive right)
        @param[in] galois_keys The Galois keys
        @param[out] destination The ciphertext to overwrite with the rotated result
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if encrypted or galois_keys is not valid for the
        encryption parameters
        @throws std::invalid_argument if encrypted has size greater than two
        @throws std::invalid_argument if steps has too big absolute value
        @throws std::invalid_argument if necessary Galois keys are not present
        @throws std::logic_error if destination is aliased and needs to be reallocated
        @throws std::invalid_argument if pool is uninitialized
        */
        inline void rotate_rows(const Ciphertext &encrypted, int steps, 
            const GaloisKeys &galois_keys, Ciphertext &destination, const MemoryPoolHandle &pool)
        {
            destination = encrypted;
            rotate_rows(destination, steps, galois_keys, pool);
        }

        /**
        Rotates plaintext matrix rows cyclically. When batching is used, this function rotates
        the encrypted plaintext matrix rows cyclically to the left (steps > 0) or to the right
        (steps < 0) and writes the result to the destination parameter. Since the size of the
        batched matrix is 2-by-(N/2), where N is the degree of the polynomial modulus, the
        number of steps to rotate must have absolute value at most N/2-1. Dynamic memory
        allocations in the process are allocated from the memory pool pointed to by the local
        MemoryPoolHandle.

        @param[in] encrypted The ciphertext to rotate
        @param[in] steps The number of steps to rotate (negative left, positive right)
        @param[in] galois_keys The Galois keys
        @param[out] destination The ciphertext to overwrite with the rotated result
        @throws std::invalid_argument if encrypted or galois_keys is not valid for the
        encryption parameters
        @throws std::invalid_argument if encrypted has size greater than two
        @throws std::invalid_argument if steps has too big absolute value
        @throws std::invalid_argument if necessary Galois keys are not present
        @throws std::logic_error if destination is aliased and needs to be reallocated
        */
        inline void rotate_rows(const Ciphertext &encrypted, int steps, 
            const GaloisKeys &galois_keys, Ciphertext &destination)
        {
            rotate_rows(encrypted, steps, galois_keys, destination, pool_);
        }

        /**
        Rotates plaintext matrix columns cyclically. When batching is used, this function
        rotates the encrypted plaintext matrix columns cyclically. Since the size of the 
        batched matrix is 2-by-(N/2), where N is the degree of the polynomial modulus, 
        this means simply swapping the two rows. Dynamic memory allocations in the process
        are allocated from the memory pool pointed to by the given MemoryPoolHandle.

        @param[in] encrypted The ciphertext to rotate
        @param[in] galois_keys The Galois keys
        @param[out] destination The ciphertext to overwrite with the rotated result
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if encrypted or galois_keys is not valid for the
        encryption parameters
        @throws std::invalid_argument if encrypted has size greater than two
        @throws std::invalid_argument if necessary Galois keys are not present
        @throws std::logic_error if destination is aliased and needs to be reallocated
        @throws std::invalid_argument if pool is uninitialized
        */
        inline void rotate_columns(Ciphertext &encrypted, const GaloisKeys &galois_keys, 
            const MemoryPoolHandle &pool)
        {
            std::uint64_t m = (parms_.poly_modulus().coeff_count() - 1) << 1;
            apply_galois(encrypted, m - 1, galois_keys, pool);
        }

        /**
        Rotates plaintext matrix columns cyclically. When batching is used, this function
        rotates the encrypted plaintext matrix columns cyclically. Since the size of the
        batched matrix is 2-by-(N/2), where N is the degree of the polynomial modulus,
        this means simply swapping the two rows. Dynamic memory allocations in the process
        are allocated from the memory pool pointed to by the local MemoryPoolHandle.

        @param[in] encrypted The ciphertext to rotate
        @param[in] galois_keys The Galois keys
        @param[out] destination The ciphertext to overwrite with the rotated result
        @throws std::invalid_argument if encrypted or galois_keys is not valid for the
        encryption parameters
        @throws std::invalid_argument if encrypted has size greater than two
        @throws std::invalid_argument if necessary Galois keys are not present
        @throws std::logic_error if destination is aliased and needs to be reallocated
        */
        inline void rotate_columns(Ciphertext &encrypted, const GaloisKeys &galois_keys)
        {
            rotate_columns(encrypted, galois_keys, pool_);
        }

        /**
        Rotates plaintext matrix columns cyclically. When batching is used, this function 
        rotates the encrypted plaintext matrix columns cyclically, and writes the result 
        to the destination parameter. Since the size of the batched matrix is 2-by-(N/2), 
        where N is the degree of the polynomial modulus, this means simply swapping the
        two rows. Dynamic memory allocations in the process are allocated from the memory 
        pool pointed to by the given MemoryPoolHandle.

        @param[in] encrypted The ciphertext to rotate
        @param[in] galois_keys The Galois keys
        @param[out] destination The ciphertext to overwrite with the rotated result
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if encrypted or galois_keys is not valid for the
        encryption parameters
        @throws std::invalid_argument if encrypted has size greater than two
        @throws std::invalid_argument if necessary Galois keys are not present
        @throws std::logic_error if destination is aliased and needs to be reallocated
        @throws std::invalid_argument if pool is uninitialized
        */
        inline void rotate_columns(const Ciphertext &encrypted, 
            const GaloisKeys &galois_keys,  Ciphertext &destination, 
            const MemoryPoolHandle &pool)
        {
            destination = encrypted;
            rotate_columns(destination, galois_keys, pool);
        }

        /**
        Rotates plaintext matrix columns cyclically. When batching is used, this function
        rotates the encrypted plaintext matrix columns cyclically, and writes the result
        to the destination parameter. Since the size of the batched matrix is 2-by-(N/2),
        where N is the degree of the polynomial modulus, this means simply swapping the
        two rows. Dynamic memory allocations in the process are allocated from the memory
        pool pointed to by the local MemoryPoolHandle.

        @param[in] encrypted The ciphertext to rotate
        @param[in] galois_keys The Galois keys
        @param[out] destination The ciphertext to overwrite with the rotated result
        @throws std::invalid_argument if encrypted or galois_keys is not valid for the
        encryption parameters
        @throws std::invalid_argument if encrypted has size greater than two
        @throws std::invalid_argument if necessary Galois keys are not present
        @throws std::logic_error if destination is aliased and needs to be reallocated
        */
        inline void rotate_columns(const Ciphertext &encrypted,
            const GaloisKeys &galois_keys, Ciphertext &destination)
        {
            rotate_columns(encrypted, galois_keys, destination, pool_);
        }

    private:
        Evaluator &operator =(const Evaluator &assign) = delete;

        Evaluator &operator =(Evaluator &&assign) = delete;

        void relinearize(Ciphertext &encrypted, const EvaluationKeys &evaluation_keys, int destination_size, 
            const MemoryPoolHandle &pool);

        inline void decompose_single_coeff(const std::uint64_t *value, std::uint64_t *destination, const MemoryPoolHandle &pool)
        {
#ifdef SEAL_DEBUG
            if (value == nullptr)
            {
                throw std::invalid_argument("value cannot be null");
            }
            if (destination == nullptr)
            {
                throw std::invalid_argument("destination cannot be null");
            }
            if (destination == value)
            {
                throw std::invalid_argument("value cannot be the same as destination");
            }
            if (!pool)
            {
                throw std::invalid_argument("pool cannot be null");
            }
#endif
            int coeff_mod_count = coeff_modulus_.size();

            if (coeff_mod_count == 1)
            {
                util::set_uint_uint(value, coeff_mod_count, destination);
                return;
            }

            util::Pointer value_copy(util::allocate_uint(coeff_mod_count, pool));
            for (int j = 0; j < coeff_mod_count; j++)
            {
                //destination[j] = util::modulo_uint(value, coeff_mod_count, coeff_modulus_[j], pool);
                
                // Manually inlined for efficiency
                // Make a fresh copy of value
                util::set_uint_uint(value, coeff_mod_count, value_copy.get());

                // Starting from the top, reduce always 128-bit blocks
                for (int i = coeff_mod_count - 2; i >= 0; i--)
                {
                    value_copy[i] = util::barrett_reduce_128(value_copy.get() + i, coeff_modulus_[j]);
                }
                destination[j] = value_copy[0];
            }
        }

        inline void decompose(const std::uint64_t *value, std::uint64_t *destination, const MemoryPoolHandle &pool)
        {
#ifdef SEAL_DEBUG
            if (value == nullptr)
            {
                throw std::invalid_argument("value cannot be null");
            }
            if (destination == nullptr)
            {
                throw std::invalid_argument("destination cannot be null");
            }
            if (destination == value)
            {
                throw std::invalid_argument("value cannot be the same as destination");
            }
            if (!pool)
            {
                throw std::invalid_argument("pool cannot be null");
            }
#endif
            int coeff_count = parms_.poly_modulus().coeff_count();
            int coeff_mod_count = coeff_modulus_.size();
            int total_uint64_count = coeff_mod_count * coeff_count;

            if (coeff_mod_count == 1)
            {
                util::set_uint_uint(value, total_uint64_count, destination);
                return;
            }

            util::Pointer value_copy(util::allocate_uint(coeff_mod_count, pool));
            for (int i = 0; i < coeff_count; i++)
            {
                for (int j = 0; j < coeff_mod_count; j++)
                {
                    //destination[i + (j * coeff_count)] = util::modulo_uint(value + (i * coeff_mod_count), 
                    //    coeff_mod_count, coeff_modulus_[j], pool);

                    // Manually inlined for efficiency
                    // Make a fresh copy of value + (i * coeff_mod_count)
                    util::set_uint_uint(value + (i * coeff_mod_count), coeff_mod_count, value_copy.get());

                    // Starting from the top, reduce always 128-bit blocks
                    for (int i = coeff_mod_count - 2; i >= 0; i--)
                    {
                        value_copy[i] = util::barrett_reduce_128(value_copy.get() + i, coeff_modulus_[j]);
                    }
                    destination[i + (j * coeff_count)] = value_copy[0];
                }
            }
        }

        void compose(std::uint64_t *value, const MemoryPoolHandle &pool);

        void relinearize_one_step(std::uint64_t *encrypted, int encrypted_size, 
            const EvaluationKeys &evaluation_keys, const MemoryPoolHandle &pool);

        void populate_Zmstar_to_generator();

        // The apply_galois function applies a Galois automorphism to a ciphertext. 
        // It is needed for slot permutations. 
        // Input: encryption of M(x) and an integer p such that gcd(p, m) = 1.
        // Output: encryption of M(x^p). 
        // The function requires certain GaloisKeys and auxiliary data. 
        void apply_galois(Ciphertext &encrypted, std::uint64_t galois_elt, const GaloisKeys &evaluation_keys,
            const MemoryPoolHandle &pool);

        inline void apply_galois(Ciphertext &encrypted, std::uint64_t galois_elt, const GaloisKeys &evaluation_keys)
        {
            apply_galois(encrypted, galois_elt, evaluation_keys, pool_);
        }

        inline void apply_galois(const Ciphertext &encrypted, std::uint64_t galois_elt,
            const GaloisKeys &evaluation_keys, Ciphertext &destination, const MemoryPoolHandle &pool)
        {
            destination = encrypted;
            apply_galois(destination, galois_elt, evaluation_keys, pool);
        }

        inline void apply_galois(const Ciphertext &encrypted, std::uint64_t galois_elt,
            const GaloisKeys &evaluation_keys, Ciphertext &destination)
        {
            apply_galois(encrypted, galois_elt, evaluation_keys, destination, pool_);
        }

        MemoryPoolHandle pool_;

        EncryptionParameters parms_;

        EncryptionParameterQualifiers qualifiers_;
        
        util::BaseConverter base_converter_;
        
        std::vector<util::SmallNTTTables> coeff_small_ntt_tables_;

        std::vector<util::SmallNTTTables> bsk_small_ntt_tables_;

        util::Pointer upper_half_increment_;

        util::Pointer coeff_div_plain_modulus_;

        std::uint64_t plain_upper_half_threshold_;

        util::Pointer plain_upper_half_increment_;

        std::vector<std::uint64_t> plain_upper_half_increment_array_;

        util::Pointer coeff_modulus_div_two_;

        util::Pointer coeff_products_array_;

        util::Pointer product_modulus_;

        util::Modulus mod_;

        util::PolyModulus polymod_;

        std::vector<SmallModulus> coeff_modulus_;

        std::vector<SmallModulus> bsk_mod_array_;

        std::vector<std::uint64_t> inv_coeff_products_mod_coeff_array_;

        int bsk_base_mod_count_;

        std::map<std::uint64_t, std::pair<std::uint64_t, std::uint64_t> > Zmstar_to_generator_;
    };
}
