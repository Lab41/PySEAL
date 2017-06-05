#include <algorithm>
#include <map>
#include <cstdint>
#include <cmath>
#include "chooser.h"
#include "utilities.h"
#include "util/uintarith.h"
#include "util/uintextras.h"

using namespace std;
using namespace seal::util;

namespace seal
{
    const map<int, BigUInt> ChooserEvaluator::default_parameter_options_
    {
        ///*
        //Polynomial modulus: 1x^1024 + 1
        //Coefficient modulus: 2^35 - 2^14 + 2^11 + 1
        //Hex form: 7FFFFC801
        //NTT prime: Yes
        //Plain modulus recommendation: Power of 2 up to 2^11
        //*/
        //{ 1024, BigUInt("7FFFFC801") },
        
        /*
        Polynomial modulus: 1x^2048 + 1
        Coefficient modulus: 2^60 - 2^14 + 1
        Hex form: FFFFFFFFFFFC001
        NTT prime: Yes
        Plain modulus recommendation: Power of 2 up to 2^14
        */
        { 2048, BigUInt("FFFFFFFFFFFC001") },

        /*
        Polynomial modulus: 1x^4096 + 1
        Coefficient modulus: 2^116 - 2^18 + 1
        Hex form: FFFFFFFFFFFFFFFFFFFFFFFFC0001
        NTT prime: Yes
        Plain modulus recommendation: Power of 2 up to 2^18
        */
        { 4096, BigUInt("FFFFFFFFFFFFFFFFFFFFFFFFC0001") },

        /*
        Polynomial modulus: 1x^8192 + 1
        Coefficient modulus: 2^226 - 2^26 + 1
        Hex form: 3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC000001
        NTT prime: Yes
        Plain modulus recommendation: Power of 2 up to 2^26
        */
        { 8192, BigUInt("3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC000001") },

        /*
        Polynomial modulus: 1x^16384 + 1
        Coefficient modulus: 2^435 - 2^33 + 1
        Hex form: 7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE00000001
        NTT prime: Yes
        Plain modulus recommendation: Power of 2 up to 2^33
        */
        { 16384, BigUInt("7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE00000001") },

        /*
        Polynomial modulus: 1x^32768 + 1
        Coefficient modulus: 2^889 - 2^54 - 2^53 - 2^52 + 1
        Hex form: 1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF90000000000001
        NTT prime: Yes
        Plain modulus recommendation: Power of 2 up to 2^52
        */
        { 32768, BigUInt("1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF90000000000001") },
    };

    const double ChooserEvaluator::default_noise_standard_deviation_ = 3.19;

    const double ChooserEvaluator::default_noise_max_deviation_ = 19.14;

    ChooserPoly::ChooserPoly() : max_coeff_count_(0), max_abs_value_(), comp_(nullptr)
    {
    }

    ChooserPoly::ChooserPoly(int max_coeff_count, const BigUInt &max_abs_value) :
        max_coeff_count_(max_coeff_count), max_abs_value_(max_abs_value), comp_(new FreshComputation(max_coeff_count, max_abs_value))
    {
        if (max_coeff_count <= 0)
        {
            throw invalid_argument("max_coeff_count must be strictly positive");
        }
        if (max_abs_value.is_zero())
        {
            max_coeff_count_ = 1;
        }
    }

    ChooserPoly::ChooserPoly(int max_coeff_count, uint64_t max_abs_value) :
        max_coeff_count_(max_coeff_count), max_abs_value_(bits_per_uint64, max_abs_value) 
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

    ChooserPoly::ChooserPoly(int max_coeff_count, const BigUInt &max_abs_value, Computation *comp) :
        max_coeff_count_(max_coeff_count), max_abs_value_(max_abs_value), comp_(comp)
    {
        if (max_coeff_count <= 0)
        {
            throw invalid_argument("max_coeff_count must be strictly positive");
        }

        if (max_abs_value.is_zero())
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

    ChooserPoly::ChooserPoly(int max_coeff_count, uint64_t max_abs_value, Computation *comp) :
        max_coeff_count_(max_coeff_count), max_abs_value_(bits_per_uint64, max_abs_value), comp_(comp)
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
        for (size_t i = 0; i < operands.size(); ++i)
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
            if (compare_uint_uint(operands[i].max_abs_value_.pointer(), operands[i].max_abs_value_.uint64_count(), operands[largest_abs_value_index].max_abs_value_.pointer(), operands[largest_abs_value_index].max_abs_value_.uint64_count() > 0))
            {
                largest_abs_value_index = i;
            }
        }

        int sum_max_abs_value_bit_count = operands[largest_abs_value_index].max_abs_value_.significant_bit_count() + get_significant_bit_count(operands.size());
        int sum_max_abs_value_uint64_count = divide_round_up(sum_max_abs_value_bit_count, bits_per_uint64);
        Pointer sum_max_abs_value(allocate_zero_uint(sum_max_abs_value_uint64_count, pool_));

        vector<Computation*> comps;
        for (size_t i = 0; i < operands.size(); ++i)
        {
            add_uint_uint(operands[i].max_abs_value_.pointer(), operands[i].max_abs_value_.uint64_count(), sum_max_abs_value.get(), sum_max_abs_value_uint64_count, false, sum_max_abs_value_uint64_count, sum_max_abs_value.get());
            comps.emplace_back(operands[i].comp_);
        }

        return ChooserPoly(sum_max_coeff_count, BigUInt(sum_max_abs_value_bit_count, sum_max_abs_value.get()), new AddManyComputation(comps));
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
        if (operand1.max_abs_value_.is_zero() || operand2.max_abs_value_.is_zero())
        {
            return ChooserPoly(1, 0, new MultiplyComputation(*operand1.comp_, *operand2.comp_));
        }

        uint64_t growth_factor = min(operand1.max_coeff_count_, operand2.max_coeff_count_);
        int prod_bit_count = operand1.max_abs_value_.significant_bit_count() + operand2.max_abs_value_.significant_bit_count() + get_significant_bit_count(growth_factor) + 1;
        int prod_uint64_count = divide_round_up(prod_bit_count, bits_per_uint64);

        Pointer prod_max_abs_value(allocate_zero_uint(prod_uint64_count, pool_));
        ConstPointer wide_operand2_max_abs_value(duplicate_uint_if_needed(operand2.max_abs_value_.pointer(), operand2.max_abs_value_.uint64_count(), prod_uint64_count, false, pool_));

        multiply_uint_uint(&growth_factor, 1, operand1.max_abs_value_.pointer(), operand1.max_abs_value_.uint64_count(), prod_uint64_count, prod_max_abs_value.get());
        ConstPointer temp_pointer(duplicate_uint_if_needed(prod_max_abs_value.get(), prod_uint64_count, prod_uint64_count, true, pool_));
        multiply_uint_uint(wide_operand2_max_abs_value.get(), prod_uint64_count, temp_pointer.get(), prod_uint64_count, prod_uint64_count, prod_max_abs_value.get());

        return ChooserPoly(operand1.max_coeff_count_ + operand2.max_coeff_count_ - 1, BigUInt(prod_bit_count, prod_max_abs_value.get()), new MultiplyComputation(*operand1.comp_, *operand2.comp_));
    }

    ChooserPoly ChooserEvaluator::square(const ChooserPoly &operand)
    {
        return multiply(operand, operand);
    }

    ChooserPoly ChooserEvaluator::relinearize(const ChooserPoly &operand, int destination_size)
    {
        if (operand.max_coeff_count_ <= 0 || operand.comp_ == nullptr)
        {
            throw invalid_argument("operand is not correctly initialized");
        }

        return ChooserPoly(operand.max_coeff_count_, operand.max_abs_value_, new RelinearizeComputation(*operand.comp_, destination_size));
    }

    ChooserPoly ChooserEvaluator::multiply_plain(const ChooserPoly &operand, int plain_max_coeff_count, const BigUInt &plain_max_abs_value)
    {
        if (operand.max_coeff_count_ <= 0 || operand.comp_ == nullptr)
        {
            throw invalid_argument("operand is not correctly initialized");
        }
        if (plain_max_coeff_count <= 0)
        {
            throw invalid_argument("plain_max_coeff_count must be positive");
        }
        if (plain_max_abs_value.is_zero())
        {
            throw invalid_argument("plain_max_abs_value cannot be zero");
        }
        if (operand.max_abs_value_.is_zero())
        {
            return ChooserPoly(1, 0, new MultiplyPlainComputation(*operand.comp_, plain_max_coeff_count, plain_max_abs_value));
        }

        uint64_t growth_factor = min(operand.max_coeff_count_, plain_max_coeff_count);
        int prod_bit_count = operand.max_abs_value_.significant_bit_count() + plain_max_abs_value.significant_bit_count() + get_significant_bit_count(growth_factor) + 1;
        int prod_uint64_count = divide_round_up(prod_bit_count, bits_per_uint64);

        Pointer prod_max_abs_value(allocate_zero_uint(prod_uint64_count, pool_));
        ConstPointer wide_operand_max_abs_value(duplicate_uint_if_needed(operand.max_abs_value_.pointer(), operand.max_abs_value_.uint64_count(), prod_uint64_count, false, pool_));

        multiply_uint_uint(&growth_factor, 1, plain_max_abs_value.pointer(), plain_max_abs_value.uint64_count(), prod_uint64_count, prod_max_abs_value.get());
        ConstPointer temp_pointer(duplicate_uint_if_needed(prod_max_abs_value.get(), prod_uint64_count, prod_uint64_count, true, pool_));
        multiply_uint_uint(wide_operand_max_abs_value.get(), prod_uint64_count, temp_pointer.get(), prod_uint64_count, prod_uint64_count, prod_max_abs_value.get());

        return ChooserPoly(operand.max_coeff_count_ + plain_max_coeff_count - 1, BigUInt(prod_bit_count, prod_max_abs_value.get()), new MultiplyPlainComputation(*operand.comp_, plain_max_coeff_count, plain_max_abs_value));
    }

    ChooserPoly ChooserEvaluator::multiply_plain(const ChooserPoly &operand, int plain_max_coeff_count, uint64_t plain_max_abs_value) 
    {
        return multiply_plain(operand, plain_max_coeff_count, BigUInt(bits_per_uint64, plain_max_abs_value));
    }

    ChooserPoly ChooserEvaluator::multiply_plain(const ChooserPoly &operand, const ChooserPoly &plain_chooser_poly)
    {
        if (plain_chooser_poly.comp_ != nullptr)
        {
            throw invalid_argument("plain_chooser_poly is not correctly initialized");
        }
        return multiply_plain(operand, plain_chooser_poly.max_coeff_count_, plain_chooser_poly.max_abs_value_);
    }

    ChooserPoly ChooserEvaluator::add_plain(const ChooserPoly &operand, int plain_max_coeff_count, const BigUInt &plain_max_abs_value)
    {
        if (operand.max_coeff_count_ <= 0 || operand.comp_ == nullptr)
        {
            throw invalid_argument("operand is not correctly initialized");
        }
        if (plain_max_coeff_count <= 0)
        {
            throw invalid_argument("plain_max_coeff_count must be positive");
        }
        if (plain_max_abs_value.is_zero())
        {
            return ChooserPoly(operand.max_coeff_count_, operand.max_abs_value_, new AddPlainComputation(*operand.comp_, plain_max_coeff_count, plain_max_abs_value));
        }
        if (operand.max_abs_value_.is_zero())
        {
            return ChooserPoly(plain_max_coeff_count, plain_max_abs_value, new AddPlainComputation(*operand.comp_, plain_max_coeff_count, plain_max_abs_value));
        }

        return ChooserPoly(max(operand.max_coeff_count_, plain_max_coeff_count), operand.max_abs_value_ + plain_max_abs_value, new AddPlainComputation(*operand.comp_, plain_max_coeff_count, plain_max_abs_value));
    }

    ChooserPoly ChooserEvaluator::add_plain(const ChooserPoly &operand, int plain_max_coeff_count, uint64_t plain_max_abs_value)
    {
        return add_plain(operand, plain_max_coeff_count, BigUInt(bits_per_uint64, plain_max_abs_value));
    }

    ChooserPoly ChooserEvaluator::add_plain(const ChooserPoly &operand, const ChooserPoly &plain_chooser_poly)
    {
        if (plain_chooser_poly.comp_ != nullptr)
        {
            throw invalid_argument("plain_chooser_poly is not correctly initialized");
        }
        return add_plain(operand, plain_chooser_poly.max_coeff_count_, plain_chooser_poly.max_abs_value_);
    }

    ChooserPoly ChooserEvaluator::sub_plain(const ChooserPoly &operand, int plain_max_coeff_count, const BigUInt &plain_max_abs_value)
    {
        if (operand.max_coeff_count_ <= 0 || operand.comp_ == nullptr)
        {
            throw invalid_argument("operand is not correctly initialized");
        }
        if (plain_max_coeff_count <= 0)
        {
            throw invalid_argument("plain_max_coeff_count must be positive");
        }
        if (plain_max_abs_value.is_zero())
        {
            return ChooserPoly(operand.max_coeff_count_, operand.max_abs_value_, new SubPlainComputation(*operand.comp_, plain_max_coeff_count, plain_max_abs_value));
        }
        if (operand.max_abs_value_.is_zero())
        {
            return ChooserPoly(plain_max_coeff_count, plain_max_abs_value, new SubPlainComputation(*operand.comp_, plain_max_coeff_count, plain_max_abs_value));
        }

        return ChooserPoly(max(operand.max_coeff_count_, plain_max_coeff_count), operand.max_abs_value_ + plain_max_abs_value, new SubPlainComputation(*operand.comp_, plain_max_coeff_count, plain_max_abs_value));
    }

    ChooserPoly ChooserEvaluator::sub_plain(const ChooserPoly &operand, int plain_max_coeff_count, uint64_t plain_max_abs_value)
    {
        return sub_plain(operand, plain_max_coeff_count, BigUInt(bits_per_uint64, plain_max_abs_value));
    }

    ChooserPoly ChooserEvaluator::sub_plain(const ChooserPoly &operand, const ChooserPoly &plain_chooser_poly)
    {
        if (plain_chooser_poly.comp_ != nullptr)
        {
            throw invalid_argument("plain_chooser_poly is not correctly initialized");
        }
        return sub_plain(operand, plain_chooser_poly.max_coeff_count_, plain_chooser_poly.max_abs_value_);
    }

    ChooserPoly ChooserEvaluator::exponentiate(const ChooserPoly &operand, uint64_t exponent)
    {
        if (operand.max_coeff_count_ <= 0 || operand.comp_ == nullptr)
        {
            throw invalid_argument("operand is not correctly initialized");
        }
        if (exponent == 0)
        {
            throw invalid_argument("exponent cannot be 0");
        }
        if (operand.max_abs_value_.is_zero())
        {
            return ChooserPoly(1, 0, new ExponentiateComputation(*operand.comp_, exponent));
        }

        // There is no known closed formula for the growth factor, but we use the asymptotic approximation
        // k^n * sqrt[6/((k-1)*(k+1)*Pi*n)], where k = max_coeff_count_, n = exponent.
        uint64_t growth_factor = static_cast<uint64_t>(pow(operand.max_coeff_count_, exponent) * sqrt(6 / ((operand.max_coeff_count_ - 1) * (operand.max_coeff_count_ + 1) * 3.1415 * exponent)));

        int result_bit_count = exponent * operand.max_abs_value_.significant_bit_count() + get_significant_bit_count(growth_factor) + 1;
        int result_uint64_count = divide_round_up(result_bit_count, bits_per_uint64);

        Pointer result_max_abs_value(allocate_uint(result_uint64_count, pool_));

        util::exponentiate_uint(operand.max_abs_value_.pointer(), operand.max_abs_value_.uint64_count(), &exponent, 1, result_uint64_count, result_max_abs_value.get(), pool_);

        ConstPointer temp_pointer(duplicate_uint_if_needed(result_max_abs_value.get(), result_uint64_count, result_uint64_count, true, pool_));
        multiply_uint_uint(&growth_factor, 1, temp_pointer.get(), result_uint64_count, result_uint64_count, result_max_abs_value.get());

        return ChooserPoly(exponent * (operand.max_coeff_count_ - 1) + 1, BigUInt(result_bit_count, result_max_abs_value.get()), new ExponentiateComputation(*operand.comp_, exponent));
    }

    ChooserPoly ChooserEvaluator::negate(const ChooserPoly &operand)
    {
        if (operand.max_coeff_count_ <= 0 || operand.comp_ == nullptr)
        {
            throw invalid_argument("operand is not correctly initialized");
        }
        return ChooserPoly(operand.max_coeff_count_, operand.max_abs_value_, new NegateComputation(*operand.comp_));
    }

    ChooserPoly ChooserEvaluator::multiply_many(const vector<ChooserPoly> &operands)
    {
        if (operands.empty())
        {
            throw invalid_argument("operands vector can not be empty");
        }

        int prod_max_coeff_count = 1;
        uint64_t growth_factor = 1;
        int prod_max_abs_value_bit_count = 1;
        vector<Computation*> comps;
        for (size_t i = 0; i < operands.size(); ++i)
        {
            // Throw if any of the operands is not initialized correctly
            if (operands[i].max_coeff_count_ <= 0 || operands[i].comp_ == nullptr)
            {
                throw invalid_argument("input operand is not correctly initialized");
            }

            // Return early if the product is trivially zero
            if (operands[i].max_abs_value_.is_zero())
            {
                return ChooserPoly(1, 0, new MultiplyManyComputation(comps));
            }

            prod_max_coeff_count += operands[i].max_coeff_count_ - 1;
            prod_max_abs_value_bit_count += operands[i].max_abs_value().significant_bit_count();

            growth_factor *= (i == 0 ? 1 : min(operands[i].max_coeff_count_, prod_max_coeff_count));

            comps.emplace_back(operands[i].comp_);
        }

        prod_max_abs_value_bit_count += get_significant_bit_count(growth_factor);
        int prod_max_abs_value_uint64_count = divide_round_up(prod_max_abs_value_bit_count, bits_per_uint64);

        Pointer prod_max_abs_value(allocate_zero_uint(prod_max_abs_value_uint64_count, pool_));
        *prod_max_abs_value.get() = growth_factor;
        for (size_t i = 0; i < operands.size(); ++i)
        {
            ConstPointer temp_pointer(duplicate_uint_if_needed(prod_max_abs_value.get(), prod_max_abs_value_uint64_count, prod_max_abs_value_uint64_count, true, pool_));
            multiply_uint_uint(temp_pointer.get(), prod_max_abs_value_uint64_count, operands[i].max_abs_value_.pointer(), operands[i].max_abs_value_.uint64_count(), prod_max_abs_value_uint64_count, prod_max_abs_value.get());
        }

        return ChooserPoly(prod_max_coeff_count, BigUInt(prod_max_abs_value_bit_count, prod_max_abs_value.get()), new MultiplyManyComputation(comps));
    }

    bool ChooserEvaluator::select_parameters(const std::vector<ChooserPoly> &operands, int budget_gap, EncryptionParameters &destination)
    {
        return select_parameters(operands, budget_gap, default_noise_standard_deviation_, default_noise_max_deviation_, default_parameter_options_, destination);
    }

    bool ChooserEvaluator::select_parameters(const std::vector<ChooserPoly> &operands, int budget_gap, double noise_standard_deviation, double noise_max_deviation, const std::map<int, BigUInt> &parameter_options, EncryptionParameters &destination)
    {
        if (budget_gap < 0)
        {
            throw std::invalid_argument("budget_gap cannot be negative");
        }
        if (noise_standard_deviation < 0)
        {
            throw invalid_argument("noise_standard_deviation can not be negative");
        }
        if (noise_max_deviation < 0)
        {
            throw invalid_argument("noise_max_deviation can not be negative");
        }
        if (parameter_options.size() == 0)
        {
            throw invalid_argument("parameter_options must contain at least one entry");
        }
        if (operands.empty())
        {
            throw invalid_argument("operands cannot be empty");
        }

        int largest_bit_count = 0;
        int largest_coeff_count = 0;
        for (size_t i = 0; i < operands.size(); ++i)
        {
            if (operands[i].comp_ == nullptr)
            {
                throw logic_error("no operation history to simulate");
            }
            int current_bit_count = operands[i].max_abs_value_.significant_bit_count();
            largest_bit_count = (current_bit_count > largest_bit_count) ? current_bit_count : largest_bit_count;

            int current_coeff_count = operands[i].max_coeff_count_;
            largest_coeff_count = (current_coeff_count > largest_coeff_count) ? current_coeff_count : largest_coeff_count;
        }

        // We restrict to plain moduli that are powers of two. Here largest_bit_count is the largest positive
        // coefficient that we can expect to appear. Thus, we need one more bit.
        BigUInt new_plain_modulus(1, 1);
        new_plain_modulus <<= largest_bit_count;
        destination.set_plain_modulus(new_plain_modulus);

        bool found_good_parms = false;
        map<int, BigUInt>::const_iterator iter = parameter_options.begin();
        while (iter != parameter_options.end() && !found_good_parms)
        {
            int dimension = iter->first;
            if (dimension < 512 || (dimension & (dimension - 1)) != 0)
            {
                throw invalid_argument("parameter_options keys invalid");
            }

            if (dimension > largest_coeff_count && destination.plain_modulus() < iter->second)
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

                // We truncate the gaussian at noise_max_deviation.
                destination.set_noise_max_deviation(noise_max_deviation);

                // Start initially with the maximum decomposition_bit_count, then decrement until decrypts().
                destination.set_decomposition_bit_count(destination.coeff_modulus().significant_bit_count());
                
                // If decomposition_bit_count = destination.coeff_modulus().significant_bit_count() works, then
                // the computation did not involve any relinearization operations, because relinearizing with 
                // such a large decomposition bit count would immediately make the ciphertext undecryptable.
                // This way we can detect whether relinearization was needed, and if not, to signal this we set
                // the decomposition bit count to 0.
                found_good_parms = true;

                for (size_t i = 0; i < operands.size(); i++)
                {
                    // If one of the operands does not decrypt, set found_good_parms to false.
                    found_good_parms = operands[i].simulate(destination).decrypts(budget_gap) ? found_good_parms : false;
                }

                if (found_good_parms)
                {
                    destination.set_decomposition_bit_count(0);
                }

                // We bound the decomposition bit count value by 1/5 of the maximum. A too small
                // decomposition bit count slows down multiplication significantly. This is not an
                // issue when the user wants to use multiply_norelin() instead of multiply(), as it
                // only affects the relinearization step. The fraction 1/5 is by no means an optimal 
                // choice. An expert user might want to tweak this value to be smaller or larger 
                // depending on their use case.
                int min_decomposition_bit_count = (destination.coeff_modulus().significant_bit_count() + 9) / 10;

                // Note that this part of the code will be skipped if the computation does not involve relinearization
                // the decomposition bit count was already set to 0 above.
                while (!found_good_parms && destination.decomposition_bit_count() > min_decomposition_bit_count)
                {
                    found_good_parms = true;
                    for (size_t i = 0; i < operands.size(); ++i)
                    {
                        // If one of the operands does not decrypt, set found_good_parms to false.
                        // We call decrypts() with parameter budget_gap == 0
                        found_good_parms = operands[i].simulate(destination).decrypts(budget_gap) ? found_good_parms : false;
                    }

                    if (!found_good_parms)
                    {
                        destination.set_decomposition_bit_count(destination.decomposition_bit_count() - 1);
                    }
                    else
                    {
                        // We found some good parameters. But in fact we can still decrease the decomposition count
                        // a little bit without hurting performance at all.
                        int old_dbc = destination.decomposition_bit_count();
                        int num_parts = (destination.coeff_modulus().significant_bit_count() + (old_dbc - 1))/ old_dbc;
                        destination.set_decomposition_bit_count((destination.coeff_modulus().significant_bit_count() + (num_parts - 1)) / num_parts);
                    }
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

    Simulation ChooserPoly::simulate(EncryptionParameters &parms) const
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
        max_abs_value_ = BigUInt(1, static_cast<uint64_t>(0));
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

    ChooserEncoder::ChooserEncoder(uint64_t base) : encoder_(BigUInt(get_significant_bit_count(base), base), base)
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
        BigPoly value_poly = encoder_.encode(value);
        destination.reset();
        destination.max_coeff_count() = value_poly.significant_coeff_count();
        destination.max_abs_value() = poly_infty_norm_coeffmod(value_poly, encoder_.plain_modulus());
    }

    ChooserPoly ChooserEncoder::encode(int64_t value)
    {
        ChooserPoly chooser_poly;
        encode(value, chooser_poly);
        return chooser_poly;
    }

    void ChooserEncoder::encode(int64_t value, ChooserPoly &destination)
    {
        BigPoly value_poly = encoder_.encode(value);
        destination.reset();
        destination.max_coeff_count() = max(value_poly.significant_coeff_count(), 1);
        destination.max_abs_value() = poly_infty_norm_coeffmod(value_poly, encoder_.plain_modulus());
    }

    ChooserPoly ChooserEncoder::encode(BigUInt value)
    {
        ChooserPoly chooser_poly;
        encode(value, chooser_poly);
        return chooser_poly;
    }

    void ChooserEncoder::encode(BigUInt value, ChooserPoly &destination)
    {
        BigPoly value_poly = encoder_.encode(value);
        destination.reset();
        destination.max_coeff_count() = value_poly.significant_coeff_count();
        destination.max_abs_value() = poly_infty_norm_coeffmod(value_poly, encoder_.plain_modulus());
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