#include "util/uintcore.h"
#include "util/polycore.h"
#include "util/uintarith.h"

using namespace std;

namespace seal
{
    namespace util
    {
        namespace
        {
            void do_fft_base(const uint64_t *operand1, const uint64_t *operand2, int coeff_count_power, int coeff_uint64_count, uint64_t *result, MemoryPool &pool)
            {
                // Allocate temporary storage for FFT.
                Pointer a(allocate_uint(coeff_uint64_count, pool));
                Pointer b(allocate_uint(coeff_uint64_count, pool));
                Pointer t(allocate_uint(coeff_uint64_count, pool));

                int coeff_count = 1 << coeff_count_power;
                for (int i = 0; i < coeff_count; ++i)
                {
                    set_zero_uint(coeff_uint64_count, a.get());
                    set_zero_uint(coeff_uint64_count, b.get());
                    for (int j = 0; j <= i; ++j)
                    {
                        multiply_truncate_uint_uint(get_poly_coeff(operand1, j, coeff_uint64_count), get_poly_coeff(operand2, i - j, coeff_uint64_count), coeff_uint64_count, t.get());
                        add_uint_uint(a.get(), t.get(), coeff_uint64_count, a.get());
                    }
                    for (int j = i + 1; j < coeff_count; ++j)
                    {
                        multiply_truncate_uint_uint(get_poly_coeff(operand1, j, coeff_uint64_count), get_poly_coeff(operand2, coeff_count - (j - i), coeff_uint64_count), coeff_uint64_count, t.get());
                        add_uint_uint(b.get(), t.get(), coeff_uint64_count, b.get());
                    }
                    sub_uint_uint(a.get(), b.get(), coeff_uint64_count, get_poly_coeff(result, i, coeff_uint64_count));
                }
            }

            void do_fft(const uint64_t *operand1, const uint64_t *operand2, int coeff_count_power, int coeff_uint64_count, uint64_t *result, MemoryPool &pool)
            {
                const int USE_BASE_CASE = 16;

                // Handle base case differently.
                int coeff_count = 1 << coeff_count_power;
                if (coeff_count <= USE_BASE_CASE)
                {
                    do_fft_base(operand1, operand2, coeff_count_power, coeff_uint64_count, result, pool);
                    return;
                }

                // Allocate temporary storage for FFT.
                int m = 1 << (coeff_count_power / 2);
                int r = 1 << ((coeff_count_power + 1) / 2);
                Pointer x(allocate_poly(2 * coeff_count, coeff_uint64_count, pool));
                Pointer y(allocate_poly(2 * coeff_count, coeff_uint64_count, pool));
                Pointer z(allocate_poly(2 * coeff_count, coeff_uint64_count, pool));
                Pointer temp_poly(allocate_poly(r, coeff_uint64_count, pool));

                // Populate x and y with butterfly pattern.
                for (int indexM = 0; indexM < m; ++indexM)
                {
                    for (int indexR = 0; indexR < r; ++indexR)
                    {
                        const uint64_t *operand1_coeff = get_poly_coeff(operand1, indexR * m + indexM, coeff_uint64_count);
                        set_uint_uint(operand1_coeff, coeff_uint64_count, get_poly_coeff(x.get(), indexM * r + indexR, coeff_uint64_count));
                        set_uint_uint(operand1_coeff, coeff_uint64_count, get_poly_coeff(x.get(), (indexM + m) * r + indexR, coeff_uint64_count));
                        const uint64_t *operand2_coeff = get_poly_coeff(operand2, indexR * m + indexM, coeff_uint64_count);
                        set_uint_uint(operand2_coeff, coeff_uint64_count, get_poly_coeff(y.get(), indexM * r + indexR, coeff_uint64_count));
                        set_uint_uint(operand2_coeff, coeff_uint64_count, get_poly_coeff(y.get(), (indexM + m) * r + indexR, coeff_uint64_count));
                    }
                }

                // Do first stage of FFT.
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
                                set_uint_uint(get_poly_coeff(x.get(), l * r + a - k, coeff_uint64_count), coeff_uint64_count, get_poly_coeff(temp_poly.get(), a, coeff_uint64_count));
                            }
                            for (int a = 0; a < k; ++a)
                            {
                                negate_uint(get_poly_coeff(x.get(), (l + 1) * r + a - k, coeff_uint64_count), coeff_uint64_count, get_poly_coeff(temp_poly.get(), a, coeff_uint64_count));
                            }
                            for (int a = 0; a < r; ++a)
                            {
                                sub_uint_uint(get_poly_coeff(x.get(), i * r + a, coeff_uint64_count), get_poly_coeff(temp_poly.get(), a, coeff_uint64_count), coeff_uint64_count, get_poly_coeff(x.get(), l * r + a, coeff_uint64_count));
                                add_uint_uint(get_poly_coeff(x.get(), i * r + a, coeff_uint64_count), get_poly_coeff(temp_poly.get(), a, coeff_uint64_count), coeff_uint64_count, get_poly_coeff(x.get(), i * r + a, coeff_uint64_count));
                            }
                            for (int a = k; a < r; ++a)
                            {
                                set_uint_uint(get_poly_coeff(y.get(), l * r + a - k, coeff_uint64_count), coeff_uint64_count, get_poly_coeff(temp_poly.get(), a, coeff_uint64_count));
                            }
                            for (int a = 0; a < k; ++a)
                            {
                                negate_uint(get_poly_coeff(y.get(), (l + 1) * r + a - k, coeff_uint64_count), coeff_uint64_count, get_poly_coeff(temp_poly.get(), a, coeff_uint64_count));
                            }
                            for (int a = 0; a < r; ++a)
                            {
                                sub_uint_uint(get_poly_coeff(y.get(), i * r + a, coeff_uint64_count), get_poly_coeff(temp_poly.get(), a, coeff_uint64_count), coeff_uint64_count, get_poly_coeff(y.get(), l * r + a, coeff_uint64_count));
                                add_uint_uint(get_poly_coeff(y.get(), i * r + a, coeff_uint64_count), get_poly_coeff(temp_poly.get(), a, coeff_uint64_count), coeff_uint64_count, get_poly_coeff(y.get(), i * r + a, coeff_uint64_count));
                            }
                        }
                    }
                }

                // Recursively do FFT.
                int rec_coeff_count_power = (coeff_count_power + 1) / 2;
                for (int i = 0; i < 2 * m; ++i)
                {
                    do_fft(get_poly_coeff(x.get(), i * r, coeff_uint64_count), get_poly_coeff(y.get(), i * r, coeff_uint64_count), rec_coeff_count_power, coeff_uint64_count, get_poly_coeff(z.get(), i * r, coeff_uint64_count), pool);
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
                                sub_uint_uint(get_poly_coeff(z.get(), i * r + a, coeff_uint64_count), get_poly_coeff(z.get(), l * r + a, coeff_uint64_count), coeff_uint64_count, get_poly_coeff(temp_poly.get(), a, coeff_uint64_count));
                                right_shift_sign_extend_uint(get_poly_coeff(temp_poly.get(), a, coeff_uint64_count), 1, coeff_uint64_count, get_poly_coeff(temp_poly.get(), a, coeff_uint64_count));
                                add_uint_uint(get_poly_coeff(z.get(), i * r + a, coeff_uint64_count), get_poly_coeff(z.get(), l * r + a, coeff_uint64_count), coeff_uint64_count, get_poly_coeff(z.get(), i * r + a, coeff_uint64_count));
                                right_shift_sign_extend_uint(get_poly_coeff(z.get(), i * r + a, coeff_uint64_count), 1, coeff_uint64_count, get_poly_coeff(z.get(), i * r + a, coeff_uint64_count));
                            }

                            int rsubk = r - k;
                            for (int a = 0; a < rsubk; ++a)
                            {
                                set_uint_uint(get_poly_coeff(temp_poly.get(), a + k, coeff_uint64_count), coeff_uint64_count, get_poly_coeff(z.get(), l * r + a, coeff_uint64_count));
                            }
                            for (int a = rsubk; a < r; ++a)
                            {
                                negate_uint(get_poly_coeff(temp_poly.get(), a - rsubk, coeff_uint64_count), coeff_uint64_count, get_poly_coeff(z.get(), l * r + a, coeff_uint64_count));
                            }
                        }
                    }
                }

                // Copy to result.
                for (int indexM = 0; indexM < m; ++indexM)
                {
                    sub_uint_uint(get_poly_coeff(z.get(), indexM * r, coeff_uint64_count), get_poly_coeff(z.get(), (m + indexM + 1) * r - 1, coeff_uint64_count), coeff_uint64_count, get_poly_coeff(result, indexM, coeff_uint64_count));
                    for (int indexR = 1; indexR < r; ++indexR)
                    {
                        add_uint_uint(get_poly_coeff(z.get(), indexM * r + indexR, coeff_uint64_count), get_poly_coeff(z.get(), (m + indexM) * r + indexR - 1, coeff_uint64_count), coeff_uint64_count, get_poly_coeff(result, indexR * m + indexM, coeff_uint64_count));
                    }
                }
            }
        }

        void fftmultiply_poly_poly_polymod(const uint64_t *operand1, const uint64_t *operand2, int coeff_count_power, int coeff_uint64_count, int product_coeff_uint64_count, uint64_t *result, MemoryPool &pool)
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
            if (coeff_uint64_count <= 0)
            {
                throw invalid_argument("coeff_uint64_count");
            }
            if (product_coeff_uint64_count <= 0)
            {
                throw invalid_argument("product_coeff_uint64_count");
            }
            if (result == nullptr)
            {
                throw invalid_argument("result");
            }
#endif
            if (coeff_uint64_count == product_coeff_uint64_count)
            {
                do_fft(operand1, operand2, coeff_count_power, coeff_uint64_count, result, pool);
            }
            else
            {
                int coeff_count = 1 << coeff_count_power;
                Pointer operand1resized(allocate_poly(coeff_count, product_coeff_uint64_count, pool));
                Pointer operand2resized(allocate_poly(coeff_count, product_coeff_uint64_count, pool));
                for (int i = 0; i < coeff_count; ++i)
                {
                    set_uint_uint(get_poly_coeff(operand1, i, coeff_uint64_count), coeff_uint64_count, product_coeff_uint64_count, get_poly_coeff(operand1resized.get(), i, product_coeff_uint64_count));
                    set_uint_uint(get_poly_coeff(operand2, i, coeff_uint64_count), coeff_uint64_count, product_coeff_uint64_count, get_poly_coeff(operand2resized.get(), i, product_coeff_uint64_count));
                }
                do_fft(operand1resized.get(), operand2resized.get(), coeff_count_power, product_coeff_uint64_count, result, pool);
            }
        }
    }
}
