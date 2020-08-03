//Filename:  ConnectionQueue.h

#pragma once

#include <pthread.h>
#include <queue>
#include <optional>
#include <string>
#include <variant>
#include "Config.h"

/**
 *The command to be sent to a peer.
 */
struct BroadcastCommand
{
    /**
     *The complete command message to be sent.
     */
    std::string command;
    /**
     *The peer that this command is to be sent.
     */
    Peer peer;
};

/**
 *This queue serves incoming client connections to the preallocated threads.
 */
class ConnectionQueue
{
    public:
        using Entry_t = std::variant<int, BroadcastCommand>;

    protected:
        std::queue<Entry_t> connectionQueue;

    protected:
        pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;
        pthread_cond_t queueCondition = PTHREAD_COND_INITIALIZER;

    public:
        /**
         * Add the socket of an incoming connection to this queue.
         *
         * This funciton is thread-safe and may be used from all the agnets.
         */
        void add(int clientSocket) noexcept;

        /**
         * Add the broadcast command sent to the specified peer.
         *
         * This funciton is thread-safe and may be used from all the agnets.
         */
        void add(BroadcastCommand& broadcast) noexcept;

        /**
         * Get the socket of an incoming connection in a blocking manner.
         *
         * This funciton is thread-safe and may be used from all the agnets.
         */
        Entry_t get() noexcept;

};
