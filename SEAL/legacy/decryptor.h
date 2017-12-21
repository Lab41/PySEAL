#pragma once

#include <utility>
#include "encryptionparams.h"
#include "context.h"
#include "util/modulus.h"
#include "util/polymodulus.h"
#include "util/ntt.h"
#include "memorypoolhandle.h"
#include "ciphertext.h"
#include "plaintext.h"
#include "secretkey.h"

namespace seal
{
    /**
    Decrypts Ciphertext objects into Plaintext objects. Constructing an Decryptor requires the encryption
    parameters (set through a SEALContext object) and the secret key. The public and evaluation keys 
    are not needed for decryption.
    */
    class Decryptor
    {
    public:
        /**
        Creates an Decryptor instance initialized with the specified SEALContext and secret key. 
        Optionally, the user can give a reference to a MemoryPoolHandle object to use a custom memory 
        pool instead of the global memory pool (default).

        @param[in] context The SEALContext
        @param[in] secret_key The secret key
        @param[in] pool The memory pool handle
        @throws std::invalid_argument if encryption parameters or secret key are not valid
        @see SEALContext for more details on valid encryption parameters.
        @see MemoryPoolHandle for more details on memory pool handles.
        */
        Decryptor(const SEALContext &context, const SecretKey &secret_key, 
            const MemoryPoolHandle &pool = MemoryPoolHandle::Global());

        /**
        Creates a copy of a Decryptor.

        @param[in] copy The Decryptor to copy from
        */
        Decryptor(const Decryptor &copy);

        /**
        Creates a new Decryptor by moving an old one.

        @param[in] source The Decryptor to move from
        */
        Decryptor(Decryptor &&source) = default;

        /*
        Decrypts a Ciphertext and stores the result in the destination parameter. 
        
        @param[in] encrypted The ciphertext to decrypt
        @param[out] destination The plaintext to overwrite with the decrypted ciphertext
        @throws std::invalid_argument if the ciphertext is not a valid ciphertext for the encryption parameters
        */
        void decrypt(const Ciphertext &encrypted, Plaintext &destination);

        /**
        Decrypts a Ciphertext and returns the result.

        @param[in] encrypted The ciphertext to decrypt
        @throws std::invalid_argument if the ciphertext is not a valid ciphertext for the encryption parameters
        */
        Plaintext decrypt(const Ciphertext &encrypted)
        {
            Plaintext result;
            decrypt(encrypted, result);
            return result;
        }

        /*
        Computes the invariant noise budget (in bits) of a ciphertext. The invariant noise budget
        measures the amount of room there is for the noise to grow while ensuring correct decryptions.

        @par Invariant Noise Budget
        The invariant noise polynomial of a ciphertext is a rational coefficient polynomial, such that
        a ciphertext decrypts correctly as long as the coefficients of the invariant noise polynomial are 
        of absolute value less than 1/2. Thus, we call the infinity-norm of the invariant noise polynomial 
        the invariant noise, and for correct decryption require it to be less than 1/2. If v denotes the 
        invariant noise, we define the invariant noise budget as -log2(2v). Thus, the invariant noise budget
        starts from some initial value, which depends on the encryption parameters, and decreases to 0 when
        computations are performed. When the budget reaches 0, the ciphertext becomes too noisy to decrypt 
        correctly. 
        
        @param[in] encrypted The ciphertext
        @throws std::invalid_argument if encrypted is not a valid ciphertext for the encryption parameters
        */
        int invariant_noise_budget(const Ciphertext &encrypted);

    private:
        Decryptor &operator =(const Decryptor &assign) = delete;

        Decryptor &operator =(Decryptor &&assign) = delete;

        void compute_secret_key_array(int max_power);

        MemoryPoolHandle pool_;

        const EncryptionParameters parms_;

        util::Pointer upper_half_threshold_;

        util::Pointer upper_half_increment_;

        util::Pointer coeff_div_plain_modulus_;

        util::Pointer coeff_div_plain_modulus_div_two_;

        util::Pointer secret_key_;

        util::PolyModulus polymod_;

        util::Modulus mod_;

        util::NTTTables ntt_tables_;

        BigPolyArray secret_key_array_;

        EncryptionParameterQualifiers qualifiers_;
    };
}