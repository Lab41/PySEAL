#include <algorithm>
#include "simulator.h"
#include "utilities.h"
#include "util/uintarith.h"
#include "util/polyarith.h"

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
    }

    Simulation SimulationEvaluator::add(const Simulation &simulation1, const Simulation &simulation2)
    {
        if (!simulation1.compare_encryption_parameters(simulation2))
        {
            throw invalid_argument("mismatch in encryption parameters");
        }

        // Find the largest of the noises
        int result_bit_count;
        int result_uint64_count;
        if (compare_uint_uint(simulation1.noise_.pointer(), simulation1.noise_.uint64_count(), simulation2.noise_.pointer(), simulation2.noise_.uint64_count()) > 0)
        {
            result_bit_count = simulation1.noise_.significant_bit_count() + 1;
            result_uint64_count = divide_round_up(result_bit_count, bits_per_uint64);
            Pointer result(allocate_uint(result_uint64_count, pool_));
            set_uint_uint(simulation1.noise_.pointer(), simulation1.noise_.uint64_count(), result_uint64_count, result.get());
            left_shift_uint(result.get(), 1, result_uint64_count, result.get());

            return Simulation(BigUInt(result_bit_count, result.get()), simulation1.max_noise_, simulation1.coeff_modulus_, simulation1.plain_modulus_, simulation1.poly_modulus_coeff_count_, simulation1.noise_standard_deviation_, simulation1.noise_max_deviation_, simulation1.decomposition_bit_count_);
        }

        result_bit_count = simulation2.noise_.significant_bit_count() + 1;
        result_uint64_count = divide_round_up(result_bit_count, bits_per_uint64);
        Pointer result(allocate_uint(result_uint64_count, pool_));
        set_uint_uint(simulation2.noise_.pointer(), simulation2.noise_.uint64_count(), result_uint64_count, result.get());
        left_shift_uint(result.get(), 1, result_uint64_count, result.get());

        return Simulation(BigUInt(result_bit_count, result.get()), simulation1.max_noise_, simulation1.coeff_modulus_, simulation1.plain_modulus_, simulation1.poly_modulus_coeff_count_, simulation1.noise_standard_deviation_, simulation1.noise_max_deviation_, simulation1.decomposition_bit_count_);
    }

    Simulation SimulationEvaluator::add_many(const vector<Simulation> &simulations)
    {
        if (simulations.empty())
        {
            throw invalid_argument("simulations vector cannot be empty");
        }

        // Find the largest of the noises
        vector<Simulation>::size_type largest_noise_index = 0;
        for (vector<Simulation>::size_type i = 1; i < simulations.size(); ++i)
        {
            if (!simulations[i].compare_encryption_parameters(simulations[0]))
            {
                throw invalid_argument("mismatch in encryption parameters");
            }

            if (compare_uint_uint(simulations[i].noise_.pointer(), simulations[i].noise_.uint64_count(), simulations[largest_noise_index].noise_.pointer(), simulations[largest_noise_index].noise_.uint64_count()) > 0)
            {
                largest_noise_index = i;
            }
        }

        uint64_t scaling_coeff = static_cast<uint64_t>(sqrt(simulations.size())) + 1;
        int result_bit_count = get_significant_bit_count(scaling_coeff) + simulations[largest_noise_index].noise_.significant_bit_count();
        int result_uint64_count = divide_round_up(result_bit_count, bits_per_uint64);

        Pointer result(allocate_zero_uint(result_uint64_count, pool_));
        multiply_uint_uint(simulations[largest_noise_index].noise_.pointer(), simulations[largest_noise_index].noise_.uint64_count(), &scaling_coeff, 1, result_uint64_count, result.get());

        return Simulation(BigUInt(result_bit_count, result.get()), simulations[0].max_noise_, simulations[0].coeff_modulus_, simulations[0].plain_modulus_, simulations[0].poly_modulus_coeff_count_, simulations[0].noise_standard_deviation_, simulations[0].noise_max_deviation_, simulations[0].decomposition_bit_count_);
    }

    Simulation SimulationEvaluator::sub(const Simulation &simulation1, const Simulation &simulation2)
    {
        return add(simulation1, simulation2);
    }

    Simulation SimulationEvaluator::relinearize(const Simulation &simulation)
    {
        uint64_t growth_factor = static_cast<uint64_t>(sqrt(static_cast<double>(simulation.poly_modulus_coeff_count_ - 1) * 2 / 3));
        
        int result_bit_count = max(simulation.noise_.significant_bit_count(), simulation.plain_modulus_.significant_bit_count() + 32 + simulation.coeff_modulus_.significant_bit_count() / simulation.decomposition_bit_count_ + simulation.decomposition_bit_count_) + 1;
        int result_uint64_count = divide_round_up(result_bit_count, bits_per_uint64);

        Pointer decomposition_coeff(allocate_zero_uint(result_uint64_count, pool_));
        *decomposition_coeff.get() = 1;
        left_shift_uint(decomposition_coeff.get(), simulation.decomposition_bit_count_, result_uint64_count, decomposition_coeff.get());

        Pointer result(allocate_zero_uint(result_uint64_count, pool_));
        uint64_t temp1;
        Pointer temp2(allocate_zero_uint(result_uint64_count, pool_));

        // Noise growth from relinearization
        temp1 = growth_factor * static_cast<uint64_t>(sqrt(simulation.poly_modulus_coeff_count_ - 1) * simulation.noise_max_deviation_ * (simulation.coeff_modulus_.significant_bit_count() / simulation.decomposition_bit_count_ + 2));
        multiply_uint_uint(&temp1, 1, simulation.plain_modulus_.pointer(), simulation.plain_modulus_.uint64_count(), result_uint64_count, temp2.get());
        multiply_uint_uint(temp2.get(), result_uint64_count, decomposition_coeff.get(), result_uint64_count, result_uint64_count, result.get());

        // Now add to this the original noise (relinearization contributed an additive term)
        add_uint_uint(result.get(), result_uint64_count, simulation.noise_.pointer(), simulation.noise_.uint64_count(), false, result_uint64_count, result.get());

        return Simulation(BigUInt(result_bit_count, result.get()), simulation.max_noise_, simulation.coeff_modulus_, simulation.plain_modulus_, simulation.poly_modulus_coeff_count_, simulation.noise_standard_deviation_, simulation.noise_max_deviation_, simulation.decomposition_bit_count_);
    }

    Simulation SimulationEvaluator::multiply_norelin(const Simulation &simulation1, const Simulation &simulation2)
    {
        // Verify that both simulations have the same encryption parameters
        if (!simulation1.compare_encryption_parameters(simulation2))
        {
            throw invalid_argument("mismatch in encryption parameters");
        }

        uint64_t growth_factor = static_cast<uint64_t>(sqrt(static_cast<double>(simulation1.poly_modulus_coeff_count_ - 1) * 2 / 3));

        int max_input_noise_sig_bit_count = max(simulation1.noise_.significant_bit_count(), simulation2.noise_.significant_bit_count());

        int result_bit_count = max_input_noise_sig_bit_count + 2*simulation1.plain_modulus_.significant_bit_count() + 32;
        int result_uint64_count = divide_round_up(result_bit_count, bits_per_uint64);
        Pointer result(allocate_zero_uint(result_uint64_count, pool_));

        int noise_sum_bit_count = max_input_noise_sig_bit_count + 1;
        int noise_sum_uint64_count = divide_round_up(noise_sum_bit_count, bits_per_uint64);
        Pointer noise_sum(allocate_uint(noise_sum_uint64_count, pool_));
        add_uint_uint(simulation1.noise_.pointer(), simulation2.noise_.pointer(), noise_sum_uint64_count, noise_sum.get());

        uint64_t temp1;
        Pointer temp2(allocate_uint(result_uint64_count, pool_));
        Pointer temp3(allocate_uint(result_uint64_count, pool_));

        // Noise growth from multiplication
        temp1 = growth_factor * (simulation1.poly_modulus_coeff_count_ - 1);
        multiply_uint_uint(&temp1, 1, simulation1.plain_modulus_.pointer(), simulation1.plain_modulus_.uint64_count(), result_uint64_count, temp2.get());
        multiply_uint_uint(temp2.get(), result_uint64_count, simulation1.plain_modulus_.pointer(), simulation1.plain_modulus_.uint64_count(), result_uint64_count, temp3.get());
        right_shift_uint(temp3.get(), 1, result_uint64_count, temp3.get());
        multiply_uint_uint(temp3.get(), result_uint64_count, noise_sum.get(), noise_sum_uint64_count, result_uint64_count, result.get());

        return Simulation(BigUInt(result_bit_count, result.get()), simulation1.max_noise_, simulation1.coeff_modulus_, simulation1.plain_modulus_, simulation1.poly_modulus_coeff_count_, simulation1.noise_standard_deviation_, simulation1.noise_max_deviation_, simulation1.decomposition_bit_count_);
    }

    Simulation SimulationEvaluator::multiply(const Simulation &simulation1, const Simulation &simulation2)
    {
        return relinearize(multiply_norelin(simulation1, simulation2));
    }

    Simulation SimulationEvaluator::multiply_plain(const Simulation &simulation, int plain_max_coeff_count, const BigUInt &plain_max_abs_value)
    {
        if (plain_max_coeff_count >= simulation.poly_modulus_coeff_count_ || plain_max_coeff_count <= 0)
        {
            throw invalid_argument("plain_max_coeff_count out of range");
        }
        if (simulation.noise_.is_zero())
        {
            return simulation;
        }
        if (plain_max_abs_value.is_zero())
        {            
            return Simulation(BigUInt(1, static_cast<uint64_t>(0)), simulation.max_noise_, simulation.coeff_modulus_, simulation.plain_modulus_, simulation.poly_modulus_coeff_count_, simulation.noise_standard_deviation_, simulation.noise_max_deviation_, simulation.decomposition_bit_count_);
        }

        int result_bit_count = simulation.noise_.significant_bit_count() + plain_max_abs_value.significant_bit_count() + 32;
        int result_uint64_count = divide_round_up(result_bit_count, bits_per_uint64);
        Pointer result(allocate_uint(result_uint64_count, pool_));

        uint64_t temp1;
        Pointer temp2(allocate_uint(result_uint64_count, pool_));

        temp1 = static_cast<uint64_t>(sqrt(static_cast<double>(plain_max_coeff_count)));
        multiply_uint_uint(&temp1, 1, simulation.noise_.pointer(), simulation.noise_.uint64_count(), result_uint64_count, temp2.get());
        multiply_uint_uint(temp2.get(), result_uint64_count, plain_max_abs_value.pointer(), plain_max_abs_value.uint64_count(), result_uint64_count, result.get());

        return Simulation(BigUInt(result_bit_count, result.get()), simulation.max_noise_, simulation.coeff_modulus_, simulation.plain_modulus_, simulation.poly_modulus_coeff_count_, simulation.noise_standard_deviation_, simulation.noise_max_deviation_, simulation.decomposition_bit_count_);
    }

    Simulation SimulationEvaluator::multiply_plain(const Simulation &simulation, int plain_max_coeff_count, uint64_t plain_max_abs_value)
    {
        BigUInt plain_max_abs_value_uint;
        plain_max_abs_value_uint = plain_max_abs_value;
        return multiply_plain(simulation, plain_max_coeff_count, plain_max_abs_value_uint);
    }

    Simulation SimulationEvaluator::add_plain(const Simulation &simulation)
    {
        return simulation;
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

        // Return quickly if there is nothing to do.
        if (original_size == 1)
        {
            return simulations[0];
        }

        // Repeatedly multiply and add to the back of the vector until the end is reached.
        // We will detect here if one of the elements of the simulations vector was constructed with
        // different encryption parameters than the others.
        for (vector<Simulation>::size_type i = 0; i < simulations.size() - 1; i += 2)
        {
            simulations.push_back(multiply(simulations[i], simulations[i + 1]));
        }

        return simulations[simulations.size() - 1];
    }

    Simulation SimulationEvaluator::exponentiate(const Simulation &simulation, int exponent)
    {
        if (exponent < 0)
        {
            throw invalid_argument("exponent must be non-negative");
        }
        if (exponent == 0)
        {
            return Simulation(BigUInt(1, static_cast<uint64_t>(0)), simulation.max_noise_, simulation.coeff_modulus_, simulation.plain_modulus_, simulation.poly_modulus_coeff_count_, simulation.noise_standard_deviation_, simulation.noise_max_deviation_, simulation.decomposition_bit_count_);
        }
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
        return simulation;
    }

    void Simulation::set_initial_noise_estimate()
    {
        uint64_t growth_factor = static_cast<uint64_t>(sqrt(2 * (poly_modulus_coeff_count_ - 1) / 3));
        uint64_t temp = 2 * growth_factor * static_cast<uint64_t>(noise_max_deviation_);

        int result_bit_count = plain_modulus_.significant_bit_count() + get_significant_bit_count(temp);
        int result_uint64_count = divide_round_up(result_bit_count, bits_per_uint64);

        noise_.resize(result_bit_count);
        
        multiply_uint_uint(&temp, 1, plain_modulus_.pointer(), plain_modulus_.uint64_count(), result_uint64_count, noise_.pointer());
    }

    Simulation::Simulation(const EncryptionParameters &parms) : max_noise_(inherent_noise_max(parms)), coeff_modulus_(parms.coeff_modulus()),
        plain_modulus_(parms.plain_modulus()), poly_modulus_coeff_count_(parms.poly_modulus().coeff_count()), noise_standard_deviation_(parms.noise_standard_deviation()),
        noise_max_deviation_(parms.noise_max_deviation()), decomposition_bit_count_(parms.decomposition_bit_count())
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

        set_initial_noise_estimate();
    }

    Simulation::Simulation(const EncryptionParameters &parms, const BigUInt &noise) : noise_(noise),
        max_noise_(inherent_noise_max(parms)), coeff_modulus_(parms.coeff_modulus()),
        plain_modulus_(parms.plain_modulus()), poly_modulus_coeff_count_(parms.poly_modulus().coeff_count()), noise_standard_deviation_(parms.noise_standard_deviation()),
        noise_max_deviation_(parms.noise_max_deviation()), decomposition_bit_count_(parms.decomposition_bit_count())
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
    }

    Simulation::Simulation(const BigUInt &noise, const BigUInt &max_noise, const BigUInt &coeff_modulus, const BigUInt &plain_modulus, int poly_modulus_coeff_count,
        double noise_standard_deviation, double noise_max_deviation, int decomposition_bit_count) : noise_(noise), max_noise_(max_noise), coeff_modulus_(coeff_modulus),
        plain_modulus_(plain_modulus), poly_modulus_coeff_count_(poly_modulus_coeff_count), noise_standard_deviation_(noise_standard_deviation),
        noise_max_deviation_(noise_max_deviation), decomposition_bit_count_(decomposition_bit_count)
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
    }
}