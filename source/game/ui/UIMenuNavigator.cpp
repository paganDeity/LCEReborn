#include "game/ui/UIMenuNavigator.hpp"

#include <math.h>
#include <stdlib.h>

#include "game/core/System.hpp"

namespace
{
const long long kFlashMilliseconds = 450;
const long long kPulseMilliseconds = 1200;
const float kMaxFrameMilliseconds = 250.0f;
const float kVisibleMargin = 4.0f;

bool Contains(const MenuRect& rect, float x, float y)
{
    return x >= rect.x0 && x < rect.x1 && y >= rect.y0 && y < rect.y1;
}

float Clamp(float value, float low, float high)
{
    return value < low ? low : value > high ? high : value;
}
}

MenuNavigator::MenuNavigator()
    : m_layer(0), m_lastTime(0), m_pulse(0.0f), m_elapsed(0.0f), m_flashKind(-1), m_flashIndex(-1), m_flashTime(0)
{
}

void MenuNavigator::Clear()
{
    m_targets.clear();
}

void MenuNavigator::Add(int kind, int index, const MenuRect& rect, int view, int layer, bool enabled)
{
    MenuTarget target;
    target.kind = kind;
    target.index = index;
    target.rect = rect;
    target.view = view;
    target.layer = layer;
    target.enabled = enabled;
    m_targets.push_back(target);
}

int MenuNavigator::Count() const
{
    return static_cast<int>(m_targets.size());
}

const std::vector<MenuTarget>& MenuNavigator::Targets() const
{
    return m_targets;
}

const MenuTarget& MenuNavigator::At(int target) const
{
    return m_targets[static_cast<size_t>(target)];
}

int MenuNavigator::Find(int kind, int index) const
{
    for (size_t target = 0; target < m_targets.size(); ++target)
    {
        if (m_targets[target].kind == kind && m_targets[target].index == index)
            return static_cast<int>(target);
    }
    return -1;
}

void MenuNavigator::SetLayer(int layer)
{
    m_layer = layer;
}

int MenuNavigator::Layer() const
{
    return m_layer;
}

MenuNavigator::View& MenuNavigator::ViewAt(int view)
{
    while (static_cast<int>(m_views.size()) <= view)
    {
        View added;
        added.rect.x0 = added.rect.y0 = added.rect.x1 = added.rect.y1 = 0;
        added.contentHeight = 0;
        added.timeConstant = 70.0f;
        added.position = added.target = 0.0f;
        added.mouse = false;
        m_views.push_back(added);
    }
    return m_views[static_cast<size_t>(view)];
}

const MenuNavigator::View* MenuNavigator::FindView(int view) const
{
    if (view < 0 || view >= static_cast<int>(m_views.size()))
        return NULL;
    return &m_views[static_cast<size_t>(view)];
}

void MenuNavigator::SetView(int view, const MenuRect& rect, int contentHeight, float timeConstant)
{
    View& entry = ViewAt(view);
    entry.rect = rect;
    entry.contentHeight = contentHeight;
    entry.timeConstant = timeConstant;
}

MenuRect MenuNavigator::ViewRect(int view) const
{
    const View* entry = FindView(view);
    if (entry != NULL)
        return entry->rect;
    MenuRect empty;
    empty.x0 = empty.y0 = empty.x1 = empty.y1 = 0;
    return empty;
}

int MenuNavigator::ContentHeight(int view) const
{
    const View* entry = FindView(view);
    return entry != NULL ? entry->contentHeight : 0;
}

MenuRect MenuNavigator::ScreenRect(const MenuTarget& target) const
{
    MenuRect rect = target.rect;
    const View* entry = FindView(target.view);
    if (entry == NULL)
        return rect;
    int offset = entry->rect.y0 - static_cast<int>(entry->position + 0.5f);
    rect.y0 += offset;
    rect.y1 += offset;
    return rect;
}

int MenuNavigator::TargetAt(float x, float y) const
{
    for (size_t index = 0; index < m_targets.size(); ++index)
    {
        const MenuTarget& target = m_targets[index];
        if (!target.enabled || target.layer != m_layer)
            continue;
        if (target.view != kFixedView && !Contains(ViewRect(target.view), x, y))
            continue;
        if (Contains(ScreenRect(target), x, y))
            return static_cast<int>(index);
    }
    return -1;
}

int MenuNavigator::Nearest(int from, int dx, int dy) const
{
    if (from < 0 || from >= Count())
        return -1;
    MenuRect origin = ScreenRect(At(from));
    int centreX = (origin.x0 + origin.x1) / 2;
    int centreY = (origin.y0 + origin.y1) / 2;
    int best = -1;
    int bestScore = 0;
    for (size_t index = 0; index < m_targets.size(); ++index)
    {
        const MenuTarget& candidate = m_targets[index];
        if (static_cast<int>(index) == from || !candidate.enabled || candidate.layer != m_layer)
            continue;
        MenuRect to = ScreenRect(candidate);
        int targetX = (to.x0 + to.x1) / 2;
        int targetY = (to.y0 + to.y1) / 2;
        int primary = 0;
        int secondary = 0;
        if (dy != 0)
        {
            primary = (targetY - centreY) * dy;
            if (primary <= 2)
                continue;
            if (to.x1 <= origin.x0)
                secondary = origin.x0 - to.x1;
            else if (to.x0 >= origin.x1)
                secondary = to.x0 - origin.x1;
            secondary += abs(targetX - centreX) / 8;
        }
        else
        {
            primary = (targetX - centreX) * dx;
            if (primary <= 2 || to.y1 <= origin.y0 || to.y0 >= origin.y1)
                continue;
            secondary = abs(targetY - centreY);
        }
        int score = primary + secondary * 3;
        if (best < 0 || score < bestScore)
        {
            best = static_cast<int>(index);
            bestScore = score;
        }
    }
    return best;
}

float MenuNavigator::ScrollPosition(int view) const
{
    const View* entry = FindView(view);
    return entry != NULL ? entry->position : 0.0f;
}

float MenuNavigator::MaxScroll(int view) const
{
    const View* entry = FindView(view);
    if (entry == NULL)
        return 0.0f;
    float overflow = static_cast<float>(entry->contentHeight - (entry->rect.y1 - entry->rect.y0));
    return overflow > 0.0f ? overflow : 0.0f;
}

void MenuNavigator::ScrollBy(int view, float amount)
{
    if (view < 0)
        return;
    View& entry = ViewAt(view);
    if (!entry.mouse)
        entry.target = entry.position;
    entry.mouse = true;
    entry.target = Clamp(entry.target + amount, 0.0f, MaxScroll(view));
}

void MenuNavigator::ResetScroll(int view, float position)
{
    if (view < 0)
        return;
    View& entry = ViewAt(view);
    entry.position = position;
    entry.target = position;
    entry.mouse = false;
}

void MenuNavigator::SnapScroll(int view)
{
    if (view < 0)
        return;
    View& entry = ViewAt(view);
    entry.position = entry.target;
}

void MenuNavigator::ScrollTo(int view, float position)
{
    if (view < 0)
        return;
    View& entry = ViewAt(view);
    entry.target = Clamp(position, 0.0f, MaxScroll(view));
    entry.mouse = false;
}

bool MenuNavigator::MouseScrolling(int view) const
{
    const View* entry = FindView(view);
    return entry != NULL && entry->mouse;
}

void MenuNavigator::EnsureVisible(const MenuTarget& target)
{
    if (target.view < 0)
        return;
    View& entry = ViewAt(target.view);
    float visible = static_cast<float>(entry.rect.y1 - entry.rect.y0);
    float position = entry.mouse ? entry.position : entry.target;
    if (target.rect.y0 - kVisibleMargin < position)
        position = target.rect.y0 - kVisibleMargin;
    if (target.rect.y1 + kVisibleMargin > position + visible)
        position = target.rect.y1 + kVisibleMargin - visible;
    entry.target = Clamp(position, 0.0f, MaxScroll(target.view));
    entry.mouse = false;
}

void MenuNavigator::Animate()
{
    long long now = static_cast<long long>(System::processTimeInMilliSecs());
    float elapsed = m_lastTime == 0 ? 1000.0f : static_cast<float>(now - m_lastTime);
    elapsed = Clamp(elapsed, 0.0f, kMaxFrameMilliseconds);
    m_lastTime = now;
    m_elapsed = elapsed;
    m_pulse = static_cast<float>(now % kPulseMilliseconds) / kPulseMilliseconds;
    for (size_t view = 0; view < m_views.size(); ++view)
    {
        View& entry = m_views[view];
        entry.target = Clamp(entry.target, 0.0f, MaxScroll(static_cast<int>(view)));
        entry.position = MenuApproach(entry.position, entry.target, elapsed, entry.timeConstant);
        if (fabs(entry.position - entry.target) < 0.05f)
            entry.position = entry.target;
    }
}

float MenuNavigator::Pulse() const
{
    return m_pulse;
}

float MenuNavigator::Elapsed() const
{
    return m_elapsed;
}

void MenuNavigator::Flash(int kind, int index)
{
    m_flashKind = kind;
    m_flashIndex = index;
    m_flashTime = static_cast<long long>(System::processTimeInMilliSecs());
}

float MenuNavigator::FlashAmount(int kind, int index) const
{
    long long since = static_cast<long long>(System::processTimeInMilliSecs()) - m_flashTime;
    if (m_flashKind != kind || m_flashIndex != index || since >= kFlashMilliseconds)
        return 0.0f;
    return 1.0f - static_cast<float>(since) / kFlashMilliseconds;
}
