#include "sealnet/PublicKeyWrapper.h"
#include "sealnet/Common.h"

using namespace std;
using namespace System;
using namespace System::IO;
using namespace System::Collections::Generic;

namespace seal
{
    /**
    <summary>Enables access to private members of seal::PublicKey.</summary>
    */
    struct PublicKey::PublicKeyPrivateHelper
    {
        static seal::BigPolyArray &data(seal::PublicKey *pk)
        {
            return pk->mutable_data();
        }

        static seal::EncryptionParameters::hash_block_type &hash_block(seal::PublicKey *pk)
        {
            return pk->mutable_hash_block();
        }
    };
}

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            PublicKey::PublicKey()
            {
                try
                {
                    pk_ = new seal::PublicKey();
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

            PublicKey::PublicKey(PublicKey ^copy)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    pk_ = new seal::PublicKey(copy->GetKey());
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

            void PublicKey::Set(PublicKey ^assign)
            {
                if (pk_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PublicKey is disposed");
                }
                if (assign == nullptr)
                {
                    throw gcnew ArgumentNullException("assign cannot be null");
                }
                try
                {
                    *pk_ = *assign->pk_;
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

            BigPolyArray ^PublicKey::Data::get()
            {
                if (pk_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PublicKey is disposed");
                }
                return gcnew BigPolyArray(seal::PublicKey::PublicKeyPrivateHelper::data(pk_));
            }

            void PublicKey::Save(Stream ^stream)
            {
                if (pk_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PublicKey is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                try
                {
                    Write(stream, reinterpret_cast<const char*>(&seal::PublicKey::PublicKeyPrivateHelper::hash_block(pk_)), 
                        sizeof(seal::EncryptionParameters::hash_block_type));
                    auto poly_array = gcnew BigPolyArray(&seal::PublicKey::PublicKeyPrivateHelper::data(pk_));
                    poly_array->Save(stream);
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

            void PublicKey::Load(Stream ^stream)
            {
                if (pk_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PublicKey is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                try
                {
                    Read(stream, reinterpret_cast<char*>(&seal::PublicKey::PublicKeyPrivateHelper::hash_block(pk_)), 
                        sizeof(seal::EncryptionParameters::hash_block_type));
                    auto poly_array = gcnew BigPolyArray(&seal::PublicKey::PublicKeyPrivateHelper::data(pk_));
                    poly_array->Load(stream);
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

            Tuple<UInt64, UInt64, UInt64, UInt64> ^PublicKey::HashBlock::get()
            {
                if (pk_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PublicKey is disposed");
                }
                return gcnew Tuple<UInt64, UInt64, UInt64, UInt64>(
                    seal::PublicKey::PublicKeyPrivateHelper::hash_block(pk_)[0],
                    seal::PublicKey::PublicKeyPrivateHelper::hash_block(pk_)[1],
                    seal::PublicKey::PublicKeyPrivateHelper::hash_block(pk_)[2],
                    seal::PublicKey::PublicKeyPrivateHelper::hash_block(pk_)[3]);
            }

            PublicKey::~PublicKey()
            {
                this->!PublicKey();
            }

            PublicKey::!PublicKey()
            {
                if (pk_ != nullptr)
                {
                    delete pk_;
                    pk_ = nullptr;
                }
            }

            PublicKey::PublicKey(const seal::PublicKey &ciphertext) : pk_(nullptr)
            {
                try
                {
                    pk_ = new seal::PublicKey(ciphertext);
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

            seal::PublicKey &PublicKey::GetKey()
            {
                if (pk_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PublicKey is disposed");
                }
                return *pk_;
            }
        }
    }
}