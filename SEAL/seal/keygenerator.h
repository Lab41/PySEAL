#pragma once

#include <memory>
#include <utility>
#include "seal/context.h"
#include "seal/util/polymodulus.h"
#include "seal/util/smallntt.h"
#include "seal/memorypoolhandle.h"
#include "seal/publickey.h"
#include "seal/secretkey.h"
#include "seal/evaluationkeys.h"
#include "seal/galoiskeys.h"

namespace seal
{
    /**
    Generates matching secret key and public key. An existing KeyGenerator can also at any time
    be used to generate evaluation keys and Galois keys. Constructing a KeyGenerator requires 
    only a SEALContext.

    @see EncryptionParameters for more details on encryption parameters.
    @see SecretKey for more details on secret key.
    @see PublicKey for more details on public key.
    @see EvaluationKeys for more details on evaluation keys.
    @see GaloisKeys for more details on Galois keys.
    */
    class KeyGenerator
    {
    public:
        /**
        Creates a KeyGenerator initialized with the specified SEALContext. Dynamically 
        allocated member variables are allocated from the memory pool pointed to by the 
        given MemoryPoolHandle. By default the global memory pool is used.

        @param[in] context The SEALContext
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if encryption parameters is not valid
        @throws std::invalid_argument if pool is uninitialized
        */
        KeyGenerator(const SEALContext &context, 
            const MemoryPoolHandle &pool = MemoryPoolHandle::Global());

        /**
        Creates an KeyGenerator instance initialized with the specified SEALContext and 
        specified previously secret and public keys. This can e.g. be used to increase the 
        number of evaluation keys from what had earlier been generated, or to generate 
        Galois keys in case they had not been generated earlier. Dynamically allocated 
        member variables are allocated from the memory pool pointed to by the given 
        MemoryPoolHandle. By default the global memory pool is used.

        @param[in] context The SEALContext
        @param[in] secret_key A previously generated secret key
        @param[in] public_key A previously generated public key
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if encryption parameters are not valid
        @throws std::invalid_argument if secret_key or public_key is not valid for 
        encryption parameters
        @throws std::invalid_argument if pool is uninitialized
        */
        KeyGenerator(const SEALContext &context, const SecretKey &secret_key, 
            const PublicKey &public_key,  
            const MemoryPoolHandle &pool = MemoryPoolHandle::Global());

        /**
        Returns a constant reference to the secret key.
        */
        const SecretKey &secret_key() const;

        /**
        Returns a constant reference to the public key.
        */
        const PublicKey &public_key() const;

        /**
        Generates the specified number of evaluation keys.

        @param[in] decomposition_bit_count The decomposition bit count
        @param[in] count The number of evaluation keys to generate
        @param[out] evaluation_keys The evaluation keys instance to overwrite with the 
        generated keys
        @throws std::invalid_argument if decomposition_bit_count is not within [1, 60]
        @throws std::invalid_argument if count is negative
        */
        void generate_evaluation_keys(int decomposition_bit_count, int count, 
            EvaluationKeys &evaluation_keys);

        /**
        Generates evaluation keys containing one key.

        @param[in] decomposition_bit_count The decomposition bit count
        @param[out] evaluation_keys The evaluation keys instance to overwrite with the
        generated keys
        @throws std::invalid_argument if decomposition_bit_count is not within [1, 60]
        */
        inline void generate_evaluation_keys(int decomposition_bit_count, 
            EvaluationKeys &evaluation_keys)
        {
            generate_evaluation_keys(decomposition_bit_count, 1, evaluation_keys);
        }

        /**
        Generates Galois keys.

        @param[in] decomposition_bit_count The decomposition bit count
        @param[out] galois_keys The Galois keys instance to overwrite with the generated keys
        @throws std::invalid_argument if decomposition_bit_count is not within [1, 60]
        @throws std::logic_error if the encryption parameters do not support batching
        */        
        void generate_galois_keys(int decomposition_bit_count, GaloisKeys &galois_keys);

    private:
        KeyGenerator(const KeyGenerator &copy) = delete;

        KeyGenerator &operator =(const KeyGenerator &assign) = delete;

        KeyGenerator(KeyGenerator &&source) = delete;

        KeyGenerator &operator =(KeyGenerator &&assign) = delete;

        void set_poly_coeffs_zero_one_negone(std::uint64_t *poly, UniformRandomGenerator *random) const;

        void set_poly_coeffs_normal(std::uint64_t *poly, UniformRandomGenerator *random) const;

        void set_poly_coeffs_uniform(std::uint64_t *poly, UniformRandomGenerator *random);

        void compute_secret_key_array(int max_power);

        void populate_decomposition_factors(int decomposition_bit_count, 
            std::vector<std::vector<std::uint64_t> > &decomposition_factors);

        /**
        Generates new matching set of secret key and public key.
        */
        void generate();

        /**
        Returns whether secret key and public key have been generated.
        */
        inline bool is_generated() const
        {
            return generated_;
        }

        void generate_galois_keys(int decomposition_bit_count, 
            const std::vector<std::uint64_t> &galois_elts, GaloisKeys &galois_keys);

        inline GaloisKeys generate_galois_keys(int decomposition_bit_count, 
            const std::vector<std::uint64_t> &galois_elts)
        {
            GaloisKeys keys;
            generate_galois_keys(decomposition_bit_count, galois_elts, keys);
            return keys;
        }

        MemoryPoolHandle pool_;

        EncryptionParameters parms_;

        EncryptionParameterQualifiers qualifiers_;

        std::vector<util::SmallNTTTables> small_ntt_tables_;

        PublicKey public_key_;

        SecretKey secret_key_;

        UniformRandomGeneratorFactory *random_generator_ = nullptr;

        util::PolyModulus polymod_;

        int secret_key_array_size_;

        util::Pointer secret_key_array_;

        mutable util::ReaderWriterLocker secret_key_array_locker_;

        bool generated_ = false;
    };
}
