#pragma once

#include <iostream>
#include "bigpoly.h"
#include "util/common.h"

namespace seal 
{
	/**
	Represents an array of BigPoly objects. The BigPolyArray class provides all of the functionality of a BigPoly array. The size of
	the array (which can be read with size()) is set initially by the constructor and can be resized either with the resize()
	function or with assignment (operator=()). Each polynomial in the array must have the same coefficient count and coefficient
	bit count, which can be set in the constructor or with the resize() function, and read with coeff_count() and coeff_bit_count().
	The operator[] indexer functions allow reading/writing individual BigPoly's in the array by returning references to them.
	The array can be saved and loaded from a stream with the save() and load() functions.

	All of the polynomials in the array are stored in one continuous block in memory.

	@par Thread Safety
	In general, reading from a BigPolyArray instance is thread-safe while mutating is not. Specifically, the array may be freed
	whenever a resize() occurs, the BigPolyArray instance is destroyed, or an assignment operation occurs, which will invalidate
	the aliased BigPolys returned by operator[] function.
	*/
	class BigPolyArray 
	{
	public:
		/**
		Creates an empty BigPolyArray instance with a size of zero. No memory is allocated by this constructor.
		*/
		BigPolyArray();

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
		int size() const
		{
			return size_;
		}

		/**
		Returns the number of coefficients in the polynomials.
		*/
		int coeff_count() const
		{
			return coeff_count_;
		}

		/**
		Returns the coefficient bit count of the polynomials.
		*/
		int coeff_bit_count() const
		{
			return coeff_bit_count_;
		}

		/**
		Returns the number of std::uint64_t allocated for each coefficient of each polynomial.
		*/
		int coeff_uint64_count() const;

		/**
		Returns the number of std::uint64_t allocated for each polynomial.
		*/
		int poly_uint64_count() const;

		/**
		Returns the number of std::uint64_t allocated for the entire array of polynomials.
	   */
		int uint64_count() const;

		/**
		Returns a pointer to the backing array storing all of the polynomials. The pointer points to the beginning
		of the backing array of the constant term of the polynomial at index given by parameter poly_index. The return value
		will be nullptr if the coefficient count and/or bit count is zero.

		@warning The pointer is valid only until the backing array is freed, which occurs when the BigPolyArray is resized or destroyed.
		@throws std::out_of_range If poly_index is not within [0, size())
		*/
		std::uint64_t *pointer(int poly_index);

		/**
		Returns a const pointer to the backing array storing all of the polynomials. The pointer points to the beginning
		of the backing array of the constant term of the polynomial at index given by parameter poly_index. The return value
		will be nullptr if the coefficient count and/or bit count is zero.

		@warning The pointer is valid only until the backing array is freed, which occurs when the BigPolyArray is resized or destroyed.
		@throws std::out_of_range If poly_index is not within [0, size())
		*/
		const std::uint64_t *pointer(int poly_index) const;

		/**
		Returns a reference to the BigPoly at index poly_index in the array.

		@warning The returned BigPoly is an alias backed by the BigPolyArray internal array. As such, it is only valid until
		the BigPolyArray instance is resized or destroyed.
		@throws std::out_of_range If poly_index is not within [0, size())
		*/
		BigPoly &operator[](int poly_index);

		/**
		Returns a const reference to the BigPoly at index poly_index in the array.

		@warning The returned BigPoly is an alias backed by the BigPolyArray internal array. As such, it is only valid until
		the BigPolyArray instance is resized or destroyed.
		@throws std::out_of_range If poly_index is not within [0, size())
		*/
		const BigPoly &operator[](int poly_index) const;

		/**
		Sets all polynomials to have a value of zero. This does not resize the BigPolyArray.
		*/
		void set_zero();

		/**
		Sets the polynomial with index poly_index to have value zero. This does not resize the BigPolyArray or
		modify the other polynomials.

		@param[in] poly_index The index of the polynomial to set to zero
		@throws std::out_of_range If poly_index is not within [0, size())
		*/
		void set_zero(int poly_index);

		/**
		Resizes the BigPolyArray internal array to store the specified number of polynomials of the specified size, copying
		over the old polynomials as much as will fit.

		@param[in] size The number of polynomials to allocate space for
		@param[in] coeff_count The number of coefficients allocated for each polynomial in the array
		@param[in] coeff_bit_count The bit count of each polynomial in the array
		@throws std::invalid_argument if size is negative
		@throws std::invalid_argument if coeff_count is negative
		@throws std::invalid_argument if coeff_bit_count is negative
		*/
		void resize(int size, int coeff_count, int coeff_bit_count);

		/**
		Resets the BigPolyArray instance to an empty, zero-sized instance. Any space allocated by the BigPolyArray instance is
		deallocated.
		*/
		void reset();

		/**
		Overwrites the BigPolyArray instance with a specified BigPolyArray instance. After assignment, the
		size of BigPolyArray matches the size of the assigned BigPolyArray instance.

		@param[in] assign The BigPolyArray instance to whose value should be assigned to the current BigPolyArray instance
		*/
		BigPolyArray &operator =(const BigPolyArray &assign);

		/**
		Overwrites the BigPolyArray instance by moving the specified BigPolyArray instance. After assignment, the
		size of BigPolyArray matches the size of the original assigned BigPolyArray instance, and the memory location of
		the backing array is the same. The assigned instance is reset.

		@param[in] assign The BigPolyArray instance to whose value should be assigned to the current BigPolyArray instance
		*/
		BigPolyArray &operator =(BigPolyArray &&assign);

		/**
		Saves the BigPolyArray instance to an output stream. The output is in binary format and not human-readable. The output
		stream must have the "binary" flag set.

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
		/**
		The backing array for all of the polynomials in the BigPolyArray.
		*/
		std::uint64_t *value_;

		/**
		Points to an array of BigPolys for the return value of the operator[] function. Each BigPoly in polys_ is an alias that
		points to the corresponding polynomial's location in the backing array value_. This pointer will be set to nullptr
		if and only if size is zero.
		*/
		BigPoly* polys_;

		/**
		The number of polynomials stored in the BigPolyArray.
		*/
		int size_;

		/**
		The coefficient count of each polynomial in the BigPolyArray.
		*/
		int coeff_count_;

		/**
		The coefficient bit count of each polynomial in the BigPolyArray.
		*/
		int coeff_bit_count_;
	};
}