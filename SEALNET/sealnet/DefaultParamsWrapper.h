#pragma once

#include "seal/defaultparams.h"
#include "sealnet/SmallModulusWrapper.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            public ref class DefaultParams abstract sealed
            {
            public:
                /**
                <summary>Returns the default coefficients modulus for a given polynomial modulus 
                degree.</summary>
                
                <remarks>
                Returns the default coefficients modulus for a given polynomial modulus degree.
                The polynomial modulus and the coefficient modulus obtained in this way should
                provide approdimately 128 bits of security against the best known attacks,
                assuming the standard deviation of the noise distribution is left to its default
                value.
                </remarks>
                <param name="polyModulusDegree">The degree of the polynomial modulus</param>
                <exception cref="System::ArgumentOutOfRangeException">if poly_modulus_degree is
                not 1024, 2048, 4096, 8192, 16384, or 32768</exception>
                */
                static System::Collections::Generic::List<SmallModulus^> ^CoeffModulus128(int polyModulusDegree);

                /**
                <summary>Returns the default coefficients modulus for a given polynomial modulus
                degree.</summary>

                <remarks>
                Returns the default coefficients modulus for a given polynomial modulus degree.
                The polynomial modulus and the coefficient modulus obtained in this way should
                provide approdimately 192 bits of security against the best known attacks,
                assuming the standard deviation of the noise distribution is left to its default
                value.
                </remarks>
                <param name="polyModulusDegree">The degree of the polynomial modulus</param>
                <exception cref="System::ArgumentOutOfRangeException">if poly_modulus_degree is
                not 1024, 2048, 4096, 8192, 16384, or 32768</exception>
                */
                static System::Collections::Generic::List<SmallModulus^> ^CoeffModulus192(int polyModulusDegree);

                /**
                <summary>Returns a 60-bit coefficient modulus prime.</summary>

                <param name="index">The list index of the prime</param>
                <exception cref="System::ArgumentOutOfRangeException">if index is not within 
                [0, 64)</exception>
                */
                static SmallModulus ^SmallMods60Bit(int index);
                
                /**
                <summary>Returns a 50-bit coefficient modulus prime.</summary>

                <param name="index">The list index of the prime</param>
                <exception cref="System::ArgumentOutOfRangeException">if index is not within
                [0, 64)</exception>
                */
                static SmallModulus ^SmallMods50Bit(int index);
                
                /**
                <summary>Returns a 40-bit coefficient modulus prime.</summary>

                <param name="index">The list index of the prime</param>
                <exception cref="System::ArgumentOutOfRangeException">if index is not within
                [0, 64)</exception>
                */
                static SmallModulus ^SmallMods40Bit(int index);
                
                /**
                <summary>Returns a 30-bit coefficient modulus prime.</summary>

                <param name="index">The list index of the prime</param>
                <exception cref="System::ArgumentOutOfRangeException">if index is not within
                [0, 64)</exception>
                */
                static SmallModulus ^SmallMods30Bit(int index);

                /**
                </summary>Returns the largest allowed decomposition bit count (60).</summary>
                */
                static property int DBCmax {
                    int get();
                }

                /**
                </summary>Returns the smallest allowed decomposition bit count (1).</summary>
                */
                static property int DBCmin {
                    int get();
                }
            };
        }
    }
}
