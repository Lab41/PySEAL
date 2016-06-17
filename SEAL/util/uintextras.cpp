#include "util/uintextras.h"
#include "util/uintcore.h"
#include "util/uintarith.h"
#include "util/uintarithmod.h"
#include <stdexcept>

using namespace std;

namespace seal
{
    namespace util
    {
        void exponentiate_uint(const uint64_t *operand, int operand_uint64_count, const uint64_t *exponent, int exponent_uint64_count, int result_uint64_count, uint64_t *result, MemoryPool &pool)
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
            if (exponent == nullptr)
            {
                throw invalid_argument("exponent");
            }
            if (exponent_uint64_count <= 0)
            {
                throw invalid_argument("exponent_uint64_count");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
            if (result_uint64_count <= 0)
            {
                throw invalid_argument("result_uint64_count");
            }
#endif
            // Fast cases
            if (is_zero_uint(exponent, exponent_uint64_count))
            {
                set_uint(1, result_uint64_count, result);
                return;
            }
            if (is_equal_uint(exponent, exponent_uint64_count, 1))
            {
                set_uint_uint(operand, operand_uint64_count, result_uint64_count, result);
                return;
            }

            // Need to make a copy of exponent
            Pointer exponent_copy(allocate_uint(exponent_uint64_count, pool));
            set_uint_uint(exponent, exponent_uint64_count, exponent_copy.get());

            // Perform binary exponentiation.
            Pointer big_alloc(allocate_uint(result_uint64_count + result_uint64_count + result_uint64_count, pool));

            uint64_t *powerptr = big_alloc.get();
            uint64_t *productptr = powerptr + result_uint64_count;
            uint64_t *intermediateptr = productptr + result_uint64_count;

            set_uint_uint(operand, operand_uint64_count, result_uint64_count, powerptr);
            set_uint(1, result_uint64_count, intermediateptr);

            // Initially: power = operand and intermediate = 1, product is not initialized.
            while (true)
            {
                if ((*exponent_copy.get() % 2) == 1)
                {
                    multiply_truncate_uint_uint(powerptr, intermediateptr, result_uint64_count, productptr);
                    swap(productptr, intermediateptr);
                }
                right_shift_uint(exponent_copy.get(), 1, exponent_uint64_count, exponent_copy.get());
                if (is_zero_uint(exponent_copy.get(), exponent_uint64_count))
                {
                    break;
                }
                multiply_truncate_uint_uint(powerptr, powerptr, result_uint64_count, productptr);
                swap(productptr, powerptr);
            }
            set_uint_uint(intermediateptr, result_uint64_count, result);
        }

        void exponentiate_uint_mod(const uint64_t *operand, const uint64_t *exponent, int exponent_uint64_count, const Modulus &modulus, uint64_t *result, MemoryPool &pool)
        {
            int modulus_uint64_count = modulus.uint64_count();

#ifdef _DEBUG
            if (operand == nullptr)
            {
                throw invalid_argument("operand");
            }
            if (exponent == nullptr)
            {
                throw invalid_argument("exponent");
            }
            if (exponent_uint64_count <= 0)
            {
                throw invalid_argument("exponent_uint64_count");
            }
            if (is_zero_uint(modulus.get(), modulus_uint64_count))
            {
                throw invalid_argument("modulus");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
#endif

            // Fast cases
            if (is_zero_uint(exponent, exponent_uint64_count))
            {
                set_uint(1, modulus_uint64_count, result);
                return;
            }

            Pointer big_alloc(allocate_uint(3*modulus_uint64_count + modulus_uint64_count + modulus_uint64_count + modulus_uint64_count + 4 * modulus_uint64_count, pool));
            modulo_uint(operand, modulus_uint64_count, modulus, result, pool, big_alloc.get());

            if (is_equal_uint(exponent, exponent_uint64_count, 1))
            {
                return;
            }

            // Need to make a copy of exponent
            Pointer exponent_copy(allocate_uint(exponent_uint64_count, pool));
            set_uint_uint(exponent, exponent_uint64_count, exponent_copy.get());

            // Perform binary exponentiation.
            uint64_t *powerptr = big_alloc.get() + 3*modulus_uint64_count;
            uint64_t *productptr = powerptr + modulus_uint64_count;
            uint64_t *intermediateptr = productptr + modulus_uint64_count;
            uint64_t *alloc_ptr = intermediateptr + modulus_uint64_count;

            set_uint_uint(result, modulus_uint64_count, modulus_uint64_count, powerptr);
            set_uint(1, modulus_uint64_count, intermediateptr);

            // Initially: power = operand and intermediate = 1, product is not initialized.
            while (true)
            {
                if ((*exponent_copy.get() % 2) == 1)
                {
                    multiply_uint_uint_mod(powerptr, intermediateptr, modulus, productptr, pool, alloc_ptr);
                    swap(productptr, intermediateptr);
                }
                right_shift_uint(exponent_copy.get(), 1, exponent_uint64_count, exponent_copy.get());
                if (is_zero_uint(exponent_copy.get(), exponent_uint64_count))
                {
                    break;
                }
                multiply_uint_uint_mod(powerptr, powerptr, modulus, productptr, pool, alloc_ptr);
                swap(productptr, powerptr);
            }
            set_uint_uint(intermediateptr, modulus_uint64_count, result);
        }
    }
}