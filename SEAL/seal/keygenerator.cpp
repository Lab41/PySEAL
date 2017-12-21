#include <algorithm>
#include <random>
#include "seal/keygenerator.h"
#include "seal/util/uintcore.h"
#include "seal/util/uintarith.h"
#include "seal/util/uintarithsmallmod.h"
#include "seal/util/polyarithsmallmod.h"
#include "seal/util/polyfftmultmod.h"
#include "seal/util/randomtostd.h"
#include "seal/util/clipnormal.h"
#include "seal/util/polycore.h"
#include "seal/util/smallntt.h"

using namespace std;
using namespace seal::util;

namespace seal
{
    KeyGenerator::KeyGenerator(const SEALContext &context, const MemoryPoolHandle &pool) :
        pool_(pool), parms_(context.parms()),
        qualifiers_(context.qualifiers()),
        random_generator_(parms_.random_generator())
    {
        // Verify parameters
        if (!qualifiers_.parameters_set)
        {
            throw invalid_argument("encryption parameters are not set correctly");
        }
        if (!pool)
        {
            throw invalid_argument("pool is uninitialized");
        }

        // Resize encryption parameters to consistent size.
        int coeff_count = parms_.poly_modulus().coeff_count();
        int poly_coeff_uint64_count = parms_.poly_modulus().coeff_uint64_count();
        int coeff_mod_count = parms_.coeff_modulus().size();

        for (int i = 0; i < coeff_mod_count; i++)
        {
            small_ntt_tables_.emplace_back(util::SmallNTTTables(pool_));

        }
        small_ntt_tables_ = context.small_ntt_tables_;

        // Initialize public and secret key.
        public_key_.mutable_data().resize(2, coeff_count, coeff_mod_count * bits_per_uint64);
        secret_key_.mutable_data().resize(coeff_count, coeff_mod_count * bits_per_uint64);

        // Initialize moduli.
        polymod_ = PolyModulus(parms_.poly_modulus().pointer(), coeff_count, poly_coeff_uint64_count);

        // Secret key and public key have not been generated
        generated_ = false;

        // Generate the secret and public key
        generate();
    }

    KeyGenerator::KeyGenerator(const SEALContext &context, const SecretKey &secret_key, const PublicKey &public_key, const MemoryPoolHandle &pool) :
        pool_(pool), parms_(context.parms()), qualifiers_(context.qualifiers()),
        public_key_(public_key), secret_key_(secret_key),
        random_generator_(parms_.random_generator())
    {
        // Verify parameters
        if (!qualifiers_.parameters_set)
        {
            throw invalid_argument("encryption parameters are not set correctly");
        }
        if (secret_key.hash_block() != parms_.hash_block())
        {
            throw invalid_argument("secret_key is not valid for encryption parameters");
        }
        if (public_key.hash_block() != parms_.hash_block())
        {
            throw invalid_argument("public_key is not valid for encryption parameters");
        }
        if (!pool)
        {
            throw invalid_argument("pool is uninitialized");
        }

        // Resize encryption parameters to consistent size.
        int coeff_count = parms_.poly_modulus().coeff_count();
        int poly_coeff_uint64_count = parms_.poly_modulus().coeff_uint64_count();
        int coeff_mod_count = parms_.coeff_modulus().size();

        // Set SmallNTTTables
        small_ntt_tables_.resize(coeff_mod_count, pool_);
        small_ntt_tables_ = context.small_ntt_tables_;

        // Initialize public and secret key.
        public_key_.mutable_data().resize(2, coeff_count, coeff_mod_count);
        secret_key_.mutable_data().resize(coeff_count, coeff_mod_count);

        // Initialize moduli.
        polymod_ = PolyModulus(parms_.poly_modulus().pointer(), coeff_count, poly_coeff_uint64_count);

        // Secret key and public key are generated
        generated_ = true;
    }

    void KeyGenerator::generate()
    {
        // If already generated, reset everything.
        if (generated_)
        {
            secret_key_.mutable_data().set_zero();
            public_key_.mutable_data().set_zero();
            generated_ = false;
        }

        // Extract encryption parameters.
        int coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_mod_count = parms_.coeff_modulus().size();

        unique_ptr<UniformRandomGenerator> random(random_generator_->create());

        // Generate secret key
        uint64_t *secret_key = secret_key_.mutable_data().pointer();
        set_poly_coeffs_zero_one_negone(secret_key, random.get());

        // Generate public key: (pk[0],pk[1]) = ([-(as+e)]_q, a)

        // Sample a uniformly at random
        // Set pk[1] = a
        uint64_t *public_key_1 = public_key_.mutable_data().pointer(1);
        set_poly_coeffs_uniform(public_key_1, random.get());

        // calculate a*s + e (mod q) and store in pk[0]
        for (int i = 0; i < coeff_mod_count; i++)
        {
            // Transform the secret s into NTT representation. 
            ntt_negacyclic_harvey(secret_key + (i * coeff_count), small_ntt_tables_[i]);

            // Transform the uniform random polynomial a into NTT representation. 
            ntt_negacyclic_harvey_lazy(public_key_1 + (i * coeff_count), small_ntt_tables_[i]);
        }

        Pointer noise(allocate_poly(coeff_count, coeff_mod_count, pool_));
        set_poly_coeffs_normal(noise.get(), random.get());
        for (int i = 0; i < coeff_mod_count; i++)
        {
            // Transform the noise e into NTT representation.
            ntt_negacyclic_harvey(noise.get() + (i * coeff_count), small_ntt_tables_[i]);

            // The inputs are not reduced but that's OK. We are only at most at 122 bits
            // and barrett_reduce_128 can deal with that.
            dyadic_product_coeffmod(secret_key + (i * coeff_count), public_key_1 + (i * coeff_count), coeff_count, 
                parms_.coeff_modulus()[i], public_key_.mutable_data().pointer(0) + (i * coeff_count));
            add_poly_poly_coeffmod(noise.get() + (i * coeff_count), public_key_.mutable_data().pointer(0) + (i * coeff_count),
                coeff_count, parms_.coeff_modulus()[i], public_key_.mutable_data().pointer(0) + (i * coeff_count));
        }

        // Negate and set this value to pk[0]
        // pk[0] is now -(as+e) mod q
        for (int i = 0; i < coeff_mod_count; i++)
        {
            negate_poly_coeffmod(public_key_.mutable_data().pointer(0) + (i * coeff_count), coeff_count, parms_.coeff_modulus()[i], public_key_.mutable_data().pointer(0) + (i * coeff_count));
        }

        // Set the secret_key_array to have size 1 (first power of secret) 
        secret_key_array_ = allocate_poly(coeff_count, coeff_mod_count, pool_);
        set_poly_poly(secret_key_.data().pointer(), coeff_count, coeff_mod_count, secret_key_array_.get());
        secret_key_array_size_ = 1;

        // Set the parameter hashes for public and secret key
        public_key_.mutable_hash_block() = parms_.hash_block();
        secret_key_.mutable_hash_block() = parms_.hash_block();
        
        // Secret and public keys have been generated
        generated_ = true;
    }

    void KeyGenerator::generate_evaluation_keys(int decomposition_bit_count, int count, EvaluationKeys &evaluation_keys)
    {
        // Check to see if secret key and public key have been generated
        if (!generated_)
        {
            throw logic_error("cannot generate evaluation keys for unspecified secret key");
        }

        // Validate parameters
        if (count <= 0)
        {
            throw invalid_argument("count must be positive");
        }

        // Check that decomposition_bit_count is in correct interval
        if (decomposition_bit_count < SEAL_DBC_MIN || decomposition_bit_count > SEAL_DBC_MAX)
        {
            throw invalid_argument("decomposition_bit_count is not in the valid range");
        }

        // Clear current evaluation keys
        evaluation_keys.mutable_data().clear();

        // Extract encryption parameters.
        int coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_mod_count = parms_.coeff_modulus().size();

        // Initialize decomposition_factors
        vector<vector<uint64_t> > decomposition_factors;
        populate_decomposition_factors(decomposition_bit_count, decomposition_factors);

        // Initialize the evaluation keys
        evaluation_keys.mutable_data().resize(count);
        for (int i = 0; i < count; i++)
        {
            evaluation_keys.mutable_data()[i].reserve(coeff_mod_count);
            for (int j = 0; j < coeff_mod_count; j++)
            {
                // Use the global memory pool for key allocation
                evaluation_keys.mutable_data()[i].emplace_back(
                    Ciphertext(parms_, 2 * decomposition_factors[j].size(), MemoryPoolHandle::Global()));

                // Resize to right size too (above only allocated)
                // This is slightly odd use of Ciphertext as container
                evaluation_keys.mutable_data()[i].back().resize(2 * decomposition_factors[j].size());
            }
        }

        unique_ptr<UniformRandomGenerator> random(random_generator_->create());

        // Create evaluation keys.
        Pointer noise(allocate_poly(coeff_count, coeff_mod_count, pool_));
        Pointer temp(allocate_uint(coeff_count, pool_));

        // Make sure we have enough secret keys computed
        compute_secret_key_array(count + 1);

        // assume the secret key is already transformed into NTT form. 
        for (int k = 0; k < count; k++)
        {
            for (int l = 0; l < coeff_mod_count; l++)
            {
                // populate evaluate_keys_[k]
                for (int i = 0; i < decomposition_factors[l].size(); i++)
                {
                    // generate NTT(a_i) and store in evaluation_keys_[k][l].second[i]
                    uint64_t *eval_keys_first = evaluation_keys.mutable_data()[k][l].mutable_pointer(2 * i);
                    uint64_t *eval_keys_second = evaluation_keys.mutable_data()[k][l].mutable_pointer(2 * i + 1);

                    set_poly_coeffs_uniform(eval_keys_second, random.get());
                    for (int j = 0; j < coeff_mod_count; j++)
                    {
                        ntt_negacyclic_harvey_lazy(eval_keys_second + (j * coeff_count), small_ntt_tables_[j]);

                        // calculate a_i*s and store in evaluation_keys_[k].first[i]
                        dyadic_product_coeffmod(eval_keys_second + (j * coeff_count), 
                            secret_key_.mutable_data().pointer() + (j * coeff_count), 
                            coeff_count, parms_.coeff_modulus()[j], eval_keys_first + (j * coeff_count));
                    }

                    // generate NTT(e_i) 
                    set_poly_coeffs_normal(noise.get(), random.get());
                    for (int j = 0; j < coeff_mod_count; j++)
                    {
                        ntt_negacyclic_harvey(noise.get() + (j * coeff_count), small_ntt_tables_[j]);

                        // add e_i into evaluation_keys_[k].first[i]
                        add_poly_poly_coeffmod(noise.get() + (j * coeff_count), eval_keys_first + (j * coeff_count), 
                            coeff_count, parms_.coeff_modulus()[j], eval_keys_first + (j * coeff_count));

                        // negate value in evaluation_keys_[k].first[i]
                        negate_poly_coeffmod(eval_keys_first + (j * coeff_count), coeff_count, parms_.coeff_modulus()[j], 
                            eval_keys_first + (j * coeff_count));

                        //multiply w^i * s^(k+2)
                        uint64_t decomposition_factor_mod = decomposition_factors[l][i] & static_cast<uint64_t>(-static_cast<int64_t>(l == j));
                        multiply_poly_scalar_coeffmod(secret_key_array_.get() + (k + 1) * coeff_count * coeff_mod_count + (j * coeff_count), 
                            coeff_count, decomposition_factor_mod, parms_.coeff_modulus()[j], temp.get());

                        //add w^i . s^(k+2) into evaluation_keys_[k].first[i]
                        add_poly_poly_coeffmod(eval_keys_first + (j * coeff_count), temp.get(), coeff_count, 
                            parms_.coeff_modulus()[j], eval_keys_first + (j * coeff_count));
                    }
                }
            }
        }

        // Set decomposition_bit_count
        evaluation_keys.decomposition_bit_count_ = decomposition_bit_count;

        // Set the parameter hash
        evaluation_keys.mutable_hash_block() = parms_.hash_block();
    }

    void KeyGenerator::generate_galois_keys(int decomposition_bit_count, const vector<uint64_t> &galois_elts, GaloisKeys &galois_keys)
    {
        // Check to see if secret key and public key have been generated
        if (!generated_)
        {
            throw logic_error("cannot generate galois keys for unspecified secret key");
        }
        if (!qualifiers_.enable_batching)
        {
            throw logic_error("encryption parameters are not valid for batching");
        }

        // Check that decomposition_bit_count is in correct interval
        if (decomposition_bit_count < SEAL_DBC_MIN || decomposition_bit_count > SEAL_DBC_MAX)
        {
            throw invalid_argument("decomposition_bit_count is not on the valid range");
        }

        // Clear the current keys
        galois_keys.mutable_data().clear();

        // Extract encryption parameters.
        int coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_mod_count = parms_.coeff_modulus().size();

        // The max number of keys is equal to number of coefficients
        galois_keys.mutable_data().resize(coeff_count);

        // Initialize decomposition_factors
        vector<vector<uint64_t> > decomposition_factors;
        populate_decomposition_factors(decomposition_bit_count, decomposition_factors);

        for (uint64_t galois_elt : galois_elts)
        {
            // Verify coprime conditions.
            if (!(galois_elt & 1) || (galois_elt >= 2 * (coeff_count - 1)))
            {
                throw invalid_argument("galois element is not valid");
            }

            // Do we already have the key?
            if (galois_keys.has_key(galois_elt))
            {
                continue;
            }

            // Rotate secret key for each coeff_modulus
            Pointer rotated_secret_key(allocate_poly(coeff_count, coeff_mod_count, pool_));
            for (int i = 0; i < coeff_mod_count; i++)
            {
                // Permute_ntt_poly_smallmod(secret_key_.data().pointer() + i * coeff_count, coeff_count - 1, galois_elt, 
                // rotated_secret_key.get() + i * coeff_count);
                inverse_ntt_negacyclic_harvey(secret_key_.mutable_data().pointer() + i * coeff_count, small_ntt_tables_[i]);
                apply_galois(secret_key_.mutable_data().pointer() + i * coeff_count, get_power_of_two(coeff_count - 1), galois_elt, 
                    parms_.coeff_modulus()[i], rotated_secret_key.get() + i * coeff_count);
                ntt_negacyclic_harvey(secret_key_.mutable_data().pointer() + i * coeff_count, small_ntt_tables_[i]);
                ntt_negacyclic_harvey(rotated_secret_key.get() + (i * coeff_count), small_ntt_tables_[i]);
            }

            // Initialize galois key
            // This is the location in the galois_keys vector
            uint64_t index = (galois_elt - 1) >> 1;
            galois_keys.mutable_data()[index].reserve(coeff_mod_count);
            for (int i = 0; i < coeff_mod_count; i++)
            {
                // Use the global memory pool for key allocation
                galois_keys.mutable_data()[index].emplace_back(
                    Ciphertext(parms_, 2 * decomposition_factors[i].size(), MemoryPoolHandle::Global()));

                // Resize to right size too (above only allocated)
                // This is slightly odd use of Ciphertext as container
                galois_keys.mutable_data()[index].back().resize(2 * decomposition_factors[i].size());
            }

            unique_ptr<UniformRandomGenerator> random(random_generator_->create());

            // Create evaluation keys.
            Pointer noise(allocate_poly(coeff_count, coeff_mod_count, pool_));
            Pointer temp(allocate_uint(coeff_count, pool_));

            for (int l = 0; l < coeff_mod_count; l++)
            {
                //populate evaluate_keys_[k]
                for (int i = 0; i < decomposition_factors[l].size(); i++)
                {
                    //generate NTT(a_i) and store in evaluation_keys_[k][l].second[i]
                    uint64_t *eval_keys_first = galois_keys.mutable_data()[index][l].mutable_pointer(2 * i);
                    uint64_t *eval_keys_second = galois_keys.mutable_data()[index][l].mutable_pointer(2 * i + 1);

                    set_poly_coeffs_uniform(eval_keys_second, random.get());
                    for (int j = 0; j < coeff_mod_count; j++)
                    {
                        // a_i in NTT form
                        ntt_negacyclic_harvey(eval_keys_second + (j * coeff_count), small_ntt_tables_[j]);
                        // calculate a_i*s and store in evaluation_keys_[k].first[i]
                        dyadic_product_coeffmod(eval_keys_second + (j * coeff_count), secret_key_.data().pointer() + (j * coeff_count), 
                            coeff_count, parms_.coeff_modulus()[j], eval_keys_first + (j * coeff_count));
                    }

                    //generate NTT(e_i) 
                    set_poly_coeffs_normal(noise.get(), random.get());
                    for (int j = 0; j < coeff_mod_count; j++)
                    {
                        ntt_negacyclic_harvey(noise.get() + (j * coeff_count), small_ntt_tables_[j]);

                        //add NTT(e_i) into evaluation_keys_[k].first[i]
                        add_poly_poly_coeffmod(noise.get() + (j * coeff_count), eval_keys_first + (j * coeff_count), 
                            coeff_count, parms_.coeff_modulus()[j], eval_keys_first + (j * coeff_count));

                        // negate value in evaluation_keys_[k].first[i]
                        negate_poly_coeffmod(eval_keys_first + (j * coeff_count), coeff_count, parms_.coeff_modulus()[j], 
                            eval_keys_first + (j * coeff_count));

                        //multiply w^i * rotated_secret_key
                        uint64_t decomposition_factor_mod = decomposition_factors[l][i] & static_cast<uint64_t>(-static_cast<int64_t>(l == j));
                        multiply_poly_scalar_coeffmod(rotated_secret_key.get() + (j * coeff_count), coeff_count, decomposition_factor_mod, 
                            parms_.coeff_modulus()[j], temp.get());

                        //add w^i * rotated_secret_key into evaluation_keys_[k].first[i]
                        add_poly_poly_coeffmod(eval_keys_first + (j * coeff_count), temp.get(), coeff_count, parms_.coeff_modulus()[j], 
                            eval_keys_first + (j * coeff_count));
                    }
                }
            }
        }

        // Set decomposition_bit_count
        galois_keys.decomposition_bit_count_ = decomposition_bit_count;

        // Set the parameter hash
        galois_keys.hash_block_ = parms_.hash_block();
    }

    void KeyGenerator::generate_galois_keys(int decomposition_bit_count, GaloisKeys &galois_keys)
    {
        // Check to see if secret key and public key have been generated
        if (!generated_)
        {
            throw logic_error("cannot generate galois keys for unspecified secret key");
        }
        if (!qualifiers_.enable_batching)
        {
            throw logic_error("encryption parameters are not valid for batching");
        }

        // Check that decomposition_bit_count is in correct interval
        if (decomposition_bit_count < SEAL_DBC_MIN || decomposition_bit_count > SEAL_DBC_MAX)
        {
            throw invalid_argument("decomposition_bit_count is not in the valid range");
        }

        int coeff_count = parms_.poly_modulus().coeff_count();
        int n = coeff_count - 1;
        int m = n << 1;
        int logn = get_power_of_two(n);

        vector<uint64_t> logn_galois_keys{};

        // Generate Galois keys for m - 1 (X -> X^{m-1})
        logn_galois_keys.push_back(m - 1);

        // Generate Galois key for power of 3 mod m (X -> X^{3^k}) and
        // for negative power of 3 mod m (X -> X^{-3^k}) 
        uint64_t two_power_of_three = 3;
        uint64_t neg_two_power_of_three;
        try_mod_inverse(3, m, neg_two_power_of_three);
        for (int i = 0; i < logn - 1; i++)
        {
            logn_galois_keys.push_back(two_power_of_three);
            two_power_of_three *= two_power_of_three;
            two_power_of_three &= (m - 1);

            logn_galois_keys.push_back(neg_two_power_of_three);
            neg_two_power_of_three *= neg_two_power_of_three;
            neg_two_power_of_three &= (m - 1);
        }

        generate_galois_keys(decomposition_bit_count, logn_galois_keys, galois_keys);
    }

    void KeyGenerator::set_poly_coeffs_zero_one_negone(uint64_t *poly, UniformRandomGenerator *random) const
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

    void KeyGenerator::set_poly_coeffs_normal(uint64_t *poly, UniformRandomGenerator *random) const
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

    /*Set the coeffs of a BigPoly to be uniform modulo coeff_mod*/
    void KeyGenerator::set_poly_coeffs_uniform(uint64_t *poly, UniformRandomGenerator *random)
    {
        //get parameters
        int coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_mod_count = parms_.coeff_modulus().size();

        // set up source of randomness which produces random things of size 32 bit
        RandomToStandardAdapter engine(random);

        // pointer to increment to fill in all coeffs
        uint32_t *value_ptr = reinterpret_cast<uint32_t *> (poly);

        // number of 32 bit blocks to cover all coeffs
        int significant_value_uint32_count = (coeff_count - 1) * 2;
        int value_uint32_count = significant_value_uint32_count + 2;

        // Sample randomness to all but last coefficient
        for (int j = 0; j < coeff_mod_count; j++)
        {
            for (int i = 0; i < value_uint32_count; i++)
            {
                *value_ptr++ = i < significant_value_uint32_count ? engine() : 0;
            }
        }
        
        // when poly is fully populated, reduce all coefficient modulo coeff_modulus
        for (int i = 0; i < coeff_mod_count; i++)
        {
            modulo_poly_coeffs(poly + (i * coeff_count), coeff_count, parms_.coeff_modulus()[i], poly + (i * coeff_count));
        }
    }

    const SecretKey &KeyGenerator::secret_key() const
    {
        if (!generated_)
        {
            throw logic_error("encryption keys have not been generated");
        }
        return secret_key_;
    }

    const PublicKey &KeyGenerator::public_key() const
    {
        if (!generated_)
        {
            throw logic_error("encryption keys have not been generated");
        }
        return public_key_;
    }

    void KeyGenerator::compute_secret_key_array(int max_power)
    {
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

    // decomposition_factors[i][j] = 2^(w*j) * hat-q_i mod q_i
    void KeyGenerator::populate_decomposition_factors(int decomposition_bit_count, vector<vector<uint64_t> > &decomposition_factors)
    {
        decomposition_factors.clear();

        // Initialize evaluation_factors_
        int coeff_mod_count = parms_.coeff_modulus().size();
        decomposition_factors.resize(coeff_mod_count);
        uint64_t power_of_w = 1ULL << decomposition_bit_count;

        // Compute hat-q_i mod q_i
        vector<uint64_t> coeff_prod_mod(coeff_mod_count);
        for (int i = 0; i < coeff_mod_count; i++)
        {
            coeff_prod_mod[i] = 1;
            for (int j = 0; j < coeff_mod_count; j++)
            {
                if (i != j)
                {
                    coeff_prod_mod[i] = multiply_uint_uint_mod(coeff_prod_mod[i], parms_.coeff_modulus()[j].value(), parms_.coeff_modulus()[i]);
                }
            }
        }

        for (int i = 0; i < coeff_mod_count; i++)
        {
            uint64_t current_decomposition_factor = coeff_prod_mod[i];
            uint64_t current_smallmod = parms_.coeff_modulus()[i].value();
            while (current_smallmod != 0)
            {
                decomposition_factors[i].emplace_back(current_decomposition_factor);
                //multiply 2^w mod q_i
                current_decomposition_factor = multiply_uint_uint_mod(current_decomposition_factor, power_of_w, parms_.coeff_modulus()[i]);
                current_smallmod >>= decomposition_bit_count;
            }
        }

        // We need to ensure that the total number of evaluation factors does not exceed 63 for
        // lazy reduction in relinearization to work
        int total_ev_factor_count = 0;
        for (int i = 0; i < decomposition_factors.size(); i++)
        {
            total_ev_factor_count += decomposition_factors[i].size();
        }
        if (total_ev_factor_count > 63)
        {
            throw invalid_argument("decomposition_bit_count is too small");
        }
    }
}
