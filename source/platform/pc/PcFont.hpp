#ifndef PLATFORM_PC_PCFONT_HPP
#define PLATFORM_PC_PCFONT_HPP

#include <string>
#include <vector>

#include "platform/pc/GlApi.hpp"
#include "platform/pc/PcOverlay.hpp"

class PcFont
{
public:
    PcFont();

    bool Load();
    bool Available() const;
    void Destroy();
    float Width(const std::wstring& text) const;
    void Draw(std::vector<PcOverlayVertex>& vertices, const std::wstring& text, float x, float y, float scale,
              unsigned char shade, unsigned char alpha);
    GLuint Texture();

private:
    enum
    {
        GLYPH_CELLS = 16,
        GLYPH_TEXELS = 8
    };

    static int GlyphFor(wchar_t character);

    bool mLoaded;
    std::vector<unsigned char> mPixels;
    int mWidth;
    int mHeight;
    int mAdvances[GLYPH_CELLS * GLYPH_CELLS];
    GLuint mTexture;
};

#endif
