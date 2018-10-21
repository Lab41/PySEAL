#include "seal/plaintext.h"
#include "seal/util/common.h"
#include "seal/util/uintcore.h"
#include "seal/util/uintarith.h"
#include <stdexcept>
#include <algorithm>

using namespace std;
using namespace seal::util;

namespace seal
{
    namespace
    {
        bool is_dec_char(char c)
        {
            return c >= '0' && c <= '9';
        }

        int get_dec_value(char c)
        {
            return c - '0';
        }

        int get_coeff_length(const char *poly)
        {
            int length = 0;
            while (is_hex_char(*poly))
            {
                length++;
                poly++;
            }
            return length;
        }

        int get_coeff_power(const char *poly, int *power_length)
        {
            int length = 0;
            if (*poly == '\0')
            {
                *power_length = 0;
                return 0;
            }
            if (*poly != 'x')
            {
                return -1;
            }
            poly++;
            length++;

            if (*poly != '^')
            {
                return -1;
            }
            poly++;
            length++;

            int power = 0;
            while (is_dec_char(*poly))
            {
                power *= 10;
                power += get_dec_value(*poly);
                poly++;
                length++;
            }
            *power_length = length;
            return power;
        }

        int get_plus(const char *poly)
        {
            if (*poly == '\0')
            {
                return 0;
            }
            if (*poly++ != ' ')
            {
                return -1;
            }
            if (*poly++ != '+')
            {
                return -1;
            }
            if (*poly != ' ')
            {
                return -1;
            }
            return 3;
        }
    }

    void Plaintext::resize(int coeff_count, const MemoryPoolHandle &pool)
    {
        if (coeff_count < 0)
        {
            throw invalid_argument("coeff_count cannot be negative");
        }
        if (!pool && !is_alias())
        {
            throw invalid_argument("pool is uninitialized");
        }
        if (is_alias() && pool != pool_)
        {
            throw invalid_argument("cannot resize aliased Plaintext to different memory pool");
        }
        if (is_alias() && coeff_count > capacity_)
        {
            throw logic_error("cannot resize aliased Plaintext");
        }

        // If is_alias() we will always hit this
        if (coeff_count <= capacity_)
        {
            // Are we changing size to bigger within current capacity?
            // If so, need to set top coefficients to zero
            if (coeff_count > coeff_count_)
            {
                set_zero_uint(coeff_count - coeff_count_, plaintext_poly_.get() + coeff_count_);
            }

            // Set the coeff_count
            coeff_count_ = coeff_count;

            // Are the pools different? Never if is_alias()
            if (pool != pool_)
            {
                Pointer new_allocation(allocate_uint(capacity_, pool));
                set_uint_uint(plaintext_poly_.get(), coeff_count_, new_allocation.get());
                plaintext_poly_.acquire(new_allocation);

                // Finally set new pool
                // Note that this has to be done last to keep pool_ alive
                pool_ = pool;
            }
            return;
        }

        // At this point we are guaranteed to not be alias and we know for sure
        // that capacity_ < coeff_count so need to reallocate to bigger
        Pointer new_allocation(allocate_uint(coeff_count, pool));
        set_uint_uint(plaintext_poly_.get(), coeff_count_, new_allocation.get());
        set_zero_uint(coeff_count - coeff_count_, new_allocation.get() + coeff_count_);
        plaintext_poly_.acquire(new_allocation);

        // Set the coeff_count and capacity
        capacity_ = coeff_count;
        coeff_count_ = coeff_count;

        // Finally set new pool if needed
        // Note that this has to be done last to keep pool_ alive
        if (pool_ != pool)
        {
            pool_ = pool;
        }
    }

    void Plaintext::unalias(const MemoryPoolHandle &pool)
    {
        if (!is_alias())
        {
            throw logic_error("Plaintext is not an alias");
        }
        if (!pool)
        {
            throw invalid_argument("pool is uninitialized");
        }

        // Create new allocation and copy over value
        Pointer new_allocation(allocate_uint(capacity_, pool));
        set_uint_uint(plaintext_poly_.get(), coeff_count_, new_allocation.get());
        plaintext_poly_.acquire(new_allocation);

        // Finally set new pool if necessary
        // Note that this has to be done last to keep pool_ alive
        if (pool_ != pool)
        {
            pool_ = pool;
        }
    }

    void Plaintext::reserve(int capacity, const MemoryPoolHandle &pool)
    {
        if (is_alias())
        {
            throw logic_error("cannot reserve for aliased Plaintext");
        }
        if (!pool)
        {
            throw invalid_argument("pool is uninitialized");
        }
        if (capacity < 0)
        {
            throw invalid_argument("capacity cannot be negative");
        }

        int copy_coeff_count = min(capacity, coeff_count_);

        // Create new allocation and copy over value
        Pointer new_allocation(allocate_uint(capacity, pool));
        set_uint_uint(plaintext_poly_.get(), copy_coeff_count, new_allocation.get());
        plaintext_poly_.acquire(new_allocation);

        // Set the coeff_count and capacity
        capacity_ = capacity;
        coeff_count_ = copy_coeff_count;

        // Finally set new pool if necessary
        // Note that this has to be done last to keep pool_ alive
        if (pool_ != pool)
        {
            pool_ = pool;
        }
    }

    Plaintext &Plaintext::operator =(const BigPoly &poly)
    {
        if (poly.coeff_uint64_count() > 1)
        {
            throw invalid_argument("poly coefficients are too large");
        }

        int new_coeff_count = poly.coeff_count();

        // We do this to deal with the case where poly has empty coefficients
        int new_uint64_count = new_coeff_count * poly.coeff_uint64_count();

        // Resize and set value
        resize(new_uint64_count);
        set_uint_uint(poly.pointer(), new_uint64_count, plaintext_poly_.get());

        return *this;
    }

    Plaintext &Plaintext::operator =(const string &hex_poly)
    {
        int length = hex_poly.size();

        // Determine size needed to store string coefficient.
        int assign_coeff_count = 0;
        int assign_coeff_bit_count = 0;
        int pos = 0;
        int last_power = numeric_limits<int>::max();
        const char *hex_poly_ptr = hex_poly.data();
        while (pos < length)
        {
            // Determine length of coefficient starting at pos.
            int coeff_length = get_coeff_length(hex_poly_ptr + pos);
            if (coeff_length == 0)
            {
                throw invalid_argument("unable to parse hex_poly");
            }

            // Determine bit length of coefficient.
            int coeff_bit_count = get_hex_string_bit_count(hex_poly_ptr + pos, coeff_length);
            if (coeff_bit_count > assign_coeff_bit_count)
            {
                assign_coeff_bit_count = coeff_bit_count;
            }
            pos += coeff_length;

            // Extract power-term.
            int power_length = 0;
            int power = get_coeff_power(hex_poly_ptr + pos, &power_length);
            if (power == -1 || power >= last_power)
            {
                throw invalid_argument("unable to parse hex_poly");
            }
            if (assign_coeff_count == 0)
            {
                assign_coeff_count = power + 1;
            }
            pos += power_length;
            last_power = power;

            // Extract plus (unless it is the end).
            int plus_length = get_plus(hex_poly_ptr + pos);
            if (plus_length == -1)
            {
                throw invalid_argument("unable to parse hex_poly");
            }
            pos += plus_length;
        }

        // If string is empty, then done.
        if (assign_coeff_count == 0 || assign_coeff_bit_count == 0)
        {
            set_zero();
            return *this;
        }

        // Resize polynomial if needed.
        if (assign_coeff_bit_count > bits_per_uint64)
        {
            throw invalid_argument("hex_poly has too large coefficients");
        }
        if (coeff_count_ < assign_coeff_count)
        {
            resize(max(assign_coeff_count, coeff_count_));
        }

        // Populate polynomial from string.
        pos = 0;
        last_power = coeff_count_;
        while (pos < length)
        {
            // Determine length of coefficient starting at pos.
            const char *coeff_start = hex_poly_ptr + pos;
            int coeff_length = get_coeff_length(coeff_start);
            pos += coeff_length;

            // Extract power-term.
            int power_length = 0;
            int power = get_coeff_power(hex_poly_ptr + pos, &power_length);
            pos += power_length;

            // Extract plus (unless it is the end).
            int plus_length = get_plus(hex_poly_ptr + pos);
            pos += plus_length;

            // Zero coefficients not set by string.
            for (int zero_power = last_power - 1; zero_power > power; --zero_power)
            {
                plaintext_poly_[zero_power] = 0;
            }

            // Populate coefficient.
            uint64_t *coeff_ptr = plaintext_poly_.get() + power;
            hex_string_to_uint(coeff_start, coeff_length, 1, coeff_ptr);
            last_power = power;
        }

        // Zero coefficients not set by string.
        for (int zero_power = last_power - 1; zero_power >= 0; --zero_power)
        {
            plaintext_poly_[zero_power] = 0;
        }

        return *this;
    }

    void Plaintext::save(ostream &stream) const
    {
        int32_t coeff_count32 = static_cast<int32_t>(coeff_count_);
        stream.write(reinterpret_cast<const char*>(&coeff_count32), sizeof(int32_t));
        stream.write(reinterpret_cast<const char*>(plaintext_poly_.get()), coeff_count_ * bytes_per_uint64);
    }

    void Plaintext::python_save(std::string &path) const
    {
        std::ofstream out(path);
        save(out);
        out.close();
    }

    void Plaintext::load(istream &stream)
    {
        int32_t read_coeff_count = 0;
        stream.read(reinterpret_cast<char*>(&read_coeff_count), sizeof(int32_t));

        // Set new size
        resize(read_coeff_count);
        
        // Read data
        stream.read(reinterpret_cast<char*>(plaintext_poly_.get()), read_coeff_count * bytes_per_uint64);
    }

    void Plaintext::python_load(std::string &path)
    {
        std::ifstream in(path);
        load(in);
        in.close();
    }
}
