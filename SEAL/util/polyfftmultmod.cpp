#include "util/uintcore.h"
#include "util/uintarith.h"
#include "util/uintarithmod.h"
#include "util/polycore.h"
#include "util/polyfftmult.h"
#include "util/ntt.h"
#include "util/polyarithmod.h"
#include "util/polyarith.h"

using namespace std;

namespace seal
{
    namespace util
    {
        // Compute the multiplication of two polynomials modulo x^n + 1 (and modulo q)
        // using the Nussbaumer algorithm. The modulo q step is done after the multiplication.
        void nussbaumer_multiply_poly_poly_coeffmod(const uint64_t *operand1, const uint64_t *operand2, int coeff_count_power, const Modulus &modulus, uint64_t *result, MemoryPool &pool)
        { 
            int coeff_count = 1 << coeff_count_power; 
            int coeff_uint64_count = modulus.uint64_count(); 
            int coeff_bit_count = modulus.significant_bit_count(); 
            int product_coeff_uint64_count = divide_round_up(2 * coeff_bit_count + coeff_count_power + 1, bits_per_uint64); 
            int sum_bit_count = 1 + coeff_bit_count + coeff_count_power;
            int sum_uint64_count = divide_round_up(sum_bit_count, bits_per_uint64); 

            Pointer intermediate(allocate_poly(coeff_count, product_coeff_uint64_count, pool));
            nussbaumer_multiply_poly_poly(operand1, operand2, coeff_count_power, coeff_uint64_count, sum_uint64_count, product_coeff_uint64_count, intermediate.get(), pool);

            Pointer big_alloc(allocate_uint(3 * product_coeff_uint64_count, pool));
            Pointer temp(allocate_uint(product_coeff_uint64_count, pool));

            // We need to deal with the negative coefficients. 
            uint64_t *poly_coeff = intermediate.get(); 
            uint64_t* result_coeff = result;

            for (int i = 0; i < coeff_count; ++i)
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
                modulo_uint(temp.get(), product_coeff_uint64_count, modulus, result_coeff, pool, big_alloc.get());
                if (coeff_is_negative)
                {
                    negate_uint_mod(result_coeff, modulus.get(), coeff_uint64_count, result_coeff);
                }

                poly_coeff += product_coeff_uint64_count;
                result_coeff += coeff_uint64_count;
            }
        }

        void ntt_multiply_poly_poly(const uint64_t *operand1, const uint64_t *operand2, const NTTTables &tables, uint64_t *result, MemoryPool &pool)
        {
            Pointer copy_operand1(allocate_poly(tables.coeff_count(), tables.coeff_uint64_count(), pool)); 
            set_poly_poly(operand1, tables.coeff_count(), tables.coeff_uint64_count(), copy_operand1.get()); 
            Pointer copy_operand2(allocate_poly(tables.coeff_count(), tables.coeff_uint64_count(), pool));
            set_poly_poly(operand2, tables.coeff_count(), tables.coeff_uint64_count(), copy_operand2.get());
            ntt_negacyclic_harvey(copy_operand1.get(), tables, pool); 
            ntt_negacyclic_harvey(copy_operand2.get(), tables, pool); 
            dyadic_product_coeffmod(copy_operand1.get(), copy_operand2.get(), tables.coeff_count(), tables.modulus(), result, pool); 
            inverse_ntt_negacyclic_harvey(result, tables, pool); 
        }

        /*
        Performs NTT multiply assuming one of the operands (operand2) is already transformed to NTT domain. 
        */
        void ntt_multiply_poly_nttpoly(const uint64_t *operand1, const uint64_t *operand2, const NTTTables &tables, uint64_t *result, MemoryPool &pool)
        {
            Pointer copy_operand1(allocate_poly(tables.coeff_count(), tables.coeff_uint64_count(), pool));
            set_poly_poly(operand1, tables.coeff_count(), tables.coeff_uint64_count(), copy_operand1.get());
            ntt_negacyclic_harvey(copy_operand1.get(), tables, pool);
            dyadic_product_coeffmod(copy_operand1.get(), operand2, tables.coeff_count(), tables.modulus(), result, pool);
            inverse_ntt_negacyclic_harvey(result, tables, pool);
        }

        /*
        Perform NTT multiply (a*b, a*c) when b, c are already in NTT domain. 
        */
        void ntt_double_multiply_poly_nttpoly(const uint64_t *operand1, const uint64_t *operand2, const uint64_t *operand3,  const NTTTables &tables, uint64_t *result1, uint64_t *result2,  MemoryPool &pool)
        {
            Pointer copy_operand1(allocate_poly(tables.coeff_count(), tables.coeff_uint64_count(), pool));
            set_poly_poly(operand1, tables.coeff_count(), tables.coeff_uint64_count(), copy_operand1.get());
            ntt_negacyclic_harvey(copy_operand1.get(), tables, pool);
            dyadic_product_coeffmod(copy_operand1.get(), operand2, tables.coeff_count(), tables.modulus(), result1, pool);
            dyadic_product_coeffmod(copy_operand1.get(), operand3, tables.coeff_count(), tables.modulus(), result2, pool);
            inverse_ntt_negacyclic_harvey(result1, tables, pool);
            inverse_ntt_negacyclic_harvey(result2, tables, pool);
        }

        // Perform the dot product of two bigpolyarrays array1 and array2, assuming array2 is already transformed into NTT form
        void ntt_dot_product_bigpolyarray_nttbigpolyarray(const uint64_t *array1, const uint64_t *array2, int count, int array_poly_uint64_count, const NTTTables &tables, uint64_t *result, MemoryPool &pool)
        {
#ifdef _DEBUG
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
            if (array_poly_uint64_count < 1)
            {
                throw invalid_argument("array_poly_uint64_count");
            }
            if (!tables.is_generated())
            {
                throw invalid_argument("tables");
            }
#endif
            int coeff_count = tables.coeff_count();
            int coeff_bit_count = tables.modulus().significant_bit_count();
            int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

            // Initialize pointers for multiplication
            const uint64_t *current_array1 = array1;
            const uint64_t *current_array2 = array2;

            // Create copies
            Pointer temp(allocate_poly(coeff_count, coeff_uint64_count, pool));
            Pointer copy_operand1(allocate_poly(coeff_count, coeff_uint64_count, pool));
            set_zero_poly(coeff_count, coeff_uint64_count, result);

            for (int i = 0; i < count; ++i)
            {
                // Perform the dyadic product. 
                set_poly_poly(current_array1, coeff_count, coeff_uint64_count, copy_operand1.get());
                ntt_negacyclic_harvey(copy_operand1.get(), tables, pool);
                dyadic_product_coeffmod(copy_operand1.get(), current_array2, coeff_count, tables.modulus(), temp.get(), pool);
                add_poly_poly_coeffmod(result, temp.get(), coeff_count, tables.modulus().get(), coeff_uint64_count, result);
                
                current_array1 += array_poly_uint64_count;
                current_array2 += array_poly_uint64_count;
            }

            // Perform inverse NTT
            inverse_ntt_negacyclic_harvey(result, tables, pool);
        }

        // Perform two dot products of bigpoly arrays <array1 , array2> and <array1 , array3>, assuming array2 and array3 are already transformed into NTT form
        void ntt_double_dot_product_bigpolyarray_nttbigpolyarrays(const uint64_t *array1, const uint64_t *array2, const uint64_t *array3, int count, int array_poly_uint64_count, const NTTTables &tables, uint64_t *result1, uint64_t *result2, MemoryPool &pool)
        {
#ifdef _DEBUG
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
            if (array_poly_uint64_count < 1)
            {
                throw invalid_argument("array_poly_uint64_count");
            }
            if (!tables.is_generated())
            {
                throw invalid_argument("tables");
            }
#endif
            int coeff_count = tables.coeff_count();
            int coeff_bit_count = tables.modulus().significant_bit_count();
            int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

            // Initialize pointers for multiplication
            const uint64_t *current_array1 = array1;
            const uint64_t *current_array2 = array2;
            const uint64_t *current_array3 = array3;

            // Create copies
            Pointer temp(allocate_poly(coeff_count, coeff_uint64_count, pool));
            Pointer copy_operand1(allocate_poly(coeff_count, coeff_uint64_count, pool));
            set_zero_poly(coeff_count, coeff_uint64_count, result1);
            set_zero_poly(coeff_count, coeff_uint64_count, result2);

            for (int i = 0; i < count; ++i)
            {
                // Perform the dyadic product. 
                set_poly_poly(current_array1, coeff_count, coeff_uint64_count, copy_operand1.get());
                ntt_negacyclic_harvey(copy_operand1.get(), tables, pool);
                dyadic_product_coeffmod(copy_operand1.get(), current_array2, coeff_count, tables.modulus(), temp.get(), pool);
                add_poly_poly_coeffmod(result1, temp.get(), coeff_count, tables.modulus().get(), coeff_uint64_count, result1);
                dyadic_product_coeffmod(copy_operand1.get(), current_array3, coeff_count, tables.modulus(), temp.get(), pool);
                add_poly_poly_coeffmod(result2, temp.get(), coeff_count, tables.modulus().get(), coeff_uint64_count, result2);
                current_array1 += array_poly_uint64_count;
                current_array2 += array_poly_uint64_count;
                current_array3 += array_poly_uint64_count;
            }

            // Perform inverse NTT
            inverse_ntt_negacyclic_harvey(result1, tables, pool);
            inverse_ntt_negacyclic_harvey(result2, tables, pool);
        }

        void nussbaumer_dot_product_bigpolyarray_coeffmod(const uint64_t *array1, const uint64_t *array2, int count,
            const PolyModulus &poly_modulus, const Modulus &modulus, uint64_t *result, MemoryPool &pool)
        {
            // Check validity of inputs
#ifdef _DEBUG
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
            int coeff_bit_count = modulus.significant_bit_count();
            int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
            int poly_ptr_increment = coeff_count * coeff_uint64_count;

            set_zero_poly(coeff_count, coeff_uint64_count, result);

            // initialize pointers for multiplication
            const uint64_t *current_array1 = array1;
            const uint64_t *current_array2 = array2;

            Pointer temp(allocate_poly(coeff_count, coeff_uint64_count, pool));
            for (int i = 0; i < count; ++i)
            {
                nussbaumer_multiply_poly_poly_coeffmod(current_array1, current_array2, poly_modulus.coeff_count_power_of_two(), modulus, temp.get(), pool);
                add_poly_poly_coeffmod(result, temp.get(), coeff_count, modulus.get(), coeff_uint64_count, result);
                current_array1 += poly_ptr_increment;
                current_array2 += poly_ptr_increment;
            }
        }
    }
}
