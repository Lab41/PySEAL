#ifndef SEAL_POLYCRT_H
#define SEAL_POLYCRT_H

#include <cstdint>
#include <vector>
#include <set>
#include "util/mempool.h"
#include "util/modulus.h"
#include "util/polymodulus.h"
#include "bigpoly.h"
#include "biguint.h"

namespace seal
{
    /**
    Provides functionality for encrypting several plaintext numbers into one ciphertext
    for improved memory efficiency and efficient vector operations (SIMD). Multiplying and 
    adding such polynomials together performs the respective operation on each of the slots 
    independently and simultaneously.

    Mathematically speaking, if poly_modulus is x^n+1 and plain_modulus is a prime number t such
    that 2n divides t-1, then integers modulo t contain a primitive 2n-th root of unity and
    the polynomial x^n+1 splits into n distinct linear factors as x^n+1 = (x-a_1)*...*(x-a_n) mod t,
    where the constants a_1, ..., a_n are all the distinct primitive 2n-th roots of unity in integers
    modulo t. The Chinese Remainder Theorem (CRT) states that the plaintext space Z_t[x]/(x^n+1)
    in this case is isomorphic (as an algebra) to the n-fold direct product of fields Z_t. 
    The isomorphism is easy to compute explicitly in both directions, which is what an instance
    of this class can be used for. We refer to the modulus t as the slot modulus. Typically when
    an instance of PolyCRTBuilder is created, the slot modulus is set to be the plaintext modulus.

    @par Thread Safety
    The PolyCRTBuilder class is not thread-safe and a separate PolyCRTBuilder instance is needed
    for each potentially concurrent usage. However, certain important functions of PolyCRTBuilder
    are safe to call from several threads concurrently as long as the threads are guaranteed to
    never access the same slot simultaneously, or mutate a BigUInt or BigPoly while another thread
    is using it, as these objects are not thread-safe.
    */
    class PolyCRTBuilder
    {
    public:
        /**
        Creates a PolyCRTBuilder instance with given slot modulus and given polynomial modulus.
        It is necessary that the polynomial modulus is of the form x^n+1, where n is a power of 2,
        and that the slot modulus is 1 modulo 2n.

        @param[in] slot_modulus The slot modulus
        @param[in] poly_modulus The polynomial modulus
        @throws std::invalid_argument if slot_modulus is zero
        @throws std::invalid_argument if poly_modulus is not of the form x^n+1 where n is a power of 2
        @throws std::invalid_argument if 2*degree(poly_modulus) does not divide slot_modulus-1
        */
        PolyCRTBuilder(BigUInt slot_modulus, BigPoly poly_modulus);

        /**
        Destroys the PolyCRTBuilder and deallocates all memory used by it.
        */
        ~PolyCRTBuilder();

        /**
        Performs a precomputation needed for writing to a particular slot given as argument. 

        @par Thread Safety
        This function is not thread-safe, but it should be safe to call from different threads
        as long as they are guaranteed not to attempt to prepare the same slot simultaneously.
        
        @param[in] index The index of the slot to prepare
        @throws std::invalid_argument if index is larger than or equal to the number of slots
        @throws std::logic_error if an inversion modulo the slot modulus fails
        @see prepare_all_slots() for preparing all slots at once from a single thread.
        */
        void prepare_slot(std::size_t index);

        /**
        Calls prepare_slot() for every index on a single thread in a way that avoids repeated
        memory allocation and deallocation. This is slightly more efficient than calling prepare_slot()
        repeatedly from a single thread, but for improved performance the user might want to call
        prepare_slot() from multiple threads.

        @throws std::logic_error if an inversion modulo the slot modulus fails
        */
        void prepare_all_slots();

        /**
        Writes a given vector of values (represented by BigUInt) into the slots of a given polynomial.
        If the vector contains non-zero entries for which the corresponding slots have not already
        been prepared using prepare_slot(), they will be prepared before the value is set. This function
        runs on a single thread, and for improved performance the user might instead want to call the
        functions add_to_slot(), or possibly set_slot(), from multiple threads.

        @par Format of Parameters
        The number of elements in the vector of inputs must be equal to the number of slots, which is
        equal to the degree of the polynomial modulus. Each entry in the vector of inputs must have
        bit_count() equal to that of the slot modulus and is expected to be already 
        reduced modulo the slot modulus. The destination polynomial will automatically be 
        resized to have correct size, i.e. the same number of coefficients as the polynomial modulus,
        and each coefficient of the same bit_count() as the slot modulus.

        @par Thread Safety
        This function is thread-safe as long as prepare_slot() has been called for each
        slot with a non-zero entry in the values vector, and if each thread writes to 
        a different destination polynomial.

        @param[in] values The vector of values to write into the slots
        @param[out] destination The polynomial to overwrite with the result
        @throws std::invalid_argument if the values vector has incorrect size (see Format of Parameters)
        @throws std::invalid_argument if the entries in the values vector have incorrect size (see Format of Parameters)
        @see add_to_slot() for adding numbers to the slots from multiple threads.
        @see set_slot() for setting the slots to specific values from multiple threads.
        */
        void compose(const std::vector<BigUInt> &values, BigPoly &destination);

        /**
        Writes a given vector of values (represented by BigUInt) into the slots of a polynomial
        and returns it. If the vector contains non-zero entries for which the corresponding slots have 
        not already been prepared using prepare_slot(), they will be prepared before the value is set. 
        This function runs on a single thread, and for improved performance the user might instead want 
        to call the functions add_to_slot(), or possibly set_slot(), from multiple threads.

        @par Format of Parameters
        The number of elements in the vector of inputs must be equal to the number of slots, which is
        equal to the degree of the polynomial modulus. Each entry in the vector of inputs must have
        bit_count() equal to that of the slot modulus and is expected to be already reduced 
        modulo the slot modulus. The returned polynomial will have the same number of coefficients
        as the polynomial modulus, and each coefficient of the same bit_count() as the coefficient
        modulus.

        @par Thread Safety
        This function is thread-safe as long as prepare_slot() has been called for each
        slot with a non-zero entry in the values vector, as otherwise different
        threads might attempt to prepare the same slot simultaneously.

        @param[in] values The vector of values to write into the slots
        @throws std::invalid_argument if the values vector has incorrect size (see Format of Parameters)
        @throws std::invalid_argument if the entries in the values vector have incorrect size (see Format of Parameters)
        @see add_to_slot() for adding numbers to the slots from multiple threads.
        @see set_slot() for setting the slots to specific values from multiple threads.
        */
        BigPoly compose(const std::vector<BigUInt> &values)
        {
            BigPoly destination(poly_modulus_.coeff_count(), slot_modulus_.significant_bit_count());
            compose(values, destination);

            return destination;
        }

        /**
        Reads the values in the slots of a given polynomial and writes them as the entries of 
        a given vector. This is the inverse of what compose() does. This function runs on a single 
        thread, and for improved performance the user might instead want to call the function get_slot()
        from multiple threads.

        @par Format of Parameters
        The polynomial poly must have the same number of coefficients as the polynomial modulus,
        and its coefficients must have bit_count() equal to that of the slot modulus. Moreover, poly is
        expected to be already reduced modulo the polynomial modulus, and its coefficients are expected
        to be already reduced modulo the slot modulus. The destination vector will be automatically
        resized and reset to have appropriate size and form, i.e. the number of elements will be equal
        to the number of slots, and the elements themselves will all have bit_count() equal to that 
        of the slot modulus and be reduced modulo the slot modulus.

        @param[in] poly The polynomial from which the slots will be read
        @param[out] destination The vector to be overwritten with the values of the slots
        @throws std::invalid_argument if poly has incorrect size (see Format of Parameters)
        @see get_slot() for reading from the slots from multiple threads.
        */
        void decompose(const BigPoly &poly, std::vector<BigUInt> &destination);

        /**
        Reads the values in the slots of a given polynomial and writes them as the entries of
        a vector which is returned. This is the inverse of what compose() does. This function runs on 
        a single thread, and for improved performance the user might instead want to call the function 
        get_slot() from multiple threads.

        @par Format of Parameters
        The polynomial poly must have the same number of coefficients as the polynomial modulus,
        and its coefficients must have bit_count() equal to that of the slot modulus. Moreover, poly is
        expected to be already reduced modulo the polynomial modulus, and its coefficients are expected
        to be already reduced modulo the slot modulus. The number of elements in the returned
        vector will be equal to the number of slots, and the elements themselves will all have 
        bit_count() equal to that of the slot modulus and be reduced modulo the slot modulus.

        @param[in] poly The polynomial from which the slots will be read
        @throws std::invalid_argument if poly has incorrect size (see Format of Parameters)
        @see get_slot() for reading from the slots from multiple threads.
        */
        std::vector<BigUInt> decompose(const BigPoly &poly)
        {
            std::vector<BigUInt> destination;
            decompose(poly, destination);

            return destination;
        }

        /**
        Reads the value from a given slot of a given polynomial and writes it to a BigUInt
        given by reference.

        This function is particularly useful when the user wants to use multithreading to speed up
        reading values from slots of a polynomial.
        
        @par Format of Parameters
        The polynomial poly must have the same number of coefficients as the polynomial modulus,
        and its coefficients must have bit_count() equal to that of the slot modulus. Moreover, poly is
        expected to be already reduced modulo the polynomial modulus, and its coefficients are expected
        to be already reduced modulo the slot modulus. The destination BigUInt will automatically
        be resized to have the appropriate size, i.e. it will have bit_count() equal to that of
        the slot modulus, and its value will be reduced modulo the slot modulus.

        @par Thread Safety
        This function is thread-safe as long as other threads are not mutating poly and destination
        while get_slot() is accessing them, as mutating a BigPoly or a BigUInt is not thread-safe.

        @param[in] poly The polynomial from which the slot is read
        @param[in] index The index of the slot to be read
        @param[out] destination The BigUInt to be overwritten with the content of the slot
        @throws std::invalid_argument if index is larger than or equal to the number of slots
        @throws std::invalid_argument if poly has incorrect size (see Format of Parameters)
        @see decompose() for reading the values of all slots at once from a single thread.
        */
        void get_slot(const BigPoly &poly, std::size_t index, BigUInt &destination);

        /**
        Reads the value from a given slot of a given polynomial and returns it as a BigUInt.

        This function is particularly useful when the user wants to use multithreading to speed up
        reading values from slots of a polynomial.

        @par Format of Parameters
        The polynomial poly must have the same number of coefficients as the polynomial modulus,
        and its coefficients must have bit_count() equal to that of the slot modulus. Moreover, poly is
        expected to be already reduced modulo the polynomial modulus, and its coefficients are expected
        to be already reduced modulo the slot modulus. The returned BigUInt will have
        bit_count() equal to that of the slot modulus, and its value will be reduced modulo
        the slot modulus.

        @par Thread Safety
        This function is thread-safe as long as other threads are not mutating poly while get_slot() 
        is accessing it, as mutating a BigPoly is not thread-safe.

        @param[in] poly The polynomial from which the slot is read
        @param[in] index The index of the slot to be read
        @throws std::invalid_argument if index is larger than or equal to the number of slots
        @throws std::invalid_argument if the poly has incorrect size (see Format of Parameters)
        @see decompose() for reading the values of all slots at once from a single thread.
        */
        BigUInt get_slot(const BigPoly &poly, std::size_t index)
        {
            BigUInt destination(slot_modulus_.uint64_count() * util::bits_per_uint64);
            get_slot(poly, index, destination);

            return destination;
        }

        /**
        Adds a value (modulo the slot modulus) to the current value in a given slot of a polynomial. 

        This function is particularly useful when the user wants to use multithreading to speed up
        writing values to the slots. It is faster than set_slot() and achieves the exact same thing
        then the polynomial is guaranteed to have a value of zero in the particular slot that is being
        written to. If the slot to be added to has not already been prepared using prepare_slot(), 
        that will be done first. 

        @par Format of Parameters
        The value BigUInt must have have bit_count() equal to that of the slot modulus, and its value
        is expected to be reduced modulo the slot modulus.
        The polynomial destination must have the same number of coefficients as the polynomial modulus,
        and its coefficients must have bit_count() equal to that of the slot modulus. Moreover, destination is
        expected to be already reduced modulo the polynomial modulus, and its coefficients are expected
        to be already reduced modulo the slot modulus.

        @par Thread Safety
        This function is thread-safe as long as other threads are not mutating value or destination
        while add_to_slot() is accessing them, as mutating a BigUInt or a BigPoly is not thread-safe.

        @param[in] value The value to be added to the slot
        @param[in] index The index of the slot
        @param[out] destination The BigPoly whose slot the value is added to
        @throws std::invalid_argument if index is larger than or equal to the number of slots
        @throws std::invalid_argument if value has incorrect size (see Format of Parameters)
        @throws std::invalid_argument if destination has incorrect size (see Format of Parameters)
        @see set_slot() for setting the value of a slot.
        @see compose() for setting the values of all slots.
        @see prepare_slot() for preparing the slot for writing.
        */
        void add_to_slot(const BigUInt &value, std::size_t index, BigPoly &destination);

        /**
        Sets the value in a given slot of a polynomial to a given value.
        
        This function is particularly useful when the user wants to use multithreading to speed up
        writing values to the slots. It is slower than add_to_slot(), but can be used even if the slot
        already contains some non-zero value. If the slot to be added to has not already been prepared 
        using prepare_slot(), that will be done first. 

        @par Format of Parameters
        The value BigUInt must have have bit_count() equal to that of the slot modulus, and its value
        is expected to be reduced modulo the slot modulus.
        The polynomial destination must have the same number of coefficients as the polynomial modulus,
        and its coefficients must have bit_count() equal to that of the slot modulus. Moreover, destination is
        expected to be already reduced modulo the polynomial modulus, and its coefficients are expected
        to be already reduced modulo the slot modulus.

        @par Thread Safety
        This function is thread-safe as long as other threads are not mutating value or destination
        while set_slot() is accessing them, as mutating a BigUInt or a BigPoly is not thread-safe.

        @param[in] value The value the slot is set to
        @param[in] index The index of the slot
        @param[out] destination The BigPoly whose slot is set to value
        @throws std::invalid_argument if index is larger than or equal to the number of slots
        @throws std::invalid_argument if value has incorrect size (see Format of Parameters)
        @throws std::invalid_argument if destination has incorrect size (see Format of Parameters)
        @see add_to_slot() for adding to the value of a slot.
        @see compose() for setting the values of all slots.
        @see prepare_slot() for preparing the slot for writing.
        */
        void set_slot(const BigUInt &value, std::size_t index, BigPoly &destination);

        /**
        Returns the number of slots.
        */
        std::size_t get_slot_count()
        {
            return slots_;
        }

        /**
        Returns a polynomial such that multiplication with it results in a polynomial
        whose constant coefficient is the sum of certain slots chosen by the user.
        If the slots to be added have not already been prepared using prepare_slot(), 
        that will be done first. 

        @param[in] indices The set of indices of the slots to be summed
        @throws std::invalid_argument if any of the indices is larger than or equal to the number of slots
        @throws std::logic_error if an inversion modulo the slot modulus fails
        */
        BigPoly get_slot_sum_scaling_poly(std::set<std::size_t> indices);

        /**
        Returns a polynomial such that multiplication with it results in a polynomial
        whose constant coefficient is the sum of all slots. This function will call
        prepare_all_slots().

        @throws std::logic_error if an inversion modulo the slot modulus fails
        */
        BigPoly get_slot_sum_scaling_poly();

    private:
        bool is_generator(const std::uint64_t *root);

        void set_random(std::uint64_t *destination);

        void find_minimal_generator(const std::uint64_t *generator, std::uint64_t *destination);

        void find_generator(std::uint64_t *destination);

        void populate_roots_of_unity_vector(const uint64_t *generator);

        inline bool is_slot_prepared(std::size_t index)
        {
            return (crt_base_polys_[index] != nullptr);
        }

        void prepare_slot(std::size_t index, util::MemoryPool &pool);

        void get_slot(const std::uint64_t *poly, std::size_t index, std::uint64_t *destination, util::MemoryPool &pool);

        void add_to_slot(const std::uint64_t *value, std::size_t index, std::uint64_t *destination, util::MemoryPool &pool);

        void set_slot(const std::uint64_t *value, std::size_t index, std::uint64_t *destination, util::MemoryPool &pool);
        
        PolyCRTBuilder(const PolyCRTBuilder &copy) = delete;

        PolyCRTBuilder &operator =(const PolyCRTBuilder &assign) = delete;

        util::Modulus slot_modulus_;

        util::PolyModulus poly_modulus_;

        std::size_t slots_;

        util::MemoryPool pool_;

        std::vector<util::ConstPointer> roots_of_unity_;

        std::vector<std::uint64_t*> crt_base_polys_;
    };
}

#endif // SEAL_POLYCRT_H