#include "platform/pc/PcOreUI.hpp"

#include <stb_image.h>

#include <cmath>

#include "platform/CInput.hpp"
#include "platform/pc/Log.hpp"
#include "platform/pc/PcPaths.hpp"

namespace
{
const Logger sLog("OreUI");

const float kPointerWidth = 1280.0f;
const float kPointerHeight = 720.0f;

const PcOreColour kNeutral = {0x48, 0x49, 0x4a, 255};
const PcOreColour kNeutralHovered = {0x58, 0x58, 0x5a, 255};
const PcOreColour kNeutralPressed = {0x31, 0x32, 0x33, 255};
const PcOreColour kNeutral80 = {0x31, 0x32, 0x33, 255};
const PcOreColour kNeutral80Hovered = {0x48, 0x49, 0x4a, 255};
const PcOreColour kNeutral80Pressed = {0x24, 0x24, 0x25, 255};
const PcOreColour kNeutral20 = {0xe6, 0xe8, 0xeb, 255};
const PcOreColour kNeutral20Hovered = {0xf4, 0xf6, 0xf9, 255};
const PcOreColour kNeutral20Pressed = {0xd0, 0xd1, 0xd4, 255};
const PcOreColour kPrimary = {0x3c, 0x85, 0x27, 255};
const PcOreColour kPrimaryHovered = {0x2a, 0x64, 0x1c, 255};
const PcOreColour kPrimaryPressed = {0x1d, 0x4d, 0x13, 255};
const PcOreColour kSecondary = {0xd0, 0xd1, 0xd4, 255};
const PcOreColour kSecondaryHovered = {0xb1, 0xb2, 0xb5, 255};
const PcOreColour kDestructive = {0xca, 0x36, 0x36, 255};
const PcOreColour kDestructiveHovered = {0xc0, 0x2d, 0x2d, 255};
const PcOreColour kDestructivePressed = {0xad, 0x1d, 0x1d, 255};
const PcOreColour kShadow = {0x1d, 0x4d, 0x13, 255};
const PcOreColour kSecondaryShadow = {0x58, 0x58, 0x5a, 255};
const PcOreColour kDisabled = {0xb1, 0xb2, 0xb5, 255};
const PcOreColour kDisabledText = {0x58, 0x58, 0x5a, 255};
const PcOreColour kSpecularTop = {255, 255, 255, 51};
const PcOreColour kSpecularBottom = {255, 255, 255, 26};
const PcOreColour kTrack = {0x24, 0x24, 0x25, 255};
const PcOreColour kKnob = {0xd0, 0xd1, 0xd4, 255};
const PcOreColour kBevelLight = {255, 255, 255, 26};
const PcOreColour kBevelDark = {0, 0, 0, 77};
}

const PcOreColour PcOreUI::TEXT = {255, 255, 255, 255};
const PcOreColour PcOreUI::TEXT_DIMMER = {0xd0, 0xd1, 0xd4, 255};
const PcOreColour PcOreUI::TEXT_DIMMEST = {0xb1, 0xb2, 0xb5, 255};
const PcOreColour PcOreUI::TEXT_DARK = {0x1e, 0x1e, 0x1f, 255};
const PcOreColour PcOreUI::BORDER = {0x1e, 0x1e, 0x1f, 255};
const PcOreColour PcOreUI::OUTLINE = {255, 255, 255, 255};

PcOreCanvas::PcOreCanvas() : width(0), height(0), rem(0.0f), imageTexture(0)
{
}

PcFont PcOreUI::sFont;
PcOverlayState PcOreUI::sOverlay;
GLuint PcOreUI::sWhite = 0;
GLuint PcOreUI::sLogo = 0;
bool PcOreUI::sLogoAttempted = false;
int PcOreUI::sLogoWidth = 0;
int PcOreUI::sLogoHeight = 0;
int PcOreUI::sLogoTextureWidth = 0;
int PcOreUI::sLogoTextureHeight = 0;

void PcOreCanvas::Hit(PcOreRect rect, int id)
{
    PcOreHit hit;
    hit.rect = rect;
    hit.id = id;
    hits.push_back(hit);
}

int PcOreCanvas::HitAt(float pointerX, float pointerY) const
{
    if (width <= 0 || height <= 0)
        return -1;
    const float x = pointerX / kPointerWidth * static_cast<float>(width);
    const float y = pointerY / kPointerHeight * static_cast<float>(height);
    for (size_t index = hits.size(); index > 0; --index)
    {
        const PcOreRect& rect = hits[index - 1].rect;
        if (x >= rect.left && x < rect.right && y >= rect.top && y < rect.bottom)
            return hits[index - 1].id;
    }
    return -1;
}

PcOrePointer::PcOrePointer() : mHeld(false)
{
}

void PcOrePointer::Reset()
{
    mHeld = false;
}

bool PcOrePointer::Update(int player, float* x, float* y)
{
    const bool available = g_pInput->GetMenuPointer(player, x, y);
    const bool held = available && g_pInput->MenuPointerHeld(player);
    const bool released = available && mHeld && !held;
    mHeld = held;
    return released;
}

float PcOreUI::Rem(int width, int height)
{
    const int byWidth = width / 320;
    const int byHeight = height / 240;
    int scale = byWidth < byHeight ? byWidth : byHeight;
    if (scale < 1)
        scale = 1;
    if (scale > 4)
        scale = 4;
    return static_cast<float>(scale) * 5.0f;
}

float PcOreUI::LineHeight(float rem, float size)
{
    return rem * size * 1.25f;
}

PcOreColour PcOreUI::Colour(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
    PcOreColour colour;
    colour.red = red;
    colour.green = green;
    colour.blue = blue;
    colour.alpha = alpha;
    return colour;
}

bool PcOreUI::Ready()
{
    if (!sOverlay.initialised)
    {
        static const char* const kFragment = R"(#version 330 core
in vec2 vTexCoord;
in vec4 vColour;
uniform sampler2D uArt;
out vec4 oColour;
void main()
{
    oColour = texture(uArt, vTexCoord) * vColour;
}
)";
        if (!PcOverlayInitialise(&sOverlay, kFragment, "uArt", "Ore interface"))
            return false;
    }
    if (sWhite == 0)
    {
        const unsigned char white[4] = {255, 255, 255, 255};
        Gl::GenTextures(1, &sWhite);
        Gl::BindTexture(Gl::TEXTURE_2D, sWhite);
        Gl::PixelStorei(Gl::UNPACK_ALIGNMENT, 4);
        Gl::TexImage2D(Gl::TEXTURE_2D, 0, (GLint)Gl::RGBA, 1, 1, 0, Gl::RGBA, Gl::UNSIGNED_BYTE, white);
        Gl::TexParameteri(Gl::TEXTURE_2D, Gl::TEXTURE_MIN_FILTER, (GLint)Gl::NEAREST);
        Gl::TexParameteri(Gl::TEXTURE_2D, Gl::TEXTURE_MAG_FILTER, (GLint)Gl::NEAREST);
        Gl::TexParameteri(Gl::TEXTURE_2D, Gl::TEXTURE_WRAP_S, (GLint)Gl::CLAMP_TO_EDGE);
        Gl::TexParameteri(Gl::TEXTURE_2D, Gl::TEXTURE_WRAP_T, (GLint)Gl::CLAMP_TO_EDGE);
    }
    if (!sLogoAttempted)
    {
        sLogoAttempted = true;
        const std::string path = PcContentRoot() + "/Common/res/gui/logo.png";
        int width = 0;
        int height = 0;
        int channels = 0;
        unsigned char* pixels = stbi_load(path.c_str(), &width, &height, &channels, 4);
        if (pixels != NULL)
        {
            int left = width;
            int top = height;
            int right = 0;
            int bottom = 0;
            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    if (pixels[(y * width + x) * 4 + 3] == 0)
                        continue;
                    if (x < left)
                        left = x;
                    if (y < top)
                        top = y;
                    if (x >= right)
                        right = x + 1;
                    if (y >= bottom)
                        bottom = y + 1;
                }
            }
            if (right > left && bottom > top)
            {
                sLogoWidth = right - left;
                sLogoHeight = bottom - top;
                sLogoTextureWidth = width;
                sLogoTextureHeight = height;
                Gl::GenTextures(1, &sLogo);
                Gl::BindTexture(Gl::TEXTURE_2D, sLogo);
                Gl::PixelStorei(Gl::UNPACK_ALIGNMENT, 4);
                Gl::TexImage2D(Gl::TEXTURE_2D, 0, (GLint)Gl::RGBA, width, height, 0, Gl::RGBA, Gl::UNSIGNED_BYTE,
                               pixels);
                Gl::TexParameteri(Gl::TEXTURE_2D, Gl::TEXTURE_MIN_FILTER, (GLint)Gl::NEAREST);
                Gl::TexParameteri(Gl::TEXTURE_2D, Gl::TEXTURE_MAG_FILTER, (GLint)Gl::NEAREST);
                Gl::TexParameteri(Gl::TEXTURE_2D, Gl::TEXTURE_WRAP_S, (GLint)Gl::CLAMP_TO_EDGE);
                Gl::TexParameteri(Gl::TEXTURE_2D, Gl::TEXTURE_WRAP_T, (GLint)Gl::CLAMP_TO_EDGE);
            }
            stbi_image_free(pixels);
        }
        if (sLogo == 0)
            LOG_WARN(sLog, "Title artwork %s could not be loaded", path.c_str());
    }
    sFont.Load();
    return sOverlay.available && sWhite != 0;
}

void PcOreUI::Overlay(PcOreCanvas& canvas, unsigned char alpha)
{
    Fill(canvas, {0.0f, 0.0f, static_cast<float>(canvas.width), static_cast<float>(canvas.height)},
         Colour(0, 0, 0, alpha));
}

void PcOreUI::Fill(PcOreCanvas& canvas, PcOreRect rect, PcOreColour colour)
{
    if (rect.right <= rect.left || rect.bottom <= rect.top || colour.alpha == 0)
        return;
    PcOverlayPushQuad(canvas.rectangles, rect.left, rect.top, rect.right, rect.bottom, 0.0f, 0.0f, 1.0f, 1.0f,
                      colour.red, colour.green, colour.blue, colour.alpha);
}

void PcOreUI::Frame(PcOreCanvas& canvas, PcOreRect rect, float width, PcOreColour colour)
{
    if (width <= 0.0f || rect.right <= rect.left || rect.bottom <= rect.top)
        return;
    Fill(canvas, {rect.left, rect.top, rect.right, rect.top + width}, colour);
    Fill(canvas, {rect.left, rect.bottom - width, rect.right, rect.bottom}, colour);
    Fill(canvas, {rect.left, rect.top + width, rect.left + width, rect.bottom - width}, colour);
    Fill(canvas, {rect.right - width, rect.top + width, rect.right, rect.bottom - width}, colour);
}

void PcOreUI::Panel(PcOreCanvas& canvas, PcOreRect rect)
{
    Fill(canvas, rect, kNeutral80);
    Frame(canvas, rect, canvas.rem * 0.2f, BORDER);
}

void PcOreUI::ContentPanel(PcOreCanvas& canvas, PcOreRect rect)
{
    Fill(canvas, rect, kNeutral);
    Frame(canvas, rect, canvas.rem * 0.2f, BORDER);
}

void PcOreUI::Row(PcOreCanvas& canvas, PcOreRect rect, bool nested, bool hovered, bool focused)
{
    if (hovered)
        Fill(canvas, rect, nested ? kNeutral80Hovered : kNeutralHovered);
    else
        Fill(canvas, rect, nested ? kNeutral80 : kNeutral);
    const float edge = canvas.rem * 0.2f;
    Fill(canvas, {rect.left, rect.top, rect.right, rect.top + edge}, nested ? BORDER : kBevelLight);
    Fill(canvas, {rect.left, rect.bottom - edge, rect.right, rect.bottom}, nested ? BORDER : kBevelDark);
    if (focused)
        Frame(canvas, rect, edge, OUTLINE);
}

void PcOreUI::Text(PcOreCanvas& canvas, const std::wstring& text, float x, float y, float size, PcOreColour colour,
                   bool shadow)
{
    if (text.empty())
        return;
    const float scale = size * canvas.rem / 8.0f;
    if (shadow)
    {
        const float offset = canvas.rem * 0.1f;
        sFont.Draw(canvas.glyphs, text, x + offset, y + offset, scale, 0, 100);
    }
    sFont.Draw(canvas.glyphs, text, x, y, scale, colour.red, colour.alpha);
}

void PcOreUI::TextIn(PcOreCanvas& canvas, const std::wstring& text, PcOreRect rect, float size, PcOreColour colour,
                     bool shadow)
{
    const float height = size * canvas.rem;
    const float y = rect.top + (rect.bottom - rect.top - height) * 0.5f;
    Text(canvas, text, rect.left, y, size, colour, shadow);
}

void PcOreUI::TextCentred(PcOreCanvas& canvas, const std::wstring& text, PcOreRect rect, float size,
                          PcOreColour colour, bool shadow)
{
    const float width = TextWidth(canvas, text, size);
    const float height = size * canvas.rem;
    const float x = rect.left + (rect.right - rect.left - width) * 0.5f;
    const float y = rect.top + (rect.bottom - rect.top - height) * 0.5f;
    Text(canvas, text, x, y, size, colour, shadow);
}

float PcOreUI::TextWidth(const PcOreCanvas& canvas, const std::wstring& text, float size)
{
    const float scale = size * canvas.rem / 8.0f;
    return sFont.Width(text) * scale;
}

void PcOreUI::Button(PcOreCanvas& canvas, PcOreRect rect, EPcOreVariant variant, const std::wstring& label,
                     const PcOreButtonState& state)
{
    PcOreColour fill;
    PcOreColour hovered;
    PcOreColour pressed;
    PcOreColour shadow;
    PcOreColour text;
    switch (variant)
    {
    case EPcOreVariant_Hero:
    case EPcOreVariant_Primary:
        fill = kPrimary;
        hovered = kPrimaryHovered;
        pressed = kPrimaryPressed;
        shadow = kShadow;
        text = TEXT;
        break;
    case EPcOreVariant_Secondary:
        fill = kSecondary;
        hovered = kSecondaryHovered;
        pressed = kSecondaryHovered;
        shadow = kSecondaryShadow;
        text = TEXT_DARK;
        break;
    case EPcOreVariant_Neutral:
        fill = kNeutral;
        hovered = kNeutralHovered;
        pressed = kNeutralPressed;
        shadow = kNeutralPressed;
        text = TEXT;
        break;
    default:
        fill = kDestructive;
        hovered = kDestructiveHovered;
        pressed = kDestructivePressed;
        shadow = kDestructivePressed;
        text = TEXT;
        break;
    }
    if (!state.enabled)
    {
        fill = kDisabled;
        hovered = kDisabled;
        pressed = kDisabled;
        shadow = kDisabled;
        text = kDisabledText;
    }

    const float edge = canvas.rem * 0.2f;
    const float depth = canvas.rem * 0.4f;
    const float sink = state.pressed && state.enabled ? depth : 0.0f;
    PcOreRect outer = {rect.left, rect.top + sink, rect.right, rect.bottom};
    Fill(canvas, outer, BORDER);
    PcOreRect inner = {outer.left + edge, outer.top + edge, outer.right - edge, outer.bottom - edge};
    PcOreRect face = {inner.left, inner.top, inner.right, inner.bottom - depth};
    Fill(canvas, {face.left, face.bottom, face.right, inner.bottom}, shadow);
    PcOreColour faceColour = fill;
    if (state.enabled && state.pressed)
        faceColour = pressed;
    else if (state.enabled && state.hovered)
        faceColour = hovered;
    Fill(canvas, face, faceColour);
    Fill(canvas, {face.left, face.top, face.right, face.top + edge}, kSpecularTop);
    Fill(canvas, {face.left, face.bottom - edge, face.right, face.bottom}, kSpecularBottom);
    if (state.focused)
        Frame(canvas, {outer.left - edge, outer.top - edge, outer.right + edge, outer.bottom + edge}, edge, OUTLINE);

    const float size = variant == EPcOreVariant_Hero ? 2.0f : 1.6f;
    TextCentred(canvas, label, face, size, text, variant == EPcOreVariant_Hero);
}

void PcOreUI::Toggle(PcOreCanvas& canvas, PcOreRect rect, bool on, const PcOreButtonState& state)
{
    const float edge = canvas.rem * 0.2f;
    Fill(canvas, rect, BORDER);
    PcOreRect inner = {rect.left + edge, rect.top + edge, rect.right - edge, rect.bottom - edge};
    const float middle = (inner.left + inner.right) * 0.5f;
    Fill(canvas, inner, kTrack);
    if (on)
        Fill(canvas, {inner.left, inner.top, middle, inner.bottom}, kPrimary);
    if (state.enabled)
    {
        Fill(canvas, {inner.left, inner.top, inner.right, inner.top + edge}, kSpecularTop);
        Fill(canvas, {inner.left, inner.bottom - edge, inner.right, inner.bottom}, kSpecularBottom);
    }
    if (state.focused)
        Frame(canvas, {rect.left - edge, rect.top - edge, rect.right + edge, rect.bottom + edge}, edge, OUTLINE);
    const float knob = rect.bottom - rect.top - edge * 2.0f;
    const float knobLeft = on ? inner.left + edge : inner.right - edge - knob;
    PcOreRect knobRect = {knobLeft, inner.top, knobLeft + knob, inner.bottom};
    Fill(canvas, knobRect, kKnob);
    Frame(canvas, knobRect, edge, BORDER);
}

void PcOreUI::Slider(PcOreCanvas& canvas, PcOreRect rect, float fraction, const PcOreButtonState& state)
{
    if (fraction < 0.0f)
        fraction = 0.0f;
    if (fraction > 1.0f)
        fraction = 1.0f;
    const float edge = canvas.rem * 0.2f;
    const float middle = (rect.top + rect.bottom) * 0.5f;
    const float railHeight = canvas.rem * 1.2f;
    PcOreRect rail = {rect.left, middle - railHeight * 0.5f, rect.right, middle + railHeight * 0.5f};
    Fill(canvas, rail, BORDER);
    PcOreRect inner = {rail.left + edge, rail.top + edge, rail.right - edge, rail.bottom - edge};
    Fill(canvas, inner, kTrack);
    Fill(canvas, {inner.left, inner.top, inner.right, inner.top + edge}, kSpecularTop);
    Fill(canvas, {inner.left, inner.bottom - edge, inner.right, inner.bottom}, kSpecularBottom);
    const float end = inner.left + (inner.right - inner.left) * fraction;
    if (end > inner.left)
        Fill(canvas, {inner.left, inner.top, end, inner.bottom}, kPrimary);
    const float thumb = canvas.rem * 3.2f;
    const float centre = inner.left + (inner.right - inner.left) * fraction;
    const float thumbLeft = centre - thumb * 0.5f;
    PcOreRect knob = {thumbLeft, middle - thumb * 0.5f, thumbLeft + thumb, middle + thumb * 0.5f};
    Fill(canvas, knob, kKnob);
    Frame(canvas, knob, edge, BORDER);
    if (state.focused)
        Frame(canvas, {rail.left - edge, rail.top - edge, rail.right + edge, rail.bottom + edge}, edge, OUTLINE);
}

void PcOreUI::Chevron(PcOreCanvas& canvas, PcOreRect rect, PcOreColour colour, bool pointsRight)
{
    const float pixel = canvas.rem * 0.2f;
    const float mid = (rect.top + rect.bottom) * 0.5f;
    for (int step = 0; step < 4; ++step)
    {
        const float x = pointsRight ? rect.right - static_cast<float>(step + 1) * pixel
                                    : rect.left + static_cast<float>(step) * pixel;
        const float y = mid - static_cast<float>(step + 1) * pixel;
        Fill(canvas, {x, y, x + pixel, y + pixel}, colour);
        Fill(canvas, {x, mid + static_cast<float>(step) * pixel, x + pixel, mid + static_cast<float>(step + 1) * pixel},
             colour);
    }
}

void PcOreUI::Character(PcOreCanvas& canvas, PcOreRect rect)
{
    const float unit = std::fmin((rect.right - rect.left) / 20.0f, (rect.bottom - rect.top) / 36.0f);
    if (unit <= 0.0f)
        return;
    const float modelWidth = 16.0f * unit;
    const float modelHeight = 32.0f * unit;
    const float ox = (rect.left + rect.right) * 0.5f - modelWidth * 0.5f;
    const float oy = (rect.top + rect.bottom) * 0.5f - modelHeight * 0.5f;
    const PcOreColour skin = Colour(0xc6, 0x86, 0x42, 255);
    const PcOreColour hair = Colour(0x2f, 0x1f, 0x14, 255);
    const PcOreColour shirt = Colour(0x00, 0xa8, 0xa8, 255);
    const PcOreColour trousers = Colour(0x3b, 0x3b, 0x8f, 255);
    const PcOreColour shoes = Colour(0x4a, 0x4a, 0x4a, 255);
    Fill(canvas, {ox + 4.0f * unit, oy, ox + 12.0f * unit, oy + 8.0f * unit}, skin);
    Fill(canvas, {ox + 4.0f * unit, oy, ox + 12.0f * unit, oy + 2.0f * unit}, hair);
    Fill(canvas, {ox + 6.0f * unit, oy + 4.0f * unit, ox + 7.0f * unit, oy + 5.0f * unit}, TEXT);
    Fill(canvas, {ox + 9.0f * unit, oy + 4.0f * unit, ox + 10.0f * unit, oy + 5.0f * unit}, TEXT);
    Fill(canvas, {ox + 4.0f * unit, oy + 8.0f * unit, ox + 12.0f * unit, oy + 20.0f * unit}, shirt);
    Fill(canvas, {ox, oy + 8.0f * unit, ox + 4.0f * unit, oy + 20.0f * unit}, skin);
    Fill(canvas, {ox + 12.0f * unit, oy + 8.0f * unit, ox + 16.0f * unit, oy + 20.0f * unit}, skin);
    Fill(canvas, {ox + 4.0f * unit, oy + 20.0f * unit, ox + 8.0f * unit, oy + 32.0f * unit}, trousers);
    Fill(canvas, {ox + 8.0f * unit, oy + 20.0f * unit, ox + 12.0f * unit, oy + 32.0f * unit}, trousers);
    Fill(canvas, {ox + 4.0f * unit, oy + 30.0f * unit, ox + 8.0f * unit, oy + 32.0f * unit}, shoes);
    Fill(canvas, {ox + 8.0f * unit, oy + 30.0f * unit, ox + 12.0f * unit, oy + 32.0f * unit}, shoes);
}

bool PcOreUI::Logo(PcOreCanvas& canvas, PcOreRect rect)
{
    if (!Ready() || sLogo == 0)
        return false;
    const float u1 = static_cast<float>(sLogoWidth) / static_cast<float>(sLogoTextureWidth);
    const float v1 = static_cast<float>(sLogoHeight) / static_cast<float>(sLogoTextureHeight);
    canvas.imageTexture = sLogo;
    PcOverlayPushQuad(canvas.images, rect.left, rect.top, rect.right, rect.bottom, 0.0f, 0.0f, u1, v1, 255, 255,
                      255, 255);
    return true;
}

float PcOreUI::LogoAspect()
{
    if (!Ready() || sLogoHeight <= 0)
        return 0.0f;
    return static_cast<float>(sLogoWidth) / static_cast<float>(sLogoHeight);
}

void PcOreUI::Present(PcOreCanvas& canvas)
{
    if (!Ready())
        return;
    GLint framebuffer = 0;
    Gl::GetIntegerv(Gl::DRAW_FRAMEBUFFER_BINDING, &framebuffer);
    PcOverlayBegin(sOverlay, (GLuint)framebuffer, canvas.width, canvas.height, framebuffer == 0);
    PcOverlayBatch(sWhite, canvas.rectangles);
    if (canvas.imageTexture != 0 && !canvas.images.empty())
        PcOverlayBatch(canvas.imageTexture, canvas.images);
    PcOverlayBatch(sFont.Texture(), canvas.glyphs);
    PcOverlayEnd();
}
