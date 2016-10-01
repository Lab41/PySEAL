#include "CppUnitTest.h"
#include "util/locks.h"
#include <thread>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal::util;
using namespace std;

namespace SEALTest
{
    namespace util
    {
        TEST_CLASS(ReaderWriterLockerTests)
        {
        public:
            TEST_METHOD(ReaderWriterLockNonBlocking)
            {
                ReaderWriterLocker locker;

                WriterLock writeLock = locker.acquire_write();
                Assert::IsTrue(writeLock.is_acquired());
                writeLock.release();
                Assert::IsFalse(writeLock.is_acquired());

                ReaderLock readLock = locker.acquire_read();
                Assert::IsTrue(readLock.is_acquired());
                readLock.release();

                ReaderLock readLock2 = locker.acquire_read();
                Assert::IsTrue(readLock2.is_acquired());
                readLock.release();
                Assert::IsFalse(readLock.is_acquired());
                Assert::IsTrue(readLock2.is_acquired());
                readLock2.release();
                Assert::IsFalse(readLock2.is_acquired());

                Assert::IsTrue(readLock.try_acquire(locker));
                Assert::IsFalse(writeLock.try_acquire(locker));

                Assert::IsTrue(readLock2.try_acquire(locker));
                Assert::IsFalse(writeLock.try_acquire(locker));

                readLock.release();
                Assert::IsFalse(writeLock.try_acquire(locker));

                readLock2.release();
                Assert::IsTrue(writeLock.try_acquire(locker));

                WriterLock writeLock2;
                Assert::IsFalse(writeLock2.try_acquire(locker));
                Assert::IsFalse(readLock2.try_acquire(locker));

                writeLock.release();

                Assert::IsTrue(writeLock2.try_acquire(locker));
                Assert::IsFalse(readLock2.try_acquire(locker));

                writeLock2.release();
            }

            TEST_METHOD(ReaderWriterLockBlocking)
            {
                ReaderWriterLocker locker;

                Reader *reader1 = new Reader(locker);
                Reader *reader2 = new Reader(locker);
                Writer *writer1 = new Writer(locker);
                Writer *writer2 = new Writer(locker);

                Assert::IsFalse(reader1->is_locked());
                Assert::IsFalse(reader2->is_locked());
                Assert::IsFalse(writer1->is_locked());
                Assert::IsFalse(writer2->is_locked());

                reader1->acquire_read();
                Assert::IsTrue(reader1->is_locked());
                Assert::IsFalse(reader2->is_locked());
                reader2->acquire_read();
                Assert::IsTrue(reader1->is_locked());
                Assert::IsTrue(reader2->is_locked());

                thread writer1_thread(&Writer::acquire_write, writer1);
                writer1->wait_until_trying();
                Assert::IsTrue(writer1->is_trying_to_lock());
                Assert::IsFalse(writer1->is_locked());

                reader2->release();
                Assert::IsTrue(reader1->is_locked());
                Assert::IsFalse(reader2->is_locked());
                Assert::IsTrue(writer1->is_trying_to_lock());
                Assert::IsFalse(writer1->is_locked());

                thread writer2_thread(&Writer::acquire_write, writer2);
                writer2->wait_until_trying();
                Assert::IsTrue(writer1->is_trying_to_lock());
                Assert::IsFalse(writer1->is_locked());
                Assert::IsTrue(writer2->is_trying_to_lock());
                Assert::IsFalse(writer2->is_locked());

                reader1->release();
                Assert::IsFalse(reader1->is_locked());

                while (writer1->is_trying_to_lock() && writer2->is_trying_to_lock());

                Writer *winner;
                Writer *waiting;
                if (writer1->is_locked())
                {
                    winner = writer1;
                    waiting = writer2;
                }
                else
                {
                    winner = writer2;
                    waiting = writer1;
                }
                Assert::IsTrue(winner->is_locked());
                Assert::IsFalse(waiting->is_locked());

                winner->release();
                Assert::IsFalse(winner->is_locked());
                
                waiting->wait_until_locked();
                Assert::IsTrue(waiting->is_locked());

                thread reader1_thread(&Reader::acquire_read, reader1);
                reader1->wait_until_trying();
                Assert::IsTrue(reader1->is_trying_to_lock());
                Assert::IsFalse(reader1->is_locked());

                thread reader2_thread(&Reader::acquire_read, reader2);
                reader2->wait_until_trying();
                Assert::IsTrue(reader2->is_trying_to_lock());
                Assert::IsFalse(reader2->is_locked());

                waiting->release();

                reader1->wait_until_locked();
                reader2->wait_until_locked();
                Assert::IsTrue(reader1->is_locked());
                Assert::IsTrue(reader2->is_locked());

                reader1->release();
                reader2->release();

                Assert::IsFalse(reader1->is_locked());
                Assert::IsFalse(reader2->is_locked());
                Assert::IsFalse(writer1->is_locked());
                Assert::IsFalse(reader2->is_locked());
                Assert::IsFalse(reader1->is_trying_to_lock());
                Assert::IsFalse(reader2->is_trying_to_lock());
                Assert::IsFalse(writer1->is_trying_to_lock());
                Assert::IsFalse(reader2->is_trying_to_lock());

                writer1_thread.join();
                writer2_thread.join();
                reader1_thread.join();
                reader2_thread.join();

                delete reader1;
                delete reader2;
                delete writer1;
                delete writer2;
            }

            class Reader
            {
            public:
                Reader(ReaderWriterLocker &locker) : locker_(locker), locked_(false), trying_(false)
                {
                }

                bool is_locked() const
                {
                    return locked_;
                }

                bool is_trying_to_lock() const
                {
                    return trying_;
                }

                void acquire_read()
                {
                    trying_ = true;
                    lock_.acquire(locker_);
                    locked_ = true;
                    trying_ = false;
                }

                void release()
                {
                    lock_.release();
                    locked_ = false;
                }

                void wait_until_trying()
                {
                    while (!trying_);
                }

                void wait_until_locked()
                {
                    while (!locked_);
                }

            private:
                ReaderWriterLocker &locker_;

                ReaderLock lock_;

                volatile bool locked_;

                volatile bool trying_;
            };

            class Writer
            {
            public:
                Writer(ReaderWriterLocker &locker) : locker_(locker), locked_(false), trying_(false)
                {
                }

                bool is_locked() const
                {
                    return locked_;
                }

                bool is_trying_to_lock() const
                {
                    return trying_;
                }

                void acquire_write()
                {
                    trying_ = true;
                    lock_.acquire(locker_);
                    locked_ = true;
                    trying_ = false;
                }

                void release()
                {
                    lock_.release();
                    locked_ = false;
                }

                void wait_until_trying()
                {
                    while (!trying_);
                }

                void wait_until_locked()
                {
                    while (!locked_);
                }

            private:
                ReaderWriterLocker &locker_;

                WriterLock lock_;

                volatile bool locked_;

                volatile bool trying_;
            };
        };
    }
}
