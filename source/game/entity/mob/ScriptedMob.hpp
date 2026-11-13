#ifndef GAME_ENTITY_MOB_SCRIPTEDMOB_HPP
#define GAME_ENTITY_MOB_SCRIPTEDMOB_HPP

#include "game/core/AttributeInstance.hpp"
#include "game/core/Cow.hpp"
#include "game/core/ModEntities.hpp"
#include "game/entity/mob/EntityIO.hpp"
#include "game/entity/mob/ModEntity_generated.hpp"
#include "game/entity/mob/SharedMonsterAttributes.hpp"

template <class Base> class ScriptedMob : public ModEntity<Base>
{
public:
    explicit ScriptedMob(Level* level) : ModEntity<Base>(level) { Init(); }

    void Init()
    {
        mType = EntityIO::sCreatingType;
        const ModEntities::Spec* spec = ModEntities::Find(mType);
        if (spec == NULL)
            return;
        AttributeInstance* health = this->getAttribute(SharedMonsterAttributes::MAX_HEALTH);
        health->setBaseValue(spec->health);
        this->setHealth(static_cast<float>(spec->health));
        AttributeInstance* speed = this->getAttribute(SharedMonsterAttributes::MOVEMENT_SPEED);
        speed->setBaseValue(speed->getBaseValue() * spec->speed);
    }

    static Entity* create(Level* level) { return new ScriptedMob<Base>(level); }

    virtual unsigned long long GetType() { return mType; }

private:
    unsigned long long mType;
};

template <> inline ScriptedMob<Cow>::ScriptedMob(Level* level) : ModEntity<Cow>(level, true)
{
    Init();
}

#endif
