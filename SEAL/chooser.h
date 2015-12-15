#ifndef SEAL_CHOOSER_H
#define SEAL_CHOOSER_H

#include <map>
#include "encryptionparams.h"
#include "simulator.h"
#include "encoder.h"
#include "util/computation.h"
#include "util/mempool.h"

namespace seal
{
    /**
    Models ciphertexts for the automatic parameter selection module. Choosing appropriate and secure
    parameters for homomorphic cryptosystems is difficult, and beyond what a non-expert user should have
    to worry about. The user programs their computations as they normally would, but replace plaintexts with
    ChooserPoly objects. When the program is executed, ChooserPoly objects store the computation as a directed
    acyclic graph, and can later be used to estimate the inherent noise growth in the stored computation.
    The estimated inherent noise in the output ciphertext is given by the simulate() function. This class is
    a part of the automatic parameter selection module.

    Each instance of ChooserPoly stores an estimate of the number of non-zero coefficients, and an estimate
    for their largest absolute value (represented by BigUInt) that a plaintext polynomial can be expected to
    contain after some number of homomorphic operations have been performed on it. In addition, each ChooserPoly
    stores a directed acyclic graph of computations that it has gone through (operation history).

    Instances of ChooserPoly can be manipulated using an instance of ChooserEvaluator, which has a public API
    similar to Evaluator, making existing code easy to run on ChooserPoly objects instead of running it on
    actual encrypted data. In other words, using ChooserEvaluator, ChooserPoly objects can be added, multiplied,
    subtracted, negated, etc., and the result is always a new ChooserPoly object whose estimated inherent noise
    can be obtained using the simulate() function, which uses average-case analysis of the noise behavior in the
    encryption scheme. 

    @par Inherent Noise
    Technically speaking, the inherent noise of a ciphertext is a polynomial, but the condition for decryption working
    depends on the size of the largest absolute value of its coefficients. It is really the size of this
    largest absolute value that Simulation is simulating, and that we will call the "noise", the "inherent noise",
    or the "error", in this documentation. The reader is referred to the description of the encryption scheme for more details.

    @par Thread Safety
    In general, reading a ChooserPoly is thread-safe while mutating is not. Specifically, concurrent access must
    be avoided if the size bounds of the modeled plaintext data, or the operation history are expected to be changed.

    @see ChooserEvaluator for manipulating instances of ChooserPoly.
    @see ChooserEncoder for modeling the behavior of encoding with ChooserPoly objects.
    @see ChooserEncryptor for modeling the behavior of encryption with ChooserPoly objects.
    @see Simulation for the class that handles the inherent noise growth estimates.
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
        ChooserPoly(int max_coeff_count, uint64_t max_abs_value);

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

        @warning This function is not thread-safe.
        */
        int &max_coeff_count()
        {
            return max_coeff_count_;
        }

        /**
        Returns a reference to the upper bound on the absolute value of coefficients (represented by BigUInt)
        of the plaintext modeled by the current instance of ChooserPoly.

        @warning This function is not thread-safe.
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
        must be large enough to support inherent noise growth in the performed operations.
        
        The return value is true or false depending on whether given encryption parameters are large enough
        to support the operations in the operation history of the current ChooserPoly.

        @param[in] parms The encryption parameters
        @throws std::logic_error if the current operation history is null, i.e. the current ChooserPoly models a plaintext polynomial
        @throws std::invalid_argument if encryption parameters are not valid
        @see EncryptionParameters for more details on valid encryption parameters.
        @see Simulation for more details on the inherent noise growth simulation.
        */
        bool test_parameters(const EncryptionParameters &parms) const
        {
            return (parms.plain_modulus().significant_bit_count() >= max_abs_value_.significant_bit_count() &&
                parms.poly_modulus().significant_coeff_count() > max_coeff_count_ && simulate(parms).decrypts());
        }

        /**
        Simulates inherent noise growth in the operation history of the current instance of ChooserPoly.
        The return value is a Simulation object.

        @param[in] parms The encryption parameters
        @throws std::logic_error if operation history is null, i.e. the current ChooserPoly models a plaintext polynomial
        @throws std::invalid_argument if encryption parameters are not valid
        @see EncryptionParameters for more details on valid encryption parameters.
        @see Simulation for more details on the inherent noise growth simulation.
        */
        Simulation simulate(const EncryptionParameters &parms) const;

        /**
        Sets the bounds on the degree and the absolute value of the coefficients of the modeled plaintext
        polynomial to zero, and sets the operation history to null.

        @warning This function is not thread-safe.
        @warning A null operation history is not the same as that of a freshly encrypted ciphertext.
        @see set_fresh() for setting the operation history to that of a freshly encrypted ciphertext.
        */
        void reset();

        /**
        Sets the operation history to that of a freshly encrypted ciphertext. This function leaves the bounds on
        the degree and the absolute value of the coefficients of the modeled plaintext polynomial unchanged.

        @warning This function is not thread-safe.
        @see max_coeff_count() to set the bound on the degree of the modeled plaintext polynomial.
        @see max_abs_value() to set the bound on the absolute value of coefficients of the modeled plaintext polynomial.
        */
        void set_fresh();

        /**
        Returns the estimated value of inherent noise (represented by BigUInt) in a ciphertext which is a result of
        the operations in the operation history of the current ChooserPoly.

        @param[in] parms The encryption parameters
        @throws std::logic_error if operation history is null, i.e. the current ChooserPoly models a plaintext polynomial
        @throws std::invalid_argument if encryption parameters are not valid
        @see EncryptionParameters for more details on valid encryption parameters.
        @see simulate() to return the full Simulation object instead.
        */
        BigUInt noise(const EncryptionParameters &parms) const
        {
            return simulate(parms).noise();
        }

        /**
        Returns the maximal value of inherent noise (represented by BigUInt) supported by the given encryption parameters.
        The encryption parameters are not large enough if the value returned by noise() is larger than the value
        returned by max_noise().

        @param[in] parms The encryption parameters
        @throws std::logic_error if operation history is null, i.e. the current ChooserPoly models a plaintext polynomial
        @throws std::invalid_argument if encryption parameters are not valid
        @see EncryptionParameters for more details on valid encryption parameters.
        */
        BigUInt max_noise(const EncryptionParameters &parms) const
        {
            return Simulation(parms).max_noise();
        }

        /**
        Returns the bit length of the estimated value of inherent noise (represented by BigUInt) in a ciphertext on
        which the operations in the operation history of the current ChooserPoly have been performed.

        @param[in] parms The encryption parameters
        @throws std::logic_error if operation history is null, i.e. the current ChooserPoly models a plaintext polynomial
        @throws std::invalid_argument if encryption parameters are not valid
        @see EncryptionParameters for more details on valid encryption parameters.
        @see noise() to return the estimated value of inherent noise as a BigUInt instead.
        @see simulate() to return the full Simulation object instead.
        */
        int noise_bits(const EncryptionParameters &parms) const
        {
            return simulate(parms).noise_bits();
        }

        /**
        Returns the difference between the bit lengths of the return values of max_noise() and of noise(). This gives
        the user a convenient tool for estimating how many, if any, arithmetic operations can still be performed on the
        encrypted data before it becomes too noisy to be decrypted. If the return value is negative, the encryption
        parameters used are not large enough to support the performed arithmetic operations.

        @param[in] parms The encryption parameters
        @throws std::logic_error if operation history is null, i.e. the current ChooserPoly models a plaintext polynomial
        @throws std::invalid_argument if encryption parameters are not valid
        @see EncryptionParameters for more details on valid encryption parameters.
        @see noise() to return the estimated value of inherent noise as a BigUInt instead.
        @see simulate() to return the full Simulation object instead.
        */
        int noise_bits_left(const EncryptionParameters &parms) const
        {
            return simulate(parms).noise_bits_left();
        }

        /**
        Returns true or false depending on whether the encryption parameters were large enough to support inherent
        noise growth in the performed arithmetic operations.

        @param[in] parms The encryption parameters
        @throws std::logic_error if operation history is null, i.e. the current ChooserPoly models a plaintext polynomial
        @throws std::invalid_argument if encryption parameters are not valid
        @see EncryptionParameters for more details on valid encryption parameters.
        @see simulate() to return the full Simulation object instead.
        */
        bool decrypts(const EncryptionParameters &parms) const
        {
            return simulate(parms).decrypts();
        }

    private:
        int max_coeff_count_;

        BigUInt max_abs_value_;

        seal::util::Computation *comp_;

        ChooserPoly(int max_coeff_count, const BigUInt &max_abs_value, seal::util::Computation *comp);

        ChooserPoly(int max_coeff_count, uint64_t max_abs_value, seal::util::Computation *comp);

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

    @par Inherent Noise
    Technically speaking, the inherent noise of a ciphertext is a polynomial, but the condition for decryption working
    depends on the size of the largest absolute value of its coefficients. It is really the size of this
    largest absolute value that Simulation is simulating, and that we will call the "noise", the "inherent noise",
    or the "error", in this documentation. The reader is referred to the description of the encryption scheme for more details.

    @par Thread Safety
    The ChooserEvaluator class is not thread-safe and a separate ChooserEvaluator instance is needed for each
    potentially concurrent usage.

    @see ChooserPoly for the object modeling encrypted/plaintext data for automatic parameter selection.
    @see ChooserEncoder for modeling the behavior of encoding with ChooserPoly objects.
    @see ChooserEncryptor for modeling the behavior of encryption with ChooserPoly objects.
    @see Simulation for the class that handles the inherent noise growth estimates.
    */
    class ChooserEvaluator
    {
    public:
        /**
        Creates a ChooserEvaluator.
        */
        ChooserEvaluator()
        {
        }

        /**
        Performs an operation modeling Evaluator::multiply_many() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the operation history
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
        Performs an operation modeling Evaluator::multiply_norelin() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the operation history
        accordingly.

        @par THIS FUNCTION IS BROKEN
        This function is broken and does not work correctly, except when used together with relinearize().
        See Simulator::multiply_norelin() for more details.

        @param[in] operand1 The first ChooserPoly object to multiply
        @param[in] operand2 The second ChooserPoly object to multiply
        @throws std::invalid_argument if either operand1 or operand2 is not correctly initialized
        @see Evaluator::multiply_norelin() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::multiply_norelin() for the corresponding operation on Simulation objects.
        */
        //ChooserPoly multiply_norelin(const ChooserPoly &operand1, const ChooserPoly &operand2);

        /**
        Performs an operation modeling Evaluator::relinearize() on ChooserPoly objects. This operation
        creates a new ChooserPoly with the same bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients as the input, but sets the operation history
        to include the relinearization operation.

        @par THIS FUNCTION IS BROKEN
        This function is broken and does not work correctly, except when used together with multiply_norelin().
        See Simulator::relinearize() for more details.

        @param[in] operand The ChooserPoly object to relinearize
        @throws std::invalid_argument if operand is not correctly initialized
        @see Evaluator::relinearize() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::relinearize() for the corresponding operation on Simulation objects.
        */
        //ChooserPoly relinearize(const ChooserPoly &operand);

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
        @see Evaluator::multiply_plain() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::multiply_plain() for the corresponding operation on Simulation objects.
        */
        ChooserPoly multiply_plain(const ChooserPoly &operand, int plain_max_coeff_count, uint64_t plain_max_abs_value);

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
        @throws std::invalid_argument if plain_chooser_poly has a non-positive bound on the number of non-zero coefficients
        @see Evaluator::multiply_plain() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::multiply_plain() for the corresponding operation on Simulation objects.
        @see ChooserEncoder for constructing a ChooserPoly representing the plaintext multiplier.
        */
        ChooserPoly multiply_plain(const ChooserPoly &operand, const ChooserPoly &plain_chooser_poly)
        {
            return multiply_plain(operand, plain_chooser_poly.max_coeff_count_, plain_chooser_poly.max_abs_value_);
        }

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
        ChooserPoly add_plain(const ChooserPoly &operand, int plain_max_coeff_count, const BigUInt &plain_max_abs_value) const;

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
        ChooserPoly add_plain(const ChooserPoly &operand, int plain_max_coeff_count, uint64_t plain_max_abs_value);

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
        @throws std::invalid_argument if plain_chooser_poly has a non-positive bound on the number of non-zero coefficients
        @see Evaluator::add_plain() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::add_plain() for the corresponding operation on Simulation objects.
        @see ChooserEncoder for constructing the ChooserPoly for the plaintext to add.
        */
        ChooserPoly add_plain(const ChooserPoly &operand, const ChooserPoly &plain_chooser_poly)
        {
            return add_plain(operand, plain_chooser_poly.max_coeff_count_, plain_chooser_poly.max_abs_value_);
        }

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
        ChooserPoly sub_plain(const ChooserPoly &operand, int plain_max_coeff_count, uint64_t plain_max_abs_value);

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
        @throws std::invalid_argument if plain_chooser_poly has a non-positive bound on the number of non-zero coefficients
        @see Evaluator::sub_plain() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator:sub_plain() for the corresponding operation on Simulation objects.
        @see ChooserEncoder for constructing the ChooserPoly for the plaintext to subtract.
        */
        ChooserPoly sub_plain(const ChooserPoly &operand, const ChooserPoly &plain_chooser_poly)
        {
            return sub_plain(operand, plain_chooser_poly.max_coeff_count_, plain_chooser_poly.max_abs_value_);
        }

        /**
        Performs an operation modeling Evaluator::exponentiate() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the operation history
        accordingly.

        @param[in] operand The ChooserPoly object to raise to a power
        @param[in] exponent The non-negative power to raise the ChooserPoly object to
        @throws std::invalid_argument if operand is not correctly initialized
        @throws std::invalid_argument if the exponent is negative
        @throws std::invalid_argument if operand models a zero polynomial and exponent is zero
        @see Evaluator::exponentiate() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::exponentiate() for the corresponding operation on Simulation objects.
        */
        ChooserPoly exponentiate(const ChooserPoly &operand, int exponent);

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
        operations performed on the given ChooserPoly. Some choices are made in the process that
        an expert user might want to change: (1) We use a constant small standard deviation for the noise distribution;
        (2) We choose the size of the polynomial modulus and the coefficient modulus from a hard-coded list of
        choices we consider secure, and estimate to have at least 80 bits of security against a 2^(-80) advantage
        for solving decision-LWE (see http://eprint.iacr.org/2014/062.pdf, Table 2).

        The function returns true or false depending on whether a working parameter set was found or not.

        @param[in] operand The ChooserPoly for which the parameters are optimized
        @param[out] destination The encryption parameters to overwrite with the selected parameter set
        @throws std::logic_error if operation history of the given ChooserPoly is null
        @see EncryptionParameters for a description of the encryption parameters.
        @see default_noise_standard_deviation() for the default noise standard deviation.
        @see default_parameter_options() for the default set of parameter options.
        */
        bool select_parameters(const ChooserPoly &operand, EncryptionParameters &destination);

        /**
        Provides the user with optimized encryption parameters that are large enough to support the
        operations performed on all of the given ChooserPolys. Some choices are made in the process that
        an expert user might want to change: (1) We use a constant small standard deviation for the noise distribution;
        (2) We choose the size of the polynomial modulus and the coefficient modulus from a hard-coded list of
        choices we consider secure, and estimate to have at least 80 bits of security against a 2^(-80) advantage
        for solving decision-LWE (see http://eprint.iacr.org/2014/062.pdf, Table 2).

        The function returns true or false depending on whether a working parameter set was found or not.

        @param[in] operands The ChooserPolys for which the parameters are optimized
        @param[out] destination The encryption parameters to overwrite with the selected parameter set
        @throws std::logic_error if operation history of any of the given ChooserPolys is null
        @throws std::invalid_argument if operands is empty
        @see EncryptionParameters for a description of the encryption parameters.
        @see default_noise_standard_deviation() for the default noise standard deviation.
        @see default_parameter_options() for the default set of parameter options.
        */
        bool select_parameters(const std::vector<ChooserPoly> &operands, EncryptionParameters &destination);

        /**
        Provides the user with optimized encryption parameters that are large enough to support the
        operations performed on the given ChooserPoly. Both the standard deviation of the noise distribution
        and the list from which we choose the size of the polynomial modulus and the coefficient modulus
        are provided by the user as input parameters.

        The parameter options are given as an std::map<int, BigUInt>, where the sizes of the polynomial moduli
        are the keys, and the corresponding values are the coefficient moduli (represented by BigUInt).
        The sizes of the polynomial moduli must be at least 512 and powers of 2.

        The function returns true or false depending on whether a working parameter set was found or not.

        @param[in] operand The ChooserPoly for which the parameters are optimized
        @param[in] noise_standard_deviation The noise standard deviation
        @param[in] parameter_options The parameter options to be used
        @param[out] destination The encryption parameters to overwrite with the selected parameter set
        @throws std::logic_error if operation history is null
        @throws std::invalid_argument if noise_standard_deviation is negative
        @throws std::invalid_argument if parameter_options is empty
        @throws std::invalid_argument if parameter_options has keys that are less than 512 or not powers of 2
        @see EncryptionParameters for a description of the encryption parameters.
        @see ChooserEvaluator::default_noise_standard_deviation() for the default noise standard deviation.
        @see ChooserEvaluator::default_parameter_options() for the default set of parameter options.
        */
        bool select_parameters(const ChooserPoly &operand, double noise_standard_deviation, const std::map<int, BigUInt> &parameter_options, EncryptionParameters &destination);

        /**
        Provides the user with optimized encryption parameters that are large enough to support the
        operations performed on all of the given ChooserPolys. Both the standard deviation of the noise distribution
        and the list from which we choose the size of the polynomial modulus and the coefficient modulus
        are provided by the user as input parameters.

        The parameter options are given as an std::map<int, BigUInt>, where the sizes of the polynomial moduli
        are the keys, and the corresponding values are the coefficient moduli (represented by BigUInt).
        The sizes of the polynomial moduli must be at least 512 and powers of 2.

        The function returns true or false depending on whether a working parameter set was found or not.

        @param[in] operands The ChooserPolys for which the parameters are optimized
        @param[in] noise_standard_deviation The noise standard deviation
        @param[in] parameter_options The parameter options to be used
        @param[out] destination The encryption parameters to overwrite with the selected parameter set
        @throws std::logic_error if operation history is null
        @throws std::invalid_argument if operands is empty
        @throws std::invalid_argument if noise_standard_deviation is negative
        @throws std::invalid_argument if parameter_options is empty
        @throws std::invalid_argument if parameter_options has keys that are less than 512 or not powers of 2
        @see EncryptionParameters for a description of the encryption parameters.
        @see ChooserEvaluator::default_noise_standard_deviation() for the default noise standard deviation.
        @see ChooserEvaluator::default_parameter_options() for the default set of parameter options.
        */
        bool select_parameters(const std::vector<ChooserPoly> &operands, double noise_standard_deviation, const std::map<int, BigUInt> &parameter_options, EncryptionParameters &destination);

        /**
        Returns the default set of (degree(polynomial modulus),coeff_modulus)-pairs used by automatic parameter selection.
        The functions returns an std::map<int,BigUInt>, where the degree of the polynomial modulus acts as the key,
        and the corresponding modulus is the value.
        An expert user might want to give a modified map as an argument to select_parameters() for better results.
        */
        static const std::map<int, BigUInt> &default_parameter_options()
        {
            return default_parameter_options_;
        }

        /**
        Returns the default value for the standard deviation of the noise (error) distribution used by automatic
        parameter selection. An expert user might want to give a modified value as an argument to select_parameters().
        */
        static double default_noise_standard_deviation()
        {
            return default_noise_standard_deviation_;
        }

    private:
        ChooserEvaluator(const ChooserEvaluator &copy) = delete;

        ChooserEvaluator &operator =(const ChooserEvaluator &copy) = delete;

        seal::util::MemoryPool pool_;

        static const std::map<int, BigUInt> default_parameter_options_;

        static const double default_noise_standard_deviation_;
    };

    /**
    Constructs ChooserPoly objects representing encoded plaintexts. ChooserPoly objects constructed
    in this way have null operation history. They can be further used by ChooserEncryptor,
    or in the functions ChooserEvaluator::multiply_plain(), ChooserEvaluator::add_plain(), 
    ChooserEvaluator::sub_plain() representing plaintext operands. Only the balanced odd base encodings
    (those provided by BalancedEncoder) are supported by ChooserEncoder. This class is a part of the
    automatic parameter selection module.

    @par Thread Safety
    The ChooserEncoder class is not thread-safe and a separate ChooserEncoder instance is needed for each
    potentially concurrent usage.

    @see ChooserPoly for the object modeling encrypted/plaintext data for automatic parameter selection.
    @see ChooserEvaluator for manipulating instances of ChooserPoly.
    @see ChooserEncryptor for modeling the behavior of encryption with ChooserPoly objects.
    @see BalancedEncoder for the corresponding encoder for real data.
    @see Simulation for the class that handles the inherent noise growth estimates.
    */
    class ChooserEncoder
    {
    public:
        /**
        Creates a ChooserEncoder that can be used to create ChooserPoly objects modeling plaintext
        polynomials encoded with BalancedEncoder. The base will default to 3, but any odd integer at least 3
        can be used.

        @param[in] base The base (default value is 3)
        @throws std::invalid_argument if base is not an odd integer and at least 3
        */
        ChooserEncoder(uint64_t base = 3);

        /**
        Encodes a number (represented by std::uint64_t) into a ChooserPoly object. This is done by first
        encoding it with BalancedEncoder and then simply reading the number of coefficients and the maximal
        absolute value of the coefficients in the polynomial. In the returned ChooserPoly the computation
        history is set to null.

        @param[in] value The non-negative integer to encode
        @see BalancedEncoder::encode() for the corresponding function returning a real polynomial.
        */
        ChooserPoly encode(std::uint64_t value);
        
        /**
        Encodes a number (represented by std::uint64_t) into a ChooserPoly object given as an argument.
        This is done by first encoding it with BalancedEncoder and then simply reading the number of
        coefficients and the maximal absolute value of the coefficients in the polynomial. In the output
        ChooserPoly the operation history is set to null.

        @param[in] value The non-negative integer to encode
        @param[out] destination Reference to a ChooserPoly where the output will be stored
        @see BalancedEncoder::encode() for the corresponding function returning a real polynomial.
        */
        void encode(std::uint64_t value, ChooserPoly &destination);

        /**
        Encodes a number (represented by std::int64_t) into a ChooserPoly object. This is done by first
        encoding it with BalancedEncoder and then simply reading the number of coefficients and the maximal
        absolute value of the coefficients in the polynomial. In the returned ChooserPoly the computation
        history is set to null.

        @param[in] value The integer to encode
        @see BalancedEncoder::encode() for the corresponding function returning a real polynomial.
        */
        ChooserPoly encode(std::int64_t value);

        /**
        Encodes a number (represented by std::int64_t) into a ChooserPoly object given as an argument.
        This is done by first encoding it with BalancedEncoder and then simply reading the number of
        coefficients and the maximal absolute value of the coefficients in the polynomial. In the output
        ChooserPoly the operation history is set to null.

        @param[in] value The integer to encode
        @param[out] destination Reference to a ChooserPoly where the output will be stored
        @see BalancedEncoder::encode() for the corresponding function returning a real polynomial.
        */
        void encode(std::int64_t value, ChooserPoly &destination);

        /**
        Encodes a number (represented by BigUInt) into a ChooserPoly object. This is done by first
        encoding it with BalancedEncoder and then simply reading the number of coefficients and the maximal
        absolute value of the coefficients in the polynomial. In the returned ChooserPoly the computation
        history is set to null.

        @param[in] value The non-negative integer to encode
        @see BalancedEncoder::encode() for the corresponding function returning a real polynomial.
        */
        ChooserPoly encode(BigUInt value);

        /**
        Encodes a number (represented by BigUInt) into a ChooserPoly object given as an argument.
        This is done by first encoding it with BalancedEncoder and then simply reading the number of
        coefficients and the maximal absolute value of the coefficients in the polynomial. In the output
        ChooserPoly the operation history is set to null.

        @param[in] value The non-negative integer to encode
        @param[out] destination Reference to a ChooserPoly where the output will be stored
        @see BalancedEncoder::encode() for the corresponding function returning a real polynomial.
        */
        void encode(BigUInt value, ChooserPoly &destination);

        /**
        Encodes a number (represented by std::int32_t) into a ChooserPoly object. This is done by first
        encoding it with BalancedEncoder and then simply reading the number of coefficients and the maximal
        absolute value of the coefficients in the polynomial. In the returned ChooserPoly the computation
        history is set to null.

        @param[in] value The integer to encode
        @see BalancedEncoder::encode() for the corresponding function returning a real polynomial.
        */
        ChooserPoly encode(std::int32_t value)
        {
            return encode(static_cast<std::int64_t>(value));
        }

        /**
        Encodes a number (represented by std::uint32_t) into a ChooserPoly object. This is done by first
        encoding it with BalancedEncoder and then simply reading the number of coefficients and the maximal
        absolute value of the coefficients in the polynomial. In the returned ChooserPoly the computation
        history is set to null.

        @param[in] value The non-negative integer to encode
        @see BalancedEncoder::encode() for the corresponding function returning a real polynomial.
        */
        ChooserPoly encode(std::uint32_t value)
        {
            return encode(static_cast<std::uint64_t>(value));
        }

        /**
        Returns the base used for encoding.
        */
        uint64_t base() const
        {
            return encoder_.base();
        }

    private:
        BalancedEncoder encoder_;

        ChooserEncoder(const ChooserEncoder &copy) = delete;

        ChooserEncoder &operator =(const ChooserEncoder &copy) = delete;
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
        ChooserEncryptor(const ChooserEncryptor &copy) = delete;

        ChooserEncryptor &operator =(const ChooserEncryptor &copy) = delete;
    };
}


#endif // SEAL_CHOOSER_H
