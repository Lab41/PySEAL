#include "MemoryPoolHandleWrapper.h"
#include "Common.h"

using namespace std;
using namespace System;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            MemoryPoolHandle::MemoryPoolHandle()
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

            MemoryPoolHandle::MemoryPoolHandle(MemoryPoolHandle ^copy)
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