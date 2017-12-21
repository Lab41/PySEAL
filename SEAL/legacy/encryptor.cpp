#include <algorithm>
#include <stdexcept>
#include "encryptor.h"
#include "util/common.h"
#include "util/uintarith.h"
#include "util/polyarith.h"
#include "util/polyarithmod.h"
#include "util/polyfftmultmod.h"
#include "util/clipnormal.h"
#include "util/randomtostd.h"
#include "util/ntt.h"
#include "bigpoly.h"

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

    void Encryptor::encrypt(const Plaintext &plain, Ciphertext &destination)
    {
        const BigPoly &plain_poly = plain.get_poly();
        BigPolyArray &destination_array = destination.get_mutable_array();

        int coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_bit_count = parms_.coeff_modulus().bit_count();
        int coeff_uint64_count = parms_.coeff_modulus().uint64_count();

#ifdef _DEBUG
        if (plain_poly.significant_coeff_count() >= coeff_count || !are_poly_coefficients_less_than(plain_poly, BigUInt(parms_.plain_modulus().bit_count(), *parms_.plain_modulus().pointer())))
        {
            throw invalid_argument("plain is too large to be represented by encryption parameters");
        }
#endif
        // Make destination have right size
        destination_array.resize(2, coeff_count, coeff_bit_count);

        /*
        Ciphertext (c_0,c_1) should be a BigPolyArray
        c_0 = Delta * m + public_key_[0] * u + e_1 where u sampled from R_2 and e_1 sampled from chi.
        c_1 = public_key_[1] * u + e_2 where e_2 sampled from chi. 
        */

        // Multiply plain by scalar coeff_div_plaintext and reposition if in upper-half.
        // Result gets added into the c_0 term of ciphertext (c_0,c_1).
        preencrypt(plain_poly.pointer(), plain_poly.coeff_count(), plain_poly.coeff_uint64_count(), destination_array.pointer(0));

        // Generate u 
        Pointer u(allocate_poly(coeff_count, coeff_uint64_count, pool_));
        unique_ptr<UniformRandomGenerator> random(parms_.random_generator()->create());

        set_poly_coeffs_zero_one_negone(u.get(), random.get());
        //set_poly_coeffs_zero_one(u.get(), random.get());

        // Calculate public_key_[0] * u.
        // Since we (may) need both public_key_[0] and u later, need a temp variable to store the solution.
        // Add temp into destination_array[0].
        Pointer temp(allocate_poly(coeff_count, coeff_uint64_count, pool_));

        // Multiply both u*public_key_[0] and u*public_key_[1] using the same FFT
        set_zero_uint(coeff_uint64_count, get_poly_coeff(temp.get(), coeff_count - 1, coeff_uint64_count));
        set_zero_uint(coeff_uint64_count, get_poly_coeff(destination_array.pointer(1), coeff_count - 1, coeff_uint64_count));
        
        if (qualifiers_.enable_ntt)
        {
            ntt_double_multiply_poly_nttpoly(u.get(), public_key_.get(), public_key_.get() + coeff_count * coeff_uint64_count, ntt_tables_, temp.get(), destination_array.pointer(1), pool_);
        }
        else if(!qualifiers_.enable_ntt && qualifiers_.enable_fft)
        {
            int coeff_count_power = polymod_.coeff_count_power_of_two(); 
            nussbaumer_multiply_poly_poly_coeffmod(u.get(), public_key_.get(), coeff_count_power, mod_, temp.get(), pool_); 
            nussbaumer_multiply_poly_poly_coeffmod(u.get(), public_key_.get() + coeff_count * coeff_uint64_count, coeff_count_power, mod_, destination_array.pointer(1), pool_);
        }
        else
        {
            // This branch should never be reached
            throw logic_error("invalid encryption parameters");
        }
        
        add_poly_poly_coeffmod(temp.get(), destination_array.pointer(0), coeff_count, parms_.coeff_modulus().pointer(), coeff_uint64_count, destination_array.pointer(0));

        // Generate e_0, add this value into destination_array[0].
        set_poly_coeffs_normal(temp.get(), random.get());
        add_poly_poly_coeffmod(temp.get(), destination_array.pointer(0), coeff_count, parms_.coeff_modulus().pointer(), coeff_uint64_count, destination_array.pointer(0));

        // Generate e_1, add this value into destination_array[1].
        set_poly_coeffs_normal(temp.get(), random.get());
        add_poly_poly_coeffmod(temp.get(), destination_array.pointer(1), coeff_count, parms_.coeff_modulus().pointer(), coeff_uint64_count, destination_array.pointer(1)); 

        // Set the hash block
        destination.hash_block_ = parms_.get_hash_block();
    }

    void Encryptor::preencrypt(const uint64_t *plain, int plain_coeff_count, int plain_coeff_uint64_count, uint64_t *destination)
    {
        int coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_uint64_count = parms_.coeff_modulus().uint64_count();

        if (plain_coeff_count > coeff_count)
        {
            plain_coeff_count = coeff_count;
        }

        // Multiply plain by scalar coeff_div_plain_modulus_ and reposition if in upper-half.
        if (plain == destination)
        {
            // If plain and destination are same poly, then need another storage for multiply output.
            Pointer temp(allocate_uint(coeff_uint64_count, pool_));
            for (int i = 0; i < plain_coeff_count; i++)
            {
                multiply_uint_uint(plain, plain_coeff_uint64_count, coeff_div_plain_modulus_.get(), coeff_uint64_count, coeff_uint64_count, temp.get());
                bool is_upper_half = is_greater_than_or_equal_uint_uint(temp.get(), upper_half_threshold_.get(), coeff_uint64_count);
                if (is_upper_half)
                {
                    add_uint_uint(temp.get(), upper_half_increment_.get(), coeff_uint64_count, destination);
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
            for (int i = 0; i < plain_coeff_count; i++)
            {
                //Multiply plain by coeff_div_plain_modulus_ and put the result in destination
                multiply_uint_uint(plain, plain_coeff_uint64_count, coeff_div_plain_modulus_.get(), coeff_uint64_count, coeff_uint64_count, destination);

                // check if destination >= upper half threshold
                bool is_upper_half = is_greater_than_or_equal_uint_uint(destination, upper_half_threshold_.get(), coeff_uint64_count);
                if (is_upper_half)
                {
                    add_uint_uint(destination, upper_half_increment_.get(), coeff_uint64_count, destination);
                }
                plain += plain_coeff_uint64_count;
                destination += coeff_uint64_count;
            }
        }

        // Zero any remaining coefficients.
        for (int i = plain_coeff_count; i < coeff_count; i++)
        {
            set_zero_uint(coeff_uint64_count, destination);
            destination += coeff_uint64_count;
        }
    }

    void Encryptor::set_poly_coeffs_zero_one_negone(uint64_t *poly, UniformRandomGenerator *random) const
    {
        int coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_uint64_count = parms_.coeff_modulus().uint64_count();

        Pointer coeff_modulus_minus_one(allocate_uint(coeff_uint64_count, pool_));
        decrement_uint(parms_.coeff_modulus().pointer(), coeff_uint64_count, coeff_modulus_minus_one.get());
        
        RandomToStandardAdapter engine(random);
        uniform_int_distribution<int> dist(-1, 1);

        for (int i = 0; i < coeff_count - 1; i++)
        {
            int rand_index = dist(engine);
            if (rand_index == 1)
            {
                set_uint(1, coeff_uint64_count, poly);
            }
            else if (rand_index == -1)
            {
                set_uint_uint(coeff_modulus_minus_one.get(), coeff_uint64_count, poly);
            }
            else
            {
                set_zero_uint(coeff_uint64_count, poly);
            }
            poly += coeff_uint64_count;
        }
        set_zero_uint(coeff_uint64_count, poly);
    }

    void Encryptor::set_poly_coeffs_zero_one(uint64_t *poly, UniformRandomGenerator *random) const
    {
        int coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_uint64_count = parms_.coeff_modulus().uint64_count();

        RandomToStandardAdapter engine(random);
        uniform_int_distribution<int> dist(0, 1);

        set_zero_poly(coeff_count, coeff_uint64_count, poly);

        for (int i = 0; i < coeff_count - 1; i++)
        {
            *poly = dist(engine);
            poly += coeff_uint64_count;
        }
    }

    void Encryptor::set_poly_coeffs_normal(uint64_t *poly, UniformRandomGenerator *random) const
    {
        int coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_uint64_count = parms_.coeff_modulus().uint64_count();
        if (parms_.noise_standard_deviation() == 0 || parms_.noise_max_deviation() == 0)
        {
            set_zero_poly(coeff_count, coeff_uint64_count, poly);
            return;
        }
        RandomToStandardAdapter engine(random);
        ClippedNormalDistribution dist(0, parms_.noise_standard_deviation(), parms_.noise_max_deviation());
        for (int i = 0; i < coeff_count - 1; i++)
        {
            int64_t noise = static_cast<int64_t>(dist(engine));
            if (noise > 0)
            {
                set_uint(static_cast<uint64_t>(noise), coeff_uint64_count, poly);
            }
            else if (noise < 0)
            {
                noise = -noise;
                set_uint(static_cast<uint64_t>(noise), coeff_uint64_count, poly);
                sub_uint_uint(parms_.coeff_modulus().pointer(), poly, coeff_uint64_count, poly);
            }
            else
            {
                set_zero_uint(coeff_uint64_count, poly);
            }
            poly += coeff_uint64_count;
        }
        set_zero_uint(coeff_uint64_count, poly);
    }

    Encryptor::Encryptor(const SEALContext &context, const PublicKey &public_key, const MemoryPoolHandle &pool) :
        pool_(pool), parms_(context.get_parms()),
        ntt_tables_(pool_),
        qualifiers_(context.get_qualifiers())
    {
        // Verify parameters
        if (!qualifiers_.parameters_set)
        {
            throw invalid_argument("encryption parameters are not valid");
        }

        // Verify parameters.
        if (public_key.hash_block_ != parms_.get_hash_block())
        {
            throw invalid_argument("public key is not valid for encryption parameters");
        }

        int coeff_count = parms_.poly_modulus().coeff_count();
        int poly_coeff_uint64_count = parms_.poly_modulus().coeff_uint64_count();
        int coeff_bit_count = parms_.coeff_modulus().bit_count();
        int coeff_uint64_count = parms_.coeff_modulus().uint64_count();

        // Allocate space and copy over key
        public_key_ = allocate_poly(2 * coeff_count, coeff_uint64_count, pool_);
        set_poly_poly(public_key.get_array().pointer(0), 2 * coeff_count, coeff_uint64_count, public_key_.get());

        // Calculate coeff_modulus / plain_modulus and upper_half_increment.
        coeff_div_plain_modulus_ = allocate_uint(coeff_uint64_count, pool_);
        upper_half_increment_ = allocate_uint(coeff_uint64_count, pool_);
        ConstPointer wide_plain_modulus(duplicate_uint_if_needed(parms_.plain_modulus().pointer(), parms_.plain_modulus().uint64_count(), coeff_uint64_count, false, pool_));
        divide_uint_uint(parms_.coeff_modulus().pointer(), wide_plain_modulus.get(), coeff_uint64_count, coeff_div_plain_modulus_.get(), upper_half_increment_.get(), pool_);

        // Calculate (plain_modulus + 1) / 2 * coeff_div_plain_modulus.
        Pointer temp(allocate_uint(coeff_uint64_count, pool_));
        upper_half_threshold_ = allocate_uint(coeff_uint64_count, pool_);
        half_round_up_uint(wide_plain_modulus.get(), coeff_uint64_count, temp.get());
        multiply_truncate_uint_uint(temp.get(), coeff_div_plain_modulus_.get(), coeff_uint64_count, upper_half_threshold_.get());

        // Initialize moduli.
        polymod_ = PolyModulus(parms_.poly_modulus().pointer(), coeff_count, poly_coeff_uint64_count);
        mod_ = Modulus(parms_.coeff_modulus().pointer(), coeff_uint64_count, pool_);

        // Generate NTT tables if needed
        if (qualifiers_.enable_ntt)
        {
            // Copy over NTT tables (switching to local pool)
            ntt_tables_ = context.ntt_tables_;
        }
    }

    Encryptor::Encryptor(const Encryptor &copy) : 
        pool_(copy.pool_), parms_(copy.parms_), 
        ntt_tables_(copy.ntt_tables_), 
        qualifiers_(copy.qualifiers_)
    {
        int coeff_count = parms_.poly_modulus().coeff_count();
        int poly_coeff_uint64_count = parms_.poly_modulus().coeff_uint64_count();
        int coeff_uint64_count = parms_.coeff_modulus().uint64_count();
        
        // Allocate and copy over data
        upper_half_threshold_ = allocate_uint(coeff_uint64_count, pool_);
        set_uint_uint(copy.upper_half_threshold_.get(), coeff_uint64_count, upper_half_threshold_.get());

        upper_half_increment_ = allocate_uint(coeff_uint64_count, pool_);
        set_uint_uint(copy.upper_half_increment_.get(), coeff_uint64_count, upper_half_increment_.get());

        coeff_div_plain_modulus_ = allocate_uint(coeff_uint64_count, pool_);
        set_uint_uint(copy.coeff_div_plain_modulus_.get(), coeff_uint64_count, coeff_div_plain_modulus_.get());

        public_key_ = allocate_poly(2 * coeff_count, coeff_uint64_count, pool_);
        set_poly_poly(copy.public_key_.get(), 2 * coeff_count, coeff_uint64_count, public_key_.get());

        // Initialize moduli.
        polymod_ = PolyModulus(parms_.poly_modulus().pointer(), coeff_count, poly_coeff_uint64_count);
        mod_ = Modulus(parms_.coeff_modulus().pointer(), coeff_uint64_count, pool_);
    }
}
