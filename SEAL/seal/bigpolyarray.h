#pragma once

#include <iostream>
#include "seal/bigpoly.h"
#include "seal/memorypoolhandle.h"
#include "seal/util/mempool.h"

namespace seal 
{
    /**
    Represents an array of BigPoly objects. The BigPolyArray class provides all of the functionality of 
    a BigPoly array. The size of the array (which can be read with size()) is set initially by the constructor 
    and can be resized either with the resize() function or with assignment (operator=()). Each polynomial in 
    the array must have the same coefficient count and coefficient bit count, which can be set in the constructor
    or with the resize() function, and read with coeff_count() and coeff_bit_count(). The array can be saved and 
    loaded from a stream with the save() and load() functions.

    All of the polynomials in the array are stored in one continuous block in memory.

    @par Thread Safety
    In general, reading from a BigPolyArray instance is thread-safe while mutating is not. Specifically, the array
    may be freed whenever a resize() occurs, the BigPolyArray instance is destroyed, or an assignment operation 
    occurs. A BigPolyArray allocates its backing array from the global (thread-safe) memory pool. Consequently, 
    creating or resizing a large number of BigPolyArrays can result in performance loss due to thread contention.
    */
    class BigPolyArray
    {
    public:
        /**
        Creates an empty BigPolyArray instance with a size of zero. No memory is allocated by this constructor.
        */
        BigPolyArray() = default;

        /**
        Creates a zero-initialized BigPolyArray instance with the specified size.

        @param[in] size The number of polynomials to allocate space for
        @param[in] coeff_count The number of coefficients allocated for each polynomial in the array
        @param[in] coeff_bit_count The bit count of each polynomial in the array
        @throws std::invalid_argument if size is negative
        @throws std::invalid_argument if coeff_count is negative
        @throws std::invalid_argument if coeff_bit_count is negative
        */
        BigPolyArray(int size, int coeff_count, int coeff_bit_count);

        /**
        Creates a deep copy of an BigPolyArray instance.

        @param[in] copy The BigPolyArray instance to copy from
        */
        BigPolyArray(const BigPolyArray &copy);

        /**
        Creates a new BigPolyArray by moving from another instance. The BigPolyArray source will be reset.

        @param[in] source The BigPolyArray instance to move from
        */
        BigPolyArray(BigPolyArray &&source) noexcept;

        /**
        Destroys the BigPolyArray instance and deallocates the contained array.
        */
        ~BigPolyArray();

        /**
        Returns the number of polynomials.
        */
        inline int size() const
        {
            return size_;
        }

        /**
        Returns the number of coefficients in the polynomials.
        */
        inline int coeff_count() const
        {
            return coeff_count_;
        }

        /**
        Returns the coefficient bit count of the polynomials.
        */
        inline int coeff_bit_count() const
        {
            return coeff_bit_count_;
        }

        /**
        Returns the number of std::uint64_t allocated for each coefficient of each polynomial.
        */
        inline int coeff_uint64_count() const
        {
            return coeff_uint64_count_;
        }

        /**
        Returns the number of std::uint64_t allocated for each polynomial.
        */
        inline int poly_uint64_count() const
        {
            return coeff_count_ * coeff_uint64_count_;
        }

        /**
        Returns the number of std::uint64_t allocated for the entire array of polynomials.
        */
        inline int uint64_count() const
        {
            return size_ * coeff_count_ * coeff_uint64_count_;
        }

        /**
        Returns whether or not the BigPolyArray has the exact same value as a specified BigPolyArray. Value 
        equality is determined both by the size parameters, and also by the exact values of the polynomials.

        @param[in] compare The BigPolyArray to compare against
        */
        inline bool operator ==(const BigPolyArray &compare) const
        {
            if (size_ != compare.size_
                || coeff_count_ != compare.coeff_count_
                || coeff_bit_count_ != compare.coeff_bit_count_
                || coeff_uint64_count_ != compare.coeff_uint64_count_)
            {
                return false;
            }
            return util::is_equal_poly_poly(value_.get(), compare.value_.get(), size_ * coeff_count_, coeff_uint64_count_);
        }

        /**
        Returns whether or not the BigPolyArray has a different value than a specified BigPolyArray. Value 
        equality is determined both by the size parameters, and also by the exact values of the polynomials.

        @param[in] compare The BigPolyArray to compare against
        */
        inline bool operator !=(const BigPolyArray &compare) const
        {
            return !(operator ==(compare));
        }

        /**
        Returns whether or not the BigPolyArray has all zero polynomials.
        */
        inline bool is_zero() const
        {
            if (size_ == 0 || coeff_count_ == 0 || coeff_bit_count_ == 0)
            {
                return true;
            }
            return util::is_zero_poly(value_.get(), size_ * coeff_count_, coeff_uint64_count_);
        }

        inline std::uint64_t *pointer()
        {
            return value_.get();
        }

        /**
        Returns a pointer to the backing array storing all of the polynomials. The pointer points to the 
        beginning of the backing array of the constant term of the polynomial at index given by parameter 
        poly_index. The return value will be nullptr if the coefficient count and/or bit count is zero.

        @param[in] poly_index The index of the polynomial
        @warning The pointer is valid only until the backing array is freed, which occurs when the 
        BigPolyArray is resized or destroyed.
        @throws std::out_of_range If poly_index is not within [0, size())
        */
        inline std::uint64_t *pointer(int poly_index)
        {
            if (size_ == 0 || coeff_count_ == 0 || coeff_bit_count_ == 0)
            {
                return nullptr;
            }
            if (poly_index < 0 || poly_index >= size_)
            {
                throw std::out_of_range("poly_index must be within [0, size)");
            }
            return value_.get() + poly_index * coeff_count_ * coeff_uint64_count_;
        }

        inline const std::uint64_t *pointer() const
        {
            return value_.get();
        }

        /**
        Returns a const pointer to the backing array storing all of the polynomials. The pointer points to 
        the beginning of the backing array of the constant term of the polynomial at index given by parameter 
        poly_index. The return value will be nullptr if the coefficient count and/or bit count is zero.

        @param[in] poly_index The index of the polynomial
        @warning The pointer is valid only until the backing array is freed, which occurs when the BigPolyArray 
        is resized or destroyed.
        @throws std::out_of_range If poly_index is not within [0, size())
        */
        inline const std::uint64_t *pointer(int poly_index) const
        {
            if (size_ == 0 || coeff_count_ == 0 || coeff_bit_count_ == 0)
            {
                return nullptr;
            }
            if (poly_index < 0 || poly_index >= size_)
            {
                throw std::out_of_range("poly_index must be within [0, size)");
            }
            return value_.get() + poly_index * coeff_count_ * coeff_uint64_count_;
        }

        /**
        Sets all polynomials to have a value of zero. This does not resize the BigPolyArray.
        */
        inline void set_zero()
        {
            if (size_ == 0 || coeff_count_ == 0 || coeff_bit_count_ == 0)
            {
                return;
            }
            util::set_zero_poly(size_ * coeff_count_, coeff_uint64_count_, value_.get());
        }

        /**
        Sets the polynomial with index poly_index to have value zero. This does not resize the BigPolyArray 
        or modify the other polynomials.

        @param[in] poly_index The index of the polynomial to set to zero
        @throws std::out_of_range If poly_index is not within [0, size())
        */
        inline void set_zero(int poly_index)
        {
            if (poly_index < 0 || poly_index >= size_)
            {
                throw std::out_of_range("poly_index must be within [0, size)");
            }
            util::set_zero_poly(coeff_count_, coeff_uint64_count_, pointer(poly_index));
        }

        /**
        Resizes the BigPolyArray internal array to store the specified number of polynomials of the specified 
        size, copying over the old polynomials as much as will fit.

        @param[in] size The number of polynomials to allocate space for
        @param[in] coeff_count The number of coefficients allocated for each polynomial in the array
        @param[in] coeff_bit_count The bit count of each polynomial in the array
        @throws std::invalid_argument if size is negative
        @throws std::invalid_argument if coeff_count is negative
        @throws std::invalid_argument if coeff_bit_count is negative
        */
        void resize(int size, int coeff_count, int coeff_bit_count);

        /**
        Resets the BigPolyArray instance to an empty, zero-sized instance. Any space allocated by the 
        BigPolyArray instance is deallocated.
        */
        inline void reset()
        {
            value_.release();
            size_ = 0;
            coeff_count_ = 0;
            coeff_bit_count_ = 0;
            coeff_uint64_count_ = 0;
        }

        /**
        Overwrites the BigPolyArray instance with a specified BigPolyArray instance. After assignment, the
        size of BigPolyArray matches the size of the assigned BigPolyArray instance.

        @param[in] assign The BigPolyArray instance to whose value should be assigned to the current 
        BigPolyArray instance
        */
        inline BigPolyArray &operator =(const BigPolyArray &assign)
        {
            // Do nothing if same thing.
            if (&assign == this)
            {
                return *this;
            }

            // Resize current array to same size as assign.
            resize(assign.size_, assign.coeff_count_, assign.coeff_bit_count_);

            // Copy value over.
            util::set_poly_poly(assign.value_.get(), size_ * coeff_count_, coeff_uint64_count_, value_.get());

            return *this;
        }

        /**
        Overwrites the BigPolyArray instance by moving the specified BigPolyArray instance. After assignment, 
        the size of BigPolyArray matches the size of the original assigned BigPolyArray instance, and the memory 
        location of the backing array is the same. The assigned instance is reset.

        @param[in] assign The BigPolyArray instance to whose value should be assigned to the current BigPolyArray 
        instance
        */
        inline BigPolyArray &operator =(BigPolyArray &&assign)
        {
            // Deallocate currently allocated pointers
            reset();

            // Move assign to current
            // This will set assign.value_ to released state
            value_.swap_with(assign.value_);
            size_ = assign.size_;
            coeff_count_ = assign.coeff_count_;
            coeff_bit_count_ = assign.coeff_bit_count_;
            coeff_uint64_count_ = assign.coeff_uint64_count_;

            // Manually reset assign without deallocating
            assign.size_ = 0;
            assign.coeff_count_ = 0;
            assign.coeff_bit_count_ = 0;
            assign.coeff_uint64_count_ = 0;

            return *this;
        }

        /**
        Saves the BigPolyArray instance to an output stream. The output is in binary format and not human-readable. 
        The output stream must have the "binary" flag set.

        @param[in] stream The stream to save the BigPolyArray to
        @see load() to load a saved BigPolyArray instance.
        */
        void save(std::ostream &stream) const;

        /**
        Loads an BigPolyArray instance from an input stream overwriting the current BigPolyArray instance.

        @param[in] stream The stream to load the BigPolyArray instance from
        @see save() to save an BigPolyArray instance.
        */
        void load(std::istream &stream);

    private:
        MemoryPoolHandle pool_;

        /**
        The backing array for all of the polynomials in the BigPolyArray.
        */
        util::Pointer value_;

        /**
        The number of polynomials stored in the BigPolyArray.
        */
        int size_ = 0;

        /**
        The coefficient count of each polynomial in the BigPolyArray.
        */
        int coeff_count_ = 0;

        /**
        The coefficient bit count of each polynomial in the BigPolyArray.
        */
        int coeff_bit_count_ = 0;

        /**
        The uint64 count of each polynomial in the BigPolyArray.
        */
        int coeff_uint64_count_ = 0;
    };
}