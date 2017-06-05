#include <algorithm>
#include <stdexcept>
#include "bigpolyarray.h"
#include "util/uintcore.h"
#include "util/polycore.h"

using namespace std;
using namespace seal::util;

namespace seal
{
    BigPolyArray::BigPolyArray() :
        value_(nullptr), polys_(nullptr), size_(0), coeff_count_(0), coeff_bit_count_(0)
    {
    }

    BigPolyArray::BigPolyArray(int size, int poly_coeff_count, int poly_coeff_bit_count) :
        value_(nullptr), polys_(nullptr), size_(0), coeff_count_(0), coeff_bit_count_(0)
    {
        resize(size, poly_coeff_count, poly_coeff_bit_count);
    }

    BigPolyArray::BigPolyArray(const BigPolyArray &copy) : 
        value_(nullptr), polys_(nullptr), size_(0), coeff_count_(0), coeff_bit_count_(0)
    {
        operator =(copy);
    }

    BigPolyArray::BigPolyArray(BigPolyArray &&source) noexcept : 
        value_(source.value_), polys_(source.polys_), size_(source.size_), coeff_count_(source.coeff_count_), coeff_bit_count_(source.coeff_bit_count_)
    {
        // Manually reset source without deallocating
        source.value_ = nullptr;
        source.polys_ = nullptr;
        source.size_ = 0;
        source.coeff_count_ = 0;
        source.coeff_bit_count_ = 0;
    }

    BigPolyArray::~BigPolyArray()
    {
        reset();
    }

    int BigPolyArray::coeff_uint64_count() const
    {
        return divide_round_up(coeff_bit_count_, bits_per_uint64);
    }

    int BigPolyArray::poly_uint64_count() const
    {
        return coeff_count_ * coeff_uint64_count();
    }

    int BigPolyArray::uint64_count() const
    {
        return size_ * coeff_count_ * coeff_uint64_count();
    }

    const uint64_t *BigPolyArray::pointer(int poly_index) const
    {
        if (size_ == 0 || coeff_count_ == 0 || coeff_bit_count_ == 0)
        {
            return nullptr;
        }
        if (poly_index < 0 || poly_index >= size_)
        {
            throw out_of_range("poly_index must be within [0, size)");
        }
        return value_ + poly_index * coeff_count_ * coeff_uint64_count();
    }

    uint64_t *BigPolyArray::pointer(int poly_index)
    {
        if (size_ == 0 || coeff_count_ == 0 || coeff_bit_count_ == 0)
        {
            return nullptr;
        }
        if (poly_index < 0 || poly_index >= size_)
        {
            throw out_of_range("poly_index must be within [0, size)");
        }
        return value_ + poly_index * coeff_count_ * coeff_uint64_count();
    }

    BigPoly &BigPolyArray::operator[](int poly_index)
    {
        if (poly_index < 0 || poly_index >= size_)
        {
            throw out_of_range("poly_index must be within [0, size)");
        }
        return polys_[poly_index];
    }

    const BigPoly &BigPolyArray::operator[](int poly_index) const
    {
        if (poly_index < 0 || poly_index >= size_)
        {
            throw out_of_range("poly_index must be within [0, size)");
        }
        return polys_[poly_index];
    }

    void BigPolyArray::set_zero()
    {
        if (size_ == 0 || coeff_count_ == 0 || coeff_bit_count_ == 0)
        {
            return;
        }
        set_zero_poly(size_ * coeff_count_, coeff_uint64_count(), value_);
    }

    void BigPolyArray::set_zero(int poly_index)
    {
        if (poly_index < 0 || poly_index >= size_)
        {
            throw out_of_range("poly_index must be within [0, size)");
        }
        polys_[poly_index].set_zero();
    }

    void BigPolyArray::resize(int size, int coeff_count, int coeff_bit_count)
    {
        if (size < 0)
        {
            throw invalid_argument("size must be non-negative");
        }
        if (coeff_count < 0)
        {
            throw invalid_argument("coeff_count must be non-negative");
        }
        if (coeff_bit_count < 0)
        {
            throw invalid_argument("coeff_bit_count must be non-negative");
        }

        // Size is already right?
        if (size == size_ && coeff_count == coeff_count_ && coeff_bit_count == coeff_bit_count_)
        {
            return;
        }

        int new_coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        int old_coeff_uint64_count = coeff_uint64_count();

        if (size == size_ && coeff_count == coeff_count_ && new_coeff_uint64_count == old_coeff_uint64_count)
        {
            // No need to reallocate. Simply filter high-bits for each coeff and return.
            uint64_t *coeff_ptr = value_;
            for (int coeff_index = 0; coeff_index < size_ * coeff_count_; ++coeff_index)
            {
                filter_highbits_uint(coeff_ptr, old_coeff_uint64_count, coeff_bit_count);
                coeff_ptr += old_coeff_uint64_count;
            }
            for (int poly_index = 0; poly_index < size_; ++poly_index)
            {
                polys_[poly_index].alias(coeff_count_, coeff_bit_count, pointer(poly_index));
            }
            coeff_bit_count_ = coeff_bit_count;

            return;
        }

        // Allocate new space.
        uint64_t *new_value = (size > 0 && coeff_count > 0 && coeff_bit_count > 0) ? 
            new uint64_t[size * coeff_count * new_coeff_uint64_count] : nullptr;
        
        uint64_t *value_ptr = value_;
        uint64_t *new_value_ptr = new_value;
        for (int poly_index = 0; poly_index < size; ++poly_index)
        {
            if (poly_index < size_)
            {
                set_poly_poly(value_ptr, coeff_count_, old_coeff_uint64_count, coeff_count, new_coeff_uint64_count, new_value_ptr);
                
                // Filter high-bits.
                uint64_t *coeff_ptr = new_value_ptr;
                for (int coeff_index = 0; coeff_index < coeff_count; ++coeff_index)
                {
                    filter_highbits_uint(coeff_ptr, new_coeff_uint64_count, coeff_bit_count);
                    coeff_ptr += new_coeff_uint64_count;
                }
                
                value_ptr += coeff_count_ * old_coeff_uint64_count;
                new_value_ptr += coeff_count * new_coeff_uint64_count;
            }
            else
            {
                set_zero_poly(coeff_count, new_coeff_uint64_count, new_value_ptr);
                new_value_ptr += coeff_count * new_coeff_uint64_count;
            }
        }

        // Deallocate old space.
        reset();

        // Create polys.
        if (size > 0)
        {
            polys_ = new BigPoly[size];
            uint64_t *poly_ptr = new_value;
            for (int i = 0; i < size; ++i)
            {
                polys_[i].alias(coeff_count, coeff_bit_count, poly_ptr);
                poly_ptr += coeff_count * new_coeff_uint64_count;
            }
        }

        // Update class.
        value_ = new_value;
        size_ = size;
        coeff_count_ = coeff_count;
        coeff_bit_count_ = coeff_bit_count;
    }

    BigPolyArray& BigPolyArray::operator =(const BigPolyArray &assign)
    {
        // Do nothing if same thing.
        if (&assign == this)
        {
            return *this;
        }

        // Resize current array to same size as assign.
        int count = assign.size_;
        int coeff_count = assign.coeff_count_;
        int coeff_bit_count = assign.coeff_bit_count_;
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        resize(count, coeff_count, coeff_bit_count);
        
        // Copy value over.
        set_poly_poly(assign.value_, count * coeff_count, coeff_uint64_count, value_);

        return *this;
    }

    BigPolyArray &BigPolyArray::operator =(BigPolyArray &&assign)
    {
        // Deallocate currently allocated pointers
        reset();

        // Move assign to current
        value_ = assign.value_;
        polys_ = assign.polys_;
        size_ = assign.size_;
        coeff_count_ = assign.coeff_count_;
        coeff_bit_count_ = assign.coeff_bit_count_;

        // Manually reset assign without deallocating
        assign.value_ = nullptr;
        assign.polys_ = nullptr;
        assign.size_ = 0;
        assign.coeff_count_ = 0;
        assign.coeff_bit_count_ = 0;

        return *this;
    }

    void BigPolyArray::save(ostream &stream) const
    {
        int32_t count32 = static_cast<int32_t>(size_);
        int32_t coeff_count32 = static_cast<int32_t>(coeff_count_);
        int32_t coeff_bit_count32 = static_cast<int32_t>(coeff_bit_count_);

        stream.write(reinterpret_cast<const char*>(&count32), sizeof(int32_t));
        stream.write(reinterpret_cast<const char*>(&coeff_count32), sizeof(int32_t));
        stream.write(reinterpret_cast<const char*>(&coeff_bit_count32), sizeof(int32_t));
        stream.write(reinterpret_cast<const char*>(value_), size_ * coeff_count_ * coeff_uint64_count() * bytes_per_uint64);
    }

    void BigPolyArray::load(istream &stream)
    {
        int32_t read_count = 0;
        int32_t read_coeff_count = 0;
        int32_t read_coeff_bit_count = 0;

        stream.read(reinterpret_cast<char*>(&read_count), sizeof(int32_t));
        stream.read(reinterpret_cast<char*>(&read_coeff_count), sizeof(int32_t));
        stream.read(reinterpret_cast<char*>(&read_coeff_bit_count), sizeof(int32_t));

        resize(read_count, read_coeff_count, read_coeff_bit_count);

        stream.read(reinterpret_cast<char*>(value_), size_ * coeff_count_ * coeff_uint64_count() * bytes_per_uint64);
    }
 
    void BigPolyArray::reset()
    {
        if (polys_ != nullptr)
        {
            delete[] polys_;
        }
        if (value_ != nullptr)
        {
            delete[] value_;
        }

        polys_ = nullptr;
        value_ = nullptr;
        size_ = 0;
        coeff_count_ = 0;
        coeff_bit_count_ = 0;
    }

}