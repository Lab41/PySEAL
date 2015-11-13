#ifndef SEAL_UTILITIES_H
#define SEAL_UTILITIES_H

#include "bigpoly.h"
#include "biguint.h"
#include "encryptionparams.h"

namespace seal
{
    BigUInt inherent_noise(const BigPoly &encrypted, const BigPoly &plain, const EncryptionParameters &parms, const BigPoly &secret_key);

    BigUInt inherent_noise(const BigPoly &encrypted, const EncryptionParameters &parms, const BigPoly &secret_key);

    void inherent_noise(const BigPoly &encrypted, const BigPoly &plain, const EncryptionParameters &parms, const BigPoly &secret_key, BigUInt &result);

    BigUInt inherent_noise_max(const EncryptionParameters &parms);

    BigUInt poly_infty_norm(const BigPoly &poly);

    BigUInt poly_infty_norm_coeffmod(const BigPoly &poly, const BigUInt &modulus);

    int estimate_level_max(const EncryptionParameters &parms);

    void exponentiate_uint(const BigUInt &operand, int exponent, BigUInt &result);

    inline BigUInt exponentiate_uint(const BigUInt &operand, int exponent)
    {
        BigUInt result;
        exponentiate_uint(operand, exponent, result);
        return result;
    }

    void exponentiate_poly(const BigPoly &operand, int exponent, BigPoly &result);

    inline BigPoly exponentiate_poly(const BigPoly &operand, int exponent)
    {
        BigPoly result;
        exponentiate_poly(operand, exponent, result);
        return result;
    }

    BigPoly poly_eval_poly(const BigPoly &poly_to_eval, const BigPoly &value);
}


#endif // SEAL_UTILITIES_H
