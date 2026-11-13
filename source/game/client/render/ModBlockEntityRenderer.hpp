#ifndef GAME_CLIENT_RENDER_MODBLOCKENTITYRENDERER_HPP
#define GAME_CLIENT_RENDER_MODBLOCKENTITYRENDERER_HPP

#include <map>
#include <string>

#include "game/client/render/BlockEntityRenderer.hpp"

class ModModel;

// draws the model a mod gave a block with a store (ModBlocks::DeclareModel), at the block's centre
class ModBlockEntityRenderer : public BlockEntityRenderer
{
public:
    virtual ~ModBlockEntityRenderer();
    virtual void render(boost::shared_ptr<BlockEntity> blockEntity, double x, double y, double z, float a, int breakProgress,
                        bool bTranslucent, float alpha, bool bUseDisplayList);

private:
    std::map<std::wstring, ModModel*> m_models;
};

#endif
