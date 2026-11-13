-- data kept with the game's own saves: a value on an entity (saved with it, as an entity tag), and settings per world (in the mod's data folder, under the world's name)
local std = require("lcestd:std")

-- every mob that comes into the world is counted for its owner
mod.hook("Level_addEntity_after", function(call)
  local entity = call.entity
  if std.entity.is_player(entity) or call.self:get_mIsClientSide() then return end
  if std.data.entity_get(entity, "born") == nil then
    std.data.entity_set(entity, "born", tostring(call.self:getGameTime()))
    local world = std.data.world_load(call.self, { mobs = 0 })
    world.mobs = world.mobs + 1
    std.data.world_save(call.self, world)
  end
end)
