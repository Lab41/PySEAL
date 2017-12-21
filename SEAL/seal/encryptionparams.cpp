#include "seal/encryptionparams.h"
#include "seal/chooser.h"
#include "seal/util/polycore.h"
#include "seal/util/uintarith.h"
#include "seal/util/modulus.h"
#include "seal/util/polymodulus.h"
#include "seal/defaultparams.h"
#include <stdexcept>
#include <math.h>

using namespace std;
using namespace seal::util;

namespace seal
{
    EncryptionParameters::EncryptionParameters() : poly_modulus_(1, 1)
    {
        // It is important to ensure that poly_modulus always has at least one coefficient
        // and at least one uint64 per coefficient
        compute_hash();
    }

    void EncryptionParameters::save(ostream &stream) const
    {
        poly_modulus_.save(stream);
        int32_t coeff_mod_count32 = coeff_modulus_.size();
        stream.write(reinterpret_cast<const char*>(&coeff_mod_count32), sizeof(int32_t));
        for (int i = 0; i < coeff_mod_count32; i++)
        {
            coeff_modulus_[i].save(stream);
        }
        plain_modulus_.save(stream);
        stream.write(reinterpret_cast<const char*>(&noise_standard_deviation_), sizeof(double));
        stream.write(reinterpret_cast<const char*>(&noise_max_deviation_), sizeof(double));
    }

    void EncryptionParameters::load(istream &stream)
    {
        poly_modulus_.load(stream);
        int32_t coeff_mod_count32 = 0;
        stream.read(reinterpret_cast<char*>(&coeff_mod_count32), sizeof(int32_t));
        coeff_modulus_.resize(coeff_mod_count32);
        for (int i = 0; i < coeff_mod_count32; i++)
        {
            coeff_modulus_[i].load(stream);
        }
        plain_modulus_.load(stream);
        stream.read(reinterpret_cast<char*>(&noise_standard_deviation_), sizeof(double));
        stream.read(reinterpret_cast<char*>(&noise_max_deviation_), sizeof(double));

        // Re-compute the hash
        compute_hash();
    }

    void EncryptionParameters::compute_hash()
    {
        uint64_t noise_standard_deviation64 = *reinterpret_cast<uint64_t*>(&noise_standard_deviation_);
        uint64_t noise_max_deviation64 = *reinterpret_cast<uint64_t*>(&noise_max_deviation_);

        int coeff_mod_count = coeff_modulus_.size();

        int total_uint64_count = poly_modulus_.coeff_uint64_count() * poly_modulus_.coeff_count()
            + coeff_mod_count
            + plain_modulus_.uint64_count()
            + 1 // noise_standard_deviation
            + 1; // noise_max_deviation

        Pointer param_data(allocate_uint(total_uint64_count, MemoryPoolHandle::Global()));
        uint64_t *param_data_ptr = param_data.get();

        set_poly_poly(poly_modulus_.pointer(), poly_modulus_.coeff_count(), poly_modulus_.coeff_uint64_count(), param_data_ptr);
        param_data_ptr += poly_modulus_.coeff_uint64_count() * poly_modulus_.coeff_count();

        for (int i = 0; i < coeff_mod_count; i++)
        {
            *param_data_ptr = coeff_modulus_[i].value();
            param_data_ptr++;
        }

        set_uint_uint(plain_modulus_.pointer(), plain_modulus_.uint64_count(), param_data_ptr);
        param_data_ptr += plain_modulus_.uint64_count();

        *param_data_ptr++ = noise_standard_deviation64;
        *param_data_ptr = noise_max_deviation64;

        HashFunction::sha3_hash(param_data.get(), total_uint64_count, hash_block_);
    }
}
