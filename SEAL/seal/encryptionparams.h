#pragma once

#include <iostream>
#include <string>
#include <array>
#include "seal/util/globals.h"
#include "seal/bigpoly.h"
#include "seal/randomgen.h"
#include "seal/smallmodulus.h"
#include "seal/util/hash.h"
#include "seal/defaultparams.h"

namespace seal
{
    /**
    Represents user-customizable encryption scheme settings. The parameters (most 
    importantly poly_modulus, coeff_modulus, plain_modulus) significantly affect 
    the performance, capabilities, and security of the encryption scheme. Once 
    an instance of EncryptionParameters is populated with appropriate parameters, 
    it can be used to create an instance of the SEALContext class, which verifies
    the validity of the parameters, and performs necessary pre-computations.

    Picking appropriate encryption parameters is essential to enable a particular
    application while balancing performance and security. Some encryption settings
    will not allow some inputs (e.g. attempting to encrypt a polynomial with more
    coefficients than poly_modulus or larger coefficients than plain_modulus) or,
    support the desired computations (with noise growing too fast due to too large 
    plain_modulus and too small coeff_modulus).

    @par Hash Block
    The EncryptionParameters class maintains at all times a 256-bit SHA-3 hash of 
    the currently set encryption parameters. This hash is then stored by all further
    objects created for these encryption parameters, e.g. SEALContext, KeyGenerator,
    Encryptor, Decryptor, Evaluator, all secret and public keys, and ciphertexts.
    The hash block is not intended to be directly modified by the user, and is used
    internally to perform quick input compatibility checks.

    @par Thread Safety
    In general, reading from EncryptionParameters is thread-safe, while mutating is not.

    @warning Choosing inappropriate encryption parameters may lead to an encryption
    scheme that is not secure, does not perform well, and/or does not support the
    input and computation of the desired application. We highly recommend consulting
    an expert in RLWE-based encryption when selecting parameters, as this is where
    inexperienced users seem to most often make critical mistakes.
    */
    class EncryptionParameters
    {
    public:
        /**
        The data type to store a hash block.
        */
        typedef util::HashFunction::sha3_block_type hash_block_type;

        /**
        Creates an empty set of encryption parameters. At a minimum, the user needs to 
        specify the parameters poly_modulus, coeff_modulus, and plain_modulus for the
        parameters to be usable.
        */
        EncryptionParameters();

        /**
        Creates a copy of a given instance of EncryptionParameters.

        @param[in] copy The EncryptionParameters to copy from
        */
        EncryptionParameters(const EncryptionParameters &copy) = default;

        /**
        Overwrites the EncryptionParameters instance with a copy of a given instance.

        @param[in] assign The EncryptionParameters to copy from
        */
        EncryptionParameters &operator =(const EncryptionParameters &assign) = default;

        /**
        Creates a new EncryptionParameters instance by moving a given instance.

        @param[in] source The EncryptionParameters to move from
        */
        EncryptionParameters(EncryptionParameters &&source) = default;

        /**
        Overwrites the EncryptionParameters instance by moving a given instance.

        @param[in] assign The EncryptionParameters to move from
        */
        EncryptionParameters &operator =(EncryptionParameters &&assign) = default;

        /**
        Sets the polynomial modulus parameter to the specified value (represented by 
        BigPoly). The polynomial modulus directly affects the number of coefficients 
        in plaintext polynomials, the size of ciphertext elements, the computational
        performance of the scheme (bigger is worse), and the security level (bigger 
        is better). In SEAL the polynomial modulus must be of the form "1x^N + 1",
        where N is a power of 2 (e.g. 1024, 2048, 4096, 8192, 16384, or 32768).

        @param[in] poly_modulus The new polynomial modulus
        */
        inline void set_poly_modulus(const BigPoly &poly_modulus)
        {
            // Set the poly_modulus to be as small as possible
            poly_modulus_.resize(1, 1);

            // Now operator =(...) automatically resizes to (significant_coeff_count, significant_coeff_bit_count) size
            poly_modulus_ = poly_modulus;

            // Re-compute the hash
            compute_hash();
        }

        /**
        Sets the polynomial modulus parameter to the specified value (represented by
        std::string). The polynomial modulus directly affects the number of coefficients
        in plaintext polynomials, the size of ciphertext elements, the computational
        performance of the scheme (bigger is worse), and the security level (bigger 
        is better). In SEAL the polynomial modulus must be of the form "1x^N + 1",
        where N is a power of 2 (e.g. 1024, 2048, 4096, 8192, 16384, or 32768).

        @param[in] poly_modulus The new polynomial modulus
        */
        inline void set_poly_modulus(const std::string &poly_modulus)
        {
            // Needed to enable char[] arguments
            set_poly_modulus(BigPoly(poly_modulus));
        }

        /**
        Sets the coefficient modulus parameter. The coefficient modulus consists of a list 
        of distinct prime numbers, and is represented by a vector of SmallModulus objects. 
        The coefficient modulus directly affects the size of ciphertext elements, the 
        amount of computation that the scheme can perform (bigger is better), and the 
        security level (bigger is worse). In SEAL each of the prime numbers in the 
        coefficient modulus must be at most 60 bits, and must be congruent to 1 modulo 
        2*degree(poly_modulus).

        @param[in] coeff_modulus The new coefficient modulus
        */
        inline void set_coeff_modulus(const std::vector<SmallModulus> &coeff_modulus)
        {
            // Set the coeff_modulus_
            coeff_modulus_ = coeff_modulus;

            // Re-compute the hash
            compute_hash();
        }

        /**
        Sets the plaintext modulus parameter. The plaintext modulus is an integer modulus
        represented by the SmallModulus class. The plaintext modulus determines the largest
        coefficient that plaintext polynomials can represent. It also affects the amount of
        computation that the scheme can perform (bigger is worse). In SEAL the plaintext 
        modulus can be at most 60 bits long, but can otherwise be any integer. Note, however, 
        that some features (e.g. batching) require the plaintext modulus to be of a particular 
        form. 

        @param[in] plain_modulus The new plaintext modulus
        */
        inline void set_plain_modulus(const SmallModulus &plain_modulus)
        {
            plain_modulus_ = plain_modulus;

            // Re-compute the hash
            compute_hash();
        }

        /**
        Sets the plaintext modulus parameter. The plaintext modulus is an integer modulus
        represented by the SmallModulus class. This constructor instead takes a std::uint64_t
        and automatically creates the SmallModulus object. The plaintext modulus determines 
        the largest coefficient that plaintext polynomials can represent. It also affects the 
        amount of computation that the scheme can perform (bigger is worse). In SEAL the 
        plaintext modulus can be at most 60 bits long, but can otherwise be any integer. Note,
        however, that some features (e.g. batching) require the plaintext modulus to be of 
        a particular form.

        @param[in] plain_modulus The new plaintext modulus
        */
        inline void set_plain_modulus(std::uint64_t plain_modulus)
        {
            set_plain_modulus(SmallModulus(plain_modulus));
        }

        /**
        Sets the standard deviation of the noise distribution used for error sampling. This 
        parameter directly affects the security level of the scheme. However, it should not be
        necessary for most users to change this parameter from its default value.

        @param[in] noise_standard_deviation The new standard deviation
        */
        inline void set_noise_standard_deviation(double noise_standard_deviation)
        {
            noise_standard_deviation_ = noise_standard_deviation;
            noise_max_deviation_ = util::global_variables::noise_distribution_width_multiplier * noise_standard_deviation_;

            // Re-compute the hash
            compute_hash();
        }

        /**
        Sets the random number generator factory to use for encryption. By default, the random
        generator is set to UniformRandomGeneratorFactory::default_factory(). Setting this value
        allows a user to specify a custom random number generator source.

        @param[in] random_generator Pointer to the random generator factory
        */
        inline void set_random_generator(UniformRandomGeneratorFactory *random_generator)
        {
            random_generator_ = random_generator;
        }

        /**
        Returns a constant reference to the currently set polynomial modulus parameter.
        */
        inline const BigPoly &poly_modulus() const
        {
            return poly_modulus_;
        }

        /**
        Returns a constant reference to the currently set coefficient modulus parameter.
        */
        inline const std::vector<SmallModulus> &coeff_modulus() const
        {
            return coeff_modulus_;
        }

        /**
        Returns a constant reference to the currently set plaintext modulus parameter.
        */
        inline const SmallModulus &plain_modulus() const
        {
            return plain_modulus_;
        }

        /**
        Returns the currently set standard deviation of the noise distribution.
        */
        inline double noise_standard_deviation() const
        {
            return noise_standard_deviation_;
        }

        /**
        Returns the currently set maximum deviation of the noise distribution. This value
        cannot be directly controlled by the user, and is automatically set to be an 
        appropriate multiple of the noise_standard_deviation parameter.
        */
        inline double noise_max_deviation() const
        {
            return noise_max_deviation_;
        }

        /**
        Returns a pointer to the random number generator factory to use for encryption.
        */
        inline UniformRandomGeneratorFactory *random_generator() const
        {
            return random_generator_;
        }

        /**
        Compares a given set of encryption parameters to the current set of encryption 
        parameters. The comparison is performed by comparing hash blocks of the parameter 
        sets rather than comparing the parameters individually.

        @parms[in] other The EncryptionParameters to compare against
        */
        inline bool operator ==(const EncryptionParameters &other) const
        {
            return (hash_block_ == other.hash_block_);
        }

        /**
        Compares a given set of encryption parameters to the current set of encryption
        parameters. The comparison is performed by comparing hash blocks of the parameter
        sets rather than comparing the parameters individually.

        @parms[in] other The EncryptionParameters to compare against
        */
        inline bool operator !=(const EncryptionParameters &other) const
        {
            return (hash_block_ != other.hash_block_);
        }

        /**
        Saves the EncryptionParameters to an output stream. The output is in binary format
        and is not human-readable. The output stream must have the "Binary" flag set.

        @param[in] stream The stream to save the EncryptionParameters to
        @see load() to load a saved EncryptionParameters instance.
        */
        void save(std::ostream &stream) const;

        /**
        Loads the EncryptionParameters from an input stream overwriting the current 
        EncryptionParameters.

        @param[in] stream The stream to load the EncryptionParameters from
        @see save() to save an EncryptionParameters instance.
        */
        void load(std::istream &stream);

        /**
        Returns the hash block of the current parameters. This function is intended
        mainly for internal use.
        */
        inline const hash_block_type &hash_block() const
        {
            return hash_block_;
        }

        /**
        Enables access to private members of seal::EncryptionParameters for .NET wrapper.
        */
        struct EncryptionParametersPrivateHelper;

    private:
        void compute_hash();

        BigPoly poly_modulus_;

        SmallModulus plain_modulus_;

        std::vector<SmallModulus> coeff_modulus_;

        double noise_standard_deviation_ = util::global_variables::default_noise_standard_deviation;

        double noise_max_deviation_ = util::global_variables::noise_distribution_width_multiplier 
            * util::global_variables::default_noise_standard_deviation;

        UniformRandomGeneratorFactory *random_generator_ = nullptr;

        // C++11 compatibility
        hash_block_type hash_block_{ { 0 } };

        friend class SEALContext;
    };
}
