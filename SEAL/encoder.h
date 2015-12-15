#ifndef SEAL_ENCODER_H
#define SEAL_ENCODER_H

#include <cstdint>
#include "biguint.h"
#include "bigpoly.h"
#include "util/mempool.h"

namespace seal
{
    /**
    Encodes integers into plaintext polynomials that Encryptor can encrypt. An instance of
    the BinaryEncoder class converts an integer into a plaintext polynomial by placing its
    binary digits as the coefficients of the polynomial. Decoding the integer amounts to
    evaluating the plaintext polynomial at X=2.

    Addition and multiplication on the integer side translate into addition and multiplication
    on the encoded plaintext polynomial side, provided that the length of the polynomial
    never grows to be of the size of the polynomial modulus (poly_modulus), and that the
    coefficients of the plaintext polynomials appearing throughout the computations never
    experience coefficients larger than the plaintext modulus (plain_modulus).

    @par Negative Integers
    Negative integers are represented by using -1 instead of 1 in the binary representation,
    and the negative coefficients are stored in the plaintext polynomials as unsigned integers
    that represent them modulo the plaintext modulus. Thus, for example, a coefficient of -1
    would be stored as a polynomial coefficient plain_modulus-1.
  
    @par Thread Safety
    The BinaryEncoder class is not thread-safe and a separate BinaryEncoder instance is
    needed for each potentially concurrent usage.

    @see BinaryFractionalEncoder for encoding real numbers.
    @see BalancedEncoder for encoders using other than binary representations for more compact encodings.
    */
    class BinaryEncoder
    {
    public:
        /**
        Creates a BinaryEncoder object. The constructor takes as input a reference
        to the plaintext modulus (represented by BigUInt).

        @param[in] plain_modulus The plaintext modulus (represented by BigUInt)
        @throws std::invalid_argument if plain_modulus is not at least 2
        */
        BinaryEncoder(const BigUInt &plain_modulus);

        /**
        Encodes an unsigned integer (represented by std::uint64_t) into a plaintext polynomial.
        
        @param[in] value The unsigned integer to encode
        */
        BigPoly encode(std::uint64_t value);

        /**
        Encodes an unsigned integer (represented by std::uint64_t) into a plaintext polynomial.  

        @param[in] value The unsigned integer to encode
        @param[out] destination The polynomial to overwrite with the encoding
        */
        void encode(std::uint64_t value, BigPoly &destination);

        /**
        Decodes a plaintext polynomial and returns the result as std::uint32_t.
        Mathematically this amounts to evaluating the input polynomial at X=2.

        @param[in] poly The polynomial to be decoded
        @throws std::invalid_argument if the output does not fit in std::uint32_t
        */
        std::uint32_t decode_uint32(const BigPoly &poly);

        /**
        Decodes a plaintext polynomial and returns the result as std::uint64_t.
        Mathematically this amounts to evaluating the input polynomial at X=2.

        @param[in] poly The polynomial to be decoded
        @throws std::invalid_argument if the output does not fit in std::uint32_t
        */
        std::uint64_t decode_uint64(const BigPoly &poly);

        /**
        Encodes a signed integer (represented by std::int64_t) into a plaintext polynomial.

        @par Negative Integers
        Negative integers are represented by using -1 instead of 1 in the binary representation,
        and the negative coefficients are stored in the plaintext polynomials as unsigned integers
        that represent them modulo the plaintext modulus. Thus, for example, a coefficient of -1
        would be stored as a polynomial coefficient plain_modulus-1.

        @param[in] value The signed integer to encode
        */
        BigPoly encode(std::int64_t value);

        /**
        Encodes a signed integer (represented by std::int64_t) into a plaintext polynomial.

        @par Negative Integers
        Negative integers are represented by using -1 instead of 1 in the binary representation,
        and the negative coefficients are stored in the plaintext polynomials as unsigned integers
        that represent them modulo the plaintext modulus. Thus, for example, a coefficient of -1
        would be stored as a polynomial coefficient plain_modulus-1.

        @param[in] value The signed integer to encode
        @param[out] destination The polynomial to overwrite with the encoding
        */
        void encode(std::int64_t value, BigPoly &destination);

        /**
        Encodes an unsigned integer (represented by BigUInt) into a plaintext polynomial.

        @param[in] value The unsigned integer to encode
        */
        BigPoly encode(const BigUInt &value);

        /**
        Encodes an unsigned integer (represented by BigUInt) into a plaintext polynomial.

        @param[in] value The unsigned integer to encode
        @param[out] destination The polynomial to overwrite with the encoding
        */
        void encode(const BigUInt &value, BigPoly &destination);

        /**
        Decodes a plaintext polynomial and returns the result as std::int32_t.
        Mathematically this amounts to evaluating the input polynomial at X=2.

        @param[in] poly The polynomial to be decoded
        @throws std::invalid_argument if poly is not a valid plaintext polynomial
        @throws std::invalid_argument if the output does not fit in std::int32_t
        */
        std::int32_t decode_int32(const BigPoly &poly);

        /**
        Decodes a plaintext polynomial and returns the result as std::int64_t.
        Mathematically this amounts to evaluating the input polynomial at X=2.

        @param[in] poly The polynomial to be decoded
        @throws std::invalid_argument if poly is not a valid plaintext polynomial
        @throws std::invalid_argument if the output does not fit in std::int64_t
        */
        std::int64_t decode_int64(const BigPoly &poly);

        /**
        Decodes a plaintext polynomial and returns the result as BigUInt.
        Mathematically this amounts to evaluating the input polynomial at X=2.

        @param[in] poly The polynomial to be decoded
        @throws std::invalid_argument if poly is not a valid plaintext polynomial
        @throws std::invalid_argument if the output is negative
        */
        BigUInt decode_biguint(const BigPoly &poly);

        /**
        Decodes a plaintext polynomial and stores the result in a given BigUInt.
        Mathematically this amounts to evaluating the input polynomial at X=2.

        @param[in] poly The polynomial to be decoded
        @param[out] destination The BigUInt to overwrite with the decoding
        @throws std::invalid_argument if poly is not a valid plaintext polynomial
        @throws std::invalid_argument if the output does not fit in destination
        @throws std::invalid_argument if the output is negative
        */
        void decode_biguint(const BigPoly &poly, BigUInt &destination);

        /**
        Encodes a signed integer (represented by std::int32_t) into a plaintext polynomial.
        
        @par Negative Integers
        Negative integers are represented by using -1 instead of 1 in the binary representation,
        and the negative coefficients are stored in the plaintext polynomials as unsigned integers
        that represent them modulo the plaintext modulus. Thus, for example, a coefficient of -1
        would be stored as a polynomial coefficient plain_modulus-1.

        @param[in] value The signed integer to encode
        */
        BigPoly encode(std::int32_t value)
        {
            return encode(static_cast<std::int64_t>(value));
        }

        /**
        Encodes an unsigned integer (represented by std::uint32_t) into a plaintext polynomial.

        @param[in] value The unsigned integer to encode
        */
        BigPoly encode(std::uint32_t value)
        {
            return encode(static_cast<std::uint64_t>(value));
        }

        /**
        Encodes a signed integer (represented by std::int32_t) into a plaintext polynomial.

        @par Negative Integers
        Negative integers are represented by using -1 instead of 1 in the binary representation,
        and the negative coefficients are stored in the plaintext polynomials as unsigned integers
        that represent them modulo the plaintext modulus. Thus, for example, a coefficient of -1
        would be stored as a polynomial coefficient plain_modulus-1.

        @param[in] value The signed integer to encode
        @param[out] destination The polynomial to overwrite with the encoding
        */
        void encode(std::int32_t value, BigPoly &destination)
        {
            encode(static_cast<std::int64_t>(value), destination);
        }

        /**
        Encodes an unsigned integer (represented by std::uint32_t) into a plaintext polynomial.

        @param[in] value The unsigned integer to encode
        @param[out] destination The polynomial to overwrite with the encoding
        */
        void encode(std::uint32_t value, BigPoly &destination)
        {
            encode(static_cast<std::uint64_t>(value), destination);
        }

        /**
        Returns a reference to the plaintext modulus.
        */
        const BigUInt &plain_modulus() const
        {
            return plain_modulus_;
        }

    private:
        BigUInt plain_modulus_;

        BigUInt coeff_neg_threshold_;

        BigUInt neg_one_;

        util::MemoryPool pool_;

        BinaryEncoder(const BinaryEncoder &copy) = delete;

        BinaryEncoder &operator =(const BinaryEncoder &assign) = delete;

        friend class BinaryFractionalEncoder;
    };

    /**
    Encodes integers into plaintext polynomials that Encryptor can encrypt. An instance of
    the BalancedEncoder class converts an integer into a plaintext polynomial by placing its
    digits in balanced base-b representation as the coefficients of the polynomial.
    We restrict to values of the base b to be odd positive integers and at least 3 (which is
    the default value). Digits in such a balanced representation are integers in the range
    -(b-1)/2,...,(b-1)/2. The default value for the base b is 3, which allows for more compact
    representation than BinaryEncoder, without increasing the sizes of the coefficients of 
    freshly encoded plaintext polynomials. A larger base allows for an even more compact
    representation at the cost of having larger coefficients in freshly encoded plaintext
    polynomials. Decoding the integer amounts to evaluating the plaintext polynomial at X=b. 

    Addition and multiplication on the integer side translate into addition and multiplication
    on the encoded plaintext polynomial side, provided that the length of the polynomial
    never grows to be of the size of the polynomial modulus (poly_modulus), and that the
    coefficients of the plaintext polynomials appearing throughout the computations never
    experience coefficients larger than the plaintext modulus (plain_modulus).

    @par Negative Integers
    Negative integers in the balanced base-b encoding are represented the same way as
    positive integers, namely, both positive and negative integers can have both positive and negative
    digits in their balanced base-b representation. Negative coefficients are stored in the
    plaintext polynomials as unsigned integers that represent them modulo the plaintext modulus.
    Thus, for example, a coefficient of -1 would be stored as a polynomial coefficient plain_modulus-1.

    @par Thread Safety
    The BalancedEncoder class is not thread-safe and a separate BalancedEncoder instance is
    needed for each potentially concurrent usage.

    @see BalancedFractionalEncoder for encoding real numbers.
    @see BinaryEncoder for encoding using the binary representation.
    */
    class BalancedEncoder
    {
    public:
        /**
        Creates a BalancedEncoder object. The constructor takes as input a reference
        to the plaintext modulus (represented by BigUInt), and optionally an odd integer,
        at least 3, that is used as a base in the encoding.

        @param[in] plain_modulus The plaintext modulus (represented by BigUInt)
        @param[in] base The base to be used for encoding
        @throws std::invalid_argument if base is not an odd integer and at least 3
        @throws std::invalid_argument if plain_modulus is not at least (base+1)/2
        */
        BalancedEncoder(const BigUInt &plain_modulus, uint64_t base = 3);

        /**
        Encodes an unsigned integer (represented by std::uint64_t) into a plaintext polynomial.

        @param[in] value The unsigned integer to encode
        */
        BigPoly encode(std::uint64_t value);

        /**
        Encodes an unsigned integer (represented by std::uint64_t) into a plaintext polynomial.

        @param[in] value The unsigned integer to encode
        @param[out] destination The polynomial to overwrite with the encoding
        */
        void encode(std::uint64_t value, BigPoly &destination);

        /**
        Decodes a plaintext polynomial and returns the result as std::uint32_t.
        Mathematically this amounts to evaluating the input polynomial at X=base.

        @param[in] poly The polynomial to be decoded
        @throws std::invalid_argument if poly is not a valid plaintext polynomial
        @throws std::invalid_argument if the output does not fit in std::uint32_t
        */
        std::uint32_t decode_uint32(const BigPoly &poly);

        /**
        Decodes a plaintext polynomial and returns the result as std::uint64_t.
        Mathematically this amounts to evaluating the input polynomial at X=base.

        @param[in] poly The polynomial to be decoded
        @throws std::invalid_argument if poly is not a valid plaintext polynomial
        @throws std::invalid_argument if the output does not fit in std::uint64_t
        */
        std::uint64_t decode_uint64(const BigPoly &poly);

        /**
        Encodes a signed integer (represented by std::int64_t) into a plaintext polynomial.

        @par Negative Integers
        Negative integers in the balanced base-b encoding are represented the same way as
        positive integers, namely, both positive and negative integers can have both positive and negative
        digits in their balanced base-b representation. Negative coefficients are stored in the
        plaintext polynomials as unsigned integers that represent them modulo the plaintext modulus.
        Thus, for example, a coefficient of -1 would be stored as a polynomial coefficient plain_modulus-1.

        @param[in] value The signed integer to encode
        */
        BigPoly encode(std::int64_t value);

        /**
        Encodes a signed integer (represented by std::int64_t) into a plaintext polynomial.

        @par Negative Integers
        Negative integers in the balanced base-b encoding are represented the same way as
        positive integers, namely, both positive and negative integers can have both positive and negative
        digits in their balanced base-b representation. Negative coefficients are stored in the
        plaintext polynomials as unsigned integers that represent them modulo the plaintext modulus.
        Thus, for example, a coefficient of -1 would be stored as a polynomial coefficient plain_modulus-1.

        @param[in] value The signed integer to encode
        @param[out] destination The polynomial to overwrite with the encoding
        */
        void encode(std::int64_t value, BigPoly &destination);

        /**
        Encodes an unsigned integer (represented by BigUInt) into a plaintext polynomial.

        @param[in] value The unsigned integer to encode
        */
        BigPoly encode(const BigUInt &value);

        /**
        Encodes an unsigned integer (represented by BigUInt) into a plaintext polynomial.

        @param[in] value The unsigned integer to encode
        @param[out] destination The polynomial to overwrite with the encoding
        */
        void encode(BigUInt value, BigPoly &destination);

        /**
        Decodes a plaintext polynomial and returns the result as std::int32_t.
        Mathematically this amounts to evaluating the input polynomial at X=base.

        @param[in] poly The polynomial to be decoded
        @throws std::invalid_argument if poly is not a valid plaintext polynomial
        @throws std::invalid_argument if the output does not fit in std::int32_t
        */
        std::int32_t decode_int32(const BigPoly &poly);

        /**
        Decodes a plaintext polynomial and returns the result as std::int64_t.
        Mathematically this amounts to evaluating the input polynomial at X=base.

        @param[in] poly The polynomial to be decoded
        @throws std::invalid_argument if poly is not a valid plaintext polynomial
        @throws std::invalid_argument if the output does not fit in std::int64_t
        */
        std::int64_t decode_int64(const BigPoly &poly);

        /**
        Decodes a plaintext polynomial and returns the result as BigUInt.
        Mathematically this amounts to evaluating the input polynomial at X=base.

        @param[in] poly The polynomial to be decoded
        @throws std::invalid_argument if poly is not a valid plaintext polynomial
        @throws std::invalid_argument if the output is negative
        */
        BigUInt decode_biguint(const BigPoly &poly);

        /**
        Decodes a plaintext polynomial and stores the result in a given BigUInt.
        Mathematically this amounts to evaluating the input polynomial at X=base.

        @param[in] poly The polynomial to be decoded
        @param[out] destination The BigUInt to overwrite with the decoding
        @throws std::invalid_argument if poly is not a valid plaintext polynomial
        @throws std::invalid_argument if the output does not fit in destination
        @throws std::invalid_argument if the output is negative
        */
        void decode_biguint(const BigPoly &poly, BigUInt &destination);

        /**
        Encodes a signed integer (represented by std::int32_t) into a plaintext polynomial.

        @par Negative Integers
        Negative integers in the balanced base-b encoding are represented the same way as
        positive integers, namely, both positive and negative integers can have both positive and negative
        digits in their balanced base-b representation. Negative coefficients are stored in the
        plaintext polynomials as unsigned integers that represent them modulo the plaintext modulus.
        Thus, for example, a coefficient of -1 would be stored as a polynomial coefficient plain_modulus-1.

        @param[in] value The signed integer to encode
        */
        BigPoly encode(std::int32_t value)
        {
            return encode(static_cast<std::int64_t>(value));
        }

        /**
        Encodes an unsigned integer (represented by std::uint32_t) into a plaintext polynomial.

        @param[in] value The unsigned integer to encode
        */
        BigPoly encode(std::uint32_t value)
        {
            return encode(static_cast<std::uint64_t>(value));
        }

        /**
        Encodes a signed integer (represented by std::int32_t) into a plaintext polynomial.

        @par Negative Integers
        Negative integers in the balanced base-b encoding are represented the same way as
        positive integers, namely, both positive and negative integers can have both positive and negative
        digits in their balanced base-b representation. Negative coefficients are stored in the
        plaintext polynomials as unsigned integers that represent them modulo the plaintext modulus.
        Thus, for example, a coefficient of -1 would be stored as a polynomial coefficient plain_modulus-1.

        @param[in] value The signed integer to encode
        @param[out] destination The polynomial to overwrite with the encoding
        */
        void encode(std::int32_t value, BigPoly &destination)
        {
            encode(static_cast<std::int64_t>(value), destination);
        }

        /**
        Encodes an unsigned integer (represented by std::uint32_t) into a plaintext polynomial.

        @param[in] value The unsigned integer to encode
        @param[out] destination The polynomial to overwrite with the encoding
        */
        void encode(std::uint32_t value, BigPoly &destination)
        {
            encode(static_cast<std::uint64_t>(value), destination);
        }

        /**
        Returns a reference to the plaintext modulus.
        */
        const BigUInt &plain_modulus() const
        {
            return plain_modulus_;
        }

        /**
        Returns the base used for encoding.
        */
        uint64_t base() const
        {
            return base_;
        }

    private:
        BigUInt plain_modulus_;

        uint64_t base_;

        BigUInt coeff_neg_threshold_;

        util::MemoryPool pool_;

        BalancedEncoder(const BalancedEncoder &copy) = delete;

        BalancedEncoder &operator =(const BalancedEncoder &assign) = delete;

        friend class BalancedFractionalEncoder;
    };

    /**
    Encodes floating point numbers into plaintext polynomials that Encryptor can encrypt.
    An instance of the BinaryFractionalEncoder class converts a double-precision floating-point
    number into a plaintext polynomial by computing its binary representation, encoding the
    integral part as in BinaryEncoder, and the fractional part as the highest degree
    terms of the plaintext polynomial, with signs inverted. Decoding the polynomial
    back into a double amounts to evaluating the low degree part at X=2, negating the
    coefficients of the high degree part and evaluating it at X=1/2.

    Addition and multiplication on the double side translate into addition and multiplication
    on the encoded plaintext polynomial side, provided that the integral part never mixes
    with the fractional part in the plaintext polynomials, and that the
    coefficients of the plaintext polynomials appearing throughout the computations never
    experience coefficients larger than the plaintext modulus (plain_modulus).

    @par Integral and Fractional Parts
    When homomorphic multiplications are performed, the integral part "grows up" to higher
    degree coefficients of the plaintext polynomial space, and the fractional part "grows down"
    from the top degree coefficients towards the lower degree coefficients. For decoding to work,
    these parts must not interfere with each other. When setting up the BinaryFractionalEncoder,
    onem ust specify how many coefficients of a plaintext polynomial are reserved for the integral 
    part and how many for the fractional. The sum of these numbers can be at most equal to the
    degree of the polynomial modulus minus one. If homomorphic multiplications are performed, it is
    also necessary to leave enough room for the fractional part to "grow down". 

    @par Negative Integers
    Negative integers are represented by using -1 instead of 1 in the binary representation,
    and the negative coefficients are stored in the plaintext polynomials as unsigned integers
    that represent them modulo the plaintext modulus. Thus, for example, a coefficient of -1
    would be stored as a polynomial coefficient plain_modulus-1.

    @par Thread Safety
    The BinaryFractionalEncoder class is not thread-safe and a separate BinaryFractionalEncoder
    instance is needed for each potentially concurrent usage.

    @see BinaryEncoder for encoding integers.
    @see BalancedFractionalEncoder for encoders using other than binary representations for more compact encodings.
    */
    class BinaryFractionalEncoder
    {
    public:
        /**
        Creates a new BinaryFractionalEncoder object. The constructor takes as input a reference
        to the plaintext modulus, a reference to the polynomial modulus,
        and the numbers of coefficients that are reserved for the integral and fractional parts.
        The coefficients for the integral part are counted starting from the low-degree end
        of the polynomial, and the coefficients for the fractional part are counted from the
        high-degree end.

        @param[in] plain_modulus The plaintext modulus (represented by BigUInt)
        @param[in] poly_modulus The polynomial modulus (represented by BigPoly)
        @param[in] integer_coeff_count The number of polynomial coefficients reserved for the integral part
        @param[in] fraction_coeff_count The number of polynomial coefficients reserved for the fractional part
        @throws std::invalid_argument if plain_modulus is not at least 2
        @throws std::invalid_argument if integer_coeff_count is not strictly positive
        @throws std::invalid_argument if fraction_coeff_count is not strictly positive
        @throws std::invalid_argument if poly_modulus is zero
        @throws std::invalid_argument if poly_modulus is too small for the integral and fractional parts 
        */
        BinaryFractionalEncoder(const BigUInt &plain_modulus, const BigPoly &poly_modulus, int integer_coeff_count, int fraction_coeff_count);

        /**
        Encodes a double precision floating point number into a plaintext polynomial.

        @param[in] value The double-precision floating-point number to encode
        */
        BigPoly encode(double value);

        /**
        Decodes a plaintext polynomial and returns the result as a double-precision
        floating-point number.

        @param[in] poly The polynomial to be decoded
        @throws std::invalid_argument if poly is not a valid plaintext polynomial
        @throws std::invalid_argument if the integral part does not fit in std::int64_t
        */
        double decode(const BigPoly &poly);

        /**
        Returns a reference to the plaintext modulus.
        */
        const BigUInt &plain_modulus() const
        {
            return encoder_.plain_modulus();
        }

    private:
        BinaryFractionalEncoder(const BinaryFractionalEncoder &copy) = delete;

        BinaryFractionalEncoder &operator =(const BinaryFractionalEncoder &assign) = delete;

        BinaryEncoder encoder_;

        util::MemoryPool pool_;

        int fraction_coeff_count_;

        int integer_coeff_count_;

        BigPoly poly_modulus_;
    };

    /**
    Encodes floating point numbers into plaintext polynomials that Encryptor can encrypt.
    An instance of the BalancedFractionalEncoder class converts a double-precision floating-point
    number into a plaintext polynomial by computing its balanced base-b representation, encoding the
    integral part as in BalancedEncoder, and the fractional part as the highest degree
    terms of the plaintext polynomial, with signs inverted. Decoding the polynomial
    back into a double amounts to evaluating the low degree part at X=base, negating the
    coefficients of the high degree part and evaluating it at X=1/base.

    Addition and multiplication on the double side translate into addition and multiplication
    on the encoded plaintext polynomial side, provided that the integral part never mixes
    with the fractional part in the plaintext polynomials, and that the
    coefficients of the plaintext polynomials appearing throughout the computations never
    experience coefficients larger than the plaintext modulus (plain_modulus).

    @par Integral and Fractional Parts
    When homomorphic multiplications are performed, the integral part "grows up" to higher
    degree coefficients of the plaintext polynomial space, and the fractional part "grows down"
    from the top degree coefficients towards the lower degree coefficients. For decoding to work,
    these parts must not interfere with each other. When setting up the BalancedFractionalEncoder,
    onem ust specify how many coefficients of a plaintext polynomial are reserved for the integral
    part and how many for the fractional. The sum of these numbers can be at most equal to the
    degree of the polynomial modulus minus one. If homomorphic multiplications are performed, it is
    also necessary to leave enough room for the fractional part to "grow down".
  
    @par Negative Integers
    Negative integers in the balanced base-b encoding are represented the same way as
    positive integers, namely, both positive and negative integers can have both positive and negative
    digits in their balanced base-b representation. Negative coefficients are stored in the
    plaintext polynomials as unsigned integers that represent them modulo the plaintext modulus.
    Thus, for example, a coefficient of -1 would be stored as a polynomial coefficient plain_modulus-1.

    @par Thread Safety
    The BalancedFractionalEncoder class is not thread-safe and a separate BalancedFractionalEncoder
    instance is needed for each potentially concurrent usage.

    @see BalancedEncoder for encoding integers.
    @see BinaryFractionalEncoder for encoding using the binary representation.
    */
    class BalancedFractionalEncoder
    {
    public:
        /**
        Creates a new BalancedFractionalEncoder object. The constructor takes as input a reference
        to the plaintext modulus, a reference to the polynomial modulus,
        and the numbers of coefficients that are reserved for the integral and fractional parts,
        and optionally an odd integer, at least 3, that is used as the base in the encoding.
        The coefficients for the integral part are counted starting from the low-degree end
        of the polynomial, and the coefficients for the fractional part are counted from the
        high-degree end.

        @param[in] plain_modulus The plaintext modulus (represented by BigUInt)
        @param[in] poly_modulus The polynomial modulus (represented by BigPoly)
        @param[in] integer_coeff_count The number of polynomial coefficients reserved for the integral part
        @param[in] fraction_coeff_count The number of polynomial coefficients reserved for the fractional part
        @param[in] base The base (default value is 3)
        @throws std::invalid_argument if plain_modulus is not at least (base + 1) / 2
        @throws std::invalid_argument if integer_coeff_count is not strictly positive
        @throws std::invalid_argument if fraction_coeff_count is not strictly positive
        @throws std::invalid_argument if poly_modulus is zero
        @throws std::invalid_argument if poly_modulus is too small for the integral and fractional parts
        @throws std::invalid_argument if base is not an odd integer and at least 3
        */
        BalancedFractionalEncoder(const BigUInt &plain_modulus, const BigPoly &poly_modulus, int integer_coeff_count, int fraction_coeff_count, uint64_t base = 3);

        /**
        Encodes a double precision floating point number into a plaintext polynomial.

        @param[in] value The double-precision floating-point number to encode
        */
        BigPoly encode(double value);

        /**
        Decodes a plaintext polynomial and returns the result as a double-precision
        floating-point number.

        @param[in] poly The polynomial to be decoded
        @throws std::invalid_argument if poly is not a valid plaintext polynomial
        @throws std::invalid_argument if either the integral part does not fit in std::int64_t
        */
        double decode(const BigPoly &poly);

        /**
        Returns a reference to the plaintext modulus.
        */
        const BigUInt &plain_modulus() const
        {
            return encoder_.plain_modulus();
        }

        /**
        Returns the base used for encoding.
        */
        uint64_t base() const
        {
            return encoder_.base();
        }

    private:
        BalancedFractionalEncoder(const BalancedFractionalEncoder &copy) = delete;

        BalancedFractionalEncoder &operator =(const BalancedFractionalEncoder &assign) = delete;

        BalancedEncoder encoder_;

        util::MemoryPool pool_;

        int fraction_coeff_count_;

        int integer_coeff_count_;

        BigPoly poly_modulus_;
    };
}

#endif // SEAL_ENCODER_H
