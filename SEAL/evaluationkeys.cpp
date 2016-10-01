#include "evaluationkeys.h"
#include <stdexcept>

using namespace std;

namespace seal
{
    const std::pair<BigPolyArray, BigPolyArray> &EvaluationKeys::operator[](int key_index) const
    {
        if (key_index >= keys_.size() || key_index < 0)
        {
            throw out_of_range("key_index must be nonnegative and less than number of keys");
        }
        return keys_[key_index];
    }

    std::pair<BigPolyArray, BigPolyArray> &EvaluationKeys::operator[](int key_index)
    {
        if (key_index >= keys_.size() || key_index < 0)
        {
            throw out_of_range("key_index must be nonnegative and less than number of keys");
        }
        return keys_[key_index];
    }

    void EvaluationKeys::save(std::ostream &stream) const
    {
        // save the size of keys_
        int32_t keys_size_32 = static_cast<int32_t>(keys_.size());
        stream.write(reinterpret_cast<const char*>(&keys_size_32), sizeof(int32_t));

        // loop over entries in the std::vector
        for (int i = 0; i < keys_.size(); ++i) 
        {
            // loop over BigPolyArrays in the std::pair
            keys_[i].first.save(stream);
            keys_[i].second.save(stream);
        }
    }

    void EvaluationKeys::load(std::istream &stream)
    {
        // make sure keys_ is empty before reading
        clear();

        // read in the eventual size of keys_
        int32_t read_keys_size = 0;
        stream.read(reinterpret_cast<char*>(&read_keys_size), sizeof(int32_t));
     
        BigPolyArray first;
        BigPolyArray second;

        // loop over entries in the std::vector
        for (int i = 0; i < read_keys_size; ++i)
        {
            first.load(stream);
            second.load(stream);
            keys_.emplace_back(first, second);
        }
    }

    void EvaluationKeys::clear()
    {
        keys_.clear();
    }
}