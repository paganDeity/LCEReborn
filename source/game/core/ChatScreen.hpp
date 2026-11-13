#ifndef GAME_CORE_CHATSCREEN_HPP
#define GAME_CORE_CHATSCREEN_HPP

#include <string>

#include "game/core/HudOverlay.hpp"

class Font;
class Gui;

class ChatScreen
{
public:
    enum EOpenMode
    {
        EOpenMode_Keyboard,
        EOpenMode_Command,
        EOpenMode_Controller
    };

    static bool Open(int pad, EOpenMode mode);
    static void OpenAfterMenus(int pad, EOpenMode mode);
    static void Close(int pad);
    static bool IsOpen(int pad);
    static void Tick(int pad);
    static void Draw(int pad, Gui& gui, Font& font, const HudOverlaySpace& space);
    static void RememberSender(int pad, const std::wstring& name);

    // the pointer over the chat: message is the line's number (0 is the newest), or -1 when it is over none, and x and y are
    // from the line's top left. The chat's own answer to a wheel turn is to scroll; a hook that cancels takes the event instead
    static void Pointer(int pad, int message, const std::wstring& kind, int x, int y, int button);
};

#endif
