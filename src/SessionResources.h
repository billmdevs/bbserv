//Filename:  SessionResources.h

#pragma once

#include <string>
#include <memory>
#include <cstdio>
#include <cstring>
#include <utility>
#include <poll.h>
#include <errno.h>
#include "Config.h"
#include "BBServException.h"
#include "AcknowledgeQueue.h"

/**
 *RAII class controlling the lifetime of resources, that belong to a connection to one client.
 */
class SessionResources
{
    protected:
        std::string user { "nobody" };
        int clientSocket { 0 };
        FILE* stream { nullptr };
        //std::unique_ptr<AcknowledgeQueue> ackQueue;

    public:
        /**
         *Default constructor.
         */
        SessionResources() {}
        /**
         *Non-virtual destructor.
         */
        ~SessionResources()
        {
            if (this->stream)
            {
                debug_print(this, "Closing stream from ", fileno(this->stream), " @", this);

                fclose(this->stream);
                this->stream = nullptr;
                this->clientSocket = 0;
            }
            else if (this->clientSocket)
            {
                close(this->clientSocket);
                this->clientSocket = 0;
            }
        }

        /**
         *Copy constructor.
         */
        SessionResources(const SessionResources& other)
            : user(other.user)
            , clientSocket(other.clientSocket)
            , stream(other.stream)
            //, ackQueue(other.ackQueue.get())
        { }

        /**
         *Move constructor.
         */
        SessionResources(SessionResources&& other) noexcept
            : user(std::exchange(other.user, ""))
            , clientSocket(std::exchange(other.clientSocket, 0))
            , stream(std::exchange(other.stream, nullptr))
            //, ackQueue(std::exchange(other.ackQueue, nullptr))
        { }

        /**
         *Copy assignment.
         */
        SessionResources& operator=(const SessionResources& other)
        {
            *this = SessionResources(other);
            return *this;
        }

        /**
         *Move assignment.
         */
        SessionResources& operator=(SessionResources&& other) noexcept
        {
            user = std::exchange(other.user, "");
            clientSocket = std::exchange(other.clientSocket, 0);
            stream = std::exchange(other.stream, nullptr);
            //ackQueue = std::exchange(other.ackQueue, nullptr);
            return *this;
        }

    public:
        /**
         *Get the reference to the user name.
         */
        std::string& get_user() { return this->user; }
        /**
         *Get the reference to the socket of the client connection.
         */
        int& get_clientSocket() { return this->clientSocket; }
        /**
         *Get the reference to the stream attached to the client socket.
         */
        FILE*& get_stream() { return this->stream; }
        /**
         *Revoke ownership of the encapsulated file stream.
         */
        void detach_stream() { this->stream = nullptr; this->clientSocket = 0; }
        /**
         *Get the AcknowledgeQueue instance.
         */
        //AcknowledgeQueue* get_ack_queue() { return this->ackQueue.get(); }
        /**
         *Set the AcknowledgeQueue instance.
         */
        //void set_ack_queue(AcknowledgeQueue* p) { this->ackQueue.reset(p); }

};

/**
 *Poll on the socket.
 */
inline void wait_for(int peerSocket)
{
    pollfd descriptor;
    descriptor.fd = peerSocket;
    descriptor.events = POLLIN;

    auto ready { poll(&descriptor, 1, Config::singleton().get_network_timeout_ms()) };

    if (0 == ready)
    {
        // timeout
        timeout_return(peerSocket, "Timeout occurred on ", peerSocket);
    }
    else if (-1 == ready)
    {
        // error
        error_return(peerSocket, "Failed to poll connection on ", peerSocket, "; ",
                strerror(errno));
    }

}

