#include <stdexcept>
#include "util/computation.h"

using namespace seal;
using namespace std;

namespace seal
{
    namespace util
    {
        FreshComputation::FreshComputation()
        {
        }

        FreshComputation::~FreshComputation()
        {
        }

        Simulation FreshComputation::simulate(EncryptionParameters &parms) const
        {
            return Simulation(parms);
        }

        FreshComputation *FreshComputation::clone() const
        {
            return new FreshComputation();
        }

        AddComputation::AddComputation(const Computation &input1, const Computation &input2)
        {
            input1_ = input1.clone();
            input2_ = input2.clone();
        }

        AddComputation::~AddComputation()
        {
            delete input1_;
            delete input2_;
        }

        Simulation AddComputation::simulate(EncryptionParameters &parms) const
        {
            return simulation_evaluator_.add(input1_->simulate(parms), input2_->simulate(parms));
        }

        AddComputation *AddComputation::clone() const
        {
            return new AddComputation(*input1_, *input2_);
        }

        SubComputation::SubComputation(const Computation &input1, const Computation &input2)
        {
            input1_ = input1.clone();
            input2_ = input2.clone();
        }

        SubComputation::~SubComputation()
        {
            delete input1_;
            delete input2_;
        }

        Simulation SubComputation::simulate(EncryptionParameters &parms) const
        {
            return simulation_evaluator_.sub(input1_->simulate(parms), input2_->simulate(parms));
        }

        SubComputation *SubComputation::clone() const
        {
            return new SubComputation(*input1_, *input2_);
        }

        MultiplyComputation::MultiplyComputation(const Computation &input1, const Computation &input2)
        {
            input1_ = input1.clone();
            input2_ = input2.clone();
        }

        MultiplyComputation::~MultiplyComputation()
        {
            delete input1_;
            delete input2_;
        }

        Simulation MultiplyComputation::simulate(EncryptionParameters &parms) const
        {
            return simulation_evaluator_.multiply(input1_->simulate(parms), input2_->simulate(parms));
        }

        MultiplyComputation *MultiplyComputation::clone() const
        {
            return new MultiplyComputation(*input1_, *input2_);
        }

        MultiplyNoRelinComputation::MultiplyNoRelinComputation(const Computation &input1, const Computation &input2)
        {
            input1_ = input1.clone();
            input2_ = input2.clone();
        }

        MultiplyNoRelinComputation::~MultiplyNoRelinComputation()
        {
            delete input1_;
            delete input2_;
        }

        Simulation MultiplyNoRelinComputation::simulate(EncryptionParameters &parms) const
        {
            return simulation_evaluator_.multiply_norelin(input1_->simulate(parms), input2_->simulate(parms));
        }

        MultiplyNoRelinComputation *MultiplyNoRelinComputation::clone() const
        {
            return new MultiplyNoRelinComputation(*input1_, *input2_);
        }

        RelinearizeComputation::RelinearizeComputation(const Computation &input)
        {
            input_ = input.clone();
        }

        RelinearizeComputation::~RelinearizeComputation()
        {
            delete input_;
        }

        Simulation RelinearizeComputation::simulate(EncryptionParameters &parms) const
        {
            return simulation_evaluator_.relinearize(input_->simulate(parms));
        }

        RelinearizeComputation *RelinearizeComputation::clone() const
        {
            return new RelinearizeComputation(*input_);
        }

        MultiplyPlainComputation::MultiplyPlainComputation(const Computation &input, int plain_max_coeff_count, const BigUInt &plain_max_abs_value) :
            plain_max_coeff_count_(plain_max_coeff_count), plain_max_abs_value_(plain_max_abs_value)
        {
            if (plain_max_coeff_count <= 0)
            {
                throw out_of_range("plain_max_coeff_count");
            }

            input_ = input.clone();
        }

        MultiplyPlainComputation::MultiplyPlainComputation(const Computation &input, int plain_max_coeff_count, uint64_t plain_max_abs_value) :
            plain_max_coeff_count_(plain_max_coeff_count)
        {
            if (plain_max_coeff_count <= 0)
            {
                throw out_of_range("plain_max_coeff_count");
            }

            BigUInt plain_max_abs_value_uint;
            plain_max_abs_value_uint = plain_max_abs_value;
            plain_max_abs_value_ = plain_max_abs_value_uint;

            input_ = input.clone();
        }

        MultiplyPlainComputation::~MultiplyPlainComputation()
        {
            delete input_;
        }

        Simulation MultiplyPlainComputation::simulate(EncryptionParameters &parms) const
        {
            return simulation_evaluator_.multiply_plain(input_->simulate(parms), plain_max_coeff_count_, plain_max_abs_value_);
        }

        MultiplyPlainComputation *MultiplyPlainComputation::clone() const
        {
            return new MultiplyPlainComputation(*input_, plain_max_coeff_count_, plain_max_abs_value_);
        }

        AddPlainComputation::AddPlainComputation(const Computation &input)
        {
            input_ = input.clone();
        }

        AddPlainComputation::~AddPlainComputation()
        {
            delete input_;
        }

        Simulation AddPlainComputation::simulate(EncryptionParameters &parms) const
        {
            return simulation_evaluator_.add_plain(input_->simulate(parms));
        }

        AddPlainComputation *AddPlainComputation::clone() const
        {
            return new AddPlainComputation(*input_);
        }

        SubPlainComputation::SubPlainComputation(const Computation &input)
        {
            input_ = input.clone();
        }

        SubPlainComputation::~SubPlainComputation()
        {
            delete input_;
        }

        Simulation SubPlainComputation::simulate(EncryptionParameters &parms) const
        {
            return simulation_evaluator_.sub_plain(input_->simulate(parms));
        }

        SubPlainComputation *SubPlainComputation::clone() const
        {
            return new SubPlainComputation(*input_);
        }

        NegateComputation::NegateComputation(const Computation &input)
        {
            input_ = input.clone();
        }

        NegateComputation::~NegateComputation()
        {
            delete input_;
        }

        Simulation NegateComputation::simulate(EncryptionParameters &parms) const
        {
            return simulation_evaluator_.negate(input_->simulate(parms));
        }

        NegateComputation *NegateComputation::clone() const
        {
            return new NegateComputation(*input_);
        }

        BinaryExponentiateComputation::BinaryExponentiateComputation(const Computation &input, int exponent) : exponent_(exponent)
        {
            if (exponent < 0)
            {
                throw out_of_range("exponent");
            }

            input_ = input.clone();
        }

        BinaryExponentiateComputation::~BinaryExponentiateComputation()
        {
            delete input_;
        }

        Simulation BinaryExponentiateComputation::simulate(EncryptionParameters &parms) const
        {
            return simulation_evaluator_.binary_exponentiate(input_->simulate(parms), exponent_);
        }

        BinaryExponentiateComputation *BinaryExponentiateComputation::clone() const
        {
            return new BinaryExponentiateComputation(*input_, exponent_);
        }

        TreeExponentiateComputation::TreeExponentiateComputation(const Computation &input, int exponent) : exponent_(exponent)
        {
            if (exponent < 0)
            {
                throw out_of_range("exponent");
            }

            input_ = input.clone();
        }

        TreeExponentiateComputation::~TreeExponentiateComputation()
        {
            delete input_;
        }

        Simulation TreeExponentiateComputation::simulate(EncryptionParameters &parms) const
        {
            return simulation_evaluator_.tree_exponentiate(input_->simulate(parms), exponent_);
        }

        TreeExponentiateComputation *TreeExponentiateComputation::clone() const
        {
            return new TreeExponentiateComputation(*input_, exponent_);
        }

        TreeMultiplyComputation::TreeMultiplyComputation(const vector<const Computation*> inputs)
        {
            if (inputs.empty())
            {
                throw invalid_argument("inputs");
            }
            int inputs_size = static_cast<int>(inputs_.size());
            for (int i = 0; i < inputs_size; ++i)
            {
                if (inputs[i] == nullptr)
                {
                    throw invalid_argument("inputs");
                }
            }
            for (int i = 0; i < inputs_size; ++i)
            {
                inputs_.push_back(inputs[i]->clone());
            }
        }

        TreeMultiplyComputation::~TreeMultiplyComputation()
        {
            int inputs_size = static_cast<int>(inputs_.size());
            for (int i = 0; i < inputs_size; ++i)
            {
                delete inputs_[i];
            }
        }

        Simulation TreeMultiplyComputation::simulate(EncryptionParameters &parms) const
        {
            vector<Simulation> inputs;
            int inputs_size = static_cast<int>(inputs_.size());
            for (int i = 0; i < inputs_size; ++i)
            {
                inputs.push_back(inputs_[i]->simulate(parms));
            }
            return simulation_evaluator_.tree_multiply(inputs);
        }

        TreeMultiplyComputation *TreeMultiplyComputation::clone() const
        {
            return new TreeMultiplyComputation(inputs_);
        }
    }
}