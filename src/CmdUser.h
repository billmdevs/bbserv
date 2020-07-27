//Filename:  CmdUser.h

#pragma once

#include <string>
#include "SessionResources.h"

/**
 *Command supporting the USER message.
 */
class CmdUser
{
    protected:
        constexpr static const char* const  COMMAND_ID { "USER" };
        std::string commandId;
        FILE* stream {nullptr};
        const char* line {nullptr};
        std::string& user;

    public:
        /**
         *Custom constructor accepting the command line received on the socket.
         */
        CmdUser(const std::string& commandId, FILE* stream, const char* line, std::string& user)
            : commandId(commandId)
            , stream(stream)
            , line(line)
            , user(user)
        { }

    public:
        /**
         *Process the given command.
         */
        void execute();

};

