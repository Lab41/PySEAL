#ifndef SEAL_UTIL_COMPUTATION_H
#define SEAL_UTIL_COMPUTATION_H

#include "encryptionparams.h"
#include "simulator.h"

namespace seal
{
    namespace util
    {
        class Computation
        {
        public:
            virtual Simulation simulate(EncryptionParameters &parms) const = 0;

            virtual Computation *clone() const = 0;

            virtual ~Computation() {}

            SimulationEvaluator simulation_evaluator_;
        };

        class FreshComputation : public Computation
        {
        public:
            FreshComputation();

            ~FreshComputation();

            Simulation simulate(EncryptionParameters &parms) const override;

            FreshComputation *clone() const override;

        private:
            FreshComputation(const FreshComputation &copy) = delete;

            FreshComputation &operator =(const FreshComputation &copy) = delete;
        };

        class AddComputation : public Computation
        {
        public:
            AddComputation(const Computation &input1, const Computation &input2);

            ~AddComputation();

            Simulation simulate(EncryptionParameters &parms) const override;

            AddComputation *clone() const override;

        private:
            AddComputation(const AddComputation &copy) = delete;

            AddComputation &operator =(const AddComputation &copy) = delete;

            Computation *input1_;

            Computation *input2_;
        };

        class SubComputation : public Computation
        {
        public:
            SubComputation(const Computation &input1, const Computation &input2);

            ~SubComputation();

            Simulation simulate(EncryptionParameters &parms) const override;

            SubComputation *clone() const override;

        private:
            SubComputation(const SubComputation &copy) = delete;

            SubComputation &operator =(const SubComputation &copy) = delete;

            Computation *input1_;

            Computation *input2_;
        };

        class MultiplyComputation : public Computation
        {
        public:
            MultiplyComputation(const Computation &input1, const Computation &input2);

            ~MultiplyComputation();

            Simulation simulate(EncryptionParameters &parms) const override;

            MultiplyComputation *clone() const override;

        private:
            MultiplyComputation(const MultiplyComputation &copy) = delete;

            MultiplyComputation &operator =(const MultiplyComputation &copy) = delete;

            Computation *input1_;

            Computation *input2_;
        };

        class MultiplyNoRelinComputation : public Computation
        {
        public:
            MultiplyNoRelinComputation(const Computation &input1, const Computation &input2);

            ~MultiplyNoRelinComputation();

            Simulation simulate(EncryptionParameters &parms) const override;

            MultiplyNoRelinComputation *clone() const override;

        private:
            MultiplyNoRelinComputation(const MultiplyComputation &copy) = delete;

            MultiplyNoRelinComputation &operator =(const MultiplyNoRelinComputation &copy) = delete;

            Computation *input1_;

            Computation *input2_;
        };

        class RelinearizeComputation : public Computation
        {
        public:
            RelinearizeComputation(const Computation &input);

            ~RelinearizeComputation();

            Simulation simulate(EncryptionParameters &parms) const override;

            RelinearizeComputation *clone() const override;

        private:
            RelinearizeComputation(const MultiplyComputation &copy) = delete;

            RelinearizeComputation &operator =(const RelinearizeComputation &copy) = delete;

            Computation *input_;
        };

        class MultiplyPlainComputation : public Computation
        {
        public:
            MultiplyPlainComputation(const Computation &input, int plain_max_coeff_count, const BigUInt &plain_max_abs_value);

            MultiplyPlainComputation(const Computation &input, int plain_max_coeff_count, uint64_t plain_max_abs_value);

            ~MultiplyPlainComputation();

            Simulation simulate(EncryptionParameters &parms) const override;

            MultiplyPlainComputation *clone() const override;

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
            AddPlainComputation(const Computation &input);

            ~AddPlainComputation();

            Simulation simulate(EncryptionParameters &parms) const override;

            AddPlainComputation *clone() const override;

        private:
            AddPlainComputation(const AddPlainComputation &copy) = delete;

            AddPlainComputation &operator =(const AddPlainComputation &copy) = delete;

            Computation *input_;
        };

        class SubPlainComputation : public Computation
        {
        public:
            SubPlainComputation(const Computation &input);

            ~SubPlainComputation();

            Simulation simulate(EncryptionParameters &parms) const override;

            SubPlainComputation *clone() const override;

        private:
            SubPlainComputation(const SubPlainComputation &copy) = delete;

            SubPlainComputation &operator =(const SubPlainComputation &copy) = delete;

            Computation *input_;
        };

        class NegateComputation : public Computation
        {
        public:
            NegateComputation(const Computation &input);

            ~NegateComputation();

            Simulation simulate(EncryptionParameters &parms) const override;

            NegateComputation *clone() const override;

        private:
            NegateComputation(const NegateComputation &copy) = delete;

            NegateComputation &operator =(const NegateComputation &copy) = delete;

            Computation *input_;
        };

        class BinaryExponentiateComputation : public Computation
        {
        public:
            BinaryExponentiateComputation(const Computation &input, int exponent);

            ~BinaryExponentiateComputation();

            Simulation simulate(EncryptionParameters &parms) const override;

            BinaryExponentiateComputation *clone() const override;

        private:
            BinaryExponentiateComputation(const BinaryExponentiateComputation &copy) = delete;

            BinaryExponentiateComputation &operator =(const BinaryExponentiateComputation &copy) = delete;

            Computation *input_;

            int exponent_;
        };

        class TreeExponentiateComputation : public Computation
        {
        public:
            TreeExponentiateComputation(const Computation &input, int exponent);

            ~TreeExponentiateComputation();

            Simulation simulate(EncryptionParameters &parms) const override;

            TreeExponentiateComputation *clone() const override;

        private:
            TreeExponentiateComputation(const TreeExponentiateComputation &copy) = delete;

            TreeExponentiateComputation &operator =(const TreeExponentiateComputation &copy) = delete;

            Computation *input_;

            int exponent_;
        };

        class TreeMultiplyComputation : public Computation
        {
        public:
            TreeMultiplyComputation(const std::vector<const Computation*> inputs);

            ~TreeMultiplyComputation();

            Simulation simulate(EncryptionParameters &parms) const override;

            TreeMultiplyComputation *clone() const override;

        private:
            TreeMultiplyComputation(const TreeMultiplyComputation &copy) = delete;

            TreeMultiplyComputation &operator =(const TreeMultiplyComputation &copy) = delete;

            std::vector<const Computation*> inputs_;
        };
    }
}

#endif // SEAL_UTIL_COMPUTATION_H
