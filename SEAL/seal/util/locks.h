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

            inline bool is_acquired() const
            {
                return locker_ != nullptr;
            }

            void acquire(ReaderWriterLocker &locker);

            bool try_acquire(ReaderWriterLocker &locker);

            void release();

            inline void swap_with(ReaderLock &lock)
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

            inline bool is_acquired() const
            {
                return locker_ != nullptr;
            }

            void acquire(ReaderWriterLocker &locker);

            bool try_acquire(ReaderWriterLocker &locker);

            void release();

            inline void swap_with(WriterLock &lock)
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

            inline ReaderLock acquire_read()
            {
                return ReaderLock(*this);
            }

            inline WriterLock acquire_write()
            {
                return WriterLock(*this);
            }

        private:
            ReaderWriterLocker(const ReaderWriterLocker &copy) = delete;

            ReaderWriterLocker &operator =(const ReaderWriterLocker &assign) = delete;

            std::atomic<int> reader_locks_;

            std::atomic<bool> writer_locked_;
        };

        inline void ReaderLock::release()
        {
            if (locker_ == nullptr)
            {
                return;
            }
            locker_->reader_locks_.fetch_sub(1, std::memory_order_release);
            locker_ = nullptr;
        }

        inline void ReaderLock::acquire(ReaderWriterLocker &locker)
        {
            release();
            do
            {
                locker.reader_locks_.fetch_add(1, std::memory_order_acquire);
                locker_ = &locker;
                if (locker.writer_locked_.load(std::memory_order_acquire))
                {
                    release();
                    while (locker.writer_locked_.load(std::memory_order_acquire));
                }
            } while (locker_ == nullptr);
        }

        inline bool ReaderLock::try_acquire(ReaderWriterLocker &locker)
        {
            release();
            locker.reader_locks_.fetch_add(1, std::memory_order_acquire);
            locker_ = &locker;
            if (locker.writer_locked_.load(std::memory_order_acquire))
            {
                release();
                return false;
            }
            return true;
        }

        inline void WriterLock::acquire(ReaderWriterLocker &locker)
        {
            release();
            bool expected = false;
            while (!locker.writer_locked_.compare_exchange_strong(expected, true, std::memory_order_acquire))
            {
                expected = false;
            }
            locker_ = &locker;
            while (locker.reader_locks_.load(std::memory_order_acquire) != 0);
        }

        inline bool WriterLock::try_acquire(ReaderWriterLocker &locker)
        {
            release();
            bool expected = false;
            if (!locker.writer_locked_.compare_exchange_strong(expected, true, std::memory_order_acquire))
            {
                return false;
            }
            locker_ = &locker;
            if (locker.reader_locks_.load(std::memory_order_acquire) != 0)
            {
                release();
                return false;
            }
            return true;
        }

        inline void WriterLock::release()
        {
            if (locker_ == nullptr)
            {
                return;
            }
            locker_->writer_locked_.store(false, std::memory_order_release);
            locker_ = nullptr;
        }
    }
}