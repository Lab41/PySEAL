#include "util/crt.h"
#include "util/uintcore.h"
#include "util/uintarith.h"
#include "util/uintarithmod.h"

using namespace std;

namespace seal
{
    namespace util
    {
        UIntCRTBuilder::UIntCRTBuilder(const MemoryPoolHandle &pool) : pool_(pool), generated_(false)
        {
        }

        UIntCRTBuilder::UIntCRTBuilder(vector<const uint64_t*> mods, vector<int> mod_uint64_counts, const MemoryPoolHandle &pool) : pool_(pool)
        {
            generate(mods, mod_uint64_counts);
        }

        UIntCRTBuilder::UIntCRTBuilder(const UIntCRTBuilder &copy) : pool_(copy.pool_), generated_(copy.generated_), 
            mod_uint64_counts_(copy.mod_uint64_counts_), mod_product_bit_count_(copy.mod_product_bit_count_),
            mod_product_uint64_count_(copy.mod_product_uint64_count_), mod_count_(copy.mod_count_)
        {
            if (generated_)
            {
                // Resize vectors
                mod_products_array_.resize(mod_count_);
                inv_mod_products_array_.resize(mod_count_);

                // Compute sum of uint64 counts for individual moduli to be able to do allocation for mod_array_anchor_
                int mod_uint64_counts_sum = 0;
                for (int i = 0; i < mod_count_; ++i)
                {
                    mod_uint64_counts_sum += mod_uint64_counts_[i];
                }

                // Allocate space and copy data
                mod_array_alloc_ = allocate_uint(mod_uint64_counts_sum, pool_);
                set_uint_uint(copy.mod_array_alloc_.get(), mod_uint64_counts_sum, mod_array_alloc_.get());

                mod_product_ = allocate_uint(mod_product_uint64_count_, pool_);
                set_uint_uint(copy.mod_product_.get(), mod_product_uint64_count_, mod_product_.get());

                mod_products_array_alloc_ = allocate_uint(mod_product_uint64_count_ * mod_count_, pool_);
                set_uint_uint(copy.mod_products_array_alloc_.get(), mod_product_uint64_count_ * mod_count_, mod_products_array_alloc_.get());

                inv_mod_products_array_alloc_ = allocate_uint(mod_uint64_counts_sum, pool_);
                set_uint_uint(copy.inv_mod_products_array_alloc_.get(), mod_uint64_counts_sum, inv_mod_products_array_alloc_.get());

                // Populate mod_array_
                uint64_t *mod_array_ptr = mod_array_alloc_.get();
                for (int i = 0; i < mod_count_; i++)
                {
                    mod_array_[i] = Modulus(mod_array_ptr, mod_uint64_counts_[i]);
                    mod_array_ptr += mod_uint64_counts_[i];
                }

                // Populate mod_products_array_ and inv_mod_products_array_
                uint64_t *mod_products_array_ptr = mod_products_array_alloc_.get();
                uint64_t *inv_mod_products_array_ptr = inv_mod_products_array_alloc_.get();
                for (int i = 0; i < mod_count_; i++)
                {
                    mod_products_array_[i] = mod_products_array_ptr;
                    mod_products_array_ptr += mod_product_uint64_count_;

                    inv_mod_products_array_[i] = inv_mod_products_array_ptr;
                    inv_mod_products_array_ptr += mod_uint64_counts_[i];
                }
            }
        }

        bool UIntCRTBuilder::generate(vector<const uint64_t*> mods, vector<int> mod_uint64_counts)
        {
#ifdef _DEBUG
            if (mods.size() == 0)
            {
                throw invalid_argument("mods cannot be empty");
            }
            if (mod_uint64_counts.size() == 0)
            {
                throw invalid_argument("mod_uint64_counts cannot be empty");
            }
            if (mod_uint64_counts.size() != mods.size())
            {
                throw invalid_argument("mods and mod_uint64_counts must have the same size");
            }
            for (int i = 0; i < mods.size(); ++i)
            {
                if (mods[i] == nullptr)
                {
                    throw invalid_argument("mods cannot be null");
                }
                if (mod_uint64_counts[i] <= 0)
                {
                    throw invalid_argument("mod_uint64_counts must be positive");
                }
            }
#endif
            reset();
            mod_uint64_counts_ = mod_uint64_counts;
            mod_count_ = mods.size();
            mod_array_.resize(mod_count_);
            mod_products_array_.resize(mod_count_);
            inv_mod_products_array_.resize(mod_count_);

            // Compute sum of uint64 counts for individual moduli to be able to do allocation for mod_array_alloc_
            int mod_uint64_counts_sum = 0;
            for (int i = 0; i < mod_count_; i++)
            {
                mod_uint64_counts_sum += mod_uint64_counts_[i];
            }

            // Compute sum of significant bit counts for individual moduli to be able to do allocation for mod_product_
            mod_product_bit_count_ = 0;
            for (int i = 0; i < mod_count_; i++)
            {
                mod_product_bit_count_ += get_significant_bit_count_uint(mods[i], mod_uint64_counts_[i]);
            }
            mod_product_uint64_count_ = divide_round_up(mod_product_bit_count_, bits_per_uint64);

            // Allocations
            mod_array_alloc_ = allocate_uint(mod_uint64_counts_sum, pool_);
            mod_product_ = allocate_uint(mod_product_uint64_count_, pool_);
            mod_products_array_alloc_ = allocate_uint(mod_product_uint64_count_ * mod_count_, pool_);
            inv_mod_products_array_alloc_ = allocate_uint(mod_uint64_counts_sum, pool_);
            
            // Populate mod_array_
            uint64_t *mod_array_ptr = mod_array_alloc_.get();
            for (int i = 0; i < mod_count_; i++)
            {
                set_uint_uint(mods[i], mod_uint64_counts[i], mod_array_ptr);
                mod_array_[i] = Modulus(mod_array_ptr, mod_uint64_counts_[i]);
                mod_array_ptr += mod_uint64_counts_[i];
            }

            // Compute mod_product_
            Pointer temp(allocate_uint(mod_product_uint64_count_, pool_));
            set_uint_uint(mod_array_[0].get(), mod_uint64_counts_[0], mod_product_uint64_count_, mod_product_.get());
            for (int i = 1; i < mod_count_; i++)
            {
                multiply_uint_uint(mod_product_.get(), mod_product_uint64_count_, mod_array_[i].get(), mod_uint64_counts_[i], mod_product_uint64_count_, temp.get());
                set_uint_uint(temp.get(), mod_product_uint64_count_, mod_product_.get());
            }

            // Compute mod_products_array_alloc_ and mod_products_array_
            Pointer big_alloc(allocate_uint(2 * mod_product_uint64_count_, pool_));
            uint64_t *mod_products_array_ptr = mod_products_array_alloc_.get();
            for (int i = 0; i < mod_count_; i++)
            {
                set_uint_uint(mod_product_.get(), mod_product_uint64_count_, temp.get());
                divide_uint_uint_inplace(temp.get(), mod_array_[i], mod_product_uint64_count_, mod_products_array_ptr, pool_, big_alloc.get());
                mod_products_array_[i] = mod_products_array_ptr;
                mod_products_array_ptr += mod_product_uint64_count_;
            }

            // Compute inv_mod_products_array_alloc_ and inv_mod_products_array_
            uint64_t *inv_mod_products_array_ptr = inv_mod_products_array_alloc_.get();
            for (int i = 0; i < mod_count_; i++)
            {
                set_uint_uint(mod_products_array_[i], mod_product_uint64_count_, temp.get());
                modulo_uint_inplace(temp.get(), mod_product_uint64_count_, mod_array_[i], pool_, big_alloc.get());

                if (!try_invert_uint_mod(temp.get(), mod_array_[i].get(), mod_uint64_counts_[i], inv_mod_products_array_ptr, pool_))
                {
                    reset();
                    return false;
                }

                inv_mod_products_array_[i] = inv_mod_products_array_ptr;
                inv_mod_products_array_ptr += mod_uint64_counts_[i];
            }

            // Everything went well
            generated_ = true;

            return true;
        }

        void UIntCRTBuilder::reset()
        {
            generated_ = false;

            mod_uint64_counts_.clear();
            mod_array_.clear();
            mod_products_array_.clear();
            inv_mod_products_array_.clear();

            mod_product_bit_count_ = 0;
            mod_product_uint64_count_ = 0;
            mod_count_ = 0;

            mod_array_alloc_.release();
            mod_product_.release();
            mod_products_array_alloc_.release();
            inv_mod_products_array_alloc_.release();
        }

        void UIntCRTBuilder::compose(vector<const uint64_t*> inputs, uint64_t *destination) const
        {
#ifdef _DEBUG
            if (!generated_)
            {
                throw logic_error("UIntCRTBuilder instance is not properly generated");
            }
            if (inputs.size() != mod_count_)
            {
                throw invalid_argument("inputs has incorrect size");
            }
            for (int i = 0; i < mod_count_; ++i)
            {
                if (inputs[i] == nullptr)
                {
                    throw invalid_argument("inputs cannot be null");
                }
            }
            if (destination == nullptr)
            {
                throw invalid_argument("destination cannot be null");
            }
#endif
            set_zero_uint(mod_product_uint64_count_, destination);

            Pointer big_alloc(allocate_uint(mod_product_uint64_count_+ 4 * mod_product_uint64_count_, pool_));
            for (int i = 0; i < mod_count_; ++i)
            {
                multiply_uint_uint_mod(inputs[i], inv_mod_products_array_[i], mod_array_[i], big_alloc.get(), pool_, big_alloc.get() + mod_product_uint64_count_);
                multiply_uint_uint(big_alloc.get(), mod_uint64_counts_[i], mod_products_array_[i], mod_product_uint64_count_, mod_product_uint64_count_, big_alloc.get() + mod_product_uint64_count_);
                add_uint_uint_mod(big_alloc.get() + mod_product_uint64_count_, destination, mod_product_.get(), mod_product_uint64_count_, destination);
            }
        }

        void UIntCRTBuilder::decompose(const uint64_t *input, vector<uint64_t*> destination) const
        {
#ifdef _DEBUG
            if (!generated_)
            {
                throw logic_error("UIntCRTBuilder instance is not properly generated");
            }
            if (destination.size() != mod_count_)
            {
                throw invalid_argument("destination has incorrect size");
            }
            for (int i = 0; i < mod_count_; ++i)
            {
                if (destination[i] == nullptr)
                {
                    throw invalid_argument("destination cannot be null");
                }
            }
            if (input == nullptr)
            {
                throw invalid_argument("input cannot be null");
            }
#endif
            Pointer big_alloc(allocate_uint(3 * mod_product_uint64_count_, pool_));
            for (int i = 0; i < mod_count_; ++i)
            {
                modulo_uint(input, mod_product_uint64_count_, mod_array_[i], destination[i], pool_, big_alloc.get());
            }
        }
    }
}