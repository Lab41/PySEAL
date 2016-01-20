#pragma once

#include "polycrt.h"


namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            ref class BigPoly;
            ref class BigUInt;

            /**
            <summary>Provides functionality for encrypting several plaintext numbers into one ciphertext for improved memory
            efficiency and efficient vector operations (SIMD).</summary>

            <remarks>
            <para>
            Provides functionality for encrypting several plaintext numbers into one ciphertext for improved memory efficiency and
            efficient vector operations (SIMD). Multiplying and adding such polynomials together performs the respective operation
            on each of the slots independently and simultaneously.
            </para>
            <para>
            Mathematically speaking, if polyModulus is x^n+1 and plainModulus is a prime number t such that 2n divides t-1, then
            integers modulo t contain a primitive 2n-th root of unity and the polynomial x^n+1 splits into n distinct linear
            factors as x^n+1 = (x-a_1)*...*(x-a_n) mod t, where the constants a_1, ..., a_n are all the distinct primitive 2n-th
            roots of unity in integers modulo t. The Chinese Remainder Theorem (CRT) states that the plaintext space Z_t[x]/(x^n+1)
            in this case is isomorphic (as an algebra) to the n-fold direct product of fields Z_t. The isomorphism is easy to
            compute explicitly in both directions, which is what an instance of this class can be used for. We refer to the
            modulus t as the slot modulus. Typically when an instance of PolyCRTBuilder is created, the slot modulus is set to be
            the plaintext modulus.
            </para>
            <para>
            The PolyCRTBuilder class is not thread-safe and a separate PolyCRTBuilder instance is needed for each potentially
            concurrent usage. However, certain important functions of PolyCRTBuilder are safe to call from several threads
            concurrently as long as the threads are guaranteed to never access the same slot simultaneously, or mutate 
            a <see cref="BigUInt"/> or <see cref="BigPoly"/> while another thread is using it, as these objects are not thread-safe.
            </para>
            </remarks>
            */
            public ref class PolyCRTBuilder
            {
            public:
                /**
                <summary>Creates a PolyCRTBuilder instance with given slot modulus and given polynomial modulus.</summary>

                <remarks>
                Creates a PolyCRTBuilder instance with given slot modulus and given polynomial modulus. 
                It is necessary that the polynomial modulus is of the form x^n+1, where n is a power of 2, 
                and that the slot modulus is 1 modulo 2n.
                </remarks>
                <param name="slotModulus">The slot modulus</param>
                <param name="polyModulus">The polynomial modulus</param>
                <exception cref="System::ArgumentNullException">if slotModulus or polyModulus is null</exception>
                <exception cref="System::ArgumentException">if slotModulus is zero</exception>
                <exception cref="System::ArgumentException">if polyModulus is not of the form x^n+1 where n is a power of 2</exception>
                <exception cref="System::ArgumentException">if 2*degree(polyModulus) does not divide slotModulus-1</exception>
                */
                PolyCRTBuilder(BigUInt ^slotModulus, BigPoly ^polyModulus);

                /**
                <summary>Destroys the PolyCRTBuilder</summary>
                */
                ~PolyCRTBuilder();

                /**
                <summary>Destroys the PolyCRTBuilder</summary>
                */
                !PolyCRTBuilder();

                /**
                <summary>Performs a precomputation needed for writing to a particular slot given as argument.</summary>

                <remarks>
                <para>
                Performs a precomputation needed for writing to a particular slot given as argument.
                </para>
                <para>
                This function is not thread-safe, but it should be safe to call from different threads as long as they are guaranteed
                not to attempt to prepare the same slot simultaneously.
                </para>
                </remarks>
                <param name="index">The index of the slot to prepare</param>
                <exception cref="System::ArgumentException">if index is negative</exception>
                <exception cref="System::ArgumentException">if index is larger than or equal to the number of slots</exception>
                <exception cref="System::InvalidOperationException">if an inversion modulo the slot modulus fails</exception>
                <seealso>See PrepareAllSlots() for preparing all slots at once from a single thread.</seealso>
                */
                void PrepareSlot(int index);

                /**
                <summary>Calls PrepareSlot() for every index on a single thread in a way that avoids repeated memory allocation and
                deallocation.</summary>

                <remarks>
                Calls PrepareSlot() for every index on a single thread in a way that avoids repeated memory allocation and
                deallocation. This is slightly more efficient than calling PrepareSlot() repeatedly from a single thread, but for
                improved performance the user might want to call PrepareSlot() from multiple threads.
                </remarks>
                <exception cref="System::InvalidOperationException">if an inversion modulo the slot modulus fails</exception>
                */
                void PrepareAllSlots();

                /**
                <summary>Writes a given list of values (represented by BigUInt) into the slots of a given polynomial.</summary>

                <remarks>
                <para>
                Writes a given list of values (represented by <see cref="BigUInt"/>) into the slots of a given polynomial. If the list contains
                non-zero entries for which the corresponding slots have not already been prepared using PrepareSlot(), they will be
                prepared before the value is set. This function runs on a single thread, and for improved performance the user might
                instead want to call the functions AddToSlot(), or possibly SetSlot(), from multiple threads.
                </para>
                <para>
                The number of elements in the list of inputs must be equal to the number of slots, which is equal to the degree of
                the polynomial modulus. Each entry in the list of inputs must have BitCount equal to that of the slot modulus
                and is expected to be already reduced modulo the slot modulus. The destination polynomial will automatically be
                resized to have correct size, i.e. the same number of coefficients as the polynomial modulus, and each
                coefficient of the same BitCount as the slot modulus.
                </para>
                <para>
                This function is thread-safe as long as PrepareSlot() has been called for each
                slot with a non-zero entry in the values vector, and if each thread writes to 
                a different destination polynomial.
                </para>
                </remarks>
                <param name="values">The list of values to write into the slots</param>
                <param name="destination">The polynomial to overwrite with the result</param>
                <exception cref="System::ArgumentNullException">if values or any of its entries is null</exception>
                <exception cref="System::ArgumentNullException">if destination is null</exception>
                <exception cref="System::ArgumentException">if the values list has incorrect size</exception>
                <exception cref="System::ArgumentException">if the entries in the values list have incorrect size</exception>
                <seealso>See AddToSlot() for adding numbers to the slots from multiple threads.</seealso>
                <seealso>See SetSlot() for setting the slots to specific values from multiple threads.</seealso>
                */
                void Compose(System::Collections::Generic::List<BigUInt^> ^values, BigPoly ^destination);

                /**
                <summary>Writes a given list of values (represented by BigUInt) into the slots of a polynomial and returns
                it.</summary>

                <remarks>
                <para>
                Writes a given list of values (represented by <see cref="BigUInt"/>) into the slots of a polynomial and returns it. If the list
                contains non-zero entries for which the corresponding slots have not already been prepared using PrepareSlot(), they
                will be prepared before the value is set. This function runs on a single thread, and for improved performance the user
                might instead want to call the functions AddToSlot(), or possibly SetSlot(), from multiple threads.
                </para>
                <para>
                The number of elements in the list of inputs must be equal to the number of slots, which is equal to the degree of
                the polynomial modulus. Each entry in the list of inputs must have BitCount equal to that of the slot modulus
                and is expected to be already reduced modulo the slot modulus. The returned polynomial will have the same number of
                coefficients as the polynomial modulus, and each coefficient of the same BitCount as the coefficient modulus.
                </para>
                <para>
                This function is thread-safe as long as PrepareSlot() has been called for each
                slot with a non-zero entry in the values vector, as otherwise different
                threads might attempt to prepare the same slot simultaneously.
                </para>
                </remarks>
                <param name="values">The list of values to write into the slots</param>
                <exception cref="System::ArgumentNullException">if values or any of its entries is null</exception>
                <exception cref="System::ArgumentException">if the values list has incorrect size</exception>
                <exception cref="System::ArgumentException">if the entries in the values list have incorrect size</exception>
                <seealso>See AddToSlot() for adding numbers to the slots from multiple threads.</seealso>
                <seealso>See SetSlot() for setting the slots to specific values from multiple threads.</seealso>
                */
                BigPoly ^Compose(System::Collections::Generic::List<BigUInt^> ^values);

                /**
                <summary>Reads the values in the slots of a given polynomial and writes them as the entries of a given list.</summary>

                <remarks>
                <para>
                Reads the values in the slots of a given polynomial and writes them as the entries of a given list. This is the
                inverse of what Compose() does. This function runs on a single thread, and for improved performance the user might
                instead want to call the function GetSlot() from multiple threads.
                </para>
                <para>
                The polynomial poly must have the same number of coefficients as the polynomial modulus, and its coefficients must have
                BitCount equal to that of the slot modulus. Moreover, poly is expected to be already reduced modulo the
                polynomial modulus, and its coefficients are expected to be already reduced modulo the slot modulus. The destination
                list will be automatically resized and reset to have appropriate size and form, i.e. the number of elements will be
                equal to the number of slots, and the elements themselves will all have BitCount equal to that of the slot modulus
                and be reduced modulo the slot modulus.
                </para>
                </remarks>
                <param name="poly">The polynomial from which the slots will be read</param>
                <param name="destination">The list to be overwritten with the values of the slots</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                <exception cref="System::ArgumentNullException">if destination or any of its entries is null</exception>
                <exception cref="System::ArgumentException">if poly has incorrect size</exception>
                <seealso>See GetSlot() for reading from the slots from multiple threads.</seealso>
                */
                void Decompose(BigPoly ^poly, System::Collections::Generic::List<BigUInt^> ^destination);

                /**
                <summary>Reads the values in the slots of a given polynomial and writes them as the entries of a list which is
                returned.</summary>

                <remarks>
                <para>
                Reads the values in the slots of a given polynomial and writes them as the entries of a list which is returned. This
                is the inverse of what Compose() does. This function runs on a single thread, and for improved performance the user
                might instead want to call the function GetSlot() from multiple threads.
                </para>
                <para>
                The polynomial poly must have the same number of coefficients as the polynomial modulus, and its coefficients must have
                BitCount equal to that of the slot modulus. Moreover, poly is expected to be already reduced modulo the
                polynomial modulus, and its coefficients are expected to be already reduced modulo the slot modulus. The number of
                elements in the returned list will be equal to the number of slots, and the elements themselves will all have
                BitCount equal to that of the slot modulus and be reduced modulo the slot modulus.
                </para>
                </remarks>
                <param name="poly">The polynomial from which the slots will be read</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                <exception cref="System::ArgumentException">if poly has incorrect size</exception>
                <seealso>See GetSlot() for reading from the slots from multiple threads.</seealso>
                */
                System::Collections::Generic::List<BigUInt^> ^Decompose(BigPoly ^poly);

                /**
                <summary>Reads the value from a given slot of a given polynomial and writes it to a BigUInt given by
                reference.</summary>

                <remarks>
                <para>
                Reads the value from a given slot of a given polynomial and writes it to a <see cref="BigUInt"/> given by reference.
                </para>
                <para>
                This function is particularly useful when the user wants to use multithreading to speed up reading values from slots of
                a polynomial.
                </para>
                <para>
                The polynomial poly must have the same number of coefficients as the polynomial modulus, and its coefficients must have
                BitCount equal to that of the slot modulus. Moreover, poly is expected to be already reduced modulo the
                polynomial modulus, and its coefficients are expected to be already reduced modulo the slot modulus. The destination
                <see cref="BigUInt"/> will automatically be resized to have the appropriate size, i.e. it will have BitCount equal to that of
                the slot modulus, and its value will be reduced modulo the slot modulus.
                </para>
                <para>
                This function is thread-safe as long as other threads are not mutating poly and destination while GetSlot() is
                accessing them, as mutating a <see cref="BigPoly"/> or a <see cref="BigUInt"/> is not thread-safe.
                </para>
                </remarks>
                <param name="poly">The polynomial from which the slot is read</param>
                <param name="index">The index of the slot to be read</param>
                <param name="destination">The <see cref="BigUInt"/> to be overwritten with the content of the slot</param>
                <exception cref="System::ArgumentNullException">if poly or destination is null</exception>
                <exception cref="System::ArgumentException">if index is negative</exception>
                <exception cref="System::ArgumentException">if index is larger than or equal to the number of slots</exception>
                <exception cref="System::ArgumentException">if poly has incorrect size</exception>
                <seealso>See Decompose() for reading the values of all slots at once from a single thread.</seealso>
                */
                void GetSlot(BigPoly ^poly, int index, BigUInt ^destination);

                /**
                <summary>Reads the value from a given slot of a given polynomial and returns it as a BigUInt.</summary>

                <remarks>
                <para>
                Reads the value from a given slot of a given polynomial and returns it as a <see cref="BigUInt"/>.
                </para>
                <para>
                This function is particularly useful when the user wants to use multithreading to speed up reading values from slots of
                a polynomial.
                </para>
                <para>
                The polynomial poly must have the same number of coefficients as the polynomial modulus, and its coefficients must
                have BitCount equal to that of the slot modulus. Moreover, poly is expected to be already reduced modulo the
                polynomial modulus, and its coefficients are expected to be already reduced modulo the slot modulus. The returned
                BigUInt will have BitCount equal to that of the slot modulus, and its value will be reduced modulo the slot
                modulus.
                </para>
                <para>
                This function is thread-safe as long as other threads are not mutating poly while GetSlot() is accessing it, as
                mutating a <see cref="BigPoly"/> is not thread-safe.
                </para>
                </remarks>
                <param name="poly">The polynomial from which the slot is read</param>
                <param name="index">The index of the slot to be read</param>
                <exception cref="System::ArgumentNullException">if poly is null</exception>
                <exception cref="System::ArgumentException">if index is negative</exception>
                <exception cref="System::ArgumentException">if index is larger than or equal to the number of slots</exception>
                <exception cref="System::ArgumentException">if the poly has incorrect size</exception>
                <seealso>See Decompose() for reading the values of all slots at once from a single thread.</seealso>
                */
                BigUInt ^GetSlot(BigPoly ^poly, int index);

                /**
                <summary>Adds a value (modulo the slot modulus) to the current value in a given slot of a polynomial.</summary>

                <remarks>
                <para>
                Adds a value (modulo the slot modulus) to the current value in a given slot of a polynomial.
                </para>
                <para>
                This function is particularly useful when the user wants to use multithreading to speed up writing values to the slots.
                It is faster than SetSlot() and achieves the exact same thing then the polynomial is guaranteed to have a value of zero
                in the particular slot that is being written to. If the slot to be added to has not already been prepared using
                PrepareSlot(), that will be done first.
                </para>
                <para>
                The value <see cref="BigUInt"/> must have have BitCount equal to that of the slot modulus, and its value is expected to be
                reduced modulo the slot modulus. The polynomial destination must have the same number of coefficients as the
                polynomial modulus, and its coefficients must have BitCount equal to that of the slot modulus. Moreover, destination
                is expected to be already reduced modulo the polynomial modulus, and its coefficients are expected to be already
                reduced modulo the slot modulus.
                </para>
                <para>
                This function is thread-safe as long as other threads are not mutating value or destination while AddToSlot() is
                accessing them, as mutating a <see cref="BigUInt"/> or a <see cref="BigPoly"/> is not thread-safe.
                </para>
                </remarks>
                <param name="value">The value to be added to the slot</param>
                <param name="index">The index of the slot</param>
                <param name="destination">The <see cref="BigPoly"/> whose slot the value is added to</param>
                <exception cref="System::ArgumentNullException">if value or destination is null</exception>
                <exception cref="System::ArgumentException">if index is negative</exception>
                <exception cref="System::ArgumentException">if index is larger than or equal to the number of slots</exception>
                <exception cref="System::ArgumentException">if value has incorrect size</exception>
                <exception cref="System::ArgumentException">if destination has incorrect size</exception>
                <seealso cref="SetSlot()">See SetSlot() for setting the value of a slot.</seealso>
                <seealso>See Compose() for setting the values of all slots.</seealso>
                <seealso>See PrepareSlot() for preparing the slot for writing.</seealso>
                */
                void AddToSlot(BigUInt ^value, int index, BigPoly ^destination);

                /**
                <summary>Sets the value in a given slot of a polynomial to a given value.</summary>

                <remarks>
                <para>
                Sets the value in a given slot of a polynomial to a given value.
                </para>
                <para>
                This function is particularly useful when the user wants to use multithreading to speed up writing values to the slots.
                It is slower than AddToSlot() and achieves the exact same thing then the polynomial is guaranteed to have a value of zero
                in the particular slot that is being written to. If the slot to be added to has not already been prepared using
                PrepareSlot(), that will be done first.
                </para>
                <para>
                The value <see cref="BigUInt"/> must have have BitCount equal to that of the slot modulus, and its value is expected to be
                reduced modulo the slot modulus. The polynomial destination must have the same number of coefficients as the
                polynomial modulus, and its coefficients must have BitCount equal to that of the slot modulus. Moreover, destination
                is expected to be already reduced modulo the polynomial modulus, and its coefficients are expected to be already
                reduced modulo the slot modulus.
                </para>
                <para>
                This function is thread-safe as long as other threads are not mutating value or destination while SetSlot() is
                accessing them, as mutating a <see cref="BigUInt"/> or a <see cref="BigPoly"/> is not thread-safe.
                </para>
                </remarks>
                <param name="value">The value the slot is set to</param>
                <param name="index">The index of the slot</param>
                <param name="destination">The <see cref="BigPoly"/> whose slot is set to value</param>
                <exception cref="System::ArgumentNullException">if value or destination is null</exception>
                <exception cref="System::ArgumentException">if index is negative</exception>
                <exception cref="System::ArgumentException">if index is larger than or equal to the number of slots</exception>
                <exception cref="System::ArgumentException">if value has incorrect size</exception>
                <exception cref="System::ArgumentException">if destination has incorrect size</exception>
                <seealso>See AddToSlot() for adding to the value of a slot.</seealso>
                <seealso>See Compose() for setting the values of all slots.</seealso>
                <seealso>See PrepareSlot() for preparing the slot for writing.</seealso>
                */
                void SetSlot(BigUInt ^value, int index, BigPoly ^destination);

                /**
                <summary>Returns the number of slots.</summary>
                */
                property int SlotCount
                {
                    int get();
                }

                /**
                <summary>Returns a polynomial such that multiplication with it results in a polynomial whose constant coefficient is
                the sum of certain slots chosen by the user.</summary>

                <remarks>
                Returns a polynomial such that multiplication with it results in a polynomial whose constant coefficient is the sum of
                certain slots chosen by the user. If the slots to be added have not already been prepared using PrepareSlot(), that
                will be done first.
                </remarks>
                <param name="indices">The set of indices of the slots to be summed</param>
                <exception cref="System::ArgumentNullException">if indices is null</exception>
                <exception cref="System::ArgumentException">if any of the indices is negative</exception>
                <exception cref="System::ArgumentException">if any of the indices is larger than or equal to the number of
                slots</exception>
                <exception cref="System::InvalidOperationException">if an inversion modulo the slot modulus fails</exception>
                */
                BigPoly ^GetSlotSumScalingPoly(System::Collections::Generic::SortedSet<int> ^indices);

                /**
                <summary>Returns a polynomial such that multiplication with it results in a polynomial whose constant coefficient is
                the sum of all slots.</summary>

                <remarks>
                Returns a polynomial such that multiplication with it results in a polynomial whose constant coefficient is the sum of
                all slots. This function will call <see cref="PrepareAllSlots()"/>.
                </remarks>
                <exception cref="System::InvalidOperationException">if an inversion modulo the slot modulus fails</exception>
                */
                BigPoly ^GetSlotSumScalingPoly();

                /**
                <summary>Returns a reference to the underlying C++ PolyCRTBuilder.</summary>
                */
                seal::PolyCRTBuilder &GetPolyCRTBuilder();

            private:
                seal::PolyCRTBuilder *polyCRTBuilder_;
            };
        }
    }
}
