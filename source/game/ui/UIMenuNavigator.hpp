#ifndef GAME_UI_UIMENUNAVIGATOR_HPP
#define GAME_UI_UIMENUNAVIGATOR_HPP

#include <vector>

#include "game/ui/UIMenuPainter.hpp"

struct MenuTarget
{
    int kind;
    int index;
    MenuRect rect;
    int view;
    int layer;
    bool enabled;
};

class MenuNavigator
{
public:
    static const int kFixedView = -1;

    MenuNavigator();

    void Clear();
    void Add(int kind, int index, const MenuRect& rect, int view, int layer, bool enabled);
    int Count() const;
    const std::vector<MenuTarget>& Targets() const;
    const MenuTarget& At(int target) const;
    int Find(int kind, int index) const;
    void SetLayer(int layer);
    int Layer() const;

    void SetView(int view, const MenuRect& rect, int contentHeight, float timeConstant = 70.0f);
    MenuRect ViewRect(int view) const;
    int ContentHeight(int view) const;
    MenuRect ScreenRect(const MenuTarget& target) const;
    int TargetAt(float x, float y) const;
    int Nearest(int from, int dx, int dy) const;

    float ScrollPosition(int view) const;
    float MaxScroll(int view) const;
    void ScrollBy(int view, float amount);
    void ResetScroll(int view, float position);
    void SnapScroll(int view);
    void ScrollTo(int view, float position);
    bool MouseScrolling(int view) const;
    void EnsureVisible(const MenuTarget& target);

    void Animate();
    float Pulse() const;
    float Elapsed() const;
    void Flash(int kind, int index);
    float FlashAmount(int kind, int index) const;

private:
    struct View
    {
        MenuRect rect;
        int contentHeight;
        float timeConstant;
        float position;
        float target;
        bool mouse;
    };

    View& ViewAt(int view);
    const View* FindView(int view) const;

    std::vector<MenuTarget> m_targets;
    std::vector<View> m_views;
    int m_layer;
    long long m_lastTime;
    float m_pulse;
    float m_elapsed;
    int m_flashKind;
    int m_flashIndex;
    long long m_flashTime;
};

#endif
