#include "util/polymodulus.h"
#include "util/uintcore.h"
#include "util/polyarith.h"

using namespace std;

namespace seal
{
    namespace util
    {
        PolyModulus::PolyModulus() : poly_(nullptr), coeff_count_(0), coeff_uint64_count_(0), 
            coeff_count_power_of_two_(-1), is_one_zero_one_(false)
        {
        }

        PolyModulus::PolyModulus(const uint64_t *poly, int coeff_count, int coeff_uint64_count) : 
            poly_(poly), coeff_count_(coeff_count), coeff_uint64_count_(coeff_uint64_count),
            coeff_count_power_of_two_(-1), is_one_zero_one_(false)
        {
#ifdef _DEBUG
            if (poly == nullptr)
            {
                throw invalid_argument("poly");
            }
            if (coeff_count <= 0)
            {
                throw invalid_argument("coeff_count");
            }
            if (coeff_uint64_count <= 0)
            {
                throw invalid_argument("coeff_uint64_count");
            }
            if (get_significant_coeff_count_poly(poly, coeff_count, coeff_uint64_count) != coeff_count)
            {
                throw invalid_argument("poly");
            }
#endif
            coeff_count_power_of_two_ = get_power_of_two(static_cast<uint64_t>(coeff_count - 1));
            is_one_zero_one_ = is_one_zero_one_poly(poly, coeff_count, coeff_uint64_count);
        }

        PolyModulus &PolyModulus::operator=(PolyModulus &&assign) noexcept
        {
            poly_ = assign.poly_;
            coeff_count_ = assign.coeff_count_;
            coeff_uint64_count_ = assign.coeff_uint64_count_;
            coeff_count_power_of_two_ = assign.coeff_count_power_of_two_;
            is_one_zero_one_ = assign.is_one_zero_one_;
            assign.poly_ = nullptr;

            return *this;
        }

        PolyModulus::PolyModulus(PolyModulus &&source) noexcept
        {
            operator =(move(source));
        }
    }
}