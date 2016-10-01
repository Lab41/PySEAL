#pragma once

#include <cstdint>
#include <vector>
#include "encryptionparams.h"
#include "util/modulus.h"
#include "util/polymodulus.h"
#include "util/ntt.h"
#include "bigpoly.h"
#include "biguint.h"

namespace seal
{
    /**
    Provides functionality for encrypting several plaintext numbers into one ciphertext
    for improved memory efficiency and efficient vector operations (SIMD). Multiplying and 
    adding such ciphertexts together performs the respective operation on each of the slots 
    independently and simultaneously. This functionality is often called "batching" in
    homomorphic encryption literature.

    @par Mathematical Background
    Mathematically speaking, if poly_modulus is X^N+1, N is a power of two, and plain_modulus 
    is a prime number t such that 2N divides t-1, then integers modulo t contain a primitive 
    2N-th root of unity and the polynomial X^N+1 splits into n distinct linear factors as 
    X^N+1 = (X-a_1)*...*(X-a_N) mod t, where the constants a_1, ..., a_n are all the distinct 
    primitive 2N-th roots of unity in integers modulo t. The Chinese Remainder Theorem (CRT) 
    states that the plaintext space Z_t[X]/(X^N+1) in this case is isomorphic (as an algebra) 
    to the N-fold direct product of fields Z_t. The isomorphism is easy to compute explicitly 
    in both directions, which is what an instance of this class can be used for.

    @par Valid Parameters
    Whether batching can be used depends strongly on the encryption parameters. Thus, to
    construct a PolyCRTBuilder the user must provide an instance of EncryptionParameters
    such that the EncryptionParameterQualifiers object returned by the function 
    EncryptionParameters::GetQualifiers() has the flags parameters_set and enable_batching
    set to true.

    @see EncryptionParameters for more information about encryption parameters.
    @see EncryptionParameterQualifiers for more information about encryption parameters that support batching.
    */
    class PolyCRTBuilder
    {
    public:
        /**
        Creates a PolyCRTBuilder instance given a set of encryption parameters.
        It is necessary that the given set of encryption parameters supports batching.

        @param[in] parms The encryption parameters
        @throws std::invalid_argument if parms are not valid or do not support batching
        @see EncryptionParameters for more information about encryption parameters.
        @see EncryptionParameterQualifiers for more information about encryption parameters that support batching.
        */
        PolyCRTBuilder(const EncryptionParameters &parms);

        /**
        Destroys the PolyCRTBuilder and deallocates all memory used by it.
        */
        ~PolyCRTBuilder();

        /**
        Writes a given vector of unsigned integers modulo the plaintext modulus into the slots of a given 
        plaintext polynomial. 

        @par Format of Parameters
        The number of elements in the vector of inputs must be equal to the number of slots, which is
        equal to the degree of the polynomial modulus. Each entry in the vector of inputs must have
        bit count equal to that of the plaintext modulus and is expected to be already reduced modulo 
        the plaintext modulus. The destination polynomial will automatically be resized to have correct 
        size, i.e. the same number of coefficients as the polynomial modulus, and each coefficient of the 
        same bit count as the plaintext modulus.

        @param[in] values The vector of values to write into the slots
        @param[out] destination The plaintext polynomial to overwrite with the result
        @throws std::invalid_argument if the values vector has incorrect size
        @throws std::invalid_argument if the entries in the values vector have incorrect size
        */
        void compose(const std::vector<BigUInt> &values, BigPoly &destination) const;

        /**
        Writes a given vector of unsigned integers modulo the plaintext modulus into the slots of
        a plaintext polynomial, and returns it.

        @par Format of Parameters
        The number of elements in the vector of inputs must be equal to the number of slots, which is
        equal to the degree of the polynomial modulus. Each entry in the vector of inputs must have
        bit count equal to that of the plaintext modulus and is expected to be already reduced modulo
        the plaintext modulus. The returned polynomial will automatically be resized to have correct
        size, i.e. the same number of coefficients as the polynomial modulus, and each coefficient of the
        same bit count as the plaintext modulus.

        @param[in] values The vector of values to write into the slots
        @throws std::invalid_argument if the values vector has incorrect size
        @throws std::invalid_argument if the entries in the values vector have incorrect size
        */
        BigPoly compose(const std::vector<BigUInt> &values)
        {
            BigPoly destination(poly_modulus_.coeff_count(), slot_modulus_.significant_bit_count());
            compose(values, destination);

            return destination;
        }

        /**
        Reads the values in the slots of a given plaintext polynomial and writes them as the entries of 
        a given vector. This is the inverse of what compose() does.

        @par Format of Parameters
        The plaintext polynomial poly must have the same number of coefficients as the polynomial modulus,
        and its coefficients must have bit count equal to that of the plaintext modulus. Moreover, poly is
        expected to be already reduced modulo polynomial modulus, and its coefficients are expected
        to be already reduced modulo the plaintext modulus. The number of elements in the destination
        vector will be set to equal the number of slots, the elements themselves will be set to have
        bit count equal to that of the plaintext modulus, and their values will be reduced modulo the 
        plaintext modulus.

        @param[in] poly The plaintext polynomial from which the slots will be read
        @param[out] destination The vector to be overwritten with the values of the slots
        @throws std::invalid_argument if poly has incorrect size
        */
        void decompose(const BigPoly &poly, std::vector<BigUInt> &destination) const;

        /**
        Reads the values in the slots of a given plaintext polynomial and writes them as the entries of
        a vector, which is then returned. This is the inverse of what compose() does.

        @par Format of Parameters
        The plaintext polynomial poly must have the same number of coefficients as the polynomial modulus,
        and its coefficients must have bit count equal to that of the plaintext modulus. Moreover, poly is
        expected to be already reduced modulo polynomial modulus, and its coefficients are expected
        to be already reduced modulo the plaintext modulus. The number of elements in the returned vector
        will be equal the number of slots, the elements themselves will have bit count equal to that
        of the plaintext modulus, and their values will be reduced modulo the plaintext modulus.

        @param[in] poly The plaintext polynomial from which the slots will be read
        @throws std::invalid_argument if poly has incorrect size
        */
        std::vector<BigUInt> decompose(const BigPoly &poly)
        {
            std::vector<BigUInt> destination;
            decompose(poly, destination);

            return destination;
        }

        /**
        Returns the number of slots.
        */
        int get_slot_count()
        {
            return slots_;
        }

    private:
        void populate_roots_of_unity_vector();
        
        PolyCRTBuilder(const PolyCRTBuilder &copy) = delete;

        PolyCRTBuilder &operator =(const PolyCRTBuilder &assign) = delete;

        util::NTTTables ntt_tables_;

        util::Modulus slot_modulus_;

        util::PolyModulus poly_modulus_;

        int slots_;

        util::Pointer roots_of_unity_;

        EncryptionParameterQualifiers qualifiers_;
    };
}