//Filename:  CmdUser.cpp

#include "CmdUser.h"
#include <cstdio>

void CmdUser::execute()
{
    if (this->commandId != COMMAND_ID)
    {
        debug_print(this, "Command ", this->commandId, " is not for me");
        return;
    }

    debug_print(this, "Processing USER command\n");

    char text[1024];
    if (1 != sscanf(line, "USER %s", text))
    {
        error_return(this, "Malformed USER command");
    }
    this->user = text;
    debug_print(this, "Say HELLO to ", this->user);

    fprintf(this->stream, "1.0 HELLO %s I'm ready\n", this->user.data());
    fflush(this->stream);
}
