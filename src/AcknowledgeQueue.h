//Filename:  AcknowledgeQueue.h

#pragma once

#include <pthread.h>
#include <vector>
#include <optional>
#include "Config.h"

/**
 *This queue serves reply messages from broadcasts sent out to peers.
 */
class AcknowledgeQueue
{
    public:
        /**
         *Access the singleton catalog to get/set/erase the AcknowledgeQueue instance.
         */
        static AcknowledgeQueue* TheOne(size_t messageId, bool erase = false);

    protected:
        std::vector<bool> ackQueue;

    protected:
        pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;
        pthread_cond_t queueCondition = PTHREAD_COND_INITIALIZER;

    public:
        /**
         * Add the reply of a peer to this queue.
         *
         * This function is thread-safe and may be used from all the agents.
         */
        void add(bool success) noexcept;

        /**
         * Check if all the replies are positive.
         *
         * This function is thread-safe and may be used from all the agents.
         */
        bool check_success(size_t replyCount) noexcept;

};
