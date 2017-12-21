#include "seal/polycrt.h"
#include "seal/util/uintcore.h"
#include "seal/util/uintarithmod.h"
#include "seal/util/uintarithsmallmod.h"
#include "seal/util/uintarith.h"
#include "seal/util/polyarith.h"
#include "seal/util/polyarithmod.h"
#include <stdexcept>
#include <random>

using namespace std;
using namespace seal::util;

namespace seal
{
    PolyCRTBuilder::PolyCRTBuilder(const SEALContext &context, const MemoryPoolHandle &pool) :
        pool_(pool), parms_(context.parms()),
        ntt_tables_(pool_),
        slots_(parms_.poly_modulus().coeff_count() - 1),
        qualifiers_(context.qualifiers())
    {
        int coeff_count = parms_.poly_modulus().coeff_count();

        // Verify parameters
        if (!qualifiers_.parameters_set)
        {
            throw invalid_argument("encryption parameters are not set correctly");
        }
        if (!qualifiers_.enable_batching)
        {
            throw invalid_argument("encryption parameters are not valid for batching");
        }
        if (!pool)
        {
            throw invalid_argument("pool is uninitialized");
        }

        // Set mod_ and polymod_
        mod_ = parms_.plain_modulus();
        polymod_ = PolyModulus(parms_.poly_modulus().pointer(), coeff_count, 
            parms_.poly_modulus().coeff_uint64_count());

        // Reserve space for all of the primitive roots
        roots_of_unity_ = allocate_uint(slots_, pool_);

        // Copy over NTT tables (switching to local pool)
        ntt_tables_ = context.plain_ntt_tables_;

        // Fill the vector of roots of unity with all distinct odd powers of generator.
        // These are all the primitive (2*slots_)-th roots of unity in integers modulo parms_.plain_modulus().
        populate_roots_of_unity_vector();

        // Populate matrix representation index map
        populate_matrix_reps_index_map();
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
        mod_ = parms_.plain_modulus();
        polymod_ = PolyModulus(parms_.poly_modulus().pointer(), parms_.poly_modulus().coeff_count(), 
            parms_.poly_modulus().coeff_uint64_count());
    }

    void PolyCRTBuilder::populate_roots_of_unity_vector()
    {
        uint64_t generator_sq = multiply_uint_uint_mod(ntt_tables_.get_root(), ntt_tables_.get_root(), mod_);
        roots_of_unity_[0] = ntt_tables_.get_root();

        for (int i = 0; i < slots_ - 1; i++)
        {
            roots_of_unity_[i + 1] = multiply_uint_uint_mod(roots_of_unity_[i], generator_sq, mod_);
        }
    }

    void PolyCRTBuilder::populate_matrix_reps_index_map()
    {
        int logn = get_power_of_two(slots_);
        uint32_t row_size = slots_ >> 1;
        matrix_reps_index_map_.resize(slots_);

        // Copy from the matrix to the value vectors 
        uint32_t gen = 3;
        uint32_t pos = 1;
        uint32_t m = slots_ << 1;
        for (uint32_t i = 0; i < row_size; i++)
        {
            // Position in normal bit order
            uint32_t index1 = (pos - 1) >> 1;
            uint32_t index2 = (m - pos - 1) >> 1;

            // Set the bit-reversed locations
            matrix_reps_index_map_[i] = util::reverse_bits(index1, logn);
            matrix_reps_index_map_[row_size | i] = util::reverse_bits(index2, logn);

            // Next primitive root
            pos *= gen;
            pos &= (m - 1);
        }
    }

    void PolyCRTBuilder::compose(const vector<uint64_t> &values_matrix, Plaintext &destination)
    {
        // Validate input parameters
        if (values_matrix.size() > slots_)
        {
            throw logic_error("values_matrix size is too large");
        }
#ifdef SEAL_DEBUG
        for (auto v : values_matrix)
        {
            // Validate the i-th input
            if (v >= mod_.value())
            {
                throw invalid_argument("input value is larger than plain_modulus");
            }
        }
#endif
        int input_matrix_size = values_matrix.size();

        // Set destination to full size
        destination.resize(slots_);

        // First write the values to destination coefficients. Read 
        // in top row, then bottom row.
        for (int i = 0; i < input_matrix_size; i++)
        {
            *(destination.pointer() + matrix_reps_index_map_[i]) = values_matrix[i];
        }        
        for (int i = input_matrix_size; i < slots_; i++)
        {
            *(destination.pointer() + matrix_reps_index_map_[i]) = 0;
        }

        // Transform destination using inverse of negacyclic NTT
        // Note: We already performed bit-reversal when reading in the matrix
        inverse_ntt_negacyclic_harvey(destination.pointer(), ntt_tables_);
    }

    void PolyCRTBuilder::compose(const vector<int64_t> &values_matrix, Plaintext &destination)
    {
        // Validate input parameters
        if (values_matrix.size() > slots_)
        {
            throw logic_error("values_matrix size is too large");
        }

        uint64_t plain_modulus_div_two = mod_.value() >> 1;
#ifdef SEAL_DEBUG
        for (auto v : values_matrix)
        {
            // Validate the i-th input
            if (abs(v) > plain_modulus_div_two)
            {
                throw invalid_argument("input value is larger than plain_modulus");
            }
        }
#endif
        int input_matrix_size = values_matrix.size();

        // Set destination to full size
        destination.resize(slots_);

        // First write the values to destination coefficients. Read 
        // in top row, then bottom row.
        for (int i = 0; i < input_matrix_size; i++)
        {
            *(destination.pointer() + matrix_reps_index_map_[i]) = (values_matrix[i] < 0) ? 
                (mod_.value() + values_matrix[i]) : values_matrix[i];
        }
        for (int i = input_matrix_size; i < slots_; i++)
        {
            *(destination.pointer() + matrix_reps_index_map_[i]) = 0;
        }

        // Transform destination using inverse of negacyclic NTT
        // Note: We already performed bit-reversal when reading in the matrix
        inverse_ntt_negacyclic_harvey(destination.pointer(), ntt_tables_);
    }

    void PolyCRTBuilder::compose(Plaintext &plain, const MemoryPoolHandle &pool)
    {
        int coeff_count = parms_.poly_modulus().coeff_count();

        // Validate input parameters
        if (plain.coeff_count() > coeff_count || 
            (plain.coeff_count() == coeff_count && plain[coeff_count - 1] != 0))
        {
            throw invalid_argument("plain is not valid for encryption parameters");
        }
#ifdef SEAL_DEBUG
        if (plain.significant_coeff_count() >= coeff_count || !are_poly_coefficients_less_than(plain.pointer(),
            plain.coeff_count(), 1, parms_.plain_modulus().pointer(), parms_.plain_modulus().uint64_count()))
        {
            throw invalid_argument("plain is not valid for encryption parameters");
        }
#endif
        if (!pool)
        {
            throw invalid_argument("pool is uninitialized");
        }

        // We need to permute the coefficients of plain. To do this, we allocate 
        // temporary space.
        int input_plain_coeff_count = min(plain.coeff_count(), slots_);
        Pointer temp(allocate_uint(input_plain_coeff_count, pool));
        set_uint_uint(plain.pointer(), input_plain_coeff_count, temp.get());

        // Set plain to full slot count size.
        plain.resize(slots_);

        // First write the values to destination coefficients. Read 
        // in top row, then bottom row.
        for (int i = 0; i < input_plain_coeff_count; i++)
        {
            *(plain.pointer() + matrix_reps_index_map_[i]) = temp[i];
        }
        for (int i = input_plain_coeff_count; i < slots_; i++)
        {
            *(plain.pointer() + matrix_reps_index_map_[i]) = 0;
        }

        // Transform destination using inverse of negacyclic NTT
        // Note: We already performed bit-reversal when reading in the matrix
        inverse_ntt_negacyclic_harvey(plain.pointer(), ntt_tables_);
    }

    void PolyCRTBuilder::decompose(const Plaintext &plain, vector<uint64_t> &destination,
        const MemoryPoolHandle &pool) 
    {
        int coeff_count = parms_.poly_modulus().coeff_count();

        // Validate input parameters
        if (plain.coeff_count() > coeff_count || 
            (plain.coeff_count() == coeff_count && plain[coeff_count - 1] != 0))
        {
            throw invalid_argument("plain is not valid for encryption parameters");
        }
#ifdef SEAL_DEBUG
        if (plain.significant_coeff_count() >= coeff_count || !are_poly_coefficients_less_than(plain.pointer(),
            plain.coeff_count(), 1, parms_.plain_modulus().pointer(), parms_.plain_modulus().uint64_count()))
        {
            throw invalid_argument("plain is not valid for encryption parameters");
        }
#endif
        if (!pool)
        {
            throw invalid_argument("pool is uninitialized");
        }

        // Set destination size
        destination.resize(slots_);

        // Never include the leading zero coefficient (if present)
        int plain_coeff_count = min(plain.coeff_count(), slots_);

        Pointer temp_dest(allocate_uint(slots_, pool));

        // Make a copy of poly
        set_uint_uint(plain.pointer(), plain_coeff_count, temp_dest.get());
        set_zero_uint(slots_ - plain_coeff_count, temp_dest.get() + plain_coeff_count);

        // Transform destination using negacyclic NTT.
        ntt_negacyclic_harvey(temp_dest.get(), ntt_tables_);

        // Read top row
        for (int i = 0; i < slots_; i++)
        {
            destination[i] = temp_dest[matrix_reps_index_map_[i]];
        }
    }

    void PolyCRTBuilder::decompose(const Plaintext &plain, vector<int64_t> &destination,
        const MemoryPoolHandle &pool)
    {
        int coeff_count = parms_.poly_modulus().coeff_count();

        // Validate input parameters
        if (plain.coeff_count() > coeff_count || 
            (plain.coeff_count() == coeff_count && plain[coeff_count - 1] != 0))
        {
            throw invalid_argument("plain is not valid for encryption parameters");
        }
#ifdef SEAL_DEBUG
        if (plain.significant_coeff_count() >= coeff_count || !are_poly_coefficients_less_than(plain.pointer(),
            plain.coeff_count(), 1, parms_.plain_modulus().pointer(), parms_.plain_modulus().uint64_count()))
        {
            throw invalid_argument("plain is not valid for encryption parameters");
        }
#endif
        if (!pool)
        {
            throw invalid_argument("pool is uninitialized");
        }

        // Set destination size
        destination.resize(slots_);

        // Never include the leading zero coefficient (if present)
        int plain_coeff_count = min(plain.coeff_count(), slots_);

        Pointer temp_dest(allocate_uint(slots_, pool));

        // Make a copy of poly
        set_uint_uint(plain.pointer(), plain_coeff_count, temp_dest.get());
        set_zero_uint(slots_ - plain_coeff_count, temp_dest.get() + plain_coeff_count);

        // Transform destination using negacyclic NTT.
        ntt_negacyclic_harvey(temp_dest.get(), ntt_tables_);

        // Read top row, then bottom row
        uint64_t plain_modulus_div_two = mod_.value() >> 1;
        for (int i = 0; i < slots_; i++)
        {
            int64_t curr_value = temp_dest[matrix_reps_index_map_[i]];
            destination[i] = (curr_value > plain_modulus_div_two) ?
                (curr_value - mod_.value()) : curr_value;
        }
    }

    void PolyCRTBuilder::decompose(Plaintext &plain, const MemoryPoolHandle &pool)
    {
        int coeff_count = parms_.poly_modulus().coeff_count();

        // Validate input parameters
        if (plain.coeff_count() > coeff_count || 
            (plain.coeff_count() == coeff_count && plain[coeff_count - 1] != 0))
        {
            throw invalid_argument("plain is not valid for encryption parameters");
        }
#ifdef SEAL_DEBUG
        if (plain.significant_coeff_count() >= coeff_count || !are_poly_coefficients_less_than(plain.pointer(),
            plain.coeff_count(), 1, parms_.plain_modulus().pointer(), parms_.plain_modulus().uint64_count()))
        {
            throw invalid_argument("plain is not valid for encryption parameters");
        }
#endif
        if (!pool)
        {
            throw invalid_argument("pool is uninitialized");
        }

        // Never include the leading zero coefficient (if present)
        int plain_coeff_count = min(plain.coeff_count(), slots_);

        // Allocate temporary space to store a wide copy of plain
        Pointer temp(allocate_uint(slots_, pool));

        // Make a copy of poly
        set_uint_uint(plain.pointer(), plain_coeff_count, temp.get());
        set_zero_uint(slots_ - plain_coeff_count, temp.get() + plain_coeff_count);

        // Transform destination using negacyclic NTT.
        ntt_negacyclic_harvey(temp.get(), ntt_tables_);

        // Set plain to full slot count size (note that all new coefficients are 
        // set to zero).
        plain.resize(slots_);

        // Read top row, then bottom row
        for (int i = 0; i < slots_; i++)
        {
            *(plain.pointer() + i) = temp[matrix_reps_index_map_[i]];
        }
    }
}