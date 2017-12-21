#pragma once

#include "seal/memorypoolhandle.h"

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            /**
            <summary>Manages a shared pointer to a memory pool.</summary>

            <remarks>
            <para>
            Manages a shared pointer to a memory pool. SEAL uses memory pools for
            improved performance due to the large number of memory allocations needed
            by the homomorphic encryption operations, and the underlying polynomial
            arithmetic. The library automatically creates a shared global memory pool
            that is used for all dynamic allocations by default, and the user can
            optionally create any number of custom memory pools to be used instead.
            </para>
            <para>
            Sometimes the user might want to use specific memory pools for dynamic
            allocations in certain functions. For example, in heavily multi-threaded
            applications allocating concurrently from a shared memory pool might lead
            to significant performance issues due to thread contention. For these cases
            SEAL provides overloads of the functions that take a MemoryPoolHandle as an
            additional argument, and uses the associated memory pool for all dynamic
            allocations inside the function. Whenever this functions is called, the
            user can then simply pass a thread-local MemoryPoolHandle to be used.
            </para>
            <para>
            While memory pools are by default thread-safe, in some cases it suffices
            to have a memory pool be thread-unsafe. To get a little extra performance,
            the user can optionally create such thread-unsafe memory pools and use them
            just as they would use thread-safe memory pools.
            </para>
            <para>
            A MemoryPoolHandle has to be set to point either to the global memory pool,
            or to a new memory pool. If this is not done, the MemoryPoolHandle is
            said to be uninitialized, and cannot be used. Initialization simple means
            assigning MemoryPoolHandle.Global() or MemoryPoolHandle.New() to it.
            </para>
            <para>
            Internally, the MemoryPoolHandle wraps an std::shared_ptr (C++) pointing to
            a SEAL memory pool class. Thus, as long as a MemoryPoolHandle pointing to
            a particular memory pool exists, the pool stays alive. Classes such as
            <see cref="Evaluator" /> and <see cref="Ciphertext" /> store their own 
            local copies of a MemoryPoolHandle to guarantee that the pool stays alive 
            as long as the managing object itself stays alive. The global memory pool 
            is implemented as a global std::shared_ptr (C++) to a memory pool class, 
            and is thus expected to stay alive for the entire duration of the program 
            execution. Note that it can be problematic to create other global objects 
            that use the memory pool e.g. in their constructor, as one would have to 
            ensure the initialization order of these global variables to be correct 
            (i.e. global memory pool first).
            </para>
            <para>
            In .NET applications there are two additional complications with using
            memory pools. First, since the garbage collector can run at any time, it
            is not clear that the memory pool performs well at all, as much of its
            benefits come from the fact that typically SEAL repeatedly allocates 
            memory for objects of only a few different sizes. To fix this problem, the 
            user can ensure that the managed objects are destroyed in good time, and 
            that the garbage collector runs perhaps more often than one might normally
            want it to run. Second, it is important to ensure that memory owned by the
            global memory pool is properly deallocated before the .NET application 
            closes by calling <see cref="ClearGlobalMemoryPool()" />.
            */

            public ref class MemoryPoolHandle
            {
            public:
                /**
                <summary>Creates a new uninitialized MemoryPoolHandle.</summary>
                */
                MemoryPoolHandle();

                /**
                <summary>Creates a copy of a given MemoryPoolHandle.</summary>

                <remarks>
                Creates a copy of a given MemoryPoolHandle. As a result, the created
                MemoryPoolHandle will point to the same underlying memory pool as the copied
                instance.
                </remarks>

                <param name="copy">The MemoryPoolHandle to copy from</param>
                <exception cref="System::ArgumentNullException">if copy is null</exception>
                */
                MemoryPoolHandle(MemoryPoolHandle ^copy);

                /**
                <summary>Overwrites the MemoryPoolHandle instance with the specified 
                instance.</summary>

                <remarks>
                Overwrites the MemoryPoolHandle instance with the specified instance. As a result,
                the current MemoryPoolHandle will point to the same underlying memory pool as the 
                assigned instance.
                </remarks>
                <param name="assign">The MemoryPoolHandle instance to assign to the current 
                instance</param>
                <exception cref="System::ArgumentNullException">if assign is null</exception>
                */
                void Set(MemoryPoolHandle ^assign);

                /**
                <summary>Returns a MemoryPoolHandle pointing to the global memory pool.</summary>
                */
                static MemoryPoolHandle ^Global();

                /**
                <summary>Returns a MemoryPoolHandle pointing to a new thread-safe memory
                pool.</summary>
                */
                static MemoryPoolHandle ^New();

                /**
                <summary>Returns a MemoryPoolHandle pointing to a new memory pool.</summary>

                <remarks>
                Returns a MemoryPoolHandle pointing to a new memory pool. The new memory pool
                can optionally be specified to be either thread-safe or thread-unsafe. By
                default a thread-safe memory pool is created.
                </remarks>
                <param name="threadSafe">Determines whether the new memory pool is 
                thread-safe</param>
                */
                static MemoryPoolHandle ^New(bool threadSafe);

                /**
                <summary>Returns the number of different allocation sizes.</summary>

                <remarks>
                Returns the number of different allocation sizes. This function returns the
                number of different allocation sizes the memory pool pointed to by the current
                MemoryPoolHandle has made. For example, if the memory pool has only allocated
                two allocations of sizes 128 KB, this function returns 1. If it has instead
                allocated one allocation of size 64 KB and one of 128 KB, this functions
                returns 2.
                </remarks>
                <exception cref="System::InvalidOperationException">if the MemoryPoolHandle is
                uninitialized</exception>
                */
                property System::UInt64 PoolCount {
                    System::UInt64 get();
                }

                /**
                <summary>Returns the size of allocated memory.</summary>

                <remarks>
                Returns the size of allocated memory. This functions returns the total amount
                of memory (in 64-bit words) allocated by the memory pool pointed to by the
                current MemoryPoolHandle.
                </remarks>
                <exception cref="System::InvalidOperationException">if the MemoryPoolHandle is
                uninitialized</exception>
                */
                property System::UInt64 AllocUInt64Count {
                    System::UInt64 get();
                }

                /**
                <summary>Returns the size of allocated memory.</summary>

                <remarks>
                Returns the size of allocated memory. This functions returns the total amount
                of memory (in bytes) allocated by the memory pool pointed to by the current 
                MemoryPoolHandle.
                </remarks>
                <exception cref="System::InvalidOperationException">if the MemoryPoolHandle is
                uninitialized</exception>
                */
                property System::UInt64 AllocByteCount {
                    System::UInt64 get();
                }

                /**
                <summary>Returns whether the MemoryPoolHandle is initialized.</summary>
                */
                static operator bool(MemoryPoolHandle ^pool);

                /**
                <summary>Destroys the MemoryPoolHandle.</summary>
                */
                ~MemoryPoolHandle();

                /**
                <summary>Destroys the MemoryPoolHandle.</summary>
                */
                !MemoryPoolHandle();

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