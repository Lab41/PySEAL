#include "util/ntt.h"
#include "util/polyarith.h"
#include "util/uintarith.h"
#include "util/modulus.h"
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
                // Shifts operand left by one bit
                inline void left_shift_one_bit_uint(const uint64_t *operand, int uint64_count, uint64_t *result)
                {
                    result += (uint64_count - 1);
                    operand += (uint64_count - 1);

                    for (int i = 0; i < uint64_count - 1; i++)
                    {
                        *result = *operand-- << 1;
                        *result-- |= (*operand >> (bits_per_uint64 - 1));
                    }
                    *result = (*operand << 1);
                }

                inline void right_shift_one_bit_uint(const uint64_t *operand, int uint64_count, uint64_t *result)
                {
                    for (; uint64_count >= 2; uint64_count--)
                    {
                        *result = *operand++ >> 1;
                        *result++ |= *operand << (bits_per_uint64 - 1);
                    }
                    *result = *operand >> 1;
                }

                void multiply_truncate_uint_uint_add(const uint64_t *operand1, const uint64_t *operand2, int uint64_count, uint64_t *result)
                {
                    // Handle fast case
                    if (uint64_count == 1)
                    {
                        *result += *operand1 * *operand2;
                        return;
                    }

                    // Multiply operand1 and operand2.            
                    uint64_t temp_result;
                    uint64_t carry, new_carry;
                    const uint64_t *inner_operand2;
                    uint64_t *inner_result;

                    for (int operand1_index = 0; operand1_index < uint64_count; operand1_index++)
                    {
                        inner_operand2 = operand2;
                        inner_result = result++;
                        carry = 0;
                        int operand2_index = 0;
                        for (; operand2_index < uint64_count - operand1_index; operand2_index++)
                        {
                            // Perform 64-bit multiplication of operand1 and operand2
                            temp_result = multiply_uint64_uint64(*operand1, *inner_operand2++, &new_carry);
                            carry = new_carry + add_uint64_uint64(temp_result, carry, 0, &temp_result);
                            carry += add_uint64_uint64(*inner_result, temp_result, 0, inner_result);

                            inner_result++;
                        }

                        operand1++;
                    }
                }

                void multiply_truncate_uint_uint_sub(const uint64_t *operand1, const uint64_t *operand2, int uint64_count, uint64_t *result)
                {
                    uint64_t *temp_ptr = result;
                    for (int k = 0; k < uint64_count; k++)
                    {
                        *temp_ptr++ ^= 0xFFFFFFFFFFFFFFFF;
                    }
                    multiply_truncate_uint_uint_add(operand1, operand2, uint64_count, result);
                    for (int k = 0; k < uint64_count; k++)
                    {
                        *result++ ^= 0xFFFFFFFFFFFFFFFF;
                    }
                }

                // Assume destination has uint64_count twice that of operands
                void multiply_uint_uint(const uint64_t *operand1, const uint64_t *operand2, int operand_uint64_count, uint64_t *result)
                {
                    int result_uint64_count = 2 * operand_uint64_count;

                    // Handle fast cases.
                    if (operand_uint64_count == 0)
                    {
                        // If either operand is 0, then result is 0.
                        set_zero_uint(result_uint64_count, result);
                        return;
                    }

                    // Clear out result.
                    set_zero_uint(result_uint64_count, result);

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
                        for (int operand2_index = 0; operand2_index < operand_uint64_count; operand2_index++)
                        {
                            // Perform 64-bit multiplication of operand1 and operand2
                            temp_result = multiply_uint64_uint64(*operand1, *inner_operand2++, &new_carry);
                            carry = new_carry + add_uint64_uint64(temp_result, carry, 0, &temp_result);
                            carry += add_uint64_uint64(*inner_result, temp_result, 0, inner_result);

                            inner_result++;
                        }

                        // Write carry
                        *inner_result = carry;

                        operand1++;
                    }
                }
            }
        }

        NTTTables::NTTTables(const MemoryPoolHandle &pool) : pool_(pool), generated_(false)
        {
        }

        NTTTables::NTTTables(int coeff_count_power, const Modulus &modulus, const MemoryPoolHandle &pool) : pool_(pool)
        {
            generate(coeff_count_power, modulus);
        }

        void NTTTables::reset()
        {
            generated_ = false;
            modulus_ = Modulus();
            modulus_alloc_.release();
            root_.release();
            root_powers_.release();
            scaled_root_powers_.release();
            inv_root_powers_.release();
            scaled_inv_root_powers_.release();
            inv_root_powers_div_two_.release();
            scaled_inv_root_powers_div_two_.release();
            inv_degree_modulo_.release();
            coeff_count_power_ = 0;
            coeff_count_ = 0;
            coeff_uint64_count_ = 0;
        }

        bool NTTTables::generate(int coeff_count_power, const Modulus &modulus)
        {
            int word_bit_count = modulus.uint64_count() * bits_per_uint64;

            // Verify that the modulus is not too close to a multiple of the word size
            if (modulus.significant_bit_count() >= word_bit_count - 2)
            {
                reset();
                return false;
            }

            reset();

            coeff_count_power_ = coeff_count_power;
            coeff_count_ = 1 << coeff_count_power_;
            coeff_uint64_count_ = modulus.uint64_count();

            // Allocate memory for modulus, the tables, and for the inverse of degree modulo modulus
            modulus_alloc_ = allocate_uint(coeff_uint64_count_, pool_);
            root_ = allocate_uint(coeff_uint64_count_, pool_);
            root_powers_ = allocate_uint(coeff_count_ * coeff_uint64_count_, pool_);
            inv_root_powers_ = allocate_uint(coeff_count_ * coeff_uint64_count_, pool_);
            scaled_root_powers_ = allocate_uint(coeff_count_ * coeff_uint64_count_, pool_);
            scaled_inv_root_powers_ = allocate_uint(coeff_count_ * coeff_uint64_count_, pool_);
            inv_root_powers_div_two_ = allocate_uint(coeff_count_ * coeff_uint64_count_, pool_);
            scaled_inv_root_powers_div_two_ = allocate_uint(coeff_count_ * coeff_uint64_count_, pool_);
            inv_degree_modulo_ = allocate_uint(coeff_uint64_count_, pool_);

            // Copy the value in modulus and create the local copy modulus_
            set_uint_uint(modulus.get(), coeff_uint64_count_, modulus_alloc_.get());
            modulus_ = Modulus(modulus_alloc_.get(), coeff_uint64_count_, pool_);

            Pointer inverse_root(allocate_uint(coeff_uint64_count_, pool_));

            // We defer parameter checking to try_minimal_primitive_root(...)
            if (!try_minimal_primitive_root(2 * coeff_count_, modulus_, pool_, root_.get()))
            {
                reset();
                return false;
            }
            if (!try_invert_uint_mod(root_.get(), modulus_.get(), coeff_uint64_count_, inverse_root.get(), pool_))
            {
                reset();
                return false;
            }

            // Populate the tables storing (scaled version of) powers of root mod q in bit-scrambled order.  
            ntt_powers_of_primitive_root(root_.get(), root_powers_.get());
            ntt_scale_powers_of_primitive_root(root_powers_.get(), scaled_root_powers_.get());

            // Populate the tables storing (scaled version of) powers of (root)^{-1} mod q in bit-scrambled order.  
            ntt_powers_of_primitive_root(inverse_root.get(), inv_root_powers_.get());
            ntt_scale_powers_of_primitive_root(inv_root_powers_.get(), scaled_inv_root_powers_.get());

            // Populate the tables storing (scaled version of ) 2 times powers of roots^-1 mod q  in bit-scrambled order. 
            uint64_t *inv_root_powers_ptr = inv_root_powers_.get(); 
            uint64_t *inv_root_powers_div_two_ptr = inv_root_powers_div_two_.get();      

            for (int i = 0; i < coeff_count_; ++i)
            {
                div2_uint_mod(inv_root_powers_ptr, modulus_.get(), coeff_uint64_count_, inv_root_powers_div_two_ptr);
                inv_root_powers_ptr += coeff_uint64_count_;
                inv_root_powers_div_two_ptr += coeff_uint64_count_; 
            }
            ntt_scale_powers_of_primitive_root(inv_root_powers_div_two_.get(), scaled_inv_root_powers_div_two_.get());

            // Last compute n^(-1) modulo q. 
            Pointer degree_uint(allocate_zero_uint(coeff_uint64_count_, pool_));
            *degree_uint.get() = coeff_count_;
            generated_ = try_invert_uint_mod(degree_uint.get(), modulus_.get(), coeff_uint64_count_, inv_degree_modulo_.get(), pool_);
            
            if (!generated_)
            {
                reset();
                return false;
            }
            return true;
        }

        NTTTables::NTTTables(const NTTTables &copy) : pool_(copy.pool_), generated_(copy.generated_), coeff_count_power_(copy.coeff_count_power_),
            coeff_count_(copy.coeff_count_), coeff_uint64_count_(copy.coeff_uint64_count_)
        {
            if (generated_)
            {
                // Copy modulus
                modulus_alloc_ = allocate_uint(coeff_uint64_count_, pool_);
                set_uint_uint(copy.modulus_alloc_.get(), coeff_uint64_count_, modulus_alloc_.get());
                modulus_ = Modulus(modulus_alloc_.get(), coeff_uint64_count_, pool_);

                // Allocate space and copy tables
                root_ = allocate_uint(coeff_uint64_count_, pool_);
                set_uint_uint(copy.root_.get(), coeff_uint64_count_, root_.get());

                root_powers_ = allocate_uint(coeff_count_ * coeff_uint64_count_, pool_);
                set_uint_uint(copy.root_powers_.get(), coeff_count_ * coeff_uint64_count_, root_powers_.get());

                inv_root_powers_ = allocate_uint(coeff_count_ * coeff_uint64_count_, pool_);
                set_uint_uint(copy.inv_root_powers_.get(), coeff_count_ * coeff_uint64_count_, inv_root_powers_.get());

                scaled_root_powers_ = allocate_uint(coeff_count_ * coeff_uint64_count_, pool_);
                set_uint_uint(copy.scaled_root_powers_.get(), coeff_count_ * coeff_uint64_count_, scaled_root_powers_.get());

                scaled_inv_root_powers_ = allocate_uint(coeff_count_ * coeff_uint64_count_, pool_);
                set_uint_uint(copy.scaled_inv_root_powers_.get(), coeff_count_ * coeff_uint64_count_, scaled_inv_root_powers_.get());

                inv_root_powers_div_two_ = allocate_uint(coeff_count_ * coeff_uint64_count_, pool_);
                set_uint_uint(copy.inv_root_powers_div_two_.get(), coeff_count_ * coeff_uint64_count_, inv_root_powers_div_two_.get());

                scaled_inv_root_powers_div_two_ = allocate_uint(coeff_count_ * coeff_uint64_count_, pool_);
                set_uint_uint(copy.scaled_inv_root_powers_div_two_.get(), coeff_count_ * coeff_uint64_count_, scaled_inv_root_powers_div_two_.get());

                inv_degree_modulo_ = allocate_uint(coeff_uint64_count_, pool_);
                set_uint_uint(copy.inv_degree_modulo_.get(), coeff_uint64_count_, inv_degree_modulo_.get());
            }
        }

        NTTTables &NTTTables::operator =(const NTTTables &assign)
        {
            // Check for self-assignment
            if (this == &assign)
            {
                return *this;
            }

            generated_ = assign.generated_;
            coeff_count_power_ = assign.coeff_count_power_;
            coeff_count_ = assign.coeff_count_;
            coeff_uint64_count_ = assign.coeff_uint64_count_;

            if (generated_)
            {
                // Copy modulus
                modulus_alloc_ = allocate_uint(coeff_uint64_count_, pool_);
                set_uint_uint(assign.modulus_alloc_.get(), coeff_uint64_count_, modulus_alloc_.get());
                modulus_ = Modulus(modulus_alloc_.get(), coeff_uint64_count_, pool_);

                // Allocate space and copy tables
                root_ = allocate_uint(coeff_uint64_count_, pool_);
                set_uint_uint(assign.root_.get(), coeff_uint64_count_, root_.get());

                root_powers_ = allocate_uint(coeff_count_ * coeff_uint64_count_, pool_);
                set_uint_uint(assign.root_powers_.get(), coeff_count_ * coeff_uint64_count_, root_powers_.get());

                inv_root_powers_ = allocate_uint(coeff_count_ * coeff_uint64_count_, pool_);
                set_uint_uint(assign.inv_root_powers_.get(), coeff_count_ * coeff_uint64_count_, inv_root_powers_.get());

                scaled_root_powers_ = allocate_uint(coeff_count_ * coeff_uint64_count_, pool_);
                set_uint_uint(assign.scaled_root_powers_.get(), coeff_count_ * coeff_uint64_count_, scaled_root_powers_.get());

                scaled_inv_root_powers_ = allocate_uint(coeff_count_ * coeff_uint64_count_, pool_);
                set_uint_uint(assign.scaled_inv_root_powers_.get(), coeff_count_ * coeff_uint64_count_, scaled_inv_root_powers_.get());

                inv_root_powers_div_two_ = allocate_uint(coeff_count_ * coeff_uint64_count_, pool_);
                set_uint_uint(assign.inv_root_powers_div_two_.get(), coeff_count_ * coeff_uint64_count_, inv_root_powers_div_two_.get());

                scaled_inv_root_powers_div_two_ = allocate_uint(coeff_count_ * coeff_uint64_count_, pool_);
                set_uint_uint(assign.scaled_inv_root_powers_div_two_.get(), coeff_count_ * coeff_uint64_count_, scaled_inv_root_powers_div_two_.get());

                inv_degree_modulo_ = allocate_uint(coeff_uint64_count_, pool_);
                set_uint_uint(assign.inv_degree_modulo_.get(), coeff_uint64_count_, inv_degree_modulo_.get());
            }

            return *this;
        }

        void NTTTables::ntt_powers_of_primitive_root(uint64_t *root, uint64_t *destination)
        {
            uint64_t *destination_start = destination;
            uint64_t *next_destination;
            set_uint(1, coeff_uint64_count_, destination_start);
            for (int i = 1; i < coeff_count_; ++i)
            {
                next_destination = destination_start + coeff_uint64_count_ * (reverse_bits(static_cast<uint32_t>(i)) >> (32 - coeff_count_power_));
                multiply_uint_uint_mod(destination, root, modulus_, next_destination, pool_);
                destination = next_destination;
            }
        }

        // compute floor ( input * beta /q ), where beta is a 64k power of 2 and  0 < q < beta. 
        void NTTTables::ntt_scale_powers_of_primitive_root(uint64_t *input, uint64_t *destination)
        {
            int wide_uint64_count = 2 * coeff_uint64_count_;

            Pointer wide_coeff(allocate_uint(wide_uint64_count, pool_));
            Pointer wide_modulus(allocate_uint(wide_uint64_count, pool_));
            set_uint_uint(modulus_.get(), coeff_uint64_count_, wide_uint64_count, wide_modulus.get());

            Pointer wide_quotient(allocate_uint(wide_uint64_count, pool_));
            Pointer wide_remainder(allocate_uint(wide_uint64_count, pool_));

            for (int i = 0; i < coeff_count_; ++i)
            {
                set_uint_uint(input, coeff_uint64_count_, wide_uint64_count, wide_coeff.get());
                left_shift_uint(wide_coeff.get(), coeff_uint64_count_ * bits_per_uint64, wide_uint64_count, wide_coeff.get());
                divide_uint_uint(wide_coeff.get(), wide_modulus.get(), wide_uint64_count, wide_quotient.get(), wide_remainder.get(), pool_);
                set_uint_uint(wide_quotient.get(), wide_uint64_count, coeff_uint64_count_, destination);
                input += coeff_uint64_count_;
                destination += coeff_uint64_count_;
            }
        }

        /**
        This function computes in-place the negacyclic NTT. The input is a polynomial a of degree n in R_q,
        where n is assumed to be a power of 2 and q is a prime such that q = 1 (mod 2n).

        The output is a vector A such that the following hold:
        A[j] =  a(psi**(2*bit_reverse(j) + 1)), 0 <= j < n.

        For details, see Michael Naehrig and Patrick Longa.
        */
        void ntt_negacyclic_harvey(uint64_t *operand, const NTTTables &tables, MemoryPool &pool)
        {
            int coeff_uint64_count = tables.coeff_uint64_count(); 

            const uint64_t *modulusptr = tables.modulus().get(); 
            Pointer big_alloc(allocate_uint(5 * coeff_uint64_count, pool));

            uint64_t *two_times_modulus = big_alloc.get();
            uint64_t *T = two_times_modulus + coeff_uint64_count;
            uint64_t *prod = T + coeff_uint64_count;
            uint64_t *temp = prod + 2 * coeff_uint64_count;

            tools::left_shift_one_bit_uint(modulusptr, coeff_uint64_count, two_times_modulus);

            // Return the NTT in scrambled order
            uint64_t *Q = prod + coeff_uint64_count;
            int n = 1 << tables.coeff_count_power();
            int t = n;
            for (int m = 1; m < n; m <<= 1)
            {
                t >>= 1;
                for (int i = 0; i < m; i++)
                {
                    int j1 = 2 * i * t;
                    int j2 = j1 + t - 1;
                    const uint64_t *W = tables.get_from_root_powers(m + i);
                    const uint64_t *Wprime = tables.get_from_scaled_root_powers(m + i);
                    uint64_t *X = operand + j1 * coeff_uint64_count;
                    uint64_t *Y = X + t * coeff_uint64_count;
                    for (int j = j1; j <= j2; j++)
                    {
                        // The Harvey butterfly: assume X, Y in [0, 2p), and return X', Y' in [0, 2p).
                        // X', Y' = X + WY, X - WY (mod p).

                        if (is_greater_than_or_equal_uint_uint(X, two_times_modulus, coeff_uint64_count))
                        {
                            sub_uint_uint(X, two_times_modulus, coeff_uint64_count, X);
                        }

                        tools::multiply_uint_uint(Wprime, Y, coeff_uint64_count, prod);
                        multiply_truncate_uint_uint(W, Y, coeff_uint64_count, T);
                        tools::multiply_truncate_uint_uint_sub(Q, modulusptr, coeff_uint64_count, T);
                        sub_uint_uint(two_times_modulus, T, coeff_uint64_count, temp);
                        add_uint_uint(X, temp, coeff_uint64_count, Y);
                        add_uint_uint(X, T, coeff_uint64_count, X);

                        X += coeff_uint64_count;
                        Y += coeff_uint64_count;
                    }
                }
            }
            // Finally maybe we need to reduce everything modulo q, but we know that they are in the range [0, 4q). 
            // If word size is controlled this should not be a problem. We can use modulo_poly_coeffs in this case. 
            for (int i = 0; i < n; i++)
            {
                if (is_greater_than_or_equal_uint_uint(operand, two_times_modulus, coeff_uint64_count))
                {
                    sub_uint_uint(operand, two_times_modulus, coeff_uint64_count, operand);
                }
                if (is_greater_than_or_equal_uint_uint(operand, modulusptr, coeff_uint64_count))
                {
                    sub_uint_uint(operand, modulusptr, coeff_uint64_count, operand);
                }
                operand += coeff_uint64_count;
            }
        }

        // Inverse negacyclic NTT using Harvey's butterfly. (See Patrick Longa and Michael Naehrig). 
        void inverse_ntt_negacyclic_harvey(uint64_t *operand, const NTTTables &tables, MemoryPool &pool)
        {
            int coeff_uint64_count = tables.coeff_uint64_count();

            const uint64_t *modulusptr = tables.modulus().get();
            Pointer big_alloc(allocate_uint(4 * coeff_uint64_count, pool));
            uint64_t *two_times_modulus = big_alloc.get();
            uint64_t *T = two_times_modulus + coeff_uint64_count;
            uint64_t *prod = T + coeff_uint64_count;

            tools::left_shift_one_bit_uint(modulusptr, coeff_uint64_count, two_times_modulus);

            // return the bit-reversed order of NTT. 
            uint64_t *Q = prod + coeff_uint64_count;
            int n = 1 << tables.coeff_count_power();
            int t = 1;
            for (int m = n; m > 1; m >>= 1)
            {
                int j1 = 0;
                int h = m >> 1;
                for (int i = 0; i < h; i++)
                {
                    int j2 = j1 + t - 1;
                    // Need the powers of  phi^{-1} in bit-reversed order
                    const uint64_t *W = tables.get_from_inv_root_powers_div_two(h + i);
                    const uint64_t *Wprime = tables.get_from_scaled_inv_root_powers_div_two(h + i);
                    uint64_t *U = operand + j1 * coeff_uint64_count;
                    uint64_t *V = U + t * coeff_uint64_count;
                    for (int j = j1; j <= j2; j++)
                    {
                        // U = x[i], V = x[i+m]

                        // Compute U - V + 2q
                        sub_uint_uint(two_times_modulus, V, coeff_uint64_count, T);
                        add_uint_uint(T, U, coeff_uint64_count, T);

                        add_uint_uint(U, V, coeff_uint64_count, U);
                        if (is_greater_than_or_equal_uint_uint(U, two_times_modulus, coeff_uint64_count))
                        {
                            sub_uint_uint(U, two_times_modulus, coeff_uint64_count, U);
                        }

                        // need to make it so that div2_uint_mod takes values that are > q. 
                        //div2_uint_mod(U, modulusptr, coeff_uint64_count, U); 
                        if (*U & 1)
                        {
                            uint64_t carry = static_cast<uint64_t>(add_uint_uint(U, modulusptr, coeff_uint64_count, U));
                            tools::right_shift_one_bit_uint(U, coeff_uint64_count, U);
                            *(U + coeff_uint64_count - 1) |=  (carry << (bits_per_uint64 - 1));
                        }
                        else
                        {
                            tools::right_shift_one_bit_uint(U, coeff_uint64_count, U);
                        }

                        tools::multiply_uint_uint(Wprime, T, coeff_uint64_count, prod);

                        // effectively, the next two multiply perform multiply modulo beta = 2**wordsize. 
                        multiply_truncate_uint_uint(W, T, coeff_uint64_count, V);
                        tools::multiply_truncate_uint_uint_sub(Q, modulusptr, coeff_uint64_count, V);

                        U += coeff_uint64_count;
                        V += coeff_uint64_count;
                    }
                    j1 += (t << 1); 
                }
                t <<= 1;
            }

            // final adjustments; compute a[j] = a[j] * n^{-1} mod q. 
            // We incorporate the final adjustment in the butterfly. 
            for (int i = 0; i < n; i++)
            {
                if (is_greater_than_or_equal_uint_uint(operand, two_times_modulus, coeff_uint64_count))
                {
                    sub_uint_uint(operand, two_times_modulus, coeff_uint64_count, operand);
                }
                if (is_greater_than_or_equal_uint_uint(operand, modulusptr, coeff_uint64_count))
                {
                    sub_uint_uint(operand, modulusptr, coeff_uint64_count, operand);
                }
                operand += coeff_uint64_count;
            }
        }
    }
}