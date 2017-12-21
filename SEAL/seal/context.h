#pragma once

#include <iostream>
#include <utility>
#include <string>
#include <array>
#include "seal/encryptionparams.h"
#include "seal/biguint.h"
#include "seal/bigpoly.h"
#include "seal/randomgen.h"
#include "seal/memorypoolhandle.h"
#include "seal/util/smallntt.h"
#include "seal/util/baseconverter.h"

namespace seal
{
    /**
    Stores a set of attributes (qualifiers) of a set of encryption parameters.
    These parameters are mainly used internally in various parts of the library, e.g.
    to determine which algorithmic optimizations the current support. The qualifiers 
    are automatically created by the SEALContext class, silently passed on to classes 
    such as Encryptor, Evaluator, and Decryptor, and the only way to change them is by 
    changing the encryption parameters themselves. In other words, a user will never 
    have to create their own instance of EncryptionParameterQualifiers, and in most
    cases never have to worry about them at all.

    @see EncryptionParameters::GetQualifiers for obtaining the EncryptionParameterQualifiers
    corresponding to the current parameter set.
    */
    struct EncryptionParameterQualifiers
    {
        /**
        If the encryption parameters are set in a way that is considered valid by SEAL, the
        variable parameters_set is set to true.
        */
        bool parameters_set;

        /**
        Tells whether FFT can be used for polynomial multiplication. If the polynomial modulus
        is of the form X^N+1, where N is a power of two, then FFT can be used for fast 
        multiplication of polynomials modulo the polynomial modulus. In this case the 
        variable enable_fft will be set to true. However, currently SEAL requires this
        to be the case for the parameters to be valid. Therefore, parameters_set can only
        be true if enable_fft is true.
        */
        bool enable_fft;

        /**
        Tells whether NTT can be used for polynomial multiplication. If the primes in the 
        coefficient modulus are congruent to 1 modulo 2N, where X^N+1 is the polynomial 
        modulus and N is a power of two, then the number-theoretic transform (NTT) can be 
        used for fast multiplications of polynomials modulo the polynomial modulus and 
        coefficient modulus. In this case the variable enable_ntt will be set to true. However, 
        currently SEAL requires this to be the case for the parameters to be valid. Therefore, 
        parameters_set can only be true if enable_ntt is true.
        */
        bool enable_ntt;

        /**
        Tells whether batching is supported by the encryption parameters. If the plaintext 
        modulus is congruent to 1 modulo 2N, where X^N+1 is the polynomial modulus and N is
        a power of two, then it is possible to use the PolyCRTBuilder class to view plaintext 
        elements as 2-by-(N/2) matrices of integers modulo the plaintext modulus. This is 
        called batching, and allows the user to operate on the matrix elements (slots) in 
        a SIMD fashion, and rotate the matrix rows and columns. When the computation is 
        easily vectorizable, using batching can yield a huge performance boost. If the 
        encryption parameters support batching, the variable enable_batching is set to true.
        */
        bool enable_batching;

        /**
        Tells whether fast plain lift is supported by the encryption parameters. A certain 
        performance optimization in multiplication of a ciphertext by a plaintext 
        (Evaluator::multiply_plain) and in transforming a plaintext element to NTT domain
        (Evaluator::transform_to_ntt) can be used when the plaintext modulus is smaller than
        each prime in the coefficient modulus. In this case the variable enable_fast_plain_lift
        is set to true.
        */
        bool enable_fast_plain_lift;

    private:
        EncryptionParameterQualifiers() :
            parameters_set(false),
            enable_fft(false),
            enable_ntt(false),
            enable_batching(false),
            enable_fast_plain_lift(false)
        {
        }

        friend class SEALContext;
    };

    /**
    Performs sanity checks (validation) and pre-computations for a given set of encryption
    parameters. While the EncryptionParameters class is intended to be a light-weight class 
    to store the encryption parameters, the SEALContext class is a heavy-weight class that 
    is constructed from a given set of encryption parameters. It validates the parameters 
    for correctness, evaluates their properties, and performs and stores the results of
    several costly pre-computations.

    After the user has set at least the poly_modulus, coeff_modulus, and plain_modulus
    parameters in a given EncryptionParameters instance, the parameters can be validated 
    for correctness and functionality by constructing an instance of SEALContext. The 
    constructor of SEALContext does all of its work automatically, and concludes by 
    constructing and storing an instance of the EncryptionParameterQualifiers class, with
    its flags set according to the properties of the given parameters. If the created 
    instance of EncryptionParameterQualifiers has the parameters_set flag set to true, the
    given parameter set has been deemed valid and is ready to be used. If the parameters 
    were for some reason not appropriately set, the parameters_set flag will be false,
    and a new SEALContext will have to be created after the parameters are corrected.

    @see EncryptionParameters for more details on the parameters.
    @see EncryptionParameterQualifiers for more details on the qualifiers.
    */
    class SEALContext
    {
    public:
        /**
        Creates an instance of SEALContext, and performs several pre-computations on the 
        given EncryptionParameters. The results of the pre-computations are stored in 
        allocations from the memory pool pointed to by the optionally given 
        MemoryPoolHandle. By default the global memory pool is used.

        @param[in] parms The encryption parameters
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if pool is uninitialized
        */
        SEALContext(const EncryptionParameters &parms,
            const MemoryPoolHandle &pool = MemoryPoolHandle::Global());

        /**
        Creates a new SEALContext instance by creating a deep copy of a given instance.

        @param[in] copy The SEALContext to copy from
        */
        SEALContext(const SEALContext &copy) = default;

        /**
        Overwrites the current SEALContext instance by a deep copy of a given instance.

        @param[in] assign The SEALContext instance to overwrite the current instance
        */
        SEALContext &operator =(const SEALContext &assign) = default;

        /**
        Creates a new SEALContext instance by moving a given instance.

        @param[in] source The SEALContext to move from
        */
        SEALContext(SEALContext &&source) = default;

        /**
        Overwrites the current SEALContext instance moving a given instance.

        @param[in] assign The SEALContext to move from
        */
        SEALContext &operator =(SEALContext &&assign) = default;

        /**
        Returns a constant reference to the underlying encryption parameters.
        */
        inline const EncryptionParameters &parms() const
        {
            return parms_;
        }

        /**
        Returns a copy of EncryptionParameterQualifiers corresponding to the current
        encryption parameters. Note that to change the qualifiers it is necessary to 
        create a new instance of SEALContext once appropriate changes to the encryption
        parameters have been made.
        */
        inline EncryptionParameterQualifiers qualifiers() const
        {
            return qualifiers_;
        }

        /**
        Returns a constant reference to the polynomial modulus that was given in the 
        encryption parameters.
        */
        inline const BigPoly &poly_modulus() const
        {
            return parms_.poly_modulus();
        }

        /**
        Returns a constant reference to the coefficient modulus that was given in the
        encryption parameters.
        */
        inline const std::vector<SmallModulus> &coeff_modulus() const
        {
            return parms_.coeff_modulus();
        }

        /**
        Returns a constant reference to the plaintext modulus that was given in the
        encryption parameters.
        */
        inline const SmallModulus &plain_modulus() const
        {
            return parms_.plain_modulus();
        }

        /**
        Returns the standard deviation of the noise distribution that was given in the
        encryption parameters.
        */
        inline double noise_standard_deviation() const
        {
            return parms_.noise_standard_deviation();
        }

        /**
        Returns the maximum deviation of the noise distribution that was given in the
        encryption parameters.
        */
        inline double noise_max_deviation() const
        {
            return parms_.noise_max_deviation();
        }

        /**
        Returns a constant reference to a pre-computed product of all primes in the
        coefficient modulus. The security of the encryption parameters largely depends
        on the bit-length of this product, and on the degree of the polynomial modulus.
        */
        inline const BigUInt &total_coeff_modulus() const
        {
            return total_coeff_modulus_;
        }

        /**
        Returns a constant pointer to the random number generator factory that was given
        in the encryption parameters.
        */
        inline const UniformRandomGeneratorFactory *random_generator() const
        {
            return parms_.random_generator();
        }

    private:
        EncryptionParameterQualifiers validate();

        MemoryPoolHandle pool_;

        EncryptionParameters parms_;

        EncryptionParameterQualifiers qualifiers_;

        util::BaseConverter base_converter_;

        std::vector<util::SmallNTTTables> small_ntt_tables_;

        util::SmallNTTTables plain_ntt_tables_;

        BigUInt total_coeff_modulus_;

        friend class Decryptor;

        friend class Encryptor;

        friend class Evaluator;

        friend class PolyCRTBuilder;

        friend class KeyGenerator;

        friend class PolyCRTBuilder;
    };
}
