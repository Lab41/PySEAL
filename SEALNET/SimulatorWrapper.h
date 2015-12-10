#pragma once

#include "simulator.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class EncryptionParameters;

            ref class BigUInt;

            ref class BigPoly;
            
            ref class SimulationEvaluator;

            ref class Evaluator;

            /**
            <summary>Models the inherent noise in a ciphertext based on given EncryptionParameters object.</summary>

            <remarks>
            <para>
            Models the inherent noise in a ciphertext based on given <see cref="EncryptionParameters"/> object. When performing arithmetic
            operations on encrypted data, the quality of the ciphertexts will degrade, i.e. the inherent noise in them will grow,
            until at a certain point decryption will fail to work. The Simulation object together with <see cref="SimulationEvaluator"/> can
            help the user understand how inherent noise grows in different homomorphic operations, and to adjust the encryption
            parameters accordingly.
            </para>
            <para>
            Instances of Simulation can be manipulated using <see cref="SimulationEvaluator"/>, which has a public API similar to <see cref="Evaluator"/>,
            making existing code easy to run on simulations instead of running it on actual encrypted data. In other words, using
            <see cref="SimulationEvaluator"/>, simulations can be added, multiplied, subtracted, negated, etc., and the result is always a new
            Simulation object whose inherent noise is obtained using average-case analysis of the noise behavior in the encryption
            scheme.
            </para>
            <para>
            Technically speaking, the inherent noise of a ciphertext is a polynomial, but the condition for decryption working
            depends on the size of the largest absolute value of its coefficients. It is really the size of this largest absolute
            value that Simulation is simulating, and that we will call the "noise", the "inherent noise", or the "error", in this
            documentation. The reader is referred to the description of the encryption scheme for more details.
            </para>
            </remarks>
            <seealso cref="SimulationEvaluator">See SimulationEvaluator for manipulating instances of Simulation.</seealso>
            */
            public ref class Simulation
            {
            public:
                /**
                <summary>Creates a simulation of a fresh ciphertext encrypted with the specified encryption parameters.</summary>
                <param name="parms">The encryption parameters</param>
                <exception cref="System::ArgumentNullException">if parms is null</exception>
                <exception cref="System::ArgumentException">if encryption parameters are not valid</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption
                parameters.</seealso>
                */
                Simulation(EncryptionParameters ^parms);

                /**
                <summary>Creates a simulation of a ciphertext encrypted with the specified encryption parameters and given inherent
                noise.</summary>
                <param name="parms">The encryption parameters</param>
                <param name="noise">The inherent noise in the created ciphertext</param>
                <exception cref="System::ArgumentNullException">if parms or noise is null</exception>
                <exception cref="System::ArgumentException">if encryption parameters are not valid</exception>
                <exception cref="System::ArgumentException">if noise is bigger than the given coefficient modulus</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption
                parameters.</seealso>
                */
                Simulation(EncryptionParameters ^parms, BigUInt ^noise);

                /**
                <summary>Creates a copy of a C++ Simulation.</summary>
                <remarks>
                Creates a copy of a C++ Simulation. The created Simulation will have the same inherent noise as the original.
                </remarks>
                <param name="copy">The Simulation to copy from</param>
                */
                Simulation(const seal::Simulation &copy);

                /**
                <summary>Overwrites the Simulation with the value of the specified Simulation.</summary>
                <param name="assign">The Simulation whose value should be assigned to the current Simulation</param>
                <exception cref="System::ArgumentNullException">If assign is null</exception>
                */
                void Set(Simulation ^assign);

                /**
                <summary>Returns the value of inherent noise (represented by BigUInt) that is being
                simulated.</summary>
                <remarks>
                <para>
                Returns the value of inherent noise (represented by <see cref="BigUInt"/>) that is being simulated. If the returned
                value is larger than that of <see cref="MaxNoise"/>, the encryption parameters used are possibly not large enough to
                support the performed homomorphic operations.
                </para>
                <para>
                The average-case estimates used by the simulator are typically conservative, so the size of noise tends to be
                overestimated.
                </para>
                </remarks>
                <seealso cref="NoiseBits()">See NoiseBits() to instead return the bit length of the value of inherent noise that is
                being simulated.</seealso>
                */
                property BigUInt ^Noise
                {
                    BigUInt ^get();
                }

                /**
                <summary>Returns a reference to the maximal value of inherent noise (represented by BigUInt) that a ciphertext
                encrypted using the given encryption parameters can contain and still decrypt correctly.</summary>
                <remarks>
                Returns a reference to the maximal value of inherent noise (represented by <see cref="BigUInt"/>) that a ciphertext encrypted using
                the given encryption parameters can contain and still decrypt correctly. If <see cref="Noise()"/> returns a value larger than this,
                the encryption parameters used are possibly not large enough to support the performed homomorphic operations.
                </remarks>
                */
                property BigUInt ^MaxNoise
                {
                    BigUInt ^get();
                }

                /**
                <summary>Returns the bit length of the value of inherent noise that is being simulated.</summary>
                <remarks>
                <para>
                Returns the bit length of the value of inherent noise that is being simulated.
                </para>
                <para>
                The average-case estimates used by the simulator are typically conservative, so the amount of noise tends to be
                overestimated.
                </para>
                </remarks>
                */
                property int NoiseBits
                {
                    int get();
                }

                /**
                <summary>Returns the difference between the bit lengths of the return values of maxNoise() and of noise().</summary>
                <remarks>
                <para>
                Returns the difference between the bit lengths of the return values of <see cref="MaxNoise()"/> and of <see cref="Noise()"/>. This gives the user a
                convenient tool for estimating how many, if any, arithmetic operations can still be performed on the encrypted data
                before it becomes impossible to decrypt. If the return value is negative, the encryption parameters used are not large
                enough to support the performed arithmetic operations.
                </para>
                <para>
                The average-case estimates used by the simulator are typically conservative, so the amount of noise tends to be
                overestimated.
                </para>
                </remarks>
                */
                property int NoiseBitsLeft
                {
                    int get();
                }

                /**
                <summary>Returns a reference to the coefficient modulus.</summary>
                */
                property BigUInt ^CoeffModulus
                {
                    BigUInt ^get();
                }

                /**
                <summary>Returns a reference to the plaintext modulus.</summary>
                */
                property BigUInt ^PlainModulus
                {
                    BigUInt ^get();
                }

                /**
                <summary>Returns true or false depending on whether the encryption parameters were large enough to support the
                performed homomorphic operations.</summary>
                <remarks>
                <para>
                Returns true or false depending on whether the encryption parameters were large enough to support the performed
                homomorphic operations.
                </para>
                <para>
                The average-case estimates used by the simulator are typically conservative, so the amount of noise tends to be
                overestimated, and decryption might work even if Decrypts() returns false.
                </para>
                </remarks>
                */
                bool Decrypts();

                /**
                <summary>Destroys the Simulation.</summary>
                */
                ~Simulation();

                /**
                <summary>Destroys the Simulation.</summary>
                */
                !Simulation();

                /**
                <summary>Returns a reference to the underlying C++ Simulation.</summary>
                */
                seal::Simulation &GetSimulation();

            internal:

            private:
                seal::Simulation *simulation_;
            };

            /**
            <summary>Manipulates instances of Simulation with a public API similar to how Evaluator manipulates ciphertexts
            (represented by BigPoly).</summary>
            <remarks>
            <para>
            Manipulates instances of <see cref="Simulation"/> with a public API similar to how <see cref="Evaluator"/> manipulates ciphertexts (represented by
            <see cref="BigPoly"/>). This makes existing code easy to run on Simulation objects instead of running it on actual encrypted data.
            </para>
            <para>
            Simulation objects model the inherent noise in a ciphertext based on given encryption parameters. When performing
            homomorphic operations on encrypted data, the quality of the ciphertexts will degrade, i.e. the inherent noise in them
            will grow, until at a certain point decryption will fail to work. The Simulation object together with
            SimulationEvaluator can help the user understand how the inherent noise grows in different homomorphic operations, and
            to adjust the encryption parameters accordingly.
            </para>
            <para>
            SimulationEvaluator allows the user to simulate the effect of homomorphic operations on the inherent noise in encrypted
            data. These homomorphic operations include addition, multiplication, subtraction, negation, etc., and the result is
            always a new Simulation object whose inherent noise is obtained using average-case analysis of the encryption scheme.
            </para>
            <para>
            Technically speaking, the inherent noise of a ciphertext is a polynomial, but the condition for decryption working
            depends on the size of the largest absolute value of its coefficients. It is really the size of this largest absolute
            value that Simulation is simulating, and that we will call the "noise", the "inherent noise", or the "error", in this
            documentation. The reader is referred to the description of the encryption scheme for more details.
            </para>
            <para>
            The SimulationEvaluator class is not thread-safe and a separate SimulationEvaluator instance is needed for each
            potentially concurrent usage.
            </para>
            <para>
            Accuracy of the average-case analysis depends on the encryption parameters.
            </para>
            </remarks>
            <seealso cref="Simulation">See Simulation for the object modeling the noise in ciphertexts.</seealso>
            */
            public ref class SimulationEvaluator
            {
            public:
                /**
                <summary>Creates a new SimulationEvaluator.</summary>
                */
                SimulationEvaluator();

                /**
                <summary>Destroys the SimulationEvaluator.</summary>
                */
                ~SimulationEvaluator();

                /**
                <summary>Destroys the SimulationEvaluator.</summary>
                */
                !SimulationEvaluator();

                /**
                <summary>Simulates inherent noise growth in Evaluator::Multiply() and returns the result.</summary>
                <param name="simulation1">The first <see cref="Simulation"/> object to multiply</param>
                <param name="simulation2">The second <see cref="Simulation"/> object to multiply</param>
                <exception cref="System::ArgumentNullException">if simulation1 or simulation2 is null</exception>
                <exception cref="System::ArgumentException">if simulation1 and simulation2 were constructed with different encryption
                parameters</exception>
                <seealso>See Evaluator::Multiply() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^Multiply(Simulation ^simulation1, Simulation ^simulation2);

                /*
                <summary>Simulates inherent noise growth in Evaluator::Relinearize() and returns the result.</summary>
                <para>
                THIS FUNCTION IS BROKEN This function is broken and can not be used except together with MultiplyNoRelin().
                The problem is that relinearization adds additive terms that may be significant
                depending on under what power of the secret key the result decrypts.
                This function omits those terms.
                </para>
                <param name="simulation">The <see cref="Simulation"/> object to relinearize</param>
                <exception cref="System::ArgumentNullException">if simulation is null</exception>
                <seealso>See Evaluator::Relinearize() for the corresponding operation on ciphertexts.</seealso>
                
                Simulation ^Relinearize(Simulation ^simulation);

                
                <summary>Simulates inherent noise growth in Evaluator::MultiplyNoRelin() and returns the result.</summary>
                <para>
                THIS FUNCTION IS BROKEN and can not be used except together with Relinearize().
                The problem is that multiplication without relinearization also adds an additive term,
                which this function omits, and that additive term depends on under what powers of
                the key the ciphertexts decrypt.
                </para>
                <param name="simulation1">The first <see cref="Simulation"/> object to multiply</param>
                <param name="simulation2">The second <see cref="Simulation"/> object to multiply</param>
                <exception cref="System::ArgumentNullException">if simulation1 or simulation2 is null</exception>
                <exception cref="System::ArgumentException">if simulation1 and simulation2 were constructed with different encryption
                parameters</exception>
                <seealso>See Evaluator::MultiplyNoRelin() for the corresponding operation on ciphertexts.</seealso>
                
                Simulation ^MultiplyNoRelin(Simulation ^simulation1, Simulation ^simulation2);
                */

                /**
                <summary>Simulates inherent noise growth in Evaluator::Add() and returns the result.</summary>
                <param name="simulation1">The first <see cref="Simulation"/> object to add</param>
                <param name="simulation2">The second <see cref="Simulation"/> object to add</param>
                <exception cref="System::ArgumentNullException">if simulation1 or simulation2 is null</exception>
                <exception cref="System::ArgumentException">if simulation1 and simulation2 were constructed with different encryption
                parameters</exception>
                <seealso>See Evaluator::Add() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^Add(Simulation ^simulation1, Simulation ^simulation2);

                /**
                <summary>Simulates inherent noise growth in Evaluator::AddMany() and returns the result.</summary>
                <param name="simulations">The simulations to add</param>
                <exception cref="System::ArgumentNullException">if simulations or any of its elements is null</exception>
                <exception cref="System::ArgumentException">if simulations is empty</exception>
                <exception cref="System::ArgumentException">if not all elements of simulations were constructed with the same
                encryption parameters</exception>
                <seealso>See Evaluator::AddMany() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^AddMany(System::Collections::Generic::List<Simulation^> ^simulations);

                /**
                <summary>Simulates inherent noise growth in Evaluator::Sub() and returns the result.</summary>
                <param name="simulation1">The <see cref="Simulation"/> object to subtract from</param>
                <param name="simulation2">The <see cref="Simulation"/> object to subtract</param>
                <exception cref="System::ArgumentNullException">if simulation1 or simulation2 is null</exception>
                <exception cref="System::ArgumentException">if simulation1 and simulation2 were constructed with different encryption
                parameters</exception>
                <seealso>See Evaluator::Sub() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^Sub(Simulation ^simulation1, Simulation ^simulation2);

                /**
                <summary>Simulates inherent noise growth in Evaluator::MultiplyPlain() given an upper bound for the maximum number of
                non-zero coefficients and an upper bound for their absolute value (represented by <see cref="BigUInt"/>) in the encoding of the
                plaintext multiplier and returns the result.</summary>
                <param name="simulation">The <see cref="Simulation"/> object to multiply</param>
                <param name="plainMaxCoeffCount">An upper bound on the number of non-zero coefficients in the plain polynomial to multiply</param>
                <param name="plainMaxAbsValue">An upper bound (represented by <see cref="BigUInt"/>) on the absolute value of coefficients in the
                plain polynomial to multiply</param>
                <exception cref="System::ArgumentNullException">if simulation or plainMaxAbsValue is null</exception>
                <exception cref="System::ArgumentException">if plainMaxCoeffCount is out of range</exception>
                <seealso>See Evaluator::MultiplyPlain() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^MultiplyPlain(Simulation ^simulation, int plainMaxCoeffCount, BigUInt ^plainMaxAbsValue);

                /**
                <summary>Simulates inherent noise growth in Evaluator::MultiplyPlain() given an upper bound for the maximum number of
                non-zero coefficients and an upper bound for their absolute value (represented by <see cref="System::UInt64"/>) in the encoding of the
                plaintext multiplier and returns the result.</summary>
                <param name="simulation">The <see cref="Simulation"/> object to multiply</param>
                <param name="plainMaxCoeffCount">An upper bound on the number of non-zero coefficients in the plain polynomial to multiply</param>
                <param name="plainMaxAbsValue">An upper bound (represented by <see cref="System::UInt64"/>) on the absolute value of coefficients in the
                plain polynomial to multiply</param>
                <exception cref="System::ArgumentNullException">if simulation is null</exception>
                <exception cref="System::ArgumentException">if plainMaxCoeffCount is out of range</exception>
                <seealso>See Evaluator::MultiplyPlain() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^MultiplyPlain(Simulation ^simulation, int plainMaxCoeffCount, System::UInt64 plainMaxAbsValue);

                /**
                <summary>Simulates inherent noise growth in Evaluator::AddPlain() and returns the result.</summary>
                <param name="simulation">The <see cref="Simulation"/> object to add to</param>
                <exception cref="System::ArgumentNullException">if simulation is null</exception>
                <seealso>See Evaluator::AddPlain() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^AddPlain(Simulation ^simulation);

                /**
                <summary>Simulates inherent noise growth in Evaluator::SubPlain() and returns the result.</summary>
                <param name="simulation">The <see cref="Simulation"/> object to subtract from</param>
                <exception cref="System::ArgumentNullException">if simulation is null</exception>
                <seealso>See Evaluator::SubPlain() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^SubPlain(Simulation ^simulation);

                /**
                <summary>Simulates inherent noise growth in Evaluator::MultiplyMany() and returns the result.</summary>
                <param name="simulations">The list of <see cref="Simulation"/> objects to multiply</param>
                <exception cref="System::ArgumentNullException">if simulations list or any of its elements is null</exception>
                <exception cref="System::ArgumentException">if simulations list is empty</exception>
                <exception cref="System::ArgumentException">if at least two of the elements in the simulations list were
                constructed with different encryption parameters</exception>
                <seealso>See Evaluator::MultiplyMany() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^MultiplyMany(System::Collections::Generic::List<Simulation^> ^simulations);

                /**
                <summary>Simulates inherent noise growth in Evaluator::Exponentiate() and returns the result.</summary>
                <param name="simulation">The <see cref="Simulation"/> object to raise to a power</param>
                <param name="exponent">The non-negative power to raise the <see cref="Simulation"/> object to</param>
                <exception cref="System::ArgumentNullException">if simulation is null</exception>
                <exception cref="System::ArgumentException">if the exponent is negative</exception>
                <seealso>See Evaluator::Exponentiate() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^Exponentiate(Simulation ^simulation, int exponent);

                /**
                <summary>Simulates inherent noise growth in Evaluator::Negate() and returns the result.</summary>
                <param name="simulation">The <see cref="Simulation"/> object to negate</param>
                <exception cref="System::ArgumentNullException">if simulation is null</exception>
                <seealso>See Evaluator::Negate() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^Negate(Simulation ^simulation);

            private:
                seal::SimulationEvaluator *simulationEvaluator_;
            };
        }
    }
}
