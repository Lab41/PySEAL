#ifndef SEAL_UTIL_MEMPOOL_H
#define SEAL_UTIL_MEMPOOL_H

#include <cstdint>
#include <vector>
#include <stdexcept>
#include "util/common.h"

namespace seal
{
    namespace util
    {
        class MemoryPoolItem
        {
        public:
            MemoryPoolItem(int uint64_count) : pointer_(nullptr), next_(nullptr)
            {
#ifdef _DEBUG
                if (uint64_count < 0)
                {
                    throw std::invalid_argument("uint64_count");
                }
#endif
                pointer_ = uint64_count > 0 ? new uint64_t[uint64_count] : nullptr;
            }

            std::uint64_t *pointer()
            {
                return pointer_;
            }

            const std::uint64_t *pointer() const
            {
                return pointer_;
            }

            MemoryPoolItem* &next()
            {
                return next_;
            }

            const MemoryPoolItem *next() const
            {
                return next_;
            }

            ~MemoryPoolItem()
            {
                if (pointer_ != nullptr)
                {
                    delete[] pointer_;
                    pointer_ = nullptr;
                }
            }

        private:
            MemoryPoolItem(const MemoryPoolItem &copy) = delete;

            MemoryPoolItem &operator =(const MemoryPoolItem &assign) = delete;

            std::uint64_t *pointer_;

            MemoryPoolItem* next_;
        };

        class MemoryPoolHead
        {
        public:
            MemoryPoolHead(int uint64_count) : uint64_count_(uint64_count), first_item_(nullptr)
            {
#ifdef _DEBUG
                if (uint64_count < 0)
                {
                    throw std::invalid_argument("uint64_count");
                }
#endif
            }

            int uint64_count() const
            {
                return uint64_count_;
            }

            int item_count() const;

            bool is_empty() const
            {
                return first_item_ == nullptr;
            }

            MemoryPoolItem *get()
            {
                MemoryPoolItem *old_first = first_item_;
                if (old_first == nullptr)
                {
                    return new MemoryPoolItem(uint64_count_);
                }
                first_item_ = old_first->next();
                old_first->next() = nullptr;
                return old_first;
            }

            void add(MemoryPoolItem *new_first)
            {
                MemoryPoolItem *old_first = first_item_;
                new_first->next() = old_first;
                first_item_ = new_first;
            }

            void free_items();

        private:
            MemoryPoolHead(const MemoryPoolHead &copy) = delete;

            MemoryPoolHead &operator =(const MemoryPoolHead &assign) = delete;

            int uint64_count_;

            MemoryPoolItem* first_item_;
        };

        class ConstPointer;

        class Pointer
        {
        public:
            friend class ConstPointer;

            Pointer() : head_(nullptr), item_(nullptr), pointer_(nullptr), alias_(false)
            {
            }

            Pointer(MemoryPoolHead *head) : head_(nullptr), item_(nullptr), pointer_(nullptr), alias_(false)
            {
#ifdef _DEBUG
                if (head == nullptr)
                {
                    throw std::invalid_argument("head");
                }
#endif
                head_ = head;
                item_ = head->get();
                pointer_ = item_->pointer();
            }

            Pointer(Pointer &&move) : head_(move.head_), item_(move.item_), pointer_(move.pointer_), alias_(move.alias_)
            {
                move.item_ = nullptr;
                move.head_ = nullptr;
                move.pointer_ = nullptr;
                move.alias_ = false;
            }

            std::uint64_t &operator[](int index)
            {
                return pointer_[index];
            }

            std::uint64_t operator[](int index) const
            {
                return pointer_[index];
            }

            Pointer &operator =(Pointer &&assign)
            {
                acquire(assign);
                return *this;
            }

            bool is_set() const
            {
                return pointer_ != nullptr;
            }

            std::uint64_t *get()
            {
                return pointer_;
            }

            const std::uint64_t *get() const
            {
                return pointer_;
            }

            void release()
            {
                if (head_ != nullptr)
                {
                    head_->add(item_);
                }
                else if (pointer_ != nullptr && !alias_)
                {
                    delete[] pointer_;
                }
                item_ = nullptr;
                head_ = nullptr;
                pointer_ = nullptr;
                alias_ = false;
            }

            void acquire(Pointer &other)
            {
                if (this == &other)
                {
                    return;
                }
                release();
                head_ = other.head_;
                item_ = other.item_;
                pointer_ = other.pointer_;
                alias_ = other.alias_;
                other.item_ = nullptr;
                other.head_ = nullptr;
                other.pointer_ = nullptr;
                other.alias_ = false;
            }

            void swap_with(Pointer &other)
            {
                std::swap(head_, other.head_);
                std::swap(item_, other.item_);
                std::swap(pointer_, other.pointer_);
                std::swap(alias_, other.alias_);
            }

            ~Pointer()
            {
                release();
            }

            static Pointer Owning(uint64_t *pointer)
            {
                return Pointer(pointer, false);
            }

            static Pointer Aliasing(uint64_t *pointer)
            {
                return Pointer(pointer, true);
            }

        private:
            Pointer(uint64_t *pointer, bool alias) : head_(nullptr), item_(nullptr), pointer_(pointer), alias_(alias)
            {
            }

            Pointer(const Pointer &copy) = delete;

            Pointer &operator =(const Pointer &assign) = delete;

            MemoryPoolHead *head_;

            MemoryPoolItem *item_;

            uint64_t *pointer_;

            bool alias_;
        };

        class ConstPointer
        {
        public:
            ConstPointer() : head_(nullptr), item_(nullptr), pointer_(nullptr), alias_(false)
            {
            }

            ConstPointer(MemoryPoolHead *head) : head_(nullptr), item_(nullptr), pointer_(nullptr), alias_(false)
            {
#ifdef _DEBUG
                if (head == nullptr)
                {
                    throw std::invalid_argument("head");
                }
#endif
                head_ = head;
                item_ = head->get();
                pointer_ = item_->pointer();
            }

            ConstPointer(ConstPointer &&move) : head_(move.head_), item_(move.item_), pointer_(move.pointer_), alias_(move.alias_)
            {
                move.item_ = nullptr;
                move.head_ = nullptr;
                move.pointer_ = nullptr;
                move.alias_ = false;
            }

            ConstPointer(Pointer &&move) : head_(move.head_), item_(move.item_), pointer_(move.pointer_), alias_(move.alias_)
            {
                move.item_ = nullptr;
                move.head_ = nullptr;
                move.pointer_ = nullptr;
                move.alias_ = false;
            }

            ConstPointer &operator =(ConstPointer &&assign)
            {
                acquire(assign);
                return *this;
            }

            ConstPointer &operator =(Pointer &&assign)
            {
                acquire(assign);
                return *this;
            }

            std::uint64_t operator[](int index) const
            {
                return pointer_[index];
            }

            bool is_set() const
            {
                return pointer_ != nullptr;
            }

            const std::uint64_t *get() const
            {
                return pointer_;
            }

            void release()
            {
                if (head_ != nullptr)
                {
                    head_->add(item_);
                }
                else if (pointer_ != nullptr && !alias_)
                {
                    delete[] pointer_;
                }
                item_ = nullptr;
                head_ = nullptr;
                pointer_ = nullptr;
                alias_ = false;
            }

            void acquire(ConstPointer &other)
            {
                if (this == &other)
                {
                    return;
                }
                release();
                head_ = other.head_;
                item_ = other.item_;
                pointer_ = other.pointer_;
                alias_ = other.alias_;
                other.item_ = nullptr;
                other.head_ = nullptr;
                other.pointer_ = nullptr;
                other.alias_ = false;
            }

            void acquire(Pointer &other)
            {
                release();
                head_ = other.head_;
                item_ = other.item_;
                pointer_ = other.pointer_;
                alias_ = other.alias_;
                other.item_ = nullptr;
                other.head_ = nullptr;
                other.pointer_ = nullptr;
                other.alias_ = false;
            }

            void swap_with(ConstPointer &other)
            {
                std::swap(head_, other.head_);
                std::swap(item_, other.item_);
                std::swap(pointer_, other.pointer_);
                std::swap(alias_, other.alias_);
            }

            ~ConstPointer()
            {
                release();
            }

            static ConstPointer Owning(uint64_t *pointer)
            {
                return ConstPointer(pointer, false);
            }

            static ConstPointer Aliasing(const uint64_t *pointer)
            {
                return ConstPointer(const_cast<uint64_t*>(pointer), true);
            }

        private:
            ConstPointer(uint64_t *pointer, bool alias) : head_(nullptr), item_(nullptr), pointer_(pointer), alias_(alias)
            {
            }

            ConstPointer(const ConstPointer &copy) = delete;

            ConstPointer &operator =(const ConstPointer &assign) = delete;

            MemoryPoolHead *head_;

            MemoryPoolItem *item_;

            uint64_t *pointer_;

            bool alias_;
        };

        class MemoryPool
        {
        public:
            MemoryPool() {}

            Pointer get_for_byte_count(int byte_count)
            {
#ifdef _DEBUG
                if (byte_count < 0)
                {
                    throw std::invalid_argument("byte_count");
                }
#endif
                int uint64_count = divide_round_up(byte_count, bytes_per_uint64);
                return get_for_uint64_count(uint64_count);
            }

            Pointer get_for_uint64_count(int uint64_count);

            int pool_count() const
            {
                return static_cast<int>(pools_.size());
            }

            int total_byte_count() const;

            void free_all();

            ~MemoryPool()
            {
                free_all();
            }

        private:
            MemoryPool(const MemoryPool &copy) = delete;

            MemoryPool &operator =(const MemoryPool &assign) = delete;

            std::vector<MemoryPoolHead*> pools_;
        };

        Pointer duplicate_if_needed(uint64_t *original, int uint64_count, bool condition, MemoryPool &pool);

        ConstPointer duplicate_if_needed(const uint64_t *original, int uint64_count, bool condition, MemoryPool &pool);
    }
}

#endif // SEAL_UTIL_MEMPOOL_H
