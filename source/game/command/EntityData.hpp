#ifndef GAME_COMMAND_ENTITYDATA_HPP
#define GAME_COMMAND_ENTITYDATA_HPP

#include <boost/shared_ptr.hpp>

class CompoundTag;
class Entity;

namespace EntityData
{
CompoundTag* Read(const boost::shared_ptr<Entity>& entity);
}

#endif
