#include <algorithm>
#include <stdexcept>
#include "seal/bigpolyarray.h"
#include "seal/util/uintcore.h"
#include "seal/util/polycore.h"

using namespace std;
using namespace seal::util;

namespace seal
{
    BigPolyArray::BigPolyArray(int size, int poly_coeff_count, int poly_coeff_bit_count)
    {
        resize(size, poly_coeff_count, poly_coeff_bit_count);
    }

    BigPolyArray::BigPolyArray(const BigPolyArray &copy)
    {
        operator =(copy);
    }

    BigPolyArray::BigPolyArray(BigPolyArray &&source) noexcept : 
        pool_(move(source.pool_)),
        value_(move(source.value_)), 
        size_(source.size_),
        coeff_count_(source.coeff_count_), 
        coeff_bit_count_(source.coeff_bit_count_),
        coeff_uint64_count_(source.coeff_uint64_count_)
    {
        // Manually reset source without deallocating
        source.size_ = 0;
        source.coeff_count_ = 0;
        source.coeff_bit_count_ = 0;
        source.coeff_uint64_count_ = 0;
    }

    BigPolyArray::~BigPolyArray()
    {
        reset();
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

        // Lazy initialization of MemoryPoolHandle
        if (!pool_)
        {
            pool_ = MemoryPoolHandle::Global();
        }

        // Size is already right?
        if (size == size_ && coeff_count == coeff_count_ && coeff_bit_count == coeff_bit_count_)
        {
            return;
        }

        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        if (size == size_ && coeff_count == coeff_count_ && coeff_uint64_count == coeff_uint64_count_)
        {
            // No need to reallocate. Simply filter high-bits for each coeff and return.
            uint64_t *coeff_ptr = value_.get();
            for (int coeff_index = 0; coeff_index < size_ * coeff_count_; coeff_index++)
            {
                filter_highbits_uint(coeff_ptr, coeff_uint64_count_, coeff_bit_count);
                coeff_ptr += coeff_uint64_count_;
            }
            coeff_bit_count_ = coeff_bit_count;
            coeff_uint64_count_ = coeff_uint64_count;

            return;
        }

        // Allocate new space.
        int uint64_count = size * coeff_count * coeff_uint64_count;
        Pointer new_value;
        if (uint64_count > 0)
        {
            new_value.swap_with(allocate_uint(uint64_count, pool_));
        }
        
        uint64_t *value_ptr = value_.get();
        uint64_t *new_value_ptr = new_value.get();
        for (int poly_index = 0; poly_index < size; poly_index++)
        {
            if (poly_index < size_)
            {
                set_poly_poly(value_ptr, coeff_count_, coeff_uint64_count_, coeff_count, coeff_uint64_count, new_value_ptr);
                
                // Filter high-bits.
                uint64_t *coeff_ptr = new_value_ptr;
                for (int coeff_index = 0; coeff_index < coeff_count; coeff_index++)
                {
                    filter_highbits_uint(coeff_ptr, coeff_uint64_count, coeff_bit_count);
                    coeff_ptr += coeff_uint64_count;
                }
                
                value_ptr += coeff_count_ * coeff_uint64_count_;
                new_value_ptr += coeff_count * coeff_uint64_count;
            }
            else
            {
                set_zero_poly(coeff_count, coeff_uint64_count, new_value_ptr);
                new_value_ptr += coeff_count * coeff_uint64_count;
            }
        }

        // Deallocate old space.
        reset();

        // Update class.
        value_.swap_with(new_value);
        size_ = size;
        coeff_count_ = coeff_count;
        coeff_bit_count_ = coeff_bit_count;
        coeff_uint64_count_ = coeff_uint64_count;
    }

    void BigPolyArray::save(ostream &stream) const
    {
        int32_t count32 = static_cast<int32_t>(size_);
        int32_t coeff_count32 = static_cast<int32_t>(coeff_count_);
        int32_t coeff_bit_count32 = static_cast<int32_t>(coeff_bit_count_);

        stream.write(reinterpret_cast<const char*>(&count32), sizeof(int32_t));
        stream.write(reinterpret_cast<const char*>(&coeff_count32), sizeof(int32_t));
        stream.write(reinterpret_cast<const char*>(&coeff_bit_count32), sizeof(int32_t));
        stream.write(reinterpret_cast<const char*>(value_.get()), size_ * coeff_count_ * coeff_uint64_count_ * bytes_per_uint64);
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

        stream.read(reinterpret_cast<char*>(value_.get()), size_ * coeff_count_ * coeff_uint64_count_ * bytes_per_uint64);
    }
}