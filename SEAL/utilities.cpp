#include "utilities.h"
#include "util/uintarith.h"
#include "util/uintarithmod.h"
#include "util/polyarithmod.h"
#include "util/polyextras.h"
#include "util/polycore.h"
#include "util/polyarith.h"
#include "util/uintextras.h"
#include "util/modulus.h"
#include "util/polymodulus.h"
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

        MemoryPool &pool = *MemoryPool::default_pool();
        Modulus mod(modulus.pointer(), modulus.uint64_count(), pool);
        BigUInt result(modulus.significant_bit_count());
        util::poly_infty_norm_coeffmod(poly.pointer(), poly_coeff_count, poly_coeff_uint64_count, mod, result.pointer(), pool);

        return result;
    }

    void exponentiate_uint_mod(const BigUInt &operand, const BigUInt &exponent, 
        const BigUInt &modulus, BigUInt &destination)
    {
        if (operand.significant_bit_count() > modulus.significant_bit_count())
        {
            throw invalid_argument("operand is not reduced");
        }
        if (operand.is_zero() && exponent == 0)
        {
            throw invalid_argument("undefined operation");
        }
        if (operand.is_zero())
        {
            destination.set_zero();
            return;
        }

        if (destination.bit_count() != modulus.significant_bit_count())
        {
            destination.resize(modulus.significant_bit_count());
        }

        MemoryPool &pool = *MemoryPool::default_pool();
        ConstPointer operand_ptr = duplicate_uint_if_needed(operand, modulus.uint64_count(), false, pool);
        util::exponentiate_uint_mod(operand_ptr.get(), exponent.pointer(), exponent.uint64_count(), Modulus(modulus.pointer(), modulus.uint64_count(), pool), destination.pointer(), pool);
    }

    void exponentiate_poly_polymod_coeffmod(const BigPoly &operand, const BigUInt &exponent, 
        const BigPoly &poly_modulus, const BigUInt &coeff_modulus, BigPoly &destination)
    {
        if (operand.significant_coeff_count() > poly_modulus.coeff_count() ||
            operand.significant_coeff_bit_count() > coeff_modulus.significant_bit_count())
        {
            throw invalid_argument("operand is not reduced");
        }
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
            destination.set_zero();
            return;
        }

        if (destination.coeff_bit_count() != coeff_modulus.significant_bit_count() || 
            destination.coeff_count() != poly_modulus.coeff_count())
        {
            destination.resize(poly_modulus.coeff_count(), coeff_modulus.significant_bit_count());
        }

        MemoryPool &pool = *MemoryPool::default_pool();
        ConstPointer operand_ptr = duplicate_poly_if_needed(operand, poly_modulus.coeff_count(), coeff_modulus.uint64_count(), false, pool);
        util::exponentiate_poly_polymod_coeffmod(operand_ptr.get(), exponent.pointer(), exponent.uint64_count(),
            PolyModulus(poly_modulus.pointer(), poly_modulus.coeff_count(), poly_modulus.uint64_count()), 
            Modulus(coeff_modulus.pointer(), coeff_modulus.uint64_count(), pool), 
            destination.pointer(), pool);
    }

    void poly_eval_poly(const BigPoly &poly_to_evaluate, const BigPoly &poly_to_evaluate_at, BigPoly &destination)
    {
        int poly_to_eval_coeff_uint64_count = divide_round_up(poly_to_evaluate.coeff_bit_count(), bits_per_uint64);
        int value_coeff_uint64_count = divide_round_up(poly_to_evaluate_at.coeff_bit_count(), bits_per_uint64);

        if (poly_to_evaluate.is_zero())
        {
            destination.set_zero();
            return;
        }
        if (poly_to_evaluate_at.is_zero())
        {
            destination.resize(1, poly_to_evaluate.coeff_bit_count());
            set_uint_uint(poly_to_evaluate.pointer(), poly_to_eval_coeff_uint64_count, destination.pointer());
            return;
        }

        int result_coeff_count = (poly_to_evaluate.significant_coeff_count() - 1) * (poly_to_evaluate_at.significant_coeff_count() - 1) + 1;
        int result_coeff_bit_count = poly_to_evaluate.coeff_bit_count() + (poly_to_evaluate.coeff_count() - 1) * poly_to_evaluate_at.coeff_bit_count();
        int result_coeff_uint64_count = divide_round_up(result_coeff_bit_count, bits_per_uint64);
        destination.resize(result_coeff_count, result_coeff_bit_count);

        MemoryPool &pool = *MemoryPool::default_pool();
        poly_eval_poly(poly_to_evaluate.pointer(), poly_to_evaluate.coeff_count(), poly_to_eval_coeff_uint64_count, 
            poly_to_evaluate_at.pointer(), poly_to_evaluate_at.coeff_count(), value_coeff_uint64_count, 
            result_coeff_count, result_coeff_uint64_count, destination.pointer(), pool);
    }

    BigPoly poly_eval_poly(const BigPoly &poly_to_evaluate, const BigPoly &poly_to_evaluate_at)
    {
        BigPoly result;
        poly_eval_poly(poly_to_evaluate, poly_to_evaluate_at, result);
        return result;
    }

    void poly_eval_poly_polymod_coeffmod(const BigPoly &poly_to_evaluate, const BigPoly &poly_to_evaluate_at, 
        const BigPoly &poly_modulus, const BigUInt &coeff_modulus, BigPoly &destination)
    {
        if (poly_to_evaluate.significant_coeff_count() > poly_modulus.coeff_count() ||
            poly_to_evaluate.significant_coeff_bit_count() > coeff_modulus.significant_bit_count())
        {
            throw invalid_argument("poly_to_evaluate is not reduced");
        }
        if (poly_to_evaluate_at.significant_coeff_count() > poly_modulus.coeff_count() ||
            poly_to_evaluate_at.significant_coeff_bit_count() > coeff_modulus.significant_bit_count())
        {
            throw invalid_argument("poly_to_evaluate_at is not reduced");
        }

        int poly_to_eval_coeff_uint64_count = poly_to_evaluate.coeff_uint64_count();
        int coeff_modulus_bit_count = coeff_modulus.significant_bit_count();

        if (poly_to_evaluate.is_zero())
        {
            destination.set_zero();
        }

        MemoryPool &pool = *MemoryPool::default_pool();
        Pointer big_alloc(allocate_uint(3 * poly_to_eval_coeff_uint64_count, pool));
        
        if (poly_to_evaluate_at.is_zero())
        {
            destination.resize(1, coeff_modulus_bit_count);
            modulo_uint(poly_to_evaluate.pointer(), poly_to_eval_coeff_uint64_count, 
                Modulus(coeff_modulus.pointer(), coeff_modulus.uint64_count(), pool), 
                destination.pointer(), pool, big_alloc.get());
            return;
        }

        ConstPointer poly_to_eval_ptr = duplicate_poly_if_needed(poly_to_evaluate, poly_modulus.coeff_count(), coeff_modulus.uint64_count(), false, pool);
        ConstPointer poly_to_eval_at_ptr = duplicate_poly_if_needed(poly_to_evaluate_at, poly_modulus.coeff_count(), coeff_modulus.uint64_count(), false, pool);

        destination.resize(poly_modulus.coeff_count(), coeff_modulus_bit_count);

        util::poly_eval_poly_polymod_coeffmod(poly_to_eval_ptr.get(), poly_to_eval_at_ptr.get(),
            PolyModulus(poly_modulus.pointer(), poly_modulus.coeff_count(), poly_modulus.coeff_uint64_count()), 
            Modulus(coeff_modulus.pointer(), coeff_modulus.uint64_count(), pool),
            destination.pointer(), pool);
    }

    BigPoly poly_eval_poly_polymod_coeffmod(const BigPoly &poly_to_evaluate, const BigPoly &poly_to_evaluate_at, 
        const BigPoly &poly_modulus, const BigUInt &coeff_modulus)
    {
        BigPoly result;
        poly_eval_poly_polymod_coeffmod(poly_to_evaluate, poly_to_evaluate_at, poly_modulus, coeff_modulus, result);
        return result;
    }

    void poly_eval_uint_mod(const BigPoly &poly_to_evaluate, const BigUInt &value, const BigUInt &modulus, BigUInt &destination)
    {
        if (poly_to_evaluate.significant_coeff_bit_count() > modulus.significant_bit_count())
        {
            throw invalid_argument("poly_to_evaluate is not reduced");
        }
        if (value.significant_bit_count() > modulus.significant_bit_count())
        {
            throw invalid_argument("value is not reduced");
        }

        int poly_to_eval_coeff_uint64_count = poly_to_evaluate.coeff_uint64_count();
        int modulus_bit_count = modulus.significant_bit_count();

        if (poly_to_evaluate.is_zero())
        {
            destination.set_zero();
        }

        MemoryPool &pool = *MemoryPool::default_pool();
        Pointer big_alloc(allocate_uint(3 * poly_to_eval_coeff_uint64_count, pool));

        if (value.is_zero())
        {
            destination.resize(modulus_bit_count);
            modulo_uint(poly_to_evaluate.pointer(), poly_to_eval_coeff_uint64_count,
                Modulus(modulus.pointer(), modulus.uint64_count(), pool), 
                destination.pointer(), pool, big_alloc.get());
            return;
        }

        ConstPointer value_ptr = duplicate_uint_if_needed(value, modulus.uint64_count(), false, pool);

        destination.resize(modulus_bit_count);

        util::poly_eval_uint_mod(poly_to_evaluate.pointer(), poly_to_eval_coeff_uint64_count, value_ptr.get(), 
            Modulus(modulus.pointer(), modulus.uint64_count(), pool), destination.pointer(), pool);
    }

    BigUInt poly_eval_uint_mod(const BigPoly &poly_to_evaluate, const BigUInt &value, const BigUInt &modulus)
    {
        BigUInt result;
        poly_eval_uint_mod(poly_to_evaluate, value, modulus, result);
        return result;
    }

    BigUInt exponentiate_uint_mod(const BigUInt &operand, const BigUInt &exponent, const BigUInt &modulus)
    {
        BigUInt result(modulus.significant_bit_count());
        exponentiate_uint_mod(operand, exponent, modulus, result);
        return result;
    }

    BigPoly exponentiate_poly_polymod_coeffmod(const BigPoly &operand, const BigUInt &exponent,
        const BigPoly &poly_modulus, const BigUInt &coeff_modulus)
    {
        BigPoly result(poly_modulus.coeff_count(), coeff_modulus.significant_bit_count());
        exponentiate_poly_polymod_coeffmod(operand, exponent, poly_modulus, coeff_modulus, result);
        return result;
    }
}