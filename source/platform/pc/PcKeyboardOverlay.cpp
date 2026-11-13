#include "platform/pc/PcKeyboardOverlay.hpp"

#include <SDL3/SDL.h>

#include <stddef.h>
#include <stdio.h>

#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include "platform/pc/GlApi.hpp"
#include "platform/pc/Log.hpp"
#include "platform/pc/PcInputEvents.hpp"
#include "platform/pc/PcOverlay.hpp"
#include "platform/pc/PcPaths.hpp"
#include "vendor/stb_truetype.hpp"

namespace
{
const Logger sLog("KeyboardOverlay");
const int kAtlasSize = 512;
const int kFirstGlyph = 32;
const int kGlyphCount = 224;
const float kBakeHeight = 32.0f;
const float kReferenceHeight = 720.0f;
const float kTextHeight = 22.0f;
const unsigned long long kCaretPeriod = 1000000000ull;

const char* const kFragmentSource = R"(#version 330 core
in vec2 vTexCoord;
in vec4 vColour;
uniform sampler2D uGlyphs;
out vec4 oColour;
void main()
{
    float coverage = vTexCoord.x < 0.0 ? 1.0 : texture(uGlyphs, vTexCoord).r;
    oColour = vec4(vColour.rgb, vColour.a * coverage);
}
)";

struct Colour
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct OverlayState
{
    PcOverlayState draw;
    bool hasFont;
    GLuint atlas;
    stbtt_bakedchar glyphs[kGlyphCount];
};

OverlayState sOverlay;

bool LoadFont()
{
    std::string path = PcContentRoot() + "/Common/Media/font/Mojangles.ttf";
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file)
    {
        LOG_ERROR(sLog, "Keyboard overlay font missing: %s", path.c_str());
        return false;
    }

    std::vector<unsigned char> fontData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::vector<unsigned char> bitmap(kAtlasSize * kAtlasSize);
    if (stbtt_BakeFontBitmap(fontData.data(), 0, kBakeHeight, bitmap.data(), kAtlasSize, kAtlasSize, kFirstGlyph,
                             kGlyphCount, sOverlay.glyphs)
        <= 0)
    {
        LOG_ERROR(sLog, "Keyboard overlay font atlas too small for %s", path.c_str());
        return false;
    }

    Gl::GenTextures(1, &sOverlay.atlas);
    Gl::BindTexture(Gl::TEXTURE_2D, sOverlay.atlas);
    Gl::PixelStorei(Gl::UNPACK_ALIGNMENT, 1);
    Gl::TexImage2D(Gl::TEXTURE_2D, 0, (GLint)Gl::RED, kAtlasSize, kAtlasSize, 0, Gl::RED, Gl::UNSIGNED_BYTE,
                   bitmap.data());
    Gl::PixelStorei(Gl::UNPACK_ALIGNMENT, 4);
    Gl::TexParameteri(Gl::TEXTURE_2D, Gl::TEXTURE_MIN_FILTER, (GLint)Gl::NEAREST);
    Gl::TexParameteri(Gl::TEXTURE_2D, Gl::TEXTURE_MAG_FILTER, (GLint)Gl::NEAREST);
    Gl::TexParameteri(Gl::TEXTURE_2D, Gl::TEXTURE_WRAP_S, (GLint)Gl::CLAMP_TO_EDGE);
    Gl::TexParameteri(Gl::TEXTURE_2D, Gl::TEXTURE_WRAP_T, (GLint)Gl::CLAMP_TO_EDGE);
    return true;
}

void Initialise()
{
    if (!PcOverlayInitialise(&sOverlay.draw, kFragmentSource, "uGlyphs", "Keyboard overlay"))
        return;
    sOverlay.hasFont = LoadFont();
}

void PushRect(std::vector<PcOverlayVertex>& vertices, float x0, float y0, float x1, float y1, Colour colour)
{
    PcOverlayPushQuad(vertices, x0, y0, x1, y1, -1.0f, -1.0f, -1.0f, -1.0f, colour.r, colour.g, colour.b, colour.a);
}

int GlyphIndex(wchar_t character)
{
    unsigned int code = static_cast<unsigned int>(character);
    if (code < static_cast<unsigned int>(kFirstGlyph) || code >= static_cast<unsigned int>(kFirstGlyph + kGlyphCount))
        code = '?';
    return static_cast<int>(code) - kFirstGlyph;
}

float TextWidth(const std::wstring& text, size_t begin, float scale)
{
    float width = 0.0f;
    for (size_t index = begin; index < text.size(); ++index)
        width += sOverlay.glyphs[GlyphIndex(text[index])].xadvance * scale;
    return width;
}

float PushText(std::vector<PcOverlayVertex>& vertices, const std::wstring& text, size_t begin, float x, float baseline,
               float scale, Colour colour)
{
    for (size_t index = begin; index < text.size(); ++index)
    {
        const stbtt_bakedchar& glyph = sOverlay.glyphs[GlyphIndex(text[index])];
        float x0 = x + glyph.xoff * scale;
        float y0 = baseline + glyph.yoff * scale;
        float x1 = x0 + (glyph.x1 - glyph.x0) * scale;
        float y1 = y0 + (glyph.y1 - glyph.y0) * scale;
        PcOverlayPushQuad(vertices, x0, y0, x1, y1, glyph.x0 / (float)kAtlasSize, glyph.y0 / (float)kAtlasSize,
                          glyph.x1 / (float)kAtlasSize, glyph.y1 / (float)kAtlasSize, colour.r, colour.g, colour.b,
                          colour.a);
        x += glyph.xadvance * scale;
    }
    return x;
}

void BuildOverlay(std::vector<PcOverlayVertex>& vertices, const std::wstring& title, const std::wstring& text,
                  float width, float height)
{
    const Colour dim = {0, 0, 0, 150};
    const Colour border = {0, 0, 0, 255};
    const Colour panel = {198, 198, 198, 255};
    const Colour highlight = {255, 255, 255, 255};
    const Colour shade = {85, 85, 85, 255};
    const Colour titleColour = {64, 64, 64, 255};
    const Colour focus = {255, 255, 0, 255};
    const Colour field = {108, 108, 108, 255};
    const Colour textColour = {224, 224, 224, 255};

    float unit = height / kReferenceHeight;
    float textScale = kTextHeight * unit / kBakeHeight;
    float edge = 2.0f * unit;
    float padding = 20.0f * unit;
    float lineHeight = kTextHeight * unit * 1.4f;

    float panelWidth = width * 0.5f < 640.0f * unit ? 640.0f * unit : width * 0.5f;
    if (panelWidth > width - 2.0f * padding)
        panelWidth = width - 2.0f * padding;
    float fieldHeight = lineHeight + 2.0f * edge + 8.0f * unit;
    float panelHeight = padding * 3.0f + lineHeight + fieldHeight;
    float left = (width - panelWidth) * 0.5f;
    float top = height * 0.4f - panelHeight * 0.5f;
    float right = left + panelWidth;
    float bottom = top + panelHeight;

    PushRect(vertices, 0.0f, 0.0f, width, height, dim);
    PushRect(vertices, left - edge, top - edge, right + edge, bottom + edge, border);
    PushRect(vertices, left, top, right, bottom, panel);
    PushRect(vertices, left, top, right - edge, top + edge, highlight);
    PushRect(vertices, left, top, left + edge, bottom - edge, highlight);
    PushRect(vertices, left + edge, bottom - edge, right, bottom, shade);
    PushRect(vertices, right - edge, top + edge, right, bottom, shade);

    float fieldLeft = left + padding;
    float fieldRight = right - padding;
    float fieldTop = top + padding * 2.0f + lineHeight;
    float fieldBottom = fieldTop + fieldHeight;
    PushRect(vertices, fieldLeft - edge, fieldTop - edge, fieldRight + edge, fieldBottom + edge, focus);
    PushRect(vertices, fieldLeft, fieldTop, fieldRight, fieldBottom, field);

    if (!sOverlay.hasFont)
        return;

    PushText(vertices, title, 0, left + padding, top + padding + kTextHeight * unit, textScale, titleColour);

    std::wstring shown = text;
    if ((SDL_GetTicksNS() % kCaretPeriod) < kCaretPeriod / 2)
        shown.push_back(L'_');
    float available = fieldRight - fieldLeft - 2.0f * padding * 0.5f;
    size_t begin = 0;
    while (begin < shown.size() && TextWidth(shown, begin, textScale) > available)
        ++begin;
    float baseline = fieldTop + (fieldHeight + kTextHeight * unit * 0.8f) * 0.5f;
    PushText(vertices, shown, begin, fieldLeft + padding * 0.5f, baseline, textScale, textColour);
}
}

void PcKeyboardOverlayDraw(int width, int height)
{
    std::wstring title;
    std::wstring text;
    if (width <= 0 || height <= 0 || !PcInputGetKeyboardText(&title, &text))
        return;

    if (!sOverlay.draw.initialised)
        Initialise();
    if (!sOverlay.draw.available)
        return;

    std::vector<PcOverlayVertex> vertices;
    BuildOverlay(vertices, title, text, (float)width, (float)height);
    PcOverlayBegin(sOverlay.draw, 0, width, height, true);
    PcOverlayBatch(sOverlay.atlas, vertices);
    PcOverlayEnd();
}

void PcKeyboardOverlayShutdown()
{
    if (!sOverlay.draw.initialised)
        return;
    if (sOverlay.atlas != 0)
        Gl::DeleteTextures(1, &sOverlay.atlas);
    PcOverlayShutdown(&sOverlay.draw);
    sOverlay = OverlayState();
}
