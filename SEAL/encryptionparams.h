#ifndef SEAL_ENCRYPTIONPARAMS_H
#define SEAL_ENCRYPTIONPARAMS_H

#include <iostream>
#include "biguint.h"
#include "bigpoly.h"
#include "randomgen.h"

namespace seal
{
    /**
    Enumeration of the different modes for the library. Mode is set through EncryptionParameters::mode(). NORMAL_MODE
    performs all encryption, evaluation, and decryption operations as normal. TEST_MODE disables actual encryption
    while allowing the API to still function for the application. TEST_MODE enables significantly faster library
    calls and may be very useful for testing and debugging applications.

    @warning TEST_MODE disables actual encryption in the library, so only use TEST_MODE for testing and debugging
    purposes when no security is required.
    @see EncryptionParameters to set mode for the library.
    */
    enum EncryptionMode
    {
        /**
        NORMAL_MODE enables encryption and the normal functioning of the library.
        */
        NORMAL_MODE,

        /**
        TEST_MODE disables encryption in the library. The benefit of TEST_MODE is that most library functions
        (e.g., Encryptor::encrypt(), Decryptor::decrypt(), all Evaluator functions) complete significantly faster as
        they are not secure. This mode is useful when testing and debugging applications when security is not important.

        @warning TEST_MODE disables actual encryption in the library, so only use TEST_MODE for testing and debugging
        purposes when no security is required.
        */
        TEST_MODE
    };

    /**
    Represents the user-customizable encryption scheme settings. Several settings (e.g., poly_modulus(), coeff_modulus(),
    plain_modulus()) significantly affect the performance, capabilities, and security of the encryption scheme. KeyGenerator,
    Encryptor, Decryptor, Evaluator, and other objects in the library all require the EncryptionParameters object to specify
    and agree on the encryption scheme settings.

    Picking appropriate encryption parameters is essential to enable a particular application while balancing performance and
    security. Some encryption settings will not allow some inputs (e.g., attempting to encrypt a polynomial with more
    coefficients than poly_modulus() or larger coefficients than plain_modulus()) or support some computations (with noise
    growing too fast as determined by coeff_modulus() and decomposition_bit_count()). The Chooser and Simulator classes
    provide functionality to help determine the best encryption parameters for an application. Additionally, please refer to
    external documentation to determine the best parameters.

    @par Thread Safety
    In general, reading from EncryptionParameters is thread-safe while mutating is not. Refer to the thread safety of
    individual parameter types to assess their thread safety.

    @warning Choosing inappropriate EncryptionParameters may load to an encryption scheme that is not secure, does not perform
    well, and/or does not support the input and computation of the application.
    @see Chooser and Simulator to help determine the best encryption parameters for an application.
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
        Returns a reference to the coefficient modulus (represented by a BigUInt) used for encryption. Note that the
        coefficient modulus directly determines the number of bits-per-coefficient of encrypted polynomials and the maximum
        value allowed for plain_modulus() (which should be significantly smaller than coeff_modulus()).
        */
        const BigUInt &coeff_modulus() const
        {
            return coeff_modulus_;
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
        Returns a reference to the operating mode of the library, which is set to either NORMAL_MODE or TEST_MODE.
        Setting the mode to TEST_MODE disables all encryption but may significantly improve performance for cases where
        security is not important (e.g., testing and debugging).

        @warning Only set the mode to TEST_MODE when security is not important.
        */
        EncryptionMode &mode()
        {
            return mode_;
        }

        /**
        Returns the operating mode of the library, which is set to either NORMAL_MODE or TEST_MODE. Setting the mode
        to TEST_MODE disables all encryption but may significantly improve performance for cases where security is not
        important (e.g., testing and debugging).

        @warning Only set the mode to TEST_MODE when security is not important.
        */
        EncryptionMode mode() const
        {
            return mode_;
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

    private:
        BigPoly poly_modulus_;

        BigUInt coeff_modulus_;

        BigUInt plain_modulus_;

        double noise_standard_deviation_;

        double noise_max_deviation_;

        int decomposition_bit_count_;

        EncryptionMode mode_;

        UniformRandomGeneratorFactory *random_generator_;
    };
}

#endif // SEAL_ENCRYPTIONPARAMS_H
