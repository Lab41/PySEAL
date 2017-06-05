#pragma once

#include <vector>
#include <utility>
#include "encryptionparams.h"
#include "evaluationkeys.h"
#include "util/mempool.h"
#include "util/modulus.h"
#include "util/polymodulus.h"
#include "util/ntt.h"
#include "util/crt.h"
#include "memorypoolhandle.h"
#include "ciphertext.h"

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
    However, relinearization consumes the invariant noise budget in a ciphertext by an additive factor 
    proportional to 2^DBC, and relinearizing from size K to L will require 2*(K-L)*[floor(log_2(coeff_modulus)/DBC)+1]
    polynomial multiplications, where DBC denotes the decomposition bit count set in the encryption parameters. 
    Note that the larger the decomposition bit count is, the faster relinearization will be, but also the
    more invariant noise budget will be consumed.

    Relinearization requires the key generator to generate evaluation keys. More specifically, to relinearize
    a ciphertext of size K down to any size smaller than K (but at least 2), at least K-2 evaluation keys will
    be needed. These have to be given as an input parameter to the constructor of Evaluator.

    @par Invariant Noise Budget
    The invariant noise polynomial of a ciphertext is a rational coefficient polynomial, such that
    a ciphertext decrypts correctly as long as the coefficients of the invariant noise polynomial are
    of absolute value less than 1/2. Thus, we call the infinity-norm of the invariant noise polynomial
    the invariant noise, and for correct decryption require it to be less than 1/2. If v denotes the
    invariant noise, we define the invariant noise budget as -log2(2v). Thus, the invariant noise budget
    starts from some initial value, which depends on the encryption parameters, and decreases to 0 when
    computations are performed. When the budget reaches 0, the ciphertext becomes too noisy to decrypt
    correctly.
    */
    class Evaluator
    {
    public:
        /**
        Creates an Evaluator instance initialized with the specified encryption parameters and
        evaluation keys. If no evaluation keys will be needed, one can simply pass a newly 
        created empty instance of EvaluationKeys to the function. Optionally, the user can 
        give a reference to a MemoryPoolHandle object to use a custom memory pool instead of 
        the global memory pool (default).

        @param[in] parms The encryption parameters
        @param[in] evaluation_keys The evaluation keys
        @param[in] pool The memory pool handle
        @throws std::invalid_argument if encryption parameters or evaluation keys are not valid
        @see EncryptionParameters for more details on valid encryption parameters.
        @see MemoryPoolHandle for more details on memory pool handles.
        */
        Evaluator(const EncryptionParameters &parms, const EvaluationKeys &evaluation_keys, 
            const MemoryPoolHandle &pool = MemoryPoolHandle::acquire_global());

        /**
        Creates an Evaluator instance initialized with the specified encryption parameters.
        Optionally, the user can give a reference to a MemoryPoolHandle object to use a custom 
        memory pool instead of the global memory pool (default).

        @param[in] parms The encryption parameters
        @param[in] pool The memory pool handle
        @throws std::invalid_argument if encryption parameters are not valid
        @see EncryptionParameters for more details on valid encryption parameters.
        @see MemoryPoolHandle for more details on memory pool handles.
        */
        Evaluator(const EncryptionParameters &parms, const MemoryPoolHandle &pool = MemoryPoolHandle::acquire_global())
            : Evaluator(parms, EvaluationKeys(), pool)
        {
        }

        /**
        Creates a copy of a Evaluator.

        @param[in] copy The Evaluator to copy from
        */
        Evaluator(const Evaluator &copy);

        /**
        Creates a new Evaluator by moving an old one.

        @param[in] source The Evaluator to move from
        */
        Evaluator(Evaluator &&source) = default;

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
        Adds together an number of ciphertexts stored as elements of std::vector<Ciphertext>
        and stores the result in the destination parameter.

        @param[in] encrypteds The ciphertexts to add
        @param[out] destination The ciphertext to overwrite with the addition result
        @throws std::invalid_argument if encrypteds is empty
        @throws std::invalid_argument if the ciphertexts are not valid for the encryption parameters
        */
        void add_many(const std::vector<Ciphertext> &encrypteds, Ciphertext &destination);

        /**
        Adds together an number of ciphertexts stored as elements of std::vector<Ciphertext>
        and returns the result.

        @param[in] encrypteds The ciphertexts to add
        @throws std::invalid_argument if encrypteds is empty
        @throws std::invalid_argument if the ciphertexts are not valid for the encryption parameters
        */
        Ciphertext add_many(const std::vector<Ciphertext> &encrypteds)
        {
            Ciphertext result;
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
        Squares a ciphertext and returns the result.

        @param[in] encrypted The ciphertext to square
        @throws std::invalid_argument if the ciphertexts are not valid for the encryption parameters
        */
        BigPolyArray square(const BigPolyArray &encrypted)
        {
            BigPolyArray result;
            square(encrypted, result);
            return result;
        }
        
        /**
        Squares a ciphertext and stores the result in the destination parameter.

        @param[in] encrypted The ciphertext to square
        @param[out] destination The ciphertext to overwrite with the result
        @throws std::invalid_argument if the ciphertexts are not valid for the encryption parameters
        */
        void square(const BigPolyArray &encrypted, BigPolyArray &destination);

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
        Multiplies a vector of ciphertexts together and returns the result. Relinearization is performed after 
        every multiplication, so enough encryption keys must have been given to the constructor of the Evaluator.

        @param[in] encrypteds The vector of ciphertexts to multiply
        @throws std::invalid_argument if the encrypteds vector is empty
        @throws std::invalid_argument if the ciphertexts are not valid ciphertexts for the encryption parameters
        */
        BigPolyArray multiply_many(std::vector<BigPolyArray> encrypteds);

        /**
        Multiplies a vector of ciphertexts together and stores the result in the destination parameter. 
        Relinearization is performed after every multiplication, so enough encryption keys must have been given
        to the constructor of the Evaluator.

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
        Multiplies a vector of ciphertexts together and returns the result. Relinearization is performed after
        every multiplication, so enough encryption keys must have been given to the constructor of the Evaluator.

        @param[in] encrypteds The vector of ciphertexts to multiply
        @throws std::invalid_argument if the encrypteds vector is empty
        @throws std::invalid_argument if the ciphertexts are not valid ciphertexts for the encryption parameters
        */
        Ciphertext multiply_many(std::vector<Ciphertext> encrypteds);

        /**
        Multiplies a vector of ciphertexts together and stores the result in the destination parameter.
        Relinearization is performed after every multiplication, so enough encryption keys must have been given
        to the constructor of the Evaluator.

        @param[in] encrypteds The vector of ciphertexts to multiply
        @param[out] destination The ciphertext to overwrite with the multiplication result
        @throws std::invalid_argument if the encrypteds vector is empty
        @throws std::invalid_argument if the ciphertexts are not valid ciphertexts for the encryption parameters
        */
        void multiply_many(std::vector<Ciphertext> &encrypteds, Ciphertext &destination)
        {
            destination = multiply_many(encrypteds);
        }

        /**
        Raises a ciphertext to the specified power and stores the result in the destination parameter. 
        
        Exponentiation to power 0 is not allowed and will result in the library throwing an invalid argument
        exception. The reason behind this design choice is that the result should be a fresh encryption
        of 1, but creating fresh encryptions should not be something this class does. Instead the user
        should separately handle the cases where the exponent is 0. Relinearization is performed after 
        every multiplication, so enough encryption keys must have been given to the constructor of the Evaluator.

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
        the cases where the exponent is 0. Relinearization is performed after every multiplication, 
        so enough encryption keys must have been given to the constructor of the Evaluator.

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
        @throws std::invalid_argument if the encrypted is not valid for the encryption parameters
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
        @throws std::invalid_argument if the encrypted is not valid for the encryption parameters
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

        /*
        Transform a plaintext from coefficient domain to NTT domain, with respect to the coefficient 
        modulus. This function first embeds integers modulo the plaintext modulus to integers modulo 
        the coefficient modulus, and then performs David Harvey's NTT on the resulting polynomial.

        Ciphertexts in the NTT domain can be added as usual, and multiplied by plaintext polynomials
        (also in the NTT domain) using multiply_plain_ntt, but cannot be homomorphically multiplied
        with other ciphertexts without first transforming both inputs to coefficient domain with
        transform_from_ntt.

        @param[in] plain The plaintext to transform
        @throws std::logic_error if the encryption parameters do not support NTT
        @throws std::invalid_argument if plain is not valid for the encryption parameters
        */
        void transform_to_ntt(BigPoly &plain);
 
        /*
        Transform a plaintext from NTT domain to coefficient domain, with respect to the coefficient
        modulus. This function first performs David Harvey's inverse NTT, and follows it by an inverse
        of the coefficient embedding performed by transform_to_ntt(BigPoly &plain). 
        
        Ciphertexts in the NTT domain can be added as usual, and multiplied by plaintext polynomials 
        (also in the NTT domain) using multiply_plain_ntt, but cannot be homomorphically multiplied
        with other ciphertexts without first transforming both inputs to coefficient domain with 
        transform_from_ntt.

        @param[in] plain_ntt The plaintext to transform
        @throws std::logic_error if the encryption parameters do not support NTT
        @throws std::invalid_argument if plain_ntt is not valid for the encryption parameters
        */
        void transform_from_ntt(BigPoly &plain_ntt);

        /*
        Transform a ciphertext from coefficient domain to NTT domain, with respect to the coefficient
        modulus. This function performs David Harvey's NTT separately on each of the polynomials
        in the given BigPolyArray. 
        
        Ciphertexts in the NTT domain can be added as usual, and multiplied 
        by plaintext polynomials (also in the NTT domain) using multiply_plain_ntt, but cannot be
        homomorphically multiplied with other ciphertexts without first transforming both inputs
        to coefficient domain with transform_from_ntt.

        @param[in] encrypted The ciphertext to transform
        @throws std::logic_error if the encryption parameters do not support NTT
        @throws std::invalid_argument if encrypted is not valid for the encryption parameters
        */
        void transform_to_ntt(BigPolyArray &encrypted);

        /*
        Transform a ciphertext from NTT domain to coefficient domain, with respect to the coefficient
        modulus. This function performs David Harvey's inverse NTT separately on each of the polynomials
        in the given BigPolyArray. 
        
        Ciphertexts in the NTT domain can be added as usual, and multiplied by plaintext polynomials
        (also in the NTT domain) using multiply_plain_ntt, but cannot be homomorphically multiplied
        with other ciphertexts without first transforming both inputs to coefficient domain with
        transform_from_ntt.

        @param[in] encrypted_ntt The ciphertext to transform
        @throws std::logic_error if the encryption parameters do not support NTT
        @throws std::invalid_argument if encrypted_ntt is not valid for the encryption parameters
        */
        void transform_from_ntt(BigPolyArray &encrypted_ntt);

        /*
        Multiplies a ciphertext with a plaintext, assuming both are already transformed to NTT domain.
        The result ciphertext remains in the NTT domain, and can be transformed back to coefficient
        domain with transform_from_ntt.

        Ciphertexts in the NTT domain can be added as usual, and multiplied by plaintext polynomials
        (also in the NTT domain) using multiply_plain_ntt, but cannot be homomorphically multiplied
        with other ciphertexts without first transforming both inputs to coefficient domain with
        transform_from_ntt.

        @param[in] encrypted_ntt The ciphertext to multiply (in NTT domain)
        @param[in] plain_ntt The plaintext to multiply (in NTT domain)
        @param[out] destination_ntt The ciphertext to overwrite with the multiplication result (in NTT domain)
        @throws std::logic_error if the encryption parameters do not support NTT
        @throws std::invalid_argument if encrypted_ntt is not valid for the encryption parameters
        @throws std::invalid_argument if the plaintext's significant coefficient count or coefficient
        values are too large to represent with the encryption parameters
        @throws std::invalid_argument if the plaintext multiplier is zero
        */
        void multiply_plain_ntt(const BigPolyArray &encrypted_ntt, const BigPoly &plain_ntt, BigPolyArray &destination_ntt);

        /*
        Multiplies a ciphertext with a plaintext, assuming both are already transformed to NTT domain,
        and returns the result. The result ciphertext remains in the NTT domain, and can be transformed 
        back to coefficient domain with transform_from_ntt.

        Ciphertexts in the NTT domain can be added as usual, and multiplied by plaintext polynomials
        (also in the NTT domain) using multiply_plain_ntt, but cannot be homomorphically multiplied
        with other ciphertexts without first transforming both inputs to coefficient domain with
        transform_from_ntt.

        @param[in] encrypted_ntt The ciphertext to multiply (in NTT domain)
        @param[in] plain_ntt The plaintext to multiply (in NTT domain)
        @throws std::logic_error if the encryption parameters do not support NTT
        @throws std::invalid_argument if encrypted_ntt is not valid for the encryption parameters
        @throws std::invalid_argument if the plaintext's significant coefficient count or coefficient
        values are too large to represent with the encryption parameters
        @throws std::invalid_argument if the plaintext multiplier is zero
        */
        BigPolyArray multiply_plain_ntt(const BigPolyArray &encrypted_ntt, const BigPoly &plain_ntt)
        {
            BigPolyArray result_ntt;
            multiply_plain_ntt(encrypted_ntt, plain_ntt, result_ntt);
            return result_ntt;
        }

    private:
        Evaluator &operator =(const Evaluator &assign) = delete;

        Evaluator &operator =(Evaluator &&assign) = delete;

        void preencrypt(const std::uint64_t *plain, int plain_coeff_count, int plain_coeff_uint64_count, std::uint64_t *destination);

        void relinearize_one_step(const std::uint64_t *encrypted, int encrypted_size, std::uint64_t *destination, util::MemoryPool &pool);

        MemoryPoolHandle pool_;
        
        BigPoly poly_modulus_;

        BigUInt coeff_modulus_;

        BigUInt aux_coeff_modulus_;

        BigUInt plain_modulus_;

        BigUInt upper_half_threshold_;

        // Threshold for negative integers modulo coeff_modulus_ * aux_coeff_modulus_.
        BigUInt mod_product_upper_half_threshold_;

        BigUInt upper_half_increment_;

        BigUInt plain_upper_half_threshold_;

        BigUInt plain_upper_half_increment_;

        BigUInt coeff_div_plain_modulus_;

        int decomposition_bit_count_;

        EvaluationKeys evaluation_keys_;

        BigUInt coeff_modulus_div_two_;

        util::PolyModulus polymod_;

        util::Modulus mod_;

        util::Modulus aux_mod_;

        int product_coeff_bit_count_;

        int plain_modulus_bit_count_;

        util::NTTTables ntt_tables_;

        util::NTTTables aux_ntt_tables_;

        util::UIntCRTBuilder crt_builder_;

        EncryptionParameterQualifiers qualifiers_;
    };
}