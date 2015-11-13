#ifndef SEAL_CHOOSER_H
#define SEAL_CHOOSER_H

#include <map>
#include "encryptionparams.h"
#include "simulator.h"
#include "encoder.h"
#include "util/computation.h"

namespace seal
{
    /**
    Models ciphertexts for the automatic parameter selection module. Choosing appropriate and secure
    parameters for homomorphic cryptosystems is difficult, and beyond what a normal user should have to do.
    The user programs their computations as they normally would, but replace plaintexts with ChooserPoly
    objects. ChooserPoly objects store the computation as a directed acyclic graph, and can be used to
    simulate, using the Simulation object, the inherent noise growth in the stored computation. The estimated
    inherent noise in the output ciphertext is given by the simulate() function. This class is a part of the 
    automatic parameter selection module.

    Each instance of ChooserPoly stores an estimate of the number of non-zero coefficients, and an estimate
    for their largest absolute value (represented by BigUInt) that a plaintext polynomial can be expected to
    contain after some number of arithmetic operations have been performed on it. In addition, each ChooserPoly
    stores a directed acyclic graph of computations that it has gone through.

    Instances of ChooserPoly can be manipulated using ChooserEvaluator, which has a public API similar to Evaluator,
    making existing code easy to run on ChooserPoly objects instead of running it on actual encrypted data. In other words,
    using ChooserEvaluator, ChooserPoly objects can be added, multiplied, subtracted, negated, etc., and the result is always
    a new ChooserPoly object whose inherent noise is obtained using the simulate() function, which uses average-case
    analysis of the noise behavior in the encryption scheme. 

    @see ChooserEvaluator for manipulating instances of ChooserPoly.
    @see ChooserEncoder for modeling the behavior of encoding with ChooserPoly objects.
    @see ChooserEncryptor for modeling the behavior of encryption with ChooserPoly objects.
    @see Simulation for the class that handles the inherent noise growth estimates.
    */
    class ChooserPoly
    {
    public:
        /**
        Creates an empty ChooserPoly modeling plaintext data with no non-zero coefficients.
        */
        ChooserPoly();

        /**
        Creates a ChooserPoly modeling plaintext data with a given bound on the number of non-zero coefficients,
        and a given bound on the size of the absolute value of the coefficients (represented by BigUInt).

        @param[in] max_coeff_count An upper bound on the number of non-zero coefficients in the modeled plaintext data
        @param[in] max_abs_value An upper bound on the absolute value of the coefficients in the modeled plaintext data
        */
        ChooserPoly(int max_coeff_count, BigUInt max_abs_value);

        /**
        Creates a ChooserPoly modeling plaintext data with a given bound on the number of non-zero coefficients,
        and a given bound on the size of the absolute value of the coefficients (represented by std::uint64_t).

        @param[in] max_coeff_count An upper bound on the number of non-zero coefficients in the modeled plaintext data
        @param[in] max_abs_value An upper bound on the absolute value of the coefficients in the modeled plaintext data
        */
        ChooserPoly(int max_coeff_count, uint64_t max_abs_value);

        /**
        Destroys the ChooserPoly, and deallocates all memory associated with it, including memory related to storing
        the computation history.
        */
        ~ChooserPoly();

        /**
        Creates a deep copy of a ChooserPoly. The created ChooserPoly will model plaintext data with same bounds as the
        original one, and contains a clone of its the computation history.
        and coefficient values as the original.

        @param[in] copy The ChooserPoly to copy from
        */
        ChooserPoly(const ChooserPoly &copy);

        /**
        Overwrites the ChooserPoly with the value of the specified ChooserPoly, including a deep copy of the associated
        computation history is made.

        @param[in] assign The whose value should be assigned to the current ChooserPoly
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
        Returns a reference the upper bound on the number of non-zero coefficients in the plaintext modeled by the current instance
        of ChooserPoly.
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
        Simulates inherent noise growth in the computation history of the current instance of ChooserPoly.
        The function simulate() gives information about inherent noise growth in the operations in the
        computation history of the ChooserPoly with given encryption parameters as input. The return value
        is a Simulation object.

        @param[in] parms The encryption parameters
        @see EncryptionParameters for more details on valid encryption parameters.
        @see Simulation for more details on the inherent noise growth simulation.
        */
        Simulation simulate(EncryptionParameters &parms) const
        {
            return comp_->simulate(parms);
        }

        /**
        Determines whether given encryption parameters are large enough to support operations in
        the computation history of the current ChooserPoly. A ChooserPoly produced as a result of any
        number of arithmetic operations (with ChooserEvaluator) contains information about
        bounds on the number of non-zero coefficients in a corresponding plaintext polynomial and on the
        absolute values of the coefficients. For decryption to work correctly, these bounds must be
        small enough to be supported by the encryption parameters. Additionally, the encryption parameters
        must be large enough to support inherent noise growth in the performed operations.
        
        The return value is true or false depending on whether given encryption parameters are large enough
        to support the operations in the computation history of the current ChooserPoly.

        @param[in] parms The encryption parameters
        @see EncryptionParameters for more details on valid encryption parameters.
        @see Simulation for more details on the inherent noise growth simulation.
        */
        bool test_parameters(EncryptionParameters &parms) const
        {
            return (parms.plain_modulus().significant_bit_count() >= max_abs_value_.significant_bit_count() &&
                parms.poly_modulus().significant_coeff_count() > max_coeff_count_ && simulate(parms).decrypts());
        }

        /**
        Provides the user with optimized encryption parameters that are large enough to support the
        operations performed on the current ChooserPoly. Some choices are made in the process that
        an expert user might want to change: (1) We use a constant small standard deviation for the noise distribution;
        (2) We choose the size of the polynomial modulus and the coefficient modulus from a hard-coded list of
        choices we consider secure, and estimate to have at least 80 bits of security against a 2^(-80) advantage
        for solving decision-LWE (see http://eprint.iacr.org/2014/062.pdf, Table 2).

        If no large enough encryption parameters could be found, select_parameters() returns an empty instance
        of EncryptionParameters.

        @see EncryptionParameters for a description of the encryption parameters.
        */
        EncryptionParameters select_parameters() const;

        EncryptionParameters select_parameters(const std::map<int, BigUInt> &options) const;

        /**
        Sets the bounds on the degree and the absolute value of the coefficients of the modeled plaintext
        polynomial to zero, and sets the computation history to empty.

        @warning An empty computation history is not the same as that of a freshly encrypted ciphertext.
        @see set_fresh() for setting the computation history to that of a freshly encrypted ciphertext.
        */
        void reset();

        /**
        Sets the computation history to that of a freshly encrypted ciphertext, but leaves the bounds on
        the degree and the absolute value of the coefficients of the modeled plaintext polynomial unchanged.

        @see max_coeff_count() to set the bound on the degree of the modeled plaintext polynomial.
        @see max_abs_value() to set the bound on the absolute value of coefficients of the modeled plaintext polynomial.
        */
        void set_fresh();

        /**
        Returns the estimated value of inherent noise (represented by BigUInt) in a ciphertext on which the operations
        in the computation history of the current ChooserPoly have been performed.

        @param[in] parms The encryption parameters
        @see EncryptionParameters for more details on valid encryption parameters.
        @see simulate() to return the full Simulation object instead.
        */
        BigUInt noise(EncryptionParameters &parms) const
        {
            return simulate(parms).noise();
        }

        /**
        Returns the maximal value of inherent noise (represented by BigUInt) supported by the given encryption parameters.
        The encryption parameters are not large enough if the value returned by noise() is larger than the value
        returned by max_noise().

        @param[in] parms The encryption parameters
        @see EncryptionParameters for more details on valid encryption parameters.
        */
        BigUInt max_noise(EncryptionParameters &parms) const
        {
            return simulate(parms).max_noise();
        }

        /**
        Returns the bit length of the estimated value of inherent noise (represented by BigUInt) in a ciphertext on
        which the operations in the computation history of the current ChooserPoly have been performed.

        @param[in] parms The encryption parameters
        @see EncryptionParameters for more details on valid encryption parameters.
        @see noise() to return the estimated value of inherent noise as a BigUInt instead.
        @see simulate() to return the full Simulation object instead.
        */
        int noise_bits(EncryptionParameters &parms) const
        {
            return simulate(parms).noise_bits();
        }

        /**
        Returns the difference between the bit lengths of the return values of max_noise() and of noise(). This gives
        the user a convenient tool for estimating how many, if any, arithmetic operations can still be performed on the
        encrypted data before it becomes impossible to decrypt. If the return value is negative, the encryption parameters
        used are not large enough to support the performed arithmetic operations.

        @param[in] parms The encryption parameters
        @see EncryptionParameters for more details on valid encryption parameters.
        @see noise() to return the estimated value of inherent noise as a BigUInt instead.
        @see simulate() to return the full Simulation object instead.
        */
        int noise_bits_left(EncryptionParameters &parms) const
        {
            return simulate(parms).noise_bits_left();
        }

        /**
        Returns true or false depending on whether the encryption parameters were large enough to support inherent
        noise growth in the performed arithmetic operations.

        @param[in] parms The encryption parameters
        @see EncryptionParameters for more details on valid encryption parameters.
        @see simulate() to return the full Simulation object instead.
        */
        bool decrypts(EncryptionParameters &parms) const
        {
            return simulate(parms).decrypts();
        }

        static const std::map<int, BigUInt> &default_options()
        {
            return default_options_;
        }

    private:
        int max_coeff_count_;

        BigUInt max_abs_value_;

        seal::util::Computation *comp_;

        static const std::map<int, BigUInt> default_options_;

        ChooserPoly(int max_coeff_count, BigUInt max_abs_value, seal::util::Computation *comp);

        ChooserPoly(int max_coeff_count, uint64_t max_abs_value, seal::util::Computation *comp);

        const seal::util::Computation *comp() const
        {
            return comp_;
        }

        friend class ChooserEvaluator;
    };

    /**
    Models arithmetic operations on ChooserPoly objects rather than performing them on real data.
    The class ChooserEvaluator has a public API similar to that of Evaluator,
    making it easy for the user to run existing code on ChooserPoly objects rather than on actual data.
    All of these operations take as input a varying number of ChooserPoly objects and return a new one
    with an appropriately extended computation history. This class is a part of the automatic parameter
    selection module.

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
        Performs an operation modeling Evaluator::tree_multiply() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the computation history
        accordingly.

        @param[in] operands The vector of ChooserPoly objects to multiply
        @throws std::invalid_argument if operands vector is empty
        @throws std::invalid_argument if any of the ChooserPoly objects in operands vector is not correctly initialized
        @see Evaluator::tree_multiply() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::tree_multiply() for the corresponding operation on Simulation objects.
        */
        ChooserPoly tree_multiply(const std::vector<ChooserPoly> &operands) const;

        /**
        Performs an operation modeling Evaluator::add() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the computation history
        accordingly.

        @param[in] operand1 The first ChooserPoly object to add
        @param[in] operand2 The second ChooserPoly object to add
        @throws std::invalid_argument if either operand1 or operand2 is not correctly initialized
        @see Evaluator::add() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::add() for the corresponding operation on Simulation objects.
        */
        ChooserPoly add(const ChooserPoly &operand1, const ChooserPoly &operand2) const;

        /**
        Performs an operation modeling Evaluator::sub() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the computation history
        accordingly.

        @param[in] operand1 The first ChooserPoly object to subtract
        @param[in] operand2 The second ChooserPoly object to subtract
        @throws std::invalid_argument if either operand1 or operand2 is not correctly initialized
        @see Evaluator::subtract() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::subtract() for the corresponding operation on Simulation objects.
        */
        ChooserPoly sub(const ChooserPoly &operand1, const ChooserPoly &operand2) const;

        /**
        Performs an operation modeling Evaluator::multiply() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the computation history
        accordingly.

        @param[in] operand1 The first ChooserPoly object to multiply
        @param[in] operand2 The second ChooserPoly object to multiply
        @throws std::invalid_argument if either operand1 or operand2 is not correctly initialized
        @see Evaluator::multiply() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::multiply() for the corresponding operation on Simulation objects.
        */
        ChooserPoly multiply(const ChooserPoly &operand1, const ChooserPoly &operand2) const;

        /**
        Performs an operation modeling Evaluator::multiply_norelin() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the computation history
        accordingly.

        @param[in] operand1 The first ChooserPoly object to multiply
        @param[in] operand2 The second ChooserPoly object to multiply
        @throws std::invalid_argument if either operand1 or operand2 is not correctly initialized
        @see Evaluator::multiply_norelin() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::multiply_norelin() for the corresponding operation on Simulation objects.
        */
        ChooserPoly multiply_norelin(const ChooserPoly &operand1, const ChooserPoly &operand2) const;

        /**
        Performs an operation modeling Evaluator::relinearize() on ChooserPoly objects. This operation
        creates a new ChooserPoly with the same bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients as the input, but sets the computation history
        to include the relinearization operation.

        @param[in] operand The ChooserPoly object to relinearize
        @throws std::invalid_argument if operand is not correctly initialized
        @see Evaluator::relinearize() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::relinearize() for the corresponding operation on Simulation objects.
        */
        ChooserPoly relinearize(const ChooserPoly &operand) const;

        /**
        Performs an operation modeling Evaluator::multiply_plain() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the computation history
        accordingly.

        @param[in] operand The ChooserPoly object to multiply
        @param[in] plain_max_coeff_count Bound on the number of non-zero coefficients in the plaintext polynomial to multiply
        @param[in] plain_max_abs_value Bound on the absolute value of coefficients of the plaintext polynomial to multiply
        @throws std::invalid_argument if operand is not correctly initialized
        @throws std::invalid_argument if plain_max_coeff_count is not positive
        @see Evaluator::multiply_plain() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::multiply_plain() for the corresponding operation on Simulation objects.
        */
        ChooserPoly multiply_plain(const ChooserPoly &operand, int plain_max_coeff_count, const BigUInt &plain_max_abs_value) const;

        /**
        Performs an operation modeling Evaluator::multiply_plain() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the computation history
        accordingly.

        @param[in] operand The ChooserPoly object to multiply
        @param[in] plain_max_coeff_count Bound on the number of non-zero coefficients in the plaintext polynomial to multiply
        @param[in] plain_max_abs_value Bound on the absolute value of coefficients of the plaintext polynomial to multiply
        @throws std::invalid_argument if operand is not correctly initialized
        @throws std::invalid_argument if plain_max_coeff_count is not positive
        @see Evaluator::multiply_plain() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::multiply_plain() for the corresponding operation on Simulation objects.
        */
        ChooserPoly multiply_plain(const ChooserPoly &operand, int plain_max_coeff_count, uint64_t plain_max_abs_value) const;

        /**
        Performs an operation modeling Evaluator::multiply_plain() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the computation history
        accordingly.

        This variant of the function takes the plaintext multiplier as input in the form of another ChooserPoly.
        If the plaintext multiplier is already known at the time of performing the automatic parameter selection,
        one can use ChooserEncoder to construct the appropriate ChooserPoly for plain_chooser_poly. This function
        completely ignores the computation history possibly carried by plain_chooser_poly.

        @param[in] operand The ChooserPoly object to multiply
        @param[in] plain_chooser_poly The plaintext polynomial to multiply represented by a ChooserPoly
        @throws std::invalid_argument if operand is not correctly initialized
        @throws std::invalid_argument if plain_chooser_poly has a non-positive bound on the number of non-zero coefficients
        @see Evaluator::multiply_plain() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::multiply_plain() for the corresponding operation on Simulation objects.
        @see ChooserEncoder for constructing the ChooserPoly for the plaintext multiplier.
        */
        ChooserPoly multiply_plain(const ChooserPoly &operand, const ChooserPoly &plain_chooser_poly) const
        {
            return multiply_plain(operand, plain_chooser_poly.max_coeff_count_, plain_chooser_poly.max_abs_value_);
        }

        /**
        Performs an operation modeling Evaluator::add_plain() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the computation history
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
        and on the absolute values of the coefficients based on the inputs, and sets the computation history
        accordingly.

        @param[in] operand The ChooserPoly object to add
        @param[in] plain_max_coeff_count Bound on the number of non-zero coefficients in the plaintext polynomial to add
        @param[in] plain_max_abs_value Bound on the absolute value of coefficients of the plaintext polynomial to add
        @throws std::invalid_argument if operand is not correctly initialized
        @throws std::invalid_argument if plain_max_coeff_count is not positive
        @see Evaluator::add_plain() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::add_plain() for the corresponding operation on Simulation objects.
        */
        ChooserPoly add_plain(const ChooserPoly &operand, int plain_max_coeff_count, uint64_t plain_max_abs_value) const;

        /**
        Performs an operation modeling Evaluator::add_plain() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the computation history
        accordingly.

        This variant of the function takes the plaintext to add as input in the form of another ChooserPoly.
        If the plaintext to be add is already known at the time of performing the automatic parameter selection,
        one can use ChooserEncoder to construct the appropriate ChooserPoly for plain_chooser_poly. This function
        completely ignores the computation history possibly carried by plain_chooser_poly.

        @param[in] operand The ChooserPoly object to add
        @param[in] plain_chooser_poly The plaintext polynomial to add represented by a ChooserPoly
        @throws std::invalid_argument if operand is not correctly initialized
        @throws std::invalid_argument if plain_chooser_poly has a non-positive bound on the number of non-zero coefficients
        @see Evaluator::add_plain() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator:add_plain() for the corresponding operation on Simulation objects.
        @see ChooserEncoder for constructing the ChooserPoly for the plaintext to add.
        */
        ChooserPoly add_plain(const ChooserPoly &operand, const ChooserPoly &plain_chooser_poly) const
        {
            return add_plain(operand, plain_chooser_poly.max_coeff_count_, plain_chooser_poly.max_abs_value_);
        }

        /**
        Performs an operation modeling Evaluator::sub_plain() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the computation history
        accordingly.

        @param[in] operand The ChooserPoly object to subtract
        @param[in] plain_max_coeff_count Bound on the number of non-zero coefficients in the plaintext polynomial to subtract
        @param[in] plain_max_abs_value Bound on the absolute value of coefficients of the plaintext polynomial to subtract
        @throws std::invalid_argument if operand is not correctly initialized
        @throws std::invalid_argument if plain_max_coeff_count is not positive
        @see Evaluator::sub_plain() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::sub_plain() for the corresponding operation on Simulation objects.
        */
        ChooserPoly sub_plain(const ChooserPoly &operand, int plain_max_coeff_count, const BigUInt &plain_max_abs_value) const;

        /**
        Performs an operation modeling Evaluator::sub_plain() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the computation history
        accordingly.

        @param[in] operand The ChooserPoly object to subtract
        @param[in] plain_max_coeff_count Bound on the number of non-zero coefficients in the plaintext polynomial to subtract
        @param[in] plain_max_abs_value Bound on the absolute value of coefficients of the plaintext polynomial to subtract
        @throws std::invalid_argument if operand is not correctly initialized
        @throws std::invalid_argument if plain_max_coeff_count is not positive
        @see Evaluator::sub_plain() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::sub_plain() for the corresponding operation on Simulation objects.
        */
        ChooserPoly sub_plain(const ChooserPoly &operand, int plain_max_coeff_count, uint64_t plain_max_abs_value) const;

        /**
        Performs an operation modeling Evaluator::sub_plain() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the computation history
        accordingly.

        This variant of the function takes the plaintext to subtract as input in the form of another ChooserPoly.
        If the plaintext to be subtract is already known at the time of performing the automatic parameter selection,
        one can use ChooserEncoder to construct the appropriate ChooserPoly for plain_chooser_poly. This function
        completely ignores the computation history possibly carried by plain_chooser_poly.

        @param[in] operand The ChooserPoly object to subtract
        @param[in] plain_chooser_poly The plaintext polynomial to subtract represented by a ChooserPoly
        @throws std::invalid_argument if operand is not correctly initialized
        @throws std::invalid_argument if plain_chooser_poly has a non-positive bound on the number of non-zero coefficients
        @see Evaluator::sub_plain() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator:sub_plain() for the corresponding operation on Simulation objects.
        @see ChooserEncoder for constructing the ChooserPoly for the plaintext to subtract.
        */
        ChooserPoly sub_plain(const ChooserPoly &operand, const ChooserPoly &plain_chooser_poly) const
        {
            return sub_plain(operand, plain_chooser_poly.max_coeff_count_, plain_chooser_poly.max_abs_value_);
        }

        /**
        Performs an operation modeling Evaluator::binary_exponentiate() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the computation history
        accordingly.

        @param[in] operand The ChooserPoly object to raise to a power
        @param[in] exponent The non-negative power to raise the ChooserPoly object to
        @throws std::invalid_argument if operand is not correctly initialized
        @throws std::invalid_argument if the exponent is negative
        @throws std::invalid_argument if operand models a zero polynomial and exponent is zero
        @see Evaluator::binary_exponentiate() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::binary_exponentiate() for the corresponding operation on Simulation objects.
        */
        ChooserPoly binary_exponentiate(const ChooserPoly &operand, int exponent) const;

        /**
        Performs an operation modeling Evaluator::tree_exponentiate() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the computation history
        accordingly.

        @param[in] operand The ChooserPoly object to raise to a power
        @param[in] exponent The non-negative power to raise the ChooserPoly object to
        @throws std::invalid_argument if operand is not correctly initialized
        @throws std::invalid_argument if the exponent is negative
        @throws std::invalid_argument if operand models a zero polynomial and exponent is zero
        @see Evaluator::tree_exponentiate() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::tree_exponentiate() for the corresponding operation on Simulation objects.
        */
        ChooserPoly tree_exponentiate(const ChooserPoly &operand, int exponent) const;

        /**
        Performs an operation modeling Evaluator::negate() on ChooserPoly objects. This operation
        creates a new ChooserPoly with updated bounds on the degree of the corresponding plaintext polynomial
        and on the absolute values of the coefficients based on the inputs, and sets the computation history
        accordingly.

        @param[in] operand The ChooserPoly object to negate
        @throws std::invalid_argument if operand is not correctly initialized
        @see Evaluator::negate() for the corresponding operation on ciphertexts.
        @see SimulationEvaluator::negate() for the corresponding operation on Simulation objects.
        */
        ChooserPoly negate(const ChooserPoly &operand) const;

    private:
        ChooserEvaluator(const ChooserEvaluator &copy) = delete;

        ChooserEvaluator &operator =(const ChooserEvaluator &copy) = delete;
    };

    /**
    Constructs ChooserPoly objects representing encoded plaintexts. ChooserPoly objects constructed
    in this way have empty computation history. They can be further used by ChooserEncryptor,
    or in the functions ChooserEvaluator::multiply_plain(), ChooserEvaluator::add_plain(), 
    ChooserEvaluator::sub_plain() representing plaintext operands. Only the balanced odd base encodings
    (those provided by BalancedEncoder) are supported by ChooserEncoder. This class is a part of the
    automatic parameter selection module.

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

        @throws std::invalid_argument if base is not an odd integer and at least 3
        */
        ChooserEncoder(int base = 3);

        /**
        Encodes a number (represented by std::uint64_t) into a ChooserPoly object. This is done by first
        encoding it with BalancedEncoder and then simply reading the number of coefficients and the maximal
        absolute value of the coefficients in the polynomial. In the returned ChooserPoly the computation
        history is set to empty.

        @param[in] value The non-negative integer to encode
        @see BalancedEncoder::encode() for the corresponding function returning a real polynomial.
        */
        ChooserPoly encode(std::uint64_t value);
        
        /**
        Encodes a number (represented by std::uint64_t) into a ChooserPoly object given as an argument.
        This is done by first encoding it with BalancedEncoder and then simply reading the number of
        coefficients and the maximal absolute value of the coefficients in the polynomial. In the output
        ChooserPoly the computation history is set to empty.

        @param[in] value The non-negative integer to encode
        @param[out] destination Reference to a ChooserPoly where the output will be stored
        @see BalancedEncoder::encode() for the corresponding function returning a real polynomial.
        */
        void encode(std::uint64_t value, ChooserPoly &destination);

        /**
        Encodes a number (represented by std::int64_t) into a ChooserPoly object. This is done by first
        encoding it with BalancedEncoder and then simply reading the number of coefficients and the maximal
        absolute value of the coefficients in the polynomial. In the returned ChooserPoly the computation
        history is set to empty.

        @param[in] value The integer to encode
        @see BalancedEncoder::encode() for the corresponding function returning a real polynomial.
        */
        ChooserPoly encode(std::int64_t value);

        /**
        Encodes a number (represented by std::int64_t) into a ChooserPoly object given as an argument.
        This is done by first encoding it with BalancedEncoder and then simply reading the number of
        coefficients and the maximal absolute value of the coefficients in the polynomial. In the output
        ChooserPoly the computation history is set to empty.

        @param[in] value The integer to encode
        @param[out] destination Reference to a ChooserPoly where the output will be stored
        @see BalancedEncoder::encode() for the corresponding function returning a real polynomial.
        */
        void encode(std::int64_t value, ChooserPoly &destination);

        /**
        Encodes a number (represented by BigUInt) into a ChooserPoly object. This is done by first
        encoding it with BalancedEncoder and then simply reading the number of coefficients and the maximal
        absolute value of the coefficients in the polynomial. In the returned ChooserPoly the computation
        history is set to empty.

        @param[in] value The non-negative integer to encode
        @see BalancedEncoder::encode() for the corresponding function returning a real polynomial.
        */
        ChooserPoly encode(BigUInt value);

        /**
        Encodes a number (represented by BigUInt) into a ChooserPoly object given as an argument.
        This is done by first encoding it with BalancedEncoder and then simply reading the number of
        coefficients and the maximal absolute value of the coefficients in the polynomial. In the output
        ChooserPoly the computation history is set to empty.

        @param[in] value The non-negative integer to encode
        @param[out] destination Reference to a ChooserPoly where the output will be stored
        @see BalancedEncoder::encode() for the corresponding function returning a real polynomial.
        */
        void encode(BigUInt value, ChooserPoly &destination);

        /**
        Encodes a number (represented by std::int64_t) into a ChooserPoly object. This is done by first
        encoding it with BalancedEncoder and then simply reading the number of coefficients and the maximal
        absolute value of the coefficients in the polynomial. In the returned ChooserPoly the computation
        history is set to empty. This is needed to prevent problems with automatic type conversion from std::int32_t.

        @param[in] value The integer to encode
        @see BalancedEncoder::encode() for the corresponding function returning a real polynomial.
        */
        ChooserPoly encode(std::int32_t value)
        {
            return encode(static_cast<std::int64_t>(value));
        }

        /**
        Encodes a number (represented by std::uint64_t) into a ChooserPoly object. This is done by first
        encoding it with BalancedEncoder and then simply reading the number of coefficients and the maximal
        absolute value of the coefficients in the polynomial. In the returned ChooserPoly the computation
        history is set to empty. This is needed to prevent problems with automatic type conversion from std::uint32_t.

        @param[in] value The non-negative integer to encode
        @see BalancedEncoder::encode() for the corresponding function returning a real polynomial.
        */
        ChooserPoly encode(std::uint32_t value)
        {
            return encode(static_cast<std::uint64_t>(value));
        }

        /**
        Returns the base of the encoding used. This must be an odd integer at least 3. 
        */
        int base() const
        {
            return encoder_.base();
        }

        /**
        Sets the base of the encoding used. This base must be an odd integer at least 3. 

        @param[in] base The new odd base
        @throws std::invalid_argument if base is not an odd integer and at least 3
        */
        void set_base(int base);

    private:
        BalancedEncoder encoder_;

        ChooserEncoder(const ChooserEncoder &copy) = delete;

        ChooserEncoder &operator =(const ChooserEncoder &copy) = delete;
    };

    /**
    Manipulates ChooserPolys created by ChooserEncoder by setting their computation history to that
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
        and sets the computation history of destination to that of a freshly encrypted ciphertext.

        @param[in] plain The ChooserPoly modeling a plaintext polynomial (e.g. constructed with ChooserEncoder)
        @param[out] destination The ChooserPoly to overwrite with the "encrypted" ChooserPoly
        @see Encryptor::encrypt() for the corresponding operation on real plaintext/ciphertext polynomials.
        */
        void encrypt(const ChooserPoly &plain, ChooserPoly &destination) const
        {
            destination = plain;
            destination.set_fresh();
        }

        /**
        Makes a copy of a given ChooserPoly, but sets the computation history to that of a freshly encrypted ciphertext.

        @param[in] plain The ChooserPoly modeling a plaintext polynomial (e.g. constructed with ChooserEncoder)
        @see Encryptor::encrypt() for the corresponding operation on real plaintext/ciphertext polynomials.
        */
        ChooserPoly encrypt(const ChooserPoly &plain) const
        {
            ChooserPoly result(plain);
            result.set_fresh();
            return result;
        }

        /**
        Overwrites the referenced ChooserPoly destination with the referenced ChooserPoly plain,
        and sets the computation history of destination to empty.

        @param[in] plain The ChooserPoly modeling a plaintext polynomial (e.g. constructed with ChooserEncoder)
        @param[out] destination The ChooserPoly to overwrite with the "decrypted" ChooserPoly
        @see Decryptor::decrypt() for the corresponding operation on real plaintext/ciphertext polynomials.
        */
        void decrypt(const ChooserPoly &encrypted, ChooserPoly &destination) const
        {
            destination.reset();
            destination.max_abs_value() = encrypted.max_abs_value();
            destination.max_coeff_count() = encrypted.max_coeff_count();
        }

        /**
        Makes a copy of a given ChooserPoly, but sets the computation history to empty.

        @param[in] plain The ChooserPoly modeling a plaintext polynomial (e.g. constructed with ChooserEncoder)
        @see Encryptor::decrypt() for the corresponding operation on real plaintext/ciphertext polynomials.
        */
        ChooserPoly decrypt(const ChooserPoly &encrypted) const
        {
            ChooserPoly result;
            decrypt(encrypted, result);
            return result;
        }

    private:
        ChooserEncryptor(const ChooserEncryptor &copy) = delete;

        ChooserEncryptor &operator =(const ChooserEncryptor &copy) = delete;
    };
}


#endif // SEAL_CHOOSER_H
