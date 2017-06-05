#include <stdexcept>
#include "util/computation.h"

using namespace seal;
using namespace std;

namespace seal
{
    namespace util
    {
        FreshComputation::FreshComputation(int plain_max_coeff_count, const BigUInt &plain_max_abs_value) :
            plain_max_coeff_count_(plain_max_coeff_count), plain_max_abs_value_(plain_max_abs_value)
        {
#ifdef _DEBUG
            if (plain_max_coeff_count <= 0)
            {
                throw invalid_argument("plain_max_coeff_count");
            }
#endif
        }

        FreshComputation::~FreshComputation()
        {
        }

        Simulation FreshComputation::simulate(EncryptionParameters &parms)
        {
            return simulation_evaluator_.get_fresh(parms, plain_max_coeff_count_, plain_max_abs_value_);
        }

        FreshComputation *FreshComputation::clone()
        {
            return new FreshComputation(plain_max_coeff_count_, plain_max_abs_value_);
        }

        AddComputation::AddComputation(Computation &input1, Computation &input2)
        {
            input1_ = input1.clone();
            input2_ = input2.clone();
        }

        AddComputation::~AddComputation()
        {
            delete input1_;
            delete input2_;
        }

        Simulation AddComputation::simulate(EncryptionParameters &parms)
        {
            return simulation_evaluator_.add(input1_->simulate(parms), input2_->simulate(parms));
        }

        AddComputation *AddComputation::clone()
        {
            return new AddComputation(*input1_, *input2_);
        }

        AddManyComputation::AddManyComputation(vector<Computation*> inputs)
        {
#ifdef _DEBUG
            if (inputs.empty())
            {
                throw invalid_argument("inputs can not be empty");
            }
#endif
            for (size_t i = 0; i < inputs.size(); ++i)
            {
#ifdef _DEBUG
                if (inputs[i] == nullptr)
                {
                    throw invalid_argument("inputs can not contain null pointers");
                }
#endif
                inputs_.emplace_back(inputs[i]->clone());
            }
        }

        AddManyComputation::~AddManyComputation()
        {
            for (size_t i = 0; i < inputs_.size(); ++i)
            {
                delete inputs_[i];
            }
        }

        Simulation AddManyComputation::simulate(EncryptionParameters &parms)
        {
            vector<Simulation> inputs;
            for (size_t i = 0; i < inputs_.size(); ++i)
            {
                inputs.emplace_back(inputs_[i]->simulate(parms));
            }
            return simulation_evaluator_.add_many(inputs);
        }

        AddManyComputation *AddManyComputation::clone()
        {
            return new AddManyComputation(inputs_);
        }

        SubComputation::SubComputation(Computation &input1, Computation &input2)
        {
            input1_ = input1.clone();
            input2_ = input2.clone();
        }

        SubComputation::~SubComputation()
        {
            delete input1_;
            delete input2_;
        }

        Simulation SubComputation::simulate(EncryptionParameters &parms)
        {
            return simulation_evaluator_.sub(input1_->simulate(parms), input2_->simulate(parms));
        }

        SubComputation *SubComputation::clone()
        {
            return new SubComputation(*input1_, *input2_);
        }

        MultiplyComputation::MultiplyComputation(Computation &input1, Computation &input2)
        {
            input1_ = input1.clone();
            input2_ = input2.clone();
        }

        MultiplyComputation::~MultiplyComputation()
        {
            delete input1_;
            delete input2_;
        }

        Simulation MultiplyComputation::simulate(EncryptionParameters &parms)
        {
            return simulation_evaluator_.multiply(input1_->simulate(parms), input2_->simulate(parms));
        }

        MultiplyComputation *MultiplyComputation::clone()
        {
            return new MultiplyComputation(*input1_, *input2_);
        }

        RelinearizeComputation::RelinearizeComputation(Computation &input, int destination_size)
        {
            input_ = input.clone();
            destination_size_ = destination_size;
        }

        RelinearizeComputation::~RelinearizeComputation()
        {
            delete input_;
        }

        Simulation RelinearizeComputation::simulate(EncryptionParameters &parms)
        {
            return simulation_evaluator_.relinearize(input_->simulate(parms), destination_size_);
        }

        RelinearizeComputation *RelinearizeComputation::clone()
        {
            return new RelinearizeComputation(*input_, destination_size_);
        }

        MultiplyPlainComputation::MultiplyPlainComputation(Computation &input, int plain_max_coeff_count, const BigUInt &plain_max_abs_value) :
            plain_max_coeff_count_(plain_max_coeff_count), plain_max_abs_value_(plain_max_abs_value)
        {
#ifdef _DEBUG
            if (plain_max_coeff_count <= 0)
            {
                throw invalid_argument("plain_max_coeff_count");
            }
#endif
            input_ = input.clone();
        }

        MultiplyPlainComputation::~MultiplyPlainComputation()
        {
            delete input_;
        }

        Simulation MultiplyPlainComputation::simulate(EncryptionParameters &parms)
        {
            return simulation_evaluator_.multiply_plain(input_->simulate(parms), plain_max_coeff_count_, plain_max_abs_value_);
        }

        MultiplyPlainComputation *MultiplyPlainComputation::clone()
        {
            return new MultiplyPlainComputation(*input_, plain_max_coeff_count_, plain_max_abs_value_);
        }

        AddPlainComputation::AddPlainComputation(Computation &input, int plain_max_coeff_count, const BigUInt &plain_max_abs_value) :
            plain_max_coeff_count_(plain_max_coeff_count), plain_max_abs_value_(plain_max_abs_value)
        {
#ifdef _DEBUG
            if (plain_max_coeff_count <= 0)
            {
                throw invalid_argument("plain_max_coeff_count");
            }
#endif
            input_ = input.clone();
        }

        AddPlainComputation::~AddPlainComputation()
        {
            delete input_;
        }

        Simulation AddPlainComputation::simulate(EncryptionParameters &parms)
        {
            return simulation_evaluator_.add_plain(input_->simulate(parms), plain_max_coeff_count_, plain_max_abs_value_);
        }

        AddPlainComputation *AddPlainComputation::clone()
        {
            return new AddPlainComputation(*input_, plain_max_coeff_count_, plain_max_abs_value_);
        }

        SubPlainComputation::SubPlainComputation(Computation &input, int plain_max_coeff_count, const BigUInt &plain_max_abs_value) :
            plain_max_coeff_count_(plain_max_coeff_count), plain_max_abs_value_(plain_max_abs_value)
        {
#ifdef _DEBUG
            if (plain_max_coeff_count <= 0)
            {
                throw invalid_argument("plain_max_coeff_count");
            }
#endif
            input_ = input.clone();
        }

        SubPlainComputation::~SubPlainComputation()
        {
            delete input_;
        }

        Simulation SubPlainComputation::simulate(EncryptionParameters &parms)
        {
            return simulation_evaluator_.sub_plain(input_->simulate(parms), plain_max_coeff_count_, plain_max_abs_value_);
        }

        SubPlainComputation *SubPlainComputation::clone()
        {
            return new SubPlainComputation(*input_, plain_max_coeff_count_, plain_max_abs_value_);
        }

        NegateComputation::NegateComputation(Computation &input)
        {
            input_ = input.clone();
        }

        NegateComputation::~NegateComputation()
        {
            delete input_;
        }

        Simulation NegateComputation::simulate(EncryptionParameters &parms)
        {
            return simulation_evaluator_.negate(input_->simulate(parms));
        }

        NegateComputation *NegateComputation::clone()
        {
            return new NegateComputation(*input_);
        }

        ExponentiateComputation::ExponentiateComputation(Computation &input, uint64_t exponent) : exponent_(exponent)
        {
            input_ = input.clone();
        }

        ExponentiateComputation::~ExponentiateComputation()
        {
            delete input_;
        }

        Simulation ExponentiateComputation::simulate(EncryptionParameters &parms)
        {
            return simulation_evaluator_.exponentiate(input_->simulate(parms), exponent_);
        }

        ExponentiateComputation *ExponentiateComputation::clone()
        {
            return new ExponentiateComputation(*input_, exponent_);
        }

        MultiplyManyComputation::MultiplyManyComputation(vector<Computation*> inputs)
        {
#ifdef _DEBUG
            if (inputs.empty())
            {
                throw invalid_argument("inputs can not be empty");
            }
#endif
            for (size_t i = 0; i < inputs.size(); ++i)
            {
#ifdef _DEBUG
                if (inputs[i] == nullptr)
                {
                    throw invalid_argument("inputs can not contain null pointers");
                }
#endif
                inputs_.emplace_back(inputs[i]->clone());
            }
        }

        MultiplyManyComputation::~MultiplyManyComputation()
        {
            for (size_t i = 0; i < inputs_.size(); ++i)
            {
                delete inputs_[i];
            }
        }

        Simulation MultiplyManyComputation::simulate(EncryptionParameters &parms)
        {
            vector<Simulation> inputs;
            for (size_t i = 0; i < inputs_.size(); ++i)
            {
                inputs.emplace_back(inputs_[i]->simulate(parms));
            }
            return simulation_evaluator_.multiply_many(inputs);
        }

        MultiplyManyComputation *MultiplyManyComputation::clone()
        {
            return new MultiplyManyComputation(inputs_);
        }
    }
}