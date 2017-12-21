#pragma once

#include <utility>
#include "seal/bigpolyarray.h"
#include "seal/encryptionparams.h"
#include "seal/context.h"
#include "seal/util/modulus.h"
#include "seal/util/polymodulus.h"
#include "seal/util/smallntt.h"
#include "seal/memorypoolhandle.h"
#include "seal/ciphertext.h"
#include "seal/plaintext.h"
#include "seal/secretkey.h"
#include "seal/util/baseconverter.h"
#include "seal/smallmodulus.h"
#include "seal/util/locks.h"

namespace seal
{
    /**
    Decrypts Ciphertext objects into Plaintext objects. Constructing a Decryptor requires
    a SEALContext with valid encryption parameters, and the secret key. The Decryptor is
    also used to compute the invariant noise budget in a given ciphertext.

    @par Overloads
    For the decrypt function we provide two overloads concerning the memory pool used in
    allocations needed during the operation. In one overload the local memory pool of
    the Decryptor (used to store pre-computation results and other member variables) is
    used for this purpose, and in another overload the user can supply
    a MemoryPoolHandle to to be used instead. This is to allow one single Decryptor to
    be used concurrently by several threads without running into thread contention in
    allocations taking place during operations. For example, one can share one single
    Decryptor across any number of threads, but in each thread call the decrypt function
    by giving it a thread-local MemoryPoolHandle to use. It is important for a developer
    to understand how this works to avoid unnecessary performance bottlenecks.
    */
    class Decryptor
    {
    public:
        /**
        Creates a Decryptor instance initialized with the specified SEALContext and secret
        key. Dynamically allocated member variables are allocated from the memory pool 
        pointed to by the given MemoryPoolHandle. By default the global memory pool is used.

        @param[in] context The SEALContext
        @param[in] secret_key The secret key
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if encryption parameters or secret key are not valid
        @throws std::invalid_argument if pool is uninitialized
        */
        Decryptor(const SEALContext &context, const SecretKey &secret_key,
            const MemoryPoolHandle &pool = MemoryPoolHandle::Global());

        /**
        Creates a deep copy of a given Decryptor.

        @param[in] copy The Decryptor to copy from
        */
        Decryptor(const Decryptor &copy);

        /**
        Creates a new Decryptor by moving a given one.

        @param[in] source The Decryptor to move from
        */
        Decryptor(Decryptor &&source) = default;

        /*
        Decrypts a Ciphertext and stores the result in the destination parameter. Dynamic
        memory allocations in the process are allocated from the memory pool pointed to by 
        the given MemoryPoolHandle.

        @param[in] encrypted The ciphertext to decrypt
        @param[out] destination The plaintext to overwrite with the decrypted ciphertext
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if encrypted is not valid for the encryption parameters
        @throws std::logic_error if destination is aliased and needs to be reallocated
        @throws std::invalid_argument if pool is uninitialized
        */
        void decrypt(const Ciphertext &encrypted, Plaintext &destination, const MemoryPoolHandle &pool);

        /*
        Decrypts a Ciphertext and stores the result in the destination parameter. Dynamic
        memory allocations in the process are allocated from the memory pool pointed to by
        the local MemoryPoolHandle.

        @param[in] encrypted The ciphertext to decrypt
        @param[out] destination The plaintext to overwrite with the decrypted ciphertext
        @throws std::logic_error if destination is aliased and needs to be reallocated
        @throws std::invalid_argument if encrypted is not valid for the encryption parameters
        */
        inline void decrypt(const Ciphertext &encrypted, Plaintext &destination)
        {
            decrypt(encrypted, destination, pool_);
        }

        /*
        Computes the invariant noise budget (in bits) of a ciphertext. The invariant noise 
        budget measures the amount of room there is for the noise to grow while ensuring 
        correct decryptions. Dynamic memory allocations in the process are allocated from
        the memory pool pointed to by the given MemoryPoolHandle.

        @par Invariant Noise Budget
        The invariant noise polynomial of a ciphertext is a rational coefficient polynomial, 
        such that a ciphertext decrypts correctly as long as the coefficients of the invariant
        noise polynomial are of absolute value less than 1/2. Thus, we call the infinity-norm 
        of the invariant noise polynomial the invariant noise, and for correct decryption require
        it to be less than 1/2. If v denotes the invariant noise, we define the invariant noise 
        budget as -log2(2v). Thus, the invariant noise budget starts from some initial value, 
        which depends on the encryption parameters, and decreases when computations are performed. 
        When the budget reaches zero, the ciphertext becomes too noisy to decrypt correctly.

        @param[in] encrypted The ciphertext
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if encrypted is not valid for the encryption parameters
        @throws std::invalid_argument if pool is uninitialized
        */
        int invariant_noise_budget(const Ciphertext &encrypted, const MemoryPoolHandle &pool);

        /*
        Computes the invariant noise budget (in bits) of a ciphertext. The invariant noise
        budget measures the amount of room there is for the noise to grow while ensuring
        correct decryptions. Dynamic memory allocations in the process are allocated from
        the memory pool pointed to by the local MemoryPoolHandle.

        @par Invariant Noise Budget
        The invariant noise polynomial of a ciphertext is a rational coefficient polynomial,
        such that a ciphertext decrypts correctly as long as the coefficients of the invariant
        noise polynomial are of absolute value less than 1/2. Thus, we call the infinity-norm
        of the invariant noise polynomial the invariant noise, and for correct decryption require
        it to be less than 1/2. If v denotes the invariant noise, we define the invariant noise
        budget as -log2(2v). Thus, the invariant noise budget starts from some initial value,
        which depends on the encryption parameters, and decreases when computations are performed.
        When the budget reaches zero, the ciphertext becomes too noisy to decrypt correctly.

        @param[in] encrypted The ciphertext
        @throws std::invalid_argument if encrypted is not valid for the encryption parameters
        */
        inline int invariant_noise_budget(const Ciphertext &encrypted)
        {
            return invariant_noise_budget(encrypted, pool_);
        }

    private:
        Decryptor &operator =(const Decryptor &assign) = delete;

        Decryptor &operator =(Decryptor &&assign) = delete;

        void compute_secret_key_array(int max_power);

        void compose(std::uint64_t *value);

        MemoryPoolHandle pool_;

        EncryptionParameters parms_;

        EncryptionParameterQualifiers qualifiers_;

        util::BaseConverter base_converter_;

        std::vector<util::SmallNTTTables> small_ntt_tables_;

        util::Pointer coeff_products_array_;

        util::Pointer secret_key_;

        util::Pointer product_modulus_;

        util::Modulus mod_;

        util::PolyModulus polymod_;

        int secret_key_array_size_ = 0;

        util::Pointer secret_key_array_;

        mutable util::ReaderWriterLocker secret_key_array_locker_;
    };
}
