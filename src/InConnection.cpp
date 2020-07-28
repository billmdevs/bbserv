#include "InConnection.h"
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Config.h"

InConnection::InConnection(std::shared_ptr<ConnectionQueue>& qu)
{
    this->connectionQueue = qu;
}

InConnection::~InConnection()
{
    if (this->acceptSocket) {
       close(this->acceptSocket);
       this->acceptSocket = 0;
    }
}

void InConnection::listen_on(/*const std::string_view& ipaddress,*/ in_port_t port)
{
    open_incoming_conn(/*ipaddress,*/ port);
    listen_for_clients();
}

/**
 *Create and bind a socket to any of this host's network interfaces.
 */
void InConnection::open_incoming_conn(/*const std::string_view& ipaddress,*/ in_port_t port)
{
    using std::string_view;

    int enable = 1;
    struct sockaddr_in acceptAddress;
    socklen_t addressSize = sizeof(struct sockaddr_in);

    memset(&acceptAddress, 0, addressSize);

    this->acceptSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > this->acceptSocket)
    {
        error_return(this, "Failed to connect to socket for incoming connections.");
    }
    setsockopt(this->acceptSocket, SOL_SOCKET, SO_REUSEADDR, (void*)&enable,
            sizeof(enable));

    //if (!inet_aton(ipaddress.data(), &acceptAddress.sin_addr))
    //{
        //error_return(this, "Invalid IP-address");
    //}

    acceptAddress.sin_port = htons(port);
    acceptAddress.sin_family = AF_INET;
    acceptAddress.sin_addr.s_addr = INADDR_ANY;
    if (0 != bind(this->acceptSocket, (struct sockaddr*)(&acceptAddress),
            addressSize)) {
        //error_return(this, "Failed to bind socket to ", ipaddress);
        error_return(this, "Failed to bind socket to 0.0.0.0:", port);
    }

    debug_print(this, "Created socket and bound it to ", inet_ntoa(acceptAddress.sin_addr), ":", port);
}

/**
 *Listen at the socket, accept incoming connections and delegate them to a worker thread.
 */
void InConnection::listen_for_clients()
{
    int clientSocket = 0;
    //pthread_t clientThread;
    //pthread_attr_t clientThreadOptions;

    listen(this->acceptSocket, Config::singleton().get_Tmax());
    debug_print(this, "Listening for incoming messages");

    //pthread_attr_init(&clientThreadOptions);
    //pthread_attr_setdetachstate(&clientThreadOptions,
            //PTHREAD_CREATE_DETACHED);

    while (1) {
        //pthread_mutex_lock(&clientSocketGuard);
        clientSocket = accept(this->acceptSocket, NULL, NULL);
        if (0 > clientSocket) {
            //pthread_mutex_unlock(&clientSocketGuard);
            continue;
        }

        //if (0 != pthread_create(&clientThread, &clientThreadOptions,
                //thread_main, &clientSocket)) {
            //pthread_mutex_unlock(&clientSocketGuard);
            //success = EXIT_FAILURE;
            //break;
        //}

        debug_print(this, "Accepted client connection on ", clientSocket);
        this->connectionQueue->add(clientSocket);
    }

    close(acceptSocket);
    //pthread_attr_destroy(&clientThreadOptions);
}

