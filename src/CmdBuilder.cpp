//Filename:  CmdBuilder.cpp

#include <iomanip>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include "CmdBuilder.h"
#include "CmdUser.h"
#include "CmdWrite.h"
#include "CmdRead.h"
#include "CmdReplace.h"
#include "CmdQuit.h"
#include "CmdPrecommit.h"
#include "CmdCommit.h"
#include "CmdAcknowledge.h"
#include "BroadcastPrecommit.h"


/**
 * Build a command object for the given command ID.
 *
 * This function acts as a builder for all the supported command objects. It
 * returns a specific command object or std::nullopt if the requested command
 * is not supported.
 *
 * \param commandId Supported command identifiers include USER, WRITE, READ,
 *                  REPLACE and QUIT.
 * \param line  The received request message from the client. It is ought to
 *              start with the command identifier and may convey additional
 *              arguments.
 * \param resources Resources bound to this client connection including the
 *                  clients name as issued by the USER command.
 * \param qu  The ConnectionQueue reference, which WRITE and REPLACE commands
 *            can use to enqueue their broadcast commands.
 */
std::optional<ThreadPool::Commands_t> build_command(const std::string commandId, const char* line, SessionResources& resources, ConnectionQueue* qu)
{
    debug_print(line, std::quoted(commandId), " ", line);

    if (commandId == "USER")
    {
        return CmdUser(commandId, resources.get_stream(), line, resources.get_user());
    }
    else if (commandId == "WRITE")
    {
        return CmdWrite(commandId, resources.get_stream(), line, resources.get_user(), qu);
    }
    else if (commandId == "READ")
    {
        return CmdRead(commandId, resources.get_stream(), line, resources.get_user());
    }
    else if (commandId == "REPLACE")
    {
        return CmdReplace(commandId, resources.get_stream(), line, resources.get_user(), qu);
    }
    else if (commandId == "QUIT")
    {
        return CmdQuit(commandId, resources.get_stream(), line, resources.get_user());
    }
    else if (commandId == "PRECOMMIT")
    {
        return CmdPrecommit(commandId, resources.get_stream(), line);
    }
    else if (commandId == "COMMIT")
    {
        return CmdCommit(commandId, resources.get_stream(), line);
    }
    else if (commandId == "ACK")
    {
        return CmdAcknowledge(commandId, resources.get_stream(), line);
    }
    else if (commandId == "SUCCESSFUL")
    {
        return CmdSuccessful(commandId, resources.get_stream(), line);
    }
    else if (commandId == "BROADCAST-PRECOMMIT")
    {
        return BroadcastPrecommit("PRECOMMIT", resources.get_stream(), line);
    }
    else if (commandId == "BROADCAST-COMMIT")
    {
        return BroadcastCommit("COMMIT", resources.get_stream(), line);
    }
    return {};
}

