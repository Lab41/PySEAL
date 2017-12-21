#include "encryptionparams.h"
#include "chooser.h"
#include "util/polycore.h"
#include "util/uintarith.h"
#include "util/modulus.h"
#include "util/polymodulus.h"
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace seal::util;

namespace seal
{
    EncryptionParameters::EncryptionParameters() :
        decomposition_bit_count_(0), 
        noise_standard_deviation_(ChooserEvaluator::default_noise_standard_deviation()), 
        noise_max_deviation_(ChooserEvaluator::default_noise_max_deviation()),
        random_generator_(nullptr),
        hash_block_{ 0 }
    {
        compute_hash();
    }

    void EncryptionParameters::set_poly_modulus(const BigPoly &poly_modulus)
    {
        // Set the poly_modulus to be as small as possible
        poly_modulus_.resize(1, 1);

        // Now operator =(...) automatically resizes to (significant_coeff_count, significant_coeff_bit_count) size
        poly_modulus_ = poly_modulus;

        // Re-compute the hash
        compute_hash();
    }

    void EncryptionParameters::set_coeff_modulus(const BigUInt &coeff_modulus)
    {
        // Set the coeff_modulus to be as small as possible
        coeff_modulus_.resize(1);

        // Now operator =(...) automatically resizes to significant_bit_count size
        coeff_modulus_ = coeff_modulus;

        // Re-compute the hash
        compute_hash();
    }

    void EncryptionParameters::set_plain_modulus(const SmallModulus &plain_modulus)
    {
        plain_modulus_ = plain_modulus;

        // Re-compute the hash
        compute_hash();
    }

    void EncryptionParameters::set_decomposition_bit_count(int decomposition_bit_count)
    {
        decomposition_bit_count_ = decomposition_bit_count;

        // Re-compute the hash
        compute_hash();
    }

    void EncryptionParameters::set_noise_standard_deviation(double noise_standard_deviation)
    {
        noise_standard_deviation_ = noise_standard_deviation;

        // Re-compute the hash
        compute_hash();
    }

    void EncryptionParameters::set_noise_max_deviation(double noise_max_deviation)
    {
        noise_max_deviation_ = noise_max_deviation;

        // Re-compute the hash
        compute_hash();
    }

    void EncryptionParameters::set_random_generator(UniformRandomGeneratorFactory *random_generator)
    {
        random_generator_ = random_generator;
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

        // Re-compute the hash
        compute_hash();
    }

    void EncryptionParameters::compute_hash()
    {
        uint64_t noise_standard_deviation64 = *reinterpret_cast<uint64_t*>(&noise_standard_deviation_);
        uint64_t noise_max_deviation64 = *reinterpret_cast<uint64_t*>(&noise_max_deviation_);
        uint64_t decomposition_bit_count64 = static_cast<uint64_t>(decomposition_bit_count_);

        int total_uint64_count = poly_modulus_.coeff_uint64_count() * poly_modulus_.coeff_count()
            + coeff_modulus_.uint64_count()
            + plain_modulus_.uint64_count()
            + 1 // noise_standard_deviation
            + 1 // noise_max_deviation
            + 1; // decomposition_bit_count

        Pointer param_data(allocate_uint(total_uint64_count, MemoryPoolHandle::Global()));
        uint64_t *param_data_ptr = param_data.get();

        set_poly_poly(poly_modulus_.pointer(), poly_modulus_.coeff_count(), poly_modulus_.coeff_uint64_count(), param_data_ptr);
        param_data_ptr += poly_modulus_.coeff_uint64_count() * poly_modulus_.coeff_count();

        set_uint_uint(coeff_modulus_.pointer(), coeff_modulus_.uint64_count(), param_data_ptr);
        param_data_ptr += coeff_modulus_.uint64_count();
        set_uint_uint(plain_modulus_.pointer(), plain_modulus_.uint64_count(), param_data_ptr);
        param_data_ptr += plain_modulus_.uint64_count();

        *param_data_ptr++ = noise_standard_deviation64;
        *param_data_ptr++ = noise_max_deviation64;
        *param_data_ptr = decomposition_bit_count64;

        HashFunction::sha3_hash(param_data.get(), total_uint64_count, hash_block_);
    }
}
