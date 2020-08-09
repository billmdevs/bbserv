//Filename:  CmdCommit.cpp

#include "CmdCommit.h"
#include <iomanip>
#include <string>
#include <cstring>
#include <errno.h>
#include <sstream>
#include <cstdio>
#include <sys/socket.h>
#include "CmdBuilder.h"

void CmdCommit::send_reply_text(std::string& text)
{
        auto sent { send(fileno(this->stream), text.data(), text.size(), 0) };
        if (-1 == sent)
        {
            error_return(this, "Failed to send success reply on ", fileno(this->stream));
        }
        else if (text.size() > static_cast<size_t>(sent))
        {
            error_return(this, "Failed to send all bytes of the success reply ", sent, " on ", fileno(this->stream));
        }

        //fprintf(this->stream, "SUCCESSFUL %d\n", messageId);
        //fflush(this->stream);
}

void CmdCommit::execute()
{
    if (this->commandId != COMMAND_ID)
    {
        debug_print(this, "Command ", this->commandId, " is not for me");
        return;
    }

    debug_print(this, "Processing ", COMMAND_ID, " command");

    std::istringstream sin (this->line);
    std::string localCommandId;
    std::string localLine;
    std::string localUser;
    auto messageId {0};

    localCommandId.resize(100);
    localLine.resize(1024);
    localUser.resize(100);

    try
    {
        // Prepare a FILE stream for the locally invoked WRITE or REPLACE command
        auto pipeStream { StreamResource() };

        // Prepare the local WRITE or REPLACE command
        sin >> localCommandId >> localUser >> messageId;
        sin.ignore(1, ' ');
        sin.getline(localLine.data(), localLine.size(), '\n');
        localLine += "\n";

        if (sin.fail())
        {
            error_return(this, "Invalid COMMIT command");
        }

        std::istringstream sin2 (localLine);
        sin2 >> localCommandId;
        debug_print(this, "Local command: ", std::quoted(localCommandId), " ",
                localLine);

        SessionResources localResources;
        localResources.get_stream() = pipeStream.get_pipeStreams()[WRITE_END];
        localResources.get_user() = localUser;

        auto command { build_command(localCommandId, localLine.data(), localResources).value() };
        std::visit([](auto&& cmd) { cmd.execute(); }, command);
        localResources.detach_stream();

        fgets(localLine.data(), localLine.size(), pipeStream.get_pipeStreams()[READ_END]);
        if (0 != std::strncmp("3.0 WROTE", localLine.data(), 9))
        {
            error_return(this, "Local Write operation failed: ",
                    localLine.data());
        }

        std::string text = std::string("SUCCESSFUL ") + std::to_string(messageId);
        send_reply_text(text);
        debug_print(this, "Reply SUCCESSFUL via ", fileno(this->stream));
    }
    catch (const BBServException& error)
    {
        std::cout << error.what() << std::endl;
        std::string text = std::string("UNSUCCESSFUL ") + std::to_string(messageId);
        send_reply_text(text);
    }

}
