#include <cstring>
#include "util/mempool.h"

using namespace std;

namespace seal
{
    namespace util
    {
        MemoryPoolItem *MemoryPoolHead::get()
        {
            bool expected = false;
            while (!locked_.compare_exchange_strong(expected, true, memory_order_acquire))
            {
                expected = false;
            }
            MemoryPoolItem *old_first = first_item_;
            if (old_first == nullptr)
            {
                locked_.store(false, memory_order_release);
                return new MemoryPoolItem(uint64_count_);
            }
            first_item_ = old_first->next();
            old_first->next() = nullptr;
            item_count_--;
            locked_.store(false, memory_order_release);
            return old_first;
        }

        void MemoryPoolHead::add(MemoryPoolItem *new_first)
        {
            bool expected = false;
            while (!locked_.compare_exchange_strong(expected, true, memory_order_acquire))
            {
                expected = false;
            }
            MemoryPoolItem *old_first = first_item_;
            new_first->next() = old_first;
            first_item_ = new_first;
            item_count_++;
            locked_.store(false, memory_order_release);
        }

        void MemoryPoolHead::free_items()
        {
            bool expected = false;
            while (!locked_.compare_exchange_strong(expected, true, memory_order_acquire))
            {
                expected = false;
            }
            MemoryPoolItem *curr = first_item_;
            while (curr != nullptr)
            {
                MemoryPoolItem *next = curr->next();
                delete curr;
                curr = next;
            }
            first_item_ = nullptr;
            item_count_ = 0;
            locked_.store(false, memory_order_release);
        }

        Pointer MemoryPool::get_for_uint64_count(int uint64_count)
        {
            if (uint64_count < 0)
            {
                throw std::invalid_argument("uint64_count");
            }
            if (uint64_count == 0)
            {
                return Pointer();
            }

            // For part 1, obtain just a reader lock and attempt to find size.
            ReaderLock readerLock = pools_locker_.acquire_read();
            int start = 0;
            int end = pools_.size();
            while (start < end)
            {
                int mid = (start + end) / 2;
                MemoryPoolHead *mid_head = pools_[mid];
                int mid_uint64_count = mid_head->uint64_count();
                if (uint64_count < mid_uint64_count)
                {
                    start = mid + 1;
                }
                else if (uint64_count > mid_uint64_count)
                {
                    end = mid;
                }
                else
                {
                    return Pointer(mid_head);
                }
            }
            readerLock.release();

            // Size was not found, so obtain an exclusive lock and search again.
            WriterLock writerLock = pools_locker_.acquire_write();
            start = 0;
            end = pools_.size();
            while (start < end)
            {
                int mid = (start + end) / 2;
                MemoryPoolHead *mid_head = pools_[mid];
                int mid_uint64_count = mid_head->uint64_count();
                if (uint64_count < mid_uint64_count)
                {
                    start = mid + 1;
                }
                else if (uint64_count > mid_uint64_count)
                {
                    end = mid;
                }
                else
                {
                    return Pointer(mid_head);
                }
            }

            // Size was still not found, but we own an exclusive lock so just add it.
            MemoryPoolHead *new_head = new MemoryPoolHead(uint64_count);
            if (!pools_.empty())
            {
                pools_.insert(pools_.begin() + start, new_head);
            }
            else
            {
                pools_.emplace_back(new_head);
            }

            return Pointer(new_head);
        }

        int MemoryPool::total_byte_count() const
        {
            ReaderLock lock = pools_locker_.acquire_read();
            int byte_count = 0;
            for (int i = pools_.size() - 1; i >= 0; --i)
            {
                MemoryPoolHead *head = pools_[i];
                byte_count += head->item_count() * head->uint64_count() * bytes_per_uint64;
            }
            return byte_count;
        }

        void MemoryPool::free_all()
        {
            WriterLock lock = pools_locker_.acquire_write();
            for (int i = pools_.size() - 1; i >= 0; --i)
            {
                MemoryPoolHead *head = pools_[i];
                head->free_items();
                delete head;
            }
            pools_.clear();
        }

        Pointer duplicate_if_needed(uint64_t *original, int uint64_count, bool condition, MemoryPool &pool)
        {
#ifdef _DEBUG
            if (original == nullptr && uint64_count > 0)
            {
                throw invalid_argument("original");
            }
            if (uint64_count < 0)
            {
                throw invalid_argument("uint64_count");
            }
#endif
            if (condition == false)
            {
                return Pointer::Aliasing(original);
            }
            Pointer allocation = pool.get_for_uint64_count(uint64_count);
            memcpy(allocation.get(), original, uint64_count * bytes_per_uint64);
            return allocation;
        }

        ConstPointer duplicate_if_needed(const uint64_t *original, int uint64_count, bool condition, MemoryPool &pool)
        {
#ifdef _DEBUG
            if (original == nullptr && uint64_count > 0)
            {
                throw invalid_argument("original");
            }
            if (uint64_count < 0)
            {
                throw invalid_argument("uint64_count");
            }
#endif
            if (condition == false)
            {
                return ConstPointer::Aliasing(original);
            }
            Pointer allocation = pool.get_for_uint64_count(uint64_count);
            memcpy(allocation.get(), original, uint64_count * bytes_per_uint64);
            ConstPointer const_allocation;
            const_allocation.acquire(allocation);
            return const_allocation;
        }
    }
}
