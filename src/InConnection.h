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

class InConnection
{
    protected:
        int acceptSocket {0};
        std::shared_ptr<ConnectionQueue> connectionQueue;

    public:
        InConnection(std::shared_ptr<ConnectionQueue>& qu);
        ~InConnection();

    public:
        /**
         * Creates and listens on a socket bound to the given port.
         * Upon error might throw BBServException.
         */
        void listen_on(/*const std::string_view& ipaddress,*/ in_port_t port);

    protected:
        void open_incoming_conn(/*const std::string_view& ipaddress,*/ in_port_t port);
        void listen_for_clients();
};
