#pragma once

#include <stdexcept>
#include <cstdint>

#include "util/defines.h"

#ifdef ENABLE_INTRIN
#define MOI
#endif

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            void Write(System::IO::Stream ^to, const char *pointer, int byte_count);

            void Read(System::IO::Stream ^from, char *pointer, int byte_count);

            void HandleException(const std::exception *e);

            int ComputeArrayHashCode(std::uint64_t *pointer, int uint64_count);
        }
    }
}
