#include "util/uintcore.h"
#include "util/polycore.h"
#include "util/uintarith.h"
#include <algorithm>

using namespace std;

namespace seal
{
    namespace util
    {
        namespace
        {
            namespace tools
            {
                void multiply_uint_uint_add(const uint64_t *operand1, const uint64_t *operand2, int operand_uint64_count, int result_uint64_count, uint64_t *result)
                {
                    // Handle fast cases.
                    if (operand_uint64_count == 0)
                    {
                        return;
                    }
                    if (result_uint64_count == 1)
                    {
                        *result += *operand1 * *operand2;
                        return;
                    }

                    // In some cases these improve performance.
                    //int operand1_uint64_count = get_significant_uint64_count_uint(operand1, operand_uint64_count);
                    //int operand2_uint64_count = get_significant_uint64_count_uint(operand2, operand_uint64_count);

                    // Multiply operand1 and operand2.            
                    uint64_t temp_result;
                    uint64_t carry, new_carry, sum1_carry, sum2_carry;
                    const uint64_t *inner_operand2;
                    uint64_t *inner_result;

                    int operand1_index_max = min(operand_uint64_count, result_uint64_count);
                    for (int operand1_index = 0; operand1_index < operand1_index_max; operand1_index++)
                    {
                        inner_operand2 = operand2;
                        inner_result = result++;
                        carry = 0;
                        int operand2_index = 0;
                        int operand2_index_max = min(operand_uint64_count, result_uint64_count - operand1_index);
                        for (; operand2_index < operand2_index_max; operand2_index++)
                        {
                            // Perform 64-bit multiplication of operand1 and operand2
                            temp_result = multiply_uint64_uint64(*operand1, *inner_operand2++, &new_carry);
                            sum1_carry = add_uint64_uint64(temp_result, carry, 0, &temp_result);
                            sum2_carry = add_uint64_uint64(*inner_result, temp_result, 0, inner_result);
                            carry = new_carry + sum1_carry + sum2_carry;

                            inner_result++;
                        }

                        // Write carry if there is room in result
                        if (operand1_index + operand2_index < result_uint64_count)
                        {
                            unsigned char c_carry = add_uint64_uint64(*inner_result, carry, 0, inner_result);
                            for (int i = operand1_index + operand2_index + 1; i < result_uint64_count; i++)
                            {
                                inner_result++;
                                c_carry = add_uint64_uint64(*inner_result, 0, c_carry, inner_result);
                            }
                        }

                        operand1++;
                    }
                }

                void triple_multiply_uint_uint_add(
                    const uint64_t *operand11, const uint64_t *operand12, 
                    const uint64_t *operand21, const uint64_t *operand22, 
                    const uint64_t *operand31, const uint64_t *operand32, 
                    int operand_uint64_count, int result_uint64_count, 
                    uint64_t *result1, uint64_t *result2, uint64_t *result3)
                {
                    // Handle fast cases.
                    if (operand_uint64_count == 0)
                    {
                        return;
                    }
                    if (result_uint64_count == 1)
                    {
                        *result1 += *operand11 * *operand12;
                        *result2 += *operand21 * *operand22;
                        *result3 += *operand31 * *operand32;
                        return;
                    }

                    // Multiply operand11 and operand12, operand21 and operand22, operand31 and operand32.            
                    uint64_t temp_result1, temp_result2, temp_result3;
                    uint64_t carry1, carry2, carry3;
                    uint64_t new_carry1, new_carry2, new_carry3;
                    uint64_t sum1_carry1, sum2_carry1, sum1_carry2, sum2_carry2, sum1_carry3, sum2_carry3;
                    const uint64_t *inner_operand12, *inner_operand22, *inner_operand32;
                    uint64_t *inner_result1, *inner_result2, *inner_result3;

                    int operand1_index_max = min(operand_uint64_count, result_uint64_count);
                    for (int operand1_index = 0; operand1_index < operand1_index_max; operand1_index++)
                    {
                        inner_operand12 = operand12;
                        inner_operand22 = operand22;
                        inner_operand32 = operand32;

                        inner_result1 = result1++;
                        inner_result2 = result2++;
                        inner_result3 = result3++;

                        carry1 = carry2 = carry3 = 0;
                        int operand2_index = 0;
                        int operand2_index_max = min(operand_uint64_count, result_uint64_count - operand1_index);
                        for (; operand2_index < operand2_index_max; operand2_index++)
                        {
                            // Perform 64-bit multiplication of operand1 and operand2
                            temp_result1 = multiply_uint64_uint64(*operand11, *inner_operand12++, &new_carry1);
                            sum1_carry1 = add_uint64_uint64(temp_result1, carry1, 0, &temp_result1);
                            sum2_carry1 = add_uint64_uint64(*inner_result1, temp_result1, 0, inner_result1);
                            carry1 = new_carry1 + sum1_carry1 + sum2_carry1;

                            temp_result2 = multiply_uint64_uint64(*operand21, *inner_operand22++, &new_carry2);
                            sum1_carry2 = add_uint64_uint64(temp_result2, carry2, 0, &temp_result2);
                            sum2_carry2 = add_uint64_uint64(*inner_result2, temp_result2, 0, inner_result2);
                            carry2 = new_carry2 + sum1_carry2 + sum2_carry2;

                            temp_result3 = multiply_uint64_uint64(*operand31, *inner_operand32++, &new_carry3);
                            sum1_carry3 = add_uint64_uint64(temp_result3, carry3, 0, &temp_result3);
                            sum2_carry3 = add_uint64_uint64(*inner_result3, temp_result3, 0, inner_result3);
                            carry3 = new_carry3 + sum1_carry3 + sum2_carry3;

                            inner_result1++;
                            inner_result2++;
                            inner_result3++;
                        }

                        // Write carry if there is room in result
                        if (operand1_index + operand2_index < result_uint64_count)
                        {
                            unsigned char c_carry1 = add_uint64_uint64(*inner_result1, carry1, 0, inner_result1);
                            unsigned char c_carry2 = add_uint64_uint64(*inner_result2, carry2, 0, inner_result2);
                            unsigned char c_carry3 = add_uint64_uint64(*inner_result3, carry3, 0, inner_result3);

                            for (int i = operand1_index + operand2_index + 1; i < result_uint64_count; i++)
                            {
                                inner_result1++;
                                inner_result2++;
                                inner_result3++;

                                c_carry1 = add_uint64_uint64(*inner_result1, 0, c_carry1, inner_result1);
                                c_carry2 = add_uint64_uint64(*inner_result2, 0, c_carry2, inner_result2);
                                c_carry3 = add_uint64_uint64(*inner_result3, 0, c_carry3, inner_result3);
                            }
                        }

                        operand11++;
                        operand21++;
                        operand31++;
                    }
                }

                inline void multiply_uint_uint_add_sub_if(const uint64_t *operand1, const uint64_t *operand2, int operand_uint64_count, uint64_t subtract, int result_uint64_count, uint64_t *result)
                {
                    if (subtract)
                    {
                        uint64_t *temp_ptr = result;
                        for (int k = 0; k < result_uint64_count; k++)
                        {
                            *temp_ptr++ ^= 0xFFFFFFFFFFFFFFFF;
                        }
                        tools::multiply_uint_uint_add(operand1, operand2, operand_uint64_count, result_uint64_count, result);
                        for (int k = 0; k < result_uint64_count; k++)
                        {
                            *result++ ^= 0xFFFFFFFFFFFFFFFF;
                        }
                    }
                    else
                    {
                        tools::multiply_uint_uint_add(operand1, operand2, operand_uint64_count, result_uint64_count, result);
                    }
                }

                inline void set_uint_uint_negate_if(const uint64_t *operand, int uint64_count, uint64_t negate, uint64_t *destination)
                {
                    // negate is supposed to be either 0 or 1
                    uint64_t negate_mask = ~(negate - 1);
                    for (int k = 0; k < uint64_count; k++)
                    {
                        negate = add_uint64_uint64(*operand++ ^ negate_mask, 0, static_cast<unsigned char>(negate), destination++);
                    }
                }

                inline void set_uint_uint_negate_if(const uint64_t *operand1, const uint64_t *operand2, int uint64_count, uint64_t negate1, uint64_t negate2, uint64_t *destination1, uint64_t *destination2)
                {
                    // negate is supposed to be either 0 or 1
                    uint64_t negate_mask1 = ~(negate1 - 1);
                    uint64_t negate_mask2 = ~(negate2 - 1);
                    for (int k = 0; k < uint64_count; k++)
                    {
                        negate1 = add_uint64_uint64(*operand1++ ^ negate_mask1, 0, static_cast<unsigned char>(negate1), destination1++);
                        negate2 = add_uint64_uint64(*operand2++ ^ negate_mask2, 0, static_cast<unsigned char>(negate2), destination2++);
                    }
                }
            }

            const int use_base_case = 8;

            namespace nussbaumer_multiply
            {
                void do_nussbaumer_base_signcheck(const uint64_t *operand1, const uint64_t *operand2, int coeff_count_power, int sum_uint64_count, int product_uint64_count, uint64_t *result, uint64_t *alloc_ptr)
                {
                    // alloc_ptr should point to 2 * sum_uint64_count memory
                    uint64_t *first_operand = alloc_ptr;
                    uint64_t *second_operand = first_operand + sum_uint64_count;

                    int coeff_count = 1 << coeff_count_power;
                    set_zero_poly(coeff_count, product_uint64_count, result);

                    const uint64_t *operand1_coeff_ptr = operand1;
                    for (int i = 0; i < coeff_count; i++)
                    {
                        uint64_t is_first_operand_negative = operand1_coeff_ptr[sum_uint64_count - 1] >> (bits_per_uint64 - 1);                        
                        tools::set_uint_uint_negate_if(operand1_coeff_ptr, sum_uint64_count, is_first_operand_negative, first_operand);
                        operand1_coeff_ptr += sum_uint64_count;

                        const uint64_t *operand2_coeff_ptr = operand2;
                        for (int j = 0; j < coeff_count; j++)
                        {
                            int loc = i + j;
                            if (loc >= coeff_count)
                            {
                                loc -= coeff_count;
                            }

                            uint64_t is_second_operand_negative = operand2_coeff_ptr[sum_uint64_count - 1] >> (bits_per_uint64 - 1);
                            tools::set_uint_uint_negate_if(operand2_coeff_ptr, sum_uint64_count, is_second_operand_negative, second_operand);
                            operand2_coeff_ptr += sum_uint64_count;

                            uint64_t *result_ptr = get_poly_coeff(result, loc, product_uint64_count);

                            // branch according to sign
                            //tools::multiply_uint_uint_add_sub_if(first_operand, second_operand, sum_uint64_count, 
                            //    is_first_operand_negative ^ is_second_operand_negative ^ static_cast<uint64_t>(loc < i),
                            //    product_uint64_count, result_ptr);
                            if (is_first_operand_negative ^ is_second_operand_negative ^ static_cast<uint64_t>(loc < i))
                            {
                                uint64_t *temp_ptr = result_ptr;
                                for (int k = 0; k < product_uint64_count; k++)
                                {
                                    *temp_ptr++ ^= 0xFFFFFFFFFFFFFFFF;
                                }
                                tools::multiply_uint_uint_add(first_operand, second_operand, sum_uint64_count, product_uint64_count, result_ptr);
                                for (int k = 0; k < product_uint64_count; k++)
                                {
                                    *result_ptr++ ^= 0xFFFFFFFFFFFFFFFF;
                                }
                            }
                            else
                            {
                                tools::multiply_uint_uint_add(first_operand, second_operand, sum_uint64_count, product_uint64_count, result_ptr);
                            }
                        }
                    }
                }

                void do_nussbaumer(const uint64_t *operand1, const uint64_t *operand2, int coeff_count_power, int sum_uint64_count, int product_uint64_count, uint64_t *result, MemoryPool &pool, uint64_t *fft_base_alloc_ptr)
                {
                    // fft_base_alloc_ptr should point to 2 * sum_uint64_count memory

                    // Handle base case differently.
                    int coeff_count = 1 << coeff_count_power;
                    if (coeff_count <= use_base_case)
                    {
                        do_nussbaumer_base_signcheck(operand1, operand2, coeff_count_power, sum_uint64_count, product_uint64_count, result, fft_base_alloc_ptr);
                        return;
                    }

                    // Allocate temporary storage for FFT.
                    int m = 1 << (coeff_count_power / 2);
                    int r = 1 << ((coeff_count_power + 1) / 2);

                    // To reduce the number of allocations, we allocate one big block of memory at once.
                    Pointer big_alloc_anchor(allocate_uint(4 * 2 * coeff_count * sum_uint64_count + 2 * 2 * coeff_count * product_uint64_count, pool));

                    uint64_t *x = big_alloc_anchor.get();
                    uint64_t *y = x + 2 * coeff_count * sum_uint64_count;
                    uint64_t *temp_poly = y + 2 * coeff_count * sum_uint64_count;
                    uint64_t *temp_poly2 = temp_poly + 2 * coeff_count * sum_uint64_count;

                    uint64_t *z = temp_poly2 + 2 * coeff_count * sum_uint64_count;
                    uint64_t *temp_big = z + 2 * coeff_count * product_uint64_count;

                    // Populate x and y with butterfly pattern.
                    for (int indexM = 0; indexM < m; indexM++)
                    {
                        for (int indexR = 0; indexR < r; indexR++)
                        {
                            const uint64_t *operand1_coeff = get_poly_coeff(operand1, indexR * m + indexM, sum_uint64_count);
                            set_uint_uint(operand1_coeff, sum_uint64_count, get_poly_coeff(x, indexM * r + indexR, sum_uint64_count));
                            const uint64_t *operand2_coeff = get_poly_coeff(operand2, indexR * m + indexM, sum_uint64_count);
                            set_uint_uint(operand2_coeff, sum_uint64_count, get_poly_coeff(y, indexM * r + indexR, sum_uint64_count));
                        }
                    }
                    set_poly_poly(x, coeff_count, sum_uint64_count, get_poly_coeff(x, coeff_count, sum_uint64_count));
                    set_poly_poly(y, coeff_count, sum_uint64_count, get_poly_coeff(y, coeff_count, sum_uint64_count));

                    // Do first stage of FFT.
                    int outer_start = coeff_count_power / 2 - 1;
                    for (int outer_index = outer_start; outer_index >= 0; outer_index--)
                    {
                        int outer_remaining = coeff_count_power / 2 - outer_index;
                        int middle_end = 1 << outer_remaining;		// middle_end = "m".
                        int inner_end = 1 << outer_index;			// inner_end = t.
                                                                    // In the Cooley-Tukey, we always have the relation 2mt = n. 
                                                                    // So in this code, we have n =  2 * (coeff_count_power / 2) = 2M.
                        for (int middle_index = 0; middle_index < middle_end; middle_index++) // middle_index = i. 
                        {
                            int sr = ((reverse_bits(static_cast<uint32_t>(middle_index)) >> (32 - outer_remaining)) << outer_index); //scrambling.
                            int s = middle_index << (outer_index + 1);	// s = middle_index*2t = j1
                            int k = (r / m) * sr;						// The root of unity. 
                            for (int inner_index = 0; inner_index < inner_end; inner_index++)
                            {
                                // inner_end =  t. As we know t is decreasing. 
                                // inner index is like j?
                                // s -- okay.
                                int i = s + inner_index; // j 
                                int l = i + inner_end; // j + t.

                                // Compute the FFT of x and y using Cooley-Tukey. Everything is done r times because we are working with polynomials. 
                                uint64_t *x_coeff_ptr = get_poly_coeff(x, (l + 1) * r - k, sum_uint64_count);
                                uint64_t *y_coeff_ptr = get_poly_coeff(y, (l + 1) * r - k, sum_uint64_count);
                                uint64_t *temp_poly_coeff_ptr = temp_poly;
                                uint64_t *temp_poly2_coeff_ptr = temp_poly2;

                                for (int a = 0; a < k; a++)
                                {
                                    negate_uint(x_coeff_ptr, sum_uint64_count, temp_poly_coeff_ptr);
                                    negate_uint(y_coeff_ptr, sum_uint64_count, temp_poly2_coeff_ptr);
                                    x_coeff_ptr += sum_uint64_count;
                                    y_coeff_ptr += sum_uint64_count;
                                    temp_poly_coeff_ptr += sum_uint64_count;
                                    temp_poly2_coeff_ptr += sum_uint64_count;
                                }
                                x_coeff_ptr -= r * sum_uint64_count;
                                y_coeff_ptr -= r * sum_uint64_count;

                                set_uint_uint(x_coeff_ptr, (r - k) * sum_uint64_count, temp_poly_coeff_ptr);
                                set_uint_uint(y_coeff_ptr, (r - k) * sum_uint64_count, temp_poly2_coeff_ptr);

                                x_coeff_ptr = get_poly_coeff(x, i * r, sum_uint64_count);
                                y_coeff_ptr = get_poly_coeff(y, i * r, sum_uint64_count);
                                uint64_t *x_coeff_ptr2 = get_poly_coeff(x, l * r, sum_uint64_count);
                                uint64_t *y_coeff_ptr2 = get_poly_coeff(y, l * r, sum_uint64_count);
                                temp_poly_coeff_ptr = temp_poly;
                                temp_poly2_coeff_ptr = temp_poly2;
                                for (int a = 0; a < r; a++)
                                {
                                    sub_uint_uint(x_coeff_ptr, temp_poly_coeff_ptr, sum_uint64_count, x_coeff_ptr2);
                                    add_uint_uint(x_coeff_ptr, temp_poly_coeff_ptr, sum_uint64_count, x_coeff_ptr);
                                    sub_uint_uint(y_coeff_ptr, temp_poly2_coeff_ptr, sum_uint64_count, y_coeff_ptr2);
                                    add_uint_uint(y_coeff_ptr, temp_poly2_coeff_ptr, sum_uint64_count, y_coeff_ptr);
                                    x_coeff_ptr += sum_uint64_count;
                                    y_coeff_ptr += sum_uint64_count;
                                    x_coeff_ptr2 += sum_uint64_count;
                                    y_coeff_ptr2 += sum_uint64_count;
                                    temp_poly_coeff_ptr += sum_uint64_count;
                                    temp_poly2_coeff_ptr += sum_uint64_count;
                                }
                            }
                        }
                    }

                    // Recursively do FFT.
                    int rec_coeff_count_power = (coeff_count_power + 1) / 2;
                    if ((1 << rec_coeff_count_power) > use_base_case)
                    {
                        for (int i = 0; i < 2 * m; i += 2)
                        {
                            do_nussbaumer(get_poly_coeff(x, i * r, sum_uint64_count), get_poly_coeff(y, i * r, sum_uint64_count), rec_coeff_count_power, sum_uint64_count, product_uint64_count, get_poly_coeff(z, i * r, product_uint64_count), pool, fft_base_alloc_ptr);
                            do_nussbaumer(get_poly_coeff(x, (i + 1) * r, sum_uint64_count), get_poly_coeff(y, (i + 1) * r, sum_uint64_count), rec_coeff_count_power, sum_uint64_count, product_uint64_count, get_poly_coeff(z, (i + 1) * r, product_uint64_count), pool, fft_base_alloc_ptr);
                        }
                    }
                    else
                    {
                        for (int i = 0; i < 2 * m; i += 2)
                        {
                            do_nussbaumer_base_signcheck(get_poly_coeff(x, i * r, sum_uint64_count), get_poly_coeff(y, i * r, sum_uint64_count), rec_coeff_count_power, sum_uint64_count, product_uint64_count, get_poly_coeff(z, i * r, product_uint64_count), fft_base_alloc_ptr);
                            do_nussbaumer_base_signcheck(get_poly_coeff(x, (i + 1) * r, sum_uint64_count), get_poly_coeff(y, (i + 1) * r, sum_uint64_count), rec_coeff_count_power, sum_uint64_count, product_uint64_count, get_poly_coeff(z, (i + 1) * r, product_uint64_count), fft_base_alloc_ptr);
                        }
                    }

                    // Do second stage of FFT. This is Gentleman-Sande with dividing by 2 in each stage. 
                    // In both algorithms, the bit scrambling is done in the root of unity, so there's no need to 
                    // scaramble the indices. 
                    // This is done with the large size integers. 
                    int outer_end = coeff_count_power / 2;
                    for (int outer_index = 0; outer_index <= outer_end; ++outer_index)
                    {
                        int outer_remaining = coeff_count_power / 2 - outer_index;
                        int middle_end = 1 << outer_remaining;
                        int inner_end = 1 << outer_index;
                        for (int middle_index = 0; middle_index < middle_end; middle_index++)
                        {
                            int sr = ((reverse_bits(static_cast<uint32_t>(middle_index)) >> (32 - outer_remaining)) << outer_index);
                            int s = middle_index << (outer_index + 1);
                            int k = (r / m) * sr;
                            for (int inner_index = 0; inner_index < inner_end; inner_index++)
                            {
                                int i = s + inner_index;
                                int l = i + inner_end;
                                for (int a = 0; a < r; a++)
                                {
                                    sub_uint_uint(get_poly_coeff(z, i * r + a, product_uint64_count), get_poly_coeff(z, l * r + a, product_uint64_count), product_uint64_count, get_poly_coeff(temp_big, a, product_uint64_count));
                                    right_shift_sign_extend_uint(get_poly_coeff(temp_big, a, product_uint64_count), 1, product_uint64_count, get_poly_coeff(temp_big, a, product_uint64_count));
                                    add_uint_uint(get_poly_coeff(z, i * r + a, product_uint64_count), get_poly_coeff(z, l * r + a, product_uint64_count), product_uint64_count, get_poly_coeff(z, i * r + a, product_uint64_count));
                                    right_shift_sign_extend_uint(get_poly_coeff(z, i * r + a, product_uint64_count), 1, product_uint64_count, get_poly_coeff(z, i * r + a, product_uint64_count));
                                }

                                int rsubk = r - k;
                                for (int a = 0; a < rsubk; a++)
                                {
                                    set_uint_uint(get_poly_coeff(temp_big, a + k, product_uint64_count), product_uint64_count, get_poly_coeff(z, l * r + a, product_uint64_count));
                                }
                                for (int a = rsubk; a < r; a++)
                                {
                                    negate_uint(get_poly_coeff(temp_big, a - rsubk, product_uint64_count), product_uint64_count, get_poly_coeff(z, l * r + a, product_uint64_count));
                                }
                            }
                        }
                    }

                    // Copy to result. Also done on the large size integers. 
                    for (int indexM = 0; indexM < m; indexM++)
                    {
                        sub_uint_uint(get_poly_coeff(z, indexM * r, product_uint64_count), get_poly_coeff(z, (m + indexM + 1) * r - 1, product_uint64_count), product_uint64_count, get_poly_coeff(result, indexM, product_uint64_count));
                        for (int indexR = 1; indexR < r; indexR++)
                        {
                            add_uint_uint(get_poly_coeff(z, indexM * r + indexR, product_uint64_count), get_poly_coeff(z, (m + indexM) * r + indexR - 1, product_uint64_count), product_uint64_count, get_poly_coeff(result, indexR * m + indexM, product_uint64_count));
                        }
                    }
                }
            }

            namespace nussbaumer_cross_multiply
            {
                void do_nussbaumer_base_signcheck(const uint64_t *operand1, const uint64_t *operand2, int coeff_count_power, int sum_uint64_count, int product_uint64_count, uint64_t *result_11, uint64_t *result_22, uint64_t *result_12, uint64_t *alloc_ptr)
                {
                    // alloc_ptr should point to 4 * sum_uint64_count memory
                    uint64_t *first_operand_i = alloc_ptr;
                    uint64_t *first_operand_j = first_operand_i + sum_uint64_count;
                    uint64_t *second_operand_i = first_operand_j + sum_uint64_count;
                    uint64_t *second_operand_j = second_operand_i + sum_uint64_count;

                    int coeff_count = 1 << coeff_count_power;
                    set_zero_poly(coeff_count, product_uint64_count, result_11);
                    set_zero_poly(coeff_count, product_uint64_count, result_12);
                    set_zero_poly(coeff_count, product_uint64_count, result_22);

                    const uint64_t *operand1_coeff_i_ptr = operand1;
                    const uint64_t *operand2_coeff_i_ptr = operand2;
                    for (int i = 0; i < coeff_count; i++)
                    {
                        uint64_t is_first_operand_i_negative = operand1_coeff_i_ptr[sum_uint64_count - 1] >> (bits_per_uint64 - 1);
                        tools::set_uint_uint_negate_if(operand1_coeff_i_ptr, sum_uint64_count, is_first_operand_i_negative, first_operand_i);
                        operand1_coeff_i_ptr += sum_uint64_count;

                        uint64_t is_second_operand_i_negative = operand2_coeff_i_ptr[sum_uint64_count - 1] >> (bits_per_uint64 - 1);
                        tools::set_uint_uint_negate_if(operand2_coeff_i_ptr, sum_uint64_count, is_second_operand_i_negative, second_operand_i);
                        operand2_coeff_i_ptr += sum_uint64_count;

                        const uint64_t *operand1_coeff_j_ptr = operand1;
                        const uint64_t *operand2_coeff_j_ptr = operand2;
                        for (int j = 0; j < coeff_count; j++)
                        {
                            int loc = i + j;
                            if (loc >= coeff_count)
                            {
                                loc -= coeff_count;
                            }

                            uint64_t is_first_operand_j_negative = operand1_coeff_j_ptr[sum_uint64_count - 1] >> (bits_per_uint64 - 1);
                            uint64_t is_second_operand_j_negative = operand2_coeff_j_ptr[sum_uint64_count - 1] >> (bits_per_uint64 - 1);
                            tools::set_uint_uint_negate_if(operand1_coeff_j_ptr, operand2_coeff_j_ptr, sum_uint64_count, 
                                is_first_operand_j_negative, is_second_operand_j_negative, first_operand_j, second_operand_j);
                            operand1_coeff_j_ptr += sum_uint64_count;
                            operand2_coeff_j_ptr += sum_uint64_count;

                            uint64_t *result_ptr_11 = get_poly_coeff(result_11, loc, product_uint64_count);
                            uint64_t *result_ptr_12 = get_poly_coeff(result_12, loc, product_uint64_count);
                            uint64_t *result_ptr_22 = get_poly_coeff(result_22, loc, product_uint64_count);

                            // branch according to sign
                            switch (
                                (is_first_operand_i_negative ^ is_first_operand_j_negative ^ static_cast<uint64_t>(loc < i))
                                | ((is_first_operand_i_negative ^ is_second_operand_j_negative ^ static_cast<uint64_t>(loc < i)) << 1)
                                | ((is_second_operand_i_negative ^ is_second_operand_j_negative ^ static_cast<uint64_t>(loc < i)) << 2)
                                )
                            {
                                // add, add, add
                                case 0:
                                    tools::triple_multiply_uint_uint_add(
                                        first_operand_i, first_operand_j,
                                        first_operand_i, second_operand_j,
                                        second_operand_i, second_operand_j,
                                        sum_uint64_count, product_uint64_count,
                                        result_ptr_11, result_ptr_12, result_ptr_22
                                    );

                                    break;

                                // sub, add, add
                                case 1:
                                {
                                    uint64_t *temp_ptr_11 = result_ptr_11;

                                    for (int k = 0; k < product_uint64_count; k++)
                                    {
                                        *temp_ptr_11++ ^= 0xFFFFFFFFFFFFFFFF;
                                    }

                                    tools::triple_multiply_uint_uint_add(
                                        first_operand_i, first_operand_j,
                                        first_operand_i, second_operand_j,
                                        second_operand_i, second_operand_j,
                                        sum_uint64_count, product_uint64_count,
                                        result_ptr_11, result_ptr_12, result_ptr_22
                                    );

                                    for (int k = 0; k < product_uint64_count; k++)
                                    {
                                        *result_ptr_11++ ^= 0xFFFFFFFFFFFFFFFF;
                                    }

                                    break;
                                }

                                // add, sub, add
                                case 2:
                                {
                                    uint64_t *temp_ptr_12 = result_ptr_12;
                                    for (int k = 0; k < product_uint64_count; k++)
                                    {
                                        *temp_ptr_12++ ^= 0xFFFFFFFFFFFFFFFF;
                                    }

                                    tools::triple_multiply_uint_uint_add(
                                        first_operand_i, first_operand_j,
                                        first_operand_i, second_operand_j,
                                        second_operand_i, second_operand_j,
                                        sum_uint64_count, product_uint64_count,
                                        result_ptr_11, result_ptr_12, result_ptr_22
                                    );

                                    for (int k = 0; k < product_uint64_count; k++)
                                    {
                                        *result_ptr_12++ ^= 0xFFFFFFFFFFFFFFFF;
                                    }

                                    break;
                                }

                                // sub, sub, add
                                case 3:
                                {
                                    uint64_t *temp_ptr_11 = result_ptr_11;
                                    uint64_t *temp_ptr_12 = result_ptr_12;
                                    for (int k = 0; k < product_uint64_count; k++)
                                    {
                                        *temp_ptr_11++ ^= 0xFFFFFFFFFFFFFFFF;
                                        *temp_ptr_12++ ^= 0xFFFFFFFFFFFFFFFF;
                                    }

                                    tools::triple_multiply_uint_uint_add(
                                        first_operand_i, first_operand_j,
                                        first_operand_i, second_operand_j,
                                        second_operand_i, second_operand_j,
                                        sum_uint64_count, product_uint64_count,
                                        result_ptr_11, result_ptr_12, result_ptr_22
                                    );
                                    for (int k = 0; k < product_uint64_count; k++)
                                    {
                                        *result_ptr_11++ ^= 0xFFFFFFFFFFFFFFFF;
                                        *result_ptr_12++ ^= 0xFFFFFFFFFFFFFFFF;
                                    }

                                    break;
                                }

                                // add, add, sub
                                case 4:
                                {
                                    uint64_t *temp_ptr_22 = result_ptr_22;
                                    for (int k = 0; k < product_uint64_count; k++)
                                    {
                                        *temp_ptr_22++ ^= 0xFFFFFFFFFFFFFFFF;
                                    }

                                    tools::triple_multiply_uint_uint_add(
                                        first_operand_i, first_operand_j,
                                        first_operand_i, second_operand_j,
                                        second_operand_i, second_operand_j,
                                        sum_uint64_count, product_uint64_count,
                                        result_ptr_11, result_ptr_12, result_ptr_22
                                    );

                                    for (int k = 0; k < product_uint64_count; k++)
                                    {
                                        *result_ptr_22++ ^= 0xFFFFFFFFFFFFFFFF;
                                    }

                                    break;
                                }

                                // sub, add, sub
                                case 5:
                                {
                                    uint64_t *temp_ptr_11 = result_ptr_11;
                                    uint64_t *temp_ptr_22 = result_ptr_22;
                                    for (int k = 0; k < product_uint64_count; k++)
                                    {
                                        *temp_ptr_11++ ^= 0xFFFFFFFFFFFFFFFF;
                                        *temp_ptr_22++ ^= 0xFFFFFFFFFFFFFFFF;
                                    }

                                    tools::triple_multiply_uint_uint_add(
                                        first_operand_i, first_operand_j,
                                        first_operand_i, second_operand_j,
                                        second_operand_i, second_operand_j,
                                        sum_uint64_count, product_uint64_count,
                                        result_ptr_11, result_ptr_12, result_ptr_22
                                    );

                                    for (int k = 0; k < product_uint64_count; k++)
                                    {
                                        *result_ptr_11++ ^= 0xFFFFFFFFFFFFFFFF;
                                        *result_ptr_22++ ^= 0xFFFFFFFFFFFFFFFF;
                                    }


                                    break;
                                }

                                // add, sub, sub
                                case 6:
                                {
                                    uint64_t *temp_ptr_12 = result_ptr_12;
                                    uint64_t *temp_ptr_22 = result_ptr_22;
                                    for (int k = 0; k < product_uint64_count; k++)
                                    {
                                        *temp_ptr_12++ ^= 0xFFFFFFFFFFFFFFFF;
                                        *temp_ptr_22++ ^= 0xFFFFFFFFFFFFFFFF;
                                    }

                                    tools::triple_multiply_uint_uint_add(
                                        first_operand_i, first_operand_j,
                                        first_operand_i, second_operand_j,
                                        second_operand_i, second_operand_j,
                                        sum_uint64_count, product_uint64_count,
                                        result_ptr_11, result_ptr_12, result_ptr_22
                                    );

                                    for (int k = 0; k < product_uint64_count; k++)
                                    {
                                        *result_ptr_12++ ^= 0xFFFFFFFFFFFFFFFF;
                                        *result_ptr_22++ ^= 0xFFFFFFFFFFFFFFFF;
                                    }

                                    break;
                                }

                                // sub, sub, sub
                                case 7:
                                {
                                    uint64_t *temp_ptr_11 = result_ptr_11;
                                    uint64_t *temp_ptr_12 = result_ptr_12;
                                    uint64_t *temp_ptr_22 = result_ptr_22;
                                    for (int k = 0; k < product_uint64_count; k++)
                                    {
                                        *temp_ptr_11++ ^= 0xFFFFFFFFFFFFFFFF;
                                        *temp_ptr_12++ ^= 0xFFFFFFFFFFFFFFFF;
                                        *temp_ptr_22++ ^= 0xFFFFFFFFFFFFFFFF;
                                    }

                                    tools::triple_multiply_uint_uint_add(
                                        first_operand_i, first_operand_j,
                                        first_operand_i, second_operand_j,
                                        second_operand_i, second_operand_j,
                                        sum_uint64_count, product_uint64_count,
                                        result_ptr_11, result_ptr_12, result_ptr_22
                                    );

                                    for (int k = 0; k < product_uint64_count; k++)
                                    {
                                        *result_ptr_11++ ^= 0xFFFFFFFFFFFFFFFF;
                                        *result_ptr_12++ ^= 0xFFFFFFFFFFFFFFFF;
                                        *result_ptr_22++ ^= 0xFFFFFFFFFFFFFFFF;
                                    }

                                    break;
                                }
                            }
                        }
                    }
                }

                void do_nussbaumer(const uint64_t *operand1, const uint64_t *operand2, int coeff_count_power, int sum_uint64_count, int product_uint64_count, uint64_t *result_op1op1, uint64_t *result_op2op2, uint64_t *result_op1op2, MemoryPool &pool, uint64_t *fft_base_alloc_ptr)
                {
                    // fft_base_alloc_ptr should point to 4 * sum_uint64_count memory

                    // Handle base case differently.
                    int coeff_count = 1 << coeff_count_power;
                    if (coeff_count <= use_base_case)
                    {
                        do_nussbaumer_base_signcheck(operand1, operand2, coeff_count_power, sum_uint64_count, product_uint64_count, result_op1op1, result_op2op2, result_op1op2, fft_base_alloc_ptr);
                        return;
                    }

                    // Allocate temporary storage for FFT.
                    int m = 1 << (coeff_count_power / 2);
                    int r = 1 << ((coeff_count_power + 1) / 2);

                    // To reduce the number of allocations, we allocate one big block of memory at once.
                    Pointer big_alloc_anchor(allocate_uint(4 * 2 * coeff_count * sum_uint64_count + 6 * 2 * coeff_count * product_uint64_count, pool));

                    uint64_t *x = big_alloc_anchor.get();
                    uint64_t *y = x + 2 * coeff_count * sum_uint64_count;
                    uint64_t *temp_poly = y + 2 * coeff_count * sum_uint64_count;
                    uint64_t *temp_poly2 = temp_poly + 2 * coeff_count * sum_uint64_count;

                    uint64_t *z1 = temp_poly2 + 2 * coeff_count * sum_uint64_count;
                    uint64_t *z2 = z1 + 2 * coeff_count * product_uint64_count;
                    uint64_t *z3 = z2 + 2 * coeff_count * product_uint64_count;
                    uint64_t *temp_poly_prod1 = z3 + 2 * coeff_count * product_uint64_count;
                    uint64_t *temp_poly_prod2 = temp_poly_prod1 + 2 * coeff_count * product_uint64_count;
                    uint64_t *temp_poly_prod3 = temp_poly_prod2 + 2 * coeff_count * product_uint64_count;

                    // Populate x and y with butterfly pattern.
                    for (int indexM = 0; indexM < m; indexM++)
                    {
                        for (int indexR = 0; indexR < r; indexR++)
                        {
                            int index1 = indexR * m + indexM;
                            int index2 = indexM * r + indexR;

                            const uint64_t *operand1_coeff = get_poly_coeff(operand1, index1, sum_uint64_count);
                            set_uint_uint(operand1_coeff, sum_uint64_count, get_poly_coeff(x, index2, sum_uint64_count));

                            const uint64_t *operand2_coeff = get_poly_coeff(operand2, index1, sum_uint64_count);
                            set_uint_uint(operand2_coeff, sum_uint64_count, get_poly_coeff(y, index2, sum_uint64_count));
                        }
                    }
                    set_poly_poly(x, coeff_count, sum_uint64_count, get_poly_coeff(x, coeff_count, sum_uint64_count));
                    set_poly_poly(y, coeff_count, sum_uint64_count, get_poly_coeff(y, coeff_count, sum_uint64_count));

                    // Do first stage of FFT.
                    int outer_start = coeff_count_power / 2 - 1;
                    for (int outer_index = outer_start; outer_index >= 0; outer_index--)
                    {
                        int outer_remaining = coeff_count_power / 2 - outer_index;
                        int middle_end = 1 << outer_remaining;
                        int inner_end = 1 << outer_index;
                        for (int middle_index = 0; middle_index < middle_end; middle_index++)
                        {
                            int sr = ((reverse_bits(static_cast<uint32_t>(middle_index)) >> (32 - outer_remaining)) << outer_index);
                            int s = middle_index << (outer_index + 1);
                            int k = (r / m) * sr;
                            for (int inner_index = 0; inner_index < inner_end; inner_index++)
                            {
                                int i = s + inner_index;
                                int l = i + inner_end;

                                uint64_t *x_coeff_ptr = get_poly_coeff(x, (l + 1) * r - k, sum_uint64_count);
                                uint64_t *y_coeff_ptr = get_poly_coeff(y, (l + 1) * r - k, sum_uint64_count);
                                uint64_t *temp_poly_coeff_ptr = temp_poly;
                                uint64_t *temp_poly2_coeff_ptr = temp_poly2;

                                for (int a = 0; a < k; a++)
                                {
                                    negate_uint(x_coeff_ptr, sum_uint64_count, temp_poly_coeff_ptr);
                                    negate_uint(y_coeff_ptr, sum_uint64_count, temp_poly2_coeff_ptr);
                                    x_coeff_ptr += sum_uint64_count;
                                    y_coeff_ptr += sum_uint64_count;
                                    temp_poly_coeff_ptr += sum_uint64_count;
                                    temp_poly2_coeff_ptr += sum_uint64_count;
                                }
                                x_coeff_ptr -= r * sum_uint64_count;
                                y_coeff_ptr -= r * sum_uint64_count;

                                set_uint_uint(x_coeff_ptr, (r - k) * sum_uint64_count, temp_poly_coeff_ptr);
                                set_uint_uint(y_coeff_ptr, (r - k) * sum_uint64_count, temp_poly2_coeff_ptr);

                                x_coeff_ptr = get_poly_coeff(x, i * r, sum_uint64_count);
                                y_coeff_ptr = get_poly_coeff(y, i * r, sum_uint64_count);
                                uint64_t *x_coeff_ptr2 = get_poly_coeff(x, l * r, sum_uint64_count);
                                uint64_t *y_coeff_ptr2 = get_poly_coeff(y, l * r, sum_uint64_count);
                                temp_poly_coeff_ptr = temp_poly;
                                temp_poly2_coeff_ptr = temp_poly2;
                                for (int a = 0; a < r; a++)
                                {
                                    sub_uint_uint(x_coeff_ptr, temp_poly_coeff_ptr, sum_uint64_count, x_coeff_ptr2);
                                    add_uint_uint(x_coeff_ptr, temp_poly_coeff_ptr, sum_uint64_count, x_coeff_ptr);
                                    sub_uint_uint(y_coeff_ptr, temp_poly2_coeff_ptr, sum_uint64_count, y_coeff_ptr2);
                                    add_uint_uint(y_coeff_ptr, temp_poly2_coeff_ptr, sum_uint64_count, y_coeff_ptr);
                                    x_coeff_ptr += sum_uint64_count;
                                    y_coeff_ptr += sum_uint64_count;
                                    x_coeff_ptr2 += sum_uint64_count;
                                    y_coeff_ptr2 += sum_uint64_count;
                                    temp_poly_coeff_ptr += sum_uint64_count;
                                    temp_poly2_coeff_ptr += sum_uint64_count;
                                }
                            }
                        }
                    }

                    // Recursively do FFT. 
                    // z1, z2, z3 hold the 3 intermediate results. (They are reused r times).
                    // allocated all at once. 
                    int rec_coeff_count_power = (coeff_count_power + 1) / 2;
                    if ((1 << rec_coeff_count_power) > use_base_case)
                    {
                        for (int i = 0; i < 2 * m; i += 2)
                        {
                            do_nussbaumer(get_poly_coeff(x, i*r, sum_uint64_count), get_poly_coeff(y, i*r, sum_uint64_count), rec_coeff_count_power, sum_uint64_count, product_uint64_count, get_poly_coeff(z1, i*r, product_uint64_count), get_poly_coeff(z2, i*r, product_uint64_count), get_poly_coeff(z3, i*r, product_uint64_count), pool, fft_base_alloc_ptr);
                            do_nussbaumer(get_poly_coeff(x, (i + 1)*r, sum_uint64_count), get_poly_coeff(y, (i+1)*r, sum_uint64_count), rec_coeff_count_power, sum_uint64_count, product_uint64_count, get_poly_coeff(z1, (i+1)*r, product_uint64_count), get_poly_coeff(z2, (i+1)*r, product_uint64_count), get_poly_coeff(z3, (i+1)*r, product_uint64_count), pool, fft_base_alloc_ptr);
                        }
                    }
                    else
                    {
                        for (int i = 0; i < 2 * m; i += 2)
                        {
                            do_nussbaumer_base_signcheck(get_poly_coeff(x, i*r, sum_uint64_count), get_poly_coeff(y, i*r, sum_uint64_count), rec_coeff_count_power, sum_uint64_count, product_uint64_count, get_poly_coeff(z1, i*r, product_uint64_count), get_poly_coeff(z2, i*r, product_uint64_count), get_poly_coeff(z3, i*r, product_uint64_count), fft_base_alloc_ptr);
                            do_nussbaumer_base_signcheck(get_poly_coeff(x, (i + 1)*r, sum_uint64_count), get_poly_coeff(y, (i + 1)*r, sum_uint64_count), rec_coeff_count_power, sum_uint64_count, product_uint64_count, get_poly_coeff(z1, (i + 1)*r, product_uint64_count), get_poly_coeff(z2, (i + 1)*r, product_uint64_count), get_poly_coeff(z3, (i + 1)*r, product_uint64_count), fft_base_alloc_ptr);
                        }
                    }

                    // Do second stage of FFT.
                    int outer_end = coeff_count_power / 2;
                    for (int outer_index = 0; outer_index <= outer_end; outer_index++)
                    {
                        int outer_remaining = coeff_count_power / 2 - outer_index;
                        int middle_end = 1 << outer_remaining;
                        int inner_end = 1 << outer_index;
                        for (int middle_index = 0; middle_index < middle_end; middle_index++)
                        {
                            int sr = ((reverse_bits(static_cast<uint32_t>(middle_index)) >> (32 - outer_remaining)) << outer_index);
                            int s = middle_index << (outer_index + 1);
                            int k = (r / m) * sr;
                            for (int inner_index = 0; inner_index < inner_end; inner_index++)
                            {
                                int i = s + inner_index;
                                int l = i + inner_end;
                                for (int a = 0; a < r; a++)
                                {
                                    int index1 = i * r + a;
                                    int index2 = l * r + a;

                                    // z1
                                    sub_uint_uint(get_poly_coeff(z1, index1, product_uint64_count), get_poly_coeff(z1, index2, product_uint64_count), product_uint64_count, get_poly_coeff(temp_poly_prod1, a, product_uint64_count));
                                    right_shift_sign_extend_uint(get_poly_coeff(temp_poly_prod1, a, product_uint64_count), 1, product_uint64_count, get_poly_coeff(temp_poly_prod1, a, product_uint64_count));
                                    add_uint_uint(get_poly_coeff(z1, index1, product_uint64_count), get_poly_coeff(z1, index2, product_uint64_count), product_uint64_count, get_poly_coeff(z1, index1, product_uint64_count));
                                    right_shift_sign_extend_uint(get_poly_coeff(z1, index1, product_uint64_count), 1, product_uint64_count, get_poly_coeff(z1, index1, product_uint64_count));

                                    // z2
                                    sub_uint_uint(get_poly_coeff(z2, index1, product_uint64_count), get_poly_coeff(z2, index2, product_uint64_count), product_uint64_count, get_poly_coeff(temp_poly_prod2, a, product_uint64_count));
                                    right_shift_sign_extend_uint(get_poly_coeff(temp_poly_prod2, a, product_uint64_count), 1, product_uint64_count, get_poly_coeff(temp_poly_prod2, a, product_uint64_count));
                                    add_uint_uint(get_poly_coeff(z2, index1, product_uint64_count), get_poly_coeff(z2, index2, product_uint64_count), product_uint64_count, get_poly_coeff(z2, index1, product_uint64_count));
                                    right_shift_sign_extend_uint(get_poly_coeff(z2, index1, product_uint64_count), 1, product_uint64_count, get_poly_coeff(z2, index1, product_uint64_count));

                                    // z3
                                    sub_uint_uint(get_poly_coeff(z3, index1, product_uint64_count), get_poly_coeff(z3, index2, product_uint64_count), product_uint64_count, get_poly_coeff(temp_poly_prod3, a, product_uint64_count));
                                    right_shift_sign_extend_uint(get_poly_coeff(temp_poly_prod3, a, product_uint64_count), 1, product_uint64_count, get_poly_coeff(temp_poly_prod3, a, product_uint64_count));
                                    add_uint_uint(get_poly_coeff(z3, index1, product_uint64_count), get_poly_coeff(z3, index2, product_uint64_count), product_uint64_count, get_poly_coeff(z3, index1, product_uint64_count));
                                    right_shift_sign_extend_uint(get_poly_coeff(z3, index1, product_uint64_count), 1, product_uint64_count, get_poly_coeff(z3, index1, product_uint64_count));
                                }

                                int rsubk = r - k;
                                for (int a = 0; a < rsubk; a++)
                                {
                                    int index1 = a + k;
                                    int index2 = l * r + a;
                                    // z1
                                    set_uint_uint(get_poly_coeff(temp_poly_prod1, index1, product_uint64_count), product_uint64_count, get_poly_coeff(z1, index2, product_uint64_count));
                                    // z2
                                    set_uint_uint(get_poly_coeff(temp_poly_prod2, index1, product_uint64_count), product_uint64_count, get_poly_coeff(z2, index2, product_uint64_count));
                                    // z3
                                    set_uint_uint(get_poly_coeff(temp_poly_prod3, index1, product_uint64_count), product_uint64_count, get_poly_coeff(z3, index2, product_uint64_count));
                                }
                                for (int a = rsubk; a < r; a++)
                                {
                                    int index1 = a - rsubk;
                                    int index2 = l * r + a;
                                    // z1
                                    negate_uint(get_poly_coeff(temp_poly_prod1, index1, product_uint64_count), product_uint64_count, get_poly_coeff(z1, index2, product_uint64_count));
                                    // z2
                                    negate_uint(get_poly_coeff(temp_poly_prod2, index1, product_uint64_count), product_uint64_count, get_poly_coeff(z2, index2, product_uint64_count));
                                    // z3
                                    negate_uint(get_poly_coeff(temp_poly_prod3, index1, product_uint64_count), product_uint64_count, get_poly_coeff(z3, index2, product_uint64_count));
                                }
                            }
                        }
                    }

                    // Copy to result.
                    for (int indexM = 0; indexM < m; indexM++)
                    {
                        int index1 = indexM * r;
                        int index2 = (m + indexM + 1) * r - 1;
                        // z1
                        sub_uint_uint(get_poly_coeff(z1, index1, product_uint64_count), get_poly_coeff(z1, index2, product_uint64_count), product_uint64_count, get_poly_coeff(result_op1op1, indexM, product_uint64_count));
                        // z2
                        sub_uint_uint(get_poly_coeff(z2, index1, product_uint64_count), get_poly_coeff(z2, index2, product_uint64_count), product_uint64_count, get_poly_coeff(result_op2op2, indexM, product_uint64_count));
                        // z3
                        sub_uint_uint(get_poly_coeff(z3, index1, product_uint64_count), get_poly_coeff(z3, index2, product_uint64_count), product_uint64_count, get_poly_coeff(result_op1op2, indexM, product_uint64_count));

                        for (int indexR = 1; indexR < r; indexR++)
                        {
                            int index3 = indexM * r + indexR;
                            int index4 = (m + indexM) * r + indexR - 1;
                            int index5 = indexR * m + indexM;
                            // z1
                            add_uint_uint(get_poly_coeff(z1, index3, product_uint64_count), get_poly_coeff(z1, index4, product_uint64_count), product_uint64_count, get_poly_coeff(result_op1op1, index5, product_uint64_count));
                            // z2
                            add_uint_uint(get_poly_coeff(z2, index3, product_uint64_count), get_poly_coeff(z2, index4, product_uint64_count), product_uint64_count, get_poly_coeff(result_op2op2, index5, product_uint64_count));
                            // z3
                            add_uint_uint(get_poly_coeff(z3, index3, product_uint64_count), get_poly_coeff(z3, index4, product_uint64_count), product_uint64_count, get_poly_coeff(result_op1op2, index5, product_uint64_count));
                        }
                    }
                }
            }
        }

        void nussbaumer_multiply_poly_poly(const uint64_t *operand1, const uint64_t *operand2, int coeff_count_power, int coeff_uint64_count, int sum_uint64_count, int product_uint64_count, uint64_t *result, MemoryPool &pool)
        {
#ifdef _DEBUG
            if (operand1 == nullptr)
            {
                throw invalid_argument("operand1");
            }
            if (operand2 == nullptr)
            {
                throw invalid_argument("operand2");
            }
            if (coeff_count_power <= 0)
            {
                throw invalid_argument("coeff_count_power");
            }
            if (coeff_uint64_count <= 0)
            {
                throw invalid_argument("coeff_uint64_count");
            }
            if (product_uint64_count <= 0)
            {
                throw invalid_argument("product_uint64_count");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
#endif
            if (coeff_uint64_count == sum_uint64_count)
            {
                Pointer fft_base_alloc(allocate_uint(2 * sum_uint64_count, pool));
                nussbaumer_multiply::do_nussbaumer(operand1, operand2, coeff_count_power, sum_uint64_count, product_uint64_count, result, pool, fft_base_alloc.get());
            }
            else
            {
                int coeff_count = 1 << coeff_count_power;

                // Widen the operands
                Pointer operand1resized(allocate_poly(coeff_count, sum_uint64_count, pool));
                Pointer operand2resized(allocate_poly(coeff_count, sum_uint64_count, pool));
                set_poly_poly(operand1, coeff_count, coeff_uint64_count, coeff_count, sum_uint64_count, operand1resized.get());
                set_poly_poly(operand2, coeff_count, coeff_uint64_count, coeff_count, sum_uint64_count, operand2resized.get());

                Pointer fft_base_alloc(allocate_uint(2 * sum_uint64_count, pool));
                nussbaumer_multiply::do_nussbaumer(operand1resized.get(), operand2resized.get(), coeff_count_power, sum_uint64_count, product_uint64_count, result, pool, fft_base_alloc.get());
            }
        }
        
        void nussbaumer_cross_multiply_poly_poly(const uint64_t *operand1, const uint64_t *operand2, int coeff_count_power, int coeff_uint64_count, int sum_uint64_count, int product_uint64_count, uint64_t *result_op1_op1, uint64_t *result_op2_op2, uint64_t *result_op1_op2, MemoryPool &pool)
        {
#ifdef _DEBUG
            if (operand1 == nullptr)
            {
                throw invalid_argument("operand1");
            }
            if (operand2 == nullptr)
            {
                throw invalid_argument("operand2");
            }
            if (coeff_count_power <= 0)
            {
                throw invalid_argument("coeff_count_power");
            }
            if (coeff_uint64_count <= 0)
            {
                throw invalid_argument("coeff_uint64_count");
            }
            if (product_uint64_count <= 0)
            {
                throw invalid_argument("product_uint64_count");
            }
            if (result_op1_op1 == nullptr)
            {
                throw invalid_argument("result_op1_op1");
            }
            if (result_op2_op2 == nullptr)
            {
                throw invalid_argument("result_op2_op2");
            }
            if (result_op1_op2 == nullptr)
            {
                throw invalid_argument("result_op1_op2");
            }
#endif
            if (coeff_uint64_count == product_uint64_count)
            {
                Pointer fft_base_alloc(allocate_uint(4 * sum_uint64_count, pool));
                nussbaumer_cross_multiply::do_nussbaumer(operand1, operand2, coeff_count_power, coeff_uint64_count, coeff_uint64_count, result_op1_op1, result_op2_op2, result_op1_op2, pool, fft_base_alloc.get());
            }
            else
            {
                int coeff_count = 1 << coeff_count_power;
                Pointer operand1resized(allocate_poly(coeff_count, sum_uint64_count, pool));
                Pointer operand2resized(allocate_poly(coeff_count, sum_uint64_count, pool));
                for (int i = 0; i < coeff_count; ++i)
                {
                    set_uint_uint(get_poly_coeff(operand1, i, coeff_uint64_count), coeff_uint64_count, sum_uint64_count, get_poly_coeff(operand1resized.get(), i, sum_uint64_count));
                    set_uint_uint(get_poly_coeff(operand2, i, coeff_uint64_count), coeff_uint64_count, sum_uint64_count, get_poly_coeff(operand2resized.get(), i, sum_uint64_count));
                }
                Pointer fft_base_alloc(allocate_uint(4 * sum_uint64_count, pool));
                nussbaumer_cross_multiply::do_nussbaumer(operand1resized.get(), operand2resized.get(), coeff_count_power, sum_uint64_count, product_uint64_count, result_op1_op1, result_op2_op2, result_op1_op2, pool, fft_base_alloc.get());
            }
        }
    }
}