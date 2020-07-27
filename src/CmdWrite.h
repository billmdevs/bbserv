//Filename:  CmdWrite.h

#pragma once

#include <string>
#include "SessionResources.h"
#include "Config.h"

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

    public:
        /**
         *Custom constructor accepting the command line received on the socket.
         */
        CmdWrite(const std::string& commandId, FILE* stream, const char* line, std::string& user)
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

    protected:
        /**
         *Retrive the next free message number and update the storage imediately.
         */
        size_t update_message_number();
};

