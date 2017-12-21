#pragma once

#include "seal/evaluator.h"
#include "sealnet/EncryptionParamsWrapper.h"
#include "sealnet/ContextWrapper.h"
#include "sealnet/EvaluationKeysWrapper.h"
#include "sealnet/GaloisKeysWrapper.h"
#include "sealnet/CiphertextWrapper.h"
#include "sealnet/PlaintextWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            /**
            <summary>Provides operations on ciphertexts.</summary>

            <remarks>
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
            </para>
            <para>
            The core operations are arithmetic operations, in particular multiplication and addition
            of ciphertexts. In addition to these, we also provide negation, subtraction, squaring,
            exponentiation, and multiplication and addition of several ciphertexts for convenience.
            in many cases some of the inputs to a computation are plaintext elements rather than
            ciphertexts. For this we provide fast "plain" operations: plain addition, plain subtraction,
            aand plain multiplication.
            </para>
            <para>
            One of the most important non-arithmetic operations is relinearization, which takes
            as input a ciphertext of size K+1 and evaluation keys (at least K-1 keys are needed),
            and changes the size of the ciphertext down to 2 (minimum size). For most use-cases only
            one evaluation key suffices, in which case relinearization should be performed after every
            multiplication. Homomorphic multiplication of ciphertexts of size K+1 and L+1 outputs
            a ciphertext of size K+L+1, and the computational cost of multiplication is proportional
            to K*L. Plain multiplication and addition operations of any type do not change the size.
            The performance of relinearization is determined by the decomposition bit count that the
            evaluation keys were generated with.
            </para>
            <para>
            When batching is enabled, we provide operations for rotating the plaintext matrix rows
            cyclically left or right, and for rotating the columns (swapping the rows). Rotations
            require Galois keys to have been generated, and their performance depends on the
            decomposition bit count that the Galois keys were generated with.
            </para>
            <para>
            We also provide operations for transforming ciphertexts to NTT form and back, and for
            transforming plaintext polynomials to NTT form. These can be used in a very fast plain
            multiplication variant, that assumes the inputs to be in NTT transformed form. Since
            the NTT has to be done in any case in plain multiplication, this function can be used
            when e.g. one plaintext input is used in several plain multiplication, and transforming
            it several times would not make sense.
            </para>
            <para>
            For many functions we provide two flavors of overloads. In one set of overloads the
            operations act on the inputs "in place", overwriting typically the first of the input
            parameters with the result, whereas the opposite set of overloads take a destination
            parameter where the result is stored. The first of these is always faster, and should
            be preferred when performance is critical. Another flavor of overloads concerns the
            memory pool used in allocations needed during the operation. In one set of overloads
            the local memory pool of the Evaluator (used to store pre-computation results and
            other member variables) is used for this purpose, and in another set of overloads the
            user can supply a <see cref="MemoryPoolHandle" /> to to be used instead. This is to
            allow one single Evaluator to be used concurrently by several threads without running
            into thread contention in allocations taking place during operations. For example,
            one can share one single Evaluator across any number of threads, but in each thread
            call the operations by giving them a thread-local <see cref="MemoryPoolHandle" />
            to use. Thus, for many operations we provide up to four different overloads, and it is
            important for a developer to understand how these work to avoid unnecessary performance
            bottlenecks.
            </para>
            </remarks>
            <seealso cref="EncryptionParameters">See EncryptionParameters for more details on encryption 
            parameters.</seealso>
            <seealso cref="PolyCRTBuilder">See PolyCRTBuilder for more details on batching.</seealso>
            <seealso cref="EvaluationKeys">See EvaluationKeys for more details on evaluation keys.</seealso>
            <seealso cref="GaloisKeys">See GaloisKeys for more details on Galois keys.</seealso>
            */
            public ref class Evaluator
            {
            public:
                /**
                <summary>Creates an Evaluator instance initialized with the specified SEALContext.</summary>

                <remarks>
                Creates an Evaluator instance initialized with the specified <see cref="SEALContext" />. 
                Dynamically allocated member variables are allocated from the global memory pool.
                </remarks                
                <param name="context">The SEALContext</param>
                <exception cref="System::ArgumentException">if encryption parameters are not valid</exception>
                <exception cref="System::ArgumentNullException">if context is null</exception>
                */
                Evaluator(SEALContext ^context);

                /**
                <summary>Creates an Evaluator instance initialized with the specified SEALContext.</summary>

                <remarks>
                Creates an Evaluator instance initialized with the specified <see cref="SEALContext" />.
                Dynamically allocated member variables are allocated from the memory pool pointed to 
                by the given <see cref="MemoryPoolHandle" />.
                </remarks
                <param name="context">The SEALContext</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool/param>
                <exception cref="System::ArgumentException">if encryption parameters are not valid</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if context or pool is null</exception>
                */
                Evaluator(SEALContext ^context, MemoryPoolHandle ^pool);

                /**
                <summary>Creates a deep copy of a given Evaluator.</summary>

                <param name="copy">The Evaluator to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                Evaluator(Evaluator ^copy);

                /**
                <summary>Negates a ciphertext.</summary>

                <param name="encrypted">The ciphertext to negate</param>
                <exception cref="System::ArgumentException">if encrypted is not valid for the encryption 
                parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted is null</exception>
                */
                void Negate(Ciphertext ^encrypted);

                /**
                <summary>Negates a ciphertext and stores the result in the destination parameter.</summary>

                <param name="encrypted">The ciphertext to negate</param>
                <param name="destination">The ciphertext to overwrite with the negated result</param>
                <exception cref="System::ArgumentException">if encrypted is not valid for the encryption 
                parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted or destination is null</exception>
                */
                void Negate(Ciphertext ^encrypted, Ciphertext ^destination);

                /**
                <summary>Adds two ciphertexts.</summary>

                <remarks>
                Adds two ciphertexts. This function adds together encrypted1 and encrypted2 and
                stores the result in encrypted1.
                </remarks>
                <param name="encrypted1">The first ciphertext to add</param>
                <param name="encrypted2">The second ciphertext to add</param>
                <exception cref="System::ArgumentException">if encrypted1 or encrypted2 is not valid
                for the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted1 or encrypted2 is null</exception>
                */
                void Add(Ciphertext ^encrypted1, Ciphertext ^encrypted2);

                /**
                <summary>Adds two ciphertexts.</summary>

                <remarks>
                Adds two ciphertexts. This function adds together encrypted1 and encrypted2 and
                stores the result in the destination parameter.
                </remarks>
                <param name="encrypted1">The first ciphertext to add</param>
                <param name="encrypted2">The second ciphertext to add</param>
                <param name="destination">The ciphertext to overwrite with the addition result</param>
                <exception cref="System::ArgumentException">if encrypted1 or encrypted2 is not valid 
                for the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted1, encrypted2, or destination
                is null</exception>
                */
                void Add(Ciphertext ^encrypted1, Ciphertext ^encrypted2, Ciphertext ^destination);

                /**
                <summary>Adds together a list of ciphertexts and stores the result in the destination 
                parameter.</summary>

                <param name="encrypteds">The ciphertexts to add</param>
                <param name="destination">The ciphertext to overwrite with the addition result</param>
                <exception cref="System::ArgumentException">if encrypteds is empty</exception>
                <exception cref="System::ArgumentException">if the ciphertexts are not valid for the 
                encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypteds or any of its elements 
                is null</exception>
                */
                void AddMany(System::Collections::Generic::List<Ciphertext^> ^encrypteds, Ciphertext ^destination);

                /**
                <summary>Subtracts two ciphertexts.</summary>

                <remarks>
                Subtracts two ciphertexts. This function computes the difference of encrypted1 and
                encrypted2, and stores the result in encrypted1.
                </remarks>
                <param name="encrypted1">The ciphertext to subtract from</param>
                <param name="encrypted2">The ciphertext to subtract</param>
                <exception cref="System::ArgumentException">if encrypted1 or encrypted2 is not valid 
                for the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted1 or encrypted2 is null</exception>
                */
                void Sub(Ciphertext ^encrypted1, Ciphertext ^encrypted2);

                /**
                <summary>Subtracts two ciphertexts.</summary>

                <remarks>
                Subtracts two ciphertexts. This function computes the difference of encrypted1 and
                encrypted2, and stores the result in the destination parameter.
                </remarks>
                <param name="encrypted1">The ciphertext to subtract from</param>
                <param name="encrypted2">The ciphertext to subtract</param>
                <param name="destination">The ciphertext to overwrite with the subtraction result</param>
                <exception cref="System::ArgumentException">if the ciphertexts are not valid for the 
                encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted1, encrypted2, or destination
                is null</exception>
                */
                void Sub(Ciphertext ^encrypted1, Ciphertext ^encrypted2, Ciphertext ^destination);

                /**
                <summary>Multiplies two ciphertexts.</summary>

                <remarks>
                Multiplies two ciphertexts. This functions computes the product of encrypted1 and
                encrypted2 and stores the result in encrypted1. Dynamic memory allocations in the 
                process are allocated from the memory pool pointed to by the local 
                <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted1">The first ciphertext to multiply</param>
                <param name="encrypted2">The second ciphertext to multiply</param>
                <exception cref="System::ArgumentException">if encrypted1 or encrypted2 is not valid 
                for the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted1 or encrypted2 is 
                null</exception>
                */
                void Multiply(Ciphertext ^encrypted1, Ciphertext ^encrypted2);

                /**
                <summary>Multiplies two ciphertexts.</summary>

                <remarks>
                Multiplies two ciphertexts. This functions computes the product of encrypted1 and
                encrypted2 and stores the result in encrypted1. Dynamic memory allocations in the
                process are allocated from the memory pool pointed to by the given 
                <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted1">The first ciphertext to multiply</param>
                <param name="encrypted2">The second ciphertext to multiply</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool/param>
                <exception cref="System::ArgumentException">if encrypted1 or encrypted2 is not valid 
                for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if encrypted1 or encrypted2 is
                null</exception>
                */
                void Multiply(Ciphertext ^encrypted1, Ciphertext ^encrypted2, MemoryPoolHandle ^pool);

                /**
                <summary>Multiplies two ciphertexts.</summary>

                <remarks>
                Multiplies two ciphertexts. This functions computes the product of encrypted1 and
                encrypted2 and stores the result in the destination parameter. Dynamic memory
                allocations in the process are allocated from the memory pool pointed to by the
                local <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted1">The first ciphertext to multiply</param>
                <param name="encrypted2">The second ciphertext to multiply</param>
                <param name="destination">The ciphertext to overwrite with the multiplication result</param>
                <exception cref="System::ArgumentException">if encrypted1 or encrypted2 is not valid 
                for the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted1, encrypted2, or destination
                is null</exception>
                */
                void Multiply(Ciphertext ^encrypted1, Ciphertext ^encrypted2, Ciphertext ^destination);

                /**
                <summary>Multiplies two ciphertexts.</summary>

                <remarks>
                Multiplies two ciphertexts. This functions computes the product of encrypted1 and
                encrypted2 and stores the result in the destination parameter. Dynamic memory
                allocations in the process are allocated from the memory pool pointed to by the
                given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted1">The first ciphertext to multiply</param>
                <param name="encrypted2">The second ciphertext to multiply</param>
                <param name="destination">The ciphertext to overwrite with the multiplication result</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool/param>
                <exception cref="System::ArgumentException">if encrypted1 or encrypted2 is not valid 
                for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if encrypted1, encrypted2, destination,
                or pool is null</exception>
                */
                void Multiply(Ciphertext ^encrypted1, Ciphertext ^encrypted2, Ciphertext ^destination, 
                    MemoryPoolHandle ^pool);

                /**
                <summary>Squares a ciphertext.</summary>

                <remarks>
                Squares a ciphertext. This functions computes the square of encrypted and stores the
                result in encrypted1. Dynamic memory allocations in the process are allocated from 
                the memory pool pointed to by the local <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted">The ciphertext to square</param>
                <exception cref="System::ArgumentException">if encrypted is not valid for the
                encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted is null</exception>
                */
                void Square(Ciphertext ^encrypted);

                /**
                <summary>Squares a ciphertext.</summary>

                <remarks>
                Squares a ciphertext. This functions computes the square of encrypted and stores the
                result in encrypted1. Dynamic memory allocations in the process are allocated from 
                the memory pool pointed to by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted">The ciphertext to square</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool/param>
                <exception cref="System::ArgumentException">if encrypted is not valid for the
                encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted or pool is 
                null</exception>
                */
                void Square(Ciphertext ^encrypted, MemoryPoolHandle ^pool);

                /**
                <summary>Squares a ciphertext.</summary>

                <remarks>
                Squares a ciphertext. This functions computes the square of encrypted and stores the
                result in the destination parameter. Dynamic memory allocations in the process are
                allocated from the memory pool pointed to by the local <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted">The ciphertext to square</param>
                <param name="destination">The ciphertext to overwrite with the square</param>
                <exception cref="System::ArgumentException">if encrypted is not valid for the
                encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted or destination is
                null</exception>
                */
                void Square(Ciphertext ^encrypted, Ciphertext ^destination);

                /**
                <summary>Squares a ciphertext.</summary>

                <remarks>
                Squares a ciphertext. This functions computes the square of encrypted and stores the 
                result in the destination parameter. Dynamic memory allocations in the process are 
                allocated from the memory pool pointed to by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted">The ciphertext to square</param>
                <param name="destination">The ciphertext to overwrite with the square</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool/param>
                <exception cref="System::ArgumentException">if encrypted is not valid for the 
                encryption parameters</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if encrypted, destination, or pool 
                is null</exception>
                */
                void Square(Ciphertext ^encrypted, Ciphertext ^destination, MemoryPoolHandle ^pool);

                /**
                <summary>Relinearizes a ciphertext.</summary>

                <remarks>
                Relinearizes a ciphertext. This functions relinearizes encrypted, reducing its size
                down to 2. If the size of encrypted is K+1, the given evaluation keys need to have
                size at least K-1. Dynamic memory allocations in the process are allocated from the
                memory pool pointed to by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted">The ciphertext to relinearize</param>
                <param name="evaluationKeys">The evaluation keys</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool/param>
                <exception cref="System::ArgumentException">if encrypted or evaluationKeys is not 
                valid for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if the size of evaluationKeys is too 
                small</exception>
                <exception cref="System::ArgumentNullException">if encrypted, evaluationKeys, or 
                pool is null</exception>
                */
                void Relinearize(Ciphertext ^encrypted, EvaluationKeys ^evaluationKeys, MemoryPoolHandle ^pool);

                /**
                <summary>Relinearizes a ciphertext.</summary>

                <remarks>
                Relinearizes a ciphertext. This functions relinearizes encrypted, reducing its size
                down to 2. If the size of encrypted is K+1, the given evaluation keys need to have
                size at least K-1. Dynamic memory allocations in the process are allocated from the
                memory pool pointed to by the local <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted">The ciphertext to relinearize</param>
                <param name="evaluationKeys">The evaluation keys</param>
                <exception cref="System::ArgumentException">if encrypted or evaluationKeys is not
                valid for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if the size of evaluationKeys is too
                small</exception>
                <exception cref="System::ArgumentNullException">if encrypted or evaluationKeys is 
                null</exception>
                */
                void Relinearize(Ciphertext ^encrypted, EvaluationKeys ^evaluationKeys);

                /**
                <summary>Relinearizes a ciphertext.</summary>

                <remarks>
                Relinearizes a ciphertext. This functions relinearizes encrypted, reducing its size
                down to 2, and stores the result in the destination parameter. If the size of encrypted
                is K+1, the given evaluation keys need to have size at least K-1. Dynamic memory
                allocations in the process are allocated from the memory pool pointed to by the local
                <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted">The ciphertext to relinearize</param>
                <param name="evaluationKeys">The evaluation keys</param>
                <param name="destination">The ciphertext to overwrite with the relinearized result</param>
                <exception cref="System::ArgumentException">if encrypted or evaluationKeys is not valid
                for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if the size of evaluationKeys is too
                small</exception>
                <exception cref="System::ArgumentNullException">if encrypted, evaluationKeys, or 
                destination is null</exception>
                */
                void Relinearize(Ciphertext ^encrypted, EvaluationKeys ^evaluationKeys, Ciphertext ^destination);

                /**
                <summary>Relinearizes a ciphertext.</summary>

                <remarks>
                Relinearizes a ciphertext. This functions relinearizes encrypted, reducing its size
                down to 2, and stores the result in the destination parameter. If the size of encrypted
                is K+1, the given evaluation keys need to have size at least K-1. Dynamic memory
                allocations in the process are allocated from the memory pool pointed to by the given
                <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted">The ciphertext to relinearize</param>
                <param name="evaluationKeys">The evaluation keys</param>
                <param name="destination">The ciphertext to overwrite with the relinearized result</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool/param>
                <exception cref="System::ArgumentException">if encrypted or evaluationKeys is not valid
                for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if the size of evaluationKeys is too
                small</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if encrypted, evaluationKeys, destination,
                or pool is null</exception>
                */
                void Relinearize(Ciphertext ^encrypted, EvaluationKeys ^evaluationKeys, Ciphertext ^destination,
                    MemoryPoolHandle ^pool);

                /**
                <summary>Multiplies several ciphertexts together.</summary>

                <remarks>
                Multiplies several ciphertexts together. This function computes the product of several
                ciphertext given as an System::List and stores the result in the destination parameter.
                The multiplication is done in a depth-optimal order, and relinearization is performed
                automatically after every multiplication in the process. In relinearization the given
                evaluation keys are used. Dynamic memory allocations in the process are allocated from
                the memory pool pointed to by the local <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypteds">The ciphertexts to multiply</param>
                <param name="evaluationKeys">The evaluation keys</param>
                <param name="destination">The ciphertext to overwrite with the multiplication result</param>
                <exception cref="System::ArgumentException">if encrypteds is empty</exception>
                <exception cref="System::ArgumentException">if the ciphertexts or evaluationKeys are not 
                valid for the encryption parameters</exception>.
                <exception cref="System::ArgumentException">if the size of evaluationKeys is too
                small</exception>
                <exception cref="System::ArgumentNullException">if encrypteds or any of its elements is 
                null</exception>
                <exception cref="System::ArgumentNullException">if evaluationKeys or destination is 
                null</exception>
                */
                void MultiplyMany(System::Collections::Generic::List<Ciphertext^> ^encrypteds, 
                    EvaluationKeys ^evaluationKeys, Ciphertext ^destination);

                /**
                <summary>Multiplies several ciphertexts together.</summary>

                <remarks>
                Multiplies several ciphertexts together. This function computes the product of several
                ciphertext given as an System::List and stores the result in the destination parameter.
                The multiplication is done in a depth-optimal order, and relinearization is performed
                automatically after every multiplication in the process. In relinearization the given
                evaluation keys are used. Dynamic memory allocations in the process are allocated from
                the memory pool pointed to by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypteds">The ciphertexts to multiply</param>
                <param name="evaluationKeys">The evaluation keys</param>
                <param name="destination">The ciphertext to overwrite with the multiplication result</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if encrypteds is empty</exception>
                <exception cref="System::ArgumentException">if the ciphertexts or evaluationKeys are not
                valid for the encryption parameters</exception>.
                <exception cref="System::ArgumentException">if the size of evaluationKeys is too
                small</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if encrypteds or any of its elements is
                null</exception>
                <exception cref="System::ArgumentNullException">if evaluationKeys, destination, or pool
                is null</exception>
                */
                void MultiplyMany(System::Collections::Generic::List<Ciphertext^> ^encrypteds, 
                    EvaluationKeys ^evaluationKeys, Ciphertext ^destination, MemoryPoolHandle ^pool);

                /**
                <summary>Exponentiates a ciphertext.</summary>

                <remarks>
                Exponentiates a ciphertext. This functions raises encrypted to a power. Dynamic
                memory allocations in the process are allocated from the memory pool pointed to by
                the given <see cref="MemoryPoolHandle" />. The exponentiation is done in
                a depth-optimal order, and relinearization is performed automatically after every
                multiplication in the process. In relinearization the given evaluation keys are used.
                </remarks>
                <param name="encrypted">The ciphertext to exponentiate</param>
                <param name="exponent">The power to raise the ciphertext to</param>
                <param name="evaluationKeys">The evaluation keys</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if encrypted or evaluationKeys is not
                valid for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if exponent is zero</exception>
                <exception cref="System::ArgumentException">if the size of evaluationKeys is too
                small</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if encrypted, evaluationKeys,
                or pool is null</exception>
                */
                void Exponentiate(Ciphertext ^encrypted, System::UInt64 exponent,
                    EvaluationKeys ^evaluationKeys, MemoryPoolHandle ^pool);

                /**
                <summary>Exponentiates a ciphertext.</summary>

                <remarks>
                Exponentiates a ciphertext. This functions raises encrypted to a power. Dynamic
                memory allocations in the process are allocated from the memory pool pointed to by
                the local <see cref="MemoryPoolHandle" />. The exponentiation is done in
                a depth-optimal order, and relinearization is performed automatically after every
                multiplication in the process. In relinearization the given evaluation keys are used.
                </remarks>
                <param name="encrypted">The ciphertext to exponentiate</param>
                <param name="exponent">The power to raise the ciphertext to</param>
                <param name="evaluationKeys">The evaluation keys</param>
                <exception cref="System::ArgumentException">if encrypted or evaluationKeys is not
                valid for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if exponent is zero</exception>
                <exception cref="System::ArgumentException">if the size of evaluationKeys is too
                small</exception>
                <exception cref="System::ArgumentNullException">if encrypted r evaluationKeys
                is null</exception>
                */
                void Exponentiate(Ciphertext ^encrypted, System::UInt64 exponent, 
                    EvaluationKeys ^evaluationKeys);

                /**
                <summary>Raises a ciphertext to the specified power and stores the result in the destination parameter.</summary>
                <remarks>
                <para>
                Raises a ciphertext to the specified power and stores the result in the destination parameter.
                Relinearization is performed after every multiplication, so enough encryption keys must have
                been given to the constructor of the Evaluator.
                </para>
                <para>
                Exponentiation to power 0 is not allowed and will result in the library throwing an invalid argument
                exception. The reason behind this design choice is that the result should be a fresh encryption
                of 1, but creating fresh encryptions should not be something this class does. Instead the user
                should separately handle the cases where the exponent is 0.
                </para>
                </remarks>
                <param name="encrypted">The ciphertext to raise to a power</param>
                <param name="exponent">The power to raise the ciphertext to</param>
                <param name="destination">The ciphertext to overwrite with the exponentiation result</param>
                <exception cref="System::ArgumentException">if the ciphertext is not valid for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if exponent is zero</exception>
                <exception cref="System::ArgumentNullException">if encrypted or destination is null</exception>
                */

                /**
                <summary>Exponentiates a ciphertext.</summary>

                <remarks>
                Exponentiates a ciphertext. This functions raises encrypted to a power and stores
                the result in the destination parameter. Dynamic memory allocations in the process
                are allocated from the memory pool pointed to by the local <see cref="MemoryPoolHandle" />.
                The exponentiation is done in a depth-optimal order, and relinearization is performed
                automatically after every multiplication in the process. In relinearization the given
                evaluation keys are used.
                </remarks>
                <param name="encrypted">The ciphertext to exponentiate</param>
                <param name="exponent">The power to raise the ciphertext to</param>
                <param name="evaluationKeys">The evaluation keys</param>
                <param name="destination">The ciphertext to overwrite with the power</param>
                <exception cref="System::ArgumentException">if encrypted or evaluationKeys is not
                valid for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if exponent is zero</exception>
                <exception cref="System::ArgumentException">if the size of evaluationKeys is too
                small</exception>
                <exception cref="System::ArgumentNullException">if encrypted, evaluationKeys, or
                destination is null</exception>
                */
                void Exponentiate(Ciphertext ^encrypted, System::UInt64 exponent, 
                    EvaluationKeys ^evaluationKeys, Ciphertext ^destination);

                /**
                <summary>Exponentiates a ciphertext.</summary>

                <remarks>
                Exponentiates a ciphertext. This functions raises encrypted to a power and stores
                the result in the destination parameter. Dynamic memory allocations in the process 
                are allocated from the memory pool pointed to by the given <see cref="MemoryPoolHandle" />. 
                The exponentiation is done in a depth-optimal order, and relinearization is performed 
                automatically after every multiplication in the process. In relinearization the given 
                evaluation keys are used.
                </remarks>
                <param name="encrypted">The ciphertext to exponentiate</param>
                <param name="exponent">The power to raise the ciphertext to</param>
                <param name="evaluationKeys">The evaluation keys</param>
                <param name="destination">The ciphertext to overwrite with the power</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if encrypted or evaluationKeys is not
                valid for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if exponent is zero</exception>
                <exception cref="System::ArgumentException">if the size of evaluationKeys is too
                small</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if encrypted, evaluationKeys,
                destination, or pool is null</exception>
                */
                void Exponentiate(Ciphertext ^encrypted, System::UInt64 exponent, 
                    EvaluationKeys ^evaluationKeys, Ciphertext ^destination, MemoryPoolHandle ^pool);

                /**
                <summary>Adds a ciphertext and a plaintext.</summary>

                <remarks>
                Adds a ciphertext and a plaintext. This function adds a plaintext to a ciphertext.
                For the operation to be valid, the plaintext must have less than degree(PolyModulus)
                many non-zero coefficients, and each coefficient must be less than the plaintext 
                modulus, i.e. the plaintext must be a valid plaintext under the current encryption
                parameters.
                </remarks>
                <param name="encrypted">The ciphertext to add</param>
                <param name="plain">The plaintext to add</param>
                <exception cref="System::ArgumentException">if encrypted or plain is not valid for
                the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted or plain is 
                null</exception>
                */
                void AddPlain(Ciphertext ^encrypted, Plaintext ^plain);

                /**
                <summary>Adds a ciphertext and a plaintext.</summary>
                
                <remarks>
                Adds a ciphertext and a plaintext. This function adds a plaintext to a ciphertext
                and stores the result in the destination parameter. For the operation to be valid,
                the plaintext must have less than degree(PolyModulus) many non-zero coefficients,
                and each coefficient must be less than the plaintext modulus, i.e. the plaintext
                must be a valid plaintext under the current encryption parameters.
                </remarks>
                <param name="encrypted">The ciphertext to add</param>
                <param name="plain">The plaintext to add</param>
                <param name="destination">The ciphertext to overwrite with the addition result</param>
                <exception cref="System::ArgumentException">if encrypted or plain is not valid for 
                the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted, plain, or destination 
                is null</exception>
                */
                void AddPlain(Ciphertext ^encrypted, Plaintext ^plain, Ciphertext ^destination);

                /**
                <summary>Subtracts a plaintext from a ciphertext.</summary>

                <remarks>
                Subtracts a plaintext from a ciphertext. This function subtracts a plaintext from
                a ciphertext. For the operation to be valid, the plaintext must have less than 
                degree(PolyModulus) many non-zero coefficients, and each coefficient must be less
                than the plaintext modulus, i.e. the plaintext must be a valid plaintext under 
                the current encryption parameters.
                </remarks>
                <param name="encrypted">The ciphertext to subtract from</param>
                <param name="plain">The plaintext to subtract</param>
                <exception cref="System::ArgumentException">if encrypted or plain is not valid for
                the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted or plain is 
                null</exception>
                */
                void SubPlain(Ciphertext ^encrypted, Plaintext ^plain);

                /**
                <summary>Subtracts a plaintext from a ciphertext.</summary>

                <remarks>
                Subtracts a plaintext from a ciphertext. This function subtracts a plaintext from
                a ciphertext and stores the result in the destination parameter. For the operation 
                to be valid, the plaintext must have less than degree(PolyModulus) many non-zero 
                coefficients, and each coefficient must be less than the plaintext modulus, i.e.
                the plaintext must be a valid plaintext under the current encryption parameters.
                </remarks>
                <param name="encrypted">The ciphertext to subtract from</param>
                <param name="plain">The plaintext to subtract</param>
                <param name="destination">The ciphertext to overwrite with the subtraction 
                result</param>
                <exception cref="System::ArgumentException">if encrypted or plain is not valid for
                the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted, plain, or destination
                is null</exception>
                */
                void SubPlain(Ciphertext ^encrypted, Plaintext ^plain, Ciphertext ^destination);

                /**
                <summary>Multiplies a ciphertext with a plaintext.</summary>

                <remarks>
                Multiplies a ciphertext with a plaintext. This function multiplies a ciphertext
                with a plaintext. For the operation to be valid, the plaintext must have less than
                degree(PolyModulus) many non-zero coefficients, and each coefficient must be less
                than the plaintext modulus, i.e. the plaintext must be a valid plaintext under the
                current encryption parameters. Moreover, the plaintext cannot be identially 0.
                Dynamic memory allocations in the process are allocated from the memory pool
                pointed to by the local <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted">The ciphertext to multiply</param>
                <param name="plain">The plaintext to multiply</param>
                <exception cref="System::ArgumentException">if encrypted or plain is not valid 
                for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if plain is zero</exception>
                <exception cref="System::ArgumentNullException">if encrypted or plain is 
                null</exception>
                */
                void MultiplyPlain(Ciphertext ^encrypted, Plaintext ^plain);

                /**
                <summary>Multiplies a ciphertext with a plaintext.</summary>

                <remarks>
                Multiplies a ciphertext with a plaintext. This function multiplies a ciphertext 
                with a plaintext. For the operation to be valid, the plaintext must have less than 
                degree(PolyModulus) many non-zero coefficients, and each coefficient must be less 
                than the plaintext modulus, i.e. the plaintext must be a valid plaintext under the 
                current encryption parameters. Moreover, the plaintext cannot be identially 0. 
                Dynamic memory allocations in the process are allocated from the memory pool 
                pointed to by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted">The ciphertext to multiply</param>
                <param name="plain">The plaintext to multiply</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if encrypted or plain is not valid
                for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if plain is zero</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if encrypted, plain, or pool is
                null</exception>
                */
                void MultiplyPlain(Ciphertext ^encrypted, Plaintext ^plain, MemoryPoolHandle ^pool);

                /**
                <summary>Multiplies a ciphertext with a plaintext.</summary>

                <remarks>
                Multiplies a ciphertext with a plaintext. This function multiplies a ciphertext with
                a plaintext and stores the result in the destination parameter. For the operation
                to be valid, the plaintext must have less than degree(PolyModulus) many non-zero
                coefficients, and each coefficient must be less than the plaintext modulus, i.e.
                the plaintext must be a valid plaintext under the current encryption parameters.
                Moreover, the plaintext cannot be identially 0. Dynamic memory allocations in the
                process are allocated from the memory pool pointed to by the local
                <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted">The ciphertext to multiply</param>
                <param name="plain">The plaintext to multiply</param>
                <param name="destination">The ciphertext to overwrite with the multiplication
                result</param>
                <exception cref="System::ArgumentException">if encrypted or plain is not valid 
                for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if plain is zero</exception>
                <exception cref="System::ArgumentNullException">if encrypted, plain, or destination
                is null</exception>
                */
                void MultiplyPlain(Ciphertext ^encrypted, Plaintext ^plain, Ciphertext ^destination);

                /**
                <summary>Multiplies a ciphertext with a plaintext.</summary>

                <remarks>
                Multiplies a ciphertext with a plaintext. This function multiplies a ciphertext with
                a plaintext and stores the result in the destination parameter. For the operation
                to be valid, the plaintext must have less than degree(PolyModulus) many non-zero
                coefficients, and each coefficient must be less than the plaintext modulus, i.e.
                the plaintext must be a valid plaintext under the current encryption parameters.
                Moreover, the plaintext cannot be identially 0. Dynamic memory allocations in the
                process are allocated from the memory pool pointed to by the given 
                <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted">The ciphertext to multiply</param>
                <param name="plain">The plaintext to multiply</param>
                <param name="destination">The ciphertext to overwrite with the multiplication 
                result</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if encrypted or plain is not valid 
                for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if plain is zero</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if encrypted, plain, destination,
                or pool is null</exception>
                */
                void MultiplyPlain(Ciphertext ^encrypted, Plaintext ^plain, Ciphertext ^destination, 
                    MemoryPoolHandle ^pool);

                /**
                <summary>Transforms a plaintext to NTT domain.</summary>

                <remarks>
                Transforms a plaintext to NTT domain. This functions applies the Number Theoretic
                Transform to a plaintext by first embedding integers modulo the plaintext modulus
                to integers modulo the coefficient modulus, and then performing David Harvey's NTT
                on the resulting polynomial. For the operation to be valid, the plaintext must have
                less than degree(PolyModulus) many non-zero coefficients, and each coefficient must
                be less than the plaintext modulus, i.e. the plaintext must be a valid plaintext
                under the current encryption parameters. Dynamic memory allocations in the process
                are allocated from the memory pool pointed to by the local
                <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="plain">The plaintext to transform</param>
                <exception cref="System::ArgumentException">if plain is not valid for the
                encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if plain is null</exception>
                */
                void TransformToNTT(Plaintext ^plain);

                /**
                <summary>Transforms a plaintext to NTT domain.</summary>

                <remarks>
                Transforms a plaintext to NTT domain. This functions applies the Number Theoretic
                Transform to a plaintext by first embedding integers modulo the plaintext modulus
                to integers modulo the coefficient modulus, and then performing David Harvey's NTT
                on the resulting polynomial. For the operation to be valid, the plaintext must have
                less than degree(PolyModulus) many non-zero coefficients, and each coefficient must 
                be less than the plaintext modulus, i.e. the plaintext must be a valid plaintext
                under the current encryption parameters. Dynamic memory allocations in the process
                are allocated from the memory pool pointed to by the given 
                <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="plain">The plaintext to transform</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if plain is not valid for the
                encryption parameters</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if plain or pool is null</exception>
                */
                void TransformToNTT(Plaintext ^plain, MemoryPoolHandle ^pool);

                /**
                <summary>Transforms a plaintext to NTT domain.</summary>

                <remarks>
                Transforms a plaintext to NTT domain. This functions applies the Number Theoretic
                Transform to a plaintext by first embedding integers modulo the plaintext modulus
                to integers modulo the coefficient modulus, and then performing David Harvey's NTT
                on the resulting polynomial. The result is stored in the destinationNTT parameter.
                For the operation to be valid, the plaintext must have less than degree(PolyModulus)
                many non-zero coefficients, and each coefficient must be less than the plaintext
                modulus, i.e. the plaintext must be a valid plaintext under the current encryption
                parameters. Dynamic memory allocations in the process are allocated from the memory
                pool pointed to by the local <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="plain">The plaintext to transform</param>
                <param name="destinationNTT">The plaintext to overwrite with the transformed
                result</param>
                <exception cref="System::ArgumentException">if plain is not valid for the 
                encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if plain or destinationNTT is
                null</exception>
                */
                void TransformToNTT(Plaintext ^plain, Plaintext ^destinationNTT);

                /**
                <summary>Transforms a plaintext to NTT domain.</summary>

                <remarks>
                Transforms a plaintext to NTT domain. This functions applies the Number Theoretic
                Transform to a plaintext by first embedding integers modulo the plaintext modulus
                to integers modulo the coefficient modulus, and then performing David Harvey's NTT
                on the resulting polynomial. The result is stored in the destinationNTT parameter.
                For the operation to be valid, the plaintext must have less than degree(PolyModulus)
                many non-zero coefficients, and each coefficient must be less than the plaintext
                modulus, i.e. the plaintext must be a valid plaintext under the current encryption
                parameters. Dynamic memory allocations in the process are allocated from the memory
                pool pointed to by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="plain">The plaintext to transform</param>
                <param name="destinationNTT">The plaintext to overwrite with the transformed 
                result</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if plain is not valid for the 
                encryption parameters</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if plain, destinationNTT, or pool 
                is null</exception>
                */
                void TransformToNTT(Plaintext ^plain, Plaintext ^destinationNTT, MemoryPoolHandle ^pool);

                /**
                <summary>Transforms a ciphertext to NTT domain.</summary>

                <remarks>
                Transforms a ciphertext to NTT domain. This functions applies David Harvey's Number
                Theoretic Transform separately to each polynomial of a ciphertext. 
                </remarks>
                <param name="encrypted">The ciphertext to transform</param>
                <exception cref="System::ArgumentException">if encrypted is not valid for the encryption
                parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted is null</exception>
                */
                void TransformToNTT(Ciphertext ^encrypted);

                /**
                <summary>Transforms a ciphertext to NTT domain.</summary>

                <remarks>
                Transforms a ciphertext to NTT domain. This functions applies David Harvey's Number
                Theoretic Transform separately to each polynomial of a ciphertext. The result is stored 
                in the destinationNTT parameter.
                </remarks>
                <param name="encrypted">The ciphertext to transform</param>
                <param name="destinationNTT">The ciphertext to overwrite with the transformed  
                result</param>
                <exception cref="System::ArgumentException">if encrypted is not valid for the encryption 
                parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted or destinationNTT is 
                null</exception>
                */
                void TransformToNTT(Ciphertext ^encrypted, Ciphertext ^destinationNTT);

                /**
                <summary>Transforms a ciphertext back from NTT domain.</summary>

                <remarks>
                Transforms a ciphertext back from NTT domain. This functions applies the inverse of
                David Harvey's Number Theoretic Transform separately to each polynomial of a ciphertext.
                </remarks>
                <param name="encryptedNTT">The ciphertext to transform</param>
                result</param>
                <exception cref="System::ArgumentException">if encryptedNTT is not valid for the
                encryption parameters</exception
                <exception cref="System::ArgumentNullException">if encryptedNTT is
                null</exception>
                */
                void TransformFromNTT(Ciphertext ^encryptedNTT);

                /**
                <summary>Transforms a ciphertext back from NTT domain.</summary>

                <remarks>
                Transforms a ciphertext back from NTT domain. This functions applies the inverse of
                David Harvey's Number Theoretic Transform separately to each polynomial of a ciphertext.
                The result is stored in the destination parameter.
                </remarks>
                <param name="encryptedNTT">The ciphertext to transform</param>
                <param name="destination">The ciphertext to overwrite with the transformed
                result</param>
                <exception cref="System::ArgumentException">if encryptedNTT is not valid for the 
                encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encryptedNTT or destination is
                null</exception>
                */
                void TransformFromNTT(Ciphertext ^encryptedNTT, Ciphertext ^destination);

                /*
                <summary>Multiplies a ciphertext with a plaintext.</summary>

                <remarks>
                Multiplies a ciphertext with a plaintext. This function multiplies an NTT transformed
                ciphertext with an NTT transformed plaintext. The result ciphertext remains in the NTT
                domain, and can be subsequently transformed back to coefficient domain. The result is
                stored in the destinationNTT parameter. The plaintext cannot be identially 0.
                </remarks>
                <param name="encryptedNTT">The ciphertext to multiply</param>
                <param name="plainNTT">The plaintext to multiply<param>
                <param name="destinationNTT">The ciphertext to overwrite with the multiplication 
                result</param>
                <exception cref="System::ArgumentException">if encryptedNTT or plainNTT is not valid 
                for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if plainNTT is zero</exception>
                <exception cref="System::ArgumentNullException">if encryptedNTT, plainNTT, or 
                destinationNTT is null</exception>
                */
                void MultiplyPlainNTT(Ciphertext ^encryptedNTT, Plaintext ^plainNTT, 
                    Ciphertext ^destinationNTT);

                /*
                <summary>Multiplies a ciphertext with a plaintext.</summary>

                <remarks>
                Multiplies a ciphertext with a plaintext. This function multiplies an NTT transformed
                ciphertext with an NTT transformed plaintext. The result ciphertext remains in the NTT
                domain, and can be subsequently transformed back to coefficient domain. The plaintext 
                cannot be identially 0.
                </remarks>
                <param name="encryptedNTT">The ciphertext to multiply</param>
                <param name="plainNTT">The plaintext to multiply<param>
                <exception cref="System::ArgumentException">if encryptedNTT or plainNTT is not valid
                for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if plainNTT is zero</exception>
                <exception cref="System::ArgumentNullException">if encryptedNTT or plainNTT is
                null</exception>
                */
                void MultiplyPlainNTT(Ciphertext ^encryptedNTT, Plaintext ^plainNTT);

                /**
                <summary>Rotates plaintext matrix rows cyclically.</summary>

                <remarks>
                Rotates plaintext matrix rows cyclically. When batching is used, this function rotates
                the encrypted plaintext matrix rows cyclically to the left (steps > 0) or to the right
                (steps < 0). Since the size of the batched matrix is 2-by-(N/2), where N is the degree
                of the polynomial modulus, the number of steps to rotate must have absolute value at
                most N/2-1. Dynamic memory allocations in the process are allocated from the memory
                pool pointed to by the local <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted">The ciphertext to rotate</param>
                <param name="steps">The number of steps to rotate (negative left, positive right)</param>
                <param name="galoisKeys">The Galois keys</param>
                <exception cref="System::ArgumentException">if encrypted or galoisKeys is not valid
                for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if encrypted has size greater than 
                two</exception>
                <exception cref="System::ArgumentException">if steps has too big absolute
                value</exception>
                <exception cref="System::ArgumentException">if necessary Galois keys are not
                present</exception>
                <exception cref="System::ArgumentNullException">if encrypted, galoisKeys or pool
                is null</exception>
                */
                void RotateRows(Ciphertext ^encrypted, int steps, GaloisKeys ^galoisKeys);

                /**
                <summary>Rotates plaintext matrix rows cyclically.</summary>

                <remarks>
                Rotates plaintext matrix rows cyclically. When batching is used, this function rotates
                the encrypted plaintext matrix rows cyclically to the left (steps > 0) or to the right
                (steps < 0). Since the size of the batched matrix is 2-by-(N/2), where N is the degree 
                of the polynomial modulus, the number of steps to rotate must have absolute value at 
                most N/2-1. Dynamic memory allocations in the process are allocated from the memory 
                pool pointed to by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted">The ciphertext to rotate</param>
                <param name="steps">The number of steps to rotate (negative left, positive right)</param>
                <param name="galoisKeys">The Galois keys</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool/param>
                <exception cref="System::ArgumentException">if encrypted or galoisKeys is not valid
                for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if encrypted has size greater than 
                two</exception>
                <exception cref="System::ArgumentException">if steps has too big absolute
                value</exception>
                <exception cref="System::ArgumentException">if necessary Galois keys are not
                present</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if encrypted, galoisKeys or pool
                is null</exception>
                */
                void RotateRows(Ciphertext ^encrypted, int steps, GaloisKeys ^galoisKeys, 
                    MemoryPoolHandle ^pool);

                /**
                <summary>Rotates plaintext matrix rows cyclically.</summary>

                <remarks>
                Rotates plaintext matrix rows cyclically. When batching is used, this function rotates
                the encrypted plaintext matrix rows cyclically to the left (steps > 0) or to the right
                (steps < 0) and writes the result to the destination parameter. Since the size of the
                batched matrix is 2-by-(N/2), where N is the degree of the polynomial modulus, the
                number of steps to rotate must have absolute value at most N/2-1. Dynamic memory
                allocations in the process are allocated from the memory pool pointed to by the local
                <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted">The ciphertext to rotate</param>
                <param name="steps">The number of steps to rotate (negative left, positive right)</param>
                <param name="galoisKeys">The Galois keys</param>
                <param name="destination">The ciphertext to overwrite with the rotated result</param>
                <exception cref="System::ArgumentException">if encrypted or galoisKeys is not valid
                for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if encrypted has size greater than 
                two</exception>
                <exception cref="System::ArgumentException">if steps has too big absolute
                value</exception>
                <exception cref="System::ArgumentException">if necessary Galois keys are not
                present</exception>
                <exception cref="System::ArgumentNullException">if encrypted, galoisKeys, destination,
                or pool is null</exception>
                */
                void RotateRows(Ciphertext ^encrypted, int steps, GaloisKeys ^galoisKeys, 
                    Ciphertext ^destination);

                /**
                <summary>Rotates plaintext matrix rows cyclically.</summary>

                <remarks>
                Rotates plaintext matrix rows cyclically. When batching is used, this function rotates
                the encrypted plaintext matrix rows cyclically to the left (steps > 0) or to the right
                (steps < 0) and writes the result to the destination parameter. Since the size of the
                batched matrix is 2-by-(N/2), where N is the degree of the polynomial modulus, the
                number of steps to rotate must have absolute value at most N/2-1. Dynamic memory
                allocations in the process are allocated from the memory pool pointed to by the given
                <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted">The ciphertext to rotate</param>
                <param name="steps">The number of steps to rotate (negative left, positive right)</param>
                <param name="galoisKeys">The Galois keys</param>
                <param name="destination">The ciphertext to overwrite with the rotated result</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool/param>
                <exception cref="System::ArgumentException">if encrypted or galoisKeys is not valid
                for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if encrypted has size greater than 
                two</exception>
                <exception cref="System::ArgumentException">if steps has too big absolute 
                value</exception>
                <exception cref="System::ArgumentException">if necessary Galois keys are not 
                present</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if encrypted, galoisKeys, destination,
                or pool is null</exception>
                */
                void RotateRows(Ciphertext ^encrypted, int steps, GaloisKeys ^galoisKeys, 
                    Ciphertext ^destination, MemoryPoolHandle ^pool);

                /**
                <summary>Rotates plaintext matrix columns cyclically.</summary>

                <remarks>
                Rotates plaintext matrix columns cyclically. When batching is used, this function
                rotates the encrypted plaintext matrix columns cyclically. Since the size of the
                batched matrix is 2-by-(N/2), where N is the degree of the polynomial modulus,
                this means simply swapping the two rows. Dynamic memory allocations in the process
                are allocated from the memory pool pointed to by the local 
                <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted">The ciphertext to rotate</param>
                <param name="galoisKeys">The Galois keys</param>
                <exception cref="System::ArgumentException">if encrypted or galoisKeys is not valid
                for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if encrypted has size greater than 
                two</exception>
                <exception cref="System::ArgumentException">if necessary Galois keys are not
                present</exception>
                <exception cref="System::ArgumentNullException">if encrypted or galoisKeys is 
                null</exception>
                */
                void RotateColumns(Ciphertext ^encrypted, GaloisKeys ^galoisKeys);

                /**
                <summary>Rotates plaintext matrix columns cyclically.</summary>

                <remarks>
                Rotates plaintext matrix columns cyclically. When batching is used, this function
                rotates the encrypted plaintext matrix columns cyclically. Since the size of the 
                batched matrix is 2-by-(N/2), where N is the degree of the polynomial modulus, 
                this means simply swapping the two rows. Dynamic memory allocations in the process
                are allocated from the memory pool pointed to by the given 
                <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted">The ciphertext to rotate</param>
                <param name="galoisKeys">The Galois keys</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool/param>
                <exception cref="System::ArgumentException">if encrypted or galoisKeys is not valid
                for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if encrypted has size greater than 
                two</exception>
                <exception cref="System::ArgumentException">if necessary Galois keys are not
                present</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if encrypted, galoisKeys, or pool 
                is null</exception>
                */
                void RotateColumns(Ciphertext ^encrypted, GaloisKeys ^galoisKeys, 
                    MemoryPoolHandle ^pool);

                /**
                <summary>Rotates plaintext matrix columns cyclically.</summary>

                <remarks>
                Rotates plaintext matrix columns cyclically. When batching is used, this function
                rotates the encrypted plaintext matrix columns cyclically, and writes the result
                to the destination parameter. Since the size of the batched matrix is 2-by-(N/2),
                where N is the degree of the polynomial modulus, this means simply swapping the
                two rows. Dynamic memory allocations in the process are allocated from the memory
                pool pointed to by the local <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted">The ciphertext to rotate</param>
                <param name="galoisKeys">The Galois keys</param>
                <param name="destination">The ciphertext to overwrite with the rotated result</param>
                <exception cref="System::ArgumentException">if encrypted or galoisKeys is not valid
                for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if encrypted has size greater than 
                two</exception>
                <exception cref="System::ArgumentException">if necessary Galois keys are not
                present</exception>
                <exception cref="System::ArgumentNullException">if encrypted, galoisKeys, destination,
                or pool is null</exception>
                */
                void RotateColumns(Ciphertext ^encrypted, GaloisKeys ^galoisKeys, 
                    Ciphertext ^destination);

                /**
                <summary>Rotates plaintext matrix columns cyclically.</summary>

                <remarks>
                Rotates plaintext matrix columns cyclically. When batching is used, this function
                rotates the encrypted plaintext matrix columns cyclically, and writes the result
                to the destination parameter. Since the size of the batched matrix is 2-by-(N/2),
                where N is the degree of the polynomial modulus, this means simply swapping the
                two rows. Dynamic memory allocations in the process are allocated from the memory
                pool pointed to by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted">The ciphertext to rotate</param>
                <param name="galoisKeys">The Galois keys</param>
                <param name="destination">The ciphertext to overwrite with the rotated result</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool/param>
                <exception cref="System::ArgumentException">if encrypted or galoisKeys is not valid
                for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if necessary Galois keys are not
                present</exception>
                <exception cref="System::ArgumentException">if encrypted has size greater than 
                two</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if encrypted, galoisKeys, destination,
                or pool is null</exception>
                */
                void RotateColumns(Ciphertext ^encrypted, GaloisKeys ^galoisKeys, 
                    Ciphertext ^destination, MemoryPoolHandle ^pool);

                /**
                <summary>Destroys the Evaluator.</summary>
                */
                ~Evaluator();

                /**
                <summary>Destroys the Evaluator.</summary>
                */
                !Evaluator();

                /**
                <summary>Returns a reference to the underlying C++ Evaluator.</summary>
                */
                seal::Evaluator &GetEvaluator();

            private:
                seal::Evaluator *evaluator_;
            };
        }
    }
}
