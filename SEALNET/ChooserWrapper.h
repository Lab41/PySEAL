#pragma once

#include "encryptionparams.h"
#include "simulator.h"
#include "encoder.h"
#include "chooser.h"
#include "BigUIntWrapper.h"
#include "BigPolyArrayWrapper.h"
#include "SimulatorWrapper.h"
#include "EncryptionParamsWrapper.h"
#include "EncoderWrapper.h"
#include "MemoryPoolHandleWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class ChooserEvaluator;

            ref class ChooserEncoder;

            ref class ChooserEncryptor;

            /**
            <summary>Models ciphertexts for the automatic parameter selection module.</summary>

            <remarks>
            <para>
            Models ciphertexts for the automatic parameter selection module. Choosing appropriate and secure
            parameters for homomorphic cryptosystems is difficult, and beyond what a non-expert user should have
            to worry about. The user programs their computations as they normally would, but replace plaintexts with
            ChooserPoly objects. When the program is executed, ChooserPoly objects store the computation as a directed
            acyclic graph, and can later be used to estimate the invariant noise budget consumption in the stored
            computation. The estimated noise budget in the output ciphertext is given by the <see cref="Simulate()"/> 
            function. This class is a part of the automatic parameter selection module.
            </para>
            <para>
            Each instance of ChooserPoly stores an estimate of the number of non-zero coefficients, and an estimate
            for their largest absolute value (represented by <see cref="BigUInt"/>) that a plaintext polynomial can 
            be expected to contain after some number of homomorphic operations have been performed on it. In addition,
            each ChooserPoly stores a directed acyclic graph of computations that it has gone through (operation history).
            </para>
            <para>
            Instances of ChooserPoly can be manipulated using an instance of <see cref="ChooserEvaluator"/>, which 
            has a public API similar to Evaluator, making existing code easy to run on ChooserPoly objects instead 
            of running it on actual encrypted data. In other words, using <see cref="ChooserEvaluator"/>, ChooserPoly 
            objects can be added, multiplied, subtracted, negated, etc., and the result is always a new ChooserPoly 
            object whose estimated noise budget can be obtained using the <see cref="Simulate()"/> function, 
            which uses heuristic worst-case analysis of the noise behavior in the encryption scheme.
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
            In general, reading a ChooserPoly is thread-safe while mutating is not. Specifically, concurrent 
            access must be avoided if the size bounds of the modeled plaintext data, or the operation history 
            are expected to be changed.
            </para>
            </remarks>
            <seealso cref="ChooserEvaluator">See ChooserEvaluator for manipulating instances of ChooserPoly.</seealso>
            <seealso cref="ChooserEncoder">See ChooserEncoder for modeling the behavior of encoding with ChooserPoly objects.</seealso>
            <seealso cref="ChooserEncryptor">See ChooserEncryptor for modeling the behavior of encryption with ChooserPoly objects.</seealso>
            <seealso cref="Simulation">See Simulation for the class that handles the noise budget consumption estimates.</seealso>
            */
            public ref class ChooserPoly
            {
            public:
                /**
                <summary>Creates a ChooserPoly object.</summary>

                <remarks>
                Creates a ChooserPoly object. The ChooserPoly instance models a plaintext with no non-zero coefficients. More
                precisely, the number of non-zero coefficients, and the upper bound on the absolute values of the coefficients of the
                modeled plaintext polynomial are both set to zero. The operation history is set to null, which indicates that the
                ChooserPoly object does not represent a ciphertext but only a plaintext.
                </remarks>
                */
                ChooserPoly();

                /**
                <summary>Creates a ChooserPoly object.</summary>

                <remarks>
                Creates a ChooserPoly object. The ChooserPoly instance models plaintext data with a given bound on the number of
                non-zero coefficients, and a given bound on the size of the absolute value of the coefficients (represented by
                <see cref="BigUInt"/>). This constructor sets the operation history to that of a freshly encrypted ciphertext.
                </remarks>
                <param name="maxCoeffCount">An upper bound on the number of non-zero coefficients in the modeled plaintext data</param>
                <param name="maxAbsValue">An upper bound on the absolute value of the coefficients in the modeled plaintext
                data</param>
                <exception cref="System::ArgumentNullException">if maxAbsValue is null</exception>
                <exception cref="System::ArgumentException">if maxCoeffCount is less than or equal to zero</exception>
                */
                ChooserPoly(int maxCoeffCount, BigUInt ^maxAbsValue);

                /**
                <summary>Creates a ChooserPoly object.</summary>

                <remarks>
                Creates a ChooserPoly object. The ChooserPoly instance models plaintext data with a given bound on the number of
                non-zero coefficients, and a given bound on the size of the absolute value of the coefficients (represented by 
                System::UInt64. This constructor sets the operation history to that of a freshly encrypted ciphertext.
                </remarks>
                <param name="maxCoeffCount">An upper bound on the number of non-zero coefficients in the modeled plaintext data</param>
                <param name="maxAbsValue">An upper bound on the absolute value of the coefficients in the modeled plaintext
                data</param>
                <exception cref="System::ArgumentException">if maxCoeffCount is less than or equal to zero</exception>
                */
                ChooserPoly(int maxCoeffCount, System::UInt64 maxAbsValue);

                /**
                <summary>Destroys the ChooserPoly.</summary>
                */
                ~ChooserPoly();

                /**
                <summary>Destroys the ChooserPoly.</summary>
                */
                !ChooserPoly();

                /**
                <summary>Creates a deep copy of a ChooserPoly.</summary>
                <remarks>
                Creates a deep copy of a ChooserPoly. The created ChooserPoly will model plaintext data with same size bounds as the
                original one, and contains a clone of the original operation history.
                </remarks>
                <param name="copy">The ChooserPoly to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                ChooserPoly(ChooserPoly ^copy);

                /**
                <summary>Overwrites the ChooserPoly with the value of the specified ChooserPoly.</summary>
                <remarks>
                Overwrites the ChooserPoly with the value of the specified ChooserPoly. This includes creating a deep copy of the
                operation history or the original one.
                </remarks>
                <param name="assign">The ChooserPoly whose value should be assigned to the current ChooserPoly</param>
                <exception cref="System::ArgumentNullException">if assign is null</exception>
                */
                void Set(ChooserPoly ^assign);

                /**
                <summary>Gets/Sets the upper bound on the number of non-zero coefficients in the plaintext modeled by the current
                instance of ChooserPoly.</summary>
                */
                property int MaxCoeffCount
                {
                    int get();

                    void set(int value);
                }

                /**
                <summary>Gets/sets the upper bound on the absolute value of coefficients (represented by <see cref="BigUInt"/>) of the
                plaintext modeled by the current instance of ChooserPoly.</summary>

                <exception cref="System::ArgumentNullException">if value is null</exception>
                */
                property BigUInt ^MaxAbsValue
                {
                    BigUInt ^get();

                    void set(BigUInt ^value);
                }

                /**
                <summary>Determines whether given encryption parameters are large enough to support operations in 
                the operation history of the current ChooserPoly.</summary>

                <remarks>
                <para>
                Determines whether given encryption parameters are large enough to support operations in the operation 
                history of the current ChooserPoly. A ChooserPoly produced as a result of some number of arithmetic
                operations (with <see cref="ChooserEvaluator"/>) contains information about bounds on the number of 
                non-zero coefficients in a corresponding plaintext polynomial and on the absolute values of the 
                coefficients. For decryption to work correctly, these bounds must be small enough to be supported
                by the encryption parameters. Additionally, the encryption parameters must be large enough to provide 
                enough invariant noise budget for the performed operations.
                </para>
                <para>
                The budgetGap parameter can be used to ensure that a certain amount of noise budget remains unused.
                </para>
                <para>
                The return value is true or false depending on whether given encryption parameters are large enough to support the
                operations in the operation history of the current ChooserPoly.
                </para>
                </remarks>
                <param name="parms">The encryption parameters</param>
                <param name="budgetGap">The amount of noise budget (bits) that should remain unused</param>
                <exception cref="System::ArgumentNullException">if parms is null</exception>
                <exception cref="System::InvalidOperationException">if the current operation history is null, i.e. the current ChooserPoly models a
                plaintext polynomial</exception>
                <exception cref="System::ArgumentException">if encryption parameters are not valid</exception>
                <exception cref="System::ArgumentException">if budgetGap is negative</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption
                parameters.</seealso>
                <seealso cref="Simulation">See Simulation for the class that handles the noise budget consumption estimates.</seealso>
                */
                bool TestParameters(EncryptionParameters ^parms, int budgetGap);

                /**
                <summary>Simulates noise budget consumption in the operation history of the current instance of 
                ChooserPoly.</summary>

                <remarks>
                Simulates noise budget consumption in the operation history of the current instance of ChooserPoly.
                The return value is a Simulation object.
                </remarks>
                <param name="parms">The encryption parameters</param>
                <exception cref="System::ArgumentNullException">if parms is null</exception>
                <exception cref="System::InvalidOperationException">if operation history is null, i.e. the current ChooserPoly models a
                plaintext polynomial</exception>
                <exception cref="System::ArgumentException">if encryption parameters are not valid</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for more details on valid encryption
                parameters.</seealso>
                <seealso cref="Simulation">See Simulation for the class that handles the noise budget consumption estimates.</seealso>
                */
                Simulation ^Simulate(EncryptionParameters ^parms);

                /**
                <summary>Sets the bounds on the degree and the absolute value of the coefficients of the modeled plaintext polynomial
                to zero, and sets the operation history to null.</summary>

                <remarks>
                <para>
                Sets the bounds on the degree and the absolute value of the coefficients of the modeled plaintext polynomial to zero,
                and sets the operation history to null.
                </para>
                <para>
                Note that a null operation history is not the same as that of a freshly encrypted ciphertext.
                </para>
                </remarks>
                <seealso cref="SetFresh()">See SetFresh() for setting the operation history to that of a freshly encrypted
                ciphertext.</seealso>
                */
                void Reset();

                /**
                <summary>Sets the operation history to that of a freshly encrypted ciphertext.</summary>
                <remarks>
                <para>
                Sets the operation history to that of a freshly encrypted ciphertext. This function leaves the bounds on the degree and
                the absolute value of the coefficients of the modeled plaintext polynomial unchanged.
                </para>
                </remarks>
                <seealso cref="MaxCoeffCount">See MaxCoeffCount to set the bound on the degree of the modeled plaintext polynomial.</seealso>
                <seealso cref="MaxAbsValue()">See MaxAbsValue to set the bound on the absolute value of coefficients of the modeled
                plaintext polynomial.</seealso>
                */
                void SetFresh();

            internal:
                /**
                <summary>Returns a reference to the underlying C++ ChooserPoly.</summary>
                */
                seal::ChooserPoly &GetChooserPoly();

                /**
                <summary>Creates a deep copy of a C++ ChooserPoly.</summary>
                <remarks>
                Creates a deep copy of a C++ ChooserPoly. The created ChooserPoly will model plaintext data with same size bounds as the
                original one, and contains a clone of the original operation history.
                </remarks>
                <param name="value">The ChooserPoly to copy from</param>
                */
                ChooserPoly(const seal::ChooserPoly &value);

            private:
                seal::ChooserPoly *chooserPoly_;
            };

            /**
            <summary>Models arithmetic operations on ChooserPoly objects rather than performing them on real data.</summary>
            <remarks>
            <para>
            Models arithmetic operations on <see cref="ChooserPoly"/> objects rather than performing them on real 
            data. The class ChooserEvaluator has a public API similar to that of Evaluator, making it easy for 
            the user to run existing code on <see cref="ChooserPoly"/> objects rather than on actual data. All of 
            these operations take as input a varying number of <see cref="ChooserPoly"/> objects and return a new 
            one with an appropriately extended operation history. This class is a part of the automatic parameter 
            selection module.
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
            <seealso cref="ChooserPoly">See ChooserPoly for the object modeling encrypted/plaintext data 
            for automatic parameter selection.</seealso>
            <seealso cref="ChooserEncoder">See ChooserEncoder for modeling the behavior of encoding with 
            ChooserPoly objects.</seealso>
            <seealso cref="ChooserEncryptor">See ChooserEncryptor for modeling the behavior of encryption
            with ChooserPoly objects.</seealso>
            <seealso cref="Simulation">See Simulation for the class that handles the noise budget consumption 
            estimates.</seealso>

            */
            public ref class ChooserEvaluator
            {
            public:
                /**
                <summary>Creates a ChooserEvaluator.</summary>
                */
                ChooserEvaluator();

                /**
                <summary>Creates a ChooserEvaluator.</summary>

                <remarks>
                Creates a ChooserEvaluator. The user can give a <see cref="MemoryPoolHandle "/> object 
                to use a custom memory pool instead of the global memory pool (default).
                </remarks>
                <param name="pool">The memory pool handle</param>
                <exception cref="System::ArgumentNullException">if pool is null</exception>
                */
                ChooserEvaluator(MemoryPoolHandle ^pool);

                /**
                <summary>Destroys the ChooserEvaluator.</summary>
                */
                ~ChooserEvaluator();

                /**
                <summary>Destroys the ChooserEvaluator.</summary>
                */
                !ChooserEvaluator();

                /**
                <summary>Performs an operation modeling Evaluator::MultiplyMany() on ChooserPoly objects.</summary>
                
                <remarks>
                Performs an operation modeling Evaluator::MultiplyMany() on <see cref="ChooserPoly"/> objects. This 
                operation creates a new <see cref="ChooserPoly"/> with updated bounds on the degree of the corresponding 
                plaintext polynomial and on the absolute values of the coefficients based on the inputs, and sets the 
                operation history accordingly.
                </remarks>
                <param name="operands">The list of ChooserPoly objects to multiply</param>
                <exception cref="System::ArgumentNullException">if operands list or any of its elements is null</exception>
                <exception cref="System::ArgumentException">if operands list is empty</exception>
                <exception cref="System::ArgumentException">if any of the ChooserPoly objects in operands list is not correctly
                initialized</exception>
                <seealso>See Evaluator::MultiplyMany() for the corresponding operation on ciphertexts.</seealso>
                <seealso>See SimulationEvaluator::MultiplyMany() for the corresponding operation on <see cref="Simulation"/> objects.</seealso>
                */
                ChooserPoly ^MultiplyMany(System::Collections::Generic::List<ChooserPoly^> ^operands);

                /**
                <summary>Performs an operation modeling Evaluator::Add() on ChooserPoly objects.</summary>
                
                <remarks>
                Performs an operation modeling Evaluator::Add() on <see cref="ChooserPoly"/> objects. This operation 
                creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial 
                and on the absolute values of the coefficients based on the inputs, and sets the operation history accordingly.
                </remarks>
                <param name="operand1">The first ChooserPoly object to add</param>
                <param name="operand2">The second ChooserPoly object to add</param>
                <exception cref="System::ArgumentNullException">if operand1 or operand2 is null</exception>
                <exception cref="System::ArgumentException">if either operand1 or operand2 is not correctly initialized</exception>
                <seealso>See Evaluator::Add() for the corresponding operation on ciphertexts.</seealso>
                <seealso>See SimulationEvaluator::Add() for the corresponding operation on <see cref="Simulation"/> objects.</seealso>
                */
                ChooserPoly ^Add(ChooserPoly ^operand1, ChooserPoly ^operand2);

                /**
                <summary>Performs an operation modeling Evaluator::AddMany() on ChooserPoly objects.</summary>
                
                <remarks>
                Performs an operation modeling Evaluator::AddMany() on <see cref="ChooserPoly"/> objects. This operation 
                creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial 
                and on the absolute values of the coefficients based on the inputs, and sets the operation history accordingly.
                </remarks>
                <param name="operands">The ChooserPoly object to add</param>
                <exception cref="System::ArgumentNullException">if operands or any of its elements is null</exception>
                <exception cref="System::ArgumentException">if any of the elements of operands is not correctly initialized</exception>
                <seealso>See Evaluator::AddMany() for the corresponding operation on ciphertexts.</seealso>
                <seealso>See SimulationEvaluator::AddMany() for the corresponding operation on Simulation objects.</seealso>
                */
                ChooserPoly ^AddMany(System::Collections::Generic::List<ChooserPoly^> ^operands);

                /**
                <summary>Performs an operation modeling Evaluator::Sub() on ChooserPoly objects.</summary>
                
                <remarks>
                Performs an operation modeling Evaluator::Sub() on <see cref="ChooserPoly"/> objects. This operation 
                creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial 
                and on the absolute values of the coefficients based on the inputs, and sets the operation history accordingly.
                </remarks>
                <param name="operand1">The ChooserPoly object to subtract from</param>
                <param name="operand2">The ChooserPoly object to subtract</param>
                <exception cref="System::ArgumentNullException">if operand1 or operand2 is null</exception>
                <exception cref="System::ArgumentException">if either operand1 or operand2 is not correctly initialized</exception>
                <seealso>See Evaluator::Sub() for the corresponding operation on ciphertexts.</seealso>
                <seealso>See SimulationEvaluator::Sub() for the corresponding operation on <see cref="Simulation"/> objects.</seealso>
                */
                ChooserPoly ^Sub(ChooserPoly ^operand1, ChooserPoly ^operand2);

                /**
                <summary>Performs an operation modeling Evaluator::Multiply() on ChooserPoly objects.</summary>
                
                <remarks>
                Performs an operation modeling Evaluator::Multiply() on <see cref="ChooserPoly"/> objects. This 
                operation creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext 
                polynomial and on the absolute values of the coefficients based on the inputs, and sets the operation 
                history accordingly.
                </remarks>
                <param name="operand1">The first ChooserPoly object to multiply</param>
                <param name="operand2">The second ChooserPoly object to multiply</param>
                <exception cref="System::ArgumentNullException">if operand1 or operand2 is null</exception>
                <exception cref="System::ArgumentException">if either operand1 or operand2 is not correctly initialized</exception>
                <seealso>See Evaluator::Multiply() for the corresponding operation on ciphertexts.</seealso>
                <seealso>See SimulationEvaluator::Multiply() for the corresponding operation on <see cref="Simulation"/> objects.</seealso>
                */
                ChooserPoly ^Multiply(ChooserPoly ^operand1, ChooserPoly ^operand2);

                /**
                <summary>Performs an operation modeling Evaluator::Square() on ChooserPoly objects.</summary>
                
                <remarks>
                Performs an operation modeling Evaluator::Square() on <see cref="ChooserPoly"/> objects. This
                operation creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext 
                polynomial and on the absolute values of the coefficients based on the inputs, and sets the operation 
                history accordingly.
                </remarks>
                <param name="operand">The ChooserPoly object to square</param>
                <exception cref="System::ArgumentNullException">if operand is null</exception>
                <exception cref="System::ArgumentException">if operand is not correctly initialized</exception>
                <seealso>See Evaluator::Square() for the corresponding operation on ciphertexts.</seealso>
                <seealso>See SimulationEvaluator::Square() for the corresponding operation on <see cref="Simulation"/> objects.</seealso>
                */
                ChooserPoly ^Square(ChooserPoly ^operand);

                /**
                <summary>Performs an operation modeling Evaluator::Relinearize() on ChooserPoly objects.</summary>
                
                <remarks>
                Performs an operation modeling Evaluator::Relinearize() on <see cref="ChooserPoly"/> objects. This 
                operation creates a new ChooserPoly with the same bounds on the degree of the corresponding plaintext 
                polynomial and on the absolute values of the coefficients as the input, but sets the operation history
                to include the relinearization operation.
                </remarks>
                <param name="operand">The ChooserPoly object to relinearize</param>
                <exception cref="System::ArgumentNullException">if operand is null</exception>
                <exception cref="System::ArgumentException">if operand is not correctly initialized</exception>
                <seealso>See Evaluator::Relinearize() for the corresponding operation on ciphertexts.</seealso>
                <seealso>See SimulationEvaluator::Relinearize() for the corresponding
                operation on <see cref="Simulation"/> objects.</seealso>
                */
                ChooserPoly ^Relinearize(ChooserPoly ^operand);

                /**
                <summary>Performs an operation modeling Evaluator::Relinearize() on ChooserPoly objects.</summary>
                
                <remarks>
                <para>
                Performs an operation modeling Evaluator::Relinearize() on <see cref="ChooserPoly"/> objects. This 
                operation creates a new ChooserPoly with the same bounds on the degree of the corresponding plaintext
                polynomial and on the absolute values of the coefficients as the input, but sets the operation history 
                to include the relinearization operation.
                </para>
                <para>
                The parameter destinationSize is not verified for correctness in this function. Instead,
                if an impossible value is given, simulating the noise growth in the returned ChooserPoly
                will throw an exception.
                </para>
                </remarks>
                <param name="operand">The ChooserPoly object to relinearize</param>
                <param name="destinationSize">The size of the output ciphertext that is being modeled</param>
                <exception cref="System::ArgumentNullException">if operand is null</exception>
                <exception cref="System::ArgumentException">if operand is not correctly initialized</exception>
                <seealso>See Evaluator::Relinearize() for the corresponding operation on ciphertexts.</seealso>
                <seealso>See SimulationEvaluator::Relinearize() for the corresponding
                operation on <see cref="Simulation"/> objects.</seealso>
                */
                ChooserPoly ^Relinearize(ChooserPoly ^operand, int destinationSize);

                /**
                <summary>Performs an operation modeling Evaluator::MultiplyPlain() on ChooserPoly objects.</summary>
                
                <remarks>
                Performs an operation modeling Evaluator::MultiplyPlain() on <see cref="ChooserPoly"/> objects. This 
                operation creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext 
                polynomial and on the absolute values of the coefficients based on the inputs, and sets the operation 
                history accordingly.
                </remarks>
                <param name="operand">The ChooserPoly object to multiply</param>
                <param name="plainMaxCoeffCount">Bound on the number of non-zero coefficients in the plaintext polynomial to multiply</param>
                <param name="plainMaxAbsValue">Bound on the absolute value of coefficients of the plaintext polynomial to multiply</param>
                <exception cref="System::ArgumentNullException">if operand or plainMaxAbsValue is null</exception>
                <exception cref="System::ArgumentException">if operand is not correctly initialized</exception>
                <exception cref="System::ArgumentException">if plainMaxCoeffCount is not positive</exception>
                <exception cref="System::ArgumentException">if plainMaxAbsValue is zero</exception>
                <seealso>See Evaluator::MultiplyPlain() for the corresponding operation on ciphertexts.</seealso>
                <seealso>See SimulationEvaluator::MultiplyPlain() for the corresponding operation on <see cref="Simulation"/> objects.</seealso>
                */
                ChooserPoly ^MultiplyPlain(ChooserPoly ^operand, int plainMaxCoeffCount, BigUInt ^plainMaxAbsValue);

                /**
                <summary>Performs an operation modeling Evaluator::MultiplyPlain() on ChooserPoly objects.</summary>
                
                <remarks>
                Performs an operation modeling Evaluator::MultiplyPlain() on <see cref="ChooserPoly"/> objects. This 
                operation creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext 
                polynomial and on the absolute values of the coefficients based on the inputs, and sets the operation 
                history accordingly.
                </remarks>
                <param name="operand">The ChooserPoly object to multiply</param>
                <param name="plainMaxCoeffCount">Bound on the number of non-zero coefficients in the plaintext polynomial to multiply</param>
                <param name="plainMaxAbsValue">Bound on the absolute value of coefficients of the plaintext polynomial to multiply</param>
                <exception cref="System::ArgumentNullException">if operand is null</exception>
                <exception cref="System::ArgumentException">if operand is not correctly initialized</exception>
                <exception cref="System::ArgumentException">if plainMaxCoeffCount is not positive</exception>
                <exception cref="System::ArgumentException">if plainMaxAbsValue is zero</exception>
                <seealso>See Evaluator::MultiplyPlain() for the corresponding operation on ciphertexts.</seealso>
                <seealso>See SimulationEvaluator::MultiplyPlain() for the corresponding operation on <see cref="Simulation"/> objects.</seealso>
                */
                ChooserPoly ^MultiplyPlain(ChooserPoly ^operand, int plainMaxCoeffCount, System::UInt64 plainMaxAbsValue);

                /**
                <summary>Performs an operation modeling Evaluator::MultiplyPlain() on ChooserPoly objects.</summary>
                
                <remarks>
                <para>
                Performs an operation modeling Evaluator::MultiplyPlain() on <see cref="ChooserPoly"/> objects. This 
                operation creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext 
                polynomial and on the absolute values of the coefficients based on the inputs, and sets the operation 
                history accordingly.
                </para>
                <para>
                This variant of the function takes the plaintext multiplier as input in the form of another ChooserPoly.
                If the plaintext multiplier is already known at the time of performing the automatic parameter selection,
                one can use <see cref="ChooserEncoder"/> to construct the appropriate ChooserPoly for plainChooserPoly. This function
                completely ignores the operation history possibly carried by plainChooserPoly.
                </para>
                </remarks>
                <param name="operand">The ChooserPoly object to multiply</param>
                <param name="plainChooserPoly">The plaintext polynomial to multiply represented by a ChooserPoly</param>
                <exception cref="System::ArgumentNullException">if operand or plainChooserPoly is null</exception>
                <exception cref="System::ArgumentException">if operand is not correctly initialized</exception>
                <exception cref="System::ArgumentException">if plainChooserPoly is not correctly initialized</exception>
                <exception cref="System::ArgumentException">if plainChooserPoly models a zero plaintext</exception>
                <seealso>See Evaluator::MultiplyPlain() for the corresponding operation on ciphertexts.</seealso>
                <seealso>See SimulationEvaluator::MultiplyPlain() for the corresponding operation on <see cref="Simulation"/> objects.</seealso>
                <seealso cref="ChooserEncoder">See ChooserEncoder for constructing a ChooserPoly representing the plaintext multiplier.</seealso>
                */
                ChooserPoly ^MultiplyPlain(ChooserPoly ^operand, ChooserPoly ^plainChooserPoly);

                /**
                <summary>Performs an operation modeling Evaluator::AddPlain() on ChooserPoly objects.</summary>
                
                <remarks>
                Performs an operation modeling Evaluator::AddPlain() on <see cref="ChooserPoly"/> objects. This operation 
                creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial 
                and on the absolute values of the coefficients based on the inputs, and sets the operation history accordingly.
                </remarks>
                <param name="operand">The ChooserPoly object to add</param>
                <param name="plainMaxCoeffCount">Bound on the number of non-zero coefficients in the plaintext polynomial to add</param>
                <param name="plainMaxAbsValue">Bound on the absolute value of coefficients of the plaintext polynomial to add</param>
                <exception cref="System::ArgumentNullException">if operand or plainMaxAbsValue is null</exception>
                <exception cref="System::ArgumentException">if operand is not correctly initialized</exception>
                <exception cref="System::ArgumentException">if plainMaxCoeffCount is not positive</exception>
                <seealso>See Evaluator::AddPlain() for the corresponding operation on ciphertexts.</seealso>
                <seealso>See SimulationEvaluator::AddPlain() for the corresponding operation on <see cref="Simulation"/> objects.</seealso>
                */
                ChooserPoly ^AddPlain(ChooserPoly ^operand, int plainMaxCoeffCount, BigUInt ^plainMaxAbsValue);

                /**
                <summary>Performs an operation modeling Evaluator::AddPlain() on ChooserPoly objects.</summary>
                
                <remarks>
                Performs an operation modeling Evaluator::AddPlain() on <see cref="ChooserPoly"/> objects. This operation 
                creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial 
                and on the absolute values of the coefficients based on the inputs, and sets the operation history accordingly.
                </remarks>
                <param name="operand">The ChooserPoly object to add</param>
                <param name="plainMaxCoeffCount">Bound on the number of non-zero coefficients in the plaintext polynomial to add</param>
                <param name="plainMaxAbsValue">Bound on the absolute value of coefficients of the plaintext polynomial to add</param>
                <exception cref="System::ArgumentNullException">if operand is null</exception>
                <exception cref="System::ArgumentException">if operand is not correctly initialized</exception>
                <exception cref="System::ArgumentException">if plainMaxCoeffCount is not positive</exception>
                <seealso>See Evaluator::AddPlain() for the corresponding operation on ciphertexts.</seealso>
                <seealso>See SimulationEvaluator::AddPlain() for the corresponding operation on <see cref="Simulation"/> objects.</seealso>
                */
                ChooserPoly ^AddPlain(ChooserPoly ^operand, int plainMaxCoeffCount, System::UInt64 plainMaxAbsValue);

                /**
                <summary>Performs an operation modeling Evaluator::AddPlain() on ChooserPoly objects.</summary>
                
                <remarks>
                <para>
                Performs an operation modeling Evaluator::AddPlain() on <see cref="ChooserPoly"/> objects. This operation
                creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial 
                and on the absolute values of the coefficients based on the inputs, and sets the operation history accordingly.
                </para>
                <para>
                This variant of the function takes the plaintext to add as input in the form of another ChooserPoly. If the plaintext
                to be added is already known at the time of performing the automatic parameter selection, one can use ChooserEncoder to
                construct the appropriate ChooserPoly for plainChooserPoly. This function completely ignores the operation history
                possibly carried by plainChooserPoly.
                </para>
                </remarks>
                <param name="operand">The ChooserPoly object to add</param>
                <param name="plainChooserPoly">The plaintext polynomial to add represented by a ChooserPoly</param>
                <exception cref="System::ArgumentNullException">if operand or plainChooserPoly is null</exception>
                <exception cref="System::ArgumentException">if operand is not correctly initialized</exception>
                <exception cref="System::ArgumentException">if plainChooserPoly is not correctly initialized</exception>
                <exception cref="System::ArgumentException">if plainChooserPoly has a non-positive bound on the number of non-zero coefficients</exception>
                <seealso>See Evaluator::AddPlain() for the corresponding operation on ciphertexts.</seealso>
                <seealso>See SimulationEvaluator::AddPlain() for the corresponding operation on Simulation objects.</seealso>
                <seealso cref="ChooserEncoder">See ChooserEncoder for constructing the ChooserPoly for the plaintext to add.</seealso>
                */
                ChooserPoly ^AddPlain(ChooserPoly ^operand, ChooserPoly ^plainChooserPoly);

                /**
                <summary>Performs an operation modeling Evaluator::SubPlain() on ChooserPoly objects.</summary>
                
                <remarks>
                Performs an operation modeling Evaluator::SubPlain() on <see cref="ChooserPoly"/> objects. This operation 
                creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial 
                and on the absolute values of the coefficients based on the inputs, and sets the operation history accordingly.
                </remarks>
                <param name="operand">The ChooserPoly object to subtract from</param>
                <param name="plainMaxCoeffCount">Bound on the number of non-zero coefficients in the plaintext polynomial to subtract</param>
                <param name="plainMaxAbsValue">Bound on the absolute value of coefficients of the plaintext polynomial to subtract</param>
                <exception cref="System::ArgumentNullException">if operand or plainMaxAbsValue is null</exception>
                <exception cref="System::ArgumentException">if operand is not correctly initialized</exception>
                <exception cref="System::ArgumentException">if plainMaxCoeffCount is not positive</exception>
                <seealso>See Evaluator::SubPlain() for the corresponding operation on ciphertexts.</seealso>
                <seealso>See SimulationEvaluator::SubPlain() for the corresponding operation on <see cref="Simulation"/> objects.</seealso>
                */
                ChooserPoly ^SubPlain(ChooserPoly ^operand, int plainMaxCoeffCount, BigUInt ^plainMaxAbsValue);

                /**
                <summary>Performs an operation modeling Evaluator::SubPlain() on ChooserPoly objects.</summary>
                
                <remarks>
                Performs an operation modeling Evaluator::SubPlain() on <see cref="ChooserPoly"/> objects. This operation 
                creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial 
                and on the absolute values of the coefficients based on the inputs, and sets the operation history accordingly.
                </remarks>
                <param name="operand">The ChooserPoly object to subtract from</param>
                <param name="plainMaxCoeffCount">Bound on the number of non-zero coefficients in the plaintext polynomial to subtract</param>
                <param name="plainMaxAbsValue">Bound on the absolute value of coefficients of the plaintext polynomial to subtract</param>
                <exception cref="System::ArgumentNullException">if operand is null</exception>
                <exception cref="System::ArgumentException">if operand is not correctly initialized</exception>
                <exception cref="System::ArgumentException">if plainMaxCoeffCount is not positive</exception>
                <seealso>See Evaluator::SubPlain() for the corresponding operation on ciphertexts.</seealso>
                <seealso>See SimulationEvaluator::SubPlain() for the corresponding operation on <see cref="Simulation"/> objects.</seealso>
                */
                ChooserPoly ^SubPlain(ChooserPoly ^operand, int plainMaxCoeffCount, System::UInt64 plainMaxAbsValue);

                /**
                <summary>Performs an operation modeling Evaluator::SubPlain() on ChooserPoly objects.</summary>
                
                <remarks>
                <para>
                Performs an operation modeling Evaluator::SubPlain() on <see cref="ChooserPoly"/> objects. This operation 
                creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
                and on the absolute values of the coefficients based on the inputs, and sets the operation history accordingly.
                </para>
                <para>
                This variant of the function takes the plaintext to subtract as input in the form of another ChooserPoly.
                If the plaintext to be subtracted is already known at the time of performing the automatic parameter 
                selection, one can use ChooserEncoder to construct the appropriate ChooserPoly for plainChooserPoly. 
                This function completely ignores the operation history possibly carried by plainChooserPoly.
                </para>
                </remarks>
                <param name="operand">The ChooserPoly object to subtract from</param>
                <param name="plainChooserPoly">The plaintext polynomial to subtract represented by a ChooserPoly</param>
                <exception cref="System::ArgumentNullException">if operand or plainChooserPoly is null</exception>
                <exception cref="System::ArgumentException">if operand is not correctly initialized</exception>
                <exception cref="System::ArgumentException">if plainChooserPoly is not correctly initialized</exception>
                <exception cref="System::ArgumentException">if plainChooserPoly has a non-positive bound on the number of non-zero coefficients</exception>
                <seealso>See Evaluator::SubPlain() for the corresponding operation on ciphertexts.</seealso>
                <seealso>See SimulationEvaluator::SubPlain() for the corresponding operation on Simulation objects.</seealso>
                <seealso cref="ChooserEncoder">See ChooserEncoder for constructing the ChooserPoly for the plaintext to subtract.</seealso>
                */
                ChooserPoly ^SubPlain(ChooserPoly ^operand, ChooserPoly ^plainChooserPoly);

                /**
                <summary>Performs an operation modeling Evaluator::Exponentiate() on ChooserPoly objects.</summary>
                
                <remarks>
                Performs an operation modeling Evaluator::Exponentiate() on <see cref="ChooserPoly"/> objects. This 
                operation creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext
                polynomial and on the absolute values of the coefficients based on the inputs, and sets the operation 
                history accordingly.
                </remarks>
                <param name="operand">The ChooserPoly object to raise to a power</param>
                <param name="exponent">The power to raise the ChooserPoly object to</param>
                <exception cref="System::ArgumentNullException">if operand is null</exception>
                <exception cref="System::ArgumentException">if operand is not correctly initialized</exception>
                <exception cref="System::ArgumentException">if exponent is zero</exception>
                <seealso>See Evaluator::Exponentiate() for the corresponding operation on ciphertexts.</seealso>
                <seealso>See SimulationEvaluator::Exponentiate() for the corresponding operation on <see cref="Simulation"/> objects.</seealso>
                */
                ChooserPoly ^Exponentiate(ChooserPoly ^operand, System::UInt64 exponent);

                /**
                <summary>Performs an operation modeling Evaluator::Negate() on ChooserPoly objects.</summary>
                
                <remarks>
                Performs an operation modeling Evaluator::Negate() on <see cref="ChooserPoly"/> objects. This operation 
                creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial 
                and on the absolute values of the coefficients based on the inputs, and sets the operation history 
                accordingly.
                </remarks>
                <param name="operand">The ChooserPoly object to negate</param>
                <exception cref="System::ArgumentNullException">if operand is null</exception>
                <exception cref="System::ArgumentException">if operand is not correctly initialized</exception>
                <seealso>See Evaluator::Negate() for the corresponding operation on ciphertexts.</seealso>
                <seealso>See SimulationEvaluator::Negate() for the corresponding operation on <see cref="Simulation"/> objects.</seealso>
                */
                ChooserPoly ^Negate(ChooserPoly ^operand);

                /**
                <summary>Provides the user with optimized encryption parameters that are large enough to support the operations
                performed on all of the given <see cref="ChooserPoly"/> objects.</summary>
                
                <remarks>
                <para>
                Provides the user with optimized encryption parameters that are large enough to support the operations
                performed on all of the given <see cref="ChooserPoly"/> objects. The function returns true or false 
                depending on whether a working parameter set was found or not.
                </para>
                <para>
                The budgetGap parameter can be used to ensure that a certain amount of noise budget remains unused.
                </para>
                </remarks>
                <param name="operands">The ChooserPolys for which the parameters are optimized</param>
                <param name="budgetGap">The amount of noise budget (bits) that should remain unused</param>
                <param name="destination">The encryption parameters to overwrite with the selected parameter set</param>
                <exception cref="System::ArgumentNullException">if operands list or any of its elements is null</exception>
                <exception cref="System::ArgumentNullException">if destination is null</exception>
                <exception cref="System::InvalidOperationException">if operation history of any of the given ChooserPolys is null</exception>
                <exception cref="System::ArgumentException">if operands is empty</exception>
                <exception cref="System::ArgumentException">if budgetGap is negative</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for a description of the encryption parameters.</seealso>
                <seealso cref="DefaultNoiseStandardDeviation()">See DefaultNoiseStandardDeviation() for the default noise standard deviation.</seealso>
                <seealso cref="DefaultNoiseMaxDeviation()">See DefaultNoiseMaxDeviation() for the default maximal noise deviation.</seealso>
                <seealso cref="DefaultParameterOptions()">See DefaultParameterOptions() for the default set of parameter options.</seealso>
                */
                bool SelectParameters(System::Collections::Generic::List<ChooserPoly^> ^operands, int budgetGap, EncryptionParameters ^destination);

                /**
                <summary>Provides the user with optimized encryption parameters that are large enough to support the
                operations performed on all of the given <see cref="ChooserPoly"/> objects.</summary>
                
                <remarks>
                <para>
                Provides the user with optimized encryption parameters that are large enough to support the
                operations performed on all of the given <see cref="ChooserPoly"/> objects. The standard deviation 
                of the noise distribution, the maximal deviation, and the list from which we choose the size of the
                polynomial modulus and the coefficient modulus are provided by the user as input parameters. 
                The function returns true or false depending on whether a working parameter set was found or not.
                </para>
                <para>
                The budgetGap parameter can be used to ensure that a certain amount of noise budget remains unused.
                </para>
                <para>
                The parameter options are given as a dictionary, where the sizes of the polynomial moduli are the
                keys, and the corresponding values are the coefficient moduli (represented by <see cref="BigUInt"/>).
                The sizes of the polynomial moduli must be at least 512 and powers of 2.
                </para>
                </remarks>
                <param name="operands">The ChooserPolys for which the parameters are optimized</param>
                <param name="budgetGap">The amount of noise budget (bits) that should remain unused</param>
                <param name="noiseStandardDeviation">The noise standard deviation</param>
                <param name="noiseMaxDeviation">The maximal noise deviation</param>
                <param name="parameterOptions">The parameter options to be used</param>
                <param name="destination">The encryption parameters to overwrite with the selected parameter set</param>
                <exception cref="System::ArgumentNullException">if operands list or any of its elements is null</exception>
                <exception cref="System::ArgumentNullException">if destination is null</exception>
                <exception cref="System::ArgumentNullException">if parameterOptions or any of its values is null</exception>
                <exception cref="System::InvalidOperationException">if operation history is null</exception>
                <exception cref="System::ArgumentException">if operands is empty</exception>
                <exception cref="System::ArgumentException">if budgetGap is negative</exception>
                <exception cref="System::ArgumentException">if noiseStandardDeviation is negative</exception>
                <exception cref="System::ArgumentException">if noiseMaxDeviation is negative</exception>
                <exception cref="System::ArgumentException">if parameterOptions is empty</exception>
                <exception cref="System::ArgumentException">if parameterOptions has keys that are less than 512 or not powers of 2</exception>
                <seealso cref="EncryptionParameters">See EncryptionParameters for a description of the encryption parameters.</seealso>
                <seealso cref="DefaultNoiseStandardDeviation()">See DefaultNoiseStandardDeviation() for the default noise standard deviation.</seealso>
                <seealso cref="DefaultNoiseMaxDeviation()">See DefaultNoiseMaxDeviation() for the default maximal noise deviation.</seealso>
                <seealso cref="DefaultParameterOptions()">See DefaultParameterOptions() for the default set of parameter options.</seealso>
                */
                bool SelectParameters(System::Collections::Generic::List<ChooserPoly^> ^operands, int budgetGap, double noiseStandardDeviation, double noiseMaxDeviation, System::Collections::Generic::Dictionary<int, BigUInt^> ^parameterOptions, EncryptionParameters ^destination);

                /**
                <summary>Returns the default set of (degree(polynomial modulus),coeffModulus)-pairs.</summary>
                
                <remarks>
                Returns the default set of (degree(polynomial modulus),coeffModulus)-pairs. The function returns a dictionary 
                of entries where the degree of the polynomial modulus acts as the key, and the corresponding modulus is the value.
                An expert user might want to give a modified dictionary as an argument to SelectParameters() for better results.
                </remarks>
                */
                static property System::Collections::Generic::Dictionary<int, BigUInt^> ^DefaultParameterOptions
                {
                    System::Collections::Generic::Dictionary<int, BigUInt^> ^get();
                }

                /**
                <summary>Returns the default value for the standard deviation of the noise (error) distribution.</summary>
                
                <remarks>
                Returns the default value for the standard deviation of the noise (error) distribution.
                An expert user might want to give a modified value as an argument to SelectParameters().
                </remarks>
                */
                static property double DefaultNoiseStandardDeviation
                {
                    double get();
                }

                /**
                <summary>Returns the default value for the maximal deviation of the noise (error) distribution.</summary>
                
                <remarks>
                Returns the default value for the maximal deviation of the noise (error) distribution.
                An expert user might want to give a modified value as an argument to SelectParameters().
                </remarks>
                */
                static property double DefaultNoiseMaxDeviation
                {
                    double get();
                }

            internal:
                /**
                <summary>Returns a reference to the underlying C++ ChooserEvaluator.</summary>
                */
                seal::ChooserEvaluator &GetEvaluator();

            private:
                seal::ChooserEvaluator *chooserEvaluator_;
            };

            /**
            <summary>Constructs ChooserPoly objects representing encoded plaintexts.</summary>
            <remarks>
            <para>
            Constructs <see cref="ChooserPoly"/> objects representing encoded plaintexts. ChooserPoly objects constructed in 
            this way have null operation history. They can be further used by <see cref="ChooserEncryptor"/>, or in the 
            functions ChooserEvaluator::MultiplyPlain(), ChooserEvaluator::AddPlain(), ChooserEvaluator::SubPlain() 
            representing plaintext operands. Only the integer encodings (those provided by <see cref="IntegerEncoder"/>) 
            are supported by ChooserEncoder. This class is a part of the automatic parameter selection module.
            </para>
            </remarks>
            <seealso cref="ChooserPoly">See ChooserPoly for the object modeling encrypted/plaintext data for automatic parameter
            selection.</seealso>
            <seealso cref="ChooserEvaluator">See ChooserEvaluator for manipulating instances of ChooserPoly.</seealso>
            <seealso cref="ChooserEncryptor">See ChooserEncryptor for modeling the behavior of encryption with ChooserPoly
            objects.</seealso>
            <seealso cref="IntegerEncoder">See IntegerEncoder for the corresponding encoder for real data.</seealso>
            <seealso cref="Simulation">See Simulation for the class that handles the noise budget consumption estimates.</seealso>
            */
            public ref class ChooserEncoder
            {
            public:
                /**
                <summary>Creates a ChooserEncoder that can be used to create ChooserPoly objects modeling plaintext polynomials encoded
                with <see cref="IntegerEncoder"/>, with base set to 2.</summary>
                */
                ChooserEncoder();

                /**
                <summary>Creates a ChooserEncoder that can be used to create ChooserPoly objects modeling plaintext polynomials encoded
                with IntegerEncoder with the base given as argument.</summary>
                <remarks>
                Creates a ChooserEncoder that can be used to create <see cref="ChooserPoly"/> objects modeling plaintext polynomials encoded with
                <see cref="IntegerEncoder"/>. The base is given by the user as a parameter, and it can be any integer at least 2.
                </remarks>
                <param name="base">The base to be used for encoding (default value is 2)</param>
                <exception cref="System::ArgumentException">if base is not an integer and at least 2</exception>
                */
                ChooserEncoder(System::UInt64 base);

                /**
                <summary>Destroys the ChooserEncoder.</summary>
                */
                ~ChooserEncoder();

                /**
                <summary>Destroys the ChooserEncoder.</summary>
                */
                !ChooserEncoder();

                /**
                <summary>Encodes a number (represented by System::UInt64) into a ChooserPoly object.</summary>
                <remarks>
                Encodes a number (represented by System::UInt64) into a <see cref="ChooserPoly"/> object. This is done by first encoding it with
                <see cref="IntegerEncoder"/> and then simply reading the number of coefficients and the maximal absolute value of the coefficients
                in the polynomial. In the returned ChooserPoly the computation history is set to null.
                </remarks>
                <param name="value">The non-negative integer to encode</param>
                <seealso>See IntegerEncoder::Encode() for the corresponding function returning a real polynomial.</seealso>
                */
                ChooserPoly ^Encode(System::UInt64 value);

                /**
                <summary>Encodes a number (represented by System::UInt64) into a ChooserPoly object given as an argument.</summary>
                <remarks>
                Encodes a number (represented by System::UInt64) into a <see cref="ChooserPoly"/> object given as an argument. This is done by first
                encoding it with <see cref="IntegerEncoder"/> and then simply reading the number of coefficients and the maximal absolute value of
                the coefficients in the polynomial. In the output ChooserPoly the operation history is set to null.
                </remarks>
                <param name="value">The non-negative integer to encode</param>
                <param name="destination">Reference to a ChooserPoly where the output will be stored</param>
                <seealso>See IntegerEncoder::Encode() for the corresponding function returning a real polynomial.</seealso>
                */
                void Encode(System::UInt64 value, ChooserPoly ^destination);

                /**
                <summary>Encodes a number (represented by System::Int64) into a ChooserPoly object.</summary>
                <remarks>
                Encodes a number (represented by System::Int64) into a <see cref="ChooserPoly"/> object. This is done by first encoding it with
                <see cref="IntegerEncoder"/> and then simply reading the number of coefficients and the maximal absolute value of the coefficients
                in the polynomial. In the returned ChooserPoly the computation history is set to null.
                </remarks>
                <param name="value">The integer to encode</param>
                <seealso>See IntegerEncoder::Encode() for the corresponding function returning a real polynomial.</seealso>
                */
                ChooserPoly ^Encode(System::Int64 value);

                /**
                <summary>Encodes a number (represented by System::Int64) into a ChooserPoly object given as an argument.</summary>
                <remarks>
                Encodes a number (represented by System::Int64) into a <see cref="ChooserPoly"/> object given as an argument. This is done by first
                encoding it with <see cref="IntegerEncoder"/> and then simply reading the number of coefficients and the maximal absolute value of
                the coefficients in the polynomial. In the output ChooserPoly the operation history is set to null.
                </remarks>
                <param name="value">The integer to encode</param>
                <param name="destination">Reference to a ChooserPoly where the output will be stored</param>
                <seealso>See IntegerEncoder::Encode() for the corresponding function returning a real polynomial.</seealso>
                */
                void Encode(System::Int64 value, ChooserPoly ^destination);

                /**
                <summary>Encodes a number (represented by BigUInt) into a ChooserPoly object.</summary>
                <remarks>
                Encodes a number (represented by <see cref="BigUInt"/>) into a <see cref="ChooserPoly"/> object. This is done by first encoding it with
                <see cref="IntegerEncoder"/> and then simply reading the number of coefficients and the maximal absolute value of the coefficients
                in the polynomial. In the returned ChooserPoly the computation history is set to null.
                </remarks>
                <param name="value">The non-negative integer to encode</param>
                <exception cref="System::ArgumentNullException">if value is null</exception>
                <seealso>See IntegerEncoder::Encode() for the corresponding function returning a real polynomial.</seealso>
                */
                ChooserPoly ^Encode(BigUInt ^value);

                /**
                <summary>Encodes a number (represented by BigUInt) into a ChooserPoly object given as an argument.</summary>
                <remarks>
                Encodes a number (represented by <see cref="BigUInt"/>) into a <see cref="ChooserPoly"/> object given as an argument. This is done by first
                encoding it with <see cref="IntegerEncoder"/> and then simply reading the number of coefficients and the maximal absolute value of
                the coefficients in the polynomial. In the output ChooserPoly the operation history is set to null.
                </remarks>
                <param name="value">The non-negative integer to encode</param>
                <param name="destination">Reference to a ChooserPoly where the output will be stored</param>
                <exception cref="System::ArgumentNullException">if value is null</exception>
                <seealso>See IntegerEncoder::Encode() for the corresponding function returning a real polynomial.</seealso>
                */
                void Encode(BigUInt ^value, ChooserPoly ^destination);

                /**
                <summary>Encodes a number (represented by System::Int32) into a ChooserPoly object.</summary>
                <remarks>
                Encodes a number (represented by System::Int32) into a <see cref="ChooserPoly"/> object. This is done by first encoding it with
                <see cref="IntegerEncoder"/> and then simply reading the number of coefficients and the maximal absolute value of the coefficients
                in the polynomial. In the returned ChooserPoly the computation history is set to null.
                </remarks>
                <param name="value">The integer to encode</param>
                <seealso>See IntegerEncoder::Encode() for the corresponding function returning a real polynomial.</seealso>
                */
                ChooserPoly ^Encode(System::Int32 value);

                /**
                <summary>Encodes a number (represented by System::UInt32) into a ChooserPoly object.</summary>
                <remarks>
                Encodes a number (represented by System::UInt32) into a <see cref="ChooserPoly"/> object. This is done by first encoding it with
                <see cref="IntegerEncoder"/> and then simply reading the number of coefficients and the maximal absolute value of the coefficients
                in the polynomial. In the returned ChooserPoly the computation history is set to null.
                </remarks>
                <param name="value">The non-negative integer to encode</param>
                <seealso>See IntegerEncoder::Encode() for the corresponding function returning a real polynomial.</seealso>
                */
                ChooserPoly ^Encode(System::UInt32 value);

                /**
                <summary>Returns the base used for encoding.</summary>
                */
                property System::UInt64 Base
                {
                    System::UInt64 get();
                }

            internal:
                /**
                <summary>Returns a reference to the underlying C++ ChooserEncoder.</summary>
                */
                seal::ChooserEncoder &GetEncoder();

            private:
                seal::ChooserEncoder *chooserEncoder_;
            };

            /**
            <summary>Manipulates ChooserPoly objects created by ChooserEncoder by setting their operation history to that of a
            freshly encrypted ciphertext.</summary>
            <remarks>
            Manipulates <see cref="ChooserPoly"/> objects created by <see cref="ChooserEncoder"/> by setting their operation history to that of a freshly
            encrypted ciphertext. This converts them from carriers of only information about the size of plaintext polynomials
            into carriers of also a model of inherent noise. After the ChooserPoly objects have been "encrypted" using
            ChooserEncryptor::Encrypt(), they can further be manipulated using the many functions of <see cref="ChooserEvaluator"/>. This class
            is a part of the automatic parameter selection module.
            </remarks>
            <seealso cref="ChooserPoly">See ChooserPoly for the object modeling encrypted/plaintext data for automatic parameter
            selection.</seealso>
            <seealso cref="ChooserEvaluator">See ChooserEvaluator for manipulating instances of ChooserPoly.</seealso>
            <seealso cref="ChooserEncoder">See ChooserEncoder for modeling the behavior of encoding with ChooserPoly
            objects.</seealso>
            <seealso>See Encryptor for the corresponding operations on real plaintext polynomials.</seealso>
            <seealso cref="Simulation">See Simulation for the class that handles the inherent noise growth estimates.</seealso>
            */
            public ref class ChooserEncryptor
            {
            public:
                /**
                <summary>Creates a ChooserEncryptor object.</summary>
                */
                ChooserEncryptor();

                /**
                <summary>Destroys the ChooserEncryptor.</summary>
                */
                ~ChooserEncryptor();

                /**
                <summary>Destroys the ChooserEncryptor.</summary>
                */
                !ChooserEncryptor();

                /**
                <summary>Overwrites the referenced <see cref="ChooserPoly"/> destination with the referenced ChooserPoly plain, and sets the
                operation history of destination to that of a freshly encrypted ciphertext.</summary>
                <param name="plain">The ChooserPoly modeling a plaintext polynomial (e.g. constructed with <see cref="ChooserEncoder"/>)</param>
                <param name="destination">The ChooserPoly to overwrite with the "encrypted" ChooserPoly</param>
                <exception cref="System::ArgumentNullException">if plain or destination is null</exception>
                <exception cref="System::ArgumentException">if plain has non-null operation history</exception>
                <seealso>See Encryptor::Encrypt() for the corresponding operation on real plaintext/ciphertext polynomials.</seealso>
                */
                void Encrypt(ChooserPoly ^plain, ChooserPoly ^destination);

                /**
                <summary>Makes a copy of a given <see cref="ChooserPoly"/>, but sets the operation history to that of a freshly encrypted ciphertext.</summary>
                <param name="plain">The ChooserPoly modeling a plaintext polynomial (e.g. constructed with <see cref="ChooserEncoder"/>)</param>
                <exception cref="System::ArgumentNullException">if plain is null</exception>
                <exception cref="System::ArgumentException">if plain has non-null operation history</exception>
                <seealso>See Encryptor::Encrypt() for the corresponding operation on real plaintext/ciphertext polynomials.</seealso>
                */
                ChooserPoly ^Encrypt(ChooserPoly ^plain);

                /**
                <summary>Overwrites the referenced ChooserPoly destination with the referenced ChooserPoly encrypted, and sets the
                operation history of destination to null.</summary>
                <remarks>
                Overwrites the referenced <see cref="ChooserPoly"/> destination with the referenced ChooserPoly encrypted, and sets the operation
                history of destination to null. This amounts to "decryption" in the sense that destination only carries information
                about the size of a plaintext polynomial.
                </remarks>
                <param name="encrypted">The ChooserPoly modeling a ciphertext polynomial (e.g. constructed with <see cref="ChooserEncoder"/>)</param>
                <param name="destination">The ChooserPoly to overwrite with the "decrypted" ChooserPoly</param>
                <exception cref="System::ArgumentNullException">if encrypted or destination is null</exception>
                <exception cref="System::ArgumentException">if encrypted has null operation history</exception>
                <seealso>See Decryptor::Decrypt() for the corresponding operation on real plaintext/ciphertext polynomials.</seealso>
                */
                void Decrypt(ChooserPoly ^encrypted, ChooserPoly ^destination);

                /**
                <summary>Returns a copy of a given ChooserPoly, but sets the operation history to null.</summary>
                <remarks>
                Returns a copy of a given <see cref="ChooserPoly"/>, but sets the operation history to null. This amounts to "decryption" in the
                sense that the output only carries information about the size of a plaintext polynomial.
                </remarks>
                <param name="encrypted">The ChooserPoly modeling a ciphertext polynomial (e.g. constructed with <see cref="ChooserEncoder"/>)</param>
                <exception cref="System::ArgumentNullException">if encrypted is null</exception>
                <exception cref="System::ArgumentException">if encrypted has null operation history</exception>
                <seealso>See Encryptor::Decrypt() for the corresponding operation on real plaintext/ciphertext polynomials.</seealso>
                */
                ChooserPoly ^Decrypt(ChooserPoly ^encrypted);

            internal:
                /**
                <summary>Returns a reference to the underlying C++ ChooserEncryptor.</summary>
                */
                seal::ChooserEncryptor &GetEncryptor();

            private:
                seal::ChooserEncryptor *chooserEncryptor_;
            };
        }
    }
}