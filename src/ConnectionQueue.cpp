//Filename:  ConnectionQueue.cpp

#include "ConnectionQueue.h"
#include "AutoLock.h"

//void queue_add(queue q, void *value)
//{
    //pthread_mutex_lock(&q->mtx);

    //[> Add element normally. <]

    //pthread_mutex_unlock(&q->mtx);

    //[> Signal waiting threads. <]
    //pthread_cond_signal(&q->cond);
//}

//void queue_get(queue q, void **val_r)
//{
    //pthread_mutex_lock(&q->mtx);

    //[> Wait for element to become available. <]
    //while (empty(q))
        //rc = pthread_cond_wait(&q->cond, &q->mtx);

    //[> We have an element. Pop it normally and return it in val_r. <]

    //pthread_mutex_unlock(&q->mtx);
//}


void ConnectionQueue::add(int clientSocket)
{
    AutoLock guard(&queueMutex);

    this->connectionQueue.emplace(clientSocket);

    guard.unlock();
    pthread_cond_signal(&queueCondition);
}

int ConnectionQueue::get()
{
    AutoLock guard(&queueMutex);

    while (this->connectionQueue.empty())
    {
        pthread_cond_wait(&queueCondition, &queueMutex);
    }

    int clientSocket { this->connectionQueue.front() };
    this->connectionQueue.pop();
    return clientSocket;
}
