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

using namespace std;
using namespace seal::util;

namespace seal
{
    Decryptor::Decryptor(const SEALContext &context, const SecretKey &secret_key, const MemoryPoolHandle &pool) :
        pool_(pool), parms_(context.get_parms()), 
        ntt_tables_(pool_),
        qualifiers_(context.get_qualifiers())
    {
        // Verify parameters
        if (!qualifiers_.parameters_set)
        {
            throw invalid_argument("encryption parameters are not set correctly");
        }

        // Verify parameters.
        if (secret_key.hash_block_ != parms_.get_hash_block())
        {
            throw invalid_argument("secret key is not valid for encryption parameters");
        }

        int coeff_count = parms_.poly_modulus().coeff_count();
        int poly_coeff_uint64_count = parms_.poly_modulus().coeff_uint64_count();
        int coeff_bit_count = parms_.coeff_modulus().bit_count();
        int coeff_uint64_count = parms_.coeff_modulus().uint64_count();

        // Allocate secret_key_ and copy over value
        secret_key_ = allocate_poly(coeff_count, coeff_uint64_count, pool_);
        set_poly_poly(secret_key.get_poly().pointer(), coeff_count, coeff_uint64_count, secret_key_.get());

        // Set the secret_key_array to have size 1 (first power of secret) 
        secret_key_array_.resize(1, coeff_count, coeff_bit_count);
        set_poly_poly(secret_key_.get(), coeff_count, coeff_uint64_count, secret_key_array_.pointer(0));

        // Calculate coeff_modulus / plain_modulus.
        coeff_div_plain_modulus_ = allocate_uint(coeff_uint64_count, pool_);
        ConstPointer wide_plain_modulus(duplicate_uint_if_needed(parms_.plain_modulus().pointer(), parms_.plain_modulus().uint64_count(), coeff_uint64_count, false, pool_));
        Pointer temp(allocate_uint(coeff_uint64_count, pool_));
        divide_uint_uint(parms_.coeff_modulus().pointer(), wide_plain_modulus.get(), coeff_uint64_count, coeff_div_plain_modulus_.get(), temp.get(), pool_);

        // Calculate upper_half_increment.
        upper_half_increment_ = allocate_uint(coeff_uint64_count, pool_);
        set_uint_uint(temp.get(), coeff_uint64_count, upper_half_increment_.get());

        // Calculate coeff_modulus / plain_modulus / 2.
        coeff_div_plain_modulus_div_two_ = allocate_uint(coeff_uint64_count, pool_);
        right_shift_uint(coeff_div_plain_modulus_.get(), 1, coeff_uint64_count, coeff_div_plain_modulus_div_two_.get());

        // Calculate coeff_modulus / 2.
        upper_half_threshold_ = allocate_uint(coeff_uint64_count, pool_);
        half_round_up_uint(parms_.coeff_modulus().pointer(), coeff_uint64_count, upper_half_threshold_.get());

        // Initialize moduli.
        polymod_ = PolyModulus(parms_.poly_modulus().pointer(), coeff_count, poly_coeff_uint64_count);
        mod_ = Modulus(parms_.coeff_modulus().pointer(), coeff_uint64_count, pool_);
        
        if (qualifiers_.enable_ntt)
        {
            // Copy over NTT tables (switching to local pool)
            ntt_tables_ = context.ntt_tables_;
        }
    }

    Decryptor::Decryptor(const Decryptor &copy) :
        pool_(copy.pool_), parms_(copy.parms_), 
        ntt_tables_(copy.ntt_tables_),
        secret_key_array_(copy.secret_key_array_),
        qualifiers_(copy.qualifiers_)
    {
        int coeff_count = parms_.poly_modulus().coeff_count();
        int poly_coeff_uint64_count = parms_.poly_modulus().coeff_uint64_count();
        int coeff_bit_count = parms_.coeff_modulus().bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Allocate space and copy over values
        upper_half_threshold_ = allocate_uint(coeff_uint64_count, pool_);
        set_uint_uint(copy.upper_half_threshold_.get(), coeff_uint64_count, upper_half_threshold_.get());

        upper_half_increment_ = allocate_uint(coeff_uint64_count, pool_);
        set_uint_uint(copy.upper_half_increment_.get(), coeff_uint64_count, upper_half_increment_.get());

        coeff_div_plain_modulus_ = allocate_uint(coeff_uint64_count, pool_);
        set_uint_uint(copy.coeff_div_plain_modulus_.get(), coeff_uint64_count, coeff_div_plain_modulus_.get());

        coeff_div_plain_modulus_div_two_ = allocate_uint(coeff_uint64_count, pool_);
        set_uint_uint(copy.coeff_div_plain_modulus_div_two_.get(), coeff_uint64_count, coeff_div_plain_modulus_div_two_.get());

        secret_key_ = allocate_poly(coeff_count, coeff_uint64_count, pool_);
        set_poly_poly(copy.secret_key_.get(), coeff_count, coeff_uint64_count, secret_key_.get());

        // Initialize moduli.
        polymod_ = PolyModulus(parms_.poly_modulus().pointer(), coeff_count, poly_coeff_uint64_count);
        mod_ = Modulus(parms_.coeff_modulus().pointer(), coeff_uint64_count, pool_);
    }

    void Decryptor::decrypt(const Ciphertext &encrypted, Plaintext &destination)
    {
        int coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_bit_count = parms_.coeff_modulus().bit_count();
        int coeff_uint64_count = parms_.coeff_modulus().uint64_count();
        int array_poly_uint64_count = coeff_count * coeff_uint64_count;

        const BigPolyArray &encrypted_array = encrypted.get_array();
        BigPoly &destination_poly = destination.get_poly();

        // Verify parameters.
        if (encrypted.hash_block_ != parms_.get_hash_block())
        {
            throw invalid_argument("encrypted is not valid for encryption parameters");
        }

        // Make sure destination is of right size to perform all computations. At the end 
        // we will resize the coefficients to be the size of plain_modulus.
        if (destination_poly.coeff_count() != coeff_count || destination_poly.coeff_bit_count() != coeff_bit_count)
        {
            destination_poly.resize(coeff_count, coeff_bit_count);
        }

        // Make sure we have enough secret keys computed
        compute_secret_key_array(encrypted_array.size() - 1);

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
            Pointer encrypted_copy(allocate_poly((encrypted_array.size() - 1) * coeff_count, coeff_uint64_count, pool_));
            set_poly_poly(encrypted_array.pointer(1), (encrypted_array.size() - 1) * coeff_count, coeff_uint64_count, encrypted_copy.get());
            
            // Now do the dot product of encrypted_copy and the secret key array using NTT. The secret key powers are already NTT transformed.
            ntt_dot_product_bigpolyarray_nttbigpolyarray(encrypted_copy.get(), secret_key_array_.pointer(0), encrypted_array.size() - 1, array_poly_uint64_count, ntt_tables_, destination_poly.pointer(), pool_);
        }
        else if(!qualifiers_.enable_ntt && qualifiers_.enable_fft)
        {
            nussbaumer_dot_product_bigpolyarray_coeffmod(encrypted_array.pointer(1), secret_key_array_.pointer(0), encrypted_array.size() - 1, polymod_, mod_, destination_poly.pointer(), pool_);
        }
        else
        {
            // This branch should never be reached
            throw logic_error("invalid encryption parameters");
        }

        // add c_0 into destination
        add_poly_poly_coeffmod(destination_poly.pointer(), encrypted_array.pointer(0), coeff_count, parms_.coeff_modulus().pointer(), coeff_uint64_count, destination_poly.pointer());

        // For each coefficient, reposition and divide by coeff_div_plain_modulus.
        uint64_t *dest_coeff = destination_poly.pointer();
        Pointer quotient(allocate_uint(coeff_uint64_count, pool_));
        for (int i = 0; i < coeff_count; i++)
        {
            // Round to closest level by adding coeff_div_plain_modulus_div_two_ (mod coeff_modulus).
            // This is necessary, as a small negative noise coefficient and message zero can take the coefficient to close to coeff_modulus.
            // Adding coeff_div_plain_modulus_div_two_ at this fixes the problem.
            add_uint_uint_mod(dest_coeff, coeff_div_plain_modulus_div_two_.get(), parms_.coeff_modulus().pointer(), coeff_uint64_count, dest_coeff);

            // Reposition if it is in upper-half of coeff_modulus.
            bool is_upper_half = is_greater_than_or_equal_uint_uint(dest_coeff, upper_half_threshold_.get(), coeff_uint64_count);
            if (is_upper_half)
            {
                sub_uint_uint(dest_coeff, upper_half_increment_.get(), coeff_uint64_count, dest_coeff);
            }

            // Find closest level.
            divide_uint_uint_inplace(dest_coeff, coeff_div_plain_modulus_.get(), coeff_uint64_count, quotient.get(), pool_);
            set_uint_uint(quotient.get(), coeff_uint64_count, dest_coeff);
            dest_coeff += coeff_uint64_count;
        }

        // Resize the coefficient to the original plain_modulus size
        destination_poly.resize(coeff_count, parms_.plain_modulus().bit_count());
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

        int coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_bit_count = parms_.coeff_modulus().bit_count();
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
        else if(!qualifiers_.enable_ntt && qualifiers_.enable_fft)
        {
            // Non-NTT path involves computing powers of the secret key.
            for (int i = old_count; i < new_count; i++)
            {
                nussbaumer_multiply_poly_poly_coeffmod(prev_poly_ptr, secret_key_.get(), polymod_.coeff_count_power_of_two(), mod_, next_poly_ptr, pool_);
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

    int Decryptor::invariant_noise_budget(const Ciphertext &encrypted)
    {
        int coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_uint64_count = parms_.coeff_modulus().uint64_count();
        int array_poly_uint64_count = coeff_count * coeff_uint64_count;

        const BigPolyArray &encrypted_array = encrypted.get_array();

        // Verify parameters.
        if (encrypted.hash_block_ != parms_.get_hash_block())
        {
            throw invalid_argument("encrypted is not valid for encryption parameters");
        }

        // Make sure destination is of right size.
        Pointer destination(allocate_uint(coeff_uint64_count, pool_));

        // Now need to compute c(s) - Delta*m (mod q)

        // Make sure we have enough secret keys computed
        compute_secret_key_array(encrypted_array.size() - 1);

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
            Pointer encrypted_copy(allocate_poly((encrypted_array.size() - 1) * coeff_count, coeff_uint64_count, pool_));
            set_poly_poly(encrypted_array.pointer(1), (encrypted_array.size() - 1) * coeff_count, coeff_uint64_count, encrypted_copy.get());

            // Now do the dot product of encrypted_copy and the secret key array using NTT. The secret key powers are already NTT transformed.
            ntt_dot_product_bigpolyarray_nttbigpolyarray(encrypted_copy.get(), secret_key_array_.pointer(0), encrypted_array.size() - 1, array_poly_uint64_count, ntt_tables_, noise_poly.get(), pool_);
        }
        else if (!qualifiers_.enable_ntt && qualifiers_.enable_fft)
        {
            nussbaumer_dot_product_bigpolyarray_coeffmod(encrypted_array.pointer(1), secret_key_array_.pointer(0), encrypted_array.size() - 1, polymod_, mod_, noise_poly.get(), pool_);
        }
        else
        {
            // This branch should never be reached
            throw logic_error("invalid encryption parameters");
        }

        // add c_0 into noise_poly
        add_poly_poly_coeffmod(noise_poly.get(), encrypted_array.pointer(0), coeff_count, parms_.coeff_modulus().pointer(), coeff_uint64_count, noise_poly.get());

        // Multiply by parms_.plain_modulus() and reduce mod parms_.coeff_modulus() to get parms_.coeff_modulus()*noise
        ConstPointer wide_plain_modulus(duplicate_uint_if_needed(parms_.plain_modulus().pointer(), parms_.plain_modulus().uint64_count(), parms_.coeff_modulus().uint64_count(), false, pool_));
        multiply_poly_scalar_coeffmod(noise_poly.get(), coeff_count, wide_plain_modulus.get(), mod_, noise_poly.get(), pool_);

        // Next we compute the infinity norm mod parms_.coeff_modulus()
        poly_infty_norm_coeffmod(noise_poly.get(), coeff_count, coeff_uint64_count, mod_, destination.get(), pool_);

        // The -1 accounts for scaling the invariant noise by 2 
        return max(0, mod_.significant_bit_count() - get_significant_bit_count_uint(destination.get(), coeff_uint64_count) - 1); 
    }
}
