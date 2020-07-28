//Filename:  SessionResources.h

#pragma once

#include <string>
#include <cstdio>
#include <utility>
#include "Config.h"
#include "BBServException.h"

/**
 *RAII class controling the lifetime of resources, that belong to a connection to one client.
 */
class SessionResources
{
    protected:
        std::string user { "nobody" };
        int clientSocket { 0 };
        FILE* stream { nullptr };

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
        { }

        /**
         *Move constructor.
         */
        SessionResources(SessionResources&& other) noexcept
            : user(std::exchange(other.user, ""))
            , clientSocket(std::exchange(other.clientSocket, 0))
            , stream(std::exchange(other.stream, nullptr))
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

};
