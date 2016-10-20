#include "encryptionparams.h"
#include "chooser.h"
#include "util/polycore.h"
#include "util/uintarith.h"
#include "util/modulus.h"
#include "util/polymodulus.h"

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

    EncryptionParameters::EncryptionParameters() :
        noise_standard_deviation_(ChooserEvaluator::default_noise_standard_deviation()), 
        noise_max_deviation_(ChooserEvaluator::default_noise_max_deviation()),
        decomposition_bit_count_(0), random_generator_(nullptr)
    {
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

    void EncryptionParameters::inherent_noise_max(BigUInt &destination) const
    {
        EncryptionParameterQualifiers qualifiers;

        // Test that all of the parameters are of appropriate form
        int coeff_bit_count = coeff_modulus_.significant_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Verify required parameters
        if (!poly_modulus_.is_zero() && !coeff_modulus_.is_zero() && !plain_modulus_.is_zero()
            && decomposition_bit_count_ >= 0 && noise_standard_deviation_ >= 0 && noise_max_deviation_ >= 0
            && plain_modulus_ < coeff_modulus_ && are_poly_coefficients_less_than(poly_modulus_, coeff_modulus_))
        {
            // The parameters are appropriately set
            qualifiers.parameters_set = true;
        }
        else
        {
            qualifiers.parameters_set = false;
        }

        // Next check more qualities of the moduli
        Modulus coeff_mod(coeff_modulus_.pointer(), coeff_modulus_.uint64_count());
        PolyModulus poly_mod(poly_modulus_.pointer(), poly_modulus_.coeff_count(), poly_modulus_.coeff_uint64_count());

        // We will additionally require that poly_modulus is of the form x^N+1, where N is a power of two
        if (poly_mod.is_fft_modulus())
        {
            qualifiers.enable_nussbaumer = true;
        }
        else
        {
            qualifiers.parameters_set = false;
        }

        if (!qualifiers.parameters_set)
        {
            throw logic_error("EncryptionParameters are not valid");
        }

        // Resize destination
        destination.resize(coeff_bit_count);

        // Resize plaintext modulus.
        MemoryPool &pool = *MemoryPool::default_pool();
        ConstPointer plain_modulus_ptr = duplicate_uint_if_needed(plain_modulus_, coeff_uint64_count, false, pool);

        // Compute floor of coeff_modulus/plain_modulus.
        Pointer coeff_div_plain_modulus(allocate_uint(coeff_uint64_count, pool));
        Pointer remainder(allocate_uint(coeff_uint64_count, pool));
        divide_uint_uint(coeff_modulus_.pointer(), plain_modulus_ptr.get(), coeff_uint64_count, coeff_div_plain_modulus.get(), remainder.get(), pool);
        sub_uint_uint(coeff_div_plain_modulus.get(), remainder.get(), coeff_uint64_count, destination.pointer());
        right_shift_uint(destination.pointer(), 1, coeff_uint64_count, destination.pointer());
    }

    EncryptionParameterQualifiers EncryptionParameters::get_qualifiers() const
    {
        EncryptionParameterQualifiers qualifiers;

        // Verify required parameters
        if (!poly_modulus_.is_zero() && !coeff_modulus_.is_zero() && !plain_modulus_.is_zero()
            && decomposition_bit_count_ >= 0 && noise_standard_deviation_ >= 0 && noise_max_deviation_ >= 0
            && plain_modulus_ < coeff_modulus_ && are_poly_coefficients_less_than(poly_modulus_, coeff_modulus_))
        {
            // The parameters are appropriately set
            qualifiers.parameters_set = true;
        }
        else
        {
            qualifiers.parameters_set = false;
            return qualifiers;
        }

        // Next check more qualities of the moduli
        Modulus coeff_mod(coeff_modulus_.pointer(), coeff_modulus_.uint64_count());
        Modulus plain_mod(plain_modulus_.pointer(), plain_modulus_.uint64_count());
        PolyModulus poly_mod(poly_modulus_.pointer(), poly_modulus_.coeff_count(), poly_modulus_.coeff_uint64_count());

        // We will additionally require that poly_modulus is of the form x^N+1, where N is a power of two
        if (poly_mod.is_fft_modulus())
        {
            qualifiers.enable_nussbaumer = true;
        }
        else
        {
            qualifiers.parameters_set = false;
            return qualifiers;
        }

        int coeff_count_power = poly_mod.coeff_count_power_of_two();

        // Can relinearization be done? Note that also evaluation keys will have to be generated
        if (decomposition_bit_count_ > 0)
        {
            qualifiers.enable_relinearization = true;
        }

        NTTTables ntt_tables;

        // Can we use NTT with coeff_modulus?
        if (ntt_tables.generate(coeff_count_power, coeff_mod))
        {
            qualifiers.enable_ntt = true;
        }

        // Can we use batching? (NTT with plain_modulus)
        if (ntt_tables.generate(coeff_count_power, plain_mod))
        {
            qualifiers.enable_batching = true;
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
            if (ntt_tables.generate(coeff_count_power, aux_mod))
            {
                qualifiers.enable_ntt_in_multiply = true;
            }
        }
#endif

        return qualifiers;
    }
}
