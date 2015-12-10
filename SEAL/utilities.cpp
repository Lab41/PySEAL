#include "utilities.h"
#include "util/uintarith.h"
#include "util/uintarithmod.h"
#include "util/polyarithmod.h"
#include "util/polyextras.h"
#include "util/polycore.h"
#include "util/uintextras.h"
#include "decryptor.h"
#include "simulator.h"

using namespace std;
using namespace seal::util;

namespace seal
{
    namespace
    {
        ConstPointer duplicate_uint_if_needed(const BigUInt &uint, int new_uint64_count, bool force, MemoryPool &pool)
        {
            return duplicate_uint_if_needed(uint.pointer(), uint.uint64_count(), new_uint64_count, force, pool);
        }

        ConstPointer duplicate_poly_if_needed(const BigPoly &poly, int new_coeff_count, int new_coeff_uint64_count, bool force, MemoryPool &pool)
        {
            return duplicate_poly_if_needed(poly.pointer(), poly.coeff_count(), poly.coeff_uint64_count(), new_coeff_count, new_coeff_uint64_count, force, pool);
        }

        bool are_poly_coefficients_less_than(const BigPoly &poly, const BigUInt &max_coeff)
        {
            return util::are_poly_coefficients_less_than(poly.pointer(), poly.coeff_count(), poly.coeff_uint64_count(), max_coeff.pointer(), max_coeff.uint64_count());
        }
    }

    BigUInt poly_infty_norm(const BigPoly &poly)
    {
        if (poly.is_zero())
        {
            return BigUInt();
        }

        int coeff_count = poly.coeff_count();
        int coeff_bit_count = poly.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        BigUInt result(coeff_bit_count);
        util::poly_infty_norm(poly.pointer(), coeff_count, coeff_uint64_count, result.pointer());

        return result;
    }

    BigUInt poly_infty_norm_coeffmod(const BigPoly &poly, const BigUInt &modulus)
    {
        if (modulus.is_zero())
        {
            throw invalid_argument("modulus cannot be zero");
        }
        if (poly.is_zero())
        {
            return BigUInt();
        }

        int poly_coeff_count = poly.coeff_count();
        int poly_coeff_bit_count = poly.coeff_bit_count();
        int poly_coeff_uint64_count = divide_round_up(poly_coeff_bit_count, bits_per_uint64);

        MemoryPool pool;
        Modulus mod(modulus.pointer(), modulus.uint64_count(), pool);
        BigUInt result(modulus.significant_bit_count());
        util::poly_infty_norm_coeffmod(poly.pointer(), poly_coeff_count, poly_coeff_uint64_count, mod, result.pointer(), pool);

        return result;
    }

    BigUInt inherent_noise(const BigPoly &encrypted, const BigPoly &plain, const EncryptionParameters &parms, const BigPoly &secret_key)
    {
        int coeff_count = parms.poly_modulus().coeff_count();
        int coeff_bit_count = parms.coeff_modulus().significant_bit_count();
        BigUInt result(coeff_count, coeff_bit_count);
        inherent_noise(encrypted, plain, parms, secret_key, result);
        return result;
    }

    void inherent_noise(const BigPoly &encrypted, const BigPoly &plain, const EncryptionParameters &parms, const BigPoly &secret_key, BigUInt &result)
    {
        // Verify encryption parameters are non-zero and non-nullptr.
        if (parms.poly_modulus().is_zero())
        {
            throw invalid_argument("poly_modulus cannot be zero");
        }
        if (parms.coeff_modulus().is_zero())
        {
            throw invalid_argument("coeff_modulus cannot be zero");
        }
        if (parms.plain_modulus().is_zero())
        {
            throw invalid_argument("plain_modulus cannot be zero");
        }
        if (parms.noise_standard_deviation() < 0)
        {
            throw invalid_argument("noise_standard_deviation must be non-negative");
        }
        if (parms.noise_max_deviation() < 0)
        {
            throw invalid_argument("noise_max_deviation must be non-negative");
        }
        if (parms.plain_modulus() >= parms.coeff_modulus())
        {
            throw invalid_argument("plain_modulus must be smaller than coeff_modulus");
        }
        if (!are_poly_coefficients_less_than(parms.poly_modulus(), parms.coeff_modulus()))
        {
            throw invalid_argument("poly_modulus cannot have coefficients larger than coeff_modulus");
        }

        // Extract encryption parameters.
        const BigPoly &poly_modulus = parms.poly_modulus();
        const BigUInt &coeff_modulus = parms.coeff_modulus();
        const BigUInt &plain_modulus = parms.plain_modulus();
        int coeff_count = poly_modulus.coeff_count();
        int coeff_bit_count = coeff_modulus.significant_bit_count();
        int coeff_uint64_count = coeff_modulus.uint64_count();

        // Verify parameters.
        if (encrypted.coeff_count() != coeff_count || encrypted.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("encrypted is not a valid ciphertext");
        }
#ifdef _DEBUG
        if (encrypted.significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted, coeff_modulus))
        {
            throw invalid_argument("encrypted is not valid for encryption parameters");
        }
        if (plain.significant_coeff_count() >= coeff_count || !are_poly_coefficients_less_than(plain, plain_modulus))
        {
            throw invalid_argument("plain is too large to be represented by encryption parameters");
        }
#endif
        if (secret_key.is_zero())
        {
            throw invalid_argument("secret_key cannot be zero");
        }
        if (secret_key.coeff_count() != coeff_count || secret_key.coeff_bit_count() != coeff_bit_count ||
            secret_key.significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(secret_key, coeff_modulus))
        {
            throw invalid_argument("secret_key is not valid for encryption parameters");
        }

        // Resize plaintext modulus.
        MemoryPool pool;
        ConstPointer plain_modulus_ptr = duplicate_uint_if_needed(plain_modulus, coeff_uint64_count, false, pool);

        // Compute floor of coeff_modulus/plain_modulus.
        Pointer coeff_div_plain_modulus(allocate_uint(coeff_uint64_count, pool));
        Pointer remainder(allocate_uint(coeff_uint64_count, pool));
        divide_uint_uint(coeff_modulus.pointer(), plain_modulus_ptr.get(), coeff_uint64_count, coeff_div_plain_modulus.get(), remainder.get(), pool);

        // Resize plaintext to accomodate coeff_modulus size coefficients and have the full number of coefficients.
        ConstPointer plain_ptr = duplicate_poly_if_needed(plain, coeff_count, coeff_uint64_count, false, pool);

        // Resize poly_modulus to accomodate coeff_modulus size coefficients and have the full number of coefficients.
        ConstPointer poly_modulus_ptr = duplicate_poly_if_needed(poly_modulus, coeff_count, coeff_uint64_count, false, pool);

        BigPoly noise_poly(coeff_count, coeff_bit_count);
        PolyModulus polymod(poly_modulus_ptr.get(), coeff_count, coeff_uint64_count);
        Modulus mod(coeff_modulus.pointer(), coeff_uint64_count, pool);
        multiply_poly_poly_polymod_coeffmod(encrypted.pointer(), secret_key.pointer(), polymod, mod, noise_poly.pointer(), pool);

        Pointer temp(allocate_uint(coeff_uint64_count, pool));
        const uint64_t *plain_coeff_ptr = plain_ptr.get();
        uint64_t *noise_coeff_ptr = noise_poly.pointer();
        for (int coeff_index = 0; coeff_index < coeff_count; ++coeff_index)
        {
            multiply_uint_uint_mod(coeff_div_plain_modulus.get(), plain_coeff_ptr, mod, temp.get(), pool);
            sub_uint_uint_mod(noise_coeff_ptr, temp.get(), coeff_modulus.pointer(), coeff_uint64_count, noise_coeff_ptr);
            plain_coeff_ptr += coeff_uint64_count;
            noise_coeff_ptr += coeff_uint64_count;
        }

        result = poly_infty_norm_coeffmod(noise_poly, coeff_modulus);
    }

    BigUInt inherent_noise(const BigPoly &encrypted, const EncryptionParameters &parms, const BigPoly &secret_key)
    {
        Decryptor decryptor(parms, secret_key);
        BigPoly plain = decryptor.decrypt(encrypted);
        return inherent_noise(encrypted, plain, parms, secret_key);
    }

    BigUInt inherent_noise_max(const EncryptionParameters &parms)
    {
        // Verify encryption parameters are non-zero and non-nullptr.
        if (parms.poly_modulus().is_zero())
        {
            throw invalid_argument("poly_modulus cannot be zero");
        }
        if (parms.coeff_modulus().is_zero())
        {
            throw invalid_argument("coeff_modulus cannot be zero");
        }
        if (parms.plain_modulus().is_zero())
        {
            throw invalid_argument("plain_modulus cannot be zero");
        }
        if (parms.noise_standard_deviation() < 0)
        {
            throw invalid_argument("noise_standard_deviation must be non-negative");
        }
        if (parms.noise_max_deviation() < 0)
        {
            throw invalid_argument("noise_max_deviation must be non-negative");
        }
        if (parms.plain_modulus() >= parms.coeff_modulus())
        {
            throw invalid_argument("plain_modulus must be smaller than coeff_modulus");
        }
        if (!are_poly_coefficients_less_than(parms.poly_modulus(), parms.coeff_modulus()))
        {
            throw invalid_argument("poly_modulus cannot have coefficients larger than coeff_modulus");
        }

        // Extract encryption parameters
        const BigUInt &coeff_modulus = parms.coeff_modulus();
        const BigUInt &plain_modulus = parms.plain_modulus();
        int coeff_bit_count = coeff_modulus.significant_bit_count();
        int coeff_uint64_count = coeff_modulus.uint64_count();

        // Resize plaintext modulus.
        MemoryPool pool;
        ConstPointer plain_modulus_ptr = duplicate_uint_if_needed(plain_modulus, coeff_uint64_count, false, pool);

        BigUInt result(coeff_bit_count);

        // Compute floor of coeff_modulus/plain_modulus.
        Pointer coeff_div_plain_modulus(allocate_uint(coeff_uint64_count, pool));
        Pointer remainder(allocate_uint(coeff_uint64_count, pool));
        divide_uint_uint(coeff_modulus.pointer(), plain_modulus_ptr.get(), coeff_uint64_count, coeff_div_plain_modulus.get(), remainder.get(), pool);
        sub_uint_uint(coeff_div_plain_modulus.get(), remainder.get(), coeff_uint64_count, result.pointer());
        right_shift_uint(result.pointer(), 1, coeff_uint64_count, result.pointer());

        return result;
    }

    int estimate_level_max(const EncryptionParameters &parms)
    {
        SimulationEvaluator simulation_evaluator;
        Simulation sim_power(parms);
        int level = 0;
        while (true)
        {
            sim_power = simulation_evaluator.multiply(sim_power, sim_power);
            if (sim_power.decrypts())
            {
                ++level;
            }
            else
            {
                return level;
            }
        }
    }

    void exponentiate_uint(const BigUInt &operand, int exponent, BigUInt &result)
    {
        if (exponent < 0)
        {
            throw invalid_argument("exponent must be a non-negative integer");
        }
        if (operand.is_zero() && exponent == 0)
        {
            throw invalid_argument("undefined operation");
        }
        if (operand.is_zero())
        {
            result.set_zero();
            return;
        }

        int result_bit_count = exponent * operand.significant_bit_count() + 1;
        if (result.bit_count() < result_bit_count)
        {
            result.resize(result_bit_count);
        }

        MemoryPool pool;
        util::exponentiate_uint(operand.pointer(), operand.uint64_count(), exponent, result.uint64_count(), result.pointer(), pool);
    }

    BigUInt exponentiate_uint(const BigUInt &operand, int exponent)
    {
        BigUInt result;
        exponentiate_uint(operand, exponent, result);
        return result;
    }

    void exponentiate_poly(const BigPoly &operand, int exponent, BigPoly &result)
    {
        if (exponent < 0)
        {
            throw invalid_argument("exponent must be a non-negative integer");
        }
        if (operand.is_zero() && exponent == 0)
        {
            throw invalid_argument("undefined operation");
        }
        if (operand.is_zero())
        {
            result.set_zero();
            return;
        }

        int max_coeff_bit_count = operand.significant_coeff_bit_count();
        int result_coeff_count = exponent * (operand.significant_coeff_count() - 1) + 1;
        int result_coeff_bit_count = exponent * max_coeff_bit_count + 1;

        if (result.coeff_bit_count() < result_coeff_bit_count || result.coeff_count() < result_coeff_count)
        {
            result.resize(result_coeff_count, result_coeff_bit_count);
        }

        MemoryPool pool;
        util::exponentiate_poly(operand.pointer(), operand.significant_coeff_count(), operand.coeff_uint64_count(),
            exponent, result.coeff_count(), result.coeff_uint64_count(), result.pointer(), pool);
    }

    BigPoly exponentiate_poly(const BigPoly &operand, int exponent)
    {
        BigPoly result;
        exponentiate_poly(operand, exponent, result);
        return result;
    }

    BigPoly poly_eval_poly(const BigPoly &poly_to_evaluate, const BigPoly &poly_to_evaluate_at)
    {
        int poly_to_eval_coeff_uint64_count = divide_round_up(poly_to_evaluate.coeff_bit_count(), bits_per_uint64);
        int value_coeff_uint64_count = divide_round_up(poly_to_evaluate_at.coeff_bit_count(), bits_per_uint64);

        if (poly_to_evaluate.is_zero())
        {
            return BigPoly();
        }
        if (poly_to_evaluate_at.is_zero())
        {
            BigPoly result(1, poly_to_evaluate.coeff_bit_count());
            set_uint_uint(poly_to_evaluate.pointer(), poly_to_eval_coeff_uint64_count, result.pointer());
            return result;
        }

        int result_coeff_count = (poly_to_evaluate.significant_coeff_count() - 1) * (poly_to_evaluate_at.significant_coeff_count() - 1) + 1;
        int result_coeff_bit_count = poly_to_evaluate.coeff_bit_count() + (poly_to_evaluate.coeff_count() - 1) * poly_to_evaluate_at.coeff_bit_count();
        int result_coeff_uint64_count = divide_round_up(result_coeff_bit_count, bits_per_uint64);
        BigPoly result(result_coeff_count, result_coeff_bit_count);

        MemoryPool pool;
        util::poly_eval_poly(poly_to_evaluate.pointer(), poly_to_evaluate.coeff_count(), poly_to_eval_coeff_uint64_count, poly_to_evaluate_at.pointer(), poly_to_evaluate_at.coeff_count(),
            value_coeff_uint64_count, result_coeff_count, result_coeff_uint64_count, result.pointer(), pool);

        return result;
    }

}