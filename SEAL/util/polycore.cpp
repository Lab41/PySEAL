#include "util/uintcore.h"
#include "util/polycore.h"
#include <algorithm>

using namespace std;

namespace seal
{
    namespace util
    {
        void set_poly_poly(const uint64_t *poly, int poly_coeff_count, int poly_coeff_uint64_count, int result_coeff_count, int result_coeff_uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (poly == nullptr && poly_coeff_count > 0 && poly_coeff_uint64_count > 0)
            {
                throw std::invalid_argument("poly");
            }
            if (poly_coeff_count < 0)
            {
                throw std::invalid_argument("poly_coeff_count");
            }
            if (poly_coeff_uint64_count < 0)
            {
                throw std::invalid_argument("poly_coeff_uint64_count");
            }
            if (result_coeff_count < 0)
            {
                throw std::invalid_argument("result_coeff_count");
            }
            if (result_coeff_uint64_count < 0)
            {
                throw std::invalid_argument("result_coeff_uint64_count");
            }
            if (result == nullptr && result_coeff_count > 0 && result_coeff_uint64_count > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            //set_zero_uint(result_coeff_count * result_coeff_uint64_count, result);

            int min_coeff_count = min(poly_coeff_count, result_coeff_count);
            for (int i = 0; i < min_coeff_count; ++i)
            {
                set_uint_uint(poly, poly_coeff_uint64_count, result_coeff_uint64_count, result);
                poly += poly_coeff_uint64_count;
                result += result_coeff_uint64_count;
            }

            // Set the rest of the coeffs zero
            set_zero_uint((result_coeff_count - min_coeff_count) * result_coeff_uint64_count, result);
        }

        void set_bigpolyarray_bigpolyarray(const uint64_t *operand, int size, int coeff_count, int coeff_uint64_count, int result_size, int result_coeff_count, int result_coeff_uint64_count, uint64_t *result)
        {
#ifdef _DEBUG
            if (operand == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw std::invalid_argument("operand");
            }
            if (coeff_count < 0)
            {
                throw std::invalid_argument("coeff_count");
            }
            if (coeff_uint64_count < 0)
            {
                throw std::invalid_argument("coeff_uint64_count");
            }
            if (result_size < 0)
            {
                throw std::invalid_argument("result_size");
            }
            if (result_coeff_count < 0)
            {
                throw std::invalid_argument("result_coeff_count");
            }
            if (result_coeff_uint64_count < 0)
            {
                throw std::invalid_argument("result_coeff_uint64_count");
            }
            if (result == nullptr && result_coeff_count > 0 && result_coeff_uint64_count > 0 && result_size > 0)
            {
                throw std::invalid_argument("result");
            }
#endif
            //set_zero_uint(result_size * result_coeff_count * result_coeff_uint64_count, result);

            int operand_ptr_increment = coeff_count * coeff_uint64_count;
            int result_ptr_increment = result_coeff_count * result_coeff_uint64_count;

            int min_size = min(size, result_size);
            for (int i = 0; i < min_size; ++i)
            {
                set_poly_poly(operand, coeff_count, coeff_uint64_count, result_coeff_count, result_coeff_uint64_count, result);
                operand += operand_ptr_increment;
                result += result_ptr_increment;
            }

            // Set the rest of the polys to zero
            set_zero_uint((result_size - min_size) * result_coeff_count * result_coeff_uint64_count, result);
        }

        bool is_one_zero_one_poly(const uint64_t *poly, int coeff_count, int coeff_uint64_count)
        {
#ifdef _DEBUG
            if (poly == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw invalid_argument("poly");
            }
            if (coeff_count < 0)
            {
                throw invalid_argument("coeff_count");
            }
            if (coeff_uint64_count < 0)
            {
                throw invalid_argument("coeff_uint64_count");
            }
#endif
            if (coeff_count == 0 || coeff_uint64_count == 0)
            {
                return false;
            }
            if (!is_equal_uint(get_poly_coeff(poly, 0, coeff_uint64_count), coeff_uint64_count, 1))
            {
                return false;
            }
            if (!is_equal_uint(get_poly_coeff(poly, coeff_count - 1, coeff_uint64_count), coeff_uint64_count, 1))
            {
                return false;
            }
            if (coeff_count > 2 && !is_zero_poly(poly + coeff_uint64_count, coeff_count - 2, coeff_uint64_count))
            {
                return false;
            }
            return true;
        }

        int get_significant_coeff_count_poly(const uint64_t *poly, int coeff_count, int coeff_uint64_count)
        {
#ifdef _DEBUG
            if (poly == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw invalid_argument("poly");
            }
            if (coeff_count < 0)
            {
                throw invalid_argument("coeff_count");
            }
            if (coeff_uint64_count < 0)
            {
                throw invalid_argument("coeff_uint64_count");
            }
#endif
            poly += (coeff_count - 1) * coeff_uint64_count;
            for (int i = coeff_count; i > 0; --i)
            {
                if (!is_zero_uint(poly, coeff_uint64_count))
                {
                    return i;
                }
                poly -= coeff_uint64_count;
            }
            return 0;
        }

        ConstPointer duplicate_poly_if_needed(const uint64_t *poly, int coeff_count, int coeff_uint64_count, int new_coeff_count, int new_coeff_uint64_count, bool force, MemoryPool &pool)
        {
#ifdef _DEBUG
            if (poly == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw invalid_argument("poly");
            }
            if (coeff_count < 0)
            {
                throw invalid_argument("coeff_count");
            }
            if (coeff_uint64_count < 0)
            {
                throw invalid_argument("coeff_uint64_count");
            }
            if (new_coeff_count < 0)
            {
                throw invalid_argument("new_coeff_count");
            }
            if (new_coeff_uint64_count < 0)
            {
                throw invalid_argument("new_coeff_uint64_count");
            }
#endif
            if (!force && coeff_count >= new_coeff_count && coeff_uint64_count == new_coeff_uint64_count)
            {
                return ConstPointer::Aliasing(poly);
            }
            Pointer allocation = pool.get_for_uint64_count(new_coeff_count * new_coeff_uint64_count);
            set_poly_poly(poly, coeff_count, coeff_uint64_count, new_coeff_count, new_coeff_uint64_count, allocation.get());
            ConstPointer const_allocation;
            const_allocation.acquire(allocation);
            return const_allocation;
        }

        ConstPointer duplicate_bigpolyarray_if_needed(const uint64_t *operand, int size, int coeff_count, int coeff_uint64_count, int new_size, int new_coeff_count, int new_coeff_uint64_count, bool force, MemoryPool &pool)
        {
#ifdef _DEBUG
            if (operand == nullptr && coeff_count > 0 && coeff_uint64_count > 0 && size > 0)
            {
                throw invalid_argument("operand");
            }
            if (size < 0)
            {
                throw invalid_argument("size");
            }
            if (coeff_count < 0)
            {
                throw invalid_argument("coeff_count");
            }
            if (coeff_uint64_count < 0)
            {
                throw invalid_argument("coeff_uint64_count");
            }
            if (new_size < 0)
            {
                throw invalid_argument("new_size");
            }
            if (new_coeff_count < 0)
            {
                throw invalid_argument("new_coeff_count");
            }
            if (new_coeff_uint64_count < 0)
            {
                throw invalid_argument("new_coeff_uint64_count");
            }
#endif
            if (!force && coeff_count == new_coeff_count && coeff_uint64_count == new_coeff_uint64_count && size >= new_size)
            {
                return ConstPointer::Aliasing(operand);
            }
            Pointer allocation = pool.get_for_uint64_count(new_size * new_coeff_count * new_coeff_uint64_count);
            set_bigpolyarray_bigpolyarray(operand, size, coeff_count, coeff_uint64_count, new_size, new_coeff_count, new_coeff_uint64_count, allocation.get());
            ConstPointer const_allocation;
            const_allocation.acquire(allocation);
            return const_allocation;
        }

        bool are_poly_coefficients_less_than(const std::uint64_t *poly, int coeff_count, int coeff_uint64_count, const std::uint64_t *max_coeff, int max_coeff_uint64_count)
        {
#ifdef _DEBUG
            if (poly == nullptr && coeff_count > 0 && coeff_uint64_count > 0)
            {
                throw invalid_argument("poly");
            }
            if (coeff_count < 0)
            {
                throw invalid_argument("coeff_count");
            }
            if (coeff_uint64_count < 0)
            {
                throw invalid_argument("coeff_uint64_count");
            }
            if (max_coeff == nullptr && max_coeff_uint64_count > 0)
            {
                throw invalid_argument("max_coeff");
            }
            if (max_coeff_uint64_count < 0)
            {
                throw invalid_argument("max_coeff_uint64_count");
            }
#endif
            if (coeff_count == 0)
            {
                return true;
            }
            if (max_coeff_uint64_count == 0)
            {
                return false;
            }
            if (coeff_uint64_count == 0)
            {
                return true;
            }
            for (int i = 0; i < coeff_count; i++)
            {
                if (compare_uint_uint(poly, coeff_uint64_count, max_coeff, max_coeff_uint64_count) >= 0)
                {
                    return false;
                }
                poly += coeff_uint64_count;
            }
            return true;
        }
    }
}