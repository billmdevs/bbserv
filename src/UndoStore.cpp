//Filename:  UndoStore.cpp

#include "UndoStore.h"

UndoStore& UndoStore::singleton()
{
    static UndoStore theObject;
    return theObject;
}

void UndoStore::set(Undoable_t&& cmd)
{
    this->recentCommand.emplace(cmd);
}

void UndoStore::clear()
{
    this->recentCommand = std::nullopt;
}

UndoStore::Undoable_t& UndoStore::get()
{
    if (!this->recentCommand)
    {
        error_return(this, "No operation to be undone.");
    }

    return this->recentCommand.value();
}
