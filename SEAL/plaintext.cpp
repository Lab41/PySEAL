#include <utility>
#include "plaintext.h"

using namespace std;

namespace seal
{
    Plaintext::Plaintext() : plaintext_poly_(1, 1)
    {
        plaintext_poly_.set_zero();
    }

    Plaintext &Plaintext::operator =(const Plaintext &assign)
    {
        plaintext_poly_.duplicate_from(assign.plaintext_poly_);
        return *this;
    }

    Plaintext &Plaintext::operator =(const BigPoly &poly)
    {
        plaintext_poly_.duplicate_from(poly);
        return *this;
    }
 
    Plaintext &Plaintext::operator =(BigPoly &&poly)
    {
        plaintext_poly_ = move(poly);
        return *this;
    }

    Plaintext &Plaintext::operator =(const string &hex_poly)
    {
        plaintext_poly_ = hex_poly;
        return *this;
    }

    void Plaintext::save(ostream &stream) const
    {
        plaintext_poly_.save(stream);
    }

    void Plaintext::load(istream &stream)
    {
        plaintext_poly_.load(stream);
    }
}