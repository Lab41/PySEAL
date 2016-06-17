#pragma once

#include "bigpolyarith.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class BigUInt;

            ref class BigPoly;

            /**
            <summary>Provides arithmetic and modular arithmetic functions for operating on polynomials (represented by
            BigPoly's).</summary>

            <remarks>
            <para>
            Provides arithmetic and modular arithmetic functions for operating on polynomials (represented by BigPoly's).
            </para>
            <para>
            All functions in BigPolyArith are not thread safe and a separate BigPolyArith instance is needed for each potentially
            concurrent invocation.
            </para>
            </remarks>
            <seealso cref="BigPoly">See BigPoly for more details on polynomials.</seealso>
            */
            public ref class BigPolyArithmetic
            {
            public:
                /**
                <summary>Creates an BigPolyArithmetic instance.</summary>
                */
                BigPolyArithmetic();

                /**
                <summary>Negates the coefficients of a specified polynomial modulo the specified coefficient modulus, and stores the
                result in result.</summary>

                <remarks>
                Negates the coefficients of a specified polynomial modulo the specified coefficient modulus, and stores the result in
                result. The input poly parameter is not modified. All coefficients in poly must be less-than coeffMod. The result
                polynomial is resized if it has fewer coefficients than poly's coefficient count or coeffMod's bit count.
                </remarks>
                <param name="poly">The polynomial to negate</param>
                <param name="coeffMod">The coefficient modulus</param>
                <param name="result">The polynomial to store the negated result</param>
                <exception cref="System::ArgumentNullException">If poly, coeffMod, or result is null</exception>
                <exception cref="System::ArgumentException">If coeffMod is zero</exception>
                <exception cref="System::ArgumentException">If poly has any coefficients greater than or equal to coeffMod</exception>
                <exception cref="System::InvalidOperationException">If result is an alias but needs to be resized to fit the
                result</exception>
                */
                void Negate(BigPoly ^poly, BigUInt ^coeffMod, BigPoly ^result);

                /**
                <summary>Negates the coefficients of a specified polynomial modulo the specified coefficient modulus, and returns the
                result.</summary>

                <remarks>
                Negates the coefficients of a specified polynomial modulo the specified coefficient modulus, and returns the result.
                The input poly parameter is not modified. All coefficients in poly must be less-than coeffMod. The returned polynomial
                is sized to have poly's coefficient count and coeffMod's bit count.
                </remarks>
                <param name="poly">The polynomial to negate</param>
                <param name="coeffMod">The coefficient modulus</param>
                <exception cref="System::ArgumentNullException">If poly or coeffMod is null</exception>
                <exception cref="System::ArgumentException">If coeffMod is zero</exception>
                <exception cref="System::ArgumentException">If poly has any coefficients greater than or equal to coeffMod</exception>
                */
                BigPoly ^Negate(BigPoly ^poly, BigUInt ^coeffMod);

                /**
                <summary>Negates the coefficients of a specified polynomial and stores the result in result.</summary>

                <remarks>
                Negates the coefficients of a specified polynomial and stores the result in result. The input poly parameter is not
                modified. The result polynomial is resized if it is smaller than poly's coefficient count or coefficient bit count.
                </remarks>
                <param name="poly">The polynomial to negate</param>
                <param name="result">The polynomial to store the negated result</param>
                <exception cref="System::ArgumentNullException">If poly or result is null</exception>
                <exception cref="System::InvalidOperationException">If result is an alias but needs to be resized to fit the
                result</exception>
                */
                void Negate(BigPoly ^poly, BigPoly ^result);

                /**
                <summary>Negates the coefficients of a specified polynomial and returns the result.</summary>

                <remarks>
                Negates the coefficients of a specified polynomial and returns the result. The input poly parameter is not modified.
                The returned polynomial is sized identical to poly.
                </remarks>
                <param name="poly">The polynomial to negate</param>
                <exception cref="System::ArgumentNullException">If poly is null</exception>
                */
                BigPoly ^Negate(BigPoly ^poly);

                /**
                <summary>Adds the poly1 and poly2 polynomials modulo the specified coefficient modulus, and stores the result in
                result.</summary>

                <remarks>
                Adds the poly1 and poly2 polynomials modulo the specified coefficient modulus, and stores the result in result. The
                input poly1 and poly2 polynomials are not modified. All coefficients in both polynomials must be less-than coeffMod.
                The result polynomial is resized if it has fewer coefficients than the larger of poly1 or poly2's coefficient counts,
                or if it's coefficient bit count is smaller than coeffMod's bit count.
                </remarks>
                <param name="poly1">The first polynomial to add</param>
                <param name="poly2">The second polynomial to add</param>
                <param name="coeffMod">The coefficient modulus</param>
                <param name="result">The polynomial to store the addition result</param>
                <exception cref="System::ArgumentNullException">If poly1, poly2, coeffMod, or result is null</exception>
                <exception cref="System::ArgumentException">If coeffMod is zero</exception>
                <exception cref="System::ArgumentException">If poly1 or poly2 has any coefficients greater than or equal to
                coeffMod</exception>
                <exception cref="System::InvalidOperationException">If result is an alias but needs to be resized to fit the
                result</exception>
                */
                void Add(BigPoly ^poly1, BigPoly ^poly2, BigUInt ^coeffMod, BigPoly ^result);

                /**
                <summary>Adds the poly1 and poly2 polynomials modulo the specified coefficient modulus, and returns the
                result.</summary>

                <remarks>
                Adds the poly1 and poly2 polynomials modulo the specified coefficient modulus, and returns the result. The input poly1
                and poly2 polynomials are not modified. All coefficients in both polynomials must be less-than coeffMod. The returned
                polynomial is sized to have a coefficient count equal to the larger of poly1 or poly2's coefficient counts, and a
                coefficient bit count equal to coeffMod's bit count.
                </remarks>
                <param name="poly1">The first polynomial to add</param>
                <param name="poly2">The second polynomial to add</param>
                <param name="coeffMod">The coefficient modulus</param>
                <exception cref="System::ArgumentNullException">If poly1, poly2, or coeffMod is null</exception>
                <exception cref="System::ArgumentException">If coeffMod is zero</exception>
                <exception cref="System::ArgumentException">If poly1 or poly2 has any coefficients greater than or equal to
                coeffMod</exception>
                */
                BigPoly ^Add(BigPoly ^poly1, BigPoly ^poly2, BigUInt ^coeffMod);

                /**
                <summary>Subtracts the poly1 and poly2 polynomials modulo the specified coefficient modulus, and stores the result in
                result.</summary>

                <remarks>
                Subtracts the poly1 and poly2 polynomials modulo the specified coefficient modulus, and stores the result in result.
                The input poly1 and poly2 polynomials are not modified. All coefficients in both polynomials must be less-than
                coeffMod. The result polynomial is resized if it has fewer coefficients than the larger of poly1 or poly2's
                coefficient counts, or if it's coefficient bit count is smaller than coeffMod's bit count.
                </remarks>
                <param name="poly1">The first polynomial to subtract</param>
                <param name="poly2">The second polynomial to subtract</param>
                <param name="coeffMod">The coefficient modulus</param>
                <param name="result">The polynomial to store the subtraction result</param>
                <exception cref="System::ArgumentNullException">If poly1, poly2, coeffMod, or result is null</exception>
                <exception cref="System::ArgumentException">If coeffMod is zero</exception>
                <exception cref="System::ArgumentException">If poly1 or poly2 has any coefficients greater than or equal to
                coeffMod</exception>
                <exception cref="System::InvalidOperationException">If result is an alias but needs to be resized to fit the
                result</exception>
                */
                void Sub(BigPoly ^poly1, BigPoly ^poly2, BigUInt ^coeffMod, BigPoly ^result);

                /**
                <summary>Subtracts the poly1 and poly2 polynomials modulo the specified coefficient modulus, and returns the
                result.</summary>

                <remarks>
                Subtracts the poly1 and poly2 polynomials modulo the specified coefficient modulus, and returns the result. The input
                poly1 and poly2 polynomials are not modified. All coefficients in both polynomials must be less-than coeffMod. The
                returned polynomial is sized to have a coefficient count equal to the larger of poly1 or poly2's coefficient counts,
                and a coefficient bit count equal to coeffMod's bit count.
                </remarks>
                <param name="poly1">The first polynomial to subtract</param>
                <param name="poly2">The second polynomial to subtract</param>
                <param name="coeffMod">The coefficient modulus</param>
                <exception cref="System::ArgumentNullException">If poly1, poly2, or coeffMod is null</exception>
                <exception cref="System::ArgumentException">If coeffMod is zero</exception>
                <exception cref="System::ArgumentException">If poly1 or poly2 has any coefficients greater than or equal to
                coeffMod</exception>
                */
                BigPoly ^Sub(BigPoly ^poly1, BigPoly ^poly2, BigUInt ^coeffMod);

                /**
                <summary>Adds the poly1 and poly2 polynomials, and stores the result in result.</summary>

                <remarks>
                Adds the poly1 and poly2 polynomials, and stores the result in result. The input poly1 and poly2 polynomials are not
                modified. The result polynomial is resized if it has fewer coefficients than the larger of poly1 or poly2's
                coefficient counts, or if it's coefficient bit count is smaller than one greater than the larger of poly1 and poly2's
                significant coefficient bit count.
                </remarks>
                <param name="poly1">The first polynomial to add</param>
                <param name="poly2">The second polynomial to add</param>
                <param name="result">The polynomial to store the addition result</param>
                <exception cref="System::ArgumentNullException">If poly1, poly2, or result is null</exception>
                <exception cref="System::InvalidOperationException">If result is an alias but needs to be resized to fit the
                result</exception>
                */
                void Add(BigPoly ^poly1, BigPoly ^poly2, BigPoly ^result);

                /**
                <summary>Adds the poly1 and poly2 polynomials, and returns the result.</summary>

                <remarks>
                Adds the poly1 and poly2 polynomials, and returns the result. The input poly1 and poly2 polynomials are not modified.
                The result polynomial is sized to have a coefficient count equal to the larger of poly1 or poly2's coefficient counts,
                and a coefficient bit count equal to one greater than the larger of poly1 and poly2's significant coefficient bit
                count.
                </remarks>
                <param name="poly1">The first polynomial to add</param>
                <param name="poly2">The second polynomial to add</param>
                <exception cref="System::ArgumentNullException">If poly1 or poly2 is null</exception>
                */
                BigPoly ^Add(BigPoly ^poly1, BigPoly ^poly2);

                /**
                <summary>Subtracts the poly1 and poly2 polynomials, and stores the result in result.</summary>

                <remarks>
                Subtracts the poly1 and poly2 polynomials, and stores the result in result. The input poly1 and poly2 polynomials are
                not modified. The result polynomial is resized if it has fewer coefficients than the larger of poly1 or poly2's
                coefficient counts, or if it's coefficient bit count is smaller than the larger of poly1 and poly2's coefficient bit
                count.
                </remarks>
                <param name="poly1">The first polynomial to subtract</param>
                <param name="poly2">The second polynomial to subtract</param>
                <param name="result">The polynomial to store the subtraction result</param>
                <exception cref="System::ArgumentNullException">If poly1, poly2, or result is null</exception>
                <exception cref="System::InvalidOperationException">If result is an alias but needs to be resized to fit the
                result</exception>
                */
                void Sub(BigPoly ^poly1, BigPoly ^poly2, BigPoly ^result);

                /**
                <summary>Subtracts the poly1 and poly2 polynomials, and returns the result.</summary>

                <remarks>
                Subtracts the poly1 and poly2 polynomials, and returns the result. The input poly1 and poly2 polynomials are not
                modified. The result polynomial is sized to have a coefficient count equal to the larger of poly1 or poly2's
                coefficient counts, and a coefficient bit count equal to the larger of poly1 and poly2's coefficient bit count.
                </remarks>
                <param name="poly1">The first polynomial to subtract</param>
                <param name="poly2">The second polynomial to subtract</param>
                <exception cref="System::ArgumentNullException">If poly1 or poly2 is null</exception>
                */
                BigPoly ^Sub(BigPoly ^poly1, BigPoly ^poly2);

                /**
                <summary>Multiplies the poly1 and poly2 polynomials modulo the specified polynomial and coefficient moduli, and stores
                the result in result.</summary>

                <remarks>
                Multiplies the poly1 and poly2 polynomials modulo the specified polynomial and coefficient moduli, and stores the
                result in result. The input poly1 and poly2 polynomials are not modified. Both polynomials must have fewer significant
                coefficients than polyMod and have coefficients less-than coeffMod. The result polynomial is resized if it has fewer
                coefficients than polyMod's significant coefficient count or if it's coefficient bit count is smaller than coeffMod's
                bit count.
                </remarks>
                <param name="poly1">The first polynomial to multiply</param>
                <param name="poly2">The second polynomial to multiply</param>
                <param name="polyMod">The polynomial modulus</param>
                <param name="coeffMod">The coefficient modulus</param>
                <param name="result">The polynomial to store the multiplication result</param>
                <exception cref="System::ArgumentNullException">If poly1, poly2, polyMod, coeffMod, or result is null</exception>
                <exception cref="System::ArgumentException">If polyMod is zero</exception>
                <exception cref="System::ArgumentException">If coeffMod is zero</exception>
                <exception cref="System::ArgumentException">If poly1 or poly2 has greater-than or equal significant coefficient count
                to polyMod or has any coefficients greater than or equal to coeffMod</exception>
                <exception cref="System::ArgumentException">If polyMod has any coefficients greater than or equal to
                coeffMod</exception>
                <exception cref="System::InvalidOperationException">If result is an alias but needs to be resized to fit the
                result</exception>
                */
                void Multiply(BigPoly ^poly1, BigPoly ^poly2, BigPoly ^polyMod, BigUInt ^coeffMod, BigPoly ^result);

                /**
                <summary>Multiplies the poly1 and poly2 polynomials modulo the specified polynomial and coefficient moduli, and returns
                the result.</summary>

                <remarks>
                Multiplies the poly1 and poly2 polynomials modulo the specified polynomial and coefficient moduli, and returns the
                result. The input poly1 and poly2 polynomials are not modified. Both polynomials must have fewer significant
                coefficients than polyMod and have coefficients less-than coeffMod. The returned polynomial is sized to have a
                coefficient count equal to polyMod's significant coefficient count and a coefficient bit count equal to coeffMod's bit
                count.
                </remarks>
                <param name="poly1">The first polynomial to multiply</param>
                <param name="poly2">The second polynomial to multiply</param>
                <param name="polyMod">The polynomial modulus</param>
                <param name="coeffMod">The coefficient modulus</param>
                <exception cref="System::ArgumentNullException">If poly1, poly2, polyMod, or coeffMod is null</exception>
                <exception cref="System::ArgumentException">If polyMod is zero</exception>
                <exception cref="System::ArgumentException">If coeffMod is zero</exception>
                <exception cref="System::ArgumentException">If poly1 or poly2 has greater-than or equal significant coefficient count
                to polyMod or has any coefficients greater than or equal to coeffMod</exception>
                <exception cref="System::ArgumentException">If polyMod has any coefficients greater than or equal to
                coeffMod</exception>
                */
                BigPoly ^Multiply(BigPoly ^poly1, BigPoly ^poly2, BigPoly ^polyMod, BigUInt ^coeffMod);

                /**
                <summary>Multiplies the poly1 and poly2 polynomials modulo the specified coefficient modulus, and stores the result in
                result.</summary>

                <remarks>
                Multiplies the poly1 and poly2 polynomials modulo the specified coefficient modulus, and stores the result in result.
                The input poly1 and poly2 polynomials are not modified. All coefficients in both polynomials must be less-than
                coeffMod. The result polynomial is resized if it has fewer coefficients than one less than the sum of poly1 and
                poly2's significant coefficient counts, or if it's coefficient bit count is smaller than coeffMod's bit count.
                </remarks>
                <param name="poly1">The first polynomial to multiply</param>
                <param name="poly2">The second polynomial to multiply</param>
                <param name="coeffMod">The coefficient modulus</param>
                <param name="result">The polynomial to store the multiplication result</param>
                <exception cref="System::ArgumentNullException">If poly1, poly2, coeffMod, or result is null</exception>
                <exception cref="System::ArgumentException">If coeffMod is zero</exception>
                <exception cref="System::ArgumentException">If poly1 or poly2 has any coefficients greater than or equal to
                coeffMod</exception>
                <exception cref="System::InvalidOperationException">If result is an alias but needs to be resized to fit the
                result</exception>
                */
                void Multiply(BigPoly ^poly1, BigPoly ^poly2, BigUInt ^coeffMod, BigPoly ^result);

                /**
                <summary>Multiplies the poly1 and poly2 polynomials modulo the specified coefficient modulus, and returns the
                result.</summary>

                <remarks>
                Multiplies the poly1 and poly2 polynomials modulo the specified coefficient modulus, and returns the result. The input
                poly1 and poly2 polynomials are not modified. All coefficients in both polynomials must be less-than coeffMod. The
                returned polynomial is sized to have a coefficient count equal to one less than the sum of poly1 and poly2's
                significant coefficient counts, and a coefficient bit count equal to coeffMod's bit count.
                </remarks>
                <param name="poly1">The first polynomial to multiply</param>
                <param name="poly2">The second polynomial to multiply</param>
                <param name="coeffMod">The coefficient modulus</param>
                <exception cref="System::ArgumentNullException">If poly1, poly2, or coeffMod is null</exception>
                <exception cref="System::ArgumentException">If coeffMod is zero</exception>
                <exception cref="System::ArgumentException">If poly1 or poly2 has any coefficients greater than or equal to
                coeffMod</exception>
                */
                BigPoly ^Multiply(BigPoly ^poly1, BigPoly ^poly2, BigUInt ^coeffMod);

                /**
                <summary>Multiplies the poly1 and poly2 polynomials, and stores the result in result.</summary>

                <remarks>
                Multiplies the poly1 and poly2 polynomials, and stores the result in result. The input poly1 and poly2 polynomials are
                not modified. The result polynomial is resized if it has fewer coefficients than one less than the sum of poly1 and
                poly2's significant coefficient counts, or if it's coefficient bit count is smaller than the sum of poly1 and poly2's
                significant coefficient bit count added with the significant bit count of the smaller of poly1 and poly2's significant
                coefficient count (i.e., poly1.significantCoeffBitCount() + poly2.significantCoeffBitCount() +
                getSignificantBitCount(min(poly1.significantCoeffCount(), poly2.significantCoeffCount()))).
                </remarks>
                <param name="poly1">The first polynomial to multiply</param>
                <param name="poly2">The second polynomial to multiply</param>
                <param name="result">The polynomial to store the multiplication result</param>
                <exception cref="System::ArgumentNullException">If poly1, poly2, or result is null</exception>
                <exception cref="System::InvalidOperationException">If result is an alias but needs to be resized to fit the
                result</exception>
                */
                void Multiply(BigPoly ^poly1, BigPoly ^poly2, BigPoly ^result);

                /**
                <summary>Multiplies the poly1 and poly2 polynomials, and returns the result.</summary>

                <remarks>
                Multiplies the poly1 and poly2 polynomials, and returns the result. The input poly1 and poly2 polynomials are not
                modified. The returned polynomial is sized to have a coefficient count equal to one less than the sum of poly1 and
                poly2's significant coefficient counts, and a coefficient bit count equal to the sum of poly1 and poly2's significant
                coefficient bit count added with the significant bit count of the smaller of poly1 and poly2's significant coefficient
                count (i.e., poly1.significantCoeffBitCount() + poly2.significantCoeffBitCount() +
                getSignificantBitCount(min(poly1.significantCoeffCount(), poly2.significantCoeffCount()))).
                </remarks>
                <param name="poly1">The first polynomial to multiply</param>
                <param name="poly2">The second polynomial to multiply</param>
                <exception cref="System::ArgumentNullException">If poly1 or poly2 is null</exception>
                */
                BigPoly ^Multiply(BigPoly ^poly1, BigPoly ^poly2);

                /**
                <summary>Multiplies a polynomial by an integer modulo the specified coefficient modulus, and stores the result in
                result.</summary>

                <remarks>
                Multiplies a polynomial by an integer modulo the specified coefficient modulus, and stores the result in result. The
                inputs poly1 and uint2 are not modified. All coefficients in poly1 and uint2 must be less-than coeffMod. The result
                polynomial is resized if it has fewer coefficients than poly1's coefficient count, or if its coefficient bit count is
                smaller than coeffMod's bit count.
                </remarks>
                <param name="poly1">The first polynomial to multiply</param>
                <param name="uint2">The integer to multiply</param>
                <param name="coeffMod">The coefficient modulus</param>
                <param name="result">The polynomial to store the multiplication result</param>
                <exception cref="System::ArgumentException">If coeffMod is zero</exception>
                <exception cref="System::ArgumentException">If poly1 has any coefficients greater than or equal to coeffMod</exception>
                <exception cref="System::ArgumentException">If uint2 is greater than or equal to coeffMod</exception>
                <exception cref="System::InvalidOperationException">If result is an alias but needs to be resized to fit the
                result</exception>
                */
                void Multiply(BigPoly ^poly1, BigUInt ^uint2, BigUInt ^coeffMod, BigPoly ^result);

                /**
                <summary>Multiplies a polynomial by an integer modulo the specified coefficient modulus, and returns the
                result.</summary>

                <remarks>
                Multiplies a polynomial by an integer modulo the specified coefficient modulus, and returns the result. The inputs
                poly1 and uint2 are not modified. All coefficients in poly1 and uint2 must be less-than coeffMod. The returned
                polynomial is sized to have a coefficient count equal to poly1's coefficient count, and a coefficient bit count equal
                to coeffMod's bit count.
                </remarks>
                <param name="poly1">The polynomial to multiply</param>
                <param name="uint2">The integer to multiply</param>
                <param name="coeffMod">The coefficient modulus</param>
                <exception cref="System::ArgumentException">If coeffMod is zero</exception>
                <exception cref="System::ArgumentException">If poly1 has any coefficients greater than or equal to coeffMod</exception>
                <exception cref="System::ArgumentException">If uint2 is greater than or equal to coeffMod</exception>
                */
                BigPoly ^Multiply(BigPoly ^poly1, BigUInt ^uint2, BigUInt ^coeffMod);

                /**
                <summary>Multiplies a polynomial by an integer, and stores the result in result.</summary>

                <remarks>
                Multiplies a polynomial by an integer, and stores the result in result. The inputs poly1 and uint2 are not modified.
                The result polynomial is resized if it has fewer coefficients than poly1's coefficient count, or if its coefficient
                bit count is smaller than the sum of poly1's significant coefficient bit count and uint2's significant bit count.
                </remarks>
                <param name="poly1">The polynomial to multiply</param>
                <param name="uint2">The integer to multiply</param>
                <param name="result">The polynomial to store the multiplication result</param>
                <exception cref="System::InvalidOperationException">If result is an alias but needs to be resized to fit the
                result</exception>
                */
                void Multiply(BigPoly ^poly1, BigUInt ^uint2, BigPoly ^result);

                /**
                <summary>Multiplies a polynomial by an integer, and returns the result.</summary>

                <remarks>
                Multiplies a polynomial by an integer, and returns the result. The inputs poly1 and uint2 are not modified. The
                returned polynomial is sized to have a coefficient count equal to poly1's coefficient count, and a coefficient bit
                count equal to the sum of poly1's significant coefficient bit count and uint2's significant bit count.
                </remarks>
                <param name="poly1">The polynomial to multiply</param>
                <param name="uint2">The integer to multiply</param>
                */
                BigPoly ^Multiply(BigPoly ^poly1, BigUInt ^uint2);

                /**
                <summary>Divides the numerator and denominator polynomials modulo the specified coefficient modulus, and stores the
                quotient and remainder in provided parameters.</summary>

                <remarks>
                Divides the numerator and denominator polynomials modulo the specified coefficient modulus, and stores the quotient and
                remainder in provided parameters. The input numerator and denominator polynomials are not modified. All coefficients in
                numerator and denominator must be less-than coeffMod. The quotient and remainder polynomials are resized if they have
                fewer coefficients than the greater of numerator and denominator's coefficient counts, or if their coefficient bit
                counts are smaller than coeffMod's bit count.
                </remarks>
                <param name="numerator">The numerator to divide</param>
                <param name="denominator">The denominator to divide</param>
                <param name="coeffMod">The coefficient modulus</param>
                <param name="quotient">The polynomial to store the quotient of the division result</param>
                <param name="remainder">The polynomial to store the remainder of the division result</param>
                <exception cref="System::ArgumentNullException">If numerator, denominator, coeffMod, quotient, or remainder is null
                </exception>
                <exception cref="System::ArgumentException">If denominator is zero</exception>
                <exception cref="System::ArgumentException">If coeffMod is zero</exception>
                <exception cref="System::ArgumentException">If numerator or denominator has any coefficients greater than or equal to
                coeffMod</exception>
                <exception cref="System::ArgumentException">If quotient and remainder point to the same backing array</exception>
                <exception cref="System::InvalidOperationException">If quotient or remainder is an alias but needs to be resized to fit
                the result</exception>
                */
                void Divide(BigPoly ^numerator, BigPoly ^denominator, BigUInt ^coeffMod, BigPoly ^quotient, BigPoly ^remainder);

                /**
                <summary>Divides the numerator and denominator polynomials modulo the specified coefficient modulus, and stores the
                remainder in remainder.</summary>

                <remarks>
                Divides the numerator and denominator polynomials modulo the specified coefficient modulus, and stores the remainder in
                remainder. The input numerator and denominator polynomials are not modified. All coefficients in numerator and
                denominator must be less-than coeffMod. The remainder polynomial is resized if it has fewer coefficients than the
                larger of numerator and denominator's coefficient counts, or if it's coefficient bit count is smaller than coeffMod's
                bit count.
                </remarks>
                <param name="numerator">The numerator to divide</param>
                <param name="denominator">The denominator to divide</param>
                <param name="coeffMod">The coefficient modulus</param>
                <param name="remainder">The polynomial to store the remainder of the division result</param>
                <exception cref="System::ArgumentNullException">If numerator, denominator, coeffMod, or remainder is null</exception>
                <exception cref="System::ArgumentException">If denominator is zero</exception>
                <exception cref="System::ArgumentException">If coeffMod is zero</exception>
                <exception cref="System::ArgumentException">If numerator or denominator has any coefficients greater than or equal to
                coeffMod</exception>
                <exception cref="System::InvalidOperationException">If remainder is an alias but needs to be resized to fit the
                result</exception>
                */
                void Modulo(BigPoly ^numerator, BigPoly ^denominator, BigUInt ^coeffMod, BigPoly ^remainder);

                /**
                <summary>Divides the numerator and denominator polynomials modulo the specified coefficient modulus, and returns the
                remainder.</summary>

                <remarks>
                Divides the numerator and denominator polynomials modulo the specified coefficient modulus, and returns the remainder.
                The input numerator and denominator polynomials are not modified. All coefficients in numerator and denominator must
                be less-than coeffMod. The returned polynomial is sized to have a coefficient count equal to the larger of numerator
                and denominator's coefficient counts, and a coefficient bit count equal to coeffMod's bit count.
                </remarks>
                <param name="numerator">The numerator to divide</param>
                <param name="denominator">The denominator to divide</param>
                <param name="coeffMod">The coefficient modulus</param>
                <exception cref="System::ArgumentNullException">If numerator, denominator, or coeffMod is null</exception>
                <exception cref="System::ArgumentException">If denominator is zero</exception>
                <exception cref="System::ArgumentException">If coeffMod is zero</exception>
                <exception cref="System::ArgumentException">If numerator or denominator has any coefficients greater than or equal to
                coeffMod</exception>
                */
                BigPoly ^Modulo(BigPoly ^numerator, BigPoly ^denominator, BigUInt ^coeffMod);

                /**
                <summary>Attempts to invert a polynomial with the specified coefficient and polynomial moduli, returning whether or not
                the inverse was successful and setting the result parameter to the inverse.</summary>

                <remarks>
                Attempts to invert a polynomial with the specified coefficient and polynomial moduli, returning whether or not the
                inverse was successful and setting the result parameter to the inverse. The poly polynomials must have fewer
                significant coefficients than polyMod and have coefficients less-than coeffMod. The result polynomial is resized if it
                has fewer coefficients than polyMod's significant coefficient count or if it's coefficient bit count is smaller than
                coeffMod's bit count.
                </remarks>
                <param name="poly">The polynomial to invert</param>
                <param name="polyMod">The polynomial modulus</param>
                <param name="coeffMod">The coefficient modulus</param>
                <param name="result">The polynomial to store the inverse result</param>
                <exception cref="System::ArgumentNullException">If poly, polyMod, coeffMod, or result is null</exception>
                <exception cref="System::ArgumentException">If polyMod is zero</exception>
                <exception cref="System::ArgumentException">If coeffMod is zero</exception>
                <exception cref="System::ArgumentException">If poly has greater-than or equal significant coefficient count to polyMod
                or has any coefficients greater than or equal to coeffMod</exception>
                <exception cref="System::ArgumentException">If polyMod has any coefficients greater than or equal to
                coeffMod</exception>
                <exception cref="System::InvalidOperationException">If result is an alias but needs to be resized to fit the
                result</exception>
                */
                bool TryInvert(BigPoly ^poly, BigPoly ^polyMod, BigUInt ^coeffMod, BigPoly ^result);

                /**
                <summary>Destroys the BigPolyArithmetic instance.</summary>
                */
                ~BigPolyArithmetic();

                /**
                <summary>Destroys the BigPolyArithmetic instance.</summary>
                */
                !BigPolyArithmetic();

                /**
                <summary>Returns a reference to the underlying C++ BigPolyArith instance.</summary>
                */
                seal::BigPolyArith &GetBigPolyArith();

            private:
                seal::BigPolyArith *arith_;
            };
        }
    }
}
