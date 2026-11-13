#include "game/world/block/ScriptedEntityBlock.hpp"

#include "game/world/Level.hpp"
#include "game/world/block/ScriptedBlockEntity.hpp"

ScriptedEntityBlock::ScriptedEntityBlock(Material* material, const std::vector<PropertySpec>& properties,
                                         const std::vector<FaceSpec>& faces, const std::vector<ShapeSpec>& shapes,
                                         int slots)
    : ScriptedBlock(material, properties, faces, shapes), mSlots(slots)
{
    mBlockEntity = true;
}

boost::shared_ptr<BlockEntity> ScriptedEntityBlock::newBlockEntity(Level*, int)
{
    return boost::shared_ptr<BlockEntity>(new ScriptedBlockEntity(static_cast<unsigned int>(mSlots)));
}

void ScriptedEntityBlock::onRemove(Level* level, const BlockPos& pos, const BlockState* state)
{
    ScriptedBlock::onRemove(level, pos, state);
    level->removeBlockEntity(pos);
}
