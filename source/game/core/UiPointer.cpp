#include "game/core/UiPointer.hpp"

#include <math.h>

#include "game/core/GameSettings.hpp"
#include "game/core/Gui.hpp"
#include "platform/CInput.hpp"

namespace
{
const float kCursorSpeed = 5.0f;
const float kStickDeadzone = 0.25f;
}

UiPointer::UiPointer()
{
    Reset();
    m_space.width = 0;
    m_space.height = 0;
    m_space.originX = 0.0f;
    m_space.originY = 0.0f;
    m_space.unit = 0.0f;
}

void UiPointer::Reset()
{
    m_x = 0.0f;
    m_y = 0.0f;
    m_known = false;
    m_drawn = false;
    m_held = false;
}

void UiPointer::SetSpace(const HudOverlaySpace& space)
{
    m_space = space;
}

void UiPointer::MoveFromStick(int pad)
{
    const float stickX = g_pInput->GetJoypadStick_RX(pad, false);
    const float stickY = g_pInput->GetJoypadStick_RY(pad, false);
    if (fabsf(stickX) < kStickDeadzone && fabsf(stickY) < kStickDeadzone)
        return;
    if (!m_known)
    {
        m_x = static_cast<float>(m_space.width) * 0.5f;
        m_y = static_cast<float>(m_space.height) * 0.5f;
    }
    m_known = true;
    m_drawn = true;
    m_x += stickX * kCursorSpeed;
    m_y -= stickY * kCursorSpeed;
    if (m_x < 0.0f)
        m_x = 0.0f;
    if (m_y < 0.0f)
        m_y = 0.0f;
    if (m_x > static_cast<float>(m_space.width - 1))
        m_x = static_cast<float>(m_space.width - 1);
    if (m_y > static_cast<float>(m_space.height - 1))
        m_y = static_cast<float>(m_space.height - 1);
}

void UiPointer::Poll(int pad, bool stick, std::vector<Event>& events)
{
    events.clear();
#if LCE_PLATFORM_PC
    bool moved = false;
    bool secondary = false;
    bool down = false;
    int wheel = 0;
    float touchX;
    float touchY;
    if (m_space.unit > 0.0f && g_pInput->ConsumeMenuPointer(pad, &touchX, &touchY))
    {
        m_x = (touchX - m_space.originX) / m_space.unit;
        m_y = (touchY - m_space.originY) / m_space.unit;
        m_known = true;
        m_drawn = false;
        moved = true;
    }
    if (stick)
    {
        const float before = m_x * 4096.0f + m_y;
        MoveFromStick(pad);
        moved = moved || before != m_x * 4096.0f + m_y;
        down = m_known && g_pInput->ButtonDown(pad, static_cast<unsigned char>(EControllerActions_MenuA));
    }
    else
    {
        secondary = (g_pInput->ConsumeMenuClicks(pad) & C_4JInput::EPcMenuClick_Secondary) != 0;
        down = g_pInput->MenuPointerHeld(pad);
        wheel = g_pInput->ConsumeMenuWheel(pad);
    }
    if (!m_known)
        return;

    Event event;
    event.button = 0;
    if (moved)
    {
        event.kind = eKind_Move;
        events.push_back(event);
    }
    if (down != m_held)
    {
        event.kind = down ? eKind_Press : eKind_Release;
        event.button = 1;
        events.push_back(event);
    }
    m_held = down;
    if (secondary)
    {
        event.button = 2;
        event.kind = eKind_Press;
        events.push_back(event);
        event.kind = eKind_Release;
        events.push_back(event);
    }
    if (wheel != 0)
    {
        event.kind = eKind_Scroll;
        event.button = wheel;
        events.push_back(event);
    }
#else
    (void)pad;
    (void)stick;
#endif
}

void UiPointer::Draw(Gui& gui) const
{
    if (!m_drawn || !m_known)
        return;
    const int x = static_cast<int>(m_x);
    const int y = static_cast<int>(m_y);
    for (int row = 0; row < 8; ++row)
    {
        gui.fill(x, y + row, x + 1 + row / 2 + 1, y + row + 1, 0xFF000000);
        gui.fill(x + 1, y + row, x + 1 + (row > 0 ? (row + 1) / 2 : 0) + 1, y + row + 1, 0xFFFFFFFF);
    }
}

const wchar_t* UiPointer::Name(EKind kind)
{
    switch (kind)
    {
    case eKind_Press:
        return L"press";
    case eKind_Release:
        return L"release";
    case eKind_Scroll:
        return L"scroll";
    default:
        return L"move";
    }
}
