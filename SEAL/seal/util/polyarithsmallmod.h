#pragma once

#include <cstdint>
#include <stdexcept>
#include "seal/smallmodulus.h"
#include "seal/util/polycore.h"
#include "seal/util/uintarithsmallmod.h"
#include "seal/util/mempool.h"
#include "seal/util/polymodulus.h"

namespace seal
{
    namespace util
    {
        inline void modulo_poly_coeffs(const std::uint64_t *poly, int coeff_count, const SmallModulus &modulus, 
            std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (poly == nullptr && coeff_count > 0)
            {
                throw std::invalid_argument("poly");
            }
            if (result == nullptr && coeff_count > 0)
            {
                throw std::invalid_argument("result");
            }
            if (coeff_count < 0)
            {
                throw std::invalid_argument("coeff_count");
            }
            if (modulus.is_zero())
            {
                throw std::invalid_argument("modulus");
            }
#endif
            const std::uint64_t modulus_value = modulus.value();
            for (int i = 0; i < coeff_count; i++)
            {
                *result++ = *poly++ % modulus_value;
            }
        }

        inline void negate_poly_coeffmod(const std::uint64_t *poly, int coeff_count, const SmallModulus &modulus,
            std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (poly == nullptr && coeff_count > 0)
            {
                throw std::invalid_argument("poly");
            }
            if (coeff_count < 0)
            {
                throw std::invalid_argument("coeff_count");
            }
            if (modulus.is_zero())
            {
                throw std::invalid_argument("modulus");
            }
            if (result == nullptr && coeff_count > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            const uint64_t modulus_value = modulus.value();
            for (int i = 0; i < coeff_count; i++)
            {
                // Explicit inline
                //*result++ = negate_uint_mod(*poly++, modulus);
#ifdef SEAL_DEBUG
                if (*poly >= modulus_value)
                {
                    throw std::out_of_range("poly");
                }
#endif
                std::int64_t non_zero = (*poly != 0);
                *result++ = (modulus_value - *poly++) & static_cast<std::uint64_t>(-non_zero);
            }
        }

        inline void add_poly_poly_coeffmod(const std::uint64_t *operand1, const std::uint64_t *operand2, 
            int coeff_count, const SmallModulus &modulus, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr && coeff_count > 0)
            {
                throw std::invalid_argument("operand1");
            }
            if (operand2 == nullptr && coeff_count > 0)
            {
                throw std::invalid_argument("operand2");
            }
            if (coeff_count < 0)
            {
                throw std::invalid_argument("coeff_count");
            }
            if (modulus.is_zero())
            {
                throw std::invalid_argument("modulus");
            }
            if (result == nullptr && coeff_count > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            const uint64_t modulus_value = modulus.value();
            for (int i = 0; i < coeff_count; i++)
            {
                // Explicit inline
                //result[i] = add_uint_uint_mod(operand1[i], operand2[i], modulus);
#ifdef SEAL_DEBUG
                if (*operand1 >= modulus_value)
                {
                    throw std::out_of_range("operand1");
                }
                if (*operand2 >= modulus_value)
                {
                    throw std::out_of_range("operand2");
                }
#endif
                std::uint64_t sum = *operand1++ + *operand2++;
                *result++ = sum - (modulus_value & static_cast<std::uint64_t>(-static_cast<std::int64_t>(sum >= modulus_value)));
            }
        }

        inline void sub_poly_poly_coeffmod(const std::uint64_t *operand1, const std::uint64_t *operand2, 
            int coeff_count, const SmallModulus &modulus, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr && coeff_count > 0)
            {
                throw std::invalid_argument("operand1");
            }
            if (operand2 == nullptr && coeff_count > 0)
            {
                throw std::invalid_argument("operand2");
            }
            if (coeff_count < 0)
            {
                throw std::invalid_argument("coeff_count");
            }
            if (modulus.is_zero())
            {
                throw std::invalid_argument("modulus");
            }
            if (result == nullptr && coeff_count > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            const uint64_t modulus_value = modulus.value();
            for (int i = 0; i < coeff_count; i++)
            {
                // Explicit inline
                //result[i] = sub_uint_uint_mod(operand1[i], operand2[i], modulus);
#ifdef SEAL_DEBUG
                if (*operand1 >= modulus_value)
                {
                    throw std::out_of_range("operand1");
                }
                if (*operand2 >= modulus_value)
                {
                    throw std::out_of_range("operand2");
                }
#endif
                std::int64_t borrow = SEAL_SUB_BORROW_UINT64(*operand1++, *operand2++, 0, result);
                *result++ += (modulus_value & static_cast<std::uint64_t>(-borrow));
            }
        }

        inline void multiply_poly_scalar_coeffmod(const std::uint64_t *poly, int coeff_count, std::uint64_t scalar,
            const SmallModulus &modulus, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (poly == nullptr && coeff_count > 0)
            {
                throw std::invalid_argument("poly");
            }
            if (coeff_count < 0)
            {
                throw std::invalid_argument("coeff_count");
            }
            if (result == nullptr && coeff_count > 0)
            {
                throw std::invalid_argument("result");
            }
            if (modulus.is_zero())
            {
                throw std::invalid_argument("modulus");
            }
#endif
            // Explicit inline
            //for (int i = 0; i < coeff_count; i++)
            //{
            //    *result++ = multiply_uint_uint_mod(*poly++, scalar, modulus);
            //}
            const std::uint64_t modulus_value = modulus.value();
            const std::uint64_t const_ratio_0 = modulus.const_ratio()[0];
            const std::uint64_t const_ratio_1 = modulus.const_ratio()[1];
            std::uint64_t z[2];
            std::uint64_t tmp1, tmp2[2], tmp3, carry;
            for (int i = 0; i < coeff_count; i++)
            {
                multiply_uint64(*poly++, scalar, z);
            
                // Reduces z using base 2^64 Barrett reduction
                
                // Multiply input and const_ratio
                // Round 1
                multiply_uint64_hw64(z[0], const_ratio_0, &carry);

                multiply_uint64(z[0], const_ratio_1, tmp2);
                tmp3 = tmp2[1] + add_uint64(tmp2[0], carry, 0, &tmp1);

                // Round 2
                multiply_uint64(z[1], const_ratio_0, tmp2);
                carry = tmp2[1] + add_uint64(tmp1, tmp2[0], 0, &tmp1);

                // This is all we care about
                tmp1 = z[1] * const_ratio_1 + tmp3 + carry;

                // Barrett subtraction
                tmp3 = z[0] - tmp1 * modulus_value;

                // Claim: One more subtraction is enough
                *result++ = tmp3 - (modulus_value & static_cast<uint64_t>(-static_cast<std::int64_t>(tmp3 >= modulus_value)));
            }
        }

        void multiply_poly_poly_coeffmod(const std::uint64_t *operand1, int operand1_coeff_count,
            const std::uint64_t *operand2, int operand2_coeff_count,
            const SmallModulus &modulus, int result_coeff_count, std::uint64_t *result);

        void multiply_poly_poly_coeffmod(const std::uint64_t *operand1, const std::uint64_t *operand2,
            int coeff_count, const SmallModulus &modulus, std::uint64_t *result);

        inline void multiply_truncate_poly_poly_coeffmod(const std::uint64_t *operand1, const std::uint64_t *operand2, 
            int coeff_count, const SmallModulus &modulus, std::uint64_t *result)
        {
            multiply_poly_poly_coeffmod(operand1, coeff_count, operand2, coeff_count, modulus, coeff_count, result);
        }

        void divide_poly_poly_coeffmod_inplace(std::uint64_t *numerator, const std::uint64_t *denominator,
            int coeff_count, const SmallModulus &modulus, std::uint64_t *quotient, MemoryPool &pool);

        inline void divide_poly_poly_coeffmod(const std::uint64_t *numerator, const std::uint64_t *denominator, 
            int coeff_count, const SmallModulus &modulus, std::uint64_t *quotient, std::uint64_t *remainder, MemoryPool &pool)
        {
            int coeff_uint64_count = modulus.uint64_count();
            set_poly_poly(numerator, coeff_count, coeff_uint64_count, remainder);
            divide_poly_poly_coeffmod_inplace(remainder, denominator, coeff_count, modulus, quotient, pool);
        }

        inline void add_bigpolyarray_coeffmod(const std::uint64_t *array1, const std::uint64_t *array2, int count,
            int coeff_count, const SmallModulus &modulus, std::uint64_t *result)
        {
            // Check validity of inputs
#ifdef SEAL_DEBUG
            if (array1 == nullptr)
            {
                throw std::invalid_argument("array1");
            }
            if (array2 == nullptr)
            {
                throw std::invalid_argument("array2");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
            if (count < 1)
            {
                throw std::invalid_argument("count");
            }
            if (coeff_count < 1)
            {
                throw std::invalid_argument("coeff_count");
            }
            if (modulus.is_zero())
            {
                throw std::invalid_argument("modulus");
            }
#endif
            // initialise pointers for addition
            const uint64_t *current_array1 = array1;
            const uint64_t *current_array2 = array2;
            uint64_t *current_result = result;

            for (int i = 0; i < count; i++)
            {
                add_poly_poly_coeffmod(current_array1, current_array2, coeff_count, modulus, current_result);
                current_array1 += coeff_count;
                current_array2 += coeff_count;
                current_result += coeff_count;
            }
        }

        inline void apply_galois(const std::uint64_t *input, int coeff_count_power, std::uint64_t galois_elt,
            const SmallModulus &modulus, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (input == nullptr)
            {
                throw std::invalid_argument("input");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
            if (input == result)
            {
                throw std::invalid_argument("result cannot point to the same value as input");
            }
            if (coeff_count_power <= 0)
            {
                throw std::invalid_argument("coeff_count_power");
            }
            // Verify coprime conditions.
            if (!(galois_elt & 1) || galois_elt >= (1ULL << (coeff_count_power + 1)))
            {
                throw std::invalid_argument("galois element is not valid");
            }
            if (modulus.is_zero())
            {
                throw std::invalid_argument("modulus");
            }
#endif
            const std::uint64_t modulus_value = modulus.value();
            std::uint64_t coeff_count_minus_one = (1ULL << coeff_count_power) - 1;
            for (std::uint64_t i = 0; i <= coeff_count_minus_one; i++)
            {
                std::uint64_t index_raw = i * galois_elt;
                std::uint64_t index = index_raw & coeff_count_minus_one;
                std::uint64_t result_value = *input++;
                if ((index_raw >> coeff_count_power) & 1)
                {
                    // Explicit inline
                    //result[index] = negate_uint_mod(result[index], modulus);
                    std::int64_t non_zero = (result_value != 0);
                    result_value = (modulus_value - result_value) & static_cast<std::uint64_t>(-non_zero);
                }
                result[index] = result_value;
            }
        }

        inline void apply_galois_ntt(const std::uint64_t *input, int coeff_count_power, std::uint64_t galois_elt,
            std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (input == nullptr)
            {
                throw std::invalid_argument("input");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
            if (input == result)
            {
                throw std::invalid_argument("result cannot point to the same value as input");
            }
            if (coeff_count_power <= 0)
            {
                throw std::invalid_argument("coeff_count_power");
            }
            // Verify coprime conditions.
            if (!(galois_elt & 1) || galois_elt >= (1ULL << (coeff_count_power + 1)))
            {
                throw std::invalid_argument("galois element is not valid");
            }
#endif
            std::uint32_t coeff_count = 1U << coeff_count_power;
            std::uint32_t m_minus_one = 2 * coeff_count;
            for (std::uint32_t i = 0; i < coeff_count; i++)
            {
                std::uint32_t reversed = reverse_bits(i, coeff_count_power);
                std::uint64_t index_raw = galois_elt * (2 * reversed + 1);
                index_raw &= m_minus_one;
                std::uint32_t index = reverse_bits((static_cast<std::uint32_t>(index_raw) - 1) >> 1, coeff_count_power);
                result[i] = input[index];
            }
        }

        inline void dyadic_product_coeffmod(const std::uint64_t *operand1, const std::uint64_t *operand2, 
            int coeff_count, const SmallModulus &modulus, std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr)
            {
                throw std::invalid_argument("operand1");
            }
            if (operand2 == nullptr)
            {
                throw std::invalid_argument("operand2");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
            if (coeff_count <= 0)
            {
                throw std::invalid_argument("coeff_count");
            }
            if (modulus.is_zero())
            {
                throw std::invalid_argument("modulus");
            }
#endif
            // Explicit inline
            //for (int i = 0; i < coeff_count; i++)
            //{
            //    *result++ = multiply_uint_uint_mod(*operand1++, *operand2++, modulus);
            //}
            const std::uint64_t modulus_value = modulus.value();
            const std::uint64_t const_ratio_0 = modulus.const_ratio()[0];
            const std::uint64_t const_ratio_1 = modulus.const_ratio()[1];
            for (int i = 0; i < coeff_count; i++)
            {
                std::uint64_t z[2];
                multiply_uint64(*operand1++, *operand2++, z);

                // Reduces z using base 2^64 Barrett reduction
                std::uint64_t tmp1, tmp2[2], tmp3, carry;

                // Multiply input and const_ratio
                // Round 1
                multiply_uint64_hw64(z[0], const_ratio_0, &carry);

                multiply_uint64(z[0], const_ratio_1, tmp2);
                tmp3 = tmp2[1] + add_uint64(tmp2[0], carry, 0, &tmp1);

                // Round 2
                multiply_uint64(z[1], const_ratio_0, tmp2);
                carry = tmp2[1] + add_uint64(tmp1, tmp2[0], 0, &tmp1);

                // This is all we care about
                tmp1 = z[1] * const_ratio_1 + tmp3 + carry;

                // Barrett subtraction
                tmp3 = z[0] - tmp1 * modulus_value;

                // Claim: One more subtraction is enough
                *result++ = tmp3 - (modulus_value & static_cast<uint64_t>(-static_cast<std::int64_t>(tmp3 >= modulus_value)));
            }
        }

        void modulo_poly_inplace(std::uint64_t *value, int value_coeff_count, const PolyModulus &poly_modulus,
            const SmallModulus &modulus);

        inline void modulo_poly(const std::uint64_t *value, int value_coeff_count, const PolyModulus &poly_modulus,
            const SmallModulus &modulus, std::uint64_t *result, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
            if (value == nullptr)
            {
                throw std::invalid_argument("value");
            }
            if (value_coeff_count <= 0)
            {
                throw std::invalid_argument("value_coeff_count");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
            if (modulus.is_zero())
            {
                throw std::invalid_argument("modulus");
            }
#endif
            int coeff_uint64_count = modulus.uint64_count();
            Pointer value_copy(allocate_poly(value_coeff_count, coeff_uint64_count, pool));
            set_poly_poly(value, value_coeff_count, coeff_uint64_count, value_copy.get());
            modulo_poly_inplace(value_copy.get(), value_coeff_count, poly_modulus, modulus);
            set_poly_poly(value_copy.get(), poly_modulus.coeff_count(), coeff_uint64_count, result);
        }

        inline void nonfft_multiply_poly_poly_polymod_coeffmod(const std::uint64_t *operand1, 
            const std::uint64_t *operand2, const PolyModulus &poly_modulus, const SmallModulus &modulus, 
            std::uint64_t *result, MemoryPool &pool)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr)
            {
                throw std::invalid_argument("operand1");
            }
            if (operand2 == nullptr)
            {
                throw std::invalid_argument("operand2");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
            if (get_significant_coeff_count_poly(operand1, poly_modulus.coeff_count(), poly_modulus.coeff_uint64_count()) >= poly_modulus.coeff_count())
            {
                throw std::out_of_range("operand1");
            }
            if (get_significant_coeff_count_poly(operand2, poly_modulus.coeff_count(), poly_modulus.coeff_uint64_count()) >= poly_modulus.coeff_count())
            {
                throw std::out_of_range("operand2");
            }
            if (modulus.is_zero())
            {
                throw std::invalid_argument("modulus");
            }
#endif
            // Calculate normal product.
            int coeff_count = poly_modulus.coeff_count();
            int intermediate_coeff_count = coeff_count * 2 - 1;
            Pointer intermediate(allocate_uint(intermediate_coeff_count, pool));
            multiply_poly_poly_coeffmod(operand1, operand2, coeff_count, modulus, intermediate.get());

            // Perform modulo operation.
            modulo_poly_inplace(intermediate.get(), intermediate_coeff_count, poly_modulus, modulus);

            // Copy to result.
            set_uint_uint(intermediate.get(), coeff_count, result);
        }

        inline void nonfft_multiply_poly_poly_polymod_coeffmod_inplace(const std::uint64_t *operand1,
            const std::uint64_t *operand2, const PolyModulus &poly_modulus, const SmallModulus &modulus,
            std::uint64_t *result)
        {
#ifdef SEAL_DEBUG
            if (operand1 == nullptr)
            {
                throw std::invalid_argument("operand1");
            }
            if (operand2 == nullptr)
            {
                throw std::invalid_argument("operand2");
            }
            if (result == nullptr)
            {
                throw std::invalid_argument("result");
            }
            if (get_significant_coeff_count_poly(operand1, poly_modulus.coeff_count(), poly_modulus.coeff_uint64_count()) >= poly_modulus.coeff_count())
            {
                throw std::out_of_range("operand1");
            }
            if (get_significant_coeff_count_poly(operand2, poly_modulus.coeff_count(), poly_modulus.coeff_uint64_count()) >= poly_modulus.coeff_count())
            {
                throw std::out_of_range("operand2");
            }
            if (modulus.is_zero())
            {
                throw std::invalid_argument("modulus");
            }
#endif
            // Calculate normal product.
            int coeff_count = poly_modulus.coeff_count();
            int result_coeff_count = coeff_count * 2 - 1;
            multiply_poly_poly_coeffmod(operand1, operand2, coeff_count, modulus, result);

            // Perform modulo operation.
            modulo_poly_inplace(result, result_coeff_count, poly_modulus, modulus);
        }

        std::uint64_t poly_infty_norm_coeffmod(const std::uint64_t *poly, int poly_coeff_count,
            const SmallModulus &modulus);

        bool try_invert_poly_coeffmod(const std::uint64_t *operand, const std::uint64_t *poly_modulus, 
            int coeff_count, const SmallModulus &modulus, std::uint64_t *result, MemoryPool &pool);

        void exponentiate_poly_polymod_coeffmod(const std::uint64_t *poly, const std::uint64_t *exponent,
            int exponent_uint64_count, const PolyModulus &poly_modulus, const SmallModulus &modulus, 
            std::uint64_t *result, MemoryPool &pool);
    }
}
