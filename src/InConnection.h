// InConnection.h
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <memory>
#include <utility>
#include <string_view>
#include "ConnectionQueue.h"

#pragma once

/**
 *RAII wrapper for the socket.
 */
class SocketResource
{
    private:
        int acceptSocket {0};

    public:
        SocketResource(int domain, int type, int protocol)
        {
            this->acceptSocket = socket(domain, type, protocol);
            //debug_print(this, "!!! Set socket ", this->acceptSocket, " @", this);

            if (-1 == this->acceptSocket)
            {
                error_return(this, "Failed to connect to socket for incoming connections: ",
                        strerror(errno));
            }

            auto enable {1};
            setsockopt(this->get_accept_socket(), SOL_SOCKET, SO_REUSEADDR,
                    (void*)&enable, sizeof(enable));
        }

        SocketResource()
            : acceptSocket(0)
        { }

        ~SocketResource()
        {
            if (0 < this->acceptSocket)
            {
                debug_print(this, "Close connection at socket ",
                        this->acceptSocket, " @", this);
                close(this->acceptSocket);
                this->acceptSocket = 0;
            }
        }

        SocketResource(const SocketResource& other)
            : acceptSocket(other.acceptSocket)
        { }

        SocketResource(SocketResource&& other) noexcept
            : acceptSocket(std::exchange(other.acceptSocket, 0))
            { }

        SocketResource& operator=(const SocketResource& other)
        {
            return *this = SocketResource(other);
        }

        SocketResource& operator=(SocketResource&& other) noexcept
        {
            std::swap(this->acceptSocket, other.acceptSocket);
            return *this;
        }

    public:
        int get_accept_socket()
        {
            auto p {reinterpret_cast<void*>(this)};

            if (!p)
            {
                return -1;
            }
            return this->acceptSocket;
        }

};

/**
 *Network connection listener.
 */
class InConnection
{
    private:
        static constexpr auto StopId { 0xFFFE };

    protected:
        std::unique_ptr<SocketResource> resources;
        std::shared_ptr<ConnectionQueue> connectionQueue;
        bool isNonblocking {false};
        bool run {true};

    public:
        /**
         * Creates and listens on a socket bound to the given port.
         *
         * Upon error might throw BBServException.
         */
        void listen_on(/*const std::string_view& ipaddress,*/ in_port_t port);

        /**
         *Start working in your own thread context.
         *
         * Upon error might throw BBServException.
         */
        void operate(/*const std::string_view& ipaddress,*/ in_port_t port,
                std::shared_ptr<ConnectionQueue>& qu, bool isNonblocking = false);

        /**
         *Let the inherent thread stop gracefully.
         */
        void stop();

    protected:
        void open_incoming_conn(/*const std::string_view& ipaddress,*/ in_port_t port);
        void listen_for_clients();
        bool is_nonblocking();
};
