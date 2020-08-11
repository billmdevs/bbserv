//Filename:  RWLock.h

#pragma once

#include <memory>
#include <pthread.h>

/**
 *Reader/Writer lock allowing one single writer or multiple readers at a time.
 */
class RWLock
{
    protected:
        size_t readerCount {0};
        size_t writerCount {0};
        size_t rWaitCount {0};
        size_t wWaitCount {0};

        pthread_mutex_t conditionLock = PTHREAD_MUTEX_INITIALIZER;
        pthread_cond_t readFlag = PTHREAD_COND_INITIALIZER;
        pthread_cond_t writeFlag = PTHREAD_COND_INITIALIZER;

    public:
        /**
         *Wait for read-access.
         */
        void aquire_read();

        /**
         *Trigger others after we have finished reading.
         */
        void release_read();

        /**
         *Wait for write-access.
         */
        void aquire_write();

        /**
         *Trigger others after we have finished writing.
         */
        void release_write();

};

inline RWLock globalRWLock;

class ReadLock
{
        RWLock* rwLock {nullptr};
    public:
        ReadLock(RWLock* rwLock) : rwLock(rwLock) {}
        void lock()   { this->rwLock->aquire_read(); }
        void unlock()   { this->rwLock->release_read(); }
};

class WriteLock
{
        RWLock* rwLock {nullptr};
    public:
        WriteLock(RWLock* rwLock) : rwLock(rwLock) {}
        void lock()   { this->rwLock->aquire_write(); }
        void unlock()   { this->rwLock->release_write(); }
};

template<class T>
class RWAutoLock
{
    protected:
        RWLock* rwLock {nullptr};
        std::unique_ptr<T> role;

    public:
        RWAutoLock(RWLock* rwLock)
            : rwLock(rwLock)
            , role(std::make_unique<T>(rwLock))
        {
            role->lock();
        }

        ~RWAutoLock()
        {
            this->unlock();
        }

        void unlock()
        {
            if (this->rwLock)
            {
                this->role->unlock();
                this->rwLock = nullptr;
                this->role.reset();
            }
        }
};

