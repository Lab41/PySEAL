#include "seal/ciphertext.h"

using namespace std;
using namespace seal::util;

namespace seal
{
    Ciphertext &Ciphertext::operator =(const Ciphertext &assign)
    {
        // Check for self-assignment
        if (this == &assign)
        {
            return *this;
        }

        // First copy over hash block
        hash_block_ = assign.hash_block_;

        // Then resize
        resize(assign.size_, assign.poly_coeff_count_, assign.coeff_mod_count_);

        // Size is guaranteed to be OK now so copy over
        // Note: set_uint_uint checks if the value pointers are equal and makes a copy only if they are not
        set_uint_uint(assign.ciphertext_array_.get(), size_ * poly_coeff_count_ * coeff_mod_count_, ciphertext_array_.get());

        return *this;
    }

    void Ciphertext::unalias(const MemoryPoolHandle &pool)
    {
        if (!is_alias())
        {
            throw logic_error("Ciphertext is not an alias");
        }
        if (!pool)
        {
            throw invalid_argument("pool is uninitialized");
        }

        // Create new allocation and copy over value
        int capacity_uint64_count = size_capacity_ * poly_coeff_count_ * coeff_mod_count_;
        int uint64_count = size_ * poly_coeff_count_ * coeff_mod_count_;
        Pointer new_allocation(allocate_uint(capacity_uint64_count, pool));
        set_uint_uint(ciphertext_array_.get(), uint64_count, new_allocation.get());
        ciphertext_array_.acquire(new_allocation);

        // Finally set new pool if necessary
        // Note that this has to be done last to keep pool_ alive
        if (pool_ != pool)
        {
            pool_ = pool;
        }
    }

    void Ciphertext::reserve(int size_capacity, int poly_coeff_count, int coeff_mod_count,
        const MemoryPoolHandle &pool)
    {
        if (is_alias())
        {
            throw logic_error("cannot reserve for aliased Ciphertext");
        }
        if (!pool)
        {
            throw invalid_argument("pool is uninitialized");
        }
        if (size_capacity < 2)
        {
            throw invalid_argument("size_capacity must be at least 2");
        }
#ifdef SEAL_DEBUG
        if (poly_coeff_count < 1)
        {
            throw invalid_argument("poly_coeff_count must be at least 1");
        }
        if (coeff_mod_count < 0)
        {
            throw invalid_argument("coeff_mod_count cannot be negative");
        }
#endif
        int new_uint64_count = size_capacity * poly_coeff_count * coeff_mod_count;
        int new_size = min(size_capacity, size_);
        int copy_uint64_count = new_size * poly_coeff_count_ * coeff_mod_count_;

        // Create new allocation and copy over value
        Pointer new_allocation(allocate_uint(new_uint64_count, pool));
        set_uint_uint(ciphertext_array_.get(), copy_uint64_count, new_allocation.get());
        ciphertext_array_.acquire(new_allocation);

        // Set the size and size_capacity
        size_capacity_ = size_capacity;
        size_ = new_size;
        poly_coeff_count_ = poly_coeff_count;
        coeff_mod_count_ = coeff_mod_count;

        // Finally set new pool if necessary
        // Note that this has to be done last to keep pool_ alive
        if (pool_ != pool)
        {
            pool_ = pool;
        }
    }

    void Ciphertext::save(ostream &stream) const
    {
        stream.write(reinterpret_cast<const char*>(&hash_block_), sizeof(EncryptionParameters::hash_block_type));
        int32_t size32 = static_cast<int32_t>(size_);
        stream.write(reinterpret_cast<const char*>(&size32), sizeof(int32_t));
        int32_t poly_coeff_count32 = static_cast<int32_t>(poly_coeff_count_);
        stream.write(reinterpret_cast<const char*>(&poly_coeff_count32), sizeof(int32_t));
        int32_t coeff_mod_count32 = static_cast<int32_t>(coeff_mod_count_);
        stream.write(reinterpret_cast<const char*>(&coeff_mod_count32), sizeof(int32_t));
        stream.write(reinterpret_cast<const char*>(ciphertext_array_.get()), size_ * poly_coeff_count_ * coeff_mod_count_ * bytes_per_uint64);
    }

    void Ciphertext::python_save(std::string &path) const
    {
        std::ofstream out(path);
        save(out);
        out.close();
    }

    void Ciphertext::load(istream &stream)
    {
        stream.read(reinterpret_cast<char*>(&hash_block_), sizeof(EncryptionParameters::hash_block_type));
        int32_t read_size32 = 0;
        stream.read(reinterpret_cast<char*>(&read_size32), sizeof(int32_t));
        int32_t read_poly_coeff_count32 = 0;
        stream.read(reinterpret_cast<char*>(&read_poly_coeff_count32), sizeof(int32_t));
        int32_t read_coeff_mod_count32 = 0;
        stream.read(reinterpret_cast<char*>(&read_coeff_mod_count32), sizeof(int32_t));

        // Resize
        resize(read_size32, read_poly_coeff_count32, read_coeff_mod_count32);

        // Read data
        stream.read(reinterpret_cast<char*>(ciphertext_array_.get()), size_ * poly_coeff_count_ * coeff_mod_count_ * bytes_per_uint64);
    }

    void Ciphertext::python_load(std::string &path)
    {
        std::ifstream in(path);
        load(in);
        in.close();
    }

    void Ciphertext::resize(int size, int poly_coeff_count, int coeff_mod_count, const MemoryPoolHandle &pool)
    {
        if (size < 2)
        {
            throw invalid_argument("size must be at least 2");
        }
#ifdef SEAL_DEBUG
        if (poly_coeff_count < 0)
        {
            throw invalid_argument("poly_coeff_count cannot be negative");
        }
        if (coeff_mod_count < 0)
        {
            throw invalid_argument("coeff_mod_count cannot be negative");
        }
#endif
        if (!pool && !is_alias())
        {
            throw invalid_argument("pool is uninitialized");
        }
        if (is_alias() && pool != pool_)
        {
            throw invalid_argument("cannot resize aliased Ciphertext to different memory pool");
        }

        int old_capacity_uint64_count = size_capacity_ * poly_coeff_count_ * coeff_mod_count_;
        int old_uint64_count = size_ * poly_coeff_count_ * coeff_mod_count_;
        int new_uint64_count = size * poly_coeff_count * coeff_mod_count;

        if (is_alias() && new_uint64_count > old_capacity_uint64_count)
        {
            throw logic_error("cannot resize aliased Ciphertext");
        }

        // If is_alias() we will always hit this
        if (new_uint64_count <= old_capacity_uint64_count)
        {
            // Warning: We are not setting new space to zero!

            // Set the size parameters
            size_ = size;
            poly_coeff_count_ = poly_coeff_count;
            coeff_mod_count_ = coeff_mod_count;

            // Are the pools different? Never if is_alias()
            if (pool != pool_)
            {
                Pointer new_allocation(allocate_uint(old_capacity_uint64_count, pool));
                set_uint_uint(ciphertext_array_.get(), new_uint64_count, new_allocation.get());
                ciphertext_array_.acquire(new_allocation);

                // Finally set new pool
                // Note that this has to be done last to keep pool_ alive
                pool_ = pool;
            }
            return;
        }

        // At this point we are guaranteed to not be alias and we know for sure
        // that old_uint64_count <= new_uint64_count, also pool != pool so need 
        // to resize to bigger size
        Pointer new_allocation(allocate_uint(new_uint64_count, pool));
        set_uint_uint(ciphertext_array_.get(), old_uint64_count, new_allocation.get());
        ciphertext_array_.acquire(new_allocation);

        // Set the size parameters
        size_capacity_ = size;
        size_ = size;
        poly_coeff_count_ = poly_coeff_count;
        coeff_mod_count_ = coeff_mod_count;

        // Finally set new pool if needed
        // Note that this has to be done last to keep pool_ alive
        if (pool_ != pool)
        {
            pool_ = pool;
        }
    }
}
