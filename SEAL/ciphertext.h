#pragma once

#include <string>
#include <iostream>
#include "bigpolyarray.h"

namespace seal
{
    /**
    Represents a ciphertext element. Currently the Ciphertext class simply wraps an instance of
    the BigPolyArray class. In particular, it does not perform any sanity checking on the BigPolyArray
    that it wraps.
    */
    class Ciphertext
    {
    public:
        /**
        Creates a Ciphertext by copying a given BigPolyArray instance. The created Ciphertext
        will wrap a duplicate of the given BigPolyArray.

        @param[in] poly_array The polynomial array
        */
        Ciphertext(const BigPolyArray &poly_array) : ciphertext_array_(poly_array)
        {
        }

        /**
        Creates a Ciphertext by moving a given BigPolyArray instance.

        @param[in] poly_array The polynomial array
        */
        Ciphertext(BigPolyArray &&poly_array) noexcept : ciphertext_array_(std::move(poly_array))
        {
        }

        /**
        Creates a new Ciphertext by copying an old one.

        @param[in] copy The Ciphertext to copy from
        */
        Ciphertext(const Ciphertext &copy) = default;

        /**
        Creates a new Ciphertext by movin an old one.

        @param[in] source The Ciphertext to move from
        */
        Ciphertext(Ciphertext &&source) = default;

        /**
        Copies an old Ciphertext to the current one.

        @param[in] assign The Ciphertext to copy from
        */
        Ciphertext &operator =(const Ciphertext &assign) = default;

        /**
        Moves an old Ciphertext to the current one.

        @param[in] assign The Ciphertext to move from
        */
        Ciphertext &operator =(Ciphertext &&assign) = default;

        /**
        Sets the current Ciphertext to wrap a given BigPolyArray by creating a copy of it.

        @param[in] poly_array The polynomial array to copy from
        */
        Ciphertext &operator =(const BigPolyArray &poly_array);

        /**
        Sets the current Ciphertext to wrap a given BigPolyArray by moving it.

        @param[in] poly_array The polynomial array to move from
        */
        Ciphertext &operator =(BigPolyArray &&poly_array);

        /**
        Returns a reference to the underlying BigPolyArray.
        */
        operator BigPolyArray &()
        {
            return ciphertext_array_;
        }

        /**
        Returns a constant reference to the underlying BigPolyArray.
        */
        operator const BigPolyArray &() const
        {
            return ciphertext_array_;
        }

        /**
        Returns the size of the ciphertext.
        */
        int size() const
        {
            return ciphertext_array_.size();
        }

        /**
        Saves the Ciphertext to an output stream. The output is in binary format and not human-readable.
        The output stream must have the "binary" flag set.

        @param[in] stream The stream to save the Ciphertext to
        @see load() to load a saved Ciphertext.
        */
        void save(std::ostream &stream) const;

        /**
        Loads a Ciphertext from an input stream overwriting the current Ciphertext.

        @param[in] stream The stream to load the Ciphertext from
        @see save() to save a Ciphertext.
        */
        void load(std::istream &stream);

    private:
        Ciphertext() = default;

        BigPolyArray ciphertext_array_;

        friend class Evaluator;
    };
}