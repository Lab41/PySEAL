#include "encryptionparams.h"

using namespace std;

namespace seal
{
    EncryptionParameters::EncryptionParameters() :
        noise_standard_deviation_(0), noise_max_deviation_(0), decomposition_bit_count_(1), mode_(NORMAL_MODE), random_generator_(nullptr)
    {
    }

    void EncryptionParameters::save(ostream &stream) const
    {
        poly_modulus_.save(stream);
        coeff_modulus_.save(stream);
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
        plain_modulus_.load(stream);
        stream.read(reinterpret_cast<char*>(&noise_standard_deviation_), sizeof(double));
        stream.read(reinterpret_cast<char*>(&noise_max_deviation_), sizeof(double));
        int32_t decomp_bit_count32 = 0;
        stream.read(reinterpret_cast<char*>(&decomp_bit_count32), sizeof(int32_t));
        decomposition_bit_count_ = static_cast<int>(decomp_bit_count32);
    }
}
