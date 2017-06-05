#pragma once

#include "memorypoolhandle.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            /**
            <summary>Provides the functionality for creating and using local memory pools.</summary>

            <remarks>
            <para>
            SEAL uses memory pools for improved performance due to the large number of memory allocations needed
            by the homomorphic encryption operations, and the underlying polynomial arithmetic. The library
            automatically creates a shared global memory pool, that is by default used by all instances of the
            computation-heavy classes such as Encryptor, Evaluator, and PolyCRTBuilder. However, sometimes the
            user might want to use local memory pools with some of these classes. For example, in heavily
            multi-threaded applications the global memory pool might become clogged due to concurrent allocations.
            Instead, the user might want to create a separate---say Evaluator---object for each thread, and have it
            use a thread-local memory pool. The MemoryPoolHandle class provides the functionality for doing this.
            </para>
            <para>
            For example, the user can create a MemoryPoolHandle that points to a new local memory pool by calling
            the static function acquire_new(). The MemoryPoolHandle it returns (or copies of it) can now be passed
            on as an argument to the constructors of one or more classes (such as Encryptor, Evaluator, and PolyCRTBuilder).
            </para>
            <para>
            Internally, a MemoryPoolHandle simply wraps a shared pointer to a util::MemoryPool object. This way
            the local memory pool will be automatically destroyed, and the memory released, as soon as no existing
            handles point to it. Since the global memory pool is a static object, it will always have a positive
            reference count, and thus will not be destroyed until the program terminates.
            </para>
            </remarks>
            */
            public ref class MemoryPoolHandle
            {
            public:
                /**
                <summary>Creates a new MemoryPoolHandle pointing to the global memory pool.</summary>
                */
                MemoryPoolHandle();

                /**
                <summary>Destroys the MemoryPoolHandle.</summary>
                */
                ~MemoryPoolHandle();

                /**
                <summary>Destroys the MemoryPoolHandle.</summary>
                */
                !MemoryPoolHandle();

                /**
                <summary>Creates a copy of a MemoryPoolHandle.</summary>

                <param name="copy">The MemoryPoolHandle to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                MemoryPoolHandle(MemoryPoolHandle ^copy);

                /**
                <summary>Overwrites the MemoryPoolHandle instance with a specified MemoryPoolHandle instance.</summary>

                Overwrites the MemoryPoolHandle instance with the specified instance, so that the current instance 
                will point to the same underlying memory pool as the assigned instance.
                </remarks>
                <param name="assign">The MemoryPoolHandle instance to whose value should be assigned to the current instance</param>
                <exception cref="System::ArgumentNullException">if assign is null</exception>
                */
                void Set(MemoryPoolHandle ^assign);

                /**
                <summary>Returns a MemoryPoolHandle pointing to the global memory pool.</summary>
                */
                static MemoryPoolHandle ^AcquireGlobal()
                {
                    return gcnew MemoryPoolHandle();
                }

                /**
                <summary>Returns a MemoryPoolHandle pointing to a new memory pool.</summary>
                */
                static MemoryPoolHandle ^AcquireNew()
                {
                    return gcnew MemoryPoolHandle(seal::MemoryPoolHandle::acquire_new());
                }

            internal:
                MemoryPoolHandle(const seal::MemoryPoolHandle &copy);

                /**
                <summary>Returns a reference to the underlying C++ MemoryPoolHandle.</summary>
                */
                seal::MemoryPoolHandle &GetHandle();

            private:
                seal::MemoryPoolHandle *handle_;
            };
        }
    }
}