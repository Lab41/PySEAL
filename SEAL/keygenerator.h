#ifndef SEAL_KEYGENERATOR_H
#define SEAL_KEYGENERATOR_H

#include <memory>
#include "encryptionparams.h"
#include "evaluationkeys.h"
#include "util/mempool.h"
#include "util/modulus.h"
#include "util/polymodulus.h"

namespace seal
{
    /**
    Generates matching secret key, public key, and evaluation keys for encryption, decryption, and evaluation functions.
    Constructing a KeyGenerator requires the encryption parameters (set through an EncryptionParameters object). Invoking
    the generate() function will generate a new secret key (which can be read from secret_key()), public key (which can
    be read from public_key()), and evaluation keys (which can be read from evaluation_keys()). Invoking the
    generate(const BigPoly&, int) variant will generate new public key and evaluation keys for a specified secret key,
    raised to an optional power.

    @warning The generate() functions are not thread safe and a separate KeyGenerator instance is needed for each potentially
    concurrent key generation operation.
    */
    class KeyGenerator
    {
    public:
        /**
        Creates a KeyGenerator instance initialized with the specified encryption parameters.

        @param[in] parms The encryption parameters
        @throws std::invalid_argument if encryption parameters are not valid
        @see EncryptionParameters for more details on valid encryption parameters.
        */
        KeyGenerator(const EncryptionParameters &parms);

        /**
        Generates new matching secret key, public key, and evaluation keys.

        @warning generate() is not thread safe.
        @see secret_key() to read generated secret key.
        @see public_key() to read generated public key.
        @see evaluation_keys() to read generated evaluation keys.
        */
        void generate();

        /**
        Generates new matching public key and evaluation keys for a specified secret key, raised to an optional power. The
        optional power parameter raises the secret key to the specified power prior to generating the keys. Raising the secret
        key to a power enables more efficient operations in some advanced cases. Note that due to randomness during the
        generation process, the generated public key and evaluation keys may not match prior generated public key and
        evaluation keys for the same secret key.

        @warning generate(const BigPoly&, int) is not thread safe.
        @param[in] secret_key The secret key
        @param[in] power The power to raise the secret key to, defaults to 1
        @throws std::invalid_argument if secret key is not valid
        @throws std::invalid_argument if power is not positive
        @see secret_key() to read the specified secret key, raised to the specified power.
        @see public_key() to read generated public key.
        @see evaluation_keys() to read generated evaluation keys.
        */
        void generate(const BigPoly &secret_key, int power = 1);

        /**
        Returns the generated secret key after a generate() invocation, or the specified secret key raised to the specified
        power after a generate(const BigPoly&, int) invocation.
        */
        const BigPoly &secret_key() const
        {
            return secret_key_;
        }

        /**
        Returns the generated public key after a generate() invocation.
        */
        const BigPoly &public_key() const
        {
            return public_key_;
        }

        /**
        Returns the generated evaluation keys after a generate() invocation.
        */
        const EvaluationKeys &evaluation_keys() const
        {
            return evaluation_keys_;
        }

    private:
        KeyGenerator(const KeyGenerator &copy) = delete;

        KeyGenerator &operator =(const KeyGenerator &assign) = delete;

        void set_poly_coeffs_zero_one_negone(uint64_t *poly) const;

        void set_poly_coeffs_normal(uint64_t *poly) const;

        BigPoly poly_modulus_;

        BigUInt coeff_modulus_;

        BigUInt plain_modulus_;

        BigUInt coeff_modulus_minus_one_;

        BigPoly public_key_;

        BigPoly secret_key_;

        double noise_standard_deviation_;

        double noise_max_deviation_;

        int decomposition_bit_count_;

        EncryptionMode mode_;

        std::unique_ptr<UniformRandomGenerator> random_generator_;

        EvaluationKeys evaluation_keys_;

        util::MemoryPool pool_;

        util::PolyModulus polymod_;

        util::Modulus mod_;
    };
}

#endif // SEAL_KEYGENERATOR_H
