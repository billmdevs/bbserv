//Filename:  CmdRead.cpp

#include "CmdRead.h"
#include <cstring>
#include <string>
#include <string_view>
#include <fstream>
#include <climits>
#include <optional>
#include <sstream>

std::optional<std::string> get_record(std::istream& input, std::string& id)
{
    input.seekg(0);

    for (std::string line; std::getline(input, line); )
    {
        if (0 == line.find(id + "/"))
        {
           return line;
        }
    }
    return {};
}

void CmdRead::execute()
{
    if (this->commandId != COMMAND_ID)
    {
        debug_print(this, "Command ", this->commandId, " is not for me");
        return;
    }

    debug_print(this, "Processing ", COMMAND_ID, " command\n");

    std::string id;
    std::stringstream sout(this->line);
    sout >> id;   // command READ
    sout >> id;   // message ID

    try
    {
        // Throw if the request does not contain a valid number
        if (sout.fail() || !std::isdigit(*id.data()))
        {
            error_return(this, "Request malformed");
        }

        std::ifstream fin (Config::singleton().get_bbfile());

        // Throw if bbfile is absent
        if (fin.fail())
        {
            error_return(this, "bbfile is not available");
        }

        if (auto record {get_record(fin, id)}; record)
        {
            auto pos { record->find_first_of("/") };
            (*record)[pos] = ' ';

            fprintf(this->stream, "2.0 MESSAGE %s\n", record->data());
        }
        else
        {
            fprintf(this->stream, "2.1 UNKNOWN %s Record not found\n", id.data());
            fflush(this->stream);
        }
    }
    catch (const BBServException& error)
    {
        fprintf(this->stream, "2.2 ERROR READ %s\n", error.what());
        fflush(this->stream);
    }
}
