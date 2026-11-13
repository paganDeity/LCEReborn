#include <string.h>

#include "game/client/render/Tesselator.hpp"
#include "game/client/render/Textures.hpp"
#include "game/core/BufferBuilder.hpp"
#include "game/core/GlStateManager.hpp"
#include "game/core/Gui.hpp"
#include "game/core/Minecraft.hpp"
#include "game/core/ModHandles.hpp"
#include "game/core/ModSurfaces.hpp"
#include "game/ui/UIMenuPainter.hpp"

namespace
{
// the state every flat or picture draw of a mod starts from
void BeginDraw(bool textured)
{
    GlStateManager::enableBlend();
    GlStateManager::blendFunc(4, 5);
    if (textured)
        GlStateManager::enableTexture();
    else
        GlStateManager::disableTexture();
    GlStateManager::color4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void BindPicture(const boost::shared_ptr<ModSurface>& picture)
{
    Textures* textures = Minecraft::GetInstance()->mTextures;
    ModSurfaces::ReleaseTextures(textures);
    textures->bind(picture->Texture(textures));
}

bool DrawTriangles(const ModHandles::WorldInfo& world, const std::string& data, bool textured)
{
    const size_t kCorner = 9 * sizeof(float);
    const size_t corners = data.size() / kCorner;
    if (data.size() % kCorner != 0 || corners % 3 != 0 || corners == 0 || corners > 30000)
        return false;
    BeginDraw(textured);
    GlStateManager::disableCull();
    BufferBuilder* builder = Tesselator::getInstance()->getBuilder();
    builder->begin(0);
    for (size_t corner = 0; corner < corners; ++corner)
    {
        float v[9];
        memcpy(v, data.data() + corner * kCorner, kCorner);
        builder->color(v[5], v[6], v[7], v[8]);
        if (textured)
            builder->vertexUV(v[0] - world.cameraX, v[1] - world.cameraY, v[2] - world.cameraZ, v[3], v[4]);
        else
            builder->vertex(v[0] - world.cameraX, v[1] - world.cameraY, v[2] - world.cameraZ);
    }
    builder->end();
    GlStateManager::enableTexture();
    GlStateManager::enableCull();
    return true;
}
}

int ModHandles::GuiInfo::GetWidth() const
{
    return width;
}

int ModHandles::GuiInfo::GetHeight() const
{
    return height;
}

MenuPainter* ModHandles::GuiInfo::Painter() const
{
    // the interface is already in pixels, so the painter needs no frame and keeps no state on the font
    static MenuPainter* painter = NULL;
    delete painter;
    painter = new MenuPainter(*gui, *font, false);
    return painter;
}

bool ModHandles::GuiInfo::Image(const boost::shared_ptr<ModSurface>& picture, int x, int y, int imageWidth, int imageHeight, float u, float v,
                                float u2, float v2) const
{
    BindPicture(picture);
    BeginDraw(true);
    gui->blitUV(x, y, imageWidth, imageHeight, u, v, u2, v2);
    return true;
}

float ModHandles::WorldInfo::CameraX() const
{
    return static_cast<float>(cameraX);
}

float ModHandles::WorldInfo::CameraY() const
{
    return static_cast<float>(cameraY);
}

float ModHandles::WorldInfo::CameraZ() const
{
    return static_cast<float>(cameraZ);
}

bool ModHandles::WorldInfo::Image(const boost::shared_ptr<ModSurface>& picture, double x, double y, double z, double acrossX, double acrossY,
                                  double acrossZ, double downX, double downY, double downZ, int light) const
{
    BindPicture(picture);
    x -= cameraX;
    y -= cameraY;
    z -= cameraZ;
    light = light < 0 ? 0 : (light > 15 ? 15 : light);
    const float shade = 0.2f + 0.8f * static_cast<float>(light) / 15.0f;
    BeginDraw(true);
    GlStateManager::disableCull();
    BufferBuilder* builder = Tesselator::getInstance()->getBuilder();
    builder->begin();
    builder->color(shade, shade, shade, 1.0f);
    builder->vertexUV(x, y, z, 0.0f, 0.0f);
    builder->vertexUV(x + acrossX, y + acrossY, z + acrossZ, 1.0f, 0.0f);
    builder->vertexUV(x + acrossX + downX, y + acrossY + downY, z + acrossZ + downZ, 1.0f, 1.0f);
    builder->vertexUV(x + downX, y + downY, z + downZ, 0.0f, 1.0f);
    builder->end();
    GlStateManager::enableCull();
    return true;
}

bool ModHandles::WorldInfo::Triangles(const boost::shared_ptr<ModSurface>& picture, const std::string& vertices) const
{
    BindPicture(picture);
    return DrawTriangles(*this, vertices, true);
}

bool ModHandles::WorldInfo::ColoredTriangles(const std::string& vertices) const
{
    return DrawTriangles(*this, vertices, false);
}
