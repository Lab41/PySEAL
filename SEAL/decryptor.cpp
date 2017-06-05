#include <algorithm>
#include <stdexcept>
#include "decryptor.h"
#include "util/common.h"
#include "util/uintcore.h"
#include "util/uintarith.h"
#include "util/polycore.h"
#include "util/polyarith.h"
#include "util/polyarithmod.h"
#include "util/polyfftmultmod.h"
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

    Decryptor::Decryptor(const EncryptionParameters &parms, const BigPoly &secret_key, const MemoryPoolHandle &pool) :
        pool_(pool), poly_modulus_(parms.poly_modulus()), coeff_modulus_(parms.coeff_modulus()), plain_modulus_(parms.plain_modulus()), 
        secret_key_(secret_key), orig_plain_modulus_bit_count_(parms.plain_modulus().significant_bit_count()), ntt_tables_(pool_),
        qualifiers_(parms.get_qualifiers())
    {
        // Verify parameters
        if (!qualifiers_.parameters_set)
        {
            throw invalid_argument("encryption parameters are not set correctly");
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

        // Secret key has to have right size.
        if (secret_key_.coeff_count() != coeff_count || secret_key_.coeff_bit_count() != coeff_bit_count ||
            secret_key_.significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(secret_key_, coeff_modulus_))
        {
            throw invalid_argument("secret_key is not valid for encryption parameters");
        }

        // Set the secret_key_array to have size 1 (first power of secret) 
        secret_key_array_.resize(1, coeff_count, coeff_bit_count);
        set_poly_poly(secret_key_.pointer(), coeff_count, coeff_uint64_count, secret_key_array_.pointer(0));

        // Calculate coeff_modulus / plain_modulus.
        coeff_div_plain_modulus_.resize(coeff_bit_count);
        Pointer temp(allocate_uint(coeff_uint64_count, pool_));
        divide_uint_uint(coeff_modulus_.pointer(), plain_modulus_.pointer(), coeff_uint64_count, coeff_div_plain_modulus_.pointer(), temp.get(), pool_);

        // Calculate upper_half_increment.
        upper_half_increment_.resize(coeff_bit_count);
        set_uint_uint(temp.get(), coeff_uint64_count, upper_half_increment_.pointer());

        // Calculate coeff_modulus / plain_modulus / 2.
        coeff_div_plain_modulus_div_two_.resize(coeff_bit_count);
        right_shift_uint(coeff_div_plain_modulus_.pointer(), 1, coeff_uint64_count, coeff_div_plain_modulus_div_two_.pointer());

        // Calculate coeff_modulus / 2.
        upper_half_threshold_.resize(coeff_bit_count);
        half_round_up_uint(coeff_modulus_.pointer(), coeff_uint64_count, upper_half_threshold_.pointer());

        // Initialize moduli.
        polymod_ = PolyModulus(poly_modulus_.pointer(), coeff_count, coeff_uint64_count);
        mod_ = Modulus(coeff_modulus_.pointer(), coeff_uint64_count, pool_);
        
        if (qualifiers_.enable_ntt)
        {
            // Copy over NTT tables (switching to local pool)
            ntt_tables_ = parms.ntt_tables_;
        }
    }

    Decryptor::Decryptor(const Decryptor &copy) : pool_(copy.pool_), poly_modulus_(copy.poly_modulus_), coeff_modulus_(copy.coeff_modulus_),
        plain_modulus_(copy.plain_modulus_), upper_half_threshold_(copy.upper_half_threshold_), upper_half_increment_(copy.upper_half_increment_),
        coeff_div_plain_modulus_(copy.coeff_div_plain_modulus_), coeff_div_plain_modulus_div_two_(copy.coeff_div_plain_modulus_div_two_),
        secret_key_(copy.secret_key_), orig_plain_modulus_bit_count_(copy.orig_plain_modulus_bit_count_), ntt_tables_(copy.ntt_tables_),
        secret_key_array_(copy.secret_key_array_), qualifiers_(copy.qualifiers_)
    {
        int coeff_count = poly_modulus_.significant_coeff_count();
        int coeff_bit_count = coeff_modulus_.significant_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Initialize moduli.
        polymod_ = PolyModulus(poly_modulus_.pointer(), coeff_count, coeff_uint64_count);
        mod_ = Modulus(coeff_modulus_.pointer(), coeff_uint64_count, pool_);
    }

    void Decryptor::decrypt(const BigPolyArray &encrypted, BigPoly &destination)
    {
        // Extract encryption parameters.
        // Remark: poly_modulus_ has enlarged coefficient size set in constructor         
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        int array_poly_uint64_count = coeff_count * coeff_uint64_count;

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

        // Make sure we have enough secret keys computed
        compute_secret_key_array(encrypted.size() - 1);

        /*
        Firstly find c_0 + c_1 *s + ... + c_{count-1} * s^{count-1} mod q
        This is equal to Delta m + v where ||v|| < Delta/2.
        So, add Delta / 2 and now we have something which is Delta * (m + epsilon) where epsilon < 1
        Therefore, we can (integer) divide by Delta and the answer will round down to m.
        */
        // put < (c_1 , c_2, ... , c_{count-1}) , (s,s^2,...,s^{count-1}) > mod q in destination
        if (qualifiers_.enable_ntt)
        {
            // Make a copy of the encrypted BigPolyArray for NTT (except the first polynomial is not needed)
            Pointer encrypted_copy(allocate_poly((encrypted.size() - 1) * coeff_count, coeff_uint64_count, pool_));
            set_poly_poly(encrypted.pointer(1), (encrypted.size() - 1) * coeff_count, coeff_uint64_count, encrypted_copy.get());
            
            // Now do the dot product of encrypted_copy and the secret key array using NTT. The secret key powers are already NTT transformed.
            ntt_dot_product_bigpolyarray_nttbigpolyarray(encrypted_copy.get(), secret_key_array_.pointer(0), encrypted.size() - 1, array_poly_uint64_count, ntt_tables_, destination.pointer(), pool_);
        }
        else if(!qualifiers_.enable_ntt && qualifiers_.enable_nussbaumer)
        {
            nussbaumer_dot_product_bigpolyarray_coeffmod(encrypted.pointer(1), secret_key_array_.pointer(0), encrypted.size() - 1, polymod_, mod_, destination.pointer(), pool_);
        }
        else
        {
            // This branch should never be reached
            throw logic_error("invalid encryption parameters");
        }

        // add c_0 into destination
        add_poly_poly_coeffmod(destination.pointer(), encrypted[0].pointer(), coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, destination.pointer());

        // For each coefficient, reposition and divide by coeff_div_plain_modulus.
        uint64_t *dest_coeff = destination.pointer();
        Pointer quotient(allocate_uint(coeff_uint64_count, pool_));
        Pointer big_alloc(allocate_uint(2 * coeff_uint64_count, pool_));
        for (int i = 0; i < coeff_count; i++)
        {
            // Round to closest level by adding coeff_div_plain_modulus_div_two (mod coeff_modulus).
            // This is necessary, as a small negative noise coefficient and message zero can take the coefficient to close to coeff_modulus.
            // Adding coeff_div_plain_modulus_div_two at this fixes the problem.
            add_uint_uint_mod(dest_coeff, coeff_div_plain_modulus_div_two_.pointer(), coeff_modulus_.pointer(), coeff_uint64_count, dest_coeff);

            // Reposition if it is in upper-half of coeff_modulus.
            bool is_upper_half = is_greater_than_or_equal_uint_uint(dest_coeff, upper_half_threshold_.pointer(), coeff_uint64_count);
            if (is_upper_half)
            {
                sub_uint_uint(dest_coeff, upper_half_increment_.pointer(), coeff_uint64_count, dest_coeff);
            }

            // Find closest level.
            divide_uint_uint_inplace(dest_coeff, coeff_div_plain_modulus_.pointer(), coeff_uint64_count, quotient.get(), pool_, big_alloc.get());
            set_uint_uint(quotient.get(), coeff_uint64_count, dest_coeff);
            dest_coeff += coeff_uint64_count;
        }

        // Resize the coefficient to the original plain_modulus size
        destination.resize(coeff_count, orig_plain_modulus_bit_count_);
    }

    void Decryptor::compute_secret_key_array(int max_power)
    {
        //// This check is not needed. The function will never be called with max_power < 1.
        //if (max_power < 1)
        //{
        //    throw invalid_argument("max_power cannot be less than 1");
        //}

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

        int poly_ptr_increment = coeff_count * coeff_uint64_count;
        uint64_t *prev_poly_ptr = secret_key_array_.pointer(old_count - 1);
        uint64_t *next_poly_ptr = prev_poly_ptr + poly_ptr_increment;
        
        if (qualifiers_.enable_ntt)
        {
            // Since all of the key powers in secret_key_array_ are already NTT transformed, to get the next one 
            // we simply need to compute a dyadic product of the last one with the first one [which is equal to NTT(secret_key_)].
            for (int i = old_count; i < new_count; i++)
            {
                dyadic_product_coeffmod(prev_poly_ptr, secret_key_array_.pointer(0), coeff_count, mod_, next_poly_ptr, pool_);
                prev_poly_ptr = next_poly_ptr;
                next_poly_ptr += poly_ptr_increment;
            }
        }
        else if(!qualifiers_.enable_ntt && qualifiers_.enable_nussbaumer)
        {
            // Non-NTT path involves computing powers of the secret key.
            for (int i = old_count; i < new_count; i++)
            {
                nussbaumer_multiply_poly_poly_coeffmod(prev_poly_ptr, secret_key_.pointer(), polymod_.coeff_count_power_of_two(), mod_, next_poly_ptr, pool_);
                prev_poly_ptr = next_poly_ptr;
                next_poly_ptr += poly_ptr_increment;
            }
        }
        else
        {
            // This branch should never be reached
            throw logic_error("invalid encryption parameters");
        }
    }

    void Decryptor::inherent_noise(const BigPolyArray &encrypted, BigUInt &destination)
    {
        // Extract encryption parameters.
        // Remark: poly_modulus_ has enlarged coefficient size set in constructor         
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        int array_poly_uint64_count = coeff_count * coeff_uint64_count;

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
        // Make sure destination is of right size.
        if (destination.bit_count() != coeff_bit_count)
        {
            destination.resize(coeff_bit_count);
        }

        // Now need to compute c(s) - Delta*m (mod q)

        // Make sure we have enough secret keys computed
        compute_secret_key_array(encrypted.size() - 1);

        Pointer noise_poly(allocate_poly(coeff_count, coeff_uint64_count, pool_));
        Pointer plain_poly(allocate_poly(coeff_count, coeff_uint64_count, pool_));

        /*
        Firstly find c_0 + c_1 *s + ... + c_{count-1} * s^{count-1} mod q
        This is equal to Delta m + v where ||v|| < Delta/2.
        */
        // put < (c_1 , c_2, ... , c_{count-1}) , (s,s^2,...,s^{count-1}) > mod q in destination_poly
        if (qualifiers_.enable_ntt)
        {
            // Make a copy of the encrypted BigPolyArray for NTT (except the first polynomial is not needed)
            Pointer encrypted_copy(allocate_poly((encrypted.size() - 1) * coeff_count, coeff_uint64_count, pool_));
            set_poly_poly(encrypted.pointer(1), (encrypted.size() - 1) * coeff_count, coeff_uint64_count, encrypted_copy.get());

            // Now do the dot product of encrypted_copy and the secret key array using NTT. The secret key powers are already NTT transformed.
            ntt_dot_product_bigpolyarray_nttbigpolyarray(encrypted_copy.get(), secret_key_array_.pointer(0), encrypted.size() - 1, array_poly_uint64_count, ntt_tables_, noise_poly.get(), pool_);
        }
        else if(!qualifiers_.enable_ntt && qualifiers_.enable_nussbaumer)
        {
            nussbaumer_dot_product_bigpolyarray_coeffmod(encrypted.pointer(1), secret_key_array_.pointer(0), encrypted.size() - 1, polymod_, mod_, noise_poly.get(), pool_);
        }
        else
        {
            // This branch should never be reached
            throw logic_error("invalid encryption parameters");
        }

        // add c_0 into noise_poly
        add_poly_poly_coeffmod(noise_poly.get(), encrypted[0].pointer(), coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, noise_poly.get());

        // Copy noise_poly to plain_poly
        set_poly_poly(noise_poly.get(), coeff_count, coeff_uint64_count, plain_poly.get());

        // We need to find the plaintext first, so finish decryption (see Decryptor::decrypt).
        // For each coefficient, reposition and divide by coeff_div_plain_modulus.
        uint64_t *plain_coeff = plain_poly.get();
        Pointer quotient(allocate_uint(coeff_uint64_count, pool_));
        Pointer big_alloc(allocate_uint(2 * coeff_uint64_count, pool_));
        for (int i = 0; i < coeff_count; i++)
        {
            // Round to closest level by adding coeff_div_plain_modulus_div_two (mod coeff_modulus).
            add_uint_uint_mod(plain_coeff, coeff_div_plain_modulus_div_two_.pointer(), coeff_modulus_.pointer(), coeff_uint64_count, plain_coeff);

            // Reposition if it is in upper-half of coeff_modulus.
            bool is_upper_half = is_greater_than_or_equal_uint_uint(plain_coeff, upper_half_threshold_.pointer(), coeff_uint64_count);
            if (is_upper_half)
            {
                sub_uint_uint(plain_coeff, upper_half_increment_.pointer(), coeff_uint64_count, plain_coeff);
            }

            // Find closest level.
            divide_uint_uint_inplace(plain_coeff, coeff_div_plain_modulus_.pointer(), coeff_uint64_count, quotient.get(), pool_, big_alloc.get());

            // Now perform preencrypt correction.
            multiply_truncate_uint_uint(quotient.get(), coeff_div_plain_modulus_.pointer(), coeff_uint64_count, plain_coeff);
            if (is_upper_half)
            {
                add_uint_uint(plain_coeff, upper_half_increment_.pointer(), coeff_uint64_count, plain_coeff);
            }

            plain_coeff += coeff_uint64_count;
        }

        // Next subtract from current noise_poly the plain_poly. Inherent noise (poly) is this difference mod coeff_modulus.
        sub_poly_poly_coeffmod(noise_poly.get(), plain_poly.get(), coeff_count, mod_.get(), coeff_uint64_count, noise_poly.get());

        // Return the infinity norm of noise_poly
        poly_infty_norm_coeffmod(noise_poly.get(), coeff_count, coeff_uint64_count, mod_, destination.pointer(), pool_);
    }

    int Decryptor::invariant_noise_budget(const BigPolyArray &encrypted)
    {
        // Extract encryption parameters.
        // Remark: poly_modulus_ has enlarged coefficient size set in constructor         
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        int array_poly_uint64_count = coeff_count * coeff_uint64_count;

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
        // Make sure destination is of right size.
        BigUInt destination;
        destination.resize(coeff_bit_count);
        // Now need to compute c(s) - Delta*m (mod q)

        // Make sure we have enough secret keys computed
        compute_secret_key_array(encrypted.size() - 1);

        Pointer noise_poly(allocate_poly(coeff_count, coeff_uint64_count, pool_));
        Pointer plain_poly(allocate_poly(coeff_count, coeff_uint64_count, pool_));
        Pointer plain_poly_copy(allocate_poly(coeff_count, coeff_uint64_count, pool_));
        Pointer result_poly(allocate_poly(coeff_count, coeff_uint64_count, pool_));

        /*
        Firstly find c_0 + c_1 *s + ... + c_{count-1} * s^{count-1} mod q
        This is equal to Delta m + v where ||v|| < Delta/2.
        */
        // put < (c_1 , c_2, ... , c_{count-1}) , (s,s^2,...,s^{count-1}) > mod q in destination_poly
        if (qualifiers_.enable_ntt)
        {
            // Make a copy of the encrypted BigPolyArray for NTT (except the first polynomial is not needed)
            Pointer encrypted_copy(allocate_poly((encrypted.size() - 1) * coeff_count, coeff_uint64_count, pool_));
            set_poly_poly(encrypted.pointer(1), (encrypted.size() - 1) * coeff_count, coeff_uint64_count, encrypted_copy.get());

            // Now do the dot product of encrypted_copy and the secret key array using NTT. The secret key powers are already NTT transformed.
            ntt_dot_product_bigpolyarray_nttbigpolyarray(encrypted_copy.get(), secret_key_array_.pointer(0), encrypted.size() - 1, array_poly_uint64_count, ntt_tables_, noise_poly.get(), pool_);
        }
        else if (!qualifiers_.enable_ntt && qualifiers_.enable_nussbaumer)
        {
            nussbaumer_dot_product_bigpolyarray_coeffmod(encrypted.pointer(1), secret_key_array_.pointer(0), encrypted.size() - 1, polymod_, mod_, noise_poly.get(), pool_);
        }
        else
        {
            // This branch should never be reached
            throw logic_error("invalid encryption parameters");
        }

        // add c_0 into noise_poly
        add_poly_poly_coeffmod(noise_poly.get(), encrypted[0].pointer(), coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, noise_poly.get());

        // Multiply by plain_modulus_ and reduce mod coeff_modulus_ to get coeff_modulus_*noise
        multiply_poly_scalar_coeffmod(noise_poly.get(), coeff_count, plain_modulus_.pointer(), mod_, noise_poly.get(), pool_);

        // Next we compute the infinity norm mod coeff_modulus_
        poly_infty_norm_coeffmod(noise_poly.get(), coeff_count, coeff_uint64_count, mod_, destination.pointer(), pool_);

        // The -1 accounts for scaling the invariant noise by 2 
        return max(0, mod_.significant_bit_count() - destination.significant_bit_count() - 1); 
    }
}
