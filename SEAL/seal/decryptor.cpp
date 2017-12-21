#include <algorithm>
#include <stdexcept>
#include "seal/decryptor.h"
#include "seal/util/common.h"
#include "seal/util/uintcore.h"
#include "seal/util/uintarith.h"
#include "seal/util/uintarithmod.h"
#include "seal/util/uintarithsmallmod.h"
#include "seal/util/polycore.h"
#include "seal/util/polyarithmod.h"
#include "seal/util/polyarithsmallmod.h"
#include "seal/util/polyfftmultsmallmod.h"

using namespace std;
using namespace seal::util;

namespace seal
{
    Decryptor::Decryptor(const SEALContext &context, const SecretKey &secret_key, const MemoryPoolHandle &pool) :
        pool_(pool), parms_(context.parms()), qualifiers_(context.qualifiers()), base_converter_(context.base_converter_)
    {
        // Verify parameters
        if (!qualifiers_.parameters_set)
        {
            throw invalid_argument("encryption parameters are not set correctly");
        }
        if (secret_key.hash_block_ != parms_.hash_block())
        {
            throw invalid_argument("secret key is not valid for encryption parameters");
        }
        if (!pool)
        {
            throw invalid_argument("pool is uninitialized");
        }
        
        int coeff_count = parms_.poly_modulus().coeff_count();
        int poly_coeff_uint64_count = parms_.poly_modulus().coeff_uint64_count();
        int coeff_mod_count = base_converter_.coeff_base_mod_count();

        // Set SmallNTTTables
        small_ntt_tables_.resize(coeff_mod_count, pool_);
        small_ntt_tables_ = context.small_ntt_tables_;

        // Populate coeff products array for compose functions (used in noise budget)
        coeff_products_array_ = allocate_uint(coeff_mod_count * coeff_mod_count, pool_);
        Pointer tmp_coeff(allocate_uint(coeff_mod_count, pool_));
        set_zero_uint(coeff_mod_count * coeff_mod_count, coeff_products_array_.get());

        for (int i = 0; i < coeff_mod_count; i++)
        {
            coeff_products_array_[i * coeff_mod_count] = 1;
            for (int j = 0; j < coeff_mod_count; j++)
            {
                if (i != j)
                {
                    multiply_uint_uint64(coeff_products_array_.get() + (i * coeff_mod_count), coeff_mod_count, parms_.coeff_modulus()[j].value(), coeff_mod_count, tmp_coeff.get());
                    set_uint_uint(tmp_coeff.get(), coeff_mod_count, coeff_products_array_.get() + (i * coeff_mod_count));
                }
            }
        }

        // Allocate secret_key_ and copy over value
        secret_key_ = allocate_poly(coeff_count, coeff_mod_count, pool_);
        set_poly_poly(secret_key.data().pointer(), coeff_count, coeff_mod_count, secret_key_.get());

        // Set the secret_key_array to have size 1 (first power of secret) 
        secret_key_array_ = allocate_poly(coeff_count, coeff_mod_count, pool_);
        set_poly_poly(secret_key_.get(), coeff_count, coeff_mod_count, secret_key_array_.get());
        secret_key_array_size_ = 1;

        // Set the big coeff modulus for noise computation
        product_modulus_ = allocate_uint(coeff_mod_count, pool_);
        set_uint_uint(context.total_coeff_modulus().pointer(), coeff_mod_count, product_modulus_.get());

        // Initialize moduli.
        mod_ = Modulus(product_modulus_.get(), coeff_mod_count);
        polymod_ = PolyModulus(parms_.poly_modulus().pointer(), coeff_count, poly_coeff_uint64_count);
    }

    Decryptor::Decryptor(const Decryptor &copy) :
        pool_(copy.pool_), parms_(copy.parms_), qualifiers_(copy.qualifiers_),
        base_converter_(copy.base_converter_), 
        small_ntt_tables_(copy.small_ntt_tables_),
        secret_key_array_size_(copy.secret_key_array_size_)
    {
        int coeff_count = parms_.poly_modulus().coeff_count();
        int poly_coeff_uint64_count = parms_.poly_modulus().coeff_uint64_count();
        int coeff_mod_count = base_converter_.coeff_base_mod_count();

        // Populate coeff products array for compose functions (used in noise budget)
        coeff_products_array_ = allocate_uint(coeff_mod_count * coeff_mod_count, pool_);
        set_poly_poly(copy.coeff_products_array_.get(), coeff_mod_count, coeff_mod_count, coeff_products_array_.get());

        // Allocate secret_key_ and copy over value
        secret_key_ = allocate_poly(coeff_count, coeff_mod_count, pool_);
        set_poly_poly(copy.secret_key_.get(), coeff_count, coeff_mod_count, secret_key_.get());

        // Allocate secret_key_array_ and copy over value
        secret_key_array_ = allocate_poly(secret_key_array_size_ * coeff_count, coeff_mod_count, pool_);
        set_poly_poly(copy.secret_key_array_.get(), secret_key_array_size_ * coeff_count, coeff_mod_count, secret_key_array_.get());

        // Set the big coeff modulus for noise computation
        product_modulus_ = allocate_uint(coeff_mod_count, pool_);
        set_uint_uint(copy.product_modulus_.get(), coeff_mod_count, product_modulus_.get());

        // Initialize moduli.
        mod_ = Modulus(product_modulus_.get(), coeff_mod_count);
        polymod_ = PolyModulus(parms_.poly_modulus().pointer(), coeff_count, poly_coeff_uint64_count);
    }

    void Decryptor::decrypt(const Ciphertext &encrypted, Plaintext &destination, const MemoryPoolHandle &pool)
    {
        int coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_mod_count = base_converter_.coeff_base_mod_count();
        int array_poly_uint64_count = coeff_count * coeff_mod_count;
        int encrypted_size = encrypted.size();
        
        // The number of uint64 count for plain_modulus and gamma together
        int plain_gamma_uint64_count = 2;

        // Verify parameters.
        if (encrypted.hash_block_ != parms_.hash_block())
        {
            throw invalid_argument("encrypted is not valid for encryption parameters");
        }
        if (!pool)
        {
            throw invalid_argument("pool is uninitialized");
        }

        // Allocate a full size destination to write to
        Pointer wide_destination(allocate_uint(coeff_count, pool));

        // Make sure we have enough secret key powers computed
        compute_secret_key_array(encrypted_size - 1);

        /*
        Firstly find c_0 + c_1 *s + ... + c_{count-1} * s^{count-1} mod q
        This is equal to Delta m + v where ||v|| < Delta/2.
        So, add Delta / 2 and now we have something which is Delta * (m + epsilon) where epsilon < 1
        Therefore, we can (integer) divide by Delta and the answer will round down to m.
        */

        // Make a temp destination for all the arithmetic mod qi before calling FastBConverse
        Pointer tmp_dest_modq(allocate_zero_poly(coeff_count, coeff_mod_count, pool));

        // put < (c_1 , c_2, ... , c_{count-1}) , (s,s^2,...,s^{count-1}) > mod q in destination

        // Now do the dot product of encrypted_copy and the secret key array using NTT. The secret key powers are already NTT transformed.
        Pointer copy_operand1(allocate_uint(coeff_count, pool));
        for (int i = 0; i < coeff_mod_count; i++)
        {
            // Initialize pointers for multiplication
            const uint64_t *current_array1 = encrypted.pointer(1) + (i * coeff_count);
            const uint64_t *current_array2 = secret_key_array_.get() + (i * coeff_count);

            for (int j = 0; j < encrypted_size - 1; j++)
            {
                // Perform the dyadic product. 
                set_uint_uint(current_array1, coeff_count, copy_operand1.get());

                // Lazy reduction
                ntt_negacyclic_harvey_lazy(copy_operand1.get(), small_ntt_tables_[i]);

                dyadic_product_coeffmod(copy_operand1.get(), current_array2, coeff_count, small_ntt_tables_[i].modulus(), copy_operand1.get());
                add_poly_poly_coeffmod(tmp_dest_modq.get() + (i * coeff_count), copy_operand1.get(), coeff_count, small_ntt_tables_[i].modulus(), 
                    tmp_dest_modq.get() + (i * coeff_count));

                current_array1 += array_poly_uint64_count;
                current_array2 += array_poly_uint64_count;
            }

            // Perform inverse NTT
            inverse_ntt_negacyclic_harvey(tmp_dest_modq.get() + (i * coeff_count), small_ntt_tables_[i]);
        }

        // add c_0 into destination
        for (int i = 0; i < coeff_mod_count; i++)
        {
            //add_poly_poly_coeffmod(tmp_dest_modq.get() + (i * coeff_count), encrypted.pointer() + (i * coeff_count), 
            //    coeff_count, coeff_modulus_[i], tmp_dest_modq.get() + (i * coeff_count));

            // Lazy reduction
            for (int j = 0; j < coeff_count; j++)
            {
                tmp_dest_modq[j + (i * coeff_count)] += encrypted[j + (i * coeff_count)];
            }

            // Compute |gamma * plain|qi * ct(s)
            multiply_poly_scalar_coeffmod(tmp_dest_modq.get() + (i * coeff_count), coeff_count, 
                base_converter_.get_plain_gamma_product()[i], parms_.coeff_modulus()[i], tmp_dest_modq.get() + (i * coeff_count));
        }
        
        // Make another temp destination to get the poly in mod {gamma U plain_modulus}
        Pointer tmp_dest_plain_gamma(allocate_poly(coeff_count, plain_gamma_uint64_count, pool));

        // Compute FastBConvert from q to {gamma, plain_modulus}
        base_converter_.fastbconv_plain_gamma(tmp_dest_modq.get(), tmp_dest_plain_gamma.get(), pool);
        
        // Compute result multiply by coeff_modulus inverse in mod {gamma U plain_modulus}
        for (int i = 0; i < plain_gamma_uint64_count; i++)
        {
            multiply_poly_scalar_coeffmod(tmp_dest_plain_gamma.get() + (i * coeff_count), coeff_count, 
                base_converter_.get_neg_inv_coeff()[i], base_converter_.get_plain_gamma_array()[i], tmp_dest_plain_gamma.get() + (i * coeff_count));
        }

        // First correct the values which are larger than floor(gamma/2)
        uint64_t gamma_div_2 = base_converter_.get_plain_gamma_array()[1].value() >> 1;

        // Now compute the subtraction to remove error and perform final multiplication by gamma inverse mod plain_modulus
        for (int i = 0; i < coeff_count; i++)
        {
            // Need correction beacuse of center mod
            if (tmp_dest_plain_gamma[i + coeff_count] > gamma_div_2)
            {
                // Compute -(gamma - a) instead of (a - gamma)
                tmp_dest_plain_gamma[i + coeff_count] = base_converter_.get_plain_gamma_array()[1].value() - tmp_dest_plain_gamma[i + coeff_count];
                tmp_dest_plain_gamma[i + coeff_count] %= base_converter_.get_plain_gamma_array()[0].value();
                wide_destination[i] = add_uint_uint_mod(tmp_dest_plain_gamma[i], tmp_dest_plain_gamma[i + coeff_count], 
                    base_converter_.get_plain_gamma_array()[0]);
            }
            // No correction needed
            else
            {
                tmp_dest_plain_gamma[i + coeff_count] %= base_converter_.get_plain_gamma_array()[0].value();
                wide_destination[i] = sub_uint_uint_mod(tmp_dest_plain_gamma[i], tmp_dest_plain_gamma[i + coeff_count], 
                    base_converter_.get_plain_gamma_array()[0]);
            }
        }

        // How many non-zero coefficients do we really have in the result?
        int plain_coeff_count = get_significant_uint64_count_uint(wide_destination.get(), coeff_count);

        // Resize destination to appropriate size
        destination.resize(plain_coeff_count);

        // Perform final multiplication by gamma inverse mod plain_modulus
        multiply_poly_scalar_coeffmod(wide_destination.get(), plain_coeff_count, base_converter_.get_inv_gamma(), 
            base_converter_.get_plain_gamma_array()[0], destination.pointer());
    }

    void Decryptor::compute_secret_key_array(int max_power)
    {
#ifdef SEAL_DEBUG
        if (max_power < 1)
        {
            throw invalid_argument("max_power must be at least 1");
        }
#endif
        ReaderLock reader_lock = secret_key_array_locker_.acquire_read();

        int old_size = secret_key_array_size_;
        int new_size = max(max_power, old_size);

        if (old_size == new_size)
        {
            return;
        }

        reader_lock.release();

        // Need to extend the array 
        int coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_mod_count = parms_.coeff_modulus().size();

        // Compute powers of secret key until max_power
        Pointer new_secret_key_array(allocate_poly(new_size * coeff_count, coeff_mod_count, pool_));
        set_poly_poly(secret_key_array_.get(), old_size * coeff_count, coeff_mod_count, new_secret_key_array.get());

        int poly_ptr_increment = coeff_count * coeff_mod_count;
        uint64_t *prev_poly_ptr = new_secret_key_array.get() + (old_size - 1) * poly_ptr_increment;
        uint64_t *next_poly_ptr = prev_poly_ptr + poly_ptr_increment;

        // Since all of the key powers in secret_key_array_ are already NTT transformed, to get the next one 
        // we simply need to compute a dyadic product of the last one with the first one [which is equal to NTT(secret_key_)].
        for (int i = old_size; i < new_size; i++)
        {
            for (int j = 0; j < coeff_mod_count; j++)
            {
                dyadic_product_coeffmod(prev_poly_ptr + (j * coeff_count), new_secret_key_array.get() + (j * coeff_count),
                    coeff_count, parms_.coeff_modulus()[j], next_poly_ptr + (j * coeff_count));
            }
            prev_poly_ptr = next_poly_ptr;
            next_poly_ptr += poly_ptr_increment;
        }

        
        // Take writer lock to update array
        WriterLock writer_lock = secret_key_array_locker_.acquire_write();

        // Do we still need to update size?
        old_size = secret_key_array_size_;
        new_size = max(max_power, secret_key_array_size_);

        if (old_size == new_size)
        {
            return;
        }

        // Acquire new array
        secret_key_array_size_ = new_size;
        secret_key_array_.acquire(new_secret_key_array);
    }

    void Decryptor::compose(uint64_t *value)
    {
#ifdef SEAL_DEBUG
        if (value == nullptr)
        {
            throw invalid_argument("input cannot be null");
        }
#endif
        int coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_mod_count = parms_.coeff_modulus().size();
        int total_uint64_count = coeff_mod_count * coeff_count;

        // Set temporary coefficients_ptr pointer to point to either an existing allocation given as parameter,
        // or else to a new allocation from the memory pool.
        Pointer coefficients(allocate_uint(total_uint64_count, pool_));
        uint64_t *coefficients_ptr = coefficients.get();

        // Re-merge the coefficients first
        for (int i = 0; i < coeff_count; i++)
        {
            for (int j = 0; j < coeff_mod_count; j++)
            {
                coefficients_ptr[j + (i * coeff_mod_count)] = value[(j * coeff_count) + i];
            }
        }

        Pointer temp(allocate_uint(coeff_mod_count, pool_));
        set_zero_uint(total_uint64_count, value);

        for (int i = 0; i < coeff_count; i++)
        {
            for (int j = 0; j < coeff_mod_count; j++)
            {
                uint64_t tmp = multiply_uint_uint_mod(coefficients_ptr[j], base_converter_.get_inv_coeff_mod_coeff_array()[j], parms_.coeff_modulus()[j]);
                multiply_uint_uint64(coeff_products_array_.get() + (j * coeff_mod_count), coeff_mod_count, tmp, coeff_mod_count, temp.get());
                add_uint_uint_mod(temp.get(), value + (i * coeff_mod_count), mod_.get(), coeff_mod_count, value + (i * coeff_mod_count));
            }
            set_zero_uint(coeff_mod_count, temp.get());
            coefficients_ptr += coeff_mod_count;
        }
    }

    int Decryptor::invariant_noise_budget(const Ciphertext &encrypted, const MemoryPoolHandle &pool)
    {
        int coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_mod_count = parms_.coeff_modulus().size();
        int array_poly_uint64_count = coeff_count * coeff_mod_count;
        int encrypted_size = encrypted.size();

        // Verify parameters.
        if (encrypted.hash_block_ != parms_.hash_block())
        {
            throw invalid_argument("encrypted is not valid for encryption parameters");
        }
        if (!pool)
        {
            throw invalid_argument("pool is uninitialized");
        }

        // Storage for noise uint
        Pointer destination(allocate_uint(coeff_mod_count, pool));

        // Storage for noise poly
        Pointer noise_poly(allocate_zero_poly(coeff_count, coeff_mod_count, pool));

        // Now need to compute c(s) - Delta*m (mod q)

        // Make sure we have enough secret keys computed
        compute_secret_key_array(encrypted_size - 1);

        /*
        Firstly find c_0 + c_1 *s + ... + c_{count-1} * s^{count-1} mod q
        This is equal to Delta m + v where ||v|| < Delta/2.
        */
        // put < (c_1 , c_2, ... , c_{count-1}) , (s,s^2,...,s^{count-1}) > mod q in destination_poly
        // Make a copy of the encrypted BigPolyArray for NTT (except the first polynomial is not needed)
        Pointer encrypted_copy(allocate_poly((encrypted_size - 1) * coeff_count, coeff_mod_count, pool));
        set_poly_poly(encrypted.pointer(1), (encrypted_size - 1) * coeff_count, coeff_mod_count, encrypted_copy.get());

        // Now do the dot product of encrypted_copy and the secret key array using NTT. The secret key powers are already NTT transformed.
        Pointer copy_operand1(allocate_uint(coeff_count, pool));
        for (int i = 0; i < coeff_mod_count; i++)
        {
            // Initialize pointers for multiplication
            const uint64_t *current_array1 = encrypted.pointer(1) + (i * coeff_count);
            const uint64_t *current_array2 = secret_key_array_.get() + (i * coeff_count);

            for (int j = 0; j < encrypted_size - 1; j++)
            {
                // Perform the dyadic product. 
                set_uint_uint(current_array1, coeff_count, copy_operand1.get());

                // Lazy reduction
                ntt_negacyclic_harvey_lazy(copy_operand1.get(), small_ntt_tables_[i]);

                dyadic_product_coeffmod(copy_operand1.get(), current_array2, coeff_count, small_ntt_tables_[i].modulus(), copy_operand1.get());
                add_poly_poly_coeffmod(noise_poly.get() + (i * coeff_count), copy_operand1.get(), coeff_count, small_ntt_tables_[i].modulus(),
                    noise_poly.get() + (i * coeff_count));

                current_array1 += array_poly_uint64_count;
                current_array2 += array_poly_uint64_count;
            }

            // Perform inverse NTT
            inverse_ntt_negacyclic_harvey(noise_poly.get() + (i * coeff_count), small_ntt_tables_[i]);
        }

        for (int i = 0; i < coeff_mod_count; i++)
        {
            // add c_0 into noise_poly
            add_poly_poly_coeffmod(noise_poly.get() + (i * coeff_count), encrypted.pointer() + (i * coeff_count),
                coeff_count, parms_.coeff_modulus()[i], noise_poly.get() + (i * coeff_count));

            // Multiply by parms_.plain_modulus() and reduce mod parms_.coeff_modulus() to get parms_.coeff_modulus()*noise
            multiply_poly_scalar_coeffmod(noise_poly.get() + (i * coeff_count), coeff_count,
                parms_.plain_modulus().value(), parms_.coeff_modulus()[i], noise_poly.get() + (i * coeff_count));
        }

        // Compose the noise
        compose(noise_poly.get());
        
        // Next we compute the infinity norm mod parms_.coeff_modulus()
        poly_infty_norm_coeffmod(noise_poly.get(), coeff_count, coeff_mod_count, mod_, destination.get(), pool);

        // The -1 accounts for scaling the invariant noise by 2 
        return max(0, mod_.significant_bit_count() - get_significant_bit_count_uint(destination.get(), coeff_mod_count) - 1);
    }
}
