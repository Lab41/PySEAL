#pragma once

#include <atomic>
#include <utility>

namespace seal
{
    namespace util
    {
        class ReaderWriterLocker;

        class ReaderLock
        {
        public:
            ReaderLock() : locker_(nullptr)
            {
            }

            ReaderLock(ReaderLock &&move) noexcept : locker_(move.locker_)
            {
                move.locker_ = nullptr;
            }

            ReaderLock(ReaderWriterLocker &locker) : locker_(nullptr)
            {
                acquire(locker);
            }

            ~ReaderLock()
            {
                release();
            }

            bool is_acquired() const
            {
                return locker_ != nullptr;
            }

            void acquire(ReaderWriterLocker &locker);

            bool try_acquire(ReaderWriterLocker &locker);

            void release();

            void swap_with(ReaderLock &lock)
            {
                std::swap(locker_, lock.locker_);
            }

        private:
            ReaderWriterLocker *locker_;
        };

        class WriterLock
        {
        public:
            WriterLock() : locker_(nullptr)
            {
            }

            WriterLock(WriterLock &&move) noexcept : locker_(move.locker_)
            {
                move.locker_ = nullptr;
            }

            WriterLock(ReaderWriterLocker &locker) : locker_(nullptr)
            {
                acquire(locker);
            }

            ~WriterLock()
            {
                release();
            }

            bool is_acquired() const
            {
                return locker_ != nullptr;
            }

            void acquire(ReaderWriterLocker &locker);

            bool try_acquire(ReaderWriterLocker &locker);

            void release();

            void swap_with(WriterLock &lock)
            {
                std::swap(locker_, lock.locker_);
            }

        private:
            ReaderWriterLocker *locker_;
        };

        class ReaderWriterLocker
        {
            friend class ReaderLock;

            friend class WriterLock;

        public:
            ReaderWriterLocker() : reader_locks_(0), writer_locked_(false)
            {
            }

            ReaderLock acquire_read()
            {
                return ReaderLock(*this);
            }

            WriterLock acquire_write()
            {
                return WriterLock(*this);
            }

        private:
            ReaderWriterLocker(const ReaderWriterLocker &copy) = delete;

            ReaderWriterLocker &operator =(const ReaderWriterLocker &assign) = delete;

            std::atomic<int> reader_locks_;

            std::atomic<bool> writer_locked_;
        };
    }
}