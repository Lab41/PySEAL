#pragma once

#include <iostream>
#include <vector>
#include "bigpolyarray.h"
#include "encryptionparams.h"

namespace seal 
{
    /**
    Class to store evaluation keys. Internally, each instance stores a std::vector of objects 
    of type std::pair<BigPolyArray, BigPolyArray>, where the evaluation keys are stored. Each 
    entry of the vector is called an evaluation key. Each evaluation key is associated to a 
    particular power of the secret key, and can be repeatedly used by the relinearization 
    operation typically performed after homomorphic multiplication. The evaluation keys can be
    saved and loaded from a stream with the save() and load() functions.

    @par Thread Safety
    In general, reading from EvaluationKeys is thread-safe as long as no other thread is concurrently
    mutating it. This is due to the underlying data structure storing the evaluation keys not being 
    thread-safe.

    @see SecretKey for the class that stores the secret key.
    @see PublicKey for the class that stores the public key.
    @see KeyGenerator for the class that generates the evaluation keys.
    */
    class EvaluationKeys 
    {
    public:
        /**
        Creates an empty set of evaluation keys. No memory is allocated by this 
        constructor.
        */
        EvaluationKeys() : keys_{}, hash_block_{ 0 }
        {
        }

        /**
        Creates a new EvaluationKeys by copying an old one.

        @param[in] copy The EvaluationKeys to copy from
        */
        EvaluationKeys(const EvaluationKeys &copy) = default;

        /**
        Creates a new EvaluationKeys by moving an old one.

        @param[in] source The EvaluationKeys to move from
        */
        EvaluationKeys(EvaluationKeys &&source) = default;

        /**
        Copies an old EvaluationKeys to the current one.

        @param[in] assign The EvaluationKeys to copy from
        */
        EvaluationKeys &operator =(const EvaluationKeys &assign) = default;

        /**
        Moves an old EvaluationKeys to the current one.

        @param[in] assign The EvaluationKeys to move from
        */
        EvaluationKeys &operator =(EvaluationKeys &&assign) = default;

        /**
        Returns the current number of evaluation keys.
        */
        int size() const
        {
            return keys_.size();
        }

        /**
        Saves the EvaluationKeys instance to an output stream. The output is in binary 
        format and not human-readable. The output stream must have the "binary" flag set.

        @param[in] stream The stream to save the EvaluationKeys to
        @see load() to load a saved EvaluationKeys instance.
        */
        void save(std::ostream &stream) const;
        
        /**
        Loads an EvaluationKeys instance from an input stream overwriting the current 
        EvaluationKeys instance.

        @param[in] stream The stream to load the EvaluationKeys instance from
        @see save() to save an EvaluationKeys instance.
        */
        void load(std::istream &stream);
        
        /**
        Returns a const reference to the vector of evaluation keys.
        */
        const std::vector<std::pair<BigPolyArray, BigPolyArray> > &get_data() const
        {
            return keys_;
        }

        /**
        Returns a constant reference to the hash block.

        @see EncryptionParameters for more information about the hash block.
        */
        inline const EncryptionParameters::hash_block_type &get_hash_block() const
        {
            return hash_block_;
        }

        /**
        Enables access to private members of seal::EvaluationKeys for .NET wrapper.
        */
        struct EvaluationKeysPrivateHelper;
    
    private:
        /**
        Returns a reference to the vector of evaluation keys. The user should never 
        have a reason to modify the evaluation keys by hand.
        */
        inline std::vector<std::pair<BigPolyArray, BigPolyArray> > &get_mutable_data()
        {
            return keys_;
        }

        /**
        Returns a reference to the hash block. The user should never have a reason to
        modify the hash block by hand.

        @see EncryptionParameters for more information about the hash block.
        */
        inline EncryptionParameters::hash_block_type &get_mutable_hash_block()
        {
            return hash_block_;
        }

        /**
        The vector of evaluation keys (each of type std::pair<BigPolyArray, BigPolyArray>).
        Each key corresponds to one particular power of the secret key whose influence in
        a ciphertext is removed using the relinearize() function of Evaluator.
        */
        std::vector<std::pair<BigPolyArray, BigPolyArray> > keys_;

        EncryptionParameters::hash_block_type hash_block_;

        friend class KeyGenerator;

        friend class Evaluator;

        friend class RNSKeyGenerator;
    };
}