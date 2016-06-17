#include <algorithm>
#include <random>
#include "keygenerator.h"
#include "util/uintcore.h"
#include "util/uintarith.h"
#include "util/uintarithmod.h"
#include "util/polyarith.h"
#include "util/polyarithmod.h"
#include "util/randomtostd.h"
#include "util/clipnormal.h"
#include "util/polyextras.h"
#include "util/polycore.h"

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

    void KeyGenerator::generate(int evaluation_keys_count)
    {
        // if decomposition bit count is zero, evaluation keys must be empty
        if (decomposition_bit_count_ == 0 && evaluation_keys_count != 0)
        {
            throw invalid_argument("cannot generate evaluation keys for specified encryption parameters");
        }

        // If already generated, reset everything.
        if (is_generated_)
        {
            evaluation_keys_.clear();
            secret_key_.set_zero();
            public_key_.set_zero();
            is_generated_ = false;
        }

        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        MemoryPool &pool = *MemoryPool::default_pool();
        unique_ptr<UniformRandomGenerator> random(random_generator_->create());

        // generate secret key
        uint64_t *secret_key = secret_key_.pointer();
        set_poly_coeffs_zero_one_negone(secret_key, random.get());

        //generate public key: (pk[0],pk[1]) = ([-(as+e)]_q, a)
        
        // sample a uniformly at random
        // set pk[1] = a
        uint64_t *public_key_1 = public_key_.pointer(1);
        set_poly_coeffs_uniform(public_key_1, random.get());

        // calculate a*s (mod q) and store in pk[0]
        // pk[0] is now as mod q
        multiply_poly_poly_polymod_coeffmod(public_key_1, secret_key, polymod_, mod_, public_key_.pointer(0), pool);

        // add error into pk[0] (mod q)
        // pk[0] is now (a*s + e) mod q
        Pointer noise(allocate_poly(coeff_count, coeff_uint64_count, pool));
        set_poly_coeffs_normal(noise.get(), random.get());
        add_poly_poly_coeffmod(noise.get(), public_key_[0].pointer(), coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, public_key_[0].pointer());
        
        // negate and set this value to pk[0]
        // pk[0] is now -(as+e) mod q
        negate_poly_coeffmod(public_key_[0].pointer(), coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, public_key_[0].pointer());

        // Secret key and public key have been generated
        is_generated_ = true;

        // generate the requested number of evaluation keys
        generate_evaluation_keys(evaluation_keys_count);
    }

    void KeyGenerator::generate_evaluation_keys(int count)
    {
        // if decomposition bit count is zero, evaluation keys must be empty
        if (decomposition_bit_count_ == 0 && count != 0)
        {
            throw invalid_argument("cannot generate evaluation keys for specified encryption parameters");
        }

        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Check to see if secret key and public key have been generated
        if (!is_generated_)
        {
            throw logic_error("cannot generate evaluation keys for unspecified secret key");
        }
        
        // Check to see if count is not negative
        if (count < 0)
        {
            throw invalid_argument("count must be non-negative");
        }

        // Check if the specified number of evaluation keys have already been generated.
        // This would be the case if count is less than the current evaluation_keys_.size().
        // In this case, do nothing.
        if (count <= evaluation_keys_.size() )
        {
            return;
        }

        // In the constructor, evaluation_keys_ is initialized to have evaluation_keys_.size() = 0.
        // In a previous call to generate_evaluation_keys, evaluation_keys_ was only initialized to contain evaluation_keys_.size() entries.
        // Therefore need to initialize further if count > evaluation_keys_.size().
        int initial_evaluation_key_size = static_cast<int>(evaluation_keys_.size());
        int evaluation_factors_count = static_cast<int>(evaluation_factors_.size());
        for (int j = initial_evaluation_key_size; j < count; ++j)
        {
            evaluation_keys_.keys().push_back(std::make_pair(BigPolyArray(evaluation_factors_count, coeff_count, coeff_bit_count), BigPolyArray(evaluation_factors_count, coeff_count, coeff_bit_count)));
        }

        MemoryPool &pool = *MemoryPool::default_pool();
        unique_ptr<UniformRandomGenerator> random(random_generator_->create());

        // Create evaluation keys.
        Pointer noise(allocate_poly(coeff_count, coeff_uint64_count, pool));
        Pointer power(allocate_uint(coeff_uint64_count, pool));
        Pointer secret_key_power(allocate_poly(coeff_count, coeff_uint64_count, pool));
        Pointer temp(allocate_poly(coeff_count, coeff_uint64_count, pool));

        for (int k = initial_evaluation_key_size; k < count; ++k)
        {
            // calculate s^{k+2}
            // (evaluation_keys_[k] corresponds to s^{k+2})
            set_uint(k + 2, coeff_uint64_count, power.get());
            exponentiate_poly_polymod_coeffmod(secret_key_.pointer(), power.get(), coeff_uint64_count, polymod_, mod_, secret_key_power.get(), pool);

            //populate evaluate_keys_[k]
            for (int i = 0; i < evaluation_factors_.size(); ++i)
            {
                //generate a_i and store in evaluation_keys_[k].second[i]
                uint64_t *eval_keys_second = evaluation_keys_[k].second[i].pointer();
                set_poly_coeffs_uniform(eval_keys_second, random.get());

                // calculate a_i*s and store in evaluation_keys_[k].first[i]
                multiply_poly_poly_polymod_coeffmod(eval_keys_second, secret_key_.pointer(), polymod_, mod_, evaluation_keys_[k].first[i].pointer(), pool);

                //generate e_i 
                set_poly_coeffs_normal(noise.get(), random.get());

                //add e_i into evaluation_keys_[k].first[i]
                add_poly_poly_coeffmod(noise.get(), evaluation_keys_[k].first[i].pointer(), coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, evaluation_keys_[k].first[i].pointer());

                // negate value in evaluation_keys_[k].first[i]
                negate_poly_coeffmod(evaluation_keys_[k].first[i].pointer(), coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, evaluation_keys_[k].first[i].pointer());

                //multiply w^i by s^(k+2)
                multiply_poly_scalar_coeffmod(secret_key_power.get(), coeff_count, evaluation_factors_[i].pointer(), mod_, temp.get(), pool);

                //add w^i . s^(k+2) into evaluation_keys_[k].first[i]
                add_poly_poly_coeffmod(evaluation_keys_[k].first[i].pointer(), temp.get(), coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, evaluation_keys_[k].first[i].pointer());
            }
        }
    }

    void KeyGenerator::set_poly_coeffs_zero_one_negone(uint64_t *poly, UniformRandomGenerator *random) const
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
                set_uint_uint(coeff_modulus_minus_one_.pointer(), coeff_uint64_count, poly);
            }
            else
            {
                set_zero_uint(coeff_uint64_count, poly);
            }
            poly += coeff_uint64_count;
        }
        set_zero_uint(coeff_uint64_count, poly);
    }

    void KeyGenerator::set_poly_coeffs_normal(uint64_t *poly, UniformRandomGenerator *random) const
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

    /*Set the coeffs of a BigPoly to be uniform modulo coeff_mod*/
    void KeyGenerator::set_poly_coeffs_uniform(uint64_t *poly, UniformRandomGenerator *random)
    {
        //get parameters
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
       
        // set up source of randomness which produces random things of size 32 bit
        RandomToStandardAdapter engine(random);

        // pointer to increment to fill in all coeffs
        uint32_t * value_ptr = reinterpret_cast<uint32_t *> (poly);
        
        // number of 32 bit blocks to cover all coeffs
        int significant_value_uint32_count = (coeff_count-1) * 2 * coeff_modulus_.uint64_count();
        int value_uint32_count = significant_value_uint32_count + 2 * coeff_modulus_.uint64_count();

        // Sample randomness to all but last coefficient
        for (int i = 0; i < value_uint32_count; ++i)
        {
            *value_ptr++ = i < significant_value_uint32_count ? engine() : 0;
        }

        // when poly is fully populated, reduce all coefficient modulo coeff_modulus
        MemoryPool &pool = *MemoryPool::default_pool();
        modulo_poly_coeffs(poly, coeff_count, mod_, pool);
    }

    KeyGenerator::KeyGenerator(const EncryptionParameters &parms) :
        poly_modulus_(parms.poly_modulus()), coeff_modulus_(parms.coeff_modulus()), plain_modulus_(parms.plain_modulus()),
        noise_standard_deviation_(parms.noise_standard_deviation()), noise_max_deviation_(parms.noise_max_deviation()),
        decomposition_bit_count_(parms.decomposition_bit_count()), 
        random_generator_(parms.random_generator() != nullptr ? parms.random_generator() : UniformRandomGeneratorFactory::default_factory())
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
        if (noise_standard_deviation_ < 0)
        {
            throw invalid_argument("noise_standard_deviation must be non-negative");
        }
        if (noise_max_deviation_ < 0)
        {
            throw invalid_argument("noise_max_deviation must be non-negative");
        }
        if (decomposition_bit_count_ < 0)
        {
            throw invalid_argument("decomposition_bit_count must be non-negative");
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

        // Calculate -1 (mod coeff_modulus).
        coeff_modulus_minus_one_.resize(coeff_bit_count);
        decrement_uint(coeff_modulus_.pointer(), coeff_uint64_count, coeff_modulus_minus_one_.pointer());

        // Initialize public and secret key.
        public_key_.resize(2, coeff_count, coeff_bit_count);
        secret_key_.resize(coeff_count, coeff_bit_count);

        // Initialize evaluation_factors_, if required
        if (decomposition_bit_count_ != 0)
        {
            populate_evaluation_factors();
        }

        // Initialize evaluation keys to empty
        evaluation_keys_.clear();

        MemoryPool &pool = *MemoryPool::default_pool();

        // Initialize moduli.
        polymod_ = PolyModulus(poly_modulus_.pointer(), coeff_count, coeff_uint64_count);
        mod_ = Modulus(coeff_modulus_.pointer(), coeff_uint64_count, pool);

        // Secret key and public key have not been generated
        is_generated_ = false;

    }

    KeyGenerator::KeyGenerator(const EncryptionParameters &parms, const BigPoly &secret_key, const BigPolyArray &public_key, EvaluationKeys &evaluation_keys) :
        poly_modulus_(parms.poly_modulus()), coeff_modulus_(parms.coeff_modulus()), plain_modulus_(parms.plain_modulus()),
        noise_standard_deviation_(parms.noise_standard_deviation()), noise_max_deviation_(parms.noise_max_deviation()),
        decomposition_bit_count_(parms.decomposition_bit_count()),
        random_generator_(parms.random_generator() != nullptr ? parms.random_generator() : UniformRandomGeneratorFactory::default_factory())
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
        if (noise_standard_deviation_ < 0)
        {
            throw invalid_argument("noise_standard_deviation must be non-negative");
        }
        if (noise_max_deviation_ < 0)
        {
            throw invalid_argument("noise_max_deviation must be non-negative");
        }
        if (decomposition_bit_count_ < 0)
        {
            throw invalid_argument("decomposition_bit_count must be non-negative");
        }

        // Decomposition bit count should only be zero if evaluation keys are empty
        if (decomposition_bit_count_ == 0 && evaluation_keys.size() != 0)
        {
            throw invalid_argument("evaluation keys are not valid for encryption parameters");
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

        // Calculate -1 (mod coeff_modulus).
        coeff_modulus_minus_one_.resize(coeff_bit_count);
        decrement_uint(coeff_modulus_.pointer(), coeff_uint64_count, coeff_modulus_minus_one_.pointer());

        // Initialize evaluation_factors_, if required
        if (decomposition_bit_count_ != 0)
        {
            populate_evaluation_factors();
        }

        // Check validity of provided keys
        if (public_key.size() != 2 || public_key.coeff_count() != coeff_count || public_key.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("public_key is not correct size");
        }
        if (secret_key.coeff_count() != coeff_count || secret_key.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("secret_key is not correct size");
        }
        if (evaluation_keys.size() != 0)
        {
            int evaluation_factors_size = static_cast<int>(evaluation_factors_.size());
            for (int i = 0; i < evaluation_keys.size(); ++i)
            {
                if (evaluation_keys[i].first.size() != evaluation_factors_size || evaluation_keys[i].second.size() != evaluation_factors_size || 
                    evaluation_keys[i].first.coeff_count() != coeff_count || evaluation_keys[i].second.coeff_count() != coeff_count ||
                    evaluation_keys[i].first.coeff_bit_count() != coeff_bit_count || evaluation_keys[i].second.coeff_bit_count() != coeff_bit_count)
                {
                    throw invalid_argument("evaluation_keys are not correct size");
                }
            }
        }

        // If valid, set the provided keys
        public_key_ = public_key;
        secret_key_ = secret_key;
        evaluation_keys_ = evaluation_keys;

        MemoryPool &pool = *MemoryPool::default_pool();

        // Initialize moduli.
        polymod_ = PolyModulus(poly_modulus_.pointer(), coeff_count, coeff_uint64_count);
        mod_ = Modulus(coeff_modulus_.pointer(), coeff_uint64_count, pool);

        // Secret key and public key are generated
        is_generated_ = true;
    }

    void KeyGenerator::populate_evaluation_factors()
    {
        evaluation_factors_.clear();

        int coeff_bit_count = coeff_modulus_.bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        MemoryPool &pool = *MemoryPool::default_pool();

        // Initialize evaluation_factors_
        Pointer current_evaluation_factor(allocate_uint(coeff_uint64_count, pool));
        set_uint(1, coeff_uint64_count, current_evaluation_factor.get());
        while (!is_zero_uint(current_evaluation_factor.get(), coeff_uint64_count) && is_less_than_uint_uint(current_evaluation_factor.get(), coeff_modulus_.pointer(), coeff_uint64_count))
        {
            evaluation_factors_.push_back(BigUInt(coeff_bit_count));
            set_uint_uint(current_evaluation_factor.get(), coeff_uint64_count, evaluation_factors_.back().pointer());
            left_shift_uint(current_evaluation_factor.get(), decomposition_bit_count_, coeff_uint64_count, current_evaluation_factor.get());
        }
    }

    const BigPoly &KeyGenerator::secret_key() const
    {
        if (!is_generated_)
        {
            throw logic_error("encryption keys have not been generated");
        }
        return secret_key_;
    }

    const BigPolyArray &KeyGenerator::public_key() const
    {
        if (!is_generated_)
        {
            throw logic_error("encryption keys have not been generated");
        }
        return public_key_;
    }

    const EvaluationKeys &KeyGenerator::evaluation_keys() const
    {
        if (!is_generated_)
        {
            throw logic_error("encryption keys have not been generated");
        }
        if (evaluation_keys_.size() == 0)
        {
            throw logic_error("no evaluation keys have been generated");
        }
        return evaluation_keys_;
    }
}
