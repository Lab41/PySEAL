#include "util/uintcore.h"
#include "util/polycore.h"

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
            for (int i = 0; i < result_coeff_count; ++i)
            {
                if (i < poly_coeff_count)
                {
                    set_uint_uint(poly, poly_coeff_uint64_count, result_coeff_uint64_count, result);
                }
                else
                {
                    set_zero_uint(result_coeff_uint64_count, result);
                }
                poly += poly_coeff_uint64_count;
                result += result_coeff_uint64_count;
            }
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
            poly += coeff_count * coeff_uint64_count;
            for (int i = coeff_count; i > 0; --i)
            {
                poly -= coeff_uint64_count;
                if (!is_zero_uint(poly, coeff_uint64_count))
                {
                    return i;
                }
            }
            return 0;
        }

        ConstPointer duplicate_poly_if_needed(const std::uint64_t *poly, int coeff_count, int coeff_uint64_count, int new_coeff_count, int new_coeff_uint64_count, bool force, MemoryPool &pool)
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