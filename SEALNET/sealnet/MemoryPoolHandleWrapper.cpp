#include "sealnet/MemoryPoolHandleWrapper.h"
#include "sealnet/Common.h"
#include "seal/util/mempool.h"
#include <memory>

using namespace std;
using namespace System;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            MemoryPoolHandle::MemoryPoolHandle() : handle_(nullptr)
            {
                try
                {
                    handle_ = new seal::MemoryPoolHandle();
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

            MemoryPoolHandle::operator bool(MemoryPoolHandle ^pool)
            {
                if (pool->handle_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("MemoryPoolHandle is disposed");
                }
                return pool->handle_->operator bool();
            }
            
            MemoryPoolHandle::~MemoryPoolHandle()
            {
                this->!MemoryPoolHandle();
            }

            MemoryPoolHandle::!MemoryPoolHandle()
            {
                if (handle_ != nullptr)
                {
                    delete handle_;     
                    handle_ = nullptr;
                }
            }

            MemoryPoolHandle::MemoryPoolHandle(MemoryPoolHandle ^copy) : handle_(nullptr)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    handle_ = new seal::MemoryPoolHandle(copy->GetHandle());
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

            void MemoryPoolHandle::Set(MemoryPoolHandle ^assign)
            {
                if (handle_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("MemoryPoolHandle is disposed");
                }
                if (assign == nullptr)
                {
                    throw gcnew ArgumentNullException("assign cannot be null");
                }
                try
                {
                    *handle_ = assign->GetHandle();
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

            MemoryPoolHandle ^MemoryPoolHandle::Global()
            {
                try
                {
                    return gcnew MemoryPoolHandle(seal::MemoryPoolHandle::Global());
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

            MemoryPoolHandle ^MemoryPoolHandle::New()
            {
                try
                {
                    return gcnew MemoryPoolHandle(seal::MemoryPoolHandle::New());
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

            MemoryPoolHandle ^MemoryPoolHandle::New(bool threadSafe)
            {
                try
                {
                    return gcnew MemoryPoolHandle(seal::MemoryPoolHandle::New(threadSafe));
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

            UInt64 MemoryPoolHandle::PoolCount::get()
            {
                if (handle_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("MemoryPoolHandle is disposed");
                }
                try
                {
                    return handle_->pool_count();
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

            UInt64 MemoryPoolHandle::AllocUInt64Count::get()
            {
                if (handle_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("MemoryPoolHandle is disposed");
                }
                try
                {
                    return handle_->alloc_uint64_count();
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

            UInt64 MemoryPoolHandle::AllocByteCount::get()
            {
                if (handle_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("MemoryPoolHandle is disposed");
                }
                try
                {
                    return handle_->alloc_byte_count();
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

            MemoryPoolHandle::MemoryPoolHandle(const seal::MemoryPoolHandle &copy) : handle_(nullptr)
            {
                try
                {
                    handle_ = new seal::MemoryPoolHandle(copy);
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

            seal::MemoryPoolHandle &MemoryPoolHandle::GetHandle()
            {
                if (handle_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("MemoryPoolHandle is disposed");
                }
                return *handle_;
            }
        }
    }
}