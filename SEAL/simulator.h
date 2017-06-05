#pragma once

#include <utility>
#include <string>
#include <vector>
#include "encryptionparams.h"
#include "memorypoolhandle.h"
#include "bigpoly.h"
#include "biguint.h"

namespace seal
{
    /**
    Models the invariant noise budget in a ciphertext based on given EncryptionParameters object.
    When performing arithmetic operations on encrypted data, the quality of the ciphertexts will degrade,
    i.e. the invariant noise budget will be consumed, until at a certain point the budget will reach 0, and
    decryption will fail to work. The Simulation object together with SimulationEvaluator can help the user 
    understand how the invariant noise budget is consumed in different homomorphic operations, and to adjust 
    the encryption parameters accordingly.
    
    Instances of Simulation can be manipulated using SimulationEvaluator, which has a public API similar 
    to Evaluator, making existing code easy to run on simulations instead of running it on actual encrypted 
    data. In other words, using SimulationEvaluator, simulations can be added, multiplied, subtracted, 
    negated, etc., and the result is always a new Simulation object whose noise budget is obtained using 
    heuristic worst-case analysis of the noise behavior in the encryption scheme.

    @par Invariant Noise Budget
    The invariant noise polynomial of a ciphertext is a rational coefficient polynomial, such that
    a ciphertext decrypts correctly as long as the coefficients of the invariant noise polynomial are
    of absolute value less than 1/2. Thus, we call the infinity-norm of the invariant noise polynomial
    the invariant noise, and for correct decryption require it to be less than 1/2. If v denotes the
    invariant noise, we define the invariant noise budget as -log2(2v). Thus, the invariant noise budget
    starts from some initial value, which depends on the encryption parameters, and decreases to 0 when
    computations are performed. When the budget reaches 0, the ciphertext becomes too noisy to decrypt
    correctly.

    @see SimulationEvaluator for manipulating instances of Simulation.
    */
    class Simulation
    {
    public:
        /**
        Creates a simulation of a ciphertext encrypted with the specified encryption parameters and given
        invariant noise budget. The given noise budget must be at least zero, and at most the significant
        bit count of the coefficient modulus minus two.

        @param[in] parms The encryption parameters
        @param[in] noise_budget The invariant noise budget of the created ciphertext
        @param[in] ciphertext_size The size of the created ciphertext
        @throws std::invalid_argument if encryption parameters are not valid
        @throws std::invalid_argument if noise_budget is not in the valid range
        @throws std::invalid_argument if ciphertext_size is less than 2
        @see EncryptionParameters for more details on valid encryption parameters.
        */
        Simulation(EncryptionParameters &parms, int noise_budget, int ciphertext_size);

        /**
        Returns the invariant noise budget that is being simulated. If the returned value is less than or equal to 0,
        the encryption parameters used are possibly not large enough to support the performed homomorphic operations.
        */
        int invariant_noise_budget() const;

        /**
        Returns true or false depending on whether the encryption parameters were large enough to support 
        the performed homomorphic operations. The budget_gap parameter can be used to ensure that a certain
        amount of noise budget remains unused.

        @param[in] budget_gap The amount of noise budget (bits) that should remain unused
        @throws std::invalid_argument if budget_gap is negative
        */
        bool decrypts(int budget_gap = 0) const;

        /**
        Returns the size of the ciphertext whose noise is modeled by the simulation.
        @see BigPolyArray::size() for the corresponding function on BigPolyArray objects.
        */
        int size() const
        {
            return ciphertext_size_;
        }

    private:
        /**
        Creates a simulation of a ciphertext encrypted with the specified encryption parameters and given
        invariant noise. The invariant noise is interpreted as having been scaled by the coefficient modulus.
        */
        Simulation(const BigUInt &noise, const BigUInt &coeff_modulus, const BigUInt &plain_modulus, int poly_modulus_coeff_count,
            double noise_standard_deviation, double noise_max_deviation, int decomposition_bit_count, int ciphertext_size);

        /**
        Compares the encryption parameters given in the constructor to those of the argument simulation.
        */
        bool compare_encryption_parameters(const Simulation &simulation) const
        {
            return (coeff_modulus_ == simulation.coeff_modulus_ && plain_modulus_ == simulation.plain_modulus_
                && noise_max_deviation_ == simulation.noise_max_deviation_ && decomposition_bit_count_ == simulation.decomposition_bit_count_
                && poly_modulus_coeff_count_ == simulation.poly_modulus_coeff_count_) && noise_standard_deviation_ == simulation.noise_standard_deviation_;
        }

        /**
        Stores the current noise scaled by coeff_modulus_.
        */
        BigUInt noise_;

        BigUInt coeff_modulus_;

        BigUInt plain_modulus_;

        int poly_modulus_coeff_count_;

        double noise_standard_deviation_;

        double noise_max_deviation_;

        int decomposition_bit_count_;

        int ciphertext_size_;

        friend class SimulationEvaluator;
    };

    /**
    Manipulates instances of Simulation with a public API similar to how Evaluator manipulates
    ciphertexts (represented by BigPoly). This makes existing code easy to run on Simulation
    objects instead of running it on actual encrypted data.
    
    Simulation objects model the invariant noise budget in a ciphertext based on given encryption parameters.
    When performing homomorphic operations on encrypted data, the quality of the ciphertexts will degrade,
    i.e. the invariant noise budget will be consumed, until at a certain point the budget will reach 0, and
    decryption will fail to work. The Simulation object together with SimulationEvaluator can help the user 
    understand how the noise budget is consumed in different homomorphic operations, and to adjust the 
    encryption parameters accordingly.

    SimulationEvaluator allows the user to simulate the effect of homomorphic operations on the invariant
    noise budget in encrypted data. These homomorphic operations include addition, multiplication, 
    subtraction, negation, etc., and the result is always a new Simulation object whose noise budget
    is obtained using heuristic worst-case analysis of the encryption scheme.

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
    The SimulationEvaluator class is not thread-safe and a separate SimulationEvaluator instance is needed
    for each potentially concurrent usage.

    @see Simulation for the object modeling the noise in ciphertexts.
    */
    class SimulationEvaluator
    {
    public:
        /**
        Creates a new SimulationEvaluator. Optionally, the user can give a reference to a MemoryPoolHandle 
        object to use a custom memory pool instead of the global memory pool (default).

        @param[in] pool The memory pool handle
        @see MemoryPoolHandle for more details on memory pool handles.
        */
        SimulationEvaluator(const MemoryPoolHandle &pool = MemoryPoolHandle::acquire_global()) : pool_(pool)
        {
        }

        /**
        Creates a Simulation object corresponding to a freshly encrypted ciphertext. The noise is estimated
        based on the given encryption parameters, and size parameters of a virtual input plaintext polynomial, 
        namely an upper bound plain_max_coeff_count on the number of non-zero coefficients in the polynomial, 
        and an upper bound plain_max_abs_value (represented by BigUInt) on the absolute value (modulo the plaintext 
        modulus) of the polynomial coefficients.

        @param[in] parms The encryption parameters
        @param[in] plain_max_coeff_count An upper bound on the number of non-zero coefficients in the underlying plaintext
        @param[in] plain_max_abs_value An upper bound on the absolute value of the coefficients in the underlying plaintext
        @throws std::invalid_argument if plain_max_coeff_count is negative or bigger than the degree of the polynomial modulus
        @throws std::invalid_argument if plain_max_abs_value is bigger than the plaintext modulus divided by 2
        */
        Simulation get_fresh(EncryptionParameters &parms, int plain_max_coeff_count, const BigUInt &plain_max_abs_value);

        /**
        Creates a Simulation object corresponding to a freshly encrypted ciphertext. The noise is estimated
        based on the given encryption parameters, and size parameters of a virtual input plaintext polynomial,
        namely an upper bound plain_max_coeff_count on the number of non-zero coefficients in the polynomial,
        and an upper bound plain_max_abs_value (represented by std::uint64_t) on the absolute value (modulo the 
        plaintext modulus) of the polynomial coefficients.

        @param[in] parms The encryption parameters
        @param[in] plain_max_coeff_count An upper bound on the number of non-zero coefficients in the underlying plaintext
        @param[in] plain_max_abs_value An upper bound on the absolute value of the coefficients in the underlying plaintext
        @throws std::invalid_argument if plain_max_coeff_count is negative or bigger than the degree of the polynomial modulus
        @throws std::invalid_argument if plain_max_abs_value is bigger than plaintext modulus divided by 2
        */
        Simulation get_fresh(EncryptionParameters &parms, int plain_max_coeff_count, std::uint64_t plain_max_abs_value);

        /**
        Simulates noise budget consumption in Evaluator::multiply_many() and returns the result.

        @param[in] simulations The vector of Simulation objects to multiply
        @throws std::invalid_argument if the simulations vector is empty
        @throws std::invalid_argument if at least two of the elements in the simulations vector were 
        constructed with different encryption parameters
        @throws std::invalid_argument if any of elements in the simulations vector has size() less than 2
        @see Evaluator::multiply_many() for the corresponding operation on ciphertexts.
        */
        Simulation multiply_many(std::vector<Simulation> simulations);

        /**
        Simulates noise budget consumption in Evaluator::add() and returns the result.

        @param[in] simulation1 The first Simulation object to add
        @param[in] simulation2 The second Simulation object to add
        @throws std::invalid_argument if simulation1 and simulation2 were constructed with different encryption parameters
        @throws std::invalid_argument if simulation1 or simulation2 has size() less than 2
        @see Evaluator::add() for the corresponding operation on ciphertexts.
        */
        Simulation add(const Simulation &simulation1, const Simulation &simulation2);

        /**
        Simulates noise budget consumption in Evaluator::add_many() and returns the result.

        @param[in] simulations The simulations to add
        @throws std::invalid_argument if simulations is empty
        @throws std::invalid_argument if not all elements of simulations were constructed with the same encryption parameters
        @throws std::invalid_argument if any of the elements in the simulations vector has size() less than 2
        @see Evaluator::add_many() for the corresponding operation on ciphertexts.
        */
        Simulation add_many(const std::vector<Simulation> &simulations);

        /**
        Simulates noise budget consumption in Evaluator::sub() and returns the result.

        @param[in] simulation1 The Simulation object to subtract from
        @param[in] simulation2 The Simulation object to subtract
        @throws std::invalid_argument if simulation1 and simulation2 were constructed with different encryption parameters
        @throws std::invalid_argument if simulation1 or simulation2 has size() less than 2
        @see Evaluator::sub() for the corresponding operation on ciphertexts.
        */
        Simulation sub(const Simulation &simulation1, const Simulation &simulation2);

        /**
        Simulates noise budget consumption in Evaluator::multiply_plain() given an upper bound for the maximum number of
        non-zero coefficients and an upper bound for their absolute value (represented by BigUInt) in the encoding of
        the plaintext multiplier and returns the result.

        @param[in] simulation The Simulation object to multiply
        @param[in] plain_max_coeff_count An upper bound on the number of non-zero coefficients in the plain polynomial to multiply
        @param[in] plain_max_abs_value An upper bound (represented by BigUInt) on the absolute value of coefficients in the plain polynomial to multiply
        @throws std::invalid_argument if plain_max_coeff_count is out of range
        @throws std::invalid_argument if plain_max_coeff_count or plain_max_abs_value is zero
        @throws std::invalid_argument if simulation has size() less than 2
        @see Evaluator::multiply_plain() for the corresponding operation on ciphertexts.
        */
        Simulation multiply_plain(const Simulation &simulation, int plain_max_coeff_count, const BigUInt &plain_max_abs_value);

        /**
        Simulates noise budget consumption in Evaluator::multiply_plain() given an upper bound for the maximum number of
        non-zero coefficients and an upper bound for their absolute value (represented by std::uint64_t) in the encoding of
        the plain-text multiplier and returns the result.

        @param[in] simulation The Simulation object to multiply
        @param[in] plain_max_coeff_count An upper bound on the number of non-zero coefficients in the plain polynomial to multiply
        @param[in] plain_max_abs_value An upper bound (represented by std::uint64_t) on the absolute value of coefficients in the plain polynomial to multiply
        @throws std::invalid_argument if plain_max_coeff_count is out of range
        @throws std::invalid_argument if plain_max_coeff_count or plain_max_abs_value is zero
        @throws std::invalid_argument if simulation has size() less than 2
        @see Evaluator::multiply_plain() for the corresponding operation on ciphertexts.
        */
        Simulation multiply_plain(const Simulation &simulation, int plain_max_coeff_count, std::uint64_t plain_max_abs_value);

        /**
        Simulates noise budget consumption in Evaluator::add_plain() and returns the result.

        @param[in] simulation The Simulation object to add to
        @param[in] plain_max_coeff_count An upper bound on the number of non-zero coefficients in the plain polynomial to add
        @param[in] plain_max_abs_value An upper bound (represented by BigUInt) on the absolute value of coefficients in the plain polynomial to add
        @throws std::invalid_argument if simulation has size() less than 2
        @throws std::invalid_argument if plain_max_coeff_count is out of range
        @see Evaluator::add_plain() for the corresponding operation on ciphertexts.
        */
        Simulation add_plain(const Simulation &simulation, int plain_max_coeff_count, const BigUInt &plain_max_abs_value);

        /**
        Simulates noise budget consumption in Evaluator::add_plain() and returns the result.

        @param[in] simulation The Simulation object to add to
        @param[in] plain_max_coeff_count An upper bound on the number of non-zero coefficients in the plain polynomial to add
        @param[in] plain_max_abs_value An upper bound (represented by std::uint64_t) on the absolute value of coefficients in the plain polynomial to add
        @throws std::invalid_argument if simulation has size() less than 2
        @throws std::invalid_argument if plain_max_coeff_count is out of range
        @see Evaluator::add_plain() for the corresponding operation on ciphertexts.
        */
        Simulation add_plain(const Simulation &simulation, int plain_max_coeff_count, std::uint64_t plain_max_abs_value);

        /**
        Simulates noise budget consumption in Evaluator::sub_plain() and returns the result.

        @param[in] simulation The Simulation object to subtract from
        @param[in] plain_max_coeff_count An upper bound on the number of non-zero coefficients in the plain polynomial to subtract
        @param[in] plain_max_abs_value An upper bound (represented by BigUInt) on the absolute value of coefficients in the plain polynomial to subtract
        @throws std::invalid_argument if simulation has size() less than 2
        @throws std::invalid_argument if plain_max_coeff_count is out of range
        @see Evaluator::sub_plain() for the corresponding operation on ciphertexts.
        */
        Simulation sub_plain(const Simulation &simulation, int plain_max_coeff_count, const BigUInt &plain_max_abs_value);

        /**
        Simulates noise budget consumption in Evaluator::sub_plain() and returns the result.

        @param[in] simulation The Simulation object to subtract from
        @param[in] plain_max_coeff_count An upper bound on the number of non-zero coefficients in the plain polynomial to subtract
        @param[in] plain_max_abs_value An upper bound (represented by std::uint64_t) on the absolute value of coefficients in the plain polynomial to subtract
        @throws std::invalid_argument if simulation has size() less than 2
        @throws std::invalid_argument if plain_max_coeff_count is out of range
        @see Evaluator::sub_plain() for the corresponding operation on ciphertexts.
        */
        Simulation sub_plain(const Simulation &simulation, int plain_max_coeff_count, std::uint64_t plain_max_abs_value);

        /**
        Simulates noise budget consumption in Evaluator::exponentiate() and returns the result.

        @param[in] simulation The Simulation object to raise to a power
        @param[in] exponent The power to raise the Simulation object to
        @throws std::invalid_argument if exponent is zero
        @throws std::invalid_argument if simulation has size() less than 2
        @see Evaluator::exponentiate() for the corresponding operation on ciphertexts.
        */
        Simulation exponentiate(const Simulation &simulation, std::uint64_t exponent);

        /**
        Simulates noise budget consumption in Evaluator::negate() and returns the result.

        @param[in] simulation The Simulation object to negate
        @throws std::invalid_argument if simulation has size() less than 2
        @see Evaluator::negate() for the corresponding operation on ciphertexts.
        */
        Simulation negate(const Simulation &simulation);

        /**
        Simulates noise budget consumption in Evaluator::multiply() and returns the result.

        @param[in] simulation1 The first Simulation object to multiply
        @param[in] simulation2 The second Simulation object to multiply
        @throws std::invalid_argument if simulation1 and simulation2 were constructed with different encryption parameters
        @throws std::invalid_argument if simulation1 or simulation2 has size() less than 2
        @see Evaluator::multiply() for the corresponding operation on ciphertexts.
        */
        Simulation multiply(const Simulation &simulation1, const Simulation &simulation2);

        /**
        Simulates noise budget consumption in Evaluator::square() and returns the result.

        @param[in] simulation The Simulation object to square
        @throws std::invalid_argument if simulation has size() less than 2
        @see Evaluator::square() for the corresponding operation on ciphertexts.
        */
        Simulation square(const Simulation &simulation);

        /**
        Simulates noise budget consumption in Evaluator::relinearize() and returns the result.

        @param[in] simulation The Simulation object to relinearize
        @param[in] destination_size The size of the ciphertext (represented by the output simulation) after relinearization, defaults to 2
        @throws std::invalid_argument if destination_size is less than 2 or greater than that of the ciphertext represented by simulation 
        @throws std::invalid_argument if simulation has size() less than 2
        @see Evaluator::relinearize() for the corresponding operation on ciphertexts.
        */
        Simulation relinearize(const Simulation &simulation, int destination_size = 2);

    private:
        SimulationEvaluator &operator =(const SimulationEvaluator &assign) = delete;

        SimulationEvaluator &operator =(SimulationEvaluator &&assign) = delete;

        MemoryPoolHandle pool_;
    };
}