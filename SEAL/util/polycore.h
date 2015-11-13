#ifndef SEAL_UTIL_POLYCORE_H
#define SEAL_UTIL_POLYCORE_H

#include <cstdint>
#include <stdexcept>
#include "util/mempool.h"

namespace seal
{
    namespace util
    {
        inline Pointer allocate_poly(int coeff_count, int coeff_uint64_count, MemoryPool &pool)
        {
#ifdef _DEBUG
            if (coeff_count < 0)
            {
                throw std::invalid_argument("coeff_count");
            }
            if (coeff_uint64_count < 0)
            {
                throw std::invalid_argument("coeff_uint64_count");
            }
#endif
            int poly_uint64_count = coeff_count * coeff_uint64_count;
            return pool.get_for_uint64_count(poly_uint64_count);
        }

        inline void set_zero_poly(int coeff_count, int coeff_uint64_count, std::uint64_t* result)
        {
#ifdef _DEBUG
            if (coeff_count < 0)
            {
                throw std::invalid_argument("coeff_count");
            }
            if (coeff_uint64_count < 0)
            {
                throw std::invalid_argument("coeff_uint64_count");
            }
            if (result == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            int poly_uint64_count = coeff_count * coeff_uint64_count;
            for (int i = 0; i < poly_uint64_count; ++i)
            {
                *result++ = 0;
            }
        }

        inline Pointer allocate_zero_poly(int coeff_count, int coeff_uint64_count, MemoryPool &pool)
        {
#ifdef _DEBUG
            if (coeff_count < 0)
            {
                throw std::invalid_argument("coeff_count");
            }
            if (coeff_uint64_count < 0)
            {
                throw std::invalid_argument("coeff_uint64_count");
            }
#endif
            Pointer allocated = allocate_poly(coeff_count, coeff_uint64_count, pool);
            set_zero_poly(coeff_count, coeff_uint64_count, allocated.get());
            return allocated;
        }

        inline std::uint64_t *get_poly_coeff(std::uint64_t *poly, int coeff_index, int coeff_uint64_count)
        {
#ifdef _DEBUG
            if (poly == nullptr)
            {
                throw std::invalid_argument("poly");
            }
            if (coeff_index < 0)
            {
                throw std::invalid_argument("coeff_index");
            }
            if (coeff_uint64_count < 0)
            {
                throw std::invalid_argument("coeff_uint64_count");
            }
#endif
            return poly + coeff_index * coeff_uint64_count;
        }

        inline const std::uint64_t *get_poly_coeff(const std::uint64_t *poly, int coeff_index, int coeff_uint64_count)
        {
#ifdef _DEBUG
            if (poly == nullptr)
            {
                throw std::invalid_argument("poly");
            }
            if (coeff_index < 0)
            {
                throw std::invalid_argument("coeff_index");
            }
            if (coeff_uint64_count < 0)
            {
                throw std::invalid_argument("coeff_uint64_count");
            }
#endif
            return poly + coeff_index * coeff_uint64_count;
        }

        inline void set_poly_poly(const std::uint64_t *poly, int coeff_count, int coeff_uint64_count, std::uint64_t *result)
        {
#ifdef _DEBUG
            if (poly == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw std::invalid_argument("poly");
            }
            if (coeff_count < 0)
            {
                throw std::invalid_argument("coeff_count");
            }
            if (coeff_uint64_count < 0)
            {
                throw std::invalid_argument("coeff_uint64_count");
            }
            if (result == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            if (poly == result)
            {
                // Fast path to handle self-assignment.
                return;
            }
            int poly_uint64_count = coeff_count * coeff_uint64_count;
            for (int i = 0; i < poly_uint64_count; ++i)
            {
                *result++ = *poly++;
            }
        }

        inline bool is_zero_poly(const std::uint64_t *poly, int coeff_count, int coeff_uint64_count)
        {
#ifdef _DEBUG
            if (poly == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw std::invalid_argument("poly");
            }
            if (coeff_count < 0)
            {
                throw std::invalid_argument("coeff_count");
            }
            if (coeff_uint64_count < 0)
            {
                throw std::invalid_argument("coeff_uint64_count");
            }
#endif
            int poly_uint64_count = coeff_count * coeff_uint64_count;
            for (int i = 0; i < poly_uint64_count; ++i)
            {
                if (*poly++ != 0)
                {
                    return false;
                }
            }
            return true;
        }

        inline bool is_equal_poly_poly(const std::uint64_t *operand1, const std::uint64_t *operand2, int coeff_count, int coeff_uint64_count)
        {
#ifdef _DEBUG
            if (operand1 == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw std::invalid_argument("operand1");
            }
            if (operand2 == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw std::invalid_argument("operand2");
            }
            if (coeff_count < 0)
            {
                throw std::invalid_argument("coeff_count");
            }
            if (coeff_uint64_count < 0)
            {
                throw std::invalid_argument("coeff_uint64_count");
            }
#endif
            if (operand1 == operand2)
            {
                // Fast path to handle self comparison.
                return true;
            }
            int poly_uint64_count = coeff_count * coeff_uint64_count;
            for (int i = 0; i < poly_uint64_count; ++i)
            {
                if (*operand1++ != *operand2++)
                {
                    return false;
                }
            }
            return true;
        }

        void set_poly_poly(const std::uint64_t *poly, int poly_coeff_count, int poly_coeff_uint64_count, int result_coeff_count, int result_coeff_uint64_count, std::uint64_t *result);

        bool is_one_zero_one_poly(const uint64_t *poly, int coeff_count, int coeff_uint64_count);

        int get_significant_coeff_count_poly(const std::uint64_t *poly, int coeff_count, int coeff_uint64_count);

        ConstPointer duplicate_poly_if_needed(const std::uint64_t *poly, int coeff_count, int coeff_uint64_count, int new_coeff_count, int new_coeff_uint64_count, bool force, MemoryPool &pool);

        bool are_poly_coefficients_less_than(const std::uint64_t *poly, int coeff_count, int coeff_uint64_count, const std::uint64_t *max_coeff, int max_coeff_uint64_count);
    }
}
#endif // SEAL_UTIL_POLYCORE_H
