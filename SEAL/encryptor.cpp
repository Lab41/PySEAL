#include <algorithm>
#include <stdexcept>
#include "encryptor.h"
#include "util/common.h"
#include "util/uintarith.h"
#include "util/polyarith.h"
#include "util/polyarithmod.h"
#include "util/clipnormal.h"
#include "util/randomtostd.h"
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

    void Encryptor::encrypt(const BigPoly &plain, BigPolyArray &destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        int plain_coeff_uint64_count = divide_round_up(plain.coeff_bit_count(), bits_per_uint64);

        // Verify parameters.
#ifdef _DEBUG
        if (plain.significant_coeff_count() >= coeff_count || !are_poly_coefficients_less_than(plain, plain_modulus_))
        {
            throw invalid_argument("plain is too large to be represented by encryption parameters");
        }
#endif

        // Make destination have right size
        destination.resize(2, coeff_count, coeff_bit_count);

        /*
        Ciphertext (c_0,c_1) should be a BigPolyArray
        c_0 = Delta * m + public_key_[0] * u + e_1 where u sampled from R_2 and e_1 sampled from chi.
        c_1 = public_key_[1] * u + e_2 where e_2 sampled from chi. 
        */

        // Multiply plain by scalar coeff_div_plaintext and reposition if in upper-half.
        // Result gets added into the c_0 term of ciphertext (c_0,c_1).
        preencrypt(plain.pointer(), plain.coeff_count(), plain_coeff_uint64_count, destination[0].pointer());

        // Generate u 
        MemoryPool &pool = *MemoryPool::default_pool();
        unique_ptr<UniformRandomGenerator> random(random_generator_->create());
        Pointer u(allocate_poly(coeff_count, coeff_uint64_count, pool));
        set_poly_coeffs_zero_one_negone(u.get(), random.get());

        // Calculate public_key_[0] * u.
        // Since we (may) need both public_key_[0] and u later, need a temp variable to store the solution.
        // Add temp into destination[0].
        Pointer temp(allocate_poly(coeff_count, coeff_uint64_count, pool));
        multiply_poly_poly_polymod_coeffmod(u.get(), public_key_[0].pointer(), polymod_, mod_, temp.get(), pool);
        add_poly_poly_coeffmod(temp.get(), destination[0].pointer(), coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, destination[0].pointer());

        // Generate e_1, add this value into destination[0].
        set_poly_coeffs_normal(temp.get(), random.get());
        add_poly_poly_coeffmod(temp.get(), destination[0].pointer(), coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, destination[0].pointer());

        // Calculate public_key_[1] * u and set this value to destination[1].
        multiply_poly_poly_polymod_coeffmod(u.get(), public_key_[1].pointer(), polymod_, mod_, destination[1].pointer(), pool);

        // Generate e_2, add this value into destination[1].
        set_poly_coeffs_normal(temp.get(), random.get());
        add_poly_poly_coeffmod(temp.get(), destination[1].pointer(), coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, destination[1].pointer());
    }

    void Encryptor::preencrypt(const std::uint64_t *plain, int plain_coeff_count, int plain_coeff_uint64_count, std::uint64_t *destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        if (plain_coeff_count > coeff_count)
        {
            plain_coeff_count = coeff_count;
        }

        // Multiply plain by scalar coeff_div_plaintext and reposition if in upper-half.
        MemoryPool &pool = *MemoryPool::default_pool();
        if (plain == destination)
        {
            // If plain and destination are same poly, then need another storage for multiply output.
            Pointer temp(allocate_uint(coeff_uint64_count, pool));
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
                //Multiply plain by coeff_div_plain_modulus and put the result in destination
                multiply_uint_uint(plain, plain_coeff_uint64_count, coeff_div_plain_modulus_.pointer(), coeff_uint64_count, coeff_uint64_count, destination);

                // check if destination >= upper half threshold
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


    void Encryptor::set_poly_coeffs_zero_one_negone(uint64_t *poly, UniformRandomGenerator *random) const
    {
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        RandomToStandardAdapter engine(random);
        uniform_int_distribution<int> dist(-1, 1);
        for (int i = 0; i < coeff_count - 1; ++i)
        {
            int rand_index = dist(engine);
            if (rand_index == 1)
            {
                set_uint(1, coeff_uint64_count, poly);
            }
            else if (rand_index == -1)
            {
                set_uint_uint((coeff_modulus_ - 1).pointer(), coeff_uint64_count, poly);
            }
            else
            {
                set_zero_uint(coeff_uint64_count, poly);
            }
            poly += coeff_uint64_count;
        }
        set_zero_uint(coeff_uint64_count, poly);
    }

    void Encryptor::set_poly_coeffs_normal(std::uint64_t *poly, UniformRandomGenerator *random) const
    {
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        if (noise_standard_deviation_ == 0 || noise_max_deviation_ == 0)
        {
            set_zero_poly(coeff_count, coeff_uint64_count, poly);
            return;
        }
        RandomToStandardAdapter engine(random);
        ClippedNormalDistribution dist(0, noise_standard_deviation_, noise_max_deviation_);
        for (int i = 0; i < coeff_count - 1; ++i)
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
                sub_uint_uint(coeff_modulus_.pointer(), poly, coeff_uint64_count, poly);
            }
            else
            {
                set_zero_uint(coeff_uint64_count, poly);
            }
            poly += coeff_uint64_count;
        }
        set_zero_uint(coeff_uint64_count, poly);
    }

    Encryptor::Encryptor(const EncryptionParameters &parms, const BigPolyArray &public_key) :
        poly_modulus_(parms.poly_modulus()), coeff_modulus_(parms.coeff_modulus()), plain_modulus_(parms.plain_modulus()), public_key_(public_key),
        noise_standard_deviation_(parms.noise_standard_deviation()), noise_max_deviation_(parms.noise_max_deviation()), 
        random_generator_(parms.random_generator() != nullptr ? parms.random_generator() : UniformRandomGeneratorFactory::default_factory())
    {
        int coeff_count = poly_modulus_.significant_coeff_count();
        int coeff_bit_count = coeff_modulus_.significant_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

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
        if (noise_standard_deviation_ < 0)
        {
            throw invalid_argument("noise_standard_deviation must be non-negative");
        }
        if (noise_max_deviation_ < 0)
        {
            throw invalid_argument("noise_max_deviation must be non-negative");
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

        // Public key has to have right size.
        if (public_key_.size() != 2 || public_key_.coeff_count() != coeff_count || public_key_.coeff_bit_count() != coeff_bit_count ||
            public_key_[0].significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(public_key_[0], coeff_modulus_) ||
            public_key_[1].significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(public_key_[1], coeff_modulus_))
        {
            throw invalid_argument("public_key is not valid for encryption parameters");
        }

        // Calculate coeff_modulus / plain_modulus.
        MemoryPool &pool = *MemoryPool::default_pool();
        coeff_div_plain_modulus_.resize(coeff_bit_count);
        Pointer temp(allocate_uint(coeff_uint64_count, pool));
        divide_uint_uint(coeff_modulus_.pointer(), plain_modulus_.pointer(), coeff_uint64_count, coeff_div_plain_modulus_.pointer(), temp.get(), pool);

        // Calculate (plain_modulus + 1) / 2 * coeff_div_plain_modulus.
        upper_half_threshold_.resize(coeff_bit_count);
        half_round_up_uint(plain_modulus_.pointer(), coeff_uint64_count, temp.get());
        multiply_truncate_uint_uint(temp.get(), coeff_div_plain_modulus_.pointer(), coeff_uint64_count, upper_half_threshold_.pointer());

        // Calculate upper_half_increment.
        upper_half_increment_.resize(coeff_bit_count);
        multiply_truncate_uint_uint(plain_modulus_.pointer(), coeff_div_plain_modulus_.pointer(), coeff_uint64_count, upper_half_increment_.pointer());
        sub_uint_uint(coeff_modulus_.pointer(), upper_half_increment_.pointer(), coeff_uint64_count, upper_half_increment_.pointer());

        // Initialize moduli.
        polymod_ = PolyModulus(poly_modulus_.pointer(), coeff_count, coeff_uint64_count);
        mod_ = Modulus(coeff_modulus_.pointer(), coeff_uint64_count, pool);
        
    }
}
