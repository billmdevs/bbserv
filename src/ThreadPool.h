//Filename:  ThreadPool.h

#pragma once

#include <pthread.h>
#include <vector>
#include <memory>
#include <variant>
#include <optional>
#include "ConnectionQueue.h"
#include "BBServException.h"
#include "CmdUser.h"
#include "CmdWrite.h"
#include "CmdRead.h"
#include "CmdReplace.h"
#include "CmdQuit.h"
#include "CmdPrecommit.h"
#include "CmdCommit.h"
#include "CmdAcknowledge.h"
#include "CmdSuccessful.h"
#include "CmdUnsuccessful.h"
#include "BroadcastPrecommit.h"
#include "BroadcastCommit.h"

/**
 *A container of agents operating on client requests.
 */
class ThreadPool
{
    public:
        using Commands_t = std::variant<CmdUser, CmdWrite, CmdRead, CmdReplace, CmdQuit,
              CmdPrecommit, CmdCommit, CmdAcknowledge, CmdSuccessful, CmdUnsuccessful,
              BroadcastPrecommit, BroadcastCommit>;

    protected:
        size_t size {1};
        std::vector<pthread_t> pool;
        std::shared_ptr<ConnectionQueue> connectionQueue;

    public:
        /**
         *Create a new container with a fixed number of agents.
         */
        ThreadPool(size_t size)
            : size(size)
        {
            this->pool.resize(size);
        }

    public:
        /**
         *Start processing all incoming client requests.
         *Upon error this throws BBServException.
         */
        void operate(std::shared_ptr<ConnectionQueue>& qu);

        /**
         *Wait for the next incoming client connection and return the socket.
         */
        ConnectionQueue::Entry_t get_entry() noexcept;

        /**
         *Get the ConnectionQueue instance this thread pool is operating on.
         */
        ConnectionQueue* get_connection_queue() noexcept;

};
