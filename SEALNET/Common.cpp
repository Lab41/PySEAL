#include "util/common.h"
#include <msclr\marshal.h>
#include <stdexcept>
#include <cstdint>
#include <iostream>

using namespace System;
using namespace System::IO;
using namespace msclr::interop;
using namespace std;
using namespace seal::util;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            void Write(Stream ^to, const char *pointer, int byte_count)
            {
                for (int i = 0; i < byte_count; ++i)
                {
                    to->WriteByte(*pointer);
                    pointer++;
                }
            }

            void Read(Stream ^from, char *pointer, int byte_count)
            {
                for (int i = 0; i < byte_count; ++i)
                {
                    int value = from->ReadByte();
                    if (value == -1)
                    {
                        throw gcnew InvalidOperationException("Unexpected end of stream");
                    }
                    *pointer = value;
                    pointer++;
                }
            }

            void HandleException(const exception *e)
            {
                if (e == nullptr)
                {
                    throw gcnew Exception("Unknown SEAL library exception");
                }
                String ^message = e->what() != nullptr ? marshal_as<String^>(e->what()) : "SEAL library exception";
                if (dynamic_cast<const invalid_argument*>(e) != nullptr)
                {
                    throw gcnew ArgumentException(message);
                }
                if (dynamic_cast<const out_of_range*>(e) != nullptr)
                {
                    throw gcnew ArgumentOutOfRangeException(message);
                }
                if (dynamic_cast<const bad_alloc*>(e) != nullptr)
                {
                    throw gcnew OutOfMemoryException(message);
                }
                if (dynamic_cast<const logic_error*>(e) != nullptr)
                {
                    throw gcnew InvalidOperationException(message);
                }
                throw gcnew Exception(message);
            }

            int ComputeArrayHashCode(uint64_t *pointer, int uint64_count)
            {
                const uint64_t hash_seed = 17;
                const uint64_t hash_multiply = 23;
                int hash = hash_seed;
                for (int i = 0; i < uint64_count; ++i)
                {
                    uint64_t value = *pointer++;
                    if (value != 0)
                    {
                        hash *= hash_multiply;
                        hash += static_cast<uint32_t>(value);
                        value >>= 32;
                        hash *= hash_multiply;
                        hash += static_cast<uint32_t>(value);
                    }
                }
                return hash;
            }
        }
    }
}