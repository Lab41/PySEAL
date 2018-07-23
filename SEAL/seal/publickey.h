#pragma once

#include "seal/bigpolyarray.h"
#include "seal/encryptionparams.h"
#include <iostream>
#include <fstream>

namespace seal
{
    /**
    Class to store a public key. Internally, the public key is represented by 
    a BigPolyArray object, and is created by KeyGenerator.

    @par Thread Safety
    In general, reading from PublicKey is thread-safe as long as no other thread
    is concurrently mutating it. This is due to the underlying data structure 
    storing the public key not being thread-safe.

    @see KeyGenerator for the class that generates the public key.
    @see SecretKey for the class that stores the secret key.
    @see EvaluationKeys for the class that stores the evaluation keys.
    @see GaloisKeys for the class that stores the Galois keys.
    */
    class PublicKey
    {
    public:
        /**
        Creates an empty public key.
        */
        PublicKey() = default;

        /**
        Creates a new PublicKey by copying an old one.

        @param[in] copy The PublicKey to copy from
        */
        PublicKey(const PublicKey &copy) = default;

        /**
        Creates a new PublicKey by moving an old one.

        @param[in] source The PublicKey to move from
        */
        PublicKey(PublicKey &&source) = default;

        /**
        Copies an old PublicKey to the current one.

        @param[in] assign The PublicKey to copy from
        */
        PublicKey &operator =(const PublicKey &assign) = default;

        /**
        Moves an old PublicKey to the current one.

        @param[in] assign The PublicKey to move from
        */
        PublicKey &operator =(PublicKey &&assign) = default;

        /**
        Returns a constant reference to the underlying BigPolyArray.
        */
        inline const BigPolyArray &data() const
        {
            return pk_array_;
        }

        /**
        Saves the PublicKey to an output stream. The output is in binary format
        and not human-readable. The output stream must have the "binary" flag set.

        @param[in] stream The stream to save the PublicKey to
        */
        void save(std::ostream &stream) const
        {
            stream.write(reinterpret_cast<const char*>(&hash_block_), 
                sizeof(EncryptionParameters::hash_block_type));
            pk_array_.save(stream);
        }
        void python_save(std::string &path) const
        {
            std::ofstream out(path);
            save(out);
            out.close();
        }

        /**
        Loads a PublicKey from an input stream overwriting the current PublicKey.

        @param[in] stream The stream to load the PublicKey from
        */
        void load(std::istream &stream)
        {
            stream.read(reinterpret_cast<char*>(&hash_block_), 
                sizeof(EncryptionParameters::hash_block_type));
            pk_array_.load(stream);
        }
        void python_load(std::string &path)
        {
            std::ifstream in(path);
            load(in);
            in.close();
        }

        /**
        Returns a constant reference to the hash block.
        */
        inline const EncryptionParameters::hash_block_type &hash_block() const
        {
            return hash_block_;
        }

        /**
        Enables access to private members of seal::PublicKey for .NET wrapper.
        */
        struct PublicKeyPrivateHelper;

    private:
        /**
        Returns a reference to the underlying BigPolyArray. The user should never 
        have a reason to modify the public key by hand.
        */
        inline BigPolyArray &mutable_data()
        {
            return pk_array_;
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

        BigPolyArray pk_array_;

        friend class KeyGenerator;

        friend class Encryptor;

        friend class KeyGenerator;

        friend class Encryptor;
    };
}
