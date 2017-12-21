#include "polycrt.h"
#include "util/uintcore.h"
#include "util/uintarithmod.h"
#include "util/uintarith.h"
#include "util/polyarith.h"
#include "util/polyarithmod.h"
#include <stdexcept>
#include <random>

using namespace std;
using namespace seal::util;

namespace seal
{
    PolyCRTBuilder::PolyCRTBuilder(const SEALContext &context, const MemoryPoolHandle &pool) :
        pool_(pool), parms_(context.get_parms()),
        ntt_tables_(pool_), 
        slots_(parms_.poly_modulus().coeff_count() - 1), 
        qualifiers_(context.get_qualifiers())
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

        int coeff_uint64_count = parms_.plain_modulus().uint64_count();

        // Set mod_ and polymod_
        mod_ = Modulus(parms_.plain_modulus().pointer(), coeff_uint64_count, pool_);
        polymod_ = PolyModulus(parms_.poly_modulus().pointer(), parms_.poly_modulus().coeff_count(), parms_.poly_modulus().coeff_uint64_count());

        // Reserve space for all of the primitive roots
        roots_of_unity_ = allocate_poly(slots_, coeff_uint64_count, pool_);

        // Copy over NTT tables (switching to local pool)
        ntt_tables_ = context.plain_ntt_tables_;

        // Fill the vector of roots of unity with all distinct odd powers of generator.
        // These are all the primitive (2*slots_)-th roots of unity in integers modulo parms_.plain_modulus().
        populate_roots_of_unity_vector();
    }

    PolyCRTBuilder::PolyCRTBuilder(const PolyCRTBuilder &copy) : 
        pool_(copy.pool_), parms_(copy.parms_),
        ntt_tables_(copy.ntt_tables_),
        slots_(copy.slots_),
        qualifiers_(copy.qualifiers_)
    {
        int coeff_uint64_count = parms_.plain_modulus().uint64_count();

        // Allocate and copy over roots of unity
        roots_of_unity_ = allocate_poly(slots_, coeff_uint64_count, pool_);
        set_poly_poly(copy.roots_of_unity_.get(), slots_, coeff_uint64_count, roots_of_unity_.get());

        // Set mod_ and polymod_
        mod_ = Modulus(parms_.plain_modulus().pointer(), parms_.plain_modulus().uint64_count(), pool_);
        polymod_ = PolyModulus(parms_.poly_modulus().pointer(), parms_.poly_modulus().coeff_count(), parms_.poly_modulus().coeff_uint64_count());
    }

    void PolyCRTBuilder::populate_roots_of_unity_vector()
    {
        int coeff_uint64_count = parms_.plain_modulus().uint64_count();

        Pointer generator_sq(allocate_uint(coeff_uint64_count, pool_));
        uint64_t root = ntt_tables_.get_root();
        multiply_uint_uint_mod(&root, &root, mod_, generator_sq.get(), pool_);
        
        uint64_t *destination_ptr = roots_of_unity_.get();
        set_uint_uint(&root, coeff_uint64_count, destination_ptr);
                 
        for (int i = 1; i < slots_; i++)
        {
            multiply_uint_uint_mod(destination_ptr, generator_sq.get(), mod_, destination_ptr + coeff_uint64_count, pool_);
            destination_ptr += coeff_uint64_count;
        }
    }

    void PolyCRTBuilder::compose(vector<uint64_t> values, Plaintext &destination)
    {
        // Verify that slot modulus is not too large
        if (parms_.plain_modulus().uint64_count() > 1)
        {
            throw logic_error("plaintext modulus in encryption parameters is too large");
        }

        // Verify that the slot count matches the size of values
        if (values.size() != slots_)
        {
            throw invalid_argument("incorrect number of input values");
        }

        vector<BigUInt> newvalues;
        for (int i = 0; i < values.size(); i++) 
        {
#ifdef _DEBUG
            if (values[i] >= *parms_.plain_modulus().pointer())
            {
                throw invalid_argument("input value is larger than slot_modulus");
            }
#endif
            newvalues.emplace_back(BigUInt(parms_.plain_modulus().bit_count(), &values[i])); 
        }
        compose(newvalues, destination);
    }


    void PolyCRTBuilder::compose(const vector<BigUInt> &values, Plaintext &destination)
    {
        BigPoly &destination_poly = destination.get_poly();

        int coeff_bit_count = parms_.plain_modulus().bit_count();
        int poly_coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_uint64_count = parms_.plain_modulus().uint64_count();

        // Validate input parameters
        if (values.size() != slots_)
        {
            throw invalid_argument("incorrect number of input values");
        }

        if (destination_poly.coeff_count() != poly_coeff_count || destination_poly.coeff_bit_count() != coeff_bit_count)
        {
            destination_poly.resize(poly_coeff_count, coeff_bit_count);
        }
        destination_poly.set_zero();

        // First write the values to destination coefficients
        for (int i = 0; i < slots_; i++)
        {
            // Validate the i-th input
            if (values[i].bit_count() != coeff_bit_count)
            {
                throw invalid_argument("input value has incorrect size");
            }
#ifdef _DEBUG
            if (is_greater_than_or_equal_uint_uint(values[i].pointer(), parms_.plain_modulus().pointer(), coeff_uint64_count))
            {
                throw invalid_argument("input value is larger than slot_modulus");
            }
#endif
            set_uint_uint(values[i].pointer(), coeff_uint64_count, destination_poly.pointer(i));
        }

        // Transform destination using inverse of negacyclic NTT. The slots are in a "permuted" order, 
        // where the value in the slot corresponding to the (2i+1)-st power of the primitive root is in the 
        // location given by reverse_bits(i, coeff_count_power).
        inverse_ntt_negacyclic_harvey(destination_poly.pointer(), ntt_tables_, pool_);
    }

    void PolyCRTBuilder::decompose(const Plaintext &plain, vector<BigUInt> &destination)
    {
        const BigPoly &plain_poly = plain.get_poly();

        int poly_coeff_count = parms_.poly_modulus().coeff_count();
        int coeff_bit_count = parms_.plain_modulus().bit_count();
        int coeff_uint64_count = parms_.plain_modulus().uint64_count();

        // Validate input parameters
        if (plain_poly.coeff_count() != poly_coeff_count || plain_poly.coeff_bit_count() != coeff_bit_count)
        {
            throw invalid_argument("poly has incorrect size");
        }

        // Make a copy of poly
        Pointer poly_copy(allocate_poly(plain_poly.coeff_count(), plain_poly.coeff_uint64_count(), pool_));
        set_poly_poly(plain_poly.pointer(), plain_poly.coeff_count(), plain_poly.coeff_uint64_count(), poly_copy.get());

        // Transform destination using negacyclic NTT.
        ntt_negacyclic_harvey(poly_copy.get(), ntt_tables_, pool_);
        
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