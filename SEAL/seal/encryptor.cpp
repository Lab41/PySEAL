#include <algorithm>
#include <stdexcept>
#include "seal/encryptor.h"
#include "seal/util/common.h"
#include "seal/util/uintarith.h"
#include "seal/util/polyarithsmallmod.h"
#include "seal/util/polyfftmultsmallmod.h"
#include "seal/util/clipnormal.h"
#include "seal/util/randomtostd.h"
#include "seal/util/smallntt.h"
#include "seal/smallmodulus.h"

using namespace std;
using namespace seal::util;

namespace seal
{
    Encryptor::Encryptor(const SEALContext &context, const PublicKey &public_key, const MemoryPoolHandle &pool) :
        pool_(pool), parms_(context.parms()), qualifiers_(context.qualifiers())
    {
        // Verify parameters
        if (!qualifiers_.parameters_set)
        {
            throw invalid_argument("encryption parameters are not valid");
        }
        if (public_key.hash_block_ != parms_.hash_block())
        {
            throw invalid_argument("public key is not valid for encryption parameters");
        }
        if (!pool)
        {
            throw invalid_argument("pool is uninitialized");
        }

        int coeff_count = parms_.poly_modulus().coeff_count();
        int poly_coeff_uint64_count = parms_.poly_modulus().coeff_uint64_count();
        int coeff_mod_count = parms_.coeff_modulus().size();

        // Set SmallNTTTables
        small_ntt_tables_.resize(coeff_mod_count, pool_);
        small_ntt_tables_ = context.small_ntt_tables_;
        
        // Allocate space and copy over key
        public_key_ = allocate_poly(2 * coeff_count, coeff_mod_count, pool_);
        set_poly_poly(public_key.data().pointer(0), 2 * coeff_count, coeff_mod_count, public_key_.get());

        // Calculate coeff_modulus / plain_modulus and upper_half_increment.
        coeff_div_plain_modulus_ = allocate_uint(coeff_mod_count, pool_);
        upper_half_increment_ = allocate_uint(coeff_mod_count, pool_);
        ConstPointer wide_plain_modulus(duplicate_uint_if_needed(parms_.plain_modulus().pointer(), 
            parms_.plain_modulus().uint64_count(), coeff_mod_count, false, pool_));
        divide_uint_uint(context.total_coeff_modulus().pointer(), wide_plain_modulus.get(), 
            coeff_mod_count, coeff_div_plain_modulus_.get(), upper_half_increment_.get(), pool_);
        
        // Decompose coeff_div_plain_modulus and upper_half_increment
        Pointer temp_reduction(allocate_uint(coeff_mod_count, pool_));
        for (int i = 0; i < coeff_mod_count; i++)
        {
            temp_reduction[i] = modulo_uint(coeff_div_plain_modulus_.get(), coeff_mod_count, parms_.coeff_modulus()[i], pool_);
        }
        set_uint_uint(temp_reduction.get(), coeff_mod_count, coeff_div_plain_modulus_.get());
        for (int i = 0; i < coeff_mod_count; i++)
        {
            temp_reduction[i] = modulo_uint(upper_half_increment_.get(), coeff_mod_count, parms_.coeff_modulus()[i], pool_);
        }
        set_uint_uint(temp_reduction.get(), coeff_mod_count, upper_half_increment_.get());

        // Calculate (plain_modulus + 1) / 2.
        plain_upper_half_threshold_ = (parms_.plain_modulus().value() + 1) >> 1;

        // Initialize moduli.
        polymod_ = PolyModulus(parms_.poly_modulus().pointer(), coeff_count, poly_coeff_uint64_count);
    }

    void Encryptor::encrypt(const Plaintext &plain, Ciphertext &destination, const MemoryPoolHandle &pool)
    {
        int coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_mod_count = parms_.coeff_modulus().size();

        if (plain.coeff_count() > coeff_count || (plain.coeff_count() == coeff_count && plain[coeff_count - 1] != 0))
        {
            throw invalid_argument("plain is not valid for encryption parameters");
        }
#ifdef SEAL_DEBUG
        if (plain.significant_coeff_count() >= coeff_count || !are_poly_coefficients_less_than(plain.pointer(), 
            plain.coeff_count(), 1, parms_.plain_modulus().pointer(), 1))
        {
            throw invalid_argument("plain is not valid for encryption parameters");
        }
#endif
        if (!pool)
        {
            throw invalid_argument("pool is uninitialized");
        }

        // Make destination have right size and hash block
        destination.resize(parms_, 2);

        /*
        Ciphertext (c_0,c_1) should be a BigPolyArray
        c_0 = Delta * m + public_key_[0] * u + e_1 where u sampled from R_2 and e_1 sampled from chi.
        c_1 = public_key_[1] * u + e_2 where e_2 sampled from chi.
        */

        // Generate u 
        Pointer u(allocate_poly(coeff_count, coeff_mod_count, pool));
        unique_ptr<UniformRandomGenerator> random(parms_.random_generator()->create());
        
        set_poly_coeffs_zero_one_negone(u.get(), random.get());
        //set_poly_coeffs_zero_one(u.get(), random.get());

        // Multiply both u * public_key_[0] and u * public_key_[1] using the same FFT
        set_zero_uint(coeff_mod_count, destination.mutable_pointer() + (coeff_count - 1));
        set_zero_uint(coeff_mod_count, destination.mutable_pointer(1) + (coeff_count - 1));
        
        for (int i = 0; i < coeff_mod_count; i++)
        {
            ntt_double_multiply_poly_nttpoly(u.get() + (i * coeff_count), public_key_.get() + (i * coeff_count), 
                public_key_.get() + (coeff_count * coeff_mod_count) + (i * coeff_count), small_ntt_tables_[i], 
                destination.mutable_pointer() + (i * coeff_count), destination.mutable_pointer(1) + (i * coeff_count), pool);
        }

        // Multiply plain by scalar coeff_div_plaintext and reposition if in upper-half.
        // Result gets added into the c_0 term of ciphertext (c_0,c_1).
        preencrypt(plain.pointer(), plain.coeff_count(), destination.mutable_pointer());

        // Generate e_0, add this value into destination[0].
        set_poly_coeffs_normal(u.get(), random.get());
        for (int i = 0; i < coeff_mod_count; i++)
        {
            add_poly_poly_coeffmod(u.get() + (i * coeff_count), destination.pointer() + (i * coeff_count), 
                coeff_count, parms_.coeff_modulus()[i], destination.mutable_pointer() + (i * coeff_count));
        }
        // Generate e_1, add this value into destination[1].
        set_poly_coeffs_normal(u.get(), random.get());
        for (int i = 0; i < coeff_mod_count; i++)
        {
            add_poly_poly_coeffmod(u.get() + (i * coeff_count), destination.pointer(1) + (i * coeff_count), 
                coeff_count, parms_.coeff_modulus()[i], destination.mutable_pointer(1) + (i * coeff_count));
        }
    }

    void Encryptor::preencrypt(const uint64_t *plain, int plain_coeff_count, uint64_t *destination)
    {
        int coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_mod_count = parms_.coeff_modulus().size();

        // Multiply plain by scalar coeff_div_plain_modulus_ and reposition if in upper-half.
        for (int i = 0; i < plain_coeff_count; i++)
        {
            if (plain[i] >= plain_upper_half_threshold_)
            {
                // Loop over primes
                for (int j = 0; j < coeff_mod_count; j++)
                {
                    uint64_t temp[2]{ 0 };
                    multiply_uint64(*(coeff_div_plain_modulus_.get() + j), plain[i], temp);
                    temp[1] += add_uint64(temp[0], *(upper_half_increment_.get() + j), 0, temp);
                    uint64_t scaled_plain_coeff = barrett_reduce_128(temp, parms_.coeff_modulus()[j]);
                    destination[j * coeff_count] = add_uint_uint_mod(destination[j * coeff_count], scaled_plain_coeff, parms_.coeff_modulus()[j]);
                }
            }
            else
            {
                for (int j = 0; j < coeff_mod_count; j++)
                {
                    uint64_t scaled_plain_coeff = multiply_uint_uint_mod(coeff_div_plain_modulus_[j], plain[i], parms_.coeff_modulus()[j]);
                    destination[j * coeff_count] = add_uint_uint_mod(destination[j * coeff_count], scaled_plain_coeff, parms_.coeff_modulus()[j]);
                }
            }
            destination++;
        }
    }

    void Encryptor::set_poly_coeffs_zero_one_negone(uint64_t *poly, UniformRandomGenerator *random) const
    {
        int coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_mod_count = parms_.coeff_modulus().size();

        RandomToStandardAdapter engine(random);
        uniform_int_distribution<int> dist(-1, 1);
        for (int i = 0; i < coeff_count - 1; i++)
        {
            int rand_index = dist(engine);
            if (rand_index == 1)
            {
                for (int j = 0; j < coeff_mod_count; j++)
                {
                    poly[i + (j * coeff_count)] = 1;
                }
            }
            else if (rand_index == -1)
            {
                for (int j = 0; j < coeff_mod_count; j++)
                {
                    poly[i + (j * coeff_count)] = parms_.coeff_modulus()[j].value() - 1;
                }
            }
            else
            {
                for (int j = 0; j < coeff_mod_count; j++)
                {
                    poly[i + (j * coeff_count)] = 0;
                }
            }
        }

        // Set the last coefficient equal to zero in RNS representation
        for (int i = 0; i < coeff_mod_count; i++)
        {
            poly[(coeff_count - 1) + (i * coeff_count)] = 0;
        }
    }

    void Encryptor::set_poly_coeffs_zero_one(uint64_t *poly, UniformRandomGenerator *random) const
    {
        int coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_mod_count = parms_.coeff_modulus().size();

        RandomToStandardAdapter engine(random);
        uniform_int_distribution<int> dist(0, 1);

        set_zero_poly(coeff_count, coeff_mod_count, poly);
        for (int i = 0; i < coeff_count; i++)
        {
            int rand_index = dist(engine);
            for (int j = 0; j < coeff_mod_count; j++)
            {
                poly[i + (j * coeff_count)] = rand_index;
            }
        }
    }

    void Encryptor::set_poly_coeffs_normal(uint64_t *poly, UniformRandomGenerator *random) const
    {
        int coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_mod_count = parms_.coeff_modulus().size();
        if (parms_.noise_standard_deviation() == 0 || parms_.noise_max_deviation() == 0)
        {
            set_zero_poly(coeff_count, coeff_mod_count, poly);
            return;
        }
        RandomToStandardAdapter engine(random);
        ClippedNormalDistribution dist(0, parms_.noise_standard_deviation(), parms_.noise_max_deviation());
        for (int i = 0; i < coeff_count - 1; i++)
        {
            int64_t noise = static_cast<int64_t>(dist(engine));
            if (noise > 0)
            {
                for (int j = 0; j < coeff_mod_count; j++)
                {
                    poly[i + (j * coeff_count)] = static_cast<uint64_t>(noise);
                }
            }
            else if (noise < 0)
            {
                noise = -noise;
                for (int j = 0; j < coeff_mod_count; j++)
                {
                    poly[i + (j * coeff_count)] = parms_.coeff_modulus()[j].value() - static_cast<uint64_t>(noise);
                }
            }
            else
            {
                for (int j = 0; j < coeff_mod_count; j++)
                {
                    poly[i + (j * coeff_count)] = 0;
                }
            }
        }
        
        // Set the last coefficient equal to zero in RNS representation
        for (int i = 0; i < coeff_mod_count; i++)
        {
            poly[(coeff_count - 1) + (i * coeff_count)] = 0;
        }
    }

    Encryptor::Encryptor(const Encryptor &copy) :
        pool_(copy.pool_), parms_(copy.parms_), qualifiers_(copy.qualifiers_),
        small_ntt_tables_(copy.small_ntt_tables_),
        plain_upper_half_threshold_(copy.plain_upper_half_threshold_)
    {
        int coeff_count = parms_.poly_modulus().coeff_count();
        int poly_coeff_uint64_count = parms_.poly_modulus().coeff_uint64_count();
        int coeff_uint64_count = parms_.coeff_modulus().size();

        // Allocate and copy over data
        upper_half_increment_ = allocate_uint(coeff_uint64_count, pool_);
        set_uint_uint(copy.upper_half_increment_.get(), coeff_uint64_count, upper_half_increment_.get());

        coeff_div_plain_modulus_ = allocate_uint(coeff_uint64_count, pool_);
        set_uint_uint(copy.coeff_div_plain_modulus_.get(), coeff_uint64_count, coeff_div_plain_modulus_.get());

        public_key_ = allocate_poly(2 * coeff_count, coeff_uint64_count, pool_);
        set_poly_poly(copy.public_key_.get(), 2 * coeff_count, coeff_uint64_count, public_key_.get());

        // Initialize moduli.
        polymod_ = PolyModulus(parms_.poly_modulus().pointer(), coeff_count, poly_coeff_uint64_count);
    }
}