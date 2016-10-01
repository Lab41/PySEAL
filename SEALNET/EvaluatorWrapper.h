#pragma once

#include "evaluator.h"
#include "EvaluationKeysWrapper.h"
#include "EncryptionParamsWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class EncryptionParameters;

            ref class BigPoly;

            /**
            <summary>Provides arithmetic functions for operating on ciphertexts.</summary>

            <remarks>
            <para>
            Provides arithmetic functions for operating on ciphertexts. The Add, Subtract, and Multiply function variants allow
            both operands to be encrypted. The "Plain" variants allow one of the inputs to be encrypted and the other unencrypted.
            </para>
            <para>
            Every valid ciphertext consists of at least two polynomials. Homomorphic multiplication increases
            the size of the ciphertext in such a way that if the input ciphertexts have size M and N, then the
            output ciphertext will have size M+N-1. The multiplication operation will require M*N polynomial
            multiplications to be performed. To read the current size of a ciphertext the user can use 
            <see cref="BigPolyArray::Size"/>.
            </para>
            <para>
            A relinearization operation can be used to reduce the size of a ciphertext to any smaller size
            (but at least 2), potentially improving the performance of a subsequent multiplication using it.
            However, relinearization grows the inherent noise in a ciphertext by an additive factor proportional
            to 2^DBC, and relinearizing from size K to L will require 2*(K-L)*[floor(log_2(coeff_modulus)/DBC)+1]
            polynomial multiplications. Here DBC denotes the decomposition bit count set in the encryption parameters.
            Note that the larger the decomposition bit count is, the faster relinearization will be, but also the
            inherent noise growth will be larger. In typical cases, however, the user might not want to relinearize
            at all as even the computational cost of a relinearization might be larger than what might be gained
            from a subsequent multiplication with a ciphertext of smaller size.
            </para>
            <para>
            Relinearization requires the key generator to generate evaluation keys. More specifically, to relinearize
            a ciphertext of size K down to any size smaller than K (but at least 2), at least K-2 evaluation keys will
            be needed. These have to be given as an input parameter to the constructor of Evaluator.
            </para>
            <para>
            Technically speaking, the inherent noise of a ciphertext is a polynomial, but the condition for decryption working
            depends on the size of the largest absolute value of its coefficients. This is what we will call the "noise", the
            "inherent noise", or the "error", in this documentation. Typically multiplication will be the most heavy operation
            in terms of noise growth. The reader is referred to the description of the encryption scheme for more details.
            </para>
            </remarks>
            */
            public ref class Evaluator
            {
            public:
                /**
                <summary>Creates an Evaluator instance initialized with the specified encryption parameters and evaluation
                keys.</summary>
                <remarks>Creates an Evaluator instance initialized with the specified encryption parameters and evaluation
                keys. If no evaluation keys will be needed, one can simply pass a newly created empty instance of EvaluationKeys 
                to the function.
                </remarks>
                <param name="parms">The encryption parameters</param>
                <param name="evaluationKeys">The evaluation keys</param>
                <exception cref="System::ArgumentException">if encryption parameters or evaluation keys are not valid</exception>
                <exception cref="System::ArgumentNullException">if parms or evaluationKeys is null</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption
                parameters.</seealso>
                */
                Evaluator(EncryptionParameters ^parms, EvaluationKeys ^evaluationKeys);

                /**
                <summary>Creates an Evaluator instance initialized with the specified encryption parameters.</summary>
                <param name="parms">The encryption parameters</param>
                <exception cref="System::ArgumentException">if encryption parameters are not valid</exception>
                <exception cref="System::ArgumentNullException">if parms is null</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption
                parameters.</seealso>
                */
                Evaluator(EncryptionParameters ^parms);

                /**
                <summary>Returns the evaluation keys used by the Evaluator.</summary>
                */
                property EvaluationKeys ^EvaluationKeys {
                    Microsoft::Research::SEAL::EvaluationKeys ^get();
                }

                /**
                <summary>Negates a ciphertext and stores the result in the destination parameter.</summary>
                <param name="encrypted">The ciphertext to negate</param>
                <param name="destination">The ciphertext to overwrite with the negated result</param>
                <exception cref="System::ArgumentException">if the ciphertext is not valid for the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted or destination is null</exception>
                */
                void Negate(BigPolyArray ^encrypted, BigPolyArray ^destination);

                /**
                <summary>Negates a ciphertext and returns the result.</summary>
                <param name="encrypted">The ciphertext to negate</param>
                <exception cref="System::ArgumentException">if the ciphertext is not valid for the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted is null</exception>
                */
                BigPolyArray ^Negate(BigPolyArray ^encrypted);

                /**
                <summary>Adds two ciphertexts and stores the result in the destination parameter.</summary>
                <param name="encrypted1">The first ciphertext to add</param>
                <param name="encrypted2">The second ciphertexct to add</param>
                <param name="destination">The ciphertext to overwrite with the addition result</param>
                <exception cref="System::ArgumentException">if the ciphertexts are not valid for the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted1, encrypted2, or destination is null</exception>
                */
                void Add(BigPolyArray ^encrypted1, BigPolyArray ^encrypted2, BigPolyArray ^destination);

                /**
                <summary>Adds two ciphertexts and returns the result.</summary>
                <param name="encrypted1">The first ciphertext to add</param>
                <param name="encrypted2">The second ciphertexct to add</param>
                <exception cref="System::ArgumentException">if the ciphertexts are not valid for the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted1 or encrypted2 is null</exception>
                */
                BigPolyArray ^Add(BigPolyArray ^encrypted1, BigPolyArray ^encrypted2);

                /**
                <summary>Adds together an number of ciphertexts stored as elements of a list and stores the result
                in the destination parameter.</summary>
                <param name="encrypteds">The ciphertexts to add</param>
                <param name="destination">The ciphertext to overwrite with the addition result</param>
                <exception cref="System::ArgumentNullException">if encrypteds or any of its elements is null</exception>
                <exception cref="System::ArgumentException">if encrypteds is empty</exception>
                <exception cref="System::ArgumentException">if the ciphertexts are not valid for the encryption parameters</exception>
                */
                void AddMany(System::Collections::Generic::List<BigPolyArray^> ^encrypteds, BigPolyArray ^destination);

                /**
                <summary>Adds together an number of ciphertexts stored as elements of a list and returns the result.</summary>
                <param name="encrypteds">The ciphertexts to add</param>
                <exception cref="System::ArgumentNullException">if encrypteds or any of its elements is null</exception>
                <exception cref="System::ArgumentException">if encrypteds is empty</exception>
                <exception cref="System::ArgumentException">if the ciphertexts are not valid for the encryption parameters</exception>
                */
                BigPolyArray ^AddMany(System::Collections::Generic::List<BigPolyArray^> ^encrypteds);

                /**
                <summary>Subtracts two ciphertexts and stores the result in the destination parameter.</summary>
                <param name="encrypted1">The ciphertext to subtract from</param>
                <param name="encrypted2">The ciphertext to subtract</param>
                <param name="destination">The ciphertext to overwrite with the subtraction result</param>
                <exception cref="System::ArgumentException">if the ciphertexts are not valid for the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted1, encrypted2, or destination is null</exception>
                */
                void Sub(BigPolyArray ^encrypted1, BigPolyArray ^encrypted2, BigPolyArray ^destination);

                /**
                <summary>Subtracts two ciphertexts and returns the result.</summary>
                <param name="encrypted1">The ciphertext to subtract from</param>
                <param name="encrypted2">The ciphertext to subtract</param>
                <exception cref="System::ArgumentException">if the ciphertexts are not valid for the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted1 or encrypted2 is null</exception>
                */
                BigPolyArray ^Sub(BigPolyArray ^encrypted1, BigPolyArray ^encrypted2);

                /**
                <summary>Multiplies two ciphertexts and stores the result in the destination parameter.</summary>
                <param name="encrypted1">The first ciphertext to multiply</param>
                <param name="encrypted2">The second ciphertext to multiply</param>
                <param name="destination">The ciphertext to overwrite with the multiplication result</param>
                <exception cref="System::ArgumentException">if the ciphertexts are not valid for the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted1, encrypted2, or destination is null</exception>
                */
                void Multiply(BigPolyArray ^encrypted1, BigPolyArray ^encrypted2, BigPolyArray ^destination);

                /**
                <summary>Multiplies two ciphertexts and returns the result.</summary>
                <param name="encrypted1">The first ciphertext to multiply</param>
                <param name="encrypted2">The second ciphertext to multiply</param>
                <exception cref="System::ArgumentException">if the ciphertexts are not valid for the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted1 or encrypted2 is null</exception>
                */
                BigPolyArray ^Multiply(BigPolyArray ^encrypted1, BigPolyArray ^encrypted2);

                /**
                <summary>Squares a ciphertext and stores the result in the destination parameter.</summary>
                <param name="encrypted">The ciphertext to square</param>
                <param name="destination">The ciphertext to overwrite with the result</param>
                <exception cref="System::ArgumentException">if the ciphertexts are not valid for the encryption parameters</exception>
                */
                void Square(BigPolyArray ^encrypted, BigPolyArray ^destination);

                /**
                <summary>Squares a ciphertext and returns the result.</summary>
                <param name="encrypted">The ciphertext to square</param>
                <exception cref="System::ArgumentException">if the ciphertexts are not valid for the encryption parameters</exception>
                */
                BigPolyArray ^Square(BigPolyArray ^encrypted);

                /**
                <summary>Relinearizes a ciphertext and stores the result in the destination parameter.</summary>
                <param name="encrypted">The ciphertext to relinearize</param>
                <param name="destination">The ciphertext to overwrite with the relinearized result</param>
                <exception cref="System::ArgumentException">if the ciphertext is not valid for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if not enough evaluation keys have been generated</exception>
                <exception cref="System::ArgumentNullException">if encrypted or destination is null</exception>
                */
                void Relinearize(BigPolyArray ^encrypted, BigPolyArray ^destination);

                /**
                <summary>Relinearizes a ciphertext and stores the result in the destination parameter.</summary>
                <param name="encrypted">The ciphertext to relinearize</param>
                <param name="destination">The ciphertext to overwrite with the relinearized result</param>
                <param name="destinationSize">The size of the output ciphertext</param>
                <exception cref="System::ArgumentException">if the ciphertext is not valid for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if destinationSize is less than 2 or too large</exception>
                <exception cref="System::ArgumentException">if not enough evaluation keys have been generated</exception>
                <exception cref="System::ArgumentNullException">if encrypted or destination is null</exception>
                */
                void Relinearize(BigPolyArray ^encrypted, BigPolyArray ^destination, int destinationSize);

                /**
                <summary>Relinearizes a ciphertext and returns the result.</summary>
                <param name="encrypted">The ciphertext to relinearize</param>
                <exception cref="System::ArgumentException">if the ciphertext is not valid for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if not enough evaluation keys have been generated</exception>
                <exception cref="System::ArgumentNullException">if encrypted is null</exception>
                */
                BigPolyArray ^Relinearize(BigPolyArray ^encrypted);

                /**
                <summary>Relinearizes a ciphertext and returns the result.</summary>
                <param name="encrypted">The ciphertext to relinearize</param>
                <exception cref="System::ArgumentException">if the ciphertext is not valid for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if destinationSize is less than 2 or too large</exception>
                <exception cref="System::ArgumentException">if not enough evaluation keys have been generated</exception>
                <exception cref="System::ArgumentNullException">if encrypted is null</exception>
                */
                BigPolyArray ^Relinearize(BigPolyArray ^encrypted, int destinationSize);

                /**
                <summary>Adds a ciphertext with a plaintext, and stores the result in the destination parameter.</summary>

                <remarks>
                Adds a ciphertext with a plaintext, and stores the result in the destination parameter. The plaintext must 
                have a significant coefficient count smaller than the coefficient count specified by the encryption parameters, 
                and with coefficient values less-than the plain modulus (<see cref="EncryptionParameters::PlainModulus"/>).
                </remarks>
                <param name="encrypted">The ciphertext to add</param>
                <param name="plain">The plaintext to add</param>
                <param name="destination">The ciphertext to overwrite with the addition result</param>
                <exception cref="System::ArgumentException">if the ciphertexts are not valid for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if the plain polynomial's significant coefficient count or coefficient
                values are too large to represent with the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted, plain, or destination is null</exception>
                */
                void AddPlain(BigPolyArray ^encrypted, BigPoly ^plain, BigPolyArray ^destination);

                /**
                <summary>Adds a ciphertext with a plaintext, and returns the result.</summary>

                <remarks>
                Adds a ciphertext with a plaintext, and returns the result. The plaintext must have a
                significant coefficient count smaller than the coefficient count specified by the encryption parameters, and with
                coefficient values less-than the plain modulus (<see cref="EncryptionParameters::PlainModulus"/>).
                </remarks>
                <param name="encrypted">The ciphertext to add</param>
                <param name="plain">The plaintext to add</param>
                <exception cref="System::ArgumentException">if the ciphertexts are not valid for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if the plain polynomial's significant coefficient count or coefficient
                values are too large to represent with the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted or plain is null</exception>
                */
                BigPolyArray ^AddPlain(BigPolyArray ^encrypted, BigPoly ^plain);

                /**
                <summary>Subtracts a ciphertext with a plaintext, and stores the result in the destination
                parameter.</summary>

                <remarks>
                Subtracts a ciphertext with a plaintext, and stores the result in the destination parameter.
                The plaintext must have a significant coefficient count smaller than the coefficient
                count specified by the encryption parameters, and with coefficient values less-than the plain modulus
                (<see cref="EncryptionParameters::PlainModulus"/>).
                </remarks>
                <param name="encrypted">The ciphertext to subtract</param>
                <param name="plain">The plaintext to subtract</param>
                <param name="destination">The ciphertext to overwrite with the subtraction result</param>
                <exception cref="System::ArgumentException">if the ciphertexts are not valid for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if the plain polynomial's significant coefficient count or coefficient
                values are too large to represent with the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted, plain, or destination is null</exception>
                */
                void SubPlain(BigPolyArray ^encrypted, BigPoly ^plain, BigPolyArray ^destination);

                /**
                <summary>Subtracts a ciphertext with a plaintext, and returns the result.</summary>

                <remarks>
                Subtracts a ciphertext with a plaintext, and returns the result. The plaintext must have a
                significant coefficient count smaller than the coefficient count specified by the encryption parameters, and with
                coefficient values less-than the plain modulus (<see cref="EncryptionParameters::PlainModulus"/>).
                </remarks>
                <param name="encrypted">The ciphertext to subtract</param>
                <param name="plain">The plaintext to subtract</param>
                <exception cref="System::ArgumentException">if the ciphertexts are not valid for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if the plain polynomial's significant coefficient count or coefficient
                values are too large to represent with the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypted or plain is null</exception>
                */
                BigPolyArray ^SubPlain(BigPolyArray ^encrypted, BigPoly ^plain);

                /**
                <summary>Multiplies a ciphertext with a plaintext, and stores the result in the destination
                parameter.</summary>

                <remarks>
                <para>
                Multiplies a ciphertext with a plaintext, and stores the result in the destination parameter. 
                The plaintext must have a significant coefficient count smaller than the coefficient
                count specified by the encryption parameters, and with coefficient values less-than the plain modulus
                (<see cref="EncryptionParameters::PlainModulus"/>).
                </para>
                <para>
                Multiplying by a plaintext 0 is not allowed and will result in the library throwing an invalid
                argument exception. The reason behind this design choice is that the result should
                be a fresh encryption of 0, but creating fresh encryptions should not be something
                this class does. Instead the user should separately handle the cases where the
                plain multiplier is 0.
                </para>
                </remarks>
                <param name="encrypted">The ciphertext to multiply</param>
                <param name="plain">The plaintext to multiply</param>
                <param name="destination">The ciphertext to overwrite with the multiplication result</param>
                <exception cref="System::ArgumentException">if the ciphertexts are not valid for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if the plain polynomial's significant coefficient count or coefficient
                values are too large to represent with the encryption parameters</exception>
                <exception cref="System::ArgumentException">if the plaintext multiplier is zero</exception>
                <exception cref="System::ArgumentNullException">if encrypted, plain, or destination is null</exception>
                */
                void MultiplyPlain(BigPolyArray ^encrypted, BigPoly ^plain, BigPolyArray ^destination);

                /**
                <summary>Multiplies a ciphertext with a plaintext, and returns the result.</summary>

                <remarks>
                <para>
                Multiplies a ciphertext with a plaintext, and returns the result. The plaintext must have a
                significant coefficient count smaller than the coefficient count specified by the encryption parameters, and with
                coefficient values less-than the plain modulus (<see cref="EncryptionParameters::PlainModulus"/>).
                </para>
                <para>
                Multiplying by a plaintext 0 is not allowed and will result in the library throwing an invalid
                argument exception. The reason behind this design choice is that the result should
                be a fresh encryption of 0, but creating fresh encryptions should not be something
                this class does. Instead the user should separately handle the cases where the
                plain multiplier is 0.
                </para>
                </remarks>
                <param name="encrypted">The ciphertext to multiply</param>
                <param name="plain">The plaintext to multiply</param>
                <exception cref="System::ArgumentException">if the ciphertexts are not valid for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if the plain polynomial's significant coefficient count or coefficient
                values are too large to represent with the encryption parameters</exception>
                <exception cref="System::ArgumentException">if the plaintext multiplier is zero</exception>
                <exception cref="System::ArgumentNullException">if encrypted or plain is null</exception>
                */
                BigPolyArray ^MultiplyPlain(BigPolyArray ^encrypted, BigPoly ^plain);

                /**
                <summary>Multiplies a list of ciphertexts together and stores the result in the destination parameter.</summary>

                <remarks>
                <para>
                Multiplies a list of ciphertexts together and stores the result in the destination parameter. Relinearization 
                is performed after every multiplication, so enough encryption keys must have been given to the constructor 
                of the Evaluator.
                </para>
                </remarks>
                <param name="encrypteds">The list of ciphertexts to multiply</param>
                <param name="destination">The ciphertext to overwrite with the multiplication result</param>
                <exception cref="System::ArgumentException">if the encrypteds list is empty</exception>
                <exception cref="System::ArgumentException">if the ciphertexts are not valid ciphertexts for the encryption parameters</exception>.
                <exception cref="System::ArgumentNullException">if encrypteds or any of its elements is null</exception>
                <exception cref="System::ArgumentNullException">if destination is null</exception>
                */
                void MultiplyMany(System::Collections::Generic::List<BigPolyArray^> ^encrypteds, BigPolyArray ^destination);

                /**
                <summary>Multiplies a list of ciphertexts together and returns the result.</summary>

                <remarks>
                <para>
                Multiplies a list of ciphertexts together and returns the result. Relinearization is performed after 
                every multiplication, so enough encryption keys must have been given to the constructor of the Evaluator.
                </para>
                </remarks>
                <param name="encrypteds">The list of ciphertexts to multiply</param>
                <exception cref="System::ArgumentException">if the encrypteds list is empty</exception>
                <exception cref="System::ArgumentException">if the ciphertexts are not valid ciphertexts for the encryption parameters</exception>
                <exception cref="System::ArgumentNullException">if encrypteds or any of its elements is null</exception>
                */
                BigPolyArray ^MultiplyMany(System::Collections::Generic::List<BigPolyArray^> ^encrypteds);

                /**
                <summary>Raises a ciphertext to the specified power and stores the result in the destination parameter.</summary>
                <remarks>
                <para>
                Raises a ciphertext to the specified power and stores the result in the destination parameter. 
                Relinearization is performed after every multiplication, so enough encryption keys must have 
                been given to the constructor of the Evaluator.
                </para>
                <para>
                Exponentiation to power 0 is not allowed and will result in the library throwing an invalid argument
                exception. The reason behind this design choice is that the result should be a fresh encryption
                of 1, but creating fresh encryptions should not be something this class does. Instead the user
                should separately handle the cases where the exponent is 0.
                </para>
                </remarks>
                <param name="encrypted">The ciphertext to raise to a power</param>
                <param name="exponent">The power to raise the ciphertext to</param>
                <param name="destination">The ciphertext to overwrite with the exponentiation result</param>
                <exception cref="System::ArgumentException">if the ciphertext is not valid for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if exponent is zero</exception>
                <exception cref="System::ArgumentNullException">if encrypted or destination is null</exception>
                */
                void Exponentiate(BigPolyArray ^encrypted, System::UInt64 exponent, BigPolyArray ^destination);

                /**
                <summary>Raises a ciphertext to the specified power and returns the result.</summary>
                <remarks>
                <para>
                Raises a ciphertext to the specified power and returns the result. Relinearization is performed 
                after every multiplication, so enough encryption keys must have been given to the constructor 
                of the Evaluator.
                </para>
                <para>
                Exponentiation to power 0 is not allowed and will result in the library throwing an invalid argument
                exception. The reason behind this design choice is that the result should be a fresh encryption
                of 1, but creating fresh encryptions should not be something this class does. Instead the user
                should separately handle the cases where the exponent is 0.
                </para>
                </remarks>
                <param name="encrypted">The ciphertext to raise to a power</param>
                <param name="exponent">The power to raise the ciphertext to</param>
                <exception cref="System::ArgumentException">if the ciphertext is not valid for the encryption parameters</exception>
                <exception cref="System::ArgumentException">if exponent is zero</exception>
                <exception cref="System::ArgumentNullException">if encrypted is null</exception>
                */
                BigPolyArray ^Exponentiate(BigPolyArray ^encrypted, System::UInt64 exponent);

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
