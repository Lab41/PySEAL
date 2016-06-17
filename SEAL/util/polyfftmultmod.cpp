#include "util/uintcore.h"
#include "util/uintarithmod.h"
#include "util/polycore.h"

using namespace std;

namespace seal
{
    namespace util
    {
        namespace
        {
            void do_fft_base(const uint64_t *operand1, const uint64_t *operand2, int coeff_count_power, const Modulus &modulus, uint64_t *result, MemoryPool &pool, uint64_t *alloc_ptr = nullptr)
            {
                // alloc_ptr should point to 8 x modulus.uint64_count() memory

                // Allocate temporary storage for FFT.
                int coeff_uint64_count = modulus.uint64_count();

                // To reduce the number of allocations, we allocate one big block of memory at once.
                // Set big_alloc to point to either an existing allocation given as parameter,
                // or else to a new allocation from the memory pool.
                Pointer big_alloc_anchor;
                uint64_t *big_alloc = alloc_ptr;
                if (alloc_ptr == nullptr)
                {
                    big_alloc_anchor = allocate_uint(coeff_uint64_count + coeff_uint64_count + 2 * coeff_uint64_count + 4 * coeff_uint64_count, pool);
                    big_alloc = big_alloc_anchor.get();
                }

                uint64_t *a = big_alloc;
                uint64_t *b = a + coeff_uint64_count;
                uint64_t *t = b + coeff_uint64_count; // Twice as large to allow for multiplying in-place.
                uint64_t *fast_alloc = t + 2 * coeff_uint64_count; // Will be passed all the way down to modulo_uint_inplace

                const uint64_t *modulusptr = modulus.get();
                int coeff_count = 1 << coeff_count_power;
                for (int i = 0; i < coeff_count; ++i)
                {
                    set_zero_uint(coeff_uint64_count, a);
                    set_zero_uint(coeff_uint64_count, b);
                    for (int j = 0; j <= i; ++j)
                    {
                        multiply_uint_uint_mod_inplace(get_poly_coeff(operand1, j, coeff_uint64_count), get_poly_coeff(operand2, i - j, coeff_uint64_count), modulus, t, pool, fast_alloc);
                        add_uint_uint_mod(a, t, modulusptr, coeff_uint64_count, a);
                    }
                    for (int j = i + 1; j < coeff_count; ++j)
                    {
                        multiply_uint_uint_mod_inplace(get_poly_coeff(operand1, j, coeff_uint64_count), get_poly_coeff(operand2, coeff_count - (j - i), coeff_uint64_count), modulus, t, pool, fast_alloc);
                        add_uint_uint_mod(b, t, modulusptr, coeff_uint64_count, b);
                    }
                    sub_uint_uint_mod(a, b, modulusptr, coeff_uint64_count, get_poly_coeff(result, i, coeff_uint64_count));
                }
            }

            void do_fft(const uint64_t *operand1, const uint64_t *operand2, int coeff_count_power, const Modulus &modulus, uint64_t *result, MemoryPool &pool, uint64_t *fft_base_alloc_ptr = nullptr)
            {
                // fft_base_alloc_ptr should point to 8 x modulus.uint64_count() memory

                const int USE_BASE_CASE = 16;

                // Handle base case differently.
                int coeff_count = 1 << coeff_count_power;
                if (coeff_count <= USE_BASE_CASE)
                {
                    do_fft_base(operand1, operand2, coeff_count_power, modulus, result, pool, fft_base_alloc_ptr);
                    return;
                }

                // Allocate temporary storage for FFT.
                int m = 1 << (coeff_count_power / 2);
                int r = 1 << ((coeff_count_power + 1) / 2);
                int coeff_uint64_count = modulus.uint64_count();

                // To reduce the number of allocations, we allocate one big block of memory at once.
                // Set big_alloc to point to either an existing allocation given as parameter,
                // or else to a new allocation from the memory pool.
                Pointer big_alloc_anchor = allocate_poly(2 * coeff_count + 2 * coeff_count + 2 * coeff_count + r, coeff_uint64_count, pool);
                uint64_t *big_alloc = big_alloc_anchor.get();

                uint64_t *x = big_alloc;
                uint64_t *y = get_poly_coeff(x, 2 * coeff_count, coeff_uint64_count);
                uint64_t *z = get_poly_coeff(y, 2 * coeff_count, coeff_uint64_count);
                uint64_t *temp_poly = get_poly_coeff(z, 2 * coeff_count, coeff_uint64_count);

                // Populate x and y with butterfly pattern.
                const uint64_t *operand1_coeff;
                const uint64_t *operand2_coeff;
                for (int indexM = 0; indexM < m; ++indexM)
                {
                    for (int indexR = 0; indexR < r; ++indexR)
                    {
                        operand1_coeff = get_poly_coeff(operand1, indexR * m + indexM, coeff_uint64_count);
                        set_uint_uint(operand1_coeff, coeff_uint64_count, get_poly_coeff(x, indexM * r + indexR, coeff_uint64_count));
                        set_uint_uint(operand1_coeff, coeff_uint64_count, get_poly_coeff(x, (indexM + m) * r + indexR, coeff_uint64_count));
                        operand2_coeff = get_poly_coeff(operand2, indexR * m + indexM, coeff_uint64_count);
                        set_uint_uint(operand2_coeff, coeff_uint64_count, get_poly_coeff(y, indexM * r + indexR, coeff_uint64_count));
                        set_uint_uint(operand2_coeff, coeff_uint64_count, get_poly_coeff(y, (indexM + m) * r + indexR, coeff_uint64_count));
                    }
                }

                // Do first stage of FFT.
                const uint64_t *modulusptr = modulus.get();
                int outer_start = coeff_count_power / 2 - 1;
                for (int outer_index = outer_start; outer_index >= 0; --outer_index)
                {
                    int outer_remaining = coeff_count_power / 2 - outer_index;
                    int middle_end = 1 << outer_remaining;
                    int inner_end = 1 << outer_index;
                    for (int middle_index = 0; middle_index < middle_end; ++middle_index)
                    {
                        int sr = static_cast<int>((reverse_bits(static_cast<uint32_t>(middle_index)) >> (32 - outer_remaining)) << outer_index);
                        int s = middle_index << (outer_index + 1);
                        int k = (r / m) * sr;
                        for (int inner_index = 0; inner_index < inner_end; ++inner_index)
                        {
                            int i = s + inner_index;
                            int l = i + inner_end;
                            for (int a = k; a < r; ++a)
                            {
                                set_uint_uint(get_poly_coeff(x, l * r + a - k, coeff_uint64_count), coeff_uint64_count, get_poly_coeff(temp_poly, a, coeff_uint64_count));
                            }
                            for (int a = 0; a < k; ++a)
                            {
                                negate_uint_mod(get_poly_coeff(x, (l + 1) * r + a - k, coeff_uint64_count), modulusptr, coeff_uint64_count, get_poly_coeff(temp_poly, a, coeff_uint64_count));
                            }
                            for (int a = 0; a < r; ++a)
                            {
                                sub_uint_uint_mod(get_poly_coeff(x, i * r + a, coeff_uint64_count), get_poly_coeff(temp_poly, a, coeff_uint64_count), modulusptr, coeff_uint64_count, get_poly_coeff(x, l * r + a, coeff_uint64_count));
                                add_uint_uint_mod(get_poly_coeff(x, i * r + a, coeff_uint64_count), get_poly_coeff(temp_poly, a, coeff_uint64_count), modulusptr, coeff_uint64_count, get_poly_coeff(x, i * r + a, coeff_uint64_count));
                            }
                            for (int a = k; a < r; ++a)
                            {
                                set_uint_uint(get_poly_coeff(y, l * r + a - k, coeff_uint64_count), coeff_uint64_count, get_poly_coeff(temp_poly, a, coeff_uint64_count));
                            }
                            for (int a = 0; a < k; ++a)
                            {
                                negate_uint_mod(get_poly_coeff(y, (l + 1) * r + a - k, coeff_uint64_count), modulusptr, coeff_uint64_count, get_poly_coeff(temp_poly, a, coeff_uint64_count));
                            }
                            for (int a = 0; a < r; ++a)
                            {
                                sub_uint_uint_mod(get_poly_coeff(y, i * r + a, coeff_uint64_count), get_poly_coeff(temp_poly, a, coeff_uint64_count), modulusptr, coeff_uint64_count, get_poly_coeff(y, l * r + a, coeff_uint64_count));
                                add_uint_uint_mod(get_poly_coeff(y, i * r + a, coeff_uint64_count), get_poly_coeff(temp_poly, a, coeff_uint64_count), modulusptr, coeff_uint64_count, get_poly_coeff(y, i * r + a, coeff_uint64_count));
                            }
                        }
                    }
                }

                // Recursively do FFT.
                int rec_coeff_count_power = (coeff_count_power + 1) / 2;
                for (int i = 0; i < 2 * m; ++i)
                {
                    do_fft(get_poly_coeff(x, i * r, coeff_uint64_count), get_poly_coeff(y, i * r, coeff_uint64_count), rec_coeff_count_power, modulus, get_poly_coeff(z, i * r, coeff_uint64_count), pool, fft_base_alloc_ptr);
                }

                // Do second stage of FFT.
                int outer_end = coeff_count_power / 2;
                for (int outer_index = 0; outer_index <= outer_end; ++outer_index)
                {
                    int outer_remaining = coeff_count_power / 2 - outer_index;
                    int middle_end = 1 << outer_remaining;
                    int inner_end = 1 << outer_index;
                    for (int middle_index = 0; middle_index < middle_end; ++middle_index)
                    {
                        int sr = static_cast<int>((reverse_bits(static_cast<uint32_t>(middle_index)) >> (32 - outer_remaining)) << outer_index);
                        int s = middle_index << (outer_index + 1);
                        int k = (r / m) * sr;
                        for (int inner_index = 0; inner_index < inner_end; ++inner_index)
                        {
                            int i = s + inner_index;
                            int l = i + inner_end;
                            for (int a = 0; a < r; ++a)
                            {
                                sub_uint_uint_mod(get_poly_coeff(z, i * r + a, coeff_uint64_count), get_poly_coeff(z, l * r + a, coeff_uint64_count), modulusptr, coeff_uint64_count, get_poly_coeff(temp_poly, a, coeff_uint64_count));
                                div2_uint_mod(get_poly_coeff(temp_poly, a, coeff_uint64_count), modulusptr, coeff_uint64_count, get_poly_coeff(temp_poly, a, coeff_uint64_count));
                                add_uint_uint_mod(get_poly_coeff(z, i * r + a, coeff_uint64_count), get_poly_coeff(z, l * r + a, coeff_uint64_count), modulusptr, coeff_uint64_count, get_poly_coeff(z, i * r + a, coeff_uint64_count));
                                div2_uint_mod(get_poly_coeff(z, i * r + a, coeff_uint64_count), modulusptr, coeff_uint64_count, get_poly_coeff(z, i * r + a, coeff_uint64_count));
                            }

                            int rsubk = r - k;
                            for (int a = 0; a < rsubk; ++a)
                            {
                                set_uint_uint(get_poly_coeff(temp_poly, a + k, coeff_uint64_count), coeff_uint64_count, get_poly_coeff(z, l * r + a, coeff_uint64_count));
                            }
                            for (int a = rsubk; a < r; ++a)
                            {
                                negate_uint_mod(get_poly_coeff(temp_poly, a - rsubk, coeff_uint64_count), modulusptr, coeff_uint64_count, get_poly_coeff(z, l * r + a, coeff_uint64_count));
                            }
                        }
                    }
                }

                // Copy to result.
                for (int indexM = 0; indexM < m; ++indexM)
                {
                    sub_uint_uint_mod(get_poly_coeff(z, indexM * r, coeff_uint64_count), get_poly_coeff(z, (m + indexM + 1) * r - 1, coeff_uint64_count), modulusptr, coeff_uint64_count, get_poly_coeff(result, indexM, coeff_uint64_count));
                    for (int indexR = 1; indexR < r; ++indexR)
                    {
                        add_uint_uint_mod(get_poly_coeff(z, indexM * r + indexR, coeff_uint64_count), get_poly_coeff(z, (m + indexM) * r + indexR - 1, coeff_uint64_count), modulusptr, coeff_uint64_count, get_poly_coeff(result, indexR * m + indexM, coeff_uint64_count));
                    }
                }
            }
        }

        void fftmultiply_poly_poly_polymod_coeffmod(const uint64_t *operand1, const uint64_t *operand2, int coeff_count_power, const Modulus &modulus, uint64_t *result, MemoryPool &pool)
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
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
#endif
            int coeff_uint64_count = modulus.uint64_count();
            Pointer fft_base_alloc(allocate_uint(8 * coeff_uint64_count, pool));
            do_fft(operand1, operand2, coeff_count_power, modulus, result, pool, fft_base_alloc.get());
        }
    }
}
