#include "Common.h"
#include "EvaluationKeysWrapper.h"
#include "BigPolyWrapper.h"

using namespace System;
using namespace System::IO;
using namespace std;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            EvaluationKeys::EvaluationKeys() : keys_(nullptr), owned_(true)
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

            EvaluationKeys::EvaluationKeys(int count) : keys_(nullptr), owned_(true)
            {
                try
                {
                    keys_ = new seal::EvaluationKeys(count);
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

            EvaluationKeys::EvaluationKeys(EvaluationKeys ^copy) : keys_(nullptr), owned_(true)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    keys_ = new seal::EvaluationKeys(copy->GetKeys());
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

            int EvaluationKeys::Count::get()
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EvaluationKeys is disposed");
                }
                return keys_->count();
            }

            BigPoly ^EvaluationKeys::default::get(int poly_index)
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EvaluationKeys is disposed");
                }
                try
                {
                    return gcnew BigPoly(&keys_->operator[](poly_index));
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

            void EvaluationKeys::Resize(int count)
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EvaluationKeys is disposed");
                }
                try
                {
                    keys_->resize(count);
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

            void EvaluationKeys::Clear()
            {
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EvaluationKeys is disposed");
                }
                keys_->clear();
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
                    *keys_ = assign->GetKeys();
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
                    int32_t count32 = static_cast<int32_t>(keys_->count());
                    Write(stream, reinterpret_cast<const char*>(&count32), sizeof(int32_t));
                    for (int i = 0; i < keys_->count(); ++i)
                    {
                        BigPoly ^key = gcnew BigPoly(&keys_->operator[](i));
                        key->Save(stream);
                    }
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
                    int32_t count32 = 0;
                    Read(stream, reinterpret_cast<char*>(&count32), sizeof(int32_t));
                    if (keys_->count() != count32)
                    {
                        keys_->resize(static_cast<int>(count32));
                    }
                    for (int i = 0; i < keys_->count(); ++i)
                    {
                        BigPoly ^key = gcnew BigPoly(&keys_->operator[](i));
                        key->Load(stream);
                    }
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
                if (keys_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EvaluationKeys is disposed");
                }
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
                    if (owned_)
                    {
                        delete keys_;
                    }
                    keys_ = nullptr;
                }
            }

            EvaluationKeys::EvaluationKeys(const seal::EvaluationKeys &keys) : keys_(nullptr), owned_(true)
            {
                try
                {
                    keys_ = new seal::EvaluationKeys(keys);
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

            EvaluationKeys::EvaluationKeys(seal::EvaluationKeys *keys) : keys_(keys), owned_(false)
            {
            }
        }
    }
}