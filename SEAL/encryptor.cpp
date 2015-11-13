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

    void Encryptor::encrypt(const BigPoly &plain, BigPoly &destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Verify parameters.
#ifdef _DEBUG
        if (plain.significant_coeff_count() >= coeff_count || !are_poly_coefficients_less_than(plain, plain_modulus_))
        {
            throw invalid_argument("plain is too large to be represented by encryption parameters");
        }
#endif
        if (destination.coeff_count() != coeff_count || destination.coeff_bit_count() != coeff_bit_count)
        {
            destination.resize(coeff_count, coeff_bit_count);
        }

        // Handle test-mode case.
        int plain_coeff_uint64_count = divide_round_up(plain.coeff_bit_count(), bits_per_uint64);
        if (mode_ == TEST_MODE)
        {
            set_poly_poly(plain.pointer(), plain.coeff_count(), plain_coeff_uint64_count, coeff_count, coeff_uint64_count, destination.pointer());
            modulo_poly_coeffs(destination.pointer(), coeff_count, mod_, pool_);
            return;
        }

        // Multiply plain by scalar coeff_div_plaintext and reposition if in upper-half.
        preencrypt(plain.pointer(), plain.coeff_count(), plain_coeff_uint64_count, destination.pointer());

        // Multiply public_key*normal noise and add into destination.
        Pointer noise(allocate_poly(coeff_count, coeff_uint64_count, pool_));
        set_poly_coeffs_normal(noise.get());
        multiply_poly_poly_polymod_coeffmod(noise.get(), public_key_.pointer(), polymod_, mod_, noise.get(), pool_);
        add_poly_poly_coeffmod(noise.get(), destination.pointer(), coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, destination.pointer());

        // Add-in more normal noise to evaluation_key.
        set_poly_coeffs_normal(noise.get());
        add_poly_poly_coeffmod(noise.get(), destination.pointer(), coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, destination.pointer());
    }

    void Encryptor::preencrypt(const uint64_t *plain, int plain_coeff_count, int plain_coeff_uint64_count, uint64_t *destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Only care about coefficients up till coeff_count.
        if (plain_coeff_count > coeff_count)
        {
            plain_coeff_count = coeff_count;
        }

        // Multiply plain by scalar coeff_div_plaintext and reposition if in upper-half.
        if (plain == destination)
        {
            // If plain and destination are same poly, then need another storage for multiply output.
            Pointer temp(allocate_uint(coeff_uint64_count, pool_));
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
                multiply_uint_uint(plain, plain_coeff_uint64_count, coeff_div_plain_modulus_.pointer(), coeff_uint64_count, coeff_uint64_count, destination);
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

    void Encryptor::set_poly_coeffs_normal(uint64_t *poly) const
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

    Encryptor::Encryptor(const EncryptionParameters &parms, const BigPoly &public_key) :
        poly_modulus_(parms.poly_modulus()), coeff_modulus_(parms.coeff_modulus()), plain_modulus_(parms.plain_modulus()), public_key_(public_key),
        noise_standard_deviation_(parms.noise_standard_deviation()), noise_max_deviation_(parms.noise_max_deviation()), mode_(parms.mode()),
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
        if (public_key_.is_zero())
        {
            throw invalid_argument("public_key cannot be zero");
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
        if (public_key_.coeff_count() != coeff_count || public_key_.coeff_bit_count() != coeff_bit_count ||
            public_key_.significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(public_key_, coeff_modulus_))
        {
            throw invalid_argument("public_key is not valid for encryption parameters");
        }

        // Calculate coeff_modulus / plain_modulus.
        coeff_div_plain_modulus_.resize(coeff_bit_count);
        Pointer temp(allocate_uint(coeff_uint64_count, pool_));
        divide_uint_uint(coeff_modulus_.pointer(), plain_modulus_.pointer(), coeff_uint64_count, coeff_div_plain_modulus_.pointer(), temp.get(), pool_);

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
        if (mode_ == TEST_MODE)
        {
            mod_ = Modulus(plain_modulus_.pointer(), coeff_uint64_count, pool_);
        }
        else
        {
            mod_ = Modulus(coeff_modulus_.pointer(), coeff_uint64_count, pool_);
        }
    }
}
