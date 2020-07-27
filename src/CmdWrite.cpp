//Filename:  CmdWrite.cpp

#include "CmdWrite.h"
#include <cstring>
#include <string>
#include <string_view>
#include <fstream>

size_t CmdWrite::update_message_number()
{
    auto &in =  std::ios_base::in;
    auto &out =  std::ios_base::out;
    auto &trunc =  std::ios_base::trunc;

    std::string_view noFile { Config::singleton().get_bbfile() + ".no" };
    std::fstream io(noFile.data());
    size_t number;

    if (io.fail())
    {
        io.open(noFile.data(), in|out|trunc);
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
        error_return(this, "Failed to read/write message number (", noFile, ")");
    }
    return number;
}

void CmdWrite::execute()
{
    auto &in =  std::ios_base::in;
    auto &out =  std::ios_base::out;
    auto &trunc =  std::ios_base::trunc;

    if (this->commandId != COMMAND_ID)
    {
        debug_print(this, "Command ", this->commandId, " is not for me");
        return;
    }

    debug_print(this, "Processing ", COMMAND_ID, " command\n");

    std::string message { this->line + std::strlen(COMMAND_ID) + 1 };

    try
    {
        auto id { update_message_number() };
        std::fstream fout (Config::singleton().get_bbfile());

        // Create the file if needed
        if (fout.fail())
        {
            fout.open(Config::singleton().get_bbfile(), in|out|trunc);
            fout.flush();
        }

        // Throw if this fails too
        if (fout.fail())
        {
            error_return(this, "Failed to open/create file ", Config::singleton().get_bbfile());
        }

        fout.seekp(0, std::ios_base::end);
        debug_print(this, "file pos ", fout.tellp());
        fout << id << "/" << this->user << "/" << message;

        fprintf(this->stream, "3.0 WROTE %lu\n", id);
        fflush(this->stream);
    }
    catch (const BBServException& error)
    {
        fprintf(this->stream, "3.2 ERROR WRITE %s\n", error.what());
        fflush(this->stream);
    }
}
