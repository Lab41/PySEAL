#include "Common.h"
#include "sealnet/BigPolyArrayWrapper.h"
#include "sealnet/BigPolyWrapper.h"
#include "seal/util/uintcore.h"
#include "seal/util/common.h"

using namespace System;
using namespace System::IO;
using namespace std;
using namespace seal::util;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            BigPolyArray::BigPolyArray() : array_(nullptr), owned_(true)
            {
                try
                {
                    array_ = new seal::BigPolyArray();
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

            BigPolyArray::BigPolyArray(int size, int coeffCount, int coeffBitCount) : array_(nullptr), owned_(true)
            {
                try
                {
                    array_ = new seal::BigPolyArray(size, coeffCount, coeffBitCount);
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

            BigPolyArray::BigPolyArray(BigPolyArray ^copy) : array_(nullptr), owned_(true)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    array_ = new seal::BigPolyArray(copy->GetArray());
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

            int BigPolyArray::Size::get()
            {
                if (array_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArray is disposed");
                }
                return array_->size();
            }

            int BigPolyArray::CoeffCount::get()
            {
                if (array_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArray is disposed");
                }
                return array_->coeff_count();
            }

            int BigPolyArray::CoeffBitCount::get()
            {
                if (array_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArray is disposed");
                }
                return array_->coeff_bit_count();
            }

            int BigPolyArray::CoeffUInt64Count::get()
            {
                if (array_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArray is disposed");
                }
                return array_->coeff_uint64_count();
            }

            int BigPolyArray::PolyUInt64Count()
            {
                if (array_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArray is disposed");
                }
                return array_->poly_uint64_count();
            }

            int BigPolyArray::UInt64Count()
            {
                if (array_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArray is disposed");
                }
                return array_->uint64_count();
            }

            BigPoly ^BigPolyArray::default::get(int polyIndex)
            {
                if (array_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArray is disposed");
                }
                try
                {
                    return gcnew BigPoly(array_->coeff_count(), array_->coeff_bit_count(), array_->pointer(polyIndex));
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

            bool BigPolyArray::Equals(BigPolyArray ^compare)
            {
                if (array_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArray is disposed");
                }
                if (compare == nullptr)
                {
                    return false;
                }
                auto result = (*array_ == compare->GetArray());
                GC::KeepAlive(compare);
                return result;
            }

            bool BigPolyArray::Equals(Object ^compare)
            {
                if (array_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArray is disposed");
                }
                BigPolyArray ^comparearr = dynamic_cast<BigPolyArray^>(compare);
                if (comparearr != nullptr)
                {
                    return Equals(comparearr);
                }
                return false;
            }

            bool BigPolyArray::IsZero::get()
            {
                if (array_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArray is disposed");
                }
                return array_->is_zero();
            }

            void BigPolyArray::SetZero()
            {
                if (array_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArray is disposed");
                }
                array_->set_zero();
            }

            void BigPolyArray::SetZero(int polyIndex)
            {
                if (array_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArray is disposed");
                }
                try
                {
                    array_->set_zero(polyIndex);
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

            void BigPolyArray::Resize(int size, int coeffCount, int coeffBitCount)
            {
                if (array_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArray is disposed");
                }
                try
                {
                    array_->resize(size, coeffCount, coeffBitCount);
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

            void BigPolyArray::Reset()
            {
                if (array_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArray is disposed");
                }
                array_->reset();
            }

            void BigPolyArray::Set(BigPolyArray ^assign)
            {
                if (array_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArray is disposed");
                }
                if (assign == nullptr)
                {
                    throw gcnew ArgumentNullException("assign cannot be null");
                }
                try
                {
                    *array_ = assign->GetArray();
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

            void BigPolyArray::Save(Stream ^stream)
            {
                if (array_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArray is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                try
                {
                    int32_t size32 = static_cast<int32_t>(array_->size());
                    int32_t coeff_count32 = static_cast<int32_t>(array_->coeff_count());
                    int32_t coeff_bit_count32 = static_cast<int32_t>(array_->coeff_bit_count());
                    Write(stream, reinterpret_cast<const char*>(&size32), sizeof(int32_t));
                    Write(stream, reinterpret_cast<const char*>(&coeff_count32), sizeof(int32_t));
                    Write(stream, reinterpret_cast<const char*>(&coeff_bit_count32), sizeof(int32_t));
                    Write(stream, reinterpret_cast<const char*>(array_->pointer(0)), array_->size() * array_->coeff_count() 
                        * array_->coeff_uint64_count() * bytes_per_uint64);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (Exception^)
                {
                    throw;
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            void BigPolyArray::Load(Stream ^stream)
            {
                if (array_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArray is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                try
                {
                    int32_t size32 = 0;
                    int32_t coeff_count32 = 0;
                    int32_t coeff_bit_count32 = 0;
                    Read(stream, reinterpret_cast<char*>(&size32), sizeof(int32_t));
                    Read(stream, reinterpret_cast<char*>(&coeff_count32), sizeof(int32_t));
                    Read(stream, reinterpret_cast<char*>(&coeff_bit_count32), sizeof(int32_t));
                    array_->resize(size32, coeff_count32, coeff_bit_count32);
                    Read(stream, reinterpret_cast<char*>(array_->pointer(0)), array_->size() * array_->coeff_count() * array_->coeff_uint64_count() * bytes_per_uint64);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (Exception^)
                {
                    throw;
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            int BigPolyArray::GetHashCode()
            {
                if (array_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArray is disposed");
                }
                return ComputeArrayHashCode(array_->pointer(0), array_->uint64_count());
            }

            seal::BigPolyArray &BigPolyArray::GetArray()
            {
                if (array_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPolyArray is disposed");
                }
                return *array_;
            }

            BigPolyArray::~BigPolyArray()
            {
                this->!BigPolyArray();
            }

            BigPolyArray::!BigPolyArray()
            {
                if (array_ != nullptr)
                {
                    if(owned_) 
                    {
                        delete array_;
                        array_ = nullptr;
                    }
                }
            }

            BigPolyArray::BigPolyArray(const seal::BigPolyArray &array) : array_(nullptr), owned_(true)
            {
                try
                {
                    array_ = new seal::BigPolyArray(array);
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

            BigPolyArray::BigPolyArray(seal::BigPolyArray *value) : array_(value), owned_(false)
            {
            }
        }
    }
}