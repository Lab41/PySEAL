#include <utility>
#include "ciphertext.h"

using namespace std;

namespace seal
{
    Ciphertext &Ciphertext::operator =(const BigPolyArray &poly_array)
    {
        ciphertext_array_ = poly_array;
        return *this;
    }
    Ciphertext &Ciphertext::operator =(BigPolyArray &&poly_array)
    {
        ciphertext_array_ = move(poly_array);
        return *this;
    }

    void Ciphertext::save(ostream &stream) const
    {
        ciphertext_array_.save(stream);
    }

    void Ciphertext::load(istream &stream)
    {
        ciphertext_array_.load(stream);
    }
}