#include "seal/context.h"
#include "seal/util/polycore.h"
#include "seal/util/uintarith.h"
#include "seal/util/modulus.h"
#include "seal/util/polymodulus.h"
#include "seal/util/numth.h"
#include "seal/defaultparams.h"
#include <stdexcept>

using namespace std;
using namespace seal::util;

namespace seal
{
    EncryptionParameterQualifiers SEALContext::validate()
    {
        qualifiers_ = EncryptionParameterQualifiers();
        int coeff_mod_count = parms_.coeff_modulus().size();
        int coeff_count = parms_.poly_modulus().coeff_count();

        // The number of coeff moduli is restricted to 62 for lazy reductions in baseconverter.cpp to work
        if (coeff_mod_count == 0 || coeff_mod_count > SEAL_COEFF_MOD_COUNT_BOUND)
        {
            qualifiers_.parameters_set = false;
            return qualifiers_;
        }

        // Plain modulus must be at least 2 and at most 60 bits
        if (parms_.plain_modulus().value() < 2 ||
            parms_.plain_modulus().value() >> SEAL_USER_MODULO_BIT_BOUND)
        {
            qualifiers_.parameters_set = false;
            return qualifiers_;
        }

        for (int i = 0; i < coeff_mod_count; i++)
        {
            // Coeff moduli must be at least 2 and at most USER_MODULO_BOUND bits
            if (parms_.coeff_modulus()[i].value() >> SEAL_USER_MODULO_BIT_BOUND ||
                parms_.coeff_modulus()[i].value() < 2)
            {
                qualifiers_.parameters_set = false;
                return qualifiers_;
            }

            // Check that all coeff moduli are pairwise relatively prime
            for (int j = 0; j < i; j++)
            {
                if (gcd(parms_.coeff_modulus()[i].value(), parms_.coeff_modulus()[j].value()) > 1)
                {
                    qualifiers_.parameters_set = false;
                    return qualifiers_;
                }
            }

            // Check that all coeff moduli are relatively prime to plain_modulus
            if (gcd(parms_.coeff_modulus()[i].value(), parms_.plain_modulus().value()) > 1)
            {
                qualifiers_.parameters_set = false;
                return qualifiers_;
            }
        }

        // Compute the product of all coeff moduli
        total_coeff_modulus_.resize(coeff_mod_count * bits_per_uint64);
        Pointer tmp_products_all(allocate_uint(coeff_mod_count, pool_));
        set_uint(1, coeff_mod_count, total_coeff_modulus_.pointer());
        for (int i = 0; i < coeff_mod_count; i++)
        {
            multiply_uint_uint64(total_coeff_modulus_.pointer(), coeff_mod_count, parms_.coeff_modulus()[i].value(), coeff_mod_count, tmp_products_all.get());
            set_uint_uint(tmp_products_all.get(), coeff_mod_count, total_coeff_modulus_.pointer());
        }

        // Check that plain_modulus is smaller than total coeff modulus
        if (!is_less_than_uint_uint(parms_.plain_modulus().pointer(), parms_.plain_modulus().uint64_count(), total_coeff_modulus_.pointer(), coeff_mod_count))
        {
            // Parameters are not valid
            qualifiers_.parameters_set = false;
            return qualifiers_;
        }

        // Check polynomial modulus
        if (parms_.poly_modulus().is_zero())
        {
            // Parameters are not valid
            qualifiers_.parameters_set = false;
            return qualifiers_;
        }
        PolyModulus poly_mod(parms_.poly_modulus().pointer(), parms_.poly_modulus().coeff_count(),
            parms_.poly_modulus().coeff_uint64_count());

        // We will additionally require that poly_modulus is of the form x^N+1, where N is a power of two
        if (poly_mod.is_fft_modulus())
        {
            qualifiers_.enable_fft = true;
        }
        else
        {
            // Parameters are not valid
            qualifiers_.enable_fft = false;
            qualifiers_.parameters_set = false;
            return qualifiers_;
        }

        // Verify that noise_standard_deviation is positive
        if (parms_.noise_standard_deviation() >= 0 &&
            parms_.noise_max_deviation() >= 0)
        {
            // The parameters look good so far
            qualifiers_.parameters_set = true;
        }
        else
        {
            // Parameters are not valid
            qualifiers_.parameters_set = false;
            return qualifiers_;
        }

        int coeff_count_power = poly_mod.coeff_count_power_of_two();

        // Can we use NTT with coeff_modulus?
        qualifiers_.enable_ntt = true;
        for (int i = 0; i < coeff_mod_count; i++)
        {
            if (!small_ntt_tables_[i].generate(coeff_count_power, parms_.coeff_modulus()[i]))
            {
                // Parameters are not valid
                qualifiers_.enable_ntt = false;
                qualifiers_.parameters_set = false;
                return qualifiers_;
            }
        }

        // Can we use batching? (NTT with plain_modulus)
        qualifiers_.enable_batching = false;
        if (plain_ntt_tables_.generate(coeff_count_power, parms_.plain_modulus()))
        {
            qualifiers_.enable_batching = true;
        }

        base_converter_ = BaseConverter(parms_.coeff_modulus(), coeff_count, coeff_count_power, parms_.plain_modulus(), pool_);
        if (!base_converter_.is_generated())
        {
            // Parameters are not valid
            qualifiers_.parameters_set = false;
            return qualifiers_;
        }

        // Check for plain_lift 
        // If all the small coefficient moduli are larger than plain modulus, we can quickly lift plain coefficients to RNS form
        qualifiers_.enable_fast_plain_lift = true;
        for (int i = 0; i < coeff_mod_count; i++)
        {
            if (parms_.coeff_modulus()[i].value() <= parms_.plain_modulus().value())
            {
                qualifiers_.enable_fast_plain_lift = false;
            }
        }

        // Done with validation and pre-computations
        return qualifiers_;
    }

    SEALContext::SEALContext(const EncryptionParameters &parms, const MemoryPoolHandle &pool) :
        pool_(pool), parms_(parms), base_converter_(pool_), plain_ntt_tables_(pool_) 
    {
        if (!pool)
        {
            throw invalid_argument("pool is uninitialized");
        }

        int coeff_mod_count = parms_.coeff_modulus().size();
        small_ntt_tables_.resize(coeff_mod_count, pool_);

        // Set random generator
        if (parms_.random_generator() == nullptr)
        {
            parms_.set_random_generator(UniformRandomGeneratorFactory::default_factory());
        }

        qualifiers_ = validate();
    }
}
