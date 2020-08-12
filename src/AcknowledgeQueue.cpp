//Filename:  AcknowledgeQueue.cpp

#include "AcknowledgeQueue.h"
#include "AutoLock.h"
#include <map>
#include <memory>
#include <pthread.h>

pthread_mutex_t TheOneMapGuard = PTHREAD_MUTEX_INITIALIZER;

AcknowledgeQueue* AcknowledgeQueue::TheOne(size_t messageId, bool erase)
{
    static std::map<size_t, std::unique_ptr<AcknowledgeQueue>> theMap;

    AutoLock guard (&TheOneMapGuard);

    if (!erase)
    {
        if (theMap.find(messageId) == theMap.end())
        {
            theMap.insert(std::make_pair(messageId, std::make_unique<AcknowledgeQueue>()));
        }
        return theMap[messageId].get();
    }
    else
    {
        theMap.erase(theMap.find(messageId));
    }

    return nullptr;
}

void AcknowledgeQueue::add(bool success) noexcept
{
    AutoLock guard(&queueMutex);
    //debug_print(this, "Acknowledge positive=", success);

    this->ackQueue.emplace_back(success);

    guard.unlock();
    pthread_cond_signal(&queueCondition);
    //debug_print(this, "Acknowledge signalled ", success);
}

bool AcknowledgeQueue::check_success(size_t replyCount) noexcept
{
    AutoLock guard(&queueMutex);
    debug_print(this, "Acknowledge check for ", replyCount, " replies");

    while (this->ackQueue.size() < replyCount)
    {
        pthread_cond_wait(&queueCondition, &queueMutex);
    }

    // Determine if there is at least one negative reply
    for (auto success : this->ackQueue)
    {
        if (!success)
        {
            debug_print(this, "Acknowledge check NOK");
            return false;
        }
    }
    debug_print(this, "Acknowledge check OK");
    return true;
}
