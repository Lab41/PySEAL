#ifndef SEAL_EVALUATOR_H
#define SEAL_EVALUATOR_H

#include <vector>
#include "encryptionparams.h"
#include "evaluationkeys.h"
#include "util/modulus.h"
#include "util/polymodulus.h"

namespace seal
{
    /**
    Provides arithmetic functions for operating on ciphertexts. The add, subtract, and multiply function
    variants allow both operands to be encrypted. The "_plain" variants allow one of the inputs to be 
    encrypted and the other unencrypted.

    Every valid ciphertext consists of at least two polynomials. Homomorphic multiplication increases 
    the size of the ciphertext in such a way that if the input ciphertexts have size M and N, then the
    output ciphertext will have size M+N-1. The multiplication operation will require M*N polynomial
    multiplications to be performed. To read the current size of a ciphertext the user can use
    BigPolyArray::size().
    
    A relinearization operation can be used to reduce the size of a ciphertext to any smaller size
    (but at least 2), potentially improving the performance of a subsequent multiplication using it.
    However, relinearization grows the inherent noise in a ciphertext by an additive factor proportional
    to 2^DBC, and relinearizing from size K to L will require 2*(K-L)*[floor(log_2(coeff_modulus)/DBC)+1]
    polynomial multiplications. Here DBC denotes the decomposition bit count set in the encryption parameters. 
    Note that the larger the decomposition bit count is, the faster relinearization will be, but also the
    inherent noise growth will be larger. In typical cases, however, the user might not want to relinearize
    at all as even the computational cost of a relinearization might be larger than what might be gained
    from a subsequent multiplication with a ciphertext of smaller size.

    Relinearization requires the key generator to generate evaluation keys. More specifically, to relinearize
    a ciphertext of size K down to any size smaller than K (but at least 2), at least K-2 evaluation keys will
    be needed. These have to be given as an input parameter to the constructor of Evaluator.

    @par Inherent Noise
    Technically speaking, the inherent noise of a ciphertext is a polynomial, but the condition for decryption working
    depends on the size of the largest absolute value of its coefficients. This is what we will call the "noise", the 
    "inherent noise", or the "error", in this documentation. Typically multiplication will be the most heavy operation 
    in terms of noise growth. The reader is referred to the description of the encryption scheme for more details.
    */
    class Evaluator
    {
    public:
        /**
        Creates an Evaluator instance initialized with the specified encryption parameters and
        evaluation keys. If no evaluation keys will be needed, one can simply pass a newly 
        created empty instance of EvaluationKeys to the function.

        @param[in] parms The encryption parameters
        @param[in] evaluation_keys The evaluation keys
        @throws std::invalid_argument if encryption parameters or evaluation keys are not valid
        @see EncryptionParameters for more details on valid encryption parameters.
        */
        Evaluator(const EncryptionParameters &parms, const EvaluationKeys &evaluation_keys);

        /**
        Creates an Evaluator instance initialized with the specified encryption parameters.

        @param[in] parms The encryption parameters
        @throws std::invalid_argument if encryption parameters are not valid
        @see EncryptionParameters for more details on valid encryption parameters.
        */
        Evaluator(const EncryptionParameters &parms) : Evaluator(parms, EvaluationKeys())
        {
        }

        /**
        Negates a ciphertext and stores the result in the destination parameter.

        @param[in] encrypted The ciphertext to negate
        @param[out] destination The ciphertext to overwrite with the negated result
        @throws std::invalid_argument if the ciphertext is not valid for the encryption parameters
        */
        void negate(const BigPolyArray &encrypted, BigPolyArray &destination);

        /**
        Negates a ciphertext and returns the result.

        @param[in] encrypted The ciphertext to negate
        @throws std::invalid_argument if the ciphertext is not valid for the encryption parameters
        */
        BigPolyArray negate(const BigPolyArray &encrypted)
        {
            BigPolyArray result;
            negate(encrypted, result);
            return result;
        }

        /**
        Adds two ciphertexts and stores the result in the destination parameter.

        @param[in] encrypted1 The first ciphertext to add
        @param[in] encrypted2 The second ciphertext to add
        @param[out] destination The ciphertext to overwrite with the addition result
        @throws std::invalid_argument if the ciphertexts are not valid for the encryption parameters
        */
        void add(const BigPolyArray &encrypted1, const BigPolyArray &encrypted2, BigPolyArray &destination);

        /**
        Adds two ciphertexts and returns the result.

        @param[in] encrypted1 The first ciphertext to add
        @param[in] encrypted2 The second ciphertext to add
        @throws std::invalid_argument if the ciphertexts are not valid for the encryption parameters
        */
        BigPolyArray add(const BigPolyArray &encrypted1, const BigPolyArray &encrypted2)
        {
            BigPolyArray result;
            add(encrypted1, encrypted2, result);
            return result;
        }

        /**
        Adds together an number of ciphertexts stored as elements of std::vector<BigPolyArray>
        and stores the result in the destination parameter.

        @param[in] encrypteds The ciphertexts to add
        @param[out] destination The ciphertext to overwrite with the addition result
        @throws std::invalid_argument if encrypteds is empty
        @throws std::invalid_argument if the ciphertexts are not valid for the encryption parameters
        */
        void add_many(const std::vector<BigPolyArray> &encrypteds, BigPolyArray &destination);

        /**
        Adds together an number of ciphertexts stored as elements of std::vector<BigPolyArray>
        and returns the result.

        @param[in] encrypteds The ciphertexts to add
        @throws std::invalid_argument if encrypteds is empty
        @throws std::invalid_argument if the ciphertexts are not valid for the encryption parameters
        */
        BigPolyArray add_many(const std::vector<BigPolyArray> &encrypteds)
        {
            BigPolyArray result;
            add_many(encrypteds, result);
            return result;
        }

        /**
        Subtracts two ciphertexts and stores the result in the destination parameter. 

        @param[in] encrypted1 The ciphertext to subtract from
        @param[in] encrypted2 The ciphertext to subtract
        @param[out] destination The ciphertext to overwrite with the subtraction result
        @throws std::invalid_argument if the ciphertexts are not valid for the encryption parameters
        */
        void sub(const BigPolyArray &encrypted1, const BigPolyArray &encrypted2, BigPolyArray &destination);

        /**
        Subtracts two ciphertexts and returns the result.

        @param[in] encrypted1 The ciphertext to subtract from
        @param[in] encrypted2 The ciphertext to subtract
        @throws std::invalid_argument if the ciphertexts are not valid for the encryption parameters
        */
        BigPolyArray sub(const BigPolyArray &encrypted1, const BigPolyArray &encrypted2)
        {
            BigPolyArray result;
            sub(encrypted1, encrypted2, result);
            return result;
        }

        /**
        Multiplies two ciphertexts and stores the result in the destination parameter. 

        @param[in] encrypted1 The first ciphertext to multiply
        @param[in] encrypted2 The second ciphertext to multiply
        @param[out] destination The ciphertext to overwrite with the multiplication result
        @throws std::invalid_argument if the ciphertexts are not valid for the encryption parameters
        */
        void multiply(const BigPolyArray &encrypted1, const BigPolyArray &encrypted2, BigPolyArray &destination);

        /**
        Multiplies two ciphertexts without performing relinearization, and returns the result.

        @param[in] encrypted1 The first ciphertext to multiply
        @param[in] encrypted2 The second ciphertext to multiply
        @throws std::invalid_argument if the ciphertexts are not valid for the encryption parameters
        */
        BigPolyArray multiply(const BigPolyArray &encrypted1, const BigPolyArray &encrypted2)
        {
            BigPolyArray result;
            multiply(encrypted1, encrypted2, result);
            return result;
        }

        /**
        Relinearizes a ciphertext and stores the result in the destination parameter.

        @param[in] encrypted The ciphertext to relinearize
        @param[in] destination_size The size of the output ciphertext (defaults to 2)
        @param[out] destination The ciphertext to overwrite with the relinearized result
        @throws std::invalid_argument if the ciphertext is not valid for the encryption parameters
        @throws std::invalid_argument if destination_size is less than 2 or greater than number of elements currently in ciphertext
        @throws std::invalid_argument if not enough evaluation keys have been generated
        */
        void relinearize(const BigPolyArray &encrypted, BigPolyArray &destination, int destination_size = 2);

        /**
        Relinearizes a ciphertext and returns the result.

        @param[in] encrypted The ciphertext to relinearize
        @param[in] destination_size The size of the output ciphertext (defaults to 2)
        @throws std::invalid_argument if the ciphertext is not valid for the encryption parameters
        @throws std::invalid_argument if destination_size is less than 2 or greater than number of elements currently in ciphertext
        @throws std::invalid_argument if not enough evaluation keys have been generated
        */
        BigPolyArray relinearize(const BigPolyArray &encrypted, int destination_size = 2)
        {
            BigPolyArray result;
            relinearize(encrypted, result, destination_size);
            return result;
        }

        /**
        Multiplies a vector of ciphertexts together and returns the result.

        @param[in] encrypteds The vector of ciphertexts to multiply
        @throws std::invalid_argument if the encrypteds vector is empty
        @throws std::invalid_argument if the ciphertexts are not valid ciphertexts for the encryption parameters
        */
        BigPolyArray multiply_many(std::vector<BigPolyArray> encrypteds);

        /**
        Multiplies a vector of ciphertexts together and stores the result in the destination parameter. 

        @param[in] encrypteds The vector of ciphertexts to multiply
        @param[out] destination The ciphertext to overwrite with the multiplication result
        @throws std::invalid_argument if the encrypteds vector is empty
        @throws std::invalid_argument if the ciphertexts are not valid ciphertexts for the encryption parameters
        */
        void multiply_many(std::vector<BigPolyArray> &encrypteds, BigPolyArray &destination)
        {
            destination = multiply_many(encrypteds);
        }

        /**
        Raises a ciphertext to the specified power and stores the result in the destination parameter. 
        
        Exponentiation to power 0 is not allowed and will result in the library throwing an invalid argument
        exception. The reason behind this design choice is that the result should be a fresh encryption
        of 1, but creating fresh encryptions should not be something this class does. Instead the user
        should separately handle the cases where the exponent is 0.

        @param[in] encrypted The ciphertext to raise to a power
        @param[in] exponent The power to raise the ciphertext to
        @param[out] destination The ciphertext to overwrite with the exponentiation result
        @throws std::invalid_argument if the ciphertext is not valid for the encryption parameters
        @throws std::invalid_argument if the exponent is zero
        */
        void exponentiate(const BigPolyArray &encrypted, std::uint64_t exponent, BigPolyArray &destination);

        /**
        Raises a ciphertext to the specified power and returns the result. 
        
        Exponentiation to power 0 is not allowed and will result in the library throwing 
        an invalid argument exception. The reason behind this design choice is that the 
        result should be a fresh encryption of 1, but creating fresh encryptions should 
        not be something this class does. Instead the user has to separately handle 
        the cases where the exponent is 0.

        @param[in] encrypted The ciphertext to raise to a power
        @param[in] exponent The power to raise the ciphertext to
        @throws std::invalid_argument if the ciphertext is not valid for the encryption parameters
        @throws std::invalid_argument if the exponent is zero
        */
        BigPolyArray exponentiate(const BigPolyArray &encrypted, std::uint64_t exponent)
        {
            BigPolyArray result;
            exponentiate(encrypted, exponent, result);
            return result;
        }

        /**
        Adds a ciphertext with a plaintext, and stores the result in the destination
        parameter. The plaintext must have a significant coefficient count smaller than 
        the coefficient count specified by the encryption parameters, and with
        coefficient values less than the plain modulus (EncryptionParameters::plain_modulus()).

        @param[in] encrypted The ciphertext to add
        @param[in] plain The plaintext to add
        @param[out] destination The ciphertext to overwrite with the addition result
        @throws std::invalid_argument if the ciphertexts are not valid for the encryption parameters
        @throws std::invalid_argument if the plaintext's significant coefficient count or coefficient
        values are too large to represent with the encryption parameters
        */
        void add_plain(const BigPolyArray &encrypted, const BigPoly &plain, BigPolyArray &destination);

        /**
        Adds a ciphertext with a plaintext, and returns the result. The plaintext
        must have a significant coefficient count smaller than the coefficient count specified by the
        encryption parameters, and with coefficient values less than the plain modulus
        (EncryptionParameters::plain_modulus()).

        @param[in] encrypted The ciphertext to add
        @param[in] plain The plaintext to add
        @throws std::invalid_argument if the ciphertexts are not valid for the encryption parameters
        @throws std::invalid_argument if the plaintext's significant coefficient count or coefficient
        values are too large to represent with the encryption parameters
        */
        BigPolyArray add_plain(const BigPolyArray &encrypted, const BigPoly &plain)
        {
            BigPolyArray result;
            add_plain(encrypted, plain, result);
            return result;
        }

        /**
        Subtracts a ciphertext with a plaintext, and stores the result in the destination
        parameter. The plaintext must have a significant coefficient count smaller than 
        the coefficient count specified by the encryption parameters, and with
        coefficient values less than the plain modulus (EncryptionParameters::plain_modulus()).

        @param[in] encrypted The ciphertext to subtract from
        @param[in] plain The plaintext to subtract
        @param[out] destination The ciphertext to overwrite with the subtraction result
        @throws std::invalid_argument if the ciphertexts are not valid for the encryption parameters
        @throws std::invalid_argument if the plaintext's significant coefficient count or coefficient
        values are too large to represent with the encryption parameters
        */
        void sub_plain(const BigPolyArray &encrypted, const BigPoly &plain, BigPolyArray &destination);

        /**
        Subtracts a ciphertext with a plaintext, and returns the result. The plaintext
        must have a significant coefficient count smaller than the coefficient count specified by the
        encryption parameters, and with coefficient values less than the plain modulus
        (EncryptionParameters::plain_modulus()).

        @param[in] encrypted The ciphertext to subtract from
        @param[in] plain The plaintext to subtract
        @throws std::invalid_argument if the ciphertexts are not valid for the encryption parameters
        @throws std::invalid_argument if the plaintext's significant coefficient count or coefficient
        values are too large to represent with the encryption parameters
        */
        BigPolyArray sub_plain(const BigPolyArray &encrypted, const BigPoly &plain)
        {
            BigPolyArray result;
            sub_plain(encrypted, plain, result);
            return result;
        }

        /**
        Multiplies a ciphertext with a plaintext, and stores the result in the destination
        parameter. The plaintext must have a significant coefficient count smaller than the 
        coefficient count specified by the encryption parameters, and with coefficient values 
        less than the plain modulus (EncryptionParameters::plain_modulus()). 
        
        Multiplying by a plaintext 0 is not allowed and will result in the library throwing an invalid 
        argument exception. The reason behind this design choice is that the result should 
        be a fresh encryption of 0, but creating fresh encryptions should not be something 
        this class does. Instead the user should separately handle the cases where the 
        plain multiplier is 0.

        @param[in] encrypted The ciphertext to multiply
        @param[in] plain The plaintext to multiply
        @param[out] destination The ciphertext to overwrite with the multiplication result
        @throws std::invalid_argument if the ciphertexts are not valid for the encryption parameters
        @throws std::invalid_argument if the plaintext's significant coefficient count or coefficient
        values are too large to represent with the encryption parameters
        @throws std::invalid_argument if the plaintext multiplier is zero
        */
        void multiply_plain(const BigPolyArray &encrypted, const BigPoly &plain, BigPolyArray &destination);

        /**
        Multiplies a ciphertext with a plaintext, and returns the result. The plaintext
        must have a significant coefficient count smaller than the coefficient count specified by the
        encryption parameters, and with coefficient values less than the plain modulus
        (EncryptionParameters::plain_modulus()).

        Multiplying by a plaintext 0 is not allowed and will result in the library throwing an invalid
        argument exception. The reason behind this design choice is that the result should
        be a fresh encryption of 0, but creating fresh encryptions should not be something
        this class does. Instead the user should separately handle the cases where the
        plain multiplier is 0.
        
        @param[in] encrypted The ciphertext to multiply
        @param[in] plain The plaintext to multiply
        @throws std::invalid_argument if the ciphertexts are not valid for the encryption parameters
        @throws std::invalid_argument if the plaintext's significant coefficient count or coefficient
        values are too large to represent with the encryption parameters
        @throws std::invalid_argument if the plaintext multiplier is zero
        */
        BigPolyArray multiply_plain(const BigPolyArray &encrypted, const BigPoly &plain)
        {
            BigPolyArray result;
            multiply_plain(encrypted, plain, result);
            return result;
        }

        /**
        Returns the evaluation keys used by the Evaluator.
        */
        const EvaluationKeys &evaluation_keys() const
        {
            return evaluation_keys_;
        }

    private:
        Evaluator(const Evaluator &copy) = delete;

        Evaluator &operator =(const Evaluator &assign) = delete;

        void preencrypt(const std::uint64_t *plain, int plain_coeff_count, int plain_coeff_uint64_count, std::uint64_t *destination);

        void relinearize_internal(const BigPolyArray &encrypted, BigPolyArray &destination, int destination_size = 2);

        void relinearize_one_step(BigPolyArray &encrypted, BigPolyArray &destination);

        BigPoly poly_modulus_;

        BigUInt coeff_modulus_;

        BigUInt plain_modulus_;

        BigUInt upper_half_threshold_;

        BigUInt upper_half_increment_;

        BigUInt plain_upper_half_threshold_;

        BigUInt plain_upper_half_increment_;

        BigUInt coeff_div_plain_modulus_;

        int decomposition_bit_count_;

        EvaluationKeys evaluation_keys_;

        BigUInt wide_coeff_modulus_;

        BigUInt wide_coeff_modulus_div_two_;

        util::PolyModulus polymod_;

        util::Modulus mod_;

        int product_coeff_bit_count_;

        int plain_modulus_bit_count_;
    };
}

#endif // SEAL_EVALUATOR_H
