#ifndef GAME_COMMAND_CHAT_REPLACEITEMCHATCOMMAND_HPP
#define GAME_COMMAND_CHAT_REPLACEITEMCHATCOMMAND_HPP

#include <boost/shared_ptr.hpp>

#include "game/command/ChatCommand.hpp"
#include "game/item/not_null_ptr.hpp"

class Container;
class ItemInstance;
class ServerPlayer;

class ReplaceItemChatCommand : public ChatCommand
{
public:
    ReplaceItemChatCommand();

    virtual bool run(const CommandSource& source, const Words& words);

    // the slot names a player has, and the ones of a chest-sized container, as they are typed
    static const std::vector<std::wstring>& PlayerSlotNames();
    static const std::vector<std::wstring>& ContainerSlotNames();

    // put a stack in a named slot, or in the slot offset places after it, which is how /loot replace fills several; false when the name or the
    // offset is no slot of the player or of the container
    static bool SetPlayerSlot(const boost::shared_ptr<ServerPlayer>& player, const std::wstring& slotName, int offset,
                              not_null_ptr<ItemInstance> stack);
    static bool SetContainerSlot(const boost::shared_ptr<Container>& container, const std::wstring& slotName, int offset,
                                 not_null_ptr<ItemInstance> stack);

private:
    static bool ParsePlayerSlot(const std::wstring& name, int* slot);
    static bool ParseContainerSlot(const std::wstring& name, int* slot);
};

#endif
