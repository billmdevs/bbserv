//Filename:  CmdUnsuccessful.cpp

#include "CmdUnsuccessful.h"
#include "UndoStore.h"
#include <variant>

void CmdUnsuccessful::execute()
{
    if (this->commandId != COMMAND_ID)
    {
        debug_print(this, "Command ", this->commandId, " is not for me");
        return;
    }

    debug_print(this, "Processing ", COMMAND_ID, " command");

    auto messageId {0};
    std::string localCommandId;

    localCommandId.resize(100);

    try
    {
        std::istringstream sin(this->line);

        sin >> localCommandId >> messageId;
        debug_print(this, "[", localCommandId, "] ", "[", messageId, "]");

        if (sin.fail())
        {
            error_return(this, "Invalid command reply; state=",
                    name_statebits(sin.rdstate()));
        }

        debug_print(this, "Undo the last operation");
        try
        {
            std::visit([](auto&& cmd) { cmd.undo(); },
                    UndoStore::singleton().get());
        }
        catch (const BBServException& error)
        {
            std::cout << error.what() << std::endl;
        }
    }
    catch (const BBServException& error)
    {
        AcknowledgeQueue::TheOne(messageId)->add(false);
    }

    AcknowledgeQueue::TheOne(messageId)->add(false);
}
