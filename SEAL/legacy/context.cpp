#include "context.h"
#include "encryptionparams.h"
#include "util/polycore.h"
#include "util/uintarith.h"
#include "util/modulus.h"
#include "util/polymodulus.h"
#include <stdexcept>

using namespace std;
using namespace seal::util;

namespace seal
{
    namespace
    {
        ConstPointer duplicate_uint_if_needed(const BigUInt &uint, int new_uint64_count, bool force, MemoryPool &pool)
        {
            return duplicate_uint_if_needed(uint.pointer(), uint.uint64_count(), new_uint64_count, force, pool);
        }

        ConstPointer duplicate_poly_if_needed(const BigPoly &poly, int new_coeff_count, int new_coeff_uint64_count, bool force, MemoryPool &pool)
        {
            return duplicate_poly_if_needed(poly.pointer(), poly.coeff_count(), poly.coeff_uint64_count(), new_coeff_count, new_coeff_uint64_count, force, pool);
        }

        bool are_poly_coefficients_less_than(const BigPoly &poly, const BigUInt &max_coeff)
        {
            return util::are_poly_coefficients_less_than(poly.pointer(), poly.coeff_count(), poly.coeff_uint64_count(), max_coeff.pointer(), max_coeff.uint64_count());
        }
    }

    EncryptionParameterQualifiers SEALContext::validate()
    {
        qualifiers_ = EncryptionParameterQualifiers();

        // Verify required parameters
        if (!parms_.poly_modulus().is_zero() && !parms_.coeff_modulus().is_zero()
            && !parms_.plain_modulus().is_zero() && parms_.decomposition_bit_count() >= 0
            && parms_.noise_standard_deviation() >= 0 && parms_.noise_max_deviation() >= 0
            && parms_.coeff_modulus() > *parms_.plain_modulus().pointer()
            && are_poly_coefficients_less_than(parms_.poly_modulus(), parms_.coeff_modulus()))
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

        // Next check more qualities of the moduli
        Modulus coeff_mod(parms_.coeff_modulus().pointer(), parms_.coeff_modulus().uint64_count());
        SmallModulus plain_mod(parms_.plain_modulus());
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
            qualifiers_.parameters_set = false;
            return qualifiers_;
        }

        int coeff_count_power = poly_mod.coeff_count_power_of_two();

        // Can relinearization be done? Note that also evaluation keys will have to be generated
        if (parms_.decomposition_bit_count() > 0)
        {
            if (parms_.decomposition_bit_count() < parms_.coeff_modulus().bit_count())
            {
                qualifiers_.enable_relinearization = true;
            }
            else
            {
                // Parameters are not valid
                qualifiers_.parameters_set = false;
                return qualifiers_;
            }
        }

        // Can we use NTT with coeff_modulus?
        if (ntt_tables_.generate(coeff_count_power, coeff_mod))
        {
            qualifiers_.enable_ntt = true;
        }

        // Can we use batching? (NTT with plain_modulus)
        if (plain_ntt_tables_.generate(coeff_count_power, plain_mod))
        {
            qualifiers_.enable_batching = true;
        }

        // Done with validation and pre-computations
        return qualifiers_;
    }

    SEALContext::SEALContext(const EncryptionParameters &parms, const MemoryPoolHandle &pool) :
        pool_(pool), parms_(parms),
        ntt_tables_(pool), plain_ntt_tables_(pool)
    {
        // Set random generator
        if (parms_.random_generator() == nullptr)
        {
            parms_.set_random_generator(UniformRandomGeneratorFactory::default_factory());
        }

        qualifiers_ = validate();
    }
}