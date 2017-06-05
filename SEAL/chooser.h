#pragma once

#include <map>
#include <utility>
#include "encryptionparams.h"
#include "simulator.h"
#include "encoder.h"
#include "util/computation.h"
#include "memorypoolhandle.h"

namespace seal
{
    /**
    Models ciphertexts for the automatic parameter selection module. Choosing appropriate and secure
    parameters for homomorphic cryptosystems is difficult, and beyond what a non-expert user should have
    to worry about. The user programs their computations as they normally would, but replace plaintexts with
    ChooserPoly objects. When the program is executed, ChooserPoly objects store the computation as a directed
    acyclic graph, and can later be used to estimate the invariant noise budget consumption in the stored 
    computation. The estimated noise budget in the output ciphertext is given by the simulate() function. 
    This class is a part of the automatic parameter selection module.

    Each instance of ChooserPoly stores an estimate of the number of non-zero coefficients, and an estimate
    for their largest absolute value (represented by BigUInt) that a plaintext polynomial can be expected to
    contain after some number of homomorphic operations have been performed on it. In addition, each ChooserPoly
    stores a directed acyclic graph of computations that it has gone through (operation history).

    Instances of ChooserPoly can be manipulated using an instance of ChooserEvaluator, which has a public API
    similar to Evaluator, making existing code easy to run on ChooserPoly objects instead of running it on
    actual encrypted data. In other words, using ChooserEvaluator, ChooserPoly objects can be added, multiplied,
    subtracted, negated, etc., and the result is always a new ChooserPoly object whose estimated noise budget
    can be obtained using the simulate() function, which uses heuristic worst-case analysis of the noise 
    behavior in the encryption scheme. 

    @par Invariant Noise Budget
    The invariant noise polynomial of a ciphertext is a rational coefficient polynomial, such that
    a ciphertext decrypts correctly as long as the coefficients of the invariant noise polynomial are
    of absolute value less than 1/2. Thus, we call the infinity-norm of the invariant noise polynomial
    the invariant noise, and for correct decryption require it to be less than 1/2. If v denotes the
    invariant noise, we define the invariant noise budget as -log2(2v). Thus, the invariant noise budget
    starts from some initial value, which depends on the encryption parameters, and decreases to 0 when
    computations are performed. When the budget reaches 0, the ciphertext becomes too noisy to decrypt
    correctly.

    @par Thread Safety
    In general, reading a ChooserPoly is thread-safe while mutating is not. Specifically, concurrent access must
    be avoided if the size bounds of the modeled plaintext data, or the operation history are expected to be changed.

    @see ChooserEvaluator for manipulating instances of ChooserPoly.
    @see ChooserEncoder for modeling the behavior of encoding with ChooserPoly objects.
    @see ChooserEncryptor for modeling the behavior of encryption with ChooserPoly objects.
    @see Simulation for the class that handles the noise budget consumption estimates.
    */
    class ChooserPoly
    {
    public:
        /**
        Creates a ChooserPoly object. The ChooserPoly instance models a plaintext with no non-zero coefficients.
        More precisely, the number of non-zero coefficients, and the upper bound on the absolute values of the
        coefficients of the modeled plaintext polynomial are both set to zero. The operation history is set to null,
        which indicates that the ChooserPoly object does not represent a ciphertext but only a plaintext.
        */
        ChooserPoly();

        /**
        Creates a ChooserPoly object. The ChooserPoly instance models plaintext data with a given bound on
        the number of non-zero coefficients, and a given bound on the size of the absolute value of the
        coefficients (represented by BigUInt). This constructor sets the operation history to that of
        a freshly encrypted ciphertext.

        @param[in] max_coeff_count An upper bound on the number of non-zero coefficients in the modeled plaintext data
        @param[in] max_abs_value An upper bound on the absolute value of the coefficients in the modeled plaintext data
        @throws std::invalid_argument if max_coeff_count is less than or equal to zero
        */
        ChooserPoly(int max_coeff_count, const BigUInt &max_abs_value);

        /**
        Creates a ChooserPoly object. The ChooserPoly instance models plaintext data with a given bound on
        the number of non-zero coefficients, and a given bound on the size of the absolute value of the
        coefficients (represented by std::uint64_t). This constructor sets the operation history to that of
        a freshly encrypted ciphertext.

        @param[in] max_coeff_count An upper bound on the number of non-zero coefficients in the modeled plaintext data
        @param[in] max_abs_value An upper bound on the absolute value of the coefficients in the modeled plaintext data
        @throws std::invalid_argument if max_coeff_count is less than or equal to zero
        */
        ChooserPoly(int max_coeff_count, std::uint64_t max_abs_value);

        /**
        Destroys the ChooserPoly, and deallocates all memory associated with it.
        */
        ~ChooserPoly();

        /**
        Creates a deep copy of a ChooserPoly. The created ChooserPoly will model plaintext data with same
        size bounds as the original one, and contains a clone of the original operation history.

        @param[in] copy The ChooserPoly to copy from
        */
        ChooserPoly(const ChooserPoly &copy);

        /**
        Overwrites the ChooserPoly with the value of the specified ChooserPoly. This includes creating
        a deep copy of the operation history or the original one.

        @param[in] assign The ChooserPoly whose value should be assigned to the current ChooserPoly
        */
        ChooserPoly &operator =(const ChooserPoly &assign);

        /**
        Returns the upper bound on the number of non-zero coefficients in the plaintext modeled by the current instance
        of ChooserPoly.
        */
        int max_coeff_count() const
        {
            return max_coeff_count_;
        }

        /**
        Returns a reference to the upper bound on the absolute value of coefficients (represented by BigUInt) 
        of the plaintext modeled by the current instance of ChooserPoly.
        */
        const BigUInt &max_abs_value() const
        {
            return max_abs_value_;
        }

        /**
        Returns a reference to the upper bound on the number of non-zero coefficients in the plaintext modeled
        by the current instance of ChooserPoly.
        */
        int &max_coeff_count()
        {
            return max_coeff_count_;
        }

        /**
        Returns a reference to the upper bound on the absolute value of coefficients (represented by BigUInt)
        of the plaintext modeled by the current instance of ChooserPoly.
        */
        BigUInt &max_abs_value()
        {
            return max_abs_value_;
        }

        /**
        Determines whether given encryption parameters are large enough to support operations in
        the operation history of the current ChooserPoly. A ChooserPoly produced as a result of some
        number of arithmetic operations (with ChooserEvaluator) contains information about
        bounds on the number of non-zero coefficients in a corresponding plaintext polynomial and on the
        absolute values of the coefficients. For decryption to work correctly, these bounds must be
        small enough to be supported by the encryption parameters. Additionally, the encryption parameters
        must be large enough to provide enough invariant noise budget for the performed operations.

        The budget_gap parameter can be used to ensure that a certain amount of noise budget remains unused.
        
        The return value is true or false depending on whether given encryption parameters are large enough
        to support the operations in the operation history of the current ChooserPoly.

        @param[in] parms The encryption parameters
        @param[in] budget_gap The amount of noise budget (bits) that should remain unused
        @throws std::logic_error if the current operation history is null, i.e. the current ChooserPoly models a plaintext polynomial
        @throws std::invalid_argument if encryption parameters are not valid
        @throws std::invalid_argument if budget_gap is negative
        @see EncryptionParameters for more details on valid encryption parameters.
        @see Simulation for the class that handles the noise budget consumption estimates.
        */
        bool test_parameters(EncryptionParameters &parms, int budget_gap) const
        {
            return (parms.plain_modulus().significant_bit_count() >= max_abs_value_.significant_bit_count() &&
                parms.poly_modulus().significant_coeff_count() > max_coeff_count_ && simulate(parms).decrypts(budget_gap));
        }

        /**
        Simulates noise budget consumption in the operation history of the current instance of ChooserPoly.
        The return value is a Simulation object.

        @param[in] parms The encryption parameters
        @throws std::logic_error if operation history is null, i.e. the current ChooserPoly models a plaintext polynomial
        @throws std::invalid_argument if encryption parameters are not valid
        @see EncryptionParameters for more details on valid encryption parameters.
        @see Simulation for the class that handles the noise budget consumption estimates.
        */
        Simulation simulate(EncryptionParameters &parms) const;

        /**
        Sets the bounds on the degree and the absolute value of the coefficients of the modeled plaintext
        polynomial to zero, and sets the operation history to null.

        @warning A null operation history is not the same as that of a freshly encrypted ciphertext.
        @see set_fresh() for setting the operation history to that of a freshly encrypted ciphertext.
        */
        void reset();

        /**
        Sets the operation history to that of a freshly encrypted ciphertext. This function leaves the bounds on
        the degree and the absolute value of the coefficients of the modeled plaintext polynomial unchanged.

        @see max_coeff_count() to set the bound on the degree of the modeled plaintext polynomial.
        @see max_abs_value() to set the bound on the absolute value of coefficients of the modeled plaintext polynomial.
        */
        void set_fresh();

    private:
        int max_coeff_count_;

        BigUInt max_abs_value_;

        seal::util::Computation *comp_;

        ChooserPoly(int max_coeff_count, const BigUInt &max_abs_value, seal::util::Computation *comp);

        ChooserPoly(int max_coeff_count, std::uint64_t max_abs_value, seal::util::Computation *comp);

        const seal::util::Computation *comp() const
        {
            return comp_;
        }

        friend class ChooserEvaluator;

        friend class ChooserEncryptor;
    };

    /**
    Models arithmetic operations on ChooserPoly objects rather than performing them on real data.
    The class ChooserEvaluator has a public API similar to that of Evaluator,
    making it easy for the user to run existing code on ChooserPoly objects rather than on actual data.
    All of these operations take as input a varying number of ChooserPoly objects and return a new one
    with an appropriately extended operation history. This class is a part of the automatic parameter
    selection module.

    @par Invariant Noise Budget
    The invariant noise polynomial of a ciphertext is a rational coefficient polynomial, such that
    a ciphertext decrypts correctly as long as the coefficients of the invariant noise polynomial are
    of absolute value less than 1/2. Thus, we call the infinity-norm of the invariant noise polynomial
    the invariant noise, and for correct decryption require it to be less than 1/2. If v denotes the
    invariant noise, we define the invariant noise budget as -log2(2v). Thus, the invariant noise budget
    starts from some initial value, which depends on the encryption parameters, and decreases to 0 when
    computations are performed. When the budget reaches 0, the ciphertext becomes too noisy to decrypt
    correctly.

    @see ChooserPoly for the object modeling encrypted/plaintext data for automatic parameter selection.
    @see ChooserEncoder for modeling the behavior of encoding with ChooserPoly objects.
    @see ChooserEncryptor for modeling the behavior of encryption with ChooserPoly objects.
    @see Simulation for the class that handles the noise budget consumption estimates.
    */
    class ChooserEvaluator
    {
    public:
        /**
        Creates a ChooserEvaluator.
        */
        ChooserEvaluator(const MemoryPoolHandle &pool = MemoryPoolHandle::acquire_global()) : pool_(pool)
        {
        }

        /**
        Performs an operation modeling Evaluator::multiply_many() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of  the coefficients based on the inputs, and sets the operation history
        accordingly.

        @param[in] operands The vector of ChooserPoly objects to multiply
        @throws std::invalid_argument if operands vector is empty
        @throws std::invalid_argument if any of the ChooserPoly objects in operands vector is not correctly initialized
        @see Evaluator::multiply_many() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::multiply_many() for the corresponding operation on Simulation objects.
        */
        ChooserPoly multiply_many(const std::vector<ChooserPoly> &operands);

        /**
        Performs an operation modeling Evaluator::add() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the operation history
        accordingly.

        @param[in] operand1 The first ChooserPoly object to add
        @param[in] operand2 The second ChooserPoly object to add
        @throws std::invalid_argument if either operand1 or operand2 is not correctly initialized
        @see Evaluator::add() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::add() for the corresponding operation on Simulation objects.
        */
        ChooserPoly add(const ChooserPoly &operand1, const ChooserPoly &operand2);

        /**
        Performs an operation modeling Evaluator::add_many() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the operation history
        accordingly.

        @param[in] operands The ChooserPoly object to add
        @throws std::invalid_argument if any of the elements of operands is not correctly initialized
        @see Evaluator::add_many() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::add_many() for the corresponding operation on Simulation objects.
        */
        ChooserPoly add_many(const std::vector<ChooserPoly> &operands);

        /**
        Performs an operation modeling Evaluator::sub() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the operation history
        accordingly.

        @param[in] operand1 The ChooserPoly object to subtract from
        @param[in] operand2 The ChooserPoly object to subtract
        @throws std::invalid_argument if either operand1 or operand2 is not correctly initialized
        @see Evaluator::sub() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::sub() for the corresponding operation on Simulation objects.
        */
        ChooserPoly sub(const ChooserPoly &operand1, const ChooserPoly &operand2);

        /**
        Performs an operation modeling Evaluator::multiply() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the operation history
        accordingly.

        @param[in] operand1 The first ChooserPoly object to multiply
        @param[in] operand2 The second ChooserPoly object to multiply
        @throws std::invalid_argument if either operand1 or operand2 is not correctly initialized
        @see Evaluator::multiply() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::multiply() for the corresponding operation on Simulation objects.
        */
        ChooserPoly multiply(const ChooserPoly &operand1, const ChooserPoly &operand2);

        /**
        Performs an operation modeling Evaluator::square() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the operation history
        accordingly.

        @param[in] operand The ChooserPoly object to square
        @throws std::invalid_argument if operand is not correctly initialized
        @see Evaluator::square() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::square() for the corresponding operation on Simulation objects.
        */
        ChooserPoly square(const ChooserPoly &operand);

        /**
        Performs an operation modeling Evaluator::relinearize() on ChooserPoly objects. This operation
        creates a new ChooserPoly with the same bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients as the input, but sets the operation history
        to include the relinearization operation.

        The parameter destination_size is not verified for correctness in this function. Instead,
        if an impossible value is given, simulating the noise growth in the returned ChooserPoly
        will throw an exception.

        @param[in] operand The ChooserPoly object to relinearize
        @param[in] destination_size The size of the output ciphertext that is being modeled (defaults to 2)
        @throws std::invalid_argument if operand is not correctly initialized
        @see Evaluator::relinearize() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::relinearize() for the corresponding operation on Simulation objects.
        */
        ChooserPoly relinearize(const ChooserPoly &operand, int destination_size = 2);

        /**
        Performs an operation modeling Evaluator::multiply_plain() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the operation history
        accordingly.

        @param[in] operand The ChooserPoly object to multiply
        @param[in] plain_max_coeff_count Bound on the number of non-zero coefficients in the plaintext polynomial to multiply
        @param[in] plain_max_abs_value Bound on the absolute value of coefficients of the plaintext polynomial to multiply
        @throws std::invalid_argument if operand is not correctly initialized
        @throws std::invalid_argument if plain_max_coeff_count is not positive
        @throws std::invalid_argument if plain_max_abs_value is zero
        @see Evaluator::multiply_plain() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::multiply_plain() for the corresponding operation on Simulation objects.
        */
        ChooserPoly multiply_plain(const ChooserPoly &operand, int plain_max_coeff_count, const BigUInt &plain_max_abs_value);

        /**
        Performs an operation modeling Evaluator::multiply_plain() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the operation history
        accordingly.

        @param[in] operand The ChooserPoly object to multiply
        @param[in] plain_max_coeff_count Bound on the number of non-zero coefficients in the plaintext polynomial to multiply
        @param[in] plain_max_abs_value Bound on the absolute value of coefficients of the plaintext polynomial to multiply
        @throws std::invalid_argument if operand is not correctly initialized
        @throws std::invalid_argument if plain_max_coeff_count is not positive
        @throws std::invalid_argument if plain_max_abs_value is zero
        @see Evaluator::multiply_plain() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::multiply_plain() for the corresponding operation on Simulation objects.
        */
        ChooserPoly multiply_plain(const ChooserPoly &operand, int plain_max_coeff_count, std::uint64_t plain_max_abs_value);

        /**
        Performs an operation modeling Evaluator::multiply_plain() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the operation history
        accordingly.

        This variant of the function takes the plaintext multiplier as input in the form of another ChooserPoly.
        If the plaintext multiplier is already known at the time of performing the automatic parameter selection,
        one can use ChooserEncoder to construct the appropriate ChooserPoly for plain_chooser_poly. This function
        completely ignores the operation history possibly carried by plain_chooser_poly.

        @param[in] operand The ChooserPoly object to multiply
        @param[in] plain_chooser_poly The plaintext polynomial to multiply represented by a ChooserPoly
        @throws std::invalid_argument if operand is not correctly initialized
        @throws std::invalid_argument if plain_chooser_poly is not correctly initialized
        @throws std::invalid_argument if plain_chooser_poly models a zero plaintext
        @see Evaluator::multiply_plain() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::multiply_plain() for the corresponding operation on Simulation objects.
        @see ChooserEncoder for constructing a ChooserPoly representing the plaintext multiplier.
        */
        ChooserPoly multiply_plain(const ChooserPoly &operand, const ChooserPoly &plain_chooser_poly);

        /**
        Performs an operation modeling Evaluator::add_plain() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the operation history
        accordingly.

        @param[in] operand The ChooserPoly object to add
        @param[in] plain_max_coeff_count Bound on the number of non-zero coefficients in the plaintext polynomial to add
        @param[in] plain_max_abs_value Bound on the absolute value of coefficients of the plaintext polynomial to add
        @throws std::invalid_argument if operand is not correctly initialized
        @throws std::invalid_argument if plain_max_coeff_count is not positive
        @see Evaluator::add_plain() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::add_plain() for the corresponding operation on Simulation objects.
        */
        ChooserPoly add_plain(const ChooserPoly &operand, int plain_max_coeff_count, const BigUInt &plain_max_abs_value);

        /**
        Performs an operation modeling Evaluator::add_plain() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the operation history
        accordingly.

        @param[in] operand The ChooserPoly object to add
        @param[in] plain_max_coeff_count Bound on the number of non-zero coefficients in the plaintext polynomial to add
        @param[in] plain_max_abs_value Bound on the absolute value of coefficients of the plaintext polynomial to add
        @throws std::invalid_argument if operand is not correctly initialized
        @throws std::invalid_argument if plain_max_coeff_count is not positive
        @see Evaluator::add_plain() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::add_plain() for the corresponding operation on Simulation objects.
        */
        ChooserPoly add_plain(const ChooserPoly &operand, int plain_max_coeff_count, std::uint64_t plain_max_abs_value);

        /**
        Performs an operation modeling Evaluator::add_plain() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the operation history
        accordingly.

        This variant of the function takes the plaintext to add as input in the form of another ChooserPoly.
        If the plaintext to be added is already known at the time of performing the automatic parameter selection,
        one can use ChooserEncoder to construct the appropriate ChooserPoly for plain_chooser_poly. This function
        completely ignores the operation history possibly carried by plain_chooser_poly.

        @param[in] operand The ChooserPoly object to add
        @param[in] plain_chooser_poly The plaintext polynomial to add represented by a ChooserPoly
        @throws std::invalid_argument if operand is not correctly initialized
        @throws std::invalid_argument if plain_chooser_poly is not correctly initialized
        @throws std::invalid_argument if plain_chooser_poly has a non-positive bound on the number of non-zero coefficients
        @see Evaluator::add_plain() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::add_plain() for the corresponding operation on Simulation objects.
        @see ChooserEncoder for constructing the ChooserPoly for the plaintext to add.
        */
        ChooserPoly add_plain(const ChooserPoly &operand, const ChooserPoly &plain_chooser_poly);

        /**
        Performs an operation modeling Evaluator::sub_plain() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the operation history
        accordingly.

        @param[in] operand The ChooserPoly object to subtract from
        @param[in] plain_max_coeff_count Bound on the number of non-zero coefficients in the plaintext polynomial to subtract
        @param[in] plain_max_abs_value Bound on the absolute value of coefficients of the plaintext polynomial to subtract
        @throws std::invalid_argument if operand is not correctly initialized
        @throws std::invalid_argument if plain_max_coeff_count is not positive
        @see Evaluator::sub_plain() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::sub_plain() for the corresponding operation on Simulation objects.
        */
        ChooserPoly sub_plain(const ChooserPoly &operand, int plain_max_coeff_count, const BigUInt &plain_max_abs_value);

        /**
        Performs an operation modeling Evaluator::sub_plain() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the operation history
        accordingly.

        @param[in] operand The ChooserPoly object to subtract from
        @param[in] plain_max_coeff_count Bound on the number of non-zero coefficients in the plaintext polynomial to subtract
        @param[in] plain_max_abs_value Bound on the absolute value of coefficients of the plaintext polynomial to subtract
        @throws std::invalid_argument if operand is not correctly initialized
        @throws std::invalid_argument if plain_max_coeff_count is not positive
        @see Evaluator::sub_plain() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::sub_plain() for the corresponding operation on Simulation objects.
        */
        ChooserPoly sub_plain(const ChooserPoly &operand, int plain_max_coeff_count, std::uint64_t plain_max_abs_value);

        /**
        Performs an operation modeling Evaluator::sub_plain() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the operation history
        accordingly.

        This variant of the function takes the plaintext to subtract as input in the form of another ChooserPoly.
        If the plaintext to be subtracted is already known at the time of performing the automatic parameter selection,
        one can use ChooserEncoder to construct the appropriate ChooserPoly for plain_chooser_poly. This function
        completely ignores the operation history possibly carried by plain_chooser_poly.

        @param[in] operand The ChooserPoly object to subtract from
        @param[in] plain_chooser_poly The plaintext polynomial to subtract represented by a ChooserPoly
        @throws std::invalid_argument if operand is not correctly initialized
        @throws std::invalid_argument if plain_chooser_poly is not correctly initialized
        @throws std::invalid_argument if plain_chooser_poly has a non-positive bound on the number of non-zero coefficients
        @see Evaluator::sub_plain() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator:sub_plain() for the corresponding operation on Simulation objects.
        @see ChooserEncoder for constructing the ChooserPoly for the plaintext to subtract.
        */
        ChooserPoly sub_plain(const ChooserPoly &operand, const ChooserPoly &plain_chooser_poly);

        /**
        Performs an operation modeling Evaluator::exponentiate() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the operation history
        accordingly.

        @param[in] operand The ChooserPoly object to raise to a power
        @param[in] exponent The power to raise the ChooserPoly object to
        @throws std::invalid_argument if operand is not correctly initialized
        @throws std::invalid_argument if exponent is zero
        @see Evaluator::exponentiate() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::exponentiate() for the corresponding operation on Simulation objects.
        */
        ChooserPoly exponentiate(const ChooserPoly &operand, std::uint64_t exponent);

        /**
        Performs an operation modeling Evaluator::negate() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the operation history
        accordingly.

        @param[in] operand The ChooserPoly object to negate
        @throws std::invalid_argument if operand is not correctly initialized
        @see Evaluator::negate() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::negate() for the corresponding operation on Simulation objects.
        */
        ChooserPoly negate(const ChooserPoly &operand);

        /**
        Provides the user with optimized encryption parameters that are large enough to support the
        operations performed on all of the given ChooserPoly objects. The function returns true or
        false depending on whether a working parameter set was found or not.

        The budget_gap parameter can be used to ensure that a certain amount of noise budget remains unused.

        @param[in] operands The ChooserPolys for which the parameters are optimized
        @param[in] budget_gap The amount of noise budget (bits) that should remain unused
        @param[out] destination The encryption parameters to overwrite with the selected parameter set
        @throws std::logic_error if operation history of any of the given ChooserPolys is null
        @throws std::invalid_argument if operands is empty
        @throws std::invalid_argument if budget_gap is negative
        @see EncryptionParameters for a description of the encryption parameters.
        @see default_noise_standard_deviation() for the default noise standard deviation.
        @see default_noise_max_deviation() for the default maximal noise deviation.
        @see default_parameter_options() for the default set of parameter options.
        */
        bool select_parameters(const std::vector<ChooserPoly> &operands, int budget_gap, EncryptionParameters &destination);

        /**
        Provides the user with optimized encryption parameters that are large enough to support the
        operations performed on all of the given ChooserPoly objects. The standard deviation of the 
        noise distribution, the maximal deviation, and the list from which we choose the size of the 
        polynomial modulus and the coefficient modulus are provided by the user as input parameters. 
        The function returns true or false depending on whether a working parameter set was found or not.

        The budget_gap parameter can be used to ensure that a certain amount of noise budget remains unused.

        The parameter options are given as an std::map<int, BigUInt>, where the sizes of the polynomial moduli
        are the keys, and the corresponding values are the coefficient moduli (represented by BigUInt).
        The sizes of the polynomial moduli must be at least 512 and powers of 2.

        @param[in] operands The ChooserPolys for which the parameters are optimized
        @param[in] budget_gap The amount of noise budget (bits) that should remain unused
        @param[in] noise_standard_deviation The noise standard deviation
        @param[in] parameter_options The parameter options to be used
        @param[out] destination The encryption parameters to overwrite with the selected parameter set
        @throws std::logic_error if operation history is null
        @throws std::invalid_argument if operands is empty
        @throws std::invalid_argument if budget_gap is negative
        @throws std::invalid_argument if noise_standard_deviation is negative
        @throws std::invalid_argument if noise_max_deviation is negative
        @throws std::invalid_argument if parameter_options is empty
        @throws std::invalid_argument if parameter_options has keys that are less than 512 or not powers of 2
        @see EncryptionParameters for a description of the encryption parameters.
        @see default_noise_standard_deviation() for the default noise standard deviation.
        @see default_noise_max_deviation() for the default maximal noise deviation.
        @see default_parameter_options() for the default set of parameter options.
        */
        bool select_parameters(const std::vector<ChooserPoly> &operands, int budget_gap, double noise_standard_deviation, double noise_max_deviation, const std::map<int, BigUInt> &parameter_options, EncryptionParameters &destination);

        /**
        Returns the default set of (degree(polynomial modulus),coeff_modulus)-pairs.
        The functions returns an std::map<int,BigUInt>, where the degree of the polynomial modulus acts as the key,
        and the corresponding modulus is the value.
        An expert user might want to give a modified map as an argument to select_parameters() for better results.
        */
        static const std::map<int, BigUInt> &default_parameter_options()
        {
            return default_parameter_options_;
        }

        /**
        Returns the default value for the standard deviation of the noise (error) distribution. 
        An expert user might want to give a modified value as an argument to select_parameters().
        */
        static double default_noise_standard_deviation()
        {
            return default_noise_standard_deviation_;
        }

        /**
        Returns the default value for the maximal deviation of the noise (error) distribution.
        An expert user might want to give a modified value as an argument to select_parameters().
        */
        static double default_noise_max_deviation()
        {
            return default_noise_max_deviation_;
        }

    private:
        ChooserEvaluator &operator =(const ChooserEvaluator &assign) = delete;

        ChooserEvaluator &operator =(ChooserEvaluator &&assign) = delete;

        MemoryPoolHandle pool_;

        static const std::map<int, BigUInt> default_parameter_options_;

        static const double default_noise_standard_deviation_;

        static const double default_noise_max_deviation_;
    };

    /**
    Constructs ChooserPoly objects representing encoded plaintexts. ChooserPoly objects constructed
    in this way have null operation history. They can be further used by ChooserEncryptor,
    or in the functions ChooserEvaluator::multiply_plain(), ChooserEvaluator::add_plain(), 
    ChooserEvaluator::sub_plain() representing plaintext operands. Only the integer encodings
    (those provided by IntegerEncoder) are supported by ChooserEncoder. This class is a part of the
    automatic parameter selection module.

    @see ChooserPoly for the object modeling encrypted/plaintext data for automatic parameter selection.
    @see ChooserEvaluator for manipulating instances of ChooserPoly.
    @see ChooserEncryptor for modeling the behavior of encryption with ChooserPoly objects.
    @see IntegerEncoder for the corresponding encoder for real data.
    @see Simulation for the class that handles the noise budget consumption estimates.
    */
    class ChooserEncoder
    {
    public:
        /**
        Creates a ChooserEncoder that can be used to create ChooserPoly objects modeling plaintext
        polynomials encoded with IntegerEncoder. The base will default to 2, but any integer at least 2
        can be used.

        @param[in] base The base to be used for encoding (default value is 2)
        @throws std::invalid_argument if base is not an integer and at least 2
        */
        ChooserEncoder(std::uint64_t base = 2);

        /**
        Encodes a number (represented by std::uint64_t) into a ChooserPoly object. This is done by first
        encoding it with IntegerEncoder and then simply reading the number of coefficients and the maximal
        absolute value of the coefficients in the polynomial. In the returned ChooserPoly the computation
        history is set to null.

        @param[in] value The non-negative integer to encode
        @see IntegerEncoder::encode() for the corresponding function returning a real polynomial.
        */
        ChooserPoly encode(std::uint64_t value);
        
        /**
        Encodes a number (represented by std::uint64_t) into a ChooserPoly object given as an argument.
        This is done by first encoding it with IntegerEncoder and then simply reading the number of
        coefficients and the maximal absolute value of the coefficients in the polynomial. In the output
        ChooserPoly the operation history is set to null.

        @param[in] value The non-negative integer to encode
        @param[out] destination Reference to a ChooserPoly where the output will be stored
        @see IntegerEncoder::encode() for the corresponding function returning a real polynomial.
        */
        void encode(std::uint64_t value, ChooserPoly &destination);

        /**
        Encodes a number (represented by std::int64_t) into a ChooserPoly object. This is done by first
        encoding it with IntegerEncoder and then simply reading the number of coefficients and the maximal
        absolute value of the coefficients in the polynomial. In the returned ChooserPoly the computation
        history is set to null.

        @param[in] value The integer to encode
        @see IntegerEncoder::encode() for the corresponding function returning a real polynomial.
        */
        ChooserPoly encode(std::int64_t value);

        /**
        Encodes a number (represented by std::int64_t) into a ChooserPoly object given as an argument.
        This is done by first encoding it with IntegerEncoder and then simply reading the number of
        coefficients and the maximal absolute value of the coefficients in the polynomial. In the output
        ChooserPoly the operation history is set to null.

        @param[in] value The integer to encode
        @param[out] destination Reference to a ChooserPoly where the output will be stored
        @see IntegerEncoder::encode() for the corresponding function returning a real polynomial.
        */
        void encode(std::int64_t value, ChooserPoly &destination);

        /**
        Encodes a number (represented by BigUInt) into a ChooserPoly object. This is done by first
        encoding it with IntegerEncoder and then simply reading the number of coefficients and the maximal
        absolute value of the coefficients in the polynomial. In the returned ChooserPoly the computation
        history is set to null.

        @param[in] value The non-negative integer to encode
        @see IntegerEncoder::encode() for the corresponding function returning a real polynomial.
        */
        ChooserPoly encode(BigUInt value);

        /**
        Encodes a number (represented by BigUInt) into a ChooserPoly object given as an argument.
        This is done by first encoding it with IntegerEncoder and then simply reading the number of
        coefficients and the maximal absolute value of the coefficients in the polynomial. In the output
        ChooserPoly the operation history is set to null.

        @param[in] value The non-negative integer to encode
        @param[out] destination Reference to a ChooserPoly where the output will be stored
        @see IntegerEncoder::encode() for the corresponding function returning a real polynomial.
        */
        void encode(BigUInt value, ChooserPoly &destination);

        /**
        Encodes a number (represented by std::int32_t) into a ChooserPoly object. This is done by first
        encoding it with IntegerEncoder and then simply reading the number of coefficients and the maximal
        absolute value of the coefficients in the polynomial. In the returned ChooserPoly the computation
        history is set to null.

        @param[in] value The integer to encode
        @see IntegerEncoder::encode() for the corresponding function returning a real polynomial.
        */
        ChooserPoly encode(std::int32_t value)
        {
            return encode(static_cast<std::int64_t>(value));
        }

        /**
        Encodes a number (represented by std::uint32_t) into a ChooserPoly object. This is done by first
        encoding it with IntegerEncoder and then simply reading the number of coefficients and the maximal
        absolute value of the coefficients in the polynomial. In the returned ChooserPoly the computation
        history is set to null.

        @param[in] value The non-negative integer to encode
        @see IntegerEncoder::encode() for the corresponding function returning a real polynomial.
        */
        ChooserPoly encode(std::uint32_t value)
        {
            return encode(static_cast<std::uint64_t>(value));
        }

        /**
        Returns the base used for encoding.
        */
        std::uint64_t base() const
        {
            return encoder_.base();
        }

    private:
        ChooserEncoder &operator =(const ChooserEncoder &assign) = delete;

        ChooserEncoder &operator =(ChooserEncoder &&assign) = delete;

        IntegerEncoder encoder_;
    };

    /**
    Manipulates ChooserPoly objects created by ChooserEncoder by setting their operation history to that
    of a freshly encrypted ciphertext. This converts them from carriers of only information about
    the size of plaintext polynomials into carriers of also a model of inherent noise. After the
    ChooserPoly objects have been "encrypted" using ChooserEncryptor::encrypt(), they can further be
    manipulated using the many functions of ChooserEvaluator. This class is a part of the automatic
    parameter selection module.

    @see ChooserPoly for the object modeling encrypted/plaintext data for automatic parameter selection.
    @see ChooserEvaluator for manipulating instances of ChooserPoly.
    @see ChooserEncoder for modeling the behavior of encoding with ChooserPoly objects.
    @see Encryptor for the corresponding operations on real plaintext polynomials.
    @see Simulation for the class that handles the inherent noise growth estimates.
    */
    class ChooserEncryptor
    {
    public:
        /**
        Creates a ChooserEncryptor object.
        */
        ChooserEncryptor()
        {
        }

        /**
        Overwrites the referenced ChooserPoly destination with the referenced ChooserPoly plain,
        and sets the operation history of destination to that of a freshly encrypted ciphertext.

        @param[in] plain The ChooserPoly modeling a plaintext polynomial (e.g. constructed with ChooserEncoder)
        @param[out] destination The ChooserPoly to overwrite with the "encrypted" ChooserPoly
        @throws std::invalid_argument if plain has non-null operation history
        @see Encryptor::encrypt() for the corresponding operation on real plaintext/ciphertext polynomials.
        */
        void encrypt(const ChooserPoly &plain, ChooserPoly &destination) const;

        /**
        Makes a copy of a given ChooserPoly, but sets the operation history to that of a freshly encrypted ciphertext.

        @param[in] plain The ChooserPoly modeling a plaintext polynomial (e.g. constructed with ChooserEncoder)
        @throws std::invalid_argument if plain has non-null operation history
        @see Encryptor::encrypt() for the corresponding operation on real plaintext/ciphertext polynomials.
        */
        ChooserPoly encrypt(const ChooserPoly &plain) const;

        /**
        Overwrites the referenced ChooserPoly destination with the referenced ChooserPoly encrypted,
        and sets the operation history of destination to null. This amounts to "decryption" in the sense that
        destination only carries information about the size of a plaintext polynomial.

        @param[in] encrypted The ChooserPoly modeling a ciphertext polynomial (e.g. constructed with ChooserEncoder)
        @param[out] destination The ChooserPoly to overwrite with the "decrypted" ChooserPoly
        @throws std::invalid_argument if encrypted has null operation history
        @see Decryptor::decrypt() for the corresponding operation on real plaintext/ciphertext polynomials.
        */
        void decrypt(const ChooserPoly &encrypted, ChooserPoly &destination) const;

        /**
        Returns a copy of a given ChooserPoly, but sets the operation history to null.
        This amounts to "decryption" in the sense that the output only carries information
        about the size of a plaintext polynomial.

        @param[in] encrypted The ChooserPoly modeling a ciphertext polynomial (e.g. constructed with ChooserEncoder)
        @throws std::invalid_argument if encrypted has null operation history
        @see Encryptor::decrypt() for the corresponding operation on real plaintext/ciphertext polynomials.
        */
        ChooserPoly decrypt(const ChooserPoly &encrypted) const;

    private:
        ChooserEncryptor &operator =(const ChooserEncryptor &assign) = delete;

        ChooserEncryptor &operator =(ChooserEncryptor &&assign) = delete;
    };
}