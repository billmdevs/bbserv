//Filename:  BroadcastCommit.cpp

#include "BroadcastCommit.h"
#include <cstring>
#include <cstdio>
#include <poll.h>
#include <sys/socket.h>
#include "CmdBuilder.h"

void BroadcastCommit::execute()
{
    if (this->commandId != COMMAND_ID)
    {
        debug_print(this, "Command ", this->commandId, " is not for me");
        return;
    }

    debug_print(this, "Broadcasting ", COMMAND_ID, " command");

    std::array<char, 1024> localLine;
    std::string localCommandId;
    SessionResources localResources;

    memset(localLine.data(), 0, localLine.size());

    //debug_print(this, "Broadcasting ", COMMAND_ID, " via socket ", fileno(this->stream));
    fprintf(this->stream, "%s\n", this->line + std::strlen("BROADCAST-"));
    fflush(this->stream);
    debug_print(this, "Sending on ", fileno(this->stream), ": ", this->line + std::strlen("BROADCAST-"));

    try
    {
        debug_print(this, "Polling for reply on ", fileno(this->stream));
        wait_for(fileno(this->stream));
        debug_print(this, "...data available");

        auto read {recv(fileno(this->stream), localLine.data(), localLine.size(), 0)};
        if (-1 == read)
        {
            error_return(this, "Failed to read from ", fileno(this->stream), ": ", strerror(errno));
        }
        else if (0 == read)
        {
            error_return(this, "No data read from ", fileno(this->stream), ": ", strerror(errno));
        }

        //if (!std::fgets(localLine.data(), localLine.size(), this->stream))
        //{
            //error_return(this, "Failed to read from ", fileno(this->stream), ": ", strerror(errno));
        //}
        debug_print(this, "Processing reply: ", localLine.data(), " received via ", fileno(this->stream));

        // SUCCESSFUL or UNSUCCESSFUL
        std::istringstream sin(localLine.data());
        sin >> localCommandId;

        if (sin.fail())
        {
            error_return(this, "Invalid command reply; state=",
                    name_statebits(sin.rdstate()));
        }

        localResources.get_stream() = this->stream;

        auto command { build_command(localCommandId, localLine.data(), localResources).value() };
        std::visit([](auto&& command) { command.execute(); }, command);
    }
    catch (const BBServException& error)
    {
        std::cout << error.what() << std::endl;
        localResources.detach_stream();
    }

    localResources.detach_stream();
}
