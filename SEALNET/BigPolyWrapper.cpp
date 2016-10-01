#include <msclr\marshal_cppstd.h>
#include <stdexcept>
#include <algorithm>
#include "Common.h"
#include "util/common.h"
#include "util/uintcore.h"
#include "util/polycore.h"
#include "BigPolyWrapper.h"
#include "BigUIntWrapper.h"

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
            BigPoly::BigPoly() : bigpoly_(nullptr), owned_(true)
            {
                try
                {
                    bigpoly_ = new seal::BigPoly();
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

            BigPoly::BigPoly(int coeffCount, int coeffBitCount) : bigpoly_(nullptr), owned_(true)
            {
                try
                {
                    bigpoly_ = new seal::BigPoly(coeffCount, coeffBitCount);
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

            BigPoly::BigPoly(String ^hexPoly) : bigpoly_(nullptr), owned_(true)
            {
                if (hexPoly == nullptr)
                {
                    throw gcnew ArgumentNullException("hexPoly cannot be null");
                }
                marshal_context ^context = gcnew marshal_context();
                try
                {
                    bigpoly_ = new seal::BigPoly(context->marshal_as<const char*>(hexPoly));
                    GC::KeepAlive(hexPoly);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                finally
                {
                    delete context;
                }
            }

            BigPoly::BigPoly(int coeffCount, int coeffBitCount, String ^hexPoly) : bigpoly_(nullptr), owned_(true)
            {
                if (hexPoly == nullptr)
                {
                    throw gcnew ArgumentNullException("hexPoly cannot be null");
                }
                marshal_context ^context = gcnew marshal_context();
                try
                {
                    bigpoly_ = new seal::BigPoly(coeffCount, coeffBitCount, context->marshal_as<const char*>(hexPoly));
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                finally
                {
                    delete context;
                }
            }

            BigPoly::BigPoly(BigPoly ^copy) : bigpoly_(nullptr), owned_(true)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    bigpoly_ = new seal::BigPoly(copy->GetPolynomial());
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

            bool BigPoly::IsAlias::get()
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                return bigpoly_->is_alias();
            }

            int BigPoly::CoeffCount::get()
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                return bigpoly_->coeff_count();
            }

            int BigPoly::CoeffBitCount::get()
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                return bigpoly_->coeff_bit_count();
            }

            int BigPoly::UInt64Count::get()
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                return bigpoly_->uint64_count();
            }

            int BigPoly::CoeffUInt64Count::get()
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                return bigpoly_->coeff_uint64_count();
            }

            UInt64 *BigPoly::Pointer::get()
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                return bigpoly_->pointer();
            }

            bool BigPoly::IsZero::get()
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                return bigpoly_->is_zero();
            }

            BigUInt ^BigPoly::default::get(int index)
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                try
                {
                    return gcnew BigUInt(&bigpoly_->operator[](index));
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

            int BigPoly::GetSignificantCoeffCount()
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                return bigpoly_->significant_coeff_count();
            }

            int BigPoly::GetSignificantCoeffBitCount()
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                return bigpoly_->significant_coeff_bit_count();
            }

            void BigPoly::Set(BigPoly ^assign)
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                if (assign == nullptr)
                {
                    throw gcnew ArgumentNullException("assign cannot be null");
                }
                try
                {
                    *bigpoly_ = assign->GetPolynomial();
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

            void BigPoly::Set(String ^assign)
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                if (assign == nullptr)
                {
                    throw gcnew ArgumentNullException("assign cannot be null");
                }
                marshal_context ^context = gcnew marshal_context();
                try
                {
                    *bigpoly_ = context->marshal_as<const char*>(assign);
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
                finally
                {
                    delete context;
                }
            }

            void BigPoly::SetZero()
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                bigpoly_->set_zero();
            }

            void BigPoly::SetZero(int startCoeff)
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                bigpoly_->set_zero(startCoeff);
            }

            void BigPoly::SetZero(int startCoeff, int coeffCount)
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                bigpoly_->set_zero(startCoeff, coeffCount);
            }

            void BigPoly::Save(Stream ^stream)
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                try
                {
                    int32_t coeff_count32 = static_cast<int32_t>(bigpoly_->coeff_count());
                    Write(stream, reinterpret_cast<const char*>(&coeff_count32), sizeof(int32_t));
                    int32_t coeff_bit_count32 = static_cast<int32_t>(bigpoly_->coeff_bit_count());
                    Write(stream, reinterpret_cast<const char*>(&coeff_bit_count32), sizeof(int32_t));
                    int coeff_uint64_count = divide_round_up(bigpoly_->coeff_bit_count(), bits_per_uint64);
                    Write(stream, reinterpret_cast<const char*>(bigpoly_->pointer()), bigpoly_->coeff_count() * coeff_uint64_count * bytes_per_uint64);
                
                    GC::KeepAlive(stream);
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

            void BigPoly::Load(Stream ^stream)
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                try
                {
                    int32_t read_coeff_count = 0;
                    Read(stream, reinterpret_cast<char*>(&read_coeff_count), sizeof(int32_t));
                    int32_t read_coeff_bit_count = 0;
                    Read(stream, reinterpret_cast<char*>(&read_coeff_bit_count), sizeof(int32_t));
                    if (read_coeff_count > bigpoly_->coeff_count() || read_coeff_bit_count > bigpoly_->coeff_bit_count())
                    {
                        // Size is too large to currently fit, so resize.
                        bigpoly_->resize(std::max(read_coeff_count, bigpoly_->coeff_count()), std::max(read_coeff_bit_count, bigpoly_->coeff_bit_count()));
                    }
                    int read_coeff_uint64_count = divide_round_up(read_coeff_bit_count, bits_per_uint64);
                    int coeff_uint64_count = divide_round_up(bigpoly_->coeff_bit_count(), bits_per_uint64);
                    if (read_coeff_uint64_count == coeff_uint64_count)
                    {
                        Read(stream, reinterpret_cast<char*>(bigpoly_->pointer()), read_coeff_count * coeff_uint64_count * bytes_per_uint64);
                    }
                    else
                    {
                        // Coefficients are different sizes, so read one at a time.
                        uint64_t *coeff_ptr = bigpoly_->pointer();
                        for (int i = 0; i < read_coeff_count; ++i)
                        {
                            Read(stream, reinterpret_cast<char*>(coeff_ptr), read_coeff_uint64_count * bytes_per_uint64);
                            set_zero_uint(coeff_uint64_count - read_coeff_uint64_count, coeff_ptr + read_coeff_uint64_count);
                            coeff_ptr += coeff_uint64_count;
                        }
                    }

                    // Zero any remaining coefficients.
                    if (bigpoly_->coeff_count() > read_coeff_count)
                    {
                        set_zero_poly(bigpoly_->coeff_count() - read_coeff_count, coeff_uint64_count, bigpoly_->pointer() + read_coeff_count * coeff_uint64_count);
                    }

                    GC::KeepAlive(stream);
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

            void BigPoly::Resize(int coeffCount, int coeffBitCount)
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                try
                {
                    bigpoly_->resize(coeffCount, coeffBitCount);
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

            bool BigPoly::Equals(BigPoly ^compare)
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                if (compare == nullptr)
                {
                    return false;
                }
                auto result = *bigpoly_ == compare->GetPolynomial();
                GC::KeepAlive(compare);
                return result;
            }

            bool BigPoly::Equals(Object ^compare)
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                BigPoly ^comparepoly = dynamic_cast<BigPoly^>(compare);
                if (comparepoly != nullptr)
                {
                    return Equals(comparepoly);
                }
                return false;
            }

            String ^BigPoly::ToString()
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                return marshal_as<String^>(bigpoly_->to_string());
            }

            int BigPoly::GetHashCode()
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                return ComputeArrayHashCode(bigpoly_->pointer(), bigpoly_->uint64_count());
            }

            seal::BigPoly &BigPoly::GetPolynomial()
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                return *bigpoly_;
            }

            BigPoly::~BigPoly()
            {
                this->!BigPoly();
            }

            BigPoly::!BigPoly()
            {
                if (bigpoly_ != nullptr)
                {
                    if (owned_)
                    {
                        delete bigpoly_;
                    }
                    bigpoly_ = nullptr;
                }
            }

            BigPoly::BigPoly(const seal::BigPoly &value) : bigpoly_(nullptr), owned_(true)
            {
                try
                {
                    bigpoly_ = new seal::BigPoly(value);
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

            BigPoly::BigPoly(seal::BigPoly *value) : bigpoly_(value), owned_(false)
            {
            }

            void BigPoly::DuplicateTo(BigPoly ^destination)
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    bigpoly_->duplicate_to(*destination->bigpoly_);
                    GC::KeepAlive(destination);
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

            void BigPoly::DuplicateFrom(BigPoly ^value)
            {
                if (bigpoly_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigPoly is disposed");
                }
                if (value == nullptr)
                {
                    throw gcnew ArgumentNullException("value cannot be null");
                }
                try
                {
                    bigpoly_->duplicate_from(*value->bigpoly_);
                    GC::KeepAlive(value);
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
        }
    }
}