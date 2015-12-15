#include <algorithm>
#include <stdexcept>
#include "evaluator.h"
#include "util/common.h"
#include "util/uintcore.h"
#include "util/uintarith.h"
#include "util/uintarithmod.h"
#include "util/polycore.h"
#include "util/polyarith.h"
#include "util/polyarithmod.h"
#include "util/polyfftmult.h"
#include "bigpoly.h"

using namespace std;
using namespace seal::util;

namespace seal
{
    namespace
    {
        ConstPointer duplicate_poly_if_needed(const BigPoly &poly, int new_coeff_count, int new_coeff_uint64_count, bool force, MemoryPool &pool)
        {
            return duplicate_poly_if_needed(poly.pointer(), poly.coeff_count(), poly.coeff_uint64_count(), new_coeff_count, new_coeff_uint64_count, force, pool);
        }

        ConstPointer duplicate_poly_if_needed(const BigPoly &poly, bool force, MemoryPool &pool)
        {
            return duplicate_if_needed(poly.pointer(), poly.coeff_count() * poly.coeff_uint64_count(), force, pool);
        }

        bool are_poly_coefficients_less_than(const BigPoly &poly, const BigUInt &max_coeff)
        {
            return util::are_poly_coefficients_less_than(poly.pointer(), poly.coeff_count(), poly.coeff_uint64_count(), max_coeff.pointer(), max_coeff.uint64_count());
        }
    }

    void Evaluator::negate(const BigPoly &encrypted, BigPoly &destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Verify parameters.
        if (encrypted.coeff_count() != coeff_count || encrypted.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("encrypted is not valid for encryption parameters");
        }
#ifdef _DEBUG
        if (encrypted.significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted, coeff_modulus_))
        {
            throw invalid_argument("encrypted is not valid for encryption parameters");
        }
#endif
        if (destination.coeff_count() != coeff_count || destination.coeff_bit_count() != coeff_bit_count)
        {
            destination.resize(coeff_count, coeff_bit_count);
        }

        // Handle test-mode case.
        if (mode_ == TEST_MODE)
        {
            negate_poly_coeffmod(encrypted.pointer(), coeff_count, plain_modulus_.pointer(), coeff_uint64_count, destination.pointer());
            return;
        }

        // Negate polynomial.
        negate_poly_coeffmod(encrypted.pointer(), coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, destination.pointer());
    }

    void Evaluator::add(const BigPoly &encrypted1, const BigPoly &encrypted2, BigPoly &destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Verify parameters.
        if (encrypted1.coeff_count() != coeff_count || encrypted1.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("encrypted1 is not valid for encryption parameters");
        }
        if (encrypted2.coeff_count() != coeff_count || encrypted2.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("encrypted2 is not valid for encryption parameters");
        }
#ifdef _DEBUG
        if (encrypted1.significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted1, coeff_modulus_))
        {
            throw invalid_argument("encrypted1 is not valid for encryption parameters");
        }
        if (encrypted2.significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted2, coeff_modulus_))
        {
            throw invalid_argument("encrypted2 is not valid for encryption parameters");
        }
#endif
        if (destination.coeff_count() != coeff_count || destination.coeff_bit_count() != coeff_bit_count)
        {
            destination.resize(coeff_count, coeff_bit_count);
        }

        // Handle test-mode case.
        if (mode_ == TEST_MODE)
        {
            add_poly_poly_coeffmod(encrypted1.pointer(), encrypted2.pointer(), coeff_count, plain_modulus_.pointer(), coeff_uint64_count, destination.pointer());
            return;
        }

        // Add polynomials.
        add_poly_poly_coeffmod(encrypted1.pointer(), encrypted2.pointer(), coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, destination.pointer());
    }

    void Evaluator::add_many(const std::vector<BigPoly> &encrypteds, BigPoly &destination)
    {
        if (encrypteds.empty())
        {
            throw invalid_argument("encrypteds cannot be empty");
        }

        if (destination.coeff_count() != encrypteds[0].coeff_count() || destination.coeff_bit_count() != encrypteds[0].coeff_bit_count())
        {
            destination.resize(encrypteds[0].coeff_count(), encrypteds[0].coeff_bit_count());
        }

        destination = encrypteds[0];
        for (vector<BigPoly>::size_type i = 1; i < encrypteds.size(); ++i)
        {
            add(destination, encrypteds[i], destination);
        }
    }

    void Evaluator::sub(const BigPoly &encrypted1, const BigPoly &encrypted2, BigPoly &destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Verify parameters.
        if (encrypted1.coeff_count() != coeff_count || encrypted1.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("encrypted1 is not valid for encryption parameters");
        }
        if (encrypted2.coeff_count() != coeff_count || encrypted2.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("encrypted2 is not valid for encryption parameters");
        }
#ifdef _DEBUG
        if (encrypted1.significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted1, coeff_modulus_))
        {
            throw invalid_argument("encrypted1 is not valid for encryption parameters");
        }
        if (encrypted2.significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted2, coeff_modulus_))
        {
            throw invalid_argument("encrypted2 is not valid for encryption parameters");
        }
#endif
        if (destination.coeff_count() != coeff_count || destination.coeff_bit_count() != coeff_bit_count)
        {
            destination.resize(coeff_count, coeff_bit_count);
        }

        // Handle test-mode case.
        if (mode_ == TEST_MODE)
        {
            sub_poly_poly_coeffmod(encrypted1.pointer(), encrypted2.pointer(), coeff_count, plain_modulus_.pointer(), coeff_uint64_count, destination.pointer());
            return;
        }

        // Subtract polynomials.
        sub_poly_poly_coeffmod(encrypted1.pointer(), encrypted2.pointer(), coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, destination.pointer());
    }

    void Evaluator::multiply(const BigPoly &encrypted1, const BigPoly &encrypted2, BigPoly &destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Verify parameters.
        if (encrypted1.coeff_count() != coeff_count || encrypted1.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("encrypted1 is not valid for encryption parameters");
        }
        if (encrypted2.coeff_count() != coeff_count || encrypted2.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("encrypted2 is not valid for encryption parameters");
        }
#ifdef _DEBUG
        if (encrypted1.significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted1, coeff_modulus_))
        {
            throw invalid_argument("encrypted1 is not valid for encryption parameters");
        }
        if (encrypted2.significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted2, coeff_modulus_))
        {
            throw invalid_argument("encrypted2 is not valid for encryption parameters");
        }
#endif
        if (destination.coeff_count() != coeff_count || destination.coeff_bit_count() != coeff_bit_count)
        {
            destination.resize(coeff_count, coeff_bit_count);
        }

        // Handle test-mode case.
        if (mode_ == TEST_MODE)
        {
            // Get pointer to inputs (duplicated if needed).
            ConstPointer encrypted1ptr = duplicate_poly_if_needed(encrypted1, encrypted1.pointer() == destination.pointer(), pool_);
            ConstPointer encrypted2ptr = duplicate_poly_if_needed(encrypted2, encrypted2.pointer() == destination.pointer(), pool_);

            multiply_poly_poly_polymod_coeffmod(encrypted1ptr.get(), encrypted2ptr.get(), polymod_, mod_, destination.pointer(), pool_);
            return;
        }

        // Multiply encrypted polynomials and perform key switching.
        Pointer product(allocate_poly(coeff_count, coeff_uint64_count, pool_));
        multiply(encrypted1.pointer(), encrypted2.pointer(), product.get());
        relinearize(product.get(), destination.pointer());
    }

    void Evaluator::multiply_norelin(const BigPoly &encrypted1, const BigPoly &encrypted2, BigPoly &destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();

        // Verify parameters.
        if (encrypted1.coeff_count() != coeff_count || encrypted1.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("encrypted1 is not valid for encryption parameters");
        }
        if (encrypted2.coeff_count() != coeff_count || encrypted2.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("encrypted2 is not valid for encryption parameters");
        }
#ifdef _DEBUG
        if (encrypted1.significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted1, coeff_modulus_))
        {
            throw invalid_argument("encrypted1 is not valid for encryption parameters");
        }
        if (encrypted2.significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted2, coeff_modulus_))
        {
            throw invalid_argument("encrypted2 is not valid for encryption parameters");
        }
#endif
        if (destination.coeff_count() != coeff_count || destination.coeff_bit_count() != coeff_bit_count)
        {
            destination.resize(coeff_count, coeff_bit_count);
        }

        // Get pointer to inputs (duplicated if needed).
        ConstPointer encrypted1ptr = duplicate_poly_if_needed(encrypted1, encrypted1.pointer() == destination.pointer(), pool_);
        ConstPointer encrypted2ptr = duplicate_poly_if_needed(encrypted2, encrypted2.pointer() == destination.pointer(), pool_);

        // Handle test-mode case.
        if (mode_ == TEST_MODE)
        {
            multiply_poly_poly_polymod_coeffmod(encrypted1ptr.get(), encrypted2ptr.get(), polymod_, mod_, destination.pointer(), pool_);
            return;
        }

        // Multiply encrypted polynomials without performing key switching.
        multiply(encrypted1ptr.get(), encrypted2ptr.get(), destination.pointer());
    }

    void Evaluator::relinearize(const BigPoly &encrypted, BigPoly &destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Verify parameters.
        if (encrypted.coeff_count() != coeff_count || encrypted.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("encrypted is not valid for encryption parameters");
        }
#ifdef _DEBUG
        if (encrypted.significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted, coeff_modulus_))
        {
            throw invalid_argument("encrypted is not valid for encryption parameters");
        }
#endif
        if (destination.coeff_count() != coeff_count || destination.coeff_bit_count() != coeff_bit_count)
        {
            destination.resize(coeff_count, coeff_bit_count);
        }

        // Handle test-mode case.
        if (mode_ == TEST_MODE)
        {
            set_poly_poly(encrypted.pointer(), coeff_count, coeff_uint64_count, destination.pointer());
            return;
        }

        // Get pointer to inputs (duplicated if needed).
        ConstPointer encryptedptr = duplicate_poly_if_needed(encrypted, encrypted.pointer() == destination.pointer(), pool_);

        // Relinearize polynomial.
        relinearize(encryptedptr.get(), destination.pointer());
    }

    void Evaluator::add_plain(const BigPoly &encrypted1, const BigPoly &plain2, BigPoly &destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Verify parameters.
        if (encrypted1.coeff_count() != coeff_count || encrypted1.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("encrypted1 is not valid for encryption parameters");
        }
#ifdef _DEBUG
        if (encrypted1.significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted1, coeff_modulus_))
        {
            throw invalid_argument("encrypted1 is not valid for encryption parameters");
        }
        if (plain2.significant_coeff_count() >= coeff_count || !are_poly_coefficients_less_than(plain2, plain_modulus_))
        {
            throw invalid_argument("plain2 is too large to be represented by encryption parameters");
        }
#endif
        if (destination.coeff_count() != coeff_count || destination.coeff_bit_count() != coeff_bit_count)
        {
            destination.resize(coeff_count, coeff_bit_count);
        }

        int plain2_coeff_uint64_count = divide_round_up(plain2.coeff_bit_count(), bits_per_uint64);
        if (mode_ == TEST_MODE)
        {
            // Handle test-mode case.
            set_poly_poly(plain2.pointer(), plain2.coeff_count(), plain2_coeff_uint64_count, coeff_count, coeff_uint64_count, destination.pointer());
            modulo_poly_coeffs(destination.pointer(), coeff_count, mod_, pool_);
            add_poly_poly_coeffmod(encrypted1.pointer(), destination.pointer(), coeff_count, plain_modulus_.pointer(), coeff_uint64_count, destination.pointer());
            return;
        }

        // Multiply plain by scalar coeff_div_plaintext and reposition if in upper-half.
        preencrypt(plain2.pointer(), plain2.coeff_count(), plain2_coeff_uint64_count, destination.pointer());

        // Add encrypted polynomial and encrypted-version of plain2.
        add_poly_poly_coeffmod(encrypted1.pointer(), destination.pointer(), coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, destination.pointer());
    }

    void Evaluator::sub_plain(const BigPoly &encrypted1, const BigPoly &plain2, BigPoly &destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Verify parameters.
        if (encrypted1.coeff_count() != coeff_count || encrypted1.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("encrypted1 is not valid for encryption parameters");
        }
#ifdef _DEBUG
        if (encrypted1.significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted1, coeff_modulus_))
        {
            throw invalid_argument("encrypted1 is not valid for encryption parameters");
        }
        if (plain2.significant_coeff_count() >= coeff_count || !are_poly_coefficients_less_than(plain2, plain_modulus_))
        {
            throw invalid_argument("plain2 is too large to be represented by encryption parameters");
        }
#endif
        if (destination.coeff_count() != coeff_count || destination.coeff_bit_count() != coeff_bit_count)
        {
            destination.resize(coeff_count, coeff_bit_count);
        }

        int plain2_coeff_uint64_count = divide_round_up(plain2.coeff_bit_count(), bits_per_uint64);
        if (mode_ == TEST_MODE)
        {
            // Handle test-mode case.
            set_poly_poly(plain2.pointer(), plain2.coeff_count(), plain2_coeff_uint64_count, coeff_count, coeff_uint64_count, destination.pointer());
            modulo_poly_coeffs(destination.pointer(), coeff_count, mod_, pool_);
            sub_poly_poly_coeffmod(encrypted1.pointer(), destination.pointer(), coeff_count, plain_modulus_.pointer(), coeff_uint64_count, destination.pointer());
            return;
        }

        // Multiply plain by scalar coeff_div_plaintext and reposition if in upper-half.
        preencrypt(plain2.pointer(), plain2.coeff_count(), plain2_coeff_uint64_count, destination.pointer());

        // Subtract encrypted polynomial and encrypted-version of plain2.
        sub_poly_poly_coeffmod(encrypted1.pointer(), destination.pointer(), coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, destination.pointer());
    }

    void Evaluator::multiply_plain(const BigPoly &encrypted1, const BigPoly &plain2, BigPoly &destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Verify parameters.
        if (encrypted1.coeff_count() != coeff_count || encrypted1.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("encrypted1 is not valid for encryption parameters");
        }
#ifdef _DEBUG
        if (encrypted1.significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted1, coeff_modulus_))
        {
            throw invalid_argument("encrypted1 is not valid for encryption parameters");
        }
        if (plain2.significant_coeff_count() >= coeff_count || !are_poly_coefficients_less_than(plain2, plain_modulus_))
        {
            throw invalid_argument("plain2 is too large to be represented by encryption parameters");
        }
#endif
        if (destination.coeff_count() != coeff_count || destination.coeff_bit_count() != coeff_bit_count)
        {
            destination.resize(coeff_count, coeff_bit_count);
        }

        // Get pointer to inputs (duplicated if needed).
        ConstPointer encrypted1ptr = duplicate_poly_if_needed(encrypted1, encrypted1.pointer() == destination.pointer(), pool_);

        // Handle test-mode case.
        if (mode_ == TEST_MODE)
        {
            // Get pointer to inputs (duplicated and resized if needed).
            ConstPointer plain2ptr = duplicate_poly_if_needed(plain2, coeff_count, coeff_uint64_count, plain2.pointer() == destination.pointer(), pool_);

            // Resize second operand if needed.
            multiply_poly_poly_polymod_coeffmod(encrypted1ptr.get(), plain2ptr.get(), polymod_, mod_, destination.pointer(), pool_);
            return;
        }

        // Reposition coefficients.
        Pointer moved2ptr(allocate_poly(coeff_count, coeff_uint64_count, pool_));
        int plain_coeff_count = min(plain2.significant_coeff_count(), coeff_count);
        int plain2_coeff_uint64_count = plain2.coeff_uint64_count();
        const uint64_t *plain2_coeff = plain2.pointer();
        uint64_t *moved2_coeff = moved2ptr.get();
        for (int i = 0; i < plain_coeff_count; ++i)
        {
            set_uint_uint(plain2_coeff, plain2_coeff_uint64_count, coeff_uint64_count, moved2_coeff);
            bool is_upper_half = is_greater_than_or_equal_uint_uint(moved2_coeff, plain_upper_half_threshold_.pointer(), coeff_uint64_count);
            if (is_upper_half)
            {
                add_uint_uint(moved2_coeff, plain_upper_half_increment_.pointer(), coeff_uint64_count, moved2_coeff);
            }
            moved2_coeff += coeff_uint64_count;
            plain2_coeff += plain2_coeff_uint64_count;
        }
        for (int i = plain_coeff_count; i < coeff_count; ++i)
        {
            set_zero_uint(coeff_uint64_count, moved2_coeff);
            moved2_coeff += coeff_uint64_count;
        }

        // Use normal polynomial multiplication.
        multiply_poly_poly_polymod_coeffmod(encrypted1ptr.get(), moved2ptr.get(), polymod_, mod_, destination.pointer(), pool_);
    }

    void Evaluator::preencrypt(const uint64_t *plain, int plain_coeff_count, int plain_coeff_uint64_count, uint64_t *destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Only care about coefficients up till coeff_count.
        if (plain_coeff_count > coeff_count)
        {
            plain_coeff_count = coeff_count;
        }

        // Multiply plain by scalar coeff_div_plaintext and reposition if in upper-half.
        if (plain == destination)
        {
            // If plain and destination are same poly, then need another storage for multiply output.
            Pointer temp(allocate_uint(coeff_uint64_count, pool_));
            for (int i = 0; i < plain_coeff_count; ++i)
            {
                multiply_uint_uint(plain, plain_coeff_uint64_count, coeff_div_plain_modulus_.pointer(), coeff_uint64_count, coeff_uint64_count, temp.get());
                bool is_upper_half = is_greater_than_or_equal_uint_uint(temp.get(), upper_half_threshold_.pointer(), coeff_uint64_count);
                if (is_upper_half)
                {
                    add_uint_uint(temp.get(), upper_half_increment_.pointer(), coeff_uint64_count, destination);
                }
                else
                {
                    set_uint_uint(temp.get(), coeff_uint64_count, destination);
                }
                plain += plain_coeff_uint64_count;
                destination += coeff_uint64_count;
            }
        }
        else
        {
            for (int i = 0; i < plain_coeff_count; ++i)
            {
                multiply_uint_uint(plain, plain_coeff_uint64_count, coeff_div_plain_modulus_.pointer(), coeff_uint64_count, coeff_uint64_count, destination);
                bool is_upper_half = is_greater_than_or_equal_uint_uint(destination, upper_half_threshold_.pointer(), coeff_uint64_count);
                if (is_upper_half)
                {
                    add_uint_uint(destination, upper_half_increment_.pointer(), coeff_uint64_count, destination);
                }
                plain += plain_coeff_uint64_count;
                destination += coeff_uint64_count;
            }
        }

        // Zero any remaining coefficients.
        for (int i = plain_coeff_count; i < coeff_count; ++i)
        {
            set_zero_uint(coeff_uint64_count, destination);
            destination += coeff_uint64_count;
        }
    }

    void Evaluator::multiply(const uint64_t *encrypted1, const uint64_t *encrypted2, uint64_t *destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Clear destatintion.
        set_zero_poly(coeff_count, coeff_uint64_count, destination);

        // Determine if FFT can be used.
        bool use_fft = polymod_.coeff_count_power_of_two() >= 0 && polymod_.is_one_zero_one();

        if (use_fft)
        {
            // Use FFT to multiply polynomials.

            // Allocate polynomial to store product of two polynomials, with poly but no coeff modulo yet (and signed).
            int product_coeff_bit_count = coeff_bit_count + coeff_bit_count + get_significant_bit_count(static_cast<uint64_t>(coeff_count)) + 2;
            int product_coeff_uint64_count = divide_round_up(product_coeff_bit_count, bits_per_uint64);
            Pointer product(allocate_poly(coeff_count, product_coeff_uint64_count, pool_));

            // Use FFT to multiply polynomials.
            set_zero_uint(product_coeff_uint64_count, get_poly_coeff(product.get(), coeff_count - 1, product_coeff_uint64_count));
            fftmultiply_poly_poly_polymod(encrypted1, encrypted2, polymod_.coeff_count_power_of_two(), coeff_uint64_count, product_coeff_uint64_count, product.get(), pool_);

            // For each coefficient in product, multiply by plain_modulus and divide by coeff_modulus and then modulo by coeff_modulus.
            int plain_modulus_bit_count = plain_modulus_.significant_bit_count();
            int plain_modulus_uint64_count = divide_round_up(plain_modulus_bit_count, bits_per_uint64);
            int intermediate_bit_count = product_coeff_bit_count + plain_modulus_bit_count - 1;
            int intermediate_uint64_count = divide_round_up(intermediate_bit_count, bits_per_uint64);
            Pointer intermediate(allocate_uint(intermediate_uint64_count, pool_));
            Pointer quotient(allocate_uint(intermediate_uint64_count, pool_));
            for (int coeff_index = 0; coeff_index < coeff_count; ++coeff_index)
            {
                uint64_t *product_coeff = get_poly_coeff(product.get(), coeff_index, product_coeff_uint64_count);
                bool coeff_is_negative = is_high_bit_set_uint(product_coeff, product_coeff_uint64_count);
                if (coeff_is_negative)
                {
                    negate_uint(product_coeff, product_coeff_uint64_count, product_coeff);
                }
                multiply_uint_uint(product_coeff, product_coeff_uint64_count, plain_modulus_.pointer(), plain_modulus_uint64_count, intermediate_uint64_count, intermediate.get());
                add_uint_uint(intermediate.get(), wide_coeff_modulus_div_two_.pointer(), intermediate_uint64_count, intermediate.get());
                divide_uint_uint_inplace(intermediate.get(), wide_coeff_modulus_.pointer(), intermediate_uint64_count, quotient.get(), pool_);
                modulo_uint_inplace(quotient.get(), intermediate_uint64_count, mod_, pool_);
                uint64_t *dest_coeff = get_poly_coeff(destination, coeff_index, coeff_uint64_count);
                if (coeff_is_negative)
                {
                    negate_uint_mod(quotient.get(), coeff_modulus_.pointer(), coeff_uint64_count, dest_coeff);
                }
                else
                {
                    set_uint_uint(quotient.get(), coeff_uint64_count, dest_coeff);
                }
            }
        }
        else
        {
            // Use normal multiplication to multiply polynomials.

            // Allocate polynomial to store product of two polynomials, with no poly or coeff modulo yet.
            int product_coeff_count = coeff_count + coeff_count - 1;
            int product_coeff_bit_count = coeff_bit_count + coeff_bit_count + get_significant_bit_count(static_cast<uint64_t>(coeff_count));
            int product_coeff_uint64_count = divide_round_up(product_coeff_bit_count, bits_per_uint64);
            Pointer product(allocate_poly(product_coeff_count, product_coeff_uint64_count, pool_));

            // Multiply polynomials.
            multiply_poly_poly(encrypted1, coeff_count, coeff_uint64_count, encrypted2, coeff_count, coeff_uint64_count, product_coeff_count, product_coeff_uint64_count, product.get(), pool_);

            // For each coefficient in product, multiply by plain_modulus and divide by coeff_modulus and then modulo by coeff_modulus.
            int plain_modulus_bit_count = plain_modulus_.significant_bit_count();
            int plain_modulus_uint64_count = divide_round_up(plain_modulus_bit_count, bits_per_uint64);
            int intermediate_bit_count = product_coeff_bit_count + plain_modulus_bit_count;
            int intermediate_uint64_count = divide_round_up(intermediate_bit_count, bits_per_uint64);
            Pointer intermediate(allocate_uint(intermediate_uint64_count, pool_));
            Pointer quotient(allocate_uint(intermediate_uint64_count, pool_));
            Pointer productmoded(allocate_poly(product_coeff_count, coeff_uint64_count, pool_));
            for (int coeff_index = 0; coeff_index < product_coeff_count; ++coeff_index)
            {
                const uint64_t *product_coeff = get_poly_coeff(product.get(), coeff_index, product_coeff_uint64_count);
                multiply_uint_uint(product_coeff, product_coeff_uint64_count, plain_modulus_.pointer(), plain_modulus_uint64_count, intermediate_uint64_count, intermediate.get());
                add_uint_uint(intermediate.get(), wide_coeff_modulus_div_two_.pointer(), intermediate_uint64_count, intermediate.get());
                divide_uint_uint_inplace(intermediate.get(), wide_coeff_modulus_.pointer(), intermediate_uint64_count, quotient.get(), pool_);
                modulo_uint_inplace(quotient.get(), intermediate_uint64_count, mod_, pool_);
                uint64_t *productmoded_coeff = get_poly_coeff(productmoded.get(), coeff_index, coeff_uint64_count);
                set_uint_uint(quotient.get(), coeff_uint64_count, productmoded_coeff);
            }

            // Perform polynomial modulo.
            modulo_poly_inplace(productmoded.get(), product_coeff_count, polymod_, mod_, pool_);

            // Copy to destination.
            set_poly_poly(productmoded.get(), coeff_count, coeff_uint64_count, destination);
        }
    }

    void Evaluator::relinearize(const uint64_t *encrypted, uint64_t *destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Clear destatintion.
        set_zero_poly(coeff_count, coeff_uint64_count, destination);

        // Create polynomial to store decomposed polynomial (one at a time).
        Pointer decomp_poly(allocate_poly(coeff_count, coeff_uint64_count, pool_));
        Pointer decomp_eval_poly(allocate_poly(coeff_count, coeff_uint64_count, pool_));
        int shift = 0;
        for (int decomp_index = 0; decomp_index < evaluation_keys_.count(); ++decomp_index)
        {
            // Isolate decomposition_bit_count_ bits for each coefficient.
            for (int coeff_index = 0; coeff_index < coeff_count; ++coeff_index)
            {
                const uint64_t *productmoded_coeff = get_poly_coeff(encrypted, coeff_index, coeff_uint64_count);
                uint64_t *decomp_coeff = get_poly_coeff(decomp_poly.get(), coeff_index, coeff_uint64_count);
                right_shift_uint(productmoded_coeff, shift, coeff_uint64_count, decomp_coeff);
                filter_highbits_uint(decomp_coeff, coeff_uint64_count, decomposition_bit_count_);
            }

            // Multiply decomposed poly by evaluation key and accumulate to result.
            const BigPoly &evaluation_key = evaluation_keys_[decomp_index];
            multiply_poly_poly_polymod_coeffmod(decomp_poly.get(), evaluation_key.pointer(), polymod_, mod_, decomp_eval_poly.get(), pool_);
            add_poly_poly_coeffmod(decomp_eval_poly.get(), destination, coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, destination);

            // Increase shift by decomposition_bit_count_ for next iteration.
            shift += decomposition_bit_count_;
        }
    }

    BigPoly Evaluator::multiply_many(vector<BigPoly> encrypteds)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();

        vector<BigPoly>::size_type original_size = encrypteds.size();

        // Verify parameters.
        if (original_size == 0)
        {
            throw invalid_argument("encrypteds vector must not be empty");
        }
        for (vector<BigPoly>::size_type i = 0; i < original_size; ++i)
        {
            if (encrypteds[i].coeff_count() != coeff_count || encrypteds[i].coeff_bit_count() != coeff_bit_count)
            {
                throw invalid_argument("encrypteds is not valid for encryption parameters");
            }
#ifdef _DEBUG
            if (encrypteds[i].significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypteds[i], coeff_modulus_))
            {
                throw invalid_argument("encrypteds is not valid for encryption parameters");
            }
#endif
        }

        if (original_size == 1)
        {
            return encrypteds[0];
        }

        // Repeatedly multiply and add to the back of the vector until the end is reached
        for (vector<BigPoly>::size_type i = 0; i < encrypteds.size() - 1; i += 2)
        {
            encrypteds.push_back(multiply(encrypteds[i], encrypteds[i+1]));
        }
   
        return encrypteds[encrypteds.size() - 1];
    }

    BigPoly Evaluator::multiply_norelin_many(vector<BigPoly> encrypteds)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();

        vector<BigPoly>::size_type original_size = encrypteds.size();

        // Verify parameters.
        if (original_size == 0)
        {
            throw invalid_argument("encrypteds vector must not be empty");
        }
        for (vector<BigPoly>::size_type i = 0; i < original_size; ++i)
        {
            if (encrypteds[i].coeff_count() != coeff_count || encrypteds[i].coeff_bit_count() != coeff_bit_count)
            {
                throw invalid_argument("encrypteds is not valid for encryption parameters");
            }
#ifdef _DEBUG
            if (encrypteds[i].significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypteds[i], coeff_modulus_))
            {
                throw invalid_argument("encrypteds is not valid for encryption parameters");
            }
#endif
        }

        if (original_size == 1)
        {
            return encrypteds[0];
        }

        // Repeatedly multiply and add to the back of the vector until the end is reached
        for (vector<BigPoly>::size_type i = 0; i < encrypteds.size() - 1; i += 2)
        {
            encrypteds.push_back(multiply_norelin(encrypteds[i], encrypteds[i + 1]));
        }

        return encrypteds[encrypteds.size() - 1];
    }

    void Evaluator::exponentiate(const BigPoly &encrypted, int exponent, BigPoly &destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Verify parameters.
        if (encrypted.coeff_count() != coeff_count || encrypted.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("encrypted is not valid for encryption parameters");
        }
#ifdef _DEBUG
        if (encrypted.significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted, coeff_modulus_))
        {
            throw invalid_argument("encrypted is not valid for encryption parameters");
        }
#endif
        if (exponent < 0)
        {
            throw invalid_argument("exponent must be non-negative");
        }

        if (exponent == 0)
        {
            if (destination.coeff_count() != coeff_count || destination.coeff_bit_count() != coeff_bit_count)
            {
                destination.resize(coeff_count, coeff_bit_count);
            }
            set_uint_uint(coeff_div_plain_modulus_.pointer(), coeff_uint64_count, destination.pointer());
            return;
        }
        if (exponent == 1)
        {
            encrypted.duplicate_to(destination);
            return;
        }

        vector<BigPoly> exp_vector(exponent, encrypted);
        multiply_many(exp_vector, destination);

        // Binary exponentiation
        /*
        if (exponent % 2 == 0)
        {
            exponentiate(multiply(encrypted, encrypted), exponent >> 1, destination);
            return;
        }
        multiply(exponentiate(multiply(encrypted, encrypted), (exponent - 1) >> 1), encrypted, destination);
        */
    }

    void Evaluator::exponentiate_norelin(const BigPoly &encrypted, int exponent, BigPoly &destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Verify parameters.
        if (encrypted.coeff_count() != coeff_count || encrypted.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("encrypted is not valid for encryption parameters");
        }
#ifdef _DEBUG
        if (encrypted.significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted, coeff_modulus_))
        {
            throw invalid_argument("encrypted is not valid for encryption parameters");
        }
#endif
        if (exponent < 0)
        {
            throw invalid_argument("exponent must be non-negative");
        }

        if (exponent == 0)
        {
            if (destination.coeff_count() != coeff_count || destination.coeff_bit_count() != coeff_bit_count)
            {
                destination.resize(coeff_count, coeff_bit_count);
            }
            set_uint_uint(coeff_div_plain_modulus_.pointer(), coeff_uint64_count, destination.pointer());
            return;
        }
        if (exponent == 1)
        {
            encrypted.duplicate_to(destination);
            return;
        }

        vector<BigPoly> exp_vector(exponent, encrypted);
        multiply_norelin_many(exp_vector, destination);

        // Binary exponentiation
        /*
        if (exponent % 2 == 0)
        {
            exponentiate_norelin(multiply_norelin(encrypted, encrypted), exponent >> 1, destination);
            return;
        }
        multiply_norelin(exponentiate_norelin(multiply_norelin(encrypted, encrypted), (exponent - 1) >> 1), encrypted, destination);
        */
    }

    Evaluator::Evaluator(const EncryptionParameters &parms, const EvaluationKeys &evaluation_keys) :
        poly_modulus_(parms.poly_modulus()), coeff_modulus_(parms.coeff_modulus()), plain_modulus_(parms.plain_modulus()),
        decomposition_bit_count_(parms.decomposition_bit_count()), evaluation_keys_(evaluation_keys), mode_(parms.mode())
    {
        // Verify required parameters are non-zero and non-nullptr.
        if (poly_modulus_.is_zero())
        {
            throw invalid_argument("poly_modulus cannot be zero");
        }
        if (coeff_modulus_.is_zero())
        {
            throw invalid_argument("coeff_modulus cannot be zero");
        }
        if (plain_modulus_.is_zero())
        {
            throw invalid_argument("plain_modulus cannot be zero");
        }
        if (decomposition_bit_count_ <= 0)
        {
            throw invalid_argument("decomposition_bit_count must be positive");
        }
        if (evaluation_keys_.count() == 0)
        {
            throw invalid_argument("evaluation_keys cannot be empty");
        }

        // Verify parameters.
        if (plain_modulus_ >= coeff_modulus_)
        {
            throw invalid_argument("plain_modulus must be smaller than coeff_modulus");
        }
        if (!are_poly_coefficients_less_than(poly_modulus_, coeff_modulus_))
        {
            throw invalid_argument("poly_modulus cannot have coefficients larger than coeff_modulus");
        }

        // Resize encryption parameters to consistent size.
        int coeff_count = poly_modulus_.significant_coeff_count();
        int coeff_bit_count = coeff_modulus_.significant_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        if (poly_modulus_.coeff_count() != coeff_count || poly_modulus_.coeff_bit_count() != coeff_bit_count)
        {
            poly_modulus_.resize(coeff_count, coeff_bit_count);
        }
        if (coeff_modulus_.bit_count() != coeff_bit_count)
        {
            coeff_modulus_.resize(coeff_bit_count);
        }
        if (plain_modulus_.bit_count() != coeff_bit_count)
        {
            plain_modulus_.resize(coeff_bit_count);
        }
        if (decomposition_bit_count_ > coeff_bit_count)
        {
            decomposition_bit_count_ = coeff_bit_count;
        }

        // Determine correct number of evaluation keys.
        int evaluation_key_count = 0;
        Pointer evaluation_factor(allocate_uint(coeff_uint64_count, pool_));
        set_uint(1, coeff_uint64_count, evaluation_factor.get());
        while (!is_zero_uint(evaluation_factor.get(), coeff_uint64_count) && is_less_than_uint_uint(evaluation_factor.get(), coeff_modulus_.pointer(), coeff_uint64_count))
        {
            left_shift_uint(evaluation_factor.get(), decomposition_bit_count_, coeff_uint64_count, evaluation_factor.get());
            evaluation_key_count++;
        }

        // Verify evaluation keys.
        if (evaluation_keys_.count() != evaluation_key_count)
        {
            throw invalid_argument("evaluation_keys is not valid for encryption parameters");
        }
        for (int i = 0; i < evaluation_keys_.count(); ++i)
        {
            BigPoly &evaluation_key = evaluation_keys_[i];
            if (evaluation_key.coeff_count() != coeff_count || evaluation_key.coeff_bit_count() != coeff_bit_count ||
                evaluation_key.significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(evaluation_key, coeff_modulus_))
            {
                throw invalid_argument("evaluation_keys is not valid for encryption parameters");
            }
        }

        // Calculate coeff_modulus / plain_modulus.
        coeff_div_plain_modulus_.resize(coeff_bit_count);
        Pointer temp(allocate_uint(coeff_uint64_count, pool_));
        divide_uint_uint(coeff_modulus_.pointer(), plain_modulus_.pointer(), coeff_uint64_count, coeff_div_plain_modulus_.pointer(), temp.get(), pool_);

        // Calculate (plain_modulus + 1) / 2.
        plain_upper_half_threshold_.resize(coeff_bit_count);
        half_round_up_uint(plain_modulus_.pointer(), coeff_uint64_count, plain_upper_half_threshold_.pointer());

        // Calculate coeff_modulus - plain_modulus.
        plain_upper_half_increment_.resize(coeff_bit_count);
        sub_uint_uint(coeff_modulus_.pointer(), plain_modulus_.pointer(), coeff_uint64_count, plain_upper_half_increment_.pointer());

        // Calculate (plain_modulus + 1) / 2 * coeff_div_plain_modulus.
        upper_half_threshold_.resize(coeff_bit_count);
        multiply_truncate_uint_uint(plain_upper_half_threshold_.pointer(), coeff_div_plain_modulus_.pointer(), coeff_uint64_count, upper_half_threshold_.pointer());

        // Calculate upper_half_increment.
        upper_half_increment_.resize(coeff_bit_count);
        multiply_truncate_uint_uint(plain_modulus_.pointer(), coeff_div_plain_modulus_.pointer(), coeff_uint64_count, upper_half_increment_.pointer());
        sub_uint_uint(coeff_modulus_.pointer(), upper_half_increment_.pointer(), coeff_uint64_count, upper_half_increment_.pointer());

        // Widen coeff modulus.
        int product_coeff_bit_count = coeff_bit_count + coeff_bit_count + get_significant_bit_count(static_cast<uint64_t>(coeff_count));
        int plain_modulus_bit_count = plain_modulus_.significant_bit_count();
        int wide_bit_count = product_coeff_bit_count + plain_modulus_bit_count;
        int wide_uint64_count = divide_round_up(wide_bit_count, bits_per_uint64);
        wide_coeff_modulus_.resize(wide_bit_count);
        wide_coeff_modulus_ = coeff_modulus_;

        // Calculate wide_coeff_modulus_ / 2.
        wide_coeff_modulus_div_two_.resize(wide_bit_count);
        right_shift_uint(wide_coeff_modulus_.pointer(), 1, wide_uint64_count, wide_coeff_modulus_div_two_.pointer());

        // Initialize moduli.
        polymod_ = PolyModulus(poly_modulus_.pointer(), coeff_count, coeff_uint64_count);
        if (mode_ == TEST_MODE)
        {
            mod_ = Modulus(plain_modulus_.pointer(), coeff_uint64_count, pool_);
        }
        else
        {
            mod_ = Modulus(coeff_modulus_.pointer(), coeff_uint64_count, pool_);
        }
    }
}
