//Filename:  BroadcastPrecommit.cpp

#include "BroadcastPrecommit.h"
#include <cstring>
#include <cstdio>

void BroadcastPrecommit::execute()
{
    if (this->commandId != COMMAND_ID)
    {
        debug_print(this, "Command ", this->commandId, " is not for me");
        return;
    }

    debug_print(this, "Broadcasting ", COMMAND_ID, " command");

    //debug_print(this, "Broadcasting ", COMMAND_ID, " via socket ", fileno(this->stream));
    fprintf(this->stream, "%s\n", this->line + std::strlen("BROADCAST-"));

    auto success {0};
    auto messageId {0};
    std::array<char, 1024> buffer;
    std::array<char, 100> dummy;

    std::fgets(buffer.data(), buffer.size(), this->stream);
    std::istringstream sin(buffer.data());

    debug_print(this, "Processing ACK reply: ", buffer.data());

    sin >> dummy.data() >> messageId >> success;

    if (sin.fail())
    {
        error_return(this, "Invalid ACK command; state=",
                name_statebits(sin.rdstate()));
    }

    AcknowledgeQueue::TheOne(messageId)->add(1 == success);
}
