//Filename:  AcknowledgeQueue.cpp

#include "AcknowledgeQueue.h"
#include "AutoLock.h"
#include <cstring>
#include <map>
#include <memory>
#include <pthread.h>
#include <ctime>

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
    // Locked region
    {
        AutoLock guard(&queueMutex);
        this->ackQueue.emplace_back(success);
        pthread_cond_signal(&queueCondition);
    }

    debug_print(this, "Acknowledge signalled ", success);
}

bool AcknowledgeQueue::check_success(size_t replyCount) noexcept
{
    timespec tpoint;
    std::memset(&tpoint, 0, sizeof(timespec));
    clock_gettime(CLOCK_REALTIME, &tpoint);
    tpoint.tv_sec += Config::singleton().get_network_timeout_ms() / 1000;

    // Locked region
    {
        AutoLock guard(&queueMutex);
        debug_print(this, "Acknowledge check for ", replyCount, " replies");

        while (this->ackQueue.size() < replyCount)
        {
            debug_print(this, "Acknowledges pending: ", this->ackQueue.size(),
                    " < ", replyCount);

            //auto rc { pthread_cond_wait(&queueCondition, &queueMutex) };
            auto rc { pthread_cond_timedwait(&queueCondition, &queueMutex, &tpoint) };

            debug_print(this, "Acknowledge received ...");
            if (0 != rc)
            {
                debug_print(this, "Failed to wait for Acknowledge: ", strerror(rc));
                break;
            }
        }
    }

    if (this->ackQueue.size() < replyCount)
    {
        debug_print(this, "Acknowledge check timed out");
        return false;
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
