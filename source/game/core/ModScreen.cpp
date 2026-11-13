#include "game/core/ModScreen.hpp"

#include <string>
#include <vector>
#include <wchar.h>

#include "game/core/ConsoleUIController.hpp"
#include "game/core/GameSettings.hpp"
#include "game/core/HudOverlay.hpp"
#include "game/core/Minecraft.hpp"
#include "game/core/ModHandles.hpp"
#include "game/core/ModHooks.hpp"
#include "game/core/ModOwned.hpp"
#include "game/core/UiPointer.hpp"
#include "game/ui/Tooltips.hpp"
#include "game/world/generation/MultiplayerLocalPlayer.hpp"
#include "game/world/Level.hpp"
#include "platform/CInput.hpp"

extern ConsoleUIController gConsoleUIController;

namespace
{
struct State
{
    bool open;
    bool keyboard;
    bool released;
    std::string owner;
    std::wstring name;
    UiPointer pointer;
};

State sStates[4];

class ScreenOwners : public ModOwnedRegistryBase
{
public:
    ScreenOwners() { InitializeCriticalSection(&m_lock); }

    void Set(State& s, const std::string& owner)
    {
        EnterCriticalSection(&m_lock);
        s.owner = owner;
        s.released = false;
        LeaveCriticalSection(&m_lock);
    }

    bool Released(const State& s)
    {
        EnterCriticalSection(&m_lock);
        const bool released = s.released;
        LeaveCriticalSection(&m_lock);
        return released;
    }

protected:
    virtual void ReleaseOwner(const std::string& owner)
    {
        EnterCriticalSection(&m_lock);
        for (int pad = 0; pad < 4; ++pad)
        {
            if (sStates[pad].open && sStates[pad].owner == owner)
                sStates[pad].released = true;
        }
        LeaveCriticalSection(&m_lock);
    }

private:
    C4JCriticalSection m_lock;
};

ScreenOwners sOwners;

void HideTooltips(int pad)
{
    gConsoleUIController.SetTooltips(pad, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, false,
                                     false);
}

const wchar_t* KeyName(int key)
{
    switch (key)
    {
    case C_4JInput::ETextKey_Backspace:
    case C_4JInput::ETextKey_WordBackspace:
        return L"Backspace";
    case C_4JInput::ETextKey_Delete:
    case C_4JInput::ETextKey_WordDelete:
        return L"Delete";
    case C_4JInput::ETextKey_Enter:
        return L"Enter";
    case C_4JInput::ETextKey_Escape:
        return L"Escape";
    case C_4JInput::ETextKey_Tab:
        return L"Tab";
    case C_4JInput::ETextKey_Up:
        return L"Up";
    case C_4JInput::ETextKey_Down:
        return L"Down";
    case C_4JInput::ETextKey_Left:
    case C_4JInput::ETextKey_WordLeft:
        return L"Left";
    case C_4JInput::ETextKey_Right:
    case C_4JInput::ETextKey_WordRight:
        return L"Right";
    case C_4JInput::ETextKey_Home:
        return L"Home";
    case C_4JInput::ETextKey_End:
        return L"End";
    case C_4JInput::ETextKey_PageUp:
        return L"PageUp";
    case C_4JInput::ETextKey_PageDown:
        return L"PageDown";
    default:
        return NULL;
    }
}

void SendKey(State& s, int pad, const wchar_t* name)
{
    ModScreen::Input(pad, L"key", static_cast<int>(s.pointer.X()), static_cast<int>(s.pointer.Y()), 0, name);
}

void TickKeyboard(State& s, int pad)
{
#if LCE_PLATFORM_PC
    std::wstring typed;
    if (g_pInput->ConsumeCapturedText(&typed))
        ModScreen::Input(pad, L"text", static_cast<int>(s.pointer.X()), static_cast<int>(s.pointer.Y()), 0, typed);
    for (int input = g_pInput->ConsumeTextKey(); input != C_4JInput::ETextKey_None && s.open;
         input = g_pInput->ConsumeTextKey())
    {
        const wchar_t* name = KeyName(input & ~C_4JInput::ETextKey_Shift);
        if (name == NULL)
            continue;
        std::wstring full = (input & C_4JInput::ETextKey_Shift) != 0 ? L"Shift+" : L"";
        full += name;
        SendKey(s, pad, full.c_str());
    }
#else
    (void)s;
    (void)pad;
#endif
}

void TickController(State& s, int pad)
{
    static const int actions[6] = {EControllerActions_MenuUp,    EControllerActions_MenuDown,
                                   EControllerActions_MenuLeft,  EControllerActions_MenuRight,
                                   EControllerActions_MenuCancel, EControllerActions_MenuA};
    static const wchar_t* const names[6] = {L"Up", L"Down", L"Left", L"Right", L"Escape", L"Enter"};
    const int count = s.pointer.CursorShown() ? 5 : 6;
    for (int index = 0; index < count && s.open; ++index)
    {
        if (g_pInput->ButtonPressed(pad, static_cast<unsigned char>(actions[index]), 0))
            SendKey(s, pad, names[index]);
    }
}

void DrawScreen(State& s, int pad, Gui& gui, Font& font, const HudOverlaySpace& space)
{
    s.pointer.SetSpace(space);
    const ModHandles::GuiInfo display = {&gui, &font, space.width, space.height};
    ModScreen::Draw(pad, display, space.width, space.height);
}

class ModScreenLayer : public HudOverlayLayer
{
public:
    virtual void Draw(int pad, Gui& gui, Font& font, const HudOverlaySpace& space)
    {
        if (pad >= 0 && pad < 4 && sStates[pad].open)
            DrawScreen(sStates[pad], pad, gui, font, space);
    }
};

ModScreenLayer sLayer;

struct Registration
{
    Registration() { HudOverlay::Add(&sLayer); }
};

Registration sRegistration;
}

bool ModScreen::Open(int pad, const std::wstring& name)
{
    if (pad < 0 || pad >= 4 || name.empty())
        return false;
    Minecraft* minecraft = Minecraft::GetInstance();
    State& s = sStates[pad];
    if (s.open || !minecraft->mLocalPlayers[pad] || gConsoleUIController.GetMenuDisplayed(pad))
        return false;
    s.open = true;
    s.name = name;
    s.keyboard = g_pInput->UsesKeyboard(pad);
    sOwners.Set(s, ModHandles::Caller::Mod());
    s.pointer.Reset();
    gConsoleUIController.SetMenuDisplayed(pad, true);
    HideTooltips(pad);
#if LCE_PLATFORM_PC
    if (s.keyboard)
        g_pInput->BeginTextCapture(pad);
#endif
    return true;
}

void ModScreen::Close(int pad)
{
    if (pad < 0 || pad >= 4 || !sStates[pad].open)
        return;
    State& s = sStates[pad];
    s.open = false;
#if LCE_PLATFORM_PC
    if (s.keyboard)
        g_pInput->EndTextCapture();
#endif
    gConsoleUIController.SetMenuDisplayed(pad, false);
    Tooltips::ForceTooltipReload(pad);
}

bool ModScreen::IsOpen(int pad)
{
    return pad >= 0 && pad < 4 && sStates[pad].open;
}

void ModScreen::Tick(int pad)
{
    if (!IsOpen(pad))
        return;
    State& s = sStates[pad];
    Minecraft* minecraft = Minecraft::GetInstance();
    if (!minecraft->mLocalPlayers[pad] || !minecraft->mLocalPlayers[pad]->isAlive() || minecraft->mLevel == NULL
        || !ModHooks::IsActive(kModHook_ModScreen_Draw) || sOwners.Released(s))
    {
        Close(pad);
        return;
    }
    std::vector<UiPointer::Event> events;
    s.pointer.Poll(pad, !s.keyboard, events);
    for (size_t index = 0; index < events.size(); ++index)
    {
        ModScreen::Input(pad, UiPointer::Name(events[index].kind), static_cast<int>(s.pointer.X()),
                         static_cast<int>(s.pointer.Y()), events[index].button, L"");
    }
    if (s.keyboard)
        TickKeyboard(s, pad);
    else
        TickController(s, pad);
}

std::wstring ModScreen::Name(int pad)
{
    return IsOpen(pad) ? sStates[pad].name : std::wstring();
}

void ModScreen::Draw(int pad, const ModHandles::GuiInfo& gui, int width, int height)
{
    (void)width;
    (void)height;
    sStates[pad].pointer.Draw(*gui.gui);
}

bool ModScreen::Input(int pad, const std::wstring& kind, int x, int y, int button, const std::wstring& text)
{
    (void)x;
    (void)y;
    (void)button;
    if (kind == L"key" && text == L"Escape")
        Close(pad);
    return true;
}
