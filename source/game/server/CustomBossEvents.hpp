#ifndef GAME_SERVER_CUSTOMBOSSEVENTS_HPP
#define GAME_SERVER_CUSTOMBOSSEVENTS_HPP

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "game/core/BossEvent.hpp"
#include "game/data/SavedData.hpp"

class CompoundTag;
class ServerBossEvent;
class ServerPlayer;

// the boss bars /bossbar makes, saved with the world; the players who see one are kept by their id, so a player who
// leaves and joins again sees it again
class CustomBossEvents : public SavedData
{
public:
    struct Bar
    {
        std::wstring id;
        std::wstring name;
        BossEvent::BossBarColor color;
        BossEvent::BossBarOverlay overlay;
        int value;
        int max;
        bool visible;
        std::vector<std::wstring> players;
        boost::shared_ptr<ServerBossEvent> event;
    };

    static const std::wstring FILE_ID;

    explicit CustomBossEvents(const std::wstring& id);

    virtual void load(CompoundTag* tag);
    virtual CompoundTag* save(CompoundTag* tag);

    Bar* find(const std::wstring& id);
    Bar* add(const std::wstring& id, const std::wstring& name);
    void remove(const std::wstring& id);
    const std::vector<Bar*>& bars() const { return mBars; }
    // after a field of the bar changed: what the players see follows it, and the world is saved with it
    void changed(Bar* bar);
    void setPlayers(Bar* bar, const std::vector<boost::shared_ptr<ServerPlayer> >& players);

    static boost::shared_ptr<CustomBossEvents> get();
    static void onJoin(const boost::shared_ptr<ServerPlayer>& player);
    static void onLeave(const boost::shared_ptr<ServerPlayer>& player);

private:
    void show(Bar* bar);

    std::vector<Bar*> mBars;
};

#endif
