#ifndef GAME_CORE_MODSCREEN_HPP
#define GAME_CORE_MODSCREEN_HPP

#include <string>

#include "game/core/ModHandles.hpp"

// a screen a mod owns for a local player, drawn over the world and answering the pointer, the keyboard and the controller
class ModScreen
{
public:
    static bool Open(int pad, const std::wstring& name);
    static void Close(int pad);
    static bool IsOpen(int pad);
    static std::wstring Name(int pad);
    static void Tick(int pad);

    // the pointer, drawn over what a mod drew; a mod draws its screen by hooking this before it runs
    static void Draw(int pad, const ModHandles::GuiInfo& gui, int width, int height);

    // an event for the open screen: kind is move, press, release, scroll, key or text, x and y the pointer in interface
    // pixels. The game's own answer is to close on Escape; a hook that cancels takes the event instead. True when the game answered.
    static bool Input(int pad, const std::wstring& kind, int x, int y, int button, const std::wstring& text);
};

#endif
