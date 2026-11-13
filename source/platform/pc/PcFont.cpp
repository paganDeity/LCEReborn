#include "platform/pc/PcFont.hpp"

#include <stb_image.h>

#include <cmath>
#include <cstring>

#include "platform/pc/GlApi.hpp"
#include "platform/pc/Log.hpp"
#include "platform/pc/PcPaths.hpp"

namespace
{
const Logger sLog("Font");

const unsigned short kHighGlyphs[] = {0x00C7, 0x00FC, 0x00E9, 0x00E2, 0x00E4, 0x00E0, 0x00E5, 0x00E7, 0x00EA, 0x00EB,
                                      0x00E8, 0x00EF, 0x00EE, 0x00EC, 0x00C4, 0x00C5, 0x00C9, 0x00E6, 0x00C6, 0x00F4,
                                      0x00F6, 0x00F2, 0x00FB, 0x00F9, 0x00FF, 0x00D6, 0x00DC, 0x00F8, 0x00A3, 0x00D8,
                                      0x00D7, 0x0192, 0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x00F1, 0x00D1, 0x00AA, 0x00BA,
                                      0x00BF, 0x00AE, 0x00AC, 0x00BD, 0x00BC, 0x00A1, 0x00AB, 0x00BB};
}

PcFont::PcFont() : mLoaded(false), mWidth(0), mHeight(0), mTexture(0)
{
    std::memset(mAdvances, 0, sizeof(mAdvances));
}

int PcFont::GlyphFor(wchar_t character)
{
    if (character >= 32 && character < 127)
        return (int)character;
    for (size_t index = 0; index < sizeof(kHighGlyphs) / sizeof(kHighGlyphs[0]); ++index)
    {
        if ((wchar_t)kHighGlyphs[index] == character)
            return 0x80 + (int)index;
    }
    return -1;
}

bool PcFont::Load()
{
    if (mLoaded)
        return !mPixels.empty();
    mLoaded = true;
    const std::string path = PcContentRoot() + "/Common/res/font/default.png";
    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char* pixels = stbi_load(path.c_str(), &width, &height, &channels, 4);
    if (pixels == NULL || width % GLYPH_CELLS != 0 || height % GLYPH_CELLS != 0)
    {
        LOG_ERROR(sLog, "Font %s could not be loaded", path.c_str());
        if (pixels != NULL)
            stbi_image_free(pixels);
        return false;
    }
    mWidth = width;
    mHeight = height;
    mPixels.assign(pixels, pixels + width * height * 4);
    stbi_image_free(pixels);
    const int cellWidth = width / GLYPH_CELLS;
    const int cellHeight = height / GLYPH_CELLS;
    for (int glyph = 0; glyph < GLYPH_CELLS * GLYPH_CELLS; ++glyph)
    {
        const int left = (glyph % GLYPH_CELLS) * cellWidth;
        const int top = (glyph / GLYPH_CELLS) * cellHeight;
        int right = -1;
        for (int y = 0; y < cellHeight; ++y)
        {
            for (int x = 0; x < cellWidth; ++x)
            {
                if (mPixels[((top + y) * width + left + x) * 4 + 3] != 0 && x > right)
                    right = x;
            }
        }
        mAdvances[glyph] = right < 0 ? 0 : right + 2;
    }
    mAdvances[' '] = cellWidth / 2;
    return true;
}

bool PcFont::Available() const
{
    return !mPixels.empty();
}

void PcFont::Destroy()
{
    if (mTexture != 0)
        Gl::DeleteTextures(1, &mTexture);
    mTexture = 0;
    mPixels.clear();
    mLoaded = false;
    mWidth = 0;
    mHeight = 0;
}

float PcFont::Width(const std::wstring& text) const
{
    if (mPixels.empty() || text.empty())
        return -1.0f;
    const float texel = (float)GLYPH_TEXELS / (float)(mWidth / GLYPH_CELLS);
    float width = 0.0f;
    for (size_t index = 0; index < text.size(); ++index)
    {
        const int glyph = GlyphFor(text[index]);
        if (glyph < 0)
            return -1.0f;
        width += mAdvances[glyph] * texel;
    }
    return width - texel;
}

void PcFont::Draw(std::vector<PcOverlayVertex>& vertices, const std::wstring& text, float x, float y, float scale,
                  unsigned char shade, unsigned char alpha)
{
    if (mPixels.empty())
        return;
    const int cellWidth = mWidth / GLYPH_CELLS;
    const int cellHeight = mHeight / GLYPH_CELLS;
    const float texel = scale * (float)GLYPH_TEXELS / (float)cellWidth;
    for (size_t index = 0; index < text.size(); ++index)
    {
        const int glyph = GlyphFor(text[index]);
        if (glyph < 0)
            continue;
        const int advance = mAdvances[glyph];
        if (advance > 1)
        {
            const float u0 = (float)((glyph % GLYPH_CELLS) * cellWidth) / (float)mWidth;
            const float v0 = (float)((glyph / GLYPH_CELLS) * cellHeight) / (float)mHeight;
            const float u1 = u0 + (float)(advance - 1) / (float)mWidth;
            const float v1 = v0 + (float)cellHeight / (float)mHeight;
            const float right = x + (advance - 1) * texel;
            const float bottom = y + cellHeight * texel;
            PcOverlayPushQuad(vertices, x, y, right, bottom, u0, v0, u1, v1, shade, shade, shade, alpha);
        }
        x += advance * texel;
    }
}

GLuint PcFont::Texture()
{
    if (mTexture != 0 || mPixels.empty())
        return mTexture;
    Gl::GenTextures(1, &mTexture);
    Gl::BindTexture(Gl::TEXTURE_2D, mTexture);
    Gl::PixelStorei(Gl::UNPACK_ALIGNMENT, 4);
    Gl::TexImage2D(Gl::TEXTURE_2D, 0, (GLint)Gl::RGBA, mWidth, mHeight, 0, Gl::RGBA, Gl::UNSIGNED_BYTE,
                   mPixels.data());
    Gl::TexParameteri(Gl::TEXTURE_2D, Gl::TEXTURE_MIN_FILTER, (GLint)Gl::NEAREST);
    Gl::TexParameteri(Gl::TEXTURE_2D, Gl::TEXTURE_MAG_FILTER, (GLint)Gl::NEAREST);
    Gl::TexParameteri(Gl::TEXTURE_2D, Gl::TEXTURE_WRAP_S, (GLint)Gl::CLAMP_TO_EDGE);
    Gl::TexParameteri(Gl::TEXTURE_2D, Gl::TEXTURE_WRAP_T, (GLint)Gl::CLAMP_TO_EDGE);
    return mTexture;
}
