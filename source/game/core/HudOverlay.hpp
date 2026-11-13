#ifndef GAME_CORE_HUDOVERLAY_HPP
#define GAME_CORE_HUDOVERLAY_HPP

class Font;
class Gui;

struct HudOverlaySpace
{
    int width;
    int height;
    float originX;
    float originY;
    float unit;
};

class HudOverlayLayer
{
public:
    virtual ~HudOverlayLayer() {}
    virtual void Draw(int pad, Gui& gui, Font& font, const HudOverlaySpace& space) = 0;
};

namespace HudOverlay
{
void Add(HudOverlayLayer* layer);
void Render(int pad, Gui& gui, Font& font, int width, int height, float pixelsPerUnit);
bool DrawnOverHud(int pad);
void RenderOverHud(int pad, int viewport, int width, int height);
}

#endif
