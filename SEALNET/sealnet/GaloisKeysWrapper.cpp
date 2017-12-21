#include "sealnet/GaloisKeysWrapper.h"
#include "sealnet/Common.h"

using namespace System;
using namespace System::IO;
using namespace System::Collections::Generic;
using namespace std;

namespace seal
{
    /**
    <summary>Enables access to private members of seal::SecretKey.</summary>
    */
    struct GaloisKeys::GaloisKeysPrivateHelper
    {
        static std::vector<std::vector<seal::Ciphertext> > &data(seal::GaloisKeys *galKeys)
        {
            return galKeys->mutable_data();
        }

        static int &decomposition_bit_count(seal::GaloisKeys *galKeys)
        {
            return galKeys->decomposition_bit_count_;
        }

        static seal::EncryptionParameters::hash_block_type &hash_block(seal::GaloisKeys *galKeys)
        {
            return galKeys->mutable_hash_block();
        }
    };
}

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            GaloisKeys::GaloisKeys()
            {
                try
                {
                    keys_ = new seal::GaloisKeys();
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

            GaloisKeys::GaloisKeys(GaloisKeys ^copy)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    keys_ = new seal::GaloisKeys(copy->GetKeys());
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

            void GaloisKeys::Set(GaloisKeys ^assign)
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("GaloisKeys is disposed");
                }
                if (assign == nullptr)
                {
                    throw gcnew ArgumentNullException("assign cannot be null");
                }
                try
                {
                    *keys_ = *assign->keys_;
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

            int GaloisKeys::Size::get()
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("GaloisKeys is disposed");
                }
                return keys_->size();
            }

            int GaloisKeys::DecompositionBitCount::get()
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("GaloisKeys is disposed");
                }
                return keys_->decomposition_bit_count();
            }

            List<List<Ciphertext^> ^> ^GaloisKeys::Data::get()
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("GaloisKeys is disposed");
                }
                try
                {
                    auto result = gcnew List<List<Ciphertext^> ^>;
                    for (int i = 0; i < keys_->data().size(); i++)
                    {
                        auto key_list = gcnew List<Ciphertext^>;
                        for (int j = 0; j < keys_->data()[i].size(); j++)
                        {
                            key_list->Add(gcnew Ciphertext(keys_->data()[i][j]));
                        }
                        result->Add(key_list);
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

            List<Ciphertext^> ^GaloisKeys::Key(UInt64 galoisElt)
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("GaloisKeys is disposed");
                }
                try
                {
                    auto result = gcnew List<Ciphertext^>;
                    for (int j = 0; j < keys_->key(galoisElt).size(); j++)
                    {
                        result->Add(gcnew Ciphertext(keys_->key(galoisElt)[j]));
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

            bool GaloisKeys::HasKey(UInt64 galoisElt)
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("GaloisKeys is disposed");
                }
                return keys_->has_key(galoisElt);
            }

            Tuple<UInt64, UInt64, UInt64, UInt64> ^GaloisKeys::HashBlock::get()
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("GaloisKeys is disposed");
                }
                return gcnew Tuple<UInt64, UInt64, UInt64, UInt64>(
                    seal::GaloisKeys::GaloisKeysPrivateHelper::hash_block(keys_)[0],
                    seal::GaloisKeys::GaloisKeysPrivateHelper::hash_block(keys_)[1],
                    seal::GaloisKeys::GaloisKeysPrivateHelper::hash_block(keys_)[2],
                    seal::GaloisKeys::GaloisKeysPrivateHelper::hash_block(keys_)[3]);
            }

            void GaloisKeys::Save(Stream ^stream)
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("GaloisKeys is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                try
                {
                    // Save the hash block
                    Write(stream,
                        reinterpret_cast<const char*>(&seal::GaloisKeys::GaloisKeysPrivateHelper::hash_block(keys_)),
                        sizeof(seal::EncryptionParameters::hash_block_type)
                    );

                    // Save the decomposition bit count
                    int32_t decomposition_bit_count32 = keys_->decomposition_bit_count();
                    Write(stream, reinterpret_cast<const char*>(&decomposition_bit_count32), sizeof(int32_t));

                    auto keysData = Data;

                    // Save the size of keys_
                    int32_t size32 = keysData->Count;
                    Write(stream, reinterpret_cast<const char*>(&size32), sizeof(int32_t));

                    // Loop over entries in the first list
                    for (int32_t i = 0; i < size32; i++)
                    {
                        auto keyList = keysData[i];

                        int32_t keySize32 = keyList->Count;
                        Write(stream, reinterpret_cast<const char*>(&keySize32), sizeof(int32_t));

                        // Loop over Ciphertexts and save all
                        for (int32_t j = 0; j < keySize32; j++)
                        {
                            keyList[j]->Save(stream);
                        }
                    }
                    GC::KeepAlive(keysData);
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

            void GaloisKeys::Load(Stream ^stream)
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("GaloisKeys is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                try
                {
                    // Make sure keys_ is empty before reading
                    seal::GaloisKeys::GaloisKeysPrivateHelper::data(keys_).clear();

                    // Read the hash block
                    Read(stream,
                        reinterpret_cast<char*>(&seal::GaloisKeys::GaloisKeysPrivateHelper::hash_block(keys_)),
                        sizeof(seal::EncryptionParameters::hash_block_type)
                    );

                    // Read the decomposition bit count
                    int32_t decomposition_bit_count32 = 0;
                    Read(stream, reinterpret_cast<char*>(&decomposition_bit_count32), sizeof(int32_t));
                    seal::GaloisKeys::GaloisKeysPrivateHelper::decomposition_bit_count(keys_) = decomposition_bit_count32;

                    // Read the size
                    int32_t readSize32 = 0;
                    Read(stream, reinterpret_cast<char*>(&readSize32), sizeof(int32_t));
                    seal::GaloisKeys::GaloisKeysPrivateHelper::data(keys_).resize(readSize32);

                    // Loop over entries in the list and read them all
                    for (int32_t i = 0; i < readSize32; i++)
                    {
                        // Read size of second list
                        int32_t keySize32 = 0;
                        Read(stream, reinterpret_cast<char*>(&keySize32), sizeof(int32_t));

                        // Loop over Ciphertexts and save all
                        auto key = gcnew Ciphertext;
                        for (int32_t j = 0; j < keySize32; j++)
                        {
                            key->Load(stream);
                            seal::GaloisKeys::GaloisKeysPrivateHelper::data(keys_)[i].emplace_back(key->GetCiphertext());
                        }
                        GC::KeepAlive(key);
                    }
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

            seal::GaloisKeys &GaloisKeys::GetKeys()
            {
                return *keys_;
            }

            GaloisKeys::~GaloisKeys()
            {
                this->!GaloisKeys();
            }

            GaloisKeys::!GaloisKeys()
            {
                if (keys_ != nullptr)
                {
                    delete keys_;
                    keys_ = nullptr;
                }
            }

            GaloisKeys::GaloisKeys(const seal::GaloisKeys &value) : keys_(nullptr)
            {
                try
                {
                    keys_ = new seal::GaloisKeys(value);
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