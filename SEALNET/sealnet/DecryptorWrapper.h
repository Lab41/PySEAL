#pragma once

#include "seal/decryptor.h"
#include "sealnet/ContextWrapper.h"
#include "sealnet/PlaintextWrapper.h"
#include "sealnet/CiphertextWrapper.h"
#include "sealnet/SecretKeyWrapper.h"
#include "sealnet/MemoryPoolHandleWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            /**
            <summary>Decrypts Ciphertext objects into Plaintext objects.</summary>

            <remarks>
            <para>
            Decrypts <see cref="Ciphertext" /> objects into <see cref="Plaintext" /> objects. 
            Constructing a Decryptor requires a <see cref="SEALContext" /> with valid encryption 
            parameters, and the secret key. The Decryptor is also used to compute the invariant 
            noise budget in a given ciphertext.
            </para>
            <para>
            For the Decrypt function we provide two overloads concerning the memory pool used in
            allocations needed during the operation. In one overload the local memory pool of
            the Decryptor (used to store pre-computation results and other member variables) is
            used for this purpose, and in another overload the user can supply
            a <see cref="MemoryPoolHandle" /> to to be used instead. This is to allow one single
            Decryptor to be used concurrently by several threads without running into thread 
            contention in allocations taking place during operations. For example, one can share
            one single Decryptor across any number of threads, but in each thread call the 
            Decrypt function by giving it a thread-local <see cref="MemoryPoolHandle" /> to use. 
            It is important for a developer to understand how this works to avoid unnecessary 
            performance bottlenecks.
            </para>
            </remarks>
            */
            public ref class Decryptor
            {
            public:
                /**
                <summary>Creates a Decryptor instance initialized with the specified SEALContext
                and secret key.</summary>
                
                <remarks>
                Creates a Decryptor instance initialized with the specified <see cref="SEALContext" />
                and secret key. Dynamically allocated member variables are allocated from the global 
                memory pool.
                </remarks>
                <param name="context">The SEALContext</param>
                <param name="secretKey">The secret key</param>
                <exception cref="System::ArgumentNullException">if context or secretKey is null</exception>
                <exception cref="System::ArgumentException">if encryption parameters or secret key are 
                not valid</exception>
                */
                Decryptor(SEALContext ^context, SecretKey ^secretKey);

                /**
                <summary>Creates a Decryptor instance initialized with the specified SEALContext
                and secret key.</summary>

                <remarks>
                Creates a Decryptor instance initialized with the specified <see cref="SEALContext" />
                and secret key. Dynamically allocated member variables are allocated from the memory
                pool pointed to by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="context">The SEALContext</param>
                <param name="secretKey">The secret key</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if encryption parameters or secret key are not 
                valid</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if context, secretKey, or pool is null</exception>
                */
                Decryptor(SEALContext ^context, SecretKey ^secretKey, MemoryPoolHandle ^pool);

                /**
                <summary>Creates a deep copy of a given Decryptor.</summary>

                <param name="copy">The Decryptor to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                Decryptor(Decryptor ^copy);

                /*
                <summary>Decrypts a Ciphertext and stores the result in the destination parameter.</summary>

                <remarks>
                Decrypts a <see cref="Ciphertext" /> and stores the result in the destination parameter. 
                Dynamic memory allocations in the process are allocated from the memory pool pointed to 
                by the local <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted">The ciphertext to decrypt</param>
                <param name="destination">The plaintext to overwrite with the decrypted ciphertext</param>
                <exception cref="System::ArgumentException">if encrypted is not a valid for the
                encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted or destination is null</exception>
                */
                void Decrypt(Ciphertext ^encrypted, Plaintext ^destination);

                /*
                <summary>Decrypts a Ciphertext and stores the result in the destination parameter.</summary>

                <remarks>
                Decrypts a <see cref="Ciphertext" /> and stores the result in the destination parameter.
                Dynamic memory allocations in the process are allocated from the memory pool pointed to
                by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="encrypted">The ciphertext to decrypt</param>
                <param name="destination">The plaintext to overwrite with the decrypted ciphertext</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if encrypted is not a valid for the
                encryption parameters</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if encrypted, destination, or pool is 
                null</exception>
                */
                void Decrypt(Ciphertext ^encrypted, Plaintext ^destination, MemoryPoolHandle ^pool);

                /**
                <summary>Computes the invariant noise budget (in bits) of a Ciphertext.</summary>
                
                <remarks>
                <para>
                Computes the invariant noise budget (in bits) of a <see cref="Ciphertext" />. The 
                invariant noise budget measures the amount of room there is for the noise to grow 
                while ensuring correct decryptions. Dynamic memory allocations in the process are 
                allocated from the memory pool pointed to by the local MemoryPoolHandle.
                </para>
                <para>
                The invariant noise polynomial of a ciphertext is a rational coefficient polynomial,
                such that a ciphertext decrypts correctly as long as the coefficients of the invariant
                noise polynomial are of absolute value less than 1/2. Thus, we call the infinity-norm
                of the invariant noise polynomial the invariant noise, and for correct decryption require
                it to be less than 1/2. If v denotes the invariant noise, we define the invariant noise
                budget as -log2(2v). Thus, the invariant noise budget starts from some initial value,
                which depends on the encryption parameters, and decreases when computations are performed.
                When the budget reaches zero, the ciphertext becomes too noisy to decrypt correctly.
                </para>
                </remarks>
                <param name="encrypted">The ciphertext</param>
                <exception cref="System::ArgumentException">if encrypted is not a valid ciphertext for 
                the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted is null</exception>
                */
                int InvariantNoiseBudget(Ciphertext ^encrypted);

                /**
                <summary>Computes the invariant noise budget (in bits) of a ciphertext.</summary>

                <remarks>
                <para>
                Computes the invariant noise budget (in bits) of a <see cref="Ciphertext" />. The
                invariant noise budget measures the amount of room there is for the noise to grow
                while ensuring correct decryptions. Dynamic memory allocations in the process are
                allocated from the memory pool pointed to by the given MemoryPoolHandle.
                </para>
                <para>
                The invariant noise polynomial of a ciphertext is a rational coefficient polynomial,
                such that a ciphertext decrypts correctly as long as the coefficients of the invariant
                noise polynomial are of absolute value less than 1/2. Thus, we call the infinity-norm
                of the invariant noise polynomial the invariant noise, and for correct decryption require
                it to be less than 1/2. If v denotes the invariant noise, we define the invariant noise
                budget as -log2(2v). Thus, the invariant noise budget starts from some initial value,
                which depends on the encryption parameters, and decreases when computations are performed.
                When the budget reaches zero, the ciphertext becomes too noisy to decrypt correctly.
                </para>
                </remarks>
                <param name="encrypted">The ciphertext</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if encrypted is not a valid ciphertext for
                the encryption parameters</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if encrypted or pool is null</exception>
                */
                int InvariantNoiseBudget(Ciphertext ^encrypted, MemoryPoolHandle ^pool);

                /**
                <summary>Destroys the Decryptor.</summary>
                */
                ~Decryptor();

                /**
                <summary>Destroys the Decryptor.</summary>
                */
                !Decryptor();

            internal:
                /**
                <summary>Returns a reference to the underlying C++ Decryptor.</summary>
                */
                seal::Decryptor &GetDecryptor();

            private:
                seal::Decryptor *decryptor_;
            };
        }
    }
}
