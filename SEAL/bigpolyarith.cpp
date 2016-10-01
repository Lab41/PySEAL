#include <stdexcept>
#include <algorithm>
#include "bigpolyarith.h"
#include "util/polyarith.h"
#include "util/polyarithmod.h"
#include "util/polyfftmultmod.h"
#include "util/polyfftmult.h"
#include "util/common.h"
#include "util/mempool.h"
#include "util/modulus.h"
#include "util/polymodulus.h"
#include "util/uintarith.h"

using namespace std;
using namespace seal::util;

namespace seal
{
    namespace
    {
        ConstPointer duplicate_poly_if_needed(const BigPoly &poly, int new_coeff_count, int new_coeff_uint64_count, bool force, MemoryPool &pool)
        {
            return util::duplicate_poly_if_needed(poly.pointer(), poly.coeff_count(), poly.coeff_uint64_count(), new_coeff_count, new_coeff_uint64_count, force, pool);
        }

        ConstPointer duplicate_poly_if_needed(const BigPoly &poly, bool force, MemoryPool &pool)
        {
            return util::duplicate_if_needed(poly.pointer(), poly.coeff_count() * poly.coeff_uint64_count(), force, pool);
        }

        void resize_destination_if_needed(BigPoly &destination, int coeff_count, int coeff_bit_count)
        {
            int dest_coeff_count = destination.coeff_count();
            int dest_coeff_bit_count = destination.coeff_bit_count();
            if (dest_coeff_count < coeff_count || dest_coeff_bit_count != coeff_bit_count)
            {
                destination.resize(coeff_count, coeff_bit_count);
            }
            else if (dest_coeff_count > coeff_count)
            {
                destination.set_zero(coeff_count);
            }
        }

        bool are_poly_coefficients_less_than(const BigPoly &poly, const BigUInt &max_coeff)
        {
            return util::are_poly_coefficients_less_than(poly.pointer(), poly.coeff_count(), poly.coeff_uint64_count(), max_coeff.pointer(), max_coeff.uint64_count());
        }
    }

    void BigPolyArith::negate(const BigPoly &poly, const BigUInt &coeff_mod, BigPoly &result)
    {
        // Verify parameters.
        int coeff_count = poly.coeff_count();
        int coeff_bit_count = coeff_mod.bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        if (coeff_mod.is_zero())
        {
            throw invalid_argument("coeff_mod cannot be zero");
        }
        if (!are_poly_coefficients_less_than(poly, coeff_mod))
        {
            throw invalid_argument("poly must have coefficients less than coeff_mod");
        }

        // Get pointer to inputs (duplicated and resized if needed).
        MemoryPool &pool = *MemoryPool::default_pool();
        ConstPointer polyptr = duplicate_poly_if_needed(poly, coeff_count, coeff_uint64_count, false, pool);

        // Verify destination size.
        resize_destination_if_needed(result, coeff_count, coeff_bit_count);

        // Negate polynomial.
        negate_poly_coeffmod(polyptr.get(), coeff_count, coeff_mod.pointer(), coeff_uint64_count, result.pointer());
    }

    void BigPolyArith::negate(const BigPoly &poly, BigPoly &result)
    {
        // Verify destination.
        int coeff_count = poly.coeff_count();
        int coeff_bit_count = poly.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        resize_destination_if_needed(result, coeff_count, coeff_bit_count);

        // Negate polynomial.
        negate_poly(poly.pointer(), coeff_count, coeff_uint64_count, result.pointer());
    }

    void BigPolyArith::add(const BigPoly &poly1, const BigPoly &poly2, const BigUInt &coeff_mod, BigPoly &result)
    {
        // Verify parameters.
        int coeff_count = max(poly1.coeff_count(), poly2.coeff_count());
        int coeff_bit_count = coeff_mod.bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        if (coeff_mod.is_zero())
        {
            throw invalid_argument("coeff_mod cannot be zero");
        }
        if (!are_poly_coefficients_less_than(poly1, coeff_mod))
        {
            throw invalid_argument("poly1 must have coefficients less than coeff_mod");
        }
        if (!are_poly_coefficients_less_than(poly2, coeff_mod))
        {
            throw invalid_argument("poly2 must have coefficients less than coeff_mod");
        }

        // Get pointer to inputs (duplicated and resized if needed).
        MemoryPool &pool = *MemoryPool::default_pool();
        ConstPointer poly1ptr = duplicate_poly_if_needed(poly1, coeff_count, coeff_uint64_count, false, pool);
        ConstPointer poly2ptr = duplicate_poly_if_needed(poly2, coeff_count, coeff_uint64_count, false, pool);

        // Verify destination size.
        resize_destination_if_needed(result, coeff_count, coeff_bit_count);

        // Add polynomials.
        add_poly_poly_coeffmod(poly1ptr.get(), poly2ptr.get(), coeff_count, coeff_mod.pointer(), coeff_uint64_count, result.pointer());
    }

    void BigPolyArith::sub(const BigPoly &poly1, const BigPoly &poly2, const BigUInt &coeff_mod, BigPoly &result)
    {
        // Verify parameters.
        int coeff_count = max(poly1.coeff_count(), poly2.coeff_count());
        int coeff_bit_count = coeff_mod.bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        if (coeff_mod.is_zero())
        {
            throw invalid_argument("coeff_mod cannot be zero");
        }
        if (!are_poly_coefficients_less_than(poly1, coeff_mod))
        {
            throw invalid_argument("poly1 must have coefficients less than coeff_mod");
        }
        if (!are_poly_coefficients_less_than(poly2, coeff_mod))
        {
            throw invalid_argument("poly2 must have coefficients less than coeff_mod");
        }

        // Get pointer to inputs (duplicated and resized if needed).
        MemoryPool &pool = *MemoryPool::default_pool();
        ConstPointer poly1ptr = duplicate_poly_if_needed(poly1, coeff_count, coeff_uint64_count, false, pool);
        ConstPointer poly2ptr = duplicate_poly_if_needed(poly2, coeff_count, coeff_uint64_count, false, pool);

        // Verify destination size.
        resize_destination_if_needed(result, coeff_count, coeff_bit_count);

        // Subtract polynomials.
        sub_poly_poly_coeffmod(poly1ptr.get(), poly2ptr.get(), coeff_count, coeff_mod.pointer(), coeff_uint64_count, result.pointer());
    }

    void BigPolyArith::add(const BigPoly &poly1, const BigPoly &poly2, BigPoly &result)
    {
        // Verify parameters.
        int coeff_count = max(poly1.coeff_count(), poly2.coeff_count());
        int coeff_bit_count = max(poly1.significant_coeff_bit_count(), poly2.significant_coeff_bit_count()) + 1;
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Get pointer to inputs (duplicated and resized if needed).
        MemoryPool &pool = *MemoryPool::default_pool();
        ConstPointer poly1ptr = duplicate_poly_if_needed(poly1, coeff_count, coeff_uint64_count, false, pool);
        ConstPointer poly2ptr = duplicate_poly_if_needed(poly2, coeff_count, coeff_uint64_count, false, pool);

        // Verify destination size.
        resize_destination_if_needed(result, coeff_count, coeff_bit_count);

        // Add polynomials.
        add_poly_poly(poly1ptr.get(), poly2ptr.get(), coeff_count, coeff_uint64_count, result.pointer());
    }

    void BigPolyArith::sub(const BigPoly &poly1, const BigPoly &poly2, BigPoly &result)
    {
        // Verify parameters.
        int coeff_count = max(poly1.coeff_count(), poly2.coeff_count());
        int coeff_bit_count = max(poly1.coeff_bit_count(), poly2.coeff_bit_count());
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Get pointer to inputs (duplicated and resized if needed).
        MemoryPool &pool = *MemoryPool::default_pool();
        ConstPointer poly1ptr = duplicate_poly_if_needed(poly1, coeff_count, coeff_uint64_count, false, pool);
        ConstPointer poly2ptr = duplicate_poly_if_needed(poly2, coeff_count, coeff_uint64_count, false, pool);

        // Verify destination size.
        resize_destination_if_needed(result, coeff_count, coeff_bit_count);

        // Subtract polynomials.
        sub_poly_poly(poly1ptr.get(), poly2ptr.get(), coeff_count, coeff_uint64_count, result.pointer());
    }

    void BigPolyArith::multiply(const BigPoly &poly1, const BigPoly &poly2, const BigPoly &poly_mod, const BigUInt &coeff_mod, BigPoly &result)
    {
        // Verify parameters.
        int coeff_count = poly_mod.significant_coeff_count();
        int coeff_bit_count = coeff_mod.bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        if (poly_mod.is_zero())
        {
            throw invalid_argument("poly_mod cannot be zero");
        }
        if (coeff_mod.is_zero())
        {
            throw invalid_argument("coeff_mod cannot be zero");
        }
        if (poly1.significant_coeff_count() >= coeff_count || !are_poly_coefficients_less_than(poly1, coeff_mod))
        {
            throw invalid_argument("poly1 must have fewer coefficients than poly_mod and coefficients less than coeff_mod");
        }
        if (poly2.significant_coeff_count() >= coeff_count || !are_poly_coefficients_less_than(poly2, coeff_mod))
        {
            throw invalid_argument("poly2 must have fewer coefficients than poly_mod and coefficients less than coeff_mod");
        }
        if (!are_poly_coefficients_less_than(poly_mod, coeff_mod))
        {
            throw invalid_argument("poly_mod must have coefficients less than coeff_mod");
        }

        // Get pointer to inputs (duplicated and resized if needed).
        MemoryPool &pool = *MemoryPool::default_pool();
        ConstPointer poly1ptr = duplicate_poly_if_needed(poly1, coeff_count, coeff_uint64_count, poly1.pointer() == result.pointer(), pool);
        ConstPointer poly2ptr = duplicate_poly_if_needed(poly2, coeff_count, coeff_uint64_count, poly2.pointer() == result.pointer(), pool);
        ConstPointer polymodptr = duplicate_poly_if_needed(poly_mod, coeff_count, coeff_uint64_count, poly_mod.pointer() == result.pointer(), pool);

        // Verify destination size.
        resize_destination_if_needed(result, coeff_count, coeff_bit_count);

        // Multiply polynomials.
        Modulus modulus(coeff_mod.pointer(), coeff_uint64_count, pool);
        PolyModulus polymod(polymodptr.get(), coeff_count, coeff_uint64_count);
        if (polymod.is_fft_modulus())
        {
             nussbaumer_multiply_poly_poly_coeffmod(poly1ptr.get(), poly2ptr.get(), polymod.coeff_count_power_of_two(), modulus, result.pointer(), pool);
        }
        else
        {
            nonfft_multiply_poly_poly_polymod_coeffmod(poly1ptr.get(), poly2ptr.get(), polymod, modulus, result.pointer(), pool);
        }
    }

    void BigPolyArith::multiply(const BigPoly &poly1, const BigPoly &poly2, const BigUInt &coeff_mod, BigPoly &result)
    {
        // Verify parameters.
        int coeff_count = poly1.significant_coeff_count() + poly2.significant_coeff_count() - 1;
        int coeff_bit_count = coeff_mod.bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        if (coeff_mod.is_zero())
        {
            throw invalid_argument("coeff_mod cannot be zero");
        }
        if (!are_poly_coefficients_less_than(poly1, coeff_mod))
        {
            throw invalid_argument("poly1 must have coefficients less than coeff_mod");
        }
        if (!are_poly_coefficients_less_than(poly2, coeff_mod))
        {
            throw invalid_argument("poly2 must have coefficients less than coeff_mod");
        }

        // Get pointer to inputs (duplicated and resized if needed).
        MemoryPool &pool = *MemoryPool::default_pool();
        ConstPointer poly1ptr = duplicate_poly_if_needed(poly1, poly1.pointer() == result.pointer(), pool);
        ConstPointer poly2ptr = duplicate_poly_if_needed(poly2, poly2.pointer() == result.pointer(), pool);

        // Verify destination size.
        resize_destination_if_needed(result, coeff_count, coeff_bit_count);

        // Multiply polynomials.
        Modulus modulus(coeff_mod.pointer(), coeff_uint64_count, pool);
        multiply_poly_poly_coeffmod(poly1ptr.get(), poly1.coeff_count(), poly1.coeff_uint64_count(), poly2ptr.get(), poly2.coeff_count(), poly2.coeff_uint64_count(), modulus, coeff_count, result.pointer(), pool);
    }

    void BigPolyArith::multiply(const BigPoly &poly1, const BigPoly &poly2, BigPoly &result)
    {
        // Verify parameters.
        int coeff_count = poly1.significant_coeff_count() + poly2.significant_coeff_count() - 1;
        int coeff_bit_count = poly1.significant_coeff_bit_count() + poly2.significant_coeff_bit_count() + get_significant_bit_count(min(poly1.significant_coeff_count(), poly2.significant_coeff_count()));
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Get pointer to inputs (duplicated and resized if needed).
        MemoryPool &pool = *MemoryPool::default_pool();
        ConstPointer poly1ptr = duplicate_poly_if_needed(poly1, poly1.pointer() == result.pointer(), pool);
        ConstPointer poly2ptr = duplicate_poly_if_needed(poly2, poly2.pointer() == result.pointer(), pool);

        // Verify destination size.
        resize_destination_if_needed(result, coeff_count, coeff_bit_count);

        // Multiply polynomials.
        multiply_poly_poly(poly1ptr.get(), poly1.coeff_count(), poly1.coeff_uint64_count(), poly2ptr.get(), poly2.coeff_count(), poly2.coeff_uint64_count(), coeff_count, coeff_uint64_count, result.pointer(), pool);
    }

    void BigPolyArith::multiply(const BigPoly &poly1, const BigUInt &uint2, const BigUInt &coeff_mod, BigPoly &result)
    {
        // Verify parameters.
        int coeff_count = poly1.coeff_count();
        int coeff_bit_count = coeff_mod.bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        if (coeff_mod.is_zero())
        {
            throw invalid_argument("coeff_mod cannot be zero");
        }
        if (!are_poly_coefficients_less_than(poly1, coeff_mod))
        {
            throw invalid_argument("poly1 must have coefficients less than coeff_mod");
        }
        if (uint2 >= coeff_mod)
        {
            throw invalid_argument("uint2 must be less than coeff_mod");
        }

        // Verify destination size.
        resize_destination_if_needed(result, coeff_count, coeff_bit_count);

        // Multiply polynomial and uint.
        MemoryPool &pool = *MemoryPool::default_pool();
        int temp_bit_count = poly1.significant_coeff_bit_count() + uint2.significant_bit_count();
        int temp_uint64_count = divide_round_up(temp_bit_count, bits_per_uint64);
        Pointer temp_ptr = allocate_uint(temp_uint64_count, pool);
        int sig_coeff_count = poly1.significant_coeff_count();
        const uint64_t *poly1_coeff = poly1.pointer();
        int poly1_uint64_count = poly1.coeff_uint64_count();
        const uint64_t *uint2ptr = uint2.pointer();
        int uint2_uint64_count = uint2.uint64_count();
        uint64_t *result_coeff = result.pointer();
        Modulus modulus(coeff_mod.pointer(), coeff_uint64_count, pool);

        Pointer big_alloc(allocate_uint(3 * temp_uint64_count, pool));
        for (int i = 0; i < coeff_count; ++i)
        {
            if (i < sig_coeff_count)
            {
                multiply_uint_uint(poly1_coeff, poly1_uint64_count, uint2ptr, uint2_uint64_count, temp_uint64_count, temp_ptr.get());
                modulo_uint(temp_ptr.get(), temp_uint64_count, modulus, result_coeff, pool, big_alloc.get());
            }
            else
            {
                set_zero_uint(coeff_uint64_count, result_coeff);
            }
            poly1_coeff += poly1_uint64_count;
            result_coeff += coeff_uint64_count;
        }
    }

    void BigPolyArith::multiply(const BigPoly &poly1, const BigUInt &uint2, BigPoly &result)
    {
        // Verify parameters.
        int coeff_count = poly1.coeff_count();
        int coeff_bit_count = poly1.significant_coeff_bit_count() + uint2.significant_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Get pointer to inputs (duplicated and resized if needed).
        MemoryPool &pool = *MemoryPool::default_pool();
        ConstPointer poly1ptr = duplicate_poly_if_needed(poly1, poly1.pointer() == result.pointer(), pool);

        // Verify destination size.
        resize_destination_if_needed(result, coeff_count, coeff_bit_count);

        // Multiply polynomial and uint.
        int sig_coeff_count = poly1.significant_coeff_count();
        const uint64_t *poly1_coeff = poly1ptr.get();
        int poly1_uint64_count = poly1.coeff_uint64_count();
        const uint64_t *uint2ptr = uint2.pointer();
        int uint2_uint64_count = uint2.uint64_count();
        uint64_t *result_coeff = result.pointer();
        for (int i = 0; i < coeff_count; ++i)
        {
            if (i < sig_coeff_count)
            {
                multiply_uint_uint(poly1_coeff, poly1_uint64_count, uint2ptr, uint2_uint64_count, coeff_uint64_count, result_coeff);
            }
            else
            {
                set_zero_uint(coeff_uint64_count, result_coeff);
            }
            poly1_coeff += poly1_uint64_count;
            result_coeff += coeff_uint64_count;
        }
    }

    void BigPolyArith::divide(const BigPoly &numerator, const BigPoly &denominator, const BigUInt &coeff_mod, BigPoly &quotient, BigPoly &remainder)
    {
        // Verify parameters.
        int coeff_count = max(numerator.coeff_count(), denominator.coeff_count());
        int coeff_bit_count = coeff_mod.bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        if (denominator.is_zero())
        {
            throw invalid_argument("denominator cannot be zero");
        }
        if (coeff_mod.is_zero())
        {
            throw invalid_argument("coeff_mod cannot be zero");
        }
        if (!are_poly_coefficients_less_than(numerator, coeff_mod))
        {
            throw invalid_argument("numerator must have coefficients less than coeff_mod");
        }
        if (!are_poly_coefficients_less_than(denominator, coeff_mod))
        {
            throw invalid_argument("denominator must have coefficients less than coeff_mod");
        }
        if (quotient.pointer() == remainder.pointer())
        {
            throw invalid_argument("remainder and quotient must be distinct");
        }

        // Get pointer to inputs (duplicated and resized if needed).
        MemoryPool &pool = *MemoryPool::default_pool();
        ConstPointer numptr = duplicate_poly_if_needed(numerator, coeff_count, coeff_uint64_count, false, pool);
        ConstPointer denomptr = duplicate_poly_if_needed(denominator, coeff_count, coeff_uint64_count, denominator.pointer() == quotient.pointer(), pool);

        // Verify destination size.
        resize_destination_if_needed(quotient, coeff_count, coeff_bit_count);
        resize_destination_if_needed(remainder, coeff_count, coeff_bit_count);

        // Divide polynomials.
        Modulus modulus(coeff_mod.pointer(), coeff_uint64_count, pool);
        divide_poly_poly_coeffmod(numptr.get(), denomptr.get(), coeff_count, modulus, quotient.pointer(), remainder.pointer(), pool);
    }

    void BigPolyArith::modulo(const BigPoly &numerator, const BigPoly &denominator, const BigUInt &coeff_mod, BigPoly &remainder)
    {
        // Verify parameters.
        int coeff_count = denominator.coeff_count();
        int coeff_bit_count = coeff_mod.bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        if (denominator.is_zero())
        {
            throw invalid_argument("denominator cannot be zero");
        }
        if (coeff_mod.is_zero())
        {
            throw invalid_argument("coeff_mod cannot be zero");
        }
        if (!are_poly_coefficients_less_than(numerator, coeff_mod))
        {
            throw invalid_argument("numerator must have coefficients less than coeff_mod");
        }
        if (!are_poly_coefficients_less_than(denominator, coeff_mod))
        {
            throw invalid_argument("denominator must have coefficients less than coeff_mod");
        }

        // Get pointer to inputs (duplicated and resized if needed).
        MemoryPool &pool = *MemoryPool::default_pool();
        ConstPointer numptr = duplicate_poly_if_needed(numerator, numerator.coeff_count(), coeff_uint64_count, numerator.pointer() == remainder.pointer(), pool);
        ConstPointer denomptr = duplicate_poly_if_needed(denominator, coeff_count, coeff_uint64_count, denominator.pointer() == remainder.pointer(), pool);

        // Verify destination size.
        resize_destination_if_needed(remainder, coeff_count, coeff_bit_count);

        // Divide polynomials.
        Modulus modulus(coeff_mod.pointer(), coeff_uint64_count, pool);
        PolyModulus polymod(denomptr.get(), coeff_count, coeff_uint64_count);
        modulo_poly(numptr.get(), numerator.coeff_count(), polymod, modulus, remainder.pointer(), pool);
    }

    bool BigPolyArith::try_invert(const BigPoly &poly, const BigPoly &poly_mod, const BigUInt &coeff_mod, BigPoly &result)
    {
        // Verify parameters.
        int coeff_count = poly_mod.significant_coeff_count();
        int coeff_bit_count = coeff_mod.bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        if (poly_mod.is_zero())
        {
            throw invalid_argument("poly_mod cannot be zero");
        }
        if (coeff_mod.is_zero())
        {
            throw invalid_argument("coeff_mod cannot be zero");
        }
        if (poly.significant_coeff_count() >= coeff_count || !are_poly_coefficients_less_than(poly, coeff_mod))
        {
            throw invalid_argument("poly must have fewer coefficients than poly_mod and coefficients less than coeff_mod");
        }
        if (!are_poly_coefficients_less_than(poly_mod, coeff_mod))
        {
            throw invalid_argument("poly_mod must have coefficients less than coeff_mod");
        }

        // Get pointer to inputs (duplicated and resized if needed).
        MemoryPool &pool = *MemoryPool::default_pool();
        ConstPointer polyptr = duplicate_poly_if_needed(poly, coeff_count, coeff_uint64_count, poly.pointer() == result.pointer(), pool);
        ConstPointer polymodptr = duplicate_poly_if_needed(poly_mod, coeff_count, coeff_uint64_count, poly_mod.pointer() == result.pointer(), pool);

        // Verify destination size.
        resize_destination_if_needed(result, coeff_count, coeff_bit_count);

        // Invert polynomial.
        Modulus modulus(coeff_mod.pointer(), coeff_uint64_count, pool);
        return try_invert_poly_coeffmod(polyptr.get(), polymodptr.get(), coeff_count, modulus, result.pointer(), pool);
    }
}
