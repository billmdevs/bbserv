//Filename:  BroadcastUnsuccessful.cpp

#include "BroadcastUnsuccessful.h"
#include <cstring>
#include <cstdio>

void BroadcastUnsuccessful::execute()
{
    if (this->commandId != COMMAND_ID)
    {
        debug_print(this, "Command ", this->commandId, " is not for me");
        return;
    }

    debug_print(this, "Broadcasting ", COMMAND_ID, " command");

    //debug_print(this, "Broadcasting ", COMMAND_ID, " via socket ", fileno(this->stream));
    fprintf(this->stream, "%s\n", this->line + std::strlen("BROADCAST-"));

}
