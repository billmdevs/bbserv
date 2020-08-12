//Filename:  CmdWrite.cpp

#include "CmdWrite.h"
#include <cstring>
#include <iomanip>
#include <string>
#include <sstream>
#include <string_view>
#include <fstream>
#include <algorithm>
#include <variant>
#include <unistd.h>
#include <sys/types.h>
#include "ConnectionQueue.h"
#include "CmdBuilder.h"
#include "RWLock.h"
#include "UndoStore.h"

size_t CmdWrite::update_message_number()
{
    auto &in =  std::ios_base::in;
    auto &out =  std::ios_base::out;
    auto &trunc =  std::ios_base::trunc;

    std::string noFile { Config::singleton().get_bbfile() + ".no" };
    std::fstream io(noFile);
    size_t number;

    if (io.fail())
    {
        io.open(noFile, in|out|trunc);
        io << 0;
        io.flush();
        io.seekg(0);
    }

    io >> number;
    io.seekp(0);
    io.clear();
    io << number + 1;

    if (io.fail())
    {
        error_return(this, "Failed to read/write message number (", noFile,
                "), state=", name_statebits(io.rdstate()));

    }

    debug_print(this, "Current message number: ", number);
    return number;
}

void CmdWrite::open_db(std::fstream& fout)
{
    auto &in =  std::ios_base::in;
    auto &out =  std::ios_base::out;
    auto &trunc =  std::ios_base::trunc;

    fout.open(Config::singleton().get_bbfile());

    if (fout.fail())
    {
        debug_print(this, "Creating file: ",
                Config::singleton().get_bbfile());
        fout.open(Config::singleton().get_bbfile(), in|out|trunc);
        fout.flush();
    }

    // Throw if this fails too
    if (fout.fail())
    {
        error_return(this, "Failed to open/create file ",
                Config::singleton().get_bbfile());
    }
}

void CmdWrite::execute()
{
    if (this->commandId != COMMAND_ID)
    {
        debug_print(this, "Command ", this->commandId, " is not for me");
        return;
    }

    debug_print(this, "Processing ", COMMAND_ID, " command");

    std::string message { this->line + std::strlen(COMMAND_ID) + 1 };

    auto localWriteOnly { prepareLocalOperation(message) };

    auto id {0ul};

    try
    {
        // Get a new message ID and open the DB file
        id = update_message_number();

        // Create the DB file if needed
        std::fstream fout;
        open_db(fout);

        if (this->connectionQueue && !localWriteOnly)
        {
            // Have all the peers process this command as well
            if (!replicate_command(this, this->user, id))
            {
                // TODO undo
            }
        }

        debug_print(this, "Begin write operation...");
        RWAutoLock<WriteLock> guard (&globalRWLock);

        // Finally, the local write operation
        fout.seekp(0, std::ios_base::end);
        this->lastLinePos = fout.tellp();
        debug_print(this, "File pos ", this->lastLinePos);
        fout << id << "/" << this->user << "/" << message.data();

        if (localWriteOnly)
        {
            fout << std::endl;
        }

        guard.unlock();
        debug_print(this, " ...done");

        fprintf(this->stream, "3.0 WROTE %lu\n", id);
        fflush(this->stream);

        if (localWriteOnly)
        {
            UndoStore::singleton().set(*this);
        }
        else
        {
            UndoStore::singleton().clear();
        }
    }
    catch (const BBServException& error)
    {
        broadcast_asynchronous(this, "UNSUCCESSFUL", "", id, "");

        debug_print(this, error.what());
        fprintf(this->stream, "3.2 ERROR WRITE %s\n", error.what());
        fflush(this->stream);
    }
}

void CmdWrite::undo()
{
    try
    {
        debug_print(this, "Begin undo operation...");
        RWAutoLock<WriteLock> guard (&globalRWLock);

        truncate(Config::singleton().get_bbfile().data(), this->lastLinePos);

        debug_print(this, "...undone. Truncated bbfile to pos ",
                this->lastLinePos);
        UndoStore::singleton().clear();
    }
    catch (const BBServException& error)
    {
        std::cout << error.what() << std::endl;
    }
}
