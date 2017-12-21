#pragma once

#include <memory>
#include <stdexcept>
#include <utility>
#include "seal/util/mempool.h"
#include "seal/util/globals.h"

namespace seal
{
    /**
    Manages a shared pointer to a memory pool. SEAL uses memory pools for 
    improved performance due to the large number of memory allocations needed
    by the homomorphic encryption operations, and the underlying polynomial 
    arithmetic. The library automatically creates a shared global memory pool
    that is used for all dynamic allocations by default, and the user can
    optionally create any number of custom memory pools to be used instead.
    
    @Uses in Multi-Threaded Applications
    Sometimes the user might want to use specific memory pools for dynamic
    allocations in certain functions. For example, in heavily multi-threaded
    applications allocating concurrently from a shared memory pool might lead 
    to significant performance issues due to thread contention. For these cases
    SEAL provides overloads of the functions that take a MemoryPoolHandle as an
    additional argument, and uses the associated memory pool for all dynamic
    allocations inside the function. Whenever this functions is called, the 
    user can then simply pass a thread-local MemoryPoolHandle to be used.
    
    @Thread-Unsafe Memory Pools
    While memory pools are by default thread-safe, in some cases it suffices
    to have a memory pool be thread-unsafe. To get a little extra performance,
    the user can optionally create such thread-unsafe memory pools and use them
    just as they would use thread-safe memory pools.

    @Initialized and Uninitialized Handles
    A MemoryPoolHandle has to be set to point either to the global memory pool,
    or to a new memory pool. If this is not done, the MemoryPoolHandle is
    said to be uninitialized, and cannot be used. Initialization simple means
    assigning MemoryPoolHandle::Global() or MemoryPoolHandle::New() to it.

    @Managing Lifetime
    Internally, the MemoryPoolHandle wraps an std::shared_ptr pointing to
    a SEAL memory pool class. Thus, as long as a MemoryPoolHandle pointing to
    a particular memory pool exists, the pool stays alive. Classes such as
    Evaluator and Ciphertext store their own local copies of a MemoryPoolHandle
    to guarantee that the pool stays alive as long as the managing object 
    itself stays alive. The global memory pool is implemented as a global
    std::shared_ptr to a memory pool class, and is thus expected to stay
    alive for the entire duration of the program execution. Note that it can
    be problematic to create other global objects that use the memory pool
    e.g. in their constructor, as one would have to ensure the initialization
    order of these global variables to be correct (i.e. global memory pool
    first).
    */
    class MemoryPoolHandle
    {
    public:
        /**
        Creates a new uninitialized MemoryPoolHandle.
        */
        MemoryPoolHandle() = default;

        /**
        Creates a copy of a given MemoryPoolHandle. As a result, the created
        MemoryPoolHandle will point to the same underlying memory pool as the copied
        instance.

        @param[in] copy The MemoryPoolHandle to copy from
        */
        MemoryPoolHandle(const MemoryPoolHandle &copy)
        {
            operator =(copy);
        }

        /**
        Creates a new MemoryPoolHandle by moving a given one. As a result, the moved
        MemoryPoolHandle will become uninitialized.

        @param[in] source The MemoryPoolHandle to move from
        */
        MemoryPoolHandle(MemoryPoolHandle &&source) noexcept
        {
            operator =(std::move(source));
        }

        /**
        Overwrites the MemoryPoolHandle instance with the specified instance. As a result, 
        the current MemoryPoolHandle will point to the same underlying memory pool as 
        the assigned instance.

        @param[in] assign The MemoryPoolHandle instance to assign to the current instance
        */
        inline MemoryPoolHandle &operator =(const MemoryPoolHandle &assign)
        {
            pool_ = assign.pool_;
            return *this;
        }

        /**
        Moves a specified MemoryPoolHandle instance to the current instance. As a result,
        the assigned MemoryPoolHandle will become uninitialized.

        @param[in] assign The MemoryPoolHandle instance to assign to the current instance
        */
        inline MemoryPoolHandle &operator =(MemoryPoolHandle &&assign) noexcept
        {
            pool_ = std::move(assign.pool_);
            return *this;
        }

        /**
        Returns a MemoryPoolHandle pointing to the global memory pool.
        */
        inline static MemoryPoolHandle Global()
        {
            // We return an aliased shared_ptr; a hack necessary due to C++/CLI complications
            // with global shared_ptr.
            return MemoryPoolHandle(std::shared_ptr<util::MemoryPool>(std::shared_ptr<util::MemoryPool>(), 
                util::global_variables::global_memory_pool));
        }

        /**
        Returns a MemoryPoolHandle pointing to a new memory pool. The new memory pool
        can optionally be specified to be either thread-safe or thread-unsafe. By 
        default a thread-safe memory pool is created.

        @param[in] thread_safe Determines whether the new memory pool is thread-safe
        */
        inline static MemoryPoolHandle New(bool thread_safe = true)
        {
            if (thread_safe)
            {
                return MemoryPoolHandle(std::make_shared<util::MemoryPoolMT>());
            }
            return MemoryPoolHandle(std::make_shared<util::MemoryPoolST>());
        }

        /**
        Returns a reference to the internal SEAL memory pool that the MemoryPoolHandle
        points to. This function is mainly for internal use.

        @throws std::logic_error if the MemoryPoolHandle is uninitialized
        */
        inline operator util::MemoryPool &() const
        {
#ifdef SEAL_DEBUG
            if (!pool_)
            {
                throw std::logic_error("pool not initialized");
            }
#endif
            return *pool_.get();
        }

        /**
        Returns the number of different allocation sizes. This function returns the 
        number of different allocation sizes the memory pool pointed to by the current
        MemoryPoolHandle has made. For example, if the memory pool has only allocated 
        two allocations of sizes 128 KB, this function returns 1. If it has instead
        allocated one allocation of size 64 KB and one of 128 KB, this functions
        returns 2.

        @throws std::logic_error if the MemoryPoolHandle is uninitialized
        */        
        inline std::uint64_t pool_count() const
        {
            if (!pool_)
            {
                throw std::logic_error("pool not initialized");
            }
            return pool_->pool_count();
        }

        /**
        Returns the size of allocated memory. This functions returns the total amount
        of memory (in 64-bit words) allocated by the memory pool pointed to by the 
        current MemoryPoolHandle.
        
        @throws std::logic_error if the MemoryPoolHandle is uninitialized
        */
        inline std::uint64_t alloc_uint64_count() const
        {
            if (!pool_)
            {
                throw std::logic_error("pool not initialized");
            }
            return pool_->alloc_uint64_count();
        }

        /**
        Returns the size of allocated memory. This functions returns the total amount
        of memory (in bytes) allocated by the memory pool pointed to by the current 
        MemoryPoolHandle.

        @throws std::logic_error if the MemoryPoolHandle is uninitialized
        */
        inline std::uint64_t alloc_byte_count() const
        {
            if (!pool_)
            {
                throw std::logic_error("pool not initialized");
            }
            return pool_->alloc_byte_count();
        }

        /**
        Returns whether the MemoryPoolHandle is initialized.
        */
        inline operator bool () const
        {
            return !!pool_;
        }

        /**
        Compares MemoryPoolHandles. This function returns whether the current
        MemoryPoolHandle points to the same memory pool as a given MemoryPoolHandle.
        */
        inline bool operator ==(const MemoryPoolHandle &compare)
        {
            return pool_ == compare.pool_;
        }

        /**
        Compares MemoryPoolHandles. This function returns whether the current
        MemoryPoolHandle points to a different memory pool than a given 
        MemoryPoolHandle.
        */
        inline bool operator !=(const MemoryPoolHandle &compare)
        {
            return pool_ != compare.pool_;
        }

    private:
        MemoryPoolHandle(std::shared_ptr<util::MemoryPool> pool) noexcept : 
            pool_(std::move(pool))
        {
        }

        std::shared_ptr<util::MemoryPool> pool_ = nullptr;
    };
}