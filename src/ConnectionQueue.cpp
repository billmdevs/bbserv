//Filename:  ConnectionQueue.cpp

#include "ConnectionQueue.h"
#include "AutoLock.h"


void ConnectionQueue::add(int clientSocket) noexcept
{
    AutoLock guard(&queueMutex);

    this->connectionQueue.emplace(clientSocket);

    guard.unlock();
    pthread_cond_signal(&queueCondition);
}

void ConnectionQueue::add(BroadcastCommand& broadcast) noexcept
{
    AutoLock guard(&queueMutex);

    this->connectionQueue.emplace(broadcast);

    guard.unlock();
    pthread_cond_signal(&queueCondition);
}

ConnectionQueue::Entry_t ConnectionQueue::get() noexcept
{
    AutoLock guard(&queueMutex);

    while (this->connectionQueue.empty())
    {
        pthread_cond_wait(&queueCondition, &queueMutex);
    }

    auto entry { this->connectionQueue.front() };
    this->connectionQueue.pop();
    return entry;
}

