#include "game/core/L10N.hpp"

#include "game/core/CMinecraftApp.hpp"
#include "game/core/CConsoleMinecraftApp.hpp"
#include "game/core/ExtraStrings.hpp"
#include "game/core/ModStrings.hpp"
#include "platform/Platform.hpp"
#include "game/core/StringIds.hpp"
#include "game/data/StringTable.hpp"

static StringTable* g_stringTable = NULL;
extern CConsoleMinecraftApp gApp;

int IdentityInUseMessageId()
{
#if LCE_PLATFORM_PC
    return IDS_LCE_IDENTITY_IN_USE;
#else
    return STRING_MII_ALREADY_IN_USE;
#endif
}

const wchar_t* ExtraString(int id)
{
    const wchar_t* modText = ModStrings::Lookup(id);
    if (modText != NULL)
        return modText;
    switch (id)
    {
    case IDS_LCE_CHAT:
        return L"Chat";
    case IDS_LCE_CHAT_SEND:
        return L"Send";
    case IDS_LCE_CHAT_CLOSE:
        return L"Close";
    case IDS_LCE_CHAT_COMPLETE:
        return L"Complete";
    case IDS_LCE_CHAT_TYPE:
        return L"Type";
    case IDS_LCE_CHAT_DELETE:
        return L"Delete";
    case IDS_LCE_CHAT_SPACE:
        return L"Space";
    case IDS_LCE_CHAT_SHIFT:
        return L"Shift";
    case IDS_LCE_CHAT_QUICK:
        return L"Quick Chat";
    case IDS_LCE_CHAT_KEYBOARD:
        return L"Keyboard";
    case IDS_LCE_CHAT_CHOOSE:
        return L"Choose";
    case IDS_LCE_CHAT_HISTORY:
        return L"History";
    case IDS_LCE_CHAT_SCROLL:
        return L"Scroll";
    case IDS_LCE_DEBUG_CHANGE:
        return L"Change";
    case IDS_LCE_DEBUG_PREVIEW:
        return L"Preview";
    case IDS_LCE_DEBUG_PRESET:
        return L"Preset";
    case IDS_LCE_DEBUG_RESET:
        return L"Reset";
    case IDS_LCE_HOLD_TO_SNEAK:
        return L"Hold to Sneak";
    case IDS_LCE_SHADER_PACKS:
        return L"Shader Packs";
    case IDS_LCE_SHADER_SETTINGS:
        return L"Shader Settings";
    case IDS_LCE_SHADER_PACK_SETTINGS:
        return L"Shader Pack Settings";
    case IDS_LCE_SHADER_REFRESH:
        return L"Refresh";
    case IDS_LCE_SHADER_APPLY:
        return L"Apply";
    case IDS_LCE_ONLINE_SETTINGS:
        return L"Online Settings";
    case IDS_LCE_HOST_PRIVILEGES_WARNING:
        return L"Turning on Host Privileges disables achievements for this world, even if you turn it off again "
               L"later. Are you sure you want to continue?";
    case IDS_LCE_TURN_ON:
        return L"Turn On";
    case IDS_LCE_GAME_MODE:
        return L"Game Mode";
    case IDS_LCE_CREATIVE_WARNING:
        return L"Setting this world to Creative Mode disables achievements for it, even if you set it back to "
               L"Survival Mode later. Are you sure you want to continue?";
    case IDS_LCE_SET_TO_CREATIVE:
        return L"Set to Creative";
    case IDS_LCE_STOP_LAN_WORLD:
        return L"Stop LAN World";
    case IDS_LCE_STOP_LAN_WARNING:
        return L"Players who joined over the network will be disconnected. Players on this console keep playing, "
               L"and you can open the world to LAN again from the pause menu.";
    case IDS_LCE_SERVER_PINGING:
        return L"Pinging...";
    case IDS_LCE_SERVER_CANT_CONNECT:
        return L"Can't connect to server";
    case IDS_LCE_CLIENT_OUT_OF_DATE:
        return L"Client out of date!";
    case IDS_LCE_SERVER_OUT_OF_DATE:
        return L"Server out of date!";
    case IDS_LCE_LEADERBOARD_TRAVELLING:
        return L"Travelling";
    case IDS_LCE_LEADERBOARD_MINING:
        return L"Mining";
    case IDS_LCE_LEADERBOARD_FARMING:
        return L"Farming";
    case IDS_LCE_LEADERBOARD_KILLS:
        return L"Kills";
    case IDS_LCE_LEADERBOARD_MY_STATISTICS:
        return L"My Statistics";
    case IDS_LCE_LEADERBOARD_UNAVAILABLE:
        return L"This game doesn't keep a leaderboard.";
    case IDS_LCE_LEADERBOARD_PLAYER:
        return L"Player";
    case IDS_LCE_SHADER_PREVIOUS_NEXT:
        return L"Previous / Next";
    case IDS_LCE_SHADER_DEFAULT:
        return L"Default";
    case IDS_LCE_SHADER_HIDE_MENU:
        return L"Hide Menu";
    case IDS_LCE_SHADER_SHOW_MENU:
        return L"Show Menu";
    case IDS_LCE_SHADER_OPEN:
        return L"Open";
    case IDS_LCE_SHADER_SWITCH_TAB:
        return L"Switch Tab";
    case IDS_LCE_DISCONNECTED_MISSING_MODS:
        return L"This game uses mods you do not have. Install them to join. The names are in the log.";
    case IDS_LCE_DISCONNECTED_NAME_IN_USE:
        return L"Someone with your name is already playing in this game. Change your username to join.";
    case IDS_LCE_TOUCH_CONTROLS:
        return L"Touch Controls";
    case IDS_LCE_TOUCH_JOYSTICK_TAP:
        return L"Joystick & Tap";
    case IDS_LCE_TOUCH_JOYSTICK_CROSSHAIR:
        return L"Joystick & Crosshair";
    case IDS_LCE_TOUCH_DPAD_TAP:
        return L"D-Pad & Tap";
    case IDS_LCE_TOUCH_PICK_BLOCK:
        return L"Pick Block Button";
    case IDS_LCE_TOUCH_LEFT_HANDED:
        return L"Left-Handed Inventory";
    case IDS_LCE_TOUCH_EASY_SPRINT:
        return L"Sprint Using the Joystick";
    case IDS_LCE_TOUCH_PERSPECTIVE:
        return L"Camera Perspective Button";
    case IDS_LCE_TOUCH_JOYSTICK:
        return L"Joystick";
    case IDS_LCE_TOUCH_JOYSTICK_VISIBLE:
        return L"Always Visible";
    case IDS_LCE_TOUCH_JOYSTICK_HIDDEN_UNUSED:
        return L"Hidden When Unused";
    case IDS_LCE_TOUCH_JOYSTICK_HIDDEN:
        return L"Always Hidden";
    case IDS_LCE_TOUCH_SWAP_JUMP_SNEAK:
        return L"Swap Jump and Sneak";
    case IDS_LCE_WORLD_TOO_NEW_TITLE:
        return L"Can't Open World";
    case IDS_LCE_WORLD_TOO_NEW:
        return L"This world uses data that this version of the game can't read. Open it with a newer version.";
    case IDS_LCE_WORLD_DAMAGED_TITLE:
        return L"Can't Open World";
    case IDS_LCE_WORLD_DAMAGED:
        return L"This world's save file is damaged, so it was not opened and nothing was changed. Restore it from a copy.";
    case IDS_LCE_FRIEND_NOT_FOUND:
        return L"No one has that friend code. Check it and try again.";
    case IDS_LCE_FRIENDS:
        return L"Friends";
    case IDS_LCE_YOUR_FRIEND_CODE:
        return L"Your friend code";
    case IDS_LCE_ADD_FRIEND:
        return L"Add Friend";
    case IDS_LCE_FRIEND_CODE:
        return L"Friend Code";
    case IDS_LCE_NICKNAME:
        return L"Nickname";
    case IDS_LCE_INVITE_FRIEND:
        return L"Invite";
    case IDS_LCE_REMOVE_FRIEND_TITLE:
        return L"Remove Friend";
    case IDS_LCE_REMOVE_FRIEND:
        return L"Remove this friend? They will no longer be able to see or join your games.";
    case IDS_LCE_NO_FRIENDS:
        return L"No friends yet. Choose Add Friend and enter a friend code.";
    case IDS_LCE_FRIEND_CODE_INVALID:
        return L"That friend code isn't valid. Friend codes look like ABCD-EFGH-JKLM-NPQR.";
    case IDS_LCE_TOOLTIP_NICKNAME:
        return L"Nickname";
    case IDS_LCE_TOOLTIP_REMOVE:
        return L"Remove";
    case IDS_LCE_FRIEND_INVITED:
        return L"Invitation sent.";
    case IDS_LCE_IDENTITY_IN_USE:
        return L"This identity is already online, in another copy of the game or on this server. Choose another identity or close the other copy.";
    case IDS_LCE_IDENTITIES:
        return L"Identities";
    case IDS_LCE_NEW_IDENTITY:
        return L"New Identity";
    case IDS_LCE_USERNAME:
        return L"Username";
    case IDS_LCE_USE_IDENTITY:
        return L"Use";
    case IDS_LCE_CHANGE_SKIN:
        return L"Skin";
    case IDS_LCE_RENAME_IDENTITY:
        return L"Username";
    case IDS_LCE_REMOVE_IDENTITY_TITLE:
        return L"Remove Identity";
    case IDS_LCE_REMOVE_IDENTITY:
        return L"Remove this identity? Its key and skin are deleted. If you don't have a copy of the file, it can't be recovered.";
    case IDS_LCE_IDENTITY_UNUSABLE:
        return L"This identity file can't be used";
    case IDS_LCE_IDENTITY_NAME_INVALID:
        return L"A username is 3 to 16 letters, digits and underscores.";
    case IDS_LCE_IDENTITY_ONLINE:
        return L"You are online with this identity. Leave the game before changing it.";
    case IDS_LCE_USERNAME_PLACEHOLDER:
        return L"username...";
    case IDS_LCE_SKIN_FILE:
        return L"Skin file...";
    case IDS_LCE_OPEN_IDENTITY_FOLDER:
        return L"Open identities folder";
    case IDS_LCE_CUSTOM_SKIN:
        return L"Custom skin";
    case IDS_LCE_SKIN_FILE_FAILED:
        return L"That file can't be used as a skin. Use a PNG of at most 1 MiB.";
    default:
        return NULL;
    }
}

const wchar_t* L10N::GetString(int id)
{
    const wchar_t* extra = ExtraString(id);
    if (extra != NULL)
        return extra;
    if (g_stringTable == NULL)
        return L"";
    return g_stringTable->getString(id);
}

void L10N::loadStringTable()
{
    delete g_stringTable;
    std::wstring path = L"languages.loc";
    if (gApp.hasArchiveFile(path, true))
    {
        arrayWithLength<unsigned char> data = gApp.getArchiveFile(path, true);
        g_stringTable = new StringTable(data, false, false);
        delete[] data.data;
    }
    else
    {
        g_stringTable = NULL;
    }
}
