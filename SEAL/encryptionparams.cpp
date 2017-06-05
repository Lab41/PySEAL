#include "encryptionparams.h"
#include "chooser.h"
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

    EncryptionParameters::EncryptionParameters(const MemoryPoolHandle &pool) :
        pool_(pool), decomposition_bit_count_(0), 
        noise_standard_deviation_(ChooserEvaluator::default_noise_standard_deviation()), 
        noise_max_deviation_(ChooserEvaluator::default_noise_max_deviation()),
        random_generator_(nullptr), validated_(false), ntt_tables_(pool_), aux_ntt_tables_(pool_), plain_ntt_tables_(pool_)
    {
    }

    void EncryptionParameters::set_poly_modulus(const BigPoly &poly_modulus)
    {
        invalidate();
        poly_modulus_.duplicate_from(poly_modulus);
    }

    void EncryptionParameters::set_coeff_modulus(const BigUInt &coeff_modulus)
    {
        invalidate();
        coeff_modulus_.duplicate_from(coeff_modulus);
    }
#ifndef DISABLE_NTT_IN_MULTIPLY
    void EncryptionParameters::set_aux_coeff_modulus(const BigUInt &aux_coeff_modulus)
    {
        invalidate();
        aux_coeff_modulus_.duplicate_from(aux_coeff_modulus);
    }
#endif
    void EncryptionParameters::set_plain_modulus(const BigUInt &plain_modulus)
    {
        invalidate();
        plain_modulus_.duplicate_from(plain_modulus);
    }

    void EncryptionParameters::set_decomposition_bit_count(int decomposition_bit_count)
    {
        invalidate();
        decomposition_bit_count_ = decomposition_bit_count;
    }

    void EncryptionParameters::set_noise_standard_deviation(double noise_standard_deviation)
    {
        invalidate();
        noise_standard_deviation_ = noise_standard_deviation;
    }

    void EncryptionParameters::set_noise_max_deviation(double noise_max_deviation)
    {
        invalidate();
        noise_max_deviation_ = noise_max_deviation;
    }

    void EncryptionParameters::set_random_generator(UniformRandomGeneratorFactory *random_generator)
    {
        invalidate();
        random_generator_ = random_generator;
    }

    EncryptionParameterQualifiers EncryptionParameters::validate()
    {
        if (validated_)
        {
            return qualifiers_;
        }

        qualifiers_ = EncryptionParameterQualifiers();

        // Verify required parameters
        if (!poly_modulus_.is_zero() && !coeff_modulus_.is_zero() && !plain_modulus_.is_zero()
            && decomposition_bit_count_ >= 0 && noise_standard_deviation_ >= 0 && noise_max_deviation_ >= 0
            && plain_modulus_ < coeff_modulus_ && are_poly_coefficients_less_than(poly_modulus_, coeff_modulus_))
        {
            // The encryption parameters are appropriately set
            qualifiers_.parameters_set = true;
        }
        else
        {
            // Parameters have been validated but are not valid
            validated_ = true;
            qualifiers_.parameters_set = false;
            return qualifiers_;
        }

        // Next check more qualities of the moduli
        Modulus coeff_mod(coeff_modulus_.pointer(), coeff_modulus_.uint64_count());
        Modulus plain_mod(plain_modulus_.pointer(), plain_modulus_.uint64_count());
        PolyModulus poly_mod(poly_modulus_.pointer(), poly_modulus_.coeff_count(), poly_modulus_.coeff_uint64_count());

        // We will additionally require that poly_modulus is of the form x^N+1, where N is a power of two
        if (poly_mod.is_fft_modulus())
        {
            qualifiers_.enable_nussbaumer = true;
        }
        else
        {
            // Parameters have been validated but are not valid
            validated_ = true;
            qualifiers_.parameters_set = false;
            return qualifiers_;
        }

        int coeff_count_power = poly_mod.coeff_count_power_of_two();

        // Can relinearization be done? Note that also evaluation keys will have to be generated
        if (decomposition_bit_count_ > 0)
        {
            qualifiers_.enable_relinearization = true;
        }

        // Can we use NTT with coeff_modulus?
        if (ntt_tables_.generate(coeff_count_power, coeff_mod))
        {
            qualifiers_.enable_ntt = true;
        }
#ifndef DISABLE_NTT_IN_MULTIPLY
        // Can we use NTT in homomorphic multiplication?
        int coeff_bit_count = coeff_modulus_.significant_bit_count();
        int aux_coeff_bit_count = aux_coeff_modulus_.significant_bit_count();
        int aux_coeff_uint64_count = divide_round_up(aux_coeff_bit_count, bits_per_uint64);
        bool aux_coeff_large_enough = aux_coeff_bit_count > coeff_count_power + coeff_bit_count + 1;
        if (aux_coeff_large_enough && !aux_coeff_modulus_.is_zero())
        {
            Modulus aux_mod(aux_coeff_modulus_.pointer(), aux_coeff_modulus_.uint64_count());
            if (aux_ntt_tables_.generate(coeff_count_power, aux_mod))
            {
                qualifiers_.enable_ntt_in_multiply = true;
            }
        }
#endif
        // Can we use batching? (NTT with plain_modulus)
        if (plain_ntt_tables_.generate(coeff_count_power, plain_mod))
        {
            qualifiers_.enable_batching = true;
        }
        
        // Parameters have been validated and are valid
        validated_ = true;
        return qualifiers_;
    }

    void EncryptionParameters::invalidate()
    {
        if (validated_)
        {
            validated_ = false;
            qualifiers_ = EncryptionParameterQualifiers();
            ntt_tables_.reset();
            aux_ntt_tables_.reset();
            plain_ntt_tables_.reset();
        }
    }

    void EncryptionParameters::save(ostream &stream) const
    {
        poly_modulus_.save(stream);
        coeff_modulus_.save(stream);
        aux_coeff_modulus_.save(stream);
        plain_modulus_.save(stream);
        stream.write(reinterpret_cast<const char*>(&noise_standard_deviation_), sizeof(double));
        stream.write(reinterpret_cast<const char*>(&noise_max_deviation_), sizeof(double));
        int32_t decomp_bit_count32 = static_cast<int32_t>(decomposition_bit_count_);
        stream.write(reinterpret_cast<const char*>(&decomp_bit_count32), sizeof(int32_t));
    }

    void EncryptionParameters::load(istream &stream)
    {
        // Invalidate current parameters
        invalidate();

        poly_modulus_.load(stream);
        coeff_modulus_.load(stream);
        aux_coeff_modulus_.load(stream);
        plain_modulus_.load(stream);
        stream.read(reinterpret_cast<char*>(&noise_standard_deviation_), sizeof(double));
        stream.read(reinterpret_cast<char*>(&noise_max_deviation_), sizeof(double));
        int32_t decomp_bit_count32 = 0;
        stream.read(reinterpret_cast<char*>(&decomp_bit_count32), sizeof(int32_t));
        decomposition_bit_count_ = decomp_bit_count32;
    }

    void EncryptionParameters::inherent_noise_max(BigUInt &destination)
    {
        if (!qualifiers_.parameters_set)
        {
            throw logic_error("encryption parameters are not valid");
        }

        int coeff_bit_count = coeff_modulus_.significant_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Resize destination
        destination.resize(coeff_bit_count);

        // Resize plaintext modulus.
        ConstPointer plain_modulus_ptr = duplicate_uint_if_needed(plain_modulus_, coeff_uint64_count, false, pool_);

        // Compute floor of coeff_modulus/plain_modulus.
        Pointer coeff_div_plain_modulus(allocate_uint(coeff_uint64_count, pool_));
        Pointer remainder(allocate_uint(coeff_uint64_count, pool_));
        divide_uint_uint(coeff_modulus_.pointer(), plain_modulus_ptr.get(), coeff_uint64_count, coeff_div_plain_modulus.get(), remainder.get(), pool_);
        
        // For extreme parameter choices it can be that we in fact coeff_div_plain_modulus < remainder, 
        // in which case the noise bound should be 0.
        if (is_less_than_uint_uint(coeff_div_plain_modulus.get(), remainder.get(), coeff_uint64_count))
        {
            set_zero_uint(coeff_uint64_count, destination.pointer());
        }
        else
        {
            sub_uint_uint(coeff_div_plain_modulus.get(), remainder.get(), coeff_uint64_count, destination.pointer());
            right_shift_uint(destination.pointer(), 1, coeff_uint64_count, destination.pointer());
        }
    }
}
