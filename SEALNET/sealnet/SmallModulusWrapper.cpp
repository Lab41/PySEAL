#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>
#include <stdexcept>
#include <array>
#include "sealnet/SmallModulusWrapper.h"
#include "seal/util/common.h"
#include "sealnet/Common.h"

using namespace std;
using namespace seal::util;
using namespace System;
using namespace System::IO;
using namespace System::Collections::Generic;
using namespace msclr::interop;

namespace seal
{
    /**
    <summary>Enables access to private members of seal::SmallModulus.</summary>
    */
    struct SmallModulus::SmallModulusPrivateHelper
    {
        static seal::SmallModulus create_modulus(uint64_t value, array<uint64_t, 3> const_ratio, int bit_count, int uint64_count)
        {
            return seal::SmallModulus(value, const_ratio, bit_count, uint64_count);
        }
    };
}

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            SmallModulus::SmallModulus()
            {
                try
                {
                    modulus_ = new seal::SmallModulus();
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            SmallModulus::SmallModulus(UInt64 value)
            {
                try
                {
                    modulus_ = new seal::SmallModulus(value);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            SmallModulus::SmallModulus(SmallModulus ^copy)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    modulus_ = new seal::SmallModulus(copy->GetModulus());
                    GC::KeepAlive(copy);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            void SmallModulus::Set(SmallModulus ^assign)
            {
                if (modulus_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SmallModulus is disposed");
                }
                if (assign == nullptr)
                {
                    throw gcnew ArgumentNullException("assign cannot be null");
                }
                try
                {
                    *modulus_ = *assign->modulus_;
                    GC::KeepAlive(assign);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            void SmallModulus::Set(UInt64 assign)
            {
                if (modulus_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SmallModulus is disposed");
                }
                try
                {
                    *modulus_ = assign;
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            int SmallModulus::BitCount::get()
            {
                if (modulus_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SmallModulus is disposed");
                }
                return modulus_->bit_count();
            }

            int SmallModulus::UInt64Count::get()
            {
                if (modulus_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SmallModulus is disposed");
                }
                return modulus_->uint64_count();
            }

            UInt64 SmallModulus::Value::get()
            {
                if (modulus_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SmallModulus is disposed");
                }
                return modulus_->value();
            }

            Tuple<UInt64, UInt64, UInt64> ^SmallModulus::ConstRatio::get()
            {
                if (modulus_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SmallModulus is disposed");
                }
                array<uint64_t, 3> const_ratio = modulus_->const_ratio();
                auto result = gcnew Tuple<UInt64, UInt64, UInt64>(const_ratio[0], const_ratio[1], const_ratio[2]);
                return result;
            }

            bool SmallModulus::IsZero::get()
            {
                if (modulus_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SmallModulus is disposed");
                }
                return modulus_->is_zero();
            }

            void SmallModulus::Save(Stream ^stream)
            {
                if (modulus_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SmallModulus is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                try
                {
                    int32_t bit_count32 = static_cast<int32_t>(modulus_->bit_count());
                    Write(stream, reinterpret_cast<const char*>(&bit_count32), sizeof(int32_t));
                    int32_t uint64_count32 = static_cast<int32_t>(modulus_->uint64_count());
                    Write(stream, reinterpret_cast<const char*>(&uint64_count32), sizeof(int32_t));
                    Write(stream, reinterpret_cast<const char*>(modulus_->pointer()), bytes_per_uint64);
                    array<uint64_t, 3> const_ratio = modulus_->const_ratio();
                    Write(stream, reinterpret_cast<const char*>(const_ratio.data()), sizeof(const_ratio));
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            void SmallModulus::Load(Stream ^stream)
            {
                if (modulus_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SmallModulus is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                try
                {
                    int32_t bit_count32 = 0;
                    Read(stream, reinterpret_cast<char*>(&bit_count32), sizeof(int32_t));
                    int32_t uint64_count32 = 0;
                    Read(stream, reinterpret_cast<char*>(&uint64_count32), sizeof(int32_t));
                    uint64_t value = 0;
                    Read(stream, reinterpret_cast<char*>(&value), bytes_per_uint64);
                    array<uint64_t, 3> const_ratio;
                    Read(stream, reinterpret_cast<char*>(const_ratio.data()), sizeof(const_ratio));
                    *modulus_ = seal::SmallModulus(seal::SmallModulus::SmallModulusPrivateHelper::create_modulus(
                        value, const_ratio, bit_count32, uint64_count32));
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            int SmallModulus::GetHashCode()
            {
                if (modulus_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SmallModulus is disposed");
                }
                return ComputeArrayHashCode(modulus_->pointer(), modulus_->uint64_count());
            }

            bool SmallModulus::Equals(SmallModulus ^compare)
            {
                if (modulus_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SmallModulus is disposed");
                }
                if (compare == nullptr)
                {
                    return false;
                }
                auto result = (*modulus_ == compare->GetModulus());
                GC::KeepAlive(compare);
                return result;
            }

            bool SmallModulus::Equals(Object ^compare)
            {
                if (modulus_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SmallModulus is disposed");
                }
                SmallModulus ^comparepoly = dynamic_cast<SmallModulus^>(compare);
                if (comparepoly != nullptr)
                {
                    return Equals(comparepoly);
                }
                return false;
            }

            bool SmallModulus::Equals(UInt64 compare)
            {
                if (modulus_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SmallModulus is disposed");
                }
                return (*modulus_ == compare);
            }

            SmallModulus::~SmallModulus()
            {
                this->!SmallModulus();
            }

            SmallModulus::!SmallModulus()
            {
                if (modulus_ != nullptr)
                {
                    delete modulus_;
                    modulus_ = nullptr;
                }
            }

            SmallModulus::SmallModulus(const seal::SmallModulus &modulus) : modulus_(nullptr)
            {
                try
                {
                    modulus_ = new seal::SmallModulus(modulus);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            seal::SmallModulus &SmallModulus::GetModulus()
            {
                if (modulus_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SmallModulus is disposed");
                }
                return *modulus_;
            }

            SmallModulus::operator SmallModulus ^(UInt64 value)
            {
                return gcnew SmallModulus(value);
            }
        }
    }
}