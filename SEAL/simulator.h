#ifndef SEAL_SIMULATOR_H
#define SEAL_SIMULATOR_H

#include "encryptionparams.h"
#include "bigpoly.h"
#include "biguint.h"
#include "util/mempool.h"
#include <string>
#include <vector>


namespace seal
{
    /**
    Models the inherent noise in a ciphertext based on given EncryptionParameters object.
    When performing arithmetic operations on encrypted data, the quality of the ciphertexts will degrade,
    i.e. the inherent noise in them will grow, until at a certain point decryption will fail to work.
    The Simulation object together with SimulationEvaluator can help the user understand how inherent
    noise grows in different homomorphic operations, and to adjust the encryption parameters accordingly.
    
    Instances of Simulation can be manipulated using SimulationEvaluator, which has a public API similar to Evaluator,
    making existing code easy to run on simulations instead of running it on actual encrypted data. In other words,
    using SimulationEvaluator, simulations can be added, multiplied, subtracted, negated, etc., and the result is always
    a new Simulation object whose inherent noise is obtained using average-case analysis of the noise behavior in the 
    encryption scheme.

    @par Inherent Noise
    Technically speaking, the inherent noise of a ciphertext is a polynomial, but the condition for decryption working
    depends on the size of the largest absolute value of its coefficients. It is really the size of this
    largest absolute value that Simulation is simulating, and that we will call the "noise", the "inherent noise",
    or the "error", in this documentation. The reader is referred to the description of the encryption scheme for more details.

    @see SimulationEvaluator for manipulating instances of Simulation.
    */
    class Simulation
    {
    public:
        /**
        Creates a simulation of a fresh ciphertext encrypted with the specified encryption parameters.

        @param[in] parms The encryption parameters
        @throws std::invalid_argument if encryption parameters are not valid
        @see EncryptionParameters for more details on valid encryption parameters.
        */
        Simulation(const EncryptionParameters &parms);

        /**
        Creates a simulation of a ciphertext encrypted with the specified encryption parameters and given inherent noise.

        @param[in] parms The encryption parameters
        @param[in] noise The inherent noise in the created ciphertext
        @throws std::invalid_argument if encryption parameters are not valid
        @throws std::invalid_argument if noise is bigger than the given coefficient modulus
        @see EncryptionParameters for more details on valid encryption parameters.
        */
        Simulation(const EncryptionParameters &parms, const BigUInt &noise);

        /**
        Returns a reference to the value of inherent noise (represented by BigUInt) that is being simulated. If the
        returned value is larger than that returned by max_noise(), the encryption parameters used are possibly
        not large enough to support the performed homomorphic operations.

        @warning The average-case estimates used by the simulator are typically conservative, so the size of noise tends
        to be overestimated.
        @see noise_bits() to instead return the bit length of the value of inherent noise that is being simulated.
        */
        const BigUInt &noise() const
        {
            return noise_;
        }

        /**
        Returns a reference to the maximal value of inherent noise (represented by BigUInt) that a ciphertext encrypted
        using the given encryption parameters can contain and still decrypt correctly. If noise() returns a value larger than this,
        the encryption parameters used are possibly not large enough to support the performed homomorphic operations.
       */
        const BigUInt &max_noise() const
        {
            return max_noise_;
        }

        /**
        Returns the bit length of the value of inherent noise that is being simulated.

        @warning The average-case estimates used by the simulator are typically conservative, so the amount of noise tends
        to be overestimated.
        */
        int noise_bits() const
        {
            return noise_.significant_bit_count();
        }

        /**
        Returns the difference between the bit lengths of the return values of max_noise() and of noise(). This gives
        the user a convenient tool for estimating how many, if any, arithmetic operations can still be performed on the
        encrypted data before it becomes impossible to decrypt. If the return value is negative, the encryption parameters
        used are not large enough to support the performed arithmetic operations.

        @warning The average-case estimates used by the simulator are typically conservative, so the amount of noise tends
        to be overestimated.
        */
        int noise_bits_left() const
        {
            return max_noise_.significant_bit_count() - noise_bits();
        }

        /**
        Returns true or false depending on whether the encryption parameters were large enough to support the performed
        homomorphic operations.

        @warning The average-case estimates used by the simulator are typically conservative, so the amount of noise tends
        to be overestimated, and decryption might work even if decrypts() returns false.
        */
        bool decrypts() const
        {
            return (noise_bits_left() >= 0);
        }

        /**
        Returns a reference to the coefficient modulus.
        */
        const BigUInt &coeff_modulus() const
        {
            return coeff_modulus_;
        }

        /**
        Returns a reference to the plaintext modulus.
        */
        const BigUInt &plain_modulus() const
        {
            return plain_modulus_;
        }

    private:
        Simulation(const BigUInt &noise, const BigUInt &max_noise, const BigUInt &coeff_modulus, const BigUInt &plain_modulus, int poly_modulus_coeff_count,
            double noise_standard_deviation, double noise_max_deviation, int decomposition_bit_count);

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
        Sets the inherent noise to correspond to that of a freshly encrypted ciphertext using an average-case estimate.
        */
        void set_initial_noise_estimate();

        BigUInt noise_;

        BigUInt max_noise_;

        BigUInt coeff_modulus_;

        BigUInt plain_modulus_;

        int poly_modulus_coeff_count_;

        double noise_standard_deviation_;

        double noise_max_deviation_;

        int decomposition_bit_count_;

        friend class SimulationEvaluator;
    };

    /**
    Manipulates instances of Simulation with a public API similar to how Evaluator manipulates
    ciphertexts (represented by BigPoly). This makes existing code easy to run on Simulation
    objects instead of running it on actual encrypted data.
    
    Simulation objects model the inherent noise in a ciphertext based on given encryption parameters.
    When performing homomorphic operations on encrypted data, the quality of the ciphertexts will degrade,
    i.e. the inherent noise in them will grow, until at a certain point decryption will fail to work.
    The Simulation object together with SimulationEvaluator can help the user understand how the inherent
    noise grows in different homomorphic operations, and to adjust the encryption parameters accordingly.

    SimulationEvaluator allows the user to simulate the effect of homomorphic operations on the inherent
    noise in encrypted data. These homomorphic operations include addition, multiplication, subtraction,
    negation, etc., and the result is always a new Simulation object whose inherent noise is obtained using
    average-case analysis of the encryption scheme.

    @par Inherent Noise
    Technically speaking, the inherent noise of a ciphertext is a polynomial, but the condition for decryption working
    depends on the size of the largest absolute value of its coefficients. It is really the size of this
    largest absolute value that Simulation is simulating, and that we will call the "noise", the "inherent noise",
    or the "error", in this documentation. The reader is referred to the description of the encryption scheme for more details.

    @par Thread Safety
    The SimulationEvaluator class is not thread-safe and a separate SimulationEvaluator instance is needed
    for each potentially concurrent usage.

    @warning Accuracy of the average-case analysis depends on the encryption parameters.
    @see Simulation for the object modeling the noise in ciphertexts.
    */
    class SimulationEvaluator
    {
    public:
        /**
        Simulates inherent noise growth in Evaluator::multiply_many() and returns the result.

        @param[in] simulations The vector of Simulation objects to multiply
        @throws std::invalid_argument if the simulations vector is empty
        @throws std::invalid_argument if at least two of the elements in the simulations vector were constructed with different encryption parameters
        @see Evaluator::multiply_many() for the corresponding operation on ciphertexts.
        */
        Simulation multiply_many(std::vector<Simulation> simulations);

        /**
        Simulates inherent noise growth in Evaluator::multiply() and returns the result.

        @param[in] simulation1 The first Simulation object to multiply
        @param[in] simulation2 The second Simulation object to multiply
        @throws std::invalid_argument if simulation1 and simulation2 were constructed with different encryption parameters
        @see Evaluator::multiply() for the corresponding operation on ciphertexts.
        */
        Simulation multiply(const Simulation &simulation1, const Simulation &simulation2);

        /**
        Simulates inherent noise growth in Evaluator::add() and returns the result.

        @param[in] simulation1 The first Simulation object to add
        @param[in] simulation2 The second Simulation object to add
        @throws std::invalid_argument if simulation1 and simulation2 were constructed with different encryption parameters
        @see Evaluator::add() for the corresponding operation on ciphertexts.
        */
        Simulation add(const Simulation &simulation1, const Simulation &simulation2);

        /**
        Simulates inherent noise growth in Evaluator::add_many() and returns the result.

        @param[in] simulations The simulations to add
        @throws std::invalid_argument if simulations is empty
        @throws std::invalid_argument if not all elements of simulations were constructed with the same encryption parameters
        @see Evaluator::add_many() for the corresponding operation on ciphertexts.
        */
        Simulation add_many(const std::vector<Simulation> &simulations);

        /**
        Simulates inherent noise growth in Evaluator::sub() and returns the result.

        @param[in] simulation1 The Simulation object to subtract from
        @param[in] simulation2 The Simulation object to subtract
        @throws std::invalid_argument if simulation1 and simulation2 were constructed with different encryption parameters
        @see Evaluator::sub() for the corresponding operation on ciphertexts.
        */
        Simulation sub(const Simulation &simulation1, const Simulation &simulation2);

        /**
        Simulates inherent noise growth in Evaluator::multiply_plain() given an upper bound for the maximum number of
        non-zero coefficients and an upper bound for their absolute value (represented by BigUInt) in the encoding of
        the plaintext multiplier and returns the result.

        @param[in] simulation The Simulation object to multiply
        @param[in] plain_max_coeff_count An upper bound on the number of non-zero coefficients in the plain polynomial to multiply
        @param[in] plain_max_abs_value An upper bound (represented by BigUInt) on the absolute value of coefficients in the plain polynomial to multiply
        @throws std::invalid_argument if plain_max_coeff_count is out of range
        @see Evaluator::multiply_plain() for the corresponding operation on ciphertexts.
        */
        Simulation multiply_plain(const Simulation &simulation, int plain_max_coeff_count, const BigUInt &plain_max_abs_value);

        /**
        Simulates inherent noise growth in Evaluator::multiply_plain() given an upper bound for the maximum number of
        non-zero coefficients and an upper bound for their absolute value (represented by std::uint64_t) in the encoding of
        the plain-text multiplier and returns the result.

        @param[in] simulation The Simulation object to multiply
        @param[in] plain_max_coeff_count An upper bound on the number of non-zero coefficients in the plain polynomial to multiply
        @param[in] plain_max_abs_value An upper bound (represented by std::uint64_t) on the absolute value of coefficients in the plain polynomial to multiply
        @throws std::invalid_argument if plain_max_coeff_count is out of range
        @see Evaluator::multiply_plain() for the corresponding operation on ciphertexts.
        */
        Simulation multiply_plain(const Simulation &simulation, int plain_max_coeff_count, uint64_t plain_max_abs_value);

        /**
        Simulates inherent noise growth in Evaluator::add_plain() and returns the result.

        @param[in] simulation The Simulation object to add to
        @see Evaluator::add_plain() for the corresponding operation on ciphertexts.
        */
        Simulation add_plain(const Simulation &simulation);

        /**
        Simulates inherent noise growth in Evaluator::sub_plain() and returns the result.

        @param[in] simulation The Simulation object to subtract from
        @see Evaluator::sub_plain() for the corresponding operation on ciphertexts.
        */
        Simulation sub_plain(const Simulation &simulation);

        /**
        Simulates inherent noise growth in Evaluator::exponentiate() and returns the result.

        @param[in] simulation The Simulation object to raise to a power
        @param[in] exponent The non-negative power to raise the Simulation object to
        @throws std::invalid_argument if the exponent is negative
        @see Evaluator::exponentiate() for the corresponding operation on ciphertexts.
        */
        Simulation exponentiate(const Simulation &simulation, int exponent);

        /**
        Simulates inherent noise growth in Evaluator::negate() and returns the result.

        @param[in] simulation The Simulation object to negate
        @see Evaluator::negate() for the corresponding operation on ciphertexts.
        */
        Simulation negate(const Simulation &simulation);

    private:
        seal::util::MemoryPool pool_;

        /**
        Simulates inherent noise growth in Evaluator::multiply_norelin() and returns the result.

        @par THIS FUNCTION IS BROKEN
        This function is broken and can not be used except together with relinearize().
        The problem is that multiplication without relinearization also adds an additive term,
        which this function omits, and that additive term depends on under what powers of
        the key the ciphertexts decrypt.

        @param[in] simulation1 The first Simulation object to multiply
        @param[in] simulation2 The second Simulation object to multiply
        @throws std::invalid_argument if simulation1 and simulation2 were constructed with different encryption parameters
        @see Evaluator::multiply_norelin() for the corresponding operation on ciphertexts.
        */
        Simulation multiply_norelin(const Simulation &simulation1, const Simulation &simulation2);

        /**
        Simulates inherent noise growth in Evaluator::relinearize() and returns the result.

        @par THIS FUNCTION IS BROKEN
        This function is broken and can not be used except together with multiply_norelin().
        The problem is that relinearization adds additive terms that may be significant
        depending on under what power of the secret key the result decrypts.
        This function omits those terms.

        @param[in] simulation The Simulation object to relinearize
        @see Evaluator::relinearize() for the corresponding operation on ciphertexts.
        */
        Simulation relinearize(const Simulation &simulation);
    };
}


#endif // SEAL_SIMULATOR_H
