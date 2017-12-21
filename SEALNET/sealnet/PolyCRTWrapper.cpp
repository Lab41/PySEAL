#include <vector>
#include "sealnet/PolyCRTWrapper.h"
#include "sealnet/BigPolyWrapper.h"
#include "sealnet/BigUIntWrapper.h"
#include "sealnet/EncryptionParamsWrapper.h"
#include "sealnet/Common.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace std;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            PolyCRTBuilder::PolyCRTBuilder(SEALContext ^context)
            {
                if (context == nullptr)
                {
                    throw gcnew ArgumentNullException("context cannot be null");
                }
                try
                {
                    polyCRTBuilder_ = new seal::PolyCRTBuilder(context->GetContext());
                    GC::KeepAlive(context);
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

            PolyCRTBuilder::PolyCRTBuilder(SEALContext ^context, MemoryPoolHandle ^pool)
            {
                if (context == nullptr)
                {
                    throw gcnew ArgumentNullException("context cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    polyCRTBuilder_ = new seal::PolyCRTBuilder(context->GetContext(), pool->GetHandle());
                    GC::KeepAlive(context);
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

            PolyCRTBuilder::!PolyCRTBuilder()
            {
                if (polyCRTBuilder_ != nullptr)
                {
                    delete polyCRTBuilder_;
                    polyCRTBuilder_ = nullptr;
                }
            }

            PolyCRTBuilder::PolyCRTBuilder(PolyCRTBuilder ^copy)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    polyCRTBuilder_ = new seal::PolyCRTBuilder(copy->GetPolyCRTBuilder());
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

            PolyCRTBuilder::~PolyCRTBuilder()
            {
                this->!PolyCRTBuilder();
            }

            seal::PolyCRTBuilder &PolyCRTBuilder::GetPolyCRTBuilder()
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                return *polyCRTBuilder_;
            }

            void PolyCRTBuilder::Compose(List<UInt64> ^values, Plaintext ^destination)
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                if (values == nullptr)
                {
                    throw gcnew ArgumentNullException("values cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    vector<uint64_t> v_values;
                    for each (UInt64 val in values)
                    {
                        v_values.emplace_back(val);
                    }

                    polyCRTBuilder_->compose(v_values, destination->GetPlaintext());
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

            void PolyCRTBuilder::Compose(List<Int64> ^values, Plaintext ^destination)
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                if (values == nullptr)
                {
                    throw gcnew ArgumentNullException("values cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    vector<int64_t> v_values;
                    for each (Int64 val in values)
                    {
                        v_values.emplace_back(val);
                    }

                    polyCRTBuilder_->compose(v_values, destination->GetPlaintext());
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

            void PolyCRTBuilder::Compose(Plaintext ^plain, MemoryPoolHandle ^pool)
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    polyCRTBuilder_->compose(plain->GetPlaintext(), pool->GetHandle());
                    GC::KeepAlive(plain);
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

            void PolyCRTBuilder::Compose(Plaintext ^plain)
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                try
                {
                    polyCRTBuilder_->compose(plain->GetPlaintext());
                    GC::KeepAlive(plain);
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

            void PolyCRTBuilder::Decompose(Plaintext ^plain, List<UInt64> ^destination)
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    destination->Clear();
                    vector<uint64_t> v_destination;
                    polyCRTBuilder_->decompose(plain->GetPlaintext(), v_destination);
                    GC::KeepAlive(plain);

                    for (size_t i = 0; i < v_destination.size(); i++)
                    {
                        destination->Add(v_destination[i]);
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

            void PolyCRTBuilder::Decompose(Plaintext ^plain, List<Int64> ^destination)
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    destination->Clear();
                    vector<int64_t> v_destination;
                    polyCRTBuilder_->decompose(plain->GetPlaintext(), v_destination);
                    GC::KeepAlive(plain);

                    for (size_t i = 0; i < v_destination.size(); i++)
                    {
                        destination->Add(v_destination[i]);
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

            void PolyCRTBuilder::Decompose(Plaintext ^plain, List<UInt64> ^destination, MemoryPoolHandle ^pool)
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    destination->Clear();
                    vector<uint64_t> v_destination;
                    polyCRTBuilder_->decompose(plain->GetPlaintext(), v_destination, pool->GetHandle());
                    GC::KeepAlive(plain);
                    GC::KeepAlive(pool);

                    for (size_t i = 0; i < v_destination.size(); i++)
                    {
                        destination->Add(v_destination[i]);
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

            void PolyCRTBuilder::Decompose(Plaintext ^plain, List<Int64> ^destination, MemoryPoolHandle ^pool)
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    destination->Clear();
                    vector<int64_t> v_destination;
                    polyCRTBuilder_->decompose(plain->GetPlaintext(), v_destination, pool->GetHandle());
                    GC::KeepAlive(plain);
                    GC::KeepAlive(pool);

                    for (size_t i = 0; i < v_destination.size(); i++)
                    {
                        destination->Add(v_destination[i]);
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

            void PolyCRTBuilder::Decompose(Plaintext ^plain, MemoryPoolHandle ^pool)
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    polyCRTBuilder_->decompose(plain->GetPlaintext(), pool->GetHandle());
                    GC::KeepAlive(plain);
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

            void PolyCRTBuilder::Decompose(Plaintext ^plain)
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                if (plain == nullptr)
                {
                    throw gcnew ArgumentNullException("plain cannot be null");
                }
                try
                {
                    polyCRTBuilder_->decompose(plain->GetPlaintext());
                    GC::KeepAlive(plain);
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

            int PolyCRTBuilder::SlotCount::get()
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                return polyCRTBuilder_->slot_count();
            }
        }
    }
}