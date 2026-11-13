#include "game/world/block/ScriptedBlockEntity.hpp"

#include "game/core/EntityType.hpp"
#include "game/core/ModEvents_generated.hpp"
#include "game/data/ListTag.hpp"
#include "game/network/ClientboundBlockEntityDataPacket.hpp"
#include "game/core/SharedConstants.hpp"
#include "game/item/ItemInstance.hpp"
#include "game/item/Inventory.hpp"
#include "game/item/ContainerHelper.hpp"
#include "game/network/ClientboundContainerOpenPacket.hpp"
#include "game/server/ServerLevel.hpp"
#include "game/ui/ChestMenu.hpp"
#include "game/world/generation/Player.hpp"
#include "game/world/chunk/PlayerChunkMap.hpp"
#include "game/world/Level.hpp"
#include "game/world/Dimension.hpp"
#include "game/world/DimensionType.hpp"
#include "game/world/block/ScriptedEntityBlock.hpp"

namespace
{
const unsigned long long kType = BlockEntityType::BLOCK_ENTITY | BlockEntityType::TICKABLE | 0x40;
const int kTypeId = 40;
}

ScriptedBlockEntity::ScriptedBlockEntity(unsigned int slots)
    : mItems(NonNullList<not_null_ptr<ItemInstance> >::withSize(slots, ItemInstance::EMPTY)), mSlots(slots)
{
}

ScriptedBlockEntity::~ScriptedBlockEntity()
{
    delete[] mItems.items.data;
}

BlockEntity* ScriptedBlockEntity::create()
{
    return new ScriptedBlockEntity(0);
}

void ScriptedBlockEntity::Register()
{
    BlockEntity::Register(kTypeId, ScriptedBlockEntity::create, kType, L"lcereborn:scripted_block");
}

unsigned long long ScriptedBlockEntity::Type()
{
    return kType;
}

unsigned long long ScriptedBlockEntity::GetType()
{
    return kType;
}

bool ScriptedBlockEntity::Has(const std::wstring& key) const
{
    return mValues.find(key) != mValues.end();
}

std::wstring ScriptedBlockEntity::Get(const std::wstring& key) const
{
    std::map<std::wstring, std::wstring>::const_iterator found = mValues.find(key);
    return found != mValues.end() ? found->second : std::wstring();
}

bool ScriptedBlockEntity::Set(const std::wstring& key, const std::wstring& value)
{
    if (key.empty() || key.size() > kMaxKeyLength || value.size() > kMaxValueLength)
        return false;
    if (mValues.find(key) == mValues.end() && static_cast<int>(mValues.size()) >= kMaxKeys)
        return false;
    mValues[key] = value;
    setChanged();
    return true;
}

void ScriptedBlockEntity::load(CompoundTag* tag)
{
    BlockEntity::load(tag);
    mValues.clear();
    const unsigned int slots = static_cast<unsigned int>(tag->getInt(L"Slots"));
    if (slots != mSlots && slots <= 54)
    {
        delete[] mItems.items.data;
        mSlots = slots;
        mItems = NonNullList<not_null_ptr<ItemInstance> >::withSize(slots, ItemInstance::EMPTY);
    }
    if (mSlots > 0)
        ContainerHelper::loadAllItems(tag, mItems);
    ListTag* list = tag->getList(L"Values", 10);
    for (int index = 0; list != NULL && index < list->size() && index < kMaxKeys; ++index)
    {
        CompoundTag* entry = list->getCompound(index);
        mValues[entry->getString(L"k")] = entry->getString(L"v");
    }
}

CompoundTag* ScriptedBlockEntity::save(CompoundTag* tag)
{
    BlockEntity::save(tag);
    tag->putInt(L"Slots", static_cast<int>(mSlots));
    if (mSlots > 0)
        ContainerHelper::saveAllItems(tag, mItems);
    ListTag* list = new ListTag();
    for (std::map<std::wstring, std::wstring>::const_iterator it = mValues.begin(); it != mValues.end(); ++it)
    {
        CompoundTag* entry = new CompoundTag();
        entry->putString(L"k", it->first);
        entry->putString(L"v", it->second);
        list->add(entry);
    }
    tag->put(L"Values", list);
    return tag;
}

void ScriptedBlockEntity::tick()
{
    // the mod's part is the hook ScriptedBlockEntity_tick
    BlockEntity::tick();
}

boost::shared_ptr<Packet> ScriptedBlockEntity::getUpdatePacket()
{
    return boost::shared_ptr<ClientboundBlockEntityDataPacket>(
        new ClientboundBlockEntityDataPacket(mPos, kPacketType, mLevel->mDimension->getType()->getId(), getUpdateTag()));
}

CompoundTag* ScriptedBlockEntity::getUpdateTag()
{
    return save(new CompoundTag());
}

boost::shared_ptr<BlockEntity> ScriptedBlockEntity::clone()
{
    boost::shared_ptr<ScriptedBlockEntity> result(new ScriptedBlockEntity(mSlots));
    BlockEntity::clone(result);
    result->mValues = mValues;
    return result;
}

std::wstring ScriptedBlockEntity::getName()
{
    return L"Container";
}

std::wstring ScriptedBlockEntity::getCustomName()
{
    return L"";
}

bool ScriptedBlockEntity::hasCustomName()
{
    return false;
}

std::wstring ScriptedBlockEntity::getDisplayName()
{
    return getName();
}

unsigned int ScriptedBlockEntity::getContainerSize()
{
    return mSlots;
}

bool ScriptedBlockEntity::isEmpty()
{
    for (unsigned int index = 0; index < mItems.size(); ++index)
    {
        if (!mItems[index]->isEmpty())
            return false;
    }
    return true;
}

not_null_ptr<ItemInstance> ScriptedBlockEntity::getItem(unsigned int slot)
{
    return slot < mItems.size() ? mItems[slot] : ItemInstance::EMPTY;
}

not_null_ptr<ItemInstance> ScriptedBlockEntity::removeItem(unsigned int slot, int count)
{
    not_null_ptr<ItemInstance> removed = ContainerHelper::removeItem(mItems, slot, count);
    setChanged();
    return removed;
}

not_null_ptr<ItemInstance> ScriptedBlockEntity::removeItemNoUpdate(int slot)
{
    return ContainerHelper::takeItem(mItems, slot);
}

void ScriptedBlockEntity::setItem(unsigned int slot, not_null_ptr<ItemInstance> itemInstance)
{
    if (slot < mItems.size())
        mItems[slot] = itemInstance;
    setChanged();
}

int ScriptedBlockEntity::getMaxStackSize()
{
    return 64;
}

void ScriptedBlockEntity::setChanged()
{
    BlockEntity::setChanged();
    if (mLevel != NULL && !mLevel->mIsClientSide)
        static_cast<ServerLevel*>(mLevel)->getChunkMap()->blockChanged(mPos);
}

bool ScriptedBlockEntity::stillValid(boost::shared_ptr<Player> player)
{
    if (mLevel->getBlockEntity(mPos) != shared_from_this())
        return false;
    return player->distanceAsIfHitscanned(mPos, *player->mBoundingBox) <= SharedConstants::CONTAINER_INTERACT_DISTANCE_SQR;
}

void ScriptedBlockEntity::startOpen(boost::shared_ptr<Player>) {}

void ScriptedBlockEntity::stopOpen(boost::shared_ptr<Player>) {}

bool ScriptedBlockEntity::canPlaceItem(int, not_null_ptr<ItemInstance>)
{
    return true;
}

int ScriptedBlockEntity::getContainerData(int)
{
    return 0;
}

void ScriptedBlockEntity::setContainerData(int, int) {}

int ScriptedBlockEntity::getContainerDataCount()
{
    return 0;
}

void ScriptedBlockEntity::clearContent()
{
    mItems.clear();
}

AbstractContainerMenu* ScriptedBlockEntity::createMenu(boost::shared_ptr<Inventory> inventory,
                                                       boost::shared_ptr<Player> player)
{
    return new ChestMenu(boost::static_pointer_cast<Container>(inventory),
                         boost::static_pointer_cast<Container>(boost::static_pointer_cast<ScriptedBlockEntity>(shared_from_this())),
                         player);
}

int ScriptedBlockEntity::getTypeId(bool)
{
    return ClientboundContainerOpenPacket::CONTAINER;
}
