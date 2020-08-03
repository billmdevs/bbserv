//Filename:  CmdWrite.h

#pragma once

#include <string>
#include "SessionResources.h"
#include "Config.h"
#include "ConnectionQueue.h"

/**
 *Command supporting the WRITE message.
 */
class CmdWrite
{
    protected:
        constexpr static const char* const  COMMAND_ID { "WRITE" };
        std::string commandId;
        FILE* stream {nullptr};
        const char* line {nullptr};
        std::string user;
        ConnectionQueue* connectionQueue {nullptr};

    public:
        /**
         *Custom constructor accepting the command line received on the socket.
         */
        CmdWrite(const std::string& commandId, FILE* stream, const char* line, std::string& user, ConnectionQueue* qu)
            : commandId(commandId)
            , stream(stream)
            , line(line)
            , user(user)
            , connectionQueue(qu)
        { }

    public:
        /**
         *Process the given command.
         */
        void execute();

    protected:
        /**
         * Retrieve the next free message number and update the storage immediately.
         */
        size_t update_message_number();

        /**
         *Broadcast this WRITE command to known peers.
         */
        bool replicate_command(std::string userName, size_t messageId);

        /**
         *Broadcast one command and wait for the acknowledge.
         */
        void broadcast_synchronous(std::string commandId, std::string userName, size_t messageId, std::string arguments);
};

