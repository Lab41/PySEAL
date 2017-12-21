#include "seal/galoiskeys.h"
#include "seal/util/common.h"
#include <stdexcept>

using namespace std;
using namespace seal::util;

namespace seal
{
    void GaloisKeys::save(std::ostream &stream) const
    {
        // Save the hash block
        stream.write(reinterpret_cast<const char*>(&hash_block_), sizeof(EncryptionParameters::hash_block_type));

        // Save the decomposition bit count
        int32_t decomposition_bit_count32 = static_cast<int32_t>(decomposition_bit_count_);
        stream.write(reinterpret_cast<const char*>(&decomposition_bit_count32), sizeof(int32_t));

        // Save the size of keys_
        int32_t keys_dim1 = static_cast<int32_t>(keys_.size());
        stream.write(reinterpret_cast<const char*>(&keys_dim1), sizeof(int32_t));

        // Now loop again over keys_dim1
        for (int32_t index = 0; index < keys_dim1; index++)
        {
            // Save second dimension of keys_
            int32_t keys_dim2 = keys_[index].size();
            stream.write(reinterpret_cast<const char*>(&keys_dim2), sizeof(int32_t));

            // Loop over keys_dim2 and save all (or none)
            for (int32_t j = 0; j < keys_dim2; j++)
            {
                // Save the key
                keys_[index][j].save(stream);
            }
        }
    }

    void GaloisKeys::load(std::istream &stream)
    {
        // Clear current keys
        keys_.clear();

        // Read the hash block
        stream.read(reinterpret_cast<char*>(&hash_block_), sizeof(EncryptionParameters::hash_block_type));

        // Read the decomposition_bit_count
        int32_t decomposition_bit_count32 = 0;
        stream.read(reinterpret_cast<char*>(&decomposition_bit_count32), sizeof(int32_t));
        decomposition_bit_count_ = decomposition_bit_count32;

        // Read in the size of keys_
        int32_t keys_dim1 = 0;
        stream.read(reinterpret_cast<char*>(&keys_dim1), sizeof(int32_t));

        // Resize first dimension of keys_
        keys_.resize(keys_dim1);

        // Loop over the first dimension of keys_
        for (int32_t index = 0; index < keys_dim1; index++)
        {
            // Read the size of the second dimension
            int32_t keys_dim2 = 0;
            stream.read(reinterpret_cast<char*>(&keys_dim2), sizeof(int32_t));

            // Resize
            keys_[index].resize(keys_dim2);
            for (int32_t j = 0; j < keys_dim2; j++)
            {
                keys_[index][j].load(stream);
            }
        }
    }
}
