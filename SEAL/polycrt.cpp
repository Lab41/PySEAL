#include "polycrt.h"
#include "util/uintextras.h"
#include "util/uintcore.h"
#include "util/uintarithmod.h"
#include "util/uintarith.h"
#include "util/polyarith.h"
#include "util/polyarithmod.h"
#include "util/polyextras.h"
#include <stdexcept>
#include <random>

using namespace std;
using namespace seal::util;

namespace seal
{
    PolyCRTBuilder::PolyCRTBuilder(BigUInt slot_modulus, BigPoly poly_modulus) :
        slots_(poly_modulus.significant_coeff_count() - 1)
    {
        int coeff_uint64_count = slot_modulus.uint64_count();
        int poly_coeff_count = poly_modulus.coeff_count();

        // Validate input parameters
        if (slot_modulus.is_zero())
        {
            throw invalid_argument("slot_modulus cannot be zero");
        }
        if (poly_modulus.is_zero())
        {
            throw invalid_argument("poly_modulus cannot be zero");
        }

        // Allocate memory for slot_modulus_ and poly_modulus_
        uint64_t *slot_modulus_ptr = new uint64_t[coeff_uint64_count];
        uint64_t *poly_modulus_ptr = new uint64_t[poly_modulus.coeff_count() * coeff_uint64_count];

        // Set slot_modulus_ and poly_modulus_
        MemoryPool &pool = *MemoryPool::default_pool();
        set_uint_uint(slot_modulus.pointer(), coeff_uint64_count, slot_modulus_ptr);
        set_poly_poly(poly_modulus.pointer(), poly_modulus.coeff_count(), poly_modulus.coeff_uint64_count(), poly_coeff_count, coeff_uint64_count, poly_modulus_ptr);
        slot_modulus_ = Modulus(slot_modulus_ptr, coeff_uint64_count, pool);
        poly_modulus_ = PolyModulus(poly_modulus_ptr, poly_modulus.coeff_count(), coeff_uint64_count);

        // Check that slots_ is a power of two
        if (!poly_modulus_.is_coeff_count_power_of_two())
        {
            throw invalid_argument("degree of poly_modulus must be a power of two");
        }

        // Test if integers modulo slot_modulus contains a primitive 2*deg(poly_modulus)-th root
        --slot_modulus;
        uint64_t cyclic_size = 2 * slots_;
        modulo_uint_inplace(slot_modulus.pointer(), coeff_uint64_count, Modulus(&cyclic_size, 1, pool), pool);
        if (!slot_modulus.is_zero())
        {
            throw invalid_argument("integers modulo slot_modulus do not contain a valid primitive root of unity");
        }

        // Reserve space for all of the primitive roots
        roots_of_unity_.reserve(slots_);

        // Find a minimal primitive (2*slots_)-th root of unity in integers modulo slot_modulus_.
        Pointer generator(allocate_uint(coeff_uint64_count, pool));
        find_generator(generator.get());

        // Fill the vector of roots of unity with all distinct odd powers of generator.
        // These are all the primitive (2*slots_)-th roots of unity in integers modulo slot_modulus_.
        populate_roots_of_unity_vector(generator.get());

        // Populate the vector of uint64_t pointers for CRT base polys
        crt_base_polys_.assign(slots_, nullptr);
    }

    PolyCRTBuilder::~PolyCRTBuilder()
    {
        delete[] slot_modulus_.get();
        delete[] poly_modulus_.get();

        for (size_t i = 0; i < slots_; ++i)
        {
            delete[] crt_base_polys_[i];
        }
    }

    void PolyCRTBuilder::populate_roots_of_unity_vector(const uint64_t *generator)
    {
        int coeff_uint64_count = slot_modulus_.uint64_count();

        MemoryPool &pool = *MemoryPool::default_pool();
        Pointer current_root(allocate_uint(coeff_uint64_count, pool));
        set_uint_uint(generator, coeff_uint64_count, current_root.get());

        Pointer generator_sq(allocate_uint(coeff_uint64_count, pool));
        multiply_uint_uint_mod(generator, generator, slot_modulus_, generator_sq.get(), pool);

        ConstPointer current_root_copy;
        for (size_t i = 0; i < slots_; ++i)
        {
            roots_of_unity_.push_back(duplicate_uint_if_needed(current_root.get(), coeff_uint64_count, coeff_uint64_count, true, pool));

            current_root_copy = duplicate_uint_if_needed(current_root.get(), coeff_uint64_count, coeff_uint64_count, true, pool);
            multiply_uint_uint_mod(current_root_copy.get(), generator_sq.get(), slot_modulus_, current_root.get(), pool);
        }
    }

    void PolyCRTBuilder::prepare_slot(size_t index)
    {
        // Validate input parameters
        if (index > slots_ - 1)
        {
            throw invalid_argument("index out of bounds");
        }

        // Is the pointer at the index-th slot already set? In that case do nothing.
        if (is_slot_prepared(index))
        {
            return;
        }

        int coeff_uint64_count = slot_modulus_.uint64_count();
        int poly_coeff_count = poly_modulus_.coeff_count();

        // Allocate memory for the poly
        crt_base_polys_[index] = new uint64_t[coeff_uint64_count * poly_coeff_count];
        set_zero_poly(poly_coeff_count, coeff_uint64_count, crt_base_polys_[index]);
        
        uint64_t *crt_base_poly_coeff_ptr = get_poly_coeff(crt_base_polys_[index], poly_coeff_count - 2, coeff_uint64_count);

        /*
        Basically we want to divide x^n+1 by x-root_of_unity. The result is x^(n-1) + root*x^(n-2) + ... + root^(n-1).
        But this poly is still not equal to 1 when evaluated at the root, so we need to normalize each coefficient accordingly.
        The normalizing factor is the inverse of poly_coeff_count * roots_of_unity_[index]^(poly_coeff_count - 1),
        which we therefore need to first compute.
        */

        // Set normalize_polys to false to instead prepare the slots in a way that allows for very fast addition of slots
        // by simply looking at the top coefficient of the composed polynomial.
        bool normalize_polys = true;
        MemoryPool &pool = *MemoryPool::default_pool();
        if (normalize_polys)
        {
            Pointer temp_uint(allocate_uint(coeff_uint64_count, pool));
            Pointer value_at_root(allocate_uint(coeff_uint64_count, pool));
            Pointer inv_value_at_root(allocate_uint(coeff_uint64_count, pool));
            Pointer poly_coeff_count_uint(allocate_uint(coeff_uint64_count, pool));

            set_uint(static_cast<uint64_t>(poly_coeff_count) - 1, coeff_uint64_count, poly_coeff_count_uint.get());
            uint64_t exponent = static_cast<uint64_t>(poly_coeff_count) - 2;
            exponentiate_uint_mod(roots_of_unity_[index].get(), &exponent, 1, slot_modulus_, temp_uint.get(), pool);
            multiply_uint_uint_mod(temp_uint.get(), poly_coeff_count_uint.get(), slot_modulus_, value_at_root.get(), pool);
            if (!try_invert_uint_mod(value_at_root.get(), slot_modulus_.get(), coeff_uint64_count, inv_value_at_root.get(), pool))
            {
                // Perhaps slot_modulus_ is not prime?
                throw logic_error("inversion modulo slot_modulus_ failed");
            }

            // Set the top coefficient of the result to inv_value_at_root. This multiplier will then propagate to all
            // of the rest of the coefficients in the loop.
            set_uint_uint(inv_value_at_root.get(), coeff_uint64_count, crt_base_poly_coeff_ptr);
        }
        else
        {
            set_uint(1, coeff_uint64_count, crt_base_poly_coeff_ptr);
        }

        // Compute the remaining coefficients.
        for (int i = poly_coeff_count - 3; i >= 0; --i)
        {
            crt_base_poly_coeff_ptr -= coeff_uint64_count;
            multiply_uint_uint_mod(crt_base_poly_coeff_ptr + coeff_uint64_count, roots_of_unity_[index].get(), slot_modulus_, crt_base_poly_coeff_ptr, pool);
        }
    }

    void PolyCRTBuilder::prepare_all_slots()
    {
        for (size_t i = 0; i < slots_; ++i)
        {
            prepare_slot(i);
        }
    }

    bool PolyCRTBuilder::is_generator(const uint64_t *root)
    {
        int coeff_uint64_count = slot_modulus_.uint64_count();

        if (is_zero_uint(root, coeff_uint64_count))
        {
            return false;
        }

        // We check if root is a 2*slots_-th root of unity in integers modulo slot_modulus_.
        // It suffices to check that root^slots_ is -1 modulo slot_modulus_.
        MemoryPool &pool = *MemoryPool::default_pool();
        Pointer power(allocate_uint(coeff_uint64_count, pool));
        exponentiate_uint_mod(root, &slots_, 1, slot_modulus_, power.get(), pool);
        increment_uint_mod(power.get(), slot_modulus_.get(), coeff_uint64_count, power.get());

        return is_zero_uint(power.get(), coeff_uint64_count);
    }

    void PolyCRTBuilder::set_random(uint64_t *destination)
    {
        int coeff_uint64_count = slot_modulus_.uint64_count();
        int *random_int_ptr = reinterpret_cast<int*>(destination);

        for (int i = 0; i < 2 * coeff_uint64_count; ++i)
        {
            *random_int_ptr++ = rand();
        }

        MemoryPool &pool = *MemoryPool::default_pool();
        modulo_uint_inplace(destination, coeff_uint64_count, slot_modulus_, pool);
    }

    void PolyCRTBuilder::find_minimal_generator(const uint64_t *generator, uint64_t *destination)
    {
        int coeff_uint64_count = slot_modulus_.uint64_count();

        MemoryPool &pool = *MemoryPool::default_pool();
        Pointer generator_sq(allocate_uint(coeff_uint64_count, pool));
        multiply_uint_uint_mod(generator, generator, slot_modulus_, generator_sq.get(), pool);

        Pointer current_generator(allocate_uint(coeff_uint64_count, pool));
        set_uint_uint(generator, coeff_uint64_count, current_generator.get());

        // destination is going to contain always the smallest generator found
        set_uint_uint(current_generator.get(), coeff_uint64_count, destination);

        for (size_t i = 0; i < slots_; ++i)
        {
            // If our current generator is strictly smaller than destination, update
            if (compare_uint_uint(current_generator.get(), destination, coeff_uint64_count) == -1)
            {
                set_uint_uint(current_generator.get(), coeff_uint64_count, destination);
            }

            // Then move on to the next generator
            ConstPointer current_generator_copy(duplicate_uint_if_needed(current_generator.get(), coeff_uint64_count, coeff_uint64_count, true, pool));
            multiply_uint_uint_mod(current_generator_copy.get(), generator_sq.get(), slot_modulus_, current_generator.get(), pool);
        }
    }

    void PolyCRTBuilder::find_generator(uint64_t *destination)
    {
        MemoryPool &pool = *MemoryPool::default_pool();
        int coeff_uint64_count = slot_modulus_.uint64_count();
        Pointer random_ptr(allocate_uint(coeff_uint64_count, pool));

        // We need to divide slot_modulus_-1 by 2*slots_ to get the size of the quotient group
        Pointer size_entire_group(allocate_uint(coeff_uint64_count, pool));
        decrement_uint(slot_modulus_.get(), coeff_uint64_count, size_entire_group.get());

        Pointer divisor(allocate_uint(coeff_uint64_count, pool));
        set_uint(slots_, coeff_uint64_count, divisor.get());
        left_shift_uint(divisor.get(), 1, coeff_uint64_count, divisor.get());

        // Compute size of quotient group
        Pointer size_quotient_group(allocate_uint(coeff_uint64_count, pool));
        divide_uint_uint_inplace(size_entire_group.get(), divisor.get(), coeff_uint64_count, size_quotient_group.get(), pool);

        while (true)
        {
            set_random(random_ptr.get());

            // Raise the random number to power the size of the quotient to get rid of irrelevant part
            exponentiate_uint_mod(random_ptr.get(), size_quotient_group.get(), coeff_uint64_count, slot_modulus_, random_ptr.get(), pool);

            // Now test if we have a generator of the cyclic group of 2*slots_ elements
            if (is_generator(random_ptr.get()))
            {
                // OK we have a generator. Let's find the smallest generator in the group
                // and use that instead.
                find_minimal_generator(random_ptr.get(), destination);
                return;
            }
        }
    }

    void PolyCRTBuilder::compose(const vector<BigUInt> &values, BigPoly &destination)
    {
        int coeff_bit_count = slot_modulus_.significant_bit_count();
        int poly_coeff_count = poly_modulus_.coeff_count();

        // Validate input parameters
        if (values.size() != slots_)
        {
            throw invalid_argument("incorrect number of input values");
        }

        if (destination.coeff_count() != poly_coeff_count || destination.coeff_bit_count() != coeff_bit_count)
        {
            destination.resize(poly_coeff_count, coeff_bit_count);
        }
        destination.set_zero();

        for (size_t i = 0; i < slots_; ++i)
        {
            // Validate the i-th input
            if (values[i].bit_count() != coeff_bit_count)
            {
                throw invalid_argument("input value has incorrect size");
            }

            // We use the local memory pool here
            add_to_slot(values[i].pointer(), i, destination.pointer());
        }
    }

    void PolyCRTBuilder::decompose(const BigPoly &poly, vector<BigUInt> &destination)
    {
        int coeff_bit_count = slot_modulus_.significant_bit_count();
        int poly_coeff_count = poly_modulus_.coeff_count();

        // Validate input parameters
        if (poly.coeff_count() != poly_coeff_count || poly.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("poly has incorrect size");
        }

        // Create correct size polynomials to destination
        destination.assign(slots_, BigUInt(coeff_bit_count));

        // Read each of the slots by evaluating the polynomial at all of the primitive roots
        for (size_t i = 0; i < slots_; ++i)
        {
            get_slot(poly.pointer(), i, destination[i].pointer());
        }
    }

    void PolyCRTBuilder::get_slot(const uint64_t *poly, size_t index, uint64_t *destination)
    {
        int coeff_uint64_count = slot_modulus_.uint64_count();
        int poly_coeff_count = poly_modulus_.coeff_count();

#ifdef _DEBUG
        if (poly == nullptr)
        {
            throw invalid_argument("poly cannot be null");
        }
        if (destination == nullptr)
        {
            throw invalid_argument("destination cannot be null");
        }
        if (index > slots_ - 1)
        {
            throw invalid_argument("index out of bounds");
        }
        if (get_significant_coeff_count_poly(poly, poly_coeff_count, coeff_uint64_count) == poly_coeff_count || !are_poly_coefficients_less_than(poly, poly_coeff_count, coeff_uint64_count, slot_modulus_.get(), coeff_uint64_count))
        {
            throw invalid_argument("poly is larger than slot_modulus and poly_modulus allow");
        }
#endif

        // Evaluate poly at the particular root of unity to recover the slot
        MemoryPool &pool = *MemoryPool::default_pool();
        poly_eval_uint_mod(poly, poly_coeff_count, roots_of_unity_[index].get(), slot_modulus_, destination, pool);
    }

    void PolyCRTBuilder::get_slot(const BigPoly &poly, size_t index, BigUInt &destination)
    {
        int coeff_bit_count = slot_modulus_.significant_bit_count();
        int poly_coeff_count = poly_modulus_.coeff_count();

        // Validate input parameters
        if (index > slots_ - 1)
        {
            throw invalid_argument("index out of bounds");
        }
        if (poly.coeff_count() != poly_coeff_count || poly.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("poly has incorrect size");
        }

        // Resize destination if needed
        if (destination.bit_count() != coeff_bit_count)
        {
            destination.resize(coeff_bit_count);
        }

        // Evaluate poly at the particular root of unity to recover the slot
        get_slot(poly.pointer(), index, destination.pointer());
    }

    void PolyCRTBuilder::add_to_slot(const uint64_t *value, size_t index, uint64_t *destination)
    {
        int coeff_uint64_count = slot_modulus_.uint64_count();
        int poly_coeff_count = poly_modulus_.coeff_count();

#ifdef _DEBUG
        if (value == nullptr)
        {
            throw invalid_argument("value cannot be null");
        }
        if (destination == nullptr)
        {
            throw invalid_argument("destination cannot be null");
        }
        if (index > slots_ - 1)
        {
            throw invalid_argument("index out of bounds");
        }
        if (compare_uint_uint(value, coeff_uint64_count, slot_modulus_.get(), coeff_uint64_count) >= 0)
        {
            throw invalid_argument("value is larger than slot_modulus allows");
        }
        if (get_significant_coeff_count_poly(destination, poly_coeff_count, coeff_uint64_count) == poly_coeff_count || !are_poly_coefficients_less_than(destination, poly_coeff_count, coeff_uint64_count, slot_modulus_.get(), coeff_uint64_count))
        {
            throw invalid_argument("destination is larger than slot_modulus and poly_modulus allow");
        }
#endif

        // Is there anything to do?
        if (is_zero_uint(value, coeff_uint64_count))
        {
            return;
        }

        MemoryPool &pool = *MemoryPool::default_pool();
        Pointer poly_crt_term(allocate_poly(poly_coeff_count, coeff_uint64_count, pool));

        prepare_slot(index);
        multiply_poly_scalar_coeffmod(crt_base_polys_[index], poly_coeff_count, value, slot_modulus_, poly_crt_term.get(), pool);
        add_poly_poly_coeffmod(destination, poly_crt_term.get(), poly_coeff_count, slot_modulus_.get(), coeff_uint64_count, destination);
    }

    void PolyCRTBuilder::add_to_slot(const BigUInt &value, std::size_t index, BigPoly &destination)
    {
        int coeff_bit_count = slot_modulus_.significant_bit_count();
        int poly_coeff_count = poly_modulus_.coeff_count();

        if (destination.coeff_bit_count() != coeff_bit_count || destination.coeff_count() != poly_coeff_count)
        {
            throw invalid_argument("destination has incorrect size");
        }
        if (value.bit_count() != coeff_bit_count)
        {
            throw invalid_argument("value has incorrect size");
        }

        add_to_slot(value.pointer(), index, destination.pointer());
    }


    void PolyCRTBuilder::set_slot(const uint64_t *value, size_t index, uint64_t *destination)
    {
        int coeff_uint64_count = slot_modulus_.uint64_count();
        int poly_coeff_count = poly_modulus_.coeff_count();

#ifdef _DEBUG
        if (value == nullptr)
        {
            throw invalid_argument("value cannot be null");
        }
        if (destination == nullptr)
        {
            throw invalid_argument("destination cannot be null");
        }
        if (index > slots_ - 1)
        {
            throw invalid_argument("index out of bounds");
        }
        if (compare_uint_uint(value, coeff_uint64_count, slot_modulus_.get(), coeff_uint64_count) >= 0)
        {
            throw invalid_argument("value is larger than slot_modulus allows");
        }
        if (get_significant_coeff_count_poly(destination, poly_coeff_count, coeff_uint64_count) == poly_coeff_count || !are_poly_coefficients_less_than(destination, poly_coeff_count, coeff_uint64_count, slot_modulus_.get(), coeff_uint64_count))
        {
            throw invalid_argument("destination is larger than slot_modulus and poly_modulus allow");
        }
#endif
        MemoryPool &pool = *MemoryPool::default_pool();
        Pointer diff_slot_content(allocate_uint(coeff_uint64_count, pool));

        // First read off the old slot content
        poly_eval_uint_mod(destination, poly_coeff_count, roots_of_unity_[index].get(), slot_modulus_, diff_slot_content.get(), pool);

        // Compute the difference of new and old slot contents
        sub_uint_uint_mod(value, diff_slot_content.get(), slot_modulus_.get(), coeff_uint64_count, diff_slot_content.get());

        // Is there anything to do?
        if (is_zero_uint(diff_slot_content.get(), coeff_uint64_count))
        {
            return;
        }

        // Now old_slot_content holds the difference betwen the new and old contents so add it to the slot
        add_to_slot(diff_slot_content.get(), index, destination);
    }

    void PolyCRTBuilder::set_slot(const BigUInt &value, std::size_t index, BigPoly &destination)
    {
        int coeff_bit_count = slot_modulus_.significant_bit_count();
        int poly_coeff_count = poly_modulus_.coeff_count();

        if (destination.coeff_bit_count() != coeff_bit_count || destination.coeff_count() != poly_coeff_count)
        {
            throw invalid_argument("destination has incorrect size");
        }
        if (value.bit_count() != coeff_bit_count)
        {
            throw invalid_argument("value has incorrect size");
        }

        set_slot(value.pointer(), index, destination.pointer());
    }

    BigPoly PolyCRTBuilder::get_slot_sum_scaling_poly(set<size_t> indices)
    {
        int coeff_bit_count = slot_modulus_.significant_bit_count();
        int coeff_uint64_count = slot_modulus_.uint64_count();
        
        // Set all slots first to zero
        vector<BigUInt> slot_sum_scaling_vector(slots_, BigUInt(coeff_bit_count, static_cast<uint64_t>(0)));
        
        // Then set those given as argument to the correct value
        MemoryPool &pool = *MemoryPool::default_pool();
        for (auto iter = indices.begin(); iter != indices.end(); ++iter)
        {
            prepare_slot(*iter);

            if (!try_invert_uint_mod(crt_base_polys_[*iter], slot_modulus_.get(), coeff_uint64_count, slot_sum_scaling_vector[*iter].pointer(), pool))
            {
                // Perhaps slot_modulus_ is not prime?
                throw logic_error("inversion modulo slot_modulus_ failed");
            }
        }

        // Create the polynomial
        return compose(slot_sum_scaling_vector);
    }

    BigPoly PolyCRTBuilder::get_slot_sum_scaling_poly()
    {
        int coeff_bit_count = slot_modulus_.significant_bit_count();
        int coeff_uint64_count = slot_modulus_.uint64_count();

        // Set all slots first to zero
        vector<BigUInt> slot_sum_scaling_vector(slots_, BigUInt(coeff_bit_count, static_cast<uint64_t>(0)));

        prepare_all_slots();

        // Then set those given as argument to the correct value
        MemoryPool &pool = *MemoryPool::default_pool();
        for (size_t i = 0; i < slots_; ++i)
        {
            if (!try_invert_uint_mod(crt_base_polys_[i], slot_modulus_.get(), coeff_uint64_count, slot_sum_scaling_vector[i].pointer(), pool))
            {
                // Perhaps slot_modulus_ is not prime?
                throw logic_error("inversion modulo slot_modulus_ failed");
            }
        }

        // Create the polynomial
        return compose(slot_sum_scaling_vector);
    }
}