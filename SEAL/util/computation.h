#pragma once

#include "encryptionparams.h"
#include "biguint.h"
#include "simulator.h"

namespace seal
{
    namespace util
    {
        class Computation
        {
        public:
            virtual Simulation simulate(EncryptionParameters &parms) = 0;

            virtual Computation *clone() = 0;

            virtual ~Computation() {}

            SimulationEvaluator simulation_evaluator_;
        };

        class FreshComputation : public Computation
        {
        public:
            FreshComputation(int max_coeff_count, const BigUInt &plain_max_abs_value);

            ~FreshComputation();

            Simulation simulate(EncryptionParameters &parms) override;

            FreshComputation *clone() override;

        private:
            FreshComputation(const FreshComputation &copy) = delete;

            FreshComputation &operator =(const FreshComputation &copy) = delete;

            int plain_max_coeff_count_;

            BigUInt plain_max_abs_value_;
        };

        class AddComputation : public Computation
        {
        public:
            AddComputation(Computation &input1, Computation &input2);

            ~AddComputation();

            Simulation simulate(EncryptionParameters &parms) override;

            AddComputation *clone() override;

        private:
            AddComputation(const AddComputation &copy) = delete;

            AddComputation &operator =(const AddComputation &copy) = delete;

            Computation *input1_;

            Computation *input2_;
        };

        class AddManyComputation : public Computation
        {
        public:
            AddManyComputation(std::vector<Computation*> inputs);

            ~AddManyComputation();

            Simulation simulate(EncryptionParameters &parms) override;

            AddManyComputation *clone() override;

        private:
            AddManyComputation(const AddManyComputation &copy) = delete;

            AddManyComputation &operator =(const AddManyComputation &copy) = delete;

            std::vector<Computation*> inputs_;
        };

        class SubComputation : public Computation
        {
        public:
            SubComputation(Computation &input1, Computation &input2);

            ~SubComputation();

            Simulation simulate(EncryptionParameters &parms) override;

            SubComputation *clone() override;

        private:
            SubComputation(const SubComputation &copy) = delete;

            SubComputation &operator =(const SubComputation &copy) = delete;

            Computation *input1_;

            Computation *input2_;
        };

        class MultiplyComputation : public Computation
        {
        public:
            MultiplyComputation(Computation &input1, Computation &input2);

            ~MultiplyComputation();

            Simulation simulate(EncryptionParameters &parms) override;

            MultiplyComputation *clone() override;

        private:
            MultiplyComputation(const MultiplyComputation &copy) = delete;

            MultiplyComputation &operator =(const MultiplyComputation &copy) = delete;

            Computation *input1_;

            Computation *input2_;
        };
      
        class RelinearizeComputation : public Computation
        {
        public:
            RelinearizeComputation(Computation &input, int destination_size);

            ~RelinearizeComputation();

            Simulation simulate(EncryptionParameters &parms) override;

            RelinearizeComputation *clone() override;

        private:
            RelinearizeComputation(const MultiplyComputation &copy) = delete;

            RelinearizeComputation &operator =(const RelinearizeComputation &copy) = delete;

            Computation *input_;

            int destination_size_;
        };
        

        class MultiplyPlainComputation : public Computation
        {
        public:
            MultiplyPlainComputation(Computation &input, int plain_max_coeff_count, const BigUInt &plain_max_abs_value);

            ~MultiplyPlainComputation();

            Simulation simulate(EncryptionParameters &parms) override;

            MultiplyPlainComputation *clone() override;

        private:
            MultiplyPlainComputation(const MultiplyPlainComputation &copy) = delete;

            MultiplyPlainComputation &operator =(const MultiplyPlainComputation &copy) = delete;

            Computation *input_;

            int plain_max_coeff_count_;

            BigUInt plain_max_abs_value_;
        };

        class AddPlainComputation : public Computation
        {
        public:
            AddPlainComputation(Computation &input, int plain_max_coeff_count, const BigUInt &plain_max_abs_value);

            ~AddPlainComputation();

            Simulation simulate(EncryptionParameters &parms) override;

            AddPlainComputation *clone() override;

        private:
            AddPlainComputation(const AddPlainComputation &copy) = delete;

            AddPlainComputation &operator =(const AddPlainComputation &copy) = delete;

            Computation *input_;

            int plain_max_coeff_count_;

            BigUInt plain_max_abs_value_;
        };

        class SubPlainComputation : public Computation
        {
        public:
            SubPlainComputation(Computation &input, int plain_max_coeff_count, const BigUInt &plain_max_abs_value);

            ~SubPlainComputation();

            Simulation simulate(EncryptionParameters &parms) override;

            SubPlainComputation *clone() override;

        private:
            SubPlainComputation(const SubPlainComputation &copy) = delete;

            SubPlainComputation &operator =(const SubPlainComputation &copy) = delete;

            Computation *input_;

            int plain_max_coeff_count_;

            BigUInt plain_max_abs_value_;
        };

        class NegateComputation : public Computation
        {
        public:
            NegateComputation(Computation &input);

            ~NegateComputation();

            Simulation simulate(EncryptionParameters &parms) override;

            NegateComputation *clone() override;

        private:
            NegateComputation(const NegateComputation &copy) = delete;

            NegateComputation &operator =(const NegateComputation &copy) = delete;

            Computation *input_;
        };

        class ExponentiateComputation : public Computation
        {
        public:
            ExponentiateComputation(Computation &input, std::uint64_t exponent);

            ~ExponentiateComputation();

            Simulation simulate(EncryptionParameters &parms) override;

            ExponentiateComputation *clone() override;

        private:
            ExponentiateComputation(const ExponentiateComputation &copy) = delete;

            ExponentiateComputation &operator =(const ExponentiateComputation &copy) = delete;

            Computation *input_;

            std::uint64_t exponent_;
        };

        class MultiplyManyComputation : public Computation
        {
        public:
            MultiplyManyComputation(std::vector<Computation*> inputs);

            ~MultiplyManyComputation();

            Simulation simulate(EncryptionParameters &parms) override;

            MultiplyManyComputation *clone() override;

        private:
            MultiplyManyComputation(const MultiplyManyComputation &copy) = delete;

            MultiplyManyComputation &operator =(const MultiplyManyComputation &copy) = delete;

            std::vector<Computation*> inputs_;
        };
    }
}