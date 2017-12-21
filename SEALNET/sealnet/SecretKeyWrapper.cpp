#include "sealnet/SecretKeyWrapper.h"
#include "sealnet/Common.h"

using namespace std;
using namespace System;
using namespace System::IO;
using namespace System::Collections::Generic;

namespace seal
{
    /**
    <summary>Enables access to private members of seal::SecretKey.</summary>
    */
    struct SecretKey::SecretKeyPrivateHelper
    {
        static seal::BigPoly &data(seal::SecretKey *sk)
        {
            return sk->mutable_data();
        }

        static seal::EncryptionParameters::hash_block_type &hash_block(seal::SecretKey *sk)
        {
            return sk->mutable_hash_block();
        }
    };
}

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            SecretKey::SecretKey()
            {
                try
                {
                    sk_ = new seal::SecretKey();
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

            SecretKey::SecretKey(SecretKey ^copy)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    sk_ = new seal::SecretKey(copy->GetKey());
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

            void SecretKey::Set(SecretKey ^assign)
            {
                if (sk_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SecretKey is disposed");
                }
                if (assign == nullptr)
                {
                    throw gcnew ArgumentNullException("assign cannot be null");
                }
                try
                {
                    *sk_ = *assign->sk_;
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

            BigPoly ^SecretKey::Data::get()
            {
                if (sk_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SecretKey is disposed");
                }
                return gcnew BigPoly(seal::SecretKey::SecretKeyPrivateHelper::data(sk_));
            }

            void SecretKey::Save(Stream ^stream)
            {
                if (sk_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SecretKey is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                try
                {
                    Write(stream, reinterpret_cast<const char*>(&seal::SecretKey::SecretKeyPrivateHelper::hash_block(sk_)), 
                        sizeof(seal::EncryptionParameters::hash_block_type));
                    auto poly = gcnew BigPoly(&seal::SecretKey::SecretKeyPrivateHelper::data(sk_));
                    poly->Save(stream);
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

            void SecretKey::Load(Stream ^stream)
            {
                if (sk_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SecretKey is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                try
                {
                    Read(stream, reinterpret_cast<char*>(&seal::SecretKey::SecretKeyPrivateHelper::hash_block(sk_)), 
                        sizeof(seal::EncryptionParameters::hash_block_type));
                    auto poly = gcnew BigPoly(&seal::SecretKey::SecretKeyPrivateHelper::data(sk_));
                    poly->Load(stream);
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

            SecretKey::~SecretKey()
            {
                this->!SecretKey();
            }

            SecretKey::!SecretKey()
            {
                if (sk_ != nullptr)
                {
                    delete sk_;
                    sk_ = nullptr;
                }
            }

            Tuple<UInt64, UInt64, UInt64, UInt64> ^SecretKey::HashBlock::get()
            {
                if (sk_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SecretKey is disposed");
                }
                return gcnew Tuple<UInt64, UInt64, UInt64, UInt64>(
                    seal::SecretKey::SecretKeyPrivateHelper::hash_block(sk_)[0],
                    seal::SecretKey::SecretKeyPrivateHelper::hash_block(sk_)[1],
                    seal::SecretKey::SecretKeyPrivateHelper::hash_block(sk_)[2],
                    seal::SecretKey::SecretKeyPrivateHelper::hash_block(sk_)[3]);
            }

            SecretKey::SecretKey(const seal::SecretKey &ciphertext) : sk_(nullptr)
            {
                try
                {
                    sk_ = new seal::SecretKey(ciphertext);
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

            seal::SecretKey &SecretKey::GetKey()
            {
                if (sk_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SecretKey is disposed");
                }
                return *sk_;
            }
        }
    }
}