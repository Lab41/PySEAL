#include "sealnet/DefaultParamsWrapper.h"
#include "sealnet/Common.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace std;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            List<SmallModulus^> ^DefaultParams::CoeffModulus128(int polyModulusDegree)
            {
                try
                {
                    auto result = gcnew List<SmallModulus^>;
                    for (auto mod : seal::coeff_modulus_128(polyModulusDegree))
                    {
                        result->Add(gcnew SmallModulus(mod));
                    }
                    return result;
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                throw gcnew Exception("Unexpected exception");
            }
            List<SmallModulus^> ^DefaultParams::CoeffModulus192(int polyModulusDegree)
            {
                try
                {
                    auto result = gcnew List<SmallModulus^>;
                    for (auto mod : seal::coeff_modulus_192(polyModulusDegree))
                    {
                        result->Add(gcnew SmallModulus(mod));
                    }
                    return result;
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                throw gcnew Exception("Unexpected exception");
            }
            SmallModulus ^DefaultParams::SmallMods60Bit(int index)
            {
                try
                {
                    return gcnew SmallModulus(seal::small_mods_60bit(index));
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                throw gcnew Exception("Unexpected exception");
            }
            SmallModulus ^DefaultParams::SmallMods50Bit(int index)
            {
                try
                {
                    return gcnew SmallModulus(seal::small_mods_50bit(index));
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                throw gcnew Exception("Unexpected exception");
            }
            SmallModulus ^DefaultParams::SmallMods40Bit(int index)
            {
                try
                {
                    return gcnew SmallModulus(seal::small_mods_40bit(index));
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                throw gcnew Exception("Unexpected exception");
            }
            SmallModulus ^DefaultParams::SmallMods30Bit(int index)
            {
                try
                {
                    return gcnew SmallModulus(seal::small_mods_30bit(index));
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                throw gcnew Exception("Unexpected exception");
            }

            int DefaultParams::DBCmax::get()
            {
                return seal::dbc_max();
            }

            int DefaultParams::DBCmin::get()
            {
                return seal::dbc_min();
            }
        }
    }
}