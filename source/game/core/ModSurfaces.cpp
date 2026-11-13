#include "game/core/ModSurfaces.hpp"

#include <map>
#include <utility>

#include <stb_image.h>

#include "game/client/render/Textures.hpp"
#include "game/core/BufferedImage.hpp"
#include "game/core/ModHandles.hpp"
#include "game/core/ModOwned.hpp"

namespace
{
const int kMaxSide = 4096;
const size_t kMaxSurfacesPerMod = 64;

struct Pending
{
    Pending() { InitializeCriticalSection(&lock); }
    C4JCriticalSection lock;
    std::vector<std::pair<int, BufferedImage*> > released;
};

Pending& Released()
{
    static Pending pending;
    return pending;
}

class Locked
{
public:
    explicit Locked(C4JCriticalSection* lock) : mLock(lock) { EnterCriticalSection(mLock); }
    ~Locked() { LeaveCriticalSection(mLock); }

private:
    C4JCriticalSection* mLock;
};
}

ModSurface::ModSurface(const std::string& owner, int width, int height)
    : mOwner(owner), mWidth(width), mHeight(height), mPixels(static_cast<size_t>(width) * height, 0), mDirty(true),
      mTexture(-1), mImage(NULL)
{
    InitializeCriticalSection(&mLock);
}

ModSurface::~ModSurface()
{
    if (mTexture >= 0)
    {
        Pending& pending = Released();
        Locked locked(&pending.lock);
        pending.released.push_back(std::make_pair(mTexture, mImage));
    }
    DeleteCriticalSection(&mLock);
}

bool ModSurface::Write(int x, int y, int width, int height, const std::string& rgba)
{
    if (x < 0 || y < 0 || width < 0 || height < 0 || x + width > mWidth || y + height > mHeight
        || rgba.size() != static_cast<size_t>(width) * height * 4)
        return false;
    Locked locked(&mLock);
    for (int row = 0; row < height; ++row)
        for (int column = 0; column < width; ++column)
        {
            const unsigned char* p = reinterpret_cast<const unsigned char*>(rgba.data()) + (static_cast<size_t>(row) * width + column) * 4;
            mPixels[static_cast<size_t>(y + row) * mWidth + x + column] =
                static_cast<int>((static_cast<unsigned int>(p[0]) << 24) | (static_cast<unsigned int>(p[1]) << 16)
                                 | (static_cast<unsigned int>(p[2]) << 8) | p[3]);
        }
    mDirty = true;
    return true;
}

std::string ModSurface::Read(int x, int y, int width, int height)
{
    std::string rgba;
    if (x < 0 || y < 0 || width < 1 || height < 1 || x + width > mWidth || y + height > mHeight)
        return rgba;
    Locked locked(&mLock);
    rgba.reserve(static_cast<size_t>(width) * height * 4);
    for (int row = 0; row < height; ++row)
        for (int column = 0; column < width; ++column)
        {
            const unsigned int pixel = static_cast<unsigned int>(mPixels[static_cast<size_t>(y + row) * mWidth + x + column]);
            rgba += static_cast<char>(pixel >> 24);
            rgba += static_cast<char>((pixel >> 16) & 0xFF);
            rgba += static_cast<char>((pixel >> 8) & 0xFF);
            rgba += static_cast<char>(pixel & 0xFF);
        }
    return rgba;
}

void ModSurface::SnapshotArgb(std::vector<int>* argb)
{
    Locked locked(&mLock);
    argb->resize(mPixels.size());
    for (size_t index = 0; index < mPixels.size(); ++index)
    {
        const unsigned int rgba = static_cast<unsigned int>(mPixels[index]);
        (*argb)[index] = static_cast<int>((rgba << 24) | (rgba >> 8));
    }
}

bool ModSurface::Load(const std::string& image)
{
    int width, height, components;
    unsigned char* data = stbi_load_from_memory(reinterpret_cast<const unsigned char*>(image.data()),
                                                static_cast<int>(image.size()), &width, &height, &components, 4);
    if (data == NULL)
        return false;
    const bool fits = width == mWidth && height == mHeight;
    const bool wrote = fits && Write(0, 0, width, height, std::string(reinterpret_cast<char*>(data), static_cast<size_t>(width) * height * 4));
    stbi_image_free(data);
    return wrote;
}

int ModSurface::Texture(Textures* textures)
{
    Locked locked(&mLock);
    if (mTexture < 0)
    {
        mImage = new BufferedImage(mWidth, mHeight, 0);
        mTexture = textures->getTexture(mImage, C4JRender::eTextureFormat_RGBA8, false);
        mDirty = true;
    }
    if (mDirty)
    {
        textures->replaceTextureDirect(arrayWithLength<int>(&mPixels[0], static_cast<unsigned int>(mPixels.size())), mWidth,
                                       mHeight, mTexture);
        mDirty = false;
    }
    return mTexture;
}

void ModSurface::Destroy()
{
    ModHandles::ReleasePersistentObject(this);
}

namespace ModSurfaces
{
boost::shared_ptr<ModSurface> Create(const std::string& owner, int width, int height)
{
    if (width < 1 || height < 1 || width > kMaxSide || height > kMaxSide)
        return boost::shared_ptr<ModSurface>();
    return boost::shared_ptr<ModSurface>(new ModSurface(owner, width, height));
}

boost::shared_ptr<ModSurface> Decode(const std::string& owner, const std::string& image)
{
    int width, height, components;
    if (!stbi_info_from_memory(reinterpret_cast<const unsigned char*>(image.data()), static_cast<int>(image.size()), &width, &height,
                               &components))
        return boost::shared_ptr<ModSurface>();
    boost::shared_ptr<ModSurface> surface(new ModSurface(owner, width, height));
    if (width > kMaxSide || height > kMaxSide || !surface->Load(image))
        return boost::shared_ptr<ModSurface>();
    return surface;
}

void ReleaseTextures(Textures* textures)
{
    Pending& pending = Released();
    std::vector<std::pair<int, BufferedImage*> > released;
    {
        Locked locked(&pending.lock);
        released.swap(pending.released);
    }
    for (size_t index = 0; index < released.size(); ++index)
    {
        textures->releaseTexture(released[index].first);
        delete released[index].second;
    }
}
}
