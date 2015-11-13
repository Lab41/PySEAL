#include <algorithm>
#include <random>
#include "keygenerator.h"
#include "util/uintcore.h"
#include "util/uintarith.h"
#include "util/uintarithmod.h"
#include "util/polycore.h"
#include "util/polyarith.h"
#include "util/polyarithmod.h"
#include "util/randomtostd.h"
#include "util/clipnormal.h"
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

    void KeyGenerator::generate()
    {
        // Handle test-mode case.
        if (mode_ == TEST_MODE)
        {
            public_key_.set_zero();
            public_key_[0] = 1;
            secret_key_.set_zero();
            secret_key_[0] = 1;
            for (int i = 0; i < evaluation_keys_.count(); ++i)
            {
                evaluation_keys_[i].set_zero();
                evaluation_keys_[i][0] = 1;
            }
            return;
        }

        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Loop until find a valid secret key.
        uint64_t *secret_key = secret_key_.pointer();
        set_zero_poly(coeff_count, coeff_uint64_count, secret_key);
        Pointer secret_key_inv(allocate_poly(coeff_count, coeff_uint64_count, pool_));
        while (true)
        {
            // Create noise with random [-1, 1] coefficients.
            set_poly_coeffs_zero_one_negone(secret_key);

            // Calculate secret_key * plaintext_modulus + 1.
            multiply_poly_scalar_coeffmod(secret_key, coeff_count, plain_modulus_.pointer(), mod_, secret_key, pool_);

            uint64_t *constant_coeff = get_poly_coeff(secret_key, 0, coeff_uint64_count);
            increment_uint_mod(constant_coeff, coeff_modulus_.pointer(), coeff_uint64_count, constant_coeff);

            // Attempt to invert secret_key.
            if (try_invert_poly_coeffmod(secret_key, poly_modulus_.pointer(), coeff_count, mod_, secret_key_inv.get(), pool_))
            {
                // Secret_key is invertible, so is valid
                break;
            }
        }

        // Calculate plaintext_modulus * noise * secret_key_inv.
        Pointer noise(allocate_poly(coeff_count, coeff_uint64_count, pool_));
        set_poly_coeffs_zero_one_negone(noise.get());
        uint64_t *public_key = public_key_.pointer();
        multiply_poly_poly_polymod_coeffmod(noise.get(), secret_key_inv.get(), polymod_, mod_, noise.get(), pool_);
        multiply_poly_scalar_coeffmod(noise.get(), coeff_count, plain_modulus_.pointer(), mod_, public_key, pool_);

        // Create evaluation keys.
        Pointer evaluation_factor(allocate_uint(coeff_uint64_count, pool_));
        set_uint(1, coeff_uint64_count, evaluation_factor.get());
        for (int i = 0; i < evaluation_keys_.count(); ++i)
        {
            // Multiply secret_key by evaluation_factor (mod coeff modulus).
            uint64_t *evaluation_key = evaluation_keys_[i].pointer();
            multiply_poly_scalar_coeffmod(secret_key, coeff_count, evaluation_factor.get(), mod_, evaluation_key, pool_);

            // Multiply public_key*normal noise and add into evaluation_key.
            set_poly_coeffs_normal(noise.get());
            multiply_poly_poly_polymod_coeffmod(noise.get(), public_key, polymod_, mod_, noise.get(), pool_);
            add_poly_poly_coeffmod(noise.get(), evaluation_key, coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, evaluation_key);

            // Add-in more normal noise to evaluation_key.
            set_poly_coeffs_normal(noise.get());
            add_poly_poly_coeffmod(noise.get(), evaluation_key, coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, evaluation_key);

            // Left shift evaluation factor.
            left_shift_uint(evaluation_factor.get(), decomposition_bit_count_, coeff_uint64_count, evaluation_factor.get());
        }
    }

    void KeyGenerator::generate(const BigPoly &secret_key, int power)
    {
        // Validate arguments.
        if (secret_key.is_zero())
        {
            throw invalid_argument("secret_key cannot be zero");
        }
        if (power <= 0)
        {
            throw invalid_argument("power must be positive");
        }

        // Handle test-mode case.
        if (mode_ == TEST_MODE)
        {
            public_key_.set_zero();
            public_key_[0] = 1;
            secret_key_.set_zero();
            secret_key_[0] = 1;
            for (int i = 0; i < evaluation_keys_.count(); ++i)
            {
                evaluation_keys_[i].set_zero();
                evaluation_keys_[i][0] = 1;
            }
            return;
        }

        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Verify secret key looks valid.
        secret_key_ = secret_key;
        if (secret_key_.coeff_count() != coeff_count || secret_key_.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("secret_key is not valid for encryption parameters");
        }
#ifdef _DEBUG
        if (secret_key_.significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(secret_key_, coeff_modulus_))
        {
            throw invalid_argument("secret_key is not valid for encryption parameters");
        }
#endif

        // Raise level of secret key.
        if (power > 1)
        {
            exponentiate_poly(secret_key_.pointer(), power, polymod_, mod_, secret_key_.pointer(), pool_);
        }

        // Attempt to invert secret_key.
        Pointer secret_key_inv(allocate_poly(coeff_count, coeff_uint64_count, pool_));
        if (!try_invert_poly_coeffmod(secret_key_.pointer(), poly_modulus_.pointer(), coeff_count, mod_, secret_key_inv.get(), pool_))
        {
            // Secret_key is not invertible, so not valid.
            throw invalid_argument("secret_key is not valid for encryption parameters");
        }

        // Calculate plaintext_modulus * noise * secret_key_inv.
        Pointer noise(allocate_poly(coeff_count, coeff_uint64_count, pool_));
        set_poly_coeffs_zero_one_negone(noise.get());
        uint64_t *public_key = public_key_.pointer();
        multiply_poly_poly_polymod_coeffmod(noise.get(), secret_key_inv.get(), polymod_, mod_, noise.get(), pool_);
        multiply_poly_scalar_coeffmod(noise.get(), coeff_count, plain_modulus_.pointer(), mod_, public_key, pool_);

        // Create evaluation keys.
        Pointer evaluation_factor(allocate_uint(coeff_uint64_count, pool_));
        set_uint(1, coeff_uint64_count, evaluation_factor.get());
        for (int i = 0; i < evaluation_keys_.count(); ++i)
        {
            // Multiply secret_key by evaluation_factor (mod coeff modulus).
            uint64_t *evaluation_key = evaluation_keys_[i].pointer();
            multiply_poly_scalar_coeffmod(secret_key_.pointer(), coeff_count, evaluation_factor.get(), mod_, evaluation_key, pool_);

            // Multiply public_key*normal noise and add into evaluation_key.
            set_poly_coeffs_normal(noise.get());
            multiply_poly_poly_polymod_coeffmod(noise.get(), public_key, polymod_, mod_, noise.get(), pool_);
            add_poly_poly_coeffmod(noise.get(), evaluation_key, coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, evaluation_key);

            // Add-in more normal noise to evaluation_key.
            set_poly_coeffs_normal(noise.get());
            add_poly_poly_coeffmod(noise.get(), evaluation_key, coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, evaluation_key);

            // Left shift evaluation factor.
            left_shift_uint(evaluation_factor.get(), decomposition_bit_count_, coeff_uint64_count, evaluation_factor.get());
        }
    }

    void KeyGenerator::set_poly_coeffs_zero_one_negone(uint64_t *poly) const
    {
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        RandomToStandardAdapter engine(random_generator_.get());
        uniform_int_distribution<int> random(-1, 1);
        for (int i = 0; i < coeff_count - 1; ++i)
        {
            int rand_index = random(engine);
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

    void KeyGenerator::set_poly_coeffs_normal(uint64_t *poly) const
    {
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        if (noise_standard_deviation_ == 0 || noise_max_deviation_ == 0)
        {
            set_zero_poly(coeff_count, coeff_uint64_count, poly);
            return;
        }
        RandomToStandardAdapter engine(random_generator_.get());
        ClippedNormalDistribution random(0, noise_standard_deviation_, noise_max_deviation_);
        for (int i = 0; i < coeff_count - 1; ++i)
        {
            int64_t noise = static_cast<int64_t>(random(engine));
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

    KeyGenerator::KeyGenerator(const EncryptionParameters &parms) :
        poly_modulus_(parms.poly_modulus()), coeff_modulus_(parms.coeff_modulus()), plain_modulus_(parms.plain_modulus()),
        noise_standard_deviation_(parms.noise_standard_deviation()), noise_max_deviation_(parms.noise_max_deviation()),
        decomposition_bit_count_(parms.decomposition_bit_count()), mode_(parms.mode()),
        random_generator_(parms.random_generator() != nullptr ? parms.random_generator()->create() : UniformRandomGeneratorFactory::default_factory()->create())
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
        if (decomposition_bit_count_ <= 0)
        {
            throw invalid_argument("decomposition_bit_count must be positive");
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
        public_key_.resize(coeff_count, coeff_bit_count);
        secret_key_.resize(coeff_count, coeff_bit_count);

        // Initialize evaluation keys.
        int evaluation_key_count = 0;
        Pointer evaluation_factor(allocate_uint(coeff_uint64_count, pool_));
        set_uint(1, coeff_uint64_count, evaluation_factor.get());
        while (!is_zero_uint(evaluation_factor.get(), coeff_uint64_count) && is_less_than_uint_uint(evaluation_factor.get(), coeff_modulus_.pointer(), coeff_uint64_count))
        {
            left_shift_uint(evaluation_factor.get(), decomposition_bit_count_, coeff_uint64_count, evaluation_factor.get());
            evaluation_key_count++;
        }
        evaluation_keys_.resize(evaluation_key_count);
        for (int i = 0; i < evaluation_key_count; ++i)
        {
            evaluation_keys_[i].resize(coeff_count, coeff_bit_count);
        }

        // Initialize moduli.
        polymod_ = PolyModulus(poly_modulus_.pointer(), coeff_count, coeff_uint64_count);
        mod_ = Modulus(coeff_modulus_.pointer(), coeff_uint64_count, pool_);
    }
}
