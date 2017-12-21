#pragma once

#include <stdexcept>
#include "seal/memorypoolhandle.h"
#include "seal/smallmodulus.h"

namespace seal
{
    namespace util
    {
        class SmallNTTTables
        {
        public:
            SmallNTTTables(const MemoryPoolHandle &pool);

            SmallNTTTables(int coeff_count_power, const SmallModulus &modulus, const MemoryPoolHandle &pool);

            SmallNTTTables(const SmallNTTTables &copy);

            SmallNTTTables &operator =(const SmallNTTTables &assign);

            SmallNTTTables(SmallNTTTables &&source) = default;

            SmallNTTTables &operator =(SmallNTTTables &&assign) = default;

            inline bool is_generated() const
            {
                return generated_;
            }

            bool generate(int coeff_count_power, const SmallModulus &modulus);

            void reset();

            inline std::uint64_t get_root() const
            {
#ifdef SEAL_DEBUG
                if (!generated_)
                {
                    throw std::logic_error("tables are not generated");
                }
#endif
                return root_;
            }

            inline std::uint64_t get_from_root_powers(int index) const
            {
#ifdef SEAL_DEBUG
                if (index >= coeff_count_)
                {
                    throw std::out_of_range("index");
                }
                if (!generated_)
                {
                    throw std::logic_error("tables are not generated");
                }
#endif
                return root_powers_[index];
            }

            inline std::uint64_t get_from_scaled_root_powers(int index) const
            {
#ifdef SEAL_DEBUG
                if (index >= coeff_count_)
                {
                    throw std::out_of_range("index");
                }
                if (!generated_)
                {
                    throw std::logic_error("tables are not generated");
                }
#endif
                return scaled_root_powers_[index];
            }

            inline std::uint64_t get_from_inv_root_powers(int index) const
            {
#ifdef SEAL_DEBUG
                if (index >= coeff_count_)
                {
                    throw std::out_of_range("index");
                }
                if (!generated_)
                {
                    throw std::logic_error("tables are not generated");
                }
#endif
                return inv_root_powers_[index];
            }

            inline std::uint64_t get_from_scaled_inv_root_powers(int index) const
            {
#ifdef SEAL_DEBUG
                if (index >= coeff_count_)
                {
                    throw std::out_of_range("index");
                }
                if (!generated_)
                {
                    throw std::logic_error("tables are not generated");
                }
#endif
                return scaled_inv_root_powers_[index];
            }

            inline std::uint64_t get_from_inv_root_powers_div_two(int index) const
            {
#ifdef SEAL_DEBUG
                if (index >= coeff_count_)
                {
                    throw std::out_of_range("index");
                }
                if (!generated_)
                {
                    throw std::logic_error("tables are not generated");
                }
#endif
                return inv_root_powers_div_two_[index];
            }

            inline std::uint64_t get_from_scaled_inv_root_powers_div_two(int index) const 
            {
#ifdef SEAL_DEBUG
                if (index >= coeff_count_)
                {
                    throw std::out_of_range("index");
                }
                if (!generated_)
                {
                    throw std::logic_error("tables are not generated");
                }
#endif
                return scaled_inv_root_powers_div_two_[index];
            }

            inline const std::uint64_t *get_inv_degree_modulo() const
            {
#ifdef SEAL_DEBUG
                if (!generated_)
                {
                    throw std::logic_error("tables are not generated");
                }
#endif
                return &inv_degree_modulo_;
            }

            inline const SmallModulus &modulus() const
            {
                return modulus_;
            }

            inline int coeff_count_power() const
            {
                return coeff_count_power_;
            }

            inline int coeff_count() const
            {
                return coeff_count_;
            }

        private:
            // Computed bit-scrambled vector of first 1 << coeff_count_power powers of a primitive root.
            void ntt_powers_of_primitive_root(std::uint64_t root, std::uint64_t *destination) const;

            // Scales the elements of a vector returned by powers_of_primitive_root(...) by word_size/modulus and rounds down.
            void ntt_scale_powers_of_primitive_root(const std::uint64_t *input, std::uint64_t *destination) const;

            MemoryPoolHandle pool_;

            // Size coeff_count_
            Pointer root_powers_;

            // Size coeff_count_
            Pointer scaled_root_powers_;

            // Size coeff_count_
            Pointer inv_root_powers_div_two_;

            // Size coeff_count_
            Pointer scaled_inv_root_powers_div_two_;

            bool generated_ = false;

            int coeff_count_power_ = 0;

            int coeff_count_ = 0;

            SmallModulus modulus_;

            std::uint64_t root_ = 0;

            // Size coeff_count_
            Pointer inv_root_powers_;

            // Size coeff_count_
            Pointer scaled_inv_root_powers_;

            std::uint64_t inv_degree_modulo_ = 0;

        };

        void ntt_negacyclic_harvey_lazy(std::uint64_t *operand, const SmallNTTTables &tables);

        inline void ntt_negacyclic_harvey(std::uint64_t *operand, const SmallNTTTables &tables)
        {
            ntt_negacyclic_harvey_lazy(operand, tables);

            // Finally maybe we need to reduce every coefficient modulo q, but we know that they are in the range [0, 4q). 
            // Since word size is controlled this is fast.
            int n = 1 << tables.coeff_count_power();
            std::uint64_t modulus = tables.modulus().value();
            std::uint64_t two_times_modulus = modulus * 2;

            for (int i = 0; i < n; i++)
            {
                if (*operand >= two_times_modulus)
                {
                    *operand -= two_times_modulus;
                }
                if (*operand >= modulus)
                {
                    *operand -= modulus;
                }
                operand++;
            }
        }

        void inverse_ntt_negacyclic_harvey_lazy(std::uint64_t *operand, const SmallNTTTables &tables);

        inline void inverse_ntt_negacyclic_harvey(std::uint64_t *operand, const SmallNTTTables &tables)
        {
            inverse_ntt_negacyclic_harvey_lazy(operand, tables);

            std::uint64_t modulus = tables.modulus().value();
            int n = 1 << tables.coeff_count_power();

            // Final adjustments; compute a[j] = a[j] * n^{-1} mod q. 
            // We incorporated the final adjustment in the butterfly. Only need to reduce here.
            for (int i = 0; i < n; i++)
            {
                if (*operand >= modulus)
                {
                    *operand -= modulus;
                }
                operand++;
            }
        }
    }
}
