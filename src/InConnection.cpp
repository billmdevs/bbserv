#include "InConnection.h"
#include <cstring>
#include <memory>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <pthread.h>
#include "Config.h"
#include "AcknowledgeQueue.h"

/**
 *Data passed on to the thread/context.
 */
struct ContextData
{
    ContextData(InConnection* self, in_port_t port)
        : self(self)
        , port(port)
    {};

    InConnection* self {nullptr};
    in_port_t port {0};
};

/**
 *The thread's main entry point.
 */
static void* thread_main(void* p)
{
    auto contextData { reinterpret_cast<ContextData*>(p) };
    auto port { contextData->port };
    auto self { contextData->self };
    delete contextData;

    self->listen_on(port);

    return nullptr;
}

void InConnection::listen_on(/*const std::string_view& ipaddress,*/ in_port_t port)
{
    open_incoming_conn(/*ipaddress,*/ port);
    listen_for_clients();
}

void InConnection::operate(/*const std::string_view& ipaddress,*/ in_port_t port,
        std::shared_ptr<ConnectionQueue>& qu, bool isNonblocking)
{
    this->connectionQueue = qu;
    this->isNonblocking = isNonblocking;

    pthread_t context;
    pthread_attr_t contextOptions;
    pthread_attr_init(&contextOptions);
    pthread_attr_setdetachstate(&contextOptions, PTHREAD_CREATE_DETACHED);

    auto contextData = new ContextData(this, port);

    bool success { 0 == pthread_create(&context, &contextOptions,
            thread_main, reinterpret_cast<void*>(contextData))};

    if (!success)
    {
        error_return(this, "Failed to create context: ", strerror(errno));
    }

    pthread_attr_destroy(&contextOptions);
    debug_print(this, "Context created and ready");
}

/**
 *Create and bind a socket to any of this host's network interfaces.
 */
void InConnection::open_incoming_conn(/*const std::string_view& ipaddress,*/ in_port_t port)
{
    using std::string_view;

    addrinfo hints, *result;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    auto portId { std::to_string(port) };

    auto status { getaddrinfo(nullptr, portId.data(), &hints, &result) };
    if (0 != status)
    {
        error_return(this, "Failed to determine address to bind socket to: ",
                gai_strerror(status));
    }

    for (auto* it{result}; it != nullptr; it = it->ai_next)
    {
        try
        {
            resources = std::make_unique<SocketResource> (it->ai_family, it->ai_socktype,
                        it->ai_protocol);
        }
        catch (const BBServException& error)
        {
            std::cout << error.what() << std::endl;
            continue;
        }

        if (this->is_nonblocking())
        {
            fcntl(resources->get_accept_socket(), F_SETFL, O_NONBLOCK);
        }

        if (0 != bind(resources->get_accept_socket(), it->ai_addr,
                    it->ai_addrlen))
        {
            debug_print(this, "Failed to bind socket to ",
                    inet_ntoa(((sockaddr_in*)result->ai_addr)->sin_addr), ":",
                    port, " - ", strerror(errno));
            continue;
        }

        // All went well with this it's address, so let's take it
        debug_print(this, "Created socket ", resources->get_accept_socket(),
                " and bound it to ",
                inet_ntoa(((sockaddr_in*)result->ai_addr)->sin_addr), ":",
                port);
        break;
    }

    freeaddrinfo(result);
}

/**
 * Listen at the socket, accept incoming connections and delegate them to a worker thread.
 *
 * In case this instance is operating on a socket in non-blocking mode, it
 * might throw BBServTimeout.
 */
void InConnection::listen_for_clients()
{
    auto clientSocket {0};

    listen(resources->get_accept_socket(), Config::singleton().get_Tmax());
    debug_print(this, "Listening for incoming messages");

    for (;this->run;)
    {
        // Wait a limited ammount of time for incoming connections if the
        // socket is in non-blocking mode.
        if (this->is_nonblocking())
        {
            pollfd descriptor;
            descriptor.fd = resources->get_accept_socket();
            descriptor.events = POLLIN;

            debug_print(this, "Waiting for data to arrive on socket ",
                    resources->get_accept_socket());

            auto ready { poll(&descriptor, 1,
                    Config::singleton().get_network_timeout_ms()) };

            if (0 == ready)
            {
                // timeout
                //timeout_return(this, "Timeout at waiting for incoming network connection");

                //just continue to keep on listening for data replication
                continue;
            }
            else if (-1 == ready)
            {
                // error
                error_return(this, "Failed to poll socket ", resources->get_accept_socket());
            }
        }

        debug_print(this, "Accepting connection on socket ", resources->get_accept_socket());
        clientSocket = accept(resources->get_accept_socket(), NULL, NULL);
        if (-1 == clientSocket)
        {
            debug_print(this, "Failed to accept connection on socket ",
                    resources->get_accept_socket(), ": ", strerror(errno));
            break;
        }

        debug_print(this, "Accepted client connection on ", clientSocket);
        this->connectionQueue->add(clientSocket);

        //// Don't keep on waiting for other connections in non-blocking mode.
        //if (this->is_nonblocking())
        //{
            //break;
        //}
    }

    debug_print(this, "Stop listening on socket ", resources->get_accept_socket());
    //close(this->acceptSocket);
    //this->acceptSocket = 0;

    AcknowledgeQueue::TheOne(StopId)->add(true);
}

bool InConnection::is_nonblocking()
{
    return this->isNonblocking;
}

void InConnection::stop()
{
    this->run = false;
    this->resources.reset();
    AcknowledgeQueue::TheOne(StopId)->check_success(1);
}
