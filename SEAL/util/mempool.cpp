#include <cstring>
#include "util/mempool.h"

using namespace std;

namespace seal
{
    namespace util
    {
        int MemoryPoolHead::item_count() const
        {
            int count = 0;
            MemoryPoolItem *curr = first_item_;
            while (curr != nullptr)
            {
                count++;
                MemoryPoolItem *next = curr->next();
                curr = next;
            }
            return count;
        }

        void MemoryPoolHead::free_items()
        {
            MemoryPoolItem *curr = first_item_;
            first_item_ = nullptr;
            while (curr != nullptr)
            {
                MemoryPoolItem *next = curr->next();
                delete curr;
                curr = next;
            }
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
            int start = 0;
            int end = static_cast<int>(pools_.size());
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
            MemoryPoolHead *new_head = new MemoryPoolHead(uint64_count);
            pools_.insert(pools_.begin() + start, new_head);
            return Pointer(new_head);
        }

        int MemoryPool::total_byte_count() const
        {
            int byte_count = 0;
            for (int i = static_cast<int>(pools_.size()) - 1; i >= 0; --i)
            {
                MemoryPoolHead *head = pools_[i];
                byte_count += head->item_count() * head->uint64_count() * bytes_per_uint64;
            }
            return byte_count;
        }

        void MemoryPool::free_all()
        {
            for (int i = static_cast<int>(pools_.size()) - 1; i >= 0; --i)
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
