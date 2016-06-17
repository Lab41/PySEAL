#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>
#include <stdexcept>
#include "util/common.h"
#include "util/uintcore.h"
#include "Common.h"
#include "BigUIntWrapper.h"

using namespace System;
using namespace System::IO;
using namespace System::Numerics;
using namespace msclr::interop;
using namespace seal::util;
using std::exception;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            BigUInt::BigUInt() : biguint_(nullptr), owned_(true)
            {
                try
                {
                    biguint_ = new seal::BigUInt();
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

            BigUInt::BigUInt(int bitCount) : biguint_(nullptr), owned_(true)
            {
                try
                {
                    biguint_ = new seal::BigUInt(bitCount);
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

            BigUInt::BigUInt(int bitCount, String ^hexString) : biguint_(nullptr), owned_(true)
            {
                if (hexString == nullptr)
                {
                    throw gcnew ArgumentNullException("hexString cannot be null");
                }
                marshal_context ^context = gcnew marshal_context();
                try
                {
                    biguint_ = new seal::BigUInt(bitCount, context->marshal_as<const char*>(hexString));
                    GC::KeepAlive(hexString);
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

            BigUInt::BigUInt(int bitCount, UInt64 value) : biguint_(nullptr), owned_(true)
            {
                try
                {
                    biguint_ = new seal::BigUInt(bitCount, value);
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

            BigUInt::BigUInt(System::Numerics::BigInteger ^bigInteger) : biguint_(nullptr), owned_(true)
            {
                if (bigInteger == nullptr)
                {
                    throw gcnew ArgumentNullException("bigInteger cannot be null");
                }
                array<Byte> ^bytes = bigInteger->ToByteArray();
                try
                {
                    int byte_count = bytes->Length;
                    biguint_ = new seal::BigUInt(byte_count * 8);
                    for (int i = 0; i < byte_count; i++)
                    {
                        (*biguint_)[i] = bytes[i];
                    }
                    GC::KeepAlive(bigInteger);
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

            BigUInt::BigUInt(String ^hexString) : biguint_(nullptr), owned_(true)
            {
                if (hexString == nullptr)
                {
                    throw gcnew ArgumentNullException("hexString cannot be null");
                }
                marshal_context ^context = gcnew marshal_context();
                try
                {
                    biguint_ = new seal::BigUInt(context->marshal_as<const char*>(hexString));
                    GC::KeepAlive(hexString);
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

            BigUInt::BigUInt(BigUInt ^copy) : biguint_(nullptr), owned_(true)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    biguint_ = new seal::BigUInt(copy->GetUInt());
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

            bool BigUInt::IsAlias::get()
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                return biguint_->is_alias();
            }

            int BigUInt::BitCount::get()
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                return biguint_->bit_count();
            }

            int BigUInt::ByteCount::get()
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                return biguint_->byte_count();
            }

            int BigUInt::UInt64Count::get()
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                return biguint_->uint64_count();
            }

            UInt64 *BigUInt::Pointer::get()
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                return biguint_->pointer();
            }

            bool BigUInt::IsZero::get()
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                return biguint_->is_zero();
            }

            Byte BigUInt::default::get(int index)
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                try
                {
                    return biguint_->operator[](index);
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

            void BigUInt::default::set(int index, Byte value)
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                try
                {
                    biguint_->operator[](index) = value;
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

            int BigUInt::GetSignificantBitCount()
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                return biguint_->significant_bit_count();
            }

            void BigUInt::Set(BigUInt ^assign)
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                if (assign == nullptr)
                {
                    throw gcnew ArgumentNullException("assign cannot be null");
                }
                try
                {
                    *biguint_ = assign->GetUInt();
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

            void BigUInt::Set(String ^assign)
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                if (assign == nullptr)
                {
                    throw gcnew ArgumentNullException("assign cannot be null");
                }
                marshal_context ^context = gcnew marshal_context();
                try
                {
                    *biguint_ = context->marshal_as<const char*>(assign);
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

            void BigUInt::Set(UInt64 assign)
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                try
                {
                    *biguint_ = assign;
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

            void BigUInt::SetZero()
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                biguint_->set_zero();
            }

            void BigUInt::Save(Stream ^stream)
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                try
                {
                    int32_t bit_count32 = static_cast<int32_t>(biguint_->bit_count());
                    Write(stream, reinterpret_cast<const char*>(&bit_count32), sizeof(int32_t));
                    int uint64_count = divide_round_up(biguint_->bit_count(), bits_per_uint64);
                    Write(stream, reinterpret_cast<const char*>(biguint_->pointer()), uint64_count * bytes_per_uint64);

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

            void BigUInt::Load(Stream ^stream)
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                try
                {
                    int32_t read_bit_count = 0;
                    Read(stream, reinterpret_cast<char*>(&read_bit_count), sizeof(int32_t));
                    if (read_bit_count > biguint_->bit_count())
                    {
                        // Size is too large to currently fit, so resize.
                        biguint_->resize(read_bit_count);
                    }
                    int read_uint64_count = divide_round_up(read_bit_count, bits_per_uint64);
                    Read(stream, reinterpret_cast<char*>(biguint_->pointer()), read_uint64_count * bytes_per_uint64);

                    // Zero any extra space.
                    int uint64_count = divide_round_up(biguint_->bit_count(), bits_per_uint64);
                    if (uint64_count > read_uint64_count)
                    {
                        set_zero_uint(uint64_count - read_uint64_count, biguint_->pointer() + read_uint64_count);
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

            void BigUInt::Resize(int bitCount)
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                try
                {
                    biguint_->resize(bitCount);
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

            bool BigUInt::Equals(BigUInt ^compare)
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                if (compare == nullptr)
                {
                    return false;
                }
                auto result = *biguint_ == compare->GetUInt();
                GC::KeepAlive(compare);
                return result;
            }

            bool BigUInt::Equals(Object ^compare)
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                BigUInt ^compareuint = dynamic_cast<BigUInt^>(compare);
                if (compareuint != nullptr)
                {
                    return Equals(compareuint);
                }
                return false;
            }

            BigInteger ^BigUInt::ToBigInteger()
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                int byte_count = biguint_->byte_count() + 1;
                array<Byte> ^bytes = gcnew array<Byte>(byte_count);
                for (int i = 0; i < byte_count - 1; i++)
                {
                    bytes[i] = (*biguint_)[i];
                }
                bytes[byte_count - 1] = 0;

                return gcnew BigInteger(bytes);
            }

            String ^BigUInt::ToString()
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                return marshal_as<String^>(biguint_->to_string());
            }

            String ^BigUInt::ToDecimalString()
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                return marshal_as<String^>(biguint_->to_dec_string());
            }

            int BigUInt::GetHashCode()
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                return ComputeArrayHashCode(biguint_->pointer(), biguint_->uint64_count());
            }

            seal::BigUInt &BigUInt::GetUInt()
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                return *biguint_;
            }

            BigUInt::~BigUInt()
            {
                this->!BigUInt();
            }

            BigUInt::!BigUInt()
            {
                if (biguint_ != nullptr)
                {
                    if (owned_)
                    {
                        delete biguint_;
                    }
                    biguint_ = nullptr;
                }
            }

            int BigUInt::CompareTo(BigUInt ^compare)
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                if (compare == nullptr)
                {
                    throw gcnew ArgumentNullException("compare cannot be null");
                }
                try
                {
                    auto result = biguint_->compareto(compare->GetUInt());
                    GC::KeepAlive(compare);
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

            int BigUInt::CompareTo(UInt64 compare)
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                try
                {
                    return biguint_->compareto(compare);
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

            BigUInt ^BigUInt::DivideRemainder(BigUInt ^operand2, BigUInt ^remainder)
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                if (operand2 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand2 cannot be null");
                }
                if (remainder == nullptr)
                {
                    throw gcnew ArgumentNullException("remainder cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(biguint_->divrem(operand2->GetUInt(), remainder->GetUInt()));
                    GC::KeepAlive(operand2);
                    GC::KeepAlive(remainder);
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

            BigUInt ^BigUInt::DivideRemainder(UInt64 operand2, BigUInt ^remainder)
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                if (remainder == nullptr)
                {
                    throw gcnew ArgumentNullException("remainder cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(biguint_->divrem(operand2, remainder->GetUInt()));
                    GC::KeepAlive(remainder);
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

            BigUInt ^BigUInt::ModuloInvert(BigUInt ^modulus)
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                if (modulus == nullptr)
                {
                    throw gcnew ArgumentNullException("modulus cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(biguint_->modinv(modulus->GetUInt()));
                    GC::KeepAlive(modulus);
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

            BigUInt ^BigUInt::ModuloInvert(UInt64 modulus)
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                try
                {
                    return gcnew BigUInt(biguint_->modinv(modulus));
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

            bool BigUInt::TryModuloInvert(BigUInt ^modulus, BigUInt ^inverse)
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                if (modulus == nullptr)
                {
                    throw gcnew ArgumentNullException("modulus cannot be null");
                }
                if (inverse == nullptr)
                {
                    throw gcnew ArgumentNullException("inverse cannot be null");
                }
                try
                {
                    auto result = biguint_->trymodinv(modulus->GetUInt(), inverse->GetUInt());
                    GC::KeepAlive(modulus);
                    GC::KeepAlive(inverse);
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

            bool BigUInt::TryModuloInvert(UInt64 modulus, BigUInt ^inverse)
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                if (inverse == nullptr)
                {
                    throw gcnew ArgumentNullException("inverse cannot be null");
                }
                try
                {
                    auto result = biguint_->trymodinv(modulus, inverse->GetUInt());
                    GC::KeepAlive(inverse);
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

            BigUInt ^BigUInt::operator +(BigUInt ^operand)
            {
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(+operand->GetUInt());
                    GC::KeepAlive(operand);
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

            BigUInt ^BigUInt::operator -(BigUInt ^operand)
            {
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(-operand->GetUInt());
                    GC::KeepAlive(operand);
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

            BigUInt ^BigUInt::operator ~(BigUInt ^operand)
            {
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(~operand->GetUInt());
                    GC::KeepAlive(operand);
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

            BigUInt ^BigUInt::operator ++(BigUInt ^operand)
            {
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(operand->GetUInt() + 1);
                    GC::KeepAlive(operand);
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

            BigUInt ^BigUInt::operator --(BigUInt ^operand)
            {
                if (operand == nullptr)
                {
                    throw gcnew ArgumentNullException("operand cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(operand->GetUInt() - 1);
                    GC::KeepAlive(operand);
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

            BigUInt ^BigUInt::operator +(BigUInt ^operand1, BigUInt ^operand2)
            {
                if (operand1 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand1 cannot be null");
                }
                if (operand2 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand2 cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(operand1->GetUInt() + operand2->GetUInt());
                    GC::KeepAlive(operand1);
                    GC::KeepAlive(operand2);
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

            BigUInt ^BigUInt::operator +(BigUInt ^operand1, UInt64 operand2)
            {
                if (operand1 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand1 cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(operand1->GetUInt() + operand2);
                    GC::KeepAlive(operand1);
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

            BigUInt ^BigUInt::operator -(BigUInt ^operand1, BigUInt ^operand2)
            {
                if (operand1 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand1 cannot be null");
                }
                if (operand2 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand2 cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(operand1->GetUInt() - operand2->GetUInt());
                    GC::KeepAlive(operand1);
                    GC::KeepAlive(operand2);
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

            BigUInt ^BigUInt::operator -(BigUInt ^operand1, UInt64 operand2)
            {
                if (operand1 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand1 cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(operand1->GetUInt() - operand2);
                    GC::KeepAlive(operand1);
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

            BigUInt ^BigUInt::operator *(BigUInt ^operand1, BigUInt ^operand2)
            {
                if (operand1 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand1 cannot be null");
                }
                if (operand2 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand2 cannot be null");
                }
                try
                {
                    auto result =gcnew BigUInt(operand1->GetUInt() * operand2->GetUInt());
                    GC::KeepAlive(operand1);
                    GC::KeepAlive(operand2);
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

            BigUInt ^BigUInt::operator *(BigUInt ^operand1, UInt64 operand2)
            {
                if (operand1 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand1 cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(operand1->GetUInt() * operand2);
                    GC::KeepAlive(operand1);
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

            BigUInt ^BigUInt::operator /(BigUInt ^operand1, BigUInt ^operand2)
            {
                if (operand1 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand1 cannot be null");
                }
                if (operand2 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand2 cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(operand1->GetUInt() / operand2->GetUInt());
                    GC::KeepAlive(operand1);
                    GC::KeepAlive(operand2);
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

            BigUInt ^BigUInt::operator /(BigUInt ^operand1, UInt64 operand2)
            {
                if (operand1 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand1 cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(operand1->GetUInt() / operand2);
                    GC::KeepAlive(operand1);
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

            BigUInt ^BigUInt::operator %(BigUInt ^operand1, BigUInt ^operand2)
            {
                if (operand1 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand1 cannot be null");
                }
                if (operand2 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand2 cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(operand1->GetUInt() % operand2->GetUInt());
                    GC::KeepAlive(operand1);
                    GC::KeepAlive(operand2);
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

            BigUInt ^BigUInt::operator %(BigUInt ^operand1, UInt64 operand2)
            {
                if (operand1 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand1 cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(operand1->GetUInt() % operand2);
                    GC::KeepAlive(operand1);
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

            BigUInt ^BigUInt::operator ^(BigUInt ^operand1, BigUInt ^operand2)
            {
                if (operand1 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand1 cannot be null");
                }
                if (operand2 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand2 cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(operand1->GetUInt() ^ operand2->GetUInt());
                    GC::KeepAlive(operand1);
                    GC::KeepAlive(operand2);
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

            BigUInt ^BigUInt::operator ^(BigUInt ^operand1, UInt64 operand2)
            {
                if (operand1 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand1 cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(operand1->GetUInt() ^ operand2);
                    GC::KeepAlive(operand1);
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

            BigUInt ^BigUInt::operator &(BigUInt ^operand1, BigUInt ^operand2)
            {
                if (operand1 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand1 cannot be null");
                }
                if (operand2 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand2 cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(operand1->GetUInt() & operand2->GetUInt());
                    GC::KeepAlive(operand1);
                    GC::KeepAlive(operand2);
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

            BigUInt ^BigUInt::operator &(BigUInt ^operand1, UInt64 operand2)
            {
                if (operand1 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand1 cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(operand1->GetUInt() & operand2);
                    GC::KeepAlive(operand1);
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

            BigUInt ^BigUInt::operator |(BigUInt ^operand1, BigUInt ^operand2)
            {
                if (operand1 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand1 cannot be null");
                }
                if (operand2 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand2 cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(operand1->GetUInt() | operand2->GetUInt());
                    GC::KeepAlive(operand1);
                    GC::KeepAlive(operand2);
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

            BigUInt ^BigUInt::operator |(BigUInt ^operand1, UInt64 operand2)
            {
                if (operand1 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand1 cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(operand1->GetUInt() | operand2);
                    GC::KeepAlive(operand1);
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

            BigUInt ^BigUInt::operator <<(BigUInt ^operand1, int shift)
            {
                if (operand1 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand1 cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(operand1->GetUInt() << shift);
                    GC::KeepAlive(operand1);
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

            BigUInt ^BigUInt::operator >>(BigUInt ^operand1, int shift)
            {
                if (operand1 == nullptr)
                {
                    throw gcnew ArgumentNullException("operand1 cannot be null");
                }
                try
                {
                    auto result = gcnew BigUInt(operand1->GetUInt() >> shift);
                    GC::KeepAlive(operand1);
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

            BigUInt::operator double(BigUInt ^value)
            {
                if (value == nullptr)
                {
                    throw gcnew ArgumentNullException("value cannot be null");
                }
                auto result = value->GetUInt().to_double();
                GC::KeepAlive(value);
                return result;
            }

            BigUInt::operator float(BigUInt ^value)
            {
                if (value == nullptr)
                {
                    throw gcnew ArgumentNullException("value cannot be null");
                }
                auto result = static_cast<float>((double)value);
                GC::KeepAlive(value);
                return result;
            }

            BigUInt::operator UInt64(BigUInt ^value)
            {
                if (value == nullptr)
                {
                    throw gcnew ArgumentNullException("value cannot be null");
                }
                seal::BigUInt &uint = value->GetUInt();
                if (uint.bit_count() == 0)
                {
                    GC::KeepAlive(value);
                    return 0;
                }
                auto result = uint.pointer()[0];
                GC::KeepAlive(value);
                return result;
            }

            BigUInt::operator Int64(BigUInt ^value)
            {
                if (value == nullptr)
                {
                    throw gcnew ArgumentNullException("value cannot be null");
                }
                auto result = static_cast<Int64>((UInt64)value);
                GC::KeepAlive(value);
                return result;
            }

            BigUInt::operator UInt32(BigUInt ^value)
            {
                if (value == nullptr)
                {
                    throw gcnew ArgumentNullException("value cannot be null");
                }
                auto result = static_cast<UInt32>((UInt64)value);
                GC::KeepAlive(value);
                return result;
            }

            BigUInt::operator Int32(BigUInt ^value)
            {
                if (value == nullptr)
                {
                    throw gcnew ArgumentNullException("value cannot be null");
                }
                auto result = static_cast<Int32>((UInt64)value);
                GC::KeepAlive(value);
                return result;
            }

            BigUInt ^BigUInt::Of(UInt64 value)
            {
                BigUInt ^result = gcnew BigUInt();
                result->Set(value);
                return result;
            }

            BigUInt::BigUInt(const seal::BigUInt &copy) : biguint_(nullptr), owned_(true)
            {
                try
                {
                    biguint_ = new seal::BigUInt(copy);
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

            BigUInt::BigUInt(seal::BigUInt *value) : biguint_(value), owned_(false)
            {
            }

            void BigUInt::DuplicateTo(BigUInt ^destination)
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }                
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    biguint_->duplicate_to(*destination->biguint_);
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

            void BigUInt::DuplicateFrom(BigUInt ^value)
            {
                if (biguint_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("BigUInt is disposed");
                }
                if (value == nullptr)
                {
                    throw gcnew ArgumentNullException("value cannot be null");
                }
                try
                {
                    biguint_->duplicate_from(*value->biguint_);
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
