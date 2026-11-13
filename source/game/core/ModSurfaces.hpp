#ifndef GAME_CORE_MODSURFACES_HPP
#define GAME_CORE_MODSURFACES_HPP

#include <stdint.h>

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "game/core/CriticalSection.hpp"

class BufferedImage;
class Textures;

class ModSurface
{
public:
    ModSurface(const std::string& owner, int width, int height);
    ~ModSurface();

    int Width() const { return mWidth; }
    int Height() const { return mHeight; }
    const std::string& Owner() const { return mOwner; }
    // Replace a rectangle of pixels with width*height*4 bytes, row by row, each pixel red, green, blue, alpha. False if the rectangle is outside the surface or the
    // byte count is wrong.
    bool Write(int x, int y, int width, int height, const std::string& rgba);
    // The pixels of a rectangle, as Write takes them: width*height*4 bytes, row by row, each pixel red, green, blue, alpha. An empty string if the rectangle is
    // outside the surface.
    std::string Read(int x, int y, int width, int height);
    bool Load(const std::string& image);
    void SnapshotArgb(std::vector<int>* argb);
    int Texture(Textures* textures);
    // Free the surface now.
    void Destroy();

private:
    std::string mOwner;
    int mWidth;
    int mHeight;
    std::vector<int> mPixels;
    bool mDirty;
    int mTexture;
    BufferedImage* mImage;
    C4JCriticalSection mLock;
};

namespace ModSurfaces
{
// A picture you own: a grid of pixels you write, which the game can draw anywhere it lets you draw. It lives until you destroy it or your mod is unloaded.
// At most 4096 pixels on a side.
boost::shared_ptr<ModSurface> Create(const std::string& owner, int width, int height);
// A picture made from the bytes of a PNG or another image file (read it with file.read_mod).
boost::shared_ptr<ModSurface> Decode(const std::string& owner, const std::string& image);
void ReleaseTextures(Textures* textures);
}

#endif
