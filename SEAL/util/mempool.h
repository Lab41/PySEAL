#pragma once

#include <cstdint>
#include <vector>
#include <stdexcept>
#include <memory>
#include "util/common.h"
#include "util/locks.h"

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
                pointer_ = uint64_count > 0 ? new std::uint64_t[uint64_count] : nullptr;
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
            MemoryPoolHead(int uint64_count) : locked_(false), uint64_count_(uint64_count), item_count_(0), first_item_(nullptr)
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

            int item_count() const
            {
                return item_count_;
            }

            bool is_empty() const
            {
                return item_count_ == 0;
            }

            MemoryPoolItem *get();

            void add(MemoryPoolItem *new_first);

            void free_items();

        private:
            MemoryPoolHead(const MemoryPoolHead &copy) = delete;

            MemoryPoolHead &operator =(const MemoryPoolHead &assign) = delete;

            mutable std::atomic<bool> locked_;

            volatile int uint64_count_;

            volatile int item_count_;

            MemoryPoolItem* volatile first_item_;
        };

        class ConstPointer;

        class Pointer
        {
        public:
            friend class ConstPointer;

            Pointer() : pointer_(nullptr), head_(nullptr), item_(nullptr), alias_(false)
            {
            }

            Pointer(MemoryPoolHead *head) : pointer_(nullptr), head_(nullptr), item_(nullptr), alias_(false)
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

            Pointer(Pointer &&move) noexcept : pointer_(move.pointer_), head_(move.head_), item_(move.item_), alias_(move.alias_)
            {
                move.pointer_ = nullptr;
                move.head_ = nullptr;
                move.item_ = nullptr;
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
                pointer_ = nullptr;
                head_ = nullptr;
                item_ = nullptr;
                alias_ = false;
            }

            void acquire(Pointer &other)
            {
                if (this == &other)
                {
                    return;
                }
                release();
                pointer_ = other.pointer_;
                head_ = other.head_;
                item_ = other.item_;
                alias_ = other.alias_;
                other.pointer_ = nullptr;
                other.head_ = nullptr;
                other.item_ = nullptr;
                other.alias_ = false;
            }

            void swap_with(Pointer &other) noexcept
            {
                std::swap(pointer_, other.pointer_);
                std::swap(head_, other.head_);
                std::swap(item_, other.item_);
                std::swap(alias_, other.alias_);
            }

            ~Pointer()
            {
                release();
            }

            static Pointer Owning(std::uint64_t *pointer)
            {
                return Pointer(pointer, false);
            }

            static Pointer Aliasing(std::uint64_t *pointer)
            {
                return Pointer(pointer, true);
            }

        private:
            Pointer(std::uint64_t *pointer, bool alias) : pointer_(pointer), head_(nullptr), item_(nullptr), alias_(alias)
            {
            }

            Pointer(const Pointer &copy) = delete;

            Pointer &operator =(const Pointer &assign) = delete;

            std::uint64_t *pointer_;

            MemoryPoolHead *head_;

            MemoryPoolItem *item_;

            bool alias_;
        };

        class ConstPointer
        {
        public:
            ConstPointer() : pointer_(nullptr), head_(nullptr), item_(nullptr), alias_(false)
            {
            }

            ConstPointer(MemoryPoolHead *head) : pointer_(nullptr), head_(nullptr), item_(nullptr), alias_(false)
            {
#ifdef _DEBUG
                if (head == nullptr)
                {
                    throw std::invalid_argument("head");
                }
#endif
                pointer_ = item_->pointer();
                head_ = head;
                item_ = head->get();
            }

            ConstPointer(ConstPointer &&move) noexcept : pointer_(move.pointer_), head_(move.head_), item_(move.item_), alias_(move.alias_)
            {
                move.pointer_ = nullptr;
                move.head_ = nullptr;
                move.item_ = nullptr;
                move.alias_ = false;
            }

            ConstPointer(Pointer &&move) noexcept : pointer_(move.pointer_), head_(move.head_), item_(move.item_), alias_(move.alias_)
            {
                move.pointer_ = nullptr;
                move.head_ = nullptr;
                move.item_ = nullptr;
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
                pointer_ = nullptr;
                head_ = nullptr;
                item_ = nullptr;
                alias_ = false;
            }

            void acquire(ConstPointer &other)
            {
                if (this == &other)
                {
                    return;
                }
                release();
                pointer_ = other.pointer_;
                head_ = other.head_;
                item_ = other.item_;
                alias_ = other.alias_;
                other.pointer_ = nullptr;
                other.head_ = nullptr;
                other.item_ = nullptr;
                other.alias_ = false;
            }

            void acquire(Pointer &other)
            {
                release();
                pointer_ = other.pointer_;
                head_ = other.head_;
                item_ = other.item_;
                alias_ = other.alias_;
                other.pointer_ = nullptr;
                other.head_ = nullptr;
                other.item_ = nullptr;
                other.alias_ = false;
            }

            void swap_with(ConstPointer &other) noexcept
            {
                std::swap(pointer_, other.pointer_);
                std::swap(head_, other.head_);
                std::swap(item_, other.item_);
                std::swap(alias_, other.alias_);
            }

            ~ConstPointer()
            {
                release();
            }

            static ConstPointer Owning(std::uint64_t *pointer)
            {
                return ConstPointer(pointer, false);
            }

            static ConstPointer Aliasing(const std::uint64_t *pointer)
            {
                return ConstPointer(const_cast<uint64_t*>(pointer), true);
            }

        private:
            ConstPointer(std::uint64_t *pointer, bool alias) : pointer_(pointer), head_(nullptr), item_(nullptr), alias_(alias)
            {
            }

            ConstPointer(const ConstPointer &copy) = delete;

            ConstPointer &operator =(const ConstPointer &assign) = delete;

            std::uint64_t *pointer_;

            MemoryPoolHead *head_;

            MemoryPoolItem *item_;

            bool alias_;
        };

        class MemoryPool
        {
        public:
            MemoryPool() 
            {
            }

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
                ReaderLock lock = pools_locker_.acquire_read();
                return static_cast<int>(pools_.size());
            }

            int total_byte_count() const;

            void free_all();

            ~MemoryPool()
            {
                free_all();
            }

            static std::shared_ptr<MemoryPool> default_pool()
            {
                static std::shared_ptr<MemoryPool> default_pool_(std::make_shared<MemoryPool>());

                return default_pool_;
            }

        private:
            MemoryPool(const MemoryPool &copy) = delete;

            MemoryPool &operator =(const MemoryPool &assign) = delete;

            mutable ReaderWriterLocker pools_locker_;

            std::vector<MemoryPoolHead*> pools_;
        };

        Pointer duplicate_if_needed(std::uint64_t *original, int uint64_count, bool condition, MemoryPool &pool);

        ConstPointer duplicate_if_needed(const std::uint64_t *original, int uint64_count, bool condition, MemoryPool &pool);
    }
}
