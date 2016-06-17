#include <algorithm>
#include "simulator.h"
#include "utilities.h"
#include "util/uintarith.h"
#include "util/polyarith.h"
#include <cmath>

using namespace std;
using namespace seal::util;

namespace seal
{
    namespace
    {
        bool are_poly_coefficients_less_than(const BigPoly &poly, const BigUInt &max_coeff)
        {
            return util::are_poly_coefficients_less_than(poly.pointer(), poly.coeff_count(), poly.coeff_uint64_count(), max_coeff.pointer(), max_coeff.uint64_count());
        }

        uint64_t exponentiate_uint(uint64_t base, uint64_t exponent)
        {
            if (exponent == 0)
            {
                return 1;
            }
            uint64_t result = base;
            uint64_t prev_result = 0;
            for (int i = 0; i < exponent - 1; ++i)
            {
                if (result < prev_result)
                {
                    throw invalid_argument("uint64 overflow");
                }
                prev_result = result;
                result *= base;
            }
            return result;
        }
    }

    Simulation SimulationEvaluator::add(const Simulation &simulation1, const Simulation &simulation2)
    {
        if (!simulation1.compare_encryption_parameters(simulation2))
        {
            throw invalid_argument("mismatch in encryption parameters");
        }
        if (simulation1.ciphertext_size_ < 2 || simulation2.ciphertext_size_ < 2)
        {
            throw invalid_argument("simulation has invalid ciphertext size");
        }

        // Find the largest of the noises
        MemoryPool &pool = *MemoryPool::default_pool();
        int result_bit_count;
        int result_uint64_count;

        int max_ciphertext_size = max(simulation1.ciphertext_size_, simulation2.ciphertext_size_);

        if (compare_uint_uint(simulation1.noise_.pointer(), simulation1.noise_.uint64_count(), simulation2.noise_.pointer(), simulation2.noise_.uint64_count()) > 0)
        {
            result_bit_count = max(simulation1.noise_.significant_bit_count() + 1, simulation1.plain_modulus_.significant_bit_count()) + 1;
            result_uint64_count = divide_round_up(result_bit_count, bits_per_uint64);

            Pointer remainder(allocate_uint(result_uint64_count, pool));
            Pointer quotient(allocate_uint(result_uint64_count, pool));
            divide_uint_uint(simulation1.coeff_modulus_.pointer(), simulation1.plain_modulus_.pointer(), result_uint64_count, quotient.get(), remainder.get(), pool);

            Pointer result(allocate_uint(result_uint64_count, pool));
            set_uint_uint(simulation1.noise_.pointer(), simulation1.noise_.uint64_count(), result_uint64_count, result.get());
            left_shift_uint(result.get(), 1, result_uint64_count, result.get());
            add_uint_uint(result.get(), remainder.get(), result_uint64_count, result.get());

            return Simulation(BigUInt(result_bit_count, result.get()), simulation1.max_noise_, simulation1.coeff_modulus_, simulation1.plain_modulus_, simulation1.poly_modulus_coeff_count_, simulation1.noise_standard_deviation_, simulation1.noise_max_deviation_, simulation1.decomposition_bit_count_, max_ciphertext_size);
        }

        result_bit_count = max(simulation2.noise_.significant_bit_count() + 1, simulation2.plain_modulus_.significant_bit_count()) + 1;
        result_uint64_count = divide_round_up(result_bit_count, bits_per_uint64);

        Pointer remainder(allocate_uint(result_uint64_count, pool));
        Pointer quotient(allocate_uint(result_uint64_count, pool));
        divide_uint_uint(simulation1.coeff_modulus_.pointer(), simulation1.plain_modulus_.pointer(), result_uint64_count, quotient.get(), remainder.get(), pool);

        Pointer result(allocate_uint(result_uint64_count, pool));
        set_uint_uint(simulation2.noise_.pointer(), simulation2.noise_.uint64_count(), result_uint64_count, result.get());
        left_shift_uint(result.get(), 1, result_uint64_count, result.get());
        add_uint_uint(result.get(), remainder.get(), result_uint64_count, result.get());
        
        return Simulation(BigUInt(result_bit_count, result.get()), simulation1.max_noise_, simulation1.coeff_modulus_, simulation1.plain_modulus_, simulation1.poly_modulus_coeff_count_, simulation1.noise_standard_deviation_, simulation1.noise_max_deviation_, simulation1.decomposition_bit_count_, max_ciphertext_size);
    }

    Simulation SimulationEvaluator::add_many(const vector<Simulation> &simulations)
    {
        if (simulations.empty())
        {
            throw invalid_argument("simulations vector cannot be empty");
        }
        for (size_t i = 0; i < simulations.size(); ++i)
        {
            if (simulations[i].ciphertext_size_ < 2)
            {
                throw invalid_argument("simulation has invalid ciphertext size");
            }
            if (!simulations[i].compare_encryption_parameters(simulations[0]))
            {
                throw invalid_argument("mismatch in encryption parameters");
            }
        }

        // Find the largest of the noises
        vector<Simulation>::size_type largest_noise_index = 0;
        int largest_ciphertext_size = simulations[0].ciphertext_size_;
        for (size_t i = 1; i < simulations.size(); ++i)
        {
            if (compare_uint_uint(simulations[i].noise_.pointer(), simulations[i].noise_.uint64_count(), simulations[largest_noise_index].noise_.pointer(), simulations[largest_noise_index].noise_.uint64_count()) > 0)
            {
                largest_noise_index = i;
            }

            if (simulations[i].ciphertext_size_ > largest_ciphertext_size)
            {
                largest_ciphertext_size = simulations[i].ciphertext_size_;
            }
        }

        // Determine the size of result
        uint64_t scaling_coeff = static_cast<uint64_t>(simulations.size());
        uint64_t scaling_coeff_minus1 = static_cast<uint64_t>(simulations.size() - 1);

        int result_bit_count = max(get_significant_bit_count(scaling_coeff) + simulations[largest_noise_index].noise_.significant_bit_count(), get_significant_bit_count(scaling_coeff_minus1) + simulations[largest_noise_index].plain_modulus_.significant_bit_count()) + 1;
        int result_uint64_count = divide_round_up(result_bit_count, bits_per_uint64);

        MemoryPool &pool = *MemoryPool::default_pool();
        Pointer result(allocate_zero_uint(result_uint64_count, pool));

        // Put scaling_coeff * simulations[largest_noise_index] into result
        multiply_uint_uint(simulations[largest_noise_index].noise_.pointer(), simulations[largest_noise_index].noise_.uint64_count(), &scaling_coeff, 1, result_uint64_count, result.get());

        // Calculate remainder
        Pointer remainder(allocate_uint(result_uint64_count, pool));
        Pointer quotient(allocate_uint(result_uint64_count, pool));
        divide_uint_uint(simulations[largest_noise_index].coeff_modulus_.pointer(), simulations[largest_noise_index].plain_modulus_.pointer(), result_uint64_count, quotient.get(), remainder.get(), pool);
        
       // Calculate (scaling_coeff - 1) * remainder. Store in quotient, since we don't use this again now that remainder is calculated.
        multiply_uint_uint(&scaling_coeff_minus1, 1, remainder.get(), result_uint64_count, result_uint64_count, quotient.get());

        // Add (scaling_coeff - 1) * remainder into result
        add_uint_uint(quotient.get(), result.get(), result_uint64_count, result.get());

        return Simulation(BigUInt(result_bit_count, result.get()), simulations[0].max_noise_, simulations[0].coeff_modulus_, simulations[0].plain_modulus_, simulations[0].poly_modulus_coeff_count_, simulations[0].noise_standard_deviation_, simulations[0].noise_max_deviation_, simulations[0].decomposition_bit_count_, largest_ciphertext_size);
    }

    Simulation SimulationEvaluator::sub(const Simulation &simulation1, const Simulation &simulation2)
    {
        return add(simulation1, simulation2);
    }

    Simulation SimulationEvaluator::relinearize(const Simulation &simulation, int destination_size)
    {
        //check destination_size is reasonable
        if (destination_size < 2 || destination_size > simulation.ciphertext_size_)
        {
            throw invalid_argument("cannot relinearize to destination_size");
        }
        if (simulation.ciphertext_size_ < 2)
        {
            throw invalid_argument("simulation has invalid ciphertext size");
        }

        // Determine number of relinearize_one_step calls which would be needed
        uint64_t relinearize_one_step_calls = static_cast<uint64_t>(simulation.ciphertext_size_ - destination_size);
        if (relinearize_one_step_calls == 0)
        {
            return simulation;
        }
        int relinearize_one_step_bit_count = get_significant_bit_count(relinearize_one_step_calls);

        // Take delta = sqrt(n)
        uint64_t growth_factor = static_cast<uint64_t>(ceil(sqrt(simulation.poly_modulus_coeff_count_) - 1));
        int growth_factor_bit_count = get_significant_bit_count(growth_factor);

        // Determine size of B_err
        uint64_t error_max = static_cast<uint64_t>(std::ceil(simulation.noise_max_deviation_));
        int error_max_bit_count = get_significant_bit_count(error_max);

        // Determine result_bit_count.
        int relinearize_bit_count = relinearize_one_step_bit_count + growth_factor_bit_count + error_max_bit_count + (simulation.coeff_modulus_.significant_bit_count() / simulation.decomposition_bit_count_ + 1) + simulation.decomposition_bit_count_;
        int result_bit_count = max(simulation.noise_.significant_bit_count(), relinearize_bit_count) + 1;
        int result_uint64_count = divide_round_up(result_bit_count, bits_per_uint64);

        // Calculate w
        MemoryPool &pool = *MemoryPool::default_pool();
        Pointer decomposition_coeff(allocate_zero_uint(result_uint64_count, pool));
        *decomposition_coeff.get() = 1;
        left_shift_uint(decomposition_coeff.get(), simulation.decomposition_bit_count_ - 1, result_uint64_count, decomposition_coeff.get());

        Pointer result(allocate_uint(result_uint64_count, pool));
        uint64_t temp1;
        Pointer temp2(allocate_uint(result_uint64_count, pool));

        // Set temp1 equal to relinearize_one_step_calls * delta * B_err * (l+1)
        temp1 = relinearize_one_step_calls * growth_factor * static_cast<uint64_t>(ceil(simulation.noise_max_deviation_)) * (simulation.coeff_modulus_.significant_bit_count() / simulation.decomposition_bit_count_ + 1);

        // Set temp2 equal to temp1 * w
        multiply_uint_uint(&temp1, 1, decomposition_coeff.get(), result_uint64_count, result_uint64_count, temp2.get());

        // Now add to this the original noise (relinearization contributed an additive term)
        add_uint_uint(temp2.get(), result_uint64_count, simulation.noise_.pointer(), simulation.noise_.uint64_count(), false, result_uint64_count, result.get());

        return Simulation(BigUInt(result_bit_count, result.get()), simulation.max_noise_, simulation.coeff_modulus_, simulation.plain_modulus_, simulation.poly_modulus_coeff_count_, simulation.noise_standard_deviation_, simulation.noise_max_deviation_, simulation.decomposition_bit_count_, destination_size);
    }

    Simulation SimulationEvaluator::multiply(const Simulation &simulation1, const Simulation &simulation2)
    {
        // Verify that both simulations have the same encryption parameters
        if (!simulation1.compare_encryption_parameters(simulation2))
        {
            throw invalid_argument("mismatch in encryption parameters");
        }
        if (simulation1.ciphertext_size_ < 2 || simulation2.ciphertext_size_ < 2)
        {
            throw invalid_argument("simulation has invalid ciphertext size");
        }

        const BigUInt &plain_modulus = simulation1.plain_modulus_;
        uint64_t poly_modulus_degree = static_cast<uint64_t>(simulation1.poly_modulus_coeff_count_) - 1;

        // This makes sense in the case of encoding/encrypting relatively small integers using
        // binary or balanced encoders, but does not make sense in the context of PolyCRTBuilder or
        // possibly fractional encoder, in which case it should be closer to n, or really the number of
        // non-zero coefficients in the underlying plaintext.
        uint64_t plain_growth_factor = 1;

        // Determine new size
        int result_ciphertext_size = simulation1.ciphertext_size_ + simulation2.ciphertext_size_ - 1;

        // Compute ceil(sqrt(2n/3))^(k-1)
        // This throws invalid argument if the result does not fit in a uint64
        uint64_t sqrt_factor = exponentiate_uint(static_cast<uint64_t>(ceil(sqrt((2.0 * poly_modulus_degree) / 3))), result_ciphertext_size - 2);

        // Determine size of the sum of the input noises
        int input_noise_sum_bit_count = max(simulation1.noise_.significant_bit_count(), simulation2.noise_.significant_bit_count()) + 1;
        int result_bit_count = max(input_noise_sum_bit_count, plain_modulus.significant_bit_count()) + 1
            + get_significant_bit_count(plain_growth_factor)
            + plain_modulus.significant_bit_count()
            + result_ciphertext_size - 1
            + get_significant_bit_count(sqrt_factor);
        int result_uint64_count = divide_round_up(result_bit_count, bits_per_uint64);

        MemoryPool &pool = *MemoryPool::default_pool();
        Pointer temp1(allocate_uint(result_uint64_count, pool));
        Pointer temp2(allocate_uint(result_uint64_count, pool));
        Pointer temp3(allocate_uint(result_uint64_count, pool));
        Pointer result(allocate_zero_uint(result_uint64_count, pool));

        // We need to widen coeff_modulus and plain_modulus
        Pointer wide_coeff_modulus(allocate_uint(result_uint64_count, pool));
        Pointer wide_plain_modulus(allocate_uint(result_uint64_count, pool));
        set_uint_uint(simulation1.coeff_modulus_.pointer(), simulation1.coeff_modulus_.uint64_count(), result_uint64_count, wide_coeff_modulus.get());
        set_uint_uint(simulation1.plain_modulus_.pointer(), simulation1.plain_modulus_.uint64_count(), result_uint64_count, wide_plain_modulus.get());

        // Compute Delta and r_t(q)
        Pointer quotient(allocate_uint(result_uint64_count, pool));
        Pointer remainder(allocate_uint(result_uint64_count, pool));
        divide_uint_uint(wide_coeff_modulus.get(), wide_plain_modulus.get(), result_uint64_count, quotient.get(), remainder.get(), pool);

        set_uint(sqrt_factor, result_uint64_count, temp1.get());
        left_shift_uint(temp1.get(), result_ciphertext_size - 1, result_uint64_count, temp1.get());
        multiply_uint_uint(temp1.get(), result_uint64_count, plain_modulus.pointer(), plain_modulus.uint64_count(), result_uint64_count, temp2.get());
        multiply_uint_uint(&plain_growth_factor, 1, temp2.get(), result_uint64_count, result_uint64_count, temp3.get());

        // Compute sum of noises plus r_t(q)
        // We reuse temp1 and temp2
        int noise1_bit_count = simulation1.noise_.significant_bit_count();
        int noise2_bit_count = simulation2.noise_.significant_bit_count();
        int noise1_uint64_count = divide_round_up(noise1_bit_count, bits_per_uint64);
        int noise2_uint64_count = divide_round_up(noise2_bit_count, bits_per_uint64);
        add_uint_uint(simulation1.noise_.pointer(), noise1_uint64_count, simulation2.noise_.pointer(), noise2_uint64_count, false, result_uint64_count, temp1.get());
        add_uint_uint(temp1.get(), remainder.get(), result_uint64_count, temp2.get());

        // Do final multiplication
        multiply_uint_uint(temp2.get(), result_uint64_count, temp3.get(), result_uint64_count, result_uint64_count, result.get());

        return Simulation(BigUInt(result_bit_count, result.get()), simulation1.max_noise_, simulation1.coeff_modulus_, simulation1.plain_modulus_, simulation1.poly_modulus_coeff_count_, simulation1.noise_standard_deviation_, simulation1.noise_max_deviation_, simulation1.decomposition_bit_count_, result_ciphertext_size);
    }

    Simulation SimulationEvaluator::multiply_plain(const Simulation &simulation, int plain_max_coeff_count, const BigUInt &plain_max_abs_value)
    {
        if (simulation.ciphertext_size_ < 2)
        {
            throw invalid_argument("simulation has invalid ciphertext size");
        }
        if (plain_max_coeff_count >= simulation.poly_modulus_coeff_count_ || plain_max_coeff_count <= 0)
        {
            throw invalid_argument("plain_max_coeff_count out of range");
        }

        // Disallow multiply_plain by the zero plaintext
        if (plain_max_abs_value.is_zero() || plain_max_coeff_count == 0)
        {
            throw invalid_argument("plaintext multiplier cannot be zero");
        }

        MemoryPool &pool = *MemoryPool::default_pool();
        
        uint64_t temp1 = static_cast<uint64_t>(plain_max_coeff_count);

        // Result size should be max(additive term, multiplicative term) + 1
        int additive_bit_count = simulation.plain_modulus_.significant_bit_count() + plain_max_abs_value.significant_bit_count() + get_significant_bit_count(temp1);
        int multiplicative_bit_count = simulation.noise_.significant_bit_count() + get_significant_bit_count(temp1) + plain_max_abs_value.significant_bit_count();
        int result_bit_count = max(additive_bit_count, multiplicative_bit_count) + 1;
        int result_uint64_count = divide_round_up(result_bit_count, bits_per_uint64);
        
        Pointer temp2(allocate_uint(result_uint64_count, pool));
        Pointer result(allocate_uint(result_uint64_count, pool));

        // Calculate multiplicative term and store in result
        multiply_uint_uint(&temp1, 1, simulation.noise_.pointer(), simulation.noise_.uint64_count(), result_uint64_count, temp2.get());
        multiply_uint_uint(temp2.get(), result_uint64_count, plain_max_abs_value.pointer(), plain_max_abs_value.uint64_count(), result_uint64_count, result.get());

        // Add in additive term
        Pointer remainder(allocate_uint(result_uint64_count, pool));
        Pointer temp3(allocate_uint(result_uint64_count, pool));
        Pointer temp4(allocate_uint(result_uint64_count, pool));

        // Calculate remainder
        divide_uint_uint(simulation.coeff_modulus_.pointer(), simulation.plain_modulus_.pointer(), result_uint64_count, temp3.get(), remainder.get(), pool);

        // Store N * remainder in temp3
        multiply_uint_uint(&temp1, 1, remainder.get(), result_uint64_count, result_uint64_count, temp3.get());

        // Store N * remainder * plain_max_abs_value in temp4
        multiply_uint_uint(temp3.get(), result_uint64_count, plain_max_abs_value.pointer(), plain_max_abs_value.uint64_count(), result_uint64_count, temp4.get());

        // Divide by 2
        right_shift_uint(temp4.get(), 1, result_uint64_count, temp4.get());

        // Add into result
        add_uint_uint(temp4.get(), result.get(), result_uint64_count, result.get());

        return Simulation(BigUInt(result_bit_count, result.get()), simulation.max_noise_, simulation.coeff_modulus_, simulation.plain_modulus_, simulation.poly_modulus_coeff_count_, simulation.noise_standard_deviation_, simulation.noise_max_deviation_, simulation.decomposition_bit_count_, simulation.ciphertext_size_);
    }

    Simulation SimulationEvaluator::multiply_plain(const Simulation &simulation, int plain_max_coeff_count, uint64_t plain_max_abs_value)
    {
        BigUInt plain_max_abs_value_uint;
        plain_max_abs_value_uint = plain_max_abs_value;
        return multiply_plain(simulation, plain_max_coeff_count, plain_max_abs_value_uint);
    }

    Simulation SimulationEvaluator::add_plain(const Simulation &simulation)
    {
        if (simulation.ciphertext_size_ < 2)
        {
            throw invalid_argument("simulation has invalid ciphertext size");
        }

        int result_bit_count = max(simulation.plain_modulus_.significant_bit_count(), simulation.noise_.significant_bit_count()) + 1;
        int result_uint64_count = divide_round_up(result_bit_count, bits_per_uint64);

        MemoryPool &pool = *MemoryPool::default_pool();
        Pointer remainder(allocate_uint(result_uint64_count, pool));
        Pointer quotient(allocate_uint(result_uint64_count, pool));
        divide_uint_uint(simulation.coeff_modulus_.pointer(), simulation.plain_modulus_.pointer(), result_uint64_count, quotient.get(), remainder.get(), pool);

        Pointer result(allocate_uint(result_uint64_count, pool));
        add_uint_uint(simulation.noise_.pointer(), simulation.noise_.uint64_count(), remainder.get(), result_uint64_count, false, result_uint64_count, result.get());

        return Simulation(BigUInt(result_bit_count, result.get()), simulation.max_noise_, simulation.coeff_modulus_, simulation.plain_modulus_, simulation.poly_modulus_coeff_count_, simulation.noise_standard_deviation_, simulation.noise_max_deviation_, simulation.decomposition_bit_count_, simulation.ciphertext_size_);
    }

    Simulation SimulationEvaluator::sub_plain(const Simulation &simulation)
    {
        return add_plain(simulation);
    }

    Simulation SimulationEvaluator::multiply_many(vector<Simulation> simulations)
    {
        size_t original_size = simulations.size();

        // Verify parameters.
        if (original_size == 0)
        {
            throw invalid_argument("simulations vector cannot be empty");
        }
        for (size_t i = 0; i < original_size; ++i)
        {
            if (simulations[i].ciphertext_size_ < 2)
            {
                throw invalid_argument("simulation has invalid ciphertext size");
            }
        }

        // Return quickly if there is nothing to do.
        if (original_size == 1)
        {
            return simulations[0];
        }

        // Repeatedly multiply and add to the back of the vector until the end is reached.
        // We will detect here if one of the elements of the simulations vector was constructed with
        // different encryption parameters than the others.
        for (size_t i = 0; i < simulations.size() - 1; i += 2)
        {
            simulations.push_back(multiply(simulations[i], simulations[i + 1]));
        }

        return simulations[simulations.size() - 1];
    }

    Simulation SimulationEvaluator::exponentiate(const Simulation &simulation, uint64_t exponent)
    {
        if (simulation.ciphertext_size_ < 2)
        {
            throw invalid_argument("simulation has invalid ciphertext size");
        }
        if (exponent == 0)
        {
            throw invalid_argument("exponent cannot be zero");
        }

        // Fast path
        if (exponent == 1)
        {
            return simulation;
        }

        vector<Simulation> exp_vector(exponent, simulation);
        return multiply_many(exp_vector);

        // Binary exponentiation
        /*
        if (exponent % 2 == 0)
        {
            return exponentiate(multiply(simulation, simulation), exponent >> 1);
        }
        return multiply(exponentiate(multiply(simulation, simulation), (exponent - 1) >> 1), simulation);
        */
    }

    Simulation SimulationEvaluator::negate(const Simulation &simulation)
    {
        if (simulation.ciphertext_size_ < 2)
        {
            throw invalid_argument("simulation has invalid ciphertext size");
        }
        return simulation;
    }

    void Simulation::set_initial_noise_estimate()
    {
        uint64_t growth_factor = static_cast<uint64_t>(sqrt(2 * (poly_modulus_coeff_count_ - 1) / 3));
        uint64_t temp = 2 * growth_factor * static_cast<uint64_t>(noise_max_deviation_);

        int result_bit_count = get_significant_bit_count(temp);
        int result_uint64_count = divide_round_up(result_bit_count, bits_per_uint64);

        noise_.resize(result_bit_count);
        set_uint_uint(&temp, 1, result_uint64_count, noise_.pointer());
    }

    Simulation::Simulation(const EncryptionParameters &parms) : max_noise_(inherent_noise_max(parms)), coeff_modulus_(parms.coeff_modulus()),
        plain_modulus_(parms.plain_modulus()), poly_modulus_coeff_count_(parms.poly_modulus().coeff_count()), noise_standard_deviation_(parms.noise_standard_deviation()),
        noise_max_deviation_(parms.noise_max_deviation()), decomposition_bit_count_(parms.decomposition_bit_count()), ciphertext_size_(0)
    {
        // Verify encryption parameters are non-zero and non-nullptr.
        if (parms.poly_modulus().is_zero())
        {
            throw invalid_argument("poly_modulus cannot be zero");
        }
        if (parms.coeff_modulus().is_zero())
        {
            throw invalid_argument("coeff_modulus cannot be zero");
        }
        if (parms.plain_modulus().is_zero())
        {
            throw invalid_argument("plain_modulus cannot be zero");
        }
        if (parms.noise_standard_deviation() < 0)
        {
            throw invalid_argument("noise_standard_deviation must be non-negative");
        }
        if (parms.noise_max_deviation() < 0)
        {
            throw invalid_argument("noise_max_deviation must be non-negative");
        }

        // Verify parameters.
        if (parms.plain_modulus() >= parms.coeff_modulus())
        {
            throw invalid_argument("plain_modulus must be smaller than coeff_modulus");
        }
        if (!are_poly_coefficients_less_than(parms.poly_modulus(), parms.coeff_modulus()))
        {
            throw invalid_argument("poly_modulus cannot have coefficients larger than coeff_modulus");
        }

        // Set noise
        set_initial_noise_estimate();

        // Set ciphertext_size_
        ciphertext_size_ = 2;
    }

    bool Simulation::decrypts(int noise_gap) const
    {
        if (noise_gap < 0)
        {
            throw std::invalid_argument("noise_gap cannot be negative");
        }
        return (noise_bits_left() >= noise_gap);
    }

    Simulation::Simulation(const EncryptionParameters &parms, const BigUInt &noise, int ciphertext_size) : noise_(noise),
        max_noise_(inherent_noise_max(parms)), coeff_modulus_(parms.coeff_modulus()),
        plain_modulus_(parms.plain_modulus()), poly_modulus_coeff_count_(parms.poly_modulus().coeff_count()), noise_standard_deviation_(parms.noise_standard_deviation()),
        noise_max_deviation_(parms.noise_max_deviation()), decomposition_bit_count_(parms.decomposition_bit_count()), ciphertext_size_(ciphertext_size)
    {
        // Verify encryption parameters are non-zero and non-nullptr.
        if (parms.poly_modulus().is_zero())
        {
            throw invalid_argument("poly_modulus cannot be zero");
        }
        if (parms.coeff_modulus().is_zero())
        {
            throw invalid_argument("coeff_modulus cannot be zero");
        }
        if (parms.plain_modulus().is_zero())
        {
            throw invalid_argument("plain_modulus cannot be zero");
        }
        if (parms.noise_standard_deviation() < 0)
        {
            throw invalid_argument("noise_standard_deviation must be non-negative");
        }
        if (parms.noise_max_deviation() < 0)
        {
            throw invalid_argument("noise_max_deviation must be non-negative");
        }

        // Verify parameters.
        if (parms.plain_modulus() >= parms.coeff_modulus())
        {
            throw invalid_argument("plain_modulus must be smaller than coeff_modulus");
        }
        if (!are_poly_coefficients_less_than(parms.poly_modulus(), parms.coeff_modulus()))
        {
            throw invalid_argument("poly_modulus cannot have coefficients larger than coeff_modulus");
        }
        if (noise > parms.coeff_modulus())
        {
            throw invalid_argument("noise can not be bigger than coeff_modulus");
        }
        if (ciphertext_size < 2)
        {
            throw invalid_argument("ciphertext_size must be greater than or equal to 2");
        }
    }

    Simulation::Simulation(const BigUInt &noise, const BigUInt &max_noise, const BigUInt &coeff_modulus, const BigUInt &plain_modulus, int poly_modulus_coeff_count,
        double noise_standard_deviation, double noise_max_deviation, int decomposition_bit_count, int ciphertext_size) : noise_(noise), max_noise_(max_noise), coeff_modulus_(coeff_modulus),
        plain_modulus_(plain_modulus), poly_modulus_coeff_count_(poly_modulus_coeff_count), noise_standard_deviation_(noise_standard_deviation),
        noise_max_deviation_(noise_max_deviation), decomposition_bit_count_(decomposition_bit_count), ciphertext_size_(ciphertext_size)
    {
        // Verify encryption parameters are non-zero and non-nullptr.
        if (poly_modulus_coeff_count <= 0)
        {
            throw invalid_argument("poly_modulus cannot be zero");
        }
        if (coeff_modulus.is_zero())
        {
            throw invalid_argument("coeff_modulus cannot be zero");
        }
        if (plain_modulus.is_zero())
        {
            throw invalid_argument("plain_modulus cannot be zero");
        }
        if (noise_standard_deviation_ < 0)
        {
            throw invalid_argument("noise_standard_deviation must be non-negative");
        }
        if (noise_max_deviation_ < 0)
        {
            throw invalid_argument("noise_max_deviation must be non-negative");
        }

        // Verify parameters.
        if (plain_modulus >= coeff_modulus)
        {
            throw invalid_argument("plain_modulus must be smaller than coeff_modulus");
        }
        if (ciphertext_size < 2)
        {
            throw invalid_argument("ciphertext_size must be greater than or equal to 2");
        }
    }
}