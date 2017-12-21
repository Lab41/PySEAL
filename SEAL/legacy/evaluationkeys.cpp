#include "evaluationkeys.h"
#include <stdexcept>

using namespace std;

namespace seal
{
    void EvaluationKeys::save(std::ostream &stream) const
    {
        // Save the hash block
        stream.write(reinterpret_cast<const char*>(&hash_block_), sizeof(EncryptionParameters::hash_block_type));

        // Save the size of keys_
        int32_t keys_size_32 = static_cast<int32_t>(keys_.size());
        stream.write(reinterpret_cast<const char*>(&keys_size_32), sizeof(int32_t));

        // Loop over entries in the std::vector
        for (int i = 0; i < keys_.size(); i++) 
        {
            // Loop over BigPolyArrays in the std::pair
            keys_[i].first.save(stream);
            keys_[i].second.save(stream);
        }
    }

    void EvaluationKeys::load(std::istream &stream)
    {
        // Clear current keys
        keys_.clear();

        // Read the hash block
        stream.read(reinterpret_cast<char*>(&hash_block_), sizeof(EncryptionParameters::hash_block_type));

        // Read in the eventual size of keys_
        int32_t read_keys_size = 0;
        stream.read(reinterpret_cast<char*>(&read_keys_size), sizeof(int32_t));
     
        BigPolyArray first;
        BigPolyArray second;

        // Loop over entries in the std::vector
        for (int i = 0; i < read_keys_size; i++)
        {
            first.load(stream);
            second.load(stream);
            keys_.emplace_back(first, second);
        }
    }
}