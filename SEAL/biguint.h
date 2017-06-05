#pragma once

#include <iostream>
#include <cstdint>
#include <string>

namespace seal
{
    /**
    Represents an unsigned integer with a specified bit width. Non-const BigUInts are mutable and able to be resized.
    The bit count for a BigUInt (which can be read with bit_count()) is set initially by the constructor and can be
    resized either explicitly with the resize() function or implicitly with an assignment operation (e.g., operator=(),
    operator+=(), etc.). A rich set of unsigned integer operations are provided by the BigUInt class, including
    comparison, traditional arithmetic (addition, subtraction, multiplication, division), and modular arithmetic
    functions.

    @par Backing Array
    The backing array for a BigUInt stores its unsigned integer value as a contiguous std::uint64_t array. Each
    std::uint64_t in the array sequentially represents 64-bits of the integer value, with the least significant
    quad-word storing the lower 64-bits and the order of the bits for each quad word dependent on the architecture's
    std::uint64_t representation. The size of the array equals the bit count of the BigUInt (which can be read with
    bit_count()) rounded up to the next std::uint64_t boundary (i.e., rounded up to the next 64-bit boundary). The
    uint64_count() function returns the number of std::uint64_t in the backing array. The pointer() function returns
    a pointer to the first std::uint64_t in the array. Additionally, the operator[] function allows accessing the
    individual bytes of the integer value in a platform-independent way - for example, reading the third
    byte will always return bits 16-24 of the BigUInt value regardless of the platform being little-endian or
    big-endian.

    @par Implicit Resizing
    Both the copy constructor and operator=() allocate more memory for the backing array when needed, i.e. when
    the source BigUInt has a larger backing array than the destination. Conversely, when the destination backing
    array is already large enough, the data is only copied and the unnecessary higher order bits are set
    to zero. When new memory has to be allocated, only the significant bits of the source BigUInt are taken 
    into account. This is is important, because it avoids unnecessary zero bits to be included in the destination,
    which in some cases could accumulate and result in very large unnecessary allocations.
    However, sometimes it is necessary to preserve the original size, even if some of the leading bits are zero. 
    For this purpose BigUInt contains functions duplicate_from and duplicate_to, which create an exact copy of 
    the source BigUInt.

    @par Alias BigUInts
    An aliased BigUInt (which can be determined with is_alias()) is a special type of BigUInt that does not manage
    its underlying std::uint64_t pointer used to store the value. An aliased BigUInt supports most of the same
    operations as a non-aliased BigUInt, including reading and writing the value, however an aliased BigUInt does
    not internally allocate or deallocate its backing array and, therefore, does not support resizing. Any attempt,
    either explicitly or implicitly, to resize the BigUInt will result in an exception being thrown. An aliased
    BigUInt can be created with the BigUInt(int, std::uint64_t*) constructor or the alias() function. Note that the
    pointer specified to be aliased must be deallocated externally after the BigUInt is no longer in use. Aliasing
    is useful in cases where it is desirable to not have each BigUInt manage its own memory allocation and/or to
    prevent unnecessary copying. For example, BigPoly uses aliased BigUInt's to return BigUInt representations of
    its coefficients, where the aliasing allows read/writes to the BigUInt to refer directly to the coefficient's
    corresponding region in the backing array of the BigPoly.

    @par Thread Safety
    In general, reading a BigUInt is thread-safe while mutating is not. Specifically, the backing array may be freed
    whenever a resize occurs, the BigUInt is destroyed, or alias() is called, which would invalidate the address
    returned by pointer() and the byte references returned by operator[]. When it is known that a resize will not occur,
    concurrent reading and mutating will not inherently fail but it is possible for a read to see a partially updated
    value from a concurrent write.
    */
    class BigUInt
    {
    public:
        /**
        Creates an empty BigUInt with zero bit width. No memory is allocated by this constructor.
        */
        BigUInt();

        /**
        Creates a zero-initialized BigUInt of the specified bit width.

        @param[in] bit_count The bit width
        @throws std::invalid_argument if bit_count is negative
        */
        explicit BigUInt(int bit_count);

        /**
        Creates a BigUInt initialized and minimally sized to fit the unsigned hexadecimal integer specified by the
        string. The string matches the format returned by to_string() and must consist of only the characters
        0-9, A-F, or a-f, most-significant nibble first.

        @param[in] hex_value The hexadecimal integer string specifying the initial value
        @throws std::invalid_argument if hex_value does not adhere to the expected format
        */
        BigUInt(const std::string &hex_value);

        /**
        Creates a BigUInt of the specified bit width and initializes it with the unsigned hexadecimal integer
        specified by the string. The string must match the format returned by to_string() and must consist of only the
        characters 0-9, A-F, or a-f, most-significant nibble first.

        @param[in] bit_count The bit width
        @param[in] hex_value The hexadecimal integer string specifying the initial value
        @throws std::invalid_argument if bit_count is negative
        @throws std::invalid_argument if hex_value does not adhere to the expected format
        */
        BigUInt(int bit_count, const std::string &hex_value);

        /**
        Creates an aliased BigUInt with the specified bit width and backing array. An aliased BigUInt does not
        internally allocate or deallocate the backing array, and instead uses the specified backing array for all
        read/write operations. Note that resizing is not supported by an aliased BigUInt and any required deallocation
        of the specified backing array must occur externally after the aliased BigUInt is no longer in use.

        @param[in] bit_count The bit width
        @param[in] value The backing array to use
        @throws std::invalid_argument if bit_count is negative or value is null
        @see BigUInt for a more detailed description of aliased BigUInts.
        */
        BigUInt(int bit_count, std::uint64_t *value);

        /**
        Creates a BigUInt of the specified bit width and initializes it to the specified unsigned integer value.

        @param[in] bit_count The bit width
        @param[in] value The initial value to set the BigUInt
        @throws std::invalid_argument if bit_count is negative
        */
        BigUInt(int bit_count, std::uint64_t value);

        /**
        Creates a deep copy of a BigUInt. The created BigUInt will have the same bit count and value as the original.

        @param[in] copy The BigUInt to copy from
        */
        BigUInt(const BigUInt &copy);

        /**
        Destroys the BigUInt and deallocates the backing array if it is not an aliased BigUInt.
        */
        ~BigUInt();

        /**
        Returns whether or not the BigUInt is an alias.

        @see BigUInt for a detailed description of aliased BigUInt.
        */
        bool is_alias() const
        {
            return is_alias_;
        }

        /**
        Returns the bit count for the BigUInt.

        @see significant_bit_count() to instead ignore leading zero bits.
        */
        int bit_count() const
        {
            return bit_count_;
        }

        /**
        Returns a const pointer to the backing array storing the BigUInt value. The pointer points to the beginning
        of the backing array at the least-significant quad word.

        @warning The pointer is valid only until the backing array is freed, which occurs when the BigUInt is resized, destroyed,
        or the alias() function is called.

        @see uint64_count() to determine the number of std::uint64_t values in the backing array.
        @see BigUInt for a detailed description of the format of the backing array.
        */
        const std::uint64_t *pointer() const
        {
            return value_;
        }

        /**
        Returns a pointer to the backing array storing the BigUInt value. The pointer points to the beginning of the backing array
        at the least-significant quad word.

        @warning The pointer is valid only until the backing array is freed, which occurs when the BigUInt is resized, destroyed,
        or the alias() function is called.
        @see uint64_count() to determine the number of std::uint64_t values in the backing array.
        @see BigUInt for a detailed description of the format of the backing array.
        */
        std::uint64_t *pointer()
        {
            return value_;
        }

        /**
        Returns the number of bytes in the backing array used to store the BigUInt value.

        @see BigUInt for a detailed description of the format of the backing array.
        */
        int byte_count() const;

        /**
        Returns the number of std::uint64_t in the backing array used to store the BigUInt value.

        @see BigUInt for a detailed description of the format of the backing array.
        */
        int uint64_count() const;

        /**
        Returns the number of significant bits for the BigUInt.

        @see bit_count() to instead return the bit count regardless of leading zero bits.
        */
        int significant_bit_count() const;

        /**
        Returns the BigUInt value as a double. Note that precision may be lost during the conversion.
        */
        double to_double() const;

        /**
        Returns the BigUInt value as a hexadecimal string.
        */
        std::string to_string() const;

        /**
        Returns the BigUInt value as a decimal string.
        */
        std::string to_dec_string() const;

        /**
        Returns whether or not the BigUInt has the value zero.
        */
        bool is_zero() const;

        /**
        Returns the byte at the corresponding byte index of the BigUInt's integer value. The bytes of the BigUInt
        are indexed least-significant byte first.

        @param[in] index The index of the byte to read
        @throws std::out_of_range if index is not within [0, byte_count())
        @see BigUInt for a detailed description of the format of the backing array.
        */
        std::uint8_t operator[](int index) const;

        /**
        Returns an byte reference that can read/write the byte at the corresponding byte index of the BigUInt's integer value.
        The bytes of the BigUInt are indexed least-significant byte first.

        @warning The returned byte is an reference backed by the BigUInt's backing array. As such, it is only valid until the BigUInt
        is resized, destroyed, or alias() is called.

        @param[in] index The index of the byte to read
        @throws std::out_of_range if index is not within [0, byte_count())
        @see BigUInt for a detailed description of the format of the backing array.
        */
        std::uint8_t &operator[](int index);

        /**
        Sets the BigUInt value to zero. This does not resize the BigUInt.
        */
        void set_zero();

        /**
        Resizes the BigUInt to the specified bit width, copying over the old value as much as will fit.

        @param[in] bit_count The bit width
        @throws std::invalid_argument if bit_count is negative
        @throws std::logic_error if the BigUInt is an alias
        */
        void resize(int bit_count);

        /**
        Makes the BigUInt an aliased BigUInt with the specified bit width and backing array. An aliased BigUInt does not
        internally allocate or deallocate the backing array, and instead uses the specified backing array for all
        read/write operations. Note that resizing is not supported by an aliased BigUInt and any required deallocation
        of the specified backing array must occur externally after the aliased BigUInt is no longer in use.

        @param[in] bit_count The bit width
        @param[in] value The backing array to use
        @throws std::invalid_argument if bit_count is negative or value is null
        @see BigUInt for a more detailed description of aliased BigUInts.
        */
        void alias(int bit_count, std::uint64_t *value);

        /**
        Resets an aliased BigUInt into an empty non-alias BigUInt with bit count of zero.

        @throws std::logic_error if BigUInt is not an alias
        */
        void unalias();

        /**
        Overwrites the BigUInt with the value of the specified BigUInt, enlarging if needed to fit the assigned
        value. Only significant bits are used to size the BigUInt.

        @param[in] assign The BigUInt whose value should be assigned to the current BigUInt
        @throws std::logic_error if BigUInt is an alias and the assigned BigUInt is too large to fit the current bit width
        */
        BigUInt &operator =(const BigUInt &assign);

        /**
        Overwrites the BigUInt with the unsigned hexadecimal value specified by the string, enlarging if needed to fit the
        assigned value. The string must match the format returned by to_string() and must consist of only the characters
        0-9, A-F, or a-f, most-significant nibble first.

        @param[in] hex_value The hexadecimal integer string specifying the value to assign
        @throws std::invalid_argument if hex_value does not adhere to the expected format
        @throws std::logic_error if BigUInt is an alias and the assigned value is too large to fit the current bit width
        */
        BigUInt &operator =(const std::string &hex_value);

        /**
        Overwrites the BigUInt with the specified integer value, enlarging if needed to fit the value.

        @param[in] value The value to assign
        @throws std::logic_error if BigUInt is an alias and the significant bit count of value is too large to fit the
        current bit width
        */
        BigUInt &operator =(std::uint64_t value);

        /**
        Returns a copy of the BigUInt value resized to the significant bit count.
        */
        BigUInt operator +() const;

        /**
        Returns a negated copy of the BigUInt value. The bit count does not change.
        */
        BigUInt operator -() const;

        /**
        Returns an inverted copy of the BigUInt value. The bit count does not change.
        */
        BigUInt operator ~() const;

        /**
        Increments the BigUInt and returns the incremented value. The BigUInt will increment the bit count if needed to fit the carry.

        @throws std::logic_error if BigUInt is an alias and a carry occurs requiring the BigUInt to be resized
        */
        BigUInt &operator ++();

        /**
        Decrements the BigUInt and returns the decremented value. The bit count does not change.
        */
        BigUInt &operator --();

        /**
        Increments the BigUInt but returns its old value. The BigUInt will increment the bit count if needed to fit the carry.
        */
        BigUInt operator ++(int postfix);

        /**
        Decrements the BigUInt but returns its old value. The bit count does not change.
        */
        BigUInt operator --(int postfix);

        /**
        Adds two BigUInts and returns the sum. The input operands are not modified. The bit count of the sum is set to be one greater
        than the significant bit count of the larger of the two input operands.

        @param[in] operand2 The second operand to add
        */
        BigUInt operator +(const BigUInt &operand2) const;

        /**
        Adds a BigUInt and an unsigned integer and returns the sum. The input operands are not modified. The bit count of the sum is set
        to be one greater than the significant bit count of the larger of the two operands.

        @param[in] operand2 The second operand to add
        */
        BigUInt operator +(std::uint64_t operand2) const
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return *this + operand2uint;
        }

        /**
        Subtracts two BigUInts and returns the difference. The input operands are not modified. The bit count of the difference is set
        to be the significant bit count of the larger of the two input operands.

        @param[in] operand2 The second operand to subtract
        */
        BigUInt operator -(const BigUInt &operand2) const;

        /**
        Subtracts a BigUInt and an unsigned integer and returns the difference. The input operands are not modified. The bit count of
        the difference is set to be the significant bit count of the larger of the two operands.

        @param[in] operand2 The second operand to subtract
        */
        BigUInt operator -(std::uint64_t operand2) const
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return *this - operand2uint;
        }

        /**
        Multiplies two BigUInts and returns the product. The input operands are not modified. The bit count of the product is set
        to be the sum of the significant bit counts of the two input operands.

        @param[in] operand2 The second operand to multiply
        */
        BigUInt operator *(const BigUInt &operand2) const;

        /**
        Multiplies a BigUInt and an unsigned integer and returns the product. The input operands are not modified. The bit count of
        the product is set to be the sum of the significant bit counts of the two input operands.

        @param[in] operand2 The second operand to multiply
        */
        BigUInt operator *(std::uint64_t operand2) const
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return *this * operand2uint;
        }

        /**
        Divides two BigUInts and returns the quotient. The input operands are not modified. The bit count of the quotient is set
        to be the significant bit count of the first input operand.

        @param[in] operand2 The second operand to divide
        @throws std::invalid_argument if operand2 is zero
        */
        BigUInt operator /(const BigUInt &operand2) const;

        /**
        Divides a BigUInt and an unsigned integer and returns the quotient. The input operands are not modified. The bit count of the
        quotient is set to be the significant bit count of the first input operand.

        @param[in] operand2 The second operand to divide
        @throws std::invalid_argument if operand2 is zero
        */
        BigUInt operator /(std::uint64_t operand2) const
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return *this / operand2uint;
        }

        /**
        Divides two BigUInts and returns the remainder. The input operands are not modified. The bit count of the remainder is set
        to be the significant bit count of the first input operand.

        @param[in] operand2 The second operand to divide
        @throws std::invalid_argument iff operand2 is zero
        */
        BigUInt operator %(const BigUInt &operand2) const;

        /**
        Divides a BigUInt and an unsigned integer and returns the remainder. The input operands are not modified. The bit count of the
        remainder is set to be the significant bit count of the first input operand.

        @param[in] operand2 The second operand to divide
        @throws std::invalid_argument if operand2 is zero
        */
        BigUInt operator %(std::uint64_t operand2) const
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return *this % operand2uint;
        }

        /**
        Performs a bit-wise XOR operation between two BigUInts and returns the result. The input operands are not modified. The bit count
        of the result is set to the maximum of the two input operand bit counts.

        @param[in] operand2 The second operand to XOR
        */
        BigUInt operator ^(const BigUInt &operand2) const;

        /**
        Performs a bit-wise XOR operation between a BigUInt and an unsigned integer and returns the result. The input operands are not
        modified. The bit count of the result is set to the maximum of the two input operand bit counts.

        @param[in] operand2 The second operand to XOR
        */
        BigUInt operator ^(std::uint64_t operand2) const
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return *this ^ operand2uint;
        }

        /**
        Performs a bit-wise AND operation between two BigUInts and returns the result. The input operands are not modified. The bit count
        of the result is set to the maximum of the two input operand bit counts.

        @param[in] operand2 The second operand to AND
        */
        BigUInt operator &(const BigUInt &operand2) const;

        /**
        Performs a bit-wise AND operation between a BigUInt and an unsigned integer and returns the result. The input operands are not
        modified. The bit count of the result is set to the maximum of the two input operand bit counts.

        @param[in] operand2 The second operand to AND
        */
        BigUInt operator &(std::uint64_t operand2) const
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return *this & operand2uint;
        }

        /**
        Performs a bit-wise OR operation between two BigUInts and returns the result. The input operands are not modified. The bit count
        of the result is set to the maximum of the two input operand bit counts.

        @param[in] operand2 The second operand to OR
        */
        BigUInt operator |(const BigUInt &operand2) const;

        /**
        Performs a bit-wise OR operation between a BigUInt and an unsigned integer and returns the result. The input operands are not
        modified. The bit count of the result is set to the maximum of the two input operand bit counts.

        @param[in] operand2 The second operand to OR
        */
        BigUInt operator |(std::uint64_t operand2) const
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return *this | operand2uint;
        }

        /**
        Compares two BigUInts and returns -1, 0, or 1 if the BigUInt is less-than, equal-to, or greater-than the second operand
        respectively. The input operands are not modified.

        @param[in] compare The value to compare against
        */
        int compareto(const BigUInt &compare) const;

        /**
        Compares a BigUInt and an unsigned integer and returns -1, 0, or 1 if the BigUInt is less-than, equal-to, or greater-than the
        second operand respectively. The input operands are not modified.

        @param[in] compare The value to compare against
        */
        int compareto(std::uint64_t compare) const
        {
            BigUInt compareuint;
            compareuint = compare;
            return compareto(compareuint);
        }

        /**
        Returns whether or not a BigUInt is less-than a second BigUInt. The input operands are not modified.

        @param[in] operand2 The value to compare against
        */
        bool operator <(const BigUInt &operand2) const;

        /**
        Returns whether or not a BigUInt is less-than an unsigned integer. The input operands are not modified.

        @param[in] operand2 The value to compare against
        */
        bool operator <(std::uint64_t operand2) const
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return *this < operand2uint;
        }

        /**
        Returns whether or not a BigUInt is greater-than a second BigUInt. The input operands are not modified.

        @param[in] operand2 The value to compare against
        */
        bool operator >(const BigUInt &operand2) const;

        /**
        Returns whether or not a BigUInt is greater-than an unsigned integer. The input operands are not modified.

        @param[in] operand2 The value to compare against
        */
        bool operator >(std::uint64_t operand2) const
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return *this > operand2uint;
        }

        /**
        Returns whether or not a BigUInt is less-than or equal to a second BigUInt. The input operands are not modified.

        @param[in] operand2 The value to compare against
        */
        bool operator <=(const BigUInt &operand2) const;

        /**
        Returns whether or not a BigUInt is less-than or equal to an unsigned integer. The input operands are not modified.

        @param[in] operand2 The value to compare against
        */
        bool operator <=(std::uint64_t operand2) const
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return *this <= operand2uint;
        }

        /**
        Returns whether or not a BigUInt is greater-than or equal to a second BigUInt. The input operands are not modified.

        @param[in] operand2 The value to compare against
        */
        bool operator >=(const BigUInt &operand2) const;

        /**
        Returns whether or not a BigUInt is greater-than or equal to an unsigned integer. The input operands are not modified.

        @param[in] operand2 The value to compare against
        */
        bool operator >=(std::uint64_t operand2) const
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return *this >= operand2uint;
        }

        /**
        Returns whether or not a BigUInt is equal to a second BigUInt. The input operands are not modified.

        @param[in] compare The value to compare against
        */
        bool operator ==(const BigUInt &compare) const;

        /**
        Returns whether or not a BigUInt is equal to an unsigned integer. The input operands are not modified.

        @param[in] operand2 The value to compare against
        */
        bool operator ==(std::uint64_t operand2) const
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return *this == operand2uint;
        }

        /**
        Returns whether or not a BigUInt is not equal to a second BigUInt. The input operands are not modified.

        @param[in] compare The value to compare against
        */
        bool operator !=(const BigUInt &compare) const;

        /**
        Returns whether or not a BigUInt is not equal to an unsigned integer. The input operands are not modified.

        @param[in] operand2 The value to compare against
        */
        bool operator !=(std::uint64_t operand2) const
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return *this != operand2uint;
        }

        /**
        Returns a left-shifted copy of the BigUInt. The bit count of the returned value is the sum of the original
        significant bit count and the shift amount.

        @param[in] shift The number of bits to shift by
        @throws std::invalid_argument if shift is negative
        */
        BigUInt operator <<(int shift) const;

        /**
        Returns a right-shifted copy of the BigUInt. The bit count of the returned value is the original significant bit
        count subtracted by the shift amount (clipped to zero if negative).

        @param[in] shift The number of bits to shift by
        @throws std::invalid_argument if shift is negative
        */
        BigUInt operator >>(int shift) const;

        /**
        Adds two BigUInts saving the sum to the first operand, returning a reference of the first operand. The second input operand
        is not modified. The first operand is resized if and only if its bit count is smaller than one greater than the significant
        bit count of the larger of the two input operands.

        @param[in] operand2 The second operand to add
        @throws std::logic_error if the BigUInt is an alias and the operator attempts to enlarge the BigUInt to fit the result
        */
        BigUInt &operator +=(const BigUInt &operand2);

        /**
        Adds a BigUInt and an unsigned integer saving the sum to the first operand, returning a reference of the first operand. The
        second input operand is not modified. The first operand is resized if and only if its bit count is smaller than one greater
        than the significant bit count of the larger of the two input operands.

        @param[in] operand2 The second operand to add
        @throws std::logic_error if the BigUInt is an alias and the operator attempts to enlarge the BigUInt to fit the result
        */
        BigUInt &operator +=(std::uint64_t operand2)
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return *this += operand2uint;
        }

        /**
        Subtracts two BigUInts saving the difference to the first operand, returning a reference of the first operand. The second
        input operand is not modified. The first operand is resized if and only if its bit count is smaller than the significant
        bit count of the second operand.

        @param[in] operand2 The second operand to subtract
        @throws std::logic_error if the BigUInt is an alias and the operator attempts to enlarge the BigUInt to fit the result
        */
        BigUInt &operator -=(const BigUInt &operand2);

        /**
        Subtracts a BigUInt and an unsigned integer saving the difference to the first operand, returning a reference of the first
        operand. The second input operand is not modified. The first operand is resized if and only if its bit count is smaller than
        the significant bit count of the second operand.

        @param[in] operand2 The second operand to subtract
        @throws std::logic_error if the BigUInt is an alias and the operator attempts to enlarge the BigUInt to fit the result
        */
        BigUInt &operator -=(std::uint64_t operand2)
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return *this -= operand2uint;
        }

        /**
        Multiplies two BigUInts saving the product to the first operand, returning a reference of the first operand. The second
        input operand is not modified. The first operand is resized if and only if its bit count is smaller than the sum of the
        significant bit counts of the two input operands.

        @param[in] operand2 The second operand to multiply
        @throws std::logic_error if the BigUInt is an alias and the operator attempts to enlarge the BigUInt to fit the result
        */
        BigUInt &operator *=(const BigUInt &operand2);

        /**
        Multiplies a BigUInt and an unsigned integer saving the product to the first operand, returning a reference of the first
        operand. The second input operand is not modified. The first operand is resized if and only if its bit count is smaller than
        the sum of the significant bit counts of the two input operands.

        @param[in] operand2 The second operand to multiply
        @throws std::logic_error if the BigUInt is an alias and the operator attempts to enlarge the BigUInt to fit the result
        */
        BigUInt &operator *=(std::uint64_t operand2)
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return *this *= operand2uint;
        }

        /**
        Divides two BigUInts saving the quotient to the first operand, returning a reference of the first operand. The second
        input operand is not modified. The first operand is never resized.

        @param[in] operand2 The second operand to divide
        @throws std::invalid_argument if operand2 is zero
        */
        BigUInt &operator /=(const BigUInt &operand2);

        /**
        Divides a BigUInt and an unsigned integer saving the quotient to the first operand, returning a reference of the first operand.
        The second input operand is not modified. The first operand is never resized.

        @param[in] operand2 The second operand to divide
        @throws std::invalid_argument if operand2 is zero
        */
        BigUInt &operator /=(std::uint64_t operand2)
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return *this /= operand2uint;
        }

        /**
        Divides two BigUInts saving the remainder to the first operand, returning a reference of the first operand. The second
        input operand is not modified. The first operand is never resized.

        @param[in] operand2 The second operand to divide
        @throws std::invalid_argument if operand2 is zero
        */
        BigUInt &operator %=(const BigUInt &operand2);

        /**
        Divides a BigUInt and an unsigned integer saving the remainder to the first operand, returning a reference of the first operand.
        The second input operand is not modified. The first operand is never resized.

        @param[in] operand2 The second operand to divide
        @throws std::Invalid_argument if operand2 is zero
        */
        BigUInt &operator %=(std::uint64_t operand2)
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return *this %= operand2uint;
        }

        /**
        Performs a bit-wise XOR operation between two BigUInts saving the result to the first operand, returning a reference of the first
        operand. The second input operand is not modified. The first operand is resized if and only if its bit count is smaller than the
        significant bit count of the second operand.

        @param[in] operand2 The second operand to XOR
        @throws std::logic_error if the BigUInt is an alias and the operator attempts to enlarge the BigUInt to fit the result
        */
        BigUInt &operator ^=(const BigUInt &operand2);

        /**
        Performs a bit-wise XOR operation between a BigUInt and an unsigned integer saving the result to the first operand, returning a
        reference of the first operand. The second input operand is not modified. The first operand is resized if and only if its bit count
        is smaller than the significant bit count of the second operand.

        @param[in] operand2 The second operand to XOR
        @throws std::logic_error if the BigUInt is an alias and the operator attempts to enlarge the BigUInt to fit the result
        */
        BigUInt &operator ^=(std::uint64_t operand2)
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return *this ^= operand2uint;
        }

        /**
        Performs a bit-wise AND operation between two BigUInts saving the result to the first operand, returning a reference of the first
        operand. The second input operand is not modified. The first operand is resized if and only if its bit count is smaller than the
        significant bit count of the second operand.

        @param[in] operand2 The second operand to AND
        @throws std::logic_error if the BigUInt is an alias and the operator attempts to enlarge the BigUInt to fit the result
        */
        BigUInt &operator &=(const BigUInt &operand2);

        /**
        Performs a bit-wise AND operation between a BigUInt and an unsigned integer saving the result to the first operand, returning a
        reference of the first operand. The second input operand is not modified. The first operand is resized if and only if its bit count
        is smaller than the significant bit count of the second operand.

        @param[in] operand2 The second operand to AND
        @throws std::logic_error if the BigUInt is an alias and the operator attempts to enlarge the BigUInt to fit the result
        */
        BigUInt &operator &=(std::uint64_t operand2)
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return *this &= operand2uint;
        }

        /**
        Performs a bit-wise OR operation between two BigUInts saving the result to the first operand, returning a reference of the first
        operand. The second input operand is not modified. The first operand is resized if and only if its bit count is smaller than the
        significant bit count of the second operand.

        @param[in] operand2 The second operand to OR
        @throws std::logic_error if the BigUInt is an alias and the operator attempts to enlarge the BigUInt to fit the result
        */
        BigUInt &operator |=(const BigUInt &operand2);

        /**
        Performs a bit-wise OR operation between a BigUInt and an unsigned integer saving the result to the first operand, returning a
        reference of the first operand. The second input operand is not modified. The first operand is resized if and only if its bit count
        is smaller than the significant bit count of the second operand.

        @param[in] operand2 The second operand to OR
        @throws std::logic_error if the BigUInt is an alias and the operator attempts to enlarge the BigUInt to fit the result
        */
        BigUInt &operator |=(std::uint64_t operand2)
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return *this |= operand2uint;
        }

        /**
        Left-shifts a BigUInt by the specified amount. The BigUInt is resized if and only if its bit count is smaller than the sum of its
        significant bit count and the shift amount.

        @param[in] shift The number of bits to shift by
        @throws std::Invalid_argument if shift is negative
        @throws std::logic_error if the BigUInt is an alias and the operator attempts to enlarge the BigUInt to fit the result
        */
        BigUInt &operator <<=(int shift);

        /**
        Right-shifts a BigUInt by the specified amount. The BigUInt is never resized.

        @param[in] shift The number of bits to shift by
        @throws std::Invalid_argument if shift is negative
        */
        BigUInt &operator >>=(int shift);

        /**
        Divides two BigUInts and returns the quotient and sets the remainder parameter to the remainder. The bit count of the
        quotient is set to be the significant bit count of the BigUInt. The remainder is resized if and only if it is smaller
        than the bit count of the BigUInt.

        @param[in] operand2 The second operand to divide
        @param[out] remainder The BigUInt to store the remainder
        @throws std::Invalid_argument if operand2 is zero
        @throws std::logic_error if the remainder is an alias and the operator attempts to enlarge the BigUInt to fit the result
        */
        BigUInt divrem(const BigUInt &operand2, BigUInt &remainder) const;

        /**
        Divides a BigUInt and an unsigned integer and returns the quotient and sets the remainder parameter to the remainder.
        The bit count of the quotient is set to be the significant bit count of the BigUInt. The remainder is resized if and only
        if it is smaller than the bit count of the BigUInt.

        @param[in] operand2 The second operand to divide
        @param[out] remainder The BigUInt to store the remainder
        @throws std::Invalid_argument if operand2 is zero
        @throws std::logic_error if the remainder is an alias which the function attempts to enlarge to fit the result
        */
        BigUInt divrem(std::uint64_t operand2, BigUInt &remainder) const
        {
            BigUInt operand2uint;
            operand2uint = operand2;
            return divrem(operand2uint, remainder);
        }

        /**
        Returns the inverse of a BigUInt with respect to the specified modulus. The original BigUInt is not modified. The bit
        count of the inverse is set to be the significant bit count of the modulus.

        @param[in] modulus The modulus to calculate the inverse with respect to
        @throws std::Invalid_argument if modulus is zero
        @throws std::Invalid_argument if modulus is not greater than the BigUInt value
        @throws std::Invalid_argument if the BigUInt value and modulus are not co-prime
        */
        BigUInt modinv(const BigUInt &modulus) const;

        /**
        Returns the inverse of a BigUInt with respect to the specified modulus. The original BigUInt is not modified. The bit
        count of the inverse is set to be the significant bit count of the modulus.

        @param[in] modulus The modulus to calculate the inverse with respect to
        @throws std::Invalid_argument if modulus is zero
        @throws std::Invalid_argument if modulus is not greater than the BigUInt value
        @throws std::Invalid_argument if the BigUInt value and modulus are not co-prime
        */
        BigUInt modinv(std::uint64_t modulus) const
        {
            BigUInt modulusuint;
            modulusuint = modulus;
            return modinv(modulusuint);
        }

        /**
        Attempts to calculate the inverse of a BigUInt with respect to the specified modulus, returning whether or not the inverse
        was successful and setting the inverse parameter to the inverse. The original BigUInt is not modified. The inverse parameter
        is resized if and only if its bit count is smaller than the significant bit count of the modulus.

        @param[in] modulus The modulus to calculate the inverse with respect to
        @param[out] inverse Stores the inverse if the inverse operation was successful
        @throws std::Invalid_argument if modulus is zero
        @throws std::Invalid_argument if modulus is not greater than the BigUInt value
        @throws std::logic_error if the inverse is an alias which the function attempts to enlarge to fit the result
        */
        bool trymodinv(const BigUInt &modulus, BigUInt &inverse) const;

        /**
        Attempts to calculate the inverse of a BigUInt with respect to the specified modulus, returning whether or not the inverse
        was successful and setting the inverse parameter to the inverse. The original BigUInt is not modified. The inverse parameter
        is resized if and only if its bit count is smaller than the significant bit count of the modulus.

        @param[in] modulus The modulus to calculate the inverse with respect to
        @param[out] inverse Stores the inverse if the inverse operation was successful
        @throws std::Invalid_argument if modulus is zero
        @throws std::Invalid_argument if modulus is not greater than the BigUInt value
        @throws std::logic_error if the inverse is an alias which the function attempts to enlarge to fit the result
        */
        bool trymodinv(std::uint64_t modulus, BigUInt &inverse) const
        {
            BigUInt modulusuint;
            modulusuint = modulus;
            return trymodinv(modulusuint, inverse);
        }

        /**
        Saves the BigUInt to an output stream. The full state of the BigUInt is serialized, including insignificant bits. The output
        is in binary format and not human-readable. The output stream must have the "binary" flag set.

        @param[in] stream The stream to save the BigUInt to
        @see load() to load a saved BigUInt.
        */
        void save(std::ostream &stream) const;

        /**
        Loads a BigUInt from an input stream overwriting the current BigUInt and enlarging if needed to fit the loaded
        BigUInt.

        @param[in] stream The stream to load the BigUInt from
        @throws std::logic_error if BigUInt is an alias and the loaded BigUInt is too large to fit with the current bit width
        @see save() to save a BigUInt.
        */
        void load(std::istream &stream);

        /**
        Creates a minimally sized BigUInt initialized to the specified unsigned integer value.
        
        @param[in] value The value to initialized the BigUInt to
        */
        static BigUInt of(std::uint64_t value)
        {
            BigUInt result;
            result = value;
            return result;
        }

        /**
        Duplicates the current BigUInt. The bit count and the value of the given BigUInt
        are set to be exactly the same as in the current one.

        @param[out] destination The BigUInt to overwrite with the duplicate
        @throws std::logic_error if the destination BigPoly is an alias
        */
        void duplicate_to(BigUInt &destination) const;

        /**
        Duplicates a given BigUInt. The bit count and the value of the current BigUInt
        are set to be exactly the same as in the given one.

        @param[in] value The BigUInt to duplicate
        @throws std::logic_error if the current BigPoly is an alias
        */
        void duplicate_from(const BigUInt &value);
        
        /**
        Moves an old BigUInt to the current one.

        @param[in] assign The BigUInt to move from
        */
        BigUInt &operator =(BigUInt &&assign);

        /**
        Creates a new BigUInt by moving an old one.

        @param[in] source The BigUInt to move from
        */
        BigUInt(BigUInt &&source) noexcept;

    private:
        /**
        Resets the entire state of the BigUInt to an empty, zero-sized state, freeing any memory it internally allocated.
        If the BigUInt was an alias, the backing array is not freed but the alias is no longer referenced.
        */
        void reset();

        /**
        Points to the backing array for the BigUInt. This pointer will be set to nullptr if and only if the bit count
        is zero. This pointer is automatically allocated and freed by the BigUInt if and only if the BigUInt is not an alias.
        If the BigUInt is an alias, then the pointer was passed-in to a constructor or alias() call, and will not be
        deallocated by the BigUInt.

        @see BigUInt for more information about aliased BigUInts or the format of the backing array.
        */
        std::uint64_t *value_;

        /**
        The bit count for the BigUInt.
        */
        int bit_count_;

        /**
        Specifies whether or not the BigUInt is an alias. An alias BigUInt does not allocate its own backing array
        and instead uses the pointer of a backing array passed in to the constructor or alias() function.

        @see BigUInt for more information about the format of the backing array.
        */
        bool is_alias_;
    };
}