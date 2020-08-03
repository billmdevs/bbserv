//Filename:  CmdPrecommit.cpp

#include "CmdPrecommit.h"
#include <sstream>

void CmdPrecommit::execute()
{
    if (this->commandId != COMMAND_ID)
    {
        debug_print(this, "Command ", this->commandId, " is not for me");
        return;
    }

    debug_print(this, "Processing ", COMMAND_ID, " command\n");

    auto messageId {0};
    std::string dummy;
    std::string userName;
    std::istringstream sin(this->line);
    dummy.resize(100);

    sin >> dummy >> userName >> messageId;

    if (sin.fail())
    {
        error_return(this, "Invalid ", COMMAND_ID, " command");
    }

    debug_print(this, "Acknowldging ", COMMAND_ID, " via socket ", fileno(this->stream));
    fprintf(this->stream, "ACK %d 1\n", messageId);
    fflush(this->stream);
}
