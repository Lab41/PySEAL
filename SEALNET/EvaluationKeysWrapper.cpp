#include "EvaluationKeysWrapper.h"
#include "Common.h"

using namespace System;
using namespace std;

using namespace System::Collections::Generic;
using namespace System::IO;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            EvaluationKeys::EvaluationKeys(List<System::Tuple<BigPolyArray^, BigPolyArray^> ^> ^keys) : keys_(nullptr)
            {
                if (keys == nullptr)
                {
                    throw gcnew ArgumentNullException("keys cannot be null");
                }
                try
                {
                    vector < std::pair<seal::BigPolyArray, seal::BigPolyArray> > v_keys;
                    for each(System::Tuple<BigPolyArray^, BigPolyArray^> ^key in keys)
                    {
                        if (key == nullptr)
                        {
                            throw gcnew ObjectDisposedException("keys cannot be null");
                        }
                        v_keys.emplace_back(key->Item1->GetArray(), key->Item2->GetArray());
                        GC::KeepAlive(key);
                    }
                    keys_ = new seal::EvaluationKeys(v_keys);
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

            EvaluationKeys::EvaluationKeys() : keys_(nullptr)
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

            int EvaluationKeys::Size::get()
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EvaluationKeys is disposed");
                }
                return keys_->size();
            }

            System::Tuple<BigPolyArray^, BigPolyArray^> ^EvaluationKeys::default::get(int index)
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EvaluationKeys is disposed");
                }
                try
                {
                    return gcnew System::Tuple<BigPolyArray^, BigPolyArray^>(gcnew BigPolyArray(&keys_->operator[](index).first), gcnew BigPolyArray(&keys_->operator[](index).second));
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

            List<System::Tuple<BigPolyArray^, BigPolyArray^> ^> ^ EvaluationKeys::Keys::get()
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EvaluationKeys is disposed");
                }
                try 
                {
                    List<System::Tuple<BigPolyArray^, BigPolyArray^> ^> ^output = gcnew List<System::Tuple<BigPolyArray^, BigPolyArray^> ^>;
                    for (int i = 0; i < Size; ++i) 
                    {
                        if (default[i] == nullptr) 
                        {
                            throw gcnew ObjectDisposedException("evaluation keys cannot be null");
                        }
                        output->Add(default[i]);
                    }
                    return output;
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
                    // save the size of keys_
                    int32_t size32 = static_cast<int32_t> (keys_->size());
                    Write(stream, reinterpret_cast<const char*>(&size32), sizeof(int32_t));

                    // loop over entries in the list, calling Save of BigPolyArray
                    for (int i = 0; i < (keys_->size()) ; ++i)
                    {
                        default[i]->Item1->Save(stream);
                        default[i]->Item2->Save(stream);
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
                    keys_->clear();

                    // Read the size
                    int32_t readKeysSize = 0;
                    Read(stream, reinterpret_cast<char*>(&readKeysSize), sizeof(int32_t));

                    auto first = gcnew BigPolyArray();
                    auto second = gcnew BigPolyArray();

                    seal::BigPolyArray &first_underlying = first->GetArray();
                    seal::BigPolyArray &second_underlying = second->GetArray();

                    //loop, reading consecutive BigPolyArrays as pairs.
                    for (int i = 0; i < readKeysSize; ++i)
                    {
                        first->Load(stream);
                        second->Load(stream);
                        keys_->keys().emplace_back(first_underlying, second_underlying);
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