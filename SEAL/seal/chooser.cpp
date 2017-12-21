#include <algorithm>
#include <cstdint>
#include <cmath>
#include <numeric>
#include "seal/chooser.h"
#include "seal/util/uintarith.h"
#include "seal/util/polyarithsmallmod.h"
#include "seal/defaultparams.h"

using namespace std;
using namespace seal::util;

namespace seal
{
    ChooserPoly::ChooserPoly(int max_coeff_count, uint64_t max_abs_value) :
        max_coeff_count_(max_coeff_count), max_abs_value_(max_abs_value)
    {
        if (max_coeff_count <= 0)
        {
            throw invalid_argument("max_coeff_count must be strictly positive");
        }
        if (max_abs_value == 0)
        {
            max_coeff_count_ = 1;
        }
        comp_ = new FreshComputation(max_coeff_count_, max_abs_value_);
    }

    ChooserPoly::ChooserPoly(int max_coeff_count, uint64_t max_abs_value, Computation *comp) :
        max_coeff_count_(max_coeff_count), max_abs_value_(max_abs_value), comp_(comp)
    {
        if (max_coeff_count <= 0)
        {
            throw invalid_argument("max_coeff_count must be strictly positive");
        }

        if (max_abs_value == 0)
        {
            max_coeff_count_ = 1;
        }
        /*
        if (comp == nullptr)
        {
        comp_ = new FreshComputation();
        }
        */
    }

    ChooserPoly::~ChooserPoly()
    {
        reset();
    }

    ChooserPoly::ChooserPoly(const ChooserPoly &copy) : max_coeff_count_(0), max_abs_value_(), comp_(nullptr)
    {
        operator =(copy);
    }

    ChooserPoly &ChooserPoly::operator =(const ChooserPoly &assign)
    {
        reset();
        if (assign.comp_ != nullptr)
        {
            comp_ = assign.comp_->clone();
        }
        max_abs_value_ = assign.max_abs_value_;
        max_coeff_count_ = assign.max_coeff_count_;

        return *this;
    }

    ChooserPoly ChooserEvaluator::add(const ChooserPoly &operand1, const ChooserPoly &operand2)
    {
        if (operand1.max_coeff_count_ <= 0 || operand1.comp_ == nullptr)
        {
            throw invalid_argument("operand1 is not correctly initialized");
        }
        if (operand2.max_coeff_count_ <= 0 || operand2.comp_ == nullptr)
        {
            throw invalid_argument("operand2 is not correctly initialized");
        }

        return ChooserPoly(max(operand1.max_coeff_count_, operand2.max_coeff_count_), operand1.max_abs_value_ + operand2.max_abs_value_, new AddComputation(*operand1.comp_, *operand2.comp_));
    }

    ChooserPoly ChooserEvaluator::add_many(const std::vector<ChooserPoly> &operands)
    {
        if (operands.empty())
        {
            throw invalid_argument("operands vector can not be empty");
        }

        int sum_max_coeff_count = operands[0].max_coeff_count_;
        size_t largest_abs_value_index = 0;
        for (size_t i = 0; i < operands.size(); i++)
        {
            // Throw if any of the operands is not initialized correctly
            if (operands[i].max_coeff_count_ <= 0 || operands[i].comp_ == nullptr)
            {
                throw invalid_argument("input operand is not correctly initialized");
            }

            if (operands[i].max_coeff_count_ > sum_max_coeff_count)
            {
                sum_max_coeff_count = operands[i].max_coeff_count_;
            }
            if (operands[i].max_abs_value_ > operands[largest_abs_value_index].max_abs_value_)
            {
                largest_abs_value_index = i;
            }
        }

        uint64_t sum_max_abs_value = 0;
        vector<Computation*> comps;
        for (size_t i = 0; i < operands.size(); i++)
        {
            sum_max_abs_value += operands[i].max_abs_value_;
            comps.emplace_back(operands[i].comp_);
        }

        return ChooserPoly(sum_max_coeff_count, sum_max_abs_value, new AddManyComputation(comps));
    }

    ChooserPoly ChooserEvaluator::sub(const ChooserPoly &operand1, const ChooserPoly &operand2)
    {
        if (operand1.max_coeff_count_ <= 0 || operand1.comp_ == nullptr)
        {
            throw invalid_argument("operand1 is not correctly initialized");
        }
        if (operand2.max_coeff_count_ <= 0 || operand2.comp_ == nullptr)
        {
            throw invalid_argument("operand2 is not correctly initialized");
        }

        return ChooserPoly(max(operand1.max_coeff_count_, operand2.max_coeff_count_), operand1.max_abs_value_ + operand2.max_abs_value_, new SubComputation(*operand1.comp_, *operand2.comp_));
    }

    ChooserPoly ChooserEvaluator::multiply(const ChooserPoly &operand1, const ChooserPoly &operand2)
    {
        if (operand1.max_coeff_count_ <= 0 || operand1.comp_ == nullptr)
        {
            throw invalid_argument("operand1 is not correctly initialized");
        }
        if (operand2.max_coeff_count_ <= 0 || operand2.comp_ == nullptr)
        {
            throw invalid_argument("operand2 is not correctly initialized");
        }
        if (operand1.max_abs_value_ == 0 || operand2.max_abs_value_ == 0)
        {
            return ChooserPoly(1, 0, new MultiplyComputation(*operand1.comp_, *operand2.comp_));
        }

        uint64_t growth_factor = min(operand1.max_coeff_count_, operand2.max_coeff_count_);
        uint64_t prod_max_abs_value[2];
        multiply_uint64(growth_factor, operand1.max_abs_value_, prod_max_abs_value);
        if (prod_max_abs_value[1])
        {
            throw invalid_argument("polynomial coefficients too large");
        }
        multiply_uint64(prod_max_abs_value[0], operand2.max_abs_value_, prod_max_abs_value);
        if (prod_max_abs_value[1])
        {
            throw invalid_argument("polynomial coefficients too large");
        }

        return ChooserPoly(operand1.max_coeff_count_ + operand2.max_coeff_count_ - 1, prod_max_abs_value[0], new MultiplyComputation(*operand1.comp_, *operand2.comp_));
    }

    ChooserPoly ChooserEvaluator::square(const ChooserPoly &operand)
    {
        return multiply(operand, operand);
    }

    ChooserPoly ChooserEvaluator::relinearize(const ChooserPoly &operand, int decomposition_bit_count)
    {
        if (operand.max_coeff_count_ <= 0 || operand.comp_ == nullptr)
        {
            throw invalid_argument("operand is not correctly initialized");
        }

        // Check that decomposition_bit_count is in correct interval
        if (decomposition_bit_count < SEAL_DBC_MIN || decomposition_bit_count > SEAL_DBC_MAX)
        {
            throw invalid_argument("decomposition_bit_count is not in the valid range");
        }

        return ChooserPoly(operand.max_coeff_count_, operand.max_abs_value_, 
            new RelinearizeComputation(*operand.comp_, decomposition_bit_count));
    }

    ChooserPoly ChooserEvaluator::multiply_plain(const ChooserPoly &operand, int plain_max_coeff_count, uint64_t plain_max_abs_value)
    {
        if (operand.max_coeff_count_ <= 0 || operand.comp_ == nullptr)
        {
            throw invalid_argument("operand is not correctly initialized");
        }
        if (plain_max_coeff_count <= 0)
        {
            throw invalid_argument("plain_max_coeff_count must be positive");
        }
        if (plain_max_abs_value == 0)
        {
            throw invalid_argument("plain_max_abs_value cannot be zero");
        }
        if (operand.max_abs_value_ == 0)
        {
            return ChooserPoly(1, 0, new MultiplyPlainComputation(*operand.comp_, plain_max_coeff_count, plain_max_abs_value));
        }

        uint64_t growth_factor = min(operand.max_coeff_count_, plain_max_coeff_count);
        uint64_t prod_max_abs_value[2];
        multiply_uint64(growth_factor, operand.max_abs_value_, prod_max_abs_value);
        if (prod_max_abs_value[1])
        {
            throw invalid_argument("polynomial coefficients too large");
        }
        multiply_uint64(prod_max_abs_value[0], plain_max_abs_value, prod_max_abs_value);
        if (prod_max_abs_value[1])
        {
            throw invalid_argument("polynomial coefficients too large");
        }

        return ChooserPoly(operand.max_coeff_count_ + plain_max_coeff_count - 1, prod_max_abs_value[0], new MultiplyPlainComputation(*operand.comp_, plain_max_coeff_count, plain_max_abs_value));
    }

    ChooserPoly ChooserEvaluator::multiply_plain(const ChooserPoly &operand, const ChooserPoly &plain_chooser_poly)
    {
        if (plain_chooser_poly.comp_ != nullptr)
        {
            throw invalid_argument("plain_chooser_poly is not correctly initialized");
        }
        return multiply_plain(operand, plain_chooser_poly.max_coeff_count_, plain_chooser_poly.max_abs_value_);
    }

    ChooserPoly ChooserEvaluator::add_plain(const ChooserPoly &operand, int plain_max_coeff_count, uint64_t plain_max_abs_value)
    {
        if (operand.max_coeff_count_ <= 0 || operand.comp_ == nullptr)
        {
            throw invalid_argument("operand is not correctly initialized");
        }
        if (plain_max_coeff_count <= 0)
        {
            throw invalid_argument("plain_max_coeff_count must be positive");
        }
        if (plain_max_abs_value == 0)
        {
            return ChooserPoly(operand.max_coeff_count_, operand.max_abs_value_, new AddPlainComputation(*operand.comp_, plain_max_coeff_count, plain_max_abs_value));
        }
        if (operand.max_abs_value_ == 0)
        {
            return ChooserPoly(plain_max_coeff_count, plain_max_abs_value, new AddPlainComputation(*operand.comp_, plain_max_coeff_count, plain_max_abs_value));
        }

        return ChooserPoly(max(operand.max_coeff_count_, plain_max_coeff_count), operand.max_abs_value_ + plain_max_abs_value, new AddPlainComputation(*operand.comp_, plain_max_coeff_count, plain_max_abs_value));
    }

    ChooserPoly ChooserEvaluator::add_plain(const ChooserPoly &operand, const ChooserPoly &plain_chooser_poly)
    {
        if (plain_chooser_poly.comp_ != nullptr)
        {
            throw invalid_argument("plain_chooser_poly is not correctly initialized");
        }
        return add_plain(operand, plain_chooser_poly.max_coeff_count_, plain_chooser_poly.max_abs_value_);
    }

    ChooserPoly ChooserEvaluator::sub_plain(const ChooserPoly &operand, int plain_max_coeff_count, uint64_t plain_max_abs_value)
    {
        if (operand.max_coeff_count_ <= 0 || operand.comp_ == nullptr)
        {
            throw invalid_argument("operand is not correctly initialized");
        }
        if (plain_max_coeff_count <= 0)
        {
            throw invalid_argument("plain_max_coeff_count must be positive");
        }
        if (plain_max_abs_value == 0)
        {
            return ChooserPoly(operand.max_coeff_count_, operand.max_abs_value_, new SubPlainComputation(*operand.comp_, plain_max_coeff_count, plain_max_abs_value));
        }
        if (operand.max_abs_value_ == 0)
        {
            return ChooserPoly(plain_max_coeff_count, plain_max_abs_value, new SubPlainComputation(*operand.comp_, plain_max_coeff_count, plain_max_abs_value));
        }

        return ChooserPoly(max(operand.max_coeff_count_, plain_max_coeff_count), operand.max_abs_value_ + plain_max_abs_value, new SubPlainComputation(*operand.comp_, plain_max_coeff_count, plain_max_abs_value));
    }

    ChooserPoly ChooserEvaluator::sub_plain(const ChooserPoly &operand, const ChooserPoly &plain_chooser_poly)
    {
        if (plain_chooser_poly.comp_ != nullptr)
        {
            throw invalid_argument("plain_chooser_poly is not correctly initialized");
        }
        return sub_plain(operand, plain_chooser_poly.max_coeff_count_, plain_chooser_poly.max_abs_value_);
    }

    ChooserPoly ChooserEvaluator::exponentiate(const ChooserPoly &operand, uint64_t exponent, int decomposition_bit_count)
    {
        if (operand.max_coeff_count_ <= 0 || operand.comp_ == nullptr)
        {
            throw invalid_argument("operand is not correctly initialized");
        }
        if (exponent == 0)
        {
            throw invalid_argument("exponent cannot be 0");
        }

        // Check that decomposition_bit_count is in correct interval
        if (decomposition_bit_count < SEAL_DBC_MIN || decomposition_bit_count > SEAL_DBC_MAX)
        {
            throw invalid_argument("decomposition_bit_count is not in the valid range");
        }

        if (operand.max_abs_value_ == 0)
        {
            return ChooserPoly(1, 0, new ExponentiateComputation(*operand.comp_, exponent, decomposition_bit_count));
        }

        // There is no known closed formula for the growth factor, but we use the asymptotic approximation
        // k^n * sqrt[6/((k-1)*(k+1)*Pi*n)], where k = max_coeff_count_, n = exponent.
        uint64_t growth_factor = static_cast<uint64_t>(pow(operand.max_coeff_count_, exponent) * sqrt(6 / ((operand.max_coeff_count_ - 1) * (operand.max_coeff_count_ + 1) * 3.1415 * exponent)));

        int result_bit_count = exponent * get_significant_bit_count(operand.max_abs_value_) + get_significant_bit_count(growth_factor) + 1 + get_significant_bit_count(growth_factor);
        if (result_bit_count > bits_per_uint64)
        {
            throw invalid_argument("polynomial coefficients too large");
        }
        uint64_t result_max_abs_value = exponentiate_uint64(operand.max_abs_value_, exponent) * growth_factor;

        return ChooserPoly(exponent * (operand.max_coeff_count_ - 1) + 1, result_max_abs_value, new ExponentiateComputation(*operand.comp_, exponent, decomposition_bit_count));
    }

    ChooserPoly ChooserEvaluator::negate(const ChooserPoly &operand)
    {
        if (operand.max_coeff_count_ <= 0 || operand.comp_ == nullptr)
        {
            throw invalid_argument("operand is not correctly initialized");
        }
        return ChooserPoly(operand.max_coeff_count_, operand.max_abs_value_, new NegateComputation(*operand.comp_));
    }

    ChooserPoly ChooserEvaluator::multiply_many(const vector<ChooserPoly> &operands, int decomposition_bit_count)
    {
        if (operands.empty())
        {
            throw invalid_argument("operands vector can not be empty");
        }

        // Check that decomposition_bit_count is in correct interval
        if (decomposition_bit_count < SEAL_DBC_MIN || decomposition_bit_count > SEAL_DBC_MAX)
        {
            throw invalid_argument("decomposition_bit_count is not in the valid range");
        }

        int prod_max_coeff_count = 1;
        uint64_t growth_factor = 1;
        int prod_max_abs_value_bit_count = 1;
        vector<Computation*> comps;
        for (size_t i = 0; i < operands.size(); i++)
        {
            // Throw if any of the operands is not initialized correctly
            if (operands[i].max_coeff_count_ <= 0 || operands[i].comp_ == nullptr)
            {
                throw invalid_argument("input operand is not correctly initialized");
            }

            // Return early if the product is trivially zero
            if (operands[i].max_abs_value_ == 0)
            {
                return ChooserPoly(1, 0, new MultiplyManyComputation(comps, decomposition_bit_count));
            }

            prod_max_coeff_count += operands[i].max_coeff_count_ - 1;
            prod_max_abs_value_bit_count += get_significant_bit_count(operands[i].max_abs_value_);

            growth_factor *= (i == 0 ? 1 : min(operands[i].max_coeff_count_, prod_max_coeff_count));

            comps.emplace_back(operands[i].comp_);
        }

        prod_max_abs_value_bit_count += get_significant_bit_count(growth_factor);
        if (prod_max_abs_value_bit_count >= bits_per_uint64)
        {
            throw invalid_argument("polynomial coefficients too large");
        }

        uint64_t prod_max_abs_value = growth_factor;
        for (size_t i = 0; i < operands.size(); i++)
        {
            prod_max_abs_value *= operands[i].max_abs_value_;
        }

        return ChooserPoly(prod_max_coeff_count, prod_max_abs_value, new MultiplyManyComputation(comps, decomposition_bit_count));
    }

    bool ChooserEvaluator::select_parameters(const std::vector<ChooserPoly> &operands, int budget_gap, EncryptionParameters &destination)
    {
        return select_parameters(operands, budget_gap, global_variables::default_noise_standard_deviation, global_variables::default_coeff_modulus_128, destination);
    }

    bool ChooserEvaluator::select_parameters(const std::vector<ChooserPoly> &operands, int budget_gap, double noise_standard_deviation, const map<int, vector<SmallModulus> > &coeff_modulus_options, EncryptionParameters &destination)
    {
        if (budget_gap < 0)
        {
            throw std::invalid_argument("budget_gap cannot be negative");
        }
        if (noise_standard_deviation < 0)
        {
            throw invalid_argument("noise_standard_deviation can not be negative");
        }
        if (coeff_modulus_options.size() == 0)
        {
            throw invalid_argument("parameter_options must contain at least one entry");
        }
        if (operands.empty())
        {
            throw invalid_argument("operands cannot be empty");
        }

        int largest_bit_count = 0;
        int largest_coeff_count = 0;
        for (size_t i = 0; i < operands.size(); i++)
        {
            if (operands[i].comp_ == nullptr)
            {
                throw logic_error("no operation history to simulate");
            }
            int current_bit_count = get_significant_bit_count(operands[i].max_abs_value_);
            largest_bit_count = (current_bit_count > largest_bit_count) ? current_bit_count : largest_bit_count;

            int current_coeff_count = operands[i].max_coeff_count_;
            largest_coeff_count = (current_coeff_count > largest_coeff_count) ? current_coeff_count : largest_coeff_count;
        }

        // We restrict to plain moduli that are powers of two. Here largest_bit_count is the largest positive
        // coefficient that we can expect to appear. Thus, we need one more bit.
        uint64_t new_plain_modulus;
        if (largest_bit_count >= SEAL_USER_MODULO_BIT_BOUND)
        {
            // The plain_modulus needed is too big
            return false;
        }
        new_plain_modulus = 1ULL << largest_bit_count;
        destination.set_plain_modulus(new_plain_modulus);

        bool found_good_parms = false;
        map<int, vector<SmallModulus> >::const_iterator iter = coeff_modulus_options.begin();
        while (iter != coeff_modulus_options.end() && !found_good_parms)
        {
            int dimension = iter->first;
            if (dimension < 512 || (dimension & (dimension - 1)) != 0)
            {
                throw invalid_argument("coeff_modulus_options keys invalid");
            }

            int coeff_bit_count = 0;
            for(auto mod : iter->second)
            {
                coeff_bit_count += mod.bit_count();
            }

            if (dimension > largest_coeff_count && coeff_bit_count > destination.plain_modulus().bit_count())
            {
                // Set the polynomial
                destination.set_coeff_modulus(iter->second);
                BigPoly new_poly_modulus(dimension + 1, 1);
                new_poly_modulus.set_zero();
                new_poly_modulus[0] = 1;
                new_poly_modulus[dimension] = 1;
                destination.set_poly_modulus(new_poly_modulus);

                // The bound needed for GapSVP->search-LWE reduction
                //parms.noise_standard_deviation() = round(sqrt(dimension / (2 * 3.1415)) + 0.5);

                // Use constant (small) standard deviation.
                destination.set_noise_standard_deviation(noise_standard_deviation);

                found_good_parms = true;
                for (size_t i = 0; i < operands.size(); i++)
                {
                    // If one of the operands does not decrypt, set found_good_parms to false.
                    found_good_parms = operands[i].simulate(destination).decrypts(budget_gap) ? found_good_parms : false;
                }
            }

            // This dimension/coeff_modulus are to small. Move on to the next pair.
            iter++;
        }

        if (!found_good_parms)
        {
            destination = EncryptionParameters();
        }

        return found_good_parms;
    }

    Simulation ChooserPoly::simulate(const EncryptionParameters &parms) const
    {
        if (comp_ == nullptr)
        {
            throw logic_error("no operation history to simulate");
        }
        return comp_->simulate(parms);
    }

    void ChooserPoly::reset()
    {
        if (comp_ != nullptr)
        {
            delete comp_;
            comp_ = nullptr;
        }
        max_abs_value_ = 0;
        max_coeff_count_ = 0;
    }

    void ChooserPoly::set_fresh()
    {
        if (comp_ != nullptr)
        {
            delete comp_;
            comp_ = nullptr;
        }
        comp_ = new FreshComputation(max_coeff_count_, max_abs_value_);
    }

    ChooserEncoder::ChooserEncoder(uint64_t base) : encoder_(SmallModulus(base), base)
    {
    }

    ChooserPoly ChooserEncoder::encode(uint64_t value)
    {
        ChooserPoly chooser_poly;
        encode(value, chooser_poly);
        return chooser_poly;
    }

    void ChooserEncoder::encode(uint64_t value, ChooserPoly &destination)
    {
        Plaintext value_poly;
        encoder_.encode(value, value_poly);
        destination.reset();
        destination.max_coeff_count() = value_poly.significant_coeff_count();
        destination.max_abs_value() = poly_infty_norm_coeffmod(value_poly.pointer(), value_poly.coeff_count(), encoder_.plain_modulus());
    }

    ChooserPoly ChooserEncoder::encode(int64_t value)
    {
        ChooserPoly chooser_poly;
        encode(value, chooser_poly);
        return chooser_poly;
    }

    void ChooserEncoder::encode(int64_t value, ChooserPoly &destination)
    {
        Plaintext value_poly;
        encoder_.encode(value, value_poly);
        destination.reset();
        destination.max_coeff_count() = max(value_poly.significant_coeff_count(), 1);
        destination.max_abs_value() = poly_infty_norm_coeffmod(value_poly.pointer(), value_poly.coeff_count(), encoder_.plain_modulus());
    }

    ChooserPoly ChooserEncoder::encode(BigUInt value)
    {
        ChooserPoly chooser_poly;
        encode(value, chooser_poly);
        return chooser_poly;
    }

    void ChooserEncoder::encode(BigUInt value, ChooserPoly &destination)
    {
        Plaintext value_poly;
        encoder_.encode(value, value_poly);
        destination.reset();
        destination.max_coeff_count() = value_poly.significant_coeff_count();
        destination.max_abs_value() = poly_infty_norm_coeffmod(value_poly.pointer(), value_poly.coeff_count(), encoder_.plain_modulus());
    }

    void ChooserEncryptor::encrypt(const ChooserPoly &plain, ChooserPoly &destination) const
    {
        if (plain.comp_ != nullptr)
        {
            throw invalid_argument("plain has non-null operation history");
        }
        destination = plain;
        destination.set_fresh();
    }

    ChooserPoly ChooserEncryptor::encrypt(const ChooserPoly &plain) const
    {
        ChooserPoly result(plain);
        result.set_fresh();
        return result;
    }

    void ChooserEncryptor::decrypt(const ChooserPoly &encrypted, ChooserPoly &destination) const
    {
        if (encrypted.comp_ == nullptr)
        {
            throw invalid_argument("encrypted has null operation history");
        }
        destination.reset();
        destination.max_abs_value() = encrypted.max_abs_value();
        destination.max_coeff_count() = encrypted.max_coeff_count();
    }

    ChooserPoly ChooserEncryptor::decrypt(const ChooserPoly &encrypted) const
    {
        ChooserPoly result;
        decrypt(encrypted, result);
        return result;
    }
}