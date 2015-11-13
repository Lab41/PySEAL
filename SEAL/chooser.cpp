#include <algorithm>
#include <map>
#include <cstdint>
#include "chooser.h"
#include "utilities.h"
#include "util/uintarith.h"

using namespace std;
using namespace seal::util;

namespace seal
{
    const map<int, BigUInt> ChooserPoly::default_options_
    {
        { 1024, BigUInt("FFFFFFFFC001") },
        { 2048, BigUInt("7FFFFFFFFFFFFFFFFFFF001") },
        { 4096, BigUInt("7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF") },
        { 8192, BigUInt("1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC000001") }
    };

    ChooserPoly::ChooserPoly() : max_coeff_count_(0), max_abs_value_(), comp_(nullptr)
    {
    }

    ChooserPoly::ChooserPoly(int max_coeff_count, BigUInt max_abs_value) :
        max_coeff_count_(max_coeff_count), max_abs_value_(max_abs_value), comp_(new FreshComputation())
    {
        if (max_coeff_count <= 0)
        {
            throw out_of_range("max_coeff_count");
        }
        if (max_abs_value.is_zero())
        {
            max_coeff_count_ = 1;
        }
    }

    ChooserPoly::ChooserPoly(int max_coeff_count, uint64_t max_abs_value) :
        max_coeff_count_(max_coeff_count), comp_(new FreshComputation())
    {
        if (max_coeff_count <= 0)
        {
            throw out_of_range("max_coeff_count");
        }
        if (max_abs_value == 0)
        {
            max_coeff_count_ = 1;
        }
        BigUInt max_abs_value_uint;
        max_abs_value_uint = max_abs_value;
        max_abs_value_ = max_abs_value_uint;
    }

    ChooserPoly::ChooserPoly(int max_coeff_count, BigUInt max_abs_value, Computation *comp) :
        max_coeff_count_(max_coeff_count), max_abs_value_(max_abs_value), comp_(comp)
    {
        if (max_coeff_count <= 0)
        {
            throw out_of_range("max_coeff_count");
        }
        if (max_abs_value.is_zero())
        {
            max_coeff_count_ = 1;
        }
        if (comp == nullptr)
        {
            comp_ = new FreshComputation();
        }
    }

    ChooserPoly::ChooserPoly(int max_coeff_count, uint64_t max_abs_value, Computation *comp) :
        max_coeff_count_(max_coeff_count), comp_(comp)
    {
        if (max_coeff_count <= 0)
        {
            throw out_of_range("max_coeff_count");
        }
        if (max_abs_value == 0)
        {
            max_coeff_count_ = 1;
        }
        BigUInt max_abs_value_uint;
        max_abs_value_uint = max_abs_value;
        max_abs_value_ = max_abs_value_uint;
        if (comp == nullptr)
        {
            comp_ = new FreshComputation();
        }
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

    ChooserPoly ChooserEvaluator::add(const ChooserPoly &operand1, const ChooserPoly &operand2) const
    {
        if (operand1.max_coeff_count_ <= 0 && operand1.comp_ != nullptr)
        {
            throw invalid_argument("operand1 is not correctly initialized");
        }
        if (operand2.max_coeff_count_ <= 0 && operand2.comp_ != nullptr)
        {
            throw invalid_argument("operand2 is not correctly initialized");
        }
        if (operand1.comp_ == nullptr && operand2.comp_ == nullptr)
        {
            return ChooserPoly();
        }
        if (operand1.comp_ == nullptr && operand2.comp_ != nullptr)
        {
            return operand2;
        }
        if (operand1.comp_ != nullptr && operand2.comp_ == nullptr)
        {
            return operand1;
        }

        int wide_bit_length = max(operand1.max_abs_value_.significant_bit_count(), operand2.max_abs_value_.significant_bit_count()) + 1;
        int wide_uint64_count = divide_round_up(wide_bit_length, bits_per_uint64);
        BigUInt wide_max_abs_value(wide_bit_length);
        BigUInt wide_operand_max_abs_value(wide_bit_length);
        BigUInt sum_max_abs_value(wide_bit_length);
        wide_max_abs_value = operand1.max_abs_value_;
        wide_operand_max_abs_value = operand2.max_abs_value_;
        add_uint_uint(wide_max_abs_value.pointer(), wide_operand_max_abs_value.pointer(), wide_uint64_count, sum_max_abs_value.pointer());

        return ChooserPoly(max(operand1.max_coeff_count_, operand2.max_coeff_count_), sum_max_abs_value, new AddComputation(*operand1.comp_, *operand2.comp_));
    }

    ChooserPoly ChooserEvaluator::sub(const ChooserPoly &operand1, const ChooserPoly &operand2) const
    {
        if (operand1.max_coeff_count_ <= 0 && operand1.comp_ != nullptr)
        {
            throw invalid_argument("operand1 is not correctly initialized");
        }
        if (operand2.max_coeff_count_ <= 0 && operand2.comp_ != nullptr)
        {
            throw invalid_argument("operand2 is not correctly initialized");
        }
        if (operand1.comp_ == nullptr && operand2.comp_ == nullptr)
        {
            return ChooserPoly();
        }
        if (operand1.comp_ == nullptr && operand2.comp_ != nullptr)
        {
            return operand2;
        }
        if (operand1.comp_ != nullptr && operand2.comp_ == nullptr)
        {
            return operand1;
        }

        int wide_bit_length = max(operand1.max_abs_value_.significant_bit_count(), operand2.max_abs_value_.significant_bit_count()) + 1;
        int wide_uint64_count = divide_round_up(wide_bit_length, bits_per_uint64);
        BigUInt wide_max_abs_value(wide_bit_length);
        BigUInt wide_operand_max_abs_value(wide_bit_length);
        BigUInt diff_max_abs_value(wide_bit_length);
        wide_max_abs_value = operand1.max_abs_value_;
        wide_operand_max_abs_value = operand2.max_abs_value_;

        add_uint_uint(wide_max_abs_value.pointer(), wide_operand_max_abs_value.pointer(), wide_uint64_count, diff_max_abs_value.pointer());

        return ChooserPoly(max(operand1.max_coeff_count_, operand2.max_coeff_count_), diff_max_abs_value, new SubComputation(*operand1.comp_, *operand2.comp_));
    }

    ChooserPoly ChooserEvaluator::multiply(const ChooserPoly &operand1, const ChooserPoly &operand2) const
    {
        if (operand1.max_coeff_count_ <= 0 && operand1.comp_ != nullptr)
        {
            throw invalid_argument("operand1 is not correctly initialized");
        }
        if (operand2.max_coeff_count_ <= 0 && operand2.comp_ != nullptr)
        {
            throw invalid_argument("operand2 is not correctly initialized");
        }
        if (operand1.comp_ == nullptr || operand2.comp_ == nullptr)
        {
            return ChooserPoly();
        }
        if (operand1.max_abs_value_.is_zero() || operand2.max_abs_value_.is_zero())
        {
            return ChooserPoly(1, 0, new MultiplyComputation(*operand1.comp_, *operand2.comp_));
        }

        int growth_factor = min(operand1.max_coeff_count_, operand2.max_coeff_count_);
        int wide_bit_length = operand1.max_abs_value_.significant_bit_count() + operand2.max_abs_value_.significant_bit_count() + get_significant_bit_count(growth_factor) + 1;
        int wide_uint64_count = divide_round_up(wide_bit_length, bits_per_uint64);
        BigUInt growth_factor_uint;
        BigUInt prod_max_abs_value(wide_bit_length);
        BigUInt temp_uint(wide_bit_length);
        growth_factor_uint = growth_factor;

        multiply_uint_uint(operand1.max_abs_value_.pointer(), operand1.max_abs_value_.uint64_count(), operand2.max_abs_value_.pointer(), operand2.max_abs_value_.uint64_count(), wide_uint64_count, temp_uint.pointer());
        multiply_uint_uint(temp_uint.pointer(), wide_uint64_count, growth_factor_uint.pointer(), growth_factor_uint.uint64_count(), wide_uint64_count, prod_max_abs_value.pointer());

        return ChooserPoly(operand1.max_coeff_count_ + operand2.max_coeff_count_ - 1, prod_max_abs_value, new MultiplyComputation(*operand1.comp_, *operand2.comp_));
    }

    ChooserPoly ChooserEvaluator::multiply_norelin(const ChooserPoly &operand1, const ChooserPoly &operand2) const
    {
        if (operand1.max_coeff_count_ <= 0 && operand1.comp_ != nullptr)
        {
            throw invalid_argument("operand1 is not correctly initialized");
        }
        if (operand2.max_coeff_count_ <= 0 && operand2.comp_ != nullptr)
        {
            throw invalid_argument("operand2 is not correctly initialized");
        }
        if (operand1.comp_ == nullptr || operand2.comp_ == nullptr)
        {
            return ChooserPoly();
        }
        if (operand1.max_abs_value_.is_zero() || operand2.max_abs_value_.is_zero())
        {
            return ChooserPoly(1, 0, new MultiplyNoRelinComputation(*operand1.comp_, *operand2.comp_));
        }

        int growth_factor = min(operand1.max_coeff_count_, operand2.max_coeff_count_);
        int wide_bit_length = operand1.max_abs_value_.significant_bit_count() + operand2.max_abs_value_.significant_bit_count() + get_significant_bit_count(growth_factor) + 1;
        int wide_uint64_count = divide_round_up(wide_bit_length, bits_per_uint64);
        BigUInt growth_factor_uint;
        BigUInt prod_max_abs_value(wide_bit_length);
        BigUInt temp_uint(wide_bit_length);
        growth_factor_uint = growth_factor;

        multiply_uint_uint(operand1.max_abs_value_.pointer(), operand1.max_abs_value_.uint64_count(), operand2.max_abs_value_.pointer(), operand2.max_abs_value_.uint64_count(), wide_uint64_count, temp_uint.pointer());
        multiply_uint_uint(temp_uint.pointer(), wide_uint64_count, growth_factor_uint.pointer(), growth_factor_uint.uint64_count(), wide_uint64_count, prod_max_abs_value.pointer());

        return ChooserPoly(operand1.max_coeff_count_ + operand2.max_coeff_count_ - 1, prod_max_abs_value, new MultiplyNoRelinComputation(*operand1.comp_, *operand2.comp_));
    }

    ChooserPoly ChooserEvaluator::relinearize(const ChooserPoly &operand) const
    {
        if (operand.max_coeff_count_ <= 0 || operand.comp_ == nullptr)
        {
            throw invalid_argument("operand is not correctly initialized");
        }

        return ChooserPoly(operand.max_coeff_count_, operand.max_abs_value_, new RelinearizeComputation(*operand.comp_));
    }

    ChooserPoly ChooserEvaluator::multiply_plain(const ChooserPoly &operand, int plain_max_coeff_count, const BigUInt &plain_max_abs_value) const
    {
        if (operand.max_coeff_count_ <= 0 && operand.comp_ != nullptr)
        {
            throw invalid_argument("operand is not correctly initialized");
        }
        if (plain_max_coeff_count <= 0)
        {
            throw invalid_argument("plain_max_coeff_count must be positive");
        }
        if (operand.comp_ == nullptr)
        {
            return ChooserPoly();
        }
        if (plain_max_abs_value.is_zero())
        {
            return ChooserPoly(1, 0, new MultiplyPlainComputation(*operand.comp_, plain_max_coeff_count, plain_max_abs_value));
        }
        if (operand.max_abs_value_.is_zero())
        {
            return ChooserPoly(1, 0, new MultiplyPlainComputation(*operand.comp_, plain_max_coeff_count, plain_max_abs_value));
        }

        int growth_factor = min(operand.max_coeff_count_, plain_max_coeff_count);
        int wide_bit_length = operand.max_abs_value_.significant_bit_count() + plain_max_abs_value.significant_bit_count() + get_significant_bit_count(growth_factor) + 1;
        int wide_uint64_count = divide_round_up(wide_bit_length, bits_per_uint64);
        BigUInt growth_factor_uint;
        BigUInt wide_plain_max_abs_value;
        BigUInt prod_max_abs_value(wide_bit_length);
        BigUInt temp_uint(wide_bit_length);
        growth_factor_uint = growth_factor;
        wide_plain_max_abs_value = plain_max_abs_value;

        multiply_uint_uint(operand.max_abs_value_.pointer(), operand.max_abs_value_.uint64_count(), wide_plain_max_abs_value.pointer(), wide_plain_max_abs_value.uint64_count(), wide_uint64_count, temp_uint.pointer());
        multiply_uint_uint(temp_uint.pointer(), wide_uint64_count, growth_factor_uint.pointer(), growth_factor_uint.uint64_count(), wide_uint64_count, prod_max_abs_value.pointer());

        return ChooserPoly(operand.max_coeff_count_ + plain_max_coeff_count - 1, prod_max_abs_value, new MultiplyPlainComputation(*operand.comp_, plain_max_coeff_count, plain_max_abs_value));
    }

    ChooserPoly ChooserEvaluator::multiply_plain(const ChooserPoly &operand, int plain_max_coeff_count, uint64_t plain_max_abs_value) const
    {
        BigUInt plain_max_abs_value_uint;
        plain_max_abs_value_uint = plain_max_abs_value;
        return multiply_plain(operand, plain_max_coeff_count, plain_max_abs_value_uint);
    }

    ChooserPoly ChooserEvaluator::add_plain(const ChooserPoly &operand, int plain_max_coeff_count, const BigUInt &plain_max_abs_value) const
    {
        if (operand.max_coeff_count_ <= 0 && operand.comp_ != nullptr)
        {
            throw invalid_argument("operand is not correctly initialized");
        }
        if (plain_max_coeff_count <= 0)
        {
            throw invalid_argument("plain_max_coeff_count must be positive");
        }
        if (operand.comp_ == nullptr)
        {
            throw invalid_argument("operand is not correctly initialized");
        }
        if (plain_max_abs_value.is_zero())
        {
            return ChooserPoly(operand.max_coeff_count_, operand.max_abs_value_, new AddPlainComputation(*operand.comp_));
        }
        if (operand.max_abs_value_.is_zero())
        {
            return ChooserPoly(plain_max_coeff_count, plain_max_abs_value, new AddPlainComputation(*operand.comp_));
        }

        int wide_bit_length = max(operand.max_abs_value_.significant_bit_count(), plain_max_abs_value.significant_bit_count()) + 1;
        int wide_uint64_count = divide_round_up(wide_bit_length, bits_per_uint64);
        BigUInt wide_max_abs_value(wide_bit_length);
        BigUInt wide_plain_max_abs_value(wide_bit_length);
        BigUInt sum_max_abs_value(wide_bit_length);
        wide_max_abs_value = operand.max_abs_value_;
        wide_plain_max_abs_value = plain_max_abs_value;
        add_uint_uint(wide_max_abs_value.pointer(), wide_plain_max_abs_value.pointer(), wide_uint64_count, sum_max_abs_value.pointer());

        return ChooserPoly(max(operand.max_coeff_count_, plain_max_coeff_count), sum_max_abs_value, new AddPlainComputation(*operand.comp_));
    }

    ChooserPoly ChooserEvaluator::add_plain(const ChooserPoly &operand, int plain_max_coeff_count, uint64_t plain_max_abs_value) const
    {
        BigUInt plain_max_abs_value_uint;
        plain_max_abs_value_uint = plain_max_abs_value;
        return add_plain(operand, plain_max_coeff_count, plain_max_abs_value_uint);
    }

    ChooserPoly ChooserEvaluator::sub_plain(const ChooserPoly &operand, int plain_max_coeff_count, const BigUInt &plain_max_abs_value) const
    {
        if (operand.max_coeff_count_ <= 0 && operand.comp_ != nullptr)
        {
            throw invalid_argument("operand is not correctly initialized");
        }
        if (plain_max_coeff_count <= 0)
        {
            throw invalid_argument("plain_max_coeff_count must be positive");
        }
        if (operand.comp_ == nullptr)
        {
            throw invalid_argument("operand is not correctly initialized");
        }
        if (plain_max_abs_value.is_zero())
        {
            return ChooserPoly(operand.max_coeff_count_, operand.max_abs_value_, new SubPlainComputation(*operand.comp_));
        }
        if (operand.max_abs_value_.is_zero())
        {
            return ChooserPoly(plain_max_coeff_count, plain_max_abs_value, new SubPlainComputation(*operand.comp_));
        }

        int wide_bit_length = max(operand.max_abs_value_.significant_bit_count(), plain_max_abs_value.significant_bit_count()) + 1;
        int wide_uint64_count = divide_round_up(wide_bit_length, bits_per_uint64);
        BigUInt wide_max_abs_value(wide_bit_length);
        BigUInt wide_plain_max_abs_value(wide_bit_length);
        BigUInt diff_max_abs_value(wide_bit_length);
        wide_max_abs_value = operand.max_abs_value_;
        wide_plain_max_abs_value = plain_max_abs_value;
        add_uint_uint(wide_max_abs_value.pointer(), wide_plain_max_abs_value.pointer(), wide_uint64_count, diff_max_abs_value.pointer());

        return ChooserPoly(max(operand.max_coeff_count_, plain_max_coeff_count), diff_max_abs_value, new SubPlainComputation(*operand.comp_));
    }

    ChooserPoly ChooserEvaluator::sub_plain(const ChooserPoly &operand, int plain_max_coeff_count, uint64_t plain_max_abs_value) const
    {
        BigUInt plain_max_abs_value_uint;
        plain_max_abs_value_uint = plain_max_abs_value;
        return sub_plain(operand, plain_max_coeff_count, plain_max_abs_value_uint);
    }

    ChooserPoly ChooserEvaluator::binary_exponentiate(const ChooserPoly &operand, int exponent) const
    {
        if (operand.max_coeff_count_ <= 0 && operand.comp_ != nullptr)
        {
            throw invalid_argument("operand is not correctly initialized");
        }
        if (exponent < 0)
        {
            throw invalid_argument("exponent can not be negative");
        }
        if (operand.comp_ == nullptr)
        {
            return ChooserPoly();
        }
        if (exponent == 0 && operand.max_abs_value_.is_zero())
        {
            throw invalid_argument("undefined operation");
        }
        if (exponent == 0)
        {
            return ChooserPoly(1, 1, new BinaryExponentiateComputation(*operand.comp_, exponent));
        }
        if (operand.max_abs_value_.is_zero())
        {
            return ChooserPoly(1, 0, new BinaryExponentiateComputation(*operand.comp_, exponent));
        }

        // There is no known closed formula for the growth factor, but we use the asymptotic approximation
        // k^n * sqrt[6/((k-1)*(k+1)*Pi*n)], where k = max_coeff_count_, n = exponent.
        uint64_t growth_factor = static_cast<uint64_t>(pow(operand.max_coeff_count_, exponent) * sqrt(6 / ((operand.max_coeff_count_ - 1) * (operand.max_coeff_count_ + 1) * 3.1415 * exponent)));
        int wide_bit_length = exponent * operand.max_abs_value_.significant_bit_count() + get_significant_bit_count(growth_factor) + 1;
        int wide_uint64_count = divide_round_up(wide_bit_length, bits_per_uint64);
        BigUInt growth_factor_uint;
        BigUInt exp_max_abs_value(wide_bit_length);
        BigUInt temp_uint(wide_bit_length);
        growth_factor_uint = growth_factor;
        exponentiate_uint(operand.max_abs_value_, exponent, temp_uint);
        multiply_uint_uint(temp_uint.pointer(), wide_uint64_count, growth_factor_uint.pointer(), growth_factor_uint.uint64_count(), wide_uint64_count, exp_max_abs_value.pointer());

        return ChooserPoly(exponent * (operand.max_coeff_count_ - 1) + 1, exp_max_abs_value, new BinaryExponentiateComputation(*operand.comp_, exponent));
    }

    ChooserPoly ChooserEvaluator::tree_exponentiate(const ChooserPoly &operand, int exponent) const
    {
        if (operand.max_coeff_count_ <= 0 && operand.comp_ != nullptr)
        {
            throw invalid_argument("operand is not correctly initialized");
        }
        if (exponent < 0)
        {
            throw invalid_argument("exponent can not be negative");
        }
        if (operand.comp_ == nullptr)
        {
            return ChooserPoly();
        }
        if (exponent == 0 && operand.max_abs_value_.is_zero())
        {
            throw invalid_argument("undefined operation");
        }
        if (exponent == 0)
        {
            return ChooserPoly(1, 1, new TreeExponentiateComputation(*operand.comp_, exponent));
        }
        if (operand.max_abs_value_.is_zero())
        {
            return ChooserPoly(1, 0, new TreeExponentiateComputation(*operand.comp_, exponent));
        }

        // There is no known closed formula for the growth factor, but we use the asymptotic approximation
        // k^n * sqrt[6/((k-1)*(k+1)*Pi*n)], where k = max_coeff_count_, n = exponent.
        uint64_t growth_factor = static_cast<uint64_t>(pow(operand.max_coeff_count_, exponent) * sqrt(6 / ((operand.max_coeff_count_ - 1) * (operand.max_coeff_count_ + 1) * 3.1415 * exponent)));
        int wide_bit_length = exponent * operand.max_abs_value_.significant_bit_count() + get_significant_bit_count(growth_factor) + 1;
        int wide_uint64_count = divide_round_up(wide_bit_length, bits_per_uint64);
        BigUInt growth_factor_uint;
        BigUInt exp_max_abs_value(wide_bit_length);
        BigUInt temp_uint(wide_bit_length);
        growth_factor_uint = growth_factor;
        exponentiate_uint(operand.max_abs_value_, exponent, temp_uint);
        multiply_uint_uint(temp_uint.pointer(), wide_uint64_count, growth_factor_uint.pointer(), growth_factor_uint.uint64_count(), wide_uint64_count, exp_max_abs_value.pointer());

        return ChooserPoly(exponent * (operand.max_coeff_count_ - 1) + 1, exp_max_abs_value, new TreeExponentiateComputation(*operand.comp_, exponent));
    }

    ChooserPoly ChooserEvaluator::negate(const ChooserPoly &operand) const
    {
        if (operand.max_coeff_count_ <= 0 && operand.comp_ != nullptr)
        {
            throw invalid_argument("operand is not correctly initialized");
        }
        if (operand.comp_ == nullptr)
        {
            return ChooserPoly();
        }
        return ChooserPoly(operand.max_coeff_count_, operand.max_abs_value_, new NegateComputation(*operand.comp_));
    }

    ChooserPoly ChooserEvaluator::tree_multiply(const vector<ChooserPoly> &operands) const
    {
        if (operands.empty())
        {
            throw invalid_argument("operands vector must not be empty");
        }

        int prod_max_coeff_count = 1;
        uint64_t growth_factor = 1;
        int wide_bit_length = 1;
        vector<const Computation*> comps;
        int operand_count = static_cast<int>(operands.size());
        for (int i = 0; i < operand_count; ++i)
        {
            // Throw if any of the operands is not initialized correctly
            if (operands[i].max_coeff_count_ <= 0 && operands[i].comp_ != nullptr)
            {
                throw invalid_argument("input operand is not correctly initialized");
            }
            if (operands[i].comp_ == nullptr)
            {
                return ChooserPoly();
            }

            prod_max_coeff_count += operands[i].max_coeff_count_ - 1;
            wide_bit_length += operands[i].max_abs_value().significant_bit_count();

            // We could get a better estimate for the growth factor by doing this in a particular order...?
            growth_factor *= (i == 0 ? 1 : min(operands[i].max_coeff_count_, prod_max_coeff_count));

            comps.push_back(operands[i].comp_);
        }

        // Return early if the product is trivially zero
        for (int i = 0; i < operand_count; ++i)
        {
            if (operands[i].max_abs_value_.is_zero())
            {
                return ChooserPoly(1, 0, new TreeMultiplyComputation(comps));
            }
        }

        wide_bit_length += get_significant_bit_count(growth_factor);
        int wide_uint64_count = divide_round_up(wide_bit_length, bits_per_uint64);

        BigUInt prod_max_abs_value(wide_bit_length);
        BigUInt temp_uint(wide_bit_length);
        prod_max_abs_value = growth_factor;
        for (int i = 0; i < operand_count; ++i)
        {
            multiply_uint_uint(prod_max_abs_value.pointer(), wide_bit_length, operands[i].max_abs_value_.pointer(), operands[i].max_abs_value_.uint64_count(), wide_uint64_count, temp_uint.pointer());
            set_uint_uint(temp_uint.pointer(), wide_uint64_count, prod_max_abs_value.pointer());
        }

        return ChooserPoly(prod_max_coeff_count, prod_max_abs_value, new TreeMultiplyComputation(comps));
    }

    EncryptionParameters ChooserPoly::select_parameters() const
    {
        return select_parameters(default_options_);
    }

    EncryptionParameters ChooserPoly::select_parameters(const std::map<int, BigUInt> &options) const
    {
        EncryptionParameters parms;
        parms.plain_modulus() = 1 << max_abs_value_.significant_bit_count();

        bool found_good_parms = false;
        map<int, BigUInt>::const_iterator iter = options.begin();
        while (iter != options.end() && !found_good_parms)
        {
            int dimension = iter->first;
            if (dimension + 1 > max_coeff_count_)
            {
                parms.coeff_modulus() = iter->second;
                parms.poly_modulus().resize(dimension + 1, 1);
                parms.poly_modulus().set_zero();
                parms.poly_modulus()[0] = 1;
                parms.poly_modulus()[dimension] = 1;

                // The bound needed for GapSVP->search-LWE reduction
                //parms.noise_standard_deviation() = round(sqrt(dimension / (2 * 3.1415)) + 0.5);

                // Use constant (small) standard deviation. For possibly more security, instead use the
                // setting above.
                parms.noise_standard_deviation() = 3.19;

                parms.noise_max_deviation() = 5 * parms.noise_standard_deviation();

                // Start initially with the maximum decomposition_bit_count, then decrement until decrypts().
                parms.decomposition_bit_count() = parms.coeff_modulus().significant_bit_count();

                // We bound the decomposition bit count value by one fifth of the maximum. A too small
                // decomposition bit count slows down multiplication significantly. This is not an
                // issue when the user wants to use multiply_norelin() instead of multiply(), as it
                // only affects the relinearization step. The fraction one fifth is not an optimal choice
                // in any sense, but was rather arbitrarily chosen. An expert user might want to tweak this
                // value to be smaller or larger depending on their use case.
                int min_decomposition_bit_count = parms.coeff_modulus().significant_bit_count() / 5;

                while (!found_good_parms && parms.decomposition_bit_count() > min_decomposition_bit_count)
                {
                    found_good_parms = simulate(parms).decrypts();
                    if (!found_good_parms)
                    {
                        --parms.decomposition_bit_count();
                    }
                    else
                    {
                        // Now make the decomposition_bit_count as small as possible without harming performance
                        int old_dbc = parms.decomposition_bit_count();
                        int num_parts = parms.coeff_modulus().significant_bit_count() / old_dbc + (parms.coeff_modulus().significant_bit_count() % old_dbc != 0);
                        parms.decomposition_bit_count() = parms.coeff_modulus().significant_bit_count() / num_parts + (parms.coeff_modulus().significant_bit_count() % num_parts != 0);
                    }
                }
            }

            ++iter;
        }

        if (found_good_parms)
        {
            return parms;
        }
        else
        {
            return EncryptionParameters();
        }
    }

    void ChooserPoly::reset()
    {
        if (comp_ != nullptr)
        {
            delete comp_;
            comp_ = nullptr;
        }
        max_abs_value_ = BigUInt();
        max_coeff_count_ = 0;
    }

    void ChooserPoly::set_fresh()
    {
        if (comp_ != nullptr)
        {
            delete comp_;
            comp_ = nullptr;
        }
        comp_ = new FreshComputation();
    }

    ChooserEncoder::ChooserEncoder(int base) : encoder_(BigUInt(get_significant_bit_count(base), base), base)
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
        destination.max_coeff_count() = value_poly.significant_coeff_count();
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

    void ChooserEncoder::set_base(int base)
    {
        encoder_.set_base(base);
        BigUInt plain_mod(get_significant_bit_count(base), base);
        encoder_.set_plain_modulus(plain_mod);
    }
}