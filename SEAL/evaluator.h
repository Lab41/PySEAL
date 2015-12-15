#ifndef SEAL_EVALUATOR_H
#define SEAL_EVALUATOR_H

#include <vector>
#include "encryptionparams.h"
#include "evaluationkeys.h"
#include "util/mempool.h"
#include "util/modulus.h"
#include "util/polymodulus.h"

namespace seal
{
    /**
    Provides arithmetic functions for operating on encrypted polynomials (represented by BigPoly's). The
    add, subtract, and multiply function variants allow both operands to be encrypted or for one operand
    to be plain-text with the "_plain" variants.

    As multiplication is typically the most expensive arithmetic operation, three variants of the multiply
    function are provided that may allow for more performance depending on the application. The multiply()
    function performs full-blown multiplication, which actually internally consists of a multiplication
    and relinearization step internally. The multiply_norelin() and relinearize() functions break-up
    multiplication into its two steps to allow the relinearlization step to be optional. Refer to
    documentation on the encryption scheme to determine which multiply variants may be beneficial for
    different applications.

    @par Thread Safety
    The Evaluator class is not thread-safe and a separate Evaluator instance is needed for each
    potentially concurrent usage.
    */
    class Evaluator
    {
    public:
        /**
        Creates an Evaluator instances initialized with the specified encryption parameters and
        evaluation keys.

        @param[in] parms The encryption parameters
        @param[in] evaluation_keys The evaluation keys
        @throws std::invalid_argument if encryption parameters or evaluation keys are not valid
        @see EncryptionParameters for more details on valid encryption parameters.
        */
        Evaluator(const EncryptionParameters &parms, const EvaluationKeys &evaluation_keys);

        /**
        Negates an encrypted polynomial and stores the result in the destination parameter. The
        destination parameter is resized if and only if its coefficient count or coefficient
        bit count does not match the encryption parameters.

        @param[in] encrypted The encrypted polynomial to negate
        @param[out] destination The polynomial to overwrite with the negated result
        @throws std::invalid_argument if the encrypted polynomial is not a valid for the encryption
        parameters
        @throws std::logic_error If destination is an alias but needs to be resized
        */
        void negate(const BigPoly &encrypted, BigPoly &destination);

        /**
        Negates an encrypted polynomial and returns the result.

        @param[in] encrypted The encrypted polynomial to negate
        @throws std::invalid_argument if the encrypted polynomial is not a valid encrypted polynomial
        for the encryption parameters
        */
        BigPoly negate(const BigPoly &encrypted)
        {
            BigPoly result;
            negate(encrypted, result);
            return result;
        }

        /**
        Adds two encrypted polynomials and stores the result in the destination parameter. The
        destination parameter is resized if and only if its coefficient count or coefficient
        bit count does not match the encryption parameters.

        @param[in] encrypted1 The first encrypted polynomial to add
        @param[in] encrypted2 The second encrypted polynomial to add
        @param[out] destination The polynomial to overwrite with the addition result
        @throws std::invalid_argument if the encrypted polynomials are not valid for the
        encryption parameters
        @throws std::logic_error If destination is an alias but needs to be resized
        */
        void add(const BigPoly &encrypted1, const BigPoly &encrypted2, BigPoly &destination);

        /**
        Adds two encrypted polynomials and returns the result.

        @param[in] encrypted1 The first encrypted polynomial to add
        @param[in] encrypted2 The second encrypted polynomial to add
        @throws std::invalid_argument if the encrypted polynomials are not valid for the
        encryption parameters
        */
        BigPoly add(const BigPoly &encrypted1, const BigPoly &encrypted2)
        {
            BigPoly result;
            add(encrypted1, encrypted2, result);
            return result;
        }

        /**
        Adds together an number of ciphertexts stored as elements of std::vector<BigPoly>
        and stores the result in the destination parameter. The destination parameter 
        is resized if and only if its coefficient count or coefficient
        bit count does not match the encryption parameters.

        @param[in] encrypteds The encrypted polynomials to add
        @param[out] destination The polynomial to overwrite with the addition result
        @throws std::invalid_argument if encrypteds is empty
        @throws std::invalid_argument if the encrypted polynomials are not valid for the
        encryption parameters
        @throws std::logic_error If destination is an alias but needs to be resized
        */
        void add_many(const std::vector<BigPoly> &encrypteds, BigPoly &destination);

        /**
        Adds together an number of ciphertexts stored as elements of std::vector<BigPoly>
        and returns the result.

        @param[in] encrypteds The encrypted polynomials to add
        @throws std::invalid_argument if encrypteds is empty
        @throws std::invalid_argument if the encrypted polynomials are not valid for the
        encryption parameters
        */
        BigPoly add_many(const std::vector<BigPoly> &encrypteds)
        {
            BigPoly result;
            add_many(encrypteds, result);
            return result;
        }

        /**
        Subtracts two encrypted polynomials and stores the result in the destination parameter. The
        destination parameter is resized if and only if its coefficient count or coefficient
        bit count does not match the encryption parameters.

        @param[in] encrypted1 The encrypted polynomial to subtract from
        @param[in] encrypted2 The encrypted polynomial to subtract
        @param[out] destination The polynomial to overwrite with the subtraction result
        @throws std::invalid_argument if the encrypted polynomials are not valid for the
        encryption parameters
        @throws std::logic_error If destination is an alias but needs to be resized
        */
        void sub(const BigPoly &encrypted1, const BigPoly &encrypted2, BigPoly &destination);

        /**
        Subtracts two encrypted polynomials and returns the result.

        @param[in] encrypted1 The encrypted polynomial to subtract from
        @param[in] encrypted2 The encrypted polynomial to subtract
        @throws std::invalid_argument if the encrypted polynomials are not valid for the
        encryption parameters
        */
        BigPoly sub(const BigPoly &encrypted1, const BigPoly &encrypted2)
        {
            BigPoly result;
            sub(encrypted1, encrypted2, result);
            return result;
        }

        /**
        Multiplies two encrypted polynomials and stores the result in the destination parameter. The
        destination parameter is resized if and only if its coefficient count or coefficient
        bit count does not match the encryption parameters.

        @param[in] encrypted1 The first encrypted polynomial to multiply
        @param[in] encrypted2 The second encrypted polynomial to multiply
        @param[out] destination The polynomial to overwrite with the multiplication result
        @throws std::invalid_argument if the encrypted polynomials are not valid for the
        encryption parameters
        @throws std::logic_error If destination is an alias but needs to be resized
        */
        void multiply(const BigPoly &encrypted1, const BigPoly &encrypted2, BigPoly &destination);

        /**
        Multiplies two encrypted polynomials and returns the result.

        @param[in] encrypted1 The first encrypted polynomial to multiply
        @param[in] encrypted2 The second encrypted polynomial to multiply
        @throws std::invalid_argument if the encrypted polynomials are not valid for the
        encryption parameters
        */
        BigPoly multiply(const BigPoly &encrypted1, const BigPoly &encrypted2)
        {
            BigPoly result;
            multiply(encrypted1, encrypted2, result);
            return result;
        }

        /**
        Multiplies two encrypted polynomials without performing relinearization, and stores the result
        in the destination parameter. The destination parameter is resized if and only if its
        coefficient count or coefficient bit count does not match the encryption parameters.

        @param[in] encrypted1 The first encrypted polynomial to multiply
        @param[in] encrypted2 The second encrypted polynomial to multiply
        @param[out] destination The polynomial to overwrite with the multiplication result
        @throws std::invalid_argument if the encrypted polynomials are not valid for the
        encryption parameters
        @throws std::logic_error If destination is an alias but needs to be resized
        */
        void multiply_norelin(const BigPoly &encrypted1, const BigPoly &encrypted2, BigPoly &destination);

        /**
        Multiplies two encrypted polynomials without performing relinearization, and returns the result.

        @param[in] encrypted1 The first encrypted polynomial to multiply
        @param[in] encrypted2 The second encrypted polynomial to multiply
        @throws std::invalid_argument if the encrypted polynomials are not valid for the
        encryption parameters
        */
        BigPoly multiply_norelin(const BigPoly &encrypted1, const BigPoly &encrypted2)
        {
            BigPoly result;
            multiply_norelin(encrypted1, encrypted2, result);
            return result;
        }

        /**
        Relinearizes an encrypted polynomial and stores the result in the destination parameter. The
        destination parameter is resized if and only if its coefficient count or coefficient bit count does
        not match the encryption parameters.

        @param[in] encrypted The encrypted polynomial to relinearize
        @param[out] destination The polynomial to overwrite with the relinearized result
        @throws std::invalid_argument if the encrypted polynomial is not valid for the encryption parameters
        @throws std::logic_error If destination is an alias but needs to be resized
        */
        void relinearize(const BigPoly &encrypted, BigPoly &destination);

        /**
        Relinearizes an encrypted polynomial and returns the result.

        @param[in] encrypted The encrypted polynomial to relinearize
        @throws std::invalid_argument if the encrypted polynomial is not valid for the encryption parameters
        */
        BigPoly relinearize(const BigPoly &encrypted)
        {
            BigPoly result;
            relinearize(encrypted, result);
            return result;
        }

        /**
        Multiplies a vector of encrypted polynomials together and returns the result.

        @param[in] encrypteds The vector of encrypted polynomials to multiply
        @throws std::invalid_argument if the encrypteds vector is empty
        @throws std::invalid_argument if the encrypted polynomials are not valid encrypted polynomials
        for the encryption parameters
        */
        BigPoly multiply_many(std::vector<BigPoly> encrypteds);

        /**
        Multiplies a vector of encrypted polynomials together and stores the result in the destination
        parameter. The destination parameter is resized if and only if its coefficient count or coefficient
        bit count does not match the encryption parameters.

        @param[in] encrypteds The vector of encrypted polynomials to multiply
        @param[out] destination The polynomial to overwrite with the multiplication result
        @throws std::invalid_argument if the encrypteds vector is empty
        @throws std::invalid_argument if the encrypted polynomials are not valid encrypted polynomials
        for the encryption parameters
        @throws std::logic_error If destination is an alias but needs to be resized
        */
        void multiply_many(std::vector<BigPoly> &encrypteds, BigPoly &destination)
        {
            multiply_many(encrypteds).duplicate_to(destination);
        }

        /**
        Multiplies a vector of encrypted polynomials together without relinearization and returns the result.

        @param[in] encrypteds The vector of encrypted polynomials to multiply
        @throws std::invalid_argument if the encrypteds vector is empty
        @throws std::invalid_argument if the encrypted polynomials are not valid encrypted polynomials
        for the encryption parameters
        */
        BigPoly multiply_norelin_many(std::vector<BigPoly> encrypteds);

        /**
        Multiplies a vector of encrypted polynomials together without relinearization and stores the result in the destination
        parameter. The destination parameter is resized if and only if its coefficient count or coefficient
        bit count does not match the encryption parameters.

        @param[in] encrypteds The vector of encrypted polynomials to multiply
        @param[out] destination The polynomial to overwrite with the multiplication result
        @throws std::invalid_argument if the encrypteds vector is empty
        @throws std::invalid_argument if the encrypted polynomials are not valid encrypted polynomials
        for the encryption parameters
        @throws std::logic_error If destination is an alias but needs to be resized
        */
        void multiply_norelin_many(std::vector<BigPoly> &encrypteds, BigPoly &destination)
        {
            multiply_norelin_many(encrypteds).duplicate_to(destination);
        }

        /**
        Raises an encrypted polynomial to the specified power and stores the result in the destination
        parameter. The destination parameter is resized if and only if its coefficient count or coefficient
        bit count does not match the encryption parameters.

        @param[in] encrypted The encrypted polynomial to raise to a power
        @param[in] exponent The power to raise the encrypted polynomial to
        @param[out] destination The polynomial to overwrite with the exponentiation result
        @throws std::invalid_argument if the encrypted polynomial is not valid for the encryption parameters
        @throws std::invalid_argument if the exponent is negative
        @throws std::logic_error If destination is an alias but needs to be resized
        */
        void exponentiate(const BigPoly &encrypted, int exponent, BigPoly &destination);

        /**
        Raises an encrypted polynomial to the specified power and returns the result.

        @param[in] encrypted The encrypted polynomial to raise to a power
        @param[in] exponent The non-negative power to raise the encrypted polynomial to
        @throws std::invalid_argument if the encrypted polynomial is not valid for the encryption parameters
        @throws std::invalid_argument if the exponent is negative
        */
        BigPoly exponentiate(const BigPoly &encrypted, int exponent)
        {
            BigPoly result;
            exponentiate(encrypted, exponent, result);
            return result;
        }

        /**
        Raises an encrypted polynomial to the specified power without performing relinearization and stores the result in the destination
        parameter. The destination parameter is resized if and only if its coefficient count or coefficient
        bit count does not match the encryption parameters.

        @param[in] encrypted The encrypted polynomial to raise to a power
        @param[in] exponent The power to raise the encrypted polynomial to
        @param[out] destination The polynomial to overwrite with the exponentiation result
        @throws std::invalid_argument if the encrypted polynomial is not valid for the encryption parameters
        @throws std::invalid_argument if the exponent is negative
        @throws std::logic_error If destination is an alias but needs to be resized
        */
        void exponentiate_norelin(const BigPoly &encrypted, int exponent, BigPoly &destination);

        /**
        Raises an encrypted polynomial to the specified power without performing relinearization and returns the result.

        @param[in] encrypted The encrypted polynomial to raise to a power
        @param[in] exponent The non-negative power to raise the encrypted polynomial to
        @throws std::invalid_argument if the encrypted polynomial is not valid for the encryption parameters
        @throws std::invalid_argument if the exponent is negative
        */
        BigPoly exponentiate_norelin(const BigPoly &encrypted, int exponent)
        {
            BigPoly result;
            exponentiate_norelin(encrypted, exponent, result);
            return result;
        }

        /**
        Adds an encrypted polynomial with a plain polynomial, and stores the result in the destination
        parameter. The destination parameter is resized if and only if its coefficient count or coefficient
        bit count does not match the encryption parameters. The plain polynomial must have a significant
        coefficient count smaller than the coefficient count specified by the encryption parameters, and with
        coefficient values less-than the plain modulus (EncryptionParameters::plain_modulus()).

        @param[in] encrypted1 The first encrypted polynomial to add
        @param[in] plain2 The second plain polynomial to add
        @param[out] destination The polynomial to overwrite with the addition result
        @throws std::invalid_argument if the encrypted1 polynomials are not valid for the encryption parameters
        @throws std::invalid_argument if the plain2 polynomial's significant coefficient count or coefficient
        values are too large to represent with the encryption parameters
        @throws std::logic_error If destination is an alias but needs to be resized
        */
        void add_plain(const BigPoly &encrypted1, const BigPoly &plain2, BigPoly &destination);

        /**
        Adds an encrypted polynomial with a plain polynomial, and returns the result. The plain polynomial
        must have a significant coefficient count smaller than the coefficient count specified by the
        encryption parameters, and with coefficient values less-than the plain modulus
        (EncryptionParameters::plain_modulus()).

        @param[in] encrypted1 The first encrypted polynomial to add
        @param[in] plain2 The second plain polynomial to add
        @throws std::invalid_argument if the encrypted1 polynomials are not valid for the encryption parameters
        @throws std::invalid_argument if the plain2 polynomial's significant coefficient count or coefficient
        values are too large to represent with the encryption parameters
        */
        BigPoly add_plain(const BigPoly &encrypted1, const BigPoly &plain2)
        {
            BigPoly result;
            add_plain(encrypted1, plain2, result);
            return result;
        }

        /**
        Subtracts an encrypted polynomial with a plain polynomial, and stores the result in the destination
        parameter. The destination parameter is resized if and only if its coefficient count or coefficient
        bit count does not match the encryption parameters. The plain polynomial must have a significant
        coefficient count smaller than the coefficient count specified by the encryption parameters, and with
        coefficient values less-than the plain modulus (EncryptionParameters::plain_modulus()).

        @param[in] encrypted1 The encrypted polynomial to subtract from
        @param[in] plain2 The plain polynomial to subtract
        @param[out] destination The polynomial to overwrite with the subtraction result
        @throws std::invalid_argument if the encrypted1 polynomials are not valid for the encryption parameters
        @throws std::invalid_argument if the plain2 polynomial's significant coefficient count or coefficient
        values are too large to represent with the encryption parameters
        @throws std::logic_error If destination is an alias but needs to be resized
        */
        void sub_plain(const BigPoly &encrypted1, const BigPoly &plain2, BigPoly &destination);

        /**
        Subtracts an encrypted polynomial with a plain polynomial, and returns the result. The plain polynomial
        must have a significant coefficient count smaller than the coefficient count specified by the
        encryption parameters, and with coefficient values less-than the plain modulus
        (EncryptionParameters::plain_modulus()).

        @param[in] encrypted1 The encrypted polynomial to subtract from
        @param[in] plain2 The plain polynomial to subtract
        @throws std::invalid_argument if the encrypted1 polynomials are not valid for the encryption parameters
        @throws std::invalid_argument if the plain2 polynomial's significant coefficient count or coefficient
        values are too large to represent with the encryption parameters
        */
        BigPoly sub_plain(const BigPoly &encrypted1, const BigPoly &plain2)
        {
            BigPoly result;
            sub_plain(encrypted1, plain2, result);
            return result;
        }

        /**
        Multiplies an encrypted polynomial with a plain polynomial, and stores the result in the destination
        parameter. The destination parameter is resized if and only if its coefficient count or coefficient
        bit count does not match the encryption parameters. The plain polynomial must have a significant
        coefficient count smaller than the coefficient count specified by the encryption parameters, and with
        coefficient values less-than the plain modulus (EncryptionParameters::plain_modulus()).

        @param[in] encrypted1 The first encrypted polynomial to multiply
        @param[in] plain2 The second plain polynomial to multiply
        @param[out] destination The polynomial to overwrite with the multiplication result
        @throws std::invalid_argument if the encrypted1 polynomials are not valid for the encryption parameters
        @throws std::invalid_argument if the plain2 polynomial's significant coefficient count or coefficient
        values are too large to represent with the encryption parameters
        @throws std::logic_error If destination is an alias but needs to be resized
        */
        void multiply_plain(const BigPoly &encrypted1, const BigPoly &plain2, BigPoly &destination);

        /**
        Multiplies an encrypted polynomial with a plain polynomial, and returns the result. The plain polynomial
        must have a significant coefficient count smaller than the coefficient count specified by the
        encryption parameters, and with coefficient values less-than the plain modulus
        (EncryptionParameters::plain_modulus()).

        @param[in] encrypted1 The first encrypted polynomial to multiply
        @param[in] plain2 The second plain polynomial to multiply
        @throws std::invalid_argument if the encrypted1 polynomials are not valid for the encryption parameters
        @throws std::invalid_argument if the plain2 polynomial's significant coefficient count or coefficient
        values are too large to represent with the encryption parameters
        @throws std::logic_error If destination is an alias but needs to be resized
        */
        BigPoly multiply_plain(const BigPoly &encrypted1, const BigPoly &plain2)
        {
            BigPoly result;
            multiply_plain(encrypted1, plain2, result);
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

        void multiply(const std::uint64_t *encrypted1, const std::uint64_t *encrypted2, std::uint64_t *destination);

        void relinearize(const uint64_t *encrypted, uint64_t *destination);

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

        EncryptionMode mode_;

        util::MemoryPool pool_;

        util::PolyModulus polymod_;

        util::Modulus mod_;
    };
}

#endif // SEAL_EVALUATOR_H
