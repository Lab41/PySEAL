#include "seal/util/uintcore.h"
#include "seal/util/uintarith.h"
#include "seal/util/uintarithsmallmod.h"
#include "seal/util/polycore.h"
#include "seal/util/nussbaumer.h"
#include "seal/util/smallntt.h"
#include "seal/util/polyarithsmallmod.h"
#include <string>

using namespace std;

namespace seal
{
    namespace util
    {
        void ntt_multiply_poly_poly(const uint64_t *operand1, const uint64_t *operand2, const SmallNTTTables &tables, uint64_t *result, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr)
            {
                throw invalid_argument("operand1");
            }
            if (operand2 == nullptr)
            {
                throw invalid_argument("operand2");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (!tables.is_generated())
            {
                throw invalid_argument("tables");
            }
#endif
            int coeff_count = tables.coeff_count() + 1;
            Pointer copy_operand1(allocate_uint(coeff_count, pool));
            set_uint_uint(operand1, coeff_count, copy_operand1.get());
            Pointer copy_operand2(allocate_uint(coeff_count, pool));
            set_uint_uint(operand2, coeff_count, copy_operand2.get());

            // Lazy reduction
            ntt_negacyclic_harvey(copy_operand1.get(), tables);
            ntt_negacyclic_harvey(copy_operand2.get(), tables);

            dyadic_product_coeffmod(copy_operand1.get(), copy_operand2.get(), coeff_count, tables.modulus(), result);
            inverse_ntt_negacyclic_harvey(result, tables);
        }

        /*
        Performs NTT multiply assuming one of the operands (operand2) is already transformed to NTT domain.
        */
        void ntt_multiply_poly_nttpoly(const uint64_t *operand1, const uint64_t *operand2, const SmallNTTTables &tables, uint64_t *result, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr)
            {
                throw invalid_argument("operand1");
            }
            if (operand2 == nullptr)
            {
                throw invalid_argument("operand2");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (!tables.is_generated())
            {
                throw invalid_argument("tables");
            }
#endif
            int coeff_count = tables.coeff_count() + 1;
            Pointer copy_operand1(allocate_uint(coeff_count, pool));
            set_uint_uint(operand1, coeff_count, copy_operand1.get());

            // Lazy reduction
            ntt_negacyclic_harvey_lazy(copy_operand1.get(), tables);
            
            dyadic_product_coeffmod(copy_operand1.get(), operand2, coeff_count, tables.modulus(), result);
            inverse_ntt_negacyclic_harvey(result, tables);
        }

        /*
        Perform NTT multiply (a*b, a*c) when b, c are already in NTT domain.
        */
        void ntt_double_multiply_poly_nttpoly(const uint64_t *operand1, const uint64_t *operand2, const uint64_t *operand3, const SmallNTTTables &tables, uint64_t *result1, uint64_t *result2, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr)
            {
                throw invalid_argument("operand1");
            }
            if (operand2 == nullptr)
            {
                throw invalid_argument("operand2");
            }
            if (operand3 == nullptr)
            {
                throw invalid_argument("operand3");
            }
            if (result1 == nullptr)
            {
                throw invalid_argument("result1");
            }
            if (result2 == nullptr)
            {
                throw invalid_argument("result2");
            }
            if (!tables.is_generated())
            {
                throw invalid_argument("tables");
            }
#endif
            int coeff_count = tables.coeff_count() + 1;
            Pointer copy_operand1(allocate_uint(coeff_count, pool));
            set_uint_uint(operand1, coeff_count, copy_operand1.get());

            // Lazy reduction
            ntt_negacyclic_harvey_lazy(copy_operand1.get(), tables);
            dyadic_product_coeffmod(copy_operand1.get(), operand2, coeff_count, tables.modulus(), result1);

            inverse_ntt_negacyclic_harvey(result1, tables);
            dyadic_product_coeffmod(copy_operand1.get(), operand3, coeff_count, tables.modulus(), result2);

            inverse_ntt_negacyclic_harvey(result2, tables);
        }

        // Perform the dot product of two bigpolyarrays array1 and array2, assuming array2 is already transformed into NTT form
        void ntt_dot_product_bigpolyarray_nttbigpolyarray(const uint64_t *array1, const uint64_t *array2, int count, const SmallNTTTables &tables, uint64_t *result, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
            if (array1 == nullptr)
            {
                throw invalid_argument("array1");
            }
            if (array2 == nullptr)
            {
                throw invalid_argument("array2");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (count < 1)
            {
                throw invalid_argument("count");
            }
            if (!tables.is_generated())
            {
                throw invalid_argument("tables");
            }
#endif
            int coeff_count = tables.coeff_count() + 1;
            set_zero_uint(coeff_count, result);

            // Initialize pointers for multiplication
            const uint64_t *current_array1 = array1;
            const uint64_t *current_array2 = array2;

            // Create copy
            Pointer copy_operand1(allocate_uint(coeff_count, pool));

            for (int i = 0; i < count; i++)
            {
                // Perform the dyadic product. 
                set_uint_uint(current_array1, coeff_count, copy_operand1.get());

                // Lazy reduction
                ntt_negacyclic_harvey_lazy(copy_operand1.get(), tables);

                dyadic_product_coeffmod(copy_operand1.get(), current_array2, coeff_count, tables.modulus(), copy_operand1.get());
                add_poly_poly_coeffmod(result, copy_operand1.get(), coeff_count, tables.modulus(), result);

                current_array1 += coeff_count;
                current_array2 += coeff_count;
            }

            // Perform inverse NTT
            inverse_ntt_negacyclic_harvey(result, tables);
        }

        // Perform two dot products of bigpoly arrays <array1 , array2> and <array1 , array3>, assuming array2 and array3 are already transformed into NTT form
        void ntt_double_dot_product_bigpolyarray_nttbigpolyarrays(const uint64_t *array1, const uint64_t *array2, const uint64_t *array3, int count, const SmallNTTTables &tables, uint64_t *result1, uint64_t *result2, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
            if (array1 == nullptr)
            {
                throw invalid_argument("array1");
            }
            if (array2 == nullptr)
            {
                throw invalid_argument("array2");
            }
            if (array3 == nullptr)
            {
                throw invalid_argument("array3");
            }
            if (result1 == nullptr)
            {
                throw invalid_argument("result1");
            }
            if (result2 == nullptr)
            {
                throw invalid_argument("result2");
            }
            if (count < 1)
            {
                throw invalid_argument("count");
            }
            if (!tables.is_generated())
            {
                throw invalid_argument("tables");
            }
#endif
            int coeff_count = tables.coeff_count() + 1;
            set_zero_uint(coeff_count, result1);
            set_zero_uint(coeff_count, result2);

            // Initialize pointers for multiplication
            const uint64_t *current_array1 = array1;
            const uint64_t *current_array2 = array2;
            const uint64_t *current_array3 = array3;

            // Create copy
            Pointer copy_operand1(allocate_uint(coeff_count, pool));

            for (int i = 0; i < count; i++)
            {
                // Perform the dyadic product. 
                set_uint_uint(current_array1, coeff_count, copy_operand1.get());

                // Lazy reduction
                ntt_negacyclic_harvey_lazy(copy_operand1.get(), tables);

                dyadic_product_coeffmod(copy_operand1.get(), current_array2, coeff_count, tables.modulus(), copy_operand1.get());
                add_poly_poly_coeffmod(result1, copy_operand1.get(), coeff_count, tables.modulus(), result1);
                dyadic_product_coeffmod(copy_operand1.get(), current_array3, coeff_count, tables.modulus(), copy_operand1.get());
                add_poly_poly_coeffmod(result2, copy_operand1.get(), coeff_count, tables.modulus(), result2);
                current_array1 += coeff_count;
                current_array2 += coeff_count;
                current_array3 += coeff_count;
            }

            // Perform inverse NTT
            inverse_ntt_negacyclic_harvey(result1, tables);
            inverse_ntt_negacyclic_harvey(result2, tables);
        }

        // Compute the multiplication of two polynomials modulo x^n + 1 (and modulo q)
        // using the Nussbaumer algorithm. The modulo q step is done after the multiplication.
        void nussbaumer_multiply_poly_poly_coeffmod(const uint64_t *operand1, const uint64_t *operand2, int coeff_count_power, const SmallModulus &modulus, uint64_t *result, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr)
            {
                throw invalid_argument("operand1");
            }
            if (operand2 == nullptr)
            {
                throw invalid_argument("operand2");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (modulus.is_zero())
            {
                throw invalid_argument("modulus");
            }
            if (coeff_count_power < 0)
            {
                throw invalid_argument("coeff_count_power");
            }
#endif
            int coeff_count = 1 << coeff_count_power;
            int product_coeff_uint64_count = divide_round_up(2 * modulus.bit_count() + coeff_count_power + 1, bits_per_uint64);
            int sum_uint64_count = divide_round_up(modulus.bit_count() + coeff_count_power + 1, bits_per_uint64);

            Pointer intermediate(allocate_poly(coeff_count, product_coeff_uint64_count, pool));
            nussbaumer_multiply_poly_poly(operand1, operand2, coeff_count_power, 1, sum_uint64_count, product_coeff_uint64_count, intermediate.get(), pool);

            Pointer temp(allocate_uint(product_coeff_uint64_count, pool));

            // We need to deal with the negative coefficients. 
            uint64_t *poly_coeff = intermediate.get();
            uint64_t *result_coeff = result;

            for (int i = 0; i < coeff_count; i++)
            {
                bool coeff_is_negative = is_high_bit_set_uint(poly_coeff, product_coeff_uint64_count);
                if (coeff_is_negative)
                {
                    negate_uint(poly_coeff, product_coeff_uint64_count, temp.get());
                }
                else
                {
                    set_uint_uint(poly_coeff, product_coeff_uint64_count, temp.get());
                }

                // Perform the modular reduction and reduce size. 
                *result_coeff = modulo_uint(temp.get(), product_coeff_uint64_count, modulus, pool);
                if (coeff_is_negative)
                {
                    *result_coeff = negate_uint_mod(*result_coeff, modulus);
                }

                poly_coeff += product_coeff_uint64_count;
                result_coeff++;
            }
        }

        void nussbaumer_dot_product_bigpolyarray_coeffmod(const uint64_t *array1, const uint64_t *array2, int count,
            const PolyModulus &poly_modulus, const SmallModulus &modulus, uint64_t *result, MemoryPool &pool)
        {
            // Check validity of inputs
#ifdef SEAL_DEBUG
            if (array1 == nullptr)
            {
                throw invalid_argument("array1");
            }
            if (array2 == nullptr)
            {
                throw invalid_argument("array2");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (count < 1)
            {
                throw invalid_argument("count");
            }
#endif
            // Calculate pointer increment
            int coeff_count = poly_modulus.coeff_count();
            set_zero_uint(coeff_count, result);

            // initialize pointers for multiplication
            const uint64_t *current_array1 = array1;
            const uint64_t *current_array2 = array2;

            Pointer temp(allocate_uint(coeff_count, pool));
            for (int i = 0; i < count; i++)
            {
                nussbaumer_multiply_poly_poly_coeffmod(current_array1, current_array2, poly_modulus.coeff_count_power_of_two(), modulus, temp.get(), pool);
                add_poly_poly_coeffmod(result, temp.get(), coeff_count, modulus, result);
                current_array1 += coeff_count;
                current_array2 += coeff_count;
            }
        }
    }
}
