 //Filename:  AutoLock.h
 
#pragma once

#include <pthread.h>

/**
 *RAII wrapper for mutexes.
 */
class AutoLock
{
    protected:
        pthread_mutex_t* guard {nullptr};

    public:
        /**
         *Lock the given mutex.
         */
        AutoLock(pthread_mutex_t* guard)
            : guard(guard)
        {
            pthread_mutex_lock(guard);
        }

        /**
         *Unlock the mutex again.
         */
        ~AutoLock()
        {
            this->unlock();
        }

        /**
         *Unlock the mutex again.
         */
        void unlock()
        {
            if (this->guard){
                pthread_mutex_unlock(this->guard);
                this->guard = nullptr;
            }
        }
};
