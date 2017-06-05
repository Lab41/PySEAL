#pragma once

#include <utility>
#include "encryptionparams.h"
#include "util/modulus.h"
#include "util/polymodulus.h"
#include "bigpolyarray.h"
#include "util/ntt.h"
#include "memorypoolhandle.h"

namespace seal
{
    /**
    Decrypts BigPolyArray objects into BigPoly objects. Constructing an Decryptor requires the encryption
    parameters (set through an EncryptionParameters object) and the secret key. The public and evaluation keys 
    are not needed for decryption.
    */
    class Decryptor
    {
    public:
        /**
        Creates an Decryptor instance initialized with the specified encryption parameters and secret key. 
        Optionally, the user can give a reference to a MemoryPoolHandle object to use a custom memory pool
        instead of the global memory pool (default).

        @param[in] parms The encryption parameters
        @param[in] secret_key The secret key
        @param[in] pool The memory pool handle
        @throws std::invalid_argument if encryption parameters or secret key are not valid
        @see EncryptionParameters for more details on valid encryption parameters.
        @see MemoryPoolHandle for more details on memory pool handles.
        */
        Decryptor(const EncryptionParameters &parms, const BigPoly &secret_key, 
            const MemoryPoolHandle &pool = MemoryPoolHandle::acquire_global());

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
        Decrypts an FV ciphertext and stores the result in the destination parameter. 
        
        @param[in] encrypted The ciphertext to decrypt
        @param[out] destination The plaintext to overwrite with the decrypted ciphertext
        @throws std::invalid_argument if the ciphertext is not a valid ciphertext for the encryption parameters
        @throws std::logic_error If destination is an alias but needs to be resized
        */
        void decrypt(const BigPolyArray &encrypted, BigPoly &destination);

        /**
        Decrypts an BigPolyArray and returns the result.

        @param[in] encrypted The ciphertext to decrypt
        @throws std::invalid_argument if the ciphertext is not a valid ciphertext for the encryption parameters
        */
        BigPoly decrypt(const BigPolyArray &encrypted)
        {
            BigPoly result;
            decrypt(encrypted, result);
            return result;
        }

        /**
        Returns the secret key used by the Decryptor.
        */
        const BigPoly &secret_key() const
        {
            return secret_key_;
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
        int invariant_noise_budget(const BigPolyArray &encrypted);

        /**
        Computes and returns the number of bits of inherent noise in a ciphertext. The user can easily compare
        this with the maximum possible value returned by the function EncryptionParameters::inherent_noise_bits_max().
        Instead of this function, consider using invariant_noise_budget(), which has several subtle advantages.

        @param[in] encrypted The ciphertext
        @throws std::invalid_argument if encrypted is not a valid ciphertext for the encryption parameters
        @see inherent_noise for computing the exact size of inherent noise.
        */
        int inherent_noise_bits(const BigPolyArray &encrypted)
        {
            BigUInt result;
            inherent_noise(encrypted, result);
            return result.significant_bit_count();
        }

        /**
        Computes the inherent noise in a ciphertext. The result is written in a BigUInt given as a parameter.
        The user can easily compare this with the maximum possible value returned by the function
        EncryptionParameters::inherent_noise_max(). It is often easier to analyze the size of the inherent
        noise by using the functions inherent_noise_bits() and EncryptionParameters::inherent_noise_max().
        Instead of this function, consider using invariant_noise_budget(), which has several subtle advantages.

        @param[in] encrypted The ciphertext
        @param[out] destination The BigUInt to overwrite with the inherent noise
        @throws std::invalid_argument if encrypted is not a valid ciphertext for the encryption parameters
        @see inherent_noise_bits for returning the significant bit count of the inherent noise instead.
        */
        void inherent_noise(const BigPolyArray &encrypted, BigUInt &destination);

    private:
        Decryptor &operator =(const Decryptor &assign) = delete;

        Decryptor &operator =(Decryptor &&assign) = delete;

        void compute_secret_key_array(int max_power);

        MemoryPoolHandle pool_;

        BigPoly poly_modulus_;

        BigUInt coeff_modulus_;

        BigUInt plain_modulus_;

        BigUInt upper_half_threshold_;

        BigUInt upper_half_increment_;

        BigUInt coeff_div_plain_modulus_;

        BigUInt coeff_div_plain_modulus_div_two_;

        BigPoly secret_key_;

        int orig_plain_modulus_bit_count_;

        util::PolyModulus polymod_;

        util::Modulus mod_;

        util::NTTTables ntt_tables_;

        BigPolyArray secret_key_array_;

        EncryptionParameterQualifiers qualifiers_;
    };
}