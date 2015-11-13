#include <algorithm>
#include <stdexcept>
#include "evaluationkeys.h"

using namespace std;

namespace seal
{
    EvaluationKeys::EvaluationKeys() : polys_(nullptr), count_(0)
    {
    }

    EvaluationKeys::EvaluationKeys(int count) : polys_(nullptr), count_(0)
    {
        resize(count);
    }

    EvaluationKeys::EvaluationKeys(const EvaluationKeys &copy) : polys_(nullptr), count_(0)
    {
        operator =(copy);
    }

    EvaluationKeys::~EvaluationKeys()
    {
        reset();
    }

    const BigPoly& EvaluationKeys::operator[](int poly_index) const
    {
        if (poly_index < 0 || poly_index >= count_)
        {
            throw out_of_range("poly_index must be within [0, count)");
        }
        return polys_[poly_index];
    }

    BigPoly& EvaluationKeys::operator[](int poly_index)
    {
        if (poly_index < 0 || poly_index >= count_)
        {
            throw out_of_range("poly_index must be within [0, count)");
        }
        return polys_[poly_index];
    }

    void EvaluationKeys::resize(int count)
    {
        if (count < 0)
        {
            throw invalid_argument("count must be non-negative");
        }
        if (count == count_)
        {
            return;
        }

        // Allocate new space.
        BigPoly *new_polys = count > 0 ? new BigPoly[count] : nullptr;

        // Copy over old values.
        int copy_count = min(count_, count);
        for (int i = 0; i < copy_count; ++i)
        {
            BigPoly &to_poly = new_polys[i];
            const BigPoly &from_poly = polys_[i];
            to_poly.resize(from_poly.coeff_count(), from_poly.coeff_bit_count());
            to_poly = from_poly;
        }

        // Deallocate old space.
        reset();

        // Update class.
        polys_ = new_polys;
        count_ = count;
    }

    void EvaluationKeys::clear()
    {
        reset();
    }

    EvaluationKeys& EvaluationKeys::operator=(const EvaluationKeys& assign)
    {
        // Do nothing if same thing.
        if (&assign == this)
        {
            return *this;
        }

        // Resize array if needed.
        int count = assign.count();
        if (count != count_)
        {
            reset();
            resize(count);
        }

        // Copy polys over.
        for (int i = 0; i < count; ++i)
        {
            BigPoly &to_poly = polys_[i];
            const BigPoly &from_poly = assign[i];
            to_poly.resize(from_poly.coeff_count(), from_poly.coeff_bit_count());
            to_poly = from_poly;
        }

        return *this;
    }

    void EvaluationKeys::save(ostream &stream) const
    {
        int32_t count32 = static_cast<int32_t>(count_);
        stream.write(reinterpret_cast<const char*>(&count32), sizeof(int32_t));
        for (int i = 0; i < count_; ++i)
        {
            polys_[i].save(stream);
        }
    }

    void EvaluationKeys::load(istream &stream)
    {
        int32_t count32 = 0;
        stream.read(reinterpret_cast<char*>(&count32), sizeof(int32_t));
        if (count_ != count32)
        {
            resize(static_cast<int>(count32));
        }
        for (int i = 0; i < count_; ++i)
        {
            polys_[i].load(stream);
        }
    }

    void EvaluationKeys::reset()
    {
        if (polys_ != nullptr)
        {
            delete[] polys_;
        }
        polys_ = nullptr;
        count_ = 0;
    }

}