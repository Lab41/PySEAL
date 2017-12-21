#include "sealnet/EvaluationKeysWrapper.h"
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
    struct EvaluationKeys::EvaluationKeysPrivateHelper
    {
        static std::vector<std::vector<seal::Ciphertext> > &data(seal::EvaluationKeys *evks)
        {
            return evks->mutable_data();
        }

        static int &decomposition_bit_count(seal::EvaluationKeys *evks)
        {
            return evks->decomposition_bit_count_;
        }

        static seal::EncryptionParameters::hash_block_type &hash_block(seal::EvaluationKeys *evks)
        {
            return evks->mutable_hash_block();
        }
    };
}

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            EvaluationKeys::EvaluationKeys()
            {
                try
                {
                    keys_ = new seal::EvaluationKeys();
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

            EvaluationKeys::EvaluationKeys(EvaluationKeys ^copy)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    keys_ = new seal::EvaluationKeys(copy->GetKeys());
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

            void EvaluationKeys::Set(EvaluationKeys ^assign)
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EvaluationKeys is disposed");
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

            int EvaluationKeys::Size::get()
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EvaluationKeys is disposed");
                }
                return keys_->size();
            }

            int EvaluationKeys::DecompositionBitCount::get()
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EvaluationKeys is disposed");
                }
                return keys_->decomposition_bit_count();
            }

            List<List<Ciphertext^> ^> ^EvaluationKeys::Data::get()
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EvaluationKeys is disposed");
                }
                try
                {
                    auto result = gcnew List<List<Ciphertext^> ^>;
                    for (int i = 0; i < Size; i++)
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

            List<Ciphertext^> ^EvaluationKeys::Key(int keyPower)
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EvaluationKeys is disposed");
                }
                try
                {
                    auto result = gcnew List<Ciphertext^>;
                    for (int j = 0; j < keys_->key(keyPower).size(); j++)
                    {
                        result->Add(gcnew Ciphertext(keys_->key(keyPower)[j]));
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

            bool EvaluationKeys::HasKey(int keyPower)
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EvaluationKeys is disposed");
                }
                return keys_->has_key(keyPower);
            }

            Tuple<UInt64, UInt64, UInt64, UInt64> ^EvaluationKeys::HashBlock::get()
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EvaluationKeys is disposed");
                }
                return gcnew Tuple<UInt64, UInt64, UInt64, UInt64>(
                    seal::EvaluationKeys::EvaluationKeysPrivateHelper::hash_block(keys_)[0],
                    seal::EvaluationKeys::EvaluationKeysPrivateHelper::hash_block(keys_)[1],
                    seal::EvaluationKeys::EvaluationKeysPrivateHelper::hash_block(keys_)[2],
                    seal::EvaluationKeys::EvaluationKeysPrivateHelper::hash_block(keys_)[3]);
            }

            void EvaluationKeys::Save(Stream ^stream)
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EvaluationKeys is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                try
                {
                    // Save the hash block
                    Write(stream,
                        reinterpret_cast<const char*>(&seal::EvaluationKeys::EvaluationKeysPrivateHelper::hash_block(keys_)),
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

            void EvaluationKeys::Load(Stream ^stream)
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EvaluationKeys is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                try
                {
                    // Make sure keys_ is empty before reading
                    seal::EvaluationKeys::EvaluationKeysPrivateHelper::data(keys_).clear();

                    // Read the hash block
                    Read(stream,
                        reinterpret_cast<char*>(&seal::EvaluationKeys::EvaluationKeysPrivateHelper::hash_block(keys_)),
                        sizeof(seal::EncryptionParameters::hash_block_type)
                    );

                    // Read the decomposition bit count
                    int32_t decomposition_bit_count32 = 0;
                    Read(stream, reinterpret_cast<char*>(&decomposition_bit_count32), sizeof(int32_t));
                    seal::EvaluationKeys::EvaluationKeysPrivateHelper::decomposition_bit_count(keys_) = decomposition_bit_count32;

                    // Read the size
                    int32_t readSize32 = 0;
                    Read(stream, reinterpret_cast<char*>(&readSize32), sizeof(int32_t));
                    seal::EvaluationKeys::EvaluationKeysPrivateHelper::data(keys_).resize(readSize32);

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
                            seal::EvaluationKeys::EvaluationKeysPrivateHelper::data(keys_)[i].emplace_back(key->GetCiphertext());
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

            seal::EvaluationKeys &EvaluationKeys::GetKeys()
            {
                return *keys_;
            }

            EvaluationKeys::~EvaluationKeys()
            {
                this->!EvaluationKeys();
            }

            EvaluationKeys::!EvaluationKeys()
            {
                if (keys_ != nullptr)
                {
                    delete keys_;
                    keys_ = nullptr;
                }
            }

            EvaluationKeys::EvaluationKeys(const seal::EvaluationKeys &value) : keys_(nullptr)
            {
                try
                {
                    keys_ = new seal::EvaluationKeys(value);
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