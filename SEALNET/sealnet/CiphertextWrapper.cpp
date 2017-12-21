#include "sealnet/CiphertextWrapper.h"
#include "seal/ciphertext.h"
#include "seal/bigpolyarray.h"
#include "seal/util/common.h"
#include "sealnet/Common.h"
#include <array>

using namespace std;
using namespace System;
using namespace System::IO;
using namespace System::Collections::Generic;

namespace seal
{
    /**
    <summary>Enables access to private members of seal::Ciphertext.</summary>
    */
    struct Ciphertext::CiphertextPrivateHelper
    {
        static void resize(seal::Ciphertext *ciphertext, int size, int poly_coeff_count, int coeff_mod_count)
        {
            ciphertext->resize(size, poly_coeff_count, coeff_mod_count);
        }

        static uint64_t *pointer(seal::Ciphertext *ciphertext)
        {
            return ciphertext->mutable_pointer();
        }

        static seal::EncryptionParameters::hash_block_type &hash_block(seal::Ciphertext *ciphertext)
        {
            return ciphertext->mutable_hash_block();
        }
    };
}

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            Ciphertext::Ciphertext()
            {
                try
                {
                    ciphertext_ = new seal::Ciphertext();
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

            Ciphertext::Ciphertext(MemoryPoolHandle ^pool)
            {
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    ciphertext_ = new seal::Ciphertext(pool->GetHandle());
                    GC::KeepAlive(pool);
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

            Ciphertext::Ciphertext(EncryptionParameters ^parms, MemoryPoolHandle ^pool)
            {
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    ciphertext_ = new seal::Ciphertext(parms->GetParms(), pool->GetHandle());
                    GC::KeepAlive(parms);
                    GC::KeepAlive(pool);
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

            Ciphertext::Ciphertext(EncryptionParameters ^parms)
            {
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                try
                {
                    ciphertext_ = new seal::Ciphertext(parms->GetParms());
                    GC::KeepAlive(parms);
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

            Ciphertext::Ciphertext(EncryptionParameters ^parms, int sizeCapacity, MemoryPoolHandle ^pool)
            {
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    ciphertext_ = new seal::Ciphertext(parms->GetParms(), sizeCapacity, pool->GetHandle());
                    GC::KeepAlive(parms);
                    GC::KeepAlive(pool);
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

            Ciphertext::Ciphertext(EncryptionParameters ^parms, int sizeCapacity)
            {
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                try
                {
                    ciphertext_ = new seal::Ciphertext(parms->GetParms(), sizeCapacity);
                    GC::KeepAlive(parms);
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

            Ciphertext::Ciphertext(Ciphertext ^copy)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    ciphertext_ = new seal::Ciphertext(copy->GetCiphertext());
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

            void Ciphertext::Set(Ciphertext ^assign)
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                if (assign == nullptr)
                {
                    throw gcnew ArgumentNullException("assign cannot be null");
                }
                try
                {
                    *ciphertext_ = *assign->ciphertext_;
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
            
            UInt64 Ciphertext::default::get(int coeffIndex)
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                try
                {
                    return ciphertext_->operator[](coeffIndex);
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

            void Ciphertext::Reserve(EncryptionParameters ^parms, int sizeCapacity, MemoryPoolHandle ^pool)
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    ciphertext_->reserve(parms->GetParms(), sizeCapacity, pool->GetHandle());
                    GC::KeepAlive(parms);
                    GC::KeepAlive(pool);
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

            void Ciphertext::Reserve(EncryptionParameters ^parms, int sizeCapacity)
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                try
                {
                    ciphertext_->reserve(parms->GetParms(), sizeCapacity);
                    GC::KeepAlive(parms);
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

            void Ciphertext::Reserve(int sizeCapacity, MemoryPoolHandle ^pool)
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    ciphertext_->reserve(sizeCapacity, pool->GetHandle());
                    GC::KeepAlive(pool);
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

            void Ciphertext::Reserve(int sizeCapacity)
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                try
                {
                    ciphertext_->reserve(sizeCapacity);
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

            void Ciphertext::Release()
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                try
                {
                    ciphertext_->release();
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

            int Ciphertext::CoeffModCount::get()
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                return ciphertext_->coeff_mod_count();
            }

            int Ciphertext::PolyCoeffCount::get()
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                return ciphertext_->poly_coeff_count();
            }

            int Ciphertext::SizeCapacity::get()
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                return ciphertext_->size_capacity();
            }

            int Ciphertext::Size::get()
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                return ciphertext_->size();
            }

            int Ciphertext::UInt64CountCapacity::get()
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                return ciphertext_->uint64_count_capacity();
            }

            int Ciphertext::UInt64Count::get()
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                return ciphertext_->uint64_count();
            }

            void Ciphertext::Save(Stream ^stream)
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                try
                {
                    Write(stream, reinterpret_cast<const char*>(&seal::Ciphertext::CiphertextPrivateHelper::hash_block(ciphertext_)), 
                        sizeof(seal::EncryptionParameters::hash_block_type));
                    int32_t size32 = static_cast<int32_t>(ciphertext_->size());
                    int32_t poly_coeff_count32 = static_cast<int32_t>(ciphertext_->poly_coeff_count());
                    int32_t coeff_mod_count32 = static_cast<int32_t>(ciphertext_->coeff_mod_count());
                    Write(stream, reinterpret_cast<const char*>(&size32), sizeof(int32_t));
                    Write(stream, reinterpret_cast<const char*>(&poly_coeff_count32), sizeof(int32_t));
                    Write(stream, reinterpret_cast<const char*>(&coeff_mod_count32), sizeof(int32_t));
                    Write(stream, reinterpret_cast<const char*>(seal::Ciphertext::CiphertextPrivateHelper::pointer(ciphertext_)),
                        ciphertext_->size() * ciphertext_->poly_coeff_count() * ciphertext_->coeff_mod_count() * seal::util::bytes_per_uint64);
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

            void Ciphertext::Load(Stream ^stream)
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                try
                {
                    Read(stream, reinterpret_cast<char*>(&seal::Ciphertext::CiphertextPrivateHelper::hash_block(ciphertext_)), 
                        sizeof(seal::EncryptionParameters::hash_block_type));
                    int32_t size32 = 0;
                    int32_t poly_coeff_count32 = 0;
                    int32_t coeff_mod_count32 = 0;
                    Read(stream, reinterpret_cast<char*>(&size32), sizeof(int32_t));
                    Read(stream, reinterpret_cast<char*>(&poly_coeff_count32), sizeof(int32_t));
                    Read(stream, reinterpret_cast<char*>(&coeff_mod_count32), sizeof(int32_t));
                    seal::Ciphertext::CiphertextPrivateHelper::resize(ciphertext_, size32, poly_coeff_count32, coeff_mod_count32);
                    Read(stream, reinterpret_cast<char*>(seal::Ciphertext::CiphertextPrivateHelper::pointer(ciphertext_)),
                        ciphertext_->size() * ciphertext_->poly_coeff_count() * ciphertext_->coeff_mod_count() * seal::util::bytes_per_uint64);
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

            Tuple<UInt64, UInt64, UInt64, UInt64> ^Ciphertext::HashBlock::get()
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                return gcnew Tuple<UInt64, UInt64, UInt64, UInt64>(
                    seal::Ciphertext::CiphertextPrivateHelper::hash_block(ciphertext_)[0],
                    seal::Ciphertext::CiphertextPrivateHelper::hash_block(ciphertext_)[1],
                    seal::Ciphertext::CiphertextPrivateHelper::hash_block(ciphertext_)[2],
                    seal::Ciphertext::CiphertextPrivateHelper::hash_block(ciphertext_)[3]);
            }

            Ciphertext::~Ciphertext()
            {
                this->!Ciphertext();
            }

            Ciphertext::!Ciphertext()
            {
                if (ciphertext_ != nullptr)
                {
                    delete ciphertext_;
                    ciphertext_ = nullptr;
                }
            }

            Ciphertext::Ciphertext(const seal::Ciphertext &ciphertext) : ciphertext_(nullptr)
            {
                try
                {
                    ciphertext_ = new seal::Ciphertext(ciphertext);
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

            seal::Ciphertext &Ciphertext::GetCiphertext()
            {
                if (ciphertext_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("Ciphertext is disposed");
                }
                return *ciphertext_;
            }
        }
    }
}