#include "game/world/block/ScriptedBlock.hpp"

#include "game/core/Direction.hpp"
#include "game/core/ModEvents_generated.hpp"
#include "game/world/Level.hpp"
#include "game/world/generation/Player.hpp"
#include "game/core/ModItemNames.hpp"
#include "game/item/Items.hpp"
#include "game/item/ItemInstance.hpp"
#include "game/util/Random.hpp"
#include "game/util/AABB.hpp"
#include "game/core/IconRegister.hpp"
#include "game/world/block/BlockState.hpp"
#include "game/world/block/BlockStateDefinition.hpp"
#include "game/world/block/IntegerProperty.hpp"
#include "game/world/block/Material.hpp"
#include "game/entity/mob/LivingEntity.hpp"

ScriptedBlock::ScriptedBlock(Material* material, const std::vector<PropertySpec>& properties,
                             const std::vector<FaceSpec>& faces, const std::vector<ShapeSpec>& shapes)
    : ModBlock<Block>(material), mSpecs(properties), mInteractive(false), mPowerProperty(-1), mHasDrop(false), mDropMin(1), mDropMax(1), mRotateProperty(-1)
{
    for (size_t index = 0; index < shapes.size(); ++index)
    {
        Shape shape;
        shape.spec = shapes[index];
        float minX = 1.0f, minY = 1.0f, minZ = 1.0f, maxX = 0.0f, maxY = 0.0f, maxZ = 0.0f;
        for (size_t box = 0; box < shapes[index].boxes.size(); ++box)
        {
            const Box& b = shapes[index].boxes[box];
            shape.aabbs.push_back(AABB::newPermanent(b.x0, b.y0, b.z0, b.x1, b.y1, b.z1));
            minX = b.x0 < minX ? b.x0 : minX;
            minY = b.y0 < minY ? b.y0 : minY;
            minZ = b.z0 < minZ ? b.z0 : minZ;
            maxX = b.x1 > maxX ? b.x1 : maxX;
            maxY = b.y1 > maxY ? b.y1 : maxY;
            maxZ = b.z1 > maxZ ? b.z1 : maxZ;
        }
        shape.bounds = shapes[index].boxes.empty() ? AABB::newPermanent(0, 0, 0, 1, 1, 1)
                                                    : AABB::newPermanent(minX, minY, minZ, maxX, maxY, maxZ);
        mShapes.push_back(shape);
    }
    for (size_t index = 0; index < properties.size(); ++index)
        mProperties.push_back(static_cast<AbstractProperty<int>*>(
            IntegerProperty::create(properties[index].name, 0, properties[index].count - 1)));
    for (size_t index = 0; index < faces.size(); ++index)
    {
        Face face = {faces[index], NULL};
        mFaces.push_back(face);
    }
    DerivedInit();
}

int ScriptedBlock::GetPropertyIndex(const std::wstring& name) const
{
    for (size_t index = 0; index < mSpecs.size(); ++index)
    {
        if (mSpecs[index].name == name)
            return static_cast<int>(index);
    }
    return -1;
}

int ScriptedBlock::GetPropertyCount(int index) const
{
    return index >= 0 && index < static_cast<int>(mSpecs.size()) ? mSpecs[index].count : 0;
}

int ScriptedBlock::Stride(int index) const
{
    int stride = 1;
    for (int before = 0; before < index; ++before)
        stride *= mSpecs[before].count;
    return stride;
}

int ScriptedBlock::GetValue(const BlockState* state, int index) const
{
    return state->getValue(mProperties[index]);
}

const BlockState* ScriptedBlock::WithValue(const BlockState* state, int index, int value) const
{
    if (index < 0 || index >= static_cast<int>(mSpecs.size()) || value < 0 || value >= mSpecs[index].count)
        return state;
    return state->setValue(mProperties[index], value);
}

void ScriptedBlock::SetPower(const std::wstring& property)
{
    mPowerProperty = GetPropertyIndex(property);
}

bool ScriptedBlock::isSignalSource(const BlockState* state)
{
    return mPowerProperty >= 0;
}

int ScriptedBlock::getSignal(const BlockState* state, LevelSource*, const BlockPos&, const Direction*)
{
    if (mPowerProperty < 0)
        return 0;
    const int value = GetValue(state, mPowerProperty);
    return value > 15 ? 15 : value;
}

int ScriptedBlock::getDirectSignal(const BlockState* state, LevelSource* levelSource, const BlockPos& pos,
                                   const Direction* direction)
{
    return getSignal(state, levelSource, pos, direction);
}

bool ScriptedBlock::TestUse()
{
    return mInteractive;
}

bool ScriptedBlock::use(Level* level, const BlockPos& pos, const BlockState* state, boost::shared_ptr<Player> player,
                        InteractionHand::EInteractionHand hand, const Direction* direction, float hitX, float hitY,
                        float hitZ, bool clientSide)
{
    if (!mInteractive)
        return Block::use(level, pos, state, player, hand, direction, hitX, hitY, hitZ, clientSide);
    if (!clientSide && !level->mIsClientSide)
    {
        bool handled = false;
        ModEvents::BlockUse(this, level, pos.x, pos.y, pos.z, convertBlockStateToLegacyData(state), player,
                            static_cast<int>(hand), direction != NULL ? direction->get3DDataValue() : -1, hitX, hitY, hitZ,
                            clientSide, handled);
    }
    return true;
}

void ScriptedBlock::SetDrop(const std::wstring& item, int minimum, int maximum)
{
    mHasDrop = true;
    mDropItem = item;
    mDropMin = minimum;
    mDropMax = maximum;
}

bool ScriptedBlock::SetRotates()
{
    const int facing = GetPropertyIndex(L"facing");
    if (facing < 0 || mSpecs[facing].count < 4)
        return false;
    mRotateProperty = facing;
    return true;
}

int ScriptedBlock::Turns(const BlockState* state) const
{
    return mRotateProperty < 0 ? 0 : GetValue(state, mRotateProperty) % 4;
}

void ScriptedBlock::RotatedBoxes(const BlockState* state, std::vector<Box>* boxes) const
{
    const int turns = Turns(state);
    for (int turn = 0; turn < turns; ++turn)
    {
        for (size_t index = 0; index < boxes->size(); ++index)
        {
            Box& b = (*boxes)[index];
            const Box rotated = {1.0f - b.z1, b.y0, b.x0, 1.0f - b.z0, b.y1, b.x1};
            b = rotated;
        }
    }
}

Item* ScriptedBlock::getResource(const BlockState* state, Random* random, int lootBonus)
{
    if (!mHasDrop)
        return Block::getResource(state, random, lootBonus);
    ItemInstance* stack = NULL;
    if (mDropItem.empty() || !ModItemNames::Resolve(mDropItem, 1, 0, &stack))
        return Items::air;
    Item* item = stack->getItem();
    delete stack;
    return item;
}

int ScriptedBlock::getResourceCount(Random* random)
{
    if (!mHasDrop || mDropMax <= mDropMin)
        return mHasDrop ? mDropMin : Block::getResourceCount(random);
    return mDropMin + random->nextInt(mDropMax - mDropMin + 1);
}

void ScriptedBlock::getOutlineAABBs(const BlockState* state, Level* level, const BlockPos& pos,
                                    std::vector<const AABB*>& boxes)
{
    if (mShapes.empty())
    {
        Block::getOutlineAABBs(state, level, pos, boxes);
        return;
    }
    std::vector<Box> parts;
    GetBoxes(state, &parts);
    for (size_t index = 0; index < parts.size(); ++index)
        boxes.push_back(AABB::newTemp(parts[index].x0 + pos.x, parts[index].y0 + pos.y, parts[index].z0 + pos.z,
                                      parts[index].x1 + pos.x, parts[index].y1 + pos.y, parts[index].z1 + pos.z));
}

void ScriptedBlock::DerivedInit()
{
    Block::DerivedInit();
    const BlockState* state = mStateDefinition->any();
    for (size_t index = 0; index < mProperties.size(); ++index)
        state = state->setValue(mProperties[index], 0);
    registerDefaultState(state);
}

BlockStateDefinition* ScriptedBlock::createBlockStateDefinition()
{
    std::vector<const Property*> properties;
    for (size_t index = 0; index < mProperties.size(); ++index)
        properties.push_back(mProperties[index]);
    if (properties.empty())
        return new BlockStateDefinition(this);
    return new BlockStateDefinition(this, &properties[0], static_cast<unsigned int>(properties.size()));
}

const BlockState* ScriptedBlock::getBlockState(int data)
{
    const BlockState* state = defaultBlockState();
    for (size_t index = 0; index < mProperties.size(); ++index)
    {
        const int value = (data / Stride(static_cast<int>(index))) % mSpecs[index].count;
        state = state->setValue(mProperties[index], value);
    }
    return state;
}

int ScriptedBlock::convertBlockStateToLegacyData(const BlockState* state)
{
    int data = 0;
    for (size_t index = 0; index < mProperties.size(); ++index)
        data += state->getValue(mProperties[index]) * Stride(static_cast<int>(index));
    return data;
}

void ScriptedBlock::registerIcons(IconRegister* iconRegister)
{
    Block::registerIcons(iconRegister);
    for (size_t index = 0; index < mFaces.size(); ++index)
        mFaces[index].sprite = iconRegister->registerIcon(mFaces[index].spec.key);
}

TextureAtlasSprite* ScriptedBlock::getTexture(const Direction* direction, const BlockState* state)
{
    int side = direction->get3DDataValue();
    const int turns = Turns(state);
    if (turns != 0 && side >= eSide_North)
    {
        static const int clockwise[4] = {eSide_North, eSide_East, eSide_South, eSide_West};
        for (int index = 0; index < 4; ++index)
        {
            if (clockwise[index] == side)
            {
                side = clockwise[(index + 4 - turns) % 4];
                break;
            }
        }
    }
    int bestScore = -1;
    TextureAtlasSprite* best = mIcon;
    for (size_t index = 0; index < mFaces.size(); ++index)
    {
        const FaceSpec& spec = mFaces[index].spec;
        const bool exact = spec.side == side;
        const bool sides = spec.side == eSide_Sides && side >= eSide_North;
        if (!exact && !sides && spec.side != eSide_All)
            continue;
        if (spec.property >= 0 && GetValue(state, spec.property) != spec.value)
            continue;
        const int score = (spec.property >= 0 ? 4 : 0) + (exact ? 2 : (sides ? 1 : 0));
        if (score > bestScore && mFaces[index].sprite != NULL)
        {
            bestScore = score;
            best = mFaces[index].sprite;
        }
    }
    return best;
}

const BlockState* ScriptedBlock::getStateForPlacement(Level*, const BlockPos& pos, const Direction*, float, float,
                                                      float, int auxValue, boost::shared_ptr<LivingEntity> placer)
{
    const BlockState* state = getBlockState(auxValue);
    const int facing = GetPropertyIndex(L"facing");
    if (facing < 0 || !placer)
        return state;
    const Direction* look = GetPropertyCount(facing) == 4 ? Direction::fromYRot(placer->mYRot)
                                                           : Direction::getDirectionFromLivingEntity(pos, placer);
    static const int order[6] = {5, 4, 0, 2, 3, 1};
    return WithValue(state, facing, order[look->getOpposite()->get3DDataValue()]);
}

const ScriptedBlock::Shape* ScriptedBlock::BestShape(const BlockState* state) const
{
    int bestScore = -1;
    const Shape* best = NULL;
    for (size_t index = 0; index < mShapes.size(); ++index)
    {
        const ShapeSpec& spec = mShapes[index].spec;
        if (spec.property >= 0 && GetValue(state, spec.property) != spec.value)
            continue;
        const int score = spec.property >= 0 ? 1 : 0;
        if (score > bestScore)
        {
            bestScore = score;
            best = &mShapes[index];
        }
    }
    return best;
}

void ScriptedBlock::GetBoxes(const BlockState* state, std::vector<Box>* boxes) const
{
    boxes->clear();
    const Shape* shape = BestShape(state);
    if (shape != NULL)
    {
        *boxes = shape->spec.boxes;
        RotatedBoxes(state, boxes);
        return;
    }
    const Box full = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f};
    boxes->push_back(full);
}

bool ScriptedBlock::isSolidRender(const BlockState* state) const
{
    return mShapes.empty() && Block::isSolidRender(state);
}

bool ScriptedBlock::isCubeShaped(const BlockState* state)
{
    return mShapes.empty() && Block::isCubeShaped(state);
}

EBlockRenderShape ScriptedBlock::getRenderShape(const BlockState* state)
{
    return mShapes.empty() ? Block::getRenderShape(state) : BLOCK_RENDER_SCRIPTED;
}

const AABB* ScriptedBlock::getShape(const BlockState* state, LevelSource* levelSource, const BlockPos& pos)
{
    const Shape* shape = BestShape(state);
    if (shape == NULL)
        return Block::getShape(state, levelSource, pos);
    std::vector<Box> parts;
    GetBoxes(state, &parts);
    float minX = 1.0f, minY = 1.0f, minZ = 1.0f, maxX = 0.0f, maxY = 0.0f, maxZ = 0.0f;
    for (size_t index = 0; index < parts.size(); ++index)
    {
        minX = parts[index].x0 < minX ? parts[index].x0 : minX;
        minY = parts[index].y0 < minY ? parts[index].y0 : minY;
        minZ = parts[index].z0 < minZ ? parts[index].z0 : minZ;
        maxX = parts[index].x1 > maxX ? parts[index].x1 : maxX;
        maxY = parts[index].y1 > maxY ? parts[index].y1 : maxY;
        maxZ = parts[index].z1 > maxZ ? parts[index].z1 : maxZ;
    }
    return Turns(state) == 0 ? shape->bounds : AABB::newTemp(minX, minY, minZ, maxX, maxY, maxZ);
}

void ScriptedBlock::addCollisionAABBs(const BlockState* state, Level* level, const BlockPos& pos,
                                      const AABB* entityBox, std::vector<AABB*>* collidingBoxes,
                                      boost::shared_ptr<Entity> entity, bool isActualState)
{
    const Shape* shape = BestShape(state);
    if (shape == NULL)
    {
        Block::addCollisionAABBs(state, level, pos, entityBox, collidingBoxes, entity, isActualState);
        return;
    }
    if (Turns(state) == 0)
    {
        for (size_t index = 0; index < shape->aabbs.size(); ++index)
            addCollisionAABB(pos, entityBox, collidingBoxes, shape->aabbs[index]);
        return;
    }
    std::vector<Box> parts;
    GetBoxes(state, &parts);
    for (size_t index = 0; index < parts.size(); ++index)
        addCollisionAABB(pos, entityBox, collidingBoxes,
                         AABB::newTemp(parts[index].x0, parts[index].y0, parts[index].z0, parts[index].x1,
                                       parts[index].y1, parts[index].z1));
}
