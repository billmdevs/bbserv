//Filename:  CmdWrite.h

#pragma once

#include <string>
#include "SessionResources.h"
#include "Config.h"
#include "ConnectionQueue.h"

/**
 *Command supporting the WRITE message.
 */
class CmdWrite
{
    protected:
        constexpr static const char* const  COMMAND_ID { "WRITE" };
        std::string commandId;
        FILE* stream {nullptr};
        const char* line {nullptr};
        std::string user;
        ConnectionQueue* connectionQueue {nullptr};

    public:
        /**
         *Custom constructor accepting the command line received on the socket.
         */
        CmdWrite(const std::string& commandId, FILE* stream, const char* line, std::string& user, ConnectionQueue* qu)
            : commandId(commandId)
            , stream(stream)
            , line(line)
            , user(user)
            , connectionQueue(qu)
        { }

    public:
        /**
         *Process the given command.
         */
        void execute();

        /**
         *Get the command identifier.
         */
        std::string_view get_command_id() { return this->commandId; }

        /**
         *Get this command's definition.
         */
        std::string_view get_line() { return this->line; }

        /**
         *Get the connect queue for broadcast commands.
         */
        ConnectionQueue* get_connection_queue() { return this->connectionQueue; }

    protected:
        /**
         * Retrieve the next free message number and update the storage immediately.
         */
        size_t update_message_number();

};

/**
 *Strip the LOCAL prefix if present.
 */
inline bool prepareLocalOperation(std::string& message)
{
    // Ignore some leading spaces
    auto localWriteOnly { 3 > message.find("LOCAL ") };
    if (localWriteOnly)
    {
        // Eliminate the prefix
        auto diff { std::strlen("LOCAL ") };
        message.replace(message.begin(), message.end() - diff, message.begin()
                + diff, message.end());
        message.replace(message.end() - diff, message.end(), diff, '\0');
    }
    return localWriteOnly;
}

/**
 *Broadcast one command and wait for the acknowledge.
 */
template<class T>
inline void broadcast_synchronous(T* cmd, std::string commandId, std::string userName, size_t messageId, std::string arguments)
{
    BroadcastCommand broadcast;

    for (auto& peer : Config::singleton().get_peers())
    {
        debug_print(cmd, "Add ", commandId, "/peer to connectionQueue: ", peer);
        broadcast.peer = peer;
        broadcast.command = "BROADCAST-";
        broadcast.command += commandId;
        broadcast.command += " " + userName;
        broadcast.command += " " + std::to_string(messageId);
        broadcast.command += " " + arguments;
        cmd->get_connection_queue()->add(broadcast);
    }

    // Wait for acknowledges from all peers
    if (!AcknowledgeQueue::TheOne(messageId)->check_success(Config::singleton().get_peers().size()))
    {
        error_return(cmd, "Did not get positive acknowledge from all peers");
    }
    debug_print(cmd, "All peers acknowledged");
    // Delete this AcknowledgeQueue
    AcknowledgeQueue::TheOne(messageId, true);
}

/**
 *Broadcast this REPLACE command to known peers.
 */
template<class T>
inline bool replicate_command(T* cmd, std::string userName, size_t messageId)
{
    broadcast_synchronous(cmd, "PRECOMMIT", userName, messageId, "");
    broadcast_synchronous(cmd, "COMMIT", userName, messageId,
            std::string(cmd->get_command_id()) + " LOCAL" +
            (cmd->get_line().data() + cmd->get_command_id().size()));

    return true;
}

