#ifndef SEAL_DECRYPTOR_H
#define SEAL_DECRYPTOR_H

#include "encryptionparams.h"
#include "util/mempool.h"
#include "util/modulus.h"
#include "util/polymodulus.h"

namespace seal
{
    /**
    Decrypts encrypted polynomials (represented by BigPoly) into plain-text polynomials. Constructing a Decryptor requires the encryption
    parameters (set through an EncryptionParameters object) and the secret key polynomial. The public and evaluation keys are not needed
    for decryption.

    @warning The decrypt() function is not thread safe and a separate Decryptor instance is needed for each potentially concurrent decrypt
    operation.
    */
    class Decryptor
    {
    public:
        /**
        Creates a Decryptor instance initialized with the specified encryption parameters and secret key. The optional power parameter
        raises the secret key to the specified power during initialization. Raising the secret key to a power enables more efficient
        operations in some advanced cases.

        @param[in] parms The encryption parameters
        @param[in] secret_key The secret key
        @param[in] power The power to raise the secret key to, defaults to 1
        @throws std::invalid_argument if encryption parameters or secret key are not valid
        @throws std::invalid_argument if power is not positive
        @see EncryptionParameters for more details on valid encryption parameters.
        */
        Decryptor(const EncryptionParameters &parms, const BigPoly &secret_key, int power = 1);

        /**
        Decrypts an encrypted polynomial and stores the result in the destination parameter. The destination parameter is resized if
        and only if its coefficient count or coefficient bit count does not match the encryption parameters.

        @warning decrypt() is not thread safe.
        @param[in] encrypted The encrypted polynomial to decrypt
        @param[out] destination The polynomial to overwrite with the plain-text polynomial
        @throws std::invalid_argument if the encrypted polynomial is not a valid encrypted polynomial for the encryption parameters
        @throws std::logic_error If destination is an alias but needs to be resized
        */
        void decrypt(const BigPoly &encrypted, BigPoly &destination);

        /**
        Decrypts an encrypted polynomial and returns the result.

        @warning decrypt() is not thread safe.
        @param[in] encrypted The encrypted polynomial to decrypt
        @throws std::invalid_argument if the encrypted polynomial is not a valid encrypted polynomial for the encryption parameters
        */
        BigPoly decrypt(const BigPoly &encrypted)
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

    private:
        Decryptor(const Decryptor &copy) = delete;

        Decryptor &operator =(const Decryptor &assign) = delete;

        BigPoly poly_modulus_;

        BigUInt coeff_modulus_;

        BigUInt plain_modulus_;

        BigUInt upper_half_threshold_;

        BigUInt upper_half_increment_;

        BigUInt coeff_div_plain_modulus_;

        BigUInt coeff_div_plain_modulus_div_two_;

        BigPoly secret_key_;

        EncryptionMode mode_;

        util::MemoryPool pool_;

        util::PolyModulus polymod_;

        util::Modulus mod_;
    };
}

#endif // SEAL_DECRYPTOR_H
