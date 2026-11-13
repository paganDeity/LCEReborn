#include "game/client/render/ModBlockEntityRenderer.hpp"

#include "game/client/render/BlockEntityRenderDispatcher.hpp"
#include "game/client/render/ModModel.hpp"
#include "game/client/render/Textures.hpp"
#include "game/core/GlStateManager.hpp"
#include "game/core/ModBlocks.hpp"
#include "game/core/ModSurfaces.hpp"
#include "game/entity/mob/Entity.hpp"
#include "game/util/ResourceLocation.hpp"
#include "game/world/block/Block.hpp"
#include "game/world/block/BlockEntity.hpp"

ModBlockEntityRenderer::~ModBlockEntityRenderer()
{
    for (std::map<std::wstring, ModModel*>::iterator it = m_models.begin(); it != m_models.end(); ++it)
        delete it->second;
}

void ModBlockEntityRenderer::render(boost::shared_ptr<BlockEntity> blockEntity, double x, double y, double z, float a, int breakProgress,
                                    bool bTranslucent, float alpha, bool bUseDisplayList)
{
    (void)a;
    (void)breakProgress;
    (void)bTranslucent;
    (void)alpha;
    Block* block = blockEntity->getBlock();
    if (block == NULL)
        return;
    const std::wstring key = Block::blockRegistry->getKey(block).toString();
    std::wstring text;
    boost::shared_ptr<ModSurface> picture;
    if (!ModBlocks::ModelOf(key, &text, &picture))
        return;
    ModModel*& model = m_models[key];
    if (model == NULL)
        model = new ModModel(text);
    Textures* textures = mDispatcher->mTextures;
    ModSurfaces::ReleaseTextures(textures);
    textures->bind(picture->Texture(textures));
    GlStateManager::enableDepthTest();
    GlStateManager::depthFunc(3);
    GlStateManager::depthMask(true);
    GlStateManager::pushMatrix();
    GlStateManager::translatef(static_cast<float>(x) + 0.5f, static_cast<float>(y) + 1.5f, static_cast<float>(z) + 0.5f);
    GlStateManager::scalef(1.0f, -1.0f, -1.0f);
    boost::shared_ptr<Entity> none;
    model->render(none, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0625f, bUseDisplayList);
    GlStateManager::popMatrix();
}
