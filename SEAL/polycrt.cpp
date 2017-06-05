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

    PolyCRTBuilder::PolyCRTBuilder(const EncryptionParameters &parms, const MemoryPoolHandle &pool) :
        pool_(pool), ntt_tables_(pool_), slot_modulus_(parms.plain_modulus()), poly_modulus_(parms.poly_modulus()),
        slots_(parms.poly_modulus().significant_coeff_count() - 1), qualifiers_(parms.get_qualifiers())
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

        int coeff_bit_count = slot_modulus_.bit_count();
        int coeff_uint64_count = slot_modulus_.uint64_count();
        int poly_coeff_count = poly_modulus_.coeff_count();

        slot_modulus_.resize(coeff_bit_count);
        poly_modulus_.resize(poly_coeff_count, coeff_uint64_count * bits_per_uint64);

        // Set mod_ and polymod_
        mod_ = Modulus(slot_modulus_.pointer(), coeff_uint64_count, pool_);
        polymod_ = PolyModulus(poly_modulus_.pointer(), poly_coeff_count, coeff_uint64_count);

        // Reserve space for all of the primitive roots
        roots_of_unity_.resize(slots_, coeff_bit_count);

        // Copy over NTT tables (switching to local pool)
        ntt_tables_ = parms.plain_ntt_tables_;

        // Fill the vector of roots of unity with all distinct odd powers of generator.
        // These are all the primitive (2*slots_)-th roots of unity in integers modulo slot_modulus_.
        populate_roots_of_unity_vector();
    }

    PolyCRTBuilder::PolyCRTBuilder(const PolyCRTBuilder &copy) : pool_(copy.pool_), ntt_tables_(copy.ntt_tables_),
        slot_modulus_(copy.slot_modulus_), poly_modulus_(copy.poly_modulus_), slots_(copy.slots_),
        roots_of_unity_(copy.roots_of_unity_), qualifiers_(copy.qualifiers_)
    {
        int coeff_uint64_count = slot_modulus_.uint64_count();
        int poly_coeff_count = poly_modulus_.coeff_count();

        // Set mod_ and polymod_
        mod_ = Modulus(slot_modulus_.pointer(), coeff_uint64_count, pool_);
        polymod_ = PolyModulus(poly_modulus_.pointer(), poly_coeff_count, coeff_uint64_count);
    }

    void PolyCRTBuilder::populate_roots_of_unity_vector()
    {
        int coeff_uint64_count = slot_modulus_.uint64_count();

        //Pointer current_root(allocate_uint(coeff_uint64_count, pool_));
        Pointer generator_sq(allocate_uint(coeff_uint64_count, pool_));
        multiply_uint_uint_mod(ntt_tables_.get_root(), ntt_tables_.get_root(), mod_, generator_sq.get(), pool_);
        
        uint64_t *destination_ptr = roots_of_unity_.pointer();
        set_uint_uint(ntt_tables_.get_root(), coeff_uint64_count, destination_ptr);
         
        //Pointer current_root_copy(allocate_uint(coeff_uint64_count, pool_));
        
        for (int i = 1; i < slots_; i++)
        {
            multiply_uint_uint_mod(destination_ptr, generator_sq.get(), mod_, destination_ptr + coeff_uint64_count, pool_);
            destination_ptr += coeff_uint64_count;
        }
    }

    void PolyCRTBuilder::compose(vector<uint64_t> values, BigPoly &destination)
    {
        // Verify that slot modulus is not too large
        if (slot_modulus_.uint64_count() > 1)
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
            if (values[i] >= slot_modulus_[0])
            {
                throw invalid_argument("input value is larger than slot_modulus");
            }
#endif
            newvalues.emplace_back(BigUInt(slot_modulus_.bit_count(), &values[i])); 
        }
        compose(newvalues, destination);
    }


    void PolyCRTBuilder::compose(const vector<BigUInt> &values, BigPoly &destination)
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
            if (is_greater_than_or_equal_uint_uint(values[i].pointer(), slot_modulus_.pointer(), coeff_uint64_count))
            {
                throw invalid_argument("input value is larger than slot_modulus");
            }
#endif
            set_uint_uint(values[i].pointer(), coeff_uint64_count, destination[i].pointer());
        }

        // Transform destination using inverse of negacyclic NTT. The slots are in a "permuted" order, 
        // where the value in the slot corresponding to the (2i+1)-st power of the primitive root is in the 
        // location given by negacyclic_ntt_index_scramble(i).
        inverse_ntt_negacyclic_harvey(destination.pointer(), ntt_tables_, pool_);
    }

    void PolyCRTBuilder::decompose(const BigPoly &poly, vector<BigUInt> &destination)
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
        Pointer poly_copy(allocate_poly(poly.coeff_count(), poly.coeff_uint64_count(), pool_));
        set_poly_poly(poly.pointer(), poly.coeff_count(), poly.coeff_uint64_count(), poly_copy.get());

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