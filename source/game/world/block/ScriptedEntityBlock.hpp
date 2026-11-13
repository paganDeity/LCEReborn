#ifndef GAME_WORLD_BLOCK_SCRIPTEDENTITYBLOCK_HPP
#define GAME_WORLD_BLOCK_SCRIPTEDENTITYBLOCK_HPP

#include "game/world/block/EntityBlock.hpp"
#include "game/world/block/ScriptedBlock.hpp"

class ScriptedEntityBlock : public ScriptedBlock, public EntityBlock
{
public:
    ScriptedEntityBlock(Material* material, const std::vector<PropertySpec>& properties,
                        const std::vector<FaceSpec>& faces, const std::vector<ShapeSpec>& shapes, int slots);

    int Slots() const { return mSlots; }

    virtual boost::shared_ptr<BlockEntity> newBlockEntity(Level* level, int data);
    virtual void onRemove(Level* level, const BlockPos& pos, const BlockState* state);

private:
    int mSlots;
};

#endif
