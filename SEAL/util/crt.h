#pragma once

#include <vector>
#include "memorypoolhandle.h"
#include "util/mempool.h"
#include "util/modulus.h"

namespace seal
{
    namespace util
    {
        class UIntCRTBuilder
        {
        public:
            UIntCRTBuilder(const MemoryPoolHandle &pool = MemoryPoolHandle::acquire_global());

            UIntCRTBuilder(std::vector<const std::uint64_t*> mods, std::vector<int> mod_uint64_counts, const MemoryPoolHandle &pool = MemoryPoolHandle::acquire_global());

            UIntCRTBuilder(const UIntCRTBuilder &copy);

            UIntCRTBuilder(UIntCRTBuilder &&source) = default;

            void compose(std::vector<const std::uint64_t*> inputs, std::uint64_t *destination) const;

            void decompose(const std::uint64_t *input, std::vector<std::uint64_t*> destination) const;

            bool generate(std::vector<const std::uint64_t*> mods, std::vector<int> mod_uint64_counts);

            inline bool is_generated() const
            {
                return generated_;
            }

            void reset();

            inline const std::uint64_t *get_from_mod_array(int index) const
            {
                return mod_array_[index].get();
            }

            inline const std::uint64_t *mod_product() const
            {
                return mod_product_.get();
            }

            inline std::vector<int> mod_uint64_counts() const
            {
                return mod_uint64_counts_;
            }

            inline int mod_uint64_counts(int index) const
            {
                return mod_uint64_counts_[index];
            }

            inline int mod_product_bit_count() const
            {
                return mod_product_bit_count_;
            }

            inline int mod_product_uint64_count() const
            {
                return mod_product_uint64_count_;
            }

            inline int mod_count() const
            {
                return mod_count_;
            }

        private:
            UIntCRTBuilder &operator =(const UIntCRTBuilder &assign) = delete;

            UIntCRTBuilder &operator =(UIntCRTBuilder &&assign) = delete;

            MemoryPoolHandle pool_;

            bool generated_;

            std::vector<int> mod_uint64_counts_;

            int mod_product_bit_count_;

            int mod_product_uint64_count_;

            int mod_count_;

            // Moduli m_1, m_2, ..., m_k where k = mod_count_
            Pointer mod_array_alloc_;

            std::vector<Modulus> mod_array_;

            // Product m_1*m_2*...*m_k
            Pointer mod_product_;

            // Contains an array of terms of the form mod_product_ / mod_array_[i] (all sizes mod_product_uint64_count_)
            Pointer mod_products_array_alloc_;

            // Pointers to individual modulus locations in mod_products_array_alloc_
            std::vector<std::uint64_t*> mod_products_array_;

            // Contains an array of terms of the form (mod_product_ / mod_array_[i])^(-1) mod mod_array_[i]
            Pointer inv_mod_products_array_alloc_;

            // Pointers to individual modulus locations in inv_mod_products_array_alloc_
            std::vector<std::uint64_t*> inv_mod_products_array_;
        };
    }
}