#pragma once

#include "evaluator.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class EncryptionParameters;
            ref class BigPoly;
            ref class EvaluationKeys;

            /**
            <summary>Provides arithmetic functions for operating on encrypted polynomials (represented by BigPoly's).</summary>

            <remarks>
            <para>
            Provides arithmetic functions for operating on encrypted polynomials (represented by <see cref="BigPoly"/>'s). The add,
            subtract, and multiply function variants allow both operands to be encrypted or for one operand to be plain-text with
            the "Plain" variants.
            </para>
            <para>
            As multiplication is typically the most expensive arithmetic operation, three variants of the multiply function are
            provided that may allow for more performance depending on the application. The Multiply() function performs full-blown
            multiplication, which internally consists of a multiplication and relinearization step. The MultiplyNoRelin() and
            Relinearize() functions break-up multiplication into its two steps to allow the relinearlization step to be optional.
            Refer to documentation on the encryption scheme to determine which multiply variants may be beneficial for different
            applications.
            </para>
            <para>
            The Evaluator class is not thread-safe and a separate Evaluator instance is needed for each potentially concurrent
            usage.
            </para>
            </remarks>
            */
            public ref class Evaluator
            {
            public:
                /**
                <summary>Creates an Evaluator instances initialized with the specified encryption parameters and evaluation
                keys.</summary>
                <param name="parms">The encryption parameters</param>
                <param name="evaluationKeys">The evaluation keys</param>
                <exception cref="System::ArgumentException">if encryption parameters or evaluation keys are not valid</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption
                parameters.</seealso>
                */
                Evaluator(EncryptionParameters ^parms, EvaluationKeys ^evaluationKeys);

                /**
                <summary>Returns the evaluation keys used by the Evaluator.</summary>
                */
                property EvaluationKeys ^EvaluationKeys {
                    Microsoft::Research::SEAL::EvaluationKeys ^get();
                }

                /**
                <summary>Negates an encrypted polynomial and stores the result in the destination parameter.</summary>

                <remarks>
                Negates an encrypted polynomial and stores the result in the destination parameter. The destination parameter is
                resized if and only if its coefficient count or coefficient bit count does not match the encryption parameters.
                </remarks>
                <param name="encrypted">The encrypted polynomial to negate</param>
                <param name="destination">The polynomial to overwrite with the negated result</param>
                <exception cref="System::ArgumentException">if the encrypted polynomial is not a valid for the encryption
                parameters</exception>
                <exception cref="System::InvalidOperationException">If destination is an alias but needs to be resized</exception>
                */
                void Negate(BigPoly ^encrypted, BigPoly ^destination);

                /**
                <summary>Negates an encrypted polynomial and returns the result.</summary>
                <param name="encrypted">The encrypted polynomial to negate</param>
                <exception cref="System::ArgumentException">if the encrypted polynomial is not a valid encrypted polynomial for the
                encryption parameters</exception>
                */
                BigPoly ^Negate(BigPoly ^encrypted);

                /**
                <summary>Adds two encrypted polynomials and stores the result in the destination parameter.</summary>
                <remarks>
                Adds two encrypted polynomials and stores the result in the destination parameter. The destination parameter is resized
                if and only if its coefficient count or coefficient bit count does not match the encryption parameters.
                </remarks>
                <param name="encrypted1">The first encrypted polynomial to add</param>
                <param name="encrypted2">The second encrypted polynomial to add</param>
                <param name="destination">The polynomial to overwrite with the addition result</param>
                <exception cref="System::ArgumentException">if the encrypted polynomials are not valid for the encryption
                parameters</exception>
                <exception cref="System::InvalidOperationException">If destination is an alias but needs to be resized</exception>
                */
                void Add(BigPoly ^encrypted1, BigPoly ^encrypted2, BigPoly ^destination);

                /**
                <summary>Adds two encrypted polynomials and returns the result.</summary>
                <param name="encrypted1">The first encrypted polynomial to add</param>
                <param name="encrypted2">The second encrypted polynomial to add</param>
                <exception cref="System::ArgumentException">if the encrypted polynomials are not valid for the encryption
                parameters</exception>
                */
                BigPoly ^Add(BigPoly ^encrypted1, BigPoly ^encrypted2);

                /**
                <summary>Adds together an number of ciphertexts stored as elements of a list and stores the result
                in the destination parameter.</summary>
                <remarks>
                Adds together an number of ciphertexts stored as elements of a list and stores the result in the
                destination parameter. The destination parameter is resized if and only if its coefficient count or coefficient bit
                count does not match the encryption parameters.
                </remarks>
                <param name="encrypteds">The encrypted polynomials to add</param>
                <param name="destination">The polynomial to overwrite with the addition result</param>
                <exception cref="System::ArgumentNullException">if encrypteds or any of its elements is null</exception>
                <exception cref="System::ArgumentException">if encrypteds is empty</exception>
                <exception cref="System::ArgumentException">if the encrypted polynomials are not valid for the encryption parameters</exception>
                <exception cref="System::InvalidOperationException">If destination is an alias but needs to be resized</exception>
                */
                void AddMany(System::Collections::Generic::List<BigPoly^> ^encrypteds, BigPoly ^destination);

                /**
                <summary>Adds together an number of ciphertexts stored as elements of a list and returns the result.</summary>
                <param name="encrypteds">The encrypted polynomials to add</param>
                <exception cref="System::ArgumentNullException">if encrypteds or any of its elements is null</exception>
                <exception cref="System::ArgumentException">if encrypteds is empty</exception>
                <exception cref="System::ArgumentException">if the encrypted polynomials are not valid for the encryption parameters</exception>
                */
                BigPoly ^AddMany(System::Collections::Generic::List<BigPoly^> ^encrypteds);

                /**
                <summary>Subtracts two encrypted polynomials and stores the result in the destination parameter.</summary>
                <remarks>
                Subtracts two encrypted polynomials and stores the result in the destination parameter. The destination parameter is
                resized if and only if its coefficient count or coefficient bit count does not match the encryption parameters.
                </remarks>
                <param name="encrypted1">The first encrypted polynomial to subtract</param>
                <param name="encrypted2">The second encrypted polynomial to subtract</param>
                <param name="destination">The polynomial to overwrite with the subtraction result</param>
                <exception cref="System::ArgumentException">if the encrypted polynomials are not valid for the encryption
                parameters</exception>
                <exception cref="System::InvalidOperationException">If destination is an alias but needs to be resized</exception>
                */
                void Sub(BigPoly ^encrypted1, BigPoly ^encrypted2, BigPoly ^destination);

                /**
                <summary>Subtracts two encrypted polynomials and returns the result.</summary>
                <param name="encrypted1">The first encrypted polynomial to subtract</param>
                <param name="encrypted2">The second encrypted polynomial to subtract</param>
                <exception cref="System::ArgumentException">if the encrypted polynomials are not valid for the encryption
                parameters</exception>
                */
                BigPoly ^Sub(BigPoly ^encrypted1, BigPoly ^encrypted2);

                /**
                <summary>Multiplies two encrypted polynomials and stores the result in the destination parameter.</summary>

                <remarks>
                Multiplies two encrypted polynomials and stores the result in the destination parameter. The destination parameter is
                resized if and only if its coefficient count or coefficient bit count does not match the encryption parameters.
                </remarks>
                <param name="encrypted1">The first encrypted polynomial to multiply</param>
                <param name="encrypted2">The second encrypted polynomial to multiply</param>
                <param name="destination">The polynomial to overwrite with the multiplication result</param>
                <exception cref="System::ArgumentException">if the encrypted polynomials are not valid for the encryption
                parameters</exception>
                <exception cref="System::InvalidOperationException">If destination is an alias but needs to be resized</exception>
                */
                void Multiply(BigPoly ^encrypted1, BigPoly ^encrypted2, BigPoly ^destination);

                /**
                <summary>Multiplies two encrypted polynomials and returns the result.</summary>
                <param name="encrypted1">The first encrypted polynomial to multiply</param>
                <param name="encrypted2">The second encrypted polynomial to multiply</param>
                <exception cref="System::ArgumentException">if the encrypted polynomials are not valid for the encryption
                parameters</exception>
                */
                BigPoly ^Multiply(BigPoly ^encrypted1, BigPoly ^encrypted2);

                /**
                <summary>Multiplies two encrypted polynomials without performing relinearization, and stores the result in the
                destination parameter.</summary>

                <remarks>
                Multiplies two encrypted polynomials without performing relinearization, and stores the result in the destination
                parameter. The destination parameter is resized if and only if its coefficient count or coefficient bit count does not
                match the encryption parameters.
                </remarks>
                <param name="encrypted1">The first encrypted polynomial to multiply</param>
                <param name="encrypted2">The second encrypted polynomial to multiply</param>
                <param name="destination">The polynomial to overwrite with the multiplication result</param>
                <exception cref="System::ArgumentException">if the encrypted polynomials are not valid for the encryption
                parameters</exception>
                <exception cref="System::InvalidOperationException">If destination is an alias but needs to be resized</exception>
                */
                void MultiplyNoRelin(BigPoly ^encrypted1, BigPoly ^encrypted2, BigPoly ^destination);

                /**
                <summary>Multiplies two encrypted polynomials without performing relinearization, and returns the result.</summary>
                <param name="encrypted1">The first encrypted polynomial to multiply</param>
                <param name="encrypted2">The second encrypted polynomial to multiply</param>
                <exception cref="System::ArgumentException">if the encrypted polynomials are not valid for the encryption
                parameters</exception>
                */
                BigPoly ^MultiplyNoRelin(BigPoly ^encrypted1, BigPoly ^encrypted2);

                /**
                <summary>Relinearizes an encrypted polynomial and stores the result in the destination parameter.</summary>

                <remarks>
                Relinearizes an encrypted polynomial and stores the result in the destination parameter. The destination parameter is
                resized if and only if its coefficient count or coefficient bit count does not match the encryption parameters.
                </remarks>
                <param name="encrypted">The encrypted polynomial to relinearize</param>
                <param name="destination">The polynomial to overwrite with the relinearized result</param>
                <exception cref="System::ArgumentException">if the encrypted polynomial is not valid for the encryption
                parameters</exception>
                <exception cref="System::InvalidOperationException">If destination is an alias but needs to be resized</exception>
                */
                void Relinearize(BigPoly ^encrypted, BigPoly ^destination);

                /**
                <summary>Relinearizes an encrypted polynomial and returns the result.</summary>
                <param name="encrypted">The encrypted polynomial to relinearize</param>
                <exception cref="System::ArgumentException">if the encrypted polynomial is not valid for the encryption
                parameters</exception>
                */
                BigPoly ^Relinearize(BigPoly ^encrypted);

                /**
                <summary>Adds an encrypted polynomial with a plain polynomial, and stores the result in the destination
                parameter.</summary>

                <remarks>
                Adds an encrypted polynomial with a plain polynomial, and stores the result in the destination parameter. The
                destination parameter is resized if and only if its coefficient count or coefficient bit count does not match the
                encryption parameters. The plain polynomial must have a significant coefficient count smaller than the coefficient
                count specified by the encryption parameters, and with coefficient values less-than the plain modulus
                (<see cref="EncryptionParameters::PlainModulus"/>).
                </remarks>
                <param name="encrypted1">The first encrypted polynomial to add</param>
                <param name="plain2">The second plain polynomial to add</param>
                <param name="destination">The polynomial to overwrite with the addition result</param>
                <exception cref="System::ArgumentException">if the encrypted1 polynomials are not valid for the encryption
                parameters</exception>
                <exception cref="System::ArgumentException">if the plain2 polynomial's significant coefficient count or coefficient
                values are too large to represent with the encryption parameters</exception>
                <exception cref="System::InvalidOperationException">If destination is an alias but needs to be resized</exception>
                */
                void AddPlain(BigPoly ^encrypted1, BigPoly ^plain2, BigPoly ^destination);

                /**
                <summary>Adds an encrypted polynomial with a plain polynomial, and returns the result.</summary>

                <remarks>
                Adds an encrypted polynomial with a plain polynomial, and returns the result. The plain polynomial must have a
                significant coefficient count smaller than the coefficient count specified by the encryption parameters, and with
                coefficient values less-than the plain modulus (<see cref="EncryptionParameters::PlainModulus"/>).
                </remarks>
                <param name="encrypted1">The first encrypted polynomial to add</param>
                <param name="plain2">The second plain polynomial to add</param>
                <exception cref="System::ArgumentException">if the encrypted1 polynomials are not valid for the encryption
                parameters</exception>
                <exception cref="System::ArgumentException">if the plain2 polynomial's significant coefficient count or coefficient
                values are too large to represent with the encryption parameters</exception>
                */
                BigPoly ^AddPlain(BigPoly ^encrypted1, BigPoly ^plain2);

                /**
                <summary>Subtracts an encrypted polynomial with a plain polynomial, and stores the result in the destination
                parameter.</summary>

                <remarks>
                Subtracts an encrypted polynomial with a plain polynomial, and stores the result in the destination parameter. The
                destination parameter is resized if and only if its coefficient count or coefficient bit count does not match the
                encryption parameters. The plain polynomial must have a significant coefficient count smaller than the coefficient
                count specified by the encryption parameters, and with coefficient values less-than the plain modulus
                (<see cref="EncryptionParameters::PlainModulus"/>).
                </remarks>
                <param name="encrypted1">The first encrypted polynomial to subtract</param>
                <param name="plain2">The second plain polynomial to subtract</param>
                <param name="destination">The polynomial to overwrite with the subtraction result</param>
                <exception cref="System::ArgumentException">if the encrypted1 polynomials are not valid for the encryption
                parameters</exception>
                <exception cref="System::ArgumentException">if the plain2 polynomial's significant coefficient count or coefficient
                values are too large to represent with the encryption parameters</exception>
                <exception cref="System::InvalidOperationException">If destination is an alias but needs to be resized</exception>
                */
                void SubPlain(BigPoly ^encrypted1, BigPoly ^plain2, BigPoly ^destination);

                /**
                <summary>Subtracts an encrypted polynomial with a plain polynomial, and returns the result.</summary>

                <remarks>
                Subtracts an encrypted polynomial with a plain polynomial, and returns the result. The plain polynomial must have a
                significant coefficient count smaller than the coefficient count specified by the encryption parameters, and with
                coefficient values less-than the plain modulus (<see cref="EncryptionParameters::PlainModulus"/>).
                </remarks>
                <param name="encrypted1">The first encrypted polynomial to subtract</param>
                <param name="plain2">The second plain polynomial to subtract</param>
                <exception cref="System::ArgumentException">if the encrypted1 polynomials are not valid for the encryption
                parameters</exception>
                <exception cref="System::ArgumentException">if the plain2 polynomial's significant coefficient count or coefficient
                values are too large to represent with the encryption parameters</exception>
                */
                BigPoly ^SubPlain(BigPoly ^encrypted1, BigPoly ^plain2);

                /**
                <summary>Multiplies an encrypted polynomial with a plain polynomial, and stores the result in the destination
                parameter.</summary>

                <remarks>
                Multiplies an encrypted polynomial with a plain polynomial, and stores the result in the destination parameter. The
                destination parameter is resized if and only if its coefficient count or coefficient bit count does not match the
                encryption parameters. The plain polynomial must have a significant coefficient count smaller than the coefficient
                count specified by the encryption parameters, and with coefficient values less-than the plain modulus
                (<see cref="EncryptionParameters::PlainModulus"/>).
                </remarks>
                <param name="encrypted1">The first encrypted polynomial to multiply</param>
                <param name="plain2">The second plain polynomial to multiply</param>
                <param name="destination">The polynomial to overwrite with the multiplication result</param>
                <exception cref="System::ArgumentException">if the encrypted1 polynomials are not valid for the encryption
                parameters</exception>
                <exception cref="System::ArgumentException">if the plain2 polynomial's significant coefficient count or coefficient
                values are too large to represent with the encryption parameters</exception>
                <exception cref="System::InvalidOperationException">If destination is an alias but needs to be resized</exception>
                */
                void MultiplyPlain(BigPoly ^encrypted1, BigPoly ^plain2, BigPoly ^destination);

                /**
                <summary>Multiplies an encrypted polynomial with a plain polynomial, and returns the result.</summary>

                <remarks>
                Multiplies an encrypted polynomial with a plain polynomial, and returns the result. The plain polynomial must have a
                significant coefficient count smaller than the coefficient count specified by the encryption parameters, and with
                coefficient values less-than the plain modulus (<see cref="EncryptionParameters::PlainModulus"/>).
                </remarks>
                <param name="encrypted1">The first encrypted polynomial to multiply</param>
                <param name="plain2">The second plain polynomial to multiply</param>
                <exception cref="System::ArgumentException">if the encrypted1 polynomials are not valid for the encryption
                parameters</exception>
                <exception cref="System::ArgumentException">if the plain2 polynomial's significant coefficient count or coefficient
                values are too large to represent with the encryption parameters</exception>
                <exception cref="System::InvalidOperationException">If destination is an alias but needs to be resized</exception>
                */
                BigPoly ^MultiplyPlain(BigPoly ^encrypted1, BigPoly ^plain2);

                /**
                <summary>Multiplies a vector of encrypted polynomials together and stores the result in the destination
                parameter.</summary>
                <remarks>
                Multiplies a vector of encrypted polynomials together and stores the result in the destination parameter. The
                destination parameter is resized if and only if its coefficient count or coefficient bit count does not match the
                encryption parameters.
                </remarks>
                <param name="encrypteds">The vector of encrypted polynomials to multiply</param>
                <param name="destination">The polynomial to overwrite with the multiplication result</param>
                <exception cref="System::ArgumentException">if the encrypteds vector is empty</exception>
                <exception cref="System::ArgumentException">if the encrypted polynomials are not valid encrypted polynomials for the
                encryption parameters</exception>
                <exception cref="System::InvalidOperationException">If destination is an alias but needs to be resized</exception>
                */
                void MultiplyMany(System::Collections::Generic::List<BigPoly^> ^encrypteds, BigPoly ^destination);

                /**
                <summary>Multiplies a vector of encrypted polynomials together and returns the result.</summary>
                <param name="encrypteds">The vector of encrypted polynomials to multiply</param>
                <exception cref="System::ArgumentException">if the encrypteds vector is empty</exception>
                <exception cref="System::ArgumentException">if the encrypted polynomials are not valid encrypted polynomials for the
                encryption parameters</exception>
                */
                BigPoly ^MultiplyMany(System::Collections::Generic::List<BigPoly^> ^encrypteds);

                /**
                <summary>Multiplies a vector of encrypted polynomials together without performing relinearization and stores the result in the destination
                parameter.</summary>
                <remarks>
                Multiplies a vector of encrypted polynomials together and stores the result in the destination parameter. The
                destination parameter is resized if and only if its coefficient count or coefficient bit count does not match the
                encryption parameters.
                </remarks>
                <param name="encrypteds">The vector of encrypted polynomials to multiply</param>
                <param name="destination">The polynomial to overwrite with the multiplication result</param>
                <exception cref="System::ArgumentException">if the encrypteds vector is empty</exception>
                <exception cref="System::ArgumentException">if the encrypted polynomials are not valid encrypted polynomials for the
                encryption parameters</exception>
                <exception cref="System::InvalidOperationException">If destination is an alias but needs to be resized</exception>
                */
                void MultiplyNoRelinMany(System::Collections::Generic::List<BigPoly^> ^encrypteds, BigPoly ^destination);

                /**
                <summary>Multiplies a vector of encrypted polynomials together without performing relinearization and returns the result.</summary>
                <param name="encrypteds">The vector of encrypted polynomials to multiply</param>
                <exception cref="System::ArgumentException">if the encrypteds vector is empty</exception>
                <exception cref="System::ArgumentException">if the encrypted polynomials are not valid encrypted polynomials for the
                encryption parameters</exception>
                */
                BigPoly ^MultiplyNoRelinMany(System::Collections::Generic::List<BigPoly^> ^encrypteds);

                /**
                <summary>Raises an encrypted polynomial to the specified power and stores the result in the destination
                parameter.</summary>
                <remarks>
                Raises an encrypted polynomial to the specified power and stores the result in the destination parameter. The
                destination parameter is resized if and only if its coefficient count or coefficient bit count does not match the
                encryption parameters.
                </remarks>
                <param name="encrypted">The encrypted polynomial to raise to a power</param>
                <param name="exponent">The power to raise the encrypted polynomial to</param>
                <param name="destination">The polynomial to overwrite with the exponentiation result</param>
                <exception cref="System::ArgumentException">if the encrypted polynomial is not valid for the encryption
                parameters</exception>
                <exception cref="System::ArgumentException">if the exponent is negative</exception>
                <exception cref="System::InvalidOperationException">If destination is an alias but needs to be resized</exception>
                */
                void Exponentiate(BigPoly ^encrypted, int exponent, BigPoly ^destination);

                /**
                <summary>Raises an encrypted polynomial to the specified power and returns the result.</summary>
                <param name="encrypted">The encrypted polynomial to raise to a power</param>
                <param name="exponent">The power to raise the encrypted polynomial to</param>
                <exception cref="System::ArgumentException">if the encrypted polynomial is not valid for the encryption
                parameters</exception>
                <exception cref="System::ArgumentException">if the exponent is negative</exception>
                */
                BigPoly ^Exponentiate(BigPoly ^encrypted, int exponent);

                /**
                <summary>Raises an encrypted polynomial to the specified power without performing relinearization and stores the result in the destination
                parameter.</summary>
                <remarks>
                Raises an encrypted polynomial to the specified power and stores the result in the destination parameter. The
                destination parameter is resized if and only if its coefficient count or coefficient bit count does not match the
                encryption parameters.
                </remarks>
                <param name="encrypted">The encrypted polynomial to raise to a power</param>
                <param name="exponent">The power to raise the encrypted polynomial to</param>
                <param name="destination">The polynomial to overwrite with the exponentiation result</param>
                <exception cref="System::ArgumentException">if the encrypted polynomial is not valid for the encryption
                parameters</exception>
                <exception cref="System::ArgumentException">if the exponent is negative</exception>
                <exception cref="System::InvalidOperationException">If destination is an alias but needs to be resized</exception>
                */
                void ExponentiateNoRelin(BigPoly ^encrypted, int exponent, BigPoly ^destination);

                /**
                <summary>Raises an encrypted polynomial to the specified power without performing relinearization and returns the result.</summary>
                <param name="encrypted">The encrypted polynomial to raise to a power</param>
                <param name="exponent">The power to raise the encrypted polynomial to</param>
                <exception cref="System::ArgumentException">if the encrypted polynomial is not valid for the encryption
                parameters</exception>
                <exception cref="System::ArgumentException">if the exponent is negative</exception>
                */
                BigPoly ^ExponentiateNoRelin(BigPoly ^encrypted, int exponent);

                /**
                <summary>Destroys the Evaluator.</summary>
                */
                ~Evaluator();

                /**
                <summary>Destroys the Evaluator.</summary>
                */
                !Evaluator();

                /**
                <summary>Returns a reference to the underlying C++ Evaluator.</summary>
                */
                seal::Evaluator &GetEvaluator();

            private:
                seal::Evaluator *evaluator_;
            };
        }
    }
}
