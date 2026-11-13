#ifndef GAME_WORLD_BLOCK_SCRIPTEDBLOCKENTITY_HPP
#define GAME_WORLD_BLOCK_SCRIPTEDBLOCKENTITY_HPP

#include <map>
#include <string>

#include "game/core/InteractionObject.hpp"
#include "game/item/Container.hpp"
#include "game/item/NonNullList.hpp"
#include "game/item/not_null_ptr.hpp"
#include "game/world/block/BlockEntity.hpp"

class ScriptedBlockEntity : public BlockEntity, public Container, public InteractionObject
{
public:
    static const int kMaxKeys = 64;
    static const size_t kMaxKeyLength = 32;
    static const size_t kMaxValueLength = 1024;

    explicit ScriptedBlockEntity(unsigned int slots = 0);
    virtual ~ScriptedBlockEntity();

    static BlockEntity* create();
    // the type every mod block's store has, which the renderer of mod blocks is registered for
    static unsigned long long Type();
    static void Register();

    bool Has(const std::wstring& key) const;
    // the text stored under a key, empty when none is
    std::wstring Get(const std::wstring& key) const;
    bool Set(const std::wstring& key, const std::wstring& value);

    virtual unsigned long long GetType();

    virtual std::wstring getName();
    virtual std::wstring getCustomName();
    virtual bool hasCustomName();
    virtual std::wstring getDisplayName();
    virtual unsigned int getContainerSize();
    virtual bool isEmpty();
    virtual not_null_ptr<ItemInstance> getItem(unsigned int slot);
    virtual not_null_ptr<ItemInstance> removeItem(unsigned int slot, int count);
    virtual not_null_ptr<ItemInstance> removeItemNoUpdate(int slot);
    virtual void setItem(unsigned int slot, not_null_ptr<ItemInstance> itemInstance);
    virtual int getMaxStackSize();
    virtual void setChanged();
    virtual bool stillValid(boost::shared_ptr<Player> player);
    virtual void startOpen(boost::shared_ptr<Player> player);
    virtual void stopOpen(boost::shared_ptr<Player> player);
    virtual bool canPlaceItem(int slot, not_null_ptr<ItemInstance> itemInstance);
    virtual int getContainerData(int id);
    virtual void setContainerData(int id, int value);
    virtual int getContainerDataCount();
    virtual void clearContent();
    virtual AbstractContainerMenu* createMenu(boost::shared_ptr<Inventory> inventory,
                                              boost::shared_ptr<Player> player);
    virtual int getTypeId(bool large);
    virtual void load(CompoundTag* tag);
    virtual CompoundTag* save(CompoundTag* tag);
    virtual void tick();
    virtual boost::shared_ptr<BlockEntity> clone();
    virtual boost::shared_ptr<Packet> getUpdatePacket();
    virtual CompoundTag* getUpdateTag();

    static const int kPacketType = 14;

private:
    std::map<std::wstring, std::wstring> mValues;
    NonNullList<not_null_ptr<ItemInstance> > mItems;
    unsigned int mSlots;
};

#endif
