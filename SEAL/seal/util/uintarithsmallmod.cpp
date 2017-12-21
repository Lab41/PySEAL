#include "seal/util/uintcore.h"
#include "seal/util/uintarith.h"
#include "seal/util/uintarithmod.h"
#include "seal/util/uintarithsmallmod.h"
#include <random>

using namespace std;

namespace seal
{
    namespace util
    {
        bool is_primitive_root(uint64_t root, uint64_t degree, const SmallModulus &modulus)
        {
#ifdef SEAL_DEBUG
            if (modulus.bit_count() < 2)
            {
                throw invalid_argument("modulus");
            }
            if (root >= modulus.value())
            {
                throw out_of_range("operand");
            }
            if (get_power_of_two(degree) < 1)
            {
                throw invalid_argument("degree must be a power of two and at least two");
            }
#endif
            if (root == 0)
            {
                return false;
            }

            // We check if root is a degree-th root of unity in integers modulo modulus, where degree is a power of two.
            // It suffices to check that root^(degree/2) is -1 modulo modulus.
            return exponentiate_uint_mod(root, degree >> 1, modulus) == (modulus.value() - 1);
        }
        
        bool try_primitive_root(uint64_t degree, const SmallModulus &modulus, uint64_t &destination) 
        {
#ifdef SEAL_DEBUG
            if (modulus.bit_count() < 2)
            {
                throw invalid_argument("modulus");
            }
            if (get_power_of_two(degree) < 1)
            {
                throw invalid_argument("degree must be a power of two and at least two");
            }
#endif
            // We need to divide modulus-1 by degree to get the size of the quotient group
            uint64_t size_entire_group = modulus.value() - 1;

            // Compute size of quotient group
            uint64_t size_quotient_group = size_entire_group / degree;

            // size_entire_group must be divisible by degree, or otherwise the primitive root does not exist in integers modulo modulus
            if (size_entire_group - size_quotient_group * degree != 0)
            {
                return false;
            }

            // For randomness
            random_device rd;

            int attempt_counter = 0;
            int attempt_counter_max = 100;
            do
            {
                attempt_counter++;

                // Set destination to be a random number modulo modulus
                destination = (static_cast<uint64_t>(rd()) << 32) | static_cast<uint64_t>(rd());
                destination %= modulus.value();

                // Raise the random number to power the size of the quotient to get rid of irrelevant part
                destination = exponentiate_uint_mod(destination, size_quotient_group, modulus);
            } while (!is_primitive_root(destination, degree, modulus) && (attempt_counter < attempt_counter_max));

            return is_primitive_root(destination, degree, modulus);
        }
        
        bool try_minimal_primitive_root(uint64_t degree, const SmallModulus &modulus, uint64_t &destination)
        {
            uint64_t root;
            if (!try_primitive_root(degree, modulus, root))
            {
                return false;
            }
            uint64_t generator_sq = multiply_uint_uint_mod(root, root, modulus);
            uint64_t current_generator = root;

            // destination is going to always contain the smallest generator found
            for (size_t i = 0; i < degree; i++)
            {
                // If our current generator is strictly smaller than destination, update
                if (current_generator < root)
                {
                    root = current_generator;
                }

                // Then move on to the next generator
                current_generator = multiply_uint_uint_mod(current_generator, generator_sq, modulus);
            }

            destination = root;
            return true;
        }

        uint64_t exponentiate_uint_mod(uint64_t operand, uint64_t exponent, const SmallModulus &modulus)
        {
#ifdef SEAL_DEBUG
            if (modulus.value() == 0)
            {
                throw invalid_argument("modulus");
            }
            if (operand >= modulus.value())
            {
                throw invalid_argument("operand");
            }
#endif
            // Fast cases
            if (exponent == 0)
            {
                // Result is supposed to be only one digit
                return 1;
            }

            if (exponent == 1)
            {
                return operand;
            }

            // Perform binary exponentiation.
            uint64_t power = operand;
            uint64_t product = 0;
            uint64_t intermediate = 1;

            // Initially: power = operand and intermediate = 1, product is irrelevant.
            while (true)
            {
                if (exponent & 1)
                {
                    product = multiply_uint_uint_mod(power, intermediate, modulus);
                    swap(product, intermediate);
                }
                exponent >>= 1;
                if (exponent == 0)
                {
                    break;
                }
                product = multiply_uint_uint_mod(power, power, modulus);
                swap(product, power);
            }
            return intermediate;
        }
    }
}
