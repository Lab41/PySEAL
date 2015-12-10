#include "util/uintextras.h"
#include "util/uintcore.h"
#include "util/uintarith.h"
#include <stdexcept>

using namespace std;

namespace seal
{
    namespace util
    {
        void exponentiate_uint(const std::uint64_t *operand, int operand_uint64_count, int exponent, int result_uint64_count, std::uint64_t *result, MemoryPool &pool)
        {
#ifdef _DEBUG
            if (operand == nullptr)
            {
                throw invalid_argument("operand");
            }
            if (operand_uint64_count <= 0)
            {
                throw invalid_argument("operand_uint64_count");
            }
            if (exponent < 0)
            {
                throw out_of_range("exponent");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (result_uint64_count <= 0)
            {
                throw out_of_range("result_uint64_count");
            }
#endif
            // Fast cases
            if (exponent == 0)
            {
                set_uint(1, result_uint64_count, result);
                return;
            }
            if (exponent == 1)
            {
                set_uint_uint(operand, operand_uint64_count, result_uint64_count, result);
                return;
            }

            // Perform binary exponentiation.
            Pointer power(allocate_uint(result_uint64_count, pool));
            Pointer temp1(allocate_uint(result_uint64_count, pool));
            Pointer temp2(allocate_uint(result_uint64_count, pool));
            uint64_t *powerptr = power.get();
            uint64_t *productptr = temp1.get();
            uint64_t *intermediateptr = temp2.get();
            set_uint_uint(operand, operand_uint64_count, result_uint64_count, powerptr);
            set_uint(1, result_uint64_count, intermediateptr);
            
            // Initially: power = operand and intermediate = 1, product is not initialized.
            while (true)
            {
                if ((exponent % 2) == 1)
                {
                    multiply_truncate_uint_uint(powerptr, intermediateptr, result_uint64_count, productptr);
                    swap(productptr, intermediateptr);
                }
                exponent >>= 1;
                if (exponent == 0)
                {
                    break;
                }
                multiply_truncate_uint_uint(powerptr, powerptr, result_uint64_count, productptr);
                swap(productptr, powerptr);
            }
            set_uint_uint(intermediateptr, result_uint64_count, result);
        }
    }
}