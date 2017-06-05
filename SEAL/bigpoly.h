#pragma once

#include <iostream>
#include <cstdint>
#include <string>
#include "biguint.h"

namespace seal
{
    /**
    Represents a polynomial consisting of a set of unsigned integer coefficients with a specified bit width.
    Non-const BigPolys are mutable and able to be resized. Individual coefficients can be read/written through the
    operator[] function. A BigPoly has a set coefficient count (which can be read with coeff_count()) and coefficient
    bit width (which can be read with coeff_bit_count()), and all coefficients in a BigPoly have the same bit width.
    The coefficient count and bit width of a BigPoly is set initially by the constructor, and can be resized either
    explicitly with the resize() function, or implicitly with for example the operator=() function.

    @par Backing Array
    A BigPoly's coefficients are stored sequentially, index-zero coefficient first, in a contiguous std::uint64_t
    array. The width of each coefficient is rounded up to the next std::uint64_t width (i.e., to the next 64-bit
    boundary). The coeff_uint64_count() function returns the number of std::uint64_t values used per coefficient. The
    uint64_count() function returns the number of std::uint64_t values used to store all coefficients. Each
    coefficient is stored in an identical format to BigUInt, with the least quad word first and the order of bits
    for each quad word dependent on the architecture's std::uint64_t representation. For each coefficient, the bits higher
    than the coefficient bit count must be set to zero to prevent undefined behavior. The pointer() function returns 
    a pointer to the first std::uint64_t of the array.

    @par Implicit Resizing
    Both the copy constructor and operator=() allocate more memory for the backing array when needed, i.e. when
    the source polynomial has a larger backing array than the destination. Conversely, when the destination backing 
    array is already large enough, the data is only copied and the unnecessary higher degree coefficients are set
    to zero. When new memory has to be allocated, only the significant coefficients of the source polynomial
    are taken into account. This is is important, because it avoids unnecessary zero coefficients to be included 
    in the destination, which in some cases could accumulate and result in very large unnecessary allocations. 
    However, sometimes it is necessary to preserve the original coefficient count, even if some of the
    leading coefficients are zero. This comes up for example when copying individual polynomials of ciphertext 
    BigPolyArray objects, as these polynomials need to have the leading coefficient equal to zero to be considered
    valid by classes such as Evaluator and Decryptor. For this purpose BigPoly contains functions duplicate_from
    and duplicate_to, which create an exact copy of the source BigPoly.

    @par Alias BigPolys
    An aliased BigPoly (which can be determined with is_alias()) is a special type of BigPoly that does not manage
    its underlying std::uint64_t pointer that stores the coefficients. An aliased BigPoly supports most of the same
    operations as a non-aliased BigPoly, including reading/writing the coefficients and assignment, however an
    aliased BigPoly does not internally allocate or deallocate its backing array and, therefore, does not support resizing.
    Any attempt, either explicitly or implicitly, to resize the BigPoly will result in an exception being thrown.
    An aliased BigPoly can be created with the BigPoly(int, int, std::uint64_t*) constructor or the
    alias() function. Note that the pointer specified to be aliased must be deallocated externally after the BigPoly is
    no longer in use. Aliasing is useful in cases where it is desirable to not have each BigPoly manage its own memory
    allocation and/or to prevent unnecessary coefficient copying.

    @par Thread Safety
    In general, reading a BigPoly is thread-safe while mutating is not. Specifically, the backing array may be freed
    whenever a resize occurs, the BigPoly is destroyed, or alias() is called, which would invalidate the address returned by
    pointer() and the coefficients returned by operator[]. When it is known a resize will not occur, concurrent reading and
    mutating is safe as long as it is known that concurrent reading and mutating for the same coefficient will not occur,
    as the reader may see a partially updated coefficient value.

    @see BigPolyArith for arithmetic and modular functions on BigPolys.
    @see BigUInt for more details on the coefficients returned by operator[]().
    */
    class BigPoly
    {
    public:
        /**
        Creates an empty BigPoly with zero coefficients and zero coefficient bit width. No memory is allocated by this
        constructor.
        */
        BigPoly();

        /**
        Creates a zero-initialized BigPoly of the specified coefficient count and bit width.

        @param[in] coeff_count The number of coefficients
        @param[in] coeff_bit_count The bit width of each coefficient
        @throws std::invalid_argument if either coeff_count or coeff_bit_count is negative
        */
        BigPoly(int coeff_count, int coeff_bit_count);

        /**
        Creates a BigPoly populated and minimally sized to fit the polynomial described by the formatted string.

        The string description of the polynomial must adhere to the format returned by to_string(), which is of the form
        "7FFx^3 + 1x^1 + 3" and summarized by the following rules:
        1. Terms are listed in order of strictly decreasing exponent
        2. Coefficient values are non-negative and in hexadecimal format (upper and lower case letters are both supported)
        3. Exponents are positive and in decimal format
        4. Zero coefficient terms (including the constant term) may be (but do not have to be) omitted
        5. Term with the exponent value of one must be exactly written as x^1
        6. Term with the exponent value of zero (the constant term) must be written as just a hexadecimal number without exponent
        7. Terms must be separated by exactly <space>+<space> and minus is not allowed
        8. Other than the +, no other terms should have whitespace

        @param[in] hex_poly The formatted polynomial string specifying the initial value
        @throws std::invalid_argument if hex_poly does not adhere to the expected format
        */
        BigPoly(const std::string &hex_poly);

        /**
        Creates a BigPoly of the specified coefficient count and bit width and initializes it with the polynomial described by
        the formatted string.

        The string description of the polynomial must adhere to the format returned by to_string(), which is of the form
        "7FFx^3 + 1x^1 + 3" and summarized by the following rules:
        1. Terms are listed in order of strictly decreasing exponent
        2. Coefficient values are non-negative and in hexadecimal format (upper and lower case letters are both supported)
        3. Exponents are positive and in decimal format
        4. Zero coefficient terms (including the constant term) may be (but do not have to be) omitted
        5. Term with the exponent value of one must be exactly written as x^1
        6. Term with the exponent value of zero (the constant term) must be written as just a hexadecimal number without exponent
        7. Terms must be separated by exactly <space>+<space> and minus is not allowed
        8. Other than the +, no other terms should have whitespace

        @param[in] coeff_count The number of coefficients
        @param[in] coeff_bit_count The bit width of each coefficient
        @param[in] hex_poly The formatted polynomial string specifying the initial value
        @throws std::invalid_argument if either coeff_count or coeff_bit_count is negative
        @throws std::invalid_argument if hex_poly does not adhere to the expected format
        */
        BigPoly(int coeff_count, int coeff_bit_count, const std::string &hex_poly);

        /**
        Creates an aliased BigPoly with the specified coefficient count, bit width, and backing array. An aliased BigPoly
        does not internally allocate or deallocate the backing array, and instead uses the specified backing array for all
        read/write operations. Note that resizing is not supported by an aliased BigPoly and any required deallocation of the
        specified backing array must occur externally after the aliased BigPoly is no longer in use.

        @param[in] coeff_count The number of coefficients
        @param[in] coeff_bit_count The bit width of each coefficient
        @param[in] value The backing array to use
        @throws std::invalid_argument if coeff_count or coeff_bit_count is negative or value is null
        @see BigPoly for a more detailed description of aliased BigPolys.
        */
        BigPoly(int coeff_count, int coeff_bit_count, std::uint64_t *value);

        /**
        Creates a deep copy of a BigPoly. The created BigPoly will have the same coefficient count, coefficient bit count,
        and coefficient values as the original.

        @param[in] copy The BigPoly to copy from
        */
        BigPoly(const BigPoly &copy);

        /**
        Destroys the BigPoly and deallocates the backing array if it is not an aliased BigPoly.
        */
        ~BigPoly();

        /**
        Returns whether or not the BigPoly is an alias.

        @see BigPoly for a detailed description of aliased BigPolys.
        */
        bool is_alias() const
        {
            return is_alias_;
        }

        /**
        Returns the coefficient count for the BigPoly.
        
        @see significant_coeff_count() to instead ignore the leading coefficients that have a value of zero.
        */
        int coeff_count() const
        {
            return coeff_count_;
        }

        /**
        Returns the number of bits per coefficient.
        
        @see significant_coeff_bit_count() to instead get the number of significant bits of the largest coefficient in the BigPoly.
        */
        int coeff_bit_count() const
        {
            return coeff_bit_count_;
        }

        /**
        Returns a const pointer to the backing array storing all of the coefficient values. The pointer points to the beginning
        of the backing array where all coefficients are stored sequentially. The pointer points to the coefficient for the constant
        (degree 0) term of the BigPoly. Note that the return value will be nullptr if the coefficient count and/or bit count is zero.

        @warning The pointer is valid only until the backing array is freed, which occurs when the BigPoly is resized, destroyed,
        or the alias() function is called.
        @see coeff_uint64_count() to determine the number of std::uint64_t values used for each coefficient in the backing array.
        @see uint64_count() to determine the total number of std::uint64_t values in the backing array.
        @see BigPoly for a more detailed description of the format of the backing array.
        */
        const std::uint64_t *pointer() const
        {
            return value_;
        }

        /**
        Returns a pointer to the backing array storing all of the coefficient values. The pointer points to the beginning of the
        backing array where all coefficients are stored sequentially. The pointer points to the coefficient for the constant
        (degree 0) term of the BigPoly. Note that the return value will be nullptr if the coefficient count and/or bit count is zero.

        @warning The pointer is valid only until the backing array is freed, which occurs when the BigPoly is resized, destroyed,
        or the alias() function is called.
        @see coeff_uint64_count() to determine the number of std::uint64_t values used for each coefficient in the backing array.
        @see BigPoly for a more detailed description of the format of the backing array.
        */
        std::uint64_t *pointer()
        {
            return value_;
        }

        /**
        Returns the total number of std::uint64_t in the backing array to store all of the coefficients of the BigPoly.

        @see coeff_uint64_count() to determine the number of std::uint64_t values used for each individual coefficient.
        @see BigPoly for a more detailed description of the format of the backing array.
        */
        int uint64_count() const;

        /**
        Returns the number of std::uint64_t in the backing array used to store each coefficient.

        @see uint64_count() to determine the total number of std::uint64_t values used to store all of the coefficients.
        @see BigPoly for a more detailed description of the format of the backing array.
        */
        int coeff_uint64_count() const;

        /**
        Returns the coefficient count for the BigPoly ignoring all of the highest coefficients that have value zero.

        @see coeff_count() to instead return the coefficient count regardless of the number of leading zero terms.
        */
        int significant_coeff_count() const;

        /**
        Returns the number of significant bits of the largest coefficient in the BigPoly.

        @see coeff_bit_count() to instead return the coefficient bit count regardless of leading zero bits.
        */
        int significant_coeff_bit_count() const;

        /**
        Returns a human-readable string description of the BigPoly.

        The returned string is of the form "7FFx^3 + 1x^1 + 3" with a format summarized by the following:
        1. Terms are listed in order of strictly decreasing exponent
        2. Coefficient values are non-negative and in hexadecimal format (hexadecimal letters are in upper-case)
        3. Exponents are positive and in decimal format
        4. Zero coefficient terms (including the constant term) are omitted unless the BigPoly is exactly 0 (see rule 9)
        5. Term with the exponent value of one is written as x^1
        6. Term with the exponent value of zero (the constant term) is written as just a hexadecimal number without x or exponent
        7. Terms are separated exactly by <space>+<space>
        8. Other than the +, no other terms have whitespace
        9. If the BigPoly is exactly 0, the string "0" is returned
        */
        std::string to_string() const;

        /**
        Returns whether or not the BigPoly has the same value as a specified BigPoly. Value equality is not determined by the
        raw coefficient count or bit count, but rather if the significant bits of non-zero coefficients have the same value.

        @param[in] compare The BigPoly to compare against
        */
        bool operator ==(const BigPoly &compare) const;

        /**
        Returns whether or not the BigPoly has a different value than a specified BigPoly. Value equality is not determined by the
        raw coefficient count or bit count, but rather if the significant bits of non-zero coefficients have the same value.

        @param[in] compare The BigPoly to compare against
        */
        bool operator !=(const BigPoly &compare) const;

        /**
        Returns whether or not the BigPoly has all zero coefficients.
        */
        bool is_zero() const;

        /**
        Returns a BigUInt that can read the coefficient at the specified index. The BigUInt is an aliased BigUInt that points
        directly to the backing array of the BigPoly.

        @warning The returned BigUInt is an alias backed by a region of the BigPoly's backing array. As such, it is only valid until
        the BigPoly is resized, destroyed, or alias() is called.

        @param[in] coeff_index The index of the coefficient to read
        @throws std::out_of_range if coeff_index is not within [0, coeff_count())
        @see BigUInt for operations that can be performed on the coefficients.
        */
        const BigUInt &operator[](int coeff_index) const;

        /**
        Returns a BigUInt that can read or write the coefficient at the specified index. The BigUInt is an aliased BigUInt that points
        directly to the backing array of the BigPoly.

        @warning The returned BigUInt is an alias backed by a region of the BigPoly's backing array. As such, it is only valid until
        the BigPoly is resized, destroyed, or alias() is called.

        @param[in] coeff_index The index of the coefficient to read/write
        @throws std::out_of_range if coeff_index is not within [0, coeff_count())
        @see BigUInt for operations that can be performed on the coefficients.
        */
        BigUInt &operator[](int coeff_index);

        /**
        Sets all coefficients to have a value of zero. This does not resize the BigPoly.
        */
        void set_zero();

        /**
        Sets all coefficients within [start_coeff, coefficient count) to have a value of zero. This does not resize the BigPoly or
        modify the other coefficients.

        @param[in] start_coeff The index of the first coefficient to set to zero
        @throws out_of_range if start_coeff is not within [0, coeff_count()]
        */
        void set_zero(int start_coeff);

        /**
        Sets all coefficients within [start_coeff, start_coeff + length) to have a value of zero. This does not resize the BigPoly
        or modify the other coefficients.

        @param[in] start_coeff The index of the first coefficient to set to zero
        @param[in] length The number of coefficients to set to zero
        @throws std::out_of_range if start_coeff is not within [0, coeff_count()]
        @throws std::out_of_range if length is negative or start_coeff + length is not within [0, coeff_count()]
        */
        void set_zero(int start_coeff, int length);

        /**
        Resizes a BigPoly to the specified coefficient count and bit width, copying over and resizing existing coefficient values
        as much as will fit. If coefficient count is reduced, the leading coefficients are dropped. If coefficient count increases,
        the new coefficients are initialized to zero.

        @param[in] coeff_count The number of coefficients
        @param[in] coeff_bit_count The bit width of each coefficient
        @throws std::invalid_argument if either coeff_count or coeff_bit_count is negative
        @throws std::logic_error if the BigPoly is an alias
        */
        void resize(int coeff_count, int coeff_bit_count);

        /**
        Makes the BigPoly an aliased BigPoly with the specified coefficient count, bit width, and backing array. An aliased BigPoly
        does not internally allocate or deallocate the backing array, and instead uses the specified backing array for all
        read/write operations. Note that resizing is not supported by an aliased BigPoly and any required deallocation of the
        specified backing array must occur externally after the aliased BigPoly is no longer in use.

        @param[in] coeff_count The number of coefficients
        @param[in] coeff_bit_count The bit width of each coefficient
        @param[in] value The backing array to use
        @throws std::invalid_argument if coeff_count or coeff_bit_count is negative or value is null
        @see BigPoly for a more detailed description of aliased BigPolys.
        */
        void alias(int coeff_count, int coeff_bit_count, std::uint64_t *value);

        /**
        Resets an aliased BigPoly into an empty non-alias BigPoly with coefficient count and bit count of zero.

        @throws std::logic_error if BigPoly is not an alias
        */
        void unalias();

        /**
        Overwrites the BigPoly with the value of the specified BigPoly, enlarging if needed to fit the assigned value.
        Only significant coefficients and significant coefficient bit counts are used to size the BigPoly.

        @param[in] assign The BigPoly whose value should be assigned to the current BigPoly
        @throws std::logic_error if BigPoly is an alias and the assigned BigPoly is too large to fit the current coefficient count
        and/or bit width
        */
        BigPoly &operator =(const BigPoly& assign);

        /**
        Overwrites the BigPoly with the value of the polynomial described by the formatted string, enlarging if needed to fit
        the assigned value.

        The string description of the polynomial must adhere to the format returned by to_string(), which is of the form
        "7FFx^3 + 1x^1 + 3" and summarized by the following rules:
        1. Terms are listed in order of strictly decreasing exponent
        2. Coefficient values are non-negative and in hexadecimal format (upper and lower case letters are both supported)
        3. Exponents are positive and in decimal format
        4. Zero coefficient terms (including the constant term) may be (but do not have to be) omitted
        5. Term with the exponent value of one must be exactly written as x^1
        6. Term with the exponent value of zero (the constant term) must be written as just a hexadecimal number without exponent
        7. Terms must be separated by exactly <space>+<space> and minus is not allowed
        8. Other than the +, no other terms should have whitespace

        @param[in] hex_poly The formatted polynomial string specifying the value to set
        @throws std::invalid_argument if hex_poly does not adhere to the expected format
        @throws std::logic_error if BigPoly is an alias and the assigned polynomial is too large to fit the current coefficient count
        and/or bit width
        */
        BigPoly &operator =(const std::string &hex_poly);

        /**
        Saves the BigPoly to an output stream. The full state of the BigPoly is serialized, including leading zero
        coefficients and insignificant coefficient bits. The output is in binary format and not human-readable. The
        output stream must have the "binary" flag set.

        @param[in] stream The stream to save the BigPoly to
        @see load() to load a saved BigPoly.
        */
        void save(std::ostream &stream) const;

        /**
        Loads a BigPoly from an input stream overwriting the current BigPoly and enlarging if needed to fit the loaded
        BigPoly.

        @param[in] stream The stream to load the BigPoly from
        @throws std::logic_error if BigPoly is an alias and the loaded polynomial is too large to fit the current coefficient count
        and/or bit width
        @see save() to save a BigPoly.
        */
        void load(std::istream &stream);

        /**
        Duplicates the current BigPoly. The coefficient count, the coefficient bit count,
        and the value of the given BigPoly are set to be exactly the same as in the current one.

        @param[out] destination The BigPoly to overwrite with the duplicate
        @throws std::logic_error if the destination BigPoly is an alias
        */
        void duplicate_to(BigPoly &destination) const;

        /**
        Duplicates a given BigPoly. The coefficient count, the coefficient bit count,
        and the value of the current BigPoly are set to be exactly the same as in the given one.

        @param[in] value The BigPoly to duplicate
        @throws std::logic_error if the current BigPoly is an alias
        */
        void duplicate_from(const BigPoly &value);

        /**
        Moves an old BigPoly to the current one.

        @param[in] assign The BigPoly to move from
        */
        BigPoly &operator =(BigPoly &&assign);

        /**
        Creates a new BigPoly by moving an old one.

        @param[in] source The BigPoly to move from
        */
        BigPoly(BigPoly &&source) noexcept;

    private:
        /**
        Resets the entire state of the BigPoly to an empty, zero-sized state, freeing any memory it internally allocated.
        If the BigPoly was an alias, the backing array is not freed but the alias is no longer referenced.
        */
        void reset();

        /**
        Points to the backing array for the coefficients. This pointer will be set to nullptr if and only if the coefficient
        count or coefficient bit count is zero. This pointer is automatically allocated and freed by the BigPoly if and
        only if the BigPoly is not an alias. If the BigPoly is an alias, then the pointer was passed-in to a constructor
        or alias() call, and will not be deallocated by the BigPoly.

        @see BigPoly for more information about aliased BigPolys or the format of the backing array.
        */
        std::uint64_t *value_;

        /**
        Points to an array of BigUInt's for the return value of the operator[] function. This array is always allocated and
        deallocated by the BigPoly regardless of whether or not it is an alias. Each BigUInt is itself an alias that
        points to the corresponding coefficient's location in the backing array value_. This pointer will be set to nullptr
        if and only if the coefficient count is zero.
        */
        BigUInt *coeffs_;

        /**
        The number of coefficients in the BigPoly.
        */
        int coeff_count_;

        /**
        The number of bits per coefficient. Note that storage requirements may be more than the bit count because the space
        reserved for each coefficient is rounded up to the next std::uint64_t boundary (i.e., the next 64-bit boundary).

        @see BigPoly for more information about the format of the backing array.
        */
        int coeff_bit_count_;

        /**
        Specifies whether or not the BigPoly is an alias. An alias BigPoly does not allocate its own backing array
        and instead uses the pointer of a backing array passed in to the constructor or alias() function.

        @see BigPoly for more information about the format of the backing array.
        */
        bool is_alias_;
    };
}