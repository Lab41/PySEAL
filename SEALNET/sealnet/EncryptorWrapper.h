#pragma once

#include "seal/encryptor.h"
#include "sealnet/EncryptionParamsWrapper.h"
#include "sealnet/ContextWrapper.h"
#include "sealnet/PlaintextWrapper.h"
#include "sealnet/CiphertextWrapper.h"
#include "sealnet/PublicKeyWrapper.h"
#include "sealnet/MemoryPoolHandleWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            /**
            <summary>Encrypts Plaintext objects into Ciphertext objects.</summary>

            <remarks>
            <para>
            Encrypts <see cref="Plaintext" /> objects into <see cref="Ciphertext" /> objects. 
            Constructing an Encryptor requires a <see cref="SEALContext" /> with valid encryption 
            parameters, and the public key.
            </para>
            <para>
            For the Encrypt function we provide two overloads concerning the memory pool used in 
            allocations needed during the operation. In one overload the local memory pool of 
            the Encryptor (used to store pre-computation results and other member variables) is
            used for this purpose, and in another overload the user can supply a 
            <see cref="MemoryPoolHandle" /> to to be used instead. This is to allow one single 
            Encryptor to be used concurrently by several threads without running into thread
            contention in allocations taking place during operations. For example, one can share
            one single Encryptor across any number of threads, but in each thread call the Encrypt 
            function by giving it a thread-local <see cref="MemoryPoolHandle" /> to use. It is 
            important for a developer to understand how this works to avoid unnecessary performance 
            bottlenecks.
            </para>
            </remarks>
            */
            public ref class Encryptor
            {
            public:
                /**
                <summary>Creates an Encryptor instance initialized with the specified SEALContext and public
                key.</summary>
                
                <remarks>
                Creates an Encryptor instance initialized with the specified <see cref="SEALContext" /> and 
                public key. Dynamically allocated member variables are allocated from the global memory pool.
                </remarks>
                <param name="context">The SEALContext</param>
                <param name="publicKey">The public key</param>
                <exception cref="System::ArgumentException">if encryption parameters or public key are 
                not valid</exception>
                <exception cref="System::ArgumentNullException">if context or publicKey is null</exception>
                */
                Encryptor(SEALContext ^context, PublicKey ^publicKey);

                /**
                <summary>Creates an Encryptor instance initialized with the specified SEALContext and public
                key.</summary>

                <remarks>
                Creates an Encryptor instance initialized with the specified <see cref="SEALContext" /> and
                public key. Dynamically allocated member variables are allocated from the memory pool pointed
                to by the given <see cref="MemoryPoolHandle" />.
                </remarks>
                <param name="context">The SEALContext</param>
                <param name="publicKey">The public key</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if encryption parameters or public key are not 
                valid</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if context, publicKey, or pool is null</exception>
                */
                Encryptor(SEALContext ^context, PublicKey ^publicKey, MemoryPoolHandle ^pool);

                /**
                <summary>Creates a deep copy of a given Encryptor.</summary>

                <param name="copy">The Encryptor to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                Encryptor(Encryptor ^copy);

                /**
                <summary>ncrypts a Plaintext and stores the result in the destination parameter.</summary>

                <remarks>
                Encrypts a <see cref="Plaintext" /> and stores the result in the destination parameter. 
                Dynamic memory allocations in the process are allocated from the memory pool pointed to
                by the local MemoryPoolHandle.
                </remarks>
                <param name="plain">The plaintext to encrypt</param>
                <param name="destination">The ciphertext to overwrite with the encrypted plaintext</param>
                <exception cref="System::ArgumentException">if plain is not valid for the encryption 
                parameters</exception>
                <exception cref="System::ArgumentNullException">if plain or destination is null</exception>
                */
                void Encrypt(Plaintext ^plain, Ciphertext ^destination);

                /**
                <summary>ncrypts a Plaintext and stores the result in the destination parameter.</summary>

                <remarks>
                Encrypts a <see cref="Plaintext" /> and stores the result in the destination parameter.
                Dynamic memory allocations in the process are allocated from the memory pool pointed to
                by the given MemoryPoolHandle.
                </remarks>
                <param name="plain">The plaintext to encrypt</param>
                <param name="destination">The ciphertext to overwrite with the encrypted plaintext</param>
                <param name="pool">The MemoryPoolHandle pointing to a valid memory pool</param>
                <exception cref="System::ArgumentException">if plain is not valid for the encryption
                parameters</exception>
                <exception cref="System::ArgumentException">if pool is uninitialized</exception>
                <exception cref="System::ArgumentNullException">if plain, destination, or pool is null</exception>
                */
                void Encrypt(Plaintext ^plain, Ciphertext ^destination, MemoryPoolHandle ^pool);

                /**
                <summary>Destroys the Encryptor.</summary>
                */
                ~Encryptor();

                /**
                <summary>Destroys the Encryptor.</summary>
                */
                !Encryptor();

            internal:
                /**
                <summary>Returns a reference to the underlying C++ Encryptor.</summary>
                */
                seal::Encryptor &GetEncryptor();

            private:
                seal::Encryptor *encryptor_;
            };

        }
    }
}