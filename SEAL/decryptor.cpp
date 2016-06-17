#include <algorithm>
#include <stdexcept>
#include "decryptor.h"
#include "util/common.h"
#include "util/uintcore.h"
#include "util/uintarith.h"
#include "util/polycore.h"
#include "util/polyarith.h"
#include "util/polyarithmod.h"
#include "bigpoly.h"
#include "util/uintarithmod.h"
#include "util/polyextras.h"

using namespace std;
using namespace seal::util;

namespace seal
{
    namespace
    {
        bool are_poly_coefficients_less_than(const BigPoly &poly, const BigUInt &max_coeff)
        {
            return util::are_poly_coefficients_less_than(poly.pointer(), poly.coeff_count(), poly.coeff_uint64_count(), max_coeff.pointer(), max_coeff.uint64_count());
        }
    }

    Decryptor::Decryptor(const EncryptionParameters &parms, const BigPoly &secret_key) :
        poly_modulus_(parms.poly_modulus()), coeff_modulus_(parms.coeff_modulus()), plain_modulus_(parms.plain_modulus()), secret_key_(secret_key), orig_plain_modulus_bit_count_(parms.plain_modulus().significant_bit_count())
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

        if (secret_key_.is_zero())
        {
            throw invalid_argument("secret_key cannot be zero");
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
        if (secret_key_.coeff_count() != coeff_count || secret_key_.coeff_bit_count() != coeff_bit_count ||
            secret_key_.significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(secret_key_, coeff_modulus_))
        {
            throw invalid_argument("secret_key is not valid for encryption parameters");
        }

        // Set the secret_key_array to have size 1 (first power of secret) 
        secret_key_array_.resize(1, coeff_count, coeff_bit_count);
        set_poly_poly(secret_key_.pointer(), coeff_count, coeff_uint64_count, secret_key_array_.pointer(0));

        MemoryPool &pool = *MemoryPool::default_pool();

        // Calculate coeff_modulus / plain_modulus.
        coeff_div_plain_modulus_.resize(coeff_bit_count);
        Pointer temp(allocate_uint(coeff_uint64_count, pool));
        divide_uint_uint(coeff_modulus_.pointer(), plain_modulus_.pointer(), coeff_uint64_count, coeff_div_plain_modulus_.pointer(), temp.get(), pool);

        // Calculate coeff_modulus / plain_modulus / 2.
        coeff_div_plain_modulus_div_two_.resize(coeff_bit_count);
        right_shift_uint(coeff_div_plain_modulus_.pointer(), 1, coeff_uint64_count, coeff_div_plain_modulus_div_two_.pointer());

        // Calculate coeff_modulus / 2.
        upper_half_threshold_.resize(coeff_bit_count);
        half_round_up_uint(coeff_modulus_.pointer(), coeff_uint64_count, upper_half_threshold_.pointer());

        // Calculate upper_half_increment.
        upper_half_increment_.resize(coeff_bit_count);
        multiply_truncate_uint_uint(plain_modulus_.pointer(), coeff_div_plain_modulus_.pointer(), coeff_uint64_count, upper_half_increment_.pointer());
        sub_uint_uint(coeff_modulus_.pointer(), upper_half_increment_.pointer(), coeff_uint64_count, upper_half_increment_.pointer());

        // Initialize moduli.
        polymod_ = PolyModulus(poly_modulus_.pointer(), coeff_count, coeff_uint64_count);
        mod_ = Modulus(coeff_modulus_.pointer(), coeff_uint64_count, pool);  
    }

    void Decryptor::decrypt(const BigPolyArray &encrypted, BigPoly &destination)
    {
        // Extract encryption parameters.
        // Remark: poly_modulus_ has enlarged coefficient size set in constructor         
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Verify parameters.
        if (encrypted.size() < 2 || encrypted.coeff_count() != coeff_count || encrypted.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("encrypted is not valid for encryption parameters");
        }

#ifdef _DEBUG
        for (int i = 0; i < encrypted.size(); ++i)
        {
            if (encrypted[i].significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted[i], coeff_modulus_))
            {
                throw invalid_argument("encrypted is not valid for encryption parameters");
            }
        }
#endif

        // Make sure destination is of right size to perform all computations. At the end we will
        // resize the coefficients to be the size of plain_modulus.
        // Remark: plain_modulus_ has enlarged coefficient size set in constructor
        if (destination.coeff_count() != coeff_count || destination.coeff_bit_count() != coeff_bit_count)
        {
            destination.resize(coeff_count, coeff_bit_count);
        }

        MemoryPool &pool = *MemoryPool::default_pool();

        // Make sure we have enough secret keys computed
        compute_secret_key_array(encrypted.size() - 1);

        /*
        Firstly find c_0 + c_1 *s + ... + c_{count-1} * s^{count-1} mod q
        This is equal to Delta m + v where ||v|| < Delta/2.
        So, add Delta / 2 and now we have something which is Delta * (m + epsilon) where epsilon < 1
        Therefore, we can (integer) divide by Delta and the answer will round down to m.
        */
        // put < (c_1 , c_2, ... , c_{count-1}) , (s,s^2,...,s^{count-1}) > mod q in destination
        dot_product_bigpolyarray_polymod_coeffmod(encrypted.pointer(1), secret_key_array_.pointer(0), encrypted.size() - 1, polymod_, mod_, destination.pointer(), pool);

        // add c_0 mod into destination
        add_poly_poly_coeffmod(destination.pointer(), encrypted[0].pointer(), coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, destination.pointer());

        // For each coefficient, reposition and divide by coeff_div_plain_modulus.
        uint64_t *dest_coeff = destination.pointer();
        Pointer quotient(allocate_uint(coeff_uint64_count, pool));
        Pointer big_alloc(allocate_uint(2 * coeff_uint64_count, pool));
        for (int i = 0; i < coeff_count; ++i)
        {
            // Round to closest level by adding coeff_div_plain_modulus_div_two (mod coeff_modulus).
            add_uint_uint_mod(dest_coeff, coeff_div_plain_modulus_div_two_.pointer(), coeff_modulus_.pointer(), coeff_uint64_count, dest_coeff);

            // Reposition if it is in upper-half of coeff_modulus.
            bool is_upper_half = is_greater_than_or_equal_uint_uint(dest_coeff, upper_half_threshold_.pointer(), coeff_uint64_count);
            if (is_upper_half)
            {
                sub_uint_uint(dest_coeff, upper_half_increment_.pointer(), coeff_uint64_count, dest_coeff);
            }

            // Find closest level.
            divide_uint_uint_inplace(dest_coeff, coeff_div_plain_modulus_.pointer(), coeff_uint64_count, quotient.get(), pool, big_alloc.get());
            set_uint_uint(quotient.get(), coeff_uint64_count, dest_coeff);
            dest_coeff += coeff_uint64_count;
        }

        // Resize the coefficient to the original plain_modulus size
        destination.resize(coeff_count, orig_plain_modulus_bit_count_);
    }

    void Decryptor::compute_secret_key_array(int max_power)
    {
        if (max_power < 1)
        {
            throw invalid_argument("max_power cannot be less than 1");
        }

        int old_count = secret_key_array_.size();
        int new_count = max(max_power, secret_key_array_.size());

        if (old_count == new_count)
        {
            return;
        }

        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = coeff_modulus_.bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Compute powers of secret key until max_power
        secret_key_array_.resize(new_count, coeff_count, coeff_bit_count);

        MemoryPool &pool = *MemoryPool::default_pool();

        int poly_ptr_increment = coeff_count * coeff_uint64_count;
        uint64_t *prev_poly_ptr = secret_key_array_.pointer(old_count - 1);
        uint64_t *next_poly_ptr = prev_poly_ptr + poly_ptr_increment;
        for (int i = old_count; i < new_count; ++i)
        {
            multiply_poly_poly_polymod_coeffmod(prev_poly_ptr, secret_key_.pointer(), polymod_, mod_, next_poly_ptr, pool);
            prev_poly_ptr = next_poly_ptr;
            next_poly_ptr += poly_ptr_increment;
        }
    }
}