#pragma once

#include <iostream>
#include <utility>
#include <string>
#include "biguint.h"
#include "bigpoly.h"
#include "randomgen.h"
#include "util/ntt.h"
#include "memorypoolhandle.h"

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
    @see EncryptionParameters::GetQualifiers for obtaining the EncryptionParameterQualifiers 
    corresponding to a certain parameter set.
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
    Represents the user-customizable encryption scheme settings. The parameters (e.g., poly_modulus(), coeff_modulus(),
    plain_modulus()) significantly affect the performance, capabilities, and security of the encryption scheme. KeyGenerator,
    Encryptor, Decryptor, Evaluator, and other objects in the library all require the EncryptionParameters object to specify
    and agree on the encryption scheme settings.

    Picking appropriate encryption parameters is essential to enable a particular application while balancing performance and
    security. Some encryption settings will not allow some inputs (e.g., attempting to encrypt a polynomial with more
    coefficients than poly_modulus() or larger coefficients than plain_modulus()) or support some computations (with noise
    growing too fast as determined by coeff_modulus() and decomposition_bit_count()). The ChooserPoly and ChooserEvaluator
    classes provide functionality to help determine the best encryption parameters for an application. Additionally, please 
    refer to external documentation to determine the best parameters.

    @par Validating Parameters
    After the user has set at least the poly_modulus(), the coeff_modulus(), and the plain_modulus() using the functions 
    set_poly_modulus(), set_coeff_modulus(), and set_plain_modulus(), respectively, and in many cases also the 
    decomposition_bit_count() using set_decomposition_bit_count(), the parameters need to be validated for correctness
    and functionality. This can be done using the function validate(), which returns an instance of EncryptionParameterQualifiers.
    If the returned instance of EncryptionParameterQualifiers has the EncryptionParameterQualifiers::parameters_set flag 
    set to true, the parameter set is valid and ready to be used. If the parameters were for some reason not appropriately
    set, the returned EncryptionParameterQualifiers instance will have EncryptionParameterQualifiers::parameters_set set to false.
    Any change to an already validated instance of EncryptionParameters immediately invalidates it, requiring another call
    to validate() before the parameters can be used.

    @par Thread Safety
    In general, reading from EncryptionParameters is thread-safe, while mutating is not.

    @warning Choosing inappropriate EncryptionParameters may load to an encryption scheme that is not secure, does not perform
    well, and/or does not support the input and computation of the application.
    */
    class EncryptionParameters
    {
    public:
        /**
        Creates an empty EncryptionParameters instance. Optionally, the user can give a reference to a 
        MemoryPoolHandle object to use a custom memory pool instead of the global memory pool (default).

        @param[in] pool The memory pool handle
        @see MemoryPoolHandle for more details on memory pool handles.
        */
        EncryptionParameters(const MemoryPoolHandle &pool = MemoryPoolHandle::acquire_global());

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
        Sets the polynomial modulus parameter to the specified value (represented by BigPoly). Note that the
        polynomial modulus directly determines the number of coefficients of encrypted polynomials, and the
        maximum number of coefficients for plaintext polynomials that are representable by the library.
        This function automatically invalidates the EncryptionParameters instance, so calling validate() is 
        necessary before the parameters can be used.

        @param[in] poly_modulus The new polynomial modulus
        */
        void set_poly_modulus(const BigPoly &poly_modulus);

        /**
        Sets the polynomial modulus parameter to the specified value (represented by std::string). Note that the
        polynomial modulus directly determines the number of coefficients of encrypted polynomials, and the
        maximum number of coefficients for plaintext polynomials that are representable by the library.
        This function automatically invalidates the EncryptionParameters instance, so calling validate() is
        necessary before the parameters can be used.

        @param[in] poly_modulus The new polynomial modulus
        */
        inline void set_poly_modulus(const std::string &poly_modulus)
        {
            // Needed to enable char[] arguments
            set_poly_modulus(BigPoly(poly_modulus));
        }

        /**
        Sets the coefficient modulus parameter to the specified value (represented by BigUInt). Note that the
        coefficient modulus directly determines the number of bits-per-coefficient of encrypted polynomials and 
        the maximum value allowed for plain_modulus() (which should be significantly smaller than coeff_modulus()).
        This function automatically invalidates the EncryptionParameters instance, so calling validate() is
        necessary before the parameters can be used.

        @param[in] coeff_modulus The new coefficient modulus
        */
        void set_coeff_modulus(const BigUInt &coeff_modulus);

        /**
        Sets the coefficient modulus parameter to the specified value (represented by std::uint64_t). Note that the
        coefficient modulus directly determines the number of bits-per-coefficient of encrypted polynomials and
        the maximum value allowed for plain_modulus() (which should be significantly smaller than coeff_modulus()).
        This function automatically invalidates the EncryptionParameters instance, so calling validate() is
        necessary before the parameters can be used.

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
        Sets the coefficient modulus parameter to the specified value (represented by std::string). Note that the
        coefficient modulus directly determines the number of bits-per-coefficient of encrypted polynomials and
        the maximum value allowed for plain_modulus() (which should be significantly smaller than coeff_modulus()).
        This function automatically invalidates the EncryptionParameters instance, so calling validate() is
        necessary before the parameters can be used.

        @param[in] coeff_modulus The new coefficient modulus
        */
        inline void set_coeff_modulus(const std::string &coeff_modulus)
        {
            // Needed to enable char[] arguments
            set_coeff_modulus(BigUInt(coeff_modulus));
        }
#ifndef DISABLE_NTT_IN_MULTIPLY
        // Not currently used
        void set_aux_coeff_modulus(const BigUInt &aux_coeff_modulus);

        // Not currently used
        inline void set_aux_coeff_modulus(std::uint64_t aux_coeff_modulus)
        {
            // Make sure NOT to call BigUInt(int)
            BigUInt new_aux_coeff_modulus;
            new_aux_coeff_modulus = aux_coeff_modulus;
            set_aux_coeff_modulus(new_aux_coeff_modulus);
        }

        // Not currently used
        inline void set_aux_coeff_modulus(const std::string &aux_coeff_modulus)
        {
            // Needed to enable char[] arguments
            set_aux_coeff_modulus(BigUInt(aux_coeff_modulus));
        }
#endif
        /**
        Sets the plaintext modulus parameter to the specified value (represented by BigUInt). Note that the
        plaintext modulus is one greater than the maximum value allowed for any plaintext coefficient that 
        the library can encrypt or represent. This function automatically invalidates the EncryptionParameters 
        instance, so calling validate() is necessary before the parameters can be used.

        @param[in] plain_modulus The new plaintext modulus
        */
        void set_plain_modulus(const BigUInt &plain_modulus);
        
        /**
        Sets the plaintext modulus parameter to the specified value (represented by std::uint64_t). Note that the
        plaintext modulus is one greater than the maximum value allowed for any plaintext coefficient that
        the library can encrypt or represent. This function automatically invalidates the EncryptionParameters
        instance, so calling validate() is necessary before the parameters can be used.

        @param[in] plain_modulus The new plaintext modulus
        */
        inline void set_plain_modulus(std::uint64_t plain_modulus)
        {
            // Make sure NOT to call BigUInt(int)
            BigUInt new_plain_modulus;
            new_plain_modulus = plain_modulus;
            set_plain_modulus(new_plain_modulus);
        }

        /**
        Sets the plaintext modulus parameter to the specified value (represented by std::string). Note that the
        plaintext modulus is one greater than the maximum value allowed for any plaintext coefficient that
        the library can encrypt or represent. This function automatically invalidates the EncryptionParameters
        instance, so calling validate() is necessary before the parameters can be used.

        @param[in] plain_modulus The new plaintext modulus
        */
        inline void set_plain_modulus(const std::string &plain_modulus)
        {
            // Needed to enable char[] arguments
            set_plain_modulus(BigUInt(plain_modulus));
        }

        /**
        Sets the decomposition bit count parameter to the specified value. The decomposition bit count directly
        determines the number of evaluation keys required by the scheme. Smaller decomposition bit count reduces 
        the accumulation of noise during multiplication operations, but can also significantly increase the time 
        required to perform multiplication. This function automatically invalidates the EncryptionParameters 
        instance, so calling validate() is necessary before the parameters can be used.

        @param[in] decomposition_bit_count The new decomposition bit count
        */
        void set_decomposition_bit_count(int decomposition_bit_count);

        /**
        Sets the standard deviation of normalized noise used during key generation and encryption. This function 
        automatically invalidates the EncryptionParameters instance, so calling validate() is necessary before the 
        parameters can be used.

        @param[in] noise_standard_deviation The new standard deviation
        */
        void set_noise_standard_deviation(double noise_standard_deviation);

        /**
        Sets the maximum deviation of normalized noise used during key generation and encryption. This function
        automatically invalidates the EncryptionParameters instance, so calling validate() is necessary before the
        parameters can be used.

        @param[in] noise_max_deviation The new maximum deviation
        */
        void set_noise_max_deviation(double noise_max_deviation);

        /**
        Sets the random number generator factory to use for encryption. By default, the random generator is set to
        UniformRandomGeneratorFactory::default_factory(). Setting this value allows a user to specify a custom random number
        generator source. This function automatically invalidates the EncryptionParameters instance, so calling validate() 
        is necessary before the parameters can be used.

        @param[in] random_generator Pointer to the random generator factory
        */
        void set_random_generator(UniformRandomGeneratorFactory *random_generator);

        /**
        Validates the parameters set in the current EncryptionParameters instance, and returns a populated instance
        of EncryptionParameterQualifiers. It is necessary to call this function before using the the encryption
        parameters to create other classes such as KeyGenerator, Encryptor, and Evaluator. Note also that any change
        to the parameters requires the validate() function to be called again before the parameters can be used.

        @see EncryptionParameterQualifiers for more details on the returned qualifiers.
        */
        EncryptionParameterQualifiers validate();

        /**
        Returns a constant reference to the polynomial modulus parameter (represented by BigPoly). Note that 
        the polynomial modulus directly determines the number of coefficients of encrypted polynomials, and the
        maximum number of coefficients for plaintext polynomials that are representable by the library.
        */
        inline const BigPoly &poly_modulus() const
        {
            return poly_modulus_;
        }

        /**
        Returns a constant reference to the coefficient modulus parameter (represented by a BigUInt). Note that the
        coefficient modulus directly determines the number of bits-per-coefficient of encrypted polynomials and the 
        maximum value allowed for plain_modulus() (which should be significantly smaller than coeff_modulus()).
        */
        inline const BigUInt &coeff_modulus() const
        {
            return coeff_modulus_;
        }
#ifndef DISABLE_NTT_IN_MULTIPLY       
        // Not currently used
        inline const BigUInt &aux_coeff_modulus() const
        {
            return aux_coeff_modulus_;
        }
#endif
        /**
        Returns a constant reference to the plaintext modulus parameter (represented by a BigUInt). Note that the
        plaintext modulus is one greater than the maximum value allowed for any plaintext coefficient that the library
        can encrypt or represent.
        */
        inline const BigUInt &plain_modulus() const
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
        Returns the decomposition bit count which directly determines the number of evaluation keys required by
        the scheme. Smaller decomposition bit count reduces the accumulation of noise during multiplication
        operations, but can also significantly increase the time required to perform multiplication.
        */
        inline int decomposition_bit_count() const
        {
            return decomposition_bit_count_;
        }

        /**
        Returns the random number generator factory to use for encryption. By default, the random generator is set to
        UniformRandomGeneratorFactory::default_factory(). Setting this value allows a user to specify a custom random number
        generator source.
        */
        inline UniformRandomGeneratorFactory *random_generator() const
        {
            return random_generator_;
        }

        /**
        Saves the EncryptionParameters to an output stream. The output is in binary format and is not 
        human-readable. The output stream must have the "Binary" flag set.

        @param[in] stream The stream to save the EncryptionParameters to
        @see load() to load a saved EncryptionParameters instance.
        */
        void save(std::ostream &stream) const;

        /**
        Loads the EncryptionParameters from an input stream overwriting the current EncryptionParameters. 
        This function automatically invalidates the EncryptionParameters instance, so calling validate() 
        is necessary before the parameters can be used.

        @param[in] stream The stream to load the EncryptionParameters from
        @see save() to save an EncryptionParameters instance.
        */
        void load(std::istream &stream);

        /**
        Returns the set of qualifiers (as an instance of EncryptionParameterQualifiers) for the current
        encryption parameters. Note that to get an updated set of qualifiers it is necessary to call
        validate() after any change to the encryption parameters.

        @see EncryptionParameterQualifiers for more details on the qualifiers.
        */
        inline EncryptionParameterQualifiers get_qualifiers() const
        {
            return qualifiers_;
        }

        /**
        Computes and returns the maximum number of bits of inherent noise supported by the current encryption
        parameters. Any ciphertext with larger inherent noise is impossible to decrypt, even with the correct 
        secret key. The function Decryptor::inherent_noise_bits() can be used to compute the number of bits
        of inherent noise in a given ciphertext.

        @throws std::logic_error if the encryption parameters are not valid
        @see inherent_noise_max for computing the exact maximum size of inherent noise.
        */
        int inherent_noise_bits_max()
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

        @param[in] destination The BigUInt to overwrite with the maximum inherent noise
        @throws std::logic_error if the encryption parameters are not valid
        @see inherent_noise_bits_max for returning instead the significant bit count of the maximum size of inherent noise.
        */
        void inherent_noise_max(BigUInt &destination);

        /**
        Invalidates the current encryption parameters. Calling validate() is necessary before the parameters 
        can be used again.
        */
        void invalidate();

    private:
        MemoryPoolHandle pool_;

        BigPoly poly_modulus_;

        BigUInt coeff_modulus_;

        BigUInt aux_coeff_modulus_;

        BigUInt plain_modulus_;

        int decomposition_bit_count_;

        double noise_standard_deviation_;

        double noise_max_deviation_;

        UniformRandomGeneratorFactory *random_generator_;

        bool validated_;

        EncryptionParameterQualifiers qualifiers_;

        util::NTTTables ntt_tables_;

        util::NTTTables aux_ntt_tables_;

        util::NTTTables plain_ntt_tables_;

        friend class Decryptor;

        friend class Encryptor;

        friend class Evaluator;

        friend class PolyCRTBuilder;
    };
}