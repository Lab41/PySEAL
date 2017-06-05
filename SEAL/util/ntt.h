#pragma once

#include <stdexcept>
#include "memorypoolhandle.h"
#include "util/modulus.h"
#include "util/mempool.h"

namespace seal
{
    namespace util
    {
        class NTTTables
        {
        public:
            NTTTables(const MemoryPoolHandle &pool);

            NTTTables(int coeff_count_power, const Modulus &modulus, const MemoryPoolHandle &pool);

            NTTTables(const NTTTables &copy);

            NTTTables &operator =(const NTTTables &assign);
            
            NTTTables(NTTTables &&source) = default;

            NTTTables &operator =(NTTTables &&assign) = default;

            inline bool is_generated() const
            {
                return generated_;
            }

            bool generate(int coeff_count_power, const Modulus &modulus);

            void reset();

            inline const std::uint64_t *get_root() const
            {
#ifdef _DEBUG
                if (!generated_)
                {
                    throw std::logic_error("tables are not generated");
                }
#endif
                return root_.get();
            }

            inline const std::uint64_t *get_from_root_powers(int index) const 
            {
#ifdef _DEBUG
                if (index >= coeff_count_)
                {
                    throw std::out_of_range("index");
                }
                if (!generated_)
                {
                    throw std::logic_error("tables are not generated");
                }
#endif
                return root_powers_.get() + index * coeff_uint64_count_;
            }

            inline const std::uint64_t *get_from_scaled_root_powers(int index) const
            {
#ifdef _DEBUG
                if (index >= coeff_count_)
                {
                    throw std::out_of_range("index");
                }
                if (!generated_)
                {
                    throw std::logic_error("tables are not generated");
                }
#endif
                return scaled_root_powers_.get() + index * coeff_uint64_count_;
            }

            inline const std::uint64_t *get_from_inv_root_powers(int index) const
            {
#ifdef _DEBUG
                if (index >= coeff_count_)
                {
                    throw std::out_of_range("index");
                }
                if (!generated_)
                {
                    throw std::logic_error("tables are not generated");
                }
#endif
                return inv_root_powers_.get() + index * coeff_uint64_count_;
            }

            inline const std::uint64_t *get_from_scaled_inv_root_powers(int index) const
            {
#ifdef _DEBUG
                if (index >= coeff_count_)
                {
                    throw std::out_of_range("index");
                }
                if (!generated_)
                {
                    throw std::logic_error("tables are not generated");
                }
#endif
                return scaled_inv_root_powers_.get() + index * coeff_uint64_count_;
            }

            inline const std::uint64_t *get_from_inv_root_powers_div_two(int index) const
            {
#ifdef _DEBUG
                if (index >= coeff_count_)
                {
                    throw std::out_of_range("index");
                }
                if (!generated_)
                {
                    throw std::logic_error("tables are not generated");
                }
#endif
                return inv_root_powers_div_two_.get() + index * coeff_uint64_count_;
            }

            inline const std::uint64_t *get_from_scaled_inv_root_powers_div_two(int index) const {
#ifdef _DEBUG
                if (index >= coeff_count_)
                {
                    throw std::out_of_range("index");
                }
                if (!generated_)
                {
                    throw std::logic_error("tables are not generated");
                }
#endif
                return scaled_inv_root_powers_div_two_.get() + index * coeff_uint64_count_;
            }

            inline const std::uint64_t *get_inv_degree_modulo() const 
            {
#ifdef _DEBUG
                if (!generated_)
                {
                    throw std::logic_error("tables are not generated");
                }
#endif
                return inv_degree_modulo_.get();
            }

            inline const Modulus &modulus() const
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

            inline int coeff_uint64_count() const
            {
                return coeff_uint64_count_;
            }

        private:
            // Computed bit-scrambled vector of first 1 << coeff_count_power powers of a primitive root.
            void ntt_powers_of_primitive_root(std::uint64_t *root, std::uint64_t *destination);

            // Scales the elements of a vector returned by powers_of_primitive_root(...) by word_size/modulus and rounds down.
            void ntt_scale_powers_of_primitive_root(std::uint64_t *input, std::uint64_t *destination);

            MemoryPoolHandle pool_;

            // Size coeff_uint64_count_ * coeff_count_
            Pointer root_powers_;

            // Size coeff_uint64_count_ * coeff_count_
            Pointer scaled_root_powers_;

            // Size coeff_uint64_count_ * coeff_count_
            Pointer inv_root_powers_div_two_;

            // Size coeff_uint64_count_ * coeff_count_
            Pointer scaled_inv_root_powers_div_two_;

            bool generated_;

            int coeff_count_power_;

            int coeff_count_;

            int coeff_uint64_count_;

            // Allocation for modulus, size coeff_uint64_count_
            Pointer modulus_alloc_;

            Modulus modulus_;

            // Size uint64_count_
            Pointer root_;

            // Size coeff_uint64_count_ * coeff_count_
            Pointer inv_root_powers_;

            // Size coeff_uint64_count_ * coeff_count_
            Pointer scaled_inv_root_powers_;

            // Size coeff_uint64_count_
            Pointer inv_degree_modulo_;
        };

        void ntt_negacyclic_harvey(std::uint64_t *operand, const NTTTables &tables, MemoryPool &pool);
        
        void inverse_ntt_negacyclic_harvey(std::uint64_t *operand, const NTTTables &tables, MemoryPool &pool);
    }
}