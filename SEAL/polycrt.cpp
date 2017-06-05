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
    namespace
    {
        inline uint32_t negacyclic_ntt_index_scramble(uint32_t index, int coeff_count_power)
        {
            return reverse_bits(index) >> (32 - coeff_count_power);
        }
    }

    PolyCRTBuilder::PolyCRTBuilder(const EncryptionParameters &parms)
        : slots_(parms.poly_modulus().significant_coeff_count() - 1), qualifiers_(parms.get_qualifiers())
    {
        // Verify parameters
        if (!qualifiers_.parameters_set)
        {
            throw invalid_argument("encryption parameters are not set correctly");
        }
        if (!qualifiers_.enable_batching)
        {
            throw invalid_argument("encryption parameters are not valid for batching");
        }

        BigPoly poly_modulus = parms.poly_modulus();
        BigUInt plain_modulus = parms.plain_modulus();

        int coeff_uint64_count = plain_modulus.uint64_count();
        int poly_coeff_count = poly_modulus.coeff_count();

        // Allocate memory for slot_modulus_ and poly_modulus_
        uint64_t *slot_modulus_ptr = new uint64_t[coeff_uint64_count];
        uint64_t *poly_modulus_ptr = new uint64_t[poly_modulus.coeff_count() * coeff_uint64_count];

        // Set slot_modulus_ and poly_modulus_
        MemoryPool &pool = *MemoryPool::default_pool();
        set_uint_uint(plain_modulus.pointer(), coeff_uint64_count, slot_modulus_ptr);
        set_poly_poly(poly_modulus.pointer(), poly_modulus.coeff_count(), poly_modulus.coeff_uint64_count(), poly_coeff_count, coeff_uint64_count, poly_modulus_ptr);
        slot_modulus_ = Modulus(slot_modulus_ptr, coeff_uint64_count, pool);
        poly_modulus_ = PolyModulus(poly_modulus_ptr, poly_modulus.coeff_count(), coeff_uint64_count);

        // Reserve space for all of the primitive roots
        roots_of_unity_ = allocate_uint(coeff_uint64_count * slots_, pool);

        // Generate the NTTTables
        if (!ntt_tables_.generate(poly_modulus_.coeff_count_power_of_two(), slot_modulus_))
        {
            // Something went wrong.
            throw invalid_argument("failed to generate NTT tables");
        }

        // Fill the vector of roots of unity with all distinct odd powers of generator.
        // These are all the primitive (2*slots_)-th roots of unity in integers modulo slot_modulus_.
        populate_roots_of_unity_vector();
    }

    PolyCRTBuilder::~PolyCRTBuilder()
    {
        delete[] slot_modulus_.get();
        delete[] poly_modulus_.get();
        slot_modulus_ = Modulus();
        poly_modulus_ = PolyModulus();
    }

    void PolyCRTBuilder::populate_roots_of_unity_vector()
    {
        MemoryPool &pool = *MemoryPool::default_pool();
        int coeff_uint64_count = slot_modulus_.uint64_count();

        //Pointer current_root(allocate_uint(coeff_uint64_count, pool));
        Pointer generator_sq(allocate_uint(coeff_uint64_count, pool));
        multiply_uint_uint_mod(ntt_tables_.get_root(), ntt_tables_.get_root(), slot_modulus_, generator_sq.get(), pool);
        
        uint64_t *destination_ptr = roots_of_unity_.get();
        set_uint_uint(ntt_tables_.get_root(), coeff_uint64_count, destination_ptr);
         
        //Pointer current_root_copy(allocate_uint(coeff_uint64_count, pool));
        
        for (int i = 1; i < slots_; i++)
        {
            multiply_uint_uint_mod(destination_ptr, generator_sq.get(), slot_modulus_, destination_ptr + coeff_uint64_count, pool);
            destination_ptr += coeff_uint64_count;
        }
    }

    void PolyCRTBuilder::compose(const vector<BigUInt> &values, BigPoly &destination) const
    {
        int coeff_bit_count = slot_modulus_.significant_bit_count();
        int poly_coeff_count = poly_modulus_.coeff_count();
        int coeff_uint64_count = slot_modulus_.uint64_count();

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

        // First write the values to destination coefficients
        for (int i = 0; i < slots_; i++)
        {
            // Validate the i-th input
            if (values[i].bit_count() != coeff_bit_count)
            {
                throw invalid_argument("input value has incorrect size");
            }
#ifdef _DEBUG
            if (is_greater_than_or_equal_uint_uint(values[i].pointer(), slot_modulus_.get(), coeff_uint64_count))
            {
                throw invalid_argument("input value is larger than slot_modulus");
            }
#endif
            set_uint_uint(values[i].pointer(), coeff_uint64_count, destination[i].pointer());
        }

        // Transform destination using inverse of negacyclic NTT. The slots are in a "permuted" order, 
        // where the value in the slot corresponding to the (2i+1)-st power of the primitive root is in the 
        // location given by negacyclic_ntt_index_scramble(i).
        MemoryPool &pool = *MemoryPool::default_pool();
        inverse_ntt_negacyclic_harvey(destination.pointer(), ntt_tables_, pool);
    }

    void PolyCRTBuilder::decompose(const BigPoly &poly, vector<BigUInt> &destination) const
    {
        int coeff_bit_count = slot_modulus_.significant_bit_count();
        int poly_coeff_count = poly_modulus_.coeff_count();
        int coeff_uint64_count = slot_modulus_.uint64_count();

        // Validate input parameters
        if (poly.coeff_count() != poly_coeff_count || poly.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("poly has incorrect size");
        }

        // Make a copy of poly
        MemoryPool &pool = *MemoryPool::default_pool();
        Pointer poly_copy(allocate_poly(poly.coeff_count(), poly.coeff_uint64_count(), pool));
        set_poly_poly(poly.pointer(), poly.coeff_count(), poly.coeff_uint64_count(), poly_copy.get());

        // Transform destination using negacyclic NTT.
        ntt_negacyclic_harvey(poly_copy.get(), ntt_tables_, pool);
        
        // Create correct size polynomials to destination
        destination.assign(slots_, BigUInt(coeff_bit_count));

        // Read each of the slots by evaluating the polynomial at all of the primitive roots
        uint64_t *poly_copy_coeff_ptr = poly_copy.get();
        for (int i = 0; i < slots_; i++)
        {
            set_uint_uint(poly_copy_coeff_ptr, coeff_uint64_count, destination[i].pointer());
            poly_copy_coeff_ptr += coeff_uint64_count;
        }
    }
}