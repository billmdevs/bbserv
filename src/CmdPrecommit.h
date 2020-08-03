//Filename:  CmdPrecommit.h

#pragma once

#include <string>
#include "SessionResources.h"
#include "AcknowledgeQueue.h"

/**
 *Command supporting the USER message.
 */
class CmdPrecommit
{
    protected:
        constexpr static const char* const  COMMAND_ID { "PRECOMMIT" };
        std::string commandId;
        FILE* stream {nullptr};
        const char* line {nullptr};

    public:
        /**
         *Custom constructor accepting the command line received on the socket.
         */
        CmdPrecommit(const std::string& commandId, FILE* stream, const char* line)
            : commandId(commandId)
            , stream(stream)
            , line(line)
        { }

    public:
        /**
         *Process the given command.
         */
        void execute();

};

