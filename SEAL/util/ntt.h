#pragma once

#include "util/modulus.h"
#include "util/mempool.h"

namespace seal
{
    namespace util
    {
        class NTTTables
        {
        public:
            NTTTables();

            NTTTables(int coeff_count_power, const Modulus &modulus);

            ~NTTTables();

            inline bool is_generated() const
            {
                return generated_;
            }

            bool generate(int coeff_count_power, const Modulus &modulus);

            void reset();

            const std::uint64_t *get_root() const;

            const std::uint64_t *get_from_root_powers(int index) const;

            const std::uint64_t *get_from_scaled_root_powers(int index) const;

            const std::uint64_t *get_from_inv_root_powers(int index) const;

            const std::uint64_t *get_from_scaled_inv_root_powers(int index) const;

            const std::uint64_t *get_from_inv_root_powers_div_two(int index) const;

            const std::uint64_t *get_from_scaled_inv_root_powers_div_two(int index) const;

            const std::uint64_t *get_inv_degree_modulo() const;

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
            NTTTables(const NTTTables &copy) = delete;

            NTTTables &operator =(const NTTTables &assign) = delete;

            // Computed bit-scrambled vector of first 1 << coeff_count_power powers of a primitive root.
            void ntt_powers_of_primitive_root(std::uint64_t *root, MemoryPool &pool, std::uint64_t *destination);

            // Scales the elements of a vector returned by powers_of_primitive_root(...) by word_size/modulus and rounds down.
            void ntt_scale_powers_of_primitive_root(std::uint64_t *input, MemoryPool &pool, std::uint64_t *destination);

            bool generated_;

            int coeff_count_power_;

            int coeff_count_;

            int coeff_uint64_count_;

            // Size coeff_uint64_count_
            Modulus modulus_;

            // Size uint64_count_
            Pointer root_;

            // Size coeff_uint64_count_ * coeff_count_
            Pointer root_powers_;

            // Size coeff_uint64_count_ * coeff_count_
            Pointer scaled_root_powers_;

            // Size coeff_uint64_count_ * coeff_count_
            Pointer inv_root_powers_;

            // Size coeff_uint64_count_ * coeff_count_
            Pointer scaled_inv_root_powers_;

            // Size coeff_uint64_count_ * coeff_count_
            Pointer inv_root_powers_div_two_; 

            // Size coeff_uint64_count_ * coeff_count_
            Pointer scaled_inv_root_powers_div_two_;

            // Size coeff_uint64_count_
            Pointer inv_degree_modulo_;
        };

        void ntt_negacyclic_harvey(std::uint64_t *operand, const NTTTables &tables, MemoryPool &pool);
        
        void inverse_ntt_negacyclic_harvey(std::uint64_t *operand, const NTTTables &tables, MemoryPool &pool);
    }
}