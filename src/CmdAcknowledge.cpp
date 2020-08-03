//Filename:  CmdAcknowledge.cpp

#include "CmdAcknowledge.h"
#include <sstream>
#include <array>

void CmdAcknowledge::execute()
{
    if (this->commandId != COMMAND_ID)
    {
        debug_print(this, "Command ", this->commandId, " is not for me");
        return;
    }

    debug_print(this, "Processing ", COMMAND_ID, " command\n");


    auto success {0};
    auto messageId {0};
    std::array<char, 100> dummy;
    std::istringstream sin(this->line);

    sin >> dummy.data() >> messageId >> success;

    if (sin.fail())
    {
        error_return(this, "Invalid ACK command; state=",
                name_statebits(sin.rdstate()));
    }

    AcknowledgeQueue::TheOne(messageId)->add(1 == success);
}
