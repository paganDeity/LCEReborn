#ifndef GAME_CORE_UIPOINTER_HPP
#define GAME_CORE_UIPOINTER_HPP

#include <vector>

#include "game/core/HudOverlay.hpp"

class Gui;

class UiPointer
{
public:
    enum EKind
    {
        eKind_Move,
        eKind_Press,
        eKind_Release,
        eKind_Scroll
    };

    struct Event
    {
        EKind kind;
        int button;
    };

    UiPointer();

    void Reset();
    void SetSpace(const HudOverlaySpace& space);
    void Poll(int pad, bool stick, std::vector<Event>& events);
    void Draw(Gui& gui) const;

    bool Known() const { return m_known; }
    bool CursorShown() const { return m_drawn; }
    float X() const { return m_x; }
    float Y() const { return m_y; }

    static const wchar_t* Name(EKind kind);

private:
    void MoveFromStick(int pad);

    HudOverlaySpace m_space;
    float m_x;
    float m_y;
    bool m_known;
    bool m_drawn;
    bool m_held;
};

#endif
