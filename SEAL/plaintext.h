#pragma once

#include <string>
#include <iostream>
#include <utility>
#include "bigpoly.h"

namespace seal
{
    /**
    Represents a plaintext element. Currently the Plaintext class simply wraps an instance of 
    the BigPoly class. In particular, it does not perform any sanity checking on the BigPoly 
    that it wraps.
    */
    class Plaintext
    {
    public:
        /**
        Creates a Plaintext wrapping a constant polynomial 0.
        */
        Plaintext();    

        /**
        Creates a Plaintext by copying a given BigPoly instance. The created Plaintext 
        will wrap a duplicate of the given BigPoly.

        @param[in] poly The plaintext polynomial
        */
        Plaintext(const BigPoly &poly) : plaintext_poly_(poly)
        {
        }

        /**
        Creates a Plaintext by moving a given BigPoly instance.

        @param[in] poly The plaintext polynomial
        */
        Plaintext(BigPoly &&poly) noexcept : plaintext_poly_(std::move(poly))
        {
        }

        /**
        Creates a Plaintext from a given hexadecimal string describing the plaintext polynomial.

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
        */
        Plaintext(const std::string &hex_poly) : plaintext_poly_(hex_poly)
        {
        }

        /**
        Creates a new Plaintext by copying an old one.

        @param[in] copy The Plaintext to copy from
        */
        Plaintext(const Plaintext &copy) = default;

        /**
        Creates a new Plaintext by moving an old one.

        @param[in] source The Plaintext to move from
        */
        Plaintext(Plaintext &&source) = default;

        /**
        Copies an old Plaintext to the current one.

        @param[in] assign The Plaintext to copy from
        */
        Plaintext &operator =(const Plaintext &assign);

        /**
        Moves an old Plaintext to the current one.

        @param[in] assign The Plaintext to move from
        */
        Plaintext &operator =(Plaintext &&assign) = default;

        /**
        Sets the current Plaintext to wrap a given BigPoly by creating a copy of it.

        @param[in] poly The polynomial to copy from
        */
        Plaintext &operator =(const BigPoly &poly);

        /**
        Sets the current Plaintext to wrap a given BigPoly by moving it.

        @param[in] poly The polynomial to move from
        */
        Plaintext &operator =(BigPoly &&poly);

        /**
        Sets the underlying plaintext polynomial from a given hexadecimal string.

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
        */
        Plaintext &operator =(const std::string &hex_poly);

        /**
        Returns a reference to the underlying plaintext polynomial.
        */
        operator BigPoly &()
        {
            return plaintext_poly_;
        }

        /**
        Returns a constant reference to the underlying plaintext polynomial.
        */
        operator const BigPoly &() const
        {
            return plaintext_poly_;
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
        std::string to_string() const
        {
            return plaintext_poly_.to_string();
        }

        /**
        Saves the Plaintext to an output stream. The output is in binary format and not human-readable. 
        The output stream must have the "binary" flag set.

        @param[in] stream The stream to save the Plaintext to
        @see load() to load a saved Plaintext.
        */
        void save(std::ostream &stream) const;

        /**
        Loads a Plaintext from an input stream overwriting the current Plaintext.

        @param[in] stream The stream to load the Plaintext from
        @see save() to save a Plaintext.
        */
        void load(std::istream &stream);

    private:
        BigPoly plaintext_poly_;
    };
}