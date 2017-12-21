#pragma once

#include <iostream>
#include <vector>
#include <numeric>
#include "seal/ciphertext.h"
#include "seal/encryptionparams.h"

namespace seal
{
    /**
    Class to store Galois keys.

    @par Slot Rotations
    Galois keys are used together with batching (PolyCRTBuilder). If the polynomial modulus
    is a polynomial of degree N, in batching the idea is to view a plaintext polynomial as
    a 2-by-(N/2) matrix of integers modulo plaintext modulus. Normal homomorphic computations
    operate on such encrypted matrices element (slot) wise. However, special rotation
    operations allow us to also rotate the matrix rows cyclically in either direction, and 
    rotate the columns (swap the rows). These operations require the Galois keys.

    @par Decomposition Bit Count
    Decomposition bit count (dbc) is a parameter that describes a performance trade-off in
    the rotation operation. Its function is exactly the same as in relinearization. Namely, 
    the polynomials in the ciphertexts (with large coefficients) get decomposed into a smaller
    base 2^dbc, coefficient-wise. Each of the decomposition factors corresponds to a piece of 
    data in the Galois keys, so the smaller the dbc is, the larger the Galois keys are. 
    Moreover, a smaller dbc results in less invariant noise budget being consumed in the
    rotation operation. However, using a large dbc is much faster, and often one would want 
    to optimize the dbc to be as large as possible for performance. The dbc is upper-bounded 
    by the value of 60, and lower-bounded by the value of 1.

    @par Thread Safety
    In general, reading from GaloisKeys is thread-safe as long as no other thread is 
    concurrently mutating it. This is due to the underlying data structure storing the
    Galois keys not being thread-safe.

    @see SecretKey for the class that stores the secret key.
    @see PublicKey for the class that stores the public key.
    @see EvaluationKeys for the class that stores the evaluation keys.
    @see KeyGenerator for the class that generates the Galois keys.
    */
    class GaloisKeys
    {
    public:
        /**
        Creates an empty set of Galois keys.
        */
        GaloisKeys() = default;

        /**
        Creates a new GaloisKeys instance by copying a given instance.

        @param[in] copy The GaloisKeys to copy from
        */
        GaloisKeys(const GaloisKeys &copy) = default;

        /**
        Creates a new GaloisKeys instance by moving a given instance.

        @param[in] source The GaloisKeys to move from
        */
        GaloisKeys(GaloisKeys &&source) = default;

        /**
        Copies a given GaloisKeys instance to the current one.

        @param[in] assign The GaloisKeys to copy from
        */
        GaloisKeys &operator =(const GaloisKeys &assign) = default;

        /**
        Moves a given GaloisKeys instance to the current one.

        @param[in] assign The GaloisKeys to move from
        */
        GaloisKeys &operator =(GaloisKeys &&assign) = default;

        /**
        Returns the current number of Galois keys.
        */
        inline int size() const
        {
            return std::accumulate(keys_.begin(), keys_.end(), 0,
                [](int current_size, const std::vector<Ciphertext> &next_key)
            {
                return current_size + static_cast<int>(next_key.size() > 0);
            });
        }

        /*
        Returns the decomposition bit count.
        */
        inline int decomposition_bit_count() const
        {
            return decomposition_bit_count_;
        }

        /**
        Returns a constant reference to the Galois keys data.
        */
        inline const std::vector<std::vector<Ciphertext> > &data() const
        {
            return keys_;
        }

        /**
        Returns a constant reference to a Galois key. The returned Galois key corresponds 
        to the given Galois element.

        @param[in] galois_elt The Galois element
        @throw std::invalid_argument if the key corresponding to galois_elt does not exist
        */
        inline const std::vector<Ciphertext> &key(std::uint64_t galois_elt) const
        {            
            if (!has_key(galois_elt))
            {
                throw std::invalid_argument("requested key does not exist");
            }
            std::uint64_t index = (galois_elt - 1) >> 1;
            return keys_[index];
        }

        /**
        Returns whether a Galois key corresponding to a given Galois element exists.

        @param[in] galois_elt The Galois element
        @throw std::invalid_argument if Galois element is not valid
        */
        inline bool has_key(std::uint64_t galois_elt) const
        {
            // Verify parameters
            if (!(galois_elt & 1))
            {
                throw std::invalid_argument("galois element is not valid");
            }
            std::uint64_t index = (galois_elt - 1) >> 1;
            return (index < keys_.size()) && !keys_[index].empty();
        }

        /**
        Returns a constant reference to the hash block.

        @see EncryptionParameters for more information about the hash block.
        */
        inline const EncryptionParameters::hash_block_type &hash_block() const
        {
            return hash_block_;
        }

        /**
        Saves the GaloisKeys instance to an output stream. The output is in binary format 
        and not human-readable. The output stream must have the "binary" flag set.

        @param[in] stream The stream to save the GaloisKeys to
        @see load() to load a saved GaloisKeys instance.
        */
        void save(std::ostream &stream) const;

        /**
        Loads an GaloisKeys instance from an input stream overwriting the current
        GaloisKeys instance.

        @param[in] stream The stream to load the GaloisKeys instance from
        @see save() to save an GaloisKeys instance.
        */
        void load(std::istream &stream);

        /**
        Enables access to private members of seal::GaloisKeys for .NET wrapper.
        */
        struct GaloisKeysPrivateHelper;

    private:
        /**
        Returns a reference to the vector of Galois keys. The user should never have 
        a reason to modify the Galois keys by hand.
        */
        inline std::vector<std::vector<Ciphertext> > &mutable_data()
        {
            return keys_;
        }
#ifdef SEAL_EXPOSE_MUTABLE_HASH_BLOCK
    public:
#endif
        /**
        Returns a reference to the hash block. The user should normally never have
        a reason to modify the hash block by hand.

        @see EncryptionParameters for more information about the hash block.
        */
        inline EncryptionParameters::hash_block_type &mutable_hash_block()
        {
            return hash_block_;
        }
#ifdef SEAL_EXPOSE_MUTABLE_HASH_BLOCK
    private:
#endif
        // C++11 compatibility
        EncryptionParameters::hash_block_type hash_block_{ { 0 } };

        /**
        The vector of Galois keys.
        */
        std::vector<std::vector<Ciphertext> > keys_;

        int decomposition_bit_count_ = 0;

        friend class KeyGenerator;

        friend class Evaluator;
    };
}
