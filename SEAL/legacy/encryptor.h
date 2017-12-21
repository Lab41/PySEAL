#pragma once

#include <memory>
#include <utility>
#include "encryptionparams.h"
#include "util/modulus.h"
#include "util/polymodulus.h"
#include "bigpolyarray.h"
#include "plaintext.h"
#include "ciphertext.h"
#include "util/ntt.h"
#include "memorypoolhandle.h"
#include "context.h"
#include "publickey.h"

namespace seal
{
    /**
    Encrypts Plaintext objects into Ciphertext objects. Constructing an Encryptor requires 
    the encryption parameters (set through an SEALContext object) and the public 
    key. The secret and evaluation keys are not needed for encryption.
    */
    class Encryptor
    {
    public:
        /**
        Creates an Encryptor instances initialized with the specified SEALContext
        and public key. Optionally, the user can give a reference to a MemoryPoolHandle 
        object to use a custom memory pool instead of the global memory pool (default).

        @param[in] context The SEALContext
        @param[in] public_key The public key
        @param[in] pool The memory pool handle
        @throws std::invalid_argument if encryption parameters or public key are not valid
        @see SEALContext for more details on valid encryption parameters.
        @see MemoryPoolHandle for more details on memory pool handles.
        */
        Encryptor(const SEALContext &context, const PublicKey &public_key, 
            const MemoryPoolHandle &pool = MemoryPoolHandle::Global());

        /**
        Creates a copy of a Encryptor.

        @param[in] copy The Encryptor to copy from
        */
        Encryptor(const Encryptor &copy);

        /**
        Creates a new Encryptor by moving an old one.

        @param[in] source The Encryptor to move from
        */
        Encryptor(Encryptor &&source) = default;

        /**
        Encrypts a plaintext and stores the result in the destination parameter. The 
        destination parameter is resized if and only if its coefficient count or 
        coefficient bit count does not match the encryption parameters. The plaintext 
        polynomial must have a significant coefficient count smaller than the coefficient 
        count specified by the encryption parameters, and with coefficient values less-than 
        the plain modulus (SEALContext::plain_modulus()).

        @param[in] plain The plaintext to encrypt
        @param[out] destination The ciphertext to overwrite with the encrypted plaintext
        @throws std::invalid_argument if the plaintext polynomial's significant coefficient 
        count or coefficient values are too large to represent with the encryption parameters
        */
        void encrypt(const Plaintext &plain, Ciphertext &destination);

        /**
        Encrypts a plaintext and returns the result. The plaintext polynomial must have 
        a significant coefficient count smaller than the coefficient count specified by 
        the encryption parameters, and with coefficient values less-than the plain modulus 
        (SEALContext::plain_modulus()).

        @param[in] plain The plaintext to encrypt
        @throws std::invalid_argument if the plaintext polynomial's significant coefficient 
        count or coefficient values are too large to represent with the encryption parameters
        */
        Ciphertext encrypt(const Plaintext &plain)
        {
            Ciphertext result;
            encrypt(plain, result);
            return result;
        }

    private:
        Encryptor &operator =(const Encryptor &assign) = delete;

        Encryptor &operator =(Encryptor &&assign) = delete;

        void preencrypt(const std::uint64_t *plain, int plain_coeff_count, int plain_coeff_uint64_count, std::uint64_t *destination);
        
        void set_poly_coeffs_normal(std::uint64_t *poly, UniformRandomGenerator *random) const;
        
        void set_poly_coeffs_zero_one_negone(uint64_t *poly, UniformRandomGenerator *random) const;

        void set_poly_coeffs_zero_one(uint64_t *poly, UniformRandomGenerator *random) const;

        MemoryPoolHandle pool_;

        const EncryptionParameters parms_;
        
        util::Pointer upper_half_threshold_;
        
        util::Pointer upper_half_increment_;
        
        util::Pointer coeff_div_plain_modulus_;
        
        util::Pointer public_key_;
        
        util::PolyModulus polymod_;
        
        util::Modulus mod_;

        util::NTTTables ntt_tables_; 

        EncryptionParameterQualifiers qualifiers_;
    };

}