//Filename:  CmdQuit.cpp

#include "CmdQuit.h"
#include <cstring>
#include <string>
#include <string_view>
#include <fstream>
#include <climits>
#include <optional>
#include <sstream>

void CmdQuit::execute()
{
    if (this->commandId != COMMAND_ID)
    {
        debug_print(this, "Command ", this->commandId, " is not for me");
        return;
    }

    debug_print(this, "Processing ", COMMAND_ID, " command\n");

    fprintf(this->stream, "4.0 BYE\n");
    fflush(this->stream);
}
