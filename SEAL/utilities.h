#pragma once

#include "bigpoly.h"
#include "biguint.h"
#include "encryptionparams.h"
#include "bigpolyarray.h"

namespace seal
{
     /**
    Computes the infinity-norm of a given polynomial with non-negative coefficients (represented by BigPoly).

    @param[in] poly The polynomial whose infinity-norm is to be computed
    */
    BigUInt poly_infty_norm(const BigPoly &poly);

    /**
    Computes the infinity-norm of a given polynomial (represented by BigPoly). The coefficients of
    the polynomial are interpreted as integers modulo a given modulus. For each coefficient in the given
    polynomial, this function first finds a representative in the symmetric interval around zero,
    then computes its absolute value, and finally outputs the largest of them all.

    @param[in] poly The polynomial whose infinity-norm is to be computed
    @param[in] modulus The modulus used in the computation
    @throws std::invalid_argument if modulus is zero
    */
    BigUInt poly_infty_norm_coeffmod(const BigPoly &poly, const BigUInt &modulus);

    /**
    Raises an unsigned integer (represented by BigUInt) to an unsigned integer power
    (represented by BigUInt) modulo a given modulus and stores the result in a given BigUInt.
    The input is expected to be already reduced modulo the modulus.

    @param[in] operand The unsigned integer to exponentiate
    @param[in] exponent The unsigned integer exponent
    @param[in] modulus The modulus
    @param[out] destination The unsigned integer to overwrite with the result
    @throws std::invalid_argument if operand is not reduced modulo modulus
    @throws std::invalid_argument if both operand and exponent are zero
    */
    void exponentiate_uint_mod(const BigUInt &operand, const BigUInt &exponent, const BigUInt &modulus, BigUInt &destination);

    /**
    Raises an unsigned integer (represented by BigUInt) to an unsigned integer power
    (represented by BigUInt) modulo a given modulus and returns the result.
    The input is expected to be already reduced modulo the modulus.

    @param[in] operand The unsigned integer to exponentiate
    @param[in] exponent The unsigned integer exponent
    @param[in] modulus The modulus
    @throws std::invalid_argument if operand is not reduced modulo modulus
    @throws std::invalid_argument if both operand and exponent are zero
    */
    BigUInt exponentiate_uint_mod(const BigUInt &operand, const BigUInt &exponent, const BigUInt &modulus);

    /**
    Raises a polynomial (represented by BigPoly) to an unsigned integer power (represented by BigUInt) modulo 
    a polynomial and a coefficient modulus, and stores the result in a given BigPoly. The input is expected 
    to be already reduced both modulo the polynomial and the coefficient modulus.

    @param[in] operand The polynomial to exponentiate
    @param[in] exponent The unsigned integer exponent
    @param[in] poly_modulus The polynomial modulus
    @param[in] coeff_modulus The coefficient modulus
    @param[out] destination The polynomial to overwrite with the result
    @throws std::invalid_argument if operand is not reduced modulo poly_modulus and coeff_modulus
    @throws std::invalid_argument if exponent is negative
    @throws std::invalid_argument if both operand and exponent are zero
    */
    void exponentiate_poly_polymod_coeffmod(const BigPoly &operand, const BigUInt &exponent, 
        const BigPoly &poly_modulus, const BigUInt &coeff_modulus, BigPoly &destination);

    /**
    Raises a polynomial (represented by BigPoly) to an unsigned integer power (represented by BigUInt) 
    modulo a polynomial and a coefficient modulus, and returns the result. The input is expected to be 
    already reduced both modulo the polynomial and the coefficient modulus.

    @param[in] operand The polynomial to exponentiate
    @param[in] exponent The unsigned integer exponent
    @param[in] poly_modulus The polynomial modulus
    @param[in] coeff_modulus The coefficient modulus
    @throws std::invalid_argument if operand is not reduced modulo poly_modulus and coeff_modulus
    @throws std::invalid_argument if exponent is negative
    @throws std::invalid_argument if both operand and exponent are zero
    */
    BigPoly exponentiate_poly_polymod_coeffmod(const BigPoly &operand, const BigUInt &exponent,
        const BigPoly &poly_modulus, const BigUInt &coeff_modulus);

    /**
    Evaluates a given polynomial at another given polynomial and stores the result in a given BigPoly.

    @param[in] poly_to_evaluate The polynomial to be evaluated
    @param[in] poly_to_evaluate_at The polynomial poly_to_evaluate will be evaluated by replacing its variable with this polynomial
    @param[out] destination The polynomial to overwrite with the result
    */
    void poly_eval_poly(const BigPoly &poly_to_evaluate, const BigPoly &poly_to_evaluate_at, BigPoly &destination);

    /**
    Evaluates a given polynomial at another given polynomial and returns the result.

    @param[in] poly_to_evaluate The polynomial to be evaluated
    @param[in] poly_to_evaluate_at The polynomial poly_to_evaluate will be evaluated by replacing its variable with this polynomial
    */
    BigPoly poly_eval_poly(const BigPoly &poly_to_evaluate, const BigPoly &poly_to_evaluate_at);

    /**
    Evaluates a given polynomial at another given polynomial modulo a polynomial and a coefficient modulus
    and stores the result in a given BigPoly. The inputs are expected to be already reduced both modulo 
    the polynomial and the coefficient modulus.

    @param[in] poly_to_evaluate The polynomial to be evaluated
    @param[in] poly_to_evaluate_at The polynomial poly_to_evaluate will be evaluated by replacing its variable with this polynomial
    @param[in] poly_modulus The polynomial modulus
    @param[in] coeff_modulus The coefficient modulus
    @param[out] destination The polynomial to overwrite with the result
    @throws std::invalid_argument if poly_to_evaluate is not reduced modulo poly_modulus and coeff_modulus
    @throws std::invalid_argument if poly_to_evaluate_at is not reduced modulo poly_modulus and coeff_modulus
    */
    void poly_eval_poly_polymod_coeffmod(const BigPoly &poly_to_evaluate, const BigPoly &poly_to_evaluate_at, 
        const BigPoly &poly_modulus, const BigUInt &coeff_modulus, BigPoly &destination);

    /**
    Evaluates a given polynomial at another given polynomial modulo a polynomial and a coefficient modulus
    and returns the result. The inputs are expected to be already reduced both modulo the polynomial and the coefficient modulus.

    @param[in] poly_to_evaluate The polynomial to be evaluated
    @param[in] poly_to_evaluate_at The polynomial poly_to_evaluate will be evaluated by replacing its variable with this polynomial
    @param[in] poly_modulus The polynomial modulus
    @param[in] coeff_modulus The coefficient modulus
    @throws std::invalid_argument if poly_to_evaluate is not reduced modulo poly_modulus and coeff_modulus
    @throws std::invalid_argument if poly_to_evaluate_at is not reduced modulo poly_modulus and coeff_modulus
    */
    BigPoly poly_eval_poly_polymod_coeffmod(const BigPoly &poly_to_evaluate, const BigPoly &poly_to_evaluate_at, 
        const BigPoly &poly_modulus, const BigUInt &coeff_modulus);

    /**
    Evaluates a given polynomial at an unsigned integer modulo a given modulus and stores the result in a given BigUInt.
    The coefficients of poly_to_evaluate and the value to evaluate at are expected to be already reduced modulo the modulus.

    @param[in] poly_to_evaluate The polynomial to be evaluated
    @param[in] value The unsigned integer at which the polynomial is evaluated
    @param[in] modulus The modulus
    @param[out] destination The unsigned integer to overwrite with the result
    @throws std::invalid_argument if the coefficients of poly_to_evaluate are not reduced modulo modulus
    @throws std::invalid_argument if value is not reduced modulo modulus
    */
    void poly_eval_uint_mod(const BigPoly &poly_to_evaluate, const BigUInt &value, const BigUInt &modulus, BigUInt &destination);

    /**
    Evaluates a given polynomial at an unsigned integer modulo a given modulus and returns the result.
    The coefficients of poly_to_evaluate and the value to evaluate at are expected to be already reduced modulo the modulus.

    @param[in] poly_to_evaluate The polynomial to be evaluated
    @param[in] value The unsigned integer at which the polynomial is evaluated
    @param[in] modulus The modulus
    @throws std::invalid_argument if the coefficients of poly_to_evaluate are not reduced modulo modulus
    @throws std::invalid_argument if value is not reduced modulo modulus
    */
    BigUInt poly_eval_uint_mod(const BigPoly &poly_to_evaluate, const BigUInt &value, const BigUInt &modulus);
}