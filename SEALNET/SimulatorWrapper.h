#pragma once

#include "simulator.h"
#include "EncryptionParamsWrapper.h"
#include "EvaluatorWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {   
            ref class SimulationEvaluator;

            /**
            <summary>Models the invariant noise budget in a ciphertext based on given EncryptionParameters object.</summary>

            <remarks>
            <para>
            Models the invariant noise budget in a ciphertext based on given <see cref="EncryptionParameters"/> object.
            When performing arithmetic operations on encrypted data, the quality of the ciphertexts will degrade,
            i.e. the invariant noise budget will be consumed, until at a certain point the budget will reach 0, and
            decryption will fail to work. The Simulation object together with <see cref="SimulationEvaluator"/> can 
            help the user understand how the invariant noise budget is consumed in different homomorphic operations, 
            and to adjust the encryption parameters accordingly.
            </para>
            <para>
            Instances of Simulation can be manipulated using <see cref="SimulationEvaluator"/>, which has a public API similar 
            to Evaluator, making existing code easy to run on simulations instead of running it on actual encrypted data. In 
            other words, using <see cref="SimulationEvaluator"/>, simulations can be added, multiplied, subtracted, negated, etc., 
            and the result is always a new Simulation object whose noise budget is obtained using  heuristic worst-case analysis 
            of the noise behavior in the encryption scheme.
            </para>
            <para>
            The invariant noise polynomial of a ciphertext is a rational coefficient polynomial, such that
            a ciphertext decrypts correctly as long as the coefficients of the invariant noise polynomial are
            of absolute value less than 1/2. Thus, we call the infinity-norm of the invariant noise polynomial
            the invariant noise, and for correct decryption require it to be less than 1/2. If v denotes the
            invariant noise, we define the invariant noise budget as -log2(2v). Thus, the invariant noise budget
            starts from some initial value, which depends on the encryption parameters, and decreases to 0 when
            computations are performed. When the budget reaches 0, the ciphertext becomes too noisy to decrypt
            correctly.
            </para>
            </remarks>
            <seealso cref="SimulationEvaluator">See SimulationEvaluator for manipulating instances of Simulation.</seealso>
            */
            public ref class Simulation
            {
            public:
                /**
                <summary>Creates a simulation of a ciphertext encrypted with the specified encryption parameters and 
                given invariant noise budget.</summary>

                <remarks>
                Creates a simulation of a ciphertext encrypted with the specified encryption parameters and
                given invariant noise budget. The given noise budget must be at least zero, and at most the 
                significant bit count of the coefficient modulus minus two.
                </remarks>
                <param name="parms">The encryption parameters</param>
                <param name="noiseBudget">The invariant noise budget of the created ciphertext</param>
                <param name="ciphertextSize">The size of the created ciphertext</param>
                <exception cref="System::ArgumentNullException">if parms is null</exception>
                <exception cref="System::ArgumentException">if encryption parameters are not valid</exception>
                <exception cref="System::ArgumentException">if noiseBudget is not in the valid range</exception>
                <exception cref="System::ArgumentException">if ciphertextSize is less than 2</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption
                parameters.</seealso>
                */
                Simulation(EncryptionParameters ^parms, int noiseBudget, int ciphertextSize);

                /**
                <summary>Overwrites the Simulation with the value of the specified Simulation.</summary>
                <param name="assign">The Simulation whose value should be assigned to the current Simulation</param>
                <exception cref="System::ArgumentNullException">If assign is null</exception>
                */
                void Set(Simulation ^assign);

                /**
                <summary>Returns the invariant noise budget that is being simulated.</summary>

                <remarks>
                Returns the invariant noise budget that is being simulated. If the returned value is less than or 
                equal to 0, the encryption parameters used are possibly not large enough to support the performed 
                homomorphic operations.
                </remarks>
                */
                property int InvariantNoiseBudget
                {
                    int get();
                }

                /**
                <summary>Returns true or false depending on whether the encryption parameters were large enough to support 
                the performed homomorphic operations.</summary>
                */
                bool Decrypts();

                /**
                <summary>Returns true or false depending on whether the encryption parameters were large enough to support 
                the performed homomorphic operations.</summary>

                <remarks>
                Returns true or false depending on whether the encryption parameters were large enough to support the performed
                homomorphic operations. The budgetGap parameter parameter can be used to ensure that a certain
                amount of noise budget remains unused.
                </remarks>
                <param name="budgetGap">The amount of noise budget (bits) that should remain unused</param>
                <exception cref="System::ArgumentException">if budgetGap is negative</exception>
                */
                bool Decrypts(int budgetGap);

                /**
                <summary>Returns the size of the ciphertext whose noise is modeled by the simulation.</summary>
                <seealso>See BigPolyArray::Size for the corresponding property on BigPolyArray objects.</seealso>
                */
                property int Size
                {
                    int get();
                }

                /**
                <summary>Destroys the Simulation.</summary>
                */
                ~Simulation();

                /**
                <summary>Destroys the Simulation.</summary>
                */
                !Simulation();

            internal:
                /**
                <summary>Returns a reference to the underlying C++ Simulation.</summary>
                */
                seal::Simulation &GetSimulation();

                /**
                <summary>Creates a copy of a C++ Simulation.</summary>
                <remarks>
                Creates a copy of a C++ Simulation. The created Simulation will have the same inherent noise as the original.
                </remarks>
                <param name="copy">The Simulation to copy from</param>
                */
                Simulation(const seal::Simulation &copy);

            private:
                seal::Simulation *simulation_;
            };

            /**
            <summary>Manipulates instances of Simulation with a public API similar to how Evaluator manipulates ciphertexts.</summary>
            
            <remarks>
            <para>
            Manipulates instances of <see cref="Simulation"/> with a public API similar to how <see cref="Evaluator" /> 
            manipulates ciphertexts. This makes existing code easy to run on Simulation objects instead of running it 
            on actual encrypted data.
            </para>
            <para>
            Simulation objects model the invariant noise budget in a ciphertext based on given encryption parameters.
            When performing homomorphic operations on encrypted data, the quality of the ciphertexts will degrade,
            i.e. the invariant noise budget will be consumed, until at a certain point the budget will reach 0, and
            decryption will fail to work. The Simulation object together with SimulationEvaluator can help the user
            understand how the noise budget is consumed in different homomorphic operations, and to adjust the
            encryption parameters accordingly.
            </para>
            <para>
            <see cref="SimulationEvaluator" /> allows the user to simulate the effect of homomorphic operations on 
            the inherent noise in encrypted data. These homomorphic operations include addition, multiplication, 
            subtraction, negation, etc., and the result is always a new Simulation object whose inherent noise is 
            obtained using average-case analysis of the encryption scheme.
            </para>
            <para>
            The invariant noise polynomial of a ciphertext is a rational coefficient polynomial, such that
            a ciphertext decrypts correctly as long as the coefficients of the invariant noise polynomial are
            of absolute value less than 1/2. Thus, we call the infinity-norm of the invariant noise polynomial
            the invariant noise, and for correct decryption require it to be less than 1/2. If v denotes the
            invariant noise, we define the invariant noise budget as -log2(2v). Thus, the invariant noise budget
            starts from some initial value, which depends on the encryption parameters, and decreases to 0 when
            computations are performed. When the budget reaches 0, the ciphertext becomes too noisy to decrypt
            correctly.
            </para>
            <para>
            The SimulationEvaluator class is not thread-safe and a separate SimulationEvaluator instance is needed for each
            potentially concurrent usage.
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
                <summary>Creates a new SimulationEvaluator.</summary>

                <remarks>
                Creates a new SimulationEvaluator. The user can give a <see cref="MemoryPoolHandle "/> object to use
                a custom memory pool instead of the global memory pool (default).
                </remarks>
                <param name="pool">The memory pool handle</param>
                <exception cref="System::ArgumentNullException">if pool is null</exception>
                <seealso cref="MemoryPoolHandle">See MemoryPoolHandle for more details on memory pool handles.</seealso>
                */
                SimulationEvaluator(MemoryPoolHandle ^pool);

                /**
                <summary>Destroys the SimulationEvaluator.</summary>
                */
                ~SimulationEvaluator();

                /**
                <summary>Destroys the SimulationEvaluator.</summary>
                */
                !SimulationEvaluator();

                /**
                <summary>Creates a Simulation object corresponding to a freshly encrypted ciphertext.</summary>
                
                <remarks>
                Creates a Simulation object corresponding to a freshly encrypted ciphertext. The noise is estimated
                based on the given encryption parameters, and size parameters of a virtual input plaintext polynomial, 
                namely an upper bound plainMaxCoeffCount on the number of non-zero coefficients in the polynomial, 
                and an upper bound plainMaxAbsValue (represented by <see cref="BigUInt" />) on the absolute value 
                (modulo the plaintext modulus) of the polynomial coefficients.
                </remarks>
                <param name="parms">The encryption parameters</param>
                <param name="plainMaxCoeffCount">An upper bound on the number of non-zero coefficients in the underlying plaintext</param>
                <param name="plainMaxAbsValue">An upper bound on the absolute value of the coefficients in the underlying plaintext</param>
                <exception cref="System::ArgumentNullException">if parms or plainMaxAbsValue is null</exception>
                <exception cref="System::ArgumentException">if plainMaxCoeffCount is negative or bigger than the degree of the polynomial modulus</exception>
                <exception cref="System::ArgumentException">if plainMaxAbsValue is bigger than the plaintext modulus divided by 2</exception>
                */
                Simulation ^GetFresh(EncryptionParameters ^parms, int plainMaxCoeffCount, BigUInt ^plainMaxAbsValue);

                /**
                <summary>Creates a Simulation object corresponding to a freshly encrypted ciphertext.</summary>

                <remarks>
                Creates a Simulation object corresponding to a freshly encrypted ciphertext. The noise is estimated
                based on the given encryption parameters, and size parameters of a virtual input plaintext polynomial,
                namely an upper bound plainMaxCoeffCount on the number of non-zero coefficients in the polynomial,
                and an upper bound plainMaxAbsValue (represented by System::UInt64) on the absolute value (modulo the 
                plaintext modulus) of the polynomial coefficients.
                </remarks>
                <param name="parms">The encryption parameters</param>
                <param name="plainMaxCoeffCount">An upper bound on the number of non-zero coefficients in the underlying plaintext</param>
                <param name="plainMaxAbsValue">An upper bound on the absolute value of the coefficients in the underlying plaintext</param>
                <exception cref="System::ArgumentNullException">if parms is null</exception>
                <exception cref="System::ArgumentException">if plainMaxCoeffCount is negative or bigger than the degree of the polynomial modulus</exception>
                <exception cref="System::ArgumentException">if plainMaxAbsValue is bigger than the plaintext modulus divided by 2</exception>
                */
                Simulation ^GetFresh(EncryptionParameters ^parms, int plainMaxCoeffCount, System::UInt64 plainMaxAbsValue);

                /**
                <summary>Simulates noise budget consumption in Evaluator::Relinearize() and returns the result.</summary>
                
                <param name="simulation">The <see cref="Simulation"/> object to relinearize</param>
                <exception cref="System::ArgumentNullException">if simulation is null</exception>
                <exception cref="System::ArgumentException">if simulation has Size less than 2</exception>
                <seealso cref="Evaluator::Relinearize(BigPolyArray^)">See Evaluator::Relinearize() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^Relinearize(Simulation ^simulation);

                /**
                <summary>Simulates noise budget consumption in Evaluator::Relinearize() when a ciphertext is relinearized to a specified size and returns the result.</summary>
                
                <param name="simulation">The <see cref="Simulation"/> object to relinearize</param>
                <param name="destinationSize"> The size of the ciphertext (represented by the output simulation) after relinearization, defaults to 2
                <exception cref="System::ArgumentException">if destinationSize is less than 2 or greater than that of the ciphertext represented by simulation</exception>
                <exception cref="System::ArgumentNullException">if simulation is null</exception>
                <exception cref="System::ArgumentException">if simulation has Size less than 2</exception>
                <seealso cref="Evaluator::Relinearize(BigPolyArray^,int)">See Evaluator::Relinearize() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^Relinearize(Simulation ^simulation, int destinationSize);

                /**
                <summary>Simulates noise budget consumption in Evaluator::Multiply() and returns the result.</summary>
                
                <param name="simulation1">The first <see cref="Simulation"/> object to multiply</param>
                <param name="simulation2">The second <see cref="Simulation"/> object to multiply</param>
                <exception cref="System::ArgumentNullException">if simulation1 or simulation2 is null</exception>
                <exception cref="System::ArgumentException">if simulation1 and simulation2 were constructed with different encryption
                parameters</exception>
                <exception cref="System::ArgumentException">if simulation1 or simulation2 has Size less than 2</exception>
                <seealso cref="Evaluator::Multiply(BigPolyArray^,BigPolyArray^)">See Evaluator::Multiply() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^Multiply(Simulation ^simulation1, Simulation ^simulation2);

                /**
                <summary>Simulates noise budget consumption in Evaluator::Square() and returns the result.</summary>
                <param name="simulation">The Simulation object to square</param>
                <exception cref="System::ArgumentException">if simulation has Size less than 2</exception>
                <seealso cref="Evaluator::Square(BigPolyArray^)">See Evaluator::Square() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^Square(Simulation ^simulation);
                
                /**
                <summary>Simulates noise budget consumption in Evaluator::Add() and returns the result.</summary>
                
                <param name="simulation1">The first <see cref="Simulation"/> object to add</param>
                <param name="simulation2">The second <see cref="Simulation"/> object to add</param>
                <exception cref="System::ArgumentNullException">if simulation1 or simulation2 is null</exception>
                <exception cref="System::ArgumentException">if simulation1 and simulation2 were constructed with different encryption
                parameters</exception>
                <exception cref="System::ArgumentException">if simulation1 or simulation2 has Size less than 2</exception>
                <seealso cref="Add(BigPolyArray^,BigPolyArray^)">See Evaluator::Add() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^Add(Simulation ^simulation1, Simulation ^simulation2);

                /**
                <summary>Simulates noise budget consumption in Evaluator::AddMany() and returns the result.</summary>
                
                <param name="simulations">The simulations to add</param>
                <exception cref="System::ArgumentNullException">if simulations or any of its elements is null</exception>
                <exception cref="System::ArgumentException">if simulations is empty</exception>
                <exception cref="System::ArgumentException">if not all elements of simulations were constructed with the same
                encryption parameters</exception>
                <exception cref="System::ArgumentException">if any of the elements in the simulation list has Size less than 2</exception>
                <seealso cref="Evaluator::AddMany(List<BigPolyArray^>^)">See Evaluator::AddMany() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^AddMany(System::Collections::Generic::List<Simulation^> ^simulations);

                /**
                <summary>Simulates noise budget consumption in Evaluator::Sub() and returns the result.</summary>
                
                <param name="simulation1">The <see cref="Simulation"/> object to subtract from</param>
                <param name="simulation2">The <see cref="Simulation"/> object to subtract</param>
                <exception cref="System::ArgumentNullException">if simulation1 or simulation2 is null</exception>
                <exception cref="System::ArgumentException">if simulation1 and simulation2 were constructed with different encryption
                parameters</exception>
                <exception cref="System::ArgumentException">if simulation1 or simulation2 has Size less than 2</exception>
                <seealso cref="Evaluator::Sub(BigPolyArray^,BigPolyArray^)">See Evaluator::Sub() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^Sub(Simulation ^simulation1, Simulation ^simulation2);

                /**
                <summary>Simulates noise budget consumption in Evaluator::MultiplyPlain() given an upper bound for the maximum number of
                non-zero coefficients and an upper bound for their absolute value (represented by <see cref="BigUInt"/>) in the encoding of the
                plaintext multiplier and returns the result.</summary>

                <param name="simulation">The <see cref="Simulation"/> object to multiply</param>
                <param name="plainMaxCoeffCount">An upper bound on the number of non-zero coefficients in the plain polynomial to multiply</param>
                <param name="plainMaxAbsValue">An upper bound (represented by <see cref="BigUInt"/>) on the absolute value of coefficients in the
                plain polynomial to multiply</param>
                <exception cref="System::ArgumentNullException">if simulation or plainMaxAbsValue is null</exception>
                <exception cref="System::ArgumentException">if simulation has Size less than 2</exception>
                <exception cref="System::ArgumentException">if plainMaxCoeffCount is out of range</exception>
                <exception cref="System::ArgumentException">if plainMaxCoeffCount or plainMaxAbsValue is zero</exception>
                <seealso cref="Evaluator::MultiplyPlain(BigPolyArray^,BigPoly^)">See Evaluator::MultiplyPlain() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^MultiplyPlain(Simulation ^simulation, int plainMaxCoeffCount, BigUInt ^plainMaxAbsValue);

                /**
                <summary>Simulates noise budget consumption in Evaluator::MultiplyPlain() given an upper bound for the maximum number of
                non-zero coefficients and an upper bound for their absolute value (represented by System::UInt64) in the encoding of the
                plaintext multiplier and returns the result.</summary>

                <param name="simulation">The <see cref="Simulation"/> object to multiply</param>
                <param name="plainMaxCoeffCount">An upper bound on the number of non-zero coefficients in the plain polynomial to multiply</param>
                <param name="plainMaxAbsValue">An upper bound (represented by System::UInt64) on the absolute value of coefficients in the
                plain polynomial to multiply</param>
                <exception cref="System::ArgumentNullException">if simulation is null</exception>
                <exception cref="System::ArgumentException">if simulation has Size less than 2</exception>
                <exception cref="System::ArgumentException">if plainMaxCoeffCount is out of range</exception>
                <exception cref="System::ArgumentException">if plainMaxCoeffCount or plainMaxAbsValue is zero</exception>
                <seealso cref="Evaluator::MultiplyPlain(BigPolyArray^,BigPoly^)">See Evaluator::MultiplyPlain() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^MultiplyPlain(Simulation ^simulation, int plainMaxCoeffCount, System::UInt64 plainMaxAbsValue);

                /**
                <summary>Simulates noise budget consumption in Evaluator::AddPlain() and returns the result.</summary>
                
                <param name="simulation">The <see cref="Simulation"/> object to add to</param>
                <param name="plainMaxCoeffCount">An upper bound on the number of non-zero coefficients in the plain polynomial to multiply</param>
                <param name="plainMaxAbsValue">An upper bound (represented by <see cref="BigUInt"/>) on the absolute value of coefficients in the
                plain polynomial to add</param>
                <exception cref="System::ArgumentNullException">if simulation or plainMaxAbsValue is null</exception>
                <exception cref="System::ArgumentException">if simulation has Size less than 2</exception>
                <exception cref="System::ArgumentException">if plainMaxCoeffCount is out of range</exception>
                <seealso cref="Evaluator::AddPlain(BigPolyArray^,BigPoly^)">See Evaluator::AddPlain() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^AddPlain(Simulation ^simulation, int plainMaxCoeffCount, BigUInt ^plainMaxAbsValue);

                /**
                <summary>Simulates noise budget consumption in Evaluator::AddPlain() and returns the result.</summary>

                <param name="simulation">The <see cref="Simulation"/> object to add to</param>
                <param name="plainMaxCoeffCount">An upper bound on the number of non-zero coefficients in the plain polynomial to multiply</param>
                <param name="plainMaxAbsValue">An upper bound (represented by System::UInt64) on the absolute value of coefficients 
                in the plain polynomial to add</param>
                <exception cref="System::ArgumentNullException">if simulation is null</exception>
                <exception cref="System::ArgumentException">if simulation has Size less than 2</exception>
                <exception cref="System::ArgumentException">if plainMaxCoeffCount is out of range</exception>
                <seealso cref="Evaluator::AddPlain(BigPolyArray^,BigPoly^)">See Evaluator::AddPlain() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^AddPlain(Simulation ^simulation, int plainMaxCoeffCount, System::UInt64 plainMaxAbsValue);

                /**
                <summary>Simulates noise budget consumption in Evaluator::SubPlain() and returns the result.</summary>
                
                <param name="simulation">The <see cref="Simulation"/> object to subtract from</param>
                <param name="plainMaxCoeffCount">An upper bound on the number of non-zero coefficients in the plain polynomial to multiply</param>
                <param name="plainMaxAbsValue">An upper bound (represented by <see cref="BigUInt"/>) on the absolute value of coefficients in the
                plain polynomial to subtract</param>
                <exception cref="System::ArgumentNullException">if simulation or plainMaxAbsValue is null</exception>
                <exception cref="System::ArgumentException">if simulation has Size less than 2</exception>
                <exception cref="System::ArgumentException">if plainMaxCoeffCount is out of range</exception>
                <seealso cref="Evaluator::SubPlain(BigPolyArray^,BigPoly^)">See Evaluator::SubPlain() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^SubPlain(Simulation ^simulation, int plainMaxCoeffCount, BigUInt ^plainMaxAbsValue);

                /**
                <summary>Simulates noise budget consumption in Evaluator::SubPlain() and returns the result.</summary>

                <param name="simulation">The <see cref="Simulation"/> object to subtract from</param>
                <param name="plainMaxCoeffCount">An upper bound on the number of non-zero coefficients in the plain polynomial to multiply</param>
                <param name="plainMaxAbsValue">An upper bound (represented by System::UInt64) on the absolute value of coefficients 
                in the plain polynomial to subtract</param>
                <exception cref="System::ArgumentNullException">if simulation is null</exception>
                <exception cref="System::ArgumentException">if simulation has Size less than 2</exception>
                <exception cref="System::ArgumentException">if plainMaxCoeffCount is out of range</exception>
                <seealso cref="Evaluator::SubPlain(BigPolyArray^,BigPoly^)">See Evaluator::SubPlain() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^SubPlain(Simulation ^simulation, int plainMaxCoeffCount, System::UInt64 plainMaxAbsValue);

                /**
                <summary>Simulates noise budget consumption in Evaluator::MultiplyMany() and returns the result.</summary>
                
                <param name="simulations">The list of <see cref="Simulation"/> objects to multiply</param>
                <exception cref="System::ArgumentNullException">if simulations list or any of its elements is null</exception>
                <exception cref="System::ArgumentException">if simulations list is empty</exception>
                <exception cref="System::ArgumentException">if at least two of the elements in the simulations list were
                constructed with different encryption parameters</exception>
                <seealso cref="Evaluator::MultiplyMany(List<BigPolyArray^>^)">See Evaluator::MultiplyMany() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^MultiplyMany(System::Collections::Generic::List<Simulation^> ^simulations);

                /**
                <summary>Simulates noise budget consumption in Evaluator::Exponentiate() and returns the result.</summary>
                
                <param name="simulation">The <see cref="Simulation"/> object to raise to a power</param>
                <param name="exponent">The power to raise the <see cref="Simulation"/> object to</param>
                <exception cref="System::ArgumentNullException">if simulation is null</exception>
                <exception cref="System::ArgumentException">if simulation has Size less than 2</exception>
                <exception cref="System::ArgumentException">if exponent is zero</exception>
                <seealso>See Evaluator::Exponentiate() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^Exponentiate(Simulation ^simulation, System::UInt64 exponent);

                /**
                <summary>Simulates noise budget consumption in Evaluator::Negate() and returns the result.</summary>
                
                <param name="simulation">The <see cref="Simulation"/> object to negate</param>
                <exception cref="System::ArgumentNullException">if simulation is null</exception>
                <exception cref="System::ArgumentException">if simulation has Size less than 2</exception>
                <seealso cref="Evaluator::Negate(BigPolyArray^)">See Evaluator::Negate() for the corresponding operation on ciphertexts.</seealso>
                */
                Simulation ^Negate(Simulation ^simulation);

            private:
                seal::SimulationEvaluator *simulationEvaluator_;
            };
        }
    }
}
