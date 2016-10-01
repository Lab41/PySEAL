#pragma once

#include "bigpoly.h"

namespace seal
{
    /**
    Provides arithmetic and modular arithmetic functions for operating on polynomials (represented by BigPoly's).

    @warning All functions in BigPolyArith are not thread safe and a separate BigPolyArith instance is needed for each
    potentially concurrent invocation.

    @see BigPoly for more details on polynomials.
    */
    class BigPolyArith
    {
    public:
        /**
        Negates the coefficients of a specified polynomial modulo the specified coefficient modulus, and stores the result
        in result. The input poly parameter is not modified. All coefficients in poly must be less-than coeff_mod. The
        result polynomial is resized if it has fewer coefficients than poly's coefficient count or coeff_mod's bit count.

        @param[in] poly The polynomial to negate
        @param[in] coeff_mod The coefficient modulus
        @param[out] result The polynomial to store the negated result
        @throws std::invalid_argument If coeff_mod is zero
        @throws std::invalid_argument If poly has any coefficients greater than or equal to coeff_mod
        @throws std::logic_error If result is an alias but needs to be resized to fit the result
        @warning all functions in BigPolyArith are not thread-safe
        */
        void negate(const BigPoly &poly, const BigUInt &coeff_mod, BigPoly &result);

        /**
        Negates the coefficients of a specified polynomial modulo the specified coefficient modulus, and returns the
        result. The input poly parameter is not modified. All coefficients in poly must be less-than coeff_mod. The
        returned polynomial is sized to have poly's coefficient count and coeff_mod's bit count.

        @param[in] poly The polynomial to negate
        @param[in] coeff_mod The coefficient modulus
        @throws std::invalid_argument If coeff_mod is zero
        @throws std::invalid_argument If poly has any coefficients greater than or equal to coeff_mod
        @warning all functions in BigPolyArith are not thread-safe
        */
        BigPoly negate(const BigPoly &poly, const BigUInt &coeff_mod)
        {
            BigPoly result;
            negate(poly, coeff_mod, result);
            return result;
        }

        /**
        Negates the coefficients of a specified polynomial and stores the result in result. The input poly parameter is not
        modified. The result polynomial is resized if it is smaller than poly's coefficient count or coefficient bit count.

        @param[in] poly The polynomial to negate
        @param[out] result The polynomial to store the negated result
        @throws std::logic_error If result is an alias but needs to be resized to fit the result
        @warning all functions in BigPolyArith are not thread-safe
        */
        void negate(const BigPoly &poly, BigPoly &result);

        /**
        Negates the coefficients of a specified polynomial and returns the result. The input poly parameter is not modified.
        The returned polynomial is sized identical to poly.

        @param[in] poly The polynomial to negate
        @warning all functions in BigPolyArith are not thread-safe
        */
        BigPoly negate(const BigPoly &poly)
        {
            BigPoly result;
            negate(poly, result);
            return result;
        }

        /**
        Adds the poly1 and poly2 polynomials modulo the specified coefficient modulus, and stores the result in result.
        The input poly1 and poly2 polynomials are not modified. All coefficients in both polynomials must be less-than
        coeff_mod. The result polynomial is resized if it has fewer coefficients than the larger of poly1 or poly2's
        coefficient counts, or if its coefficient bit count is smaller than coeff_mod's bit count.

        @param[in] poly1 The first polynomial to add
        @param[in] poly2 The second polynomial to add
        @param[in] coeff_mod The coefficient modulus
        @param[out] result The polynomial to store the addition result
        @throws std::invalid_argument If coeff_mod is zero
        @throws std::invalid_argument If poly1 or poly2 has any coefficients greater than or equal to coeff_mod
        @throws std::logic_error If result is an alias but needs to be resized to fit the result
        @warning all functions in BigPolyArith are not thread-safe
        */
        void add(const BigPoly &poly1, const BigPoly &poly2, const BigUInt &coeff_mod, BigPoly &result);

        /**
        Adds the poly1 and poly2 polynomials modulo the specified coefficient modulus, and returns the result. The
        input poly1 and poly2 polynomials are not modified. All coefficients in both polynomials must be less-than
        coeff_mod. The returned polynomial is sized to have a coefficient count equal to the larger of poly1 or
        poly2's coefficient counts, and a coefficient bit count equal to coeff_mod's bit count.

        @param[in] poly1 The first polynomial to add
        @param[in] poly2 The second polynomial to add
        @param[in] coeff_mod The coefficient modulus
        @throws std::invalid_argument If coeff_mod is zero
        @throws std::invalid_argument If poly1 or poly2 has any coefficients greater than or equal to coeff_mod
        @warning all functions in BigPolyArith are not thread-safe
        */
        BigPoly add(const BigPoly &poly1, const BigPoly &poly2, const BigUInt &coeff_mod)
        {
            BigPoly result;
            add(poly1, poly2, coeff_mod, result);
            return result;
        }

        /**
        Subtracts the poly1 and poly2 polynomials modulo the specified coefficient modulus, and stores the result in
        result. The input poly1 and poly2 polynomials are not modified. All coefficients in both polynomials must be
        less-than coeff_mod. The result polynomial is resized if it has fewer coefficients than the larger of poly1
        or poly2's coefficient counts, or if it's coefficient bit count is smaller than coeff_mod's bit count.

        @param[in] poly1 The first polynomial to subtract
        @param[in] poly2 The second polynomial to subtract
        @param[in] coeff_mod The coefficient modulus
        @param[out] result The polynomial to store the subtraction result
        @throws std::invalid_argument If coeff_mod is zero
        @throws std::invalid_argument If poly1 or poly2 has any coefficients greater than or equal to coeff_mod
        @throws std::logic_error If result is an alias but needs to be resized to fit the result
        @warning all functions in BigPolyArith are not thread-safe
        */
        void sub(const BigPoly &poly1, const BigPoly &poly2, const BigUInt &coeff_mod, BigPoly &result);

        /**
        Subtracts the poly1 and poly2 polynomials modulo the specified coefficient modulus, and returns the result.
        The input poly1 and poly2 polynomials are not modified. All coefficients in both polynomials must be
        less-than coeff_mod. The returned polynomial is sized to have a coefficient count equal to the larger of
        poly1 or poly2's coefficient counts, and a coefficient bit count equal to coeff_mod's bit count.

        @param[in] poly1 The first polynomial to subtract
        @param[in] poly2 The second polynomial to subtract
        @param[in] coeff_mod The coefficient modulus
        @throws std::invalid_argument If coeff_mod is zero
        @throws std::invalid_argument If poly1 or poly2 has any coefficients greater than or equal to coeff_mod
        @warning all functions in BigPolyArith are not thread-safe
        */
        BigPoly sub(const BigPoly &poly1, const BigPoly &poly2, const BigUInt &coeff_mod)
        {
            BigPoly result;
            sub(poly1, poly2, coeff_mod, result);
            return result;
        }

        /**
        Adds the poly1 and poly2 polynomials, and stores the result in result. The input poly1 and poly2 polynomials
        are not modified. The result polynomial is resized if it has fewer coefficients than the larger of poly1 or
        poly2's coefficient counts, or if it's coefficient bit count is smaller than one greater than the larger of
        poly1 and poly2's significant coefficient bit count.

        @param[in] poly1 The first polynomial to add
        @param[in] poly2 The second polynomial to add
        @param[out] result The polynomial to store the addition result
        @throws std::logic_error If result is an alias but needs to be resized to fit the result
        @warning all functions in BigPolyArith are not thread-safe
        */
        void add(const BigPoly &poly1, const BigPoly &poly2, BigPoly &result);

        /**
        Adds the poly1 and poly2 polynomials, and returns the result. The input poly1 and poly2 polynomials are not
        modified. The result polynomial is sized to have a coefficient count equal to the larger of poly1 or poly2's
        coefficient counts, and a coefficient bit count equal to one greater than the larger of poly1 and poly2's
        significant coefficient bit count.

        @param[in] poly1 The first polynomial to add
        @param[in] poly2 The second polynomial to add
        @warning all functions in BigPolyArith are not thread-safe
        */
        BigPoly add(const BigPoly &poly1, const BigPoly &poly2)
        {
            BigPoly result;
            add(poly1, poly2, result);
            return result;
        }

        /**
        Subtracts the poly1 and poly2 polynomials, and stores the result in result. The input poly1 and poly2 polynomials
        are not modified. The result polynomial is resized if it has fewer coefficients than the larger of poly1 or
        poly2's coefficient counts, or if it's coefficient bit count is smaller than the larger of poly1 and poly2's
        coefficient bit count.

        @param[in] poly1 The first polynomial to subtract
        @param[in] poly2 The second polynomial to subtract
        @param[out] result The polynomial to store the subtraction result
        @throws std::logic_error If result is an alias but needs to be resized to fit the result
        @warning all functions in BigPolyArith are not thread-safe
        */
        void sub(const BigPoly &poly1, const BigPoly &poly2, BigPoly &result);

        /**
        Subtracts the poly1 and poly2 polynomials, and returns the result. The input poly1 and poly2 polynomials are not
        modified. The result polynomial is sized to have a coefficient count equal to the larger of poly1 or poly2's
        coefficient counts, and a coefficient bit count equal to the larger of poly1 and poly2's coefficient bit count.

        @param[in] poly1 The first polynomial to subtract
        @param[in] poly2 The second polynomial to subtract
        @warning all functions in BigPolyArith are not thread-safe
        */
        BigPoly sub(const BigPoly &poly1, const BigPoly &poly2)
        {
            BigPoly result;
            sub(poly1, poly2, result);
            return result;
        }

        /**
        Multiplies the poly1 and poly2 polynomials modulo the specified polynomial and coefficient moduli, and stores the
        result in result. The input poly1 and poly2 polynomials are not modified. Both polynomials must have fewer
        significant coefficients than poly_mod and have coefficients less-than coeff_mod. The result polynomial is resized
        if it has fewer coefficients than poly_mod's significant coefficient count or if it's coefficient bit count is
        smaller than coeff_mod's bit count.

        @param[in] poly1 The first polynomial to multiply
        @param[in] poly2 The second polynomial to multiply
        @param[in] poly_mod The polynomial modulus
        @param[in] coeff_mod The coefficient modulus
        @param[out] result The polynomial to store the multiplication result
        @throws std::invalid_argument If poly_mod is zero
        @throws std::invalid_argument If coeff_mod is zero
        @throws std::invalid_argument If poly1 or poly2 has greater-than or equal significant coefficient count to poly_mod
        or has any coefficients greater than or equal to coeff_mod
        @throws std::invalid_argument If poly_mod has any coefficients greater than or equal to coeff_mod
        @throws std::logic_error If result is an alias but needs to be resized to fit the result
        @warning all functions in BigPolyArith are not thread-safe
        */
        void multiply(const BigPoly &poly1, const BigPoly &poly2, const BigPoly &poly_mod, const BigUInt &coeff_mod, BigPoly &result);

        /**
        Multiplies the poly1 and poly2 polynomials modulo the specified polynomial and coefficient moduli, and returns the
        result. The input poly1 and poly2 polynomials are not modified. Both polynomials must have fewer significant
        coefficients than poly_mod and have coefficients less-than coeff_mod. The returned polynomial is sized to have a
        coefficient count equal to poly_mod's significant coefficient count and a coefficient bit count equal to
        coeff_mod's bit count.

        @param[in] poly1 The first polynomial to multiply
        @param[in] poly2 The second polynomial to multiply
        @param[in] poly_mod The polynomial modulus
        @param[in] coeff_mod The coefficient modulus
        @throws std::invalid_argument If poly_mod is zero
        @throws std::invalid_argument If coeff_mod is zero
        @throws std::invalid_argument If poly1 or poly2 has greater-than or equal significant coefficient count to poly_mod
        or has any coefficients greater than or equal to coeff_mod
        @throws std::invalid_argument If poly_mod has any coefficients greater than or equal to coeff_mod
        @warning all functions in BigPolyArith are not thread-safe
        */
        BigPoly multiply(const BigPoly &poly1, const BigPoly &poly2, const BigPoly &poly_mod, const BigUInt &coeff_mod)
        {
            BigPoly result;
            multiply(poly1, poly2, poly_mod, coeff_mod, result);
            return result;
        }

        /**
        Multiplies the poly1 and poly2 polynomials modulo the specified coefficient modulus, and stores the result in result.
        The input poly1 and poly2 polynomials are not modified. All coefficients in both polynomials must be less-than
        coeff_mod. The result polynomial is resized if it has fewer coefficients than one less than the sum of poly1 and
        poly2's significant coefficient counts, or if it's coefficient bit count is smaller than coeff_mod's bit count.

        @param[in] poly1 The first polynomial to multiply
        @param[in] poly2 The second polynomial to multiply
        @param[in] coeff_mod The coefficient modulus
        @param[out] result The polynomial to store the multiplication result
        @throws std::invalid_argument If coeff_mod is zero
        @throws std::invalid_argument If poly1 or poly2 has any coefficients greater than or equal to coeff_mod
        @throws std::logic_error If result is an alias but needs to be resized to fit the result
        @warning all functions in BigPolyArith are not thread-safe
        */
        void multiply(const BigPoly &poly1, const BigPoly &poly2, const BigUInt &coeff_mod, BigPoly &result);

        /**
        Multiplies the poly1 and poly2 polynomials modulo the specified coefficient modulus, and returns the result. The
        input poly1 and poly2 polynomials are not modified. All coefficients in both polynomials must be less-than coeff_mod.
        The returned polynomial is sized to have a coefficient count equal to one less than the sum of poly1 and poly2's
        significant coefficient counts, and a coefficient bit count equal to coeff_mod's bit count.

        @param[in] poly1 The first polynomial to multiply
        @param[in] poly2 The second polynomial to multiply
        @param[in] coeff_mod The coefficient modulus
        @throws std::invalid_argument If coeff_mod is zero
        @throws std::invalid_argument If poly1 or poly2 has any coefficients greater than or equal to coeff_mod
        @warning all functions in BigPolyArith are not thread-safe
        */
        BigPoly multiply(const BigPoly &poly1, const BigPoly &poly2, const BigUInt &coeff_mod)
        {
            BigPoly result;
            multiply(poly1, poly2, coeff_mod, result);
            return result;
        }

        /**
        Multiplies the poly1 and poly2 polynomials, and stores the result in result. The input poly1 and poly2 polynomials are
        not modified. The result polynomial is resized if it has fewer coefficients than one less than the sum of poly1 and
        poly2's significant coefficient counts, or if it's coefficient bit count is smaller than the sum of poly1 and poly2's
        significant coefficient bit count added with the significant bit count of the smaller of poly1 and poly2's significant
        coefficient count (i.e., poly1.significant_coeff_bit_count() + poly2.significant_coeff_bit_count() +
        get_significant_bit_count(min(poly1.significant_coeff_count(), poly2.significant_coeff_count()))).

        @param[in] poly1 The first polynomial to multiply
        @param[in] poly2 The second polynomial to multiply
        @param[out] result The polynomial to store the multiplication result
        @throws std::logic_error If result is an alias but needs to be resized to fit the result
        @warning all functions in BigPolyArith are not thread-safe
        */
        void multiply(const BigPoly &poly1, const BigPoly &poly2, BigPoly &result);

        /**
        Multiplies the poly1 and poly2 polynomials, and returns the result. The input poly1 and poly2 polynomials are not
        modified. The returned polynomial is sized to have a coefficient count equal to one less than the sum of poly1 and
        poly2's significant coefficient counts, and a coefficient bit count equal to the sum of poly1 and poly2's significant
        coefficient bit count added with the significant bit count of the smaller of poly1 and poly2's significant coefficient
        count (i.e., poly1.significant_coeff_bit_count() + poly2.significant_coeff_bit_count() +
        get_significant_bit_count(min(poly1.significant_coeff_count(), poly2.significant_coeff_count()))).

        @param[in] poly1 The first polynomial to multiply
        @param[in] poly2 The second polynomial to multiply
        @warning all functions in BigPolyArith are not thread-safe
        */
        BigPoly multiply(const BigPoly &poly1, const BigPoly &poly2)
        {
            BigPoly result;
            multiply(poly1, poly2, result);
            return result;
        }

        /**
        Multiplies a polynomial by an integer modulo the specified coefficient modulus, and stores the result in result.
        The inputs poly1 and uint2 are not modified. All coefficients in poly1 and uint2 must be less-than coeff_mod.
        The result polynomial is resized if it has fewer coefficients than poly1's coefficient count, or if its
        coefficient bit count is smaller than coeff_mod's bit count.

        @param[in] poly1 The first polynomial to multiply
        @param[in] uint2 The integer to multiply
        @param[in] coeff_mod The coefficient modulus
        @param[out] result The polynomial to store the multiplication result
        @throws std::invalid_argument If coeff_mod is zero
        @throws std::invalid_argument If poly1 has any coefficients greater than or equal to coeff_mod
        @throws std::invalid_argument If uint2 is greater than or equal to coeff_mod
        @throws std::logic_error If result is an alias but needs to be resized to fit the result
        @warning all functions in BigPolyArith are not thread-safe
        */
        void multiply(const BigPoly &poly1, const BigUInt &uint2, const BigUInt &coeff_mod, BigPoly &result);

        /**
        Multiplies a polynomial by an integer modulo the specified coefficient modulus, and returns the result. The inputs
        poly1 and uint2 are not modified. All coefficients in poly1 and uint2 must be less-than coeff_mod. The returned
        polynomial is sized to have a coefficient count equal to poly1's coefficient count, and a coefficient bit count
        equal to coeff_mod's bit count.

        @param[in] poly1 The polynomial to multiply
        @param[in] uint2 The integer to multiply
        @param[in] coeff_mod The coefficient modulus
        @throws std::invalid_argument If coeff_mod is zero
        @throws std::invalid_argument If poly1 has any coefficients greater than or equal to coeff_mod
        @throws std::invalid_argument If uint2 is greater than or equal to coeff_mod
        @warning all functions in BigPolyArith are not thread-safe
        */
        BigPoly multiply(const BigPoly &poly1, const BigUInt &uint2, const BigUInt &coeff_mod)
        {
            BigPoly result;
            multiply(poly1, uint2, coeff_mod, result);
            return result;
        }

        /**
        Multiplies a polynomial by an integer, and stores the result in result. The inputs poly1 and uint2 are not modified.
        The result polynomial is resized if it has fewer coefficients than poly1's coefficient count, or if its coefficient bit
        count is smaller than the sum of poly1's significant coefficient bit count and uint2's significant bit count.

        @param[in] poly1 The polynomial to multiply
        @param[in] uint2 The integer to multiply
        @param[out] result The polynomial to store the multiplication result
        @throws std::logic_error If result is an alias but needs to be resized to fit the result
        @warning all functions in BigPolyArith are not thread-safe
        */
        void multiply(const BigPoly &poly1, const BigUInt &uint2, BigPoly &result);

        /**
        Multiplies a polynomial by an integer, and returns the result. The inputs poly1 and uint2 are not modified. The returned
        polynomial is sized to have a coefficient count equal to poly1's coefficient count, and a coefficient bit count equal to
        the sum of poly1's significant coefficient bit count and uint2's significant bit count.

        @param[in] poly1 The polynomial to multiply
        @param[in] uint2 The integer to multiply
        @warning all functions in BigPolyArith are not thread-safe
        */
        BigPoly multiply(const BigPoly &poly1, const BigUInt &uint2)
        {
            BigPoly result;
            multiply(poly1, uint2, result);
            return result;
        }

        /**
        Divides the numerator and denominator polynomials modulo the specified coefficient modulus, and stores the quotient
        and remainder in provided parameters. The input numerator and denominator polynomials are not modified. All
        coefficients in numerator and denominator must be less-than coeff_mod. The quotient and remainder polynomials are
        resized if they have fewer coefficients than the greater of numerator and denominator's coefficient counts, or if
        their coefficient bit counts are smaller than coeff_mod's bit count.

        @param[in] numerator The numerator to divide
        @param[in] denominator The denominator to divide
        @param[in] coeff_mod The coefficient modulus
        @param[out] quotient The polynomial to store the quotient of the division result
        @param[out] remainder The polynomial to store the remainder of the division result
        @throws std::invalid_argument If denominator is zero
        @throws std::invalid_argument If coeff_mod is zero
        @throws std::invalid_argument If numerator or denominator has any coefficients greater than or equal to coeff_mod
        @throws std::invalid_argument If quotient and remainder point to the same backing array
        @throws std::logic_error If quotient or remainder is an alias but needs to be resized to fit the result
        @warning all functions in BigPolyArith are not thread-safe
        */
        void divide(const BigPoly &numerator, const BigPoly &denominator, const BigUInt &coeff_mod, BigPoly &quotient, BigPoly &remainder);

        /**
        Divides the numerator and denominator polynomials modulo the specified coefficient modulus, and stores the remainder
        in remainder. The input numerator and denominator polynomials are not modified. All coefficients in numerator and
        denominator must be less-than coeff_mod. The remainder polynomial is resized if it has fewer coefficients than the
        larger of numerator and denominator's coefficient counts, or if it's coefficient bit count is smaller than
        coeff_mod's bit count.

        @param[in] numerator The numerator to divide
        @param[in] denominator The denominator to divide
        @param[in] coeff_mod The coefficient modulus
        @param[out] remainder The polynomial to store the remainder of the division result
        @throws std::invalid_argument If denominator is zero
        @throws std::invalid_argument If coeff_mod is zero
        @throws std::invalid_argument If numerator or denominator has any coefficients greater than or equal to coeff_mod
        @throws std::logic_error If remainder is an alias but needs to be resized to fit the result
        @warning all functions in BigPolyArith are not thread-safe
        */
        void modulo(const BigPoly &numerator, const BigPoly &denominator, const BigUInt &coeff_mod, BigPoly &remainder);

        /**
        Divides the numerator and denominator polynomials modulo the specified coefficient modulus, and returns the
        remainder. The input numerator and denominator polynomials are not modified. All coefficients in numerator and
        denominator must be less-than coeff_mod. The returned polynomial is sized to have a coefficient count equal to
        the larger of numerator and denominator's coefficient counts, and a coefficient bit count equal to coeff_mod's
        bit count.

        @param[in] numerator The numerator to divide
        @param[in] denominator The denominator to divide
        @param[in] coeff_mod The coefficient modulus
        @throws std::invalid_argument If denominator is zero
        @throws std::invalid_argument If coeff_mod is zero
        @throws std::invalid_argument If numerator or denominator has any coefficients greater than or equal to coeff_mod
        @warning all functions in BigPolyArith are not thread-safe
        */
        BigPoly modulo(const BigPoly &numerator, const BigPoly &denominator, const BigUInt &coeff_mod)
        {
            BigPoly result;
            modulo(numerator, denominator, coeff_mod, result);
            return result;
        }

        /**
        Attempts to invert a polynomial with the specified coefficient and polynomial moduli, returning whether or not
        the inverse was successful and setting the result parameter to the inverse. The poly polynomials must have fewer
        significant coefficients than poly_mod and have coefficients less-than coeff_mod. The result polynomial is resized
        if it has fewer coefficients than poly_mod's significant coefficient count or if it's coefficient bit count is
        smaller than coeff_mod's bit count.

        @param[in] poly The polynomial to invert
        @param[in] poly_mod The polynomial modulus
        @param[in] coeff_mod The coefficient modulus
        @param[out] result The polynomial to store the inverse result
        @throws std::invalid_argument If poly_mod is zero
        @throws std::invalid_argument If coeff_mod is zero
        @throws std::invalid_argument If poly has greater-than or equal significant coefficient count to poly_mod or has any
        coefficients greater than or equal to coeff_mod
        @throws std::invalid_argument If poly_mod has any coefficients greater than or equal to coeff_mod
        @throws std::logic_error If result is an alias but needs to be resized to fit the result
        @warning all functions in BigPolyArith are not thread-safe
        */
        bool try_invert(const BigPoly &poly, const BigPoly &poly_mod, const BigUInt &coeff_mod, BigPoly &result);
    };
}