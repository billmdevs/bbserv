//Filename:  ConnectionQueue.h

#pragma once

#include <pthread.h>
#include <queue>

/**
 *This queue serves incoming client connections to the preallocated threads.
 */
class ConnectionQueue
{
    protected:
        std::queue<int> connectionQueue;

    protected:
        pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;
        pthread_cond_t queueCondition = PTHREAD_COND_INITIALIZER;

    public:
        /**
         *Add the socket of an incoming connection to this queue.
         */
        void add(int clientSocket);

        /**
         *Get the socket of an incoming connection in a blocking manner.
         */
        int get();
};
