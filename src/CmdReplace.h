//Filename:  CmdReplace.h

#pragma once

#include <string>
#include <string_view>
#include "SessionResources.h"
#include "Config.h"
#include "ConnectionQueue.h"

/**
 *Command supporting the REPLACE message.
 */
class CmdReplace
{
    protected:
        constexpr static const char* const  COMMAND_ID { "REPLACE" };
        std::string commandId;
        FILE* stream {nullptr};
        const char* line {nullptr};
        std::string user;
        ConnectionQueue* connectionQueue {nullptr};

    public:
        /**
         *Custom constructor accepting the command line received on the socket.
         */
        CmdReplace(const std::string& commandId, FILE* stream, const char* line, std::string& user, ConnectionQueue* qu)
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

        /**
         *Revert this replace operation.
         */
        void undo();

        /**
         *Get the command identifier.
         */
        std::string_view get_command_id() { return this->commandId; }

        /**
         *Get this command's definition.
         */
        std::string_view get_line() { return this->line; }

        /**
         *Get the connect queue for broadcast commands.
         */
        ConnectionQueue* get_connection_queue() { return this->connectionQueue; }

    private:
        void rewrite_bbfile(size_t id, std::string_view message);

};

