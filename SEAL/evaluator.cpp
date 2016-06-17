#include <algorithm>
#include <stdexcept>
#include "evaluator.h"
#include "util/common.h"
#include "util/uintcore.h"
#include "util/uintarith.h"
#include "util/uintarithmod.h"
#include "util/polycore.h"
#include "util/polyarith.h"
#include "util/polyarithmod.h"
#include "util/polyfftmult.h"
#include "bigpoly.h"

using namespace std;
using namespace seal::util;

namespace seal
{
    namespace
    {
        ConstPointer duplicate_poly_if_needed(const BigPoly &poly, int new_coeff_count, int new_coeff_uint64_count, bool force, MemoryPool &pool)
        {
            return duplicate_poly_if_needed(poly.pointer(), poly.coeff_count(), poly.coeff_uint64_count(), new_coeff_count, new_coeff_uint64_count, force, pool);
        }

        ConstPointer duplicate_poly_if_needed(const BigPoly &poly, bool force, MemoryPool &pool)
        {
            return duplicate_if_needed(poly.pointer(), poly.coeff_count() * poly.coeff_uint64_count(), force, pool);
        }

        bool are_poly_coefficients_less_than(const BigPoly &poly, const BigUInt &max_coeff)
        {
            return util::are_poly_coefficients_less_than(poly.pointer(), poly.coeff_count(), poly.coeff_uint64_count(), max_coeff.pointer(), max_coeff.uint64_count());
        }
    }

    void Evaluator::negate(const BigPolyArray &encrypted, BigPolyArray &destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        int count = encrypted.size();

        // Verify parameters.
        if (encrypted.coeff_count() != coeff_count || encrypted.coeff_bit_count() != coeff_bit_count || count < 2)
        {
            throw invalid_argument("encrypted is not valid for encryption parameters");
        }
#ifdef _DEBUG
        for (int i = 0; i < count; ++i)
        {
            if (encrypted[i].significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted[i], coeff_modulus_))
            {
                throw invalid_argument("encrypted is not valid for encryption parameters");
            }
        }
#endif
        if (destination.size() != count || destination.coeff_count() != coeff_count || destination.coeff_bit_count() != coeff_bit_count)
        {
            destination.resize(count, coeff_count, coeff_bit_count);
        }

        // Negate each bigpoly in the array
        negate_poly_coeffmod(encrypted.pointer(0), coeff_count * count, coeff_modulus_.pointer(), coeff_uint64_count, destination.pointer(0));
    }

    void Evaluator::add(const BigPolyArray &encrypted1, const BigPolyArray &encrypted2, BigPolyArray &destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Verify parameters.
        if (encrypted1.coeff_count() != coeff_count || encrypted1.coeff_bit_count() != coeff_bit_count || encrypted1.size() < 2)
        {
            throw invalid_argument("encrypted1 is not valid for encryption parameters");
        }
        if (encrypted2.coeff_count() != coeff_count || encrypted2.coeff_bit_count() != coeff_bit_count || encrypted2.size() < 2)
        {
            throw invalid_argument("encrypted2 is not valid for encryption parameters");
        }
#ifdef _DEBUG
        for (int i = 0; i < encrypted1.size(); ++i) 
        {
           if (encrypted1[i].significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted1[i], coeff_modulus_))
          {
                throw invalid_argument("encrypted1 is not valid for encryption parameters");
           }
        }

        for (int j = 0; j < encrypted2.size(); ++j)
        {
            if (encrypted2[j].significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted2[j], coeff_modulus_))
            {
                throw invalid_argument("encrypted2 is not valid for encryption parameters");
            }
        }
#endif
        int max_count = max(encrypted1.size(), encrypted2.size());
        int min_count = min(encrypted1.size(), encrypted2.size());

        if (destination.size() != max_count || destination.coeff_count() != coeff_count || destination.coeff_bit_count() != coeff_bit_count)
        {
            destination.resize(max_count, coeff_count, coeff_bit_count);
        }

        // Add BigPolyArrays BigPoly-wise where both arrays have a BigPoly 
        add_poly_poly_coeffmod(encrypted1.pointer(0), encrypted2.pointer(0), coeff_count * min_count, coeff_modulus_.pointer(), coeff_uint64_count, destination.pointer(0));

        // If the sizes are the same, we are done
        if (max_count == min_count)
        {
            return;
        }

        // Copy the remainding BigPolys of the array with larger count into destination
        set_poly_poly((encrypted1.size() == max_count ? encrypted1 : encrypted2).pointer(min_count), 
            coeff_count * (max_count - min_count), coeff_uint64_count, destination.pointer(min_count));
    }

    void Evaluator::add_many(const vector<BigPolyArray> &encrypteds, BigPolyArray &destination)
    {
        if (encrypteds.empty())
        {
            throw invalid_argument("encrypteds cannot be empty");
        }

        destination = encrypteds[0];
        for (vector<BigPolyArray>::size_type i = 1; i < encrypteds.size(); ++i)
        {
            add(destination, encrypteds[i], destination);
        }
    }

    void Evaluator::sub(const BigPolyArray &encrypted1, const BigPolyArray &encrypted2, BigPolyArray &destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        int encrypted1_count = encrypted1.size();
        int encrypted2_count = encrypted2.size();

        // Verify parameters.
        if (encrypted1.coeff_count() != coeff_count || encrypted1.coeff_bit_count() != coeff_bit_count || encrypted1_count < 2)
        {
            throw invalid_argument("encrypted1 is not valid for encryption parameters");
        }
        if (encrypted2.coeff_count() != coeff_count || encrypted2.coeff_bit_count() != coeff_bit_count || encrypted2_count < 2)
        {
            throw invalid_argument("encrypted2 is not valid for encryption parameters");
        }
#ifdef _DEBUG
        for (int i = 0; i < encrypted1_count; ++i)
        {
            if (encrypted1[i].significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted1[i], coeff_modulus_))
            {
                throw invalid_argument("encrypted1 is not valid for encryption parameters");
            }
        }

        for (int j = 0; j < encrypted2_count; ++j)
        {
            if (encrypted2[j].significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted2[j], coeff_modulus_))
            {
                throw invalid_argument("encrypted2 is not valid for encryption parameters");
            }
        }
#endif

        int max_count = max(encrypted1_count, encrypted2_count);
        int min_count = min(encrypted1_count, encrypted2_count);

        if (destination.size() != max_count || destination.coeff_count() != coeff_count || destination.coeff_bit_count() != coeff_bit_count)
        {
            destination.resize(max_count, coeff_count, coeff_bit_count);
        }

        // Subtract polynomials.
        sub_poly_poly_coeffmod(encrypted1.pointer(0), encrypted2.pointer(0), coeff_count * min_count, coeff_modulus_.pointer(), coeff_uint64_count, destination.pointer(0));

        // If the sizes are the same, we are done
        if (max_count == min_count)
        {
            return;
        }

        // If encrypted1 has larger count, copy remaining entries into destination.
        if (encrypted1_count == max_count) 
        {
            // Copy the remainding BigPolys of the array with larger count into destination
            set_poly_poly(encrypted1.pointer(min_count), coeff_count * (max_count - min_count), coeff_uint64_count, destination.pointer(min_count));
        }
        // Otherwise, encrypted2 has larger count, negate remaining entries and copy into destination.
        else
        {
            negate_poly_coeffmod(encrypted2.pointer(min_count), coeff_count * (max_count - min_count), coeff_modulus_.pointer(), coeff_uint64_count, destination.pointer(min_count));
        }
    }

    void Evaluator::multiply(const BigPolyArray &encrypted1, const BigPolyArray &encrypted2, BigPolyArray &destination)
    {
        MemoryPool &pool = *MemoryPool::default_pool();

        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        int encrypted1_count = encrypted1.size();
        int encrypted2_count = encrypted2.size();

        // Verify parameters.
        if (encrypted1.coeff_count() != coeff_count || encrypted1.coeff_bit_count() != coeff_bit_count || encrypted1_count < 2)
        {
            throw invalid_argument("encrypted1 is not valid for encryption parameters");
        }
        if (encrypted2.coeff_count() != coeff_count || encrypted2.coeff_bit_count() != coeff_bit_count || encrypted2_count < 2)
        {
            throw invalid_argument("encrypted2 is not valid for encryption parameters");
        }
#ifdef _DEBUG
        for (int i = 0; i < encrypted1_count; ++i)
        {
            if (encrypted1[i].significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted1[i], coeff_modulus_))
            {
                throw invalid_argument("encrypted1 is not valid for encryption parameters");
            }
        }

        for (int j = 0; j < encrypted2_count; ++j)
        {
            if (encrypted2[j].significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted2[j], coeff_modulus_))
            {
                throw invalid_argument("encrypted2 is not valid for encryption parameters");
            }
        }
#endif
        // Determine destination.size()
        int dest_count = encrypted1_count + encrypted2_count - 1;

        // Resize destination if necessary
        if (destination.size() != dest_count || destination.coeff_count() != coeff_count || destination.coeff_bit_count() != coeff_bit_count)
        {
            destination.resize(dest_count, coeff_count, coeff_bit_count);
        }

        // Clear destination
        destination.set_zero();

        // Determine if FFT can be used.
        bool use_fft = polymod_.coeff_count_power_of_two() >= 0 && polymod_.is_one_zero_one();

        if (use_fft)
        {
            // Significant size of plain modulus
            int plain_modulus_uint64_count = divide_round_up(plain_modulus_bit_count_, bits_per_uint64);

            // Stores sums of products of polynomials
            int wide_product_bit_count = product_coeff_bit_count_ 
                + get_significant_bit_count(static_cast<uint64_t>(encrypted1_count))
                + get_significant_bit_count(static_cast<uint64_t>(encrypted2_count));
            int wide_product_uint64_count = divide_round_up(wide_product_bit_count, bits_per_uint64);

            // Stores the sums multiplied by plain modulus
            int intermediate_bit_count = wide_product_bit_count + plain_modulus_bit_count_;
            int intermediate_uint64_count = divide_round_up(intermediate_bit_count, bits_per_uint64);

            // Do we need to further widen the coefficient modulus due to size increase from summation of cross terms?
            ConstPointer wide_coeff_modulus = duplicate_uint_if_needed(coeff_modulus_.pointer(), coeff_modulus_.uint64_count(), intermediate_uint64_count, false, pool);

            Pointer intermediate(allocate_uint(intermediate_uint64_count, pool));
            Pointer quotient(allocate_uint(intermediate_uint64_count, pool));

            // To store the current product of individual polynomials
            Pointer temp_poly(allocate_poly(coeff_count, wide_product_uint64_count, pool));

            // To store the current component of dest before scaling or modular reduction
            Pointer temp_dest(allocate_poly(coeff_count, wide_product_uint64_count, pool)); 

            // each index of destination corresponds to the secret key s^index
            // loop over temp_dest, filling in the appropriate cross terms for each index, to be scaled and modular reduced later into destination
            // for each index, we will loop over indices of encrypted1
            // initialize a variable to denote when to stop looping of encrypted1
            int current_encrypted1_limit = 0;
            for (int secret_power_index = 0; secret_power_index < dest_count; ++secret_power_index)
            {
                // Reset temp_dest
                set_zero_poly(coeff_count, wide_product_uint64_count, temp_dest.get());

                // loop over encrypted1 components [i], seeing if a match exists with an encrypted2 component [j] such that [i+j]=[secret_power_index]
                // only need to check encrypted1 components up to and including [secret_power_index], and strictly less than [encrypted.size()]
                current_encrypted1_limit = min(encrypted1_count, secret_power_index + 1); 

                for (int encrypted1_index = 0; encrypted1_index < current_encrypted1_limit; ++encrypted1_index)
                {
                    // check if a corresponding component in encrypted2 exists
                    if (encrypted2_count > secret_power_index - encrypted1_index)
                    {
                        int encrypted2_index = secret_power_index - encrypted1_index;

                        // reset temp_poly
                        set_zero_poly(coeff_count, wide_product_uint64_count, temp_poly.get());

                        // fft multiply encrypted1[encrypted1_index] with encrypted2[encrypted2_index] and store in temp_poly
                        fftmultiply_poly_poly_polymod(encrypted1.pointer(encrypted1_index), encrypted2.pointer(encrypted2_index), polymod_.coeff_count_power_of_two(), coeff_uint64_count, wide_product_uint64_count, temp_poly.get(), pool);
                        
                        // update temp_dest with current value of temp_poly
                        add_poly_poly(temp_poly.get(), temp_dest.get(), coeff_count, wide_product_uint64_count, temp_dest.get());
                    }
                }

                // when temp_dest is fully populated,  multiply by plain_modulus and divide by coeff_modulus and then modulo by coeff_modulus
                // put the result in  destination[secret_power_index] 
                uint64_t *temp_dest_coeff = temp_dest.get();
                uint64_t *dest_coeff = destination.pointer(secret_power_index);
                
                Pointer big_alloc(allocate_uint(2 * intermediate_uint64_count, pool));
                for (int coeff_index = 0; coeff_index < coeff_count; ++coeff_index)
                {
                    bool coeff_is_negative = is_high_bit_set_uint(temp_dest_coeff, wide_product_uint64_count);
                    if (coeff_is_negative)
                    {
                        negate_uint(temp_dest_coeff, wide_product_uint64_count, temp_dest_coeff);
                    }
                    multiply_uint_uint(temp_dest_coeff, wide_product_uint64_count, plain_modulus_.pointer(), plain_modulus_uint64_count, intermediate_uint64_count, intermediate.get());
                    add_uint_uint(intermediate.get(), wide_coeff_modulus_div_two_.pointer(), intermediate_uint64_count, intermediate.get());
                    divide_uint_uint_inplace(intermediate.get(), wide_coeff_modulus.get(), intermediate_uint64_count, quotient.get(), pool, big_alloc.get());
                    modulo_uint_inplace(quotient.get(), intermediate_uint64_count, mod_, pool, big_alloc.get());

                    if (coeff_is_negative)
                    {
                        negate_uint_mod(quotient.get(), coeff_modulus_.pointer(), coeff_uint64_count, dest_coeff);
                    }
                    else
                    {
                        set_uint_uint(quotient.get(), coeff_uint64_count, dest_coeff);
                    }

                    temp_dest_coeff += wide_product_uint64_count;
                    dest_coeff += coeff_uint64_count;
                }
            }
        }
        else
        {
            // Size of the product of polynomials
            int product_coeff_count = 2 * coeff_count - 1;

            // Significant size of plain modulus
            int plain_modulus_uint64_count = divide_round_up(plain_modulus_bit_count_, bits_per_uint64);

            // Stores sums of products of polynomials
            int wide_product_bit_count = product_coeff_bit_count_
                + get_significant_bit_count(static_cast<uint64_t>(encrypted1_count))
                + get_significant_bit_count(static_cast<uint64_t>(encrypted2_count));
            int wide_product_uint64_count = divide_round_up(wide_product_bit_count, bits_per_uint64);

            // Stores the sums multiplied by plain modulus
            int intermediate_bit_count = wide_product_bit_count + plain_modulus_bit_count_;
            int intermediate_uint64_count = divide_round_up(intermediate_bit_count, bits_per_uint64);

            // Do we need to further widen the coefficient modulus due to size increase from summation of cross terms?
            ConstPointer wide_coeff_modulus = duplicate_uint_if_needed(coeff_modulus_.pointer(), coeff_modulus_.uint64_count(), intermediate_uint64_count, false, pool);

            // Sizes of the encrypteds
            int encrypted1_coeff_count = encrypted1.coeff_count();
            int encrypted1_coeff_bit_count = encrypted1.coeff_bit_count();
            int encrypted1_coeff_uint64_count = divide_round_up(encrypted1_coeff_bit_count, bits_per_uint64);
            int encrypted2_coeff_count = encrypted2.coeff_count();
            int encrypted2_coeff_bit_count = encrypted2.coeff_bit_count();
            int encrypted2_coeff_uint64_count = divide_round_up(encrypted2_coeff_bit_count, bits_per_uint64);

            Pointer intermediate(allocate_uint(intermediate_uint64_count, pool));
            Pointer quotient(allocate_uint(intermediate_uint64_count, pool));

            // To store the current product of individual polynomials
            Pointer temp_poly(allocate_poly(product_coeff_count, wide_product_uint64_count, pool));

            // To store the current component of dest before scaling or any reduction
            Pointer temp_dest(allocate_poly(product_coeff_count, wide_product_uint64_count, pool));

            // To store result of temp_dest reduced modulo coeffmod
            Pointer temp_dest_mod(allocate_poly(product_coeff_count, coeff_uint64_count, pool));

            // each index of destination corresponds to the secret key s^index
            // loop over dest_temp, filling in the appropriate cross terms for each index, to be scaled and modular reduced later into destination
            // for each index, we will loop over indices of encrypted1
            // initialize a variable to denote when to stop looping of encrypted1
            int current_encrypted1_limit = 0;
            Pointer big_alloc(allocate_uint(2 * intermediate_uint64_count, pool));
            for (int secret_power_index = 0; secret_power_index < dest_count; ++secret_power_index)
            {
                // reset temp_dest and temp_dest_mod
                set_zero_poly(product_coeff_count, wide_product_uint64_count, temp_dest.get());
                set_zero_poly(product_coeff_count, coeff_uint64_count, temp_dest_mod.get());

                // loop over encrypted1 components [i], seeing if a match exists with an encrypted2 component [j] such that [i+j]=[secret_power_index]
                // only need to check encrypted1 components up to and including [secret_power_index], and strictly less than [encrypted.size()]
                current_encrypted1_limit = min(encrypted1_count, secret_power_index + 1);

                for (int encrypted1_index = 0; encrypted1_index < current_encrypted1_limit; ++encrypted1_index)
                {
                    // check if a corresponding component in encrypted2 exists
                    if (encrypted2_count > secret_power_index - encrypted1_index)
                    {
                        int encrypted2_index = secret_power_index - encrypted1_index;

                        // reset temp_poly
                        set_zero_poly(product_coeff_count, wide_product_uint64_count, temp_poly.get());

                        //multiply encrypted1[encrypted1_index] with encrypted2[encrypted2_index] and store in temp_poly
                        multiply_poly_poly(encrypted1.pointer(encrypted1_index), encrypted1_coeff_count, encrypted1_coeff_uint64_count, 
                            encrypted2.pointer(encrypted2_index), encrypted2_coeff_count, encrypted2_coeff_uint64_count, 
                            product_coeff_count, wide_product_uint64_count, temp_poly.get(), pool);

                        // update temp_dest with current value of temp_poly
                        add_poly_poly(temp_poly.get(), temp_dest.get(), product_coeff_count, wide_product_uint64_count, temp_dest.get());
                    }
                }

                // when temp_dest is fully populated,  multiply by plain_modulus and divide by coeff_modulus and then modulo by coeff_modulus
                // put result into temp_dest_mod
                uint64_t *temp_dest_coeff = temp_dest.get();
                uint64_t *temp_dest_mod_coeff = temp_dest_mod.get();

                for (int coeff_index = 0; coeff_index < product_coeff_count; ++coeff_index)
                {
                    multiply_uint_uint(temp_dest_coeff, wide_product_uint64_count, plain_modulus_.pointer(), plain_modulus_uint64_count, intermediate_uint64_count, intermediate.get());
                    add_uint_uint(intermediate.get(), wide_coeff_modulus_div_two_.pointer(), intermediate_uint64_count, intermediate.get());
                    divide_uint_uint_inplace(intermediate.get(), wide_coeff_modulus_.pointer(), intermediate_uint64_count, quotient.get(), pool, big_alloc.get());
                    modulo_uint_inplace(quotient.get(), intermediate_uint64_count, mod_, pool, big_alloc.get());
                    
                    set_uint_uint(quotient.get(), coeff_uint64_count, temp_dest_mod_coeff);

                    temp_dest_coeff += wide_product_uint64_count;
                    temp_dest_mod_coeff += coeff_uint64_count;
                }

                // Perform polynomial modulo.
                modulo_poly_inplace(temp_dest_mod.get(), product_coeff_count, polymod_, mod_, pool);

                // Copy to destination.
                set_poly_poly(temp_dest_mod.get(), coeff_count, coeff_uint64_count, destination.pointer(secret_power_index));
            }
        }
    }


    void Evaluator::relinearize(const BigPolyArray &encrypted, BigPolyArray &destination, int destination_size)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        int encrypted_count = encrypted.size();

        // Verify parameters.
        if (encrypted.coeff_count() != coeff_count || encrypted.coeff_bit_count() != coeff_bit_count || encrypted_count < 2)
        {
            throw invalid_argument("encrypted is not valid for encryption parameters");
        }
#ifdef _DEBUG
        for (int i = 0; i < encrypted_count; ++i)
        {
            if (encrypted[i].significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted[i], coeff_modulus_))
            {
                throw invalid_argument("encrypted is not valid for encryption parameters");
            }
        }
#endif

        if (destination_size < 2 || destination_size > encrypted_count)
        {
            throw invalid_argument("destination_size must be greater than or equal to 2 and less than or equal to current count");
        }

        // Ensure destination is appropriate size
        destination.resize(destination_size, coeff_count, coeff_bit_count);

        // If encrypted is already at the desired level, return
        if (destination_size == encrypted_count)
        {
            destination = encrypted;
            return;
        }

        if (evaluation_keys_.size() < encrypted_count - 2)
        {
            throw invalid_argument("not enough evaluation keys");
        }

        // Do the relinearization
        relinearize_internal(encrypted, destination, destination_size);
    }

    void Evaluator::relinearize_internal(const BigPolyArray &encrypted, BigPolyArray &destination, int destination_size)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        int encrypted_count = encrypted.size();

        // Relinearize one step at a time
        BigPolyArray temp1(encrypted_count, coeff_count, coeff_bit_count);
        BigPolyArray temp2(encrypted_count - 1, coeff_count, coeff_bit_count);
        temp1 = encrypted;

        // Calculate number of relinearize_one_step calls needed
        int relins_needed = encrypted_count - destination_size;

        // Update temp to store the current result after relinearization
        for (int i = 0; i < relins_needed; ++i)
        {
            relinearize_one_step(temp1, temp2);
            temp1 = temp2;
        }

        // Put the output of final relinearization into destination.
        // We assume here that destination has been resized by the calling function.
        set_poly_poly(temp2.pointer(0), destination_size * coeff_count, coeff_uint64_count, destination.pointer(0));
    }

    void Evaluator::relinearize_one_step(BigPolyArray &encrypted, BigPolyArray &destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        int encrypted_count = encrypted.size();

        // Ensure destination is correct size
        destination.resize(encrypted.size() - 1, coeff_count, coeff_bit_count);
  
        // encrypted[2], encrypted[3], ... encrypted[count - 2] all stay the same
        if(encrypted.size() > 3)
        {
            set_poly_poly(encrypted.pointer(2), (encrypted.size() - 3) * coeff_count, coeff_uint64_count, destination.pointer(2));
        }

        // decompose encrypted[count-1] into base w
        // want to create a BigPolyArray, each of whose components i is (encrypted[count-1])^(i) - in the notation of FV paper
        BigPolyArray decomp_encrypted_last(evaluation_keys_[0].first.size(), coeff_count, coeff_bit_count);

        // BigPolyArray will be populated one poly at a time.
        // Create a polynomial to store the current decomposition value which will be copied into BigPolyArray to populate it at the current index.
        int shift = 0;
        for (int j = 0; j < evaluation_keys_[0].first.size(); ++j)
        {
            uint64_t *encrypted_coeff = encrypted.pointer(encrypted.size() - 1);
            uint64_t *decomp_coeff = decomp_encrypted_last.pointer(j);

            // Isolate decomposition_bit_count_ bits for each coefficient.
            for (int coeff_index = 0; coeff_index < coeff_count; ++coeff_index)
            {
                right_shift_uint(encrypted_coeff, shift, coeff_uint64_count, decomp_coeff);
                filter_highbits_uint(decomp_coeff, coeff_uint64_count, decomposition_bit_count_);

                encrypted_coeff += coeff_uint64_count;
                decomp_coeff += coeff_uint64_count;
            }

            // Increase shift by decomposition_bit_count_ for next iteration.
            shift += decomposition_bit_count_;
        }

        MemoryPool &pool = *MemoryPool::default_pool();

        Pointer innerproduct(allocate_poly(coeff_count, coeff_uint64_count, pool));
        // destination[0] = encrypted[0] + < appropriate evk , decomposition of encrypted[count-1]>
        dot_product_bigpolyarray_polymod_coeffmod(evaluation_keys_[encrypted.size() - 3].first.pointer(0), decomp_encrypted_last.pointer(0), evaluation_keys_[0].first.size(), polymod_, mod_, innerproduct.get(), pool);
        add_poly_poly_coeffmod(encrypted.pointer(0), innerproduct.get(), coeff_count, mod_.get(), coeff_uint64_count, destination.pointer(0));

        // destination[1] = encrypted[1] + < appropriate evk , appropriate term in decomposition of encrypted[count-1] >
        dot_product_bigpolyarray_polymod_coeffmod(evaluation_keys_[encrypted.size() - 3].second.pointer(0), decomp_encrypted_last.pointer(0), evaluation_keys_[0].first.size(), polymod_, mod_, innerproduct.get(), pool);
        add_poly_poly_coeffmod(encrypted.pointer(1), innerproduct.get(), coeff_count, mod_.get(), coeff_uint64_count, destination.pointer(1));
    }

    void Evaluator::add_plain(const BigPolyArray &encrypted, const BigPoly &plain, BigPolyArray &destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        int count = encrypted.size();

        // Verify parameters.
        if (encrypted.coeff_count() != coeff_count || encrypted.coeff_bit_count() != coeff_bit_count || count < 2)
        {
            throw invalid_argument("encrypted is not valid for encryption parameters");
        }
#ifdef _DEBUG
        for (int i = 0; i < count; ++i)
        {
            if (encrypted[i].significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted[i], coeff_modulus_))
            {
                throw invalid_argument("encrypted is not valid for encryption parameters");
            }
        }
        if (plain.significant_coeff_count() >= coeff_count || !are_poly_coefficients_less_than(plain, plain_modulus_))
        {
            throw invalid_argument("plain is too large to be represented by encryption parameters");
        }
#endif
        if (destination.size() != count || destination.coeff_count() != coeff_count || destination.coeff_bit_count() != coeff_bit_count)
        {
            destination.resize(count, coeff_count, coeff_bit_count);
        }

        int plain_coeff_uint64_count = divide_round_up(plain.coeff_bit_count(), bits_per_uint64);

        // Multiply plain by scalar coeff_div_plaintext and reposition if in upper-half, store in destination[0]
        preencrypt(plain.pointer(), plain.coeff_count(), plain_coeff_uint64_count, destination.pointer(0));

        // Add preencrypted-version of plain with encrypted[0], store in destination[0].
        add_poly_poly_coeffmod(encrypted.pointer(0), destination.pointer(0), coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, destination.pointer(0));

        // Set the remainder of destination to be as in encrypted.
        set_poly_poly(encrypted.pointer(1), coeff_count * (count - 1), coeff_uint64_count, destination.pointer(1));
    }

    void Evaluator::sub_plain(const BigPolyArray &encrypted, const BigPoly &plain, BigPolyArray &destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        int count = encrypted.size();

        // Verify parameters.
        if (encrypted.coeff_count() != coeff_count || encrypted.coeff_bit_count() != coeff_bit_count || count < 2)
        {
            throw invalid_argument("encrypted is not valid for encryption parameters");
        }
#ifdef _DEBUG
        for (int i = 0; i < count; ++i)
        {
            if (encrypted[i].significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted[i], coeff_modulus_))
            {
                throw invalid_argument("encrypted is not valid for encryption parameters");
            }
        }
        if (plain.significant_coeff_count() >= coeff_count || !are_poly_coefficients_less_than(plain, plain_modulus_))
        {
            throw invalid_argument("plain is too large to be represented by encryption parameters");
        }
#endif
        if (destination.size() != count || destination.coeff_count() != coeff_count || destination.coeff_bit_count() != coeff_bit_count)
        {
            destination.resize(count, coeff_count, coeff_bit_count);
        }

        int plain_coeff_uint64_count = divide_round_up(plain.coeff_bit_count(), bits_per_uint64);

        // Multiply plain by scalar coeff_div_plaintext and reposition if in upper-half, store in destination[0]
        preencrypt(plain.pointer(), plain.coeff_count(), plain_coeff_uint64_count, destination.pointer(0));

        // Subtract preencrypted-version of plain2 with encrypted[0], store in destination[0].
        sub_poly_poly_coeffmod(encrypted.pointer(0), destination.pointer(0), coeff_count, coeff_modulus_.pointer(), coeff_uint64_count, destination.pointer(0));

        // Set the remainder of destination to be as in encrypted.
        set_poly_poly(encrypted.pointer(1), coeff_count * (count - 1), coeff_uint64_count, destination.pointer(1));
    }

    void Evaluator::multiply_plain(const BigPolyArray &encrypted, const BigPoly &plain, BigPolyArray &destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        int count = encrypted.size();

        // Verify parameters.
        if (encrypted.coeff_count() != coeff_count || encrypted.coeff_bit_count() != coeff_bit_count || count < 2)
        {
            throw invalid_argument("encrypted is not valid for encryption parameters");
        }
        if (plain.is_zero())
        {
            throw invalid_argument("plain cannot be zero");
        }
#ifdef _DEBUG
        for (int i = 0; i < count; ++i)
        {
            if (encrypted[i].significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted[i], coeff_modulus_))
            {
                throw invalid_argument("encrypted is not valid for encryption parameters");
            }
        }
        if (plain.significant_coeff_count() >= coeff_count || !are_poly_coefficients_less_than(plain, plain_modulus_))
        {
            throw invalid_argument("plain is too large to be represented by encryption parameters");
        }
#endif
        if (destination.size() != count || destination.coeff_count() != coeff_count || destination.coeff_bit_count() != coeff_bit_count)
        {
            destination.resize(count, coeff_count, coeff_bit_count);
        }

        MemoryPool &pool = *MemoryPool::default_pool();

        // Reposition coefficients.
        Pointer moved2ptr(allocate_poly(coeff_count, coeff_uint64_count, pool));
        int plain_coeff_count = min(plain.significant_coeff_count(), coeff_count);
        int plain_coeff_uint64_count = plain.coeff_uint64_count();
        const uint64_t *plain_coeff = plain.pointer();
        uint64_t *moved2_coeff = moved2ptr.get();
        for (int i = 0; i < plain_coeff_count; ++i)
        {
            set_uint_uint(plain_coeff, plain_coeff_uint64_count, coeff_uint64_count, moved2_coeff);
            bool is_upper_half = is_greater_than_or_equal_uint_uint(moved2_coeff, plain_upper_half_threshold_.pointer(), coeff_uint64_count);
            if (is_upper_half)
            {
                add_uint_uint(moved2_coeff, plain_upper_half_increment_.pointer(), coeff_uint64_count, moved2_coeff);
            }
            moved2_coeff += coeff_uint64_count;
            plain_coeff += plain_coeff_uint64_count;
        }
        for (int i = plain_coeff_count; i < coeff_count; ++i)
        {
            set_zero_uint(coeff_uint64_count, moved2_coeff);
            moved2_coeff += coeff_uint64_count;
        }

        // Get pointer to inputs (duplicated if needed).
        ConstPointer encryptedptr = duplicate_if_needed(encrypted.pointer(0), count * coeff_count * coeff_uint64_count, encrypted.pointer(0) == destination.pointer(0), pool);
        const uint64_t *moving_ptr = encryptedptr.get();
        for (int i = 0; i < count; ++i)
        {
            // Use normal polynomial multiplication.
            multiply_poly_poly_polymod_coeffmod(moving_ptr, moved2ptr.get(), polymod_, mod_, destination.pointer(i), pool);
                
            // Move to next polynomial
            moving_ptr += coeff_count * coeff_uint64_count;
        }
    }

    void Evaluator::preencrypt(const uint64_t *plain, int plain_coeff_count, int plain_coeff_uint64_count, uint64_t *destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        if (plain_coeff_count > coeff_count)
        {
            plain_coeff_count = coeff_count;
        }

        // Multiply plain by scalar coeff_div_plaintext and reposition if in upper-half.
        MemoryPool &pool = *MemoryPool::default_pool();
        if (plain == destination)
        {
            // If plain and destination are same poly, then need another storage for multiply output.
            Pointer temp(allocate_uint(coeff_uint64_count, pool));
            for (int i = 0; i < plain_coeff_count; ++i)
            {
                multiply_uint_uint(plain, plain_coeff_uint64_count, coeff_div_plain_modulus_.pointer(), coeff_uint64_count, coeff_uint64_count, temp.get());
                bool is_upper_half = is_greater_than_or_equal_uint_uint(temp.get(), upper_half_threshold_.pointer(), coeff_uint64_count);
                if (is_upper_half)
                {
                    add_uint_uint(temp.get(), upper_half_increment_.pointer(), coeff_uint64_count, destination);
                }
                else
                {
                    set_uint_uint(temp.get(), coeff_uint64_count, destination);
                }
                plain += plain_coeff_uint64_count;
                destination += coeff_uint64_count;
            }
        }
        else
        {
            for (int i = 0; i < plain_coeff_count; ++i)
            {
                //Multiply plain by coeff_div_plain_modulus and put the result in destination
                multiply_uint_uint(plain, plain_coeff_uint64_count, coeff_div_plain_modulus_.pointer(), coeff_uint64_count, coeff_uint64_count, destination);

                // check if destination >= upper half threshold
                bool is_upper_half = is_greater_than_or_equal_uint_uint(destination, upper_half_threshold_.pointer(), coeff_uint64_count);
                if (is_upper_half)
                {
                    add_uint_uint(destination, upper_half_increment_.pointer(), coeff_uint64_count, destination);
                }
                plain += plain_coeff_uint64_count;
                destination += coeff_uint64_count;
            }
        }

        // Zero any remaining coefficients.
        for (int i = plain_coeff_count; i < coeff_count; ++i)
        {
            set_zero_uint(coeff_uint64_count, destination);
            destination += coeff_uint64_count;
        }
    }

    BigPolyArray Evaluator::multiply_many(vector<BigPolyArray> encrypteds)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();

        size_t original_size = encrypteds.size();

        // Verify parameters.
        if (original_size == 0)
        {
            throw invalid_argument("encrypteds vector must not be empty");
        }

        // If there is only one ciphertext, return it after checking validity.
        if (original_size == 1)
        {
            if (encrypteds[0].coeff_count() != coeff_count || encrypteds[0].coeff_bit_count() != coeff_bit_count || encrypteds[0].size() < 2)
            {
                throw invalid_argument("encrypteds is not valid for encryption parameters");
            }
            return encrypteds[0];
        }

        // Repeatedly multiply and add to the back of the vector until the end is reached
        for (size_t i = 0; i < encrypteds.size() - 1; i += 2)
        {
            encrypteds.push_back(multiply(encrypteds[i], encrypteds[i + 1]));
        }

        return encrypteds[encrypteds.size() - 1];
    }

    void Evaluator::exponentiate(const BigPolyArray &encrypted, uint64_t exponent, BigPolyArray &destination)
    {
        // Extract encryption parameters.
        int coeff_count = poly_modulus_.coeff_count();
        int coeff_bit_count = poly_modulus_.coeff_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);

        // Verify parameters.
        if (encrypted.coeff_count() != coeff_count || encrypted.coeff_bit_count() != coeff_bit_count || encrypted.size() < 2)
        {
            throw invalid_argument("encrypted is not valid for encryption parameters");
        }
        if (exponent == 0)
        {
            throw invalid_argument("exponent cannot be 0");
        }
#ifdef _DEBUG
        for (int i = 0; i < encrypted.size(); ++i)
        {
            if (encrypted[i].significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(encrypted[i], coeff_modulus_))
            {
                throw invalid_argument("encrypted is not valid for encryption parameters");
            }
        }
#endif
        if (exponent == 1)
        {
            destination = encrypted;
            return;
        }

        vector<BigPolyArray> exp_vector(exponent, encrypted);
        multiply_many(exp_vector, destination);
    }

    Evaluator::Evaluator(const EncryptionParameters &parms, const EvaluationKeys &evaluation_keys) :
        poly_modulus_(parms.poly_modulus()), coeff_modulus_(parms.coeff_modulus()), plain_modulus_(parms.plain_modulus()),
        decomposition_bit_count_(parms.decomposition_bit_count()), evaluation_keys_(evaluation_keys)
    {
        // Verify required parameters are non-zero and non-nullptr.
        if (poly_modulus_.is_zero())
        {
            throw invalid_argument("poly_modulus cannot be zero");
        }
        if (coeff_modulus_.is_zero())
        {
            throw invalid_argument("coeff_modulus cannot be zero");
        }
        if (plain_modulus_.is_zero())
        {
            throw invalid_argument("plain_modulus cannot be zero");
        }
        if (decomposition_bit_count_ < 0)
        {
            throw invalid_argument("decomposition_bit_count must be non-negative");
        }

        // Verify parameters.
        if (plain_modulus_ >= coeff_modulus_)
        {
            throw invalid_argument("plain_modulus must be smaller than coeff_modulus");
        }
        if (!are_poly_coefficients_less_than(poly_modulus_, coeff_modulus_))
        {
            throw invalid_argument("poly_modulus cannot have coefficients larger than coeff_modulus");
        }

        // Resize encryption parameters to consistent size.
        int coeff_count = poly_modulus_.significant_coeff_count();
        int coeff_bit_count = coeff_modulus_.significant_bit_count();
        int coeff_uint64_count = divide_round_up(coeff_bit_count, bits_per_uint64);
        if (poly_modulus_.coeff_count() != coeff_count || poly_modulus_.coeff_bit_count() != coeff_bit_count)
        {
            poly_modulus_.resize(coeff_count, coeff_bit_count);
        }
        if (coeff_modulus_.bit_count() != coeff_bit_count)
        {
            coeff_modulus_.resize(coeff_bit_count);
        }
        if (plain_modulus_.bit_count() != coeff_bit_count)
        {
            plain_modulus_.resize(coeff_bit_count);
        }
        if (decomposition_bit_count_ > coeff_bit_count)
        {
            decomposition_bit_count_ = coeff_bit_count;
        }

        // If nonempty, determine if provided evaluation_keys are correct size.
        MemoryPool &pool = *MemoryPool::default_pool();
        if (evaluation_keys_.size() != 0)
        {
            // Determine how many BigPolys there should be in each array in each pair
            int evaluation_factors_count = 0;
            Pointer evaluation_factor(allocate_uint(coeff_uint64_count, pool));
            set_uint(1, coeff_uint64_count, evaluation_factor.get());
            while (!is_zero_uint(evaluation_factor.get(), coeff_uint64_count) && is_less_than_uint_uint(evaluation_factor.get(), coeff_modulus_.pointer(), coeff_uint64_count))
            {
                left_shift_uint(evaluation_factor.get(), decomposition_bit_count_, coeff_uint64_count, evaluation_factor.get());
                evaluation_factors_count++;
            }

            // Verify evaluation keys.
            for (int i = 0; i < evaluation_keys_.size(); ++i)
            {
                if (evaluation_keys_[i].first.size() != evaluation_factors_count || evaluation_keys_[i].second.size() != evaluation_factors_count ||
                    evaluation_keys_[i].first.coeff_count() != coeff_count || evaluation_keys_[i].second.coeff_count() != coeff_count ||
                    evaluation_keys_[i].first.coeff_bit_count() != coeff_bit_count || evaluation_keys_[i].second.coeff_bit_count() != coeff_bit_count)
                {
                    throw invalid_argument("evaluation_keys is not valid for encryption parameters");
                }
                for (int j = 0; j < evaluation_factors_count; ++j)
                {

                    if (evaluation_keys_[i].first[j].significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(evaluation_keys_[i].first[j], coeff_modulus_) ||
                        evaluation_keys_[i].second[j].significant_coeff_count() == coeff_count || !are_poly_coefficients_less_than(evaluation_keys_[i].second[j], coeff_modulus_))
                    {
                        throw invalid_argument("evaluation_keys is not valid for encryption parameters");
                    }
                }
            }
        }

        // Calculate coeff_modulus / plain_modulus.
        coeff_div_plain_modulus_.resize(coeff_bit_count);
        Pointer temp(allocate_uint(coeff_uint64_count, pool));
        divide_uint_uint(coeff_modulus_.pointer(), plain_modulus_.pointer(), coeff_uint64_count, coeff_div_plain_modulus_.pointer(), temp.get(), pool);

        // Calculate (plain_modulus + 1) / 2.
        plain_upper_half_threshold_.resize(coeff_bit_count);
        half_round_up_uint(plain_modulus_.pointer(), coeff_uint64_count, plain_upper_half_threshold_.pointer());

        // Calculate coeff_modulus - plain_modulus.
        plain_upper_half_increment_.resize(coeff_bit_count);
        sub_uint_uint(coeff_modulus_.pointer(), plain_modulus_.pointer(), coeff_uint64_count, plain_upper_half_increment_.pointer());

        // Calculate (plain_modulus + 1) / 2 * coeff_div_plain_modulus.
        upper_half_threshold_.resize(coeff_bit_count);
        multiply_truncate_uint_uint(plain_upper_half_threshold_.pointer(), coeff_div_plain_modulus_.pointer(), coeff_uint64_count, upper_half_threshold_.pointer());

        // Calculate upper_half_increment.
        upper_half_increment_.resize(coeff_bit_count);
        multiply_truncate_uint_uint(plain_modulus_.pointer(), coeff_div_plain_modulus_.pointer(), coeff_uint64_count, upper_half_increment_.pointer());
        sub_uint_uint(coeff_modulus_.pointer(), upper_half_increment_.pointer(), coeff_uint64_count, upper_half_increment_.pointer());

        // Wide enough to store product of two polynomials and reserve one bit for sign (as result of FFT multiplication)
        product_coeff_bit_count_ = 2 * coeff_bit_count + get_significant_bit_count(static_cast<uint64_t>(coeff_count)) + 1;
        plain_modulus_bit_count_ = plain_modulus_.significant_bit_count();

        // Widen coeff modulus.
        int wide_bit_count = product_coeff_bit_count_ + plain_modulus_bit_count_;
        int wide_uint64_count = divide_round_up(wide_bit_count, bits_per_uint64);
        wide_coeff_modulus_.resize(wide_bit_count);
        wide_coeff_modulus_ = coeff_modulus_;

        // Calculate wide_coeff_modulus_ / 2.
        wide_coeff_modulus_div_two_.resize(wide_bit_count);
        right_shift_uint(wide_coeff_modulus_.pointer(), 1, wide_uint64_count, wide_coeff_modulus_div_two_.pointer());

        // Initialize moduli.
        polymod_ = PolyModulus(poly_modulus_.pointer(), coeff_count, coeff_uint64_count);
        mod_ = Modulus(coeff_modulus_.pointer(), coeff_uint64_count, pool);
    }
}
