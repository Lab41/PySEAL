#pragma once

#include <vector>
#include "seal/encryptionparams.h"
#include "seal/util/polymodulus.h"
#include "seal/plaintext.h"
#include "seal/ciphertext.h"
#include "seal/memorypoolhandle.h"
#include "seal/context.h"
#include "seal/util/smallntt.h"
#include "seal/publickey.h"

namespace seal
{
    /**
    Encrypts Plaintext objects into Ciphertext objects. Constructing an Encryptor requires
    a SEALContext with valid encryption parameters, and the public key. 

    @par Overloads
    For the encrypt function we provide two overloads concerning the memory pool used in 
    allocations needed during the operation. In one overload the local memory pool of 
    the Encryptor (used to store pre-computation results and other member variables) is 
    used for this purpose, and in another overload the user can supply 
    a MemoryPoolHandle to to be used instead. This is to allow one single Encryptor to 
    be used concurrently by several threads without running into thread contention in 
    allocations taking place during operations. For example, one can share one single 
    Encryptor across any number of threads, but in each thread call the encrypt function 
    by giving it a thread-local MemoryPoolHandle to use. It is important for a developer 
    to understand how this works to avoid unnecessary performance bottlenecks.
    */
    class Encryptor
    {
    public:
        /**
        Creates an Encryptor instance initialized with the specified SEALContext and public
        key. Dynamically allocated member variables are allocated from the memory pool
        pointed to by the given MemoryPoolHandle. By default the global memory pool is used.

        @param[in] context The SEALContext
        @param[in] public_key The public key
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if encryption parameters or public key are not valid
        @throws std::invalid_argument if pool is uninitialized
        */
        Encryptor(const SEALContext &context, const PublicKey &public_key,
            const MemoryPoolHandle &pool = MemoryPoolHandle::Global());

        /**
        Creates a deep copy of a given Encryptor.

        @param[in] copy The Encryptor to copy from
        */
        Encryptor(const Encryptor &copy);

        /**
        Creates a new Encryptor by moving a given one.

        @param[in] source The Encryptor to move from
        */
        Encryptor(Encryptor &&source) = default;

        /**
        Encrypts a Plaintext and stores the result in the destination parameter. Dynamic
        memory allocations in the process are allocated from the memory pool pointed to by 
        the given MemoryPoolHandle.

        @param[in] plain The plaintext to encrypt
        @param[out] destination The ciphertext to overwrite with the encrypted plaintext
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if plain is not valid for the encryption parameters
        @throws std::logic_error if destination is aliased and needs to be reallocated
        @throws std::invalid_argument if pool is uninitialized
        */
        void encrypt(const Plaintext &plain, Ciphertext &destination, 
            const MemoryPoolHandle &pool);

        /**
        Encrypts a Plaintext and stores the result in the destination parameter. Dynamic
        memory allocations in the process are allocated from the memory pool pointed to by
        the local MemoryPoolHandle.

        @param[in] plain The plaintext to encrypt
        @param[out] destination The ciphertext to overwrite with the encrypted plaintext
        @throws std::invalid_argument if plain is not valid for the encryption parameters
        @throws std::logic_error if destination is aliased and needs to be reallocated
        */
        inline void encrypt(const Plaintext &plain, Ciphertext &destination)
        {
            encrypt(plain, destination, pool_);
        }

    private:
        Encryptor &operator =(const Encryptor &assign) = delete;

        Encryptor &operator =(Encryptor &&assign) = delete;

        void preencrypt(const std::uint64_t *plain, int plain_coeff_count, std::uint64_t *destination);

        void set_poly_coeffs_normal(std::uint64_t *poly, UniformRandomGenerator *random) const;

        void set_poly_coeffs_zero_one_negone(uint64_t *poly, UniformRandomGenerator *random) const;

        void set_poly_coeffs_zero_one(uint64_t *poly, UniformRandomGenerator *random) const;

        MemoryPoolHandle pool_;

        EncryptionParameters parms_;

        EncryptionParameterQualifiers qualifiers_;

        std::vector<util::SmallNTTTables> small_ntt_tables_;

        std::uint64_t plain_upper_half_threshold_;

        util::Pointer upper_half_increment_;

        util::Pointer coeff_div_plain_modulus_;

        util::Pointer public_key_;

        util::PolyModulus polymod_;
    };
}