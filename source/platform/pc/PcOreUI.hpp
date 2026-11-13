#ifndef PLATFORM_PC_PCOREUI_HPP
#define PLATFORM_PC_PCOREUI_HPP

#include <string>
#include <vector>

#include "platform/pc/GlApi.hpp"
#include "platform/pc/PcFont.hpp"
#include "platform/pc/PcOverlay.hpp"

struct PcOreRect
{
    float left;
    float top;
    float right;
    float bottom;
};

struct PcOreColour
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
};

struct PcOreHit
{
    PcOreRect rect;
    int id;
};

struct PcOreCanvas
{
    int width;
    int height;
    float rem;
    std::vector<PcOverlayVertex> rectangles;
    std::vector<PcOverlayVertex> images;
    std::vector<PcOverlayVertex> glyphs;
    std::vector<PcOreHit> hits;
    GLuint imageTexture;

    PcOreCanvas();

    void Hit(PcOreRect rect, int id);
    int HitAt(float pointerX, float pointerY) const;
};

enum EPcOreVariant
{
    EPcOreVariant_Hero,
    EPcOreVariant_Primary,
    EPcOreVariant_Secondary,
    EPcOreVariant_Neutral,
    EPcOreVariant_Destructive
};

struct PcOreButtonState
{
    bool hovered;
    bool pressed;
    bool focused;
    bool enabled;
};

class PcOrePointer
{
public:
    PcOrePointer();

    void Reset();
    bool Update(int player, float* x, float* y);

private:
    bool mHeld;
};

class PcOreUI
{
public:
    static float Rem(int width, int height);
    static float LineHeight(float rem, float size);

    static void Overlay(PcOreCanvas& canvas, unsigned char alpha);
    static void Fill(PcOreCanvas& canvas, PcOreRect rect, PcOreColour colour);
    static void Frame(PcOreCanvas& canvas, PcOreRect rect, float width, PcOreColour colour);
    static void Panel(PcOreCanvas& canvas, PcOreRect rect);
    static void ContentPanel(PcOreCanvas& canvas, PcOreRect rect);
    static void Row(PcOreCanvas& canvas, PcOreRect rect, bool nested, bool hovered, bool focused);
    static void Text(PcOreCanvas& canvas, const std::wstring& text, float x, float y, float size, PcOreColour colour,
                     bool shadow);
    static void TextIn(PcOreCanvas& canvas, const std::wstring& text, PcOreRect rect, float size, PcOreColour colour,
                       bool shadow);
    static void TextCentred(PcOreCanvas& canvas, const std::wstring& text, PcOreRect rect, float size,
                            PcOreColour colour, bool shadow);
    static float TextWidth(const PcOreCanvas& canvas, const std::wstring& text, float size);

    static void Button(PcOreCanvas& canvas, PcOreRect rect, EPcOreVariant variant, const std::wstring& label,
                       const PcOreButtonState& state);
    static void Toggle(PcOreCanvas& canvas, PcOreRect rect, bool on, const PcOreButtonState& state);
    static void Slider(PcOreCanvas& canvas, PcOreRect rect, float fraction, const PcOreButtonState& state);
    static void Chevron(PcOreCanvas& canvas, PcOreRect rect, PcOreColour colour, bool pointsRight);
    static void Character(PcOreCanvas& canvas, PcOreRect rect);
    static bool Logo(PcOreCanvas& canvas, PcOreRect rect);
    static float LogoAspect();

    static void Present(PcOreCanvas& canvas);

    static PcOreColour Colour(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);

    static const PcOreColour TEXT;
    static const PcOreColour TEXT_DIMMER;
    static const PcOreColour TEXT_DIMMEST;
    static const PcOreColour TEXT_DARK;
    static const PcOreColour BORDER;
    static const PcOreColour OUTLINE;

private:
    static bool Ready();
    static PcFont sFont;
    static PcOverlayState sOverlay;
    static GLuint sWhite;
    static GLuint sLogo;
    static bool sLogoAttempted;
    static int sLogoWidth;
    static int sLogoHeight;
    static int sLogoTextureWidth;
    static int sLogoTextureHeight;
};

#endif
