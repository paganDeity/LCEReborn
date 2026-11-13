#include "game/core/HudOverlay.hpp"

#include <vector>

#include "game/client/render/Font.hpp"
#include "game/client/render/Renderer.hpp"
#include "game/core/ConsoleUIController.hpp"
#include "game/core/DebugOverlay.hpp"
#include "game/core/GlStateManager.hpp"
#include "game/core/Gui.hpp"
#include "game/core/Minecraft.hpp"

extern ConsoleUIController gConsoleUIController;

namespace
{
const float kPixelsPerUnit = 2.0f;

bool sDrawnOverHud[4];

std::vector<HudOverlayLayer*>& Layers()
{
    static std::vector<HudOverlayLayer*> layers;
    return layers;
}

void DrawLayers(int pad, Gui& gui, Font& font, int width, int height, float pixelsPerUnit, float originX,
                float originY, float unit)
{
    const float scale = kPixelsPerUnit / pixelsPerUnit;
    HudOverlaySpace space;
    space.width = static_cast<int>(static_cast<float>(width) / scale);
    space.height = static_cast<int>(static_cast<float>(height) / scale);
    if (unit > 0.0f)
    {
        space.originX = originX;
        space.originY = originY;
        space.unit = unit * scale;
    }
    else
    {
        space.originX = 0.0f;
        space.originY = 0.0f;
        space.unit = static_cast<float>(gConsoleUIController.getScreenHeight()) / static_cast<float>(space.height);
    }

    GlStateManager::pushMatrix();
    GlStateManager::scalef(scale, scale, 1.0f);
    GlStateManager::disableDepthTest();
    GlStateManager::enableBlend();
    GlStateManager::blendFunc(4, 5);
    const std::vector<HudOverlayLayer*>& layers = Layers();
    for (size_t index = 0; index < layers.size(); ++index)
        layers[index]->Draw(pad, gui, font, space);
    GlStateManager::enableDepthTest();
    GlStateManager::popMatrix();
}
}

namespace HudOverlay
{
void Add(HudOverlayLayer* layer)
{
    Layers().push_back(layer);
}

void Render(int pad, Gui& gui, Font& font, int width, int height, float pixelsPerUnit)
{
    if (pad < 0 || pad >= 4)
        return;
    DrawLayers(pad, gui, font, width, height, pixelsPerUnit, 0.0f, 0.0f, 0.0f);
}

bool DrawnOverHud(int pad)
{
    if (pad < 0 || pad >= 4)
        return false;
    const bool drawn = sDrawnOverHud[pad];
    sDrawnOverHud[pad] = false;
    return drawn;
}

void RenderOverHud(int pad, int viewport, int width, int height)
{
    Minecraft* minecraft = Minecraft::GetInstance();
    if (pad < 0 || pad >= 4 || minecraft == NULL || minecraft->mGui == NULL || minecraft->mFont == NULL
        || minecraft->mHeight <= 0)
        return;
    sDrawnOverHud[pad] = true;

    float screenWidth = 0.0f;
    float viewportWidth = static_cast<float>(width);
#if LCE_PLATFORM_PC
    const Renderer::ELayout layout = gRenderer->GetLayout();
    const bool fullWindow = viewport == C4JRender::eViewportType_FullScreen;
    if (fullWindow)
    {
        gRenderer->SetLayout(Renderer::ELayout_Screen);
        screenWidth = gConsoleUIController.getScreenHeight() * gRenderer->GetScreenAspect();
        viewportWidth = screenWidth;
    }
#endif
    const float unit = gConsoleUIController.getScreenHeight() / static_cast<float>(minecraft->mHeight);
    int originX = 0;
    int originY = 0;
    DebugOverlay::ViewportOrigin(viewport, &originX, &originY);
    DebugOverlay::BeginScreenDraw(originX, originY, unit, screenWidth);
    GlStateManager::enableBlend();
    GlStateManager::blendFunc(4, 5);
    GlStateManager::color4f(1.0f, 1.0f, 1.0f, 1.0f);
    DrawLayers(pad, *minecraft->mGui, *minecraft->mFont, static_cast<int>(viewportWidth / unit),
               static_cast<int>(static_cast<float>(height) / unit), 1.0f, static_cast<float>(originX),
               static_cast<float>(originY), unit);
    GlStateManager::disableDepthTest();
    DebugOverlay::EndScreenDraw();
#if LCE_PLATFORM_PC
    if (fullWindow)
        gRenderer->SetLayout(layout);
#endif
}
}
