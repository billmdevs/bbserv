//Filename:  UndoStore.h

#pragma once

#include "Config.h"
#include <optional>
#include <variant>
#include "CmdWrite.h"
#include "CmdReplace.h"

/**
 *This queue serves reply messages from broadcasts sent out to peers.
 */
class UndoStore
{
    public:
        using Undoable_t = std::variant<CmdWrite, CmdReplace>;

    public:
        /**
         *Access the singleton catalog to get/set/erase the UndoStore instance.
         */
        static UndoStore& singleton();

    protected:
        std::optional<Undoable_t> recentCommand {std::nullopt};

    public:
        /**
         *Set the most resent command for further reference.
         */
        void set(Undoable_t&& cmd);

        /**
         *Delete the reference to the most recent command.
         */
        void clear();

        /**
         * Returns the most recent write/replace operation if availble.
         *
         * This may throw BBServException if no operation can be undone.
         */
        Undoable_t& get();

};
