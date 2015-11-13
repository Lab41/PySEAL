#include <algorithm>
#include "simulator.h"
#include "utilities.h"
#include "util/uintarith.h"

using namespace std;
using namespace seal::util;

namespace seal
{
    Simulation SimulationEvaluator::add(const Simulation &simulation1, const Simulation &simulation2) const
    {
        if (!simulation1.CompareEncryptionParameters(simulation2))
        {
            throw invalid_argument("mismatch in encryption parameters");
        }

        // Quick return if one of the noises is zero
        if (simulation1.noise_.is_zero())
        {
            return simulation2;
        }
        if (simulation2.noise_.is_zero())
        {
            return simulation1;
        }

        // We reserve space for twice the larger noise.
        int wide_noise_bit_count = max(2 * max(simulation1.noise_.significant_bit_count(), simulation2.noise_.significant_bit_count()), simulation1.coeff_modulus_.significant_bit_count());
        int wide_noise_uint64_count = divide_round_up(wide_noise_bit_count, bits_per_uint64);

        // Resize all moduli.
        BigUInt wide_noise(wide_noise_bit_count);
        BigUInt wide_operand_noise(wide_noise_bit_count);
        BigUInt wide_plain_modulus(wide_noise_bit_count);
        BigUInt wide_coeff_modulus(wide_noise_bit_count);
        wide_noise = simulation1.noise_;
        wide_operand_noise = simulation2.noise_;
        wide_plain_modulus = simulation1.plain_modulus_;
        wide_coeff_modulus = simulation1.coeff_modulus_;

        // Compute coeff_modulus/plain_modulus.
        BigUInt wide_coeff_div_plain(wide_noise_bit_count);
        BigUInt wide_coeff_div_plain_remainder(wide_noise_bit_count);
        MemoryPool pool;
        divide_uint_uint(wide_coeff_modulus.pointer(), wide_plain_modulus.pointer(), wide_noise_uint64_count, wide_coeff_div_plain.pointer(), wide_coeff_div_plain_remainder.pointer(), pool);

        // In theory upper bound uses a sum of the noises (triangle ineq.) but we should expect the max to be closer to sqrt(noise1^2 + noise2^2). Approximate 
        // this by noise1 + noise2^2/(2*noise1).
        // Make wide_noise be the larger one.
        if (is_greater_than_uint_uint(wide_operand_noise.pointer(), wide_noise.pointer(), wide_noise_uint64_count))
        {
            swap(wide_noise, wide_operand_noise);
        }

        BigUInt temp1(wide_noise_bit_count);
        BigUInt temp2(wide_noise_bit_count);
        BigUInt temp3(wide_noise_bit_count);
        multiply_uint_uint(wide_operand_noise.pointer(), wide_noise_uint64_count, wide_operand_noise.pointer(), wide_noise_uint64_count, wide_noise_uint64_count, temp1.pointer());
        right_shift_uint(temp1.pointer(), 1, wide_noise_uint64_count, temp1.pointer());
        divide_uint_uint_inplace(temp1.pointer(), wide_noise.pointer(), wide_noise_uint64_count, temp2.pointer(), pool);

        BigUInt wide_result(wide_noise_bit_count);
        add_uint_uint(wide_coeff_div_plain_remainder.pointer(), temp2.pointer(), wide_noise_uint64_count, temp3.pointer());
        add_uint_uint(temp3.pointer(), wide_noise.pointer(), wide_noise_uint64_count, wide_result.pointer());
        wide_result.resize(wide_result.significant_bit_count());

        return Simulation(wide_result, simulation1.max_noise_, simulation1.coeff_modulus_, simulation1.plain_modulus_, simulation1.poly_modulus_coeff_count_, simulation1.noise_standard_deviation_, simulation1.noise_max_deviation_, simulation1.decomposition_bit_count_);
    }

    Simulation SimulationEvaluator::sub(const Simulation &simulation1, const Simulation &simulation2) const
    {
        return add(simulation1, simulation2);
    }

    Simulation SimulationEvaluator::relinearize(const Simulation &simulation) const
    {
        uint64_t growth_factor = static_cast<uint64_t>(sqrt(static_cast<uint64_t>(simulation.poly_modulus_coeff_count_ - 1) * 2 / 3));
        
        int wide_noise_bit_count = max(max(simulation.noise_.significant_bit_count(), simulation.plain_modulus_.significant_bit_count() + 32 + simulation.coeff_modulus_.significant_bit_count() / simulation.decomposition_bit_count_ + simulation.decomposition_bit_count_) + 1, simulation.coeff_modulus_.significant_bit_count());
        int wide_noise_uint64_count = divide_round_up(wide_noise_bit_count, bits_per_uint64);

        BigUInt wide_plain_modulus(wide_noise_bit_count);
        wide_plain_modulus = simulation.plain_modulus_;

        BigUInt decomposition_coeff(wide_noise_bit_count);
        decomposition_coeff = 1;
        left_shift_uint(decomposition_coeff.pointer(), simulation.decomposition_bit_count_, wide_noise_uint64_count, decomposition_coeff.pointer());

        BigUInt result(wide_noise_bit_count);
        BigUInt temp1(wide_noise_bit_count);
        BigUInt temp2(wide_noise_bit_count);

        // Noise growth from relinearization
        temp1 = growth_factor * static_cast<uint64_t>(sqrt(simulation.poly_modulus_coeff_count_)) * static_cast<uint64_t>(simulation.noise_max_deviation_) * (wide_noise_bit_count / simulation.decomposition_bit_count_);
        multiply_uint_uint(temp1.pointer(), wide_noise_uint64_count, wide_plain_modulus.pointer(), wide_noise_uint64_count, wide_noise_uint64_count, temp2.pointer());
        multiply_uint_uint(temp2.pointer(), wide_noise_uint64_count, decomposition_coeff.pointer(), wide_noise_uint64_count, wide_noise_uint64_count, result.pointer());

        // Finally resize the result
        result.resize(result.significant_bit_count());

        return Simulation(result, simulation.max_noise_, simulation.coeff_modulus_, simulation.plain_modulus_, simulation.poly_modulus_coeff_count_, simulation.noise_standard_deviation_, simulation.noise_max_deviation_, simulation.decomposition_bit_count_);
    }

    Simulation SimulationEvaluator::multiply_norelin(const Simulation &simulation1, const Simulation &simulation2) const
    {
        // Verify that both simulations have the same encryption parameters
        if (!simulation1.CompareEncryptionParameters(simulation2))
        {
            throw invalid_argument("mismatch in encryption parameters");
        }

        uint64_t growth_factor = static_cast<uint64_t>(sqrt(static_cast<uint64_t>(simulation1.poly_modulus_coeff_count_ - 1) * 2 / 3));

        int wide_noise_bit_count = max(max(simulation1.noise_.significant_bit_count(), simulation2.noise_.significant_bit_count()) + 2*simulation1.plain_modulus_.significant_bit_count() + 32, simulation1.coeff_modulus_.significant_bit_count());
        int wide_noise_uint64_count = divide_round_up(wide_noise_bit_count, bits_per_uint64);

        BigUInt wide_noise(wide_noise_bit_count);
        BigUInt wide_operand_noise(wide_noise_bit_count);
        BigUInt wide_plain_modulus(wide_noise_bit_count);
        wide_noise = simulation1.noise_;
        wide_operand_noise = simulation2.noise_;
        wide_plain_modulus = simulation1.plain_modulus_;

        BigUInt wide_noise_sum(wide_noise_bit_count);
        add_uint_uint(wide_noise.pointer(), wide_operand_noise.pointer(), wide_noise_uint64_count, wide_noise_sum.pointer());

        BigUInt result(wide_noise_bit_count);
        BigUInt temp1(wide_noise_bit_count);
        BigUInt temp2(wide_noise_bit_count);
        BigUInt temp3(wide_noise_bit_count);

        // Noise growth from multiplication
        temp1 = growth_factor * simulation1.poly_modulus_coeff_count_;
        multiply_uint_uint(temp1.pointer(), wide_noise_uint64_count, wide_plain_modulus.pointer(), wide_noise_uint64_count, wide_noise_uint64_count, temp2.pointer());
        multiply_uint_uint(temp2.pointer(), wide_noise_uint64_count, wide_plain_modulus.pointer(), wide_noise_uint64_count, wide_noise_uint64_count, temp3.pointer());
        right_shift_uint(temp3.pointer(), 1, wide_noise_uint64_count, temp3.pointer());
        multiply_uint_uint(temp3.pointer(), wide_noise_uint64_count, wide_noise_sum.pointer(), wide_noise_uint64_count, wide_noise_uint64_count, result.pointer());

        result.resize(result.significant_bit_count());

        return Simulation(result, simulation1.max_noise_, simulation1.coeff_modulus_, simulation1.plain_modulus_, simulation1.poly_modulus_coeff_count_, simulation1.noise_standard_deviation_, simulation1.noise_max_deviation_, simulation1.decomposition_bit_count_);
    }

    Simulation SimulationEvaluator::multiply(const Simulation &simulation1, const Simulation &simulation2) const
    {
        return relinearize(multiply_norelin(simulation1, simulation2));
    }

    Simulation SimulationEvaluator::multiply_plain(const Simulation &simulation, int plain_max_coeff_count, const BigUInt &plain_max_abs_value) const
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
            BigUInt zero_uint;
            zero_uint.set_zero();
            return Simulation(zero_uint, simulation.max_noise_, simulation.coeff_modulus_, simulation.plain_modulus_, simulation.poly_modulus_coeff_count_, simulation.noise_standard_deviation_, simulation.noise_max_deviation_, simulation.decomposition_bit_count_);
        }

        int wide_noise_bit_count = max(simulation.noise_.significant_bit_count() + simulation.coeff_modulus_.significant_bit_count(), simulation.coeff_modulus_.significant_bit_count());
        int wide_noise_uint64_count = divide_round_up(wide_noise_bit_count, bits_per_uint64);

        // Resize all moduli.
        BigUInt wide_noise(wide_noise_bit_count);
        BigUInt wide_plain_modulus(wide_noise_bit_count);
        BigUInt wide_coeff_modulus(wide_noise_bit_count);
        wide_noise = simulation.noise_;
        wide_plain_modulus = simulation.plain_modulus_;
        wide_coeff_modulus = simulation.coeff_modulus_;

        // Compute coeff_modulus/plain_modulus.
        BigUInt wide_coeff_div_plain(wide_noise_bit_count);
        BigUInt wide_coeff_div_plain_remainder(wide_noise_bit_count);
//        MemoryPool pool;
//        divide_uint_uint(wide_coeff_modulus.pointer(), wide_plain_modulus.pointer(), wide_noise_uint64_count, wide_coeff_div_plain.pointer(), wide_coeff_div_plain_remainder.pointer(), pool);

//        BigUInt term1(wide_noise_bit_count);
        BigUInt term2(wide_noise_bit_count);

        BigUInt temp1(wide_noise_bit_count);
        BigUInt temp2(wide_noise_bit_count);
        BigUInt temp3(wide_noise_bit_count);

//        temp1 = plain_max_coeff_count;
//        multiply_uint_uint(temp1.pointer(), wide_noise_uint64_count, plain_max_abs_value.pointer(), plain_max_abs_value.uint64_count(), wide_noise_uint64_count, temp2.pointer());
//        multiply_uint_uint(temp2.pointer(), wide_noise_uint64_count, wide_coeff_div_plain_remainder.pointer(), wide_noise_uint64_count, wide_noise_uint64_count, temp3.pointer());
//        right_shift_uint(temp3.pointer(), 1, wide_noise_uint64_count, term1.pointer());

//        temp1.set_zero();
//        temp2.set_zero();

        temp1 = static_cast<uint64_t>(sqrt(static_cast<double>(plain_max_coeff_count)));
        multiply_uint_uint(temp1.pointer(), wide_noise_uint64_count, wide_noise.pointer(), wide_noise_uint64_count, wide_noise_uint64_count, temp2.pointer());
        multiply_uint_uint(temp2.pointer(), wide_noise_uint64_count, plain_max_abs_value.pointer(), plain_max_abs_value.uint64_count(), wide_noise_uint64_count, term2.pointer());

        BigUInt wide_result(wide_noise_bit_count);
        wide_result = term2;
//        add_uint_uint(term1.pointer(), term2.pointer(), wide_noise_uint64_count, wide_result.pointer());
        wide_result.resize(wide_result.significant_bit_count());

        return Simulation(wide_result, simulation.max_noise_, simulation.coeff_modulus_, simulation.plain_modulus_, simulation.poly_modulus_coeff_count_, simulation.noise_standard_deviation_, simulation.noise_max_deviation_, simulation.decomposition_bit_count_);
    }

    Simulation SimulationEvaluator::multiply_plain(const Simulation &simulation, int plain_max_coeff_count, uint64_t plain_max_abs_value) const
    {
        BigUInt plain_max_abs_value_uint;
        plain_max_abs_value_uint = plain_max_abs_value;
        return multiply_plain(simulation, plain_max_coeff_count, plain_max_abs_value_uint);
    }

    Simulation SimulationEvaluator::add_plain(const Simulation &simulation) const
    {
        /*
        int wide_noise_bit_count = simulation.coeff_modulus_.significant_bit_count() + 1;
        int wide_noise_uint64_count = divide_round_up(wide_noise_bit_count, bits_per_uint64);

        // Resize all moduli.
        BigUInt wide_noise(wide_noise_bit_count);
        BigUInt wide_plain_modulus(wide_noise_bit_count);
        BigUInt wide_coeff_modulus(wide_noise_bit_count);
        wide_noise = simulation.noise_;
        wide_plain_modulus = simulation.plain_modulus_;
        wide_coeff_modulus = simulation.coeff_modulus_;

        // Compute coeff_modulus/plain_modulus.
        BigUInt wide_coeff_div_plain(wide_noise_bit_count);
        BigUInt wide_coeff_div_plain_remainder(wide_noise_bit_count);
        MemoryPool pool;
        divide_uint_uint(wide_coeff_modulus.pointer(), wide_plain_modulus.pointer(), wide_noise_uint64_count, wide_coeff_div_plain.pointer(), wide_coeff_div_plain_remainder.pointer(), pool);

        BigUInt wide_result(wide_noise_bit_count);
        add_uint_uint(wide_noise.pointer(), wide_coeff_div_plain_remainder.pointer(), wide_noise_uint64_count, wide_result.pointer());
        wide_result.resize(wide_result.significant_bit_count());

        return Simulation(wide_result, simulation.max_noise_, simulation.coeff_modulus_, simulation.plain_modulus_, simulation.poly_modulus_coeff_count_, simulation.noise_standard_deviation_, simulation.noise_max_deviation_, simulation.decomposition_bit_count_);
        */

        return simulation;
    }

    Simulation SimulationEvaluator::sub_plain(const Simulation &simulation) const
    {
        return add_plain(simulation);
    }

    Simulation SimulationEvaluator::binary_exponentiate(const Simulation &simulation, int exponent) const
    {
        if (exponent < 0)
        {
            throw invalid_argument("exponent must be non-negative");
        }
        if (exponent == 0)
        {
            return Simulation(BigUInt(), simulation.max_noise_, simulation.coeff_modulus_, simulation.plain_modulus_, simulation.poly_modulus_coeff_count_, simulation.noise_standard_deviation_, simulation.noise_max_deviation_, simulation.decomposition_bit_count_);
        }
        if (exponent == 1)
        {
            return simulation;
        }
        if (exponent % 2 == 0)
        {
            return binary_exponentiate(multiply(simulation, simulation), exponent >> 1);
        }
        return binary_exponentiate(multiply(simulation, simulation), (exponent - 1) >> 1);
    }

    Simulation SimulationEvaluator::tree_multiply(const vector<Simulation> &simulations) const
    {
        // Verify parameters.
        if (simulations.empty())
        {
            throw invalid_argument("simulations vector must not be empty");
        }

        // Return quickly if there is nothing to do.
        if (simulations.size() == 1)
        {
            return simulations[0];
        }

        vector<Simulation> simulations_intermediate = simulations;
        while (simulations_intermediate.size() > 1)
        {
            vector<Simulation>::size_type pairs = simulations_intermediate.size() / 2;
            vector<Simulation>::size_type remaining = simulations_intermediate.size() % 2;
            vector<Simulation> simulations_intermediate_next;
            simulations_intermediate_next.reserve(pairs + remaining);

            for (vector<Simulation>::size_type i = 0; i < pairs; ++i)
            {
                simulations_intermediate_next.push_back(multiply(simulations_intermediate[2 * i + 1], simulations_intermediate[2 * i]));
            }
            if (remaining == 1)
            {
                simulations_intermediate_next.push_back(*simulations_intermediate.rbegin());
            }
            simulations_intermediate = simulations_intermediate_next;
        }

        return simulations_intermediate[0];
    }

    Simulation SimulationEvaluator::tree_exponentiate(const Simulation &simulation, int exponent) const
    {
        if (exponent < 0)
        {
            throw invalid_argument("exponent must be non-negative");
        }

        // Quick return when exponent is 0 or 1
        if (exponent == 0)
        {
            return Simulation(BigUInt(), simulation.max_noise_, simulation.coeff_modulus_, simulation.plain_modulus_, simulation.poly_modulus_coeff_count_, simulation.noise_standard_deviation_, simulation.noise_max_deviation_, simulation.decomposition_bit_count_);
        }
        if (exponent == 1)
        {
            return simulation;
        }

        return tree_multiply(vector<Simulation>(exponent, simulation));
    }

    Simulation SimulationEvaluator::negate(const Simulation &simulation) const
    {
        return simulation;
    }

    void Simulation::set_initial_noise_estimate()
    {
        uint64_t growth_factor = static_cast<uint64_t>(sqrt(2 * (poly_modulus_coeff_count_ - 1) / 3));

        int wide_noise_bit_count = coeff_modulus_.significant_bit_count();
        int wide_noise_uint64_count = divide_round_up(wide_noise_bit_count, bits_per_uint64);

        BigUInt wide_plain_modulus(wide_noise_bit_count);
        wide_plain_modulus = plain_modulus_;

        BigUInt wide_coeff_div_plain(wide_noise_bit_count);
        BigUInt wide_coeff_div_plain_remainder(wide_noise_bit_count);
        MemoryPool pool;
        divide_uint_uint(coeff_modulus_.pointer(), wide_plain_modulus.pointer(), wide_noise_uint64_count, wide_coeff_div_plain.pointer(), wide_coeff_div_plain_remainder.pointer(), pool);

//        BigUInt term1(wide_noise_bit_count);
        BigUInt term2(wide_noise_bit_count);
        BigUInt temp1(wide_noise_bit_count);
        BigUInt temp2(wide_noise_bit_count);

//        temp1 = wide_coeff_div_plain_remainder;
//        multiply_uint_uint(temp1.pointer(), wide_noise_uint64_count, wide_plain_modulus.pointer(), wide_noise_uint64_count, wide_noise_uint64_count, term1.pointer());
//        right_shift_uint(term1.pointer(), 1, wide_noise_uint64_count, term1.pointer());

        temp2 = 2 * growth_factor * static_cast<uint64_t>(noise_max_deviation_);
        multiply_uint_uint(temp2.pointer(), wide_noise_uint64_count, wide_plain_modulus.pointer(), wide_noise_uint64_count, wide_noise_uint64_count, term2.pointer());

        noise_.resize(wide_noise_bit_count);
        noise_ = term2;
//        add_uint_uint(term1.pointer(), term2.pointer(), wide_noise_uint64_count, noise_.pointer());
        noise_.resize(noise_.significant_bit_count());
    }

    Simulation::Simulation(const EncryptionParameters &parms) : max_noise_(inherent_noise_max(parms)), coeff_modulus_(parms.coeff_modulus()),
        plain_modulus_(parms.plain_modulus()), poly_modulus_coeff_count_(parms.poly_modulus().coeff_count()), noise_standard_deviation_(parms.noise_standard_deviation()),
        noise_max_deviation_(parms.noise_max_deviation()), max_div_standard_deviation_(parms.noise_max_deviation() / parms.noise_standard_deviation()),
        decomposition_bit_count_(parms.decomposition_bit_count())
    {
        set_initial_noise_estimate();
    }

    Simulation::Simulation(const EncryptionParameters &parms, const BigUInt &noise) : noise_(noise),
        max_noise_(inherent_noise_max(parms)), coeff_modulus_(parms.coeff_modulus()),
        plain_modulus_(parms.plain_modulus()), poly_modulus_coeff_count_(parms.poly_modulus().coeff_count()), noise_standard_deviation_(parms.noise_standard_deviation()),
        noise_max_deviation_(parms.noise_max_deviation()), max_div_standard_deviation_(parms.noise_max_deviation() / parms.noise_standard_deviation()),
        decomposition_bit_count_(parms.decomposition_bit_count())
    {
    }

    Simulation::Simulation(const BigUInt &noise, const BigUInt &max_noise, const BigUInt &coeff_modulus, const BigUInt &plain_modulus, int poly_modulus_coeff_count,
        double noise_standard_deviation, double noise_max_deviation, int decomposition_bit_count) : noise_(noise), max_noise_(max_noise), coeff_modulus_(coeff_modulus),
        plain_modulus_(plain_modulus), poly_modulus_coeff_count_(poly_modulus_coeff_count), noise_standard_deviation_(noise_standard_deviation),
        noise_max_deviation_(noise_max_deviation), max_div_standard_deviation_(noise_max_deviation / noise_standard_deviation),
        decomposition_bit_count_(decomposition_bit_count)
    {
    }
}