#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include "seal/memorypoolhandle.h"
#include "seal/bigpoly.h"
#include "seal/encryptionparams.h"
#include "seal/util/uintcore.h"
#include "seal/util/polycore.h"

namespace seal
{
    /**
    Class to store a plaintext element. The data for the plaintext is a polynomial with coefficients
    modulo the plaintext modulus. The degree of the plaintext polynomial must be one less than the
    degree of the polynomial modulus. The backing array always allocates one 64-bit word per each
    coefficient of the polynoimal.

    @par Memory Management
    SEAL allocates the memory for a plaintext by default from the global memory pool, but a user can
    alternatively specify a different memory pool to be used through the MemoryPoolHandle class. This
    can be important, as constructing or resizing several plaintexts at once allocated in the global
    memory pool can quickly lead to contention and poor performance in multi-threaded applications.
    In addition to its coefficient count, a plaintext also has a capacity which denotes the coefficient
    count that fits in the current allocation. Since each coefficient is a 64-bit word, this is exactly
    the number of 64-bit words that are allocated. In high-performance applications unnecessary
    re-allocations should also be avoided by reserving enough memory for the plaintext to begin with
    either by providing the capacity to the constructor as an extra argument, or by calling the reserve
    function at any time.

    @par Aliased Plaintexts
    By default a plaintext manages its own memory. However, in some cases the user might want to e.g.
    pre-allocate a large continuous block of memory to be used by several plaintexts. In this case
    it is possible to create aliased plaintextds by simply giving the constructor a pointer to the
    memory location, and the relevant size parameters, including the capacity. The allocation size of
    an aliased plaintext cannot be changed with the reserve function, unless it is first reallocated
    in a memory pool using the unalias function.

    @par Thread Safety
    In general, reading from plaintext is thread-safe as long as no other thread is concurrently
    mutating it. This is due to the underlying data structure storing the plaintext not being
    thread-safe.

    @see Ciphertext for the class that stores ciphertexts.
    */
    class Plaintext
    {
    public:
        /**
        Constructs an empty plaintext allocating no memory.
        */
        Plaintext() = default;

        /**
        Constructs an empty plaintext allocating no memory. The memory pool is set to the pool pointed
        to by the given MemoryPoolHandle.

        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if pool is uninitialized
        */
        Plaintext(const MemoryPoolHandle &pool) :
            pool_(pool)
        {
            if (!pool)
            {
                throw std::invalid_argument("pool is uninitialized");
            }
        }

        /**
        Constructs a plaintext representing a constant polynomial 0. The coefficient count of the
        polynomial is set to the given value. The capacity is set to the same value. The memory pool
        is set to the pool pointed to by the given MemoryPoolHandle, or the global memory pool by default.

        @param[in] coeff_count The number of (zeroed) coefficients in the plaintext polynomial
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if coeff_count is negative
        @throws std::invalid_argument if pool is uninitialized
        */
        explicit Plaintext(int coeff_count, const MemoryPoolHandle &pool = MemoryPoolHandle::Global())
        {
            // Reserve memory, resize, and set to zero
            resize(coeff_count, pool);
        }

        /**
        Constructs a plaintext representing a constant polynomial 0. The coefficient count of the
        polynomial and the capacity are set to the given values. The memory pool is set to the pool
        pointed to by the given MemoryPoolHandle, or the global memory pool by default.

        @param[in] capacity The capacity
        @param[in] coeff_count The number of (zeroed) coefficients in the plaintext polynomial
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if capacity is less than coeff_count
        @throws std::invalid_argument if coeff_count is negative
        @throws std::invalid_argument if pool is uninitialized
        */
        Plaintext(int capacity, int coeff_count, const MemoryPoolHandle &pool = MemoryPoolHandle::Global())
        {
            if (capacity < coeff_count)
            {
                throw std::invalid_argument("capacity cannot be smaller than coeff_count");
            }

            // Reserve memory, resize, and set to zero
            reserve(capacity, pool);
            resize(coeff_count, pool);
        }

        /**
        Constructs an aliased plaintext with backing array located at the given address and with given
        coefficient count. The capacity is set to same value.

        @param[in] coeff_count The number of coefficients in the plaintext polynomial
        @param[in] poly A pointer to the backing array of the plaintext
        @throws std::invalid_argument if coeff_count is negative
        @throws std::invalid_argument if poly is null and coeff_count is positive
        */
        explicit Plaintext(int coeff_count, std::uint64_t *poly)
        {
            alias(coeff_count, poly);
        }

        /**
        Constructs an aliased plaintext with backing array located at the given address, with given
        coefficient count and given capacity.

        @param[in] capacity The capacity
        @param[in] coeff_count The number of coefficients in the plaintext polynomial
        @param[in] poly A pointer to the backing array of the plaintext
        @throws std::invalid_argument if capacity is less than coeff_count
        @throws std::invalid_argument if coeff_count is negative
        @throws std::invalid_argument if poly is null and coeff_count is positive
        */
        explicit Plaintext(int capacity, int coeff_count, std::uint64_t *poly)
        {
            alias(capacity, coeff_count, poly);
        }

        /**
        Constructs a plaintext and sets its value to the polynomial represented by the given BigPoly.
        The memory pool is set to the pool pointed to by the given MemoryPoolHandle, or the global
        memory pool by default.

        @param[in] poly The plaintext polynomial
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if pool is uninitialized
        */
        Plaintext(const BigPoly &poly, const MemoryPoolHandle &pool = MemoryPoolHandle::Global()) :
            pool_(pool)
        {
            if (!pool)
            {
                throw std::invalid_argument("pool is uninitialized");
            }
            operator =(poly);
        }

        /**
        Constructs a plaintext from a given hexadecimal string describing the plaintext polynomial.
        The memory pool is set to the pool pointed to by the given MemoryPoolHandle, or the global
        memory pool by default.

        The string description of the polynomial must adhere to the format returned by to_string(),
        which is of the form "7FFx^3 + 1x^1 + 3" and summarized by the following rules:
        1. Terms are listed in order of strictly decreasing exponent
        2. Coefficient values are non-negative and in hexadecimal format (upper and lower case letters are both supported)
        3. Exponents are positive and in decimal format
        4. Zero coefficient terms (including the constant term) may be (but do not have to be) omitted
        5. Term with the exponent value of one must be exactly written as x^1
        6. Term with the exponent value of zero (the constant term) must be written as just a hexadecimal number without exponent
        7. Terms must be separated by exactly <space>+<space> and minus is not allowed
        8. Other than the +, no other terms should have whitespace

        @param[in] hex_poly The formatted polynomial string specifying the plaintext polynomial
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if hex_poly does not adhere to the expected format
        @throws std::invalid_argument if pool is uninitialized
        */
        Plaintext(const std::string &hex_poly, const MemoryPoolHandle &pool = MemoryPoolHandle::Global()) :
            pool_(pool)
        {
            if (!pool)
            {
                throw std::invalid_argument("pool is uninitialized");
            }
            operator =(hex_poly);
        }

        /**
        Constructs a new plaintext by copying a given one.

        @param[in] copy The plaintext to copy from
        */
        Plaintext(const Plaintext &copy) :
            pool_(copy.pool_ ? copy.pool_ : MemoryPoolHandle::Global()),
            capacity_(copy.capacity_),
            coeff_count_(copy.coeff_count_),
            // pool_ is guaranteed to be good at this point so allocate memory
            plaintext_poly_(util::allocate_uint(capacity_, pool_))
        {
            // Copy over value
            util::set_uint_uint(copy.plaintext_poly_.get(), coeff_count_, plaintext_poly_.get());
        }

        /**
        Constructs a new plaintext by moving a given one.

        @param[in] source The plaintext to move from
        */
        Plaintext(Plaintext &&source) = default;

        /**
        Changes the plaintext to be an aliased plaintext with backing array located at the given
        address and with given coefficient count. The capacity is set to equal the coefficient
        count. Any previous allocation is released back to the memory pool (unless aliased).

        @param[in] coeff_count The number of coefficients in the plaintext polynomial
        @param[in] poly A pointer to the backing array of the plaintext
        @throws std::invalid_argument if coeff_count is negative
        @throws std::invalid_argument if poly is null and coeff_count is positive
        */
        inline void alias(int coeff_count, std::uint64_t *poly)
        {
            if (coeff_count < 0)
            {
                throw std::invalid_argument("coeff_count cannot be negative");
            }
            if (poly == nullptr && coeff_count != 0)
            {
                throw std::invalid_argument("poly cannot be null");
            }

            capacity_ = coeff_count;
            coeff_count_ = coeff_count;
            plaintext_poly_ = util::Pointer::Aliasing(poly);
        }

        /**
        Changes the plaintext to be an aliased plaintext with backing array located at the given
        address and with given coefficient count. The capacity is set to equal the coefficient
        count. Any previous allocation is released back to the memory pool (unless aliased).

        @param[in] coeff_count The number of coefficients in the plaintext polynomial
        @param[in] poly A pointer to the backing array of the plaintext
        @throws std::invalid_argument if capacity is less than coeff_count
        @throws std::invalid_argument if coeff_count is negative
        @throws std::invalid_argument if poly is null and coeff_count is positive
        */
        inline void alias(int capacity, int coeff_count, std::uint64_t *poly)
        {
            if (capacity < coeff_count)
            {
                throw std::invalid_argument("capacity cannot be smaller than coeff_count");
            }
            if (coeff_count < 0)
            {
                throw std::invalid_argument("coeff_count cannot be negative");
            }
            if (poly == nullptr && coeff_count != 0)
            {
                throw std::invalid_argument("poly cannot be null");
            }

            capacity_ = capacity;
            coeff_count_ = coeff_count;
            plaintext_poly_ = util::Pointer::Aliasing(poly);
        }

        /**
        Reallocates an aliased plaintext from the memory pool pointed to by the given
        MemoryPoolHandle. An aliased plaintext is no longer aliased after this function is called.

        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if the plaintext is not aliased
        @throws std::invalid_argument if pool is uninitialized
        */
        void unalias(const MemoryPoolHandle &pool);

        /**
        Reallocates an aliased plaintext from the memory pool pointed to by the currently held
        MemoryPoolHandle. If the currently held MemoryPoolHandle is uninitialized, it is set to
        point to the global memory pool instead, and the memory is allocated from the global
        memory pool. An aliased plaintext is no longer aliased after this function is called.

        @throws std::invalid_argument if the plaintext is not aliased
        */
        inline void unalias()
        {
            if (!pool_)
            {
                pool_ = MemoryPoolHandle::Global();
            }
            unalias(pool_);
        }

        /**
        Allocates enough memory to accommodate the backing array of a plaintext with given
        capacity. The allocation is made from the memory pool pointed to by the given
        MemoryPoolHandle.

        @param[in] capacity The capacity
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if capacity is negative
        @throws std::invalid_argument if pool is uninitialized
        @throws std::logic_error if the plaintext is aliased
        */
        void reserve(int capacity, const MemoryPoolHandle &pool);

        /**
        Allocates enough memory to accommodate the backing array of a plaintext with given
        capacity. The allocation is made from the memory pool pointed to by the currently held
        MemoryPoolHandle. If the currently held MemoryPoolHandle is uninitialized, it is set to
        point to the global memory pool instead, and the memory is allocated from the global
        memory pool.

        @param[in] capacity The capacity
        @throws std::invalid_argument if capacity is negative
        @throws std::logic_error if the plaintext is aliased
        */
        inline void reserve(int capacity)
        {
            if (!pool_)
            {
                pool_ = MemoryPoolHandle::Global();
            }
            reserve(capacity, pool_);
        }

        /**
        Resets the plaintext. This function releases any memory allocated by the plaintext,
        returning it to the memory pool pointed to by the current MemoryPoolHandle, if the plaintext
        is not aliased.
        */
        inline void release()
        {
            capacity_ = 0;
            coeff_count_ = 0;
            plaintext_poly_.release();
        }

        /**
        Resizes the plaintext to have a given coefficient count. The plaintext is automatically
        reallocated if the new coefficient count does not fit in the current capacity. If the
        plaintext is not aliased, the allocation is made from the memory pool pointed to by the
        given MemoryPoolHandle. If the plaintext is aliased, the coefficient count is simply
        increased within the current capacity, and an exception is thrown if the current capacity
        is exceeded.

        @param[in] coeff_count The number of coefficients in the plaintext polynomial
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if coeff_count is negative
        @throws std::invalid_argument if pool is uninitialized and the plaintext is not aliased
        @throws std::invalid_argument if pool is different from the current pool and the plaintext is aliased
        @throws std::logic_error if coeff_count is larger than the current capacity and the plaintext is aliased
        */
        void resize(int coeff_count, const MemoryPoolHandle &pool);

        /**
        Resizes the plaintext to have a given coefficient count. The plaintext is automatically
        reallocated if the new coefficient count does not fit in the current capacity. If the
        plaintext is not aliased, the allocation is made from the memory pool pointed to by the
        currently held MemoryPoolHandle. If the currently held MemoryPoolHandle is uninitialized,
        it is set to point to the global memory pool instead, and the memory is allocated from the
        global memory pool. If the plaintext is aliased, the coefficient count is simply increased
        within the current capacity, and an exception is thrown if the current capacity is exceeded.

        @param[in] coeff_count The number of coefficients in the plaintext polynomial
        @throws std::invalid_argument if coeff_count is negative
        @throws std::logic_error if coeff_count is larger than the current capacity and the plaintext is aliased
        */
        inline void resize(int coeff_count)
        {
            // Set the pool if not an alias and there is no pool set yet
            if (!is_alias() && !pool_)
            {
                pool_ = MemoryPoolHandle::Global();
            }
            resize(coeff_count, pool_);
        }

        /**
        Copies a given plaintext to the current one.

        @param[in] assign The plaintext to copy from
        */
        inline Plaintext &operator =(const Plaintext &assign)
        {
            // Check for self-assignment
            if (this == &assign)
            {
                return *this;
            }

            // First resize to correct size
            resize(assign.coeff_count_);

            // Size is guaranteed to be OK now so copy over
            // Note: set_uint_uint checks if the value pointers are equal and makes a copy only if they are not
            util::set_uint_uint(assign.plaintext_poly_.get(), coeff_count_, plaintext_poly_.get());

            return *this;
        }

        /**
        Moves a given plaintext to the current one.

        @param[in] assign The plaintext to move from
        */
        Plaintext &operator =(Plaintext &&assign) = default;

        /**
        Sets the value of the current plaintext to the polynomial represented by the given BigPoly.

        @param[in] poly The plaintext polynomial
        @throws std::invalid_argument if the coefficients of poly are too wide
        @throws std::logic_error if the plaintext is aliased and the capacity is too small to fit poly
        */
        Plaintext &operator =(const BigPoly &poly);

        /**
        Sets the value of the current plaintext to the polynomial represented by the a given hexadecimal string.

        The string description of the polynomial must adhere to the format returned by to_string(),
        which is of the form "7FFx^3 + 1x^1 + 3" and summarized by the following rules:
        1. Terms are listed in order of strictly decreasing exponent
        2. Coefficient values are non-negative and in hexadecimal format (upper and lower case letters are both supported)
        3. Exponents are positive and in decimal format
        4. Zero coefficient terms (including the constant term) may be (but do not have to be) omitted
        5. Term with the exponent value of one must be exactly written as x^1
        6. Term with the exponent value of zero (the constant term) must be written as just a hexadecimal number without exponent
        7. Terms must be separated by exactly <space>+<space> and minus is not allowed
        8. Other than the +, no other terms should have whitespace

        @param[in] hex_poly The formatted polynomial string specifying the plaintext polynomial
        @throws std::invalid_argument if hex_poly does not adhere to the expected format
        @throws std::invalid_argument if the coefficients of hex_poly are too wide
        @throws std::logic_error if the plaintext is aliased and the capacity is too small to fit hex_poly
        */
        Plaintext &operator =(const std::string &hex_poly);

        /**
        Sets the value of the current plaintext to a given constant polynomial. The coefficient count
        is set to one.

        @param[in] const_coeff The constant coefficient
        @throws std::logic_error if the plaintext is aliased and has a capacity of zero
        */
        Plaintext &operator =(std::uint64_t const_coeff)
        {
            resize(1);
            plaintext_poly_[0] = const_coeff;
            return *this;
        }

        /**
        Sets a given range of coefficients of a plaintext polynomial to zero.

        @param[in] start_coeff The index of the first coefficient to set to zero
        @param[in] length The number of coefficients to set to zero
        @throws std::out_of_range if start_coeff is not within [0, coeff_count)
        @throws std::out_of_range if length is negative or start_coeff + length is not within [0, coeff_count)
        */
        inline void set_zero(int start_coeff, int length)
        {
            if (start_coeff < 0 || start_coeff >= coeff_count_)
            {
                throw std::out_of_range("start_coeff must be within [0, coeff_count)");
            }
            if (length < 0 || start_coeff + length >= coeff_count_)
            {
                throw std::out_of_range("length must be non-negative and start_coeff + length must be within [0, coeff_count)");
            }
            util::set_zero_uint(length, plaintext_poly_.get() + start_coeff);
        }

        /**
        Sets the plaintext polynomial coefficients to zero starting at a given index.

        @param[in] start_coeff The index of the first coefficient to set to zero
        @throws std::out_of_range if start_coeff is not within [0, coeff_count)
        */
        inline void set_zero(int start_coeff)
        {
            if (start_coeff < 0 || start_coeff >= coeff_count_)
            {
                throw std::out_of_range("start_coeff must be within [0, coeff_count)");
            }
            set_zero(start_coeff, coeff_count_ - start_coeff);
        }

        /**
        Sets the plaintext polynomial to zero.
        */
        inline void set_zero()
        {
            util::set_zero_uint(coeff_count_, plaintext_poly_.get());
        }

        /**
        Returns a pointer to the beginning of the plaintext polynomial.
        */
        inline std::uint64_t *pointer()
        {
            return plaintext_poly_.get();
        }

        /**
        Returns a constant pointer to the beginning of the plaintext polynomial.
        */
        inline const std::uint64_t *pointer() const
        {
            return plaintext_poly_.get();
        }

        /**
        Returns a pointer to a given coefficient of the plaintext polynomial.

        @param[in] coeff_index The index of the coefficient in the plaintext polynomial
        @throws std::out_of_range if coeff_index is not within [0, coeff_count)
        */
        inline std::uint64_t *pointer(int coeff_index)
        {
            if (coeff_count_ == 0)
            {
                return nullptr;
            }
            if (coeff_index < 0 || coeff_index >= coeff_count_)
            {
                throw std::out_of_range("coeff_index must be within [0, coeff_count)");
            }
            return plaintext_poly_.get() + coeff_index;
        }

        /**
        Returns a constant pointer to a given coefficient of the plaintext polynomial.

        @param[in] coeff_index The index of the coefficient in the plaintext polynomial
        @throws std::out_of_range if coeff_index is not within [0, coeff_count)
        */
        inline const std::uint64_t *pointer(int coeff_index) const
        {
            if (coeff_count_ == 0)
            {
                return nullptr;
            }
            if (coeff_index < 0 || coeff_index >= coeff_count_)
            {
                throw std::out_of_range("coeff_index must be within [0, coeff_count)");
            }
            return plaintext_poly_.get() + coeff_index;
        }

        /**
        Returns a constant reference to a given coefficient of the plaintext polynomial.

        @param[in] coeff_index The index of the coefficient in the plaintext polynomial
        @throws std::out_of_range if coeff_index is not within [0, coeff_count)
        */
        inline const std::uint64_t &operator[](int coeff_index) const
        {
            if (coeff_index < 0 || coeff_index >= coeff_count_)
            {
                throw std::out_of_range("coeff_index must be within [0, coeff_count)");
            }
            return plaintext_poly_[coeff_index];
        }

        /**
        Returns a reference to a given coefficient of the plaintext polynomial.

        @param[in] coeff_index The index of the coefficient in the plaintext polynomial
        @throws std::out_of_range if coeff_index is not within [0, coeff_count)
        */
        inline std::uint64_t &operator[](int coeff_index)
        {
            if (coeff_index < 0 || coeff_index >= coeff_count_)
            {
                throw std::out_of_range("coeff_index must be within [0, coeff_count)");
            }
            return plaintext_poly_[coeff_index];
        }

        inline std::uint64_t coeff_at(int coeff_index) {
          if (coeff_index < 0 || coeff_index >= coeff_count_)
          {
              throw std::out_of_range("coeff_index must be within [0, coeff_count)");
          }
          return plaintext_poly_[coeff_index];
        }

        /**
        Returns whether or not the plaintext has the same semantic value as a given plaintext.
        Leading zero coefficients are ignored by the comparison.

        @param[in] compare The plaintext to compare against
        */
        inline bool operator ==(const Plaintext &compare) const
        {
            int sig_coeff_count = significant_coeff_count();
            int sig_coeff_count_compare = compare.significant_coeff_count();
            return (sig_coeff_count == sig_coeff_count_compare)
                && (util::is_equal_uint_uint(plaintext_poly_.get(), compare.plaintext_poly_.get(), sig_coeff_count)
                && util::is_zero_uint(plaintext_poly_.get() + sig_coeff_count, coeff_count_ - sig_coeff_count)
                && util::is_zero_uint(compare.plaintext_poly_.get() + sig_coeff_count, compare.coeff_count_ - sig_coeff_count));
        }

        /**
        Returns whether or not the plaintext has a different semantic value than a given
        plaintext. Leading zero coefficients are ignored by the comparison.

        @param[in] compare The plaintext to compare against
        */
        inline bool operator !=(const Plaintext &compare) const
        {
            return !operator ==(compare);
        }

        /**
        Returns whether the current plaintext polynomial has all zero coefficients.
        */
        inline bool is_zero() const
        {
            return (coeff_count_ == 0) || util::is_zero_uint(plaintext_poly_.get(), coeff_count_);
        }

        /**
        Returns whether the current plaintext is an alias.
        */
        inline bool is_alias() const
        {
            return plaintext_poly_.is_alias();
        }

        /**
        Returns the capacity of the current allocation.
        */
        inline int capacity() const
        {
            return capacity_;
        }

        /**
        Returns the coefficient count of the current plaintext polynomial.
        */
        inline int coeff_count() const
        {
            return coeff_count_;
        }

        /**
        Returns the significant coefficient count of the current plaintext polynomial.
        */
        inline int significant_coeff_count() const
        {
            if (coeff_count_ == 0)
            {
                return 0;
            }
            return util::get_significant_coeff_count_poly(plaintext_poly_.get(), coeff_count_, 1);
        }

        /**
        Returns a human-readable string description of the plaintext polynomial.

        The returned string is of the form "7FFx^3 + 1x^1 + 3" with a format summarized by the following:
        1. Terms are listed in order of strictly decreasing exponent
        2. Coefficient values are non-negative and in hexadecimal format (hexadecimal letters are in upper-case)
        3. Exponents are positive and in decimal format
        4. Zero coefficient terms (including the constant term) are omitted unless the polynomial is exactly 0 (see rule 9)
        5. Term with the exponent value of one is written as x^1
        6. Term with the exponent value of zero (the constant term) is written as just a hexadecimal number without x or exponent
        7. Terms are separated exactly by <space>+<space>
        8. Other than the +, no other terms have whitespace
        9. If the polynomial is exactly 0, the string "0" is returned
        */
        inline std::string to_string() const
        {
            return util::poly_to_hex_string(plaintext_poly_.get(), coeff_count_, 1);
        }

        /**
        Saves the Plaintext to an output stream. The output is in binary format and not human-readable.
        The output stream must have the "binary" flag set.

        @param[in] stream The stream to save the plaintext to
        @see load() to load a saved plaintext.
        */
        void save(std::ostream &stream) const;
        void python_save(std::string &path) const;

        /**
        Loads a Plaintext from an input stream overwriting the current plaintext.

        @param[in] stream The stream to load the plaintext from
        @see save() to save a plaintext.
        */
        void load(std::istream &stream);
        void python_load(std::string &path);

    private:
        MemoryPoolHandle pool_;

        int capacity_ = 0;

        int coeff_count_ = 0;

        util::Pointer plaintext_poly_;
    };
}
