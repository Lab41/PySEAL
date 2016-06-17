#include "util/locks.h"

using namespace std;

namespace seal
{
    namespace util
    {
        void ReaderLock::acquire(ReaderWriterLocker &locker)
        {
            release();
            do
            {
                locker.reader_locks_.fetch_add(1, memory_order_acquire);
                locker_ = &locker;
                if (locker.writer_locked_.load(memory_order_acquire))
                {
                    release();
                    while (locker.writer_locked_.load(memory_order_acquire));
                }
            } while (locker_ == nullptr);
        }

        bool ReaderLock::try_acquire(ReaderWriterLocker &locker)
        {
            release();
            locker.reader_locks_.fetch_add(1, memory_order_acquire);
            locker_ = &locker;
            if (locker.writer_locked_.load(memory_order_acquire))
            {
                release();
                return false;
            }
            return true;
        }

        void ReaderLock::release()
        {
            if (locker_ == nullptr)
            {
                return;
            }
            locker_->reader_locks_.fetch_sub(1, memory_order_release);
            locker_ = nullptr;
        }

        void WriterLock::acquire(ReaderWriterLocker &locker)
        {
            release();
            bool expected = false;
            while (!locker.writer_locked_.compare_exchange_strong(expected, true, memory_order_acquire))
            {
                expected = false;
            }
            locker_ = &locker;
            while (locker.reader_locks_.load(memory_order_acquire) != 0);
        }

        bool WriterLock::try_acquire(ReaderWriterLocker &locker)
        {
            release();
            bool expected = false;
            if (!locker.writer_locked_.compare_exchange_strong(expected, true, memory_order_acquire))
            {
                return false;
            }
            locker_ = &locker;
            if (locker.reader_locks_.load(memory_order_acquire) != 0)
            {
                release();
                return false;
            }
            return true;
        }

        void WriterLock::release()
        {
            if (locker_ == nullptr)
            {
                return;
            }
            locker_->writer_locked_.store(false, memory_order_release);
            locker_ = nullptr;
        }
    }
}
