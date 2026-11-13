#ifndef GAME_WORLD_BLOCK_SCRIPTEDBLOCK_HPP
#define GAME_WORLD_BLOCK_SCRIPTEDBLOCK_HPP

#include <string>
#include <vector>

#include "game/world/block/AbstractProperty.hpp"
#include "game/world/block/Block.hpp"
#include "game/world/block/ModBlock_generated.hpp"

class Material;

class ScriptedBlock : public ModBlock<Block>
{
public:
    enum ESide
    {
        eSide_Down = 0,
        eSide_Up = 1,
        eSide_North = 2,
        eSide_South = 3,
        eSide_West = 4,
        eSide_East = 5,
        eSide_Sides = 6,
        eSide_All = 7
    };

    struct PropertySpec
    {
        std::wstring name;
        int count;
    };

    struct FaceSpec
    {
        int side;
        int property;
        int value;
        std::wstring key;
    };

    struct Box
    {
        float x0;
        float y0;
        float z0;
        float x1;
        float y1;
        float z1;
    };

    struct ShapeSpec
    {
        int property;
        int value;
        std::vector<Box> boxes;
    };

    ScriptedBlock(Material* material, const std::vector<PropertySpec>& properties, const std::vector<FaceSpec>& faces,
                  const std::vector<ShapeSpec>& shapes);

    bool HasShapes() const { return !mShapes.empty(); }
    void SetDrop(const std::wstring& item, int minimum, int maximum);
    bool SetRotates();
    void SetInteractive() { mInteractive = true; }
    bool IsInteractive() const { return mInteractive; }
    void SetPower(const std::wstring& property);
    void GetBoxes(const BlockState* state, std::vector<Box>* boxes) const;

    int GetPropertyIndex(const std::wstring& name) const;
    int GetPropertyCount(int index) const;
    int GetValue(const BlockState* state, int index) const;
    const BlockState* WithValue(const BlockState* state, int index, int value) const;

    virtual void DerivedInit();
    virtual BlockStateDefinition* createBlockStateDefinition();
    virtual const BlockState* getBlockState(int data);
    virtual int convertBlockStateToLegacyData(const BlockState* state);
    virtual void registerIcons(IconRegister* iconRegister);
    virtual TextureAtlasSprite* getTexture(const Direction* direction, const BlockState* state);
    virtual bool isSignalSource(const BlockState* state);
    virtual int getSignal(const BlockState* state, LevelSource* levelSource, const BlockPos& pos,
                          const Direction* direction);
    virtual int getDirectSignal(const BlockState* state, LevelSource* levelSource, const BlockPos& pos,
                                const Direction* direction);
    virtual bool TestUse();
    virtual bool use(Level* level, const BlockPos& pos, const BlockState* state, boost::shared_ptr<Player> player,
                     InteractionHand::EInteractionHand hand, const Direction* direction, float hitX, float hitY,
                     float hitZ, bool clientSide);
    virtual Item* getResource(const BlockState* state, Random* random, int lootBonus);
    virtual int getResourceCount(Random* random);
    virtual void getOutlineAABBs(const BlockState* state, Level* level, const BlockPos& pos,
                                 std::vector<const AABB*>& boxes);
    virtual bool isSolidRender(const BlockState* state) const;
    virtual bool isCubeShaped(const BlockState* state);
    virtual EBlockRenderShape getRenderShape(const BlockState* state);
    virtual const AABB* getShape(const BlockState* state, LevelSource* levelSource, const BlockPos& pos);
    virtual void addCollisionAABBs(const BlockState* state, Level* level, const BlockPos& pos, const AABB* entityBox,
                                   std::vector<AABB*>* collidingBoxes, boost::shared_ptr<Entity> entity,
                                   bool isActualState);
    virtual const BlockState* getStateForPlacement(Level* level, const BlockPos& pos, const Direction* direction,
                                                   float hitX, float hitY, float hitZ, int auxValue,
                                                   boost::shared_ptr<LivingEntity> placer);

private:
    struct Face
    {
        FaceSpec spec;
        TextureAtlasSprite* sprite;
    };

    int Stride(int index) const;

    std::vector<PropertySpec> mSpecs;
    std::vector<AbstractProperty<int>*> mProperties;
    std::vector<Face> mFaces;
    struct Shape
    {
        ShapeSpec spec;
        std::vector<const AABB*> aabbs;
        const AABB* bounds;
    };
    const Shape* BestShape(const BlockState* state) const;
    int Turns(const BlockState* state) const;
    void RotatedBoxes(const BlockState* state, std::vector<Box>* boxes) const;

    bool mInteractive;
    int mPowerProperty;
    bool mHasDrop;
    std::wstring mDropItem;
    int mDropMin;
    int mDropMax;
    int mRotateProperty;

    std::vector<Shape> mShapes;
};

#endif
