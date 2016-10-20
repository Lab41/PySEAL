#include "util/uintcore.h"
#include "util/polycore.h"
#include "util/uintarith.h"
#include "util/polyarith.h"
#include <algorithm>

using namespace std;

namespace seal
{
    namespace util
    {
        namespace
        {
            const int use_base_case = 8;

            namespace tools
            {
                inline void small_right_shift_sign_extend_uint(const uint64_t *operand, int shift_amount, int uint64_count, uint64_t *result)
                {
                    int neg_bit_shift_amount = bits_per_uint64 - shift_amount;
                    uint64_t high_bits_shift_in = ~((operand[uint64_count - 1] >> (bits_per_uint64 - 1)) - 1);

                    for (; uint64_count >= 2; uint64_count--)
                    {
                        *result = *operand++ >> shift_amount;
                        *result++ |= *operand << neg_bit_shift_amount;
                    }
                    *result = *operand >> shift_amount;
                    *result |= high_bits_shift_in << neg_bit_shift_amount;
                }

                inline void fold(const uint64_t *operand, int coeff_uint64_count, int m, int r, uint64_t *result)
                {
                    for (int indexM = 0; indexM < m; indexM++)
                    {
                        sub_uint_uint(get_poly_coeff(operand, indexM * r, coeff_uint64_count), get_poly_coeff(operand, (m + indexM + 1) * r - 1, coeff_uint64_count), coeff_uint64_count, get_poly_coeff(result, indexM, coeff_uint64_count));
                        for (int indexR = 1; indexR < r; indexR++)
                        {
                            add_uint_uint(get_poly_coeff(operand, indexM * r + indexR, coeff_uint64_count), get_poly_coeff(operand, (m + indexM) * r + indexR - 1, coeff_uint64_count), coeff_uint64_count, get_poly_coeff(result, indexR * m + indexM, coeff_uint64_count));
                        }
                    }
                }

                inline void rearrange(const uint64_t *operand, int coeff_uint64_count, int m, int r, uint64_t *result)
                {
                    for (int indexM = 0; indexM < m; indexM++)
                    {
                        const uint64_t *operand_coeff = get_poly_coeff(operand, indexM, coeff_uint64_count);
                        uint64_t *result_coeff = get_poly_coeff(result, indexM * r, coeff_uint64_count);
                        for (int indexR = 0; indexR < r; indexR++)
                        {
                            set_uint_uint(operand_coeff, coeff_uint64_count, result_coeff);
                            operand_coeff += m * coeff_uint64_count;
                            result_coeff += coeff_uint64_count;
                        }
                    }
                    set_poly_poly(result, m * r, coeff_uint64_count, get_poly_coeff(result, m * r, coeff_uint64_count));
                }

                // Perform inplace one round less DFT. 
                void symbolic_dft_oneroundless(uint64_t *operand, int coeff_count_power, int coeff_uint64_count, int m, int r, uint64_t *alloc_ptr)
                {
                    // alloc_ptr needs 2 * coeff_count * coeff_uint64_count memory

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
                                uint64_t *x_coeff_ptr = get_poly_coeff(operand, (l + 1) * r - k, coeff_uint64_count);
                                uint64_t *temp_poly_coeff_ptr = alloc_ptr;
                                for (int a = 0; a < k; a++)
                                {
                                    negate_uint(x_coeff_ptr, coeff_uint64_count, temp_poly_coeff_ptr);
                                    x_coeff_ptr += coeff_uint64_count;
                                    temp_poly_coeff_ptr += coeff_uint64_count;
                                }

                                uint64_t *x_coeff_ptr2 = get_poly_coeff(operand, l * r, coeff_uint64_count);
                                set_uint_uint(x_coeff_ptr2, (r - k) * coeff_uint64_count, temp_poly_coeff_ptr);

                                x_coeff_ptr = get_poly_coeff(operand, i * r, coeff_uint64_count);
                                temp_poly_coeff_ptr = alloc_ptr;
                                for (int a = 0; a < r; a++)
                                {
                                    // Add and subtract
                                    unsigned char borrow = sub_uint64_uint64(*x_coeff_ptr, *temp_poly_coeff_ptr, 0, x_coeff_ptr2++);
                                    unsigned char carry = add_uint64_uint64(*x_coeff_ptr, *temp_poly_coeff_ptr++, 0, x_coeff_ptr);
                                    x_coeff_ptr++;
                                    for (int add_sub_index = 1; add_sub_index < coeff_uint64_count; add_sub_index++)
                                    {
                                        borrow = sub_uint64_uint64(*x_coeff_ptr, *temp_poly_coeff_ptr, borrow, x_coeff_ptr2++);
                                        carry = add_uint64_uint64(*x_coeff_ptr, *temp_poly_coeff_ptr++, carry, x_coeff_ptr);
                                        x_coeff_ptr++;
                                    }
                                }
                            }
                        }
                    }
                }

                void symbolic_inverse_dft(uint64_t *operand, int coeff_count_power, int coeff_uint64_count, int m, int r, uint64_t *alloc_ptr)
                {
                    // Do second stage of FFT (Gentleman-Sande butterfly)
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
                                uint64_t *coeff1_ptr = get_poly_coeff(operand, i * r, coeff_uint64_count);
                                uint64_t *coeff2_ptr = get_poly_coeff(operand, l * r, coeff_uint64_count);
                                uint64_t *dest_coeff_ptr = alloc_ptr;
                                for (int a = 0; a < r; a++)
                                {
                                    // Add and subtract
                                    unsigned char borrow = sub_uint64_uint64(*coeff1_ptr, *coeff2_ptr, 0, dest_coeff_ptr++);
                                    unsigned char carry = add_uint64_uint64(*coeff1_ptr, *coeff2_ptr++, 0, coeff1_ptr);
                                    coeff1_ptr++;
                                    for (int add_sub_index = 1; add_sub_index < coeff_uint64_count; add_sub_index++)
                                    {
                                        borrow = sub_uint64_uint64(*coeff1_ptr, *coeff2_ptr, borrow, dest_coeff_ptr++);
                                        carry = add_uint64_uint64(*coeff1_ptr, *coeff2_ptr++, carry, coeff1_ptr);
                                        coeff1_ptr++;
                                    }
                                }

                                int rsubk = r - k;
                                set_uint_uint(get_poly_coeff(alloc_ptr, k, coeff_uint64_count), rsubk * coeff_uint64_count, get_poly_coeff(operand, l * r, coeff_uint64_count));

                                coeff1_ptr = alloc_ptr;
                                dest_coeff_ptr = get_poly_coeff(operand, l * r + rsubk, coeff_uint64_count);
                                for (int a = rsubk; a < r; a++)
                                {
                                    negate_uint(coeff1_ptr, coeff_uint64_count, dest_coeff_ptr);
                                    coeff1_ptr += coeff_uint64_count;
                                    dest_coeff_ptr += coeff_uint64_count;
                                }
                            }
                        }
                    }

                    for (int i = 0; i < 2 * (1 << coeff_count_power); i++)
                    {
                        tools::small_right_shift_sign_extend_uint(operand, outer_end + 1, coeff_uint64_count, operand);
                        operand += coeff_uint64_count;
                    }
                }

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
                    uint64_t carry, new_carry;
                    const uint64_t *inner_operand2;
                    uint64_t *inner_result;

                    for (int operand1_index = 0; operand1_index < operand_uint64_count; operand1_index++)
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
                            carry = new_carry + add_uint64_uint64(temp_result, carry, 0, &temp_result);
                            carry += add_uint64_uint64(*inner_result, temp_result, 0, inner_result);

                            inner_result++;
                        }

                        // Write carry if there is room in result
                        if (operand1_index + operand2_index_max < result_uint64_count)
                        {
                            unsigned char c_carry = add_uint64_uint64(*inner_result, carry, 0, inner_result);
                            inner_result++;
                            for (int i = operand1_index + operand2_index + 1; i < result_uint64_count; i++)
                            {
                                c_carry = add_uint64_uint64(*inner_result, 0, c_carry, inner_result);
                                inner_result++;
                            }
                        }

                        operand1++;
                    }
                }

                // Computes op11*op12, op21*op22, op31*op32
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
                    uint64_t temp_result;
                    uint64_t carry1, carry2, carry3;
                    uint64_t new_carry;
                    const uint64_t *inner_operand12, *inner_operand22, *inner_operand32;
                    uint64_t *inner_result1, *inner_result2, *inner_result3;

                    for (int operand1_index = 0; operand1_index < operand_uint64_count; operand1_index++)
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
                            temp_result = multiply_uint64_uint64(*operand11, *inner_operand12++, &new_carry);
                            carry1 = new_carry + add_uint64_uint64(temp_result, carry1, 0, &temp_result);
                            carry1 += add_uint64_uint64(*inner_result1, temp_result, 0, inner_result1);

                            temp_result = multiply_uint64_uint64(*operand21, *inner_operand22++, &new_carry);
                            carry2 = new_carry + add_uint64_uint64(temp_result, carry2, 0, &temp_result);
                            carry2 += add_uint64_uint64(*inner_result2, temp_result, 0, inner_result2);

                            temp_result = multiply_uint64_uint64(*operand31, *inner_operand32++, &new_carry);
                            carry3 = new_carry + add_uint64_uint64(temp_result, carry3, 0, &temp_result);
                            carry3 += add_uint64_uint64(*inner_result3, temp_result, 0, inner_result3);

                            inner_result1++;
                            inner_result2++;
                            inner_result3++;
                        }

                        // Write carry if there is room in result
                        if (operand1_index + operand2_index_max < result_uint64_count)
                        {
                            unsigned char c_carry = add_uint64_uint64(*inner_result1, carry1, 0, inner_result1);
                            inner_result1++;
                            for (int i = operand1_index + operand2_index + 1; i < result_uint64_count; i++)
                            {
                                c_carry = add_uint64_uint64(*inner_result1, 0, c_carry, inner_result1);
                                inner_result1++;
                            }

                            c_carry = add_uint64_uint64(*inner_result2, carry2, 0, inner_result2);
                            inner_result2++;
                            for (int i = operand1_index + operand2_index + 1; i < result_uint64_count; i++)
                            {
                                c_carry = add_uint64_uint64(*inner_result2, 0, c_carry, inner_result2);
                                inner_result2++;
                            }

                            c_carry = add_uint64_uint64(*inner_result3, carry3, 0, inner_result3);
                            inner_result3++;
                            for (int i = operand1_index + operand2_index + 1; i < result_uint64_count; i++)
                            {
                                c_carry = add_uint64_uint64(*inner_result3, 0, c_carry, inner_result3);
                                inner_result3++;
                            }
                        }

                        operand11++;
                        operand21++;
                        operand31++;
                    }
                }

                // Computes op11*op12, op11*op22, op31*op22
                void triple_multiply_uint_uint_add(
                    const uint64_t *operand11, const uint64_t *operand12, const uint64_t *operand22, const uint64_t *operand31,
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
                        *result2 += *operand11 * *operand22;
                        *result3 += *operand31 * *operand22;
                        return;
                    }

                    // Multiply operand11 and operand12, operand11 and operand22, operand31 and operand22.            
                    uint64_t temp_result;
                    uint64_t carry1, carry2, carry3;
                    uint64_t new_carry;
                    const uint64_t *inner_operand12, *inner_operand22;
                    uint64_t *inner_result1, *inner_result2, *inner_result3;

                    for (int operand1_index = 0; operand1_index < operand_uint64_count; operand1_index++)
                    {
                        inner_operand12 = operand12;
                        inner_operand22 = operand22;

                        inner_result1 = result1++;
                        inner_result2 = result2++;
                        inner_result3 = result3++;

                        carry1 = carry2 = carry3 = 0;
                        int operand2_index = 0;
                        int operand2_index_max = min(operand_uint64_count, result_uint64_count - operand1_index);
                        for (; operand2_index < operand2_index_max; operand2_index++)
                        {
                            // Perform 64-bit multiplication of operand1 and operand2
                            temp_result = multiply_uint64_uint64(*operand11, *inner_operand12++, &new_carry);
                            carry1 = new_carry + add_uint64_uint64(temp_result, carry1, 0, &temp_result);
                            carry1 += add_uint64_uint64(*inner_result1, temp_result, 0, inner_result1);

                            temp_result = multiply_uint64_uint64(*operand11, *inner_operand22, &new_carry);
                            carry2 = new_carry + add_uint64_uint64(temp_result, carry2, 0, &temp_result);
                            carry2 += add_uint64_uint64(*inner_result2, temp_result, 0, inner_result2);

                            temp_result = multiply_uint64_uint64(*operand31, *inner_operand22++, &new_carry);
                            carry3 = new_carry + add_uint64_uint64(temp_result, carry3, 0, &temp_result);
                            carry3 += add_uint64_uint64(*inner_result3, temp_result, 0, inner_result3);

                            inner_result1++;
                            inner_result2++;
                            inner_result3++;
                        }

                        // Write carry if there is room in result
                        if (operand1_index + operand2_index_max < result_uint64_count)
                        {
                            unsigned char c_carry = add_uint64_uint64(*inner_result1, carry1, 0, inner_result1);
                            inner_result1++;
                            for (int i = operand1_index + operand2_index + 1; i < result_uint64_count; i++)
                            {
                                c_carry = add_uint64_uint64(*inner_result1, 0, c_carry, inner_result1);
                                inner_result1++;
                            }

                            c_carry = add_uint64_uint64(*inner_result2, carry2, 0, inner_result2);
                            inner_result2++;
                            for (int i = operand1_index + operand2_index + 1; i < result_uint64_count; i++)
                            {
                                c_carry = add_uint64_uint64(*inner_result2, 0, c_carry, inner_result2);
                                inner_result2++;
                            }

                            c_carry = add_uint64_uint64(*inner_result3, carry3, 0, inner_result3);
                            inner_result3++;
                            for (int i = operand1_index + operand2_index + 1; i < result_uint64_count; i++)
                            {
                                c_carry = add_uint64_uint64(*inner_result3, 0, c_carry, inner_result3);
                                inner_result3++;
                            }
                        }

                        operand11++;
                        operand31++;
                    }
                }

                inline void set_uint_uint_negate_if(const uint64_t *operand, int uint64_count, unsigned char negate, uint64_t *destination)
                {
                    // negate is supposed to be either 0 or 1
                    uint64_t negate_mask = ~(static_cast<uint64_t>(negate) - 1);
                    for (int k = 0; k < uint64_count; k++)
                    {
                        negate = add_uint64_uint64(*operand++ ^ negate_mask, 0, negate, destination++);
                    }
                }
            }

            namespace nussbaumer_multiply
            {
                // Warning: This function changes operand1 and operand2
                inline void do_nussbaumer_base_signcheck(uint64_t *operand1, uint64_t *operand2, int coeff_count_power, int sum_uint64_count, int product_uint64_count, uint64_t *result, uint64_t *alloc_ptr)
                {
                    if (coeff_count_power == 2)
                    {
                        // Need large allocation (2 x temp results)
                        // 2 * coeff_count * product_uint64_count

                        const int coeff_count = 4;

                        // Store signs of coefficients
                        unsigned char op_coeff_signs[2 * coeff_count];

                        uint64_t *operand_coeff_ptr = operand1;
                        for (int i = 0; i < coeff_count; i++)
                        {
                            op_coeff_signs[i] = *(operand_coeff_ptr + sum_uint64_count - 1) >> (bits_per_uint64 - 1);
                            tools::set_uint_uint_negate_if(operand_coeff_ptr, sum_uint64_count, op_coeff_signs[i], operand_coeff_ptr);
                            operand_coeff_ptr += sum_uint64_count;
                        }

                        operand_coeff_ptr = operand2;
                        for (int i = 0; i < coeff_count; i++)
                        {
                            op_coeff_signs[coeff_count + i] = *(operand_coeff_ptr + sum_uint64_count - 1) >> (bits_per_uint64 - 1);
                            tools::set_uint_uint_negate_if(operand_coeff_ptr, sum_uint64_count, op_coeff_signs[coeff_count + i], operand_coeff_ptr);
                            operand_coeff_ptr += sum_uint64_count;
                        }

                        // Now compute all products and add to appropriate accumulators according to sign
                        set_zero_uint(2 * coeff_count * product_uint64_count, alloc_ptr);
                        unsigned char positive_offset = coeff_count * product_uint64_count;

                        // a0
                        operand_coeff_ptr = operand1;

                        // a0*b0
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 0, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 0, product_uint64_count) + (positive_offset & ((op_coeff_signs[0] ^ op_coeff_signs[coeff_count + 0]) - 1)));

                        // a0*b1
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 1, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 1, product_uint64_count) + (positive_offset & ((op_coeff_signs[0] ^ op_coeff_signs[coeff_count + 1]) - 1)));

                        // a0*b2
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 2, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 2, product_uint64_count) + (positive_offset & ((op_coeff_signs[0] ^ op_coeff_signs[coeff_count + 2]) - 1)));

                        // a0*b3
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 3, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 3, product_uint64_count) + (positive_offset & ((op_coeff_signs[0] ^ op_coeff_signs[coeff_count + 3]) - 1)));

                        // a1
                        operand_coeff_ptr += sum_uint64_count;

                        // a1*b0
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 0, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 1, product_uint64_count) + (positive_offset & ((op_coeff_signs[1] ^ op_coeff_signs[coeff_count + 0]) - 1)));

                        // a1*b1
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 1, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 2, product_uint64_count) + (positive_offset & ((op_coeff_signs[1] ^ op_coeff_signs[coeff_count + 1]) - 1)));

                        // a1*b2
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 2, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 3, product_uint64_count) + (positive_offset & ((op_coeff_signs[1] ^ op_coeff_signs[coeff_count + 2]) - 1)));

                        // a1*b3
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 3, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 0, product_uint64_count) + (positive_offset & ((op_coeff_signs[1] ^ op_coeff_signs[coeff_count + 3] ^ 1) - 1)));

                        // a2
                        operand_coeff_ptr += sum_uint64_count;

                        // a2*b0
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 0, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 2, product_uint64_count) + (positive_offset & ((op_coeff_signs[2] ^ op_coeff_signs[coeff_count + 0]) - 1)));

                        // a2*b1
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 1, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 3, product_uint64_count) + (positive_offset & ((op_coeff_signs[2] ^ op_coeff_signs[coeff_count + 1]) - 1)));

                        // a2*b2
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 2, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 0, product_uint64_count) + (positive_offset & ((op_coeff_signs[2] ^ op_coeff_signs[coeff_count + 2] ^ 1) - 1)));

                        // a2*b3
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 3, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 1, product_uint64_count) + (positive_offset & ((op_coeff_signs[2] ^ op_coeff_signs[coeff_count + 3] ^ 1) - 1)));

                        // a3
                        operand_coeff_ptr += sum_uint64_count;

                        // a3*b0
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 0, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 3, product_uint64_count) + (positive_offset & ((op_coeff_signs[3] ^ op_coeff_signs[coeff_count + 0]) - 1)));

                        // a3*b1
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 1, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 0, product_uint64_count) + (positive_offset & ((op_coeff_signs[3] ^ op_coeff_signs[coeff_count + 1] ^ 1) - 1)));

                        // a3*b2
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 2, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 1, product_uint64_count) + (positive_offset & ((op_coeff_signs[3] ^ op_coeff_signs[coeff_count + 2] ^ 1) - 1)));

                        // a3*b3
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 3, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 2, product_uint64_count) + (positive_offset & ((op_coeff_signs[3] ^ op_coeff_signs[coeff_count + 3] ^ 1) - 1)));

                        // Subtract to get result
                        for (int i = 0; i < coeff_count; i++)
                        {
                            unsigned char borrow = 0;
                            for (int j = 0; j < product_uint64_count; j++)
                            {
                                borrow = sub_uint64_uint64(*(alloc_ptr + positive_offset), *alloc_ptr, borrow, result++);
                                alloc_ptr++;
                            }
                        }

                        return;
                    }

                    if (coeff_count_power == 3)
                    {
                        const int coeff_count = 8;

                        // Need large allocation (2 x temp results)
                        // 2 * coeff_count * product_uint64_count

                        // Store signs of coefficients
                        unsigned char op_coeff_signs[2 * coeff_count];

                        uint64_t *operand_coeff_ptr = operand1;
                        for (int i = 0; i < coeff_count; i++)
                        {
                            op_coeff_signs[i] = *(operand_coeff_ptr + sum_uint64_count - 1) >> (bits_per_uint64 - 1);
                            tools::set_uint_uint_negate_if(operand_coeff_ptr, sum_uint64_count, op_coeff_signs[i], operand_coeff_ptr);
                            operand_coeff_ptr += sum_uint64_count;
                        }

                        operand_coeff_ptr = operand2;
                        for (int i = 0; i < coeff_count; i++)
                        {
                            op_coeff_signs[coeff_count + i] = *(operand_coeff_ptr + sum_uint64_count - 1) >> (bits_per_uint64 - 1);
                            tools::set_uint_uint_negate_if(operand_coeff_ptr, sum_uint64_count, op_coeff_signs[coeff_count + i], operand_coeff_ptr);
                            operand_coeff_ptr += sum_uint64_count;
                        }

                        // Now compute all products and add to appropriate accumulators according to sign
                        set_zero_uint(2 * coeff_count * product_uint64_count, alloc_ptr);
                        unsigned char positive_offset = coeff_count * product_uint64_count;

                        // a0
                        operand_coeff_ptr = operand1;

                        // a0*b0
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 0, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 0, product_uint64_count) + (positive_offset & ((op_coeff_signs[0] ^ op_coeff_signs[coeff_count + 0]) - 1)));

                        // a0*b1
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 1, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 1, product_uint64_count) + (positive_offset & ((op_coeff_signs[0] ^ op_coeff_signs[coeff_count + 1]) - 1)));

                        // a0*b2
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 2, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 2, product_uint64_count) + (positive_offset & ((op_coeff_signs[0] ^ op_coeff_signs[coeff_count + 2]) - 1)));

                        // a0*b3
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 3, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 3, product_uint64_count) + (positive_offset & ((op_coeff_signs[0] ^ op_coeff_signs[coeff_count + 3]) - 1)));

                        // a0*b4
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 4, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 4, product_uint64_count) + (positive_offset & ((op_coeff_signs[0] ^ op_coeff_signs[coeff_count + 4]) - 1)));

                        // a0*b5
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 5, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 5, product_uint64_count) + (positive_offset & ((op_coeff_signs[0] ^ op_coeff_signs[coeff_count + 5]) - 1)));

                        // a0*b6
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 6, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 6, product_uint64_count) + (positive_offset & ((op_coeff_signs[0] ^ op_coeff_signs[coeff_count + 6]) - 1)));

                        // a0*b7
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 7, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 7, product_uint64_count) + (positive_offset & ((op_coeff_signs[0] ^ op_coeff_signs[coeff_count + 7]) - 1)));

                        // a1
                        operand_coeff_ptr += sum_uint64_count;

                        // a1*b0
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 0, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 1, product_uint64_count) + (positive_offset & ((op_coeff_signs[1] ^ op_coeff_signs[coeff_count + 0]) - 1)));

                        // a1*b1
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 1, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 2, product_uint64_count) + (positive_offset & ((op_coeff_signs[1] ^ op_coeff_signs[coeff_count + 1]) - 1)));

                        // a1*b2
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 2, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 3, product_uint64_count) + (positive_offset & ((op_coeff_signs[1] ^ op_coeff_signs[coeff_count + 2]) - 1)));

                        // a1*b3
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 3, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 4, product_uint64_count) + (positive_offset & ((op_coeff_signs[1] ^ op_coeff_signs[coeff_count + 3]) - 1)));

                        // a1*b4
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 4, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 5, product_uint64_count) + (positive_offset & ((op_coeff_signs[1] ^ op_coeff_signs[coeff_count + 4]) - 1)));

                        // a1*b5
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 5, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 6, product_uint64_count) + (positive_offset & ((op_coeff_signs[1] ^ op_coeff_signs[coeff_count + 5]) - 1)));

                        // a1*b6
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 6, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 7, product_uint64_count) + (positive_offset & ((op_coeff_signs[1] ^ op_coeff_signs[coeff_count + 6]) - 1)));

                        // a1*b7
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 7, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 0, product_uint64_count) + (positive_offset & ((op_coeff_signs[1] ^ op_coeff_signs[coeff_count + 7] ^ 1) - 1)));

                        // a2
                        operand_coeff_ptr += sum_uint64_count;

                        // a2*b0
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 0, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 2, product_uint64_count) + (positive_offset & ((op_coeff_signs[2] ^ op_coeff_signs[coeff_count + 0]) - 1)));

                        // a2*b1
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 1, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 3, product_uint64_count) + (positive_offset & ((op_coeff_signs[2] ^ op_coeff_signs[coeff_count + 1]) - 1)));

                        // a2*b2
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 2, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 4, product_uint64_count) + (positive_offset & ((op_coeff_signs[2] ^ op_coeff_signs[coeff_count + 2]) - 1)));

                        // a2*b3
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 3, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 5, product_uint64_count) + (positive_offset & ((op_coeff_signs[2] ^ op_coeff_signs[coeff_count + 3]) - 1)));

                        // a2*b4
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 4, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 6, product_uint64_count) + (positive_offset & ((op_coeff_signs[2] ^ op_coeff_signs[coeff_count + 4]) - 1)));

                        // a2*b5
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 5, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 7, product_uint64_count) + (positive_offset & ((op_coeff_signs[2] ^ op_coeff_signs[coeff_count + 5]) - 1)));

                        // a2*b6
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 6, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 0, product_uint64_count) + (positive_offset & ((op_coeff_signs[2] ^ op_coeff_signs[coeff_count + 6] ^ 1) - 1)));

                        // a2*b7
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 7, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 1, product_uint64_count) + (positive_offset & ((op_coeff_signs[2] ^ op_coeff_signs[coeff_count + 7] ^ 1) - 1)));

                        // a3
                        operand_coeff_ptr += sum_uint64_count;

                        // a3*b0
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 0, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 3, product_uint64_count) + (positive_offset & ((op_coeff_signs[3] ^ op_coeff_signs[coeff_count + 0]) - 1)));

                        // a3*b1
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 1, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 4, product_uint64_count) + (positive_offset & ((op_coeff_signs[3] ^ op_coeff_signs[coeff_count + 1]) - 1)));

                        // a3*b2
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 2, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 5, product_uint64_count) + (positive_offset & ((op_coeff_signs[3] ^ op_coeff_signs[coeff_count + 2]) - 1)));

                        // a3*b3
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 3, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 6, product_uint64_count) + (positive_offset & ((op_coeff_signs[3] ^ op_coeff_signs[coeff_count + 3]) - 1)));

                        // a3*b4
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 4, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 7, product_uint64_count) + (positive_offset & ((op_coeff_signs[3] ^ op_coeff_signs[coeff_count + 4]) - 1)));

                        // a3*b5
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 5, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 0, product_uint64_count) + (positive_offset & ((op_coeff_signs[3] ^ op_coeff_signs[coeff_count + 5] ^ 1) - 1)));

                        // a3*b6
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 6, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 1, product_uint64_count) + (positive_offset & ((op_coeff_signs[3] ^ op_coeff_signs[coeff_count + 6] ^ 1) - 1)));

                        // a3*b7
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 7, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 2, product_uint64_count) + (positive_offset & ((op_coeff_signs[3] ^ op_coeff_signs[coeff_count + 7] ^ 1) - 1)));

                        // a4
                        operand_coeff_ptr += sum_uint64_count;

                        // a4*b0
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 0, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 4, product_uint64_count) + (positive_offset & ((op_coeff_signs[4] ^ op_coeff_signs[coeff_count + 0]) - 1)));

                        // a4*b1
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 1, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 5, product_uint64_count) + (positive_offset & ((op_coeff_signs[4] ^ op_coeff_signs[coeff_count + 1]) - 1)));

                        // a4*b2
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 2, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 6, product_uint64_count) + (positive_offset & ((op_coeff_signs[4] ^ op_coeff_signs[coeff_count + 2]) - 1)));

                        // a4*b3
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 3, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 7, product_uint64_count) + (positive_offset & ((op_coeff_signs[4] ^ op_coeff_signs[coeff_count + 3]) - 1)));

                        // a4*b4
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 4, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 0, product_uint64_count) + (positive_offset & ((op_coeff_signs[4] ^ op_coeff_signs[coeff_count + 4] ^ 1) - 1)));

                        // a4*b5
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 5, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 1, product_uint64_count) + (positive_offset & ((op_coeff_signs[4] ^ op_coeff_signs[coeff_count + 5] ^ 1) - 1)));

                        // a4*b6
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 6, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 2, product_uint64_count) + (positive_offset & ((op_coeff_signs[4] ^ op_coeff_signs[coeff_count + 6] ^ 1) - 1)));

                        // a4*b7
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 7, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 3, product_uint64_count) + (positive_offset & ((op_coeff_signs[4] ^ op_coeff_signs[coeff_count + 7] ^ 1) - 1)));

                        // a5
                        operand_coeff_ptr += sum_uint64_count;

                        // a5*b0
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 0, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 5, product_uint64_count) + (positive_offset & ((op_coeff_signs[5] ^ op_coeff_signs[coeff_count + 0]) - 1)));

                        // a5*b1
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 1, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 6, product_uint64_count) + (positive_offset & ((op_coeff_signs[5] ^ op_coeff_signs[coeff_count + 1]) - 1)));

                        // a5*b2
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 2, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 7, product_uint64_count) + (positive_offset & ((op_coeff_signs[5] ^ op_coeff_signs[coeff_count + 2]) - 1)));

                        // a5*b3
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 3, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 0, product_uint64_count) + (positive_offset & ((op_coeff_signs[5] ^ op_coeff_signs[coeff_count + 3] ^ 1) - 1)));

                        // a5*b4
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 4, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 1, product_uint64_count) + (positive_offset & ((op_coeff_signs[5] ^ op_coeff_signs[coeff_count + 4] ^ 1) - 1)));

                        // a5*b5
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 5, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 2, product_uint64_count) + (positive_offset & ((op_coeff_signs[5] ^ op_coeff_signs[coeff_count + 5] ^ 1) - 1)));

                        // a5*b6
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 6, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 3, product_uint64_count) + (positive_offset & ((op_coeff_signs[5] ^ op_coeff_signs[coeff_count + 6] ^ 1) - 1)));

                        // a5*b7
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 7, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 4, product_uint64_count) + (positive_offset & ((op_coeff_signs[5] ^ op_coeff_signs[coeff_count + 7] ^ 1) - 1)));

                        // a6
                        operand_coeff_ptr += sum_uint64_count;

                        // a6*b0
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 0, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 6, product_uint64_count) + (positive_offset & ((op_coeff_signs[6] ^ op_coeff_signs[coeff_count + 0]) - 1)));

                        // a6*b1
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 1, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 7, product_uint64_count) + (positive_offset & ((op_coeff_signs[6] ^ op_coeff_signs[coeff_count + 1]) - 1)));

                        // a6*b2
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 2, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 0, product_uint64_count) + (positive_offset & ((op_coeff_signs[6] ^ op_coeff_signs[coeff_count + 2] ^ 1) - 1)));

                        // a6*b3
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 3, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 1, product_uint64_count) + (positive_offset & ((op_coeff_signs[6] ^ op_coeff_signs[coeff_count + 3] ^ 1) - 1)));

                        // a6*b4
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 4, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 2, product_uint64_count) + (positive_offset & ((op_coeff_signs[6] ^ op_coeff_signs[coeff_count + 4] ^ 1) - 1)));

                        // a6*b5
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 5, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 3, product_uint64_count) + (positive_offset & ((op_coeff_signs[6] ^ op_coeff_signs[coeff_count + 5] ^ 1) - 1)));

                        // a6*b6
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 6, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 4, product_uint64_count) + (positive_offset & ((op_coeff_signs[6] ^ op_coeff_signs[coeff_count + 6] ^ 1) - 1)));

                        // a6*b7
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 7, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 5, product_uint64_count) + (positive_offset & ((op_coeff_signs[6] ^ op_coeff_signs[coeff_count + 7] ^ 1) - 1)));

                        // a7
                        operand_coeff_ptr += sum_uint64_count;

                        // a7*b0
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 0, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 7, product_uint64_count) + (positive_offset & ((op_coeff_signs[7] ^ op_coeff_signs[coeff_count + 0]) - 1)));

                        // a7*b1
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 1, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 0, product_uint64_count) + (positive_offset & ((op_coeff_signs[7] ^ op_coeff_signs[coeff_count + 1] ^ 1) - 1)));

                        // a7*b2
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 2, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 1, product_uint64_count) + (positive_offset & ((op_coeff_signs[7] ^ op_coeff_signs[coeff_count + 2] ^ 1) - 1)));

                        // a7*b3
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 3, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 2, product_uint64_count) + (positive_offset & ((op_coeff_signs[7] ^ op_coeff_signs[coeff_count + 3] ^ 1) - 1)));

                        // a7*b4
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 4, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 3, product_uint64_count) + (positive_offset & ((op_coeff_signs[7] ^ op_coeff_signs[coeff_count + 4] ^ 1) - 1)));

                        // a7*b5
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 5, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 4, product_uint64_count) + (positive_offset & ((op_coeff_signs[7] ^ op_coeff_signs[coeff_count + 5] ^ 1) - 1)));

                        // a7*b6
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 6, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 5, product_uint64_count) + (positive_offset & ((op_coeff_signs[7] ^ op_coeff_signs[coeff_count + 6] ^ 1) - 1)));

                        // a7*b7
                        tools::multiply_uint_uint_add(operand_coeff_ptr, get_poly_coeff(operand2, 7, sum_uint64_count), sum_uint64_count, product_uint64_count,
                            get_poly_coeff(alloc_ptr, 6, product_uint64_count) + (positive_offset & ((op_coeff_signs[7] ^ op_coeff_signs[coeff_count + 7] ^ 1) - 1)));

                        // Subtract to get result
                        for (int i = 0; i < coeff_count; i++)
                        {
                            unsigned char borrow = 0;
                            for (int j = 0; j < product_uint64_count; j++)
                            {
                                borrow = sub_uint64_uint64(*(alloc_ptr + positive_offset), *alloc_ptr, borrow, result++);
                                alloc_ptr++;
                            }
                        }

                        return;
                    }
                }

                void do_nussbaumer(uint64_t *operand1, uint64_t *operand2, int coeff_count_power, int sum_uint64_count, int product_uint64_count, uint64_t *result, MemoryPool &pool)
                {
                    // Handle base case differently.
                    int coeff_count = 1 << coeff_count_power;
                    if (coeff_count <= use_base_case)
                    {
                        Pointer fft_base_alloc(allocate_uint(2 * coeff_count * product_uint64_count, pool));
                        do_nussbaumer_base_signcheck(operand1, operand2, coeff_count_power, sum_uint64_count, product_uint64_count, result, fft_base_alloc.get());
                        return;
                    }

                    // Allocate temporary storage for FFT.
                    int m = 1 << (coeff_count_power / 2);
                    int rec_coeff_count_power = (coeff_count_power + 1) / 2;
                    int r = 1 << rec_coeff_count_power;

                    // To reduce the number of allocations, we allocate one big block of memory at once.
                    Pointer big_alloc_anchor(allocate_uint(2 * coeff_count * sum_uint64_count + 2 * coeff_count * sum_uint64_count + 2 * coeff_count * product_uint64_count
                        + 2 * coeff_count * product_uint64_count, pool));
                    uint64_t *x = big_alloc_anchor.get();
                    uint64_t *y = x + 2 * coeff_count * sum_uint64_count;
                    uint64_t *fft_base_alloc_ptr = y + 2 * coeff_count * sum_uint64_count;
                    uint64_t *z = fft_base_alloc_ptr + 2 * coeff_count * product_uint64_count;

                    tools::rearrange(operand1, sum_uint64_count, m, r, x);
                    tools::symbolic_dft_oneroundless(x, coeff_count_power, sum_uint64_count, m, r, fft_base_alloc_ptr);

                    tools::rearrange(operand2, sum_uint64_count, m, r, y);
                    tools::symbolic_dft_oneroundless(y, coeff_count_power, sum_uint64_count, m, r, fft_base_alloc_ptr);

                    // Recursively do FFT.
                    if (r > use_base_case)
                    {
                        for (int i = 0; i < 2 * m; i += 2)
                        {
                            do_nussbaumer(get_poly_coeff(x, i * r, sum_uint64_count), get_poly_coeff(y, i * r, sum_uint64_count), rec_coeff_count_power, sum_uint64_count, product_uint64_count, get_poly_coeff(z, i * r, product_uint64_count), pool);
                            do_nussbaumer(get_poly_coeff(x, (i + 1) * r, sum_uint64_count), get_poly_coeff(y, (i + 1) * r, sum_uint64_count), rec_coeff_count_power, sum_uint64_count, product_uint64_count, get_poly_coeff(z, (i + 1) * r, product_uint64_count), pool);
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
                    tools::symbolic_inverse_dft(z, coeff_count_power, product_uint64_count, m, r, fft_base_alloc_ptr);

                    // Copy to result. Also done on the large size integers.  
                    tools::fold(z, product_uint64_count, m, r, result);
                }
            }

            namespace nussbaumer_cross_multiply
            {
                void do_nussbaumer_base_signcheck(uint64_t *operand1, uint64_t *operand2, int coeff_count_power, int sum_uint64_count, int product_uint64_count, uint64_t *result_11, uint64_t *result_22, uint64_t *result_12, uint64_t *alloc_ptr)
                {
                    if (coeff_count_power == 2)
                    {
                        const int coeff_count = 4;

                        // Need large allocation (3 x 2 x temp results)
                        // 3 * 2 * coeff_count * product_uint64_count

                        // Store signs of coefficients
                        unsigned char op_coeff_signs[2 * coeff_count];

                        uint64_t *operand_coeff_ptr = operand1;
                        for (int i = 0; i < coeff_count; i++)
                        {
                            op_coeff_signs[i] = *(operand_coeff_ptr + sum_uint64_count - 1) >> (bits_per_uint64 - 1);
                            tools::set_uint_uint_negate_if(operand_coeff_ptr, sum_uint64_count, op_coeff_signs[i], operand_coeff_ptr);
                            operand_coeff_ptr += sum_uint64_count;
                        }

                        operand_coeff_ptr = operand2;
                        for (int i = 0; i < coeff_count; i++)
                        {
                            op_coeff_signs[coeff_count + i] = *(operand_coeff_ptr + sum_uint64_count - 1) >> (bits_per_uint64 - 1);
                            tools::set_uint_uint_negate_if(operand_coeff_ptr, sum_uint64_count, op_coeff_signs[coeff_count + i], operand_coeff_ptr);
                            operand_coeff_ptr += sum_uint64_count;
                        }

                        // Now compute all products and add to appropriate accumulators according to sign
                        set_zero_uint(6 * coeff_count * product_uint64_count, alloc_ptr);
                        uint64_t *negative_result_11 = alloc_ptr;
                        uint64_t *negative_result_12 = alloc_ptr + 2 * coeff_count * product_uint64_count;
                        uint64_t *negative_result_22 = alloc_ptr + 4 * coeff_count * product_uint64_count;
                        unsigned char positive_offset = coeff_count * product_uint64_count;

                        for (int i = 0; i < coeff_count; i++)
                        {
                            for (int j = 0; j < coeff_count; j++)
                            {
                                int result_coeff_index = i + j;
                                unsigned char sign_adjust = result_coeff_index >= coeff_count;
                                result_coeff_index %= coeff_count;
                                tools::triple_multiply_uint_uint_add(
                                    get_poly_coeff(operand1, i, sum_uint64_count), get_poly_coeff(operand1, j, sum_uint64_count), 
                                    get_poly_coeff(operand2, j, sum_uint64_count), get_poly_coeff(operand2, i, sum_uint64_count),
                                    sum_uint64_count, product_uint64_count,
                                    get_poly_coeff(negative_result_11, result_coeff_index, product_uint64_count) + (positive_offset & ((op_coeff_signs[i] ^ op_coeff_signs[j] ^ sign_adjust) - 1)),
                                    get_poly_coeff(negative_result_12, result_coeff_index, product_uint64_count) + (positive_offset & ((op_coeff_signs[i] ^ op_coeff_signs[coeff_count + j] ^ sign_adjust) - 1)),
                                    get_poly_coeff(negative_result_22, result_coeff_index, product_uint64_count) + (positive_offset & ((op_coeff_signs[coeff_count + i] ^ op_coeff_signs[coeff_count + j] ^ sign_adjust) - 1))
                                );
                           }
                        }

                        // Subtract to get result
                        for (int i = 0; i < coeff_count; i++)
                        {
                            unsigned char borrow11 = 0, borrow12 = 0, borrow22 = 0;
                            for (int j = 0; j < product_uint64_count; j++)
                            {
                                borrow11 = sub_uint64_uint64(*(negative_result_11 + positive_offset), *negative_result_11, borrow11, result_11++);
                                negative_result_11++;
                                borrow12 = sub_uint64_uint64(*(negative_result_12 + positive_offset), *negative_result_12, borrow12, result_12++);
                                negative_result_12++;
                                borrow22 = sub_uint64_uint64(*(negative_result_22 + positive_offset), *negative_result_22, borrow22, result_22++);
                                negative_result_22++;
                            }
                        }

                        return;
                    }

                    if (coeff_count_power == 3)
                    {
                        const int coeff_count = 8;

                        // Need large allocation (3 x 2 x temp results)
                        // 3 * 2 * coeff_count * product_uint64_count

                        // Store signs of coefficients
                        unsigned char op_coeff_signs[2 * coeff_count];

                        uint64_t *operand_coeff_ptr = operand1;
                        for (int i = 0; i < coeff_count; i++)
                        {
                            op_coeff_signs[i] = *(operand_coeff_ptr + sum_uint64_count - 1) >> (bits_per_uint64 - 1);
                            tools::set_uint_uint_negate_if(operand_coeff_ptr, sum_uint64_count, op_coeff_signs[i], operand_coeff_ptr);
                            operand_coeff_ptr += sum_uint64_count;
                        }

                        operand_coeff_ptr = operand2;
                        for (int i = 0; i < coeff_count; i++)
                        {
                            op_coeff_signs[coeff_count + i] = *(operand_coeff_ptr + sum_uint64_count - 1) >> (bits_per_uint64 - 1);
                            tools::set_uint_uint_negate_if(operand_coeff_ptr, sum_uint64_count, op_coeff_signs[coeff_count + i], operand_coeff_ptr);
                            operand_coeff_ptr += sum_uint64_count;
                        }

                        // Now compute all products and add to appropriate accumulators according to sign
                        set_zero_uint(6 * coeff_count * product_uint64_count, alloc_ptr);
                        uint64_t *negative_result_11 = alloc_ptr;
                        uint64_t *negative_result_12 = alloc_ptr + 2 * coeff_count * product_uint64_count;
                        uint64_t *negative_result_22 = alloc_ptr + 4 * coeff_count * product_uint64_count;
                        unsigned char positive_offset = coeff_count * product_uint64_count;

                        for (int i = 0; i < coeff_count; i++)
                        {
                            for (int j = 0; j < coeff_count; j++)
                            {
                                int result_coeff_index = i + j;
                                unsigned char sign_adjust = result_coeff_index >= coeff_count;
                                result_coeff_index %= coeff_count;
                                tools::triple_multiply_uint_uint_add(
                                    get_poly_coeff(operand1, i, sum_uint64_count), get_poly_coeff(operand1, j, sum_uint64_count),
                                    get_poly_coeff(operand2, j, sum_uint64_count), get_poly_coeff(operand2, i, sum_uint64_count),
                                    sum_uint64_count, product_uint64_count,
                                    get_poly_coeff(negative_result_11, result_coeff_index, product_uint64_count) + (positive_offset & ((op_coeff_signs[i] ^ op_coeff_signs[j] ^ sign_adjust) - 1)),
                                    get_poly_coeff(negative_result_12, result_coeff_index, product_uint64_count) + (positive_offset & ((op_coeff_signs[i] ^ op_coeff_signs[coeff_count + j] ^ sign_adjust) - 1)),
                                    get_poly_coeff(negative_result_22, result_coeff_index, product_uint64_count) + (positive_offset & ((op_coeff_signs[coeff_count + i] ^ op_coeff_signs[coeff_count + j] ^ sign_adjust) - 1))
                                );
                            }
                        }

                        // Subtract to get result
                        for (int i = 0; i < coeff_count; i++)
                        {
                            unsigned char borrow11 = 0, borrow12 = 0, borrow22 = 0;
                            for (int j = 0; j < product_uint64_count; j++)
                            {
                                borrow11 = sub_uint64_uint64(*(negative_result_11 + positive_offset), *negative_result_11, borrow11, result_11++);
                                negative_result_11++;
                                borrow12 = sub_uint64_uint64(*(negative_result_12 + positive_offset), *negative_result_12, borrow12, result_12++);
                                negative_result_12++;
                                borrow22 = sub_uint64_uint64(*(negative_result_22 + positive_offset), *negative_result_22, borrow22, result_22++);
                                negative_result_22++;
                            }
                        }

                        return;
                    }
                }

                void do_nussbaumer(uint64_t *operand1, uint64_t *operand2, int coeff_count_power, int sum_uint64_count, int product_uint64_count, uint64_t *result_op1_op1, uint64_t *result_op2_op2, uint64_t *result_op1_op2, MemoryPool &pool)
                {
                    // Handle base case differently.
                    int coeff_count = 1 << coeff_count_power;
                    if (coeff_count <= use_base_case)
                    {
                        Pointer fft_base_alloc(allocate_uint(6 * coeff_count * product_uint64_count, pool));
                        do_nussbaumer_base_signcheck(operand1, operand2, coeff_count_power, sum_uint64_count, product_uint64_count, result_op1_op1, result_op2_op2, result_op1_op2, fft_base_alloc.get());
                        return;
                    }

                    // Allocate temporary storage for FFT.
                    int m = 1 << (coeff_count_power / 2);
                    int rec_coeff_count_power = (coeff_count_power + 1) / 2;
                    int r = 1 << rec_coeff_count_power;

                    // To reduce the number of allocations, we allocate one big block of memory at once.
                    Pointer big_alloc_anchor(allocate_uint(2 * coeff_count * sum_uint64_count + 2 * coeff_count * sum_uint64_count + 
                        2 * coeff_count * product_uint64_count + 2 * coeff_count * product_uint64_count + 2 * coeff_count * product_uint64_count
                        + 6 * coeff_count * product_uint64_count, pool));
                    uint64_t *x = big_alloc_anchor.get();
                    uint64_t *y = x + 2 * coeff_count * sum_uint64_count;
                    uint64_t *fft_base_alloc_ptr = y + 2 * coeff_count * sum_uint64_count;
                    uint64_t *z1 = fft_base_alloc_ptr + 6 * coeff_count * product_uint64_count;
                    uint64_t *z2 = z1 + 2 * coeff_count * product_uint64_count;
                    uint64_t *z3 = z2 + 2 * coeff_count * product_uint64_count;

                    tools::rearrange(operand1, sum_uint64_count, m, r, x);
                    tools::symbolic_dft_oneroundless(x, coeff_count_power, sum_uint64_count, m, r, fft_base_alloc_ptr);

                    tools::rearrange(operand2, sum_uint64_count, m, r, y);
                    tools::symbolic_dft_oneroundless(y, coeff_count_power, sum_uint64_count, m, r, fft_base_alloc_ptr);

                    // Recursively do FFT. 
                    // z1, z2, z3 hold the 3 intermediate results. (They are reused r times).
                    // allocated all at once. 
                    if (r > use_base_case)
                    {
                        for (int i = 0; i < 2 * m; i += 2)
                        {
                            do_nussbaumer(get_poly_coeff(x, i * r, sum_uint64_count), get_poly_coeff(y, i * r, sum_uint64_count), rec_coeff_count_power, sum_uint64_count, product_uint64_count, get_poly_coeff(z1, i * r, product_uint64_count), get_poly_coeff(z2, i * r, product_uint64_count), get_poly_coeff(z3, i * r, product_uint64_count), pool);
                            do_nussbaumer(get_poly_coeff(x, (i + 1) * r, sum_uint64_count), get_poly_coeff(y, (i + 1) * r, sum_uint64_count), rec_coeff_count_power, sum_uint64_count, product_uint64_count, get_poly_coeff(z1, (i + 1) * r, product_uint64_count), get_poly_coeff(z2, (i + 1) * r, product_uint64_count), get_poly_coeff(z3, (i + 1) * r, product_uint64_count), pool);
                        }
                    }
                    else
                    {
                        for (int i = 0; i < 2 * m; i += 2)
                        {
                            do_nussbaumer_base_signcheck(get_poly_coeff(x, i * r, sum_uint64_count), get_poly_coeff(y, i * r, sum_uint64_count), rec_coeff_count_power, sum_uint64_count, product_uint64_count, get_poly_coeff(z1, i * r, product_uint64_count), get_poly_coeff(z2, i * r, product_uint64_count), get_poly_coeff(z3, i * r, product_uint64_count), fft_base_alloc_ptr);
                            do_nussbaumer_base_signcheck(get_poly_coeff(x, (i + 1) * r, sum_uint64_count), get_poly_coeff(y, (i + 1) * r, sum_uint64_count), rec_coeff_count_power, sum_uint64_count, product_uint64_count, get_poly_coeff(z1, (i + 1) * r, product_uint64_count), get_poly_coeff(z2, (i + 1) * r, product_uint64_count), get_poly_coeff(z3, (i + 1) * r, product_uint64_count), fft_base_alloc_ptr);
                        }
                    }

                    // Do second stage of FFT (Gentleman-Sande butterfly)
                    tools::symbolic_inverse_dft(z1, coeff_count_power, product_uint64_count, m, r, fft_base_alloc_ptr);

                    // Copy to result. Also done on the large size integers.  
                    tools::fold(z1, product_uint64_count, m, r, result_op1_op1);

                    // Now z2
                    tools::symbolic_inverse_dft(z2, coeff_count_power, product_uint64_count, m, r, fft_base_alloc_ptr);
                    tools::fold(z2, product_uint64_count, m, r, result_op2_op2);

                    // Finally z3
                    tools::symbolic_inverse_dft(z3, coeff_count_power, product_uint64_count, m, r, fft_base_alloc_ptr);
                    tools::fold(z3, product_uint64_count, m, r, result_op1_op2);
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
            int coeff_count = 1 << coeff_count_power;

            // Make sure operands have right size
            Pointer operandsresized(allocate_uint(2 * coeff_count * sum_uint64_count, pool));
            uint64_t *operand1resized = operandsresized.get();
            uint64_t *operand2resized = operandsresized.get() + coeff_count * sum_uint64_count;
            set_poly_poly(operand1, coeff_count, coeff_uint64_count, coeff_count, sum_uint64_count, operand1resized);
            set_poly_poly(operand2, coeff_count, coeff_uint64_count, coeff_count, sum_uint64_count, operand2resized);

            nussbaumer_multiply::do_nussbaumer(operand1resized, operand2resized, coeff_count_power, sum_uint64_count, product_uint64_count, result, pool);
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
            int coeff_count = 1 << coeff_count_power;

            // Make sure operands have right size
            Pointer operandsresized(allocate_uint(2 * coeff_count * sum_uint64_count, pool));
            uint64_t *operand1resized = operandsresized.get();
            uint64_t *operand2resized = operandsresized.get() + coeff_count * sum_uint64_count;
            set_poly_poly(operand1, coeff_count, coeff_uint64_count, coeff_count, sum_uint64_count, operand1resized);
            set_poly_poly(operand2, coeff_count, coeff_uint64_count, coeff_count, sum_uint64_count, operand2resized);

            nussbaumer_cross_multiply::do_nussbaumer(operand1resized, operand2resized, coeff_count_power, sum_uint64_count, product_uint64_count, result_op1_op1, result_op2_op2, result_op1_op2, pool);
        }
    }
}