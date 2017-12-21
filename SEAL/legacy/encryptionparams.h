#pragma once

#include <iostream>
#include <utility>
#include <string>
#include <array>
#include "biguint.h"
#include "bigpoly.h"
#include "randomgen.h"
#include "util/ntt.h"
#include "memorypoolhandle.h"
#include "util/hash.h"
#include "smallmodulus.h"

namespace seal
{
    /**
    Represents the user-customizable encryption scheme settings. The parameters 
    (e.g., poly_modulus, coeff_modulus, plain_modulus) significantly affect the 
    performance, capabilities, and security of the encryption scheme. Once an instance 
    of EncryptionParameters is populated with appropriate parameters, it can be used to create 
    an instance of the SEALContext class, which additionally ensures the validity of 
    the parameters, and performs and stores pre-computation. KeyGenerator, Encryptor,
    Decryptor, Evaluator, and other objects in the library all require the SEALContext 
    object to specify and agree on the encryption scheme settings and pre-computations.

    Picking appropriate encryption parameters is essential to enable a particular 
    application while balancing performance and security. Some encryption settings
    will not allow some inputs (e.g., attempting to encrypt a polynomial with more
    coefficients than poly_modulus or larger coefficients than plain_modulus) 
    or support some computations (with noise growing too fast as determined by 
    coeff_modulus and decomposition_bit_count). The ChooserPoly and ChooserEvaluator 
    classes provide functionality to help determine the best encryption parameters 
    for an application. Additionally, please refer to external documentation to 
    determine the best parameters.

    @par Hash block
    The EncryptionParameters class keeps a track of a 128-bit hash block of the 
    currently set parameters. Classes such as Encryptor then pass on this hash value 
    to the Ciphertext objects that they create. This is mostly due to internal reasons:
    it becomes very easy to check whether a particular ciphertext is of the correct
    format by simply checking that it carries the correct hash block with it. This is
    because mutating a Ciphertext object is not possible except through these tool
    classes. The SecretKey, PublicKey, and EvaluationKeys classes behave similarly.

    @par Thread Safety
    In general, reading from EncryptionParameters is thread-safe, while mutating is not.

    @warning Choosing inappropriate encryption parameters may lead to an encryption 
    scheme that is not secure, does not perform well, and/or does not support the 
    input and computation of the application.
    */
    class EncryptionParameters
    {
    public:
        typedef util::HashFunction::sha3_block_type hash_block_type;

        /**
        Creates an empty encryption parameters. At a minimum, the user needs to specify the parameters 
        poly_modulus, coeff_modulus, and plain_modulus. 

        @see the documentation of the library for more details on the different parameters and
        their security implications.
        */
        EncryptionParameters();

        /**
        Creates a copy of a EncryptionParameters.

        @param[in] copy The EncryptionParameters to copy from
        */
        EncryptionParameters(const EncryptionParameters &copy) = default;

        /**
        Overwrites the EncryptionParameters instance with the specified instance.

        @param[in] assign The EncryptionParameters instance that should be assigned to the current instance
        */
        EncryptionParameters &operator =(const EncryptionParameters &assign) = default;

        /**
        Creates a new EncryptionParameters by moving an old one.

        @param[in] source The EncryptionParameters to move from
        */
        EncryptionParameters(EncryptionParameters &&source) = default;

        /**
        Moves an old EncryptionParameters instance to the current one.

        @param[in] assign The EncryptionParameters to move from
        */
        EncryptionParameters &operator =(EncryptionParameters &&assign) = default;

        /**
        Sets the polynomial modulus parameter to the specified value (represented by BigPoly). 
        Note that the polynomial modulus directly determines the number of coefficients of 
        encrypted polynomials, and the maximum number of coefficients for plaintext polynomials 
        that are representable by the library.

        @param[in] poly_modulus The new polynomial modulus
        */
        void set_poly_modulus(const BigPoly &poly_modulus);

        /**
        Sets the polynomial modulus parameter to the specified value (represented by std::string). 
        Note that the polynomial modulus directly determines the number of coefficients of 
        encrypted polynomials, and the maximum number of coefficients for plaintext polynomials 
        that are representable by the library.

        @param[in] poly_modulus The new polynomial modulus
        */
        inline void set_poly_modulus(const std::string &poly_modulus)
        {
            // Needed to enable char[] arguments
            set_poly_modulus(BigPoly(poly_modulus));
        }

        /**
        Sets the coefficient modulus parameter to the specified value (represented by BigUInt). 
        Note that the coefficient modulus directly determines the number of bits-per-coefficient 
        of encrypted polynomials and the maximum value allowed for plain_modulus() (which should 
        be significantly smaller than coeff_modulus()).

        @param[in] coeff_modulus The new coefficient modulus
        */
        void set_coeff_modulus(const BigUInt &coeff_modulus);

        /**
        Sets the coefficient modulus parameter to the specified value (represented by std::uint64_t). 
        Note that the coefficient modulus directly determines the number of bits-per-coefficient 
        of encrypted polynomials and the maximum value allowed for plain_modulus() (which should 
        be significantly smaller than coeff_modulus()).

        @param[in] coeff_modulus The new coefficient modulus
        */
        inline void set_coeff_modulus(std::uint64_t coeff_modulus)
        {
            // Make sure NOT to call BigUInt(int)
            BigUInt new_coeff_modulus;
            new_coeff_modulus = coeff_modulus;
            set_coeff_modulus(new_coeff_modulus);
        }

        /**
        Sets the coefficient modulus parameter to the specified value (represented by std::string). 
        Note that the coefficient modulus directly determines the number of bits-per-coefficient 
        of encrypted polynomials and the maximum value allowed for plain_modulus() (which should 
        be significantly smaller than coeff_modulus()).

        @param[in] coeff_modulus The new coefficient modulus
        */
        inline void set_coeff_modulus(const std::string &coeff_modulus)
        {
            // Needed to enable char[] arguments
            set_coeff_modulus(BigUInt(coeff_modulus));
        }

        /**
        Sets the plaintext modulus parameter to the specified value (represented by SmallModulus). 
        Note that the plaintext modulus is one greater than the maximum value allowed for any 
        plaintext coefficient that the library can encrypt or represent.

        @param[in] plain_modulus The new plaintext modulus
        */
        void set_plain_modulus(const SmallModulus &plain_modulus);

        /**
        Sets the plaintext modulus parameter to the specified value (represented by std::uint64_t). 
        Note that the plaintext modulus is one greater than the maximum value allowed for any 
        plaintext coefficient that the library can encrypt or represent.

        @param[in] plain_modulus The new plaintext modulus
        */
        inline void set_plain_modulus(std::uint64_t plain_modulus)
        {
            set_plain_modulus(SmallModulus(plain_modulus));
        }

        /**
        Sets the decomposition bit count parameter to the specified value. The decomposition bit 
        count directly determines the number of evaluation keys required by the scheme. Smaller 
        decomposition bit count reduces the accumulation of noise during multiplication operations, 
        but can also significantly increase the time required to perform multiplication. 

        @param[in] decomposition_bit_count The new decomposition bit count
        */
        void set_decomposition_bit_count(int decomposition_bit_count);

        /**
        Sets the standard deviation of normalized noise used during key generation and encryption.

        @param[in] noise_standard_deviation The new standard deviation
        */
        void set_noise_standard_deviation(double noise_standard_deviation);

        /**
        Sets the maximum deviation of normalized noise used during key generation and encryption. 

        @param[in] noise_max_deviation The new maximum deviation
        */
        void set_noise_max_deviation(double noise_max_deviation);

        /**
        Sets the random number generator factory to use for encryption. By default, the random 
        generator is set to UniformRandomGeneratorFactory::default_factory(). Setting this value 
        allows a user to specify a custom random number generator source. 

        @param[in] random_generator Pointer to the random generator factory
        */
        void set_random_generator(UniformRandomGeneratorFactory *random_generator);

        /**
        Returns a constant reference to the polynomial modulus parameter (represented by BigPoly). 
        Note that the polynomial modulus directly determines the number of coefficients of encrypted 
        polynomials, and the maximum number of coefficients for plaintext polynomials that are 
        representable by the library.
        */
        inline const BigPoly &poly_modulus() const
        {
            return poly_modulus_;
        }

        /**
        Returns a constant reference to the coefficient modulus parameter (represented by a BigUInt). 
        Note that the coefficient modulus directly determines the number of bits-per-coefficient 
        of encrypted polynomials and the maximum value allowed for plain_modulus() (which should 
        be significantly smaller than coeff_modulus()).
        */
        inline const BigUInt &coeff_modulus() const
        {
            return coeff_modulus_;
        }

        /**
        Returns a constant reference to the plaintext modulus parameter (represented by a SmallModulus). 
        Note that the plaintext modulus is one greater than the maximum value allowed for any 
        plaintext coefficient that the library can encrypt or represent.
        */
        inline const SmallModulus &plain_modulus() const
        {
            return plain_modulus_;
        }

        /**
        Returns the standard deviation of normalized noise used during key generation and encryption.
        */
        inline double noise_standard_deviation() const
        {
            return noise_standard_deviation_;
        }

        /**
        Returns the maximum deviation of normalized noise used during key generation and encryption.
        */
        inline double noise_max_deviation() const
        {
            return noise_max_deviation_;
        }

        /**
        Returns the decomposition bit count which directly determines the number of evaluation keys 
        required by the scheme. Smaller decomposition bit count reduces the accumulation of noise 
        during multiplication operations, but can also significantly increase the time required 
        to perform multiplication.
        */
        inline int decomposition_bit_count() const
        {
            return decomposition_bit_count_;
        }

        /**
        Returns the random number generator factory to use for encryption. By default, the random 
        generator is set to UniformRandomGeneratorFactory::default_factory(). Setting this value 
        allows a user to specify a custom random number generator source.
        */
        inline UniformRandomGeneratorFactory *random_generator() const
        {
            return random_generator_;
        }

        /**
        Compares a given set of encryption parameters to the current set of encryption parameters.
        The comparison is performed by comparing hashes of the parameter sets, rather than comparing
        the parameters individually.
        */
        bool operator ==(const EncryptionParameters &other) const
        {
            return (hash_block_ == other.hash_block_);
        }

        /**
        Compares a given set of encryption parameters to the current set of encryption parameters.
        The comparison is performed by comparing hashes of the parameter sets, rather than comparing
        the parameters individually.
        */
        bool operator !=(const EncryptionParameters &other) const
        {
            return (hash_block_ != other.hash_block_);
        }

        /**
        Saves the EncryptionParameters to an output stream. The output is in binary format and 
        is not human-readable. The output stream must have the "Binary" flag set.

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
        Returns a constant reference to the hash block.
        */
        inline const hash_block_type &get_hash_block() const
        {
            return hash_block_;
        }

    private:
        void compute_hash();

        BigPoly poly_modulus_;

        BigUInt coeff_modulus_;

        BigUInt aux_coeff_modulus_;

        SmallModulus plain_modulus_;

        int decomposition_bit_count_;

        double noise_standard_deviation_;

        double noise_max_deviation_;

        UniformRandomGeneratorFactory *random_generator_;

        hash_block_type hash_block_;
    };
}