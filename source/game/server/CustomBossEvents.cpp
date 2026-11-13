#include "game/server/CustomBossEvents.hpp"

#include <algorithm>

#include "game/data/CompoundTag.hpp"
#include "game/data/ListTag.hpp"
#include "game/data/SavedDataStorage.hpp"
#include "game/data/StringTag.hpp"
#include "game/data/WorldSavedData.hpp"
#include "game/server/ServerBossEvent.hpp"
#include "game/server/ServerPlayer.hpp"
#include "game/world/generation/PlayerList.hpp"

const std::wstring CustomBossEvents::FILE_ID = L"bossbars";

namespace
{
boost::shared_ptr<SavedData> MakeCustomBossEvents(const std::wstring& id)
{
    return boost::shared_ptr<SavedData>(new CustomBossEvents(id));
}

struct Registration
{
    Registration() { SavedDataStorage::RegisterType(typeid(CustomBossEvents), MakeCustomBossEvents); }
} sRegistration;

std::wstring IdOf(const boost::shared_ptr<ServerPlayer>& player)
{
    return player->getXuid().toString();
}

std::vector<boost::shared_ptr<ServerPlayer> > OnlinePlayers()
{
    std::vector<boost::shared_ptr<ServerPlayer> > online;
    if (gMinecraftServer != NULL && gMinecraftServer->getPlayers() != NULL)
        online = *gMinecraftServer->getPlayers()->getPlayers();
    return online;
}
}

CustomBossEvents::CustomBossEvents(const std::wstring& id) : SavedData(id)
{
}

void CustomBossEvents::load(CompoundTag* tag)
{
    for (size_t index = 0; index < mBars.size(); ++index)
        delete mBars[index];
    mBars.clear();
    ListTag* bars = tag->getList(L"Bars", Tag::TAG_COMPOUND);
    for (int index = 0; index < bars->size(); ++index)
    {
        CompoundTag* entry = bars->getCompound(index);
        Bar* bar = add(entry->getString(L"Id"), entry->getString(L"Name"));
        if (bar == NULL)
            continue;
        bar->color = static_cast<BossEvent::BossBarColor>(std::min(std::max(entry->getInt(L"Color"), 0), static_cast<int>(BossEvent::WHITE)));
        bar->overlay =
            static_cast<BossEvent::BossBarOverlay>(std::min(std::max(entry->getInt(L"Overlay"), 0), static_cast<int>(BossEvent::NOTCHED_20)));
        bar->value = entry->getInt(L"Value");
        bar->max = std::max(entry->getInt(L"Max"), 1);
        bar->visible = entry->getBoolean(L"Visible");
        ListTag* players = entry->getList(L"Players", Tag::TAG_STRING);
        for (int player = 0; player < players->size(); ++player)
            bar->players.push_back(static_cast<StringTag*>(players->get(player))->mData);
        show(bar);
    }
}

CompoundTag* CustomBossEvents::save(CompoundTag* tag)
{
    ListTag* bars = new ListTag();
    for (size_t index = 0; index < mBars.size(); ++index)
    {
        const Bar* bar = mBars[index];
        CompoundTag* entry = new CompoundTag();
        entry->putString(L"Id", bar->id);
        entry->putString(L"Name", bar->name);
        entry->putInt(L"Color", bar->color);
        entry->putInt(L"Overlay", bar->overlay);
        entry->putInt(L"Value", bar->value);
        entry->putInt(L"Max", bar->max);
        entry->putBoolean(L"Visible", bar->visible);
        ListTag* players = new ListTag();
        for (size_t player = 0; player < bar->players.size(); ++player)
            players->add(new StringTag(bar->players[player]));
        entry->put(L"Players", players);
        bars->add(entry);
    }
    tag->put(L"Bars", bars);
    return tag;
}

CustomBossEvents::Bar* CustomBossEvents::find(const std::wstring& id)
{
    for (size_t index = 0; index < mBars.size(); ++index)
    {
        if (mBars[index]->id == id)
            return mBars[index];
    }
    return NULL;
}

CustomBossEvents::Bar* CustomBossEvents::add(const std::wstring& id, const std::wstring& name)
{
    if (id.empty() || find(id) != NULL)
        return NULL;
    Bar* bar = new Bar();
    bar->id = id;
    bar->name = name;
    bar->color = BossEvent::WHITE;
    bar->overlay = BossEvent::PROGRESS;
    bar->value = 0;
    bar->max = 100;
    bar->visible = true;
    bar->event = boost::shared_ptr<ServerBossEvent>(new ServerBossEvent(-1, name, bar->color, bar->overlay));
    mBars.push_back(bar);
    changed(bar);
    return bar;
}

void CustomBossEvents::remove(const std::wstring& id)
{
    for (std::vector<Bar*>::iterator it = mBars.begin(); it != mBars.end(); ++it)
    {
        if ((*it)->id == id)
        {
            (*it)->event->removeAllPlayers();
            delete *it;
            mBars.erase(it);
            setDirty();
            return;
        }
    }
}

void CustomBossEvents::changed(Bar* bar)
{
    show(bar);
    setDirty();
}

void CustomBossEvents::show(Bar* bar)
{
    ServerBossEvent* event = bar->event.get();
    event->setName(bar->name);
    event->setColor(bar->color);
    event->setOverlay(bar->overlay);
    event->setPercent(static_cast<float>(bar->value) / static_cast<float>(bar->max));
    event->setVisible(bar->visible);
    const std::vector<boost::shared_ptr<ServerPlayer> > online = OnlinePlayers();
    for (size_t index = 0; index < online.size(); ++index)
    {
        const bool listed = std::find(bar->players.begin(), bar->players.end(), IdOf(online[index])) != bar->players.end();
        if (listed)
            event->addPlayer(online[index], 0);
        else
            event->removePlayer(online[index]);
    }
}

void CustomBossEvents::setPlayers(Bar* bar, const std::vector<boost::shared_ptr<ServerPlayer> >& players)
{
    bar->players.clear();
    for (size_t index = 0; index < players.size(); ++index)
        bar->players.push_back(IdOf(players[index]));
    changed(bar);
}

boost::shared_ptr<CustomBossEvents> CustomBossEvents::get()
{
    return WorldSavedData<CustomBossEvents>(FILE_ID);
}

void CustomBossEvents::onJoin(const boost::shared_ptr<ServerPlayer>& player)
{
    boost::shared_ptr<CustomBossEvents> data = get();
    if (!data)
        return;
    for (size_t index = 0; index < data->mBars.size(); ++index)
    {
        Bar* bar = data->mBars[index];
        if (std::find(bar->players.begin(), bar->players.end(), IdOf(player)) != bar->players.end())
            bar->event->addPlayer(player, 0);
    }
}

void CustomBossEvents::onLeave(const boost::shared_ptr<ServerPlayer>& player)
{
    boost::shared_ptr<CustomBossEvents> data = get();
    if (!data)
        return;
    for (size_t index = 0; index < data->mBars.size(); ++index)
        data->mBars[index]->event->removePlayer(player);
}
