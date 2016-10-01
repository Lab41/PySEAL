#pragma once

#include <iostream>
#include "biguint.h"
#include "bigpoly.h"
#include "randomgen.h"
#include "util/ntt.h"

namespace seal
{
    /**
    Stores a set of attributes (qualifiers) of a set of encryption parameteres.
    These parameters are used in various parts of the library, e.g. to determine which
    algorithms can be used. The qualifiers are silently passed on to classes such as
    Encryptor, Evaluator, and Decryptor, and the only way to change them is by changing
    the encryption parameters accordingly. In other words, a user will never have to 
    create their own instance of EncryptionParameterQualifiers.

    @see EncryptionParameters for the class that stores the encryption parameters themselves.
    @see EncryptionParameters::GetQualifiers for obtaining the EncryptionParameterQualifiers corresponding to a certain parameter set.
    */
    struct EncryptionParameterQualifiers
    {
        /**
        If the encryption parameters are set in a way that is considered valid by SEAL, the
        variable parameters_set will be set to true.
        */
        bool parameters_set;

        /**
        If EncryptionParameters::decomposition_bit_count is set to a positive value, the variable
        enable_relinearization will be set to true.
        */
        bool enable_relinearization;

        /**
        If the polynomial modulus is of the form X^N+1, where N is a power of two, then
        Nussbaumer convolution can be used for fast multiplication of polynomials modulo
        the polynomial modulus. In this case the variable enable_nussbaumer will be set to true.
        However, currently SEAL requires the polynomial modulus to be of this form to even
        consider the parameters to be valid. Therefore, parameters_set can only be true if
        enable_nussbaumer is true.
        */
        bool enable_nussbaumer;

        /**
        If the coefficient modulus is congruent to 1 modulo 2N, where X^N+1 is the polynomial
        modulus and N is a power of two, then the number-theoretic transform (NTT) can be used
        for fast multiplications of polynomials modulo the polynomial modulus and coefficient
        modulus. In this case the variable enable_ntt will be set to true.
        */
        bool enable_ntt;

        /**
        If the plaintext modulus is congruent to 1 modulo 2N, where X^N+1 is the polynomial
        modulus and N is a power of two, then it is possible to use PolyCRTBuilder to do batching,
        which is a fundamental technique in homomorphic encryption to enable powerful SIMD
        functionality, often called "batching" in homomorphic encryption literature. In this 
        case the variable enable_batching will be set to true.
        */
        bool enable_batching;

        /**
        This variable has currently no effect (see util/defines.h).
        */
        bool enable_ntt_in_multiply;

    private:
        EncryptionParameterQualifiers() :
            parameters_set(false),
            enable_relinearization(false),
            enable_nussbaumer(false),
            enable_ntt(false),
            enable_batching(false),
            enable_ntt_in_multiply(false)
        {
        }

        friend class EncryptionParameters;
    };

    /**
    Represents the user-customizable encryption scheme settings. Several settings (e.g., poly_modulus(), coeff_modulus(),
    plain_modulus()) significantly affect the performance, capabilities, and security of the encryption scheme. KeyGenerator,
    Encryptor, Decryptor, Evaluator, and other objects in the library all require the EncryptionParameters object to specify
    and agree on the encryption scheme settings.

    Picking appropriate encryption parameters is essential to enable a particular application while balancing performance and
    security. Some encryption settings will not allow some inputs (e.g., attempting to encrypt a polynomial with more
    coefficients than poly_modulus() or larger coefficients than plain_modulus()) or support some computations (with noise
    growing too fast as determined by coeff_modulus() and decomposition_bit_count()). The ChooserPoly and ChooserEvaluator
    classes provide functionality to help determine the best encryption parameters for an application. Additionally, please 
    refer to external documentation to determine the best parameters.

    @par Thread Safety
    In general, reading from EncryptionParameters is thread-safe while mutating is not. Refer to the thread-safety of
    individual parameter types to assess their thread-safety.

    @warning Choosing inappropriate EncryptionParameters may load to an encryption scheme that is not secure, does not perform
    well, and/or does not support the input and computation of the application.
    */
    class EncryptionParameters
    {
    public:
        /**
        Creates an empty EncryptionParameters instance.
        */
        EncryptionParameters();

        /**
        Returns a reference to the polynomial modulus (represented by a BigPoly) used for encryption. Note that the polynomial
        modulus also directly determines the number of coefficients of encrypted polynomials and the maximum number of
        coefficients for plain-text polynomials that are representable by the library.
        */
        BigPoly &poly_modulus()
        {
            return poly_modulus_;
        }

        /**
        Returns a reference to the polynomial modulus (represented by a BigPoly) used for encryption. Note that the polynomial
        modulus also directly determines the number of coefficients of encrypted polynomials and the maximum number of
        coefficients for plain-text polynomials that are representable by the library.
        */
        const BigPoly &poly_modulus() const
        {
            return poly_modulus_;
        }

        /**
        Returns a reference to the coefficient modulus (represented by a BigUInt) used for encryption. Note that the
        coefficient modulus directly determines the number of bits-per-coefficient of encrypted polynomials and the maximum
        value allowed for plain_modulus() (which should be significantly smaller than coeff_modulus()).
        */
        BigUInt &coeff_modulus()
        {
            return coeff_modulus_;
        }

        /**
        This variable has currently no effect (util/defines.h).
        */
        BigUInt &aux_coeff_modulus()
        {
            return aux_coeff_modulus_;
        }

        /**
        Returns a reference to the coefficient modulus (represented by a BigUInt) used for encryption. Note that the
        coefficient modulus directly determines the number of bits-per-coefficient of encrypted polynomials and the maximum
        value allowed for plain_modulus() (which should be significantly smaller than coeff_modulus()).
        */
        const BigUInt &coeff_modulus() const
        {
            return coeff_modulus_;
        }

        const BigUInt &aux_coeff_modulus() const
        {
            return aux_coeff_modulus_;
        }

        /**
        Returns a reference to the plain-text modulus (represented by a BigUInt) used for encryption. Note that the
        plain-text modulus is one greater than the maximum value allowed for any plain-text coefficient that the library
        can encrypt or represent.
        */
        BigUInt &plain_modulus()
        {
            return plain_modulus_;
        }

        /**
        Returns a reference to the plaintext modulus (represented by a BigUInt) used for encryption. Note that the
        plain-text modulus is one greater than the maximum value allowed for any plaintext coefficient that the library
        can encrypt or represent.
        */
        const BigUInt &plain_modulus() const
        {
            return plain_modulus_;
        }

        /**
        Returns a reference to the standard deviation of normalized noise used during key generation and encryption.

        @warning Setting the noise standard deviation too small is not secure.
        */
        double &noise_standard_deviation()
        {
            return noise_standard_deviation_;
        }

        /**
        Returns the standard deviation of normalized noise used during key generation and encryption.

        @warning Setting the noise standard deviation too small is not secure.
        */
        double noise_standard_deviation() const
        {
            return noise_standard_deviation_;
        }

        /**
        Returns a reference to the maximum deviation of normalized noise used during key generation and encryption.

        @warning Setting the noise maximum deviation too small is not secure.
        */
        double &noise_max_deviation()
        {
            return noise_max_deviation_;
        }

        /**
        Returns the maximum deviation of normalized noise used during key generation and encryption.

        @warning Setting the noise maximum deviation too small is not secure.
        */
        double noise_max_deviation() const
        {
            return noise_max_deviation_;
        }

        /**
        Returns a reference to the decomposition bit count which directly determines the number of evaluation keys
        required by the scheme. Smaller decomposition bit count reduces the accumulation of noise during multiplication
        operations, but can also significantly increase the time required to perform multiplication.
        */
        int &decomposition_bit_count()
        {
            return decomposition_bit_count_;
        }

        /**
        Returns the decomposition bit count which directly determines the number of evaluation keys required by
        the scheme. Smaller decomposition bit count reduces the accumulation of noise during multiplication
        operations, but can also significantly increase the time required to perform multiplication.
        */
        int decomposition_bit_count() const
        {
            return decomposition_bit_count_;
        }

        /**
        Saves the EncryptionParameters to an output stream. The output is in binary format and is not human-readable. The
        output stream must have the "Binary" flag set.

        @param[in] stream The stream to save the EncryptionParameters to
        @see load() to load a saved EncryptionParameters instance.
        */
        void save(std::ostream &stream) const;

        /**
        Loads the EncryptionParameters from an input stream overwriting the current EncryptionParameters.

        @param[in] stream The stream to load the EncryptionParameters from
        @see save() to save an EncryptionParameters instance.
        */
        void load(std::istream &stream);

        /**
        Returns a reference to the random number generator factory to use for encryption. By default, the random generator
        is set to UniformRandomGeneratorFactory::default_factory(). Setting this value allows a user to specify a custom
        random number generator source.
        */
        UniformRandomGeneratorFactory *&random_generator()
        {
            return random_generator_;
        }

        /**
        Returns the random number generator factory to use for encryption. By default, the random generator is set to
        UniformRandomGeneratorFactory::default_factory(). Setting this value allows a user to specify a custom random number
        generator source.
        */
        UniformRandomGeneratorFactory *random_generator() const
        {
            return random_generator_;
        }

        /**
        Computes and returns the maximum number of bits of inherent noise supported by the current encryption
        parameters. Any ciphertext with larger inherent noise is impossible to decrypt, even with the correct 
        secret key. The function Decryptor::inherent_noise_bits() can be used to compute the number of bits
        of inherent noise in a given ciphertext.

        @par Inherent Noise
        Technically speaking, the inherent noise of a ciphertext is a polynomial, but the condition for
        decryption working depends on the size of the largest absolute value of its coefficients. It is this
        largest absolute value that we will call the "noise", the "inherent noise", or the "error", in this
        documentation. The reader is referred to the description of the encryption scheme for more details.

        @throws std::logic_error if the encryption parameters are not valid
        @see inherent_noise_max for computing the exact maximum size of inherent noise.
        */
        int inherent_noise_bits_max() const
        {
            BigUInt result;
            inherent_noise_max(result);
            return result.significant_bit_count();
        }

        /**
        Computes the maximum amount of inherent noise supported by the current encryption parameters and stores
        it in the given BigUInt. Any ciphertext with larger inherent noise is impossible to decrypt, even with 
        the correct secret key. The function Decryptor::inherent_noise_bits() can be used to compute the number 
        of bits of inherent noise in a given ciphertext.
        
        @par Inherent Noise
        Technically speaking, the inherent noise of a ciphertext is a polynomial, but the condition for
        decryption working depends on the size of the largest absolute value of its coefficients. It is this
        largest absolute value that we will call the "noise", the "inherent noise", or the "error", in this
        documentation. The reader is referred to the description of the encryption scheme for more details.

        @param[in] destination The BigUInt to overwrite with the maximum inherent noise
        @throws std::logic_error if the encryption parameters are not valid
        @see inherent_noise_bits_max for returning instead the significant bit count of the maximum size of inherent noise.
        */
        void inherent_noise_max(BigUInt &destination) const;

        /**
        Returns the set of qualifiers (as an instance of EncryptionParameterQualifiers) for the current
        encryption parameters.

        @see EncryptionParameterQualifiers for more details.
        */
        EncryptionParameterQualifiers get_qualifiers() const;

    private:
        BigPoly poly_modulus_;

        BigUInt coeff_modulus_;

        BigUInt aux_coeff_modulus_;

        BigUInt plain_modulus_;

        double noise_standard_deviation_;

        double noise_max_deviation_;

        int decomposition_bit_count_;

        UniformRandomGeneratorFactory *random_generator_;
    };
}