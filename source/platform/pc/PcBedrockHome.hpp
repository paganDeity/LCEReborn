#ifndef PLATFORM_PC_PCBEDROCKHOME_HPP
#define PLATFORM_PC_PCBEDROCKHOME_HPP

#include <string>
#include <vector>

#include "game/ui/UIScene.hpp"
#include "platform/pc/PcOreUI.hpp"

class PcBedrockHome : public UIScene
{
public:
    PcBedrockHome(int player, void* data, UILayer* layer);
    virtual ~PcBedrockHome();

    virtual std::wstring getMoviePath();
    virtual unsigned int mapElementsAndNames();
    virtual bool needsReloaded();
    virtual void reloadMovie(bool force);
    virtual int getSceneType();
    virtual void tick();
    virtual void render(int width, int height, C4JRender::eViewportType viewport);
    virtual void handleInput(int playerId, int action, bool repeat, bool pressed, bool released, bool& handled);

private:
    struct Layout
    {
        float rem;
        bool compact;
        bool compactFooter;
        PcOreRect logo;
        PcOreRect panel;
        PcOreRect character;
        PcOreRect play;
        PcOreRect servers;
        PcOreRect settings;
        PcOreRect skins;
        PcOreRect dressing;
        PcOreRect changeSkin;
        PcOreRect footer;
        float pad;
        float actionPadding;
    };

    Layout ComputeLayout(int width, int height) const;
    PcOreRect ActionRect(const Layout& layout, int index) const;
    int ActionAt(int width, int height, float pointerX, float pointerY) const;
    int ActionCount(const Layout& layout) const;
    void Activate(int index);

    int mSelected;
    int mHovered;
    int mWidth;
    int mHeight;
    PcOrePointer mPointer;
};

#endif
